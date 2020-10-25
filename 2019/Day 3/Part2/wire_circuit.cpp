
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>

#include "wire.h"
#include "wire_circuit.h"

wire_circuit::wire_circuit(const std::string& circuitFileName)
{
    read_circuit(circuitFileName);
    m_circuit.resize(m_text.size());
}

std::size_t wire_circuit::decode()
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

    return w_ndx;
}

int wire_circuit::man_dist(const wire::vertex_type& vertex)
{
    return std::abs(vertex.first) + std::abs(vertex.second);
}

void wire_circuit::find_crosses()
{
    m_Xed_edges.clear();
    m_Xed_edges.resize(m_circuit.size());

    m_first_Xs.clear();
    m_first_Xs.resize(m_circuit.size());

    const wire& w1 = m_circuit[0];
    const wire& w2 = m_circuit[1];

    for (const auto& h1_e : w1.hs())
    {
        const int x1_m = std::min(h1_e.second.first, h1_e.second.second);
        const int x1_M = std::max(h1_e.second.first, h1_e.second.second);

        for (const auto& v2_e : w2.vs())
        {
            const int y2_m = std::min(v2_e.second.first, v2_e.second.second);
            const int y2_M = std::max(v2_e.second.first, v2_e.second.second);

            if (v2_e.first != 0 && h1_e.first != 0
                && x1_m <= v2_e.first && v2_e.first <= x1_M
                && y2_m <= h1_e.first && h1_e.first <= y2_M)
            {
                m_Xed_edges[0].insert(
                    std::make_pair(
                        wire::edge_type(h1_e.second.first, h1_e.second.second),
                        wire::vertex_type(v2_e.first, h1_e.first)
                    )
                );
                m_Xed_edges[1].insert(
                    std::make_pair(
                        wire::edge_type(v2_e.second.first, v2_e.second.second),
                        wire::vertex_type(v2_e.first, h1_e.first)
                    )
                );
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

            if (v1_e.first != 0 && h2_e.first != 0
                && x2_m <= v1_e.first && v1_e.first <= x2_M
                && y1_m <= h2_e.first && h2_e.first <= y1_M )
            {
                m_Xed_edges[0].insert(
                    std::make_pair(
                        wire::edge_type(v1_e.second.first, v1_e.second.second),
                        wire::vertex_type(v1_e.first, h2_e.first)
                    )
                );
                m_Xed_edges[1].insert(
                    std::make_pair(
                        wire::edge_type(h2_e.second.first, h2_e.second.second),
                        wire::vertex_type(v1_e.first, h2_e.first)
                    )
                );
            }
        }
    }
}

std::size_t wire_circuit::propagate(std::size_t w_ndx)
{
    std::size_t ns = 0;
    std::size_t ns_slack = 0;

    const wire& w = m_circuit[w_ndx];
    wire::vertex_type cur_v(0, 0);

    wire::edge_const_iterator e_cit = w.hs().find(cur_v.second);
    wire::edge_const_iterator end_e_cit = w.hs().end();
    bool do_h_step = e_cit != end_e_cit;
    if (!do_h_step)
        e_cit = w.vs().find(cur_v.first);
    while (e_cit != end_e_cit && m_Xed_edges[w_ndx].find(e_cit->second) == m_Xed_edges[w_ndx].end())
    {
        if (do_h_step)
        {
            end_e_cit = w.hs().end();
            e_cit = w.hs().find(cur_v.second);
            if (e_cit != end_e_cit)
            {
                cur_v.first = e_cit->second.second;
                do_h_step = false;
                ns += std::abs(e_cit->second.second - e_cit->second.first);
                continue;
            }
        }
        else
        {
            end_e_cit = w.vs().end();
            e_cit = w.vs().find(cur_v.first);
            if (e_cit != end_e_cit)
            {
                cur_v.second = e_cit->second.second;
                do_h_step = true;
                ns += std::abs(e_cit->second.second - e_cit->second.first);
                continue;
            }
        }

        std::cout << "Who uncrossed the wires?!!!" << std::endl;
        break;
    }

    if (e_cit != end_e_cit)
    {
        crossed_edge_const_iterator ce_cit = m_Xed_edges[w_ndx].find(e_cit->second);
        m_first_Xs[w_ndx] = ce_cit->second;
        ns_slack = do_h_step ?
            std::abs(e_cit->second.second - ce_cit->second.second)
            : std::abs(e_cit->second.second - ce_cit->second.first);
    }
    else
        m_first_Xs[w_ndx] = wire::vertex_type(0, 0);

    return ns - ns_slack;
}

std::size_t wire_circuit::backtrack(const wire::vertex_type& v_X, std::size_t w_ndx)
{
    std::size_t ns = 0;
    std::size_t ns_slack = 0;
    
    const wire& w = m_circuit[w_ndx];
    wire::vertex_type cur_v(v_X);
    wire::edge_const_iterator e_cit = w.hs().find(cur_v.second);
    bool do_h_step = e_cit != w.hs().end();
    if (!do_h_step)
    {
        e_cit = w.vs().find(cur_v.first);
        ns_slack = e_cit == w.vs().end() ? 0 : std::abs(e_cit->second.second - cur_v.second);
    }
    else
        ns_slack = std::abs(e_cit->second.second - cur_v.first);
    while (cur_v.first != 0 || cur_v.second != 0)
    {
        if (do_h_step)
        {
            e_cit = w.hs().find(cur_v.second);
            if (e_cit != w.hs().end())
            {
                cur_v.first = e_cit->second.first;
                do_h_step = false;
                ns += std::abs(e_cit->second.second - e_cit->second.first);
                continue;
            }
        }
        else
        {
            e_cit = w.vs().find(cur_v.first);
            if (e_cit != w.vs().end())
            {
                cur_v.second = e_cit->second.first;
                do_h_step = true;
                ns += std::abs(e_cit->second.second - e_cit->second.first);
                continue;
            }
        }

        std::cout << "Who uncrossed the wires?!!!" << std::endl;
        break;
    }

    return ns - ns_slack;
}

std::size_t wire_circuit::backtrack(std::size_t X_ndx, std::size_t w_ndx)
{
    return backtrack(m_first_Xs[X_ndx], w_ndx);
}

void wire_circuit::backtrack_all(std::size_t w_ndx)
{
    std::cout << "All cross steps for wire " << w_ndx << ":\n";
    for (const auto& crossed_e : m_Xed_edges[w_ndx])
        std::cout << "\t(" << crossed_e.second.first << ", " << crossed_e.second.second << "): ns="
                  << backtrack(crossed_e.second, w_ndx) << '\n';
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

    std::cout << "First crosses:\n";
    for (auto& first_X : m_first_Xs)
        std::cout << "\t(" << first_X.first << ", " << first_X.second << ")\n";

    std::cout << "All crosses:\n";
    for (const auto& crossed_e : m_Xed_edges[0])
        std::cout << "\t(" << crossed_e.second.first << ", " << crossed_e.second.second << ")\n";
}

void wire_circuit::read_circuit(const std::string& circuitFileName)
{
    std::ifstream ifs(circuitFileName);
    std::copy(std::istream_iterator<std::string>(ifs),
              std::istream_iterator<std::string>(),
              std::back_inserter(m_text));
}

