
#include <iostream>

#include "../reactor.h"

int main()
{
    reactor fuel_reactor("../reactions.txt");
    long fuel_yield{fuel_reactor.obtain("FUEL", "ORE", 1000000000000L)};
    std::cout << "10^12 units of ORE yields " << fuel_yield << " units of FUEL\n";
}
