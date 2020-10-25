
#include <iostream>

#include "../reactor.h"

int main()
{
    reactor fuel_reactor("../reactions.txt");
    long fuel_cost{fuel_reactor.obtain_units("FUEL")};
    std::cout << "1 unit of FUEL costs " << fuel_cost << " units of ORE\n";
}
