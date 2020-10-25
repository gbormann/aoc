
#if !defined(WIRE_CIRCUIT_H)
#define WIRE_CIRCUIT_H

#include <string>
#include <vector>

#include "wire.h"

class wire_circuit
{
public:
    static int man_dist(const wire::vertex_type& vertex);

    wire_circuit(const std::string& circuitFileName);

    void decode();
    wire::vertex_type find_nearest_cross() const;
    std::vector<std::size_t> segm_counts() const;

    void dump() const;

private:
    void read_circuit(const std::string& circuitFileName);

    std::vector<std::string> m_text;
    std::vector<wire> m_circuit;
};

#endif
