#include "Library.hpp"
#include "TagLoader.hpp"
#include "CryLoader.hpp"
#include "AddFilesDialog.hpp"
#include "TagSettingDialog.hpp"
#include "AudioPathSettingDialog.hpp"
#include "TypeFilteringDialog.hpp"
#include "HoverText.hpp"

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

	getData().saveData.load();

	// メニューバーを初期化
	{
		const Array<std::pair<String, Array<String>>> items
		{
			{ U"ファイル", { U"ファイル登録", U"世代の表示設定", U"終了"} },
			{ U"表示", { U"旧鳴き声のみ表示", U"新鳴き声のみ表示", U"全鳴き声を表示", U"タイプで絞り込み" } },
			{ U"ヘルプ", { U"GitHub", U"ライセンス", U"バージョン" } }
		};
		m_menubar = SimpleMenuBar{ items };

		SimpleMenuBar::ColorPalette palette
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

		// 鳴き声の新旧フィルタリングを適用
		{
			MenuBarItemIndex itemIndex = { 1, 0 };
			const auto cryVerFilter = getData().saveData.getCryVerFilter();

			if (not cryVerFilter)
			{
				itemIndex = { 1, 2 };
			}
			else if (cryVerFilter == 1)
			{
				itemIndex = { 1, 0 };
			}
			else if (cryVerFilter == 2)
			{
				itemIndex = { 1, 1 };
			}

			m_menubar.setItemChecked(itemIndex, true);
		}

		if (getData().saveData.getTypeFilter())
		{
			m_menubar.setItemChecked(MenuBarItemIndex{ 1, 3 }, true);
		}
	}

	getData().basicTags = TagLoader::LoadMainTagNames();
	getData().userTags = TagLoader::LoadUserTags();

	getData().cries.clear();
	for (const auto& cry : CryLoader::Load(getData().userTags))
	{
		getData().cries << std::make_shared<CryData>(cry);
	}

	m_audioPlayer = AudioPlayer{ Vec2{ 0.0, SimpleMenuBar::MenuBarHeight } };

	filterItem();

	m_tagPanel.load(getData().basicTags, getData().userTags);
}

void Library::update()
{
	getData().saveData.update();

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
		const RectF clip = Scene::Rect().stretched(-SimpleMenuBar::MenuBarHeight, 0, 0, 0);

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
		filterItem();
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

		const RectF clip = Scene::Rect().stretched(-SimpleMenuBar::MenuBarHeight, 0, 0, 0);
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

				registerFile(static_cast<int32>(index) + 1);
			};

			m_dialog = std::make_unique<AddFilesDialog>(closeFunc);
		}
		else if (item == MenuBarItemIndex{ 0, 1 }) // ファイル > タグの設定
		{
			auto closeFunc = [&](const StringView lpleDisplayGen, const StringView laDisplayGen)
			{
				String prevLPLE = getData().saveData.getLPLEDisplayGen();
				String prevLA = getData().saveData.getLADisplayGen();

				getData().saveData.setDisplayGen(lpleDisplayGen, laDisplayGen);

				// 表示中のタグに変更があったら再検索
				{
					Array<TagData> changedGenerations;
					const String category = U"Gen";

					const String lple = getData().saveData.getLPLEDisplayGen();
					if (prevLPLE != lple)
					{
						if (prevLPLE)
						{
							changedGenerations << TagData{ .category = category, .value = prevLPLE };
						}

						if (lple)
						{
							changedGenerations << TagData{ .category = category, .value =lple };
						}
					}

					const String la = getData().saveData.getLADisplayGen();
					if (prevLA != la)
					{
						if (prevLA)
						{
							changedGenerations << TagData{ .category = category, .value = prevLA };
						}

						if (la)
						{
							changedGenerations << TagData{ .category = category, .value = la };
						}
					}

					if (not changedGenerations)
					{
						return;
					}

					const Array<TagData> filters = m_tagPanel.getSelectedTags();

					for (const auto& filter : filters)
					{
						if (filter.category != category)
						{
							continue;
						}

						for (const auto& changed : changedGenerations)
						{
							if (filter == changed)
							{
								const auto& [andFilters, orFilters] = separateFilters(filters);

								m_filePanel.filter(getData().cries, andFilters, orFilters);

								return;
							}
						}
					}
				}
			};

			m_dialog = std::make_unique<TagSettingDialog>(getData().saveData.getLPLEDisplayGen(), getData().saveData.getLADisplayGen(), closeFunc);
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

				getData().saveData.setCryVerFilter(1);
				filterItem();
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

				getData().saveData.setCryVerFilter(2);
				filterItem();
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

				getData().saveData.setCryVerFilter(none);
				filterItem();
			}
		}
		else if (item == MenuBarItemIndex{ 1, 3 }) // 表示 > タイプで絞り込み
		{
			auto closeFunc = [&](const Optional<TagData>& filter)
			{
				if (getData().saveData.getTypeFilter() != filter)
				{
					getData().saveData.setTypeFilter(filter);

					filterItem();
				}

				m_menubar.setItemChecked(MenuBarItemIndex{ 1, 3 }, getData().saveData.getTypeFilter().has_value());
			};

			m_dialog = std::make_unique<TypeFilteringDialog>(getData().saveData.getTypeFilter(), closeFunc);
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
			System::MessageBoxOK(U"鳴き声DB v0.2.1", MessageBoxStyle::Info);
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

	filterItem();
}


void Library::draw() const
{
	const bool isMouseOverMenuBar = (Cursor::PosF().y < SimpleMenuBar::MenuBarHeight) || m_menubar.mouseOver();
	const bool isDialogActive = m_dialog && m_dialog->isActive();

	m_audioPlayer.draw();

	m_tagPanel.draw(not isMouseOverMenuBar && not isDialogActive);

	m_filePanel.draw();

	// サイドパネルの境界線
	{
		const RectF line = { m_sidePanelWidth, 0.0, 2.0, Scene::Height() };

		line.draw(Color{ 50 });
	}

	m_menubar.draw();

	if (not m_menubar.mouseOver() && not isDialogActive && not m_dragIcon && not m_rightClickMenu && Window::GetState().focused)
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

	// ピカブイ、Legendsアルセウスを他世代に含める
	for (const auto& filter : filters)
	{
		if (filter.category != U"Gen")
		{
			andFilters << filter;
			continue;
		}

		bool isHit = false;

		if (getData().saveData.getLPLEDisplayGen() == filter.value)
		{
			TagData lpleTag = filter;
			lpleTag.value = U"lple";

			orFilters << lpleTag;

			isHit = true;
		}

		if (getData().saveData.getLADisplayGen() == filter.value)
		{
			TagData laTag = filter;
			laTag.value = U"la";

			orFilters << laTag;

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

	// 鳴き声のバージョンの表示設定でフィルタリング
	if (not filters.contains_if([](const TagData& tag) { return tag.category == U"CryVer"; }) && getData().saveData.getCryVerFilter().has_value())
	{
		andFilters << TagData{ .category = U"CryVer", .value = Format(*getData().saveData.getCryVerFilter()) };
	}

	// タイプの絞り込み設定でフィルタリング
	if (getData().saveData.getTypeFilter())
	{
		andFilters << *getData().saveData.getTypeFilter();
	}

	return { andFilters, orFilters };
}

void Library::filterItem()
{
	Array<TagData> filters = m_tagPanel.getSelectedTags();

	const auto& [andFilters, orFilters] = separateFilters(filters);

	m_filePanel.filter(getData().cries, andFilters, orFilters);
}


RectF Library::fileArea() const
{
	return Scene::Rect().stretched(-SimpleMenuBar::MenuBarHeight, 0, 0, -static_cast<int32>(m_sidePanelWidth));
}

RectF Library::tagArea() const
{
	const double y = SimpleMenuBar::MenuBarHeight + AudioPlayer::Height;
	const double h = Scene::Height() - y;

	return RectF{ 0.0, y, m_sidePanelWidth, h };
}
