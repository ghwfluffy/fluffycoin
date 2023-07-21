#pragma once

#include <fluffycoin/log/Level.h>
#include <fluffycoin/log/Category.h>

#include <fmt/format.h>

#include <string>

namespace fluffycoin
{

/**
 * Developer logs
 */
namespace log
{
    void setConsole(bool bConsole);
    void setFile(const std::string &file);
    void setFileSize(size_t uiMaxSizeKb);
    void setLevel(Level level);

    bool check(Level level);
    void msg(Level level, const std::string &str);
    void msg(size_t category, Level level, const std::string &str);

    // Pre-formatted
    void msg(const std::string &str);

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
    void notice(fmt::format_string<Args...> s, Args&&... args)
    {
        if (check(Level::Notice))
            msg(Level::Notice, fmt::format(s, std::forward<Args>(args)...));
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

    template<typename CategoryEnum, typename... Args,
        typename = std::enable_if_t<std::is_enum<CategoryEnum>::value>>
    bool error(CategoryEnum cat, fmt::format_string<Args...> s, Args&&... args)
    {
        if (check(Level::Error))
            msg(Category::catToInt(cat), Level::Error, fmt::format(s, std::forward<Args>(args)...));
        return false;
    }

    template<typename CategoryEnum, typename... Args,
        typename = std::enable_if_t<std::is_enum<CategoryEnum>::value>>
    void warn(CategoryEnum cat, fmt::format_string<Args...> s, Args&&... args)
    {
        if (check(Level::Warn))
            msg(Category::catToInt(cat), Level::Warn, fmt::format(s, std::forward<Args>(args)...));
    }

    template<typename CategoryEnum, typename... Args,
        typename = std::enable_if_t<std::is_enum<CategoryEnum>::value>>
    void notice(CategoryEnum cat, fmt::format_string<Args...> s, Args&&... args)
    {
        if (check(Level::Notice))
            msg(Category::catToInt(cat), Level::Notice, fmt::format(s, std::forward<Args>(args)...));
    }

    template<typename CategoryEnum, typename... Args,
        typename = std::enable_if_t<std::is_enum<CategoryEnum>::value>>
    void info(CategoryEnum cat, fmt::format_string<Args...> s, Args&&... args)
    {
        if (check(Level::Info))
            msg(Category::catToInt(cat), Level::Info, fmt::format(s, std::forward<Args>(args)...));
    }

    template<typename CategoryEnum, typename... Args,
        typename = std::enable_if_t<std::is_enum<CategoryEnum>::value>>
    void debug(CategoryEnum cat, fmt::format_string<Args...> s, Args&&... args)
    {
        if (check(Level::Debug))
            msg(Category::catToInt(cat), Level::Debug, fmt::format(s, std::forward<Args>(args)...));
    }

    template<typename CategoryEnum, typename... Args,
        typename = std::enable_if_t<std::is_enum<CategoryEnum>::value>>
    void traffic(CategoryEnum cat, fmt::format_string<Args...> s, Args&&... args)
    {
        if (check(Level::Traffic))
            msg(Category::catToInt(cat), Level::Traffic, fmt::format(s, std::forward<Args>(args)...));
    }
}

}
