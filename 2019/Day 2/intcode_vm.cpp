
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "intcode_vm.h"

intcode_vm::intcode_vm(const std::string& progFileName)
{
    read_programme(progFileName);
    m_prg = m_text;
}

void intcode_vm::run()
{
    for (std::size_t ip = 0; ip < m_prg.size(); ip += 4)
    {
        if (m_prg[ip] != 99 && m_prg.size() < ip + 4)
            std::runtime_error("INTCODE VM: TRUNCATED TEXT");

        switch(m_prg[ip])
        {
        case 1: // '+' 
            {
                word_type opnd1 = load(lea(ip + 1));
                word_type opnd2 = load(lea(ip + 2));
                store(lea(ip + 3), opnd1 + opnd2);
            }
            break;
        case 2: // '*'
            {
                word_type opnd1 = load(lea(ip + 1));
                word_type opnd2 = load(lea(ip + 2));
                store(lea(ip + 3), opnd1 * opnd2);
            }
            break;
        case 99:
            std::cout << "INTCODE VM: HALT ENCOUNTERED @[" << ip << ']' << std::endl;
            return;
        default:
            std::ostringstream oss;
            oss << "INTCODE VM: ILLEGAL OPCODE '" << m_prg[ip] << "' @[" << ip << ']';
            throw std::range_error(oss.str());
        }
    }
    throw std::runtime_error("INTCODE VM: IP ACCESS VIOLATION");
}

void intcode_vm::reset()
{
    m_prg = m_text;
}

void intcode_vm::dump()
{
    std::copy(m_prg.begin(), m_prg.end(), std::ostream_iterator<word_type>(std::cout, "\n"));
}

intcode_vm::word_type intcode_vm::readMem(std::size_t addr)
{
    return load(addr);
}

void intcode_vm::writeMem(std::size_t addr, intcode_vm::word_type word)
{
    store(addr, word);
}

void intcode_vm::read_programme(const std::string& progFileName)
{
    std::ifstream ifs(progFileName);
    std::copy(std::istream_iterator<word_type>(ifs),
              std::istream_iterator<word_type>(),
              std::back_inserter(m_text));
}

std::size_t intcode_vm::lea(std::size_t addr)
{
    return static_cast<std::size_t>(m_prg[addr]);
}

intcode_vm::word_type intcode_vm::load(std::size_t addr)
{
    if (addr > m_prg.size() - 1)
    {
        std::ostringstream oss;
        oss << "INTCODE VM: ATTEMPTED LOAD FROM INVALID ADDRESS [" << addr << ']';
        throw std::runtime_error(oss.str());
    }

    return m_prg[addr];
}

void intcode_vm::store(std::size_t addr, intcode_vm::word_type word)
{
    if (addr > m_prg.size() - 1)
    {
        std::ostringstream oss;
        oss << "INTCODE VM: ATTEMPTED STORE TO INVALID ADDRESS [" << addr << ']';
        throw std::runtime_error(oss.str());
    }

    m_prg[addr] = word;
}
