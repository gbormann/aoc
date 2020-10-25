
#include <iostream>
#include <stdexcept>

#include <curses.h>

#include "../datatypes.h"
#include "../repair_droid.h"

int main()
{
    repair_droid droid("../intcode_prog.txt");

    try {
        droid.run();
    }
    catch(std::exception& exc)
    {
        droid.endConsole();
        droid.printReport();
        std::cerr << exc.what() << '\n';
    }

    return 0;
}
