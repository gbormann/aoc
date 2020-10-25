
#if !defined(_DATATYPES_H)
#define  _DATATYPES_H

#include <set>
#include <utility>

typedef int word_type;

typedef std::pair<word_type, word_type> vertex_type;
typedef std::set<vertex_type> vertex_set_type;
typedef vertex_set_type::const_iterator vertex_const_iterator;

#endif
