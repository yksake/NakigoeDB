#include "TypeFilteringDialog.hpp"

const Array<String> TypeFilteringDialog::Categories = { U"Type", U"Type1", U"Type2" };

const Array<String> TypeFilteringDialog::Types = {
	U"ノーマル",
		U"ほのお",
		U"みず",
		U"くさ",
		U"でんき",
		U"こおり",
		U"かくとう",
		U"どく",
		U"じめん",
		U"ひこう",
		U"エスパー",
		U"むし",
		U"いわ",
		U"ゴースト",
		U"ドラゴン",
		U"あく",
		U"はがね",
		U"フェアリー"
};


TypeFilteringDialog::TypeFilteringDialog(const Optional<TagData>& filter, const std::function<void(Optional<TagData>)>& closeFunc)
{
	m_closeFunc = closeFunc;

	const Font& font = FontAsset(U"Font");
	const double fontSize = 20;

	m_categoryPulldown = PulldownMenu{ { U"タイプ1または2", U"タイプ1", U"タイプ2" }, font, fontSize };

	m_typePulldown = PulldownMenu{ Types, font, fontSize, 200 };

	if (filter)
	{
		size_t categoryIndex = 0;

		for (const auto& [i, category] : Indexed(Categories))
		{
			if (filter->category == category)
			{
				categoryIndex = i;
				break;
			}
		}

		m_categoryPulldown.setIndex(categoryIndex);

		size_t typeIndex = 0;

		for (const auto& [i, type] : Indexed(Types))
		{
			if (filter->value == type)
			{
				typeIndex = i;
				break;
			}
		}

		m_typePulldown.setIndex(typeIndex);
	}
}


void TypeFilteringDialog::update()
{
	if (not m_isActive)
	{
		return;
	}

	// プルダウンメニュー
	{
		if (Cursor::OnClientRect() && MouseL.down())
		{
			if (not m_categoryPulldown.mouseOver())
			{
				m_categoryPulldown.close();
			}

			if (not m_typePulldown.mouseOver())
			{
				m_typePulldown.close();
			}
		}

		Vec2 pos = Vec2{ region().leftCenter().x, region().topCenter().y } + Vec2{ 10, 60 };

		m_categoryPulldown.update(pos);
		pos.x += m_categoryPulldown.getRect().w + 40;

		m_typePulldown.update(pos);
	}

	// ボタン
	{
		const double buttonWidth = 150.0;
		const double buttonInterval = 10.0;
		const double buttonY = region().bottomCenter().y - 50.0;

		// OK ボタン
		if (m_okButton.update(Vec2{ region().center().x - (buttonWidth * 1.5) - buttonInterval, buttonY }, buttonWidth))
		{
			TagData filter;

			filter.category = Categories[m_categoryPulldown.getIndex()];
			filter.value = Types[m_typePulldown.getIndex()];

			m_closeFunc(filter);

			m_isActive = false;
		}

		// 絞り込み解除ボタン
		if (m_clearButton.update(Vec2{ region().center().x - (buttonWidth * 0.5), buttonY }, buttonWidth))
		{
			m_closeFunc(unspecified);

			m_isActive = false;
		}

		// キャンセルボタン
		if (m_cancelButton.update(Vec2{ region().center().x + (buttonWidth * 0.5) + buttonInterval, buttonY}, buttonWidth))
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

void TypeFilteringDialog::draw() const
{
	if (not m_isActive)
	{
		return;
	}

	Scene::Rect().draw(Color{ 0, 128 });

	region().draw(Color{ 255 });

	FontAsset(U"FontMedium")(U"タイプで絞り込み").draw(Vec2{ region().leftCenter().x, region().topCenter().y }.movedBy(10, 15), Color{ 0 });

	// ボタン
	{
		const bool mouseOverEnabled = (not m_categoryPulldown.mouseOver()) && (not m_typePulldown.mouseOver());

		m_okButton.draw(mouseOverEnabled);
		m_clearButton.draw(mouseOverEnabled);
		m_cancelButton.draw(mouseOverEnabled);
	}

	m_typePulldown.draw();

	m_categoryPulldown.draw();

	FontAsset(U"Font")(U"が").draw(Arg::bottomLeft = m_categoryPulldown.getRect().br().movedBy(10, 0), Color{ 0 });
}


bool TypeFilteringDialog::isActive() const
{
	return m_isActive;
}

void TypeFilteringDialog::close()
{
	m_isActive = true;
}


RoundRect TypeFilteringDialog::region() const
{
	RoundRect rect{ 0, 0, 500, 210, 8 };

	return rect.setCenter(Scene::CenterF());
}
