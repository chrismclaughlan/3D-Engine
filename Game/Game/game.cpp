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
#include <list>

#define DRAW_WIREFRAME false

int32 Game::run()
{
	meshCube.LoadObjectFile("teapot.obj");

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
			moveDirX = 0.0f;
			moveDirY = 0.0f;
			moveDirZ = 0.0f;

			fYaw = 0.0f;
			vCamera.reset();
			vLookDir.reset();
		} break;
		case 0x57:  // 'w'
		{
			if (event.isPressed())
				moveDirZ = 1.0f;
			else if (moveDirZ > 0 && event.isReleased())
				moveDirZ = 0.0f;
		} break;
		case 0x53:  // 's'
		{
			if (event.isPressed())
				moveDirZ = -1.0f;
			else if (moveDirZ < 0 && event.isReleased())
				moveDirZ = 0.0f;
		} break;
		case 0x41:  // 'a'
		{
			if (event.isPressed())
				moveDirX = -1.0f;
			else if (moveDirX < 0 && event.isReleased())
				moveDirX = 0.0f;
		} break;
		case 0x44:  // 'd'
		{
			if (event.isPressed())
				moveDirX = 1.0f;
			else if (moveDirX > 0 && event.isReleased())
				moveDirX = 0.0f;
		} break;
		case VK_SPACE:
		{
			if (event.isPressed())
				moveDirY = 1.0f;
			else if (moveDirY > 0 && event.isReleased())
				moveDirY = 0.0f;
		} break;
		case 16:  // 0x16
		{
			if (event.isPressed())
				moveDirY = -1.0f;
			else if (moveDirY < 0 && event.isReleased())
				moveDirY = 0.0f;
		} break;
		}
	}

//	while (!win.mouse.isEmpty())
//	{
//		const auto event = win.mouse.read();
//		switch (event.getType())
//		{
//		case Mouse::Event::Type::Move:
//		{
////#if DISPLAY_DEBUG_CONSOLE && DISPLAY_MOUSE_COORDS
////			std::cout << event.getX() << " " << event.getY() << "\n";
////#endif
//		} break;
//		case Mouse::Event::Type::LPressed:
//		{
//		} break;
//		case Mouse::Event::Type::LReleased:
//		{
//		} break;
//		case Mouse::Event::Type::RPressed:
//		{
//		} break;
//		case Mouse::Event::Type::MPressed:
//		{
//		} break;
//		case Mouse::Event::Type::WheelDown:
//		{
//			fTheta += 0.05f;
//			std::cout << "fTheta = " << fTheta << std::endl;
//		} break;
//		case Mouse::Event::Type::WheelUp:
//		{
//			fTheta -= 0.05f;
//			std::cout << "fTheta = " << fTheta << std::endl;
//		} break;
//		}
//	}

//	while (!win.keyboard.charIsEmpty())
//	{
//		const uint8 e = win.keyboard.readChar();
//
//		switch (e)
//		{
//		case VK_BACK:
//		{
//			if (!userTextBuffer.empty())
//			{
//				userTextBuffer.pop_back();
//			}
//		} break;// continue to default ...
//		default:
//		{
//			if (acceptedCharacters.find(e) != std::string::npos)
//			{
//				// If char in acceptedCharacters
//				userTextBuffer += std::string(1, e);
//			}
//		} break;
//		}
//#ifdef DISPLAY_DEBUG_CONSOLE
//
//		std::cout << userTextBuffer << "\n";
//#endif
//	}
}

void Game::DoFrame()
{
	/* ---------- Simulate ---------- */
	//fTheta += 0.005f;

	// TODO FIX TIMING

	//std::chrono::time_point<std::chrono::steady_clock> timeNow = std::chrono::high_resolution_clock::now();
	//std::chrono::duration<double> diff = timeNow - timeStarted;
	//const double secondsElapsed = diff.count();
	const double secondsElapsed = 1.0f;

	double turningSpeed = 0.001f * secondsElapsed;
	double movementSpeed = 0.01f * secondsElapsed;

	Vector vForward;
	vForward = vLookDir * movementSpeed;

	vCamera += vForward * moveDirZ;			// forward / backward
	fYaw += moveDirX * turningSpeed;		// left / right
	vCamera.y += movementSpeed * moveDirY;	// up / down

	Matrix4x4 matrixRotX, matrixRotZ, matrixTranslation, matrixWorld;

	fTheta += 0.005f;

	matrixRotX.MakeRotationX(fTheta);
	matrixRotZ.MakeRotationZ(fTheta * 0.5f);
	matrixTranslation.MakeTranslation(0.0f, 0.0f, 5.0f);
	matrixWorld.MakeIdentity();
	matrixWorld = matrixRotZ * matrixRotX;
	matrixWorld *= matrixTranslation;

	// Camera
	Matrix4x4 matrixCameraRotation, matrixCamera;  // matrixView
	Vector vUp(0.0f, 1.0f, 0.0f);
	Vector vTarget(0.0f, 0.0f, 1.0f);

	matrixCameraRotation.MakeRotationY(fYaw);
	vLookDir = matrixCameraRotation * vTarget;
	vTarget = vCamera + vLookDir;

	matrixCamera.MakePointAt(vCamera, vTarget, vUp);
	//matrixView.MakeQuickInverse(matrixCamera);
	matrixCamera.MakeQuickInverse();

	// Triangles
	std::vector<Triangle> trianglesToRaster;
	for (auto tri : meshCube.tris)
	{
		Triangle triProjected, triTransformed, triCamera;

		triTransformed.points[0] = matrixWorld * tri.points[0];
		triTransformed.points[1] = matrixWorld * tri.points[1];
		triTransformed.points[2] = matrixWorld * tri.points[2];

		Vector normal, line1, line2;

		line1 = triTransformed.points[1] - triTransformed.points[0];
		line2 = triTransformed.points[2] - triTransformed.points[0];

		normal = Vector::CrossProduct(line1, line2);

		normal.Normalise();

		Vector vCameraRay;
		vCameraRay = triTransformed.points[0] - vCamera;

		if (Vector::DotProduct(normal, vCameraRay) < 0.0f)
		{
			// Shade triangle
			Vector vLightDir = { 0.0f, 1.0f, -1.0f };
			vLightDir.Normalise();
			float dp = std::max(0.1f, Vector::DotProduct(vLightDir, normal));
			float triColour = ((dp * 256.0f) * 3.0f) / 5.0f;

			triCamera.points[0] = matrixCamera * triTransformed.points[0];
			triCamera.points[1] = matrixCamera * triTransformed.points[1];
			triCamera.points[2] = matrixCamera * triTransformed.points[2];
			triCamera.colour = triColour;

			int32 nClippedTriangles = 0;
			Triangle clipped[2];
			nClippedTriangles = Vector::TriangleClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triCamera, clipped[0], clipped[1]);

			for (int32 n = 0; n < nClippedTriangles; n++)
			{
				triProjected.points[0] = projectionMatrix * clipped[n].points[0];
				triProjected.points[1] = projectionMatrix * clipped[n].points[1];
				triProjected.points[2] = projectionMatrix * clipped[n].points[2];
				triProjected.colour = clipped[n].colour;

				triProjected.points[0] /= triProjected.points[0].w;
				triProjected.points[1] /= triProjected.points[1].w;
				triProjected.points[2] /= triProjected.points[2].w;

				// Invert
				triProjected.points[0].x *= -1;
				triProjected.points[1].x *= -1;
				triProjected.points[2].x *= -1;
			
				Vector vOffsetView = { 1, 1, 0 };
				triProjected.points[0] += vOffsetView;
				triProjected.points[1] += vOffsetView;
				triProjected.points[2] += vOffsetView;

				triProjected.points[0].x *= 0.5f * (float)win.Gfx().getWidth();
				triProjected.points[0].y *= 0.5f * (float)win.Gfx().getHeight();
				triProjected.points[1].x *= 0.5f * (float)win.Gfx().getWidth();
				triProjected.points[1].y *= 0.5f * (float)win.Gfx().getHeight();
				triProjected.points[2].x *= 0.5f * (float)win.Gfx().getWidth();
				triProjected.points[2].y *= 0.5f * (float)win.Gfx().getHeight();

				trianglesToRaster.push_back(triProjected);
			}
		}
	}

	// Sort triangles from back to front
	sort(trianglesToRaster.begin(), trianglesToRaster.end(), [](Triangle& t1, Triangle& t2)
		{
			float z1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3.0f;
			float z2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3.0f;
			return z1 > z2;
		});

	/* ---------- Render ---------- */
	win.Gfx().ClearScreen(0x000000);

	for (auto& triToRaster : trianglesToRaster)
	{
		Triangle clipped[2];
		std::list<Triangle> listTriangles;
		listTriangles.push_back(triToRaster);
		int32 newTriangles = 1;

		for (int32 p = 0; p < 4; p++)
		{
			int32 trianglesToAdd = 0;
			while (newTriangles > 0)
			{
				Triangle test = listTriangles.front();
				listTriangles.pop_front();
				newTriangles--;

				// Clipping planes (borders of the screen)
				Vector pTop		(0.0f,								0.0f,								0.0f);
				Vector pBottom	(0.0f,								(float)win.Gfx().getHeight() - 1,	0.0f);
				Vector pLeft	(0.0f,								0.0f,								0.0f);
				Vector pRight	((float)win.Gfx().getWidth() - 1,	0.0f,								0.0f);

				// Normals (facing into screen boundary)
				Vector nDownwards	(0.0f,	1.0f,	0.0f);
				Vector nUpwards		(0.0f,	-1.0f,	0.0f);
				Vector nRight		(1.0f,	0.0f,	0.0f);
				Vector nLeft		(-1.0f, 0.0f,	0.0f);

				switch (p)
				{
					case 0:	trianglesToAdd = Vector::TriangleClipAgainstPlane(pTop,	nDownwards, test, clipped[0], clipped[1]); break;
					case 1:	trianglesToAdd = Vector::TriangleClipAgainstPlane(pBottom, nUpwards,	test, clipped[0], clipped[1]); break;
					case 2:	trianglesToAdd = Vector::TriangleClipAgainstPlane(pLeft,	nRight,		test, clipped[0], clipped[1]); break;
					case 3:	trianglesToAdd = Vector::TriangleClipAgainstPlane(pRight,	nLeft,		test, clipped[0], clipped[1]); break;
				}

				for (int32 w = 0; w < trianglesToAdd; w++)
				{
					listTriangles.push_back(clipped[w]);
				}
			}
			newTriangles = listTriangles.size();
		}

		for (auto& t : listTriangles)
		{
			win.Gfx().FillTriangleP(
				t.points[0].x, t.points[0].y, 
				t.points[1].x, t.points[1].y, 
				t.points[2].x, t.points[2].y,
				t.colour);
#if DRAW_WIREFRAME
			win.Gfx().DrawTriangleP(
				t.points[0].x, t.points[0].y,
				t.points[1].x, t.points[1].y,
				t.points[2].x, t.points[2].y,
				0xffffff);
#endif
		}
	}

	//performance.Update();
	//if (fpsLimit > 0)
	//{
	//	performance.LimitFps(fpsLimit);
	//}

//#if DISPLAY_DEBUG_CONSOLE && DISPLAY_FPS
//	if (performance.hasTimePassed(1.0))
//	{
//		std::cout << "FPS: " << performance.getFps() << "\n";
//	}
//#endif

	win.Gfx().Render();  // last
}
