/**
 * @file segtree.hpp
 * @brief 一点更新・区間最小値の反復型セグメント木。
 * @details 0始まり・半開区間。構築O(n)、set/minはO(log n)。空区間はTの最大値。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 一点更新・区間最小値
 * @code{.cpp}
 * SegTree<int> seg(std::vector<int>{5,2,7,3});
 * int before = seg.min(1,4); // 2
 * seg.set(1,8);
 * int after = seg.min(1,4); // 3
 * (void)before; (void)after;
 * @endcode
 */
#pragma once
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>
template<class T = int>
class SegTree {
    int count;
    std::vector<T> data;
    static constexpr T identity() { return std::numeric_limits<T>::max(); }
public:
    explicit SegTree(int n) : count(n) {
        if (n < 0) throw std::invalid_argument("negative size");
        data.assign(static_cast<std::size_t>(n) * 2, identity());
    }
    explicit SegTree(const std::vector<T>& values) : SegTree(static_cast<int>(values.size())) {
        std::copy(values.begin(), values.end(), data.begin() + count);
        for (int p = count - 1; p > 0; --p) data[p] = std::min(data[2*p], data[2*p+1]);
    }
    void set(int index, T value) {
        if (index < 0 || index >= count) throw std::out_of_range("index");
        std::size_t p = static_cast<std::size_t>(index) + count;
        data[p] = value;
        while (p > 1) { p /= 2; data[p] = std::min(data[2*p], data[2*p+1]); }
    }
    T min(int left, int right) const {
        if (left < 0 || left > right || right > count) throw std::out_of_range("interval");
        T result = identity();
        std::size_t l = static_cast<std::size_t>(left) + count, r = static_cast<std::size_t>(right) + count;
        while (l < r) {
            if (l & 1) result = std::min(result, data[l++]);
            if (r & 1) result = std::min(result, data[--r]);
            l /= 2; r /= 2;
        }
        return result;
    }
};
