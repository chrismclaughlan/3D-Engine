#pragma once

extern void Clamp(int min, int* val, int max);
extern void Clamp(float min, float* val, float max);
extern void Clamp(double min, double* val, double max);
extern const float normalise(const float min, const float max, float input);
