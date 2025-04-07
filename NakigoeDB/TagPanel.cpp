#include "TagPanel.hpp"
#include "TagPanelLoader.hpp"
#include "TagLoader.hpp"
#include "Array.hpp"

void TagPanel::load(const Array<TagData>& basicTags, const Array<TagData>& userTags)
{
	m_rootItem = ParentTagItem{};

	for (auto&& panel : TagPanelLoader::Load(basicTags, userTags))
	{
		m_rootItem.addChild(std::move(panel));
	}
}


bool TagPanel::update(const RectF& region, const bool enabled)
{
	m_region = region;

	bool isUpdated = false;

	// ヘッダ
	{
		const double buttonWidth = 100.0;
		const double buttonRightMargin = 2.0;
		Vec2 buttonPos = { headerRegion().rightX() - buttonWidth - buttonRightMargin, headerRegion().centerY() - (Button::Height * 0.5) };

		// 選択解除ボタン
		if (m_resetButton.update(buttonPos, buttonWidth, enabled))
		{
			m_rootItem.setSelection(false);
		}

		if (headerRegion().mouseOver())
		{
			Mouse::ClearLRInput();
		}
	}

	Optional<double> scrollLength;

	// マウスホイールでスクロール
	if (itemRegion().mouseOver() && m_scrollbar.enabled() && enabled)
	{
		scrollLength = TagItem::Height * 3.0 * Mouse::Wheel();
	}

	double tagH = 0.0;

	// スクロールバーを更新
	{
		double tagTopY = m_rootItem.region().topY() + TagItem::Height;
		const double tagBottomY = m_rootItem.moveY(0.0);
		tagH = tagBottomY - tagTopY;

		if (scrollLength)
		{
			tagTopY -= *scrollLength;
		}

		m_scrollbar.update(itemRegion(), tagTopY, tagH);
		m_scrollLength = m_scrollbar.scrollLength();
	}

	// アイテムを更新
	{
		double topY = itemRegion().topY() - m_scrollLength - TagItem::Height;

		Vec2 mouseTranslation = Vec2::Zero();

		// マウスホイールでスクロールしたときはマウスの座標変換をする
		if (itemRegion().mouseOver() && m_scrollbar.barArea().mouseOver() && scrollLength)
		{
			mouseTranslation.y = topY - m_rootItem.region().topY();
		}

		const Transformer2D t{ Mat3x2::Identity(), Mat3x2::Translate(mouseTranslation) };

		// パネルを更新
		{
			const bool itemEnabled = enabled && itemRegion().mouseOver() && not m_scrollbar.barArea().mouseOver();

			m_rootItem.update(itemRegion().leftX(), m_scrollbar.barArea().leftX(), itemEnabled);
			m_rootItem.setTopY(topY);

			// タグの選択状態を更新
			{
				const Array<TagData> prevSelectedTags = m_selectedTags;
				m_selectedTags = m_rootItem.getSelectedTags();

				bool isUpdatedSelection = m_selectedTags.size() != prevSelectedTags.size();

				if (isUpdatedSelection)
				{
					isUpdated = true;
				}
				else
				{
					if (not NakigoeDB::isEqual(m_selectedTags, prevSelectedTags))
					{
						isUpdated = true;
					}
				}
			}

			// 折りたたみ状態を更新
			{
				const double newTagH = m_rootItem.moveY(0.0) - (m_rootItem.region().topY() + TagItem::Height);

				if (newTagH != tagH)
				{
					TagPanelLoader::CollapseState::Save(m_rootItem.getCollapseState(U""));
				}
			}
		}

		// パネルが閉じられて短くなったらスクロールを無効化
		if (m_rootItem.moveY(0.0) < itemRegion().bottomY())
		{
			const double tagTopY = m_rootItem.region().topY() + TagItem::Height;
			const double tagBottomY = m_rootItem.moveY(0.0);
			const double newTagH = tagBottomY - tagTopY;

			m_scrollbar.update(itemRegion(), tagTopY, newTagH);
			m_scrollLength = m_scrollbar.scrollLength();

			topY = itemRegion().topY() - m_scrollLength - TagItem::Height;

			m_rootItem.setTopY(topY);
		}

		if (itemRegion().mouseOver())
		{
			Mouse::ClearLRInput();
		}
	}

	return isUpdated;
}

void TagPanel::draw(const bool mouseOverEnabled) const
{
	m_region.draw(Color{ 128 });

	{
		Graphics2D::SetScissorRect(m_region.asRect());
		RasterizerState rs = RasterizerState::Default2D;
		rs.scissorEnable = true;
		const ScopedRenderStates2D state{ rs };

		m_rootItem.draw(-1);
	}

	m_scrollbar.draw();

	headerRegion().draw(Color{ 50 });

	m_resetButton.draw(mouseOverEnabled);
}


RectF TagPanel::region() const
{
	return m_region;
}


Array<TagData> TagPanel::getSelectedTags() const
{
	return m_rootItem.getSelectedTags();
}


bool TagPanel::dropItems(Array<std::weak_ptr<CryData>> droppedItems)
{
	if (not droppedItems)
	{
		return false;
	}

	const auto mouseOverTag = m_rootItem.getMouseOverTag();

	if (not mouseOverTag)
	{
		return false;
	}
	if (not mouseOverTag->category || not mouseOverTag->value)
	{
		return false;
	}
	if (NakigoeDB::Tag::IsReserved(mouseOverTag->category))
	{
		return false;
	}

	Array<CryDataKey> taggedCryKeys = TagLoader::LoadUserTagContents(*mouseOverTag);

	bool isAdded = false;

	for (const auto& droppedItem : droppedItems)
	{
		if (auto item = droppedItem.lock())
		{
			if (not taggedCryKeys.contains(item->key))
			{
				item->userTags.insert(*mouseOverTag);
				taggedCryKeys << item->key;
				isAdded = true;
			}
		}
	}

	if (isAdded)
	{
		taggedCryKeys.sort();
	}
	else
	{
		for (const auto& droppedItem : droppedItems)
		{
			if (auto item = droppedItem.lock())
			{
				item->userTags.erase(*mouseOverTag);
				taggedCryKeys.remove(item->key);
			}
		}
	}

	return TagLoader::SaveUserTagContents(*mouseOverTag, taggedCryKeys);
}


RectF TagPanel::headerRegion() const
{
	const double h = Min(m_region.h, HeaderHeight);

	return RectF{ m_region.pos, m_region.w, h };
}

RectF TagPanel::itemRegion() const
{
	const double y = m_region.y + HeaderHeight;
	const double h = Max(0.0, m_region.h - HeaderHeight);

	return RectF{ m_region.x, y, m_region.w, h };
}
