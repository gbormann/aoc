
#include <iostream>
#include <stdexcept>

#include <curses.h>

#include "../datatypes.h"
#include "img_mod.h"

int main()
{
    img_mod cam("../intcode_prog.txt");

    try {
        cam.run();
    }
    catch(std::exception& exc)
    {
        cam.endConsole();
//        cam.printReport();
        std::cerr << exc.what() << '\n';
    }

    return 0;
}
