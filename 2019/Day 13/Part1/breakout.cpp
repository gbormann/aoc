
#include <iostream>

#include <curses.h>

#include "../datatypes.h"
#include "../game_engine.h"

int main()
{
    game_engine game("../intcode_prog.txt");
    game.run();
    std::cout << "#block tiles: " << game.blockCount() << '\n';
    std::cout << "Paddle line: " << game.hPaddleLine() << '\n';
    vertex_type ballPos{game.initialBallPos()};
    std::cout << "Ball position: (" << ballPos.first << ", " << ballPos.second << ")\n";

    return 0;
}
