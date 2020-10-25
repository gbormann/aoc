
#include <iostream>

#include "datatypes.h"
#include "intcode_vm.h"

int main()
{
    intcode_vm vm("intcode_prog.txt");
    vm.run();
/*
    const std::size_t RESULT_ADDR = 0;
    const std::size_t NOUN_ADDR = 1;
    const std::size_t VERB_ADDR = 2;
    for (word_type noun = 0; noun < 100; ++noun)
        for (word_type verb = 0; verb < 100; ++verb)
        {
            vm.reset();
            vm.writeMem(NOUN_ADDR, noun);
            vm.writeMem(VERB_ADDR, verb);
            vm.run();
            if (vm.readMem(RESULT_ADDR) == 19690720)
            {
                std::cout << "And the winner is?!...(" << noun << ", " << verb << ')' << std::endl;
                return 0;
            }
        }
*/
//    vm.dump();

    return 0;
}
