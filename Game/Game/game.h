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

	//virtual int32 run() = 0;
	int32 run();
protected:
	void HandleInput();
	void DoFrame();
protected:
	//std::string userTextBuffer;
	//std::string acceptedCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_ ";

private:
	std::vector<Mesh> objectMeshes;

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