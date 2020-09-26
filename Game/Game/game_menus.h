/*****************************************************************//**
 * \file   game_menus.h
 * \brief  Implements structures containing various gui elements.
 * 
 * \author Chris
 * \date   September 2020
 *********************************************************************/

#pragma once
#include "Engine\graphics_ui.h"

enum class MenuAction
{
	Start, Quit, Continue, Invalid,
};

struct MenuButton
{
	GUISprite* sprite = nullptr;
	MenuAction action = MenuAction::Invalid;

	MenuButton(GUISprite* sprite, MenuAction action);
	~MenuButton();
};

struct MainMenu
{
	GUISprite* background = nullptr;
	std::vector<MenuButton*> buttons;

	MainMenu();
	~MainMenu();
	void resetButtons(GUIState newState = GUIState::Inactive)
	{
		for (auto b : buttons)
		{
			b->sprite->state = newState;
		}
	}
};
