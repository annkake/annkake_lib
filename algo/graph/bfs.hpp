// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file bfs.hpp
 * @brief 無重みグラフの単一始点最短距離。
 * @details 0始まりの隣接リスト。O(V+E) 時間、O(V) 追加空間。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 頂点0からの最短距離
 * @code{.cpp}
 * std::vector<std::vector<int>> g{{1}, {0,2}, {1}, {}};
 * auto dist = bfs(g); // {0,1,2,-1}。頂点3は到達不能。
 * auto from_two = bfs(g, 2); // {2,1,0,-1}
 * @endcode
 */
#pragma once
#include <queue>
#include <stdexcept>
#include <vector>
/// @return 始点からの距離。到達不能は-1。
inline std::vector<int> bfs(const std::vector<std::vector<int>>& g, int source = 0) {
    int n = static_cast<int>(g.size());
    if (source < 0 || source >= n) throw std::out_of_range("source");
    std::vector<int> dist(n, -1);
    dist[source] = 0;
    std::queue<int> que;
    que.push(source);
    while (!que.empty()) {
        int cur = que.front();
        que.pop();
        for (auto nex : g[cur]) {
            if (dist[nex] == -1) {
                que.push(nex);
                dist[nex] = dist[cur] + 1;
            }
        }
    }
    return dist;
}
