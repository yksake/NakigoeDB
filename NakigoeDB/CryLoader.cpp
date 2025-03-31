#include "CryLoader.hpp"
#include "TagLoader.hpp"

namespace CryLoader
{
	const FilePath audioPathList = U"saves/PathList.csv";

	static Array<CryData> loadMainDB()
	{
		const FilePath path = U"data/Main.csv";

		const CSV csv{ path };
		if (not csv)
		{
			return {};
		}

		Array<CryData> cries;
		const size_t columnSize = 11;

		for (size_t row = 1; row < csv.rows(); ++row)
		{
			if (csv.columns(row) != columnSize)
			{
				continue;
			}

			const auto& currentRow = csv.getRow(row);

			CryData cry;

			cry.key.number = Parse<int32>(currentRow[0]);
			cry.key.subID = Parse<int32>(currentRow[1]);
			cry.key.cryVer = Parse<int32>(currentRow[2]);

			cry.name = TagData{ .category = U"Name", .value = currentRow[3] };
			cry.subName = TagData{ .category = U"SubName", .value = currentRow[4] };

			cry.gen = TagData{ .category = U"Gen", .value = currentRow[5] };

			cry.type1 = TagData{ .category = U"Type1", .value = currentRow[6] };
			cry.type2 = TagData{ .category = U"Type2", .value = currentRow[7] };

			cry.ability1 = TagData{ .category = U"Ability1", .value = currentRow[8] };
			cry.ability2 = TagData{ .category = U"Ability2", .value = currentRow[9] };
			cry.hiddenAbility = TagData{ .category = U"HiddenAbility", .value = currentRow[10] };

			cries << cry;
		}

		cries.sort_and_unique();

		return cries;
	}

	static void setAudioPath(Array<CryData>& cries)
	{
		if (not cries)
		{
			return;
		}

		const CSV csv{ audioPathList };
		if (not csv)
		{
			return;
		}

		const size_t columnSize = 4;
		size_t criesIndex = 0;

		for (size_t row = 0; row < csv.rows(); ++row)
		{
			if (csv.columns(row) != columnSize)
			{
				continue;
			}

			const auto& currentRow = csv.getRow(row);

			CryDataKey currentKey;
			currentKey.number = Parse<int32>(currentRow[0]);
			currentKey.subID = Parse<int32>(currentRow[1]);
			currentKey.cryVer = Parse<int32>(currentRow[2]);

			const FilePath audioPath = currentRow[3];
			if (not audioPath)
			{
				continue;
			}

			while (criesIndex < cries.size())
			{
				if (currentKey < cries[criesIndex])
				{
					break;
				}
				else if (currentKey == cries[criesIndex])
				{
					cries[criesIndex].path = audioPath;

					++criesIndex;
					break;
				}

				++criesIndex;
			}
		}
	}

	static void setUserTags(Array<CryData>& cries, const Array<TagData>& tags)
	{
		for (const auto& tag : tags)
		{
			if (NakigoeDB::Tag::IsReserved(tag.category))
			{
				continue;
			}

			const Array<CryDataKey> taggedCryKeys = TagLoader::LoadUserTagContents(tag);

			size_t criesIndex = 0;
			for (const auto& taggedCryKey : taggedCryKeys)
			{
				while (criesIndex < cries.size())
				{
					if (taggedCryKey == cries[criesIndex])
					{
						cries[criesIndex].userTags.insert(tag);

						++criesIndex;
						break;
					}
					else if (taggedCryKey < cries[criesIndex])
					{
						break;
					}

					++criesIndex;
				}
			}
		}
	}

	Array<CryData> Load(const Array<TagData>& userTags)
	{
		Array<CryData> cries = loadMainDB();

		setAudioPath(cries);

		setUserTags(cries, userTags);

		return cries;
	}

	bool SaveAudioPathList(const Array<std::shared_ptr<CryData>>& cries)
	{
		CSV csv;

		for (const auto& cry : cries)
		{
			if (not cry)
			{
				continue;
			}
			if (not cry->path)
			{
				continue;
			}

			csv.write(cry->key.number);
			csv.write(cry->key.subID);
			csv.write(cry->key.cryVer);

			csv.write(cry->path);

			csv.newLine();
		}

		return csv.save(audioPathList);
	}
}
