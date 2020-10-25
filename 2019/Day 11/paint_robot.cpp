
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "datatypes.h"
#include "intcode_vm.h"

#include "paint_robot.h"

const paint_robot::rot_map_type paint_robot::M_ROT[2] =
    {
        {{{0, 1}, {-1,0}}, {{-1, 0}, {0, -1}}, {{0, -1}, {1, 0}}, {{1, 0}, {0, 1}}},
        {{{0, 1}, {1,0}}, {{1, 0}, {0, -1}}, {{0, -1}, {-1, 0}}, {{-1, 0}, {0, 1}}}
    };

paint_robot::paint_robot(const std::string& progFileName)
    : m_vm(progFileName)
    , m_pos{0, 0}
    , m_ornt{0, 1}
    , m_state{PAINTING}
{
    m_hull_livery.insert(std::make_pair(0,0));
}

void paint_robot::reset()
{
    m_vm.reset();
    m_hull_livery.clear();
    m_paint_log.clear();
    m_pos = std::make_pair(0, 0);
    m_ornt = std::make_pair(0, 1);
    m_state = PAINTING;
}

std::size_t paint_robot::paintedPanelCount()
{
    return m_paint_log.size();
}

void paint_robot::printLivery()
{
    vertex_type bb_ll{0, 0};
    vertex_type bb_ur{0, 0};
    for (const vertex_type& pxl : m_hull_livery)
    {
        bb_ll.first = std::min(bb_ll.first, pxl.first);
        bb_ur.first = std::max(bb_ur.first, pxl.first);
        bb_ll.second = std::min(bb_ll.second, pxl.second);
        bb_ur.second = std::max(bb_ur.second, pxl.second);
    }
    const vertex_type bb{bb_ur.first - bb_ll.first + 1, bb_ur.second - bb_ll.second + 1};
    std::cout << "bb=((0, 0), (" << bb.first << ", " << bb.second << "))\n";

    std::vector<std::string> livery_bmp(bb.second, std::string(bb.first, ' '));
    for (const vertex_type& pxl : m_hull_livery)
        livery_bmp[pxl.second - bb_ll.second][pxl.first - bb_ll.first] = '#';

    std::copy(livery_bmp.rbegin(), livery_bmp.rend(), std::ostream_iterator<std::string>(std::cout, "\n"));
}

word_type paint_robot::readWord()
{
    return m_hull_livery.find(m_pos) == m_hull_livery.end() ? 0 : 1;
}

void paint_robot::writeWord(const word_type& val)
{
    switch(m_state)
    {
    case PAINTING:
        m_state = paint(val);
        break;
    case DRIVING:
        m_state = turnAndStep(val);
        break;        
    }
}

paint_robot::State paint_robot::paint(word_type colour)
{
    switch(colour)
    {
    case 0:
        m_hull_livery.erase(m_pos);
        m_paint_log.insert(m_pos);
        break;
    case 1:
        m_hull_livery.insert(m_pos);
        m_paint_log.insert(m_pos);
        break;
    default:
        std::ostringstream oss;
        oss << "INVALID COLOUR CODE: " << colour;

        throw std::domain_error(oss.str());
    }
    return DRIVING;
}

paint_robot::State paint_robot::turnAndStep(word_type direction)
{
    if (direction < 0 || direction > 1)
    {
        std::ostringstream oss;
        oss << "INVALID DIRECTION CODE: " << direction;

        throw std::domain_error(oss.str());
    }

    m_ornt = M_ROT[direction].find(m_ornt)->second;
    m_pos.first += m_ornt.first;
    m_pos.second += m_ornt.second;

    return PAINTING;
}
