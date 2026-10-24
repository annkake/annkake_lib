#!/usr/bin/env python3
"""共通定義を各テンプレートとAHC提出版へ展開する。--check は差分検査のみ。"""
from pathlib import Path
import argparse
import re
import sys

sys.dont_write_bytecode = True
from make_submit import bundle

ROOT = Path(__file__).resolve().parents[1]
BEGIN = '// BEGIN GENERATED COMMON\n'
END = '// END GENERATED COMMON\n'
RNG_BEGIN = '// BEGIN GENERATED RNG\n'
RNG_END = '// END GENERATED RNG\n'


def render(template, kind):
    mint = 'using mint = atcoder::modint998244353;'
    aliases = 'using vmint = vc<mint>;\nusing vvmint = vv<mint>;\nusing vvvmint = vv<vmint>;'
    if kind == 'ICPC':
        mint = '// ICPC用の自前mintは共通定義の前で定義する。'
    elif kind == 'AHC':
        mint = '#if __has_include(<atcoder/all>)\n' + mint + '\n#endif'
        aliases = '#if __has_include(<atcoder/all>)\n' + aliases + '\n#endif'
    return template.replace('@MINT_DEFINITION@', mint).replace('@MINT_ALIASES@', aliases)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    template = (ROOT / 'templates/common.hpp.in').read_text()
    updates = {}
    for name, kind in [('templates/atcoder.cpp', 'ALGO'),
                       ('templates/icpc.cpp', 'ICPC'),
                       ('heuristic/core.hpp', 'AHC')]:
        path = ROOT / name
        before, rest = path.read_text().split(BEGIN, 1)
        _, after = rest.split(END, 1)
        updates[path] = before + BEGIN + render(template, kind) + END + after
    rng = (ROOT / 'heuristic/rng.hpp').read_text()
    rng = re.sub(r'^\s*#(?:pragma once|include[^\n]*)\n', '', rng, flags=re.M)
    path = ROOT / 'heuristic/core.hpp'
    before, rest = updates[path].split(RNG_BEGIN, 1)
    _, after = rest.split(RNG_END, 1)
    updates[path] = before + RNG_BEGIN + rng + RNG_END + after
    core = updates[ROOT / 'heuristic/core.hpp'].replace('#pragma once\n', '', 1)
    example = (ROOT / 'heuristic/template.cpp').read_text().replace('#include "core.hpp"', '')
    updates[ROOT / 'heuristic/submit.cpp'] = core + '\n' + example
    official = bundle(ROOT / 'examples/official_score/main_case_b.cpp', updates)
    marker = '// BEGIN GENERATED SNIPPET INDEX\n'
    driver = (ROOT / 'heuristic/template.cpp').read_text()
    if marker in driver:
        official = official.rstrip() + '\n\n' + marker + driver.split(marker,1)[1]
    updates[ROOT / 'heuristic/official_submit.cpp'] = official
    changed = [path for path, text in updates.items() if not path.exists() or path.read_text() != text]
    if args.check:
        for path in changed:
            print(f'未同期: {path.relative_to(ROOT)}')
        return bool(changed)
    for path in changed:
        path.write_text(updates[path])
    print(f'共通定義を同期しました（{len(changed)}ファイル）')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
