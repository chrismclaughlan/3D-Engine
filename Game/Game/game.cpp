#include "Engine/defines.h"
#include "Engine/objects.h"
#include "Engine/gui.h"
#include "game.h"
#include <list>

int32 Game::run()
{
	if (initText() == 0)
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

		//HandleInput();
		//DoFrame();

		/* ---------- Draw to window ---------- */

		win.Gfx().Render();  // last
	}

	return Window::getExitCode();
}

const int32 Game::initText()
{
	win.Gfx().text2D = new Text2D();
	win.Gfx().text2D->LoadTextMapFromBMP("texture_font_252x108_monochrome.bmp");
	
	return 0;
}

const int32 Game::initGame()
{
	// Load GUI
	guiChat = new GUIForm();
	const float x1 = 0.0f, y1 = 0.0f, x2 = 0.75f, y2 = 0.05f;
	uint32 tColours[3] = { 0xff0000, 0x00ff00, 0x0000ff };
	GUIText* guiTextInput = new GUIText("Chat : ", tColours, x1, y1, x2, y2, false);
	guiChat->setTextInput(guiTextInput);
	uint32 rColours[3] = { 0xaaaaaa, 0x444444, 0x888888 };
	GUIRect* guiRect = new GUIRect(x1, y1, x2, y2, rColours, guiTextInput);
	guiChat->setRect(guiRect);

	// Load textures
	objectTexture1 = new Texture24();
	objectTexture2 = new Texture24();
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

	return 0;
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
			float fX = win.Gfx().pxToScreenX(event.getX());
			float fY = win.Gfx().pxToScreenY(event.getY());

			// Hover chat box
			CheckLMousePressedRect(fX, fY, guiChat->getRect());
		} break;
		case Mouse::Event::Type::RPressed:
		{
			win.hideCursor();
		} break;
		case Mouse::Event::Type::RReleased:
		{
			win.showCursor();

			float fX = win.Gfx().pxToScreenX(event.getX());
			float fY = win.Gfx().pxToScreenY(event.getY());

			// Click chat box
			disableMovement = false;
			CheckLMousePressedRect(fX, fY, guiChat->getRect());
		} break;
		case Mouse::Event::Type::MoveRelative:
		{
			if (event.isRightPressed())
			{
				int32 iCenterX = win.Gfx().getWidth() / 2;
				int32 iCenterY = win.Gfx().getHeight() / 2;
				win.GetScreenPos(&iCenterX, &iCenterY);
				win.MoveMouse(iCenterX, iCenterY);
				player.lookX( event.getX());
				player.lookY(-event.getY());
			}
		} break;
		case Mouse::Event::Type::Move:
		{
			float fX = win.Gfx().pxToScreenX(event.getX());
			float fY = win.Gfx().pxToScreenY(event.getY());
		
			// Hover chat box
			CheckMouseMoveRect(fX, fY, guiChat->getRect());
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

	//win.Gfx().DrawText(*userTextBuffer, 100, 100, 0xffffff);  // test

	win.Gfx().DrawGUIForm(guiChat);

	win.Gfx().DrawPointP(win.Gfx().getWidth() / 2, win.Gfx().getHeight() / 2, 0xff0000);
}

void Game::gsMainMenu()
{
	/* GUIForm */
	//if (guiMainMenu == nullptr)
	//{
		//guiMainMenu = new GUIMenu();

		//uint32 rColours[3] = { 0xaaaaaa, 0x444444, 0x888888 };
		//GUIRect* guiRect = new GUIRect(0.4f, 0.45f, 0.6f, 0.55f, rColours, nullptr);
		//guiMainMenu->setRect(guiRect);

		//uint32 tColours[3] = { 0xdddddd, 0xf362d4, 0xffffff };
		//GUIText* item1 = new GUIText("Start", tColours, 0.45f, 0.5f);
		//GUIText* item2 = new GUIText("Quit", tColours, 0.45f, 0.45f);
		//guiMainMenu->addText(item1);
		//guiMainMenu->addText(item2);
	//}

	/* SPRITES */
	if (guiSprite == nullptr)
	{
		//Texture24* guiSpriteTexture = new Texture24();
		//const char* filename1 = "Texture_MainMenu.bmp";
		//if (!guiSpriteTexture->LoadTextureFromBMP(filename1))
		//{
		//	std::cerr << "Error loading " << filename1 << "\n";
		//}

		Texture32* guiSpriteTexture32 = new Texture32();
		const char* filename2 = "Texture_Start.bmp";
		if (!guiSpriteTexture32->LoadTextureFromBMP(filename2))
		{
			std::cerr << "Error loading " << filename2 << "\n";
		}

		std::vector< GUISpriteClickable> vClickable;
		vClickable.push_back(GUISpriteClickable(ClickableColours::Start));
		vClickable.push_back(GUISpriteClickable(ClickableColours::Quit));

		guiSprite = new GUISprite32(guiSpriteTexture32, 0.4f, 0.4f, 0.6f, 0.6f, vClickable);
	}

	/* ---------- Input ---------- */

	while (!win.mouse.isEmpty())
	{
		const auto event = win.mouse.read();
		switch (event.getType())
		{
		case Mouse::Event::Type::Move:
		{
			int32 iX = event.getX();
			int32 iY = event.getY();
			float fX = win.Gfx().pxToScreenX(iX);
			float fY = win.Gfx().pxToScreenY(iY);
			if (guiSprite->isClickable(fX, fY))
			{
				if (guiSprite->state == GUIState::Inactive)
				{
					guiSprite->state = GUIState::Hover;
					std::cout << "Hover\n";
				}
			}
			else
			{
				if (guiSprite->state == GUIState::Hover)
				{
					guiSprite->state = GUIState::Inactive;
					std::cout << "x=" << iX << " y=" << iY << "\n";
				}
			}
		} break;
		case Mouse::Event::Type::LPressed:
		{
			int32 iX = event.getX();
			int32 iY = event.getY();
			float fX = win.Gfx().pxToScreenX(iX);
			float fY = win.Gfx().pxToScreenY(iY);
			if (guiSprite->isClickable(fX, fY))
			{
				guiSprite->state = GUIState::Active;
				std::cout << "Active\n";
			}

		} break;
		case Mouse::Event::Type::LReleased:
		{
			// find out if gui pressed
			int32 iX = event.getX();
			int32 iY = event.getY();
			float fX = win.Gfx().pxToScreenX(iX);
			float fY = win.Gfx().pxToScreenY(iY);
			if (guiSprite->isClickable(fX, fY))
			{
				if (guiSprite->state == GUIState::Active)
				{
					guiSprite->state = GUIState::Pressed;
					std::cout << "Pressed\n";
				}
			}
			else
			{
				guiSprite->state = GUIState::Inactive;
			}

			//ClickableColours cc;
			//if (guiSprite->isClickable(fX, fY, cc))
			//{
			//	switch (cc)
			//	{
			//	case ClickableColours::Start:
			//	{
			//		delete guiSprite;
			//		guiSprite = nullptr;
			//		initGame();
			//		gsPush(&Game::gsGame);
			//		return;
			//	} break;
			//	case ClickableColours::Quit:
			//	{
			//		delete guiSprite;
			//		guiSprite = nullptr;
			//		gsPop();
			//		return;
			//	} break;
			//	}
			//}
		}
		}
	}


	/* ---------- Simulate ---------- */

	//for (auto t : guiMainMenu->getVText())
	//{
	//	if (t->state == GUI_STATE_ACTIVE)
	//	{
	//		if (t->sText == "Start")  // TODO enum?
	//		{
	//			delete guiMainMenu;
	//			guiMainMenu = nullptr;
	//			initGame();
	//			gsPush(&Game::gsGame);
	//			return;
	//		}
	//		else if (t->sText == "Quit")  // TODO enum?
	//		{
	//			delete guiMainMenu;
	//			guiMainMenu = nullptr;
	//			gsPop();
	//			return;
	//		}
	//	}
	//}

	/* ---------- Render ---------- */

	win.Gfx().ClearScreen(0x000000);

	//win.Gfx().DrawGUIMenu(guiMainMenu);

	win.Gfx().DrawGUIMenuSprite(guiSprite);
}

void Game::gsGameMenu()
{
	if (guiGameMenu == nullptr)
	{
		guiGameMenu = new GUIMenu();

		uint32 rColours[3] = { 0xaaaaaa, 0x444444, 0x888888 };
		GUIRect* guiRect = new GUIRect(0.4f, 0.45f, 0.6f, 0.55f, rColours, nullptr);
		guiGameMenu->setRect(guiRect);

		uint32 tColours[3] = { 0xdddddd, 0xf362d4, 0xffffff };
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
			int32 iX = event.getX();
			int32 iY = event.getY();
			float fX = win.Gfx().pxToScreenX(iX);
			float fY = win.Gfx().pxToScreenY(iY);
			for (auto t : guiGameMenu->getVText())
			{
				CheckMouseMoveText(fX, fY, t);
			}
			//#if DISPLAY_DEBUG_CONSOLE && DISPLAY_MOUSE_COORDS
			//			std::cout << event.getX() << " " << event.getY() << "\n";
			//#endif
		} break;
		case Mouse::Event::Type::LReleased:
		{
			// find out if gui pressed
			int32 iX = event.getX();
			int32 iY = event.getY();
			float fX = win.Gfx().pxToScreenX(iX);
			float fY = win.Gfx().pxToScreenY(iY);
			for (auto t : guiGameMenu->getVText())
			{
				CheckMousePressedText(fX, fY, t);
			}
		}
		}
	}

	/* ---------- Simulate ---------- */

	for (auto t : guiGameMenu->getVText())
	{
		if (t->state == GUI_STATE_ACTIVE)
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

	win.Gfx().DrawGUIMenu(guiGameMenu);
}