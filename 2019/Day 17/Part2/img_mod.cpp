
#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <curses.h>

#include "../datatypes.h"
#include "../utils.h"
#include "../intcode_vm.h"

#include "img_mod.h"

//using namespace std::chrono;

img_mod::img_mod(const std::string& progFileName)
    : m_vm(progFileName)
    , m_state{ S_CAPTURE }
    , m_pos{ NR_POS, vertex_type{ 0, 0 } }
    , m_rom("A,B,A,C,A,B,A,C,B,C\nR,4,L,12,L,8,R,4\nL,8,R,10,R,10,R,6\nR,4,R,10,L,12\n")
    , m_rp(m_rom.cbegin())
{
    m_pos[UL] = std::make_pair(2, 10);
    m_pos[LR] = std::make_pair(41, 45);
    m_ctx.push(S_LD_APP);
}

void img_mod::initConsole()
{
    ::initscr();
    ::cbreak();
    ::noecho();
    ::nonl();
    ::intrflush(stdscr, FALSE);
    ::keypad(stdscr, TRUE);
    ::clear();
}

void img_mod::endConsole()
{
    ::nl();
    ::echo();
    ::nocbreak();
    ::endwin();
}

void img_mod::run()
{
    initConsole();
    m_vm.run(*this);
    m_state = S_INTERACTIVE;
    readWord();
}

void img_mod::reset()
{
    m_vm.reset();
    resetConsole();
    m_state = S_CAPTURE;
}

void img_mod::printReport()
{
    std::cout << "Units of stardust collected: " << m_dustvol << '\n';
//    std::cout << "Trace: " << m_msgss.str() << '\n';
}

word_type img_mod::readWord()
{
    word_type val = 0;
    if (m_state == S_PROMPT)
    {
        m_state = m_ctx.top();
        m_ctx.pop();
    }
    switch (m_state)
    {
    case S_LD_APP:
        val = word_type(*m_rp++);
        if (val == word_type('\n'))
        {
            m_state = setPostPromptContext(S_LD_LIB_FA);
            break;
        }
        putchar(val);
        refresh();
        break;
    case S_LD_LIB_FA:
        val = word_type(*m_rp++);
        if (val == word_type('\n'))
        {
            m_state = setPostPromptContext(S_LD_LIB_FB);
            break;
        }
        putchar(val);
        refresh();
        break;
    case S_LD_LIB_FB:
        val = word_type(*m_rp++);
        if (val == word_type('\n'))
        {
            m_state = setPostPromptContext(S_LD_LIB_FC);
            break;
        }
        putchar(val);
        refresh();
        break;
    case S_LD_LIB_FC:
        val = word_type(*m_rp++);
        if (val == word_type('\n'))
        {
            m_state = setPostPromptContext(S_INTERACTIVE);
            break;
        }
        putchar(val);
        refresh();
        break;
    case S_INTERACTIVE:
        val = word_type(::getch());
        putchar(val);
        refresh();
        switch(val)
        {
        case word_type('n'):
        case word_type('y'):
            m_state = S_WARN;
            break;
        case ERR:
        case int('q'):
        default:
            endConsole();
            printReport();
            exit(0);
        }
        break;
    case S_WARN:
        val = '\n';
        m_state = S_H_RETRACE;
        break;
    default:
        std::ostringstream oss;
        oss << "Unexpected read instruction! state=" << m_state << '\n';
        throw std::runtime_error(oss.str());
    }

    return val;
}

void img_mod::writeWord(const word_type& val)
{
    if (val > 255)
    {
        m_dustvol = val;
        m_state = S_REPORT;
        return;
    }
    m_state = setResponse(val);
}

void img_mod::resetConsole()
{
    ::clear();
    std::fill_n(m_pos.begin(), m_pos.size(), vertex_type(0, 0));
    m_pos[UL] = std::make_pair(2, 10);
    m_pos[LR] = std::make_pair(41, 45);
    m_fb.clear();
    m_painter.str("");
}

img_mod::State img_mod::setPostPromptContext(img_mod::State postPromptState)
{
    ++m_pos[PROMPT].first;
    m_pos[PROMPT].second = 0;
    m_ctx.push(postPromptState);
    return S_PROMPT;
}

void img_mod::paintGlyph(char glyph)
{
    m_painter << glyph;
    ++m_pos[SCANPOS].second;
}

void img_mod::paintFrame()
{
    ::clear();
    m_pos[SCANPOS].first = 0;
    for (const std::string& scanline : m_fb)
    {
        m_pos[SCANPOS].second = 0;
        for (const char glyph : scanline)
            mvaddch(m_pos[UL].first + m_pos[SCANPOS].first, m_pos[UL].second + m_pos[SCANPOS].second++, glyph);
        ++m_pos[SCANPOS].first;
    }
    refresh();
    m_fb.clear();

    std::this_thread::sleep_for(std::chrono::milliseconds{ m_pacing_delay });
}   

void img_mod::putchar(char ch)
{
    mvaddch(m_pos[UL].first + m_pos[LR].first + m_pos[PROMPT].first, m_pos[UL].second + m_pos[PROMPT].second++, ch);
}

img_mod::State img_mod::setResponse(word_type response)
{
    State state = m_state;
    switch(response)
    {
    case '\r':
    case '\n':
        switch(state)
        {
        case S_CAPTURE:
            m_fb.push_back(m_painter.str());
            m_painter.str("");
            state = S_H_RETRACE;
            break;
        case S_H_RETRACE:
            m_fb.push_back(m_painter.str());
            m_painter.str("");
            paintFrame();
            state = m_pos[PROMPT].first < 4 ? S_PROMPT : S_V_RETRACE;
            break;
        case S_PROMPT:
            refresh();
            break;
        default:
            std::ostringstream oss;
            oss << "Unexpected new-line! state=" << state << '\n';
            throw std::runtime_error(oss.str());
        }
        break;
    default:
        switch(state)
        {
        case S_PROMPT:
            putchar(response);
            break;
        case S_V_RETRACE:
            m_pos[SCANPOS] = std::make_pair(0, 0);;
            paintGlyph(response);
            state = S_CAPTURE;
            break;
        case S_H_RETRACE:
            ++m_pos[SCANPOS].first;
            m_pos[SCANPOS].second = 0;
            paintGlyph(response);
            state = S_CAPTURE;
            break;
        case S_CAPTURE:
            paintGlyph(response);
            break;
        default:
            // nothing to do for now
            break;
        }
    }

    return state;
}
