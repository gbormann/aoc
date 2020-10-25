
#include <utility>

#include "wire.h"

void wire::add_h_edge(int y, const wire::edge_type& h_edge)
{
    m_h.insert(std::make_pair(y, h_edge));
}

void wire::add_v_edge(int x, const wire::edge_type& v_edge)
{
    m_v.insert(std::make_pair(x, v_edge));
}

wire::vertex_type wire::propagate(const wire::vertex_type& v0, char d, int ns)
{
    vertex_type v(v0);

    switch(d)
    {
    case 'U':
        v.second += ns;
        add_v_edge(v0.first, edge_type(v0.second, v.second));
        break;
    case 'R':
        v.first += ns;
        add_h_edge(v0.second, edge_type(v0.first, v.first));
        break;
    case 'D':
        v.second -= ns;
        add_v_edge(v0.first, edge_type(v0.second, v.second));
        break;
    case 'L':
        v.first -= ns;
        add_h_edge(v0.second, edge_type(v0.first, v.first));
        break;
    default: throw std::runtime_error("Blechchch!!!!");
    }

    return v;
}
