#include "Engine/defines.h"
#include "game.h"
#include <list>

int32 Game::run()
{
	// Load textures
	Texture objectTexture = Texture();
	if (!objectTexture.LoadTextureFromBMP("texture_16x32.bmp"))
	{
		// error
		return -1;
	};

	// Load wavefront files
	//objectMesh.LoadTestCube();
	Mesh mesh1, mesh2;
	mesh1.LoadTestCube();
	mesh1.texture = &objectTexture;
	objectMeshes.push_back(mesh1);
	mesh2 = Mesh();
	mesh2.LoadTestCube();
	objectMeshes.push_back(mesh2);
	//objectMesh.LoadObjectFile("lowpolybuildings2.obj", true);
	//objectMesh.texture = &objectTexture;

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
			vCamera.setZero();
			vLookDir.setZero();
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
//			case VK_BACK:
//			{
//				if (!userTextBuffer.empty())
//				{
//					userTextBuffer.pop_back();
//				}
//			} break;// continue to default ...
//			default:
//			{
//				if (acceptedCharacters.find(e) != std::string::npos)
//				{
//					// If char in acceptedCharacters
//					userTextBuffer += std::string(1, e);
//				}
//			} break;
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

	double turningSpeed = 0.005f;
	double movementSpeed = 0.005f;

	Vector vForward = vLookDir * movementSpeed;

	vCamera	  += moveDirZ * vForward;		// forward / backward
	fYaw	  += moveDirX * turningSpeed;   // left / right
	vCamera.y += moveDirY * movementSpeed;  // up / down

	Matrix4x4 matrixRotX, matrixRotZ, matrixTranslation, matrixWorld;

	fTheta += 0.005f;  // rotate world
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

	//objectMeshes[0].

	/* ---------- Render ---------- */

	// Clear screen and depth buffer
	win.Gfx().ClearScreen(0x000000);
	win.Gfx().ClearDepthBuffer();

	// Draw objects in scene
	win.Gfx().RasterTriangles(matrixWorld, matrixCamera, projectionMatrix, vCamera, objectMeshes[0]);
	win.Gfx().RasterTriangles(matrixWorld, matrixCamera, projectionMatrix, vCamera, objectMeshes[1]);

	// Draw to window
	win.Gfx().Render();  // last
}
