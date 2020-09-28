#include "game_menus.h"

/* Menu Button */

MenuButton::MenuButton(GUISprite* sprite, MenuAction action)
	: sprite(sprite), action(action)
{}

MenuButton::~MenuButton()
{
	delete sprite;
	sprite = nullptr;
}


/* Main Menu */

MainMenu::MainMenu(Graphics& gfx)
{
	MenuButton* b1 = new MenuButton(
		new GUISprite("MainMenu_NewGame.bmp", 
			TextureType::RGBA, 256, 128, 
			0.43f, 0.560f, 
			0.57f, 0.640f),
		MenuAction::Start);

	MenuButton* b2 = new MenuButton(
		new GUISprite("MainMenu_Continue.bmp", 
			TextureType::RGBA, 256, 128, 
			0.43f, 0.460f, 
			0.57f, 0.540f),
		MenuAction::Continue);

	MenuButton* b3 = new MenuButton(
		new GUISprite("MainMenu_Quit.bmp", 
			TextureType::RGBA, 256, 128, 
			0.43f, 0.360f,
			0.57f, 0.440f), 
		MenuAction::Quit);

	buttons.push_back(b1);
	buttons.push_back(b2);
	buttons.push_back(b3);

	background = gfx.createSprite("ExampleTitleSprite.bmp", TextureType::RGB, 640, 360, { 0.0f, 0.0f }, { 1.0f, 1.0f });
	//background = gfx.createSprite("HUD.bmp", TextureType::RGBA, 640, 360, { 0.0f, 0.0f }, { 1.0f, 1.0f });
}

MainMenu::~MainMenu()
{
	for (auto b : buttons)
	{
		delete b;
		b = nullptr;
	}
	buttons.clear();

	if (background != nullptr)
	{
		background->parent.destroySprites();
		background = nullptr;
	}
}