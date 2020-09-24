#pragma once
#include "Engine\hwindows.h"  // first
#include "Engine\win32_window.h"
#include "Engine\vector.h"
#include "Engine\graphics_ui.h"
#include "Engine\graphics_objects.h"
#include "player.h"
#include <stack>

class Player;

enum class MenuAction
{
	Start, Quit, Continue, Invalid,
};

struct MenuButton
{
	GUISprite* sprite = nullptr;
	MenuAction action = MenuAction::Invalid;

	MenuButton(GUISprite* sprite, MenuAction action)
		: sprite(sprite), action(action)
	{}

	~MenuButton()
	{
		if (sprite != nullptr)
		{
			delete sprite;
			sprite = nullptr;
		}
	}
};

struct MainMenu
{
	std::vector<MenuButton*> buttons;

	MainMenu()
	{
		MenuButton* b1 = new MenuButton(
			new GUISprite("MainMenu_NewGame.bmp", TextureType::RGBA, 256, 128, 0.4f, 0.560f, 0.6f, 0.640f),
			MenuAction::Start);
		MenuButton* b2 = new MenuButton(
			new GUISprite("MainMenu_Continue.bmp", TextureType::RGBA, 256, 128, 0.4f, 0.460f, 0.6f, 0.540f),
			MenuAction::Continue);
		MenuButton* b3 = new MenuButton(
			new GUISprite("MainMenu_Quit.bmp", TextureType::RGBA, 256, 128, 0.4f, 0.360f, 0.6f, 0.440f),
			MenuAction::Quit);

		buttons.push_back(b1);
		buttons.push_back(b2);
		buttons.push_back(b3);
	}

	~MainMenu()
	{
		for (auto b : buttons)
		{
			if (b != nullptr)
			{
				delete b;
				b = nullptr;
			}
		}
		buttons.clear();
	}
};

class Game
{
private:
	int mouseXPrev = -1;
	int mouseYPrev = -1;

private:
	// Game States
	std::stack<void (Game::*)()> gStates;
	
	void gsPush(void (Game::* gs)())
	{
		gStates.push(gs);
		win.keyboard.flush();
		win.mouse.flush();
	}

	const bool gsSafePop()
	{
		if (gStates.empty())
		{
			return false;
		}

		gStates.pop();
		return true;
	}

	const bool gsPop()
	{
		if (gStates.empty())
		{
			return false;
		}

		if (!gsSafePop())
		{
			return false;
		}

		win.keyboard.flush();
		win.mouse.flush();
		return true;
	}

	void gsPopUntil(void (Game::*gs)())
	{
		if (!gsSafePop())
		{
			std::cout << "Hit end of stack -> cannot hit intended element\n";
			return;
		}

		while (gStates.top() != gs)
		{
			// Handle intermediate game states

			if (gStates.top() == &Game::gsGame)
			{
				// deconstruct game?
			}

			// delete intermediate GUIs!

			if (!gsSafePop())
			{
				std::cout << "Hit end of stack -> cannot hit intended element\n";
				return;
			}
		}

		// Hit intended element in stack
		win.keyboard.flush();
		win.mouse.flush();
	}

	//void resetGUIForm(GUIForm f)
	//{
	//	for (auto t : f.getVGuiText())
	//	{
	//		t->state = GUI_STATE_INACTIVE;
	//	}
	//}

	void gsCloseApplication();
	void gsInitGame();
	void gsMainMenu();
	void gsGame();
	void gsGameInput();
	void gsGameSimulate();
	void gsGameRender();
	void gsGameMenu();


private:
	Window win;

public:
	Game(const char* name, int width, int height)
		: win(name, width, height)
	{
		userTextBuffer = new std::string;
	}

	~Game()
	{
		if (mainMenu != nullptr)
		{
			delete mainMenu;
			mainMenu = nullptr;
		}
		if (guiChat != nullptr)
		{
			delete guiChat;
			guiChat     = nullptr;
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

	//virtual int run() = 0;
	int run();

private:
	// 3D
	Matrix4x4 projectionMatrix;
	float fTheta = 0.0f;

	Object* object1 = nullptr;
	Object* object2 = nullptr;
	Texture* pObjectTexture1 = nullptr;
	Texture* pObjectTexture2 = nullptr;

	Player player;

private:
	// 2D Text
	//std::vector<uint8> text;
	bool disableMovement = false;
	std::string* userTextBuffer;
	//std::vector<std::string*> textHistory;

	//GUILayout guiChat;

	GUIForm* guiChat = nullptr;
	GUIMenu* guiGameMenu = nullptr;
	//GUISpriteRGBA* guiSprite = nullptr;
	//std::vector<GUISpriteRGBA*> guiMainMenu;
	MainMenu* mainMenu = nullptr;

private:
	// returns true if string occupied
	bool CheckReturnRect(GUIRect* r, std::string*& s)
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

	void CheckEscapeRect(GUIRect* r)
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

	void CheckMouseMoveRect(const vec2f& vf, GUIRect* r)
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

	void CheckLMousePressedRect(const vec2f& vf, GUIRect* r)
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

	void CheckMouseMoveText(const vec2f& vf, GUIText* t)
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

	void CheckMousePressedText(const vec2f& vf, GUIText* t)
	{
		if (t->isAt(vf.x, vf.y, &win.Gfx()))
		{
			t->state = GUIState::Active;
		}
	}
};
