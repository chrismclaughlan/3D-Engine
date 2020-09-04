#include "Engine\exception.h"
#include "Engine\defines.h"
#include "game.h"
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

	projectionMatrix.MakeProjection(
		90.0f,
		(float)win.Gfx().getHeight() / (float)win.Gfx().getWidth(),
		0.1f,
		1000.0f);

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
	fTheta += 0.005f;

	Matrix4x4 matrixRotX, matrixRotZ, matrixTranslation, matrixWorld;

	matrixRotX.MakeRotationX(fTheta);
	matrixRotZ.MakeRotationZ(fTheta * 0.5f);
	matrixTranslation.MakeTranslation(0.0f, 0.0f, 10.0f);
	matrixWorld.MakeIdentity();
	matrixWorld = matrixRotZ * matrixRotX;
	matrixWorld = matrixWorld * matrixTranslation;

	std::vector<triangle> trianglesToRaster;

	for (auto tri : meshCube.tris)
	{
		triangle triProjected, triTransformed;

		triTransformed.points[0] = matrixWorld * tri.points[0];
		triTransformed.points[1] = matrixWorld * tri.points[1];
		triTransformed.points[2] = matrixWorld * tri.points[2];

		Vector normal, line1, line2;

		line1 = triTransformed.points[1] - triTransformed.points[0];
		line2 = triTransformed.points[2] - triTransformed.points[0];

		normal = Vector::CrossProduct(line1, line2);

		normal = Vector::Normalise(normal);

		Vector vCameraRay;
		vCameraRay = triTransformed.points[0] - vCameraRay;

		//if (normal.z < 0)
		if (Vector::DotProduct(normal, vCameraRay) < 0.0f)
		{
			// Shade triangle
			Vector vLightDir = { 0.0f, 1.0f, -1.0f };
			vLightDir = Vector::Normalise(vLightDir);
			float dp = std::max(0.1f, Vector::DotProduct(vLightDir, normal));
			float triColour = ((dp * 256.0f) * 3.0f) / 5.0f;

			triProjected.points[0] = projectionMatrix * triTransformed.points[0];
			triProjected.points[1] = projectionMatrix * triTransformed.points[1];
			triProjected.points[2] = projectionMatrix * triTransformed.points[2];
			triProjected.colour = triColour;

			triProjected.points[0] = triProjected.points[0] / triProjected.points[0].w;
			triProjected.points[1] = triProjected.points[1] / triProjected.points[1].w;
			triProjected.points[2] = triProjected.points[2] / triProjected.points[2].w;

			Vector vOffsetView = { 1, 1, 0 };
			triProjected.points[0] = triProjected.points[0] + vOffsetView;
			triProjected.points[1] = triProjected.points[1] + vOffsetView;
			triProjected.points[2] = triProjected.points[2] + vOffsetView;

			triProjected.points[0].x = triProjected.points[0].x * 0.5f * (float)win.Gfx().getWidth();
			triProjected.points[0].y = triProjected.points[0].y * 0.5f * (float)win.Gfx().getHeight();
			triProjected.points[1].x = triProjected.points[1].x * 0.5f * (float)win.Gfx().getWidth();
			triProjected.points[1].y = triProjected.points[1].y * 0.5f * (float)win.Gfx().getHeight();
			triProjected.points[2].x = triProjected.points[2].x * 0.5f * (float)win.Gfx().getWidth();
			triProjected.points[2].y = triProjected.points[2].y * 0.5f * (float)win.Gfx().getHeight();

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

	/* ---------- Render ---------- */
	win.Gfx().ClearScreen(0x000000);

	for (auto& triProjected : trianglesToRaster)
	{
		win.Gfx().DrawTriangleP(
			triProjected.points[0].x, triProjected.points[0].y,
			triProjected.points[1].x, triProjected.points[1].y,
			triProjected.points[2].x, triProjected.points[2].y,
			0xff0000);

		win.Gfx().FillTriangleP(
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
