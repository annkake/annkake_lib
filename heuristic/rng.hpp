// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file rng.hpp
 * @brief AHC共通部から単独利用できるようにしたxorshift128乱数生成器。
 * @details 状態は固定幅の符号なし整数で保持する。整数の一様分布は棄却法で剰余の偏りを除く。
 * 各操作は期待O(1)、空間O(1)。既定の初期状態では毎回同じ列を生成する。
 * uniform_intの区間は[lo,hi)。intをlong longに置き換えた環境でも利用できる。
 * 標準ヘッダーはintのマクロ定義より前に読み込むこと。
 * @note 既存AHCコードと同じライセンス範囲。docs/licensing.mdを参照。
 * @par 使用例
 * @code{.cpp}
 * RNG rng;
 * int index = rng.uniform_int(0, 10); // 0以上10未満
 * double probability = rng.uniform01(); // [0,1)
 * double positive = rng.uniform01_open(); // (0,1)、logの引数にも使える
 * std::uint64_t bits = rng.next_u64();
 * (void)index; (void)probability; (void)positive; (void)bits;
 * @endcode
 */
#pragma once
#include <cstdint>
#include <stdexcept>

struct RNG {
    std::uint32_t x = 123456789, y = 362436069, z = 521288629, w = 88675123;

    std::uint32_t next_u32() {
        std::uint32_t t = x ^ (x << 11);
        x = y; y = z; z = w;
        return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
    }
    std::uint64_t next_u64() {
        const std::uint64_t high = next_u32();
        return (high << 32) | next_u32();
    }
    std::uint32_t uniform_u32(std::uint32_t bound) {
        if (!bound) throw std::invalid_argument("乱数範囲が空");
        const std::uint32_t threshold = static_cast<std::uint32_t>(-bound) % bound;
        for (;;) {
            auto value = next_u32();
            if (value >= threshold) return value % bound;
        }
    }
    std::uint64_t uniform_u64(std::uint64_t bound) {
        if (!bound) throw std::invalid_argument("乱数範囲が空");
        const std::uint64_t threshold = static_cast<std::uint64_t>(-bound) % bound;
        for (;;) {
            auto value = next_u64();
            if (value >= threshold) return value % bound;
        }
    }
    int uniform_int(int lo, int hi) {
        if (lo >= hi) throw std::invalid_argument("乱数の上下限が不正");
        // 符号付きのhi-loは64bitの両端付近でオーバーフローするため計算しない。
        const std::uint64_t width = static_cast<std::uint64_t>(hi) - static_cast<std::uint64_t>(lo);
        return static_cast<int>(static_cast<__int128>(lo) + uniform_u64(width));
    }
    double uniform01() { return next_u32() * (1.0 / 4294967296.0); }
    double uniform01_open() { return (next_u32() + 0.5) * (1.0 / 4294967296.0); }
};
