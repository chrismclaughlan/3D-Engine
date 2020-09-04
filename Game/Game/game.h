#pragma once
#include "Engine\hwindows.h"  // first
#include "Engine\win32_window.h"
#include "Engine\win32_network.h"
#include "Engine\win32_performance.h"
#include "Engine\vector.h"

#include <iostream>

class Vector;
class Matrix4x4;

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
	Texture textureCube;
	Matrix4x4 projectionMatrix;
	Vector vCamera;
	Vector vLookDir;
	float fTheta = 0.0f;
	float fYaw = 0.0f;
	std::chrono::time_point<std::chrono::steady_clock> timeStarted = std::chrono::high_resolution_clock::now();

private:
	// Movement
	float moveDirX = 0.0f;
	float moveDirY = 0.0f;
	float moveDirZ = 0.0f;

private:
	float* pDepthBuffer = nullptr;

	void TexturedTriangle(int x1, int y1, float u1, float v1, float w1,
		int x2, int y2, float u2, float v2, float w2,
		int x3, int y3, float u3, float v3, float w3,
		Texture* tex);
};