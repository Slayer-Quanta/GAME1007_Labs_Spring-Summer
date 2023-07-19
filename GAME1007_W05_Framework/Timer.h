#pragma once
#include "Math.h"
struct Timer
{
	float duration = 0.0f;	// max time
	float elapsed = 0.0f;	// current time

	bool Expired() { return elapsed >= duration; }
	void Reset() { elapsed = 0.0f; }
	void Tick(float dt) { elapsed += dt; }

	// Returns value from 0-1 based on completion. Useful for interpolation!
	float Normalize() { return Clamp(elapsed / duration, 0.0f, 1.0f); }
};