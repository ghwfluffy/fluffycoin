#pragma once

#include <fluffycoin/utils/ArgParser.h>

namespace fluffycoin::svc
{

namespace LogArgs
{
    void initCmdLineParams(ArgParser &parser);
    void setCmdLineArgs(const Args &args);
    std::string logFile(const Args &args);
}

}
