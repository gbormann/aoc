#if !defined(_IMG_MOD_H)
#define _IMG_MOD_H

#include <iostream>

#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

#include <curses.h>

#include "../datatypes.h"
#include "../intcode_vm.h"

class drone_ctrl
{
public:
    drone_ctrl(const std::string& progFileName);

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
    enum State { S_MAPPING, S_READ_X, S_READ_Y, S_INTERACTIVE, S_REPORT };
    enum MapState { MS_MAX_Y_OUT, MS_MAX_Y_IN, MS_RANGE_Y_OUTSIDE, MS_RANGE_Y, MS_MIN_Y_OUT, MS_MIN_Y_IN };
    enum Tile { T_FREE=0, T_PULLED };
    enum Space { UL, BB, CUR, NR_POS };

    typedef std::vector<std::pair<word_type, word_type>> beam_map_type;

    static char toGlyph(Tile tile);

    void resetConsole();

    void paintGlyph(char glyph);
    MapState setResponse(word_type response);

    static const char M_TILE_GLYPHS[2];

    std::ostringstream m_msgss;
    intcode_vm m_vm;
    State m_state;
    MapState m_mstate;
    std::vector<vertex_type> m_pos;
    beam_map_type m_bm;
    word_type m_ba; // beam area
    long m_pacing_delay = 40; // in ms
};

#endif
