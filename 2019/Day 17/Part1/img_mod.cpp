
#include <algorithm>
//#include <chrono>
#include <functional>
#include <iostream>
#include <string>
//#include <thread>
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
    , m_state{ S_INTERACTIVE }
    , m_pos{ NR_POS, vertex_type{ 0, 0 } }
{
    m_pos[UL] = std::make_pair(5, 10);
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
    readWord();
    m_vm.run(*this);
    calcAlignment();
    paintFrame();
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
    std::cout << "Alignment parameter: " << m_ap << '\n';
}

word_type img_mod::readWord()
{
    switch(m_state)
    {
    case S_INTERACTIVE:
        switch(::getch())
        {
        case int('c'):
            m_state = S_CAPTURE;
            break;
        case ERR:
        case int('q'):
        default:
            endConsole();
            printReport();
            exit(0);
        }
        break;
    default:
        throw std::runtime_error("Unexpected read instruction!");
    }

    return 0;
}

void img_mod::writeWord(const word_type& val)
{
    m_state = setResponse(val);

//    std::this_thread::sleep_for(milliseconds{ m_pacing_delay });
}

void img_mod::resetConsole()
{
    ::clear();
    std::fill_n(m_pos.begin(), m_pos.size(), vertex_type(0, 0));
    m_pos[UL] = std::make_pair(5, 10);
    m_fb.clear();
    m_painter.str("");
}

void img_mod::paintGlyph(char glyph)
{
    m_painter << glyph;
}

void img_mod::paintFrame()
{
    m_pos[SCANPOS].first = 0;
    for (const std::string& scanline : m_fb)
    {
        m_pos[SCANPOS].second = 0;
        for (const char glyph : scanline)
            mvaddch(m_pos[UL].first + m_pos[SCANPOS].first, m_pos[UL].second + m_pos[SCANPOS].second++, glyph);
        ++m_pos[SCANPOS].first;
    }
    ::refresh();
}   

img_mod::State img_mod::setResponse(word_type response)
{
    State state = m_state;
    switch(response)
    {
    case '\r':
    case '\n':
        state = S_H_RETRACE;
        break;
    default:
        switch(state)
        {
        case S_H_RETRACE:
            m_fb.push_back(m_painter.str());
            m_painter.str("");
            state = S_CAPTURE;
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

void img_mod::calcAlignment()
{
    m_ap = 0;
    for (std::size_t y = 1; y < m_fb.size() - 1; ++y)
        for (std::size_t x = 1; x < m_fb[y].size() - 1; ++x)
            if (m_fb[y].at(x) == '#'
                && m_fb[y - 1].at(x) == '#' && m_fb[y + 1].at(x) == '#'
                && m_fb[y].at(x - 1) == '#' && m_fb[y].at(x + 1) == '#')
            {
                m_fb[y].at(x) = 'O';
                m_ap += static_cast<word_type>(x * y);
            }
}   
