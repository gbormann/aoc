
#if !defined(_LATCH_H)
#define  _LATCH_H

#include <chrono>
//#include <iostream>
#include <thread>

using namespace std::chrono;

#include "atomic_mpmc_queue.h"

template<class word_type>
class latch
{
public:
    /** The queue used is non-blocking so read() is hiding the spinning. */
    word_type read()
    {
        word_type val = 0;
        while (!m_queue.try_dequeue(val)) 
            std::this_thread::sleep_for(milliseconds{10}); // sleep or we'd burn a hole through the board

        return val;
    }

    bool try_read(std::size_t tries, word_type& result)
    {
        word_type val = 0;
        bool ok;
        for (ok = m_queue.try_dequeue(val); tries > 0 && !ok; --tries, ok = m_queue.try_dequeue(val))
            std::this_thread::sleep_for(milliseconds{10}); // sleep or we'd burn a hole through the board

        if (tries == 0 && !ok)
            return false;

        result = val;
        return true;
    }

    /** The queue used is non-blocking so write() is hiding the spinning. */
    void write(int val)
    {
        while (!m_queue.try_enqueue(val))
            std::this_thread::sleep_for(milliseconds{10}); // sleep or we'd burn a hole through the board
    }

    void reset()
    {
        m_queue.reset();
    }

private:
    atomic_mpmc_queue<int, 8> m_queue;
};

#endif
