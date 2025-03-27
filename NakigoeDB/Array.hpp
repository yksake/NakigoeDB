#pragma once

namespace NakigoeDB
{
	template<typename T>
	bool isEqual(const Array<T>& arr1, const Array<T>& arr2)
	{
		if (arr1.size() != arr2.size())
		{
			return false;
		}
		else
		{
			for (size_t i = 0; i < arr1.size(); ++i)
			{
				if (arr1[i] != arr2[i])
				{
					return false;
				}
			}

			return true;
		}
	}

	// 自然順ソート
	namespace NSort
	{
		/// @brief 要素を自然順で昇順に並び替えます。
		/// @param texts 並び替える配列
		/// @return *this
		Array<String>& Sort(Array<String>& texts);

		/// @brief 要素を自然順で昇順に並び替えた新しい配列を返します。
		/// @param texts 並び替える配列
		/// @return 新しい配列
		Array<String> Sorted(const Array<String>& texts);
	}
}
