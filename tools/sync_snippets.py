#!/usr/bin/env python3
"""C++スニペットとテンプレート末尾のprefix一覧を原本から生成する。"""
import argparse
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
BEGIN = '// BEGIN GENERATED SNIPPET INDEX\n'
END = '// END GENERATED SNIPPET INDEX\n'
TEMPLATES = [
    ('templates/atcoder.cpp', ['atcoder', 'ma', 'm'], 'AtCoder用テンプレート（ACL使用）'),
    ('templates/icpc.cpp', ['icpc', 'ICPC'], 'ICPC用テンプレート（ACL不要）'),
    ('heuristic/submit.cpp', ['ahc', 'mahc'], 'AHC用の単一ファイルテンプレート'),
    ('heuristic/official_submit.cpp', ['ahc_official'], '公式スコア接続の単一ファイル例（case Bの入出力は問題ごとに変更）'),
]
ALIASES = {
    'compress': ['Compress'], 'rle': ['runlength'], 'floyd': ['warshall'],
    'lca': ['LCA'], 'rolling_hash': ['Hash'], 'segtree': ['seg'], 'trie': ['Trie'],
    'digit_dp': ['digitDP'], 'div_floor': ['floor'], 'div_ceil': ['ceil'],
    'comb': ['comv'], 'ext_gcd': ['extGCD'], 'gcd': ['GCD'],
    'mod_pow': ['modpow'], 'factor': ['PFD'], 'prime_sieve': ['isprime'], 'mat_pow': ['matrixpow'],
}


def entries():
    result = [(path, names, description, []) for path, names, description in TEMPLATES]
    for header in sorted((ROOT / 'algo').rglob('*.hpp')):
        source = header.read_text()
        brief = re.search(r'@brief (.+)', source).group(1)
        dependencies = [str((header.parent / name).relative_to(ROOT))
                        for name in re.findall(r'^\s*#include\s*"([^"]+)"', source, re.M)]
        result.append((str(header.relative_to(ROOT)), [header.stem, *ALIASES.get(header.stem, [])], brief, dependencies))
    result.append(('heuristic/official_bridge.hpp', ['ahc_official_bridge'],
                   '公式スコアと最良解の接続（AHCテンプレート共通部が必要）', []))
    return result


def catalogue(items):
    lines = [BEGIN.rstrip(), '// C++スニペット prefix対応表（原本: snippets/cpp.code-snippets）',
             '// 更新: python3 tools/sync_snippets.py',
             '// 関数・クラス定義はsolve/mainの外へ展開する。依存prefixは先に各1回だけ挿入。',
             '// ライブラリ用はincludeなし。テンプレートの<bits/stdc++.h>を前提とする。',
             '// 通常は一覧の先頭prefixを使う。括弧内は旧prefixの別名。']
    for path, names, description, dependencies in items:
        prefix = names[0] + (f" ({', '.join(names[1:])})" if len(names) > 1 else '')
        dependency = ' / 先に: ' + ', '.join(Path(p).stem for p in dependencies) if dependencies else ''
        lines.append(f'// {prefix:<30} : {description}{dependency}')
    lines.append(END.rstrip())
    return '\n'.join(lines) + '\n'


def with_catalogue(source, index):
    if BEGIN in source:
        before, rest = source.split(BEGIN, 1)
        _, after = rest.split(END, 1)
        if after.strip():
            raise ValueError('prefix一覧の後ろにコードがあります。末尾への移動が必要です。')
        return before.rstrip() + '\n\n' + index
    return source.rstrip() + '\n\n' + index


def library_source(source, dependencies):
    # テンプレートが<bits/stdc++.h>を読み込むため、スニペットにはincludeを含めない。
    # 引用includeの依存先は、対応するprefixで先に一度だけ展開する。
    source = re.sub(r'^\s*#pragma once\s*\n', '', source, flags=re.M)
    source = re.sub(r'^\s*#\s*include\s*(?:<[^>]+>|"[^"]+")[^\n]*\n', '', source, flags=re.M)
    if dependencies:
        source = '// 先に依存スニペットを各1回だけ展開: ' + ', '.join(Path(p).stem for p in dependencies) + '\n' + source
    return source


def encode_line(line):
    # JSONのエスケープとは別に、VS Codeのsnippet構文で特殊な文字をエスケープする。
    return line.replace('\\', '\\\\').replace('$', '\\$').replace('}', '\\}')


def expected_updates():
    items = entries()
    index = catalogue(items)
    updates = {}
    targets = [ROOT / path for path, _, _ in TEMPLATES]
    targets.append(ROOT / 'heuristic/template.cpp')
    for path in targets:
        updates[path] = with_catalogue(path.read_text(), index)
    snippets = {}
    for path, names, description, dependencies in items:
        file = ROOT / path
        source = updates.get(file, file.read_text())
        if dependencies or file.suffix == '.hpp': source = library_source(source, dependencies)
        snippets[Path(path).stem] = {
            'scope': 'cpp', 'prefix': names[0] if len(names) == 1 else names,
            'body': [encode_line(line) for line in source.splitlines()],
            'description': description + (' / 先に: ' + ', '.join(Path(p).stem for p in dependencies) if dependencies else ''),
        }
    updates[ROOT / 'snippets/cpp.code-snippets'] = json.dumps(snippets, ensure_ascii=False, indent=2) + '\n'
    return updates


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    updates = expected_updates()
    changed = [path for path, source in updates.items() if path.read_text() != source]
    if args.check:
        for path in changed: print(f'未同期: {path.relative_to(ROOT)}')
        return bool(changed)
    for path in changed: path.write_text(updates[path])
    print(f'スニペットとprefix一覧を同期しました（{len(changed)}ファイル）')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
