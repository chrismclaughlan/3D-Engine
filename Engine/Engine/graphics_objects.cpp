#include "utils.h"
#include "graphics_objects.h"
#include <fstream>
#include <iostream>

/**
 * \brief Loads .obj file and loads data into vTris.
 * 
 * \param filename Relative filename ending in .obj
 * \param hasTexture Determines how to interpret .obj data
 * \return Returns true if successful, otherwise false
 */
bool Object::LoadObjectFile(std::string filename, bool hasTexture)
{
	if (!stringEndsWith(filename, ".obj"))
	{
		std::cerr
			<< "Error loading: " << filename
			<< " -> File is not an .obj file\n";
		return false;
	}

	vTris.clear();

	std::ifstream file(filename);

	if (!file.is_open())
	{
		std::cerr
			<< "Error loading: " << filename
			<< " -> File is already open\n";
		return false;
	}

	std::vector<Vec4f> verticies;
	std::vector<Vec3f> textures;

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
				Vec3f v;
				s >> temp >> temp >> v.u >> v.v;
				textures.push_back(v);
			}
			else if (line[1] == ' ')
			{
				Vec4f v;
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
				vTris.push_back({ this, verticies[f[0] - 1], verticies[f[1] - 1], verticies[f[2] - 1] });
			}
		}
		else
		{
			// TODO throw exception if file doesnt actually contain texture shit
			if (line[0] == 'f' && line[1] == ' ')
			{
				s >> temp;
				std::string tokens[6];
				int nTokenCount = -1;

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

				vTris.push_back({ this, Vec4f(verticies[std::stoi(tokens[0]) - 1]), Vec4f(verticies[std::stoi(tokens[2]) - 1]), Vec4f(verticies[std::stoi(tokens[4]) - 1]), Vec3f(textures[std::stoi(tokens[1]) - 1]), Vec3f(textures[std::stoi(tokens[3]) - 1]), Vec3f(textures[std::stoi(tokens[5]) - 1]) });
			}
		}
	}

	return true;
}

/**
 * \brief Fills vTris with triangles manually.
 */
void Object::LoadTestCube(std::string objectName)
{
	name = objectName;

	// South
	vTris.clear();
	vTris.push_back({ this, Vec4f(0.0f, 0.0f, 0.0f, 1.0f), Vec4f(0.0f, 1.0f, 0.0f, 1.0f), Vec4f(1.0f, 1.0f, 0.0f, 1.0f), Vec3f(0.0f, 1.0f, 1.0f), Vec3f(0.0f, 0.0f, 1.0f), Vec3f(1.0f, 0.0f, 1.0f) });
	vTris.push_back({ this, Vec4f(0.0f, 0.0f, 0.0f, 1.0f), Vec4f(1.0f, 1.0f, 0.0f, 1.0f), Vec4f(1.0f, 0.0f, 0.0f, 1.0f), Vec3f(0.0f, 1.0f, 1.0f), Vec3f(1.0f, 0.0f, 1.0f), Vec3f(1.0f, 1.0f, 1.0f) });

	vTris.push_back({ this, Vec4f(1.0f, 0.0f, 0.0f, 1.0f), Vec4f(1.0f, 1.0f, 0.0f, 1.0f), Vec4f(1.0f, 1.0f, 1.0f, 1.0f), Vec3f(0.0f, 1.0f, 1.0f), Vec3f(0.0f, 0.0f, 1.0f), Vec3f(1.0f, 0.0f, 1.0f) });
	vTris.push_back({ this, Vec4f(1.0f, 0.0f, 0.0f, 1.0f), Vec4f(1.0f, 1.0f, 1.0f, 1.0f), Vec4f(1.0f, 0.0f, 1.0f, 1.0f), Vec3f(0.0f, 1.0f, 1.0f), Vec3f(1.0f, 0.0f, 1.0f), Vec3f(1.0f, 1.0f, 1.0f) });

	vTris.push_back({ this, Vec4f(1.0f, 0.0f, 1.0f, 1.0f), Vec4f(1.0f, 1.0f, 1.0f, 1.0f), Vec4f(0.0f, 1.0f, 1.0f, 1.0f), Vec3f(0.0f, 1.0f, 1.0f), Vec3f(0.0f, 0.0f, 1.0f), Vec3f(1.0f, 0.0f, 1.0f) });
	vTris.push_back({ this, Vec4f(1.0f, 0.0f, 1.0f, 1.0f), Vec4f(0.0f, 1.0f, 1.0f, 1.0f), Vec4f(0.0f, 0.0f, 1.0f, 1.0f), Vec3f(0.0f, 1.0f, 1.0f), Vec3f(1.0f, 0.0f, 1.0f), Vec3f(1.0f, 1.0f, 1.0f) });

	vTris.push_back({ this, Vec4f(0.0f, 0.0f, 1.0f, 1.0f), Vec4f(0.0f, 1.0f, 1.0f, 1.0f), Vec4f(0.0f, 1.0f, 0.0f, 1.0f), Vec3f(0.0f, 1.0f, 1.0f), Vec3f(0.0f, 0.0f, 1.0f), Vec3f(1.0f, 0.0f, 1.0f) });
	vTris.push_back({ this, Vec4f(0.0f, 0.0f, 1.0f, 1.0f), Vec4f(0.0f, 1.0f, 0.0f, 1.0f), Vec4f(0.0f, 0.0f, 0.0f, 1.0f), Vec3f(0.0f, 1.0f, 1.0f), Vec3f(1.0f, 0.0f, 1.0f), Vec3f(1.0f, 1.0f, 1.0f) });

	vTris.push_back({ this, Vec4f(0.0f, 1.0f, 0.0f, 1.0f), Vec4f(0.0f, 1.0f, 1.0f, 1.0f), Vec4f(1.0f, 1.0f, 1.0f, 1.0f), Vec3f(0.0f, 1.0f, 1.0f), Vec3f(0.0f, 0.0f, 1.0f), Vec3f(1.0f, 0.0f, 1.0f) });
	vTris.push_back({ this, Vec4f(0.0f, 1.0f, 0.0f, 1.0f), Vec4f(1.0f, 1.0f, 1.0f, 1.0f), Vec4f(1.0f, 1.0f, 0.0f, 1.0f), Vec3f(0.0f, 1.0f, 1.0f), Vec3f(1.0f, 0.0f, 1.0f), Vec3f(1.0f, 1.0f, 1.0f) });

	vTris.push_back({ this, Vec4f(1.0f, 0.0f, 1.0f, 1.0f), Vec4f(0.0f, 0.0f, 1.0f, 1.0f), Vec4f(0.0f, 0.0f, 0.0f, 1.0f), Vec3f(0.0f, 1.0f, 1.0f), Vec3f(0.0f, 0.0f, 1.0f), Vec3f(1.0f, 0.0f, 1.0f) });
	vTris.push_back({ this, Vec4f(1.0f, 0.0f, 1.0f, 1.0f), Vec4f(0.0f, 0.0f, 0.0f, 1.0f), Vec4f(1.0f, 0.0f, 0.0f, 1.0f), Vec3f(0.0f, 1.0f, 1.0f), Vec3f(1.0f, 0.0f, 1.0f), Vec3f(1.0f, 1.0f, 1.0f) });
}

/**
 * \brief Updates the objects position in the application world.
 * 
 * \param fTheta World rotation
 */
void Object::updatePosition(const float fTheta)
{
	Matrix4x4 matrixRotX, matrixRotZ, matrixTranslation;

	matrixRotX.MakeRotationX(fTheta);
	matrixRotZ.MakeRotationZ(fTheta * 0.5f);
	matrixTranslation.MakeTranslation(vPos.x, vPos.y, vPos.z);
	matrixWorldPos.MakeIdentity();
	matrixWorldPos = matrixRotZ * matrixRotX;
	matrixWorldPos *= matrixTranslation;
}

/**
 * \brief Sets vPos coordinates.
 */
void Object::setPos(float x, float y, float z)
{
	vPos.x = x;
	vPos.y = y;
	vPos.z = z;
}


/**
 * \brief Clip triangles against a plane.
 * 
 * Checks to see if triangle is "inside" plane. It will split the in_tri into 
 * nessessary tris and return the number of tris created.
 * 
 * \param plane_p Plane to be clipped against
 * \param plane_n Normal of plane to be clipped against (?)
 * \param in_tri Triangle to clip
 * \param out_tri1 Triangle clip result if 1 triangle clipped
 * \param out_tri2 Triangle clip result if 2 triangles clipped
 * \return Number of clipped triangles (0 / 1 / 2)
 */
int TriangleClipAgainstPlane(Vec4f plane_p, Vec4f plane_n, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2)
{
	// Make sure plane normal is indeed normal
	plane_n.Normalise();

	// Return signed shortest distance from point to plane, plane normal must be normalised
	auto dist = [&](Vec4f& p)
	{
		Vec4f n = p;
		n.Normalise();
		return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vec4f::DotProduct(plane_n, plane_p));
	};

	// Create two temporary storage arrays to classify points either side of plane
	// If distance sign is positive, point lies on "inside" of plane
	Vec4f* inside_points[3];  int nInsidePointCount = 0;
	Vec4f* outside_points[3]; int nOutsidePointCount = 0;
	Vec3f* inside_tex[3];	   int nInsideTexCount = 0;
	Vec3f* outside_tex[3];   int nOutsideTexCount = 0;

	// Get signed distance of each point in triangle to plane
	float d0 = dist(in_tri.p[0]);
	float d1 = dist(in_tri.p[1]);
	float d2 = dist(in_tri.p[2]);

	if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; inside_tex[nInsideTexCount++] = &in_tri.t[0]; }
	else {
		outside_points[nOutsidePointCount++] = &in_tri.p[0]; outside_tex[nOutsideTexCount++] = &in_tri.t[0];
	}
	if (d1 >= 0) {
		inside_points[nInsidePointCount++] = &in_tri.p[1]; inside_tex[nInsideTexCount++] = &in_tri.t[1];
	}
	else {
		outside_points[nOutsidePointCount++] = &in_tri.p[1];  outside_tex[nOutsideTexCount++] = &in_tri.t[1];
	}
	if (d2 >= 0) {
		inside_points[nInsidePointCount++] = &in_tri.p[2]; inside_tex[nInsideTexCount++] = &in_tri.t[2];
	}
	else {
		outside_points[nOutsidePointCount++] = &in_tri.p[2];  outside_tex[nOutsideTexCount++] = &in_tri.t[2];
	}

	// Now classify triangle points, and break the input triangle into 
	// smaller output triangles if required. There are four possible
	// outcomes...

	if (nInsidePointCount == 0)
	{
		// All points lie on the outside of plane, so clip whole triangle
		// It ceases to exist

		return 0; // No returned triangles are valid
	}
	else if (nInsidePointCount == 3)
	{
		// All points lie on the inside of plane, so do nothing
		// and allow the triangle to simply pass through
		out_tri1 = in_tri;
		out_tri1.parent = in_tri.parent;

		return 1; // Just the one returned original triangle is valid
	}
	else if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{
		// Triangle should be clipped. As two points lie outside
		// the plane, the triangle simply becomes a smaller triangle

		// Copy appearance info to new triangle
#if SHOW_CLIPPING
		out_tri1.colour = 0x0000ff;
#else
		out_tri1.colour = in_tri.colour;
#endif

		out_tri1.parent = in_tri.parent;

		// The inside point is valid, so keep that...
		out_tri1.p[0] = *inside_points[0];
		out_tri1.t[0] = *inside_tex[0];

		// but the two new points are at the locations where the 
		// original sides of the triangle (lines) intersect with the plane
		float t;
		out_tri1.p[1] = Vec4f::IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
		out_tri1.t[1].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
		out_tri1.t[1].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
		out_tri1.t[1].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;


		out_tri1.p[2] = Vec4f::IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1], t);
		out_tri1.t[2].u = t * (outside_tex[1]->u - inside_tex[0]->u) + inside_tex[0]->u;
		out_tri1.t[2].v = t * (outside_tex[1]->v - inside_tex[0]->v) + inside_tex[0]->v;
		out_tri1.t[2].w = t * (outside_tex[1]->w - inside_tex[0]->w) + inside_tex[0]->w;



		return 1; // Return the newly formed single triangle
	}
	else if (nInsidePointCount == 2 && nOutsidePointCount == 1)
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

		out_tri1.parent = in_tri.parent;
		out_tri2.parent = in_tri.parent;

		// The first triangle consists of the two inside points and a new
		// point determined by the location where one side of the triangle
		// intersects with the plane
		out_tri1.p[0] = *inside_points[0];
		out_tri1.p[1] = *inside_points[1];
		out_tri1.t[0] = *inside_tex[0];
		out_tri1.t[1] = *inside_tex[1];

		float t;
		out_tri1.p[2] = Vec4f::IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
		out_tri1.t[2].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
		out_tri1.t[2].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
		out_tri1.t[2].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;


		// The second triangle is composed of one of he inside points, a
		// new point determined by the intersection of the other side of the 
		// triangle and the plane, and the newly created point above
		out_tri2.p[0] = *inside_points[1];
		out_tri2.p[1] = out_tri1.p[2];
		out_tri2.t[0] = *inside_tex[1];
		out_tri2.t[1] = out_tri1.t[2];
		out_tri2.p[2] = Vec4f::IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0], t);
		out_tri2.t[2].u = t * (outside_tex[0]->u - inside_tex[1]->u) + inside_tex[1]->u;
		out_tri2.t[2].v = t * (outside_tex[0]->v - inside_tex[1]->v) + inside_tex[1]->v;
		out_tri2.t[2].w = t * (outside_tex[0]->w - inside_tex[1]->w) + inside_tex[1]->w;

		return 2; // Return two newly formed triangles which form a quad
	}

	// Error we should not end up here

	std::cerr << "Error at TriangleClipAgainstPlane() -> Did not find suitable control path\n";
	return 0;  // assume triangle clipped
}
