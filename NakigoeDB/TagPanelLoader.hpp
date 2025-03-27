#pragma once
#include "ParentTagItem.hpp"

namespace TagPanelLoader
{
	Array<std::unique_ptr<TagItem>> Load(const Array<TagData>& basicTags, const Array<TagData>& userTags);

	namespace CollapseState
	{
		bool Save(const Array<std::pair<String, bool>>& collapseState);

		Array<std::pair<String, bool>> Load();
	}
}
