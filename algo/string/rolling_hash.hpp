/**
 * @file rolling_hash.hpp
 * @brief 2つの法を使う文字列のローリングハッシュ。
 * @details 構築O(n)、getはO(1)。半開区間[l,r)。衝突があり得るため一致は同一文字列の保証ではない。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 部分文字列のハッシュを比較する
 * @code{.cpp}
 * RollingHash hash("abracadabra");
 * bool equal = hash.get(0,4) == hash.get(7,11); // 両方"abra"なのでtrue。
 * (void)equal;
 * // 一般にハッシュ一致だけでは衝突の可能性を排除できない。
 * @endcode
 */
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
class RollingHash {
    static constexpr std::array<std::uint64_t,2> mod{1000000007,1000000009};
    static constexpr std::uint64_t base = 911382323;
    std::array<std::vector<std::uint64_t>,2> prefix, power;
public:
    explicit RollingHash(const std::string& text) {
        for (std::size_t k=0; k<2; ++k) {
            prefix[k].assign(text.size()+1,0); power[k].assign(text.size()+1,1);
            for (std::size_t i=0; i<text.size(); ++i) {
                std::uint64_t code = static_cast<unsigned char>(text[i])+1;
                prefix[k][i+1] = (prefix[k][i]*base+code)%mod[k];
                power[k][i+1] = power[k][i]*base%mod[k];
            }
        }
    }
    /// @return 2つの法でのハッシュ値。空区間は{0,0}。
    std::pair<int,int> get(int left, int right) const {
        if (left < 0 || left > right || static_cast<std::size_t>(right) >= prefix[0].size()) throw std::out_of_range("interval");
        std::array<int,2> value{};
        for (std::size_t k=0; k<2; ++k)
            value[k] = static_cast<int>((prefix[k][right]+mod[k]-prefix[k][left]*power[k][right-left]%mod[k])%mod[k]);
        return {value[0],value[1]};
    }
};
