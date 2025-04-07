#pragma once

namespace NakigoeDB::Tag
{
	Array<String> ReservedTags(const bool includesAliases = false);
	bool IsReserved(const String& category, const bool includesAliases = false);

	Array<String> NumberAliases();
	Array<String> CryVersionAliases();
	Array<String> GenerationAliases();
	Array<String> HiddenAbilityAliases();

	bool IsNumberTag(const String& category);
	bool IsCryVersionTag(const String& category);
	bool IsGenerationTag(const String& category);
	bool IsHiddenAbilityTag(const String& category);

	Array<char32> InvalidChars();
	bool HasInvalidChar(const String& text);
}

struct TagData
{
	String category;
	String value;
	String customName;

	static TagData Load(String text);
	static TagData Load(const String& category, String text);

	bool operator==(const TagData& tag) const
	{
		return (this->category == tag.category) && (this->value == tag.value);
	}

	bool operator<(const TagData& tag) const
	{
		if (this->category != tag.category)
		{
			return this->category < tag.category;
		}

		return this->value < tag.value;
	}

	friend void Formatter(FormatData& formatData, const TagData& value)
	{
		formatData.string += U"({}, {}, {})"_fmt(value.category, value.value, value.customName);
	}

	template <class CharType>
	friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, TagData& value)
	{
		input.ignore(1);

		value.category.clear();
		while (input && input.peek() != U',')
		{
			value.category += input.get();
		}

		input.ignore(2);

		value.value.clear();
		while (input && input.peek() != U',')
		{
			value.value += input.get();
		}

		input.ignore(2);

		while (input && input.peek() != U')')
		{
			value.customName += input.get();
		}

		input.ignore(1);

		return input;
	}
};
