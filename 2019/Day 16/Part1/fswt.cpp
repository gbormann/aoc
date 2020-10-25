
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <iterator>
#include <string>
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

int main()
{
    const std::string msg_txt{ read_msg_txt("../msgseq.txt") };
//    const std::string msg_txt("12345678");
    std::vector<int> msg[2];

    msg[0].resize(msg_txt.size());
    msg[1].resize(msg_txt.size());
    std::transform(msg_txt.begin(), msg_txt.end(), msg[0].begin(), [](const char& c){ return c - '0'; });

    std::copy(msg[0].begin(), msg[0].end(), std::ostream_iterator<int>(std::cout, ""));
    std::cout << '\n';

    std::size_t cur = 0;
    std::size_t prev = 1;
    for (int ph = 0; ph < 100/*4*/; ++ph)
    {
        std::swap(prev, cur);
        std::fill_n(msg[cur].begin(), msg[cur].size(), 0);
        for (std::size_t n = 0; n < msg[cur].size(); ++n)
        {
            const std::size_t np1 = n + 1;

            // c = 1 terms
            for (std::size_t k = n; k < msg[prev].size(); k += 4 * np1)
                msg[cur][n] += std::accumulate(
                    msg[prev].begin() + k,
                    msg[prev].begin() + std::min(k + np1, msg[prev].size()),
                    0
                );

            // c = -1 terms
            for (std::size_t k = 3 * np1 - 1; k < msg[prev].size(); k += 4 * np1)
                msg[cur][n] -= std::accumulate(
                    msg[prev].begin() + k,
                    msg[prev].begin() + std::min(k + np1, msg[prev].size()),
                    0
                );

            msg[cur][n] = std::abs(msg[cur][n]) % 10;
        }
//        std::copy(msg[cur].begin(), msg[cur].end(), std::ostream_iterator<int>(std::cout, ""));
//        std::cout << '\n';
    }
    std::copy(msg[cur].begin(), msg[cur].end(), std::ostream_iterator<int>(std::cout, ""));
    std::cout << '\n';
}
