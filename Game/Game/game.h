#pragma once
#include "Engine\hwindows.h"  // first
#include "Engine\win32_window.h"
#include "Engine\win32_network.h"
#include "Engine\win32_performance.h"

#include <iostream>

class Game
{
	// TODO error handling
protected:
	Window win;
	Performance performance;

public:
	Game(const char* name, int32 width, int32 height)
		: 
		win(name, width, height), 
		fpsLimit(0)
	{
	}
	~Game()
	{
	}

	//virtual int32 run() = 0;
	int32 run();
protected:
	void HandleInput();
	void DoFrame();
protected:
	std::string userTextBuffer;
	std::string acceptedCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_ ";
	uint32 fpsLimit;

private:
	mesh meshCube;
	matrix4x4 projectionMatrix;
	float fTheta = 0.0f;
	vector3 camera = {0};
};