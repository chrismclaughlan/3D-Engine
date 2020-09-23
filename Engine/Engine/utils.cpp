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
