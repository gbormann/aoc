
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "reactor.h"

reactor::reactor(const std::string& reactionsFileName)
{
    read_reactions(reactionsFileName);
    parse_reactions();
}

long reactor::obtain_units(const std::string& product, long tgt_units)
{
    using namespace std::placeholders;

    react(m_index.find(product)->second, tgt_units);

    // restore mass balance
    quantity_iterator last_it = m_quantities.begin() + m_quantities.size() - 1;
    auto has_shortage = std::bind(std::less<long>(), _1, 0);
    for (quantity_iterator shortage_it = std::find_if(m_quantities.begin(), last_it, has_shortage);
         shortage_it != last_it;
         shortage_it = std::find_if(m_quantities.begin(), last_it, has_shortage))
        react(shortage_it - m_quantities.begin());

    return -m_quantities.back(); // ORE cost to restore mass balance
}

long reactor::obtain(const std::string& product, const std::string& source, long max_units)
{
    std::pair<long, long> ore_cost_bracket{1, 1};
    std::pair<long, long> tgt_unit_bracket{200000, 400000};
    long cur_tgt_units;
    long cur_ore_cost;

    std::fill_n(m_quantities.begin(), m_quantities.size(), 0);
    ore_cost_bracket.first = obtain_units(product, tgt_unit_bracket.first);
    ore_cost_bracket.second = obtain_units(product, tgt_unit_bracket.second);
    std::cout << '[' << tgt_unit_bracket.first << ", " << tgt_unit_bracket.second << "] units FUEL for ["
              << ore_cost_bracket.first << ", " << ore_cost_bracket.second << "] units ORE" << std::endl;
    while (ore_cost_bracket.second < max_units) // bracket
    {
        std::fill_n(m_quantities.begin(), m_quantities.size(), 0);

        ore_cost_bracket.first = ore_cost_bracket.second;
        tgt_unit_bracket.first = tgt_unit_bracket.second;

        tgt_unit_bracket.second *= 2;
        ore_cost_bracket.second = obtain_units(product, tgt_unit_bracket.second);
        std::cout << '[' << tgt_unit_bracket.first << ", " << tgt_unit_bracket.second << "] units FUEL for ["
                  << ore_cost_bracket.first << ", " << ore_cost_bracket.second << "] units ORE" << std::endl;
    }

    while (true) // bisect
    {
        std::fill_n(m_quantities.begin(), m_quantities.size(), 0);
        if (std::abs(tgt_unit_bracket.first - tgt_unit_bracket.second) > 1)
            cur_tgt_units = (tgt_unit_bracket.first + tgt_unit_bracket.second) / 2;
        else
        {
            ore_cost_bracket.first = obtain_units(product, tgt_unit_bracket.first);
            ore_cost_bracket.second = obtain_units(product, tgt_unit_bracket.second);
            std::cout << '[' << tgt_unit_bracket.first << ", " << tgt_unit_bracket.second << "] units FUEL for ["
                      << ore_cost_bracket.first << ", " << ore_cost_bracket.second << "] units ORE\n";
            return ore_cost_bracket.second > max_units ? tgt_unit_bracket.first : tgt_unit_bracket.second;
        }

        cur_ore_cost = obtain_units(product, cur_tgt_units);

        if (cur_ore_cost == max_units)
            return cur_tgt_units;

        if (cur_ore_cost < max_units)
        {
            ore_cost_bracket.first = cur_ore_cost;
            tgt_unit_bracket.first = cur_tgt_units;
        }
        else
        {
            ore_cost_bracket.second = cur_ore_cost;
            tgt_unit_bracket.second = cur_tgt_units;
        }
        std::cout << '[' << tgt_unit_bracket.first << ", " << tgt_unit_bracket.second << "] units FUEL for ["
                  << ore_cost_bracket.first << ", " << ore_cost_bracket.second << "] units ORE" << std::endl;
    }
}

void reactor::read_reactions(const std::string& reactionsFileName)
{
    std::vector<std::string> reactions;
    std::ifstream ifs(reactionsFileName);
    std::copy(std::istream_iterator<std::string>(ifs),
              std::istream_iterator<std::string>(),
              std::back_inserter(m_text));
}

std::string reactor::parse_product(std::size_t ndx_r, const std::string& product)
{
    std::size_t stoichio;
    std::string name;

    std::istringstream iss(product);
    iss >> stoichio >> name;

    m_index[name] = ndx_r;
    m_products[ndx_r] = stoichio;

    return name;
}

reactor::reactant_map_type reactor::parse_reactants(const std::string& reactants_spec)
{
    reactant_map_type reactants;

    std::istringstream iss(reactants_spec);
    while (!iss.eof())
    {
        std::size_t stoichio;
        std::string name;
        iss >> stoichio >> name;

        reactants.insert(std::make_pair(m_index.find(name)->second, stoichio));
    }

    return reactants;
}

void reactor::parse_reactions()
{
    const std::size_t nr_reactants{m_text.size() + 1};
    m_quantities.resize(nr_reactants, 0);
    m_products.resize(nr_reactants);
    m_reactions.resize(nr_reactants);

    std::unordered_map<std::string, std::string> reactants_unparsed; // some reactions point forward in the list
    // Parse reaction products and product stoichio
    for (std::size_t ndx_r = 0; ndx_r < m_text.size(); ++ndx_r)
    {
        std::string reaction_line{m_text[ndx_r]};
        for (char& ch : reaction_line)
            if (ch == ',')
                ch = ' ';

        const std::size_t colon_pos = reaction_line.find(":"); // assuming it is always there as it's unrecoverable anyway
        const std::string reactant_stoichio{reaction_line.substr(0, colon_pos)};
        const std::string product{reaction_line.substr(colon_pos + 1)};
        const std::string product_name{parse_product(ndx_r, product)};

        reactants_unparsed.insert(std::make_pair(product_name, reactant_stoichio));
    }
    m_index["ORE"] = m_text.size(); // \
    m_products[m_text.size()] = 0;  // / source reactant

    // Parse reactant stoichio, using established reaction product indexing
    for (const auto& reaction : reactants_unparsed)
        m_reactions[m_index.find(reaction.first)->second] = parse_reactants(reaction.second);
}

void reactor::react(std::size_t ndx_r, long nr_starter_rounds)
{
    const long prod_stoichio{m_products[ndx_r]};
    const long prod_quantity{m_quantities[ndx_r]};

    long nr_rounds = prod_quantity == 0 ? nr_starter_rounds : (-prod_quantity / prod_stoichio);
    if (prod_quantity != 0 && ((-prod_quantity) % prod_stoichio) > 0)
        ++nr_rounds;

    m_quantities[ndx_r] += nr_rounds * prod_stoichio;
    const reactant_map_type& reaction{m_reactions[ndx_r]};
    for (const auto& reactant : reaction)
        m_quantities[reactant.first] -= nr_rounds * reactant.second;

//    std::copy(m_quantities.begin(), m_quantities.end(), std::ostream_iterator<long>(std::cout, " "));
//    std::cout << std::endl;
}
