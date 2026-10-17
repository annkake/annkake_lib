// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file calc_next.hpp
 * @brief 各位置以降で小文字英字が最初に出現する位置を前計算する。
 * @details 時間・空間O(26*n)。next[i][c]は位置i以降の文字('a'+c)の最初の位置。
 *          存在しないときはn。位置nの行も含むため、末尾到達後も参照できる。
 * @pre 文字列の長さ+1はintで表現可能なこと。
 * @note SPDX-License-Identifier: CC0-1.0
 * @note 出典不明の旧実装を除去し、出現位置の区間を前向きに埋める実装へ置換。
 * @par 使用例: 部分列を先頭から探す
 * @code{.cpp}
 * std::string text = "abac";
 * auto next = calc_next(text);
 * // next[0][0]=0、next[1][0]=2、next[3][0]=4（aはもうない）。
 * int position = 0;
 * bool found = true;
 * for (char c : std::string("ac")) {
 *     int at = next[position][c-'a'];
 *     if (at == static_cast<int>(text.size())) { found = false; break; }
 *     position = at+1; // 次は一致した文字の一つ後から探す。
 * }
 * // found=true。空文字列の場合もnextは1行26列で全て0になる。
 * (void)found;
 * @endcode
 */
#pragma once
#include <array>
#include <stdexcept>
#include <string>
#include <vector>
/// @throws std::invalid_argument a-z以外の文字を含む場合。
inline std::vector<std::vector<int>> calc_next(const std::string& text) {
    int n = static_cast<int>(text.size());
    std::vector<std::vector<int>> next(text.size()+1, std::vector<int>(26,n));
    std::array<int,26> begin{};
    for (int at=0; at<n; ++at) {
        char c = text[at];
        if (c < 'a' || c > 'z') throw std::invalid_argument("lowercase letters required");
        int letter = c-'a';
        // この出現位置が最初の候補となる開始位置だけを埋める。
        for (int start=begin[letter]; start<=at; ++start) next[start][letter] = at;
        begin[letter] = at+1;
    }
    return next;
}
