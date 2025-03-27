#include "TagItem.hpp"

TagItem::TagItem(const TagData& tagData)
{
	m_tag = tagData;
}


RectF TagItem::region() const
{
	return RectF{ m_pos, m_width, Height };
}

double TagItem::setTopY(const double y)
{
	m_pos.y = y;

	return region().bottomY();
}

double TagItem::moveY(const double length)
{
	m_pos.y += length;

	return region().bottomY();
}
