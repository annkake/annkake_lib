// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file factor.hpp
 * @brief 試し割りによる素因数分解。
 * @details n >= 1。O(sqrt(n)) 時間。返り値は素因数と指数の組を昇順に並べる。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 素因数と指数を列挙する
 * @code{.cpp}
 * auto factors = factor(60); // {{2,2},{3,1},{5,1}}
 * auto empty = factor(1); // 空配列
 * @endcode
 */
#pragma once
#include <stdexcept>
#include <utility>
#include <vector>
inline std::vector<std::pair<int,int>> factor(int n) {
    if (n < 1) throw std::invalid_argument("positive integer required");
    std::vector<std::pair<int,int>> result;
    for (int divisor = 2; divisor <= n / divisor; ++divisor) if (n % divisor == 0) {
        int exponent = 0;
        do { n /= divisor; ++exponent; } while (n % divisor == 0);
        result.emplace_back(divisor, exponent);
    }
    if (n > 1) result.emplace_back(n,1);
    return result;
}
