/*****************************************************************//**
 * \file   game.h
 * \brief  Interface (?) for game application.
 * 
 * \author Chris
 * \date   September 2020
 *********************************************************************/

#pragma once
#include "Engine\hwindows.h"  // first
#include "Engine\win32_window.h"
#include "Engine\utils_vector.h"
#include "Engine\graphics_objects.h"
#include "player.h"
#include "game_menus.h"
#include <stack>
#include <utility>


class Player;


struct GameSettings
{
	/* Player Properties */
	PlayerSettings player_settings;

	/* World Properties */
	const int world_num_objects_x = 5;
	const int world_num_objects_y = 5;
	const int world_num_objects_z = 5;
	//const int world_num_chunks_x = 5;
	//const int world_num_chunks_y = 5;
	//const int world_num_chunks_z = 5;
};


class Game
{
private:
	/* Game Settings */
	GameSettings game_settings;

	/* Game States */
	std::stack<std::pair<void (Game::*)(int), int>> sGameStates;
	const bool gsPush(void (Game::* func)(int), int flag);
	const bool gsPop();
	const bool gsPopUntil(void (Game::* func)(int), const int flag);
	const bool gsUpdateCurrentFlag(const int flag);
	const bool gsDestroyCurrentThenPush(void (Game::* func)(int), const int flag);

	/* Game State functions */
	void gsCloseApplication	(int flag);
	void gsGame				(const int flag);
	void gsMainMenu			(const int flag);
	void gsGameMenu			(const int flag);

	/* Game Methods */
	bool glInitialised = false;
	void glInit();
	void glDestroy();
	void glInput();
	void glSimulate();
	void glRender();

	/* Menu Methods */
	void mMain();
	void mGame();

	// TODO rename menus (mainmenu -> titlemenu)

private:
	/* Window Class */
	Window win;

public:
	Game(const char* name, int width, int height);
	~Game();
	const int run();

private:
	// 3D
	Matrix4x4 projectionMatrix;
	float fTheta = 0.0f;

	//std::vector<Object*> worldObjects;

	Object** worldCoords = nullptr;
	inline Object* getWorldObject(const int x, const int y, const int z) const
	{
		if ((x >= 0 && x < game_settings.world_num_objects_x) && (y >= 0 && y < game_settings.world_num_objects_y) && (z >= 0 && z < game_settings.world_num_objects_z))
		{
			return worldCoords[x + game_settings.world_num_objects_x * (y + game_settings.world_num_objects_z * z)];
		}
		else
		{
			return nullptr;
		}
	}

	inline bool setWorldObject(Object* o, const int x, const int y, const int z)
	{
		if ((x >= 0 && x < game_settings.world_num_objects_x) && (y >= 0 && y < game_settings.world_num_objects_y) && (z >= 0 && z < game_settings.world_num_objects_z))
		{
			worldCoords[x + game_settings.world_num_objects_x * (y + game_settings.world_num_objects_z * z)] = o;
			return true;
		}
		else
		{
			return false;
		}
	}

	Texture* pTextureGrass = nullptr;
	Texture* pTextureDirt = nullptr;
	Texture* pTextureStone = nullptr;

private:
	/* Player */
	Player player;
	int mouseXPrev = -1;  // temp fix
	int mouseYPrev = -1;  //

private:
	// 2D Text
	bool disableMovement = false;
	bool lockMouse = true;
	std::string* userTextBuffer;
	//std::vector<std::string*> textHistory;

	GUIForm* guiChat = nullptr;
	GUIMenu* guiGameMenu = nullptr;
	MainMenu* mainMenu = nullptr;

private:
	/* Old GUI Collision Checks */
	bool CheckReturnRect(GUIRect* r, std::string*& s);
	void CheckEscapeRect(GUIRect* r);
	void CheckMouseMoveRect(const Vec2f& vf, GUIRect* r);
	void CheckLMousePressedRect(const Vec2f& vf, GUIRect* r);
	void CheckMouseMoveText(const Vec2f& vf, GUIText* t);
	void CheckMousePressedText(const Vec2f& vf, GUIText* t);
};
