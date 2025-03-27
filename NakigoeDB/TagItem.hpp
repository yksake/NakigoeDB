#pragma once
#include "TagData.hpp"

class TagItem
{
public:
	static constexpr double Height = 30.0;

	TagItem() = default;
	TagItem(const TagData& tagData);
	virtual ~TagItem() = default;

	/// @brief 
	/// @param leftX 
	/// @param rightX 
	/// @param mouseOverEnabled 
	/// @return 選択状態が更新されたら true
	virtual bool update(const double leftX, const double rightX, const bool mouseOverEnabled = true) = 0;
	virtual void draw(const int8 nestLevel = 0) const = 0;

	RectF region() const;
	/// @brief 領域の上辺の Y 座標を設定します。
	/// @param y 設定する Y 座標
	/// @return 設定後の領域の下辺の Y 座標
	virtual double setTopY(const double y);
	/// @brief 領域を Y 軸方向に移動します。
	/// @param length 移動量
	/// @return 移動後の領域の下辺の Y 座標
	virtual double moveY(const double length);

	virtual bool isSelected() const = 0;
	virtual void setSelection(const bool isSelected) = 0;
	virtual Array<TagData> getSelectedTags() const = 0;
	virtual Optional<TagData> getMouseOverTag() const = 0;

	virtual Array<std::pair<String, bool>> getCollapseState(const String& parentFolder) const = 0;

protected:
	Vec2 m_pos = Vec2::Zero();
	double m_width = 0.0;

	TagData m_tag;

	bool m_isMouseOver = false;
};
