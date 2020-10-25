
#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

#include "../../latch.h"
#include "datatypes.h"

using namespace std::chrono;

class control_unit
{
public:
    control_unit(latch<word_type>& in, latch<word_type>& out)
        : m_in(in), m_out(out)
    { }

    void sensor_loop()
    {
        while(m_in.read() != 100);
        for (int i = -5; i < 6; ++i)
        {
            m_out.write(i);
            std::this_thread::sleep_for(seconds{6 + i / 2});
        }
    }

    void actuator_loop()
    {
        int s = -100;
        do
        {
            s = m_in.read();
            std::cout << "A " << s << std::endl;
            m_out.write(2 * (s - 10));
            std::this_thread::sleep_for(seconds{2});
        } while(s < 5);
        m_out.write(100);
    }

private:
    latch<word_type>& m_in;
    latch<word_type>& m_out;
};

int main()
{
    latch<word_type> in, xct, out;

    control_unit sensor_cntrl(in, xct);    
    control_unit actuator_cntrl(xct, out);

    std::thread thr_s(std::bind(&control_unit::sensor_loop, sensor_cntrl));   
    std::thread thr_a(std::bind(&control_unit::actuator_loop, actuator_cntrl));   

    std::this_thread::sleep_for(seconds{5});

    in.write(100);

    int a = -100;
    do
    {
        a = out.read();
        std::cout << "M " << a << std::endl;
    } while(a < 10);

    thr_s.join();
    thr_a.join();

    return 0;
}
