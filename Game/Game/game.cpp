#include "Engine/defines.h"
#include "game.h"
#include <list>

int32 Game::run()
{
	// Load textures
	objectTexture1 = new Texture();
	objectTexture2 = new Texture();
	if (!objectTexture1->LoadTextureFromBMP("texture_16x32.bmp"))
	{
		// error
		return -1;
	};
	if (!objectTexture2->LoadTextureFromBMP("texture_32x32.bmp"))
	{
		// error
		return -1;
	};

	// Load wavefront files
	object1 = new Mesh();
	object1->LoadTestCube();
	object1->texture = objectTexture1;
	object2 = new Mesh();
	object2->LoadObjectFile("axis.obj", false);
	object2->texture = objectTexture2;
	//mesh1.LoadObjectFile("teapot.obj", false);
	//mesh1.texture = &objectTexture;
	//objectMeshes.push_back(mesh1);

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
		case 16:  // 0x16 (shift key)
		{
			if (event.isPressed())
				moveDirY = -1.0f;
			else if (moveDirY < 0 && event.isReleased())
				moveDirY = 0.0f;
		} break;
		case VK_UP:
		{
			object1->y += 0.1f;
		} break;
		case VK_DOWN:
		{
			object1->y -= 0.1f;
		} break;
		case VK_LEFT:
		{
			object1->x += 0.1f;
		} break;
		case VK_RIGHT:
		{
			object1->x -= 0.1f;
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

	// Camera
	double turningSpeed = 0.005f;
	double movementSpeed = 0.005f;

	Vector vUp(0.0f, 1.0f, 0.0f);
	Vector vTarget(0.0f, 0.0f, 1.0f);
	Vector vForward = vLookDir * movementSpeed;
	Matrix4x4 matrixCameraRotation, matrixCamera;  // matrixView

	// Camera control
	vCamera	  += moveDirZ * vForward;		// forward / backward
	fYaw	  += moveDirX * turningSpeed;   // left / right
	vCamera.y += moveDirY * movementSpeed;  // up / down
	matrixCameraRotation.MakeRotationY(fYaw);
	vLookDir = matrixCameraRotation * vTarget;
	vTarget = vCamera + vLookDir;
	matrixCamera.MakePointAt(vCamera, vTarget, vUp);
	matrixCamera.MakeQuickInverse();

	// Update world objects
	//fTheta += 0.001f;  // rotate world
	object1->updatePosition(fTheta);
	object2->updatePosition(fTheta);

	/* ---------- Render ---------- */

	// Clear screen and depth buffer
	win.Gfx().ClearScreen(0x000000);
	win.Gfx().ClearDepthBuffer();

	// Draw objects in scene
	std::vector<Mesh*> objects;
	objects.push_back(object1);
	objects.push_back(object2);

	const uint32 strokeColour = 0xffffff;
	win.Gfx().RasterTexturedTriangles(
		matrixCamera, projectionMatrix, vCamera,
		objects, nullptr);

	// Draw to window
	win.Gfx().Render();  // last
}
