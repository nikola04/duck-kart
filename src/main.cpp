#include "game/Game.hpp"

#include <exception>
#include <iostream>
#include <tiny_gltf.h>

int main()
{
    try
    {
        Game game;
        return game.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
