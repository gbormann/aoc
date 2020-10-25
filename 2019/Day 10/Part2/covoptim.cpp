
#include <algorithm>
#include <fstream>
#include <iostream>

#include "../datatypes.h"
#include "../asteroid_watch.h"

int main()
{
/*
    vertex_set_type asteroids = {
        {1, 0}, {4, 0}, {0, 2}, {1, 2}, {2, 2},
        {3, 2}, {4, 2}, {4, 3}, {3, 4}, {4, 4}
    };
*/
    asteroid_watch a_watch("../asteroids_map.bmp");
    vertex_type vp_opt(a_watch.analyse_visibility());
    vertex_type last_cleared(a_watch.clear_field(200, vp_opt));
    std::cout << "last asteroid cleared: (" << last_cleared.first << ", " << last_cleared.second << ")\n";
}
