// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file prime_sieve.hpp
 * @brief エラトステネスの篩による素数判定表。
 * @details n >= 0。0と1はfalse。O(n log log n) 時間、O(n) 空間。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 上限以下の素数をまとめて判定する
 * @code{.cpp}
 * auto prime = prime_sieve(10);
 * // prime[0]=false、prime[1]=false、prime[2]=true、prime[9]=false。
 * std::vector<int> primes;
 * for (int n = 0; n <= 10; ++n) if (prime[n]) primes.push_back(n); // {2,3,5,7}
 * @endcode
 */
#pragma once
#include <stdexcept>
#include <vector>
inline std::vector<bool> prime_sieve(int n) {
    if (n < 0) throw std::invalid_argument("negative limit");
    std::vector<bool> prime(static_cast<std::size_t>(n) + 1, true);
    prime[0] = false; if (n >= 1) prime[1] = false;
    for (int p = 2; p <= n/p; ++p) if (prime[p]) {
        for (int multiple = p*p;;) {
            prime[multiple] = false;
            if (multiple > n-p) break;
            multiple += p;
        }
    }
    return prime;
}
