
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "datatypes.h"
#include "intcode_vm.h"

intcode_vm::intcode_vm(const std::string& progFileName)
    : m_text(), m_ram(readProgramme(progFileName), 0)
{
    loadProgramme();
}

void intcode_vm::run()
{
    std::size_t ip_incr = 0;
    for (std::size_t ip = 0; ip < m_ram.size(); ip += ip_incr)
    {
        const word_type ins = m_ram[ip];
        if (ins != 99 && m_ram.size() < ip + 1)
            throw std::runtime_error("INTCODE VM: TRUNCATED TEXT");

        const word_type opcode = ins % 100;
        switch (opcode)
        {
        case 1: // '+' (a1, a2: operands; a3: dst addr)
            {
                std::vector<word_type> addr_modes(get_addr_modes(ins / 100, 3));
                std::size_t fp = ip + 1;
                const word_type opnd1 = load(fp, addr_modes[0]);
                const word_type opnd2 = load(fp + 1, addr_modes[1]);
                store(fp + 2, addr_modes[2], opnd1 + opnd2);
                ip_incr = 4;
            }
            break;
        case 2: // '*' (a1, a2: operands; a3: dst addr)
            {
                std::vector<word_type> addr_modes(get_addr_modes(ins / 100, 3));
                std::size_t fp = ip + 1;
                const word_type opnd1 = load(fp, addr_modes[0]);
                const word_type opnd2 = load(fp + 1, addr_modes[1]);
                store(fp + 2, addr_modes[2], opnd1 * opnd2);
                ip_incr = 4;
            }
            break;
        case 3: // 'in' 1 word
            {
                std::vector<word_type> addr_modes(get_addr_modes(ins / 100, 1));
                std::size_t fp = ip + 1;
                word_type val = std::numeric_limits<word_type>::min();
                std::cout << "$ ";
                std::cin >> val;
                store(fp, addr_modes[0], val);
                ip_incr = 2;
            }
            break;
        case 4: // 'out' 1 word
            {
                std::vector<word_type> addr_modes(get_addr_modes(ins / 100, 1));
                std::size_t fp = ip + 1;
                const word_type opnd = load(fp, addr_modes[0]);
                std::cout << opnd << '\n';
                ip_incr = 2;
            }
            break;
        case 5: // 'jnz' (a1: test val; a2: dst addr)
            {
                std::vector<word_type> addr_modes(get_addr_modes(ins / 100, 2));
                std::size_t fp = ip + 1;
                const word_type test_val = load(fp, addr_modes[0]);
                if (test_val != 0)
                {
                    ip = load(fp + 1, addr_modes[1]);
                    ip_incr = 0;
                    break;
                }
                ip_incr = 3;
            }
            break;
        case 6: // 'jz' (a1: test val; a2: dst addr)
            {
                std::vector<word_type> addr_modes(get_addr_modes(ins / 100, 2));
                std::size_t fp = ip + 1;
                const word_type test_val = load(fp, addr_modes[0]);
                if (test_val == 0)
                {
                    ip = load(fp + 1, addr_modes[1]);
                    ip_incr = 0;
                    break;
                }
                ip_incr = 3;
            }
            break;
        case 7: // 'lt'(a1, a2: operands; a3: dst addr)
            {
                std::vector<word_type> addr_modes(get_addr_modes(ins / 100, 3));
                std::size_t fp = ip + 1;
                const word_type opnd1 = load(fp, addr_modes[0]);
                const word_type opnd2 = load(fp + 1, addr_modes[1]);
                store(fp + 2, addr_modes[2], (opnd1 < opnd2 ? 1 : 0));
                ip_incr = 4;
            }
            break;
        case 8: // 'eq' (a1, a2: operands; a3: dst addr)
            {
                std::vector<word_type> addr_modes(get_addr_modes(ins / 100, 3));
                std::size_t fp = ip + 1;
                const word_type opnd1 = load(fp, addr_modes[0]);
                const word_type opnd2 = load(fp + 1, addr_modes[1]);
                store(fp + 2, addr_modes[2], (opnd1 == opnd2 ? 1 : 0));
                ip_incr = 4;
            }
            break;
        case 9: // 'setba' (a: new base addr)
            {
                std::vector<word_type> addr_modes(get_addr_modes(ins / 100, 1));
                std::size_t fp = ip + 1;
                m_base_addr = lba(fp, addr_modes[0]);
                ip_incr = 2;
            }
            break;
        case 99:
#if defined(_IC_VM_REPORT_HALT)
            std::cout << "INTCODE VM: HALT ENCOUNTERED @[" << ip << ']' << std::endl;
#endif
            return;
        default:
            std::ostringstream oss;
            oss << "INTCODE VM: ILLEGAL OPCODE '" << opcode << "' @[" << ip << ']';
            throw std::range_error(oss.str());
        }
    }
    throw std::runtime_error("INTCODE VM: IP ACCESS VIOLATION");
}

void intcode_vm::reset()
{
    loadProgramme(); // Keep the memory size as at the end of the previous execution
}

void intcode_vm::dump()
{
    std::copy(m_ram.begin(), m_ram.end(), std::ostream_iterator<word_type>(std::cout, "\n"));
}

word_type intcode_vm::readMem(std::size_t addr, bool requires_resize)
{
    if (requires_resize)
        resizeMem(addr);

    return m_ram[addr];
}

void intcode_vm::writeMem(std::size_t addr, word_type word, bool requires_resize)
{
    if (requires_resize)
        resizeMem(addr);

    m_ram[addr] = word;
}

// Private members:
// ================
std::size_t intcode_vm::next_pow_2(std::size_t size_to_fit)
{
    std::size_t hi_bit = 1;
    for (std::size_t sz = size_to_fit >> 1; sz > 0; sz >>= 1, ++hi_bit);
    return 1 << hi_bit;
}

std::vector<word_type> intcode_vm::get_addr_modes(word_type addr_modes, std::size_t nr_args)
{
    std::vector<word_type> modes(nr_args);

    for (std::size_t p_ndx = 0; p_ndx < nr_args; ++p_ndx, addr_modes /= 10)
        modes[p_ndx] = addr_modes % 10;

    return modes;
}

std::size_t intcode_vm::readProgramme(const std::string& progFileName)
{
    std::ifstream ifs(progFileName);
    std::copy(std::istream_iterator<word_type>(ifs),
              std::istream_iterator<word_type>(),
              std::back_inserter(m_text));

    return next_pow_2(m_text.size());
}

void intcode_vm::loadProgramme()
{
    std::copy(m_text.begin(), m_text.end(), m_ram.begin());
    m_base_addr = 0;
}

std::size_t intcode_vm::relAddr(word_type offset)
{
    const bool is_negative = offset < 0; 

    if (is_negative && static_cast<std::size_t>(-offset) > m_base_addr)
    {
        std::ostringstream oss;
        oss << "INTCODE VM: ADDRESS UNDERFLOW FOR CURRENT BASE_ADDR=" << m_base_addr << " AND OFFSET=" << offset;
        throw std::range_error(oss.str());
    }

    const std::size_t abs_offset = static_cast<std::size_t>(std::abs(offset));
    return is_negative ? m_base_addr - abs_offset : m_base_addr + abs_offset;
}

std::size_t intcode_vm::lea(std::size_t addr, bool is_rel)
{
    if (is_rel)
        return relAddr(m_ram[addr]);

    const word_type eff_addr = m_ram[addr];
    if (eff_addr < 0)
    {
        std::ostringstream oss;
        oss << "INTCODE VM: ILLEGAL ADDRESS: [" << eff_addr << ']';
        throw std::domain_error(oss.str());
    }

    return static_cast<std::size_t>(eff_addr);
}

std::size_t intcode_vm::lba(std::size_t addr, word_type mode)
{
    return relAddr(load(addr, mode));
}

word_type intcode_vm::load(std::size_t addr, word_type mode)
{
    switch(mode)
    {
    case 0: return readMem(lea(addr), true);
    case 1: return readMem(addr, true);
    case 2: return readMem(lea(addr, true), true);
    default:
        {
            std::ostringstream oss;
            oss << "INTCODE VM: ILLEGAL OPERAND MODE'" << mode << '\'';
            throw std::range_error(oss.str());
        }
    }
}

void intcode_vm::store(std::size_t addr, word_type mode, word_type word)
{
    switch(mode)
    {
    case 0:
        writeMem(lea(addr), word, true);
        break;
    case 1:
        writeMem(addr, word, true);
        break;
    case 2:
        writeMem(lea(addr, true), word, true);
        break;
    default:
        {
            std::ostringstream oss;
            oss << "INTCODE VM: ILLEGAL OPERAND MODE'" << mode << '\'';
            throw std::range_error(oss.str());
        }
    }
}

void intcode_vm::resizeMem(std::size_t addr_to_fit)
{
    if (addr_to_fit > ((1l << 29) - 1))
    {
        std::ostringstream oss;
        oss << "INTCODE VM: ATTEMPT TO GROW MEMORY BEYOND 4GB";
        throw std::runtime_error(oss.str());
    }

    const std::size_t prev_size = m_ram.size();
    while (!(addr_to_fit < m_ram.size()))
        m_ram.resize(m_ram.size() << 1);

    std::fill(m_ram.begin() + prev_size, m_ram.end(), 0);
}
