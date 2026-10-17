// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file doubling.hpp
 * @brief 各頂点からの一意な遷移を二分持ち上げで繰り返す。
 * @details 構築O(n log(max_steps+1))、jumpはO(log(steps+1))。
 *          0始まり。next[v]=-1は遷移先なし。stepsは符号なし64bitまで指定可能。
 * @note SPDX-License-Identifier: CC0-1.0
 * @par 使用例: 5回先・遷移先なし・0回先
 * @code{.cpp}
 * Doubling db({1,2,3,1}, 1000000000000000000ULL);
 * int v = db.jump(0,5); // 2
 * Doubling path({1,2,-1}, 10);
 * int end = path.jump(0,3); // -1
 * int same = path.jump(1,0); // 1
 * (void)v; (void)end; (void)same;
 * @endcode
 */
#pragma once
#include <cstdint>
#include <stdexcept>
#include <vector>
class Doubling {
    std::vector<std::vector<int>> table;
    std::uint64_t limit;
public:
    explicit Doubling(const std::vector<int>& next, std::uint64_t max_steps) : limit(max_steps) {
        int levels = 1;
        for (auto bits = max_steps; bits > 1; bits >>= 1) ++levels;
        int n = static_cast<int>(next.size());
        for (int to : next) if (to < -1 || to >= n) throw std::out_of_range("next vertex");
        table.assign(levels, std::vector<int>(n,-1)); table[0] = next;
        for (int k=1; k<levels; ++k) for (int v=0; v<n; ++v) {
            int mid = table[k-1][v];
            if (mid != -1) table[k][v] = table[k-1][mid];
        }
    }
    /// @return vからsteps回遷移した頂点。途中で遷移先がなくなると-1。
    /// @pre steps <= コンストラクタで指定したmax_steps。
    int jump(int v, std::uint64_t steps) const {
        if (steps > limit) throw std::out_of_range("steps exceed precomputed limit");
        if (v < -1 || v >= static_cast<int>(table[0].size())) throw std::out_of_range("vertex");
        for (std::size_t k=0; steps && v != -1; ++k, steps >>= 1)
            if (steps & 1) v = table[k][v];
        return v;
    }
};
