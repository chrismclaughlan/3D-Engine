#pragma once
#include "Engine\hwindows.h"  // first
#include "Engine\win32_window.h"
#include "Engine\vector.h"


class Game
{
protected:
	Window win;

public:
	Game(const char* name, int32 width, int32 height)
		: win(name, width, height)
	{}

	~Game()
	{
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
protected:
	void HandleInput();
	void DoFrame();
protected:
	//std::string userTextBuffer;
	//std::string acceptedCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_ ";

private:
	Mesh* object1 = nullptr;
	Mesh* object2 = nullptr;
	Texture* objectTexture1 = nullptr;
	Texture* objectTexture2 = nullptr;

private:
	Matrix4x4 projectionMatrix;
	Vector vCamera;
	Vector vLookDir;
	float fTheta = 0.0f;
	float fYaw = 0.0f;

	// Movement
	float moveDirX = 0.0f;
	float moveDirY = 0.0f;
	float moveDirZ = 0.0f;
};