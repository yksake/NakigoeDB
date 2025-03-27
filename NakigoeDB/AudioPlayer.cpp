#include "AudioPlayer.hpp"
#include "Margin.hpp"

AudioPlayer::AudioPlayer(const Vec2& pos)
{
	m_pos = pos;
}


void AudioPlayer::update(const double width)
{
	m_width = width;

	if (region().mouseOver())
	{
		Mouse::ClearLRInput();
	}
}

void AudioPlayer::draw() const
{
	region().draw(Color{ 255 });

	if (const auto data = m_data.lock())
	{
		const double textSize = 17;
		const Color textColor = Color{ 0 };

		const Margin margin{ 5.0, 10.0, 10.0, 10.0 };
		const double rectH = 25.0;
		RectF textRect = { m_pos.x + margin.left, m_pos.y + margin.top, m_width - margin.getHorizontal(), rectH };

		// ナンバー、名前、新旧
		{
			String text = U"{:0>4}"_fmt(data->key.number) + U".";

			if (data->name.customName)
			{
				text += U" " + data->name.customName;
			}
			else
			{
				text += U" " + data->name.value;
			}

			if (data->key.cryVer == 1)
			{
				text += U" (旧)";
			}
			else if (data->key.cryVer == 2)
			{
				text += U" (新)";
			}

			FontAsset(U"Font")(text).draw(textSize, textRect, textColor);

			textRect.moveBy(0.0, rectH);
		}

		// フォルム名
		{
			String text;

			if (data->subName.customName)
			{
				text += data->subName.customName;
			}
			else if (data->subName.value)
			{
				text += data->subName.value;
			}

			FontAsset(U"Font")(text).draw(textSize, textRect, textColor);
		}

		// パス
		if (data->path)
		{
			const FilePath path = FileSystem::FileName(data->path);

			textRect.setPos(textRect.pos.x, m_pos.y + Height - rectH - margin.bottom);

			FontAsset(U"Font")(path).draw(textSize, textRect, textColor);
		}
	}
}


void AudioPlayer::play(const std::weak_ptr<CryData>& data)
{
	m_data = data;

	if (const auto cry = m_data.lock())
	{
		m_audio = Audio{ cry->path };
	}

	play();
}

void AudioPlayer::play()
{
	if (m_audio)
	{
		m_audio.stop();
		m_audio.play();
	}
}


RectF AudioPlayer::region() const
{
	return RectF{ m_pos, m_width, Height };
}
