#include <iostream>
#include <string>
#include <windows.h>
#include "game.hpp"
#include "jengine.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	try {
		Juego *juego = new Juego();
		juego->run();
		delete juego;
	} catch (JEngineException &e) {
		MessageBox(
			NULL, e.what()->c_str(), "Error!", MB_ICONWARNING | MB_OK
		);
	}
    
    return 0;
}
