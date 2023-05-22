#include "Core.h"
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <cassert>
#include <array>

using namespace std;

struct Time
{
	double update = 0.0;		// Update frame delta
	double render = 0.0;		// Render frame delta
	double frame = 0.0;			// update + render
	double smooth = 0.0;		// Average of update + render across samplesCount frames
	double target = 1.0 / 60.0;	// Desired seconds per frame (frame rate, 60fps by default)

	array<double, 10> samples;	// History of frame times
	size_t frameCount = 0ULL;	// Frame counter

	double previous = 0.0;	// Previous time query
	double current = 0.0;	// Current time query
} gTime;

struct App
{
	bool running = false;
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	array<Uint8, SDL_NUM_SCANCODES> keyboard{};
} gApp;

void AppInit(int width, int height)
{
	assert(!gApp.running);
	assert(gApp.window == nullptr);
	assert(gApp.renderer == nullptr);

	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	gApp.window = SDL_CreateWindow("Fundamentals 2 Framework", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
	gApp.renderer = SDL_CreateRenderer(gApp.window, -1, 0);

	gTime.previous = TotalTime();
	gApp.running = true;
}

void AppExit()
{
	assert(gApp.window != nullptr);
	assert(gApp.renderer != nullptr);

	SDL_DestroyRenderer(gApp.renderer);
	SDL_DestroyWindow(gApp.window);
	IMG_Quit();
	SDL_Quit();

	gApp.running = false;
}

void PollEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			gApp.running = false;
			break;
		}
	}

	memcpy(gApp.keyboard.data(), SDL_GetKeyboardState(nullptr), SDL_NUM_SCANCODES);
	if (IsKeyDown(SDL_SCANCODE_ESCAPE)) gApp.running = false;
}

void RenderBegin()
{
	gTime.current = TotalTime();
	gTime.update = gTime.current - gTime.previous;
	gTime.previous = gTime.current;

	SDL_SetRenderDrawColor(gApp.renderer, 0, 0, 0, 255);
	SDL_RenderClear(gApp.renderer);
}

void RenderEnd()
{
	gTime.current = TotalTime();
	gTime.render = gTime.current - gTime.previous;
	gTime.previous = gTime.current;

	gTime.frame = gTime.update + gTime.render;
	if (gTime.frame < gTime.target)
	{
		const double waitTime = gTime.target - gTime.frame;
		Wait(waitTime);
		gTime.frame += waitTime;
		gTime.current = gTime.previous = TotalTime();
	}

	array<double, 10>& samples = gTime.samples;
	samples[gTime.frameCount % samples.size()] = gTime.frame;
	if (gTime.frameCount % samples.size() == 0)
	{
		double smooth = 0.0;
		for (double sample : samples)
			smooth += sample;
		smooth /= (double)samples.size();
		gTime.smooth = smooth;
	}

	SDL_RenderPresent(gApp.renderer);	// Display result of render (after wait)
	PollEvents();						// Update events before next frame
	gTime.frameCount++;					// Finally, increment frame counter
}

Texture* LoadTexture(const char* path)
{
	Texture* texture = IMG_LoadTexture(gApp.renderer, path);
	return texture;
}

void UnloadTexture(Texture* texture)
{
	SDL_DestroyTexture(texture);
}

int GetFps()
{
	return gTime.frameCount > gTime.samples.size() ?
		round(1.0 / FrameTimeSmoothed()) : 1.0 / gTime.target;
}

void SetFps(int fps)
{
	gTime.target = 1.0 / (double)fps;
}

float FrameTime()
{
	return gTime.frame;
}

float FrameTimeSmoothed()
{
	return gTime.smooth;
}

double TotalTime()
{
	return SDL_GetTicks64() / 1000.0;
}

void Wait(double seconds)
{
	double destinationTime = TotalTime() + seconds;
	while (TotalTime() < destinationTime) {}
}

bool IsRunning()
{
	return gApp.running;
}

bool IsKeyDown(SDL_Scancode key)
{
	return gApp.keyboard[key] == 1;
}

void DrawRect(const Rect& rect, const Color& color)
{
	SDL_SetRenderDrawColor(gApp.renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRectF(gApp.renderer, &rect);
}

void DrawTexture(Texture* texture, const Rect& rect, double degrees)
{
	SDL_RenderCopyExF(gApp.renderer, texture, nullptr, &rect, degrees, nullptr, SDL_FLIP_NONE);
}
