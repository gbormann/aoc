
#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "datatypes.h"
#include "intcode_vm.h"

#include "game_engine.h"

using namespace std::chrono;

const char game_engine::M_TILE_GLYPHS[5] = { ' ', '#', '=', 'T', 'O' };

game_engine::game_engine(const std::string& progFileName)
    : m_vm(progFileName)
    , m_pos(NR_POS, vertex_type(-1, -1))
    , m_state(AWAITING_X)
    , m_score("0")
    , m_nblocks(0)
    , m_game_on(false)
{
}

void game_engine::initConsole()
{
    initscr();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    clear();
}

void game_engine::endConsole()
{
    nl();
    echo();
    nocbreak();
    endwin();
}

void game_engine::run()
{
    initConsole();
    m_vm.run(*this);
    readWord(false);
    endConsole();
    std::cout << m_reportss.str();
}

void game_engine::reset()
{
    m_vm.reset();
    std::fill_n(m_pos.begin(), NR_POS, vertex_type(-1, -1));
    m_score = "0";
    m_nblocks = 0;
    m_game_on = false;
}
 
word_type game_engine::readWord(bool is_auto)
{
    if (!m_game_on)
        m_game_on = true;

    if (is_auto) // computer plays computer
    {
        const vertex_type v{velocity()};
        const word_type k_out{baselineIntercept(v)};
        const word_type k_gap = k_out - m_pos[HPAD].first;
        const word_type k_gap_ball = m_pos[BALL].first - m_pos[HPAD].first;
//        m_reportss << v.first << ' ' << k_out << ' ' << k_gap << ' ' << m_pos[HPAD].first << '\n';
        const bool is_on_tgt = (m_blocks.size() != 1 && k_gap == 0) || k_gap == v.first;
        const bool is_on_tgt_ball = k_gap_ball == 0/* || k_gap_ball == v.first*/;
        const bool is_chasing = k_gap * v.first > 0;
        const bool is_chasing_ball = k_gap_ball * v.first > 0;
        return v.second > 0 ?
                (is_on_tgt ? 0 : (is_chasing ? v.first : -v.first))
                : (is_on_tgt_ball ? 0 : (is_chasing_ball ? v.first : -v.first));
    }
    else
    {
        switch(getch())
        {
        case int('l'):
        case KEY_LEFT: return -1;
        case int('r'):
        case KEY_RIGHT: return 1;
        case KEY_UP:
        case ERR: return 0;
        case int('q'):
            endConsole();
            exit(0);
        }
        std::this_thread::sleep_for(milliseconds{100});
    }
    return 0;
}

void game_engine::writeWord(const word_type& val)
{
    switch(m_state)
    {
    case AWAITING_X:
        m_state = setX(val);
        break;
    case AWAITING_Y:
        m_state = setY(val);
        break;
    case SKIP_Y:
        m_state = skipY(val);
        break;
    case AWAITING_TILE:
        m_state = setTile(val);
        break;
    case AWAITING_SCORE:
        m_state = setScore(val);
        break;
    }

    if (m_game_on)
        std::this_thread::sleep_for(milliseconds{1});
}

game_engine::State game_engine::setX(word_type r_x)
{
    if (r_x == -1)
        return SKIP_Y;

    m_pos[CUR].first = r_x;
    return AWAITING_Y;
}

game_engine::State game_engine::setY(word_type r_y)
{
    m_pos[CUR].second = r_y;
    return AWAITING_TILE;
}

game_engine::State game_engine::skipY(word_type dummy)
{
    return AWAITING_SCORE;
}

game_engine::State game_engine::setTile(word_type tile)
{
    switch(tile)
    {
    case T_SPACE:
        {
            block_const_iterator bl_cit = m_blocks.find(m_pos[CUR]);
            if (bl_cit != m_blocks.end())
                m_blocks.erase(bl_cit);
            break;
        }
    case T_WALL:
        m_pos[BBLR].first = std::max(m_pos[BBLR].first, m_pos[CUR].first);
        m_pos[BBLR].second = std::max(m_pos[BBLR].second, m_pos[CUR].second);
        m_pos[MAX] = m_pos[BBLR];
        --m_pos[MAX].first;
        --m_pos[MAX].second;
        break;
    case T_BLOCK:
        ++m_nblocks;
        m_pos[LOWB].second = std::max(m_pos[LOWB].second, m_pos[CUR].second);
        m_blocks.insert(m_pos[CUR]);
        break;
    case T_HPADDLE:
        m_pos[HPAD] = m_pos[CUR];
        break;
    case T_BALL:
        if (m_pos[BALL].second > 0)
            m_pos[PREV] = m_pos[BALL];
        else
        {
            m_pos[PREV] = m_pos[CUR];
            --m_pos[PREV].first;
            --m_pos[PREV].second;
        }
        m_pos[BALL] = m_pos[CUR];
        break;
    }
    mvaddch(m_pos[CUR].second, m_pos[CUR].first, M_TILE_GLYPHS[tile]);
    refresh();
    return AWAITING_X;
}

game_engine::State game_engine::setScore(word_type score)
{
    int x = m_pos[BBLR].first + 2;
    for (char ch : m_score) mvaddch(0, x++, ' ');

    m_score = std::to_string(score);
    x = m_pos[BBLR].first + 2;
    for (char ch : m_score)
        if (ch != '\n')
            mvaddch(0, x++, ch);

    refresh();
    return AWAITING_X;
}

vertex_type game_engine::velocity()
{
    return std::make_pair(
        m_pos[BALL].first  - m_pos[PREV].first,
        m_pos[BALL].second - m_pos[PREV].second
    );
}

word_type game_engine::baselineIntercept(const vertex_type& v)
{
    switch(v.first)
    {
    case -1:
        {
            word_type k_out = m_pos[BALL].first + m_pos[BALL].second - m_pos[MAX].second;
            return k_out < 1 ? 2 - k_out : k_out;
        }
    case 1:
        {
            word_type k_out = m_pos[BALL].first- m_pos[BALL].second + m_pos[MAX].second;
            return k_out > m_pos[MAX].first ? 2 * m_pos[MAX].first - k_out : k_out;
        }
    default:
        {
            std::ostringstream oss;
            oss << "Unforseen velocity: " << v.first;
            throw std::runtime_error(oss.str());
        }
    }
}
