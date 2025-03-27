#include "AudioPathSettingDialog.hpp"
#include "CryLoader.hpp"
#include "Margin.hpp"
#include "HoverText.hpp"

AudioPathSettingDialog::AudioPathSettingDialog(const std::weak_ptr<CryData>& cryData, const std::function<void()>& closeFunc)
{
	m_data = cryData;
	m_closeFunc = closeFunc;
}

void AudioPathSettingDialog::update()
{
	if (not m_isActive)
	{
		return;
	}

	const RoundRect rect = region();

	// 参照、登録解除ボタン
	{
		FilePath path;
		if (const auto cry = m_data.lock())
		{
			if (cry->path)
			{
				path = cry->path;
			}
		}

		const double buttonWidth = 100.0;

		const Vec2 basePos = { rect.leftCenter().x, rect.topCenter().y };
		const Vec2 browseButtonRelPos = { 170.0, 65.0 };
		const Vec2 deleteButtonRelPos = browseButtonRelPos.movedBy(buttonWidth + 10.0, 0.0);


		// 参照ボタン
		if (m_browseButton.update(basePos + browseButtonRelPos, buttonWidth))
		{
			Optional<FilePath> newPath = Dialog::OpenFile({ FileFilter::AllAudioFiles() }, path, U"鳴き声の音声ファイルを登録");

			if (newPath)
			{
				m_path = *newPath;
				m_isDeletePath = false;
			}
		}

		// 登録解除ボタン
		if (m_deleteButton.update(basePos + deleteButtonRelPos, buttonWidth, path || m_path))
		{
			m_isDeletePath = true;
			m_path.clear();
		}
	}

	// OK, キャンセルボタン
	{
		const double buttonWidth = 120.0;
		const double buttonInterval = 35.0;
		const double buttonY = region().bottomCenter().y - 50.0;

		// OK ボタン
		if (m_okButton.update(Vec2{ region().center().x - (buttonInterval * 0.5) - buttonWidth, buttonY }, buttonWidth))
		{
			if (const auto cry = m_data.lock())
			{
				const FilePath prevPath = cry->path;

				if (m_isDeletePath)
				{
					cry->path.clear();
				}
				else if (m_path)
				{
					cry->path = m_path;
				}

				if (prevPath != cry->path)
				{
					m_closeFunc();
				}
			}

			m_isActive = false;
		}

		// キャンセルボタン
		if (m_cancelButton.update(Vec2{ region().center().x + (buttonInterval * 0.5), buttonY }, buttonWidth))
		{
			m_isActive = false;
		}
	}

	if (Cursor::OnClientRect())
	{
		Mouse::ClearLRInput();
	}
}

void AudioPathSettingDialog::draw() const
{
	if (not m_isActive)
	{
		return;
	}

	Scene::Rect().draw(Color{ 0, 128 });

	const RoundRect rect = region();
	rect.draw(Palette::White);

	m_browseButton.draw();
	m_deleteButton.draw();

	m_okButton.draw();
	m_cancelButton.draw();

	// テキスト
	{
		String dataTitle = U"----: -----";
		FilePath oldPath = U"---";

		if (const auto cry = m_data.lock())
		{
			dataTitle = U"{:0>4}"_fmt(cry->key.number) + U": ";

			if (cry->name.customName)
			{
				dataTitle += cry->name.customName;
			}
			else
			{
				dataTitle += cry->name.value;
			}

			if (cry->subName.customName)
			{
				dataTitle += U" " + cry->subName.customName;
			}
			else if (cry->subName.value)
			{
				dataTitle += U" " + cry->subName.value;
			}

			if (cry->key.cryVer == 1)
			{
				dataTitle += U" (旧)";
			}
			else if (cry->key.cryVer == 2)
			{
				dataTitle += U" (新)";
			}

			if (cry->path)
			{
				oldPath = cry->path;
			}
		}

		const Margin textMargin{ 20.0, 10.0, 10.0, 10.0 };
		const double textLeftX = rect.leftCenter().x + textMargin.left;
		double textTopY = rect.topCenter().y + textMargin.top;
		const SizeF pathRegionSize = { rect.w - textMargin.getHorizontal(), 25.0 };
		const double pathTextSize = 16.0;
		const Color textColor = Color{ 0 };
		const Color newPathColor = Color{ 255, 0, 0 };

		FontAsset(U"Font")(dataTitle).draw(Vec2{ textLeftX, textTopY }, textColor);
		textTopY += 50.0;

		FontAsset(U"Font")(U"鳴き声ファイル:").draw(Vec2{ textLeftX, textTopY }, textColor);
		textTopY += 45.0;

		// 現在のパス
		{
			RectF oldPathRegion = { textLeftX, textTopY, pathRegionSize };

			const bool ok = FontAsset(U"Font")(oldPath).draw(pathTextSize, oldPathRegion, textColor);

			if (not ok && Cursor::OnClientRect() && oldPathRegion.mouseOver())
			{
				HoverText::Draw(oldPath, Cursor::PosF(), Scene::Rect());
			}

			textTopY += 40.0;
		}

		// 変更後のパス
		if (m_path || m_isDeletePath)
		{
			FontAsset(U"Font")(U"変更後:").draw(pathTextSize, Vec2{ textLeftX, textTopY }, newPathColor);
			textTopY += 25.0;

			if (m_path)
			{
				RectF newPathRegion = { textLeftX, textTopY, pathRegionSize };

				const bool ok = FontAsset(U"Font")(m_path).draw(pathTextSize, newPathRegion, newPathColor);

				if (not ok && Cursor::OnClientRect() && newPathRegion.mouseOver())
				{
					HoverText::Draw(m_path, Cursor::PosF(), Scene::Rect());
				}
			}
			else
			{
				FontAsset(U"Font")(U"登録解除").draw(pathTextSize, Vec2{ textLeftX, textTopY }, newPathColor);
			}
		}
	}
}


bool AudioPathSettingDialog::isActive() const
{
	return m_isActive;
}

void AudioPathSettingDialog::close()
{
	m_isActive = false;
}


RoundRect AudioPathSettingDialog::region() const
{
	RoundRect rect{ 0, 0, 400, 300, 8 };

	return rect.setCenter(Scene::CenterF());
}
