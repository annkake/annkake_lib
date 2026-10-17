// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file divisors.hpp
 * @brief 正の約数の昇順列挙。
 * @details n >= 1。O(sqrt(n)) 時間、O(約数数) 空間。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 正の約数を昇順で取得する
 * @code{.cpp}
 * auto values = divisors(12); // {1,2,3,4,6,12}
 * @endcode
 */
#pragma once
#include <stdexcept>
#include <vector>
inline std::vector<int> divisors(int n) {
    if (n < 1) throw std::invalid_argument("positive integer required");
    std::vector<int> low, high;
    for (int d = 1; d <= n / d; ++d) if (n % d == 0) {
        low.push_back(d);
        if (d != n/d) high.push_back(n/d);
    }
    low.insert(low.end(), high.rbegin(), high.rend());
    return low;
}
