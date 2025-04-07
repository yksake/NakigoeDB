#pragma once
#include "Scrollbar.hpp"

class PulldownMenu
{
public:
	PulldownMenu() = default;
	PulldownMenu(const Array<String>& items, const Font& font, const double fontSize, const Optional<double>& maxItemHeight = unspecified);

	void update(const Vec2& pos);
	void draw() const;

	bool isEmpty() const noexcept;

	void setPos(const Vec2& pos) noexcept;

	[[nodiscard]]
	const RectF& getRect() const noexcept;

	[[nodiscard]]
	size_t getIndex() const noexcept;

	void setIndex(const size_t index);

	[[nodiscard]]
	const Array<String>& getItems() const noexcept;

	void close() noexcept;

	bool mouseOver() const noexcept;

private:
	static constexpr Size Padding{ 8, 2 };
	static constexpr int32 DownButtonSize = 16;
	static constexpr ColorF TextColor{ 0.11 };

	Font m_font;
	double m_fontSize = 12;

	Array<String> m_items;
	size_t m_index = 0;

	double m_maxItemWidth = 0;
	double m_maxItemHeight = 0;
	RectF m_rect{ 0 };

	Scrollbar m_scrollbar{ 8.0 };
	double m_scrollLength = 0.0;

	bool m_isOpen = false;

	[[nodiscard]]
	double getMaxItemWidth() const;

	[[nodiscard]]
	RectF getRect(const Vec2& pos) const noexcept;

	[[nodiscard]]
	RectF getItemBaseRect() const noexcept;
};
