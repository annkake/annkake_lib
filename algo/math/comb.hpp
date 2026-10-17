// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file comb.hpp
 * @brief 階乗テーブルによる二項係数の剰余計算。
 * @details 法は素数、0 <= limit < mod。構築O(limit+log mod)、queryはO(1)。法の素数判定は行わない。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 二項係数を前処理して何度も問い合わせる
 * @code{.cpp}
 * Comb comb(100,101); // 100まで前処理、法101（素数）。
 * int value = comb.query(5,2); // 10
 * int zero = comb.query(3,4);  // 0
 * (void)value; (void)zero;
 * @endcode
 */
#pragma once
#include <stdexcept>
#include <vector>
#include "mod_pow.hpp"
class Comb {
    int modulus;
    std::vector<int> factorial, inverse_factorial;
public:
    explicit Comb(int limit, int mod = 998244353) : modulus(mod) {
        if (limit < 0 || mod < 2 || limit >= mod) throw std::invalid_argument("limit or modulus");
        factorial.assign(static_cast<std::size_t>(limit)+1, 1);
        inverse_factorial.resize(factorial.size());
        for (int i = 1; i <= limit; ++i) factorial[i] = static_cast<int>(static_cast<__int128>(factorial[i-1])*i % modulus);
        inverse_factorial[limit] = mod_pow(factorial[limit], mod-2, modulus);
        for (int i = limit; i > 0; --i) inverse_factorial[i-1] = static_cast<int>(static_cast<__int128>(inverse_factorial[i])*i % modulus);
    }
    /// @return nCk mod p。n<0,k<0,k>nは0、前処理範囲外は例外。
    int query(int n, int k) const {
        if (n < 0 || k < 0 || k > n) return 0;
        if (static_cast<std::size_t>(n) >= factorial.size()) throw std::out_of_range("n");
        return static_cast<int>(static_cast<__int128>(factorial[n])*inverse_factorial[k] % modulus * inverse_factorial[n-k] % modulus);
    }
};
