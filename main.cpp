#include <iostream>

#include "mge/core/AbstractGame.hpp"
#include "mge/MGEDemo.hpp"
#include "mge/TerrainScene.h"

int main()
{
    std::cout << "Starting Game" << std::endl;

    //AbstractGame* game = new MGEDemo();
    AbstractGame* game = new TerrainScene();
    game->initialize();
    game->run();

	delete game;

    return 0;
}



