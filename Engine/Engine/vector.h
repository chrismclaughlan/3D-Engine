#pragma once
#include <iostream>

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
};