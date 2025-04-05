#include "TagSettingDialog.hpp"

TagSettingDialog::TagSettingDialog(const StringView lpleDisplayGen, const StringView laDisplayGen, const std::function<void(StringView, StringView)>& closeFunc)
	: BaseDialog()
{
	// ピカブイ
	{
		size_t index = 0;

		if (not lpleDisplayGen)
		{
			index = 2;
		}
		else if (lpleDisplayGen == U"7")
		{
			index = 0;
		}
		else if (lpleDisplayGen == U"8")
		{
			index = 1;
		}

		m_lpleOptions.setIndex(index);
	}

	// Legendsアルセウス
	{
		size_t index = 0;

		if (not laDisplayGen)
		{
			index = 2;
		}
		else if (laDisplayGen == U"8")
		{
			index = 0;
		}
		else if (laDisplayGen == U"9")
		{
			index = 1;
		}

		m_legendsArceusOptions.setIndex(index);
	}

	m_closeFunc = closeFunc;
}

void TagSettingDialog::update()
{
	if (not m_isActive)
	{
		return;
	}

	const RoundRect rect = region();

	// ラジオボタン
	{
		const double x = rect.leftCenter().x + TextMargin.left;
		double y = rect.topCenter().y + TitleHeight + TextMargin.top;
		const double w = rect.w - TextMargin.getHorizontal();

		m_lpleOptions.update(Vec2{ x, y }, w);

		y = m_lpleOptions.region().bottomY() + TitleHeight + TextMargin.getVertical();

		m_legendsArceusOptions.update(Vec2{ x, y }, w);
	}

	// ボタン
	{
		const double buttonWidth = 120.0;
		const double buttonInterval = 20.0;
		const double buttonY = region().bottomCenter().y - 50.0;

		// OK ボタン
		if (m_okButton.update(Vec2{ region().center().x - (buttonInterval * 0.5) - buttonWidth, buttonY }, buttonWidth))
		{
			const auto lpleIndex = m_lpleOptions.getIndex();
			const auto laIndex = m_legendsArceusOptions.getIndex();

			if (lpleIndex && laIndex)
			{
				String lpleDisplayGen, laDisplayGen;

				if (lpleIndex == 0)
				{
					lpleDisplayGen = U"7";
				}
				else if (lpleIndex == 1)
				{
					lpleDisplayGen = U"8";
				}

				if (laIndex == 0)
				{
					laDisplayGen = U"8";
				}
				else if (laIndex == 1)
				{
					laDisplayGen = U"9";
				}

				m_closeFunc(lpleDisplayGen, laDisplayGen);
			}

			m_isActive = false;
		}

		// キャンセルボタン
		if (m_cancelButton.update(Vec2{ region().center().x + (buttonInterval * 0.5), buttonY }, buttonWidth))
		{
			m_isActive = false;
		}
	}

	if (Scene::Rect().leftClicked() && not region().mouseOver())
	{
		m_isActive = false;
	}

	if (Cursor::OnClientRect())
	{
		Mouse::ClearLRInput();
	}
}

void TagSettingDialog::draw() const
{
	if (not m_isActive)
	{
		return;
	}

	Scene::Rect().draw(Color{ 0, 128 });

	const RoundRect rect = region();
	rect.draw(Palette::White);

	const double textX = rect.leftCenter().x + TextMargin.left;
	double textY = rect.topCenter().y + TextMargin.top;

	FontAsset(U"FontMedium")(U"ピカブイ").draw(Vec2{ textX, textY }, Palette::Black);
	m_lpleOptions.draw();

	textY = m_lpleOptions.region().bottomY() + TextMargin.getVertical();
	FontAsset(U"FontMedium")(U"Legendsアルセウス").draw(Vec2{ textX, textY }, Palette::Black);
	m_legendsArceusOptions.draw();

	m_okButton.draw();
	m_cancelButton.draw();
}


bool TagSettingDialog::isActive() const
{
	return m_isActive;
}

void TagSettingDialog::close()
{
	m_isActive = true;
}


RoundRect TagSettingDialog::region() const
{
	RoundRect rect{ 0, 0, 300, 370, 8 };

	return rect.setCenter(Scene::CenterF());
}
