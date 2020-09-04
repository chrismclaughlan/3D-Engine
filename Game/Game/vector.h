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
	float DotProduct(const Vector& a, const Vector& b) const;
	float Length(const Vector& a) const;
	Vector Normalise(const Vector& a) const;
	Vector CrossProduct(const Vector& a, const Vector& b) const;
};

class Matrix4x4
{
public:
	float m[4][4];
public:
	Matrix4x4();
	//Matrix4x4( ... )

	friend Vector operator*(const Matrix4x4& m, const Vector& v);
	friend Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b);
	//Vector MultiplyVector(Matrix4x4& m, Vector& v) const;

	// Returns an identity matrix TODO google
	void MakeIdentity();
	void MakeRotationX(float fRad);
	void MakeRotationY(float fRad);
	void MakeRotationZ(float fRad);
	void MakeTranslation(float x, float y, float z);
	void MakeProjection(float fFov, float fAspectRatio, float fNear, float fFar);
};