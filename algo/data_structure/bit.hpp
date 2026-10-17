// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file bit.hpp
 * @brief 0始まりのFenwick木による加算と区間和。
 * @details 初期値0。add/sumは O(log n)、空間 O(n)。和はTで表現可能なこと。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 一点加算・区間和
 * @code{.cpp}
 * BIT<int> bit(5); // 添字0..4、初期値0。
 * bit.add(1,3); bit.add(3,2);
 * int prefix = bit.sum(4);  // [0,4)の和:5
 * int range = bit.sum(2,5); // [2,5)の和:2
 * (void)prefix; (void)range;
 * @endcode
 * @par 使用例: 転倒数（同じ値同士は数えない）
 * @code{.cpp}
 * #include <algorithm>
 * #include <cstdint>
 * std::vector<int> a{3,1,2,1};
 * auto values = a;
 * std::sort(values.begin(), values.end());
 * values.erase(std::unique(values.begin(), values.end()), values.end());
 * BIT<int> seen(static_cast<int>(values.size()));
 * std::int64_t inversions = 0; // 転倒数はn*(n-1)/2なので32bitを超える場合がある。
 * int processed = 0;
 * for (int x : a) {
 *     int rank = static_cast<int>(std::lower_bound(values.begin(), values.end(), x) - values.begin());
 *     // sum(rank+1)は既出の「x以下」の数。既出総数から引くと「xより大きい」数。
 *     inversions += processed - seen.sum(rank+1);
 *     seen.add(rank,1);
 *     ++processed;
 * }
 * (void)inversions;
 * // inversions = 4。座標圧縮するので負数や重複があっても同じ方法でよい。
 * @endcode
 */
#pragma once
#include <stdexcept>
#include <vector>
template<class T = int>
class BIT {
    std::vector<T> data;
public:
    explicit BIT(int n) {
        if (n < 0) throw std::invalid_argument("negative size");
        data.assign(static_cast<std::size_t>(n) + 1, T{});
    }
    /// @brief a[index] += value。indexは0始まり。
    void add(int index, T value) {
        if (index < 0 || static_cast<std::size_t>(index) >= data.size() - 1) throw std::out_of_range("index");
        for (std::size_t p = static_cast<std::size_t>(index) + 1; p < data.size(); p += p & (~p + 1))
            data[p] += value;
    }
    /// @return 半開区間[0,right)の和。
    T sum(int right) const {
        if (right < 0 || static_cast<std::size_t>(right) >= data.size()) throw std::out_of_range("right");
        T result{};
        for (std::size_t p = right; p > 0; p &= p - 1) result += data[p];
        return result;
    }
    /// @return 半開区間[left,right)の和。
    T sum(int left, int right) const {
        if (left > right) throw std::invalid_argument("reversed interval");
        return sum(right) - sum(left);
    }
};
