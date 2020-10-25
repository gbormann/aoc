#if !defined(_IMG_MOD_H)
#define _IMG_MOD_H

#include <iostream>

#include <map>
#include <sstream>
#include <stack>
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

    enum State { S_CAPTURE, S_H_RETRACE, S_V_RETRACE, S_PROMPT, S_LD_APP, S_LD_LIB_FA, S_LD_LIB_FB, S_LD_LIB_FC,
                 S_WARN, S_INTERACTIVE, S_REPORT };
    enum Space { UL, LR, SCANPOS, PROMPT, NR_POS };

     void resetConsole();

    State setPostPromptContext(State postPromptState);
    void paintGlyph(char glyph);
    void paintFrame();
    void putchar(char ch);
    State setResponse(word_type response);

    const std::string m_rom;
    std::ostringstream m_msgss;
    intcode_vm m_vm;
    std::stack<State> m_ctx;
    State m_state;
    std::vector<vertex_type> m_pos;
    framebuffer_type m_fb;
    std::ostringstream m_painter; // scan-line painter
    std::string::const_iterator m_rp;
    word_type m_dustvol;
    long m_pacing_delay = 40; // in ms
};

#endif
