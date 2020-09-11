#pragma once
#include "Engine\hwindows.h"  // first
#include "Engine\win32_window.h"
#include "Engine\vector.h"
#include "Engine\gui.h"
#include "player.h"
#include <stack>

class Player;

class Game
{
private:
	int32 mouseXPrev = -1;
	int32 mouseYPrev = -1;

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

	const int32 initText();
	void gsMainMenu();
	const int32 initGame();
	void gsGame();
	void gsGameInput();
	void gsGameSimulate();
	void gsGameRender();
	void gsGameMenu();


private:
	Window win;

public:
	Game(const char* name, int32 width, int32 height)
		: win(name, width, height)
	{
		userTextBuffer = new std::string;
	}

	~Game()
	{
		if (guiSprite != nullptr)
		{
			delete guiSprite;
			guiSprite = nullptr;
		}
		if (guiChat != nullptr)
		{
			delete guiChat;
			guiChat     = nullptr;
		}
		if (guiMainMenu != nullptr)
		{
			delete guiMainMenu;
			guiMainMenu = nullptr;
		}
		if (guiGameMenu != nullptr)
		{
			delete guiGameMenu;
			guiGameMenu = nullptr;
		}

		delete userTextBuffer;
		delete object1;
		delete object2;
		delete objectTexture1;
		delete objectTexture2;
		object1 = nullptr;
		object2 = nullptr;
		objectTexture1 = nullptr;
		objectTexture2 = nullptr;
	}

	//virtual int32 run() = 0;
	int32 run();

private:
	// 3D
	Matrix4x4 projectionMatrix;
	float fTheta = 0.0f;

	Object* object1 = nullptr;
	Object* object2 = nullptr;
	Texture* objectTexture1 = nullptr;
	Texture* objectTexture2 = nullptr;

	Player player;

private:
	// 2D Text
	//std::vector<uint8> text;
	bool disableMovement = false;
	std::string* userTextBuffer;
	//std::vector<std::string*> textHistory;

	//GUILayout guiChat;

	GUIForm* guiChat = nullptr;
	GUIMenu* guiMainMenu = nullptr;
	GUIMenu* guiGameMenu = nullptr;
	WIPGUISprite* guiSprite = nullptr;

private:
	// returns true if string occupied
	bool CheckReturnRect(GUIRect* r, std::string*& s)
	{
		if (r->state == GUI_STATE_ACTIVE && r->guiTextInput != nullptr)
		{
			r->state = GUI_STATE_INACTIVE;
			s = r->guiTextInput->pText;
			r->guiTextInput->pText = nullptr;
			userTextBuffer = new std::string;
			disableMovement = false;
			return true;
		}

		return false;
	}

	void CheckEscapeRect(GUIRect* r)
	{
		if (r->guiTextInput != nullptr)
		{
			userTextBuffer->clear();
			if (r->state == GUI_STATE_ACTIVE)
				r->state = GUI_STATE_INACTIVE;
			r->guiTextInput->pText = nullptr;
			disableMovement = false;
		}
	}

	void CheckMouseMoveRect(const float x, const float y, 
		GUIRect* r)
	{
		if (r->state != GUI_STATE_ACTIVE)
		{
			if (r->isAt(x, y))
			{
				r->state = GUI_STATE_HOVER;
			}
			else
			{
				r->state = GUI_STATE_INACTIVE;
			}
		}
	}

	void CheckLMousePressedRect(const float x, const float y, 
		GUIRect* r)
	{
		if (r->guiTextInput != nullptr)
		{
			if (r->isAt(x, y))
			{
				if (r->state != GUI_STATE_ACTIVE)
				{
					r->state = GUI_STATE_ACTIVE;
					r->guiTextInput->pText = userTextBuffer;
					disableMovement = true;
				}
				else
				{
					r->state = GUI_STATE_INACTIVE;
					r->guiTextInput->pText = nullptr;
				}
			}
			else
			{
				r->state = GUI_STATE_INACTIVE;
				r->guiTextInput->pText = nullptr;
			}
		}
	}

	void CheckMouseMoveText(const float x, const float y, 
		GUIText* t)
	{
		if (t->isAt(x, y, &win.Gfx()))
		{
			if (t->state != GUI_STATE_ACTIVE)
				t->state = GUI_STATE_HOVER;
		}
		else
		{
			t->state = GUI_STATE_INACTIVE;
		}
	}

	void CheckMousePressedText(const float x, const float y, 
		GUIText* t)
	{
		if (t->isAt(x, y, &win.Gfx()))
		{
			t->state = GUI_STATE_ACTIVE;
		}
	}
};