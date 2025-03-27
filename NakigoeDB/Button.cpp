#include "Button.hpp"
#include "Margin.hpp"

Button::Button(const StringView text)
{
	m_text = text;
}

Button::Button(const StringView text, const ColorPalette& palette)
	: Button(text)
{
	m_palette = palette;
}


bool Button::update(const Vec2& pos, const Optional<double>& width, const bool enabled)
{
	m_pos = pos;
	m_enabled = enabled;

	if (width)
	{
		m_width = *width;
	}
	else
	{
		const Margin margin{ 5.0, 0.0 };
		m_width = FontAsset(U"Font")(m_text).region(m_pos).w + margin.getHorizontal();
	}

	bool isPushed = false;

	if (m_enabled)
	{
		if (Cursor::OnClientRect())
		{
			if (not m_isPressed && region().leftClicked())
			{
				m_isPressed = true;
			}
			else if (m_isPressed && MouseL.up())
			{
				if (region().leftReleased())
				{
					isPushed = true;
				}

				m_isPressed = false;
			}
		}
	}
	else
	{
		m_isPressed = false;
	}

	if (region().mouseOver())
	{
		Mouse::ClearLRInput();
	}

	return isPushed;
}

void Button::draw() const
{
	Color baseColor = m_palette.baseColor;
	Color textColor = m_palette.textColor;
	Color frameColor = m_palette.frameColor;
	double thickness = 1.0;

	if (not m_enabled)
	{
		baseColor = m_palette.disabledBaseColor;
		textColor = m_palette.disabledTextColor;
		frameColor = m_palette.disabledFrameColor;
	}
	else if (m_isPressed && region().mouseOver())
	{
		baseColor = m_palette.pressedBaseColor;
		textColor = m_palette.pressedTextColor;
		frameColor = m_palette.pressedFrameColor;
		thickness *= 1.5;
	}
	else if (m_isPressed || region().mouseOver())
	{
		baseColor = m_palette.mouseOverBaseColor;
		textColor = m_palette.mouseOverTextColor;
		frameColor = m_palette.mouseOverFrameColor;
		thickness *= 1.5;
	}

	region()
		.draw(baseColor)
		.drawFrame(thickness, 0.0, frameColor);

	// テキストの描画
	{
		const Rect clip = RectF{ m_pos, m_width, Height }.stretched(-5.0, 0.0).asRect();
		Graphics2D::SetScissorRect(clip);
		RasterizerState rs = RasterizerState::Default2D;
		rs.scissorEnable = true;
		const ScopedRenderStates2D state{ rs };

		FontAsset(U"Font")(m_text).drawAt(clip.center(), textColor);
	}
}


RoundRect Button::region() const
{
	const double r = 7.0;

	return RoundRect{ m_pos, m_width, Height, r };
}


Button::ColorPalette Button::getPalette() const
{
	return m_palette;
}

void Button::setPalette(const ColorPalette& palette)
{
	m_palette = palette;
}
