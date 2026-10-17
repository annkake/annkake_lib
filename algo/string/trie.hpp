/**
 * @file trie.hpp
 * @brief 小文字英字のTrieと共有接頭辞長の検索。
 * @details insert/lcp/countはO(文字数)。文字列はa-zのみ。重複・空文字列にも対応。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 登録文字列と共通する接頭辞を調べる
 * @code{.cpp}
 * Trie trie;
 * trie.insert("apple"); trie.insert("apply");
 * int lcp = trie.lcp("apple",2); // 4。自分以外の登録文字列との最大LCP。
 * int count = trie.count("apple"); // 1
 * int prefix = trie.lcp("apt"); // 2。未登録の文字列でも検索できる。
 * (void)lcp; (void)count; (void)prefix;
 * @endcode
 */
#pragma once
#include <array>
#include <stdexcept>
#include <string>
#include <vector>
class Trie {
    struct Node {
        std::array<int,26> child;
        int pass = 0, terminal = 0;
        Node() { child.fill(-1); }
    };
    std::vector<Node> nodes{Node{}};
    static void validate(const std::string& word) {
        for (char c : word) if (c < 'a' || c > 'z') throw std::invalid_argument("lowercase word required");
    }
public:
    void insert(const std::string& word) {
        validate(word); int at = 0; ++nodes[at].pass;
        for (char c : word) {
            int slot = c-'a';
            if (nodes[at].child[slot] < 0) {
                int next = static_cast<int>(nodes.size());
                nodes.emplace_back(); nodes[at].child[slot] = next;
            }
            at = nodes[at].child[slot]; ++nodes[at].pass;
        }
        ++nodes[at].terminal;
    }
    int count(const std::string& word) const {
        validate(word); int at=0;
        for (char c : word) { at = nodes[at].child[c-'a']; if (at < 0) return 0; }
        return nodes[at].terminal;
    }
    /** @return prefixの通過数がmin_count以上となる最大接頭辞長。
     * @param min_count 1なら登録済み文字列とのLCP。登録済みの自分以外と比較する場合は2。
     */
    int lcp(const std::string& word, int min_count = 1) const {
        validate(word);
        if (min_count < 1) throw std::invalid_argument("positive count required");
        int at=0, length=0;
        for (char c : word) {
            at = nodes[at].child[c-'a'];
            if (at < 0 || nodes[at].pass < min_count) break;
            ++length;
        }
        return length;
    }
};
