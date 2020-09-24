/*****************************************************************//**
 * \file   objects.h
 * \brief  Contains Triangle struct & Object class to describe 
 * 3D objects in application.
 * 
 * \author Chris
 * \date   September 2020
 *********************************************************************/

#pragma once
#include "utils.h"
#include "types.h"
#include "vector.h"
#include "graphics_texture.h"

/**
 * \brief Contains positional, texture and colour info.
 */
struct Triangle
{
	const Object* parent = nullptr;	///< Pointer to parent.
									///< Mainly used to get parent's texture
	Vector p[3];					///< Triangle coords as 4D vector
	Vector2 t[3];					///< Texture coords as 3D vector
	t_colour colour;				///< Triangle colour.
									///< Used for solid/untextured triangles
};

// TODO move?
extern int TriangleClipAgainstPlane(Vector plane_p, Vector plane_n, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2);


/**
 * \brief Class representing an object in the application world.
 * 
 * \note This object does not require a texture
 */
class Object
{
public:
	Matrix4x4 matrixWorldPos;		///< Contains info on object in world
	std::vector<Triangle> vTris;	///< Contains vector of triangles that
									///< compose the object.
	Texture* pTexture = nullptr;	///< Pointer to a texture (optional)
	vec3f vPos;						///< Positional vector.
									///< x: + left		/ - right
									///< y: + upwards	/ - downwards
									///< z: + forwards	/ - backwards

public:
	/* Loading */
	bool LoadObjectFile(std::string filename, bool hasTexture);
	void LoadTestCube();

	/* Updating */
	void updatePosition(const float fTheta);
	void setPos(float x, float y, float z);
};
