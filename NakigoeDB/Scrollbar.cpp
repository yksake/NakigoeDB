#include "Scrollbar.hpp"

Scrollbar::Scrollbar(const double width, const ColorPalette& palette)
	: m_width(width)
	, m_r(width * 0.5)
	, m_palette(palette)
{

}

Scrollbar::Scrollbar(const double width)
	: m_width(width)
	, m_r(width * 0.5)
{

}

Scrollbar::Scrollbar(const ColorPalette& palette)
	: m_palette(palette)
{
	
}


void Scrollbar::update(const RectF& viewArea, const double itemY, const double itemH)
{
	m_viewArea = viewArea;
	m_itemY = itemY;
	m_itemH = itemH;

	m_enabled = (0.0 < m_itemH) && (m_viewArea.h < m_itemH);

	if (m_enabled)
	{
		const double scrollbarAreaH = m_viewArea.h - m_margin.getVertical();
		m_height = scrollbarAreaH * m_viewArea.h / m_itemH;
		if (m_height < MinHeight)
		{
			m_height = MinHeight;
		}

		m_posY = Math::Lerp(minBarY(), maxBarY(), itemScrollPercentage());

		if (MouseL.up())
		{
			m_isGrabbed = false;
		}
		else if (bar().leftClicked())
		{
			m_isGrabbed = true;
			MouseL.clearInput();
		}
		else if (m_isGrabbed)
		{
			m_posY += Cursor::DeltaF().y;
			m_itemY = Math::Lerp(m_viewArea.topY(), minItemY(), barScrollPercentage());
		}
	}
	else
	{
		m_posY = minBarY();
	}

	fixPos();

	if (barArea().mouseOver())
	{
		Mouse::ClearLRInput();
	}
}

void Scrollbar::draw() const
{
	barArea().draw(m_palette.backgroundColor);

	if (not m_enabled)
	{
		return;
	}

	Color color = m_palette.barColor;
	if (bar().mouseOver() && not m_isGrabbed)
	{
		color = m_palette.mouseOverColor;
	}
	else if (m_isGrabbed)
	{
		color = m_palette.grabbedColor;
	}

	bar().draw(color);
}


RoundRect Scrollbar::bar() const
{
	return RoundRect{ (m_viewArea.rightX() - m_width - m_margin.left), m_posY, m_width, m_height, m_r };
}

RectF Scrollbar::barArea() const
{
	const double x = m_viewArea.rightX() - m_width - m_margin.getHorizontal();
	const double w = m_width + m_margin.getHorizontal();

	return RectF{ x, m_viewArea.topY(), w, m_viewArea.h };
}

double Scrollbar::minBarY() const
{
	return m_viewArea.topY() + m_margin.top;
}

double Scrollbar::maxBarY() const
{
	return m_viewArea.bottomY() - m_margin.bottom - m_height;
}

void Scrollbar::setPosY(const double y)
{
	m_posY = minBarY() + y;
	fixPos();
}

void Scrollbar::moveY(const double length)
{
	m_posY += length;
	fixPos();
}

void Scrollbar::moveHome()
{
	m_posY = minBarY();
	m_itemY = m_viewArea.topY();
}

void Scrollbar::moveEnd()
{
	m_posY = maxBarY();
	m_itemY = minItemY();
}

void Scrollbar::fixPos()
{
	// 上
	if (m_posY < minBarY())
	{
		moveHome();
	}

	// 下
	if (maxBarY() < m_posY)
	{
		moveEnd();
	}
}


bool Scrollbar::mouseOver() const
{
	return Window::GetState().focused && m_enabled && bar().mouseOver();
}

bool Scrollbar::leftClicked() const
{
	return m_enabled && bar().leftClicked();
}


bool Scrollbar::isGrabbed() const
{
	return m_enabled && m_isGrabbed;
}

void Scrollbar::grab()
{
	m_isGrabbed = true;
}

void Scrollbar::release()
{
	m_isGrabbed = false;
}


double Scrollbar::scrollLength() const
{
	if (not m_enabled)
	{
		return 0.0;
	}

	return m_viewArea.topY() - m_itemY;
}


bool Scrollbar::enabled() const
{
	return m_enabled;
}


// スクロールバーのスクロール率
double Scrollbar::barScrollPercentage() const
{
	if (not m_enabled)
	{
		return 0;
	}

	return (m_posY - minBarY()) / (maxBarY() - minBarY());
}

// アイテムのスクロール率
double Scrollbar::itemScrollPercentage() const
{
	if (not m_enabled)
	{
		return 0;
	}

	return (m_viewArea.y - m_itemY) / (m_itemH - m_viewArea.h);
}

double Scrollbar::minItemY() const
{
	if (not m_enabled)
	{
		return 0.0;
	}

	return m_viewArea.bottomY() - m_itemH;
}


Scrollbar::ColorPalette Scrollbar::getPalette() const
{
	return m_palette;
}

void Scrollbar::setPalette(const ColorPalette& palette)
{
	m_palette = palette;
}
