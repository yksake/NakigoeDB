#pragma once

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

	private:
		static const FilePath SavePath;
		static constexpr int8 SaveVersion = 1;

		Point m_windowPos = Point::Zero();
		Size m_windowSize = Size::Zero();
		bool m_isMaximized = false;

		bool m_isSizeMoving = false;

		// ピカブイタグを表示に含める世代
		String m_lpleDisplayGen;
		// Legendsアルセウスダグを表示に含める世代
		String m_laDisplayGen;

		bool save() const;
	};
}
