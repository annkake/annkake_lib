// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file modint.hpp
 * @brief コンパイル時に法を指定する剰余整数型。
 * @details Mod >= 2。四則演算はO(1)、除算・invはO(log Mod)、powはO(log exponent)。
 * @pre コンテナのサイズ・添字と戻り値はintで表現可能なこと。個別条件は各APIを参照。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/licensing.md 旧実装の出典調査と今回の置き換え範囲。
 * @par 使用例: 法を型に指定して四則演算する
 * @code{.cpp}
 * using M = ModInt<7>;
 * M a = 3, b = 5;
 * int product = (a*b).val(); // 1
 * int quotient = (a/b).val(); // 2
 * int power = a.pow(4).val(); // 4
 * (void)product; (void)quotient; (void)power;
 * @endcode
 */
#pragma once
#include <istream>
#include <ostream>
#include <stdexcept>
#include "mod_inv.hpp"
#include "mod_pow.hpp"
template<int Mod = 998244353>
class ModInt {
    static_assert(Mod >= 2);
    int value = 0;
public:
    ModInt(int x = 0) : value(x % Mod) { if (value < 0) value += Mod; }
    int val() const { return value; }
    ModInt operator-() const { return ModInt(value == 0 ? 0 : Mod-value); }
    ModInt& operator+=(const ModInt& other) {
        value = value >= Mod-other.value ? value-(Mod-other.value) : value+other.value; return *this;
    }
    ModInt& operator-=(const ModInt& other) {
        value = value < other.value ? Mod-(other.value-value) : value-other.value; return *this;
    }
    ModInt& operator*=(const ModInt& other) {
        value = static_cast<int>(static_cast<__int128>(value)*other.value % Mod); return *this;
    }
    /// @throws std::domain_error 逆元が存在しない場合。
    ModInt inv() const { return ModInt(mod_inv(value, Mod)); }
    ModInt pow(int exponent) const { return ModInt(mod_pow(value, exponent, Mod)); }
    ModInt& operator/=(const ModInt& other) { return *this *= other.inv(); }
    friend ModInt operator+(ModInt a, const ModInt& b) { return a += b; }
    friend ModInt operator-(ModInt a, const ModInt& b) { return a -= b; }
    friend ModInt operator*(ModInt a, const ModInt& b) { return a *= b; }
    friend ModInt operator/(ModInt a, const ModInt& b) { return a /= b; }
    friend bool operator==(const ModInt& a, const ModInt& b) { return a.value == b.value; }
    friend std::ostream& operator<<(std::ostream& stream, const ModInt& x) { return stream << x.value; }
    friend std::istream& operator>>(std::istream& stream, ModInt& x) {
        int input;
        if (stream >> input) x = ModInt(input);
        return stream;
    }
};
