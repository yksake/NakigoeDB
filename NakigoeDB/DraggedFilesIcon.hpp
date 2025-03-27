#pragma once

class DraggedFilesIcon
{
public:
	struct ColorPalette
	{
		Color baseColor = Color{ 255 };
		Color frameColor = Color{ 50 };
		Color iconColor = Color{ 128 };
		Color subtoneColor = frameColor;
		Color subtoneFrameColor = baseColor;
		Color textColor = baseColor;
	};

	DraggedFilesIcon() = default;
	DraggedFilesIcon(const size_t fileCount, const size_t validFileCount);

	void draw(const Vec2& pos) const;

	ColorPalette getPalette() const;
	void setPalette(const ColorPalette& palette);

private:
	size_t m_fileCount = 0;
	size_t m_validFileCount = 0;

	ColorPalette m_palette;
};
