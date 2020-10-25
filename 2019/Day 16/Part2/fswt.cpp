
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

// Fast Square Wave Transform (aka Flawed Frequency Transform :-) )

std::string read_msg_txt(const std::string& msgFileName)
{
    std::vector<std::string> msg_container;
    std::ifstream ifs(msgFileName);
    std::copy(std::istream_iterator<std::string>(ifs),
              std::istream_iterator<std::string>(),
              std::back_inserter(msg_container));
    return msg_container[0];
}

void accumulate(const std::vector<int>& terms,
                std::vector<int>& accums,
                const std::size_t msg_offset,
                const std::size_t nt,
                std::size_t slice)
{
    const std::size_t msg_size = msg_offset + terms.size();

    std::vector<int> dstats(10, 0);
    for (std::size_t n = slice; n < msg_size; n += nt)
    {
        const std::size_t np1 = n + 1;
        const std::size_t neff = n - msg_offset;
        const std::size_t np1eff = neff + 1;

        std::fill_n(dstats.begin(), 10, 0);
        // c = 1 terms
        for (std::size_t k = n; k < msg_size; k += 4 * np1)
            for (std::size_t i = k - msg_offset; i < std::min(k + np1eff, terms.size()); ++i)
                ++dstats[terms[i]];

        // c = -1 terms
        for (std::size_t k = 3 * np1 - 1; k < msg_size; k += 4 * np1)
            for (std::size_t i = k - msg_offset; i < std::min(k + np1eff, terms.size()); ++i)
                --dstats[terms[i]];

        if (std::count(dstats.begin(), dstats.end(), 0) == 10)
            continue;

        for (std::size_t d = 1; d < 10; ++d)
            if (dstats[d] != 0)
                accums[neff] += static_cast<int>(d) * dstats[d];

        accums[neff] = std::abs(accums[neff]) % 10;
    }
}

int main()
{
    const std::string msg_txt{ read_msg_txt("../msgseq.txt") };
//    const std::string msg_txt("12345678");
    std::copy(msg_txt.begin(), msg_txt.end(), std::ostream_iterator<char>(std::cout, ""));
    std::cout << '\n';

    std::size_t msg_offset{ std::stoul(msg_txt.substr(0, 7)) };

    const std::size_t msg_size = 10000 * msg_txt.size();
    const std::size_t eff_msg_size = msg_size - msg_offset;

    std::cout << "Msg offset: " << msg_offset << '\n';
    std::cout << "Msg size: " << msg_size << '\n';
    std::cout << "Msg calc buffer size: " << eff_msg_size << '\n';

    // construct message
    std::vector<int> msg(msg_size, 0);

    std::transform(msg_txt.begin(), msg_txt.end(), msg.begin(), [](const char& c){ return c - '0'; });
    for (std::size_t nc = 1; nc < 10000; ++nc)
        std::copy(msg.begin(), msg.begin() + msg_txt.size(), msg.begin() + nc * msg_txt.size());

    // prepare effective msg buffers for calculations
    std::vector<int> bufs[2];
    bufs[0].resize(eff_msg_size);
    bufs[1].resize(eff_msg_size);

    std::copy(msg.begin() + msg_offset, msg.end(), bufs[0].begin());

    std::vector<std::thread> acc_thrs{ 8 };
    std::size_t cur = 0;
    std::size_t prev = 1;
    for (int ph = 0; ph < 100/*4*/; ++ph)
    {
        std::cout << ph << ": ";
        std::swap(prev, cur);
        std::fill_n(bufs[cur].begin(), eff_msg_size, 0);
        for (std::size_t ithr = 0; ithr < acc_thrs.size(); ++ithr)
            acc_thrs[ithr] = std::move(
                std::thread(
                    std::bind(
                        accumulate,
                        std::cref(bufs[prev]), std::ref(bufs[cur]),
                        msg_offset, acc_thrs.size(), msg_offset + ithr
                    )
                )
            );
        for (std::size_t ithr = 0; ithr < acc_thrs.size(); ++ithr)
            acc_thrs[ithr].join();
        std::copy(bufs[cur].begin(), bufs[cur].begin() + 8, std::ostream_iterator<int>(std::cout, ""));
        std::cout << '\n';
    }
}
