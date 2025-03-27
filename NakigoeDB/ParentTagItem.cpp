#include "ParentTagItem.hpp"
#include "Margin.hpp"

ParentTagItem::ParentTagItem(const TagData& tagData)
	: TagItem(tagData)
{

}

ParentTagItem::ParentTagItem(const TagData& tagData, Array<std::unique_ptr<TagItem>>& children)
	: TagItem(tagData)
{
	for (auto&& child : children)
	{
		m_children << std::move(child);
	}
}


bool ParentTagItem::update(const double leftX, const double rightX, const bool mouseOverEnabled)
{
	m_pos.x = leftX;
	m_width = rightX - leftX;

	m_children.remove(nullptr);

	m_isMouseOver = region().mouseOver() && mouseOverEnabled;

	if (Cursor::OnClientRect() && m_isMouseOver && region().leftClicked())
	{
		m_isOpend = not m_isOpend;

		setTopY(m_pos.y);
	}

	if (m_isMouseOver)
	{
		Mouse::ClearLRInput();
	}

	bool isUpdated = false;

	if (m_isOpend)
	{
		const bool isLeftClicked = MouseL.down();
		Optional<size_t> updatedIndex;

		for (size_t i = 0; i < m_children.size(); ++i)
		{
			if (not m_children[i])
			{
				continue;
			}

			if (m_children[i]->update(leftX, rightX))
			{
				updatedIndex = i;
			}
		}

		if (updatedIndex)
		{
			for (size_t i = 0; i < m_children.size(); ++i)
			{
				if (not m_children[i])
				{
					continue;
				}

				if (i == *updatedIndex)
				{
					continue;
				}

				m_children[i]->setSelection(false);
			}
		}

		isUpdated = updatedIndex && isLeftClicked;
	}

	return isUpdated;
}

void ParentTagItem::draw(const int8 nestLevel) const
{
	Color backgroundColor = Color{ 220 };
	if (m_isMouseOver)
	{
		backgroundColor = Color{ 190 };
	}

	region().draw(backgroundColor);

	String text;

	if (m_isOpend)
	{
		text += U"▼ ";
	}
	else
	{
		text += U"▶ ";
	}

	if (m_tag.customName)
	{
		text += m_tag.customName;
	}
	else
	{
		text += m_tag.category;
	}

	Margin margin{ 0.0, 5.0, 0.0, 5.0 + (30.0 * nestLevel) };
	FontAsset(U"Font")(text).draw(region().stretched(-margin.top, -margin.right, -margin.bottom, -margin.left), Palette::Black);

	if (m_isOpend)
	{
		for (const auto& child : m_children)
		{
			if (not child)
			{
				continue;
			}

			child->draw(nestLevel + 1);
		}
	}
}


double ParentTagItem::setTopY(const double y)
{
	double bottomY = TagItem::setTopY(y);

	for (auto&& child : m_children)
	{
		if (not child)
		{
			continue;
		}

		bottomY = child->setTopY(bottomY);
	}

	if (m_isOpend)
	{
		return bottomY;
	}
	else
	{
		return region().bottomY();
	}
}

double ParentTagItem::moveY(const double length)
{
	double bottomY = TagItem::moveY(length);

	for (auto&& child : m_children)
	{
		if (not child)
		{
			continue;
		}

		bottomY = child->moveY(length);
	}

	if (m_isOpend)
	{
		return bottomY;
	}
	else
	{
		return region().bottomY();
	}
}


bool ParentTagItem::isSelected() const
{
	return false;
}

void ParentTagItem::setSelection(const bool isSelect)
{
	for (auto&& child : m_children)
	{
		if (not child)
		{
			continue;
		}

		child->setSelection(isSelect);
	}
}

Array<TagData> ParentTagItem::getSelectedTags() const
{
	Array<TagData> selectedTags;

	for (const auto& child : m_children)
	{
		if (not child)
		{
			continue;
		}

		selectedTags.append(child->getSelectedTags());
	}

	return selectedTags;
}

Optional<TagData> ParentTagItem::getMouseOverTag() const
{
	if (m_isOpend)
	{
		for (const auto& child : m_children)
		{
			if (not child)
			{
				continue;
			}

			if (const auto mouseOverTag = child->getMouseOverTag())
			{
				return mouseOverTag;
			}
		}
	}

	return none;
}


Array<std::pair<String, bool>> ParentTagItem::getCollapseState(const String& parentFolder) const
{
	Array<std::pair<String, bool>> collapseState;

	String collapseID = parentFolder;
	if (collapseID)
	{
		collapseID += U'.';
	}

	if (m_tag.category)
	{
		collapseID += m_tag.category;
	}
	if (m_tag.customName)
	{
		collapseID += U'(' + m_tag.customName + U')';
	}

	collapseState.push_back({ collapseID, m_isOpend });

	for (const auto& child : m_children)
	{
		if (not child)
		{
			continue;
		}

		const auto childCollapseState = child->getCollapseState(collapseID);

		if (childCollapseState)
		{
			collapseState.append(childCollapseState);
		}
	}

	return collapseState;
}


void ParentTagItem::open()
{
	m_isOpend = true;
}

void ParentTagItem::close()
{
	m_isOpend = false;
}


void ParentTagItem::addChild(std::unique_ptr<TagItem> child)
{
	if (not child)
	{
		return;
	}

	m_children << std::move(child);
}
