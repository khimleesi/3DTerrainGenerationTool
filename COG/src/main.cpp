
#include "managers/GameManager.h"

#if defined(COG_DEBUG)
	#include <pretty_vleak/vld.h>
#endif

/*******************************************************************************************************************
	main.h, main.cpp
	Created by Kim Kane
	Last updated: 15/03/2018

	A simple SDL and OpenGL engine, used to create a 3D game.

*******************************************************************************************************************/

int main(int argc, char *argv[])
{
	//--- Full screen, core mode and Vsync bools can be adjusted below.
	//--- Press ESC to exit full screen mode (full screen looks a bit streched right now - need to try and fix this!)
	Game::Instance()->Initialize("COG : Game Engine and Terrain Generation Tool", false, true, true);
	Game::Instance()->Run();
	Game::Instance()->Shutdown();

	return 0;
} 