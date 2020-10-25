
#if !defined(_UTILS_H)
#define _UTILS_H

#include "datatypes.h"

vertex_type operator+(const vertex_type& lhs, const vertex_type& rhs)
{
    return vertex_type(lhs.first + rhs.first, lhs.second + rhs.second);
}

#endif
