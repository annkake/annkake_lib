#!/usr/bin/env python3
"""全ヘッダー・使用例のコンパイルと32/64bitの差分・境界テスト。"""
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    records = json.loads((ROOT / 'provenance/algo_originals.json').read_text())
    for entry in records:
        assert (ROOT / 'algo' / entry['new_path']).is_file()
    records = [{'new_path': str(p.relative_to(ROOT / 'algo'))}
               for p in sorted((ROOT / 'algo').rglob('*.hpp'))]
    for entry in records:
        header = ROOT / 'algo' / entry['new_path']
        text = header.read_text()
        assert '@file' in text and '@brief' in text and '@details' in text
        assert 'SPDX-License-Identifier: CC0-1.0' in text
        assert '@code{.cpp}' in text and '@endcode' in text
        assert not re.search(r'^\s*#\s*define|\b(?:rep|rrep|drep|chmax|chmin)\s*\(|\b(?:ll|vl|vvl|INF)\b', text, re.M), header
    # 対象外のファイルがテスト実行中に変わっていないことも確認する。
    excluded = {p: hashlib.sha256(p.read_bytes()).digest()
                for p in (ROOT / '.local').rglob('*') if p.is_file()}
    with tempfile.TemporaryDirectory() as work:
        work = Path(work)
        for wide in [False, True]:
            sources = []
            for i, entry in enumerate(records):
                source = work / f'header_{i}.cpp'
                prefix = '#include <bits/stdc++.h>\n#define int long long\n' if wide else ''
                source.write_text(prefix + f'#include "{ROOT / "algo" / entry["new_path"]}"\nsigned main() {{}}\n')
                sources.append(str(source))
            subprocess.run(['g++', '-std=c++20', '-Wall', '-Wextra', '-Werror', '-fsyntax-only', *sources], check=True)
            # Doxygenの使用例をそのまま取り出し、個別の関数として実行する。
            example_functions = []
            example_includes = []
            for entry in records:
                text = (ROOT / 'algo' / entry['new_path']).read_text()
                for block in re.findall(r'@code\{\.cpp\}\n(.*?)\s*\* @endcode', text, re.S):
                    lines = [re.sub(r'^\s*\* ?', '', line) for line in block.splitlines()]
                    example_includes += [line for line in lines if line.startswith('#include')]
                    body = '\n'.join(line for line in lines if not line.startswith('#include'))
                    example_functions.append(f'void example_{len(example_functions)}() {{\n{body}\n}}\n')
            example_source = work / 'examples.cpp'
            includes = ''.join(f'#include "{ROOT / "algo" / e["new_path"]}"\n' for e in records)
            prefix = '#include <bits/stdc++.h>\n' + '\n'.join(example_includes) + '\n'
            if wide: prefix += '#define int long long\n'
            example_source.write_text(prefix + includes + ''.join(example_functions) +
                                      'signed main() {\n' + ''.join(f'example_{i}();\n' for i in range(len(example_functions))) + '}\n')
            example_binary = work / 'examples'
            subprocess.run(['g++', '-std=c++20', '-Wall', '-Wextra', '-Werror', str(example_source), '-o', str(example_binary)], check=True)
            subprocess.run([str(example_binary)], check=True)
            binary = work / ('algo64' if wide else 'algo32')
            subprocess.run(['g++', '-std=c++20', '-O1', '-g', '-Wall', '-Wextra', '-Werror',
                            '-fsanitize=address,undefined', '-fno-sanitize-recover=all',
                            *(['-DALGO_INT64'] if wide else []),
                            str(ROOT / 'tests/algo_test.cpp'), '-o', str(binary)], check=True)
            env = os.environ.copy()
            # この環境ではptrace下でLSanが実行できないため、ASan/UBSanのみ有効にする。
            env['ASAN_OPTIONS'] = 'detect_leaks=0'
            subprocess.run([str(binary)], check=True, env=env)
        # 既存テンプレートのマクロ・型エイリアスと共存できることを確認する。
        headers = ''.join(f'#include "{ROOT / "algo" / entry["new_path"]}"\n' for entry in records)
        for base in ['templates/icpc.cpp', 'heuristic/core.hpp']:
            source = work / 'integration.cpp'
            source.write_text(f'#define main template_main\n#include "{ROOT / base}"\n#undef main\n' + headers + 'signed main() {}\n')
            subprocess.run(['g++', '-std=c++20', '-Wall', '-Wextra', '-Werror', '-fsyntax-only', str(source)], check=True)
    assert all(hashlib.sha256(p.read_bytes()).digest() == digest for p, digest in excluded.items())
    print(f'{len(records)} headers / {len(example_functions)} usage examples: standalone checks and randomized tests passed in 32/64-bit modes')


if __name__ == '__main__':
    main()
