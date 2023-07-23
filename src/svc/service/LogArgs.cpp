#include <fluffycoin/svc/LogArgs.h>

#include <fluffycoin/log/Log.h>
#include <fluffycoin/log/LogTime.h>

using namespace fluffycoin;
using namespace fluffycoin::svc;

void LogArgs::initCmdLineParams(ArgParser &parser)
{
    parser.addCategory("Logging");
    parser.addSwitch("log-console", "Log to console");
    parser.addParam("log-file", "Log to a specific file");
    parser.addParam("log-file-size", "Maximum size (in KB) for log file\n"
                                     "0=Don't log to file");
    parser.addSwitch("log-debug", "Log debug information");
    parser.addSwitch('v', "log-verbose", "Log verbose information");
    parser.addSwitch("log-localtime", "Log using local time (default: UTC)");
}

void LogArgs::setCmdLineArgs(const Args &args)
{
    // Log console
    if (args.hasArg("log-console"))
        log::setConsole(true);

    // Log file
    if (args.hasArg("log-file"))
        log::setFile(args.getArg("log-file"));

    if (args.hasArg("log-file-size"))
        log::setFileSize(args.getSizeT("log-file-size"));

    // Log level
    if (args.hasArg("log-verbose"))
        log::setLevel(log::Level::Traffic);
    else if (args.hasArg("log-debug"))
        log::setLevel(log::Level::Debug);
    else
        log::setLevel(log::Level::Info);

    // Timezone
    if (args.hasArg("log-localtime"))
        log::LogTime::setLocal();
}

std::string LogArgs::logFile(const Args &args)
{
    return args.getArg("log-file");
}
