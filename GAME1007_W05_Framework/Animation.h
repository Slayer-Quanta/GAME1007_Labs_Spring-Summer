#pragma once
#include "Core.h"
#include <vector>
#include "Timer.h"

class Animation
{
public:
	Texture* texture = nullptr;
	std::vector<SDL_Rect> frames{};
	SDL_Point offset{};

	float Duration() { return (float)frames.size() * frameTimer.duration; }
	void SetDuration(float time) { frameTimer.duration = time / (float)frames.size(); }

	void Update(float dt)
	{
		if (frameTimer.Expired())
		{
			frameTimer.Reset();
			++frameNumber %= frames.size();
		}
		frameTimer.Tick(dt);
	}

	void Render(const Rect& dst, float degrees = 0.0f)
	{
		SDL_Rect frame = frames[frameNumber];
		frame.x += offset.x;
		frame.y += offset.y;
		DrawTexture(texture, frame, dst, degrees);
	}

	void Reset()
	{
		frameNumber = 0;
		frameTimer.Reset();
	}

private:
	size_t frameNumber = 0;
	Timer frameTimer{};
};