#if !defined(WIRE_H)
#define WIRE_H

#include <map>
#include <utility>

class wire
{
public:
    typedef std::pair<int, int> vertex_type;
    typedef std::pair<int, int> edge_type;
    typedef std::multimap<int, edge_type> edge_map_type;

    const edge_map_type& hs() const { return m_h; }
    const edge_map_type& vs() const { return m_v; }

    void add_h_edge(int y, const edge_type& h_edge);
    void add_v_edge(int x, const edge_type& v_edge);
    vertex_type propagate(const vertex_type& v0, char d, int ns);

    void detect_degeneracy(); // detect overlapping edge segments

private:
    edge_map_type m_h;
    edge_map_type m_v;
};

#endif
