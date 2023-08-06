#include "Core.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer.h"
#include <cassert>
#include <array>
#include <unordered_map>
#include "Widgets.h"

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
	GuiCallback guiCallback = nullptr;
	void* guiData = nullptr;

	vector<Widget*> widgets;

	// Previous and current mouse positions
	int mx = 0, my = 0, pmx = 0, pmy = 0;
	Uint32 mouseStatePrevious = 0;
	Uint32 mouseStateCurrent = 0;
	bool leftMouseDown = false;
	bool rightMouseDown = false;
	bool middleMouseDown = false;
	bool leftMouseClicked = false;
	bool rightMouseClicked = false;
	bool middleMouseClicked = false;
	bool mouseMoved = false;

	array<Uint8, SDL_NUM_SCANCODES> keyboardCurrent{};
	array<Uint8, SDL_NUM_SCANCODES> keyboardPrevious{};
} gApp;

void SetGuiCallback(GuiCallback callback, void* data)
{
	gApp.guiCallback = callback;
	gApp.guiData = data;
}

void AppInit(int width, int height)
{
	assert(!gApp.running);
	assert(gApp.window == nullptr);
	assert(gApp.renderer == nullptr);

	assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
	assert(Mix_OpenAudio(48000, AUDIO_S16SYS, 2, 2048) == 0);
	assert(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == IMG_INIT_PNG | IMG_INIT_JPG);
	assert(TTF_Init() == 0);
	gApp.window = SDL_CreateWindow("Fundamentals 2 Framework", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
	gApp.renderer = SDL_CreateRenderer(gApp.window, -1, 0);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForSDLRenderer(gApp.window, gApp.renderer);
	ImGui_ImplSDLRenderer_Init(gApp.renderer);

	gTime.previous = TotalTime();
	gApp.running = true;
}

void AppExit()
{
	assert(gApp.window != nullptr);
	assert(gApp.renderer != nullptr);

	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(gApp.renderer);
	SDL_DestroyWindow(gApp.window);
	TTF_Quit();
	IMG_Quit();
	Mix_Quit();
	SDL_Quit();

	gApp.running = false;
}

void PollEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			Quit();
			break;
		}
	}

	memcpy(gApp.keyboardPrevious.data(), gApp.keyboardCurrent.data(), SDL_NUM_SCANCODES);
	memcpy(gApp.keyboardCurrent.data(), SDL_GetKeyboardState(nullptr), SDL_NUM_SCANCODES);
	if (IsKeyDown(SDL_SCANCODE_ESCAPE)) Quit();

	gApp.pmx = gApp.mx; gApp.pmy = gApp.my;
	gApp.mouseStatePrevious = gApp.mouseStateCurrent;
	gApp.mouseStateCurrent = SDL_GetMouseState(&gApp.mx, &gApp.my);
	gApp.mouseMoved = gApp.mx != gApp.pmx || gApp.my != gApp.pmy;

	gApp.leftMouseDown = gApp.mouseStateCurrent & SDL_BUTTON_LMASK;
	gApp.rightMouseDown = gApp.mouseStateCurrent & SDL_BUTTON_RMASK;
	gApp.middleMouseDown = gApp.mouseStateCurrent & SDL_BUTTON_MMASK;

	gApp.leftMouseClicked =
		(gApp.mouseStateCurrent & SDL_BUTTON_LMASK) <
		(gApp.mouseStatePrevious & SDL_BUTTON_LMASK);

	gApp.rightMouseClicked =
		(gApp.mouseStateCurrent & SDL_BUTTON_RMASK) <
		(gApp.mouseStatePrevious & SDL_BUTTON_RMASK);

	gApp.middleMouseClicked =
		(gApp.mouseStateCurrent & SDL_BUTTON_MMASK) <
		(gApp.mouseStatePrevious & SDL_BUTTON_MMASK);
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
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow();
	if (gApp.guiCallback != nullptr) gApp.guiCallback(gApp.guiData);
	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

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

void SetBlendMode(SDL_BlendMode mode)
{
	SDL_SetRenderDrawBlendMode(gApp.renderer, mode);
}

void SetBlendMode(Texture* texture, SDL_BlendMode mode)
{
	SDL_SetTextureBlendMode(texture, mode);
}

void Tint(Texture* texture, const Color& color)
{
	SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
	SDL_SetTextureAlphaMod(texture, color.a);
}

Font* LoadFont(const char* path, int size)
{
	return TTF_OpenFont(path, size);
}

void UnloadFont(Font* font)
{
	TTF_CloseFont(font);
}

Sound* LoadSound(const char* path)
{
	return Mix_LoadWAV(path);
}

void UnloadSound(Sound* sound)
{
	Mix_FreeChunk(sound);
}

int PlaySound(Sound* sound, bool loop)
{
	return Mix_PlayChannel(-1, sound, loop ? -1 : 0);
}

void PauseChannel(int channel)
{
	Mix_Pause(channel);
}

void ResumeChannel(int channel)
{
	Mix_Resume(channel);
}

Music* LoadMusic(const char* path)
{
	return Mix_LoadMUS(path);
}

void UnloadMusic(Music* music)
{
	Mix_FreeMusic(music);
}

void PlayMusic(Music* music, bool loop)
{
	Mix_PlayMusic(music, loop ? -1 : 0);
}

void PauseMusic()
{
	Mix_PauseMusic();
}

void ResumeMusic()
{
	Mix_ResumeMusic();
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

void Quit()
{
	gApp.running = false;
}

bool IsKeyDown(SDL_Scancode key)
{
	return gApp.keyboardCurrent[key] == 1;
}

bool IsKeyPressed(SDL_Scancode key)
{
	return gApp.keyboardCurrent[key] > gApp.keyboardPrevious[key];
}

bool IsMouseDown()
{
	return gApp.leftMouseDown;
}

bool IsMouseMoved()
{
	return gApp.mouseMoved;
}

bool IsMouseClicked()
{
	return gApp.leftMouseClicked;
}

Point MousePosition()
{
	return { (float)gApp.mx, (float)gApp.my };
}

Point MouseDelta()
{
	return { (float)gApp.mx - (float)gApp.pmx, (float)gApp.my - (float)gApp.pmy };
}

void DrawLine(const Point& start, const Point& end, const Color& color)
{
	SDL_SetRenderDrawColor(gApp.renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawLineF(gApp.renderer, start.x, start.y, end.x, end.y);
}

void DrawRect(const Rect& rect, const Color& color)
{
	SDL_SetRenderDrawColor(gApp.renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRectF(gApp.renderer, &rect);
}

void DrawTexture(Texture* texture, const Rect& rect, float degrees)
{
	SDL_RenderCopyExF(gApp.renderer, texture, nullptr, &rect, degrees, nullptr, SDL_FLIP_NONE);
}

void DrawTexture(Texture* texture, const SDL_Rect& src, const Rect& dst, float degrees)
{
	SDL_RenderCopyExF(gApp.renderer, texture, &src, &dst, degrees, nullptr, SDL_FLIP_NONE);
}

void DrawText(const char* text, float x, float y, Font* font, const Color& color, float degrees)
{
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(gApp.renderer, surface);
	DrawTexture(texture, { x, y, (float)surface->w, (float)surface->h }, degrees);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}

void DrawTextCentered(const char* text, float x, float y, Font* font, const Color& color, float degrees)
{
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(gApp.renderer, surface);
	DrawTexture(texture, { x - surface->w * 0.5f, y - surface->h * 0.5f, (float)surface->w, (float)surface->h }, degrees);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}
