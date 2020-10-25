
#if !defined(_INTCODE_VM_H)
#define _INTCODE_VM_H

#include <sstream>
#include <string>
#include <vector>

#include "datatypes.h"

// Default IOModule for console I/O
struct consoleModule
{
    word_type readWord();
    void writeWord(const word_type val);
};

class intcode_vm
{
    typedef std::vector<word_type> memory_type;

public:
    intcode_vm(const std::string& progFileName);

    template<class IOModule = consoleModule>
    void run(IOModule& ioModule);
    void reset();
    void dump();
    word_type readMem(std::size_t addr, bool requires_resize = false);
    void writeMem(std::size_t addr, word_type word, bool requires_resize = false);

private:
    static std::size_t next_pow_2(std::size_t size_to_fit);
    static std::vector<word_type> get_addr_modes(word_type addr_modes, std::size_t nr_args);

    std::size_t readProgramme(const std::string& progFileName);
    void loadProgramme();

    std::size_t relAddr(word_type offset);
    std::size_t lea(std::size_t addr, bool is_rel = false);
    std::size_t lba(std::size_t addr, word_type mode);
    word_type load(std::size_t addr, word_type mode);
    void store(std::size_t addr, word_type mode, word_type word);
    void resizeMem(std::size_t addr_to_fit);

    memory_type m_text;
    memory_type m_ram;
    std::size_t m_base_addr;
};

template<class IOModule>
void intcode_vm::run(IOModule& ioModule)
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
                store(fp, addr_modes[0], ioModule.readWord());
                ip_incr = 2;
            }
            break;
        case 4: // 'out' 1 word
            {
                std::vector<word_type> addr_modes(get_addr_modes(ins / 100, 1));
                std::size_t fp = ip + 1;
                ioModule.writeWord(load(fp, addr_modes[0]));
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

#endif
