#pragma once
#include "CryData.hpp"
#include "Margin.hpp"

class ItemRightClickMenu
{
public:
	struct ColorPalette
	{
		Color backgroundColor = Color{ 255 };
		Color mouseOverColor = Color{ 230, 242, 255 };
		Color textColor = Color{ 0 };
		Color frameColor = Color{ 80 };
	};

	ItemRightClickMenu(const std::weak_ptr<CryData>& data, const Vec2& pos);

	/// @brief 
	/// @param pos 
	/// @return 選択された項目の index
	Optional<size_t> update(const RectF& clipRegion);
	void draw() const;

	bool isActive() const;

	RoundRect region() const;

	std::weak_ptr<CryData> getData() const;

	ColorPalette getPalette() const;
	void setPalette(const ColorPalette& palette);

private:
	bool m_isActive = true;
	std::weak_ptr<CryData> m_data;

	const Array<String> m_texts = { U"登録ファイルの設定" };

	Vec2 m_pos = Vec2::Zero();
	double m_width = 0.0;
	static constexpr double LineHeight = 35.0;
	static constexpr double TextMarginX = 5.0;
	Margin m_margin = { 10.0 };

	ColorPalette m_palette;

	RectF textRegion(const size_t index) const;
};
