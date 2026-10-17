/**
 * @file compress.hpp
 * @brief 任意の比較可能な値の座標圧縮。
 * @details O(n log n) 時間、O(n) 空間。元の値を保持し、整数の順位を別に返す。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 元の値を保持した座標圧縮
 * @code{.cpp}
 * std::vector<int> input{50,-10,50,20};
 * auto [rank,values] = compress(input);
 * // rank={2,0,2,1}、values={-10,20,50}。inputは変更しない。
 * int restored = values[rank[0]]; // 50
 * (void)restored;
 * @endcode
 */
#pragma once
#include <algorithm>
#include <utility>
#include <vector>
/** @return {各要素の0始まりの順位, 昇順の重複のない値}。 */
template<class T>
std::pair<std::vector<int>, std::vector<T>> compress(const std::vector<T>& input) {
    std::vector<T> values(input);
    std::sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());
    std::vector<int> rank;
    rank.reserve(input.size());
    for (const T& x : input)
        rank.push_back(static_cast<int>(std::lower_bound(values.begin(), values.end(), x) - values.begin()));
    return {rank, values};
}
