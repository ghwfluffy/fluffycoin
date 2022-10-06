#include <fluffycoin/log/Log.h>

#include <algorithm>
#include <mutex>
#include <chrono>
#include <filesystem>

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

using namespace fluffycoin;

namespace
{

std::mutex mtx;
bool bConsole = false;
std::string logFile;
log::Level level = log::Level::Info;
std::function<void(log::Level, const std::string &)> gpfCallback;
size_t maxFileSizeKB = 100 * 1024;
std::string to_string_upper(log::Level eLevel);

}

void log::setConsole(bool bConsole)
{
    bConsole = bConsole;
}

void log::setFile(const std::string &file)
{
    logFile = file;
}

void log::setFileSize(size_t uiMaxSizeKb)
{
    maxFileSizeKB = uiMaxSizeKb;
}

void log::setLevel(Level eLevel)
{
    level = eLevel;
}

void log::setCallback(std::function<void(Level, const std::string &)> pfCallback)
{
    gpfCallback = pfCallback;
}

void log::msg(Level eLevel, const std::string &str)
{
    // Check level is enabled
    if (eLevel == Level::None || level < eLevel)
        return;

    // Log callback
    if (gpfCallback)
        gpfCallback(eLevel, str);

    // Check we have some place to log to
    if (!bConsole && logFile.empty())
        return;

    // Get the level and pad it to a static length
    bool bEven = false;
    std::string strLevel = to_string_upper(eLevel);

    // Calculate timestamp
    char szTimestamp[32] = {};
    std::time_t uiNow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::strftime(szTimestamp, sizeof(szTimestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&uiNow));

    // Calculate log string
    std::string text = fmt::format("[{}] | {} | {}", szTimestamp, strLevel, str);

    // One log at a time
    std::unique_lock<std::mutex> lock(mtx);

    // Log to console
    if (bConsole)
        fprintf(eLevel == Level::Error ? stderr : stdout, "%s\n", text.c_str());

    // Log to file
    if (!logFile.empty())
    {
        FILE *fp = fopen(logFile.c_str(), "a");
        if (fp)
        {
            fprintf(fp, "%s\n", text.c_str());
            long lFileSize = ftell(fp);
            fclose(fp);
            // Attempt to rotate log file if it is too big
            if (lFileSize > 0 && static_cast<size_t>(lFileSize) > maxFileSizeKB)
            {
                try
                {
                    std::filesystem::rename(logFile, logFile + ".bak");
                }
                catch (std::filesystem::filesystem_error &e)
                {
                    fprintf(stderr, "Failed to move log file for rotation: %s\n", e.what());
                    try
                    {
                        std::filesystem::remove(logFile);
                    }
                    catch (std::filesystem::filesystem_error &e2)
                    {
                        fprintf(stderr, "Failed to remove log file that failed to rotate: %s\n", e2.what());
                    }
                }
            }
        }
    }
}

bool log::check(Level eLevel)
{
    return level >= eLevel;
}

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

namespace
{
std::string to_string_upper(log::Level eLevel)
{
    switch (eLevel)
    {
        default:
        case log::Level::None: return    "  NONE   ";
        case log::Level::Error: return   "  ERROR  ";
        case log::Level::Warn: return    "  WARN   ";
        case log::Level::Notice: return  " NOTICE  ";
        case log::Level::Info: return    "  INFO   ";
        case log::Level::Debug: return   "  DEBUG  ";
        case log::Level::Traffic: return " TRAFFIC ";
    }
}
}

std::string fluffycoin::to_string(log::Level eLevel)
{
    switch (eLevel)
    {
        default:
        case log::Level::None: return "None";
        case log::Level::Error: return "Error";
        case log::Level::Warn: return "Warn";
        case log::Level::Notice: return "Notice";
        case log::Level::Info: return "Info";
        case log::Level::Debug: return "Debug";
        case log::Level::Traffic: return "Traffic";
    }
}
