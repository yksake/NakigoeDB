#pragma once

class Button
{
public:
	struct ColorPalette
	{
		Color baseColor = Color{ 255 };
		Color textColor = Color{ 0 };
		Color frameColor = Color{ 60 };
		Color mouseOverBaseColor = Color{ 230, 242, 255 };
		Color mouseOverTextColor = textColor;
		Color mouseOverFrameColor = Color{ 90, 178, 255 };
		Color pressedBaseColor = Color{ 205, 227, 255 };
		Color pressedTextColor = textColor;
		Color pressedFrameColor = mouseOverFrameColor;
		Color disabledBaseColor = Color{ 200 };
		Color disabledTextColor = textColor;
		Color disabledFrameColor = frameColor;

	};

	static constexpr double Height = 35.0;

	Button(const StringView text);
	Button(const StringView text, const ColorPalette& palette);

	bool update(const Vec2& pos, const Optional<double>& width = unspecified, const bool enabled = true);
	void draw(const bool mouseOverEnabled = true) const;

	ColorPalette getPalette() const;
	void setPalette(const ColorPalette& palette);

private:
	String m_text;
	Vec2 m_pos = Vec2::Zero();
	double m_width = 0.0;

	bool m_enabled = true;
	bool m_isPressed = false;

	ColorPalette m_palette;

	RoundRect region() const;
};
