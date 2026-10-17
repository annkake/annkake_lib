/**
 * @file dfs.hpp
 * @brief 明示的なスタックによる深さ優先探索。
 * @details 0始まり。O(V+E) 時間、O(V) 追加空間。再帰スタックを使用しない。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 始点から到達できる頂点
 * @code{.cpp}
 * std::vector<std::vector<int>> g{{1}, {2}, {}, {}};
 * auto seen = dfs(g, 0); // {true,true,true,false}
 * @endcode
 */
#pragma once
#include <stdexcept>
#include <vector>
/// @return sourceから到達できる頂点の訪問フラグ。
inline std::vector<bool> dfs(const std::vector<std::vector<int>>& graph, int source = 0) {
    if (source < 0 || source >= static_cast<int>(graph.size())) throw std::out_of_range("source");
    std::vector<bool> reached(graph.size(), false);
    std::vector<int> pending{source}; reached[source] = true;
    while (!pending.empty()) {
        int v = pending.back(); pending.pop_back();
        for (int next : graph[v]) if (!reached[next]) {
            reached[next] = true; pending.push_back(next);
        }
    }
    return reached;
}
