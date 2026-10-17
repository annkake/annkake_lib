/**
 * @file mod_inv.hpp
 * @brief 拡張Euclid法による剰余逆元。
 * @details mod >= 2。互いに素でない場合は例外。O(log mod)。128bitで中間計算する。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 剰余逆元を使う
 * @code{.cpp}
 * int inverse = mod_inv(3,11); // 4。3*4 mod 11 = 1。
 * (void)inverse;
 * // mod_inv(2,8)のように互いに素でない場合はdomain_error。
 * @endcode
 */
#pragma once
#include <stdexcept>
inline int mod_inv(int a, int mod) {
    if (mod < 2) throw std::invalid_argument("modulus must be at least two");
    __int128 r = mod, next_r = a % mod, coefficient = 0, next_coefficient = 1;
    if (next_r < 0) next_r += mod;
    while (next_r != 0) {
        __int128 q = r / next_r;
        __int128 remainder = r - q*next_r; r = next_r; next_r = remainder;
        __int128 c = coefficient - q*next_coefficient; coefficient = next_coefficient; next_coefficient = c;
    }
    if (r != 1) throw std::domain_error("inverse does not exist");
    coefficient %= mod; if (coefficient < 0) coefficient += mod;
    return static_cast<int>(coefficient);
}
