
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
#include "../../latch.h"

intcode_vm::intcode_vm(const std::string& progFileName, latch<word_type>& in, latch<word_type>& out)
    : m_in(in), m_out(out), m_needs_init(true)
{
    read_programme(progFileName);
    m_prg = m_text;
}

std::vector<word_type> intcode_vm::get_addr_modes(word_type addr_modes, std::size_t nr_args)
{
    std::vector<word_type> modes(nr_args);

    for (std::size_t p_ndx = 0; p_ndx < nr_args; ++p_ndx, addr_modes /= 10)
        modes[p_ndx] = addr_modes % 10;

    return modes;
}

void intcode_vm::run()
{
    std::size_t ip_incr = 0;
    for (std::size_t ip = 0; ip < m_prg.size(); ip += ip_incr)
    {
        if (m_prg[ip] != 99 && m_prg.size() < ip + 1)
            std::runtime_error("INTCODE VM: TRUNCATED TEXT");

        const word_type opcode = m_prg[ip] % 100;
        switch (opcode)
        {
        case 1: // '+' (a1, a2: operands; a3: dst addr)
            {
                std::vector<word_type> addr_modes(get_addr_modes(m_prg[ip] / 100, 2));
                std::size_t fp = ip + 1;
                const word_type opnd1 = load(addr_modes[0] == 0 ? lea(fp) : fp);
                const word_type opnd2 = load(addr_modes[1] == 0 ? lea(fp + 1) : (fp + 1));
                store(lea(fp + 2), opnd1 + opnd2);
                ip_incr = 4;
            }
            break;
        case 2: // '*' (a1, a2: operands; a3: dst addr)
            {
                std::vector<word_type> addr_modes(get_addr_modes(m_prg[ip] / 100, 2));
                std::size_t fp = ip + 1;
                const word_type opnd1 = load(addr_modes[0] == 0 ? lea(fp) : fp);
                const word_type opnd2 = load(addr_modes[1] == 0 ? lea(fp + 1) : (fp + 1));
                store(lea(fp + 2), opnd1 * opnd2);
                ip_incr = 4;
            }
            break;
        case 3: // 'in' 1 word
            {
                std::size_t fp = ip + 1;
                word_type val = std::numeric_limits<word_type>::min();
                if (m_needs_init)
                {
                    val = m_phase;
                    m_needs_init = false;
                }
                else
                    val = m_in.read();

                store(lea(fp), val);
                ip_incr = 2;
            }
            break;
        case 4: // 'out' 1 word
            {
                std::vector<word_type> addr_modes(get_addr_modes(m_prg[ip] / 100, 1));
                std::size_t fp = ip + 1;
                const word_type opnd = load(addr_modes[0] == 0 ? lea(fp) : fp);
                m_out.write(opnd);
                ip_incr = 2;
            }
            break;
        case 5: // 'jeq' (a1: test val; a2: dst addr)
            {
                std::vector<word_type> addr_modes(get_addr_modes(m_prg[ip] / 100, 2));
                std::size_t fp = ip + 1;
                const word_type test_val = load(addr_modes[0] == 0 ? lea(fp) : fp);
                if (test_val == 0)
                {
                    ip_incr = 3;
                    break;
                }
                ip = load(addr_modes[1] == 0 ? lea(fp + 1) : (fp + 1));
                ip_incr = 0;
            }
            break;
        case 6: // 'jne' (a1: test val; a2: dst addr)
            {
                std::vector<word_type> addr_modes(get_addr_modes(m_prg[ip] / 100, 2));
                std::size_t fp = ip + 1;
                const word_type test_val = load(addr_modes[0] == 0 ? lea(fp) : fp);
                if (test_val != 0)
                {
                    ip_incr = 3;
                    break;
                }
                ip = load(addr_modes[1] == 0 ? lea(fp + 1) : (fp + 1));
                ip_incr = 0;
            }
            break;
        case 7: // 'lt'(a1, a2: operands; a3: dst addr)
            {
                std::vector<word_type> addr_modes(get_addr_modes(m_prg[ip] / 100, 2));
                std::size_t fp = ip + 1;
                const word_type opnd1 = load(addr_modes[0] == 0 ? lea(fp) : fp);
                const word_type opnd2 = load(addr_modes[1] == 0 ? lea(fp + 1) : (fp + 1));
                store(lea(fp + 2), (opnd1 < opnd2 ? 1 : 0));
                ip_incr = 4;
            }
            break;
        case 8: // 'eq' (a1, a2: operands; a3: dst addr)
            {
                std::vector<word_type> addr_modes(get_addr_modes(m_prg[ip] / 100, 2));
                std::size_t fp = ip + 1;
                const word_type opnd1 = load(addr_modes[0] == 0 ? lea(fp) : fp);
                const word_type opnd2 = load(addr_modes[1] == 0 ? lea(fp + 1) : (fp + 1));
                store(lea(fp + 2), (opnd1 == opnd2 ? 1 : 0));
                ip_incr = 4;
            }
            break;
        case 99:
#if defined _IC_VM_REPORT_HALT
            std::cerr << "INTCODE VM: HALT ENCOUNTERED @[" << ip << ']' << '\n';
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
    m_prg = m_text;
    m_needs_init = true;
}

void intcode_vm::dump()
{
    std::copy(m_prg.begin(), m_prg.end(), std::ostream_iterator<word_type>(std::cerr, "\n"));
}

word_type intcode_vm::readMem(std::size_t addr)
{
    return load(addr);
}

void intcode_vm::writeMem(std::size_t addr, word_type word)
{
    store(addr, word);
}

void intcode_vm::writeReg(word_type phaseCode)
{
    m_phase = phaseCode;
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

word_type intcode_vm::load(std::size_t addr)
{
    if (addr > m_prg.size() - 1)
    {
        std::ostringstream oss;
        oss << "INTCODE VM: ATTEMPTED LOAD FROM INVALID ADDRESS [" << addr << ']';
        throw std::runtime_error(oss.str());
    }

    return m_prg[addr];
}

void intcode_vm::store(std::size_t addr, word_type word)
{
    if (addr > m_prg.size() - 1)
    {
        std::ostringstream oss;
        oss << "INTCODE VM: ATTEMPTED STORE TO INVALID ADDRESS [" << addr << ']';
        throw std::runtime_error(oss.str());
    }

    m_prg[addr] = word;
}
