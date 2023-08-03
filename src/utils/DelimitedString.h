#pragma once

#include <fluffycoin/utils/Strings.h>

namespace fluffycoin
{

/**
 * Utilities for de/serializing strings using delimiters
 */
namespace DelimitedString
{
    template<template <typename...> class Container, typename ElementType>
    Container<ElementType> fromString(const std::string &str, char delimiter = ',')
    {
        Container<ElementType> ret;

        size_t startPos = 0;
        size_t pos = str.find(delimiter);
        while (startPos < str.length())
        {
            if (pos == std::string::npos)
                pos = str.length();

            ret.push_back(from_string<ElementType>(str.substr(startPos, pos - startPos)));
            startPos = pos;
            pos = str.find(delimiter, startPos);
        }

        return ret;
    }

    template<typename Container>
    std::string toString(const Container &elements, char delimiter = ',')
    {
        std::string ret;
        for (const auto &element : elements)
        {
            if (!ret.empty())
                ret += delimiter;
            ret += to_string(element);
        }

        return ret;
    }
}

}
