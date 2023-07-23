#include <fluffycoin/utils/Strings.h>

#include <stdlib.h>

template<>
std::string fluffycoin::from_string(const std::string &str)
{
    return str;
}

template<>
int8_t fluffycoin::from_string(const std::string &str)
{
    return static_cast<int8_t>(atoi(str.c_str()));
}

template<>
uint8_t fluffycoin::from_string(const std::string &str)
{
    return static_cast<uint8_t>(atoi(str.c_str()));
}

template<>
int16_t fluffycoin::from_string(const std::string &str)
{
    return static_cast<int16_t>(atoi(str.c_str()));
}

template<>
uint16_t fluffycoin::from_string(const std::string &str)
{
    return static_cast<uint16_t>(atoi(str.c_str()));
}

template<>
int32_t fluffycoin::from_string(const std::string &str)
{
    return static_cast<int32_t>(atoi(str.c_str()));
}

template<>
uint32_t fluffycoin::from_string(const std::string &str)
{
    return static_cast<uint32_t>(atoll(str.c_str()));
}

template<>
int64_t fluffycoin::from_string(const std::string &str)
{
    return static_cast<int64_t>(atoll(str.c_str()));
}

template<>
uint64_t fluffycoin::from_string(const std::string &str)
{
    return static_cast<uint64_t>(strtoull(str.c_str(), nullptr, 10));
}

template<>
bool fluffycoin::from_string(const std::string &str)
{
    return atoi(str.c_str()) > 0 || str == "t" || str == "true";
}
