#pragma once
#include "types.h"

struct TextureNew
{
	int width = -1;
	int height = -1;
	float scale = 1.0f;
	uint* data = nullptr;

	// new
	float stepX;
	float stepY;
	int maxCylcesX = 0;
	int maxCylcesY = 0;

	TextureNew(const char* filename, const int sectionWidth, const int sectionHeight);
	~TextureNew();

	bool loadTextureFromBMP(const char* filename, const int sectionWidth, const int sectionHeight);
	uint lookUp(const float x, const float y, int cycleX = 0, int cycleY = 0);
};