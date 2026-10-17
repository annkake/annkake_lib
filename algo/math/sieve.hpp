/**
 * @file sieve.hpp
 * @brief 最小素因数表による繰り返し素因数分解。
 * @details 構築O(n log log n)・空間O(n)。factorはO(log x)。0 <= n、1 <= x <= n。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 同じ上限で何度も素因数分解する
 * @code{.cpp}
 * Sieve sieve(100);
 * bool prime = sieve.is_prime(97); // true
 * auto factors = sieve.factor(60); // {{2,2},{3,1},{5,1}}
 * auto values = sieve.divisors(12); // {1,2,3,4,6,12}
 * (void)prime;
 * @endcode
 */
#pragma once
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>
class Sieve {
    std::vector<int> smallest;
public:
    explicit Sieve(int n) {
        if (n < 0) throw std::invalid_argument("negative limit");
        smallest.assign(static_cast<std::size_t>(n)+1, 0);
        for (int p = 2; p <= n; ++p) if (smallest[p] == 0) {
            smallest[p] = p;
            if (p > n/p) continue;
            for (int x = p*p;;) {
                if (smallest[x] == 0) smallest[x] = p;
                if (x > n-p) break;
                x += p;
            }
        }
    }
    bool is_prime(int x) const {
        if (x < 0 || static_cast<std::size_t>(x) >= smallest.size()) throw std::out_of_range("x");
        return x >= 2 && smallest[x] == x;
    }
    std::vector<std::pair<int,int>> factor(int x) const {
        if (x < 1 || static_cast<std::size_t>(x) >= smallest.size()) throw std::out_of_range("x");
        std::vector<std::pair<int,int>> result;
        while (x > 1) {
            int p = smallest[x], count = 0;
            do { x /= p; ++count; } while (x > 1 && smallest[x] == p);
            result.emplace_back(p, count);
        }
        return result;
    }
    /// @return 昇順の約数。時間 O(d log d + log x)、空間 O(d)。
    std::vector<int> divisors(int x) const {
        std::vector<int> result{1};
        for (auto [p, count] : factor(x)) {
            std::size_t length = result.size(); int power = 1;
            for (int e = 1; e <= count; ++e) {
                power *= p;
                for (std::size_t i = 0; i < length; ++i) result.push_back(result[i]*power);
            }
        }
        std::sort(result.begin(), result.end()); return result;
    }
};
