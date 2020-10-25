
#if !defined(_DATATYPES_H)
#define  _DATATYPES_H

#include <set>
#include <utility>

typedef long word_type;

typedef std::pair<int, int> vertex_type;
typedef std::set<vertex_type> vertex_set_type;
typedef vertex_set_type::const_iterator vertex_const_iterator;

#endif
