/*****************************************************************//**
 * \file   graphics_text2d.h
 * \brief Contains BMP header info and Text2D struct.
 * 
 * \author Chris
 * \date   September 2020
 *********************************************************************/

#pragma once
#include "utils_vector.h"

// https://stackoverflow.com/questions/49215933/reading-a-monochrome-bitmap-in-c-requires-reading-every-other-line

/**
 * \brief Represents header of a .bmp file.
 */
#pragma pack(1)
struct BmpHeader {
	int8 magic[2];          // 0-1
	uint32_t fileSize;      // 2-5
	uint32_t reserved;      // 6-9
	uint32_t offset;        // 10-13
	uint32_t headerSize;    // 14-17
	uint32_t width;         // 18-21
	uint32_t height;        // 22-25
	uint16_t bitsPerPixel;  // 26-27
	uint16_t bitDepth;      // 28-29
};
#pragma pack()

/**
 * \brief Contains byte array containing character "texture" map.
 */
class Text2D
{
public:
	uint8* pCharMap;		///< Pointer to buffer containing "texture" map of characters
	int width;				///< Width of character map
	int height;				///< Height of character map
	int linesize;			///< ??
	uint32_t colortable[2];	///< 2bit colour table containing possible colours.
							///< Could be used to highlight text with secondary colour.
	
	std::string acceptedChars = " !\"#$%&'()*+,-./"\
		"0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWX"\
		"YZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";	///< Accepted chars. Only 
													///< these chars will be 
													///< valid to draw/input

public:
	~Text2D();
	const bool LoadTextMapFromBMP(const char* filename);
};