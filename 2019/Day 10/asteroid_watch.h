
#if !defined(_ASTEROID_WATCH_H)
#define _ASTEROID_WATCH_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "datatypes.h"

class asteroid_watch
{
public:
    typedef std::set<int> lambda_set_type;
    typedef std::map<vertex_type, lambda_set_type> ray_map_type;

    asteroid_watch(const std::string& astrdsFileName);
    vertex_type analyse_visibility();
    vertex_type clear_field(std::size_t nr_to_destroy, const vertex_type& vp_opt);

private:
    typedef ray_map_type::value_type ray_value_type;
    typedef ray_map_type::const_iterator ray_const_iterator;
    typedef ray_map_type::iterator ray_iterator;

    typedef std::map<double, ray_value_type> azimuth_map_type;
    typedef azimuth_map_type::iterator azimuth_iterator;

    vertex_set_type read_asteroid_map(const std::string& asteroidsFileName);
    ray_map_type calculate_ray_map(const vertex_type& vp);

    vertex_set_type m_asteroids;
};

#endif
