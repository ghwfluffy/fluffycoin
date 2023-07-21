#include <fluffycoin/log/Logger.h>
#include <fluffycoin/log/LogTime.h>
#include <fluffycoin/log/Log.h>

#include <nlohmann/json.hpp>

#include <map>
#include <list>

using namespace fluffycoin;
using namespace fluffycoin::log;

namespace fluffycoin::log
{

class LoggerImpl
{
    public:
        LoggerImpl() = default;
        LoggerImpl(LoggerImpl &&) = default;
        LoggerImpl(const LoggerImpl &) = default;
        LoggerImpl &operator=(LoggerImpl &&) = default;
        LoggerImpl &operator=(const LoggerImpl &) = default;
        ~LoggerImpl() = default;

        nlohmann::ordered_json json;

        std::map<std::string, nlohmann::ordered_json> context;
};

}

Logger::Logger()
        : impl(std::make_unique<LoggerImpl>())
{
}

Logger::Logger(Logger &&rhs)
    : impl(std::move(rhs.impl))
{
    rhs.impl = std::make_unique<LoggerImpl>();
}

Logger::Logger(const Logger &rhs)
    : impl(std::make_unique<LoggerImpl>(*rhs.impl))
{
}

Logger &Logger::operator=(Logger &&rhs)
{
    if (this != &rhs)
    {
        impl = std::move(rhs.impl);
        rhs.impl = std::make_unique<LoggerImpl>();
    }
    return (*this);
}

Logger &Logger::operator=(const Logger &rhs)
{
    if (this != &rhs)
    {
        (*impl) = *rhs.impl;
    }
    return (*this);
}

Logger::~Logger()
{
}

void Logger::addContext(const std::string &attr, const std::string &value)
{
    try {
        (impl->context)[attr] = value;
    } catch (const std::exception &e) {
        fprintf(stderr, "Failed to add log context '%s': %s\n", attr.c_str(), e.what());
    }
}

void Logger::addContext(const std::string &attr, int64_t value)
{
    try {
        (impl->context)[attr] = value;
    } catch (const std::exception &e) {
        fprintf(stderr, "Failed to add log context '%s': %s\n", attr.c_str(), e.what());
    }
}

void Logger::addContext(const std::string &attr, uint64_t value)
{
    try {
        (impl->context)[attr] = value;
    } catch (const std::exception &e) {
        fprintf(stderr, "Failed to add log context '%s': %s\n", attr.c_str(), e.what());
    }
}

void Logger::addContext(const std::string &attr, bool value)
{
    try {
        (impl->context)[attr] = value;
    } catch (const std::exception &e) {
        fprintf(stderr, "Failed to add log context '%s': %s\n", attr.c_str(), e.what());
    }
}

void Logger::addListContext(const std::string &attr, const std::string &value)
{
    try {
        auto iter = impl->context.find(attr);
        if (iter == impl->context.end())
        {
            impl->context[attr] = std::list<std::string> {value};
        }
        else if (iter->second.is_array())
        {
            iter->second.push_back(value);
        }
        else if (iter->second.is_string())
        {
            std::string existing = iter->second.get<std::string>();
            iter->second = std::list<std::string> {existing, value};
        }
        else
        {
            nlohmann::ordered_json existing = std::move(iter->second);
            iter->second = std::list<nlohmann::ordered_json> {std::move(existing), value};
        }
    } catch (const std::exception &e) {
        fprintf(stderr, "Failed to add log list context '%s': %s\n", attr.c_str(), e.what());
    }
}

void Logger::msg(size_t cat, Level level, const std::string &str, int errorCode, const char *errorStr)
{
    // Not logging this level
    if (!log::check(level))
        return;

    try {
        nlohmann::ordered_json data;
        data["time"] = LogTime::get();
        data["level"] = to_string(level);
        data["category"] = Category::get(cat);
        if (errorCode != 0)
            data["code"] = errorCode;
        if (errorStr && *errorStr)
            data["error"] = errorStr;
        // Context
        for (const auto &pair : impl->context)
            data[pair.first] = pair.second;
        // Message
        data["msg"] = str;
        // Log
        log::msg(data.dump());
    } catch (const std::exception &e) {
        fprintf(stderr, "Failed to log message (%s): %s\n", e.what(), str.c_str());
    }
}
