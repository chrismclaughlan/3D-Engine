#include "vector.h"
#include <assert.h>

#define SHOW_CLIPPING false

Vector::Vector()
	: x(0.0f), y(0.0f), z(0.0f), w(1.0f)
{}

Vector::Vector(float x, float y, float z)
	: x(x), y(y), z(z), w(1.0f)
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

Vector operator+=(Vector& a, const Vector& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

Vector operator-=(Vector& a, const Vector& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

Vector operator*=(Vector& a, const float k)
{
	a.x *= k;
	a.y *= k;
	a.z *= k;
	return a;
}

Vector operator/=(Vector& a, const float k)
{
	assert(k != 0);
	a.x /= k;
	a.y /= k;
	a.z /= k;
	return a;
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

Vector& Vector::operator-()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

float Vector::DotProduct(const Vector& a, const Vector& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float Vector::Length(const Vector& a)
{
	return sqrtf(DotProduct(a, a));
}

void Vector::Normalise()
{
	float length = Length(*this);
	if (length == 0)
		return;
	assert(length != 0);
	x /= length;
	y /= length;
	z /= length;
}

Vector Vector::Normalise(const Vector& a)
{
	Vector v;
	float length = Length(a);
	if (length == 0)
		return Vector();
	assert(length != 0);
	v.x = a.x / length;
	v.y = a.y / length;
	v.z = a.z / length;
	return v;
}

Vector Vector::CrossProduct(const Vector& a, const Vector& b)
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

Vector operator*(const Matrix4x4& m, const Vector& v)
{
	Vector vect;
	vect.x = (v.x * m.m[0][0]) + (v.y * m.m[1][0]) + (v.z * m.m[2][0]) + (v.w * m.m[3][0]);
	vect.y = (v.x * m.m[0][1]) + (v.y * m.m[1][1]) + (v.z * m.m[2][1]) + (v.w * m.m[3][1]);
	vect.z = (v.x * m.m[0][2]) + (v.y * m.m[1][2]) + (v.z * m.m[2][2]) + (v.w * m.m[3][2]);
	vect.w = (v.x * m.m[0][3]) + (v.y * m.m[1][3]) + (v.z * m.m[2][3]) + (v.w * m.m[3][3]);
	return vect;
}

Vector operator*=(const Matrix4x4& m, const Vector& v)
{
	Vector vect;
	vect.x = (v.x * m.m[0][0]) + (v.y * m.m[1][0]) + (v.z * m.m[2][0]) + (v.w * m.m[3][0]);
	vect.y = (v.x * m.m[0][1]) + (v.y * m.m[1][1]) + (v.z * m.m[2][1]) + (v.w * m.m[3][1]);
	vect.z = (v.x * m.m[0][2]) + (v.y * m.m[1][2]) + (v.z * m.m[2][2]) + (v.w * m.m[3][2]);
	vect.w = (v.x * m.m[0][3]) + (v.y * m.m[1][3]) + (v.z * m.m[2][3]) + (v.w * m.m[3][3]);
	return vect;
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

void Matrix4x4::MakePointAt(const Vector& pos, const Vector& target, const Vector& up)
{
	// Calculate new forward direction
	Vector newForward = target - pos;
	newForward.Normalise();

	// Calculate new Up direction
	Vector a = newForward * (Vector::DotProduct(up, newForward));
	Vector newUp = up - a;
	newUp.Normalise();

	// New Right direction is easy, its just cross product
	Vector newRight = Vector::CrossProduct(newUp, newForward);

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

Vector Vector::IntersectPlane(Vector& plane_p, Vector& plane_n, Vector& lineStart, Vector& lineEnd)
{
	plane_n.Normalise();
	float plane_d = -Vector::DotProduct(plane_n, plane_p);
	float ad = Vector::DotProduct(lineStart, plane_n);
	float bd = Vector::DotProduct(lineEnd, plane_n);
	float t = (-plane_d - ad) / (bd - ad);
	Vector lineStartToEnd = lineEnd - lineStart;
	Vector lineToIntersect = lineStartToEnd * t;
	return lineStart + lineToIntersect;
}

int Vector::TriangleClipAgainstPlane(Vector plane_p, Vector plane_n, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2)
{
	// Make sure plane normal is indeed normal
	plane_n.Normalise();

	// Return signed shortest distance from point to plane, plane normal must be normalised
	auto dist = [&](Vector& p)
	{
		Vector n = p;
		n.Normalise();
		return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vector::DotProduct(plane_n, plane_p));
	};

	// Create two temporary storage arrays to classify points either side of plane
	// If distance sign is positive, point lies on "inside" of plane
	Vector* inside_points[3];  int nInsidePointCount = 0;
	Vector* outside_points[3]; int nOutsidePointCount = 0;

	// Get signed distance of each point in triangle to plane
	float d0 = dist(in_tri.points[0]);
	float d1 = dist(in_tri.points[1]);
	float d2 = dist(in_tri.points[2]);

	if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.points[0]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.points[0]; }
	if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.points[1]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.points[1]; }
	if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.points[2]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.points[2]; }

	// Now classify triangle points, and break the input triangle into 
	// smaller output triangles if required. There are four possible
	// outcomes...

	if (nInsidePointCount == 0)
	{
		// All points lie on the outside of plane, so clip whole triangle
		// It ceases to exist

		return 0; // No returned triangles are valid
	}

	if (nInsidePointCount == 3)
	{
		// All points lie on the inside of plane, so do nothing
		// and allow the triangle to simply pass through
		out_tri1 = in_tri;

		return 1; // Just the one returned original triangle is valid
	}

	if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{
		// Triangle should be clipped. As two points lie outside
		// the plane, the triangle simply becomes a smaller triangle

		// Copy appearance info to new triangle
#if SHOW_CLIPPING
		out_tri1.colour = 0x0000ff;
#else
		out_tri1.colour = in_tri.colour;
#endif

		// The inside point is valid, so keep that...
		out_tri1.points[0] = *inside_points[0];

		// but the two new points are at the locations where the 
		// original sides of the triangle (lines) intersect with the plane
		out_tri1.points[1] = Vector::IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
		out_tri1.points[2] = Vector::IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

		return 1; // Return the newly formed single triangle
	}

	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{
		// Triangle should be clipped. As two points lie inside the plane,
		// the clipped triangle becomes a "quad". Fortunately, we can
		// represent a quad with two new triangles

		// Copy appearance info to new triangles
#if SHOW_CLIPPING
		out_tri1.colour = 0xff0000;
		out_tri2.colour = 0x00ff00;
#else
		out_tri1.colour = in_tri.colour;
		out_tri2.colour = in_tri.colour;
#endif

		// The first triangle consists of the two inside points and a new
		// point determined by the location where one side of the triangle
		// intersects with the plane
		out_tri1.points[0] = *inside_points[0];
		out_tri1.points[1] = *inside_points[1];
		out_tri1.points[2] = Vector::IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

		// The second triangle is composed of one of he inside points, a
		// new point determined by the intersection of the other side of the 
		// triangle and the plane, and the newly created point above
		out_tri2.points[0] = *inside_points[1];
		out_tri2.points[1] = out_tri1.points[2];
		out_tri2.points[2] = Vector::IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

		return 2; // Return two newly formed triangles which form a quad
	}
}