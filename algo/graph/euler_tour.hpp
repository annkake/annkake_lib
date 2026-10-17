// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file euler_tour.hpp
 * @brief 木のDFS訪問順・オイラー巡回順と部分木の半開区間。
 * @details 0始まりの連結な無向木。構築O(n)時間・空間。再帰を使わない。
 * @note SPDX-License-Identifier: CC0-1.0
 * @par 使用例: 部分木を配列上の区間にする
 * @code{.cpp}
 * std::vector<std::vector<int>> tree{{1,2}, {0,3}, {0}, {1}};
 * EulerTour et(tree, 0);
 * // et.order = {0,1,3,2}、et.tour = {0,1,3,1,0,2,0}。
 * int left = et.tin[1], right = et.tout[1]; // [1,3)が頂点1の部分木。
 * for (int i = left; i < right; ++i) {
 *     int v = et.order[i]; // 頂点1,3。この順に処理する。
 *     (void)v;
 * }
 * // BITやセグ木にはflat[et.tin[v]] = value[v]と並べ、[left,right)を問い合わせる。
 * @endcode
 */
#pragma once
#include <stdexcept>
#include <utility>
#include <vector>
class EulerTour {
public:
    /// @brief 各頂点を一度だけ記録した行きがけ順。部分木vはorder[tin[v],tout[v])。
    std::vector<int> order, tin, tout;
    /// @brief 親への戻りも含む巡回順（2*n-1要素）。first[v]はtour内の最初の位置。
    /// @warning tin/toutはtourではなくorderに対する添字。
    std::vector<int> tour, first, parent, depth;
    explicit EulerTour(const std::vector<std::vector<int>>& tree, int root = 0) {
        int n = static_cast<int>(tree.size());
        if (n == 0 && root == 0) return;
        if (root < 0 || root >= n) throw std::out_of_range("root");
        tin.assign(n,-1); tout.assign(n,-1); first.assign(n,-1);
        parent.assign(n,-1); depth.assign(n,0);
        std::vector<std::pair<int,std::size_t>> stack;
        auto enter = [&](int v) {
            tin[v] = static_cast<int>(order.size()); order.push_back(v);
            first[v] = static_cast<int>(tour.size()); tour.push_back(v);
            stack.emplace_back(v,0);
        };
        enter(root);
        while (!stack.empty()) {
            int v = stack.back().first;
            auto& next = stack.back().second;
            if (next == tree[v].size()) {
                tout[v] = static_cast<int>(order.size()); stack.pop_back();
                if (!stack.empty()) tour.push_back(stack.back().first);
                continue;
            }
            int to = tree[v][next++];
            if (to < 0 || to >= n) throw std::out_of_range("vertex");
            if (to == parent[v]) continue;
            if (tin[to] != -1) throw std::invalid_argument("not a tree");
            parent[to] = v; depth[to] = depth[v]+1; enter(to);
        }
        if (order.size() != tree.size()) throw std::invalid_argument("disconnected tree");
    }
};
