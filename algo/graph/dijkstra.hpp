// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file dijkstra.hpp
 * @brief 非負重みグラフのDijkstra法。
 * @details 0始まり。重みは非負。有限距離はinf未満で表現可能なこと。O((V+E) log(V+E)) 時間。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 重み付き有向グラフの最短路
 * @code{.cpp}
 * std::vector<std::vector<std::pair<int,int>>> g{
 *     {{1,5},{2,1}}, {}, {{1,2}}, {}
 * };
 * auto dist = dijkstra(g, 0, 1000000); // {0,3,1,1000000}
 * @endcode
 */
#pragma once
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>
/// @param inf 到達不能を表す値。@return 距離配列。入力は変更しない。
inline std::vector<int> dijkstra(const std::vector<std::vector<std::pair<int, int>>>& graph,
                                int source = 0, int inf = std::numeric_limits<int>::max()) {
    if (source < 0 || source >= static_cast<int>(graph.size()) || inf <= 0)
        throw std::invalid_argument("source or infinity");
    for (const auto& edges : graph) for (const auto& edge : edges)
        if (edge.second < 0) throw std::invalid_argument("negative weight");
    using Entry = std::pair<int, int>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> pending;
    std::vector<int> distance(graph.size(), inf);
    distance[source] = 0; pending.emplace(0, source);
    while (!pending.empty()) {
        auto [cost, v] = pending.top(); pending.pop();
        if (cost != distance[v]) continue;
        for (auto [next, weight] : graph[v]) {
            if (weight >= inf - cost) continue;
            int candidate = cost + weight;
            if (candidate < distance[next]) {
                distance[next] = candidate; pending.emplace(candidate, next);
            }
        }
    }
    return distance;
}
