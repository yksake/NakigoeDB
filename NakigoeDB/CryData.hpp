#pragma once
#include "TagData.hpp"

struct CryDataKey
{
	int32 number = 0;
	int32 subID = 0;
	int32 cryVer = 0;

	bool operator==(const CryDataKey& key) const
	{
		return (this->number == key.number) && (this->subID == key.subID) && (this->cryVer == key.cryVer);
	}

	bool operator<(const CryDataKey& key) const
	{
		if (this->number != key.number)
		{
			return this->number < key.number;
		}

		if (this->subID != key.subID)
		{
			return this->subID < key.subID;
		}

		return this->cryVer < key.cryVer;
	}
};

struct CryData
{
	CryDataKey key;

	FilePath path;

	TagData name;
	TagData subName;

	TagData gen;

	TagData type1;
	TagData type2;

	TagData ability1;
	TagData ability2;
	TagData hiddenAbility;

	std::set<TagData> userTags;

	bool operator==(const CryData& cd) const
	{
		return this->key == cd.key;
	}
	bool operator<(const CryData& cd) const
	{
		return this->key < cd.key;
	}

	bool operator==(const CryDataKey& cdk) const
	{
		return this->key == cdk;
	}
	bool operator<(const CryDataKey& cdk) const
	{
		return this->key < cdk;
	}

	operator CryDataKey() const
	{
		return this->key;
	}
};
