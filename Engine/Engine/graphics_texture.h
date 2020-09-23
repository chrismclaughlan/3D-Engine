#pragma once
#include "types.h"

//bool TextureBase::loadRGBTextureFromBMP(const char* filename)
//{
//	int i;
//	FILE* f;
//	errno_t err = fopen_s(&f, filename, "r");
//	if (err != 0)
//	{
//		std::cerr << "Error opening file " << filename << "\n";
//		return false;
//	}
//
//	uint8* info = new uint8[HEADER_SIZE_24_BIT_BMP];
//
//	// read the 54-byte header
//	fread(info, sizeof(uint8), HEADER_SIZE_24_BIT_BMP, f);
//
//	const int bOffset = info[10];
//
//	std::cout << "Offset: " << bOffset << std::endl;
//
//	// extract image height and width from header
//	width = *(int*)& info[18];
//	height = *(int*)& info[22];
//
//	assert(height > 0);
//	scale = (float)width / (float)height;
//
//	std::cout << "TextureRGB width=" << width << " height=" << height
//		<< " scale=" << scale << "\n";
//
//	// allocate 3 bytes per pixel
//	int size = 3 * width * height;
//	data = new uint8[size];
//
//	// read the rest of the data at once
//	fread(data, sizeof(uint8), size, f);
//
//	fclose(f);
//
//	delete[] info;
//	info = nullptr;
//
//	return true;
//}

enum class TextureType
{
	RGB, RGBA
};

struct TextureBase
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

	TextureBase(TextureType textureType, const char* filename, const int sectionWidth, const int sectionHeight);
	~TextureBase();
	bool loadRGBATextureFromBMP(const char* filename, const int sectionWidth, const int sectionHeight);
	bool loadRGBTextureFromBMP(const char* filename);
	uint lookUp(const float x, const float y, int cycleX = 0, int cycleY = 0) const;
};

//struct TextureRGB : TextureBase
//{
//	~TextureRGB();
//	bool LoadTextureFromBMP(const char* filename);
//	const uint readData(int x, int y) const;
//	uint lookUp(const float y, const float x, const int segments = 1, const int index = 1) const;
//};
//
//struct TextureRGBA : TextureBase
//{
//	TextureRGBA(const char* filename, const int sectionWidth, const int sectionHeight);
//	~TextureRGBA();
//
//	bool loadTextureFromBMP(const char* filename, const int sectionWidth, const int sectionHeight);
//	uint lookUp(const float x, const float y, int cycleX = 0, int cycleY = 0);
//};