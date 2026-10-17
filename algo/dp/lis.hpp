/**
 * @file lis.hpp
 * @brief 最長増加部分列のインデックスを復元する（狭義・広義を選択可能）。
 * @details 時間O(n log n)、追加空間O(n)。空入力では空配列を返す。
 * @tparam Strict true:狭義単調増加、false:広義単調増加。
 * @note SPDX-License-Identifier: CC0-1.0
 * @author Ryo Suzuki (Reputeless)
 * @see https://zenn.dev/reputeless/books/standard-cpp-for-competitive-programming/viewer/lis
 * @see https://zenn.dev/reputeless/books/standard-cpp-for-competitive-programming （コードのCC0表記）
 * @note 既存CheetCode内の同記事由来の復元版を移植。アルゴリズムと関数名LISは維持。
 * @par 使用例: 狭義・広義の長さと実際に選んだ値
 * @code{.cpp}
 * std::vector<int> a{3,1,2,2,4};
 * auto strict = LIS<true>(a);     // 狭義。長さ3のインデックス列。
 * auto nonstrict = LIS<false>(a); // 広義。長さ4のインデックス列。
 * std::vector<int> values;
 * for (int index : strict) values.push_back(a[index]); // {1,2,4}
 * @endcode
 */
#pragma once
#include <algorithm>
#include <vector>
template <bool Strict, class Type>
std::vector<int> LIS(const std::vector<Type>& v)
{
	std::vector<Type> dp;

	auto it = dp.begin();

	std::vector<int> positions;

	for (const auto& elem : v)
	{
		if constexpr (Strict)
		{
			it = std::lower_bound(dp.begin(), dp.end(), elem);
		}
		else
		{
			it = std::upper_bound(dp.begin(), dp.end(), elem);
		}

		positions.push_back(static_cast<int>(it - dp.begin()));

		if (it == dp.end())
		{
			dp.push_back(elem);
		}
		else
		{
			*it = elem;
		}
	}

	std::vector<int> subseq(dp.size());

	int si = static_cast<int>(subseq.size()) - 1;

	int pi = static_cast<int>(positions.size()) - 1;

	while ((0 <= si) && (0 <= pi))
	{
		if (positions[pi] == si)
		{
			subseq[si] = pi;

			--si;
		}

		--pi;
	}

	return subseq;
}

