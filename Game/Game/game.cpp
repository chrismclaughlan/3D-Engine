#include "Engine/defines.h"
#include "Engine/graphics_objects.h"
#include "Engine/graphics_ui.h"
#include "Engine/utils_vector.h"
#include "game.h"
#include <list>
#include <cmath>  // round() fps
#include <sstream>  // inventory

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
		std::cerr << "gsGame() -> FLAG_INIT\n";
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
		std::cerr << "gsGame() -> FLAG_DESTROY\n";
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
		std::cerr << "gsMainMenu() -> FLAG_INIT\n";
		if (mainMenu == nullptr)
		{
			mainMenu = new MainMenu(win.Gfx());
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
		std::cerr << "gsMainMenu() -> FLAG_DESTROY\n";
		delete mainMenu;
		mainMenu = nullptr;
		return;
	} break;
	case FLAG_RESET:
	{
		std::cerr << "gsMainMenu() -> FLAG_RESET\n";
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
		std::cerr << "gsGameMenu() -> FLAG_INIT\n";
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

			lockMouse = false;
			win.showCursor();

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
		std::cerr << "gsGameMenu() -> FLAG_DESTROY\n";

		lockMouse = true;
		win.hideCursor();

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

	player.reset();
	player.place(5.0f, 30.0f, 5.0f);

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
	pTextureDirt = new Texture(TextureType::RGB, "cubemap_dirt.bmp", 16, 16);
	pTextureGrass = new Texture(TextureType::RGB, "cubemap_grass.bmp", 16, 16);
	pTextureStone = new Texture(TextureType::RGB, "cubemap_stone.bmp", 16, 16);


	// Populate world with objects
	worldCoords = new Object*[game_settings.world_num_objects_x * game_settings.world_num_objects_y * game_settings.world_num_objects_z];

	for (int z = 0; z < game_settings.world_num_objects_z; z++)
	{
		for (int y = 0; y < game_settings.world_num_objects_y; y++)
		{
			for (int x = 0; x < game_settings.world_num_objects_x; x++)
			{
				// TODO cube / block classs
				Object* o = new Object();
				std::string name;
				if (y < game_settings.world_num_objects_y / 2)
				{
					name = "Stone";
					o->pTexture = pTextureStone;
				}
				else
				{
					name = "Dirt";
					o->pTexture = pTextureDirt;
				}
				o->LoadTestCube(name);
				o->setPos((float)x, (float)y, (float)z);
				//worldCoords[x + game_settings.world_num_objects_x * (y + game_settings.world_num_objects_z * z)] = o;
				setWorldObject(o, x, y, z);

				//int index = x + game_settings.world_num_objects_x * (y + game_settings.world_num_objects_z * z);
				//worldCoords[index].LoadTestCube("TODOreplaceme");
				//worldCoords[index].pTexture = pObjectTexture2;
				//worldCoords[index].setPos((float)x - 0.5f, (float)y - 0.5f, (float)z - 0.5f);
				//worldCoords[index].setPos((float)x - 0.5f, (float)y - 0.5f, (float)z - 0.5f);
			}
		}
	}

	
	// Load wavefront files
	//object1 = new Object();
	//object1->LoadTestCube("Cube1");
	//object1->pTexture = pObjectTexture1;
	//object1->setPos(-2.0f, 2.0f, 5.0f);
	//object2 = new Object();
	//object2->LoadTestCube("Cube2");
	//object2->pTexture = pObjectTexture2;
	//object2->setPos(2.0f, 2.0f, 5.0f);

	projectionMatrix.MakeProjection(
		90.0f,
		(float)win.Gfx().getHeight() / (float)win.Gfx().getWidth(),
		0.1f,
		1000.0f);

	win.Gfx().createSprite("HUD.bmp", TextureType::RGBA, 640, 360, { 0.0f, 0.0f }, { 1.0f, 1.0f });  // test

	win.hideCursor();

	glInitialised = true;
}

/**
 * \brief Destroys all allocated objects for Game loop.
 */
void Game::glDestroy()
{
	glInitialised = false;

	for (int i = 0; i < game_settings.world_num_objects_x * game_settings.world_num_objects_y * game_settings.world_num_objects_z; i++)
	{
		delete worldCoords[i];
		worldCoords[i] = nullptr;
	}
	delete[] worldCoords;
	worldCoords = nullptr;

	delete pTextureDirt;
	pTextureDirt = nullptr;
	delete pTextureGrass;
	pTextureGrass = nullptr;
	delete guiChat;
	guiChat = nullptr;

	win.Gfx().destroySprites();
	win.showCursor();
	std::cerr << "Destroying game...\n";
}

/**
 * \brief Handles input for Game loop.
 * \see Called by gsGame().
 */
void Game::glInput()
{
	player.action = PlayerActions::Invalid;  // Reset

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
				player.moveJump();
			else if (player.isJumping() && event.isReleased())
				player.moveJump(false);

			//if (event.isPressed())
			//	player.moveUpward();
			//else if (player.isMovingUpward() && event.isReleased())
			//	player.moveUpward(false);
		} break;
		case 16:  // 0x16 (shift key)
		{
			if (disableMovement) break;  // disable movement

			//if (event.isPressed())
			//	player.moveDownward();
			//else if (player.isMovingDownward() && event.isReleased())
			//	player.moveDownward(false);
		} break;
		case VK_UP:
		{
			//object1->vPos.y += 0.1f;
		} break;
		case VK_DOWN:
		{
			//object1->vPos.y -= 0.1f;
		} break;
		case VK_LEFT:
		{
			//object1->vPos.x += 0.1f;
		} break;
		case VK_RIGHT:
		{
			//object1->vPos.x -= 0.1f;
		} break;
		case 0x49:
		{
			std::cerr << player.inventory << "\n";
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

			player.action = PlayerActions::Remove;

			// Hover chat box
			CheckLMousePressedRect(vf, guiChat->getRect());
		} break;
		case Mouse::Event::Type::RPressed:
		{
			player.action = PlayerActions::Place;
			//win.hideCursor();
		} break;
		case Mouse::Event::Type::RReleased:
		{
			//win.showCursor();

			Vec2 v = { event.getX(), event.getY() };
			Vec2f vf = win.Gfx().pxToScreen(v);

			// Click chat box
			disableMovement = false;
			CheckLMousePressedRect(vf, guiChat->getRect());
		} break;
		case Mouse::Event::Type::MoveRelative:
		{
			//if (event.isRightPressed())
			if (lockMouse)
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
		case Mouse::Event::Type::WheelDown:
			player.inventory.indexDecr();
			break;
		case Mouse::Event::Type::WheelUp:
			player.inventory.indexIncr();
			break;
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
				std::cerr << "s : " << *s << "\n";
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
				//std::cerr << *userTextBuffer << "\n";
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

				//std::cerr << *userTextBuffer << "\n";
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
	// Update world objects
	//fTheta += 0.001f;  // rotate world

	switch (player.action)
	{
	case PlayerActions::Place:
		if (player.isLookingAtObject)
		{
			std::cerr << "PlayerAction -> Place\n";
			//Object* o = player.inventory.pop(player.inventory.currentSlot);
			Object* oHit = player.objectVisable.objectHit;
			Vec3f vHit = oHit->vPos;
			if (oHit != nullptr)
			{
				Vec4f vTranslated;
				vTranslated.x = vHit.x + (float)player.objectVisable.vNormal.x;
				vTranslated.y = vHit.y + (float)player.objectVisable.vNormal.y;
				vTranslated.z = vHit.z + (float)player.objectVisable.vNormal.z;
				std::cerr << "ObjectVisable: " 
					<< "vPos=" << vHit 
					<< " vPoint=" << player.objectVisable.vPoint 
					<< " normal=" << player.objectVisable.vNormal 
					<< " vTranslated=" << vTranslated
					<< "\n";
				//Vec3f v = o->vPos;
				Object* oTranslated = getWorldObject((int)vTranslated.x, (int)vTranslated.y, (int)vTranslated.z);
				if (oTranslated == nullptr)
				{
					Object* oInventory = player.inventory.pop(player.inventory.currentSlot);
					if (oInventory != nullptr)
					{
						oInventory->setPos(vTranslated.x, vTranslated.y, vTranslated.z);
						if (!setWorldObject(oInventory, (int)vTranslated.x, (int)vTranslated.y, (int)vTranslated.z))
						{
							// If cannot place object in world put it back where it came from
							player.inventory.push(oInventory);
						}
						//worldCoords[(int)vTranslated.x + game_settings.world_num_objects_x * ((int)vTranslated.y + game_settings.world_num_objects_z * (int)vTranslated.z)] = oInventory;
					}
				}
			}
		}
		break;
	case PlayerActions::Remove:
		if (player.isLookingAtObject)
		{
			std::cerr << "PlayerAction -> Remove\n";
			Object* o = player.objectVisable.objectHit;
			Vec3f v = o->vPos;
			player.inventory.push(o);
			setWorldObject(nullptr, (int)v.x, (int)v.y, (int)v.z);
			//worldCoords[(int)v.x + game_settings.world_num_objects_x * ((int)v.y + game_settings.world_num_objects_z * (int)v.z)] = nullptr;
		}
		break;
	}

	for (int z = 0; z < game_settings.world_num_objects_x; z++)
	{
		for (int y = 0; y < game_settings.world_num_objects_y; y++)
		{
			for (int x = 0; x < game_settings.world_num_objects_x; x++)
			{
				Object* o = getWorldObject(x, y, z);
				if (o == nullptr) continue;
				o->updatePosition(fTheta);

				//Vec4f player_pos = player.getVCamera();
				//if (player_pos.x >= o->vPos.x && player_pos.x <= o->vPos.x + 1.0f &&
				//	player_pos.z >= o->vPos.z && player_pos.z <= o->vPos.z + 1.0f &&
				//	player_pos.y >= o->vPos.y && player_pos.y <= o->vPos.y + 1.0f)
				//{
				//	stopPlayerFalling = true;
				//	std::cerr << "player_pos=" << player_pos << " o pos=" << o->vPos << "\n";
				//}
			}
		}
	}	

	player.updateMovement(win.lastDT);

	float player_height = 1.75;
	Vec4f player_pos = player.getVCamera();
	Object* oGround = getWorldObject(player_pos.x, player_pos.y - player_height, player_pos.z);

	Object* oFront = getWorldObject(player_pos.x, player_pos.y - player_height + 1.0f, player_pos.z + 1.0f);
	Object* oFront2 = getWorldObject(player_pos.x, player_pos.y - player_height + 2.0f, player_pos.z + 1.0f);
	Object* oBehind = getWorldObject(player_pos.x, player_pos.y - player_height + 1.0f, player_pos.z - 1.0f);
	Object* oBehind2 = getWorldObject(player_pos.x, player_pos.y - player_height + 2.0f, player_pos.z - 1.0f);
	Object* oLeft = getWorldObject(player_pos.x + 1.0f, player_pos.y - player_height + 1.0f, player_pos.z);
	Object* oLeft2 = getWorldObject(player_pos.x + 1.0f, player_pos.y - player_height + 2.0f, player_pos.z);
	Object* oRight = getWorldObject(player_pos.x - 1.0f, player_pos.y - player_height + 1.0f, player_pos.z);
	Object* oRight2 = getWorldObject(player_pos.x - 1.0f, player_pos.y - player_height + 2.0f, player_pos.z);

	Vec4f vGravity = { 0.0f, -1.0f, 0.0f };
	player.updateVelocity(vGravity, win.lastDT);
	if (oGround != nullptr)
	{
		player.updateVelocity(-vGravity, win.lastDT);  // reverse gravity
		
		// Allow player to jump
		player.checkJump(win.lastDT);
	}

	//if (oBehind != nullptr)
	//	std::cerr << "oBehind\n";
	//if (oBehind != nullptr)
	//	std::cerr << "oBehind\n";
	//if (oLeft != nullptr)
	//	std::cerr << "oLeft\n";
	//if (oRight != nullptr)
	//	std::cerr << "oRight\n";

	// Camera
	player.updatePosition(win.lastDT);

	/* Front blocks */
	if (oFront != nullptr)
	{
		clampf(&player.vCamera.z, player.vCamera.z - 10.0f, oFront->vPos.z - 0.2f);
	}
	else if (oFront2 != nullptr)
	{
		clampf(&player.vCamera.z, player.vCamera.z - 10.0f, oFront2->vPos.z - 0.2f);
	}

	/* Behind blocks */
	if (oBehind != nullptr)
	{
		clampf(&player.vCamera.z, oBehind->vPos.z + 1.2f, player.vCamera.z + 10.0f);
	}
	else if (oBehind2 != nullptr)
	{
		clampf(&player.vCamera.z, oBehind2->vPos.z + 1.2f, player.vCamera.z + 10.0f);
	}

	/* Left blocks */
	if (oLeft != nullptr)
	{
		clampf(&player.vCamera.x, player.vCamera.x - 10.0f, oLeft->vPos.x - 0.2f);
	}
	else if (oLeft2 != nullptr)
	{
		clampf(&player.vCamera.x, player.vCamera.x - 10.0f, oLeft2->vPos.x - 0.2f);
	}

	/* Right blocks */
	if (oRight != nullptr)
	{
		clampf(&player.vCamera.x, oRight->vPos.x + 1.2f, player.vCamera.x + 10.0f);
	}
	else if (oRight2 != nullptr)
	{
		clampf(&player.vCamera.x, oRight2->vPos.x + 1.2f, player.vCamera.x + 10.0f);
	}
}

/**
 * \brief Renders objects for Game loop.
 * \see Called by gsGame().
 */
void Game::glRender()
{
	// Clear screen and depth buffer
	win.Gfx().clearScreen(0xcdcdcd);
	win.Gfx().clearDepthBuffer();

	// Draw objects in scene
	//std::vector<Object*> objects;
	//objects.push_back(object1);
	//objects.push_back(object2);

	// Calculate visable objects
	std::vector<Object*> objectsToRender;
	for (int z = 0; z < game_settings.world_num_objects_x; z++)
	{
		for (int y = 0; y < game_settings.world_num_objects_y; y++)
		{
			for (int x = 0; x < game_settings.world_num_objects_x; x++)
			{
				Object* o = getWorldObject(x, y, z);
				if (o == nullptr) continue;
				o->resetFacesDrawable();
				
				Object* oRight		= getWorldObject(x - 1, y	 , z);
				Object* oLeft		= getWorldObject(x + 1, y	 , z);
				Object* oBottom		= getWorldObject(x	 , y - 1 , z);
				Object* oTop		= getWorldObject(x	 , y + 1 , z);
				Object* oFront		= getWorldObject(x	 , y	 , z - 1);
				Object* oBehind		= getWorldObject(x	 , y	 , z + 1);

				if (oFront == nullptr)	o->faces[0].draw = true;
				if (oBehind == nullptr) o->faces[1].draw = true;
				if (oLeft == nullptr)	o->faces[2].draw = true;
				if (oRight == nullptr)	o->faces[3].draw = true;
				if (oTop == nullptr)	o->faces[4].draw = true;
				if (oBottom == nullptr) o->faces[5].draw = true;

				if (oTop == nullptr && o->pTexture == pTextureDirt)
				{
					// Convert dirt into grass if nothing ontop
					o->replaceTexture(pTextureGrass);
				}
				else if (oTop != nullptr && o->pTexture == pTextureGrass)
				{
					// Convert grass into dirt if something ontop
					o->replaceTexture(pTextureDirt);
				}

				objectsToRender.push_back(o);
			}
		}
	}

	// Raster textured triangles and get current looking at object
	player.isLookingAtObject = false;
	colour_t colour = 0xff0000;
	if (win.Gfx().rasterTexturedTriangles(projectionMatrix, player.getMCamera(), player.getVCamera(), 
		player.getVLookDir(), player.objectVisable, 5.0f, objectsToRender, nullptr))
	{
		// Object hit do something with info
		player.isLookingAtObject = true;
		std::stringstream strstream_;
		strstream_ << "Looking at: " << player.objectVisable.objectHit->vPos;
		win.Gfx().drawText(strstream_.str(), { 100, 100 }, 0x0fffff);
	}

	win.Gfx().drawGUIForm(guiChat);  // todo

	win.Gfx().drawSprites();

	/* Draw player inventory */

	std::stringstream strstream;
	strstream << player.inventory;
	win.Gfx().drawText("Inventory slot: " + std::to_string(player.inventory.currentSlot) + "/" + std::to_string(player.inventory.maxSlots), {0, 20}, 0x000000);
	win.Gfx().drawText(strstream.str(), {0, 0}, 0x000000);

	win.Gfx().drawFPS(1.0f / win.lastDT, 0x000000);
	win.Gfx().drawPos(player.getVCamera(), player.getVelocity(), player.getAcceleration(), player.getYaw(), player.getPitch(), 0x000000);

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
						std::cerr << "Pressed\n";
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

	//win.Gfx().clearScreen(0xff0000);  // test

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
			//			std::cerr << event.getX() << " " << event.getY() << "\n";
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
				gsPop();
				return;
			}
			else if (t->sText == "Quit")  // TODO enum?
			{
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