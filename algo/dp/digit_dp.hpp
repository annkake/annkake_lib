/**
 * @file digit_dp.hpp
 * @brief dp[桁数][小さいことが確定したか] の構成を保つ桁DPテンプレート。
 * @details digit_dpはO(桁数*base*遷移処理)時間、O(桁数)個の状態を保持。先頭0を許容する。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 上限以下で、0以外の桁数を最大化する（元の最大化DPと同じ構成）
 * @code{.cpp}
 * const int empty = -1000000000;
 * auto dp = digit_dp<int>("100", empty, 0,
 *     [empty](int& next, const int& previous, int, int digit) {
 *         if (previous == empty) return; // 到達不能からは遷移しない。
 *         int candidate = previous + (digit != 0);
 *         if (next < candidate) next = candidate;
 *     });
 * // dp[i][0]: 上からi桁がNと一致。dp[i][1]: Nより小さいことが確定。
 * // dp[3][0]=1（100）、dp[3][1]=2（例:99）。
 * // 個数を数えるならempty=0、initial=1、更新をnext+=previousに変更する。
 * // Transitionは渡したラムダの型として自動的に決まる。型を自分で作る必要はない。
 * // nextはdp[i+1][次の状態]、previousはdp[i][現在の状態]への参照。
 * // 関数本体の3か所を編集する代わりに、このラムダ内の更新式だけを変更する。
 * // 問題に応じて状態Tも変更できる。
 * @endcode
 * @par 使用例: 2進数の上限以下の個数を数える（0を含む）
 * @code{.cpp}
 * auto count_transition = [](int& next, const int& previous, int, int) {
 *     next += previous; // 個数集計の式。next = previous では上書きになるので注意。
 * };
 * auto count = digit_dp<int>("101", 0, 1, count_transition, 2);
 * // 101(2)=5。count.back()[0]+count.back()[1]=6（0,1,2,3,4,5）。
 * auto hex = digit_dp<int>("F", 0, 1, count_transition, 16); // 合計16個。
 * // 前の例の最大化ではnextをcandidateとの最大値へ更新し、emptyからの遷移を省く。
 * // positionとdigitを使わなければ、引数名を省略できる。
 * @endcode
 */
#pragma once
#include <array>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
namespace digit_dp_detail {
inline std::vector<int> parse(const std::string& text, int base) {
    if (base < 2 || base > 36) throw std::invalid_argument("string base must be 2..36");
    if (text.empty()) throw std::invalid_argument("empty upper bound");
    std::vector<int> digits;
    digits.reserve(text.size());
    for (char c : text) {
        int digit = -1;
        if ('0' <= c && c <= '9') digit = c-'0';
        else if ('A' <= c && c <= 'Z') digit = c-'A'+10;
        else if ('a' <= c && c <= 'z') digit = c-'a'+10;
        if (digit < 0 || digit >= base) throw std::invalid_argument("digit outside base");
        digits.push_back(digit);
    }
    return digits;
}
}
/**
 * @brief 元の三つの遷移を維持した、問題ごとに編集するための桁DP。
 * @param digits 上位桁から順に並べた、空でない桁の配列。各要素は0以上base未満。
 * @param empty 到達不能状態の初期値（最大化なら負の無限大など）。
 * @param initial dp[0][0]の初期値。
 * @param transition transition(next,previous,position,digit)でnextを参照更新する関数。
 *                  個数ならnext += previous、最大化なら候補とnextの最大値を取る。
 * @param base 基数。既定値10。桁配列なら任意のbase>=2を指定できる。
 * @return dp[i][j]。j=0は上限と一致中、j=1は上限より小さいことが確定。
 * @note 到達不能の判定、max/加算などの更新規則はtransition側に記述する。
 *       Tをvector等にすれば問題固有の状態も追加できる。
 */
template<class T, class Transition>
std::vector<std::vector<T>> digit_dp(const std::vector<int>& digits, const T& empty,
                                    const T& initial, Transition transition, int base = 10) {
    if (base < 2 || digits.empty()) throw std::invalid_argument("base or empty upper bound");
    for (int digit : digits) if (digit < 0 || digit >= base) throw std::invalid_argument("digit outside base");
    int sz = static_cast<int>(digits.size());
    std::vector<std::vector<T>> dp(digits.size() + 1, std::vector<T>(2, empty));
    dp[0][0] = initial;
    for (int i = 0; i < sz; ++i) {
        // 1 -> 1
        for (int k = 0; k < base; ++k) {
            transition(dp[i+1][1], dp[i][1], i, k);
        }
        int ni = digits[i];
        // 0 -> 1（ここで上限より小さくなる）
        for (int k = 0; k < ni; ++k) {
            transition(dp[i+1][1], dp[i][0], i, k);
        }
        // 0 -> 0
        transition(dp[i+1][0], dp[i][0], i, ni);
    }
    return dp;
}

/**
 * @brief 文字列版。Nの各文字を桁へ変換して上の桁DPを呼び出す。
 * @param base 2..36。0-9/A-Z/a-zで桁を表す。省略すると10進数。
 * @note 既存のdigit_dp(N,empty,initial,transition)という呼び方もそのまま使える。
 */
template<class T, class Transition>
std::vector<std::vector<T>> digit_dp(const std::string& N, const T& empty,
                                    const T& initial, Transition transition, int base = 10) {
    return digit_dp(digit_dp_detail::parse(N,base),empty,initial,std::move(transition),base);
}

// 以前の具体例も残す。上の汎用テンプレートとは独立した桁和用の補助関数。
/** @return 0 <= x <= upperかつ桁和がtargetの整数の個数（modで剰余）。
 * @param upper 空でないbase進数字列。先頭0を許容。
 * @param base 2..36。省略時10。targetはその基数での桁和。
 */
inline int digit_sum_count(const std::string& upper, int target, int mod = 998244353, int base = 10) {
    if (upper.empty() || target < 0 || mod < 1) throw std::invalid_argument("upper, target or modulus");
    auto digits = digit_dp_detail::parse(upper,base);
    if (static_cast<__int128>(target) > static_cast<__int128>(digits.size())*(base-1)) return 0;
    std::vector<std::array<int,2>> ways(static_cast<std::size_t>(target)+1, {0,0});
    ways[0][0] = 1 % mod;
    for (int current : digits) {
        auto next = ways;
        for (auto& row : next) row = {0,0};
        for (int sum=0; sum<=target; ++sum) for (int less=0; less<2; ++less) {
            int limit = less ? base-1 : current;
            for (int digit=0; digit<=limit && digit<=target-sum; ++digit) {
                int smaller = less || digit < current;
                int& cell = next[sum+digit][smaller];
                cell = static_cast<int>((static_cast<__int128>(cell)+ways[sum][less])%mod);
            }
        }
        ways.swap(next);
    }
    return static_cast<int>((static_cast<__int128>(ways[target][0])+ways[target][1])%mod);
}
