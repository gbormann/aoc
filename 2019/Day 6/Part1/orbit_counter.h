
#include <string>
#include <unordered_map>
#include <vector>

class orbit_counter
{
    typedef std::unordered_map<std::string, std::string> orbit_map_type;
    typedef orbit_map_type::const_iterator orbit_const_iterator;
    typedef orbit_map_type::value_type orbit_type;

    typedef std::unordered_map<std::string, std::size_t> count_map_type;
    typedef count_map_type::const_iterator count_const_iterator;

public:
    orbit_counter(const std::string& orbitsFileName);

    /** Use Map-Reduce to count total number of orbits */
    std::size_t count_orbits();

private:
    void read_orbits(const std::string& orbitsFileName);
    void create_orbit_map();
    std::size_t count(const orbit_type& orbit);

    std::vector<std::string> m_text;
    orbit_map_type m_orbit_map;
    count_map_type m_count_map;
    std::size_t m_hits = 0;
    std::size_t m_misses = 0;
};
