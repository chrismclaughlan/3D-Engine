#pragma once
#include "types.h"
#include <strstream>
#include <assert.h>

/**
 * \brief Struct containing XY in integer from.
 */
struct Vec2
{
	int x;
	int y;
};

/**
 * \brief Struct containing XY in float from.
 */
struct Vec2f
{
	float x;
	float y;

	Vec2f();
	Vec2f(const float x, const float y);

	friend Vec2f operator+(const Vec2f& a, const Vec2f& b);
	friend Vec2f operator-(const Vec2f& a, const Vec2f& b);
};

/**
 * \brief Struct containing XYZ in integer from.
 */
struct Vec3
{
	int x;
	int y;
	int z;
};

/**
 * \brief Struct containing XYZ or UVW in float from.
 * 
 * Union ensures that values can be read with either UVW or XYZ form,
 * accomodating for use with texture coordinates.
 */
struct Vec3f
{
	union
	{
		struct
		{
			float x, y, z;
		};

		struct
		{
			float u, v, w;
		};
	};

	Vec3f();
	Vec3f(const float u, const float v);
	Vec3f(const float u, const float v, const float w);
};

/**
 * \brief Struct containing XYZW in float from and suitable mathematical 
 * operations.
 */
struct Vec4f
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;

	Vec4f();
	Vec4f(float x, float y, float z);
	Vec4f(float x, float y, float z, float w);

	friend std::ostream& operator<<(std::ostream& os, const Vec4f& v);

	friend bool operator==(const Vec4f& a, const Vec4f& b);
	friend Vec4f operator+(const Vec4f& a, const Vec4f& b);
	friend Vec4f operator-(const Vec4f& a, const Vec4f& b);
	friend Vec4f operator*(const Vec4f& a, const float k);
	friend Vec4f operator*(const float k, Vec4f& a) { return a * k; };
	friend Vec4f operator/(const Vec4f& a, const float k);
	friend Vec4f operator+=(Vec4f& a, const Vec4f& b);
	friend Vec4f operator-=(Vec4f& a, const Vec4f& b);
	friend Vec4f operator*=(Vec4f& a, const float k);
	friend Vec4f operator/=(Vec4f& a, const float k);
	Vec4f& operator-();

	static float DotProduct(const Vec4f& a, const Vec4f& b);
	static float Length(const Vec4f& a);
	void Normalise();
	static Vec4f Normalise(const Vec4f& a);
	static Vec4f CrossProduct(const Vec4f& a, const Vec4f& b);

	static Vec4f IntersectPlane(Vec4f& plane_p, Vec4f& plane_n, Vec4f& lineStart, Vec4f& lineEnd, float& t);
	//static int TriangleClipAgainstPlane(Vec4f plane_p, Vec4f plane_n, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2);

	void setZero();
};


/**
 * \brief Struct containing 4x4 matrix and suitable mathematical operations.
 */
struct Matrix4x4
{
	float m[4][4];

	Matrix4x4();

	friend Vec4f operator*(const Matrix4x4& m, const Vec4f& v);
	friend Vec4f operator*=(const Matrix4x4& m, const Vec4f& v);
	friend Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b);
	friend Matrix4x4 operator*=(Matrix4x4& a, const Matrix4x4& b);

	// Returns an identity matrix TODO google
	void MakeIdentity();
	void MakeRotationX(float fRad);
	void MakeRotationY(float fRad);
	void MakeRotationZ(float fRad);
	void MakeTranslation(float x, float y, float z);
	void MakeProjection(float fFov, float fAspectRatio, float fNear, float fFar);
	void MakePointAt(const Vec4f& pos, const Vec4f& target, const Vec4f& up);
	void MakeQuickInverse();
};
