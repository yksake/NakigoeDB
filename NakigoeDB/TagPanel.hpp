#pragma once
#include "ParentTagItem.hpp"
#include "CryData.hpp"
#include "Scrollbar.hpp"
#include "Button.hpp"

class TagPanel
{
public:
	TagPanel() = default;

	void load(const Array<TagData>& basicTags, const Array<TagData>& userTags);

	bool update(const RectF& region, const bool enabled = true);
	void draw() const;

	RectF region() const;

	Array<TagData> getSelectedTags() const;

	bool dropItems(Array<std::weak_ptr<CryData>> droppedItems);

private:
	static constexpr double HeaderHeight = 40.0;
	RectF m_region = { Vec2::Zero(), SizeF::Zero() };

	ParentTagItem m_rootItem;
	Array<TagData> m_selectedTags;

	Scrollbar m_scrollbar;
	double m_scrollLength = 0.0;

	Button m_resetButton{ U"選択解除" };

	RectF headerRegion() const;
	RectF itemRegion() const;
};
