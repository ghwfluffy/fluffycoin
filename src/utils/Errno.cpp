#include <fluffycoin/utils/Errno.h>

#include <fmt/format.h>

using namespace fluffycoin;

std::string Errno::error()
{
    char buf[1024] = {};
    if (strerror_r(errno, buf, sizeof(buf) - 1) == 0)
        return fmt::format("({}) {}", errno, buf);

    return fmt::format("Error number {}", errno);
}
