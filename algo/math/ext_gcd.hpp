// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file ext_gcd.hpp
 * @brief 拡張Euclid法によるBezout係数。
 * @details O(log max(|a|,|b|))。返り値(g,x,y)はg>=0かつa*x+b*y=g。各値がint範囲外なら例外。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: Bezout係数を取得する
 * @code{.cpp}
 * auto [g,x,y] = ext_gcd(30,18); // g=6、30*x+18*y=6。
 * (void)g; (void)x; (void)y;
 * @endcode
 */
#pragma once
#include <limits>
#include <stdexcept>
#include <tuple>
inline std::tuple<int,int,int> ext_gcd(int a, int b) {
    __int128 r = a, next_r = b, x = 1, next_x = 0, y = 0, next_y = 1;
    while (next_r != 0) {
        __int128 q = r / next_r;
        auto advance = [q](__int128& v, __int128& next) { __int128 t = v-q*next; v = next; next = t; };
        advance(r,next_r); advance(x,next_x); advance(y,next_y);
    }
    if (r < 0) { r = -r; x = -x; y = -y; }
    auto narrow = [](__int128 v) -> int {
        if (v < std::numeric_limits<int>::min() || v > std::numeric_limits<int>::max()) throw std::overflow_error("ext_gcd");
        return static_cast<int>(v);
    };
    return {narrow(r), narrow(x), narrow(y)};
}
