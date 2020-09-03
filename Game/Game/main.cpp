#include "Engine\hwindows.h"  // first
#include "Engine\exception.h"
#include "Engine\defines.h"
#include "game.h"

int WINAPI wWinMain
(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int32 nCmdShow)
{
	try
	{
		return Game("3D Demo", 720, 480).run();
	}
	catch (const Exception& e)
	{
#ifdef _DEBUG
		e.logError();
#else
		e.printError();
#endif
		Sleep(5000);
	}

	return -1;
}