#include <iostream>
#include <string>
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
		cout << "Error! " << *e.what() << endl;
        flush(cout);        
	}
    
    return 0;
}
