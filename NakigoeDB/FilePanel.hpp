#pragma once
#include "FileItem.hpp"
#include "Scrollbar.hpp"
#include "TagData.hpp"

class FilePanel
{
public:
	FilePanel() = default;

	/// @brief 
	/// @param region 
	/// @param enabled 
	/// @return 再生操作があったら true
	bool update(const RectF& region, const bool enabled = true);
	void draw() const;

	void filter(const Array<std::shared_ptr<CryData>>& cries, const Array<TagData>& andFilters, const Array<TagData>& orFilters = {});

	Optional<std::weak_ptr<CryData>> getForcusedData() const;
	Array<std::weak_ptr<CryData>> getSelectedData() const;
	Array<std::weak_ptr<CryData>> getDraggingData() const;

	Optional<std::weak_ptr<CryData>> rightClickedData() const;

	bool isStartDrag() const;
	bool isFinishDrag() const;

	String getMouseOverText() const;

private:
	RectF m_region = { Vec2::Zero(), SizeF::Zero() };

	Array<FileItem> m_fileItems;

	Scrollbar m_scrollbar;
	double m_scrollLength = 0.0;

	// 最後に選択されたアイテムのインデックス
	Optional<size_t> m_forcusedItemIndex;
	// 複数選択の基準点のアイテムのインデックス
	Optional<size_t> m_selectionAhchorIndex;

	Optional<Vec2> m_mouseLDownPos;
	static constexpr double DragStartLength = 10.0;
	bool m_prevDrag = false;
	bool m_isDrag = false;

	static constexpr double LongPressInterval = 0.1;
	static constexpr size_t MinLongPressCount = 2;
	size_t m_longPressCountUp = MinLongPressCount;
	size_t m_longPressCountDown = MinLongPressCount;
	size_t m_longPressCountW = MinLongPressCount;
	size_t m_longPressCountS = MinLongPressCount;

	Optional<size_t> m_rightClickedIndex;

	void updateScrollbar(const bool enabled);
	bool updateItems(const bool enabled);

	bool isMatch(const std::weak_ptr<CryData>& cryPtr, const TagData& filter) const;

	Array<size_t> getSelectedIndex() const;

	Optional<size_t> getHoverItemIndex(const Vec2& cursorPos) const;

	void scrollToSelectionItem();
};
