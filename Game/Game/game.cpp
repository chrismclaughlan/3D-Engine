#include "Engine/defines.h"
#include "Engine/objects.h"
#include "Engine/gui.h"
#include "game.h"
#include <list>

int32 Game::run()
{
	GUITextInput* guiTopBarTextInput = new GUITextInput(-1.0f, 0.9f, 1.0f, 1.0f, nullptr, 0x00ff00);
	GUIRect* guiTopBar = new GUIRect(-1.0f, 0.9f, 1.0f, 1.0f, 0x0000ff, guiTopBarTextInput);
	guiCurrent.addRect(guiTopBar);
	guiCurrent.addTextInput(guiTopBarTextInput);

	win.Gfx().text2D = new Text2D();
	win.Gfx().text2D->LoadTextMapFromBMP("texture_font_252x108_monochrome.bmp");

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
	object1 = new Object();
	object1->LoadTestCube();
	object1->texture = objectTexture1;
	object1->setPos(-2.0f, 2.0f, 5.0f);
	object2 = new Object();
	object2->LoadTestCube();
	object2->texture = objectTexture2;
	object2->setPos(2.0f, 2.0f, 5.0f);

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
			for (auto r : guiCurrent.getRects())
			{
				if (r->guiTextInput != nullptr)
				{
					userTextBuffer->clear();
					r->guiTextInput->text = nullptr;
					enableWriting = false;
				}
			}

			player.resetPosition();
			player.resetCamera();
		} break;
		case 0x57:  // 'w'
		{
			if (event.isPressed())
				player.moveForward();
			else if (player.isMovingForward() && event.isReleased())
				player.moveForward(false);
		} break;
		case 0x53:  // 's'
		{
			if (event.isPressed())
				player.moveBackward();
			else if (player.isMovingBackward() && event.isReleased())
				player.moveBackward(false);
		} break;
		case 0x41:  // 'a'
		{
			if (event.isPressed())
				player.moveLeft();
			else if (player.isMovingLeft() && event.isReleased())
				player.moveLeft(false);
		} break;
		case 0x44:  // 'd'
		{
			if (event.isPressed())
				player.moveRight();
			else if (player.isMovingRight() && event.isReleased())
				player.moveRight(false);
		} break;
		case VK_SPACE:
		{
			if (event.isPressed())
				player.moveUpward();
			else if (player.isMovingUpward() && event.isReleased())
				player.moveUpward(false);
		} break;
		case 16:  // 0x16 (shift key)
		{
			if (event.isPressed())
				player.moveDownward();
			else if (player.isMovingDownward() && event.isReleased())
				player.moveDownward(false);
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
			// find out if gui pressed
			int32 iX = event.getX();
			int32 iY = event.getY();
			float fX = win.Gfx().pxToScreenX(iX);
			float fY = win.Gfx().pxToScreenY(iY);
			std::cout << "Lpressed at x = " << fX << " y = " << fY << "\n";
			for (auto r : guiCurrent.getRects())
			{
				if (r->isAt(fX, fY))
				{
					if (r->guiTextInput != nullptr)
					{
						r->guiTextInput->text = userTextBuffer;
						enableWriting = true;
					}
				}
			}
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
				if (!enableWriting)
					break;
				if (!userTextBuffer->empty())
				{
					userTextBuffer->pop_back();
#ifdef DISPLAY_DEBUG_CONSOLE

					std::cout << *userTextBuffer << "\n";
#endif
				}
			} break;// continue to default ...
			default:
			{
				if (!enableWriting)
					break;
				if (win.Gfx().text2D->acceptedChars.find(e) != std::string::npos)
				{
					// If char in acceptedCharacters
					*userTextBuffer += std::string(1, e);
#ifdef DISPLAY_DEBUG_CONSOLE

					std::cout << *userTextBuffer << "\n";
#endif
				}
			} break;
		}
	}
}

void Game::DoFrame()
{
	/* ---------- Simulate ---------- */

	// Camera
	player.updatePosition();

	// Update world objects
	//fTheta += 0.001f;  // rotate world
	object1->updatePosition(fTheta);
	object2->updatePosition(fTheta);

	/* ---------- Render ---------- */

	// Clear screen and depth buffer
	win.Gfx().ClearScreen(0x000000);
	win.Gfx().ClearDepthBuffer();

	// Draw objects in scene
	std::vector<Object*> objects;
	objects.push_back(object1);
	objects.push_back(object2);

	//const uint32 strokeColour = 0xffffff;
	win.Gfx().RasterTexturedTriangles(
		projectionMatrix, 
		player.getMCamera(), player.getVCamera(),
		objects);

	//win.Gfx().DrawText(userTextBuffer, 100, 100);

	win.Gfx().DrawGUILayout(&guiCurrent);

	/* ---------- Draw to window ---------- */

	win.Gfx().Render();  // last
}
