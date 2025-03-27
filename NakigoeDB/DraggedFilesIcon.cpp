#include "DraggedFilesIcon.hpp"

DraggedFilesIcon::DraggedFilesIcon(const size_t fileCount, const size_t validFileCount)
{
	m_fileCount = fileCount;
	m_validFileCount = validFileCount;
}

void DraggedFilesIcon::draw(const Vec2& pos) const
{
	// Squircle

	const double r = 35.0;
	const double rel = 5.0;
	const Vec2 center = pos.movedBy(0.0, -(r - rel));
	const uint32 quality = 100;

	Shape2D shape = Shape2D::Squircle(r, center, quality);

	const double frameThickness = 3.0;

	shape
		.draw(m_palette.baseColor)
		.drawFrame(frameThickness, m_palette.frameColor);

	// Icon

	const double iconSize = 50.0;
	const String fileIcon = U"\U000F0224";

	FontAsset(U"FontIconMD")(fileIcon).drawAt(iconSize, center, m_palette.iconColor);

	// Count
	if (m_fileCount != 1)
	{
		const String text = Format(m_fileCount);
		const double textSize = 14;

		bool isClipped = false;

		RectF subtone = FontAsset(U"FontMedium")(text).regionAt(textSize, center);
		if ((r - rel) * 2.0 < subtone.w)
		{
			subtone.w = (r - rel) * 2.0;
			subtone.setCenter(center);

			isClipped = true;
		}

		const double subtoneR = 5.0;
		const double subtoneFrameThickness = 2.5;

		subtone
			.stretched(rel, 0.0)
			.rounded(subtoneR)
			.draw(m_palette.subtoneColor)
			.drawFrame(0.0, subtoneFrameThickness, m_palette.subtoneFrameColor);

		if (isClipped)
		{
			FontAsset(U"FontMedium")(text).draw(textSize, subtone, m_palette.textColor);
		}
		else
		{
			FontAsset(U"FontMedium")(text).drawAt(textSize, center, m_palette.textColor);
		}
	}
}


DraggedFilesIcon::ColorPalette DraggedFilesIcon::getPalette() const
{
	return m_palette;
}

void DraggedFilesIcon::setPalette(const ColorPalette& palette)
{
	m_palette = palette;
}
