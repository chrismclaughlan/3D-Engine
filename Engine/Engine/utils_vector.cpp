#include "utils_vector.h"
#include <assert.h>
#include <tgmath.h>

/**
 * \brief Boolean deciding whether triangle clipping is drawn.
 * 
 * Used for debug purposes to check if clipping is working correctly.
 */
#define SHOW_CLIPPING false


/* Vec2f */

Vec2f::Vec2f()
	: x(0.0f), y(0.0f)
{}

Vec2f::Vec2f(const float x, const float y)
	: x(x), y(y)
{};

Vec2f operator+(const Vec2f& a, const Vec2f& b)
{
	Vec2f v;
	v.x = a.x + b.x;
	v.y = a.y + b.y;
	return v;
}

Vec2f operator-(const Vec2f& a, const Vec2f& b)
{
	Vec2f v;
	v.x = a.x - b.x;
	v.y = a.y - b.y;
	return v;
}


/* Vec3f */

Vec3f::Vec3f()
	: u(0.0f), v(0.0f), w(1.0f)
{};

Vec3f::Vec3f(const float u, const float v)
	: u(u), v(v), w(1.0f)
{};

Vec3f::Vec3f(const float u, const float v, const float w)
	: u(u), v(v), w(w)
{};


/* Vec4f */

Vec4f::Vec4f()
	: x(0.0f), y(0.0f), z(0.0f), w(1.0f)
{}

Vec4f::Vec4f(float x, float y, float z)
	: x(x), y(y), z(z), w(1.0f)
{}

Vec4f::Vec4f(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w)
{}

std::ostream& operator<<(std::ostream& os, const Vec4f& v)
{
	os << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
	return os;
}

bool operator==(const Vec4f& a, const Vec4f& b)
{
	return (a.x == b.x && a.y == b.y && a.z == b.z);
}

Vec4f operator+(const Vec4f& a, const Vec4f& b)
{
	Vec4f v;
	v.x = a.x + b.x;
	v.y = a.y + b.y;
	v.z = a.z + b.z;
	return v;
}

Vec4f operator-(const Vec4f& a, const Vec4f& b)
{
	Vec4f v;
	v.x = a.x - b.x;
	v.y = a.y - b.y;
	v.z = a.z - b.z;
	return v;
}

Vec4f operator*(const Vec4f& a, const float k)
{
	Vec4f v;
	v.x = a.x * k;
	v.y = a.y * k;
	v.z = a.z * k;
	return v;
}

Vec4f operator/(const Vec4f& a, const float k)
{
	assert(k != 0);
	Vec4f v;
	v.x = a.x / k;
	v.y = a.y / k;
	v.z = a.z / k;
	return v;
}

Vec4f operator+=(Vec4f& a, const Vec4f& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

Vec4f operator-=(Vec4f& a, const Vec4f& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

Vec4f operator*=(Vec4f& a, const float k)
{
	a.x *= k;
	a.y *= k;
	a.z *= k;
	return a;
}

Vec4f operator/=(Vec4f& a, const float k)
{
	assert(k != 0);
	a.x /= k;
	a.y /= k;
	a.z /= k;
	return a;
}

Vec4f& Vec4f::operator-()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

float Vec4f::DotProduct(const Vec4f& a, const Vec4f& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float Vec4f::Length(const Vec4f& a)
{
	return sqrtf(DotProduct(a, a));
}

float Vec4f::Distance(const Vec4f& a, const Vec4f& b)
{
	return sqrtf(pow(b.x - a.x, 2.0f) + pow(b.y - a.y, 2.0f) + pow(b.z - a.z, 2.0f));
}

void Vec4f::Normalise()
{
	float length = Length(*this);
	if (length == 0)
		return;
	assert(length != 0);
	x /= length;
	y /= length;
	z /= length;
}

Vec4f Vec4f::Normalise(const Vec4f& a)
{
	Vec4f v;
	float length = Length(a);
	if (length == 0)
		return Vec4f();
	assert(length != 0);
	v.x = a.x / length;
	v.y = a.y / length;
	v.z = a.z / length;
	return v;
}

Vec4f Vec4f::CrossProduct(const Vec4f& a, const Vec4f& b)
{
	Vec4f v;
	v.x = (a.y * b.z) - (a.z * b.y);
	v.y = (a.z * b.x) - (a.x * b.z);
	v.z = (a.x * b.y) - (a.y * b.x);
	return v;
}

Vec4f operator*(const Matrix4x4& m, const Vec4f& v)
{
	Vec4f vect;
	vect.x = (v.x * m.m[0][0]) + (v.y * m.m[1][0]) + (v.z * m.m[2][0]) + (v.w * m.m[3][0]);
	vect.y = (v.x * m.m[0][1]) + (v.y * m.m[1][1]) + (v.z * m.m[2][1]) + (v.w * m.m[3][1]);
	vect.z = (v.x * m.m[0][2]) + (v.y * m.m[1][2]) + (v.z * m.m[2][2]) + (v.w * m.m[3][2]);
	vect.w = (v.x * m.m[0][3]) + (v.y * m.m[1][3]) + (v.z * m.m[2][3]) + (v.w * m.m[3][3]);
	return vect;
}

Vec4f operator*=(const Matrix4x4& m, const Vec4f& v)
{
	Vec4f vect;
	vect.x = (v.x * m.m[0][0]) + (v.y * m.m[1][0]) + (v.z * m.m[2][0]) + (v.w * m.m[3][0]);
	vect.y = (v.x * m.m[0][1]) + (v.y * m.m[1][1]) + (v.z * m.m[2][1]) + (v.w * m.m[3][1]);
	vect.z = (v.x * m.m[0][2]) + (v.y * m.m[1][2]) + (v.z * m.m[2][2]) + (v.w * m.m[3][2]);
	vect.w = (v.x * m.m[0][3]) + (v.y * m.m[1][3]) + (v.z * m.m[2][3]) + (v.w * m.m[3][3]);
	return vect;
}

Vec4f Vec4f::IntersectPlane(Vec4f& plane_p, Vec4f& plane_n, Vec4f& lineStart, Vec4f& lineEnd, float& t)
{
	float plane_d = -Vec4f::DotProduct(plane_n, plane_p);
	float ad = Vec4f::DotProduct(lineStart, plane_n);
	float bd = Vec4f::DotProduct(lineEnd, plane_n);
	t = (-plane_d - ad) / (bd - ad);
	Vec4f lineStartToEnd = lineEnd - lineStart;
	Vec4f lineToIntersect = lineStartToEnd * t;
	return lineStart + lineToIntersect;
}

void Vec4f::setZero()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}


/* Matrix4x4 */

Matrix4x4::Matrix4x4()
{
	memset(&m, 0, sizeof(float) * 4 * 4);
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

Matrix4x4 operator*=(Matrix4x4& a, const Matrix4x4& b)
{
	for (int c = 0; c < 4; c++)
	{
		for (int r = 0; r < 4; r++)
		{
			a.m[r][c] = (a.m[r][0] * b.m[0][c]) + (a.m[r][1] * b.m[1][c]) + (a.m[r][2] * b.m[2][c]) + (a.m[r][3] * b.m[3][c]);
		}
	}
	return a;
}

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

void Matrix4x4::MakePointAt(const Vec4f& pos, const Vec4f& target, const Vec4f& up)
{
	// Calculate new forward direction
	Vec4f newForward = target - pos;
	newForward.Normalise();

	// Calculate new Up direction
	Vec4f a = newForward * (Vec4f::DotProduct(up, newForward));
	Vec4f newUp = up - a;
	newUp.Normalise();

	// New Right direction is easy, its just cross product
	Vec4f newRight = Vec4f::CrossProduct(newUp, newForward);

	// Construct Dimensioning and Translation Matrix	
	m[0][0] = newRight.x;		m[0][1] = newRight.y;		m[0][2] = newRight.z;		m[0][3] = 0.0f;
	m[1][0] = newUp.x;			m[1][1] = newUp.y;			m[1][2] = newUp.z;			m[1][3] = 0.0f;
	m[2][0] = newForward.x;		m[2][1] = newForward.y;		m[2][2] = newForward.z;		m[2][3] = 0.0f;
	m[3][0] = pos.x;			m[3][1] = pos.y;			m[3][2] = pos.z;			m[3][3] = 1.0f;
}

void Matrix4x4::MakeQuickInverse() // Only for Rotation/Translation Matrices
{
	Matrix4x4 matrix;
	matrix.m[0][0] = m[0][0]; matrix.m[0][1] = m[1][0]; matrix.m[0][2] = m[2][0]; matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = m[0][1]; matrix.m[1][1] = m[1][1]; matrix.m[1][2] = m[2][1]; matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = m[0][2]; matrix.m[2][1] = m[1][2]; matrix.m[2][2] = m[2][2]; matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = -(m[3][0] * matrix.m[0][0] + m[3][1] * matrix.m[1][0] + m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m[3][0] * matrix.m[0][1] + m[3][1] * matrix.m[1][1] + m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m[3][0] * matrix.m[0][2] + m[3][1] * matrix.m[1][2] + m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;

	*this = matrix;
}
