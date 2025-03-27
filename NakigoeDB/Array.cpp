#include "Array.hpp"
#include "Shlwapi.h"

namespace NakigoeDB::NSort
{
	Array<String>& Sort(Array<String>& texts)
	{
		return texts.sort_by([](const String& s1, const String& s2)
		{
			// s1 < s2
			return StrCmpLogicalW(s1.toWstr().c_str(), s2.toWstr().c_str()) < 1;
		});
	}

	Array<String> Sorted(const Array<String>& texts)
	{
		return texts.sorted_by([](const String& s1, const String& s2)
		{
			// s1 < s2
			return StrCmpLogicalW(s1.toWstr().c_str(), s2.toWstr().c_str()) < 1;
		});
	}
}
