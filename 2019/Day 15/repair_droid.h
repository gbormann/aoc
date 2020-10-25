
#if !defined(_PAINT_ROBOT_H)
#define _PAINT_ROBOT_H

#include <iostream>

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <curses.h>

#include "datatypes.h"
#include "intcode_vm.h"

class repair_droid
{
public:
    repair_droid(const std::string& progFileName);

    void initConsole();
    void endConsole();
    void run();
    void reset();
    void printReport();

    // IOModule template implementation
    word_type readWord();
    void writeWord(const word_type& val);
    // --------------------------------

private:
    enum State { S_INTERACTIVE, S_PAINT_PATHS, S_GOTO_OXY, S_FIND_LONGEST_RPATH, S_FIND_LONGEST_LPATH, S_HALT };
    enum Tile { T_WALL=0, T_ROOM, T_OXY, T_DROID, T_START };
    enum Dir { NPOLE=0, NORTH, SOUTH, WEST, EAST, SPOLE };
    enum Spatial { CUR=0, STEP, START, OXY, NR_POS };
    enum Path { P_OXY=0, P_RWIND, P_LWIND, P_NONE };

    typedef std::vector<Dir> dir_stack_type; // need vector for the iterators
    typedef dir_stack_type::const_iterator dir_const_iterator;

    static Dir turnLeft(Dir dir);
    static Dir turnRight(Dir dir);
    static Dir revert(Dir dir);
    static Path toPath(State state);
    static vertex_type toStep(Dir dir);
    static char toDirSym(Dir dir);

    void resetConsole();

    char toGlyph(Tile tile);
    void paintTile(const vertex_type& pos, Tile tile);
    Dir turnToExplore(Dir dir);
    Dir turnToRecover(Dir dir);
    void updateTracing(Dir dir);
    State setResponse(word_type response);

    static const std::vector<vertex_type> M_STEPS; // unit vectors for each direction
    static const Dir M_ROT_R[6];
    static const Dir M_ROT_L[6];
    static const Dir M_REVERSE[6];
    static const Path M_STATE_PATH[6];
    static const char M_TILE_GLYPHS[5];
    static const char M_DIR_SYMS[6];

    std::ostringstream m_msgss;
    intcode_vm m_vm;
    State m_state;
    bool m_is_rp_done;
    long m_pacing_delay; // in ms
    Dir m_dir;
    std::vector<dir_stack_type> m_paths; // paths from a reduced trace (i.e. with dead-end branches removed) starting from S
    dir_const_iterator m_dir_cit;
    dir_const_iterator m_oxy_cit;
    std::vector<vertex_type> m_pos; // various spatial info
    std::vector<std::size_t> m_max_psize;
    
    bool m_is_oxy_reached;
};

#endif
