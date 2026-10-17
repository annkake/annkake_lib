// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file floyd.hpp
 * @brief Floyd法による全点対最短距離。
 * @details 正方行列。対角は0、辺なしはinf。O(n^3) 時間。有限最短距離はinf未満。負側オーバーフローは例外。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 隣接行列から全点対最短距離を求める
 * @code{.cpp}
 * const int inf = 1000000;
 * std::vector<std::vector<int>> d{{0,5,1}, {inf,0,inf}, {inf,2,0}};
 * bool ok = floyd(d, inf); // true。d[0][1]は3に更新される。
 * (void)ok;
 * @endcode
 */
#pragma once
#include <limits>
#include <stdexcept>
#include <vector>
/** @param distance 距離行列を破壊的に更新する。
 * @return 負閉路がなければtrue。falseのとき最短距離は定義されない。
 */
inline bool floyd(std::vector<std::vector<int>>& distance,
                  int inf = std::numeric_limits<int>::max()) {
    int n = static_cast<int>(distance.size());
    if (inf <= 0) throw std::invalid_argument("infinity");
    for (const auto& row : distance) if (row.size() != distance.size())
        throw std::invalid_argument("non-square matrix");
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) if (distance[i][k] != inf) {
            for (int j = 0; j < n; ++j) if (distance[k][j] != inf) {
                __int128 candidate = static_cast<__int128>(distance[i][k]) + distance[k][j];
                if (candidate < std::numeric_limits<int>::min()) throw std::overflow_error("distance");
                if (candidate < distance[i][j]) distance[i][j] = static_cast<int>(candidate);
            }
        }
        for (int i = 0; i < n; ++i) if (distance[i][i] < 0) return false;
    }
    return true;
}
