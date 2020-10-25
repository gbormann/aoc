
#include <iostream>

#include "orbit_counter.h"

int main()
{
    orbit_counter counter("../orbits.txt");

    std::cout << "Total number of orbits in system: " << counter.count_orbits() << '\n';

    std::cout << "Santa is " << counter.count_transfers_to_Santa() << " transfers away!" << std::endl;
}
