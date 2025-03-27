#include "TagData.hpp"

namespace NakigoeDB::Tag
{
	Array<String> ReservedTags(const bool includesAliases)
	{
		Array<String> reservedList;

		if (includesAliases)
		{
			reservedList.append(NumberAliases());
		}
		else
		{
			reservedList << U"No";
		}

		reservedList << U"SubID";

		if (includesAliases)
		{
			reservedList.append(CryVersionAliases());
		}
		else
		{
			reservedList << U"CryVer";
		}

		reservedList << U"Name";
		reservedList << U"SubName";

		if (includesAliases)
		{
			reservedList.append(GenerationAliases());
		}
		else
		{
			reservedList << U"Gen";
		}

		reservedList << U"Type";
		reservedList << U"Type1";
		reservedList << U"Type2";

		reservedList << U"Ability";
		reservedList << U"Ability1";
		reservedList << U"Ability2";

		if (includesAliases)
		{
			reservedList.append(HiddenAbilityAliases());
		}
		else
		{
			reservedList << U"HiddenAbility";
		}

		reservedList << U"Path";

		return reservedList;
	}

	bool IsReserved(const String& category, const bool includesAliases)
	{
		for (const auto& reservedCategory : ReservedTags(includesAliases))
		{
			if (category == reservedCategory)
			{
				return true;
			}
		}

		return false;
	}

	Array<String> NumberAliases()
	{
		return { U"Number", U"No" };
	}

	Array<String> CryVersionAliases()
	{
		return { U"CryVersion", U"CryVer", U"Version", U"Ver" };
	}

	Array<String> GenerationAliases()
	{
		return { U"Generation", U"Gen" };
	}

	Array<String> HiddenAbilityAliases()
	{
		return { U"HiddenAbility", U"Ability3" };
	}

	bool IsNumberTag(const String& category)
	{
		return NumberAliases().contains(category);
	}

	bool IsCryVersionTag(const String& category)
	{
		return CryVersionAliases().contains(category);
	}

	bool IsGenerationTag(const String& category)
	{
		return GenerationAliases().contains(category);
	}

	bool IsHiddenAbilityTag(const String& category)
	{
		return HiddenAbilityAliases().contains(category);
	}

	Array<char32> InvalidChars()
	{
		return { U'\\', U'/', U':', U';', U',', U'.', U'*', U'?', U'\"', U'<', U'>', U'|' };
	}

	bool HasInvalidChar(const String& text)
	{
		for (const auto& invalidChar : InvalidChars())
		{
			if (text.contains(invalidChar))
			{
				return true;
			}
		}

		return false;
	}
}

TagData TagData::Load(String text)
{
	text.trim();

	if (not text)
	{
		return TagData{};
	}

	TagData tag;
	Array<String> equalSplitList;

	// 表示名を設定
	{
		equalSplitList = text.split(U'=');

		if (not InRange(int(equalSplitList.size()), 1, 2))
		{
			return TagData{};
		}

		if (equalSplitList.size() == 2)
		{
			tag.customName = equalSplitList[1];
			tag.customName.trim();
		}
	}

	// 値を設定
	{
		Array<String> dotSplitList = equalSplitList[0].split(U'.');

		if (dotSplitList.size() == 1) // カテゴリだけ設定
		{
			tag.category = dotSplitList[0];
		}
		else if (dotSplitList.size() == 2) // カテゴリと値を設定
		{
			tag.category = dotSplitList[0];
			tag.value = dotSplitList[1];
		}
		else if (2 < dotSplitList.size()) // 不正
		{
			return TagData{};
		}

		tag.category.trim();
		tag.value.trim();

		// 不正な文字のチェック
		if (NakigoeDB::Tag::HasInvalidChar(tag.category) || NakigoeDB::Tag::HasInvalidChar(tag.value))
		{
			return TagData{};
		}
	}

	return tag;
}

TagData TagData::Load(const String& category, String text)
{
	text.trim();

	if (not text)
	{
		return TagData{};
	}

	TagData tag{ .category = category };
	tag.category.trim();

	Array<String> equalSplitList;

	// 表示名を設定
	{
		equalSplitList = text.split(U'=');

		if (not InRange(int(equalSplitList.size()), 1, 2))
		{
			return TagData{};
		}

		if (equalSplitList.size() == 2)
		{
			tag.customName = equalSplitList[1];
			tag.customName.trim();
		}
	}

	// 値を設定
	{
		Array<String> dotSplitList = equalSplitList[0].split(U'.');

		if (dotSplitList.size() == 1)
		{
			if (not tag.category) // カテゴリだけ設定
			{
				tag.category = dotSplitList[0];
			}
			else // タグIDだけ設定
			{
				tag.value = dotSplitList[0];
			}
		}
		else if (dotSplitList.size() == 2)
		{
			if (not tag.category) // カテゴリと値を設定
			{
				tag.category = dotSplitList[0];
				tag.value = dotSplitList[1];
			}
			else // 不正
			{
				return TagData{};
			}
		}
		else if (2 < dotSplitList.size()) // 不正
		{
			return TagData{};
		}

		tag.category.trim();
		tag.value.trim();

		// 不正な文字のチェック
		if (NakigoeDB::Tag::HasInvalidChar(tag.category) || NakigoeDB::Tag::HasInvalidChar(tag.value))
		{
			return TagData{};
		}
	}

	return tag;
}
