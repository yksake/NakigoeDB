#include "ItemRightClickMenu.hpp"

ItemRightClickMenu::ItemRightClickMenu(const std::weak_ptr<CryData>& data, const Vec2& pos)
{
	m_data = data;
	m_pos = pos;

	const Vec2 relPos = { 5.0, 5.0 };
	m_pos += relPos;

	m_width = 0.0;
	for (const auto& text : m_texts)
	{
		const double w = FontAsset(U"Font")(text).region().w;

		m_width = Max(m_width, w);
	}
	m_width += TextMarginX * 2.0;
}


Optional<size_t> ItemRightClickMenu::update(const RectF& clipRegion)
{
	if (not m_isActive)
	{
		return none;
	}

	// 描画位置を clipRegion 内に収める
	{
		const RoundRect roundRect = region();

		Vec2 fix = Vec2::Zero();

		if (roundRect.leftCenter().x < clipRegion.leftX())
		{
			fix.x = clipRegion.leftX() - roundRect.leftCenter().x;
		}
		else if (clipRegion.rightX() < roundRect.rightCenter().x)
		{
			fix.x = clipRegion.rightX() - roundRect.rightCenter().x;
		}

		if (roundRect.topCenter().y < clipRegion.topY())
		{
			fix.y = clipRegion.topY() - roundRect.topCenter().y;
		}
		else if (clipRegion.bottomY() < roundRect.bottomCenter().y)
		{
			fix.y = clipRegion.bottomY() - roundRect.bottomCenter().y;
		}

		m_pos += fix;
	}

	if (Cursor::OnClientRect())
	{
		if (textRegion(0).leftClicked())
		{
			m_isActive = false;

			Mouse::ClearLRInput();
			return 0;
		}

		if (region().mouseOver())
		{
			Mouse::ClearLRInput();
		}
		else
		{
			if (MouseL.down() || MouseR.down())
			{
				m_isActive = false;
			}
		}
	}

	return none;
}

void ItemRightClickMenu::draw() const
{
	if (not m_isActive)
	{
		return;
	}

	// 背景を描画
	{
		const double frameThickness = 1.0;

		region()
			.draw(m_palette.backgroundColor)
			.drawFrame(0.0, frameThickness, m_palette.frameColor);
	}

	for (size_t i = 0; i < m_texts.size(); ++i)
	{
		const RectF currentTextRegion = textRegion(i);

		if (Cursor::OnClientRect() && currentTextRegion.mouseOver())
		{
			currentTextRegion
				.rounded(TextMarginX)
				.draw(m_palette.mouseOverColor);
		}

		const Vec2 leftCenter = currentTextRegion.leftCenter().movedBy(TextMarginX, 0.0);

		FontAsset(U"Font")(m_texts[i]).draw(Arg::leftCenter = leftCenter, m_palette.textColor);
	}
}


bool ItemRightClickMenu::isActive() const
{
	return m_isActive;
}


RoundRect ItemRightClickMenu::region() const
{
	const double r = 8.0;
	RoundRect rect = { m_pos, m_width, LineHeight, r };

	rect = rect.stretched(m_margin.top, m_margin.right, m_margin.bottom, m_margin.left);

	return rect;
}

RectF ItemRightClickMenu::textRegion(const size_t index) const
{
	if (index != 0)
	{
		return {};
	}

	return RectF{ m_pos.x, m_pos.y, m_width, LineHeight };
}


std::weak_ptr<CryData> ItemRightClickMenu::getData() const
{
	return m_data;
}


ItemRightClickMenu::ColorPalette ItemRightClickMenu::getPalette() const
{
	return m_palette;
}

void ItemRightClickMenu::setPalette(const ColorPalette& palette)
{
	m_palette = palette;
}
