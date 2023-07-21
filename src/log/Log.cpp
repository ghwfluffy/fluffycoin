#include <fluffycoin/log/Log.h>
#include <fluffycoin/log/LogTime.h>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <mutex>
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
size_t maxFileSizeKB = 1024 * 1024;

}

void log::setConsole(bool bConsole)
{
    ::bConsole = bConsole;
    // Unbuffered
    if (bConsole)
        setbuf(stdout, nullptr);
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

void log::msg(Level eLevel, const std::string &str)
{
    msg(0, eLevel, str);
}

void log::msg(size_t category, Level eLevel, const std::string &str)
{
    // Check level is enabled
    if (!check(eLevel))
        return;

    // We log in JSON
    nlohmann::ordered_json json;
    json["time"] = LogTime::get();
    json["level"] = to_string(eLevel);
    json["category"] = Category::get(category);
    json["msg"] = str;

    // Build log string
    std::string text = json.dump();

    msg(text);
}

void log::msg(const std::string &text)
{
    // Check we have some place to log to
    if (!bConsole && (logFile.empty() || maxFileSizeKB == 0))
        return;

    // One log at a time
    std::unique_lock<std::mutex> lock(mtx);

    // Log to console
    if (bConsole)
        printf("%s\n", text.c_str());

    // Log to file
    if (!logFile.empty() && maxFileSizeKB > 0)
    {
        FILE *fp = fopen(logFile.c_str(), "a");
        if (fp)
        {
            fprintf(fp, "%s\n", text.c_str());
            long lFileSize = ftell(fp);
            fclose(fp);
            // Attempt to rotate log file if it is too big
            if (lFileSize > 0 && static_cast<size_t>(lFileSize / 1024) >= maxFileSizeKB)
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
    return (bConsole || (!logFile.empty() && maxFileSizeKB > 0)) && level >= eLevel;
}
