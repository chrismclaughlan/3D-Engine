#include "utils.h"
#include "graphics_text2d.h"
#include <fstream>
#include <iostream>

/**
 * \brief Destructor for Text2D.
 * 
 * Destroys pCharMap if it exists.
 */
Text2D::~Text2D()
{
	if (pCharMap != nullptr)
	{
		delete[] pCharMap;
		pCharMap = nullptr;
	}
}

/**
 * \brief Loads .bmp file and loads data into pCharMap.
 * 
 * \param filename Relative filename ending in .bmp
 * \return Returns true if successful, otherwise false
 */
const bool Text2D::LoadTextMapFromBMP(const char* filename)
{
	if (!stringEndsWith(filename, ".bmp"))
	{
		std::cerr
			<< "Error loading: " << filename
			<< " -> File is not a .bmp file\n";
		return false;
	}

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