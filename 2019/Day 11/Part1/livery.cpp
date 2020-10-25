
#include <iostream>

#include "../datatypes.h"
#include "../paint_robot.h"

int main()
{
    paint_robot robot("../intcode_prog.txt");
    robot.run();
    std::cout << "#unique panels coloured: " << robot.paintedPanelCount() << '\n';

    return 0;
}
