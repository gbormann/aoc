
#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <curses.h>

#include "datatypes.h"
#include "utils.h"
#include "intcode_vm.h"

#include "repair_droid.h"

using namespace std::chrono;

const std::vector<vertex_type> repair_droid::M_STEPS = { {0, 0}, {0, -1}, {0, 1}, {-1, 0}, {1, 0}, {0, 0}};
const repair_droid::Dir repair_droid::M_ROT_R[6] = { NPOLE, EAST, WEST, NORTH, SOUTH, SPOLE };
const repair_droid::Dir repair_droid::M_ROT_L[6] = { NPOLE, WEST, EAST, SOUTH, NORTH, SPOLE };
const repair_droid::Dir repair_droid::M_REVERSE[6] = { NPOLE, SOUTH, NORTH, EAST, WEST, SPOLE };
const repair_droid::Path repair_droid::M_STATE_PATH[6] = { P_NONE, P_OXY, P_NONE, P_RWIND, P_LWIND, P_NONE };
const char repair_droid::M_TILE_GLYPHS[5] = { '#', '.', 'O', 'D', 'S' };
const char repair_droid::M_DIR_SYMS[6] = { 'S', 'n', 's', 'w', 'e', 'O' };

repair_droid::repair_droid(const std::string& progFileName)
    : m_vm(progFileName)
    , m_state{ S_INTERACTIVE }
    , m_is_rp_done{ false }
    , m_dir{ NORTH }
    , m_paths{ 3 }
    , m_pos{ NR_POS, vertex_type{ 0, 0 } }
    , m_max_psize{ 3, 0 }
{
    m_pos[START] = std::make_pair(66, 25);
    m_is_oxy_reached = false;
}

void repair_droid::initConsole()
{
    initscr();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    clear();
}

void repair_droid::endConsole()
{
    nl();
    echo();
    nocbreak();
    endwin();
}

void repair_droid::run()
{
    initConsole();
    mvaddch(m_pos[START].second, m_pos[START].first, toGlyph(T_DROID));
    refresh();
    m_vm.run(*this);
    readWord();
    endConsole();
}

void repair_droid::reset()
{
    m_vm.reset();
    resetConsole();
    m_state = S_INTERACTIVE;
}
 
void repair_droid::printReport()
{
    std::cout << "d(S,O)=" << ((m_oxy_cit - m_paths[P_OXY].begin())) << '\n';
    std::cout << "d(O,S)=" << (m_paths[P_OXY].size() - (m_oxy_cit - m_paths[P_OXY].begin())) << '\n';
    std::cout << m_msgss.str();
    std::cout << "Max path sizes:\n";
    std::cout << "\tPainting: " << m_max_psize[P_OXY]
              << "\n\tR-wind: " << m_max_psize[P_RWIND]
              << "\n\tL-wind: " << m_max_psize[P_LWIND] << '\n';
    std::size_t rmax = m_max_psize[P_RWIND];
    std::size_t lmax = m_max_psize[P_LWIND];
    std::cout << "O2 fill time: " << std::max(lmax, rmax) << '\n';
}

word_type repair_droid::readWord()
{
    switch(m_state)
    {
    case S_INTERACTIVE:
        switch(getch())
        {
        case int('e'):
        case KEY_RIGHT:
            m_dir = EAST;
            break;
        case int('n'):
        case KEY_UP:
            m_dir = NORTH;
            break;
        case int('p'):
            resetConsole();
            m_state = S_PAINT_PATHS;
        case int('s'):
        case KEY_DOWN:
            m_dir = SOUTH;
            break;
        case int('w'):
        case KEY_LEFT:
            m_dir = WEST;
            break;
        case ERR:
        case int('q'):
        default:
            endConsole();
            printReport();
            exit(0);
        }
        break;
    case S_PAINT_PATHS:
        break;
    case S_GOTO_OXY:
        if (m_dir_cit == m_oxy_cit)
        {
            m_pacing_delay = 1;//25;
            m_state = m_is_rp_done ? S_FIND_LONGEST_LPATH : S_FIND_LONGEST_RPATH;
        }
        else
        {
            m_dir = *m_dir_cit++;
            if (m_dir == NPOLE)
                m_dir = *m_dir_cit++;
        }
        break;
    case S_FIND_LONGEST_RPATH:
        /* calculate right-winding longest path (which gives re-oxygenation time) */
        break;
    case S_FIND_LONGEST_LPATH:
        /* calculate left-winding longest path (which gives re-oxygenation time) */
        break;
    }
    return m_dir;
}

void repair_droid::writeWord(const word_type& val)
{
    m_state = setResponse(val);

    std::this_thread::sleep_for(milliseconds{ m_pacing_delay });
}

repair_droid::Dir repair_droid::turnLeft(repair_droid::Dir dir)
{
    return M_ROT_L[dir];
}

repair_droid::Dir repair_droid::turnRight(repair_droid::Dir dir)
{
    return M_ROT_R[dir];
}

repair_droid::Dir repair_droid::revert(repair_droid::Dir dir)
{
    return M_REVERSE[dir];
}

repair_droid::Path repair_droid::toPath(repair_droid::State state)
{
    return M_STATE_PATH[state];
}

vertex_type repair_droid::toStep(repair_droid::Dir dir)
{
    return M_STEPS[dir];
}

char repair_droid::toDirSym(Dir dir)
{
    return M_DIR_SYMS[dir];
}

void repair_droid::resetConsole()
{
    clear();
    m_is_rp_done = false;
    m_pacing_delay = 1; // in ms
    std::for_each(m_paths.begin(), m_paths.end(), [](dir_stack_type& path){ path.clear(); });
    m_dir = NORTH;
    std::fill_n(m_pos.begin(), m_pos.size(), vertex_type(0, 0));
    m_pos[START] = std::make_pair(66, 25);
    m_is_oxy_reached = false;
    std::fill_n(m_max_psize.begin(), m_max_psize.size(), 0);
}

char repair_droid::toGlyph(repair_droid::Tile tile)
{
    const char glyph{ M_TILE_GLYPHS[tile] };
    return (m_state == S_FIND_LONGEST_RPATH || m_state == S_FIND_LONGEST_LPATH) && glyph == '.' ? '0' : glyph;
}

void repair_droid::paintTile(const vertex_type& pos, repair_droid::Tile tile)
{
    mvaddch(m_pos[START].second + pos.second, m_pos[START].first + pos.first, toGlyph(tile));
}

repair_droid::Dir repair_droid::turnToExplore(repair_droid::Dir dir)
{
    switch(m_state)
    {
    case S_PAINT_PATHS:
    case S_FIND_LONGEST_RPATH:
        return turnRight(dir);
    case S_FIND_LONGEST_LPATH:
        return turnLeft(dir);
    default:
        m_msgss << "state@error: " << m_state << '\n';
        throw std::runtime_error("Illegal state for exploration!");
    }
}

repair_droid::Dir repair_droid::turnToRecover(repair_droid::Dir dir)
{
    switch(m_state)
    {
    case S_PAINT_PATHS:
    case S_FIND_LONGEST_RPATH:
        return turnLeft(dir);
    case S_FIND_LONGEST_LPATH:
        return turnRight(dir);
    default:
        m_msgss << "state@error: " << m_state << '\n';
        throw std::runtime_error("State should not require recovery turn!");
    }
}

void repair_droid::updateTracing(repair_droid::Dir dir)
{
    Path path{ toPath(m_state) };
    if (path == P_NONE) return;

    switch(dir)
    {
    case NPOLE:
    case SPOLE:
        if (m_state == S_PAINT_PATHS)
        {
            m_msgss << "#steps=" << m_paths[path].size() << '\n';
            m_paths[path].push_back(dir);
            break;
        }
    default:
        if (!m_paths[path].empty() && m_paths[path].back() == revert(dir))
            m_paths[path].pop_back();
        else
            m_paths[path].push_back(dir);
    }
    m_max_psize[path] = std::max(m_max_psize[path], m_paths[path].size());
}

repair_droid::State repair_droid::setResponse(word_type response)
{
    m_pos[STEP] = toStep(m_dir);
    const vertex_type new_pos{ m_pos[CUR] + m_pos[STEP] };

    bool has_state_changed = false;
    State state = m_state;
    Tile old_tile{ T_ROOM };
    Tile new_tile{ T_WALL };
    switch(response)
    {
    case T_WALL:
        if (state != S_INTERACTIVE)
            m_dir = turnToRecover(m_dir);
        break;
    case T_ROOM:
        if (m_pos[CUR].first == 0 && m_pos[CUR].second == 0)
            old_tile = T_START;
        else if (new_pos.first == 0 && new_pos.second == 0)
        {
            if (m_pos[OXY].first == 0 && m_pos[OXY].second == 0)
            {
                if (state == S_INTERACTIVE || state == S_PAINT_PATHS)
                    updateTracing(NPOLE);
            }
            else
            {
                switch (state)
                {
                case S_PAINT_PATHS:
//                    m_pacing_delay = 100;
                    m_dir_cit = m_paths[P_OXY].begin();
                    m_oxy_cit = std::find(m_paths[P_OXY].begin(), m_paths[P_OXY].end(), SPOLE);
                    state = S_GOTO_OXY;
                    has_state_changed = true;
                    break;
                case S_FIND_LONGEST_RPATH:
                    m_pacing_delay = 25;
                    m_dir_cit = m_paths[P_OXY].begin();
                    m_is_rp_done = true;
                    state = S_GOTO_OXY;
                    has_state_changed = true;
                    break;
                case S_FIND_LONGEST_LPATH:
                    m_pacing_delay = 25;
                    m_is_rp_done = false;
                    state = S_INTERACTIVE;
                    has_state_changed = true;
                    break;
                }
            }
        }
        else if (m_pos[CUR] == m_pos[OXY])
        {
            m_pacing_delay = 1;//25;
            old_tile = T_OXY;
        }

        new_tile = T_DROID;
        if (!has_state_changed)
            updateTracing(m_dir);
        break;
    case T_OXY:
        m_pos[OXY] = new_pos;
        m_is_oxy_reached = true;
        new_tile = T_DROID;
        updateTracing(m_dir);
        if (state == S_PAINT_PATHS)
            updateTracing(SPOLE);
        break;
    default:
        break;
    }
    if (new_tile != T_WALL)
    {
        paintTile(m_pos[CUR], old_tile);
        m_pos[CUR] = new_pos;
        if (!(state == S_INTERACTIVE || state == S_GOTO_OXY))
            m_dir = turnToExplore(m_dir);
    }
    paintTile(new_pos, new_tile);
    refresh();

    return state;
}
