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

	const double textSize = 17;
	const Color textColor = Color{ 0 };

	const Margin margin{ 5.0, 10.0, 10.0, 10.0 };
	const double rectH = 25.0;
	RectF textRect = { m_pos.x + margin.left, m_pos.y + margin.top, m_width - margin.getHorizontal(), rectH };

	// ナンバー、名前、新旧
	{
		FontAsset(U"Font")(m_title).draw(textSize, textRect, textColor);

		textRect.moveBy(0.0, rectH);
	}

	// フォルム名
	FontAsset(U"Font")(m_subTitle).draw(textSize, textRect, textColor);

	// パス
	if (m_path)
	{
		const FilePath path = FileSystem::FileName(m_path);

		textRect.setPos(textRect.pos.x, m_pos.y + Height - rectH - margin.bottom);

		FontAsset(U"Font")(path).draw(textSize, textRect, textColor);
	}
}


void AudioPlayer::play(const std::weak_ptr<CryData>& data)
{
	if (const auto cry = data.lock())
	{
		// タイトル
		{
			m_title = U"{:0>4}"_fmt(cry->key.number) + U".";

			if (cry->name.customName)
			{
				m_title += U" " + cry->name.customName;
			}
			else
			{
				m_title += U" " + cry->name.value;
			}

			if (cry->key.cryVer == 1)
			{
				m_title += U" (旧)";
			}
			else if (cry->key.cryVer == 2)
			{
				m_title += U" (新)";
			}
		}

		// サブタイトル
		{
			m_subTitle.clear();

			if (cry->subName.customName)
			{
				m_subTitle += cry->subName.customName;
			}
			else if (cry->subName.value)
			{
				m_subTitle += cry->subName.value;
			}
		}

		// パス
		if (m_path != cry->path)
		{
			m_path = cry->path;

			m_audio = Audio{ m_path };
		}
	}
	else
	{
		m_title.clear();
		m_subTitle.clear();
		m_path.clear();
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
