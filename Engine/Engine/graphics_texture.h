/*****************************************************************//**
 * \file   graphics_texture.h
 * \brief  Contains Texture types/struct
 * 
 * \author Chris
 * \date   September 2020
 *********************************************************************/

#pragma once
#include "types.h"

/**
 * \brief Contains info about what/how memory to be interpreted for textures.
 */
enum class TextureType
{
	RGB, RGBA
};

/**
 * \brief Texture struct to describe the texture pixel and slice information.
 */
struct Texture
{
	TextureType textureType;	///< Describes how data is to be interpreted

	int width = -1;				///< Width of texture in pixels
	int height = -1;			///< Height of texture in pixels
	float scale = 1.0f;			///< Scale of texture (width/height)
	void* data = nullptr;		///< Data buffer containing pixel information

	// Texture slices
	int maxCylcesX = 0;			///< Number of slices in x-axis
	int maxCylcesY = 0;			///< Number of slices in y-axis
	float stepX;				///< Multiplyer to slice texture in x-axis
	float stepY;				///< Multiplyer to slice texture in y-axis
		
	Texture(TextureType textureType, const char* filename, const int sectionWidth, const int sectionHeight);
	~Texture();
	bool loadTextureFromBMP(const char* filename, const int sectionWidth, const int sectionHeight);
	colour_t lookUp(const float x, const float y, int cycleX = 0, int cycleY = 0) const;
};
