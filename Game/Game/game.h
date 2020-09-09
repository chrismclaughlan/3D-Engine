#pragma once
#include "Engine\hwindows.h"  // first
#include "Engine\win32_window.h"
#include "Engine\vector.h"
#include "Engine\gui.h"
#include "player.h"

class Player;

class Game
{
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
	void HandleInput();
	void DoFrame();

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
	bool enableWriting = false;
	std::string* userTextBuffer;
	//std::vector<std::string*> textHistory;

	//GUILayout guiChat;

	GUIChat guiChat;
	GUIFormMainMenu guiMainMenu;

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
			enableWriting = false;
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
			enableWriting = false;
		}
	}

	void CheckMouseMoveRect(const float x, const float y, GUIRect* r)
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

	void CheckLMousePressedRect(const float x, const float y, GUIRect* r)
	{
		if (r->guiTextInput != nullptr)
		{
			if (r->isAt(x, y))
			{
				if (r->state != GUI_STATE_ACTIVE)
				{
					r->state = GUI_STATE_ACTIVE;
					r->guiTextInput->pText = userTextBuffer;
					enableWriting = true;
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
};