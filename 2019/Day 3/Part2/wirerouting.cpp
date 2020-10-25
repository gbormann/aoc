
#include <iostream>

#include "wire_circuit.h"

int main()
{
    wire_circuit circuit("../wire_circuit.csv");
    std::size_t nr_w = circuit.decode();
    circuit.find_crosses();
    std::cout << "d(0,X0)=" << circuit.propagate(0) << '\n';
    std::cout << "d(0,X1)=" << circuit.propagate(1) << '\n';
    std::cout << "d(X0_w1,0)=" << circuit.backtrack(0, 1) << '\n';
    std::cout << "d(X1_w0,0)=" << circuit.backtrack(1, 0) << std::endl;;
    circuit.dump();
    circuit.backtrack_all(0);
    circuit.backtrack_all(1);
}
