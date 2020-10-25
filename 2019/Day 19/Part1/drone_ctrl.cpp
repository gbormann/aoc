
#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <curses.h>

#include "../datatypes.h"
#include "../utils.h"
#include "../intcode_vm.h"

#include "drone_ctrl.h"

const char drone_ctrl::M_TILE_GLYPHS[2] = { '.', '#' };

drone_ctrl::drone_ctrl(const std::string& progFileName)
    : m_vm(progFileName)
    , m_state{ S_MAPPING }
    , m_mstate{ MS_MAX_Y_OUT }
    , m_pos{ NR_POS, vertex_type{ 0, 0 } }
    , m_bm{ 50 }
    , m_ba{ 0 }
{
    m_pos[UL] = std::make_pair(2, 5);
    m_pos[BB] = std::make_pair(49, 99);
//    m_pos[CUR].first = 1;
    std::fill_n(m_bm.begin(), m_bm.size(), std::make_pair(m_pos[BB].first, 0));
}

void drone_ctrl::initConsole()
{
    ::initscr();
    ::cbreak();
    ::noecho();
    ::nonl();
    ::intrflush(stdscr, FALSE);
    ::keypad(stdscr, TRUE);
    ::clear();
}

void drone_ctrl::endConsole()
{
    ::nl();
    ::echo();
    ::nocbreak();
    ::endwin();
}

void drone_ctrl::run()
{
    initConsole();
    while(m_state != S_INTERACTIVE)
    {
        m_vm.run(*this);
        m_vm.reset();
    }
}

void drone_ctrl::reset()
{
    m_vm.reset();
    resetConsole();
    m_state = S_MAPPING;
    m_mstate = MS_MAX_Y_OUT;
    m_ba = 0;
}

void drone_ctrl::printReport()
{
    std::cout << "Tractor beam area: " << m_ba << '\n';
    std::cout << "Trace: " << m_msgss.str() << '\n';
}

word_type drone_ctrl::readWord()
{
//    m_msgss << 'S' << m_state;
    word_type val = 0;
    switch (m_state)
    {
    case S_MAPPING:
        m_state = S_READ_X;
    case S_READ_X:
        if (m_pos[CUR].first == m_pos[BB].first)
        {
            if (m_pos[CUR].second == m_pos[BB].second)
            {
                m_state = S_REPORT;
                break;
            }
            m_pos[CUR].first = 0;
            ++m_pos[CUR].second;
        }
//        m_msgss << 'X' << m_pos[CUR].second;
        val = m_pos[CUR].second;
        m_state = S_READ_Y;
        break;
    case S_READ_Y:
        val = m_pos[CUR].first++;
//        m_msgss << 'Y' << m_pos[CUR].first;
        m_state = S_READ_X;
        break;
    case S_REPORT:
//        m_msgss << 'r';
        // TODO Calculate beam area!!
        // m_ba = 0XDEADBEEF;
        m_state = S_INTERACTIVE;
    case S_INTERACTIVE:
        m_msgss << 'I';
        val = word_type(::getch());
        putchar(val);
        refresh();
        switch(val)
        {
        case ERR:
        case int('q'):
        default:
            endConsole();
            printReport();
            exit(0);
        }
        break;
    default:
        std::ostringstream oss;
        oss << "Unexpected read instruction! state=" << m_state << '\n';
        throw std::runtime_error(oss.str());
    }

    return val;
}

void drone_ctrl::writeWord(const word_type& val)
{
    m_mstate = setResponse(val);
}

void drone_ctrl::resetConsole()
{
    ::clear();
    std::fill_n(m_pos.begin(), m_pos.size(), vertex_type(0, 0));
    m_pos[UL] = std::make_pair(2, 5);
    m_pos[BB] = std::make_pair(49, 99);
//    m_pos[CUR].first = 1;
    std::fill_n(m_bm.begin(), m_bm.size(), std::make_pair(m_pos[BB].first, 0));
}

char drone_ctrl::toGlyph(drone_ctrl::Tile tile)
{
    return M_TILE_GLYPHS[tile];
}

void drone_ctrl::paintGlyph(char glyph)
{
    mvaddch(m_pos[UL].first + m_pos[CUR].first, m_pos[UL].second + m_pos[CUR].second, glyph);
    refresh();
}

drone_ctrl::MapState drone_ctrl::setResponse(word_type response)
{
    MapState mstate = m_mstate;
//    m_msgss << 'O' << response;
    paintGlyph(toGlyph(static_cast<Tile>(response)));
    switch(response)
    {
/*
    case T_FREE:
        switch(mstate)
        {
        case MS_MAX_Y_OUT:
            m_msgss << "Mof" << m_pos[CUR].second;
            if (m_pos[CUR].second == m_pos[BB].second)
            {
                m_msgss << "Box";
                mstate = MS_RANGE_Y_OUTSIDE;
                break;
            }
            ++m_pos[CUR].second;
            mstate = MS_MAX_Y_IN;
            break;
        case MS_MAX_Y_IN:
            m_msgss << "Mif" << m_pos[CUR].first;
            --m_pos[CUR].first;
            break;
        case MS_RANGE_Y_OUTSIDE:
            m_msgss << "Rof" << m_pos[CUR].first;
            --m_pos[CUR].first;
            break;
        case MS_RANGE_Y:
            m_msgss << "Rf" << m_pos[CUR].second;
            --m_pos[CUR].second;
            mstate = MS_MIN_Y_IN;
            break;
        case MS_MIN_Y_OUT:
            m_msgss << "mof" << m_pos[CUR].second;
            if (m_pos[CUR].second == 0)
            {
                m_msgss << "Box";
                m_state = S_REPORT;
                break;
            }
            --m_pos[CUR].second;
            mstate = MS_MIN_Y_IN;
            break;
        case MS_MIN_Y_IN:
            m_msgss << "mif" << m_pos[CUR].first;
            ++m_pos[CUR].first;
            break;
        default:
            std::ostringstream oss;
            oss << "Unexpected new-line! mstate=" << mstate << '\n';
            throw std::runtime_error(oss.str());
        }
        break;
*/
    case T_PULLED:
        ++m_ba;
/*
        switch(mstate)
        {
        case MS_MAX_Y_OUT:
            m_msgss << "Mo" << m_pos[CUR].first;
            m_bm[m_pos[CUR].second].second = std::max(m_bm[m_pos[CUR].second].second, m_pos[CUR].first);
            if (m_pos[CUR].first == m_pos[BB].first)
            {
                m_msgss << "Boy" << m_pos[CUR].second;
                if (m_pos[CUR].second == m_pos[BB].second)
                {
                    m_msgss << "Box";
                    mstate = MS_RANGE_Y;
                    break;
                }
                ++m_pos[CUR].second;
                break;
            }
            ++m_pos[CUR].first;
            break;
        case MS_MAX_Y_IN:
            m_msgss << "Mi" << m_pos[CUR].first;
            m_bm[m_pos[CUR].second].second = std::max(m_bm[m_pos[CUR].second].second, m_pos[CUR].first);
            if (m_pos[CUR].second == m_pos[BB].second)
            {
                m_msgss << "Bix" << m_pos[CUR].second;
                mstate = MS_RANGE_Y;
                break;
            }
            ++m_pos[CUR].second;
            mstate = MS_MAX_Y_OUT;
            break;
        case MS_RANGE_Y_OUTSIDE:
            m_msgss << "Ro" << m_pos[CUR].first;
            m_bm[m_pos[CUR].second].second = std::max(m_bm[m_pos[CUR].second].second, m_pos[CUR].first);
            if (m_pos[CUR].first == 0)
            {
                m_msgss << "By0" << m_pos[CUR].second;
                --m_pos[CUR].second;
                mstate = MS_MIN_Y_IN;
                break;
            }
            --m_pos[CUR].first;
            mstate = MS_RANGE_Y;
            break;
        case MS_RANGE_Y:
            m_msgss << 'R' << m_pos[CUR].first;
            m_bm[m_pos[CUR].second].first = std::min(m_bm[m_pos[CUR].second].first, m_pos[CUR].first);
            if (m_pos[CUR].first == 0)
            {
                m_msgss << "By0" << m_pos[CUR].second;
                --m_pos[CUR].second;
                mstate = MS_MIN_Y_OUT;
                break;
            }
            --m_pos[CUR].first;
            break;
        case MS_MIN_Y_OUT:
            m_msgss << "mo" << m_pos[CUR].first;
            m_bm[m_pos[CUR].second].first = std::min(m_bm[m_pos[CUR].second].first, m_pos[CUR].first);
            if (m_pos[CUR].first == 0)
            {
                m_msgss << "by0" << m_pos[CUR].second;
                if (m_pos[CUR].second == 0)
                {
                    m_msgss << "box0";
                    m_state = S_REPORT;
                    break;
                }
                --m_pos[CUR].second;
                break;
            }
            --m_pos[CUR].first;
            break;
        case MS_MIN_Y_IN:
            m_msgss << "mi" << m_pos[CUR].first;
            m_bm[m_pos[CUR].second].first = std::min(m_bm[m_pos[CUR].second].first, m_pos[CUR].first);
            if (m_pos[CUR].second == m_pos[BB].second)
            {
                m_msgss << "bix" << m_pos[CUR].second;
                m_state = S_REPORT;
                break;
            }
            --m_pos[CUR].second;
            mstate = MS_MIN_Y_OUT;
            break;
        default:
            std::ostringstream oss;
            oss << "Unexpected new-line! mstate=" <<  mstate << '\n';
            throw std::runtime_error(oss.str());
        }
        break;
*/
    default:
//        m_msgss << " ?" << response;
        // nothing to do
        break;
    }

    return mstate;
}
