
#include <string>
#include <vector>

#include "datatypes.h"

class intcode_vm
{
    typedef std::vector<word_type> memory_type;

public:
    intcode_vm(const std::string& progFileName);

    void run();
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
