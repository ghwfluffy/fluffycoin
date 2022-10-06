#pragma once

#include <string>
#include <functional>

#include <fmt/format.h>

namespace fluffycoin
{

/**
 * Developer logs
 */
namespace log
{
    /**
     * Levels for log verbosity
     */
    enum class Level
    {
        None = 0,       /* <! Do not log */
        Error,          /* <! Things that should not happen and are triggering an error state */
        Warn,           /* <! Things that should not happen but are not triggering an error state */
        Notice,         /* <! Unusual events */
        Info,           /* <! Events that are useful to know about */
        Debug,          /* <! Information useful for debugging a module */
        Traffic         /* <! Very verbose logs such as trace logs */
    };

    void setConsole(bool bConsole);
    void setFile(const std::string &file);
    void setFileSize(size_t uiMaxSizeKb);
    void setLevel(Level level);
    void setCallback(std::function<void(Level, const std::string &)>);

    bool check(Level level);
    void msg(Level level, const std::string &str);

    template<typename... Args>
    bool error(fmt::format_string<Args...> s, Args&&... args)
    {
        if (check(Level::Error))
            msg(Level::Error, fmt::format(s, std::forward<Args>(args)...));
        return false;
    }

    template<typename... Args>
    void warn(fmt::format_string<Args...> s, Args&&... args)
    {
        if (check(Level::Warn))
            msg(Level::Warn, fmt::format(s, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void info(fmt::format_string<Args...> s, Args&&... args)
    {
        if (check(Level::Info))
            msg(Level::Info, fmt::format(s, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void debug(fmt::format_string<Args...> s, Args&&... args)
    {
        if (check(Level::Debug))
            msg(Level::Debug, fmt::format(s, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void traffic(fmt::format_string<Args...> s, Args&&... args)
    {
        if (check(Level::Traffic))
            msg(Level::Traffic, fmt::format(s, std::forward<Args>(args)...));
    }
}

void from_string(const std::string &, log::Level &);
std::string to_string(log::Level);

}
