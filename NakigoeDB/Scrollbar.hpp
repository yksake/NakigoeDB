#pragma once
#include "Margin.hpp"

class Scrollbar
{
public:
	struct ColorPalette
	{
		Color backgroundColor = Color{ 238 };
		Color barColor = Color{ 180 };
		Color mouseOverColor = Color{ 150 };
		Color grabbedColor = Color{ 128 };
	};

	Scrollbar() = default;
	Scrollbar(const double width, const ColorPalette& palette);
	Scrollbar(const double width);
	Scrollbar(const ColorPalette& palette);

	/// @brief スクロールバーを更新します。
	/// @param targetArea スクロール対象の表示領域
	/// @param itemY スクロール対象のy位置
	/// @param itemH スクロール対象の高さ
	void update(const RectF& targetArea, const double itemY, const double itemH);
	void draw() const;

	RoundRect bar() const;
	RectF barArea() const;
	void setPosY(const double y);
	void moveY(const double length);
	void moveHome();
	void moveEnd();

	bool mouseOver() const;
	bool leftClicked() const;

	bool isGrabbed() const;
	void grab();
	void release();

	double scrollLength() const;

	bool enabled() const;

	ColorPalette getPalette() const;
	void setPalette(const ColorPalette& palette);

private:
	double m_posY = 0.0;
	double m_width = 16.0;
	double m_height = 0.0;
	static constexpr double MinHeight = 10.0;
	Margin m_margin = { 3.0 };
	double m_r = m_width * 0.5;

	// スクロール対象の表示領域
	RectF m_viewArea = { Vec2::Zero(), SizeF::Zero() };
	// スクロール対象の高さ
	double m_itemY = 0.0;
	double m_itemH = 0.0;
	double m_scrollLength = 0.0;

	bool m_isGrabbed = false;
	bool m_enabled = true;

	ColorPalette m_palette;


	double minBarY() const;
	double maxBarY() const;
	void fixPos();

	double barScrollPercentage() const;
	// アイテムのスクロール率
	double itemScrollPercentage() const;
	double minItemY() const;
};
