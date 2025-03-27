#pragma once

class RadioButtons
{
public:
	struct ColorPalette
	{
		Color backgroundColor = Color{ 255 };
		Color buttonBaseColor = Color{ 255 };
		Color buttonFrameColor = Color{ 60 };
		Color buttonFillColor = Color{ 90, 178, 255 };
		Color mouseOverColor = Color{ 230, 242, 255 };
		Color textColor = Color{ 0 };
	};

	RadioButtons(const Array<String>& options);
	RadioButtons(const Array<String>& options, const ColorPalette& palette);

	bool update(const Vec2& pos, const Optional<double>& width = unspecified, const bool enabled = true);
	void draw() const;

	RectF region() const;

	Optional<size_t> getIndex() const;
	void setIndex(const size_t index);

	ColorPalette getPalette() const;
	void setPalette(const ColorPalette& palette);

private:
	Array<String> m_options;
	size_t m_index = 0;
	Vec2 m_pos = Vec2::Zero();
	double m_width = 0.0;
	static constexpr double LineHeight = 35.0;
	static constexpr double ButtonSize = 18.0;
	static constexpr double ButtonMarginX = 5.0;
	static constexpr double TextMarginX = 5.0;

	ColorPalette m_palette;

	Circle buttonRegion(const size_t index) const;
	RectF textRegion(const size_t index) const;

	void checkWidth();
};
