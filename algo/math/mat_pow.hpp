/**
 * @file mat_pow.hpp
 * @brief 正方行列の剰余べき乗。
 * @details exponent >= 0、mod >= 1。O(n^3 log(exponent+1))時間、O(n^2)空間。入力は変更しない。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 行列累乗でFibonacci数を計算する
 * @code{.cpp}
 * std::vector<std::vector<int>> fib{{1,1},{1,0}};
 * auto powered = mat_pow(fib,5,1000000007);
 * int fifth = powered[0][1]; // F(5)=5。元のfibは変更しない。
 * (void)fifth;
 * @endcode
 */
#pragma once
#include <stdexcept>
#include <vector>
inline std::vector<std::vector<int>> mat_pow(std::vector<std::vector<int>> matrix, int exponent, int mod) {
    if (exponent < 0 || mod < 1) throw std::invalid_argument("exponent or modulus");
    std::size_t n = matrix.size();
    for (auto& row : matrix) {
        if (row.size() != n) throw std::invalid_argument("non-square matrix");
        for (int& x : row) { x %= mod; if (x < 0) x += mod; }
    }
    auto multiply = [n,mod](const auto& a, const auto& b) {
        std::vector<std::vector<int>> product(n, std::vector<int>(n,0));
        for (std::size_t i=0; i<n; ++i) for (std::size_t k=0; k<n; ++k)
            for (std::size_t j=0; j<n; ++j)
                product[i][j] = static_cast<int>((product[i][j] + static_cast<__int128>(a[i][k])*b[k][j]) % mod);
        return product;
    };
    std::vector<std::vector<int>> result(n, std::vector<int>(n,0));
    for (std::size_t i=0; i<n; ++i) result[i][i] = 1 % mod;
    while (exponent != 0) {
        if (exponent & 1) result = multiply(result,matrix);
        exponent /= 2;
        if (exponent != 0) matrix = multiply(matrix,matrix);
    }
    return result;
}
