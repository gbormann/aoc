
#if !defined(_REACTOR_H)
#define _REACTOR_H

#include <string>
#include <unordered_map>
#include <vector>

class reactor
{
public:
    reactor(const std::string& reactionsFileName);

    long obtain_units(const std::string& product, long tgt_units = 1);
    long obtain(const std::string& product, const std::string& source, long max_units);

private:
    typedef std::unordered_map<std::string, std::size_t> index_map_type;
    typedef std::vector<long> quantity_list_type;
    typedef quantity_list_type::iterator quantity_iterator;
    typedef std::vector<long> product_list_type;

    typedef std::unordered_map<std::size_t, long> reactant_map_type;
    typedef std::vector<reactant_map_type> reaction_map_type;

    void read_reactions(const std::string& reactionsFileName);
    std::string parse_product(std::size_t ndx_r, const std::string& product);
    reactant_map_type parse_reactants(const std::string& reactants_spec);
    void parse_reactions();

    void react(std::size_t ndx_r, long nr_starter_rounds = 1);

    std::vector<std::string> m_text;
    index_map_type m_index;
    quantity_list_type m_quantities;
    product_list_type m_products;
    reaction_map_type m_reactions;
};

#endif
