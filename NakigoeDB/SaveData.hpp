#pragma once
#include "TagData.hpp"

namespace NakigoeDB
{
	class SaveData
	{
	public:
		SaveData() = default;

		bool load();

		void update();

		String getLPLEDisplayGen() const;
		String getLADisplayGen() const;
		bool setDisplayGen(const StringView lple, const StringView la);

		Optional<int32> getCryVerFilter() const;
		bool setCryVerFilter(const Optional<int32>& cryVerFilter);

		Optional<TagData> getTypeFilter() const;
		bool setTypeFilter(const Optional<TagData>& typeFilter);

	private:
		static const FilePath SavePath;
		static constexpr int8 SaveVersion = 2;

		Point m_windowPos = Point::Zero();
		Size m_windowSize = Size::Zero();
		bool m_isMaximized = false;

		bool m_isSizeMoving = false;

		// ピカブイタグを表示に含める世代
		String m_lpleDisplayGen;
		// Legendsアルセウスダグを表示に含める世代
		String m_laDisplayGen;

		Optional<int32> m_cryVerFilter = 1;
		Optional<TagData> m_typeFilter;

		bool save() const;

		void loadVersion1(const JSON& json);
		void loadVersion2(const JSON& json);
	};
}
