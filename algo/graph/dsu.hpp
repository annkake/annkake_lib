/**
 * @file dsu.hpp
 * @brief 経路圧縮とサイズによる併合を使う素集合データ構造。
 * @details 0始まり。n >= 0。root/same/sizeは償却 O(alpha(n))。全uniteのリスト移動はO(n log n)、空間 O(n)。頂点は範囲内。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 代表頂点と同じ成分の頂点を参照する
 * @code{.cpp}
 * DSU dsu(5);
 * dsu.unite(0, 2);
 * dsu.unite(2, 4);
 * int leader = dsu.root(4);
 * const auto& group = dsu.members(leader); // 頂点0,2,4を含む。順序は保証しない。
 * for (int v : group) { (void)v; } // 成分内の頂点に対する処理
 * bool connected = dsu.same(0, 4); // true
 * int size = dsu.size(2);          // 3
 * (void)connected; (void)size;
 * @endcode
 */
#pragma once
#include <stdexcept>
#include <utility>
#include <vector>
class DSU {
    std::vector<int> link;
    std::vector<std::vector<int>> component;
public:
    explicit DSU(int n) {
        if (n < 0) throw std::invalid_argument("negative size");
        link.assign(n, -1);
        component.resize(n);
        for (int v = 0; v < n; ++v) component[v].push_back(v);
    }
    int root(int v) {
        int r = v;
        while (link[r] >= 0) r = link[r];
        while (v != r) { int next = link[v]; link[v] = r; v = next; }
        return r;
    }
    bool same(int a, int b) { return root(a) == root(b); }
    /// @return 異なる集合を併合したときtrue。
    bool unite(int a, int b) {
        a = root(a); b = root(b);
        if (a == b) return false;
        if (link[a] > link[b]) std::swap(a, b);
        // small-to-large: 各頂点が移動する回数はO(log n)。
        component[a].insert(component[a].end(), component[b].begin(), component[b].end());
        std::vector<int>().swap(component[b]); // 敗者側の容量も解放し、全体O(n)空間を保つ。
        link[a] += link[b]; link[b] = a;
        return true;
    }
    /// @return vと同じ成分の頂点列への読み取り専用参照。代表頂点以外からも呼べる。
    /// @warning unite後は順序・参照・イテレータを保存し続けず、再取得すること。
    const std::vector<int>& members(int v) { return component[root(v)]; }
    int size(int v) { return -link[root(v)]; }
};
