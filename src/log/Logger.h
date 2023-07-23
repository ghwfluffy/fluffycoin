#pragma once

#include <fluffycoin/log/Log.h>

#include <memory>

namespace fluffycoin::log
{

// Private implementation
class LoggerImpl;

/**
 * Dependency injectable logger that carries additional context.
 * This class is not reentrant.
 */
class Logger
{
    public:
        Logger();
        Logger(Logger &&);
        Logger(const Logger &);
        Logger &operator=(Logger &&);
        Logger &operator=(const Logger &);
        ~Logger();

        void addContext(const std::string &attr, const std::string &value);
        void addContext(const std::string &attr, uint64_t value);
        void addContext(const std::string &attr, int64_t value);
        void addContext(const std::string &attr, bool value);
        void addListContext(const std::string &attr, const std::string &value);

        void msg(size_t cat, Level level, const std::string &str, int errorCode = 0, const char *errorStr = nullptr);

        template<typename CategoryEnum, typename... Args>
        void error(CategoryEnum cat, fmt::format_string<Args...> s, Args&&... args)
        {
            if (log::check(Level::Error))
                msg(Category::catToInt(cat), Level::Error, fmt::format(s, std::forward<Args>(args)...));
        }

        template<typename CategoryEnum, typename... Args>
        void warn(CategoryEnum cat, fmt::format_string<Args...> s, Args&&... args)
        {
            if (log::check(Level::Warn))
                msg(Category::catToInt(cat), Level::Warn, fmt::format(s, std::forward<Args>(args)...));
        }

        template<typename CategoryEnum, typename... Args>
        void info(CategoryEnum cat, fmt::format_string<Args...> s, Args&&... args)
        {
            if (log::check(Level::Info))
                msg(Category::catToInt(cat), Level::Info, fmt::format(s, std::forward<Args>(args)...));
        }

        template<typename CategoryEnum, typename... Args>
        void debug(CategoryEnum cat, fmt::format_string<Args...> s, Args&&... args)
        {
            if (log::check(Level::Debug))
                msg(Category::catToInt(cat), Level::Debug, fmt::format(s, std::forward<Args>(args)...));
        }

        template<typename CategoryEnum, typename... Args>
        void traffic(CategoryEnum cat, fmt::format_string<Args...> s, Args&&... args)
        {
            if (log::check(Level::Traffic))
                msg(Category::catToInt(cat), Level::Traffic, fmt::format(s, std::forward<Args>(args)...));
        }

    private:
        std::unique_ptr<LoggerImpl> impl;
};

}
