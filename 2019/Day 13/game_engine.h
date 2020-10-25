
#if !defined(_PAINT_ROBOT_H)
#define _PAINT_ROBOT_H

#include <map>
#include <set>
#include <sstream>
#include <string>

#include <curses.h>

#include "datatypes.h"
#include "intcode_vm.h"

enum Tile { T_SPACE=0, T_WALL, T_BLOCK, T_HPADDLE, T_BALL };

class game_engine
{
public:
    game_engine(const std::string& progFileName);

    void initConsole();
    void endConsole();
    void run();
    void reset();

    // IOModule template implementation
    word_type readWord(bool is_auto = true);
    void writeWord(const word_type& val);
    // --------------------------------

    std::size_t blockCount() { return m_nblocks; }
    vertex_type hPaddlePos() { return m_pos[HPAD]; }
    vertex_type initialBallPos() { return m_pos[BALL]; }
    const std::string& score() { return m_score; }

private:
    typedef std::set<vertex_type> block_set_type;
    typedef block_set_type::const_iterator block_const_iterator;
    
    enum State { AWAITING_X, AWAITING_Y, SKIP_Y, AWAITING_TILE, AWAITING_SCORE };
    enum Spatial { CUR=0, PREV, BALL, HPAD, MAX, LOWB, BBLR, NR_POS };

    State setX(word_type r_x);
    State setY(word_type r_y);
    State skipY(word_type dummy);
    State setTile(word_type tile);
    State setScore(word_type score);

    vertex_type velocity();
    word_type baselineIntercept(const vertex_type& v);

    static const char M_TILE_GLYPHS[5];

    std::ostringstream m_reportss;
    intcode_vm m_vm;
    block_set_type m_blocks;
    std::vector<vertex_type> m_pos; // various spatial info
    std::string m_score; // game score
    State m_state; // robot action state
    std::size_t m_nblocks; // #block tiles
    bool m_game_on;
};

#endif
