#include <fluffycoin/alg/BlockFilename.h>

#include <fmt/format.h>

using namespace fluffycoin;
using namespace fluffycoin::alg;
using namespace BlockFilename;

std::string BlockFilename::get(
    uint32_t reconciliation,
    uint32_t shard,
    uint32_t block)
{
    return fmt::format("{:08}-{:04}-{:04}.der", reconciliation, shard, block);
}

namespace
{

constexpr uint32_t from8(
    const char *p)
{
    return
        static_cast<uint32_t>((p[0] - '0') * 10000000) +
        static_cast<uint32_t>((p[1] - '0') * 1000000) +
        static_cast<uint32_t>((p[2] - '0') * 100000) +
        static_cast<uint32_t>((p[3] - '0') * 10000) +
        static_cast<uint32_t>((p[4] - '0') * 1000) +
        static_cast<uint32_t>((p[5] - '0') * 100) +
        static_cast<uint32_t>((p[6] - '0') * 10) +
        static_cast<uint32_t>((p[7] - '0') * 1);
}

constexpr uint32_t from4(
    const char *p)
{
    return
        static_cast<uint32_t>((p[0] - '0') * 1000) +
        static_cast<uint32_t>((p[1] - '0') * 100) +
        static_cast<uint32_t>((p[2] - '0') * 10) +
        static_cast<uint32_t>((p[3] - '0') * 1);
}

}

bool BlockFilename::parse(
    const std::string &str,
    uint32_t &reconciliation,
    uint32_t &shard,
    uint32_t &block)
{
    // Validate length
    if (str.length() != 22)
        return false;
    // Validate extension
    if (str.find(".der", str.length() - 4) == std::string::npos)
        return false;

    // Validate characters in each position
    for (size_t ui = 0; ui < str.length() - 4; ui++)
    {
        char c = str[ui];
        if (ui == 8 || ui == 13)
        {
            if (c != '-')
                return false;
        }
        else if (c < '0' || c > '9')
        {
            return false;
        }
    }

    // Output
    reconciliation = from8(str.c_str());
    shard = from4(str.c_str() + 9);
    block = from4(str.c_str() + 14);
    return true;
}
