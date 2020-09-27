#include "utils.h"
#include "graphics.h"
#include "graphics_texture.h"
#include <strstream>
#include <fstream>
#include <iostream>
#include <assert.h>
#include "exception.h"  // test

#define HEADER_SIZE_24_BIT_BMP (54)
#define HEADER_SIZE_32_BIT_BMP (66)

/**
 * \brief Loads texture from a .bmp file on initialistion.
 * 
 * \param textureType Describes how to read and store the .bmp data
 * \param filename Relative filename of .bmp file to read
 * \param sectionWidth Width of each slice in texture in pixels
 * \param sectionHeight Height of each slice in texture in pixels
 */
Texture::Texture(TextureType textureType, const char* filename, const int sectionWidth, const int sectionHeight)
	: textureType(textureType)
{
	if (!loadTextureFromBMP(filename, sectionWidth, sectionHeight))
	{
		std::cerr << "Error loading " << filename << "\n";
	}
}

/**
 * \brief Destroys data if it exists.
 */
Texture::~Texture()
{
	if (data != nullptr)
	{
		delete[] data;
		data = nullptr;
	}
}

/**
 * \brief Loads pixel data from .bmp file.
 * 
 * \param filename Relative filename of .bmp file to read
 * \param sectionWidth Width of each slice in texture in pixels
 * \param sectionHeight Height of each slice in texture in pixels
 * \return Returns true if successful, otherwise false.
 */
bool Texture::loadTextureFromBMP(const char* filename, const int sectionWidth, const int sectionHeight)
{
	FILE* file;
	errno_t err = fopen_s(&file, filename, "r");
	if (err != 0)
	{
		std::cerr << "Error opening file " << filename << "\n";
		return false;
	}

	int headerSize;
	if (textureType == TextureType::RGB)
	{
		headerSize = HEADER_SIZE_24_BIT_BMP;
	}
	else if (textureType == TextureType::RGBA)
	{
		headerSize = HEADER_SIZE_32_BIT_BMP;
	}
	else
	{
		return false;
	}
	
	uint8* info = new uint8[headerSize];

	// read header
	fread(info, sizeof(uint8), headerSize, file);

	// Check valid
	const int bitOffset = info[10];
	std::cout << "bitOffset: " << bitOffset << std::endl;

	if (bitOffset != headerSize)
	{
		std::cerr << "Error reading " << filename << "\n";
		delete[] info;
		info = nullptr;
		//THROW_EXCEPTION("Texture", "Wrong TextureType");
		return false;
	}

	// extract image height and width from header
	width = *(int*)& info[18];
	height = *(int*)& info[22];

	delete[] info;
	info = nullptr;

	assert(height > 0);
	scale = (float)width / (float)height;

	assert(sectionWidth > 0);
	assert(sectionHeight > 0);
	if (width % sectionWidth != 0 || height % sectionHeight != 0)
	{
		std::cerr << "Error loading texture from "
			<< filename << ": does not allign with "
			<< "sectionWidth or sectionHeight. "
			<< "Width = " << sectionWidth
			<< "Height = " << sectionHeight
			<< "\n";
		return false;
	}

	maxCylcesX = width / sectionWidth;
	maxCylcesY = height / sectionHeight;

	stepX = 1.0f / maxCylcesX;
	stepY = 1.0f / maxCylcesY;

	std::cout
		<< "width=" << width << " height=" << height
		<< " scale=" << scale
		<< " stepX=" << stepX << " stepY=" << stepY
		<< "\n";

	int size;
	if (textureType == TextureType::RGB)
	{
		size = 3 * width * height;
		data = new uint8[size];
		fread(data, sizeof(uint8), size, file);
	}
	else if (textureType == TextureType::RGBA)
	{
		size = width * height;
		data = new uint[size];
		fread(data, sizeof(uint), size, file);
	}
	else
	{
		return false;
	}

	fclose(file);

	return true;
}

// Take normalised x and y values and maps them to texture coords

/**
 * \brief Takes normalised x and y floats and maps them to texture coords.
 * 
 * \param x Normalised x-coord
 * \param y Normalised y-coord
 * \param cycleX Which slice to read from in x-axis (starting at 0, default 0)
 * \param cycleY Which slice to read from in y-axis (starting at 0, default 0)
 * \return Returns colour (colour_t) of texture coord, if failure occurs returns
 * 0
 */
colour_t Texture::lookUp(float x, float y, int cycleX, int cycleY) const
{
	clampf(&x, 0.0f, 1.0f);
	clampf(&y, 0.0f, 1.0f);

	if (textureType == TextureType::RGB)
	{
		std::swap(x, y);  // flip

		int xInd, yInd;
		xInd = (int)(x * (float)width / scale);
		yInd = (int)(y * (float)height * scale);

		//xInd += 1;

		//xInd = height - xInd;  // flip

		uint8* d = (uint8*)data;

		return rgbToHex(
			d[3 * (xInd * width + yInd) + 2],
			d[3 * (xInd * width + yInd) + 1],
			d[3 * (xInd * width + yInd) + 0]);
	}
	else if (textureType == TextureType::RGBA)
	{
		clamp(&cycleX, 0, maxCylcesX);
		clamp(&cycleY, 0, maxCylcesY);

		// Get texture slice
		x = (x * stepX) + (stepX * cycleX);
		y = (y * stepY) + (stepY * cycleY);

		// Get texture coordinates
		int xInd, yInd;
		xInd = (int)(x * (float)width);
		yInd = (int)(y * (float)height);

		colour_t* d = (uint*)data;

		return d[xInd + (yInd * width)];
	}

	return 0;  // error
}
