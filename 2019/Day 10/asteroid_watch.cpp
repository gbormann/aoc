
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "datatypes.h"
#include "gcd.h"
#include "asteroid_watch.h"

asteroid_watch::asteroid_watch(const std::string& astrdsFileName)
    : m_asteroids(read_asteroid_map(astrdsFileName))
{ }

vertex_type asteroid_watch::analyse_visibility()
{
    std::size_t max_cnt = 0;
    vertex_type vp_max;
    for (vertex_const_iterator vp_cit = m_asteroids.begin();
         vp_cit != m_asteroids.end(); ++vp_cit) // loop over vantage point
    {
        std::cout << "--------------------------------" 
                  << '(' << (vp_cit->first) << ", " << (vp_cit->second) << ")\n";

        ray_map_type rays(calculate_ray_map(*vp_cit));
        for (const auto& ray : rays)
        {
            std::cout << '(' << (ray.first.first) << ", " << (ray.first.second) << ")" << " -------- ";
            std::copy(ray.second.begin(), ray.second.end(), std::ostream_iterator<int>(std::cout, " "));
            std::cout << '\n';
        }

        std::cout << "#unoccluded asteroids: " << rays.size() << '\n';
        if (rays.size() > max_cnt)
        {
            vp_max = *vp_cit;
            max_cnt = rays.size();
        }
    }

    std::cout << "--------------------------------\n" 
              << "Optimal vantage point " << '(' << (vp_max.first) << ", " << (vp_max.second)
              << ") for " << max_cnt << " unoccluded asteroids\n";

    return vp_max;
}

vertex_type asteroid_watch::clear_field(std::size_t nr_to_destroy, const vertex_type& vp_opt)
{
    const ray_map_type rays(calculate_ray_map(vp_opt));

    azimuth_map_type azimuth_sorted;
    for (const auto& ray : rays)
        azimuth_sorted.insert(std::make_pair(M_PI-atan2(ray.first.first, ray.first.second), ray));        

    azimuth_iterator azimuth_it(azimuth_sorted.begin());
    for (std::size_t cnt = 1; cnt < nr_to_destroy; ++cnt)
    {
        if (azimuth_it == azimuth_sorted.end())
            azimuth_it = azimuth_sorted.begin();

        lambda_set_type& lambdas(azimuth_it->second.second);

        {
            const int lambda = *lambdas.begin();
            const vertex_type& b_lat(azimuth_it->second.first);
            std::cout << "asteroid #" << cnt << " vaporised @("
                      << (vp_opt.first + lambda * b_lat.first) << ", " 
                      << (vp_opt.second + lambda * b_lat.second) << ")\n";
        }
        lambdas.erase(lambdas.begin());
        if (lambdas.empty())
            azimuth_it = azimuth_sorted.erase(azimuth_it);
        else
            ++azimuth_it;
    }

    if (azimuth_it == azimuth_sorted.end())
        azimuth_it = azimuth_sorted.begin();

    const vertex_type& b_lat(azimuth_it->second.first);
    const int lambda = *azimuth_it->second.second.begin();
    
    return vertex_type(vp_opt.first + lambda * b_lat.first, vp_opt.second + lambda * b_lat.second);
}

vertex_set_type asteroid_watch::read_asteroid_map(const std::string& asteroidsFileName)
{
    std::vector<std::string> mapLines;

    std::ifstream ifs(asteroidsFileName);
    std::copy(std::istream_iterator<std::string>(ifs),
              std::istream_iterator<std::string>(),
              std::back_inserter(mapLines));

    vertex_set_type asteroids;
    for (std::size_t i = 0; i < mapLines.size(); ++i)
    {
        const std::string& curLine(mapLines[i]);
        for (std::size_t j = 0; j < curLine.size(); ++j)
            if (curLine[j] == '#')
                asteroids.insert(std::make_pair(int(j), int(i)));
    }

    return asteroids;
}

asteroid_watch::ray_map_type asteroid_watch::calculate_ray_map(const vertex_type& vp)
{
    typedef std::pair<ray_iterator, bool> ray_insert_res;

    ray_map_type rays;
    for (vertex_const_iterator asteroid_cit = m_asteroids.begin();
         asteroid_cit != m_asteroids.end(); ++asteroid_cit) // loop over body field
        if (*asteroid_cit != vp)
        {
            const vertex_type a_pos = std::make_pair(
                    asteroid_cit->first - vp.first,
                    asteroid_cit->second - vp.second
            );
            const int gcd_ij = gcd(a_pos.first, a_pos.second);
            const vertex_type b_lat(std::make_pair(a_pos.first / gcd_ij, a_pos.second / gcd_ij));
            ray_insert_res inserted = rays.insert(std::make_pair(b_lat, lambda_set_type()));
            inserted.first->second.insert(gcd_ij);
        }
    return rays;
}
