/**
 * @file div_floor.hpp
 * @brief 符号付き整数除算の切り下げ。
 * @details O(1)。除数0は例外。最小整数/-1はoverflow_error。負の除数にも対応。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 負数も数学的に切り下げる
 * @code{.cpp}
 * int a = div_floor(-7,3); // -3。通常の -7/3 は -2。
 * int b = div_floor(7,-3); // -3
 * (void)a; (void)b;
 * @endcode
 */
#pragma once
#include <limits>
#include <stdexcept>
inline int div_floor(int numerator, int denominator) {
    if (denominator == 0) throw std::domain_error("division by zero");
    if (numerator == std::numeric_limits<int>::min() && denominator == -1) throw std::overflow_error("division");
    int quotient = numerator / denominator, remainder = numerator % denominator;
    if (remainder != 0 && ((remainder > 0) != (denominator > 0))) --quotient;
    return quotient;
}
