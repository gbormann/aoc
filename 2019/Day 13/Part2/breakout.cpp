
#include <iostream>

#include <curses.h>

#include "../datatypes.h"
#include "../game_engine.h"

int main()
{
    game_engine game("../intcode_prog.txt");
    game.run();

    return 0;
}
