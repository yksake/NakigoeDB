#pragma once
#include "CryData.hpp"

namespace CryLoader
{
	Array<CryData> Load(const Array<TagData>& basicTags, const Array<TagData>& userTags);

	bool SaveAudioPathList(const Array<std::shared_ptr<CryData>>& cries);
}
