#include "game.h"
#include <iostream>
#include <string>
#include <fstream>
#include <strstream>
#include <vector>
#include <assert.h>
#include <algorithm>
#include <list>
#include <sstream>

int32 Game::run()
{
	pDepthBuffer = new float[win.Gfx().getWidth() * win.Gfx().getHeight()];

	// Load textures
	objectTexture = Texture();
	//textureCube.LoadExampleTexture();
	objectTexture.LoadTextureFromBMP("texture_16x32.bmp");

	// Load wavefront files
	//objectMesh.LoadObjectFile("lowpolybuildings2.obj", true);
	objectMesh.LoadTestCube();

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

	double turningSpeed = 0.005f * secondsElapsed;
	double movementSpeed = 0.005f * secondsElapsed;

	Vector vForward = vLookDir * movementSpeed;

	vCamera += vForward * moveDirZ;			// forward / backward
	fYaw += moveDirX * turningSpeed;		// left / right
	vCamera.y += movementSpeed * moveDirY;	// up / down

	Matrix4x4 matrixRotX, matrixRotZ, matrixTranslation, matrixWorld;

	fTheta += 0.0005f;  // Spin the world

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
	matrixCamera.MakeQuickInverse();

	// Triangles
	std::vector<Triangle> trianglesToRaster;
	for (auto tri : objectMesh.tris)
	{
		Triangle triProjected, triTransformed, triCamera;

		triTransformed.points[0] = matrixWorld * tri.points[0];
		triTransformed.points[1] = matrixWorld * tri.points[1];
		triTransformed.points[2] = matrixWorld * tri.points[2];
		triTransformed.t[0] = tri.t[0];
		triTransformed.t[1] = tri.t[1];
		triTransformed.t[2] = tri.t[2];

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
			float triColour = ((dp * 255.0f) * 3.0f) / 5.0f;

			triCamera.points[0] = matrixCamera * triTransformed.points[0];
			triCamera.points[1] = matrixCamera * triTransformed.points[1];
			triCamera.points[2] = matrixCamera * triTransformed.points[2];
			triCamera.colour = triColour;
			triCamera.t[0] = triTransformed.t[0];
			triCamera.t[1] = triTransformed.t[1];
			triCamera.t[2] = triTransformed.t[2];

			int32 nClippedTriangles = 0;
			Triangle clipped[2];
			nClippedTriangles = Vector::TriangleClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triCamera, clipped[0], clipped[1]);

			for (int32 n = 0; n < nClippedTriangles; n++)
			{
				triProjected.points[0] = projectionMatrix * clipped[n].points[0];
				triProjected.points[1] = projectionMatrix * clipped[n].points[1];
				triProjected.points[2] = projectionMatrix * clipped[n].points[2];
				triProjected.colour = clipped[n].colour;
				triProjected.t[0] = clipped[n].t[0];
				triProjected.t[1] = clipped[n].t[1];
				triProjected.t[2] = clipped[n].t[2];


				triProjected.t[0].u /= triProjected.points[0].w;
				triProjected.t[1].u /= triProjected.points[1].w;
				triProjected.t[2].u /= triProjected.points[2].w;

				triProjected.t[0].v /= triProjected.points[0].w;
				triProjected.t[1].v /= triProjected.points[1].w;
				triProjected.t[2].v /= triProjected.points[2].w;
				
				triProjected.t[0].w = 1.0f / triProjected.points[0].w;
				triProjected.t[1].w = 1.0f / triProjected.points[1].w;
				triProjected.t[2].w = 1.0f / triProjected.points[2].w;
				

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
	//sort(trianglesToRaster.begin(), trianglesToRaster.end(), [](Triangle& t1, Triangle& t2)
	//	{
	//		float z1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3.0f;
	//		float z2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3.0f;
	//		return z1 > z2;
	//	});

	/* ---------- Render ---------- */
	// Clear screen and depth buffer
	win.Gfx().ClearScreen(0x000000);
	for (int i = 0; i < win.Gfx().getWidth() * win.Gfx().getHeight(); i++)
	{
		pDepthBuffer[i] = 0.0f;
	}

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
					case 0:	trianglesToAdd = Vector::TriangleClipAgainstPlane(pTop,		nDownwards, test, clipped[0], clipped[1]); break;
					case 1:	trianglesToAdd = Vector::TriangleClipAgainstPlane(pBottom,	nUpwards,	test, clipped[0], clipped[1]); break;
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
			// Draw textured triangles
			TexturedTriangle(t.points[0].x, t.points[0].y, t.t[0].u, t.t[0].v, t.t[0].w,
							 t.points[1].x, t.points[1].y, t.t[1].u, t.t[1].v, t.t[1].w,
							 t.points[2].x, t.points[2].y, t.t[2].u, t.t[2].v, t.t[2].w,
							 &objectTexture);

			// Draw basic triangles
			//win.Gfx().FillTriangleP(
			//	t.points[0].x, t.points[0].y, 
			//	t.points[1].x, t.points[1].y, 
			//	t.points[2].x, t.points[2].y,
			//	t.colour);

			// Draw wireframe
			//win.Gfx().DrawTriangleP(
			//	t.points[0].x, t.points[0].y,
			//	t.points[1].x, t.points[1].y,
			//	t.points[2].x, t.points[2].y,
			//	0xffffff);
		}
	}

	win.Gfx().Render();  // last
}

void Game::TexturedTriangle(int x1, int y1, float u1, float v1, float w1,
	int x2, int y2, float u2, float v2, float w2,
	int x3, int y3, float u3, float v3, float w3,
	Texture* tex)
{
	if (y2 < y1)
	{
		std::swap(y1, y2);
		std::swap(x1, x2);
		std::swap(u1, u2);
		std::swap(v1, v2);
		std::swap(w1, w2);
	}

	if (y3 < y1)
	{
		std::swap(y1, y3);
		std::swap(x1, x3);
		std::swap(u1, u3);
		std::swap(v1, v3);
		std::swap(w1, w3);
	}

	if (y3 < y2)
	{
		std::swap(y2, y3);
		std::swap(x2, x3);
		std::swap(u2, u3);
		std::swap(v2, v3);
		std::swap(w2, w3);
	}

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dv1 = v2 - v1;
	float du1 = u2 - u1;
	float dw1 = w2 - w1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dv2 = v3 - v1;
	float du2 = u3 - u1;
	float dw2 = w3 - w1;

	float tex_u, tex_v, tex_w;

	float dax_step = 0, dbx_step = 0,
		du1_step = 0, dv1_step = 0,
		du2_step = 0, dv2_step = 0,
		dw1_step = 0, dw2_step = 0;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy2) du2_step = du2 / (float)abs(dy2);
	if (dy2) dv2_step = dv2 / (float)abs(dy2);
	if (dy2) dw2_step = dw2 / (float)abs(dy2);

	if (dy1)
	{
		for (int i = y1; i <= y2; i++)
		{
			int ax = x1 + (float)(i - y1) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u1 + (float)(i - y1) * du1_step;
			float tex_sv = v1 + (float)(i - y1) * dv1_step;
			float tex_sw = w1 + (float)(i - y1) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx)
			{
				std::swap(ax, bx);
				std::swap(tex_su, tex_eu);
				std::swap(tex_sv, tex_ev);
				std::swap(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;
				if (tex_w > pDepthBuffer[i * win.Gfx().getWidth() + j])
				{
					uint32 colour = tex->lookUp(tex_u / tex_w, tex_v / tex_w);
					win.Gfx().DrawPointP(j, i, colour);
					pDepthBuffer[i * win.Gfx().getWidth() + j] = tex_w;
				}
				t += tstep;
			}

		}
	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dv1 = v3 - v2;
	du1 = u3 - u2;
	dw1 = w3 - w2;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	du1_step = 0, dv1_step = 0;
	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy1)
	{
		for (int i = y2; i <= y3; i++)
		{
			int ax = x2 + (float)(i - y2) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u2 + (float)(i - y2) * du1_step;
			float tex_sv = v2 + (float)(i - y2) * dv1_step;
			float tex_sw = w2 + (float)(i - y2) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx)
			{
				std::swap(ax, bx);
				std::swap(tex_su, tex_eu);
				std::swap(tex_sv, tex_ev);
				std::swap(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;

				if (tex_w > pDepthBuffer[i * win.Gfx().getWidth() + j])
				{
					uint32 colour = tex->lookUp(tex_u / tex_w, tex_v / tex_w);
					win.Gfx().DrawPointP(j, i, colour);
					pDepthBuffer[i * win.Gfx().getWidth() + j] = tex_w;
				}
				t += tstep;
			}
		}
	}
}
