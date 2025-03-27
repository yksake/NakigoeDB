#include "HoverText.hpp"
#include "Margin.hpp"

namespace HoverText
{
	void Draw(const StringView text, const Vec2& bottomLeft, const RectF& clipRegion)
	{
		RectF textRegion = FontAsset(U"Font")(text).region();
		const Vec2 relPos = { 0.0, -8.0 };
		textRegion.setPos(bottomLeft.x + relPos.x, bottomLeft.y - textRegion.h + relPos.y);

		const Margin margin{ 10.0 };
		const double r = 8.0;
		RoundRect roundRect = textRegion.stretched(margin.top, margin.right, margin.bottom, margin.left).rounded(r);

		// 描画位置を clipRegion 内に収める
		{
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

			textRegion.moveBy(fix);
			roundRect.moveBy(fix);
		}


		// RoundRect の描画
		{
			const Vec2 shadowOffset = { 0.0, 2.0 };
			const double shadowBlur = 8.0;
			const double shadowSpread = 0.8;
			const Color baseColor = Color{ 255 };
			const double frameThickness = 1.5;
			const Color frameColor = Color{ 80 };

			roundRect
				.drawShadow(shadowOffset, shadowBlur, shadowSpread)
				.draw(baseColor)
				.drawFrame(frameThickness, frameColor);
		}

		const Color textColor = Color{ 0 };

		FontAsset(U"Font")(text).draw(textRegion, textColor);
	}
}
