/**
 * @file lca.hpp
 * @brief 二分持ち上げによる木の最近共通祖先。
 * @details 連結な無向木、0始まり。構築O(n log n)、query/dist/on_pathはO(log n)。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 共通祖先・距離・パス上判定
 * @code{.cpp}
 * std::vector<std::vector<int>> tree{{1,2}, {0,3}, {0}, {1}};
 * LCA lca(tree, 0);
 * int ancestor = lca.query(3,2); // 0
 * int distance = lca.dist(3,2);  // 3
 * bool on_path = lca.on_path(3,2,1); // true
 * (void)ancestor; (void)distance; (void)on_path;
 * @endcode
 */
#pragma once
#include <algorithm>
#include <stdexcept>
#include <vector>
class LCA {
    std::vector<std::vector<int>> jump;
    std::vector<int> depth;
public:
    explicit LCA(const std::vector<std::vector<int>>& tree, int root = 0) {
        int n = static_cast<int>(tree.size());
        if (root < 0 || root >= n) throw std::out_of_range("root");
        int levels = 1;
        for (int k = n; k > 1; k /= 2) ++levels;
        jump.assign(levels, std::vector<int>(n, root));
        depth.assign(n, -1); depth[root] = 0;
        std::vector<int> order{root};
        for (std::size_t i = 0; i < order.size(); ++i) {
            int v = order[i];
            for (int next : tree[v]) if (depth[next] < 0) {
                depth[next] = depth[v] + 1; jump[0][next] = v; order.push_back(next);
            }
        }
        if (order.size() != tree.size()) throw std::invalid_argument("disconnected tree");
        for (int k = 1; k < levels; ++k)
            for (int v = 0; v < n; ++v) jump[k][v] = jump[k-1][jump[k-1][v]];
    }
    int query(int a, int b) const {
        if (depth[a] < depth[b]) std::swap(a, b);
        int gap = depth[a] - depth[b];
        for (std::size_t k = 0; gap; ++k, gap /= 2) if (gap & 1) a = jump[k][a];
        if (a == b) return a;
        for (std::size_t k = jump.size(); k-- > 0;) if (jump[k][a] != jump[k][b]) {
            a = jump[k][a]; b = jump[k][b];
        }
        return jump[0][a];
    }
    int dist(int a, int b) const { int c = query(a,b); return (depth[a]-depth[c]) + (depth[b]-depth[c]); }
    bool on_path(int a, int b, int v) const {
        return static_cast<__int128>(dist(a,v)) + dist(v,b) == dist(a,b);
    }
};
