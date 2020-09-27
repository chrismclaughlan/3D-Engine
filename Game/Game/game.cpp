#include "Engine/defines.h"
#include "Engine/graphics_objects.h"
#include "Engine/graphics_ui.h"
#include "Engine/utils_vector.h"
#include "game.h"
#include <list>
#include <cmath>  // round() fps

/**
 * \brief Defines the maximum number of game states allowed in the game state
 * stack.
 */
#define MAX_GAME_STATE_SIZE 10

/**
 * \brief Flags to determine game state behaviour.
 */
#define FLAG_ERROR			(-1)
#define FLAG_RUN			(0)
#define FLAG_INIT			(1)
#define FLAG_DESTROY		(2)
#define FLAG_RESET			(3)


/* Game Methods */

Game::Game(const char* name, int width, int height)
	: win(name, width, height)
{
	userTextBuffer = new std::string;
}

Game::~Game()
{
	if (mainMenu != nullptr)
	{
		delete mainMenu;
		mainMenu = nullptr;
	}
	if (guiChat != nullptr)
	{
		delete guiChat;
		guiChat = nullptr;
	}
	if (guiGameMenu != nullptr)
	{
		delete guiGameMenu;
		guiGameMenu = nullptr;
	}

	delete userTextBuffer;
	delete object1;
	delete object2;
	delete pObjectTexture1;
	delete pObjectTexture2;
	object1 = nullptr;
	object2 = nullptr;
	pObjectTexture1 = nullptr;
	pObjectTexture2 = nullptr;
}

/**
 * \brief Initialises application.
 * 
 * Loads text textures, and initialises sGameStates.
 * 
 * Handles window message loop, runs the top element in sGameStates 
 * and calls the main window render function.
 * 
 * \return Returns exit code of application.
 */
const int Game::run()
{
	bool run = true;			///< Decides if game will run or close on 
								///< startup
	int status = FLAG_RUN;		///< Decides gsCloseApplication flag 
								///< (used to flag error happened)
	
	if (!win.setBigIcon("AppIcon.ico"))
	{
		status = FLAG_ERROR;	///< Set flag to error but keep game running
	}
	if (!win.setSmallIcon("AppIcon.ico"))
	{
		status = FLAG_ERROR;	///< Set flag to erorr but keep game running
	}

	win.Gfx().createSprite("ExampleTitleSprite.bmp", TextureType::RGB, 640, 360, { 0.1f, 0.1f }, { 0.9f, 0.9f });

	win.Gfx().text2D = new Text2D();
	if (!win.Gfx().text2D->LoadTextMapFromBMP("Text2D_Font_252x108_monochrome.bmp"))
	{
		status = FLAG_ERROR;
		run = false;			///< Game cannot continue without text2D
	}

	/* Initialise Bottom of Game Stack */
	gsPush(&Game::gsCloseApplication, status);  // always first

	/* Initialise Running Game States */
	if (run)
	{
		//win.initFrameTime();
		gsPush(&Game::gsMainMenu, FLAG_INIT);
	}

	while (Window::processMessages())
	{
		if (sGameStates.empty())
		{
			break;
		}

		// Get current game state
		std::pair<void (Game::*)(int), int> current = sGameStates.top();
		void (Game:: * currentState)(int) = current.first;
		int currentFlag = current.second;

		(this->*currentState)(currentFlag);  // execute method

		/* ---------- Draw to window ---------- */
		win.Gfx().Render();  // last

		/* --------- Update Frame time -------- */
		win.updateFrameTime();
	}

	return Window::getExitCode();
}


/* Game State Management Methods */

/**
 * \brief Pushes a game function and a corresponding flag to the 
 * sGameStates stack.
 * 
 * \param func Pointer to Game function.
 * \param flag Flag determining the behaviour the function should inherit.
 * \return Returns true if function pushed to stack.
 */
const bool Game::gsPush(void (Game::* func)(int), int flag)
{
	if (sGameStates.size() >= MAX_GAME_STATE_SIZE)
	{
		return false;
	}

	sGameStates.push({ func, flag });
	win.keyboard.flush();
	win.mouse.flush();

	return true;
}

/**
 * \brief Tries to pop function from sGameStates.
 *
 * If successfully popped, the keyboard and mouse inputs
 * are flushed to ensure fresh input for the new function.
 *
 * \return Returns true if function popped from sGameStates
 */
const bool Game::gsPop()
{
	if (sGameStates.empty())
	{
		return false;
	}

	// Cleanup of top stack function
	std::pair<void (Game::*)(int), int> current = sGameStates.top();
	(this->*current.first)(FLAG_DESTROY);  // execute method

	sGameStates.pop();  // remove from stack

	win.keyboard.flush();
	win.mouse.flush();

	return true;
}


/**
 * \brief Pops game functions from sGameStates until it encounters the given 
 * function.
 * 
 * If the intended element in the stack is hit, the keyboard and mouse inputs 
 * are flushed to ensure fresh input for the new function. The current flag 
 * corresponding to the function is then updated (default: FLAG_INIT so that the 
 * new game state can initialise itself).
 * 
 * If the intended element in the stack is not hit, an error message in cerr
 * is written and false is returned.
 * 
 * \important The game states popped from this function will be called with 
 * FLAG_DESTROY before being popped to let the function cleanup.
 * 
 * \param func Pointer to Game function to pop until.
 * \param flag Flag to pass on to the intended game function (default: FLAG_INIT).
 */
const bool Game::gsPopUntil(void (Game::* func)(int), const int flag = FLAG_INIT)
{
	bool hit = false;

	if (sGameStates.empty())
	{
		std::cerr << "Error popping game states -> Stack empty\n";
		return false;
	}

	while (true)
	{
		std::pair<void (Game::*)(int), int> current = sGameStates.top();

		// Check if hit intended function
		if (current.first == func)
		{
			gsUpdateCurrentFlag(flag);  // init gs again

			win.keyboard.flush();
			win.mouse.flush();
			hit = true;
			break;
		}

		//// Cleanup of top stack function
		//(this->*currentState)(FLAG_DESTROY);  // execute method

		// Pop top stack function
		if (!gsPop())
		{
			std::cerr << "Error popping game states -> End of stack hit: "\
				"cannot hit intended element\n";
			break;
		}
	}

	return hit;
}

/**
 * \brief Replaces the flag for the top stack element.
 * 
 * Replaces the top stack element with a new one with the same function 
 * pointer but different flag.
 * 
 * \param flag Flag to replace with.
 * \return Returns false if stack empty.
 */
const bool Game::gsUpdateCurrentFlag(const int flag)
{
	if (sGameStates.empty())
	{
		std::cerr << "Error updating current flag -> Stack empty\n";
		return false;
	}

	// Get current game state
	std::pair<void (Game::*)(int), int> current = sGameStates.top();
	void (Game:: * currentState)(int) = current.first;
	
	sGameStates.pop();
	sGameStates.push({currentState, flag});

	return true;
}

/**
 * \brief Cleans up current game state and pushes new one onto stack.
 *
 * \return True if successful.
 */
const bool Game::gsDestroyCurrentThenPush(void (Game::* func)(int), const int flag = FLAG_INIT)
{
	if (sGameStates.empty())
	{
		std::cerr << "Error pop-pushing -> Stack empty\n";
		return false;
	}

	// Get current game state
	std::pair<void (Game::*)(int), int> current = sGameStates.top();
	void (Game:: * currentState)(int) = current.first;
	(this->*currentState)(FLAG_DESTROY);  // execute method

	if (!gsPush(func, flag))
	{
		std::cerr << "Error pushing to game stack -> Stack full\n";
		return false;
	}

	return true;
}


/* Game State Functions */

/**
 * \brief Handles closing the application.
 * 
 * Typically placed at bottom of stack as it should be the last game state 
 * called. Should ensure the application is properly cleaned up.
 * 
 * \param flag Flag to determine behaviour (check if closing on error).
 */
void Game::gsCloseApplication(int flag)
{
	if (sGameStates.size() > 1)
	{
		std::cerr << "gsCloseApplication called early -> "\
			"Application closing unexpectedly\n";
		flag = FLAG_ERROR;
	}

	switch (flag)
	{
	case FLAG_DESTROY:
	{
		// Close application successfully
	} break;
	case FLAG_ERROR:
	{
		// Handle error flag
		std::cerr
			<< "Error at gsCloseApplpication -> FLAG_ERROR passed\n";
		win.setExitCode(-1);
	} break;
	default:
	{
		// Handle error flag
		std::cerr
			<< "Error at gsCloseApplpication -> Unknown flag:" << flag << "\n";
		win.setExitCode(-1);
	} break;
	}

	while (!sGameStates.empty())
	{
		sGameStates.pop();
	}
}

/**
 * \brief Handles the main game state.
 * 
 * When flag == FLAG_INIT: Will initialise game through glInit().
 * When flag == FLAG_RUN: Will call glInput(), glSimulate() and glRender().
 * When flag == FLAG_DESTROY: Will cleanup game through glDestroy().
 */
void Game::gsGame(const int flag)
{
	switch (flag)
	{
	case FLAG_INIT:
	{
		std::cout << "gsGame() -> FLAG_INIT\n";
		glInit();
		gsUpdateCurrentFlag(FLAG_RUN);
	} break;
	case FLAG_RUN:
	{
		glInput();
		glSimulate();
		glRender();
	} break;
	case FLAG_DESTROY:
	{
		std::cout << "gsGame() -> FLAG_DESTROY\n";
		glDestroy();
	} break;
	default:
	{
		std::cerr << "Error at gsGame() -> Unknown flag:" << flag << "\n";
		//gsUpdateCurrentFlag(FLAG_DESTORY);  // ?
	} break;
	}
}

/**
 * \brief Handles the main menu state.
 * 
 * When flag == FLAG_INIT: Will initialise create new MainMenu() object.
 * When flag == FLAG_RUN: Will call mMain().
 * When flag == FLAG_DESTROY: Will destroy the MainMenu() object.
 */
void Game::gsMainMenu(const int flag)
{
	switch (flag)
	{
	case FLAG_INIT:
	{
		std::cout << "gsMainMenu() -> FLAG_INIT\n";
		if (mainMenu == nullptr)
		{
			mainMenu = new MainMenu();
			gsUpdateCurrentFlag(FLAG_RUN);
		}
		else
		{
			std::cerr << "Cannot init new MainMenu -> One already exists\n";
			//gsUpdateCurrentFlag(FLAG_DESTORY);  // ?
		}
	} break;
	case FLAG_RUN:
	{
		mMain();
	} break;
	case FLAG_DESTROY:
	{
		std::cout << "gsMainMenu() -> FLAG_DESTROY\n";
		delete mainMenu;
		mainMenu = nullptr;
		return;
	} break;
	case FLAG_RESET:
	{
		std::cout << "gsMainMenu() -> FLAG_RESET\n";
		mainMenu->resetButtons();
		gsUpdateCurrentFlag(FLAG_RUN);
	} break;
	default:
	{
		std::cerr << "Error at gsMainMenu() -> Unknown flag:" << flag << "\n";
		//gsUpdateCurrentFlag(FLAG_DESTORY);  // ?
	} break;
	}

	// mMain();
}

/**
 * \brief Handles game menu state.
 * 
 * When flag == FLAG_INIT: Will initialise create new GUIMenu() object.
 * When flag == FLAG_RUN: Will call mGame().
 * When flag == FLAG_DESTROY: Will destroy the GUIMenu() object.
 */
void Game::gsGameMenu(const int flag)
{
	switch (flag)
	{
	case FLAG_INIT:
	{
		std::cout << "gsGameMenu() -> FLAG_INIT\n";
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

			gsUpdateCurrentFlag(FLAG_RUN);
		}
		else
		{
			std::cerr << "Cannot init new guiGameMenu -> One already exists\n";
			//gsUpdateCurrentFlag(FLAG_DESTORY);  // ?
		}
	} break;
	case FLAG_RUN:
	{
		mGame();
	} break;
	case FLAG_DESTROY:
	{
		std::cout << "gsGameMenu() -> FLAG_DESTROY\n";
		delete guiGameMenu;
		guiGameMenu = nullptr;
		return;
	} break;
	default:
	{
		std::cerr << "Error at gsGameMenu() -> Unkonwn flag\n";
		//gsUpdateCurrentFlag(FLAG_DESTORY);  // ?
	} break;
	}

	// mGame();
}


/* Game Loop Methods */

/**
 * \brief Initialises nessessary objects for Game loop.
 */
void Game::glInit()
{
	// TODO if error occurs initialising: return error
	//      and let glInitialised = false;

	if (glInitialised)
	{
		// error game already running
		// TODO overwrite? or return error
		return;
	}

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
	pObjectTexture1 = new Texture(TextureType::RGB, "CubeMap_Test.bmp", 2, 2);
	pObjectTexture2 = new Texture(TextureType::RGB, "Object_Texture_16x32_24bit.bmp", 2, 2);
	
	// Load wavefront files
	object1 = new Object();
	object1->LoadTestCube();
	object1->pTexture = pObjectTexture1;
	object1->setPos(-2.0f, 2.0f, 5.0f);
	object2 = new Object();
	object2->LoadTestCube();
	object2->pTexture = pObjectTexture2;
	object2->setPos(2.0f, 2.0f, 5.0f);

	projectionMatrix.MakeProjection(
		90.0f,
		(float)win.Gfx().getHeight() / (float)win.Gfx().getWidth(),
		0.1f,
		1000.0f);

	glInitialised = true;
}

/**
 * \brief Destroys all allocated objects for Game loop.
 */
void Game::glDestroy()
{
	glInitialised = false;
	std::cout << "Destroying game...\n";
}

/**
 * \brief Handles input for Game loop.
 * \see Called by gsGame().
 */
void Game::glInput()
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
					gsPush(&Game::gsGameMenu, FLAG_INIT);
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
			object1->vPos.y += 0.1f;
		} break;
		case VK_DOWN:
		{
			object1->vPos.y -= 0.1f;
		} break;
		case VK_LEFT:
		{
			object1->vPos.x += 0.1f;
		} break;
		case VK_RIGHT:
		{
			object1->vPos.x -= 0.1f;
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
			Vec2 v = { event.getX(), event.getY() };
			Vec2f vf = win.Gfx().pxToScreen(v);

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

			Vec2 v = { event.getX(), event.getY() };
			Vec2f vf = win.Gfx().pxToScreen(v);

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
				player.lookX((float) event.getX());
				player.lookY((float)-event.getY());
			}
		} break;
		case Mouse::Event::Type::Move:
		{
			Vec2 v = { event.getX(), event.getY() };
			Vec2f vf = win.Gfx().pxToScreen(v);

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

/**
 * \brief Simulates objects for Game loop.
 * \see Called by gsGame().
 */
void Game::glSimulate()
{
	// Camera
	player.updatePosition(win.lastDT);

	// Update world objects
	//fTheta += 0.001f;  // rotate world
	object1->updatePosition(fTheta);
	object2->updatePosition(fTheta);
}

/**
 * \brief Renders objects for Game loop.
 * \see Called by gsGame().
 */
void Game::glRender()
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

	win.Gfx().drawFPS(1.0f / win.lastDT, 0x0fffff);

	win.Gfx().drawGUIForm(guiChat);

	//win.Gfx().DrawPointP(win.Gfx().getWidth() / 2, win.Gfx().getHeight() / 2, 0xff0000);
}


/* Menu Methods */

/**
 * \brief Runs input/simulate/render loop for the main menu.
 * 
 * \note mMain stands for menu-main.
 * \see MainMenu.
 */
void Game::mMain()
{
	/* ---------- Input ---------- */

	while (!win.mouse.isEmpty())
	{
		const auto event = win.mouse.read();
		switch (event.getType())
		{
		case Mouse::Event::Type::Move:
		{
			Vec2 v = { event.getX(), event.getY() };
			Vec2f vf = win.Gfx().pxToScreen(v);

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
			Vec2 v = { event.getX(), event.getY() };
			Vec2f vf = win.Gfx().pxToScreen(v);

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
			Vec2 v = { event.getX(), event.getY() };
			Vec2f vf = win.Gfx().pxToScreen(v);

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
				gsDestroyCurrentThenPush(&Game::gsGame);
				//delete mainMenu;
				//mainMenu = nullptr;
				//gsPush(&Game::gsGame, FLAG_INIT);
			} return;
			case MenuAction::Quit:
			{
				gsPopUntil(&Game::gsCloseApplication, FLAG_DESTROY);
			} return;
			case MenuAction::Continue:
			{
				if (!glInitialised)
				{
					std::cerr << "Error: cannot 'Continue' "\
						"-> Game is not initialised\n";
					gsUpdateCurrentFlag(FLAG_RESET);
				}
				else
				{
					gsDestroyCurrentThenPush(&Game::gsGame, FLAG_RUN);
					//delete mainMenu;
					//mainMenu = nullptr;
					//gsPush(&Game::gsGame, FLAG_RUN);
				}
			} return;
			default:
			{
				std::cerr << "MenuAction not recognised\n";
			} break;
			}
		}
	}

	/* ---------- Render ---------- */

	win.Gfx().clearScreen(0xff0000);

	//win.Gfx().drawGUIMenuSprite(mainMenu->background);

	//win.Gfx().drawColourBuffer(colour)

	//win.Gfx().drawColourBuffer(sprite->pData, sprite->vf1, sprite->vf2);

	win.Gfx().drawSprites();

	for (auto b : mainMenu->buttons)
	{
		win.Gfx().drawGUIMenuSprite(b->sprite);
	}

	win.Gfx().drawFPS(1.0f / win.lastDT, 0x0fffff);
}

/**
 * \brief Runs input/simulate/render loop for the game menu.
 *
 * \note mGame stands for game-main.
 * \see GUIMenu.
 */
void Game::mGame()
{
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
				//delete guiGameMenu;
				//guiGameMenu = nullptr;
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
			Vec2 v = { event.getX(), event.getY() };
			Vec2f vf = win.Gfx().pxToScreen(v);
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
			Vec2 v = { event.getX(), event.getY() };
			Vec2f vf = win.Gfx().pxToScreen(v);
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
				//delete guiGameMenu;
				//guiGameMenu = nullptr;
				gsPop();
				return;
			}
			else if (t->sText == "Quit")  // TODO enum?
			{
				//resetGUIForm(*guiGameMenu);
				gsPopUntil(&Game::gsMainMenu);
				return;
			}
		}
	}

	glSimulate();

	/* ---------- Render ---------- */

	glRender();

	win.Gfx().drawGUIMenu(guiGameMenu);
}


/* Old GUI Collision Checks */

bool Game::CheckReturnRect(GUIRect* r, std::string*& s)
{
	if (r->state == GUIState::Active && r->pGUITextInput != nullptr)
	{
		r->state = GUIState::Inactive;
		s = r->pGUITextInput->pText;
		r->pGUITextInput->pText = nullptr;
		userTextBuffer = new std::string;
		disableMovement = false;
		return true;
	}

	return false;
}

void Game::CheckEscapeRect(GUIRect* r)
{
	if (r->pGUITextInput != nullptr)
	{
		userTextBuffer->clear();
		if (r->state == GUIState::Active)
			r->state = GUIState::Inactive;
		r->pGUITextInput->pText = nullptr;
		disableMovement = false;
	}
}

void Game::CheckMouseMoveRect(const Vec2f& vf, GUIRect* r)
{
	if (r->state != GUIState::Active)
	{
		if (r->isAt(vf.x, vf.y))
		{
			r->state = GUIState::Hover;
		}
		else
		{
			r->state = GUIState::Inactive;
		}
	}
}

void Game::CheckLMousePressedRect(const Vec2f& vf, GUIRect* r)
{
	if (r->pGUITextInput != nullptr)
	{
		if (r->isAt(vf.x, vf.y))
		{
			if (r->state != GUIState::Active)
			{
				r->state = GUIState::Active;
				r->pGUITextInput->pText = userTextBuffer;
				disableMovement = true;
			}
			else
			{
				r->state = GUIState::Inactive;
				r->pGUITextInput->pText = nullptr;
			}
		}
		else
		{
			r->state = GUIState::Inactive;
			r->pGUITextInput->pText = nullptr;
		}
	}
}

void Game::CheckMouseMoveText(const Vec2f& vf, GUIText* t)
{
	if (t->isAt(vf.x, vf.y, &win.Gfx()))
	{
		if (t->state != GUIState::Active)
			t->state = GUIState::Hover;
	}
	else
	{
		t->state = GUIState::Inactive;
	}
}

void Game::CheckMousePressedText(const Vec2f& vf, GUIText* t)
{
	if (t->isAt(vf.x, vf.y, &win.Gfx()))
	{
		t->state = GUIState::Active;
	}
}