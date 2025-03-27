#include "ChildTagItem.hpp"
#include "Margin.hpp"

ChildTagItem::ChildTagItem(const TagData& tagData)
	: TagItem(tagData)
{

}


bool ChildTagItem::update(const double leftX, const double rightX, const bool mouseOverEnabled)
{
	m_pos.x = leftX;
	m_width = rightX - leftX;

	m_isMouseOver = region().mouseOver() && mouseOverEnabled;

	bool isUpdated = false;

	if (Cursor::OnClientRect() && m_isMouseOver && (region().leftClicked() || region().rightClicked()))
	{
		m_isSelected = true;
		isUpdated = true;
	}

	if (m_isMouseOver)
	{
		Mouse::ClearLRInput();
	}

	return isUpdated;
}

void ChildTagItem::draw(const int8 nestLevel) const
{
	String fontName = U"Font";
	Color backgroundColor = Color{ 238 };
	Color textColor = Color{ 0 };

	if (m_isSelected)
	{
		fontName = U"FontMedium";
		backgroundColor = Color{ 70, 100, 200 };
		textColor = Color{ 255 };
	}
	else if (m_isMouseOver)
	{
		backgroundColor = Color{ 200 };
	}

	region().draw(backgroundColor);

	String text;
	if (m_tag.customName)
	{
		text += m_tag.customName;
	}
	else
	{
		text += m_tag.value;
	}

	Margin margin{ 0.0, 5.0, 0.0, 5.0 + (30.0 * nestLevel) };
	FontAsset(fontName)(text).draw(region().stretched(-margin.top, -margin.right, -margin.bottom, -margin.left), textColor);
}


bool ChildTagItem::isSelected() const
{
	return m_isSelected;
}

void ChildTagItem::setSelection(const bool isSelected)
{
	m_isSelected = isSelected;
}

Array<TagData> ChildTagItem::getSelectedTags() const
{
	if (m_isSelected)
	{
		return { m_tag };
	}
	else
	{
		return {};
	}
}

Optional<TagData> ChildTagItem::getMouseOverTag() const
{
	if (m_isMouseOver)
	{
		return m_tag;
	}
	else
	{
		return none;
	}
}


Array<std::pair<String, bool>> ChildTagItem::getCollapseState(const String&) const
{
	return {};
}
