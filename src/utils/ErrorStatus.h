#pragma once

#include <fluffycoin/utils/ErrorCode.h>

#include <fluffycoin/log/Logger.h>

namespace fluffycoin
{

/**
 * Track error information. Log when an error is set.
 * Combination of error code + error field should be
 * unique enough to handle errors programatically.
 * Error string is human readable and contains
 * sentence punctuation.
 */
class ErrorStatus
{
    public:
        ErrorStatus();
        ErrorStatus(ErrorStatus &&) = default;
        ErrorStatus(const ErrorStatus &) = default;
        ErrorStatus &operator=(ErrorStatus &&) = default;
        ErrorStatus &operator=(const ErrorStatus &) = default;
        ~ErrorStatus() = default;

        log::Level getLogLevel() const;
        void setLogLevel(log::Level level);

        ErrorCode getCode() const;
        void setCode(ErrorCode errorCode);

        const std::string &getStr() const;
        void setStr(std::string error);

        const std::string &getField() const;
        void setField(std::string errorField);

        const char *c_str() const;

        void set(log::Logger &log, size_t cat, ErrorCode code, const char *field, std::string msg);

        template<typename Category>
        void set(log::Logger &log, Category cat, ErrorCode code, const char *field, std::string msg)
        {
            set(log, log::Category::catToInt(cat), code, field, std::move(msg));
        }

    private:
        log::Level level;
        ErrorCode errorCode;
        std::string errorStr;
        std::string errorField;
};

}
