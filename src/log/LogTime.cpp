#include <fluffycoin/log/LogTime.h>

#include <chrono>
#include <string>

#include <time.h>

using namespace fluffycoin;
using namespace fluffycoin::log;

namespace
{

bool bUtc = true;

}

void LogTime::setUtc()
{
    bUtc = true;
}

void LogTime::setLocal()
{
    bUtc = false;
}

std::string LogTime::get()
{
    // Get time
    std::time_t uiNow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm comp = {};
    if (bUtc)
        ::gmtime_r(&uiNow, &comp);
    else
        ::localtime_r(&uiNow, &comp);

    // Stringify
    char szTimestamp[32] = {};
    std::strftime(szTimestamp, sizeof(szTimestamp), "%Y-%m-%d %H:%M:%S", &comp);
    return szTimestamp;
}
