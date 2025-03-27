#include "TagLoader.hpp"
#include "ParentTagItem.hpp"
#include "ChildTagItem.hpp"

namespace TagLoader
{
	Array<TagData> LoadMainTagNames()
	{
		const FilePath path = U"config/CustomTagNameList.txt";

		TextReader reader{ path };
		if (not reader)
		{
			return {};
		}

		Array<TagData> tags;
		String category;

		String line;
		while (reader.readLine(line))
		{
			line.trim();

			if (not line || line.starts_with(U';'))
			{
				continue;
			}

			if (line == U"/") // カテゴリの終了
			{
				category.clear();
			}
			else if (line.starts_with(U'/')) // カテゴリの開始
			{
				// カテゴリの入れ子不可
				if (category)
				{
					continue;
				}

				TagData tag = TagData::Load(line.substr(1));
				if ((not tag.category) || tag.value)
				{
					continue;
				}

				category = tag.category;

				// カテゴリの名前を設定する場合だけ追加
				if (tag.customName)
				{
					tags << tag;
				}
			}
			else // カテゴリ内外
			{
				TagData tag = TagData::Load(line);

				if (not tag.customName)
				{
					continue;
				}

				if (tag.category && (not tag.value != not category))
				{
					if (not tag.value)
					{
						tag.value = tag.category;
						tag.category = category;
					}
				}

				tags << tag;
			}
		}

		return tags;
	}

	Array<TagData> LoadUserTags()
	{
		const FilePath path = U"config/UserTagList.txt";

		TextReader reader{ path };
		if (not reader)
		{
			return {};
		}

		Array<TagData> tags;
		String category;

		String line;
		while (reader.readLine(line))
		{
			line.trim();

			if (not line || line.starts_with(U';'))
			{
				continue;
			}

			if (line == U"/") // カテゴリの終了
			{
				category.clear();
			}
			else if (line.starts_with(U'/')) // カテゴリの開始
			{
				// カテゴリの入れ子不可
				if (category)
				{
					continue;
				}

				TagData tag = TagData::Load(line.substr(1));
				if (not tag.category || tag.value)
				{
					continue;
				}

				category = tag.category;

				// カテゴリの名前を設定する場合だけ追加
				if (tag.customName)
				{
					tags << tag;
				}
			}
			else // カテゴリ内外
			{
				TagData tag = TagData::Load(line);

				if (tag.category && (not tag.value != not category))
				{
					if (not tag.value)
					{
						tag.value = tag.category;
						tag.category = category;
					}

					tags << tag;
				}
			}
		}

		return tags;
	}


	const FilePath TagLoader::UserTagDBFolder = U"saves/UserTagDB/";

	static FilePath userTagContentsPath(const TagData& tag)
	{
		return UserTagDBFolder + tag.category + U',' + tag.value + U".csv";
	}

	bool SaveUserTagContents(const TagData& tag, const Array<CryDataKey>& keys)
	{
		if (not tag.category || not tag.value)
		{
			return false;
		}

		CSV csv;

		for (const auto& key : keys)
		{
			csv.write(key.number);
			csv.write(key.subID);
			csv.write(key.cryVer);

			csv.newLine();
		}

		const FilePath path = userTagContentsPath(tag);

		return csv.save(path);
	}

	Array<CryDataKey> LoadUserTagContents(const TagData& tag)
	{
		if (not tag.category || not tag.value)
		{
			return {};
		}

		const FilePath path = userTagContentsPath(tag);

		const CSV csv{ path };
		if (not csv)
		{
			return {};
		}

		Array<CryDataKey> keys;
		const size_t columnSize = 3;

		for (size_t row = 0; row < csv.rows(); ++row)
		{
			if (csv.columns(row) != columnSize)
			{
				continue;
			}

			const auto& currentRow = csv.getRow(row);

			CryDataKey key;

			key.number = Parse<int32>(currentRow[0]);
			key.subID = Parse<int32>(currentRow[1]);
			key.cryVer = Parse<int32>(currentRow[2]);

			keys << key;
		}

		keys.sort_and_unique();

		return keys;
	}
}
