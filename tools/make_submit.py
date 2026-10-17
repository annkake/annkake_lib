#!/usr/bin/env python3
"""引用includeを依存順に展開し、単一ファイルの提出コードを作る。"""
import argparse
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]


def bundle(entry, sources=None):
    sources = {Path(path).resolve(): text for path, text in (sources or {}).items()}
    seen = set()
    stack = set()
    have_bits = False
    def expand(path):
        nonlocal have_bits
        path = path.resolve()
        if not path.is_relative_to(ROOT): raise ValueError(f'include outside library: {path}')
        if path in stack: raise ValueError(f'cyclic include: {path}')
        if path in seen: return ''
        seen.add(path); stack.add(path)
        text = sources.get(path)
        if text is None: text = path.read_text()
        lines = [f'// BEGIN FILE {path.relative_to(ROOT).as_posix()}']
        for line in text.splitlines():
            if re.fullmatch(r'\s*#pragma\s+once\s*',line): continue
            local = re.fullmatch(r'\s*#include\s*"([^"]+)"\s*',line)
            if local:
                lines.append(expand(path.parent / local.group(1)).rstrip('\n'))
            elif re.fullmatch(r'\s*#include\s*<bits/stdc\+\+\.h>\s*',line):
                if not have_bits: lines.append(line); have_bits = True
            else: lines.append(line)
        lines.append(f'// END FILE {path.relative_to(ROOT).as_posix()}')
        stack.remove(path)
        return '\n'.join(lines)+'\n'
    return expand(Path(entry))


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('entry',type=Path)
    parser.add_argument('-o','--output',type=Path,default=ROOT/'dist/official_submit.cpp')
    args=parser.parse_args()
    result=bundle(args.entry)
    if args.output.resolve()==args.entry.resolve(): parser.error('input and output must differ')
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(result)
    print(args.output)


if __name__=='__main__': main()
