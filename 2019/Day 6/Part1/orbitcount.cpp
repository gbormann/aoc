
#include <iostream>

#include "orbit_counter.h"

int main()
{
    orbit_counter counter("../orbits.txt");

    std::cout << "Total number of orbits in system: " << counter.count_orbits() << std::endl;
}
