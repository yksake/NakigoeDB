#pragma once
#include "CryData.hpp"

class AudioPlayer
{
public:
	static constexpr double Height = 100.0;

	AudioPlayer() = default;
	AudioPlayer(const Vec2& pos);

	void update(const double width);
	void draw() const;

	void play(const std::weak_ptr<CryData>& data);
	void play();

	RectF region() const;

private:
	std::weak_ptr<CryData> m_data;
	Audio m_audio;

	Vec2 m_pos = Vec2::Zero();
	double m_width = 0.0;
};
