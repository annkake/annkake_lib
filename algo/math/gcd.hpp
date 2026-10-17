/**
 * @file gcd.hpp
 * @brief 非負の最大公約数。
 * @details 符号付き整数を許容し、gcd(0,0)=0。結果がint範囲外なら例外。O(log max(|a|,|b|))。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 符号付き整数の最大公約数
 * @code{.cpp}
 * int g = ::gcd(-18,24); // 6。::を付けるとstd::gcdとの混同を避けられる。
 * (void)g;
 * @endcode
 */
#pragma once
#include <limits>
#include <stdexcept>
inline int gcd(int a, int b) {
    __int128 x = a, y = b;
    if (x < 0) x = -x;
    if (y < 0) y = -y;
    while (y != 0) { __int128 remainder = x % y; x = y; y = remainder; }
    if (x > std::numeric_limits<int>::max()) throw std::overflow_error("gcd");
    return static_cast<int>(x);
}
