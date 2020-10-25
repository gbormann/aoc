
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

#include "orbit_counter.h"

orbit_counter::orbit_counter(const std::string& orbitsFileName)
{
    read_orbits(orbitsFileName);
    create_orbit_map();
}

/** Use Map-Reduce to count total number of orbits */
std::size_t orbit_counter::count_orbits()
{
    for (const auto& orbit : m_orbit_map)
        if (m_count_map.find(orbit.first) == m_count_map.end())
        {
            ++m_misses;
            m_count_map[orbit.first] = count(orbit) + 1;
        }
        else
            ++m_hits;

    std::cout << "\n[CACHE DIAG: hits=" << m_hits << ", misses=" << m_misses << "]\n";

    std::size_t arity = 0;
    for (const auto& count : m_count_map)
        arity += count.second;

    return arity;
}

void orbit_counter::read_orbits(const std::string& orbitsFileName)
{
    std::vector<std::string> orbits;
    std::ifstream ifs(orbitsFileName);
    std::copy(std::istream_iterator<std::string>(ifs),
              std::istream_iterator<std::string>(),
              std::back_inserter(m_text));
}

void orbit_counter::create_orbit_map()
{
    for (const auto& orbit : m_text)
        m_orbit_map[orbit.substr(4, 3)] = orbit.substr(0, 3);
}

std::size_t orbit_counter::count(const orbit_type& orbit)
{
    if (orbit.second == "COM")
        return 0;

    count_const_iterator inner_cit = m_count_map.find(orbit.second);
    if (inner_cit != m_count_map.end())
    {
        ++m_hits;
        return inner_cit->second;
    }

    ++m_misses;
    orbit_const_iterator orbit_cit = m_orbit_map.find(orbit.second);
    std::size_t arity = count(*orbit_cit) + 1;
    m_count_map[orbit.second] = arity;
    return arity;
}
