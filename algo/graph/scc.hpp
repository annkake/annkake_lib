/**
 * @file scc.hpp
 * @brief 反復DFSによる強連結成分分解と縮約DAG。
 * @details 0始まり。buildはO(V+E)、dagはO(V+E+E log E)時間。再帰なし。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 強連結成分と縮約グラフ
 * @code{.cpp}
 * SCC scc(3);
 * scc.add_edge(0,1); scc.add_edge(1,0); scc.add_edge(1,2);
 * scc.build();
 * // scc.id[0] == scc.id[1]、scc.id[0] != scc.id[2]。
 * auto dag = scc.dag(); // 2成分からなり、{0,1}の成分から{2}の成分へ辺がある。
 * @endcode
 */
#pragma once
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>
class SCC {
    std::vector<std::vector<int>> graph, reverse;
public:
    std::vector<int> id;
    std::vector<std::vector<int>> groups;
    explicit SCC(int n) {
        if (n < 0) throw std::invalid_argument("negative size");
        graph.resize(n); reverse.resize(n);
    }
    void add_edge(int from, int to) { graph[from].push_back(to); reverse[to].push_back(from); }
    /// @brief 成分番号は縮約グラフのトポロジカル順。辺の追加後は再実行する。
    void build() {
        int n = static_cast<int>(graph.size());
        std::vector<bool> seen(n, false);
        std::vector<int> finish;
        for (int start = 0; start < n; ++start) if (!seen[start]) {
            std::vector<std::pair<int, std::size_t>> stack{{start,0}};
            seen[start] = true;
            while (!stack.empty()) {
                auto& [v, next] = stack.back();
                if (next == graph[v].size()) { finish.push_back(v); stack.pop_back(); }
                else {
                    int to = graph[v][next++];
                    if (!seen[to]) { seen[to] = true; stack.emplace_back(to,0); }
                }
            }
        }
        id.assign(n, -1); groups.clear();
        for (auto it = finish.rbegin(); it != finish.rend(); ++it) if (id[*it] < 0) {
            int component = static_cast<int>(groups.size());
            groups.emplace_back(); std::vector<int> stack{*it}; id[*it] = component;
            while (!stack.empty()) {
                int v = stack.back(); stack.pop_back(); groups.back().push_back(v);
                for (int to : reverse[v]) if (id[to] < 0) { id[to] = component; stack.push_back(to); }
            }
        }
    }
    /// @pre 最後の辺追加後にbuild済みであること。
    std::vector<std::vector<int>> dag() const {
        std::vector<std::vector<int>> result(groups.size());
        for (std::size_t v = 0; v < graph.size(); ++v)
            for (int to : graph[v]) if (id[v] != id[to]) result[id[v]].push_back(id[to]);
        for (auto& row : result) {
            std::sort(row.begin(), row.end()); row.erase(std::unique(row.begin(), row.end()), row.end());
        }
        return result;
    }
};
