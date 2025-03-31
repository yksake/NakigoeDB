#include "FilePanel.hpp"

bool FilePanel::update(const RectF& region, const bool enabled)
{
	m_region = region;

	m_prevDrag = m_isDrag;
	m_rightClickedIndex = none;

	updateScrollbar(enabled);

	return updateItems(enabled);
}

void FilePanel::updateScrollbar(const bool enabled)
{
	if (m_scrollbar.enabled() && enabled)
	{
		if (m_region.mouseOver())
		{
			if (Mouse::Wheel() < 0)
			{
				m_scrollLength -= FileItem::Height * 3.0;
			}
			else if (0 < Mouse::Wheel())
			{
				m_scrollLength += FileItem::Height * 3.0;
			}
		}
	}

	if (enabled)
	{
		if (KeyHome.down())
		{
			m_scrollbar.moveHome();
			m_scrollLength = m_scrollbar.scrollLength();

			if (m_fileItems)
			{
				m_fileItems.each([](FileItem& panel) { panel.setSelection(false); });
				m_fileItems.front().setSelection(true);

				m_forcusedItemIndex = 0;
				m_selectionAhchorIndex = m_forcusedItemIndex;
			}
		}
		else if (KeyEnd.down())
		{
			m_scrollbar.moveEnd();
			m_scrollLength = m_scrollbar.scrollLength();

			if (m_fileItems)
			{
				m_fileItems.each([](FileItem& panel) { panel.setSelection(false); });
				m_fileItems.back().setSelection(true);

				m_forcusedItemIndex = m_fileItems.size() - 1;
				m_selectionAhchorIndex = m_forcusedItemIndex;

			}
		}
	}

	const double itemTopY = m_region.topY() - m_scrollLength;
	const double itemH = m_fileItems.size() * 35.0;
	m_scrollbar.update(m_region, itemTopY, itemH);
	m_scrollLength = m_scrollbar.scrollLength();
}

bool FilePanel::updateItems(const bool enabled)
{
	const RectF itemRegion = m_region.stretched(0.0, -m_scrollbar.barArea().w, 0.0, 0.0);

	int8 moveInput = 0;

	// 上下移動入力
	{
		// 上キー
		if (KeyUp.down())
		{
			--moveInput;
		}
		else if (KeyUp.pressed() && Duration{ LongPressInterval * (m_longPressCountUp + 1) } <= KeyUp.pressedDuration())
		{
			--moveInput;
			++m_longPressCountUp;
		}
		else if (not KeyUp.pressed())
		{
			m_longPressCountUp = MinLongPressCount;
		}

		// 下キー
		if (KeyDown.down())
		{
			++moveInput;
		}
		else if (KeyDown.pressed() && Duration{ LongPressInterval * (m_longPressCountDown + 1) } <= KeyDown.pressedDuration())
		{
			++moveInput;
			++m_longPressCountDown;
		}
		else if (not KeyDown.pressed())
		{
			m_longPressCountDown = MinLongPressCount;
		}

		// W キー
		if (KeyW.down())
		{
			--moveInput;
		}
		else if (KeyW.pressed() && Duration{ LongPressInterval * (m_longPressCountW + 1) } <= KeyW.pressedDuration())
		{
			--moveInput;
			++m_longPressCountW;
		}
		else if (not KeyW.pressed())
		{
			m_longPressCountW = MinLongPressCount;
		}

		// S キー
		if (KeyS.down())
		{
			++moveInput;
		}
		else if (KeyS.pressed() && Duration{ LongPressInterval * (m_longPressCountS + 1) } <= KeyS.pressedDuration())
		{
			++moveInput;
			++m_longPressCountS;
		}
		else if (not KeyS.pressed())
		{
			m_longPressCountS = MinLongPressCount;
		}
	}

	// 上下移動入力を適用
	if (m_fileItems && enabled)
	{
		if (moveInput != 0)
		{
			const Array<size_t> selectedList = getSelectedIndex();

			if (not selectedList) // 選択されたアイテムがない場合
			{
				m_fileItems.front().setSelection(true);

				m_forcusedItemIndex = 0;
				m_selectionAhchorIndex = m_forcusedItemIndex;
			}
			else // 選択されたアイテムがある場合
			{
				const int32 newIndex = selectedList.back() + moveInput;

				if (InRange(newIndex, 0, int32(m_fileItems.size() - 1)))
				{
					m_fileItems.each([](FileItem& panel) { panel.setSelection(false); });
					m_fileItems[newIndex].setSelection(true);

					m_forcusedItemIndex = newIndex;
					m_selectionAhchorIndex = m_forcusedItemIndex;

					scrollToSelectionItem();
				}
			}
		}
	}

	// パネルを更新
	{
		double topY = m_region.topY() - m_scrollLength;
		for (auto&& panel : m_fileItems)
		{
			topY = panel.setTopY(topY);
			panel.update(m_region.leftX(), m_region.rightX());
		}
	}

	// 全選択
	if (m_fileItems && (KeyControl + KeyA).down())
	{
		m_fileItems.each([](FileItem& item) { item.setSelection(true); });
	}

	// クリックで選択
	if (Cursor::OnClientRect() && enabled)
	{
		if (m_region.rightClicked()) // 右クリック
		{
			const auto& mouseOverIndex = getHoverItemIndex(Cursor::PosF());
			m_rightClickedIndex = mouseOverIndex;

			if (mouseOverIndex)
			{
				m_fileItems.each([](FileItem& panel) { panel.setSelection(false); });
				m_fileItems[*mouseOverIndex].setSelection(true);

				m_forcusedItemIndex = *mouseOverIndex;
				m_selectionAhchorIndex = m_forcusedItemIndex;
			}

			m_mouseLDownPos = none;
			m_isDrag = false;
		}
		else if (m_region.leftClicked()) // 左クリック
		{
			if (const auto& mouseOverIndex = getHoverItemIndex(Cursor::PosF()))
			{
				// ドラッグ判定用にカーソル位置を記録
				m_mouseLDownPos = Cursor::PosF();

				if (KeyShift.pressed() && KeyControl.pressed()) // Shift + Ctrl + 左クリック
				{
					// 基準アイテムからクリックされたアイテムまでを選択に追加、範囲外はそのまま

					const size_t selectStart = Min(*mouseOverIndex, *m_selectionAhchorIndex);
					const size_t selectEnd = Max(*mouseOverIndex, *m_selectionAhchorIndex);

					for (size_t i = selectStart; i < selectEnd && i < m_fileItems.size(); ++i)
					{
						m_fileItems[i].setSelection(true);
					}
				}
				else if (KeyShift.pressed()) // Shift + 左クリック
				{
					if (m_selectionAhchorIndex) // 基準アイテムがある場合
					{
						// 基準アイテムからクリックされたアイテムまでを選択、範囲外は選択解除

						const size_t selectStart = Min(*mouseOverIndex, *m_selectionAhchorIndex);
						const size_t selectEnd = Max(*mouseOverIndex, *m_selectionAhchorIndex);

						m_fileItems.each_index([&selectStart, &selectEnd](const size_t i, FileItem& item)
						{
							const bool selection = InRange(i, selectStart, selectEnd);
							item.setSelection(selection);
						});
					}
					else // 基準点がない場合
					{
						// クリックされたアイテムだけを選択

						m_fileItems.each([](FileItem& item) { item.setSelection(false); });
						m_fileItems[*mouseOverIndex].setSelection(true);

						m_selectionAhchorIndex = *mouseOverIndex;
					}
				}
				else if (KeyControl.pressed()) // Ctrl + 左クリック
				{
					// クリックされたアイテムの選択を反転、基準アイテムを更新

					const bool isSelected = m_fileItems[*mouseOverIndex].isSelected();
					m_fileItems[*mouseOverIndex].setSelection(not isSelected);

					m_selectionAhchorIndex = *mouseOverIndex;
				}
				else // 左クリック
				{
					if (not m_fileItems[*mouseOverIndex].isSelected())
					{
						// クリックされたアイテムだけを選択、基準アイテムを更新

						m_fileItems.each([](FileItem& item) { item.setSelection(false); });
						m_fileItems[*mouseOverIndex].setSelection(true);
					}

					m_selectionAhchorIndex = *mouseOverIndex;
				}

				m_forcusedItemIndex = *mouseOverIndex;
			}
			else
			{
				m_mouseLDownPos = none;

				m_fileItems.each([](FileItem& item) { item.setSelection(false); });
			}
		}
	}

	// ドラッグ
	if (m_mouseLDownPos && not m_isDrag)
	{
		const double dragLength = Vec2{ *m_mouseLDownPos - Cursor::PosF() }.length();

		// ドラッグ開始
		if (DragStartLength <= dragLength)
		{
			const Array<std::weak_ptr<CryData>> selectedDataList = getSelectedData();

			if (selectedDataList)
			{
				Array<FilePath> paths;

				for (const auto& selectedData : selectedDataList)
				{
					if (const auto data = selectedData.lock())
					{
						if (data->path)
						{
							paths << data->path;
						}
					}
				}

				if (paths)
				{
					Platform::Windows::DragDrop::MakeDragDrop(paths);
				}

			}

			m_isDrag = true;
		}
	}

	bool isPlayAudio = false;

	// 音声の再生操作
	if (m_forcusedItemIndex)
	{
		// クリック
		if (m_mouseLDownPos && MouseL.up() && not m_isDrag)
		{
			if (not (KeyShift | KeyControl).pressed())
			{
				m_fileItems.each([](FileItem& item) { item.setSelection(false); });
				m_fileItems[*m_forcusedItemIndex].setSelection(true);
			}

			isPlayAudio = true;
		}

		// キー
		if (not isPlayAudio && (KeyEnter | KeySpace | KeyD).down())
		{
			isPlayAudio = true;
		}
	}

	if (MouseL.up())
	{
		m_mouseLDownPos = none;
		m_isDrag = false;
	}

	if (m_region.mouseOver())
	{
		Mouse::ClearLRInput();
	}

	return isPlayAudio;
}


void FilePanel::draw() const
{
	m_region.draw(Color{ 128 });

	const size_t startIndex = m_scrollLength / FileItem::Height;

	for (size_t i = startIndex; i < m_fileItems.size(); ++i)
	{
		if (m_region.bottomY() < m_fileItems[i].region().topY())
		{
			break;
		}

		m_fileItems[i].draw();
	}

	m_scrollbar.draw();
}


void FilePanel::filter(const Array<std::shared_ptr<CryData>>& cries, const Array<TagData>& andFilters, const Array<TagData>& orFilters)
{
	m_fileItems.clear();

	for (const auto& cry : cries)
	{
		// AND 検索
		if(andFilters)
		{
			bool match = true;

			for (const auto& andFilter : andFilters)
			{
				if (not isMatch(cry, andFilter))
				{
					match = false;
					break;
				}
			}

			if (not match)
			{
				continue;
			}
		}

		// OR 検索
		if (orFilters)
		{
			bool match = false;

			for (const auto& orFilter : orFilters)
			{
				if (isMatch(cry, orFilter))
				{
					match = true;
					break;
				}
			}

			if (not match)
			{
				continue;
			}
		}

		m_fileItems << FileItem{ cry };
	}

	// 初期化
	{
		m_scrollLength = 0.0;

		m_forcusedItemIndex = none;
		m_selectionAhchorIndex = none;

		m_mouseLDownPos = none;
		m_isDrag = false;

		m_longPressCountUp = MinLongPressCount;
		m_longPressCountDown = MinLongPressCount;
		m_longPressCountW = MinLongPressCount;
		m_longPressCountS = MinLongPressCount;
	}
}

bool FilePanel::isMatch(const std::weak_ptr<CryData>& cryPtr, const TagData& filter) const
{
	if (not filter.category || not filter.value)
	{
		return false;
	}

	const auto cry = cryPtr.lock();
	if (not cry)
	{
		return false;
	}

	const auto& key = cry->key;

	if (NakigoeDB::Tag::IsNumberTag(filter.category)) // ナンバー
	{
		if ((key.number == 0) || (filter.value == U"0") || (Format(key.number) == filter.value))
		{
			return true;
		}
	}
	else if (filter.category == U"SubID") // サブID
	{
		if ((key.subID == 0) || (filter.value == U"0") || (Format(key.subID) == filter.value))
		{
			return true;
		}
	}
	else if (NakigoeDB::Tag::IsCryVersionTag(filter.category)) // 鳴き声のバージョン
	{
		if ((key.cryVer == 0) || (filter.value == U"0") || (Format(key.cryVer) == filter.value))
		{
			return true;
		}
	}
	else if (filter.category == U"Name") // 名前
	{
		if (cry->name.value.contains(filter.value))
		{
			return true;
		}
	}
	else if (filter.category == U"SubName") // フォルム名
	{
		if (cry->subName.value.contains(filter.value))
		{
			return true;
		}
	}
	else if (NakigoeDB::Tag::IsGenerationTag(filter.category)) // 世代
	{
		if (cry->gen.value == filter.value)
		{
			return true;
		}
	}
	else if (filter.category == U"Type") // タイプ
	{
		if ((cry->type1.value == filter.value) || (cry->type2.value == filter.value))
		{
			return true;
		}
	}
	else if (filter.category == U"Type1") // タイプ 1
	{
		if (cry->type1.value == filter.value)
		{
			return true;
		}
	}
	else if (filter.category == U"Type2") // タイプ 2
	{
		if (cry->type2.value == filter.value)
		{
			return true;
		}
	}
	else if (filter.category == U"Ability") // 特性
	{
		if (cry->ability1.value.contains(filter.value) || cry->ability2.value.contains(filter.value) || cry->hiddenAbility.value.contains(filter.value))
		{
			return true;
		}
	}
	else if (filter.category == U"Ability1") // 特性 1
	{
		if (cry->ability1.value.contains(filter.value))
		{
			return true;
		}
	}
	else if (filter.category == U"Ability2") // 特性 2
	{
		if (cry->ability2.value.contains(filter.value))
		{
			return true;
		}
	}
	else if (NakigoeDB::Tag::IsHiddenAbilityTag(filter.category)) // 夢特性
	{
		if (cry->hiddenAbility.value.contains(filter.value))
		{
			return true;
		}
	}
	else
	{
		if (cry->userTags.contains(filter))
		{
			return true;
		}
	}

	return false;
}


Optional<std::weak_ptr<CryData>> FilePanel::getForcusedData() const
{
	if (m_forcusedItemIndex && *m_forcusedItemIndex < m_fileItems.size())
	{
		return m_fileItems[*m_forcusedItemIndex].getData();
	}
	else
	{
		return none;
	}
}

Array<size_t> FilePanel::getSelectedIndex() const
{
	Array<size_t> selectedIndex;

	for (size_t i = 0; i < m_fileItems.size(); ++i)
	{
		if (m_fileItems[i].isSelected())
		{
			selectedIndex << i;
		}
	}

	return selectedIndex;
}

Array<std::weak_ptr<CryData>> FilePanel::getSelectedData() const
{
	Array<std::weak_ptr<CryData>> selectedData;

	for (const auto& item : m_fileItems)
	{
		if (item.isSelected())
		{
			selectedData << item.getData();
		}
	}

	return selectedData;
}

Array<std::weak_ptr<CryData>> FilePanel::getDraggingData() const
{
	if (not m_isDrag)
	{
		return {};
	}

	return getSelectedData();
}


Optional<std::weak_ptr<CryData>> FilePanel::rightClickedData() const
{
	if (m_rightClickedIndex && *m_rightClickedIndex < m_fileItems.size())
	{
		return m_fileItems[*m_rightClickedIndex].getData();
	}
	else
	{
		return none;
	}
}


bool FilePanel::isStartDrag() const
{
	return not m_prevDrag && m_isDrag;
}

bool FilePanel::isFinishDrag() const
{
	return m_prevDrag && not m_isDrag;
}


Optional<size_t> FilePanel::getHoverItemIndex(const Vec2& cursorPos) const
{
	const RectF itemRegion = m_region.stretched(0.0, -m_scrollbar.barArea().w, 0.0, 0.0);

	if (not Scene::Rect().intersects(cursorPos) || not itemRegion.intersects(cursorPos))
	{
		return none;
	}

	const size_t mouseOverIndex = (cursorPos.y - (m_region.topY() - m_scrollLength)) / FileItem::Height;

	if (mouseOverIndex < m_fileItems.size())
	{
		return mouseOverIndex;
	}
	else
	{
		return none;
	}
}

String FilePanel::getMouseOverText() const
{
	if (const auto& mouseOverIndex = getHoverItemIndex(Cursor::PosF()))
	{
		return m_fileItems[*mouseOverIndex].getMouseOverText();
	}
	else
	{
		return {};
	}
}


void FilePanel::scrollToSelectionItem()
{
	const Array<size_t> indexList = getSelectedIndex();

	if (not indexList)
	{
		return;
	}

	if (m_fileItems[indexList.front()].region().topY() < m_region.topY())
	{
		m_scrollLength -= m_region.topY() - m_fileItems[indexList.front()].region().topY();
	}
	else if (m_region.bottomY() < m_fileItems[indexList.back()].region().bottomY())
	{
		m_scrollLength += m_fileItems[indexList.back()].region().bottomY() - m_region.bottomY();
	}
}
