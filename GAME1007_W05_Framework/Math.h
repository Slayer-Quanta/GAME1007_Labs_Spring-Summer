#pragma once
#include <SDL.h>
#include <cmath>

using Rect = SDL_FRect;
using Point = SDL_FPoint;

#ifndef PI
#define PI 3.14159265358979323846f
#endif

#ifndef EPSILON
#define EPSILON 0.000001f
#endif

#ifndef DEG2RAD
#define DEG2RAD (PI/180.0f)
#endif

#ifndef RAD2DEG
#define RAD2DEG (180.0f/PI)
#endif

// Random number between min and max (can be negative)
inline float Random(float min, float max)
{
	return min + (rand() / ((float)RAND_MAX / (max - min)));
}

// Clamps value between min and max
inline float Clamp(float value, float min, float max)
{
	float result = (value < min) ? min : value;

	if (result > max) result = max;

	return result;
}

// Vector addition
inline Point operator+(Point a, Point b)
{
	Point result{ a.x + b.x, a.y + b.y };

	return result;
}

// Vector subtraction
inline Point operator-(Point a, Point b)
{
	Point result{ a.x - b.x, a.y - b.y };

	return result;
}

// Vector-scalar multiplication
inline Point operator*(Point a, float b)
{
	Point result{ a.x * b, a.y * b };

	return result;
}

// Vector-scalar division
inline Point operator/(Point a, float b)
{
	Point result{ a.x / b, a.y / b };

	return result;
}

// Distance between two points (c = sqrt(a^2 + b^2))
inline float Distance(Point a, Point b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float result = sqrtf(dx * dx + dy * dy);

	return result;
}

// Squared distance between two points (c^2 = a^2 + b^2)
inline float DistanceSqr(Point a, Point b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float result = dx * dx + dy * dy;

	return result;
}

// Magnitude of a vector (distance between point and origin [0, 0])
inline float Length(Point p)
{
	float result = sqrtf(p.x * p.x + p.y * p.y);

	return result;
}

// Squared magnitude of a vector (squared distance between point and origin [0, 0])
inline float LengthSqr(Point p)
{
	float result = p.x * p.x + p.y * p.y;

	return result;
}

// https://www.mathsisfun.com/algebra/vectors-dot-product.html
inline float Dot(Point a, Point b)
{
	float result = (a.x * b.x + a.y * b.y);

	return result;
}

// https://www.mathsisfun.com/algebra/vectors-cross-product.html 
inline float Cross(Point a, Point b)
{
	float result = a.x * b.y - a.y * b.x;

	return result;
}

// Convert direction (unit vector) to angle in radians
inline float Angle(Point p)
{
	float result = atan2f(p.y, p.x);

	return result;
}

// Convert angle in radians to direction (unit vector)
inline Point Direction(float angle/*radians*/)
{
	Point result = { cosf(angle), sinf(angle) };

	return result;
}

// Vector projection
inline Point Project(Point a, Point b)
{
	float t = Dot(a, b) / Dot(b, b);
	Point result = { t * b.x, t * b.y };

	return result;
}

// Project point P onto line AB 
inline Point ProjectPointLine(Point a, Point b, Point p)
{
	Point AB = b - a;
	float t = Dot(p - a, AB) / Dot(AB, AB);
	Point result = a + AB * Clamp(t, 0.0f, 1.0f);

	return result;
}

// Vector normalization; produces a unit vector (magnitude of 1)
inline Point Normalize(Point p)
{
	Point result{};

	float length = Length(p);
	if (length > 0.0f)
		result = { p.x / length, p.y / length };

	return result;
}

// Rotate point P by angle in radians
inline Point Rotate(Point p, float angle/*radians*/)
{
	Point result{};

	float cosres = cosf(angle);
	float sinres = sinf(angle);

	result.x = p.x * cosres - p.y * sinres;
	result.y = p.x * sinres + p.y * cosres;

	return result;
}

// Interpolate between A and B based on t (0 = fully A, 1 = fully B)
inline Point Lerp(Point a, Point b, float t)
{
	Point result = a + (b - a) * t;

	return result;
}