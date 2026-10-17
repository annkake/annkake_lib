#!/usr/bin/env python3
"""公式評価接続・ヘッダー併用・単一ファイル化・新スニペットを検証する。"""
import argparse
import importlib.util
import json
import os
from pathlib import Path
import re
import random
import subprocess
import sys
import tempfile

sys.dont_write_bytecode=True
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('make_submit',ROOT/'tools/make_submit.py')
bundler=importlib.util.module_from_spec(spec)
spec.loader.exec_module(bundler)


def compile_cpp(source,output,flags=()):
    subprocess.run(['g++','-std=c++20','-O1','-Wall','-Wextra','-Werror',*flags,str(source),'-o',str(output)],check=True)


def test_bundler_structure():
    folder=ROOT/'tests/virtual_bundle'
    entry=folder/'main.cpp';header=folder/'part.hpp'
    text=bundler.bundle(entry,{entry:'#include "part.hpp"\n#include "part.hpp"\n',
                               header:'#pragma once\n#include <bits/stdc++.h>\nint marker;\n'})
    assert text.count('int marker;')==1 and text.count('#include <bits/stdc++.h>')==1
    try: bundler.bundle(entry,{entry:'#include "part.hpp"\n',header:'#include "main.cpp"\n'})
    except ValueError: pass
    else: raise AssertionError('cyclic include must fail')


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rust-vis',type=Path,help='照合する公式Rust visualizerの実行ファイル')
    args=parser.parse_args()
    test_bundler_structure()
    subprocess.run(['python3',str(ROOT/'tools/sync_templates.py'),'--check'],check=True)
    subprocess.run(['python3',str(ROOT/'tools/sync_snippets.py'),'--check'],check=True)
    data=json.loads((ROOT/'snippets/cpp.code-snippets').read_text())
    decode=lambda entry:'\n'.join(re.sub(r'\\([\\$}])',lambda m:m[1],line) for line in entry['body'])+'\n'
    with tempfile.TemporaryDirectory() as directory:
        work=Path(directory)
        binary=work/'test'
        compile_cpp(ROOT/'tests/official_score_test.cpp',binary,['-fsanitize=address,undefined','-fno-sanitize-recover=all'])
        env=os.environ.copy();env['ASAN_OPTIONS']='detect_leaks=0'
        subprocess.run([str(binary)],check=True,env=env)
        # 公式翻訳ヘッダーは複数TUから使ってもリンクできる。
        source=work/'second.cpp'
        source.write_text(f'#include "{ROOT}/examples/official_score/translated_case_b.hpp"\nint second(){{return 0;}}\n')
        subprocess.run(['g++','-std=c++20',str(ROOT/'tests/official_score_test.cpp'),str(source),'-o',str(binary)],check=True)
        # AHC共通部にbridgeスニペットを追加した場合も同じ検証を実行する。
        cpp=(ROOT/'tests/official_score_test.cpp').read_text()
        cpp=cpp.replace('#include "../examples/official_score/case_b_adapter.hpp"',
                        f'#include "{ROOT}/heuristic/core.hpp"\n'+decode(data['official_bridge'])+
                        f'\n#include "{ROOT}/examples/official_score/translated_case_b.hpp"\n'+
                        re.sub(r'^#include[^\n]*\n','',(ROOT/'examples/official_score/case_b_adapter.hpp').read_text().replace('#pragma once\n',''),flags=re.M))
        source.write_text(cpp);compile_cpp(source,binary);subprocess.run([str(binary)],check=True)
        sample='2 3\n0 0\n0 1\n1 0\n1 1\n'
        expected=None
        for variant in ['include','bundle','snippet']:
            if variant=='include': source=ROOT/'examples/official_score/main_case_b.cpp'
            else:
                source=work/'single.cpp'
                code=bundler.bundle(ROOT/'examples/official_score/main_case_b.cpp') if variant=='bundle' else decode(data['official_submit'])
                assert not re.search(r'^\s*#include\s*"',code,re.M)
                assert code.count('#include <bits/stdc++.h>')==1
                source.write_text(code)
            for flags in [[],['-DLOCAL']]:
                compile_cpp(source,binary,flags)
                result=subprocess.run([str(binary)],input=sample,text=True,capture_output=True,check=True)
                if flags:
                    assert result.stdout=='' and '[cerr]\n' in result.stderr and '[cout]\n' in result.stderr
                    output=result.stderr.split('[cout]\n',1)[1]
                else:
                    assert result.stderr=='';output=result.stdout
                if expected is None:expected=output
                assert output==expected
                assert len(output.split())==10
        assert expected is not None
        if args.rust_vis:
            cli=work/'cpp_score'
            compile_cpp(ROOT/'tests/official_score_cli.cpp',cli)
            rng=random.Random(70)
            for trial in range(30):
                n=1+rng.randrange(7);m=3
                targets=[(rng.randrange(n),rng.randrange(n)) for _ in range(n*n)]
                moves=[(rng.randrange(n),rng.randrange(n)) for _ in range(3)]
                actions=[rng.randrange(3) for _ in range(n*n)]
                if trial%5==0:actions[0]=3 # 不正出力の扱いも比較する。
                inp=work/'input.txt';out=work/'output.txt'
                inp.write_text(f'{n} {m}\n'+''.join(f'{x} {y}\n' for x,y in targets))
                out.write_text(''.join(f'{x} {y}\n' for x,y in moves)+''.join(f'{a}\n' for a in actions))
                cpp=subprocess.run([str(cli),str(inp),str(out)],capture_output=True,text=True,check=True,cwd=work)
                rust=subprocess.run([str(args.rust_vis.resolve()),str(inp),str(out)],capture_output=True,text=True,check=True,cwd=work)
                assert re.search(r'Score = (\d+)',cpp.stdout)[1]==re.search(r'Score = (\d+)',rust.stdout)[1],trial
            print('Rust visualizer comparison passed (30 valid/invalid cases)')
    print('official examples, bridge, multi-TU headers, bundle and snippets passed')


if __name__=='__main__':main()
