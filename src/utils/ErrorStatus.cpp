#include <fluffycoin/utils/ErrorStatus.h>

using namespace fluffycoin;

ErrorStatus::ErrorStatus()
{
    this->level = log::Level::Error;
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
    // TODO: Logger should take the error field too no?
    log.msg(cat, level, msg, static_cast<int>(code), to_string(code).c_str());

    this->errorCode = code;
    if (field)
        this->errorField = field;
    else
        this->errorField.clear();
    this->errorStr = std::move(msg);
}

// Add more context to the previous error
void ErrorStatus::extend(log::Logger &log, size_t cat, const char *field, std::string msg)
{
    if (field)
        this->errorField = field;

    if (!msg.empty())
    {
        log.msg(cat, level, msg, static_cast<int>(this->errorCode), to_string(this->errorCode).c_str());

        if (this->errorStr.empty())
            this->errorStr = std::move(msg);
        else
        {
            if (msg[msg.length() - 1] == '.')
            {
                msg[msg.length() - 1] = ':';
                msg += ' ';
            }
            else
            {
                msg += ": ";
            }

            msg += this->errorStr;
            this->errorStr = std::move(msg);
        }
    }
}
