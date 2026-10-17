#!/usr/bin/env python3
"""snippet構文の復元・prefix重複・依存・展開後のC++を検証する。"""
import importlib.util
import json
from pathlib import Path
import re
import subprocess
import sys
import tempfile

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('sync_snippets', ROOT / 'tools/sync_snippets.py')
builder = importlib.util.module_from_spec(spec)
spec.loader.exec_module(builder)


def decode(line):
    return re.sub(r'\\([\\$}])', lambda match: match.group(1), line)


def main():
    subprocess.run(['python3', str(ROOT / 'tools/sync_snippets.py'), '--check'], check=True)
    snippets = json.loads((ROOT / 'snippets/cpp.code-snippets').read_text())
    items = builder.entries()
    prefixes = {}
    sources = {}
    for path, names, _, dependencies in items:
        entry = snippets[Path(path).stem]
        assert entry['scope'] == 'cpp'
        assert entry['prefix'] == (names[0] if len(names) == 1 else names)
        for name in names:
            assert name.lower() not in prefixes or prefixes[name.lower()] == path
            prefixes[name.lower()] = path
        source = '\n'.join(decode(line) for line in entry['body']) + '\n'
        expected = (ROOT / path).read_text()
        if path.endswith('.hpp'):
            expected = builder.library_source(expected, dependencies)
            assert not re.search(r'^\s*#\s*include\b', source, re.M), path
        assert source == expected, path
        sources[path] = (source, dependencies)
    for text in [r'cout << "\n";', 'a\\', '${x} and $0', '}']:
        assert decode(builder.encode_line(text)) == text
    with tempfile.TemporaryDirectory() as directory:
        work = Path(directory)
        # テストのincludeを実際のsnippet本文へ置換。依存は順に一度ずつ挿入する。
        expanded = set()
        def expand(path):
            if path in expanded: return ''
            expanded.add(path)
            body, deps = sources[path]
            return ''.join(expand(dep) for dep in deps) + body
        tests = (ROOT / 'tests/algo_test.cpp').read_text()
        tests = re.sub(r'#include "\.\./(algo/[^"]+)"', lambda m: expand(m.group(1)), tests)
        assert expanded == {p for p in sources if p.startswith('algo/') and p.endswith('.hpp')}
        source = work / 'expanded.cpp'
        source.write_text(tests)
        for wide in [False, True]:
            binary = work / 'expanded'
            subprocess.run(['g++', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror',
                            '-fsanitize=undefined', '-fno-sanitize-recover=all',
                            *(['-DALGO_INT64'] if wide else []), str(source), '-o', str(binary)], check=True)
            subprocess.run([str(binary)], check=True)
        expanded.clear()
        library_code = ''.join(expand(path) for path in sources if path.startswith('algo/') and path.endswith('.hpp'))
        acl_available = subprocess.run(['g++','-std=c++20','-x','c++','-fsyntax-only','-'],
                                       input='#include <atcoder/all>\n',text=True,capture_output=True).returncode == 0
        for path, _, _ in builder.TEMPLATES:
            if path.endswith('/atcoder.cpp') and not acl_available: continue
            template = sources[path][0]
            combined = template + '\n' + library_code
            if path == 'heuristic/submit.cpp':
                combined += '\n' + sources['heuristic/official_bridge.hpp'][0]
            # 全ライブラリを追加してもinclude行はテンプレート単体のまま。
            includes = lambda text: re.findall(r'^\s*#\s*include[^\n]*', text, re.M)
            assert includes(combined) == includes(template), path
            source.write_text(combined)
            for flags in [[], ['-DLOCAL']]:
                subprocess.run(['g++', '-std=c++20', *flags, '-fsyntax-only', str(source)], check=True)
    print(f'{len(snippets)} C++ snippets: round-trip, prefixes, dependencies and expanded code passed')


if __name__ == '__main__':
    main()
