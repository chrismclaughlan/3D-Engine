#pragma once
#include "types.h"
#include <strstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <assert.h>

struct Triangle;
class Mesh;

// make struct?
class Vector2
{
public:
	float u = 0.0f;
	float v = 0.0f;
	float w = 1.0f;

public:
	Vector2() : u(0.0f), v(0.0f), w(1.0f) {};
	Vector2(float u, float v) : u(u), v(v), w(1.0f) {};
	Vector2(float u, float v, float w) : u(u), v(v), w(w) {};
};

class Vector
{
public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;

public:
	Vector();
	Vector(float x, float y, float z);
	Vector(float x, float y, float z, float w);

	friend std::ostream& operator<<(std::ostream& os, const Vector& v);

	friend bool operator==(const Vector& a, const Vector& b);
	friend Vector operator+(const Vector& a, const Vector& b);
	friend Vector operator-(const Vector& a, const Vector& b);
	friend Vector operator*(const Vector& a, const float k);
	friend Vector operator*(const float k, Vector& a) { return a * k; };
	friend Vector operator/(const Vector& a, const float k);
	friend Vector operator+=(Vector& a, const Vector& b);
	friend Vector operator-=(Vector& a, const Vector& b);
	friend Vector operator*=(Vector& a, const float k);
	friend Vector operator/=(Vector& a, const float k);
	Vector& operator-();

	static float DotProduct(const Vector& a, const Vector& b);
	static float Length(const Vector& a);
	void Normalise();
	static Vector Normalise(const Vector& a);
	static Vector CrossProduct(const Vector& a, const Vector& b);

	static Vector IntersectPlane(Vector& plane_p, Vector& plane_n, Vector& lineStart, Vector& lineEnd, float& t);
	static int32 TriangleClipAgainstPlane(Vector plane_p, Vector plane_n, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2);

	void setZero();
};

class Matrix4x4
{
public:
	float m[4][4];
public:
	Matrix4x4();

	friend Vector operator*(const Matrix4x4& m, const Vector& v);
	friend Vector operator*=(const Matrix4x4& m, const Vector& v);
	friend Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b);
	friend Matrix4x4 operator*=(Matrix4x4& a, const Matrix4x4& b);

	// Returns an identity matrix TODO google
	void MakeIdentity();
	void MakeRotationX(float fRad);
	void MakeRotationY(float fRad);
	void MakeRotationZ(float fRad);
	void MakeTranslation(float x, float y, float z);
	void MakeProjection(float fFov, float fAspectRatio, float fNear, float fFar);
	void MakePointAt(const Vector& pos, const Vector& target, const Vector& up);
	void MakeQuickInverse();
};

struct Triangle
{
	const Mesh* parent = nullptr;

	Vector p[3];
	Vector2 t[3];
	uint32 colour;
	// alpha
};

class Texture
{
public:
	int32 width = -1;
	int32 height = -1;
	float scaleW = 1.0f;
	float scaleH = 1.0f;
	uint8* map = nullptr;

public:
	~Texture()
	{
		if (map != nullptr)
		{
			delete[] map;
			map = nullptr;
		}
	}

	//inline void writeTo(int x, int y, uint32 a)
	//{
	//	map[x * width + y] = a;
	//}
	//
	//inline uint32 readFrom(int x, int y) const
	//{
	//	return map[x * width + y];
	//}

	//static long int binToHex(long int binaryval)
	//{
	//	long int hexadecimalval = 0, i = 1, remainder;
	//
	//	while (binaryval != 0)
	//	{
	//		remainder = binaryval % 10;
	//		hexadecimalval = hexadecimalval + remainder * i;
	//		i = i * 2;
	//		binaryval = binaryval / 10;
	//	}
	//
	//	return hexadecimalval;
	//}

	static uint32 rgbToHex(const uint8 r, const uint8 g, const uint8 b)
	{
		uint32 h = 0x0;
		h += r << 16;
		h += g << 8;
		h += b << 0;
		return h;
	}

	bool LoadTextureFromBMP(const char* filename)
	{
		int i;	
		FILE* f;
		errno_t err = fopen_s(&f, filename, "r");
		if (err != 0)
		{
			std::cerr << "Error opening file " << filename << "\n";
			return false;
		}
		unsigned char info[54];

		// read the 54-byte header
		fread(info, sizeof(uint8), 54, f);

		// extract image height and width from header
		width = *(int*)& info[18];
		height = *(int*)& info[22];
		if (width < height)
		{
			scaleH = height / width;
			scaleW = 1.0f / scaleH;
		}
		else
		{
			scaleW = width / height;
			scaleH = 1.0f / scaleW;
		}

		// allocate 3 bytes per pixel
		int size = 3 * width * height;
		map = new uint8[size];

		// read the rest of the data at once
		fread(map, sizeof(uint8), size, f);
		fclose(f);

		return true;
	}

	uint32 lookUp(float x, float y) const
	{
		int32 xIndex = (x * width) / scaleW;
		int32 yIndex = (y * height) / scaleH;

		return rgbToHex(
			map[3 * (xIndex * width + yIndex) + 2], 
			map[3 * (xIndex * width + yIndex) + 1], 
			map[3 * (xIndex * width + yIndex) + 0]);
	}
};

class Mesh
{
public:
	Matrix4x4 matrixWorldPos;
	std::vector<Triangle> tris;
	Texture* texture = nullptr;
	// check all right
	float x = 0.0f;  // + left / - right
	float y = 0.0f;  // + upwards / - downwards
	float z = 5.0f;  // + forwards / - backwards

public:
	bool LoadObjectFile(std::string filename, bool hasTexture)
	{
		tris.clear();

		std::ifstream file(filename);

		if (!file.is_open())
		{
			return false;
		}

		std::vector<Vector> verticies;
		std::vector<Vector2> textures;

		while (!file.eof())
		{
			// assuming line <= 128
			char line[128];
			file.getline(line, 128);

			std::strstream s;
			s << line;

			char temp;  // hold junk

			if (line[0] == 'v')
			{
				if (line[1] == 't' && line[2] == ' ')
				{
					Vector2 v;
					s >> temp >> temp >> v.u >> v.v;
					textures.push_back(v);
				}
				else if (line[1] == ' ')
				{
					Vector v;
					s >> temp >> v.x >> v.y >> v.z;
					verticies.push_back(v);
				}
			}

			if (!hasTexture)
			{
				if (line[0] == 'f' && line[1] == ' ')
				{
					int f[3];
					s >> temp >> f[0] >> f[1] >> f[2];
					tris.push_back({ this, verticies[f[0] - 1], verticies[f[1] - 1], verticies[f[2] - 1] });
				}
			}
			else
			{
				// TODO throw exception if file doesnt actually contain texture shit
				if (line[0] == 'f' && line[1] == ' ')
				{
					s >> temp;
					std::string tokens[6];
					int32 nTokenCount = -1;

					while (!s.eof())
					{
						char c = s.get();
						if (c == ' ' || c == '/')
						{
							nTokenCount++;
						}
						else
						{
							tokens[nTokenCount].append(1, c);
						}
					}

					tokens[nTokenCount].pop_back();

					tris.push_back({ this, Vector(verticies[std::stoi(tokens[0]) - 1]), Vector(verticies[std::stoi(tokens[2]) - 1]), Vector(verticies[std::stoi(tokens[4]) - 1]), Vector2(textures[std::stoi(tokens[1]) - 1]), Vector2(textures[std::stoi(tokens[3]) - 1]), Vector2(textures[std::stoi(tokens[5]) - 1]) });
				}
			}
		}

		return true;
	}
	void LoadTestCube()
	{
		// South
		tris.clear();
		tris.push_back({ this, Vector(0.0f, 0.0f, 0.0f, 1.0f), Vector(0.0f, 1.0f, 0.0f, 1.0f), Vector(1.0f, 1.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f) });
		tris.push_back({ this, Vector(0.0f, 0.0f, 0.0f, 1.0f), Vector(1.0f, 1.0f, 0.0f, 1.0f), Vector(1.0f, 0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f, 1.0f) });
						
		tris.push_back({ this, Vector(1.0f, 0.0f, 0.0f, 1.0f), Vector(1.0f, 1.0f, 0.0f, 1.0f), Vector(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f) });
		tris.push_back({ this, Vector(1.0f, 0.0f, 0.0f, 1.0f), Vector(1.0f, 1.0f, 1.0f, 1.0f), Vector(1.0f, 0.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f, 1.0f) });
						 
		tris.push_back({ this, Vector(1.0f, 0.0f, 1.0f, 1.0f), Vector(1.0f, 1.0f, 1.0f, 1.0f), Vector(0.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f) });
		tris.push_back({ this, Vector(1.0f, 0.0f, 1.0f, 1.0f), Vector(0.0f, 1.0f, 1.0f, 1.0f), Vector(0.0f, 0.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f, 1.0f) });
						
		tris.push_back({ this, Vector(0.0f, 0.0f, 1.0f, 1.0f), Vector(0.0f, 1.0f, 1.0f, 1.0f), Vector(0.0f, 1.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f) });
		tris.push_back({ this, Vector(0.0f, 0.0f, 1.0f, 1.0f), Vector(0.0f, 1.0f, 0.0f, 1.0f), Vector(0.0f, 0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f, 1.0f) });
						  
		tris.push_back({ this, Vector(0.0f, 1.0f, 0.0f, 1.0f), Vector(0.0f, 1.0f, 1.0f, 1.0f), Vector(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f) });
		tris.push_back({ this, Vector(0.0f, 1.0f, 0.0f, 1.0f), Vector(1.0f, 1.0f, 1.0f, 1.0f), Vector(1.0f, 1.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f, 1.0f) });
						 
		tris.push_back({ this, Vector(1.0f, 0.0f, 1.0f, 1.0f), Vector(0.0f, 0.0f, 1.0f, 1.0f), Vector(0.0f, 0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f) });
		tris.push_back({ this, Vector(1.0f, 0.0f, 1.0f, 1.0f), Vector(0.0f, 0.0f, 0.0f, 1.0f), Vector(1.0f, 0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f, 1.0f) });
	}

	// Move object in world space (to test multiple objects)
	void updatePosition(const float fTheta)
	{
		Matrix4x4 matrixRotX, matrixRotZ, matrixTranslation;

		matrixRotX.MakeRotationX(fTheta);
		matrixRotZ.MakeRotationZ(fTheta * 0.5f);
		matrixTranslation.MakeTranslation(x, y, z);
		matrixWorldPos.MakeIdentity();
		matrixWorldPos = matrixRotZ * matrixRotX;
		matrixWorldPos *= matrixTranslation;
	}

	void setPos(float x_, float y_, float z_)
	{
		x = x_;
		y = y_;
		z = z_;
	}
};

// https://stackoverflow.com/questions/49215933/reading-a-monochrome-bitmap-in-c-requires-reading-every-other-line

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

struct Text2D
{
	int32 width;
	int32 height;
	uint8* map;
	int32 linesize;

	~Text2D()
	{
		delete[] map;
		map = nullptr;
	}

	//bool getBlackAndWhiteBmp(const char* filename) {
	//	BmpHeader head;
	//	std::ifstream f(filename, std::ios::binary);

	//	if (!f) {
	//		throw "Invalid file given";
	//	}

	//	int headSize = sizeof(BmpHeader);
	//	f.read((char*)& head, headSize);

	//	if (head.bitsPerPixel != 1) {
	//		f.close();
	//		throw "Invalid bitmap loaded";
	//	}

	//	height = head.height;
	//	width = head.width;

	//	// Lines are aligned on a 4-byte boundary
	//	int lineSize = (width / 8 + (width / 8) % 4);
	//	int fileSize = lineSize * height;

	//	std::vector<uint8> rawFile(fileSize);
	//	std::vector<std::vector<int32>> img(head.height, std::vector<int32>(width, -1));

	//	// Skip to where the actual image data is
	//	f.seekg(head.offset);

	//	// Read in all of the file
	//	f.read((int8*)& rawFile[0], fileSize);

	//	//// Decode the actual boolean values of the pixesl
	//	//int row;
	//	//int reverseRow; // Because bitmaps are stored bottom to top for some reason
	//	//int columnByte;
	//	//int columnBit;
	//	//
	//	//for (row = 0, reverseRow = height - 1; row < height; ++row, --reverseRow) {
	//	//	columnBit = 0;
	//	//	for (columnByte = 0; columnByte < ceil((width / 8.0)); ++columnByte) {
	//	//		int rawPos = (row * lineSize) + columnByte;
	//	//
	//	//		for (int k = 7; k >= 0 && columnBit < width; --k, ++columnBit) {
	//	//			img[reverseRow][columnBit] = (rawFile[rawPos] >> k) & 1;
	//	//		}
	//	//	}
	//	//}
	//	// Decode the actual boolean values of the pixesl
	//	int row;
	//	int columnByte;
	//	int columnBit;

	//	for (row = 0; row < height; ++row)
	//	{
	//		columnBit = 0;
	//		for (columnByte = 0; columnByte < ceil((width / 8.0)); ++columnByte)
	//		{
	//			int rawPos = (row * lineSize) + columnByte;

	//			for (int k = 7; k >= 0 && columnBit < width; --k, ++columnBit)
	//			{
	//				img[row][columnBit] = (rawFile[rawPos] >> k) & 1;
	//			}
	//		}
	//	}

	//	f.close();
	//	image = img;
	//}

	bool test(const char* filename)
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
		map = new uint8[filesize];

		//read color table
		uint32_t color0;
		uint32_t color1;
		uint32_t colortable[2];
		f.seekg(54);
		f.read((int8*)& colortable[0], 4);
		f.read((int8*)& colortable[1], 4);
		printf("colortable: 0x%06X 0x%06X\n", colortable[0], colortable[1]);

		f.seekg(head.offset);
		f.read((int8*)& map[0], filesize);

		for (int y = height - 1; y >= 0; y--)
		{
			for (int x = 0; x < width; x++)
			{
				int pos = y * linesize + x / 8;
				int bit = 1 << (7 - x % 8);
				int v = (map[pos] & bit) > 0;
				printf("%d", v);
			}
			printf("\n");
		}

		f.close();
		return true;
	}

	void LoopUp(const int32 x, const int32 y)
	{
		
	}
};