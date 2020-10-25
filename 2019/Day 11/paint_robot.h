
#if !defined(_PAINT_ROBOT_H)
#define _PAINT_ROBOT_H

#include <map>
#include <set>
#include <string>

#include "datatypes.h"
#include "intcode_vm.h"

class paint_robot
{
    typedef std::map<vertex_type, vertex_type> rot_map_type;
    typedef std::set<vertex_type> colour_map_type;
    typedef colour_map_type::const_iterator clr_const_iterator;

public:
    paint_robot(const std::string& progFileName);

    void run() { m_vm.run(*this); }

    void reset();

    // IOModule template implementation
    word_type readWord();
    void writeWord(const word_type& val);
    // --------------------------------

    std::size_t paintedPanelCount();
    void printLivery();

private:
    enum State { PAINTING, DRIVING };

    State paint(word_type colour);
    State turnAndStep(word_type direction);

    static const rot_map_type M_ROT[2]; // [0]: rot -pi/2 (L), [1]: rot pi/2 (R)

    intcode_vm m_vm;
    colour_map_type m_hull_livery; // sparse colour bitmap: absence=black, presence=white
    colour_map_type m_paint_log;   // sparse colour bitmap: set grows monotonically with painted panels
    vertex_type m_pos; // position
    vertex_type m_ornt; // orientation
    State m_state; // robot action state
};

#endif
