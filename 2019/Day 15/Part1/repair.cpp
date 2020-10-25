
#include <iostream>

#include "../datatypes.h"
#include "../repair_droid.h"

int main()
{
    repair_droid droid("../intcode_prog.txt");
    droid.run();

    return 0;
}
