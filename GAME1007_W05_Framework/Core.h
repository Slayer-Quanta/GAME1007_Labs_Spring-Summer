#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "imgui/imgui.h"
#include "Math.h"

using Texture = SDL_Texture;
using Color = SDL_Color;
using Sound = Mix_Chunk;
using Music = Mix_Music;
using Font = TTF_Font;
using GuiCallback = void(*)(void* data);
void SetGuiCallback(GuiCallback callback, void* data);

void AppInit(int width, int height);
void AppExit();

void RenderBegin();
void RenderEnd();

Texture* LoadTexture(const char* path);
void UnloadTexture(Texture* texture);
void SetBlendMode(SDL_BlendMode mode);
void SetBlendMode(Texture* texture, SDL_BlendMode mode);
void Tint(Texture* texture, const Color& color);

Font* LoadFont(const char* path, int size = 20);
void UnloadFont(Font* font);

Sound* LoadSound(const char* path);
void UnloadSound(Sound* sound);
int PlaySound(Sound* sound, bool loop = false);
void PauseChannel(int channel);
void ResumeChannel(int channel);

Music* LoadMusic(const char* path);
void UnloadMusic(Music* music);
void PlayMusic(Music* music, bool loop = true);
void PauseMusic();
void ResumeMusic();

int GetFps();			// Average frame rate
void SetFps(int fps);	// Desired (maximum) frame rate

float FrameTime();			// Time duration for frame update + frame render
float FrameTimeSmoothed();	// Time duration for frame update + frame render over 10 frames

double TotalTime();			// Time since program start in seconds
void Wait(double seconds);	// Halts the program for seconds

bool IsRunning();
void Quit();

bool IsKeyDown(SDL_Scancode key);
bool IsKeyPressed(SDL_Scancode key);

bool IsMouseDown();
bool IsMouseMoved();
bool IsMouseClicked();
Point MousePosition();
Point MouseDelta();

void DrawLine(const Point& start, const Point& end, const Color& color);
void DrawRect(const Rect& rect, const Color& color);
void DrawTexture(Texture* texture, const Rect& rect, float degrees = 0.0f);
void DrawTexture(Texture* texture, const SDL_Rect& src, const Rect& dst, float degrees = 0.0f);
void DrawText(const char* text, float x, float y, Font* font, const Color& color = { 255, 255, 255, 255 }, float degrees = 0.0f);
void DrawTextCentered(const char* text, float x, float y, Font* font, const Color& color = { 255, 255, 255, 255 }, float degrees = 0.0f);
