#pragma once
#include "TagItem.hpp"

class ParentTagItem : public TagItem
{
public:
	ParentTagItem() = default;
	ParentTagItem(const TagData& tagData);
	ParentTagItem(const TagData& tagData, Array<std::unique_ptr<TagItem>>& children);
	~ParentTagItem() = default;

	bool update(const double leftX, const double rightX, const bool mouseOverEnabled = true) override;
	void draw(const int8 nestLevel = 0) const override;

	double setTopY(const double y) override;
	double moveY(const double length) override;

	bool isSelected() const override;
	void setSelection(const bool isSelect) override;
	Array<TagData> getSelectedTags() const override;
	Optional<TagData> getMouseOverTag() const override;

	Array<std::pair<String, bool>> getCollapseState(const String& parentFolder) const override;

	void open();
	void close();

	void addChild(std::unique_ptr<TagItem> child);

	ParentTagItem& operator=(ParentTagItem&& ptp)
	{
		this->m_pos = ptp.m_pos;
		this->m_width = ptp.m_width;
		this->m_tag = ptp.m_tag;
		this->m_isMouseOver = ptp.m_isMouseOver;

		this->m_isOpend = ptp.m_isOpend;

		this->m_children.clear();
		for (auto&& child : ptp.m_children)
		{
			this->m_children << std::move(child);
		}

		return *this;
	}

private:
	bool m_isOpend = true;

	Array<std::unique_ptr<TagItem>> m_children;
};
