
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <thread>
#include <vector>

#include "datatypes.h"
#include "intcode_vm.h"
#include "../../latch.h"
//#include "thrust_amp.h"

class thrust_amp
{
public:
    thrust_amp(latch<word_type>& in, latch<word_type>& out) : vm{"../intcode_prog.txt", in, out} {}

    void start() { vm.run(); }
    void set_phase(word_type phase_code) { vm.writeReg(phase_code);  }
    void reset() { vm.reset(); }

private:
    intcode_vm vm;
};

enum Port { IN = 0, XCT1, XCT2, XCT3, XCT4, OUT, NR_PORTS };

/**
 *   in | A1 | xct1 | A2 | xct2 | A3 | xct3 | A4 | xct4 | A5 | out | main | in, <reporting>
 *   threads: 5
 *   interconnects: 6
 */
int main()
{
    std::vector<word_type> phase_config = { 5, 6, 7, 8, 9 };
    std::vector<latch<word_type>> interconnects(NR_PORTS);
    std::vector<thrust_amp> amps;
    std::vector<std::thread> modules;

    for (std::size_t ndx = 0; ndx < 5; ++ndx)
    {
        thrust_amp ta(interconnects[ndx], interconnects[ndx + 1]);
        amps.push_back(ta);
    }

    word_type max_power = 0;
    std::vector<word_type> phase_config_max(phase_config);
    do
    {
        std::size_t ndx = 0;
        for (auto& amp : amps)
            amp.set_phase(phase_config[ndx++]);

        for (auto& amp : amps)
        {
            std::thread amp_thr(std::bind(&thrust_amp::start, amp));
            modules.push_back(std::move(amp_thr));
        }

        word_type power = 0;
        interconnects[IN].write(power);
        while (interconnects[OUT].try_read(50, power)) // about 10s max waiting time at a 10ms periodicity
            interconnects[IN].write(power);

        if (power > max_power)
        {
            max_power = power;
            phase_config_max = phase_config;
        }

        std::cerr << '.';

        for (auto& thr : modules)
            thr.join();

        modules.clear();

        for (auto& latch : interconnects)
            latch.reset();

        for (auto& amp : amps)
            amp.reset();

    } while(std::next_permutation(phase_config.begin(), phase_config.end()));

    std::cout << "\nMax power of " << max_power << " reached for phase config\n";
    std::cout << "\tPhi=";
    std::copy(phase_config_max.begin(), phase_config_max.end(), std::ostream_iterator<word_type>(std::cout, ", "));
    std::cout << '\n';

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

    vm.dump();
*/

    return 0;
}
