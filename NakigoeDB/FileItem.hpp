#pragma once
#include "CryData.hpp"

class FileItem
{
public:
	static constexpr double Height = 35.0;

	FileItem() = default;
	FileItem(const std::weak_ptr<CryData>& data);

	void update(const double leftX, const double rightX);
	void draw() const;

	RectF region() const;
	/// @brief 領域の上辺の Y 座標を設定します。
	/// @param y 設定する Y 座標
	/// @return 設定後の領域の下辺の Y 座標
	double setTopY(const double y);
	/// @brief 領域を Y 軸方向に移動します。
	/// @param translation 移動量
	/// @return 移動後の領域の下辺の Y 座標
	double moveY(const double length);

	bool isSelected() const;
	void setSelection(const bool isSelected);

	std::weak_ptr<CryData> getData() const;

	String getMouseOverText() const;

private:
	Vec2 m_pos = Vec2::Zero();
	double m_width = 0.0;
	std::weak_ptr<CryData> m_data;

	bool m_isSelected = false;
};
