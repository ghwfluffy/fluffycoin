#pragma once

#include <fluffycoin/log/Logger.h>
#include <fluffycoin/utils/AnyMap.h>
#include <fluffycoin/utils/ErrorStatus.h>

#include <fmt/format.h>

namespace fluffycoin
{

/**
 * Generically carry around extra details about
 * an operation. Helps tunnel parameters through
 * several API layers without destroying the API.
 *
 * Keeps error information tracking uniform.
 */
class Details : public AnyMap
{
    public:
        Details();
        Details(log::Level level);
        template<typename LogCategory>
        Details(LogCategory cat, log::Level level = log::Level::Error)
            : Details(level)
        {
            this->logCat = log::Category::catToInt(cat);
        }
        Details(Details &&) = default;
        Details(const Details &) = default;
        Details &operator=(Details &&) = default;
        Details &operator=(const Details &) = default;
        ~Details() = default;

        bool isOk() const;

        template<typename... Args>
        void setError(ErrorCode code, const char *field, fmt::format_string<Args...> s, Args&&... args)
        {
            errorStatus.set(logger, logCat, code, field, fmt::format(s, std::forward<Args>(args)...));
        }

        template<typename Category, typename... Args,
            typename = std::enable_if_t<std::is_enum<Category>::value>>
        void setError(Category cat, ErrorCode code, const char *field, fmt::format_string<Args...> s, Args&&... args)
        {
            errorStatus.set(logger, cat, code, field, fmt::format(s, std::forward<Args>(args)...));
        }

        template<typename Category, typename... Args,
            typename = std::enable_if_t<std::is_enum<Category>::value>>
        void setError(log::Level eLevel, Category cat, ErrorCode code, const char *field, fmt::format_string<Args...> s, Args&&... args)
        {
            log::Level eOrigLevel = errorStatus.getLogLevel();
            errorStatus.setLogLevel(eLevel);
            errorStatus.set(logger, cat, code, field, fmt::format(s, std::forward<Args>(args)...));
            errorStatus.setLogLevel(eOrigLevel);
        }

        template<typename Category, typename... Args,
            typename = std::enable_if_t<std::is_enum<Category>::value>>
        void extendError(Category cat, const char *field, fmt::format_string<Args...> s, Args&&... args)
        {
            errorStatus.extend(logger, cat, field, fmt::format(s, std::forward<Args>(args)...));
        }

        log::Logger &log() const;

        ErrorStatus &error();
        const ErrorStatus &error() const;
        void setErrorStatus(ErrorStatus errorStatus);

    private:
        size_t logCat;
        ErrorStatus errorStatus;
        mutable log::Logger logger;
};

}
