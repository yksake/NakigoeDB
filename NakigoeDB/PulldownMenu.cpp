#include "PulldownMenu.hpp"

PulldownMenu::PulldownMenu(const Array<String>& items, const Font& font, const double fontSize, const Optional<double>& maxItemHeight)
	: m_font{ font }
	, m_fontSize{ fontSize }
	, m_items{ items }
	, m_maxItemWidth{ getMaxItemWidth() }
{
	if (maxItemHeight)
	{
		m_maxItemHeight = *maxItemHeight;
	}
}


void PulldownMenu::update(const Vec2& pos)
{
	if (isEmpty())
	{
		return;
	}

	m_rect = getRect(pos);

	if (Cursor::OnClientRect() && m_rect.leftClicked())
	{
		m_isOpen = not m_isOpen;
		MouseL.clearInput();
	}

	if (not m_isOpen)
	{
		return;
	}

	const RectF itemClipRect = getItemBaseRect();

	// スクロールバーを更新
	{
		if (m_scrollbar.enabled())
		{
			if (itemClipRect.mouseOver())
			{
				m_scrollLength += Mouse::Wheel() * m_rect.h;
			}
		}

		const double itemTopY = m_rect.bottomY() - m_scrollLength;
		const double itemH = m_rect.h * m_items.size();
		m_scrollbar.update(itemClipRect, itemTopY, itemH);
		m_scrollLength = m_scrollbar.scrollLength();
	}

	Vec2 itemPos = m_rect.pos.movedBy(0, m_rect.h - m_scrollLength);

	for (size_t i = 0; i < m_items.size(); ++i)
	{
		RectF itemRect{ itemPos, m_rect.w, m_rect.h };

		if (m_scrollbar.enabled())
		{
			itemRect.w -= m_scrollbar.barArea().w;
		}

		if (Cursor::OnClientRect() && itemClipRect.mouseOver() && itemRect.leftClicked())
		{
			m_index = i;
			m_isOpen = false;
			MouseL.clearInput();
			break;
		}

		itemPos.y += m_rect.h;
	}
}

void PulldownMenu::draw() const
{
	m_rect.draw();

	if (isEmpty())
	{
		return;
	}

	// 本体の枠を描画
	{
		const bool active = m_isOpen || m_rect.mouseOver();

		const double innerThickness = active ? 1.5 : 1;
		const Color color = active ? Color{ 90, 178, 255 } : Color{ 128 };

		m_rect.drawFrame(innerThickness, 0, color);
	}

	m_font(m_items[m_index]).draw(m_fontSize, (m_rect.pos + Padding), TextColor);

	Triangle{ (m_rect.rightX() - DownButtonSize / 2.0 - Padding.x), (m_rect.y + m_rect.h / 2.0),
		(DownButtonSize * 0.5), 180_deg }.draw(TextColor);

	if (not m_isOpen)
	{
		return;
	}

	Vec2 itemPos = m_rect.bl();

	const RectF backRect = getItemBaseRect();

	backRect.drawShadow({ 1, 1 }, 5, 0).draw();

	itemPos.y -= m_scrollLength;

	{
		Graphics2D::SetScissorRect(backRect.asRect());
		RasterizerState rs = RasterizerState::Default2D;
		rs.scissorEnable = (0.0 < m_maxItemHeight);
		const ScopedRenderStates2D rasterizer{ rs };

		for (const auto& item : m_items)
		{
			RectF rect{ itemPos, m_rect.size};

			if (m_scrollbar.enabled())
			{
				rect.w -= m_scrollbar.barArea().w;
			}

			if (rect.mouseOver())
			{
				rect.draw(Palette::Skyblue);
			}

			m_font(item).draw(m_fontSize, (itemPos + Padding), TextColor);

			itemPos.y += m_rect.h;
		}
	}

	if (m_scrollbar.enabled())
	{
		m_scrollbar.draw();
	}

	backRect.drawFrame(1, 0, Palette::Gray);
}


bool PulldownMenu::isEmpty() const noexcept
{
	return m_items.empty();
}

void PulldownMenu::setPos(const Vec2& pos) noexcept
{
	m_rect.setPos(pos);
}

[[nodiscard]]
const RectF& PulldownMenu::getRect() const noexcept
{
	return m_rect;
}

[[nodiscard]]
size_t PulldownMenu::getIndex() const noexcept
{
	return m_index;
}

void PulldownMenu::setIndex(const size_t index)
{
	if (isEmpty())
	{
		return;
	}

	m_index = Clamp(index, size_t(0), m_items.size() - 1);
}

[[nodiscard]]
const Array<String>& PulldownMenu::getItems() const noexcept
{
	return m_items;
}

void PulldownMenu::close() noexcept
{
	m_isOpen = false;
}

bool PulldownMenu::mouseOver() const noexcept
{
	if (not Cursor::OnClientRect())
	{
		return false;
	}
	else if (m_rect.mouseOver())
	{
		return true;
	}
	else if (m_isOpen)
	{
		return getItemBaseRect().mouseOver();
	}
	else
	{
		return false;
	}
}


[[nodiscard]]
double PulldownMenu::getMaxItemWidth() const
{
	double result = 0.0;

	for (const auto& item : m_items)
	{
		result = Max(result, (m_font(item).region(m_fontSize).w));
	}

	return result;
}

[[nodiscard]]
RectF PulldownMenu::getRect(const Vec2& pos) const noexcept
{
	const double fontHeight = (m_font.height() * (m_fontSize / m_font.fontSize()));

	return{
		pos,
		(m_maxItemWidth + (Padding.x * 3 + DownButtonSize)),
		(fontHeight + Padding.y * 2)
	};
}

[[nodiscard]]
RectF PulldownMenu::getItemBaseRect() const noexcept
{
	RectF itemBaseRect = { m_rect.bl(), m_rect.w, m_rect.h * m_items.size() };

	if ((0.0 < m_maxItemHeight) && (m_maxItemHeight < itemBaseRect.h))
	{
		itemBaseRect.h = m_maxItemHeight;
	}

	return itemBaseRect;
}
