#pragma once
#include "TagItem.hpp"
#include "TagData.hpp"
#include "CryData.hpp"

namespace TagLoader
{
	Array<TagData> LoadMainTagNames();
	Array<TagData> LoadUserTags();

	extern const FilePath UserTagDBFolder;

	bool SaveUserTagContents(const TagData& tag, const Array<CryDataKey>& keys);
	Array<CryDataKey> LoadUserTagContents(const TagData& tag);
}
