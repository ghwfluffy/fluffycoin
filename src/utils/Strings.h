#pragma once

#include <string>
#include <sstream>

#include <stdint.h>

namespace fluffycoin
{

template<typename T>
std::string to_string(T &&t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

template<typename T>
T from_string(const std::string &str)
{
    T ret{};
    from_string(ret, str);
    return ret;
}

template<>
std::string from_string(const std::string &str);

template<>
int8_t from_string(const std::string &str);

template<>
uint8_t from_string(const std::string &str);

template<>
int16_t from_string(const std::string &str);

template<>
uint16_t from_string(const std::string &str);

template<>
int32_t from_string(const std::string &str);

template<>
uint32_t from_string(const std::string &str);

template<>
int64_t from_string(const std::string &str);

template<>
uint64_t from_string(const std::string &str);

template<>
bool from_string(const std::string &str);

}
