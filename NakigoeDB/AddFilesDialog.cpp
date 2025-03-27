#include "AddFilesDialog.hpp"
#include "Margin.hpp"

AddFilesDialog::AddFilesDialog(const std::function<void(size_t)>& closeFunc)
	: BaseDialog()
{
	m_closeFunc = closeFunc;
}


void AddFilesDialog::update()
{
	if (not m_isActive)
	{
		return;
	}

	const RoundRect rect = region();

	// ラジオボタン
	{
		const Margin margin{ 10.0, 10.0, 0.0, 10.0 };

		const double x = rect.leftCenter().x + margin.left;
		const double y = rect.topCenter().y + margin.top;
		const double w = rect.w - margin.getHorizontal();

		m_radioButtons.update(Vec2{ x, y }, w);
	}

	// ボタン
	{
		const double buttonWidth = 120.0;
		const double buttonInterval = 20.0;
		const double buttonY = region().bottomCenter().y - 50.0;

		// OK ボタン
		if (m_okButton.update(Vec2{ region().center().x - (buttonInterval * 0.5) - buttonWidth, buttonY }, buttonWidth))
		{
			if (const auto index = m_radioButtons.getIndex())
			{
				m_closeFunc(*index);
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

void AddFilesDialog::draw() const
{
	if (not m_isActive)
	{
		return;
	}

	Scene::Rect().draw(Color{ 0, 128 });

	const RoundRect rect = region();
	rect.draw(Palette::White);

	m_radioButtons.draw();

	m_okButton.draw();
	m_cancelButton.draw();
}


bool AddFilesDialog::isActive() const
{
	return m_isActive;
}

void AddFilesDialog::close()
{
	m_isActive = false;
}


RoundRect AddFilesDialog::region() const
{
	RoundRect rect{ 0, 0, 300, 200, 8 };

	return rect.setCenter(Scene::CenterF());
}
