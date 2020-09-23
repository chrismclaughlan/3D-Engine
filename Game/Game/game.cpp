#include "Engine/defines.h"
#include "Engine/objects.h"
#include "Engine/gui.h"
#include "game.h"
#include <list>

int Game::run()
{
	gsPush(&Game::gsCloseApplication);  // should be first in stack

	win.Gfx().text2D = new Text2D();
	if (win.Gfx().text2D->LoadTextMapFromBMP("Text2D_Font_252x108_monochrome.bmp"))
	{
		gsPush(&Game::gsMainMenu);
	}
	
	while (Window::processMessages())
	{
		if (gStates.empty())
		{
			break;
		}

		// Get current game state
		void (Game:: * gState)();
		gState = gStates.top();
		(this->*gState)();  // execute method

		/* ---------- Draw to window ---------- */
		win.Gfx().Render();  // last
	}

	return Window::getExitCode();
}

void Game::gsCloseApplication()
{
	gsPop();
	
	if (!gStates.empty())
	{
		std::cerr << "Application closing unexpectedly\n";
	}
}

void Game::gsInitGame()
{
	// Reset everything incase overwriting existing game

	if (guiChat != nullptr)
	{
		delete guiChat;
		guiChat = nullptr;
	}
	delete guiChat;
	guiChat = nullptr;

	if (object1 != nullptr)
	{
		delete object1;
		object1 = nullptr;
	}
	delete object1;
	object1 = nullptr;

	if (object2 != nullptr)
	{
		delete object2;
		object2 = nullptr;
	}
	delete object2;
	object2 = nullptr;

	player.resetCamera();
	player.resetPosition();


	// Load GUI
	guiChat = new GUIForm();
	const float x1 = 0.0f, y1 = 0.0f, x2 = 0.75f, y2 = 0.05f;
	uint tColours[3] = { 0xff0000, 0x00ff00, 0x0000ff };
	GUIText* guiTextInput = new GUIText("Chat : ", tColours, x1, y1, x2, y2, false);
	guiChat->setTextInput(guiTextInput);
	uint rColours[3] = { 0xaaaaaa, 0x444444, 0x888888 };
	GUIRect* guiRect = new GUIRect(x1, y1, x2, y2, rColours, guiTextInput);
	guiChat->setRect(guiRect);

	// Load textures
	objectTexture1 = new Texture(TextureType::RGB, "CubeMap_Test.bmp", 2, 2);
	objectTexture2 = new Texture(TextureType::RGB, "Object_Texture_16x32_24bit.bmp", 2, 2);

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

	gsPush(&Game::gsGame);
}

void Game::gsGame()
{
	gsGameInput();
	gsGameSimulate();
	gsGameRender();
}

void Game::gsGameInput()
{
	while (!win.keyboard.keyIsEmpty())
	{
		const auto event = win.keyboard.readKey();
		switch (event.getCode())
		{
		case VK_ESCAPE:
		{
			if (event.isReleased())
			{
				if (disableMovement)
				{
					// Escape typing
					CheckEscapeRect(guiChat->getRect());
				}
				else
				{
					// Open menu
					gsPush(&Game::gsGameMenu);
				}
			}

			//player.resetPosition();
			//player.resetCamera();
		} break;
		case 0x57:  // 'w'
		{
			if (disableMovement) break;  // disable movement

			if (event.isPressed())
				player.moveForward();
			else if (player.isMovingForward() && event.isReleased())
				player.moveForward(false);
		} break;
		case 0x53:  // 's'
		{
			if (disableMovement) break;  // disable movement

			if (event.isPressed())
				player.moveBackward();
			else if (player.isMovingBackward() && event.isReleased())
				player.moveBackward(false);
		} break;
		case 0x41:  // 'a'
		{
			if (disableMovement) break;  // disable movement

			if (event.isPressed())
				player.moveLeft();
			else if (player.isMovingLeft() && event.isReleased())
				player.moveLeft(false);
		} break;
		case 0x44:  // 'd'
		{
			if (disableMovement) break;  // disable movement

			if (event.isPressed())
				player.moveRight();
			else if (player.isMovingRight() && event.isReleased())
				player.moveRight(false);
		} break;
		case VK_SPACE:
		{
			if (disableMovement) break;  // disable movement

			if (event.isPressed())
				player.moveUpward();
			else if (player.isMovingUpward() && event.isReleased())
				player.moveUpward(false);
		} break;
		case 16:  // 0x16 (shift key)
		{
			if (disableMovement) break;  // disable movement

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
		case Mouse::Event::Type::LPressed:
		{
			vec2 v = { event.getX(), event.getY() };
			vec2f vf = win.Gfx().pxToScreen(v);

			// Hover chat box
			CheckLMousePressedRect(vf, guiChat->getRect());
		} break;
		case Mouse::Event::Type::RPressed:
		{
			win.hideCursor();
		} break;
		case Mouse::Event::Type::RReleased:
		{
			win.showCursor();

			vec2 v = { event.getX(), event.getY() };
			vec2f vf = win.Gfx().pxToScreen(v);

			// Click chat box
			disableMovement = false;
			CheckLMousePressedRect(vf, guiChat->getRect());
		} break;
		case Mouse::Event::Type::MoveRelative:
		{
			if (event.isRightPressed())
			{
				int iCenterX = win.Gfx().getWidth() / 2;
				int iCenterY = win.Gfx().getHeight() / 2;
				win.GetScreenPos(&iCenterX, &iCenterY);
				win.MoveMouse(iCenterX, iCenterY);
				player.lookX( event.getX());
				player.lookY(-event.getY());
			}
		} break;
		case Mouse::Event::Type::Move:
		{
			vec2 v = { event.getX(), event.getY() };
			vec2f vf = win.Gfx().pxToScreen(v);

			// Hover chat box
			CheckMouseMoveRect(vf, guiChat->getRect());
		} break;
		}
	}

	while (!win.keyboard.charIsEmpty())
	{
		const uint8 e = win.keyboard.readChar();

		switch (e)
		{
		case VK_RETURN:
		{
			std::string* s = nullptr;
			if (CheckReturnRect(guiChat->getRect(), s))
			{
				std::cout << "s : " << *s << "\n";
				delete s;     // temp : do something with string
				s = nullptr;
			}
		} break;
		case VK_BACK:
		{
			if (!disableMovement)
				break;
			if (!userTextBuffer->empty())
			{
				userTextBuffer->pop_back();
#ifdef DISPLAY_DEBUG_CONSOLE
				//std::cout << *userTextBuffer << "\n";
#endif
			}
		} break;// continue to default ...
		default:
		{
			if (!disableMovement)
				break;
			if (win.Gfx().text2D->acceptedChars.find(e) != std::string::npos)
			{
				// If char in acceptedCharacters
				*userTextBuffer += std::string(1, e);
#ifdef DISPLAY_DEBUG_CONSOLE

				//std::cout << *userTextBuffer << "\n";
#endif
			}
		} break;
		}
	}
}

void Game::gsGameSimulate()
{
	// Camera
	player.updatePosition();

	// Update world objects
	//fTheta += 0.001f;  // rotate world
	object1->updatePosition(fTheta);
	object2->updatePosition(fTheta);
}

void Game::gsGameRender()
{
	// Clear screen and depth buffer
	win.Gfx().clearScreen(0x000000);
	win.Gfx().clearDepthBuffer();

	// Draw objects in scene
	std::vector<Object*> objects;
	objects.push_back(object1);
	objects.push_back(object2);

	//const uint strokeColour = 0xffffff;
	win.Gfx().rasterTexturedTriangles(
		projectionMatrix, 
		player.getMCamera(), player.getVCamera(),
		objects);

	//win.Gfx().DrawText(*userTextBuffer, 100, 100, 0xffffff);  // test

	win.Gfx().drawGUIForm(guiChat);

	//win.Gfx().DrawPointP(win.Gfx().getWidth() / 2, win.Gfx().getHeight() / 2, 0xff0000);
}

void Game::gsMainMenu()
{
	if (mainMenu == nullptr)
	{
		mainMenu = new MainMenu();
	}

	/* ---------- Input ---------- */

	while (!win.mouse.isEmpty())
	{
		const auto event = win.mouse.read();
		switch (event.getType())
		{
		case Mouse::Event::Type::Move:
		{
			vec2 v = { event.getX(), event.getY() };
			vec2f vf = win.Gfx().pxToScreen(v);

			for (auto b : mainMenu->buttons)
			{
				if (b->sprite->isClickable(vf))
				{
					if (b->sprite->state == GUIState::Inactive)
					{
						b->sprite->state = GUIState::Hover;
					}
				}
				else
				{
					if (b->sprite->state == GUIState::Hover)
					{
						b->sprite->state = GUIState::Inactive;
					}
				}
			}
		} break;
		case Mouse::Event::Type::LPressed:
		{
			vec2 v = { event.getX(), event.getY() };
			vec2f vf = win.Gfx().pxToScreen(v);

			for (auto b : mainMenu->buttons)
			{				
				if (b->sprite->isClickable(vf))
				{
					b->sprite->state = GUIState::Active;
				}
			}
		} break;
		case Mouse::Event::Type::LReleased:
		{
			// find out if gui pressed
			vec2 v = { event.getX(), event.getY() };
			vec2f vf = win.Gfx().pxToScreen(v);

			for (auto b : mainMenu->buttons)
			{
				if (b->sprite->isClickable(vf))
				{
					if (b->sprite->state == GUIState::Active)
					{
						b->sprite->state = GUIState::Pressed;
						std::cout << "Pressed\n";
					}
				}
				else
				{
					b->sprite->state = GUIState::Inactive;
				}
			}
		}
		}
	}


	/* ---------- Simulate ---------- */

	for (auto b : mainMenu->buttons)
	{
		if (b->sprite->state == GUIState::Pressed)
		{
			switch (b->action)
			{
			case MenuAction::Start:
			{
				delete mainMenu;
				mainMenu = nullptr;
				gsPush(&Game::gsInitGame);
			} return;
			case MenuAction::Quit:
			{
				delete mainMenu;
				mainMenu = nullptr;
				gsPopUntil(&Game::gsCloseApplication);
			} return;
			case MenuAction::Continue:
			{
				delete mainMenu;
				mainMenu = nullptr;
				gsPush(&Game::gsGame);
			} return;
			default:
			{
				std::cerr << "MenuAction not recognised\n";
			} break;
			}
		}
	}

	/* ---------- Render ---------- */

	win.Gfx().clearScreen(0xCDCDCD);

	for (auto b : mainMenu->buttons)
	{
		win.Gfx().drawGUIMenuSprite(b->sprite);
	}
}

void Game::gsGameMenu()
{
	if (guiGameMenu == nullptr)
	{
		guiGameMenu = new GUIMenu();

		uint rColours[3] = { 0xaaaaaa, 0x444444, 0x888888 };
		GUIRect* guiRect = new GUIRect(0.4f, 0.45f, 0.6f, 0.55f, rColours, nullptr);
		guiGameMenu->setRect(guiRect);

		uint tColours[3] = { 0xdddddd, 0xf362d4, 0xffffff };
		GUIText* item1 = new GUIText("Continue", tColours, 0.45f, 0.5f);
		GUIText* item2 = new GUIText("Quit", tColours, 0.45f, 0.45f);
		guiGameMenu->addText(item1);
		guiGameMenu->addText(item2);
	}

	/* ---------- Input ---------- */

	while (!win.keyboard.keyIsEmpty())
	{
		const auto event = win.keyboard.readKey();
		switch (event.getCode())
		{
		case VK_ESCAPE:
		{
			if (event.isReleased())
			{
				delete guiGameMenu;
				guiGameMenu = nullptr;
				gsPop();
				return;
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
			vec2 v = { event.getX(), event.getY() };
			vec2f vf = win.Gfx().pxToScreen(v);
			for (auto t : guiGameMenu->getVText())
			{
				CheckMouseMoveText(vf, t);
			}
			//#if DISPLAY_DEBUG_CONSOLE && DISPLAY_MOUSE_COORDS
			//			std::cout << event.getX() << " " << event.getY() << "\n";
			//#endif
		} break;
		case Mouse::Event::Type::LReleased:
		{
			// find out if gui pressed
			vec2 v = { event.getX(), event.getY() };
			vec2f vf = win.Gfx().pxToScreen(v);
			for (auto t : guiGameMenu->getVText())
			{
				CheckMousePressedText(vf, t);
			}
		}
		}
	}

	/* ---------- Simulate ---------- */

	for (auto t : guiGameMenu->getVText())
	{
		if (t->state == GUIState::Active)
		{
			if (t->sText == "Continue")  // TODO enum?
			{
				//resetGUIForm(*guiGameMenu);
				delete guiGameMenu;
				guiGameMenu = nullptr;
				gsPop();
				return;
			}
			else if (t->sText == "Quit")  // TODO enum?
			{
				delete guiGameMenu;
				guiGameMenu = nullptr;
				//resetGUIForm(*guiGameMenu);
				gsPopUntil(&Game::gsMainMenu);
				return;
			}
		}
	}

	gsGameSimulate();

	/* ---------- Render ---------- */

	gsGameRender();

	win.Gfx().drawGUIMenu(guiGameMenu);
}