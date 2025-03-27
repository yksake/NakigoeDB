#pragma once
#include "TagData.hpp"
#include "CryData.hpp"

enum class SceneState
{
	Library
};

struct Common
{
	Array<TagData> basicTags;
	Array<TagData> userTags;
	Array<std::shared_ptr<CryData>> cries;
};

using App = SceneManager<SceneState, Common>;
