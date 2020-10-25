
#include <iostream>
#include <stdexcept>

#include <curses.h>

#include "../datatypes.h"
#include "drone_ctrl.h"

int main()
{
    drone_ctrl ctrlr("../intcode_prog.txt");

    try {
        ctrlr.run();
    }
    catch(std::exception& exc)
    {
        ctrlr.endConsole();
        ctrlr.printReport();
        std::cerr << exc.what() << '\n';
    }

    return 0;
}
