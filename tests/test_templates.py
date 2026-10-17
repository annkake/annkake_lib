"""C++20 テンプレートの型変換・debug条件・出力分離を検証する。"""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
BODY = r'''
#undef main
struct DebugVal { int val() const { return 42; } };
signed main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
#ifdef LOCAL
    LocalOutput local_output;
#ifndef endl
#error LOCAL requires newline macro
#endif
#else
#ifdef endl
#error endl must remain standard without LOCAL
#endif
#endif
    signed native = 3;
    long long wide = 5;
    assert(max(native, wide) == 5);
    assert(min(wide, native) == 3);
    assert(max(0, wide) == 5);
    assert(std::max(1, 2) == 2);
    assert(max({1, 2, 3}) == 3);
    assert(chmax(native, wide) && native == 5);
    assert(!chmax(native, wide));
    assert(chmin(wide, signed(2)) && wide == 2);
    double decimal = 1.5;
    assert(chmax(decimal, 2.5) && decimal == 2.5);
    // 整数幅はALGOが64bit、AHCが32bitのまま。
#ifdef int
    static_assert(sizeof(int) == 8);
#else
    static_assert(sizeof(int) == 4);
#endif
    static_assert(is_same_v<vl::value_type, int>);
    assert(DIR == "DRUL" && di[0] == 1 && dj[3] == -1);
    assert(!out_grid(0, 0, 1, 1) && out_grid(-1, 0, 1, 1));
    for (int a : {numeric_limits<int>::min(), static_cast<int>(-10), static_cast<int>(-1), static_cast<int>(0), static_cast<int>(1), static_cast<int>(10), numeric_limits<int>::max()}) {
        for (int b : {numeric_limits<int>::min(), static_cast<int>(-3), static_cast<int>(-1), static_cast<int>(1), static_cast<int>(3), numeric_limits<int>::max()}) {
            if (a == numeric_limits<int>::min() && b == -1) {
                for (bool up : {false, true}) {
                    bool threw = false;
                    try { template_detail::divide(a, b, up); }
                    catch (const overflow_error&) { threw = true; }
                    assert(threw);
                }
                continue;
            }
            __int128 x = a, y = b, q = x / y, r = x % y;
            assert(floor_div(a, b) == q - (r != 0 && ((r > 0) != (y > 0))));
            assert(ceil_div(a, b) == q + (r != 0 && ((r > 0) == (y > 0))));
        }
    }
    bool zero_threw = false;
    try { floor_div(1, 0); } catch (const domain_error&) { zero_threw = true; }
    assert(zero_threw);
    assert(popcount(1ULL << 63) == 1);
    int iterations = 0;
    drep(i, numeric_limits<int>::min()) ++iterations;
    dfor(i, numeric_limits<int>::min() + 1, numeric_limits<int>::min()) ++iterations;
    assert(iterations == 1);
    vvl nested{{1, 2}}; vvP pairs{{{1, 2}}}; vvvld floats;
    assert(nested[0][1] == 2 && pairs[0][0].fi == 1 && floats.empty());
    vl values; values.pb(2); values.eb(1); sort(all(values));
    assert(values[0] == 1); sort(rall(values)); assert(values[0] == 2);
    istringstream input("3 4"); input >> values;
    ostringstream formatted; formatted << values << P{5, 6};
    assert(formatted.str() == "3 4 5 6");
    ostringstream answers;
    auto* previous = cout.rdbuf(answers.rdbuf());
    YES; NO; dame;
    if (true) YN;
    if (false) YN;
    cout.rdbuf(previous);
    assert(answers.str() == "Yes\nNo\n-1\nYes\nNo\n");
    int side = 0;
    debug(++side);
#ifdef LOCAL
    assert(side == 1);
#else
    assert(side == 0);
#endif
    DebugVal v;
    debug(v);
    cout << "aaa" << endl;
    cerr << "bbb" << endl;
    cout << "ccc" << endl;
}
'''


def main():
    subprocess.run(['python3', str(ROOT / 'tools/sync_templates.py'), '--check'], check=True)
    with tempfile.TemporaryDirectory() as tmp:
        tmp = Path(tmp)
        for template in ['templates/icpc.cpp', 'heuristic/template.cpp']:
            source = tmp / 'check.cpp'
            source.write_text(f'#define main template_main\n#include "{ROOT / template}"\n' + BODY)
            for flags in [[], ['-DONLINE_JUDGE'], ['-DLOCAL'], ['-DLOCAL', '-DONLINE_JUDGE']]:
                binary = tmp / 'check'
                subprocess.run(['g++', '-std=c++20', '-fsanitize=undefined', '-fno-sanitize-recover=all', *flags,
                                str(source), '-o', str(binary)], check=True)
                result = subprocess.run([str(binary)], capture_output=True, text=True, check=True)
                if '-DLOCAL' in flags:
                    assert result.stdout == '', result
                    assert result.stderr == '[cerr]\n[DEBUG] ++side = 1\n[DEBUG] v = 42\nbbb\n[cout]\naaa\nccc\n', result
                else:
                    assert result.stdout == 'aaa\nccc\n', result
                    assert result.stderr == 'bbb\n', result
        # 連結版と元ファイルの同期も検証する。
        core = (ROOT / 'heuristic/core.hpp').read_text().replace('#pragma once\n', '', 1)
        template = (ROOT / 'heuristic/template.cpp').read_text().replace('#include "core.hpp"', '')
        assert (ROOT / 'heuristic/submit.cpp').read_text() == core + '\n' + template
    print('テンプレート検証成功（8構成・提出版の同期）')


if __name__ == '__main__':
    main()
