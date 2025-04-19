#include "FileItem.hpp"
#include "Margin.hpp"
#include "Array.hpp"

FileItem::FileItem(const std::weak_ptr<CryData>& data)
{
	m_data = data;
}


void FileItem::update(const double leftX, const double rightX)
{
	m_pos.x = leftX;
	m_width = rightX - leftX;
}

void FileItem::draw() const
{
	String fontName = U"Font";
	Color backgroundColor = Color{ 255 };
	Color textColor = Color{ 0 };

	if (m_isSelected)
	{
		fontName = U"FontMedium";
		backgroundColor = Color{ 70, 100, 200 };
		textColor = Color{ 255 };
	}

	region().draw(backgroundColor);

	String text;

	if (const auto data = m_data.lock())
	{
		const auto& key = data->key;

		text = U"{:0>4}"_fmt(key.number) + U':';

		// 名前
		if (data->name.customName)
		{
			text += data->name.customName;
		}
		else
		{
			text += data->name.value;
		}

		// フォルム名
		if (data->subName.customName)
		{
			text += U' ' + data->subName.customName;
		}
		else
		{
			text += U' ' + data->subName.value;
		}

		// 鳴き声のバージョン
		if (key.cryVer == 1)
		{
			text += U" (旧)";
		}
		else if (key.cryVer == 2)
		{
			text += U" (新)";
		}

		// 警告
		if (not data->path)
		{
			text += U" ⚠️";
		}

		// ユーザータグ
		for (const auto& tag : data->userTags)
		{
			String tagText;

			if (tag.customName)
			{
				tagText = tag.customName;
			}
			else
			{
				tagText = tag.category + U'.' + tag.value;
			}

			text += U" [" + tagText + U']';
		}
	}
	else
	{
		text = U"----: -----";
	}

	Margin margin{ 2.0, 10.0, 0.0, 10.0 };
	RectF textRegion = region().stretched(-margin.top, -margin.right, -margin.bottom, -margin.left);

	FontAsset(fontName)(text).draw(textRegion, textColor);
}


RectF FileItem::region() const
{
	return RectF{ m_pos, m_width, Height };
}

double FileItem::setTopY(const double y)
{
	m_pos.y = y;

	return region().bottomY();
}

double FileItem::moveY(const double length)
{
	m_pos.y += length;

	return region().bottomY();
}


bool FileItem::isSelected() const
{
	return m_isSelected;
}

void FileItem::setSelection(const bool isSelected)
{
	m_isSelected = isSelected;
}


std::weak_ptr<CryData> FileItem::getData() const
{
	return m_data;
}


String FileItem::getMouseOverText() const
{
	if (const auto data = m_data.lock())
	{
		if (not data->path)
		{
			String text = U"⚠️ ファイルパスが未設定です。\n" + Format(data->key.number) + U": ";

			if (data->name.customName)
			{
				text += U' ' + data->name.customName;
			}
			else if (data->name.value)
			{
				text += U' ' + data->name.value;
			}

			if (data->subName.customName)
			{
				text += U' ' + data->subName.customName;
			}
			else if (data->subName.value)
			{
				text += U' ' + data->subName.value;
			}

			return text;
		}
		else if (not FileSystem::IsFile(data->path))
		{
			return U"⚠️ ファイルが存在しません。\n" + data->path;
		}
	}
	else
	{
		return U"⚠️ データへのアクセスに失敗しました。";
	}

	return {};
}
