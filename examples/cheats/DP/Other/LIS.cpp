#include <iostream>
#include <vector>
#include <algorithm> // std::lower_bound(), std::upper_bound()

/// @brief 最長増加部分列（LIS）のインデックスを返します
/// @tparam Strict 狭義単調増加の場合 true, 広義単調増加の場合 false
/// @tparam Type 数列の要素の型
/// @param v 数列
/// @return 最長増加部分列（LIS）のインデックス
/// @note 1.4 最長増加部分列の復元
/// @see https://zenn.dev/reputeless/books/standard-cpp-for-competitive-programming/viewer/lis
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

int main()
{
	int N;
	std::cin >> N;

	std::vector<int> A(N);
	for (auto& a : A)
	{
		//std::cin >> a;
	}

	std::cout << LIS<true>(A).size() << '\n';
}

//     // 例となる数列
//     // 狭義LIS: {1, 2, 5, 6, 9} (長さ5) など
//     // 広義LIS: {1, 1, 2, 5, 5} (長さ5) など
//     std::vector<int> A = {3, 1, 4, 1, 5, 9, 2, 6, 5};

//     // 1. 狭義単調増加 (Strict = true) で実行
//     std::vector<int> strict_indices = LIS<true>(A);

//     std::cout << "[Strict = true] 狭義単調増加\n";
//     std::cout << "LIS Length : " << strict_indices.size() << '\n';

//     std::cout << "Indices    : ";
//     for (int idx : strict_indices) std::cout << idx << " ";
//     std::cout << '\n';

//     std::cout << "Values     : ";
//     for (int idx : strict_indices) std::cout << A[idx] << " ";
//     std::cout << "\n\n";

//     // 2. 広義単調増加 (Strict = false) で実行
//     std::vector<int> non_strict_indices = LIS<false>(A);

//     std::cout << "[Strict = false] 広義単調増加\n";
//     std::cout << "LIS Length : " << non_strict_indices.size() << '\n';

//     std::cout << "Indices    : ";
//     for (int idx : non_strict_indices) std::cout << idx << " ";
//     std::cout << '\n';

//     std::cout << "Values     : ";
//     for (int idx : non_strict_indices) std::cout << A[idx] << " ";
//     std::cout << '\n';
