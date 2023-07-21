#pragma once

#include <string>
#include <sstream>

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
    T ret();
    from_string(ret, str);
    return ret;
}

}
