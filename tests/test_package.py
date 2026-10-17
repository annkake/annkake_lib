#!/usr/bin/env python3
"""公開ZIPにローカル資料が含まれず、展開後の参照も利用できることを検証する。"""
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import zipfile

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('package', ROOT / 'tools/package.py')
package = importlib.util.module_from_spec(spec)
spec.loader.exec_module(package)


def check_links(root):
    for path in package.collect(root):
        if path.suffix != '.md': continue
        for target in re.findall(r'\[[^\[\]\n]*\]\(([^\s)]+)\)', path.read_text()):
            if re.match(r'\w+://',target) or target.startswith('#'): continue
            relative = target.split('#',1)[0]
            assert (path.parent / relative).exists(), (path.relative_to(root),target)


def main():
    check_links(ROOT)
    with tempfile.TemporaryDirectory() as directory:
        directory = Path(directory)
        # .gitignore と export-ignore が実際のGitでも適用されることを確認する。
        fixture = directory / 'fixture'
        fixture.mkdir()
        for name in ['.gitignore','.gitattributes']:
            (fixture / name).write_bytes((ROOT / name).read_bytes())
        subprocess.run(['git','init','--quiet',str(fixture)],check=True)
        private = fixture / '.local/notes/private.txt'
        private.parent.mkdir(parents=True)
        private.write_text('not for publication')
        subprocess.run(['git','-C',str(fixture),'check-ignore','--quiet','.local/notes/private.txt'],check=True)
        attributes = subprocess.run(['git','-C',str(fixture),'check-attr','export-ignore','--','.local/notes/private.txt'],
                                    check=True,capture_output=True,text=True).stdout
        assert attributes.rstrip().endswith(': set'),attributes
        (fixture / 'algo').symlink_to(fixture / '.local',target_is_directory=True)
        try: package.collect(fixture)
        except ValueError: pass
        else: raise AssertionError('symlink directory must not be packaged')
        output = directory / 'library.zip' 
        manifest = package.build(output)
        names = {entry['path'] for entry in manifest['files']}
        assert all(not any(p.lower() in package.EXCLUDED_PARTS for p in Path(name).parts) for name in names)
        assert not any(name.endswith('.pdf') for name in names)
        assert any(name.startswith('examples/cheats/') for name in names)
        assert not any('lazy_segtree.md' in name for name in names)
        assert {'heuristic/official_bridge.hpp','heuristic/official_submit.cpp',
                'examples/official_score/main_case_b.cpp','docs/official_score.md'} <= names
        with zipfile.ZipFile(output) as archive:
            assert set(archive.namelist()) == names | {'release-manifest.json'}
            assert archive.testzip() is None
            for entry in manifest['files']:
                assert hashlib.sha256(archive.read(entry['path'])).hexdigest() == entry['sha256']
            extracted = directory / 'extracted'
            archive.extractall(extracted)
        check_links(extracted)
        subprocess.run(['python3', str(extracted / 'tools/sync_templates.py'), '--check'], check=True)
        subprocess.run(['python3', str(extracted / 'tools/sync_snippets.py'), '--check'], check=True)
        subprocess.run(['g++', '-std=c++20', '-fsyntax-only', str(extracted / 'tests/algo_test.cpp')], check=True)
    print(f'package passed: {len(names)} public files; Notes/reference excluded; links and extracted source verified')


if __name__ == '__main__':
    main()
