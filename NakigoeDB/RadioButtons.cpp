#include "RadioButtons.hpp"

RadioButtons::RadioButtons(const Array<String>& options)
{
	m_options = options;
}

RadioButtons::RadioButtons(const Array<String>& options, const ColorPalette& palette)
	: RadioButtons(options)
{
	m_palette = palette;
}


bool RadioButtons::update(const Vec2& pos, const Optional<double>& width, const bool enabled)
{
	m_pos = pos;
	if (width)
	{
		m_width = *width;
	}
	else
	{
		checkWidth();
	}

	bool isPushed = false;

	for (size_t i = 0; i < m_options.size() && enabled; ++i)
	{
		const Circle button = buttonRegion(i);

		if (button.leftClicked() || textRegion(i).leftClicked())
		{
			m_index = i;
			isPushed = true;
		}

		if (Cursor::OnClientRect() && button.mouseOver())
		{
			Cursor::RequestStyle(CursorStyle::Hand);
		}
	}

	if (region().mouseOver())
	{
		Mouse::ClearLRInput();
	}

	return isPushed;
}

void RadioButtons::draw() const
{
	Graphics2D::SetScissorRect(region().asRect());
	RasterizerState rs = RasterizerState::Default2D;
	rs.scissorEnable = true;
	const ScopedRenderStates2D state{ rs };

	region().draw(m_palette.backgroundColor);

	for (size_t i = 0; i < m_options.size(); ++i)
	{
		// ボタンの描画
		{
			const Circle button = buttonRegion(i);

			if (i == m_index)
			{
				const double thickness = 2.2;

				button
					.draw(m_palette.buttonFillColor)
					.stretched(-thickness)
					.drawFrame(thickness, 0.0, m_palette.buttonBaseColor);
			}
			else
			{
				const double thickness = 2.0;

				button
					.draw(m_palette.buttonBaseColor)
					.drawFrame(thickness, 0.0, m_palette.buttonFrameColor);
			}
		}

		// テキストの描画
		{
			const RectF currentTextRegion = textRegion(i);

			if (currentTextRegion.mouseOver())
			{
				const double r = TextMarginX;

				currentTextRegion
					.rounded(r)
					.draw(m_palette.mouseOverColor);
			}

			FontAsset(U"Font")(m_options[i]).draw(currentTextRegion.stretched(-TextMarginX, 0.0), m_palette.textColor);
		}
	}
}


RectF RadioButtons::region() const
{
	return RectF{ m_pos, m_width, LineHeight * m_options.size() };
}

Circle RadioButtons::buttonRegion(const size_t index) const
{
	const double r = ButtonSize * 0.5;
	const double x = m_pos.x + ButtonMarginX + r;
	const double y = m_pos.y + (LineHeight * index) + (LineHeight * 0.5);

	return Circle{ x, y, r };
}

RectF RadioButtons::textRegion(const size_t index) const
{
	RectF rect = { Vec2::Zero(), SizeF::Zero() };

	if (m_options.size() <= index)
	{
		return rect;
	}

	rect.x = m_pos.x + ButtonSize + (ButtonMarginX * 2.0);
	rect.w = m_width - ButtonSize - (ButtonMarginX * 3.0);
	rect.h = FontAsset(U"Font")(m_options[index]).region(Vec2::Zero()).h;

	rect.y = m_pos.y + (LineHeight * index) + (LineHeight * 0.5) - (rect.h * 0.5);

	return rect;
}


Optional<size_t> RadioButtons::getIndex() const
{
	if (m_options)
	{
		return m_index;
	}
	else
	{
		return none;
	}
}

void RadioButtons::setIndex(const size_t index)
{
	if (m_options)
	{
		m_index = Min(index, m_options.size() - 1);
	}
	else
	{
		m_index = 0;
	}
}


void RadioButtons::checkWidth()
{
	m_width = 0.0;

	for (size_t i = 0; i < m_options.size(); ++i)
	{
		const double textWidth = FontAsset(U"Font")(m_options[i]).region(Vec2::Zero()).w;
		const double width = ButtonSize + textWidth + (ButtonMarginX * 3.0) + (TextMarginX * 2.0);

		m_width = Max(m_width, width);
	}
}


RadioButtons::ColorPalette RadioButtons::getPalette() const
{
	return m_palette;
}

void RadioButtons::setPalette(const ColorPalette& palette)
{
	m_palette = palette;
}
