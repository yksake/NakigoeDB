#pragma once
#include "TagData.hpp"
#include "CryData.hpp"
#include "SaveData.hpp"

enum class SceneState
{
	Library
};

struct Common
{
	Array<TagData> basicTags;
	Array<TagData> userTags;
	Array<std::shared_ptr<CryData>> cries;

	NakigoeDB::SaveData saveData;
};

using App = SceneManager<SceneState, Common>;
