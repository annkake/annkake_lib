/**
 * @file gf2.hpp
 * @brief GF(2)上の連立一次方程式と行列ランク。
 * @details 行列要素と右辺は0/1。0行にも対応。O(h*w*min(h,w))時間。自由変数0の解を返す。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 0/1連立方程式を解く
 * @code{.cpp}
 * // x XOR y = 1、y = 0。
 * auto result = gf2_solve({{1,1},{0,1}}, {1,0}, 2);
 * // result.solvable=true、rank=2、solution={1,0}。
 * if (result.solvable) {
 *     auto solution = result.solution;
 *     (void)solution;
 * }
 * @endcode
 */
#pragma once
#include <stdexcept>
#include <vector>
struct GF2Result {
    int rank;
    bool solvable;
    std::vector<int> solution;
};
/// @brief 行列aと右辺bを値渡しで消去する。解なしの場合solutionは空。
inline GF2Result gf2_solve(std::vector<std::vector<int>> a, std::vector<int> b, int columns) {
    if (columns < 0 || a.size() != b.size()) throw std::invalid_argument("dimensions");
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (a[i].size() != static_cast<std::size_t>(columns) || (b[i] != 0 && b[i] != 1)) throw std::invalid_argument("dimensions or binary value");
        for (int x : a[i]) if (x != 0 && x != 1) throw std::invalid_argument("nonbinary value");
    }
    int height = static_cast<int>(a.size()), rank = 0;
    std::vector<int> pivots;
    for (int col=0; col<columns && rank<height; ++col) {
        int row = rank;
        while (row < height && a[row][col] == 0) ++row;
        if (row == height) continue;
        a[row].swap(a[rank]); int temp = b[row]; b[row] = b[rank]; b[rank] = temp;
        for (int other=0; other<height; ++other) if (other != rank && a[other][col]) {
            for (int j=col; j<columns; ++j) a[other][j] ^= a[rank][j];
            b[other] ^= b[rank];
        }
        pivots.push_back(col); ++rank;
    }
    for (int row=rank; row<height; ++row) if (b[row]) return {rank,false,{}};
    std::vector<int> solution(columns,0);
    for (int row=0; row<rank; ++row) solution[pivots[row]] = b[row];
    return {rank,true,solution};
}
/// @brief GF(2)行列のランク。columnsを明示するため0行の行列にも対応。
inline int gf2_rank(const std::vector<std::vector<int>>& a, int columns) {
    return gf2_solve(a, std::vector<int>(a.size(),0), columns).rank;
}
