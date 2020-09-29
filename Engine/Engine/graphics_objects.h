/*****************************************************************//**
 * \file   graphics_objects.h
 * \brief  Contains Triangle struct & Object class to describe 
 * 3D objects in application.
 * 
 * \author Chris
 * \date   September 2020
 *********************************************************************/

#pragma once
#include "utils.h"
#include "types.h"
#include "utils_vector.h"
#include "graphics_texture.h"
#include <vector>
#include <ostream>

class Object;

/**
 * \brief Contains positional, texture and colour info.
 */
struct Triangle
{
	const Object* parent = nullptr;	///< Pointer to parent.
									///< Mainly used to get parent's texture
	Vec4f p[3];						///< Triangle coords as 4D vector
	Vec3f t[3];						///< Texture coords as 3D vector
	colour_t colour;				///< Triangle colour.
									///< Used for solid/untextured triangles
	bool hit = false;  // test
};

// TODO move?
extern int TriangleClipAgainstPlane(Vec4f plane_p, Vec4f plane_n, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2);


enum class ObjectFace : uint8
{
	Front	= 1 << 0, 
	Back	= 1 << 1,
	Left	= 1 << 2,
	Right	= 1 << 3,
	Top		= 1 << 4,
	Bottom	= 1 << 5,
};


struct Face
{
	std::vector<Triangle> vTris;
	bool draw = false;
};


/**
 * \brief Class representing an object in the application world.
 * 
 * \note This object does not require a texture
 */
class Object
{
public:
	std::string name;
	Matrix4x4 matrixWorldPos;		///< Contains info on object in world
	std::vector<Triangle> vTris;	///< Contains vector of triangles that
									///< compose the object.
	Texture* pTexture = nullptr;	///< Pointer to a texture (optional)
	Vec3f vPos;						///< Positional vector.
									///< x: + left		/ - right
									///< y: + upwards	/ - downwards
									///< z: + forwards	/ - backwards
	std::vector<Face> faces;
	void resetFacesDrawable()
	{
		for (auto f : faces)
		{
			f.draw = false;
		}
	}

public:
	/* Loading */
	bool LoadObjectFile(std::string filename, bool hasTexture);
	void LoadTestCube(std::string objectName);

	/* Updating */
	void updatePosition(const float fTheta);
	void setPos(float x, float y, float z);

	friend std::ostream& operator<<(std::ostream& os, const Object& o)
	{
		os << o.name;
		return os;
	}
};


struct ObjectHit
{
	Triangle* triangleHit = nullptr;
	Object* objectHit = nullptr;
	float fFistanceFromCamera;
	Vec4f vPoint;
	Vec4f vNormal;
};