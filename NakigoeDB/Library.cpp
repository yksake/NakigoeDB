#include "Library.hpp"
#include "TagLoader.hpp"
#include "CryLoader.hpp"
#include "AddFilesDialog.hpp"
#include "TagSettingDialog.hpp"
#include "AudioPathSettingDialog.hpp"
#include "HoverText.hpp"
#include "WindowStateChecker.hpp"

Library::Library(const InitData& init)
	: IScene(init)
{
	LicenseManager::AddLicense(
	LicenseInfo
	{
UR"-(NakigoeDB)-",
UR"-(Copyright (c) 2025 yakisake)-",
UR"-(Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.)-",
	});

	WindowStateChecker::Load();

	// MenuBar
	{
		const Array<std::pair<String, Array<String>>> items
		{
			{ U"ファイル", { U"ファイル登録", U"世代の表示設定", U"終了"}},
			{ U"表示", { U"旧鳴き声のみ表示", U"新鳴き声のみ表示", U"全鳴き声を表示" } },
			{ U"ヘルプ", { U"GitHub", U"ライセンス", U"バージョン" } }
		};
		m_menubar = MenuBar{ items };

		MenuBar::ColorPalette palette
		{
			.menuBarColor = Color{ 50 },
			.activeMenuColor = Color{ 70 },
			.menuTextColor = Palette::White,
			.itemBoxColor = Color{ 70 },
			.itemMouseoverColor = Color{ 220 },
			.itemTextColor = Palette::White,
			.itemMouseoverTextColor = Palette::Black
		};
		m_menubar.setColorPalette(palette);

		m_menubar.setItemChecked(MenuBarItemIndex{ 1, 0 }, true);
	}

	getData().basicTags = TagLoader::LoadMainTagNames();
	getData().userTags = TagLoader::LoadUserTags();

	getData().cries.clear();
	for (const auto& cry : CryLoader::Load(getData().userTags))
	{
		getData().cries << std::make_shared<CryData>(cry);
	}

	m_audioPlayer = AudioPlayer{ Vec2{ 0.0, MenuBar::MenuBarHeight } };

	TagData cryVersion{ .category = U"CryVer", .value = U"1" };
	m_filePanel.filter(getData().cries, { cryVersion });

	m_tagPanel.load(getData().basicTags, getData().userTags);
}

void Library::update()
{
	WindowStateChecker::Update();

	// ダイアログ
	if (m_dialog)
	{
		if (m_dialog->isActive())
		{
			m_dialog->update();
		}

		if (not m_dialog->isActive())
		{
			m_dialog.reset();
		}
	}

	// 右クリックメニュー
	if (m_rightClickMenu)
	{
		const RectF clip = Scene::Rect().stretched(-MenuBar::MenuBarHeight, 0.0, 0.0, 0.0);

		if (m_rightClickMenu->update(clip))
		{
			auto closeFunc = [&]()
			{
				CryLoader::SaveAudioPathList(getData().cries);
			};

			m_dialog = std::make_unique<AudioPathSettingDialog>(m_rightClickMenu->getData(), closeFunc);
		}

		if (not m_rightClickMenu->isActive())
		{
			m_rightClickMenu.reset();
		}
	}

	// メニューバー
	updateMenuBar();

	const bool isDialogActive = m_dialog && m_dialog->isActive();

	// タグパネルを更新
	if (m_tagPanel.update(tagArea(), not isDialogActive))
	{
		// 選択タグで絞り込み

		Array<TagData> filters = m_tagPanel.getSelectedTags();

		const auto& [andFilters, orFilters] = separateFilters(filters);

		m_filePanel.filter(getData().cries, andFilters, orFilters);
	}

	// タグへのドロップ
	if (m_tagPanel.region().mouseOver() && MouseL.up())
	{
		m_tagPanel.dropItems(m_filePanel.getDraggingData());
	}

	// ファイルパネルを更新
	if (m_filePanel.update(fileArea(), not isDialogActive))
	{
		// 音声を再生
		if (const auto forcusedData = m_filePanel.getForcusedData())
		{
			m_audioPlayer.play(*forcusedData);
		}
	}

	// 右クリックメニューを作成
	if (const auto& rightClicked = m_filePanel.rightClickedData())
	{
		m_rightClickMenu = std::make_unique<ItemRightClickMenu>(*rightClicked, Cursor::PosF());

		const RectF clip = Scene::Rect().stretched(-MenuBar::MenuBarHeight, 0.0, 0.0, 0.0);
		m_rightClickMenu->update(clip);
	}

	// ドラッグアイコン
	if (m_filePanel.isStartDrag())
	{
		const auto& draggedDataList = m_filePanel.getDraggingData();

		const size_t size = draggedDataList.size();
		const size_t validSize = draggedDataList.count_if([](const std::weak_ptr<CryData>& cryPtr)
		{
			if (const auto cry = cryPtr.lock())
			{
				if (cry->path && FileSystem::IsFile(cry->path))
				{
					return true;
				}
			}

			return false;
		});

		m_dragIcon = std::make_unique<DraggedFilesIcon>(size, validSize);
	}
	if (m_filePanel.isFinishDrag())
	{
		m_dragIcon.reset();
	}

	m_audioPlayer.update(m_sidePanelWidth);
}

void Library::updateMenuBar()
{
	if (const auto item = m_menubar.update())
	{
		if (item == MenuBarItemIndex{ 0, 0 }) // ファイル > 追加
		{
			auto closeFunc = [&](const size_t index)
			{
				if (index == 2)
				{
					System::MessageBoxOK(U"準備中の機能です", MessageBoxStyle::Info);
					return;
				}

				registerFile(index + 1);
			};

			m_dialog = std::make_unique<AddFilesDialog>(closeFunc);
		}
		else if (item == MenuBarItemIndex{ 0, 1 }) // ファイル > タグの設定
		{
			auto closeFunc = [&](const size_t lpleIndex, const size_t laIndex)
			{
				m_lpleDisplayOption = lpleIndex;
				m_laDisplayOption = laIndex;
			};

			m_dialog = std::make_unique<TagSettingDialog>(m_lpleDisplayOption, m_laDisplayOption, closeFunc);
		}
		else if (item == MenuBarItemIndex{ 0, 2 }) // ファイル > 終了
		{
			System::Exit();
		}
		else if (item == MenuBarItemIndex{ 1, 0 }) // 表示 > 旧鳴き声
		{
			if (not m_menubar.getItemChecked(MenuBarItemIndex{ 1, 0 }))
			{
				m_menubar
					.setItemChecked(MenuBarItemIndex{ 1, 0 }, true)
					.setItemChecked(MenuBarItemIndex{ 1, 1 }, false)
					.setItemChecked(MenuBarItemIndex{ 1, 2 }, false);

				Array<TagData> filter = m_tagPanel.getSelectedTags();
				filter << TagData{ .category = U"CryVer", .value = U"1" };
				m_filePanel.filter(getData().cries, filter);
			}
		}
		else if (item == MenuBarItemIndex{ 1, 1 }) // 表示 > 新鳴き声
		{
			if (not m_menubar.getItemChecked(MenuBarItemIndex{ 1, 1 }))
			{
				m_menubar
					.setItemChecked(MenuBarItemIndex{ 1, 0 }, false)
					.setItemChecked(MenuBarItemIndex{ 1, 1 }, true)
					.setItemChecked(MenuBarItemIndex{ 1, 2 }, false);

				Array<TagData> filter = m_tagPanel.getSelectedTags();
				filter << TagData{ .category = U"CryVer", .value = U"2" };
				m_filePanel.filter(getData().cries, filter);
			}
		}
		else if (item == MenuBarItemIndex{ 1, 2 }) // 表示 > 全鳴き声
		{
			if (not m_menubar.getItemChecked(MenuBarItemIndex{ 1, 2 }))
			{
				m_menubar
					.setItemChecked(MenuBarItemIndex{ 1, 0 }, false)
					.setItemChecked(MenuBarItemIndex{ 1, 1 }, false)
					.setItemChecked(MenuBarItemIndex{ 1, 2 }, true);

				Array<TagData> filter = m_tagPanel.getSelectedTags();
				filter << TagData{ .category = U"CryVer", .value = U"0" };
				m_filePanel.filter(getData().cries, filter);
			}
		}
		else if (item == MenuBarItemIndex{ 2, 0 }) // ヘルプ > GitHub
		{
			System::LaunchBrowser(U"https://github.com/yksake/NakigoeDB");
		}
		else if (item == MenuBarItemIndex{ 2, 1 }) // ヘルプ > ライセンス
		{
			LicenseManager::ShowInBrowser();
		}
		else if (item == MenuBarItemIndex{ 2, 2 }) // ヘルプ > バージョン
		{
			System::MessageBoxOK(U"鳴き声DB v0.1.0", MessageBoxStyle::Info);
		}
	}
}

void Library::registerFile(const int32 cryVer)
{
	const Array<FilePath> paths = Dialog::OpenFiles({ FileFilter::AllAudioFiles() }, U"", U"鳴き声の音声ファイルを追加");
	if (not paths)
	{
		return;
	}

	size_t criesIndex = 0;
	for (const auto& path : paths)
	{
		const String fileName = FileSystem::FileName(path);
		const size_t numFront = fileName.indexOfAny(U"123456789");
		const size_t notNumFront = fileName.indexNotOfAny(U"0123456789");
		if (numFront == String::npos || notNumFront <= numFront)
		{
			continue;
		}

		const String numStr = fileName.substr(numFront, notNumFront - numFront);
		const int32 number = Parse<int32>(numStr);

		while (criesIndex < getData().cries.size())
		{
			auto& cryData = getData().cries[criesIndex];
			if (not cryData)
			{
				continue;
			}

			const auto& cryKey = cryData->key;

			if (number < cryKey.number)
			{
				break;
			}

			if (number == cryKey.number && ((cryKey.cryVer == 0) || (cryKey.cryVer == cryVer)))
			{
				cryData->path = path;

				++criesIndex;
				break;
			}

			++criesIndex;
		}
	}

	CryLoader::SaveAudioPathList(getData().cries);

	Array<TagData> filter = m_tagPanel.getSelectedTags();

	TagData cryVerFilter{ .category = U"CryVer", .value = U"0" };
	if (m_menubar.getItemChecked(MenuBarItemIndex{ 1, 0 }))
	{
		cryVerFilter.value = U"1";
	}
	else if (m_menubar.getItemChecked(MenuBarItemIndex{ 1, 1 }))
	{
		cryVerFilter.value = U"2";
	}

	filter << cryVerFilter;

	m_filePanel.filter(getData().cries, filter);
}


void Library::draw() const
{
	m_audioPlayer.draw();

	m_tagPanel.draw();

	m_filePanel.draw();

	// サイドパネルの境界線
	{
		const RectF line = { m_sidePanelWidth, 0.0, 2.0, Scene::Height() };

		line.draw(Color{ 50 });
	}

	m_menubar.draw();

	const bool isDialogActive = m_dialog && m_dialog->isActive();
	if (not m_menubar.itemBoxMouseOver() && not isDialogActive && not m_dragIcon && not m_rightClickMenu)
	{
		const String mouseOverText = m_filePanel.getMouseOverText();

		if (mouseOverText)
		{
			HoverText::Draw(mouseOverText, Cursor::PosF(), Scene::Rect());
		}
	}

	if (m_rightClickMenu)
	{
		m_rightClickMenu->draw();
	}

	if (m_dialog)
	{
		m_dialog->draw();
	}

	if (m_dragIcon)
	{
		m_dragIcon->draw(Cursor::PosF());
	}
}


std::pair<Array<TagData>, Array<TagData>> Library::separateFilters(const Array<TagData>& filters) const
{
	Array<TagData> andFilters, orFilters;

	const int32 lpleDisplayGen = 8 + int32(m_lpleDisplayOption) - 1;
	const int32 laDisplayGen = 10 + int32(m_laDisplayOption) - 1;

	// ピカブイ、Legendsアルセウスを他世代に含める
	for (const auto& filter : filters)
	{
		if (filter.category != U"Gen")
		{
			andFilters << filter;
			continue;
		}

		bool isHit = false;

		if ((m_lpleDisplayOption != 1) && (filter.value == Format(lpleDisplayGen)))
		{
			TagData lple = filter;
			lple.value = Format(lpleDisplayGen - m_lpleDisplayOption + 1);

			orFilters << lple;

			isHit = true;
		}

		if ((m_laDisplayOption != 1) && (filter.value == Format(laDisplayGen)))
		{
			TagData la = filter;
			la.value = Format(laDisplayGen - m_laDisplayOption + 1);

			orFilters << la;

			isHit = true;
		}

		if (isHit)
		{
			orFilters << filter;
		}
		else
		{
			andFilters << filter;
		}
	}

	// 鳴き声のバージョンでフィルタリング
	if (not filters.contains_if([](const TagData& tag) { return tag.category == U"CryVer"; }))
	{
		TagData cryVer{ .category = U"CryVer", .value = U"0" };
		if (m_menubar.getItemChecked(MenuBarItemIndex{ 1, 0 }))
		{
			cryVer.value = U"1";
		}
		else if (m_menubar.getItemChecked(MenuBarItemIndex{ 1, 1 }))
		{
			cryVer.value = U"2";
		}

		andFilters << cryVer;
	}

	return { andFilters, orFilters };
}


RectF Library::fileArea() const
{
	return Scene::Rect().stretched(-MenuBar::MenuBarHeight, 0, 0, -m_sidePanelWidth);
}

RectF Library::tagArea() const
{
	const double y = MenuBar::MenuBarHeight + AudioPlayer::Height;
	const double h = Scene::Height() - y;

	return RectF{ 0.0, y, m_sidePanelWidth, h };
}
