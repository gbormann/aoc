
#if !defined(WIRE_CIRCUIT_H)
#define WIRE_CIRCUIT_H

#include <string>
#include <map>
#include <vector>

#include "wire.h"

class wire_circuit
{
    typedef std::multimap<wire::edge_type, wire::vertex_type> crossed_edge_map_type;
    typedef crossed_edge_map_type::const_iterator crossed_edge_const_iterator;

public:
    static int man_dist(const wire::vertex_type& vertex);

    wire_circuit(const std::string& circuitFileName);

    std::size_t decode();
    void find_crosses();
    std::size_t propagate(std::size_t w_ndx); // propagate to first cross
    std::size_t backtrack(std::size_t X_ndx, std::size_t w_ndx); // backtrack from first cross of wire X_ndx over wire w_ndx
    std::size_t backtrack(const wire::vertex_type& v_X, std::size_t w_ndx); // backtrack from given cross over wire w_ndx

    void backtrack_all(std::size_t w_ndx);

    void dump() const;

private:
    void read_circuit(const std::string& circuitFileName);

    std::vector<std::string> m_text;
    std::vector<wire> m_circuit;
    std::vector<crossed_edge_map_type> m_Xed_edges;
    std::vector<wire::vertex_type> m_first_Xs;
};

#endif
