#pragma once
#include "types.h"
#include "vector.h"

struct Triangle
{
	const Object* parent = nullptr;

	Vector p[3];
	Vector2 t[3];
	uint colour;
	// alpha
};

class Object
{
public:
	Matrix4x4 matrixWorldPos;
	std::vector<Triangle> tris;
	Texture24* texture = nullptr;
	// check all right
	float x = 0.0f;  // + left / - right
	float y = 0.0f;  // + upwards / - downwards
	float z = 5.0f;  // + forwards / - backwards

public:
	bool LoadObjectFile(std::string filename, bool hasTexture)
	{
		tris.clear();

		std::ifstream file(filename);

		if (!file.is_open())
		{
			return false;
		}

		std::vector<Vector> verticies;
		std::vector<Vector2> textures;

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
					Vector2 v;
					s >> temp >> temp >> v.u >> v.v;
					textures.push_back(v);
				}
				else if (line[1] == ' ')
				{
					Vector v;
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
					tris.push_back({ this, verticies[f[0] - 1], verticies[f[1] - 1], verticies[f[2] - 1] });
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

					tris.push_back({ this, Vector(verticies[std::stoi(tokens[0]) - 1]), Vector(verticies[std::stoi(tokens[2]) - 1]), Vector(verticies[std::stoi(tokens[4]) - 1]), Vector2(textures[std::stoi(tokens[1]) - 1]), Vector2(textures[std::stoi(tokens[3]) - 1]), Vector2(textures[std::stoi(tokens[5]) - 1]) });
				}
			}
		}

		return true;
	}
	void LoadTestCube()
	{
		// South
		tris.clear();
		tris.push_back({ this, Vector(0.0f, 0.0f, 0.0f, 1.0f), Vector(0.0f, 1.0f, 0.0f, 1.0f), Vector(1.0f, 1.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f) });
		tris.push_back({ this, Vector(0.0f, 0.0f, 0.0f, 1.0f), Vector(1.0f, 1.0f, 0.0f, 1.0f), Vector(1.0f, 0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f, 1.0f) });

		tris.push_back({ this, Vector(1.0f, 0.0f, 0.0f, 1.0f), Vector(1.0f, 1.0f, 0.0f, 1.0f), Vector(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f) });
		tris.push_back({ this, Vector(1.0f, 0.0f, 0.0f, 1.0f), Vector(1.0f, 1.0f, 1.0f, 1.0f), Vector(1.0f, 0.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f, 1.0f) });

		tris.push_back({ this, Vector(1.0f, 0.0f, 1.0f, 1.0f), Vector(1.0f, 1.0f, 1.0f, 1.0f), Vector(0.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f) });
		tris.push_back({ this, Vector(1.0f, 0.0f, 1.0f, 1.0f), Vector(0.0f, 1.0f, 1.0f, 1.0f), Vector(0.0f, 0.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f, 1.0f) });

		tris.push_back({ this, Vector(0.0f, 0.0f, 1.0f, 1.0f), Vector(0.0f, 1.0f, 1.0f, 1.0f), Vector(0.0f, 1.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f) });
		tris.push_back({ this, Vector(0.0f, 0.0f, 1.0f, 1.0f), Vector(0.0f, 1.0f, 0.0f, 1.0f), Vector(0.0f, 0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f, 1.0f) });

		tris.push_back({ this, Vector(0.0f, 1.0f, 0.0f, 1.0f), Vector(0.0f, 1.0f, 1.0f, 1.0f), Vector(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f) });
		tris.push_back({ this, Vector(0.0f, 1.0f, 0.0f, 1.0f), Vector(1.0f, 1.0f, 1.0f, 1.0f), Vector(1.0f, 1.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f, 1.0f) });

		tris.push_back({ this, Vector(1.0f, 0.0f, 1.0f, 1.0f), Vector(0.0f, 0.0f, 1.0f, 1.0f), Vector(0.0f, 0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f) });
		tris.push_back({ this, Vector(1.0f, 0.0f, 1.0f, 1.0f), Vector(0.0f, 0.0f, 0.0f, 1.0f), Vector(1.0f, 0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f, 1.0f) });
	}

	// Move object in world space (to test multiple objects)
	void updatePosition(const float fTheta)
	{
		Matrix4x4 matrixRotX, matrixRotZ, matrixTranslation;

		matrixRotX.MakeRotationX(fTheta);
		matrixRotZ.MakeRotationZ(fTheta * 0.5f);
		matrixTranslation.MakeTranslation(x, y, z);
		matrixWorldPos.MakeIdentity();
		matrixWorldPos = matrixRotZ * matrixRotX;
		matrixWorldPos *= matrixTranslation;
	}

	void setPos(float x_, float y_, float z_)
	{
		x = x_;
		y = y_;
		z = z_;
	}
};
