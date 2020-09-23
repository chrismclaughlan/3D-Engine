#pragma once
#include "types.h"

extern void Clamp(int min, int* val, int max);
extern void Clamp(float min, float* val, float max);
extern void Clamp(double min, double* val, double max);
extern const float normalise(const float min, const float max, float input);
extern uint rgbToHex(const uint8 r, const uint8 g, const uint8 b);