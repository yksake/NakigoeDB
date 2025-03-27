#pragma once

constexpr Margin::Margin(double space)noexcept
	: Margin{ space, space }
{

}

constexpr Margin::Margin(double horizontal, double vertical)noexcept
	: Margin{ vertical, horizontal, vertical, horizontal }
{

}

constexpr Margin::Margin(double top, double right, double bottom, double left)noexcept
	: top{ top }, right{ right }, bottom{ bottom }, left{ left }
{

}

constexpr SizeF Margin::getSize()const noexcept
{
	return { getHorizontal(), getVertical() };
}

constexpr double Margin::getVertical()const noexcept
{
	return top + bottom;
}

constexpr double Margin::getHorizontal()const noexcept
{
	return right + left;
}
