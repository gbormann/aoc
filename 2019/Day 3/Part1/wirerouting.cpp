
#include <iostream>
#include <vector>

#include "wire_circuit.h"

int main()
{
    wire_circuit circuit("../wire_circuit.csv");
    circuit.decode();

    std::vector<std::size_t> nr_segms(circuit.segm_counts());
    std::cout << "#segments(w1)=" << nr_segms[0]
                << "; #segments(w2)=" << nr_segms[1] << std::endl;

    const wire::vertex_type nearest_v(circuit.find_nearest_cross());
    std::cout << "Wire cross nearest to CP: ("
                << nearest_v.first << ", " << nearest_v.second << ")\n"
                << " at distance=" << wire_circuit::man_dist(nearest_v) << std::endl;
    circuit.dump();
}
