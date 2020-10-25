#if !defined(_INTCODE_VM_H)
#define  _INTCODE_VM_H

#include <string>
#include <vector>

#include "datatypes.h"
#include "../../latch.h"

class intcode_vm
{
    typedef std::vector<word_type> programme_type;

public:
    static std::vector<word_type> get_addr_modes(word_type addr_modes, std::size_t nr_args);

    intcode_vm(const std::string& progFileName, latch<word_type>& in, latch<word_type>& out);

    void run();
    void reset();
    void dump();
    word_type readMem(std::size_t addr);
    void writeMem(std::size_t addr, word_type word);
    void writeReg(word_type phaseCode);

private:
    void read_programme(const std::string& progFileName);

    std::size_t lea(std::size_t addr);
    word_type load(std::size_t addr);
    void store(std::size_t addr, word_type word);

    programme_type m_prg;
    programme_type m_text;
    latch<word_type>& m_in;
    latch<word_type>& m_out;
    word_type m_phase;
    bool m_needs_init;
};

#endif
