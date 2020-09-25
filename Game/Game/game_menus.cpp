#include "game_menus.h"

/* Menu Button */

MenuButton::MenuButton(GUISprite* sprite, MenuAction action)
	: sprite(sprite), action(action)
{}

MenuButton::~MenuButton()
{
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}


/* Main Menu */

MainMenu::MainMenu()
{
	MenuButton* b1 = new MenuButton(
		new GUISprite("MainMenu_NewGame.bmp", 
			TextureType::RGBA, 256, 128, 0.4f, 0.560f, 0.6f, 0.640f),
		MenuAction::Start);

	MenuButton* b2 = new MenuButton(
		new GUISprite("MainMenu_Continue.bmp", 
			TextureType::RGBA, 256, 128, 0.4f, 0.460f, 0.6f, 0.540f),
		MenuAction::Continue);

	MenuButton* b3 = new MenuButton(
		new GUISprite("MainMenu_Quit.bmp", 
			TextureType::RGBA, 256, 128, 0.4f, 0.360f, 0.6f, 0.440f), 
		MenuAction::Quit);

	buttons.push_back(b1);
	buttons.push_back(b2);
	buttons.push_back(b3);
}

MainMenu::~MainMenu()
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