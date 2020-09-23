#pragma once
#include "types.h"

struct TextureBase
{
};

struct TextureRGB
{
	int width = -1;
	int height = -1;
	float scale = 1.0f;
	uint8* data = nullptr;

	~TextureRGB();
	bool LoadTextureFromBMP(const char* filename);
	const uint readData(int x, int y) const;
	uint lookUp(const float y, const float x, const int segments = 1, const int index = 1) const;
};

struct TextureRGBA
{
	int width = -1;
	int height = -1;
	float scale = 1.0f;
	uint* data = nullptr;

	// Texture slices
	float stepX;
	float stepY;
	int maxCylcesX = 0;
	int maxCylcesY = 0;

	TextureRGBA(const char* filename, const int sectionWidth, const int sectionHeight);
	~TextureRGBA();

	bool loadTextureFromBMP(const char* filename, const int sectionWidth, const int sectionHeight);
	uint lookUp(const float x, const float y, int cycleX = 0, int cycleY = 0);
};