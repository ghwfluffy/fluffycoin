#pragma once

#include <string>

namespace fluffycoin::log
{

/**
 * Format log time
 */
namespace LogTime
{
    void setUtc();
    void setLocal();

    std::string get();
}

}
