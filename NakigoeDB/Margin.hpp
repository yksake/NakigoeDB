#pragma once

///@brief 周りのUIとの間隔を表すクラス
struct Margin
{
	double top = 0.0;
	double right = 0.0;
	double bottom = 0.0;
	double left = 0.0;

	constexpr Margin()noexcept = default;
	constexpr Margin(double space)noexcept;
	constexpr Margin(double horizontal, double vertical)noexcept;
	constexpr Margin(double top, double right, double bottom, double left)noexcept;

	/// @brief スペースの合計を返す
	/// @return { 水平方向の合計 , 垂直方向の合計 }
	constexpr SizeF getSize()const noexcept;

	/// @brief 垂直方向の合計を計算
	/// @return top + bottom
	constexpr double getVertical()const noexcept;

	/// @brief 水平方向の合計を計算
	/// @return right + left
	constexpr double getHorizontal()const noexcept;
};

#include "Margin.ipp"
