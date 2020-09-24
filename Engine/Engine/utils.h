/*****************************************************************//**
 * \file   utils.h
 * \brief  Contains definitions for various usefull cross-platform functions
 * 
 * \author Chris
 * \date   September 2020
 *********************************************************************/

#pragma once
#include "types.h"

struct vec2
{
	int x, y;
};

struct vec3
{
	int x, y, z;
};

struct vec2f
{
	float x, y;

	vec2f() : x(0.0f), y(0.0f) {};
	vec2f(const float x, const float y) : x(x), y(y) {};
};

struct vec3f
{
	float x, y, z;
};

extern void clamp(int* val, const int min, const int max);
extern void clampu(uint* val, const uint min, const uint max);
extern void clampf(float* val, const float min, const float max);
extern void clampd(double* val, const double min, const double max);
extern const float normalise(const float min, const float max, float input);
extern uint rgbToHex(const uint8 r, const uint8 g, const uint8 b);
