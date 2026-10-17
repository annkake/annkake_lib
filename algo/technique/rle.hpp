// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file rle.hpp
 * @brief 文字列のランレングス符号化と復元。
 * @details encode: O(n)。decode: O(復元長)。空文字列にも対応。負の連続長は例外。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 連続する同じ文字をまとめて復元する
 * @code{.cpp}
 * auto runs = rle_encode("aaabbc"); // {{'a',3},{'b',2},{'c',1}}
 * auto original = rle_decode(runs); // "aaabbc"
 * @endcode
 */
#pragma once
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
/// @return 文字と連続長の組。入力は変更しない。
inline std::vector<std::pair<char, int>> rle_encode(const std::string& text) {
    std::vector<std::pair<char, int>> runs;
    for (char c : text) {
        if (runs.empty() || runs.back().first != c) runs.emplace_back(c, 1);
        else ++runs.back().second;
    }
    return runs;
}
/// @return 各連続長を展開した文字列。
inline std::string rle_decode(const std::vector<std::pair<char, int>>& runs) {
    std::string text;
    for (const auto& [c, length] : runs) {
        if (length < 0) throw std::invalid_argument("negative run length");
        text.append(static_cast<std::size_t>(length), c);
    }
    return text;
}
