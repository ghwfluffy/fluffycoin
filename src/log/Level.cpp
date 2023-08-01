#include <fluffycoin/log/Level.h>

#include <algorithm>

void fluffycoin::from_string(const std::string &str, log::Level &eLevel)
{
    std::string strUpper(str);
    std::transform(strUpper.begin(), strUpper.end(), strUpper.begin(), ::toupper);

    if (strUpper == "ERROR")
        eLevel = log::Level::Error;
    else if (strUpper == "WARN")
        eLevel = log::Level::Warn;
    else if (strUpper == "NOTICE")
        eLevel = log::Level::Notice;
    else if (strUpper == "INFO")
        eLevel = log::Level::Info;
    else if (strUpper == "DEBUG")
        eLevel = log::Level::Debug;
    else if (strUpper == "TRAFFIC")
        eLevel = log::Level::Traffic;
    else
        eLevel = log::Level::None;
}

std::string fluffycoin::to_string(log::Level eLevel)
{
    switch (eLevel)
    {
        default:
        case log::Level::None: return "NONE";
        case log::Level::Error: return "ERROR";
        case log::Level::Warn: return "WARN";
        case log::Level::Notice: return "NOTICE";
        case log::Level::Info: return "INFO";
        case log::Level::Debug: return "DEBUG";
        case log::Level::Traffic: return "TRAFFIC";
    }
}
