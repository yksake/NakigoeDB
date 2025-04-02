#pragma once
#include "TagItem.hpp"

class ChildTagItem : public TagItem
{
public:
	ChildTagItem() = default;
	ChildTagItem(const TagData& tagData);
	~ChildTagItem() = default;

	/// @brief タグアイテムを更新します。
	/// @param leftX 
	/// @param rightX 
	/// @param mouseOverEnabled 
	/// @return 他のタグアイテムの選択状態を解除する場合 true
	bool update(const double leftX, const double rightX, const bool mouseOverEnabled = true) override;
	void draw(const int8 nestLevel = 0) const override;

	bool isSelected() const override;
	void setSelection(const bool isSelected) override;
	Array<TagData> getSelectedTags() const override;
	Optional<TagData> getMouseOverTag() const override;

	Array<std::pair<String, bool>> getCollapseState(const String&) const override;

private:
	bool m_isSelected = false;
};
