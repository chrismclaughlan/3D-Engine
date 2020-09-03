#pragma once
#ifdef _DEBUG
	#define DISPLAY_DEBUG_CONSOLE

	#ifdef DISPLAY_DEBUG_CONSOLE
		#define DISPLAY_MOUSE_COORDS
		#define DISPLAY_FPS
	#endif
#endif