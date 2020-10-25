
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>

#include "wire.h"
#include "wire_circuit.h"

int wire_circuit::man_dist(const wire::vertex_type& vertex)
{
    return std::abs(vertex.first) + std::abs(vertex.second);
}

wire_circuit::wire_circuit(const std::string& circuitFileName)
{
    read_circuit(circuitFileName);
    m_circuit.resize(m_text.size());
}

void wire_circuit::decode()
{
    std::size_t w_ndx = 0;
    for (const auto& wire_descr : m_text)
    {
        wire& w = m_circuit[w_ndx++];
        wire::vertex_type v(0, 0);
        std::size_t ip = 0;
        for (std::size_t ep = wire_descr.find_first_of(',', ip);
             ep != std::string::npos;
             ip = ep + 1, ep = wire_descr.find_first_of(',', ip))
            v = w.propagate(v, wire_descr[ip], std::stoi(wire_descr.substr(ip + 1, ep - ip), nullptr));
        v = w.propagate(v, wire_descr[ip], std::stoi(wire_descr.substr(ip + 1), nullptr));
    }
}

std::vector<std::size_t> wire_circuit::segm_counts() const
{
    std::vector<std::size_t> len_ws;

    for (const wire& w : m_circuit)
        len_ws.push_back(w.hs().size() + w.vs().size());

    return len_ws;
}

wire::vertex_type wire_circuit::find_nearest_cross() const
{
    const wire& w1 = m_circuit[0];
    const wire& w2 = m_circuit[1];

    int cur_X_dr = std::numeric_limits<int>::max();
    wire::vertex_type cur_X_v(0, 0);
    for (const auto& h1_e : w1.hs())
    {
        const int x1_m = std::min(h1_e.second.first, h1_e.second.second);
        const int x1_M = std::max(h1_e.second.first, h1_e.second.second);

        for (const auto& v2_e : w2.vs())
        {
            const int y2_m = std::min(v2_e.second.first, v2_e.second.second);
            const int y2_M = std::max(v2_e.second.first, v2_e.second.second);

            if (x1_m <= v2_e.first && v2_e.first <= x1_M && y2_m <= h1_e.first && h1_e.first <= y2_M)
            {
                const wire::vertex_type v(v2_e.first, h1_e.first);
                const int v_dr = man_dist(v);

                if (v_dr > 0 && v_dr < cur_X_dr)
                {
                    cur_X_v = v;
                    cur_X_dr = v_dr;
                }
            }
        }
    }

    for (const auto& v1_e : w1.vs())
    {
        const int y1_m = std::min(v1_e.second.first, v1_e.second.second);
        const int y1_M = std::max(v1_e.second.first, v1_e.second.second);

        for (const auto& h2_e : w2.hs())
        {
            const int x2_m = std::min(h2_e.second.first, h2_e.second.second);
            const int x2_M = std::max(h2_e.second.first, h2_e.second.second);

            if (x2_m <= v1_e.first && v1_e.first <= x2_M && y1_m <= h2_e.first && h2_e.first <= y1_M )
            {
                const wire::vertex_type v(v1_e.first, h2_e.first);
                const int v_dr = man_dist(v);

                if (v_dr > 0 && v_dr < cur_X_dr)
                {
                    cur_X_v = v;
                    cur_X_dr = v_dr;
                }
            }
        }
    }

    return cur_X_v;
}

void wire_circuit::dump() const
{
    for (auto& w : m_circuit)
    {
        std::cout << "===== wire =====" << std::endl;
        std::cout << "H edges:" << std::endl;
        for (const auto& h_e : w.hs())
            std::cout << "y=" << h_e.first << ": " << h_e.second.first << " |=> " << h_e.second.second << std::endl;

        std::cout << "V edges:" << std::endl;
        for (const auto& v_e : w.vs())
            std::cout << "x=" << v_e.first << ": " << v_e.second.first << " |=> " << v_e.second.second << std::endl;
    }
}

void wire_circuit::read_circuit(const std::string& circuitFileName)
{
    std::ifstream ifs(circuitFileName);
    std::copy(std::istream_iterator<std::string>(ifs),
              std::istream_iterator<std::string>(),
              std::back_inserter(m_text));
}

