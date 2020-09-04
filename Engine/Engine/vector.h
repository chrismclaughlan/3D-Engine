#pragma once
#include <iostream>
#include <fstream>
#include <strstream>
#include <vector>
#include <iostream>
#include <string>
#include "types.h"

struct Triangle;

// TODO
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

	void reset();
};

class Matrix4x4
{
public:
	float m[4][4];
public:
	Matrix4x4();
	//Matrix4x4( ... )

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
	Vector points[3];
	Vector2 t[3];
	int32 colour = 0x0000ff;
};

class Texture
{
public:
	static const int32 sizeX = 4;
	static const int32 sizeY = 4;
	uint16 map[sizeX][sizeY];

public:
	Texture()
	{
		map[0][0] = 0xff0000; map[0][1] = 0xff0000; map[0][2] = 0xff0000; map[0][3] = 0xff0000;
		map[1][0] = 0xff0000; map[1][1] = 0xff0000; map[1][2] = 0xff0000; map[1][3] = 0xffffff;
		map[2][0] = 0xff0000; map[2][1] = 0xff0000; map[2][2] = 0xffffff; map[2][3] = 0xffffff;
		map[3][0] = 0xff0000; map[3][1] = 0xffffff; map[3][2] = 0xffffff; map[3][3] = 0xffffff;
		//for (int i = 0; i < 10; i++)
		//{
		//	for (int j = 0; j < 10; j++)
		//	{
		//		//test
		//		if (j < 5)
		//			map[j][i] = (uint16)0x00ff00;
		//		else
		//			map[j][i] = (uint16)0xff00ff;
		//	}
		//}
	}

	uint16 lookUp(float x, float y)
	{
		int32 xIndex = x * sizeX;
		int32 yIndex = y * sizeY;
		return map[yIndex][xIndex];
	}
};

struct mesh
{
	std::vector<Triangle> tris;

	// TODO error handling
	bool LoadObjectFile(std::string filename, bool hasTexture)
	{
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
					tris.push_back({ verticies[f[0] - 1], verticies[f[1] - 1], verticies[f[2] - 1] });
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

					tris.push_back({ Vector(verticies[std::stoi(tokens[0]) - 1]), Vector(verticies[std::stoi(tokens[2]) - 1]), Vector(verticies[std::stoi(tokens[4]) - 1]), Vector2(textures[std::stoi(tokens[1]) - 1]), Vector2(textures[std::stoi(tokens[3]) - 1]), Vector2(textures[std::stoi(tokens[5]) - 1]) });
				}
			}
		}

		return true;
	}
};
