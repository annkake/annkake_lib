#!/usr/bin/env python3
"""RNGの単独ヘッダー・スニペットを32/64bitで境界検証する。"""
import json
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
BODY = r'''
signed main() {
    RNG first, second;
    assert(first.next_u32() == 3701687786u);
    first = RNG{};
    for (int i = 0; i < 1000; ++i) assert(first.next_u64() == second.next_u64());
    RNG combined, separate;
    const std::uint64_t high = separate.next_u32();
    const std::uint64_t low = separate.next_u32();
    assert(combined.next_u64() == ((high << 32) | low));

    RNG rng;
    const int lo = std::numeric_limits<int>::min();
    const int hi = std::numeric_limits<int>::max();
    for (int i = 0; i < 10000; ++i) {
        auto value = rng.uniform_int(lo, hi);
        assert(lo <= value && value < hi);
        value = rng.uniform_int(-7, 11);
        assert(-7 <= value && value < 11);
        value = rng.uniform_int(lo, -1);
        assert(lo <= value && value < -1);
        value = rng.uniform_int(1, hi);
        assert(1 <= value && value < hi);
        assert(rng.uniform_int(lo, lo + 1) == lo);
        assert(rng.uniform_int(hi - 1, hi) == hi - 1);
        assert(rng.uniform_u32(1) == 0);
        assert(rng.uniform_u64(1) == 0);
        assert(rng.uniform_u32(UINT32_MAX) < UINT32_MAX);
        assert(rng.uniform_u64(UINT64_MAX) < UINT64_MAX);
        double p = rng.uniform01();
        assert(0 <= p && p < 1);
        p = rng.uniform01_open();
        assert(0 < p && p < 1);
    }
    auto invalid = [](auto operation) {
        bool caught = false;
        try { operation(); } catch (const std::invalid_argument&) { caught = true; }
        assert(caught);
    };
    invalid([&]{ rng.uniform_int(0, 0); });
    invalid([&]{ rng.uniform_int(hi, lo); });
    invalid([&]{ rng.uniform_u32(0); });
    invalid([&]{ rng.uniform_u64(0); });
}
'''


def main():
    snippets = json.loads((ROOT / 'snippets/cpp.code-snippets').read_text())
    snippet = '\n'.join(re.sub(r'\\([\\$}])', lambda m: m[1], line)
                        for line in snippets['rng']['body']) + '\n'
    assert 'https://github.com/annkake/annkake_lib' in snippet
    with tempfile.TemporaryDirectory() as directory:
        work = Path(directory)
        for wide in [False, True]:
            for variant in ['header', 'snippet']:
                prefix = '#include <bits/stdc++.h>\n'
                if wide:
                    prefix += '#define int long long\n'
                code = f'#include "{ROOT}/heuristic/rng.hpp"\n' if variant == 'header' else snippet
                source = work / 'rng.cpp'
                source.write_text(prefix + code + BODY)
                binary = work / 'rng'
                subprocess.run(['g++', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror',
                                '-fsanitize=undefined', '-fno-sanitize-recover=all',
                                str(source), '-o', str(binary)], check=True)
                subprocess.run([str(binary)], check=True)
        source = work / 'shared.cpp'
        source.write_text(f'#include "{ROOT}/heuristic/rng.hpp"\n'
                          f'#include "{ROOT}/heuristic/core.hpp"\n'
                          'int main() { RNG a; ahc::RNG b; '
                          'for (int i=0; i<1000; ++i) {'
                          'assert(a.next_u64()==b.next_u64()); '
                          'assert(a.uniform_int(-100,100)==b.uniform_int(-100,100)); }}\n')
        subprocess.run(['g++', '-std=c++20', '-O1', str(source), '-o', str(work / 'shared')], check=True)
        subprocess.run([str(work / 'shared')], check=True)
    print('RNG header/snippet passed: int32/int64, boundary ranges, UBSan and AHC consistency')


if __name__ == '__main__':
    main()
