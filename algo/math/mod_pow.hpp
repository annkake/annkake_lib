/**
 * @file mod_pow.hpp
 * @brief 二分累乗による剰余べき乗。
 * @details exponent >= 0、mod >= 1。O(log(exponent+1))。128bitで積を計算する。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 剰余べき乗を計算する
 * @code{.cpp}
 * int value = mod_pow(2,10,1000); // 24
 * int identity = mod_pow(7,0,13); // 1
 * (void)value; (void)identity;
 * @endcode
 */
#pragma once
#include <stdexcept>
inline int mod_pow(int base, int exponent, int mod) {
    if (exponent < 0 || mod < 1) throw std::invalid_argument("exponent or modulus");
    base %= mod; if (base < 0) base += mod;
    int result = 1 % mod;
    while (exponent != 0) {
        if (exponent & 1) result = static_cast<int>(static_cast<__int128>(result)*base % mod);
        exponent /= 2;
        if (exponent != 0) base = static_cast<int>(static_cast<__int128>(base)*base % mod);
    }
    return result;
}
