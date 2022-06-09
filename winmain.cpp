#include <iostream>
#include <string>
#include <windows.h>
#include "game.hpp"
#include "jengine.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	try {
		Game *game = new Game();
		game->run();
		delete game;
	} catch (JEngineException &e) {
		MessageBox(
			NULL, e.what()->c_str(), "Error!", MB_ICONWARNING | MB_OK
		);
	}
    
    return 0;
}
