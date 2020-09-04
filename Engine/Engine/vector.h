#pragma once
#include <iostream>
#include <fstream>
#include <strstream>
#include <vector>
#include "types.h"

struct Triangle;

class Vector
{
public:
	float x;
	float y;
	float z;
	float w;

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

	static Vector IntersectPlane(Vector& plane_p, Vector& plane_n, Vector& lineStart, Vector& lineEnd);
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
	int32 colour = 0x0000ff;
};

struct mesh
{
	std::vector<Triangle> tris;

	// TODO error handling
	bool LoadObjectFile(std::string filename)
	{
		std::ifstream file(filename);

		if (!file.is_open())
		{
			return false;
		}

		std::vector<Vector> verticies;

		while (!file.eof())
		{
			// assuming line <= 128
			char line[128];
			file.getline(line, 128);

			std::strstream s;
			s << line;

			char temp;  // hold junk

			if (line[0] == 'v' && line[1] == ' ')
			{
				Vector v;
				s >> temp >> v.x >> v.y >> v.z;
				verticies.push_back(v);
			}

			if (line[0] == 'f' && line[1] == ' ')
			{
				int f[3];
				s >> temp >> f[0] >> f[1] >> f[2];
				tris.push_back({ verticies[f[0] - 1], verticies[f[1] - 1], verticies[f[2] - 1] });
			}
		}

		return true;
	}
};
