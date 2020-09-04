#include "Engine\exception.h"
#include "Engine\defines.h"
#include "game.h"
#include "vector.h"
#include <iostream>
#include <string>
#include <fstream>
#include <strstream>
#include <vector>
#include <assert.h>
#include <algorithm>

int32 Game::run()
{
	meshCube.LoadObjectFile("spaceship.obj");

	//Matrix4x4 matrixProjection;
	//matrixProjection.MakeProjection(
	//	90.0f,
	//	(float)win.Gfx().getHeight() / (float)win.Gfx().getWidth(),
	//	0.1f,
	//	1000.0f);

	float fNear = 0.1f;
	float fFar = 1000.0f;
	float fFov = 90.0f;
	float fAspectRatio = (float)win.Gfx().getHeight() / (float)win.Gfx().getWidth();
	float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);
	projectionMatrix.m[0][0] = fAspectRatio * fFovRad;
	projectionMatrix.m[1][1] = fFovRad;
	projectionMatrix.m[2][2] = fFar / (fFar / fNear);
	projectionMatrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	projectionMatrix.m[2][3] = 1.0f;
	projectionMatrix.m[3][3] = 0.0f;

	while (Window::processMessages())
	{
		HandleInput();
		DoFrame();
	}

	return Window::getExitCode();
}

void Game::HandleInput()
{
	while (!win.keyboard.keyIsEmpty())
	{
		const auto event = win.keyboard.readKey();
		switch (event.getCode())
		{
		case VK_ESCAPE:
		{
		} break;
		case 0x30:
		{
		} break;
		case 0x31:
		{
		} break;
		case 0x32:
		{
		} break;
		case 0x33:
		{
		} break;
		case VK_RETURN:
		{
			// test ping
			if (event.isPressed())
			{
			}
		} break;
		}
	}

	while (!win.mouse.isEmpty())
	{
		const auto event = win.mouse.read();
		switch (event.getType())
		{
		case Mouse::Event::Type::Move:
		{
//#if DISPLAY_DEBUG_CONSOLE && DISPLAY_MOUSE_COORDS
//			std::cout << event.getX() << " " << event.getY() << "\n";
//#endif
		} break;
		case Mouse::Event::Type::LPressed:
		{
		} break;
		case Mouse::Event::Type::LReleased:
		{
		} break;
		case Mouse::Event::Type::RPressed:
		{
		} break;
		case Mouse::Event::Type::MPressed:
		{
		} break;
		case Mouse::Event::Type::WheelDown:
		{
			fTheta += 0.05f;
			std::cout << "fTheta = " << fTheta << std::endl;
		} break;
		case Mouse::Event::Type::WheelUp:
		{
			fTheta -= 0.05f;
			std::cout << "fTheta = " << fTheta << std::endl;
		} break;
		}
	}

	while (!win.keyboard.charIsEmpty())
	{
		const uint8 e = win.keyboard.readChar();

		switch (e)
		{
		case VK_BACK:
		{
			if (!userTextBuffer.empty())
			{
				userTextBuffer.pop_back();
			}
		} break;// continue to default ...
		default:
		{
			if (acceptedCharacters.find(e) != std::string::npos)
			{
				// If char in acceptedCharacters
				userTextBuffer += std::string(1, e);
			}
		} break;
		}
#ifdef DISPLAY_DEBUG_CONSOLE

		std::cout << userTextBuffer << "\n";
#endif
	}
}

void Game::DoFrame()
{
	/* ---------- Simulate ---------- */

	/* ---------- Render ---------- */
	win.Gfx().ClearScreen(0x000000);

	matrix4x4 matrixRotX, matrixRotZ;

	matrixRotZ.m[0][0] = cosf(fTheta);
	matrixRotZ.m[0][1] = sinf(fTheta);
	matrixRotZ.m[1][0] = -sinf(fTheta);
	matrixRotZ.m[1][1] = cosf(fTheta);
	matrixRotZ.m[2][2] = 1;
	matrixRotZ.m[3][3] = 1;

	matrixRotX.m[0][0] = 1;
	matrixRotX.m[1][1] = cosf(fTheta * 0.5f);
	matrixRotX.m[1][2] = sinf(fTheta * 0.5f);
	matrixRotX.m[2][1] = -sinf(fTheta * 0.5f);
	matrixRotX.m[2][2] = cosf(fTheta * 0.5f);
	matrixRotX.m[3][3] = 1;

	std::vector<triangle> trianglesToRaster;

	for (auto tri : meshCube.tris)
	{
		fTheta += 0.00005f;

		triangle triProjected, triTranslated, triRotatedZ, triRotatedXZ;

		win.Gfx().MultiplyMatrixVector(tri.points[0], triRotatedZ.points[0], matrixRotZ);
		win.Gfx().MultiplyMatrixVector(tri.points[1], triRotatedZ.points[1], matrixRotZ);
		win.Gfx().MultiplyMatrixVector(tri.points[2], triRotatedZ.points[2], matrixRotZ);

		win.Gfx().MultiplyMatrixVector(triRotatedZ.points[0], triRotatedXZ.points[0], matrixRotX);
		win.Gfx().MultiplyMatrixVector(triRotatedZ.points[1], triRotatedXZ.points[1], matrixRotX);
		win.Gfx().MultiplyMatrixVector(triRotatedZ.points[2], triRotatedXZ.points[2], matrixRotX);

		triTranslated = triRotatedXZ;
		triTranslated.points[0].z = triRotatedXZ.points[0].z + 7.0f;
		triTranslated.points[1].z = triRotatedXZ.points[1].z + 7.0f;
		triTranslated.points[2].z = triRotatedXZ.points[2].z + 7.0f;

		vector3 normal, line1, line2;
		line1.x = triTranslated.points[1].x - triTranslated.points[0].x;
		line1.y = triTranslated.points[1].y - triTranslated.points[0].y;
		line1.z = triTranslated.points[1].z - triTranslated.points[0].z;

		line2.x = triTranslated.points[2].x - triTranslated.points[0].x;
		line2.y = triTranslated.points[2].y - triTranslated.points[0].y;
		line2.z = triTranslated.points[2].z - triTranslated.points[0].z;

		normal.x = line1.y * line2.z - line1.z * line2.y;
		normal.y = line1.z * line2.x - line1.x * line2.z;
		normal.z = line1.x * line2.y - line1.y * line2.x;
		float length = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
		normal.x /= length;
		normal.y /= length;
		normal.z /= length;

		//if (normal.z < 0)
		if (normal.x * (triTranslated.points[0].x - camera.x) + normal.y * (triTranslated.points[0].y - camera.y) + normal.z * (triTranslated.points[0].z - camera.z) < 0.0f)
		{
			win.Gfx().MultiplyMatrixVector(triTranslated.points[0], triProjected.points[0], projectionMatrix);
			win.Gfx().MultiplyMatrixVector(triTranslated.points[1], triProjected.points[1], projectionMatrix);
			win.Gfx().MultiplyMatrixVector(triTranslated.points[2], triProjected.points[2], projectionMatrix);

			// Shade triangle
			vector3 lightDir = { 0.0f, 0.0f, -1.0f };
			float length = sqrtf(lightDir.x * lightDir.x + lightDir.y * lightDir.y + lightDir.z * lightDir.z);
			lightDir.x /= length; lightDir.y /= length; lightDir.z /= length;
			float dp = normal.x * lightDir.x + normal.y * lightDir.y + normal.z * lightDir.z;

			//std::cout << "dp=" << dp << "\n";

			float multiplier = ((dp * 256.0f) * 3.0f) / 5.0f;

			triProjected.colour = multiplier;

			trianglesToRaster.push_back(triProjected);
		}
	}

	// Sort triangles from back to front
	sort(trianglesToRaster.begin(), trianglesToRaster.end(), [](triangle& t1, triangle& t2)
		{
			float z1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3.0f;
			float z2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3.0f;
			return z1 > z2;
		});

	for (auto& triProjected : trianglesToRaster)
	{
		//win.Gfx().DrawTriangle(
		//	triProjected.points[0].x, triProjected.points[0].y,
		//	triProjected.points[1].x, triProjected.points[1].y,
		//	triProjected.points[2].x, triProjected.points[2].y,
		//	0xff0000);

		win.Gfx().FillTriangle(
			triProjected.points[0].x, triProjected.points[0].y,
			triProjected.points[1].x, triProjected.points[1].y,
			triProjected.points[2].x, triProjected.points[2].y,
			triProjected.colour);
	}

	//win.Gfx().FillTriangleP(100, 100, 200, 300, 300, 100, 0x0000ff);
	//win.Gfx().FillTriangleP(300, 200, 400, 100, 500, 200, 0x00ff00);

	performance.Update();
	if (fpsLimit > 0)
	{
		performance.LimitFps(fpsLimit);
	}

//#if DISPLAY_DEBUG_CONSOLE && DISPLAY_FPS
//	if (performance.hasTimePassed(1.0))
//	{
//		std::cout << "FPS: " << performance.getFps() << "\n";
//	}
//#endif

	win.Gfx().Render();  // last
}
