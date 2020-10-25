#if !defined(_IMG_MOD_H)
#define _IMG_MOD_H

#include <iostream>

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <curses.h>

#include "../datatypes.h"
#include "../intcode_vm.h"

class img_mod
{
public:
    img_mod(const std::string& progFileName);

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
    typedef std::vector<std::string> framebuffer_type;

    enum State { S_INTERACTIVE, S_CAPTURE, S_H_RETRACE };
    enum Space { UL, SCANPOS, NR_POS };

    void resetConsole();

    void paintGlyph(char glyph);
    void paintFrame();
    State setResponse(word_type response);

    void calcAlignment();

    std::ostringstream m_msgss;
    intcode_vm m_vm;
    State m_state;
    std::vector<vertex_type> m_pos;
    framebuffer_type m_fb;
    std::ostringstream m_painter; // scan-line painter
//    long m_pacing_delay; // in ms
    word_type m_ap;
};

#endif
