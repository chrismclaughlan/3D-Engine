#include "vector.h"
#include <assert.h>

Vector::Vector()
	: x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{}

Vector::Vector(float x, float y, float z)
	: x(x), y(y), z(z), w(0.0f)
{}

Vector::Vector(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w)
{}

std::ostream& operator<<(std::ostream& os, const Vector& v)
{
	os << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
	return os;
}

bool operator==(const Vector& a, const Vector& b)
{
	return (a.x == b.x && a.y == b.y && a.z == b.z);
}

Vector operator+(const Vector& a, const Vector& b)
{
	Vector v;
	v.x = a.x + b.x;
	v.y = a.y + b.y;
	v.z = a.z + b.z;
	return v;
}

Vector operator-(const Vector& a, const Vector& b)
{
	Vector v;
	v.x = a.x - b.x;
	v.y = a.y - b.y;
	v.z = a.z - b.z;
	return v;
}

Vector operator*(const Vector& a, const float k)
{
	Vector v;
	v.x = a.x * k;
	v.y = a.y * k;
	v.z = a.z * k;
	return v;
}

Vector operator/(const Vector& a, const float k)
{
	assert(k != 0);
	Vector v;
	v.x = a.x / k;
	v.y = a.y / k;
	v.z = a.z / k;
	return v;
}

Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b)
{
	Matrix4x4 mat;
	for (int c = 0; c < 4; c++)
	{
		for (int r = 0; r < 4; r++)
		{
			mat.m[r][c] = (a.m[r][0] * b.m[0][c]) + (a.m[r][1] * b.m[1][c]) + (a.m[r][2] * b.m[2][c]) + (a.m[r][3] * b.m[3][c]);
		}
	}
	return mat;
}

float Vector::DotProduct(const Vector& a, const Vector& b) const
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float Vector::Length(const Vector& a) const
{
	return sqrtf(DotProduct(a, a));
}

Vector Vector::Normalise(const Vector& a) const
{
	Vector v;
	float length = Length(a);
	assert(length != 0);	
	v.x = a.x / length;
	v.y = a.y / length;
	v.z = a.z / length;
	return v;
}

Vector Vector::CrossProduct(const Vector& a, const Vector& b) const
{
	Vector v;
	v.x = (a.y * b.z) - (a.z * b.y);
	v.y = (a.z * b.x) - (a.x * b.z);
	v.z = (a.x * b.y) - (a.y * b.x);
	return v;
}

Matrix4x4::Matrix4x4()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = 0;
		}
	}
}

Vector operator*(const Matrix4x4 m, const Vector& v)
{
	Vector vect;
	vect.x = (v.x * m.m[0][0]) + (v.y * m.m[1][0]) + (v.z * m.m[2][0]) + (v.w * m.m[3][0]);
	vect.y = (v.x * m.m[0][1]) + (v.y * m.m[1][1]) + (v.z * m.m[2][1]) + (v.w * m.m[3][1]);
	vect.z = (v.x * m.m[0][2]) + (v.y * m.m[1][2]) + (v.z * m.m[2][2]) + (v.w * m.m[3][2]);
	vect.w = (v.x * m.m[0][3]) + (v.y * m.m[1][3]) + (v.z * m.m[2][3]) + (v.w * m.m[3][3]);
	return vect;
}

//Vector Matrix4x4::MultiplyVector(Matrix4x4& m, Vector& v) const
//{
//	Vector vect;
//	vect.x = (v.x * m.m[0][0]) + (v.y * m.m[1][0]) + (v.z * m.m[2][0]) + (v.w * m.m[3][0]);
//	vect.y = (v.x * m.m[0][1]) + (v.y * m.m[1][1]) + (v.z * m.m[2][1]) + (v.w * m.m[3][1]);
//	vect.z = (v.x * m.m[0][2]) + (v.y * m.m[1][2]) + (v.z * m.m[2][2]) + (v.w * m.m[3][2]);
//	vect.w = (v.x * m.m[0][3]) + (v.y * m.m[1][3]) + (v.z * m.m[2][3]) + (v.w * m.m[3][3]);
//	return vect;
//}

void Matrix4x4::MakeIdentity()
{
	m[0][0] = 1.0f;
	m[1][1] = 1.0f;
	m[2][2] = 1.0f;
	m[3][3] = 1.0f;
}

void Matrix4x4::MakeRotationX(float fRad)
{
	m[0][0] = 1.0f;
	m[1][1] = cosf(fRad);
	m[1][2] = sinf(fRad);
	m[2][1] = -sinf(fRad);
	m[2][2] = cosf(fRad);
	m[3][3] = 1.0f;
}
void Matrix4x4::MakeRotationY(float fRad)
{
	m[0][0] = cosf(fRad);
	m[0][2] = sinf(fRad);
	m[2][0] = -sinf(fRad);
	m[1][1] = 1.0f;
	m[2][2] = cosf(fRad);
	m[3][3] = 1.0f;
}
void Matrix4x4::MakeRotationZ(float fRad)
{
	m[0][0] = cosf(fRad);
	m[0][1] = sinf(fRad);
	m[1][0] = -sinf(fRad);
	m[1][1] = cosf(fRad);
	m[2][2] = 1.0f;
	m[3][3] = 1.0f;
}

void Matrix4x4::MakeTranslation(float x, float y, float z)
{
	m[0][0] = 1.0f;
	m[1][1] = 1.0f;
	m[2][2] = 1.0f;
	m[3][3] = 1.0f;
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
}

void Matrix4x4::MakeProjection(float fFovDeg, float fAspectRatio, float fNear, float fFar)
{
	float fFovRad = 1.0f / tanf(fFovDeg * 0.5f / 180.f * 3.14159f);
	m[0][0] = fAspectRatio * fFovRad;
	m[1][1] = fFovRad;
	m[2][2] = fFar / (fFar - fNear);
	m[3][2] = (-fFar * fNear) / (fFar - fNear);
	m[2][3] = 1.0f;
	m[3][3] = 0.0f;
}
