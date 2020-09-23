#include "utils.h"

void Clamp(int min, int* val, int max)
{
	if (*val < min)* val = min;
	else if (*val > max)* val = max;
}

void Clamp(float min, float* val, float max)
{
	if (*val < min)* val = min;
	else if (*val > max)* val = max;
}

void Clamp(double min, double* val, double max)
{
	if (*val < min)* val = min;
	else if (*val > max)* val = max;
}

const float normalise(const float min, const float max, float input)
{
	return (input - min) / (max - min);
}

uint rgbToHex(const uint8 r, const uint8 g, const uint8 b)
{
	uint h = 0x0;
	h += r << 16;
	h += g << 8;
	h += b << 0;
	return h;
}