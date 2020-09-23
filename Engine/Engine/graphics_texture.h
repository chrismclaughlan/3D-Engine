#pragma once
#include "types.h"

enum class TextureType
{
	RGB, RGBA
};

struct Texture
{
	TextureType textureType;

	int width = -1;
	int height = -1;
	float scale = 1.0f;
	void* data = nullptr;

	// Texture slices
	float stepX;
	float stepY;
	int maxCylcesX = 0;
	int maxCylcesY = 0;

	Texture(TextureType textureType, const char* filename, const int sectionWidth, const int sectionHeight);
	~Texture();
	bool loadTextureFromBMP(const char* filename, const int sectionWidth, const int sectionHeight);
	uint lookUp(const float x, const float y, int cycleX = 0, int cycleY = 0) const;
};
