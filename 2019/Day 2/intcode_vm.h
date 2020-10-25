
#include <string>
#include <vector>

class intcode_vm
{
    typedef std::vector<unsigned int> programme_type;

public:
    typedef programme_type::value_type word_type;

    intcode_vm(const std::string& progFileName);

    void run();
    void reset();
    void dump();
    word_type readMem(std::size_t addr);
    void writeMem(std::size_t addr, word_type word);

private:
    void read_programme(const std::string& progFileName);

    std::size_t lea(std::size_t addr);
    word_type load(std::size_t addr);
    void store(std::size_t addr, word_type word);

    programme_type m_prg;
    programme_type m_text;
};
