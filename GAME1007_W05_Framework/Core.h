#pragma once
#include <SDL.h>

using Texture = SDL_Texture;
using Rect = SDL_FRect;
using Color = SDL_Color;
using Point = SDL_FPoint;

void AppInit(int width, int height);
void AppExit();

void RenderBegin();
void RenderEnd();

Texture* LoadTexture(const char* path);
void UnloadTexture(Texture* texture);

int GetFps();			// Average frame rate
void SetFps(int fps);	// Desired (maximum) frame rate

float FrameTime();			// Time duration for frame update + frame render
float FrameTimeSmoothed();	// Time duration for frame update + frame render over 10 frames

double TotalTime();			// Time since program start in seconds
void Wait(double seconds);	// Halts the program for seconds

bool IsRunning();
bool IsKeyDown(SDL_Scancode key);

void DrawRect(const Rect& rect, const Color& color);
void DrawTexture(Texture* texture, const Rect& rect, double degrees = 0.0f);