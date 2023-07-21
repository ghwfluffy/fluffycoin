#include <fluffycoin/utils/ErrorStatus.h>

using namespace fluffycoin;

ErrorStatus::ErrorStatus()
{
    this->errorCode = ErrorCode::None;
}

log::Level ErrorStatus::getLogLevel() const
{
    return level;
}

void ErrorStatus::setLogLevel(log::Level level)
{
    this->level = level;
}

ErrorCode ErrorStatus::getCode() const
{
    return errorCode;
}

void ErrorStatus::setCode(ErrorCode errorCode)
{
    this->errorCode = errorCode;
}

const std::string &ErrorStatus::getStr() const
{
    return errorStr;
}

void ErrorStatus::setStr(std::string error)
{
    this->errorStr = std::move(error);
}

const std::string &ErrorStatus::getField() const
{
    return errorField;
}

void ErrorStatus::setField(std::string errorField)
{
    this->errorField = errorField;
}

const char *ErrorStatus::c_str() const
{
    return this->errorStr.c_str();
}

void ErrorStatus::set(log::Logger &log, size_t cat, ErrorCode code, const char *field, std::string msg)
{
    this->errorCode = code;
    if (field)
        this->errorField = field;
    else
        this->errorField.clear();
    this->errorStr = std::move(msg);

    log.msg(cat, level, msg, static_cast<int>(code), to_string(code).c_str());
}
