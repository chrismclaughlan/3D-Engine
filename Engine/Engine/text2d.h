/*****************************************************************//**
 * \file   text2d.h
 * \brief Contains BMP header info and Text2D struct.
 * 
 * \author Chris
 * \date   September 2020
 *********************************************************************/

#pragma once
#include "vector.h"

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
	
	// Debugging
	//                                      1         2         3         4         5         6          7         8         9
	//                           012 3456789012345678901234567890123456789012345678901234567890 123456789012345678901234567890123456789
	std::string acceptedChars = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

public:
	~Text2D();
	bool LoadTextMapFromBMP(const char* filename)
	{
		BmpHeader head;
		std::ifstream f(filename, std::ios::binary);
		if (!f.good())
			return false;

		int headsize = sizeof(BmpHeader);
		f.read((char*)& head, headsize);

		if (head.bitsPerPixel != 1)
		{
			f.close();
			throw "Invalid bitmap loaded";
		}

		height = head.height;
		width = head.width;

		int bpp = 1;
		linesize = ((width * bpp + 31) / 32) * 4;
		int filesize = linesize * height;

		//std::vector<uint8> data(filesize);
		pCharMap = new uint8[filesize];

		//read color table
		f.seekg(54);
		f.read((int8*)& colortable[0], 4);
		f.read((int8*)& colortable[1], 4);

		f.seekg(head.offset);
		f.read((int8*)& pCharMap[0], filesize);

		for (int y = height - 1; y >= 0; y--)
		{
			for (int x = 0; x < width; x++)
			{
				int pos = y * linesize + x / 8;
				int bit = 1 << (7 - x % 8);
				int v = (pCharMap[pos] & bit) > 0;
			}
		}

		f.close();
		return true;
	}
};