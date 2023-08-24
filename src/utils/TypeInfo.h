#pragma once

#include <string>
#include <typeinfo>

#include <cxxabi.h>

namespace fluffycoin
{

namespace TypeInfo
{
    template<typename T>
    std::string name(const T &t)
    {
        int status = 0;
        char *realname = abi::__cxa_demangle(typeid(t).name(), nullptr, nullptr, &status);
        std::string result(realname);
        free(realname);
        return result;
    }
}

}
