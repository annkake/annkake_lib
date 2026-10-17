#!/usr/bin/env python3
"""許可リストにある公開ファイルだけをZIPへ収録する（.localは常に対象外）。"""
import argparse
import hashlib
import json
from pathlib import Path
import zipfile

ROOT = Path(__file__).resolve().parents[1]
PUBLIC_FILES = ['README.md', 'LICENSE', '.gitignore', '.gitattributes']
PUBLIC_DIRS = ['algo', 'heuristic', 'templates', 'snippets', 'docs', 'provenance',
               'LICENSES', 'tools', 'tests', 'examples']
EXCLUDED_PARTS = {'.local', '.git', '__pycache__', 'dist', 'notes'}


def collect(root=ROOT):
    candidates = [root / name for name in PUBLIC_FILES]
    for name in PUBLIC_DIRS:
        directory = root / name
        if directory.is_symlink(): raise ValueError(f'symlink is not allowed: {name}')
        candidates.extend(directory.rglob('*'))
    files = []
    for path in candidates:
        relative = path.relative_to(root)
        if any(part.lower() in EXCLUDED_PARTS for part in relative.parts): continue
        if path.is_symlink(): raise ValueError(f'symlink is not allowed: {relative}')
        if not path.is_file(): continue
        if path.suffix in {'.pyc', '.pyo', '.o', '.out'}: continue
        files.append(path)
    return sorted(set(files), key=lambda p: p.relative_to(root).as_posix())


def build(output, root=ROOT):
    files = collect(root)
    entries = [{'path': p.relative_to(root).as_posix(),
                'sha256': hashlib.sha256(p.read_bytes()).hexdigest(),
                'size': p.stat().st_size} for p in files]
    manifest = {'format': 1, 'files': entries}
    output.parent.mkdir(parents=True, exist_ok=True)
    manifest_text = json.dumps(manifest, ensure_ascii=False, indent=2) + '\n'
    with zipfile.ZipFile(output, 'w', compression=zipfile.ZIP_DEFLATED) as archive:
        for path in files:
            info = zipfile.ZipInfo(path.relative_to(root).as_posix(), (2026,1,1,0,0,0))
            info.compress_type = zipfile.ZIP_DEFLATED
            info.external_attr = 0o100644 << 16
            archive.writestr(info, path.read_bytes())
        info = zipfile.ZipInfo('release-manifest.json', (2026,1,1,0,0,0))
        info.compress_type = zipfile.ZIP_DEFLATED
        info.external_attr = 0o100644 << 16
        archive.writestr(info,manifest_text)
    return manifest


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=ROOT / 'dist/annkake_lib.zip')
    args = parser.parse_args()
    output = args.output.resolve()
    if output.is_relative_to(ROOT) and not output.is_relative_to(ROOT / 'dist'):
        parser.error('リポジトリ内の出力先はdist/配下にしてください')
    manifest = build(output)
    print(f'{output}: {len(manifest["files"])} public files')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
