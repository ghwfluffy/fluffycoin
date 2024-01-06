#include <fluffycoin/utils/AnyMap.h>

using namespace fluffycoin;

AnyMap::AnyMap(const AnyMap &t)
{
    operator=(t);
}

AnyMap &AnyMap::operator=(const AnyMap &t)
{
    if (this != &t)
    {
        data.clear();
        for (auto &pair : t.data)
        {
            auto pt = pair.second->clone();
            if (pt)
                data.emplace(pair.first, std::move(pt));
        }
    }
    return (*this);
}

AnyMap &AnyMap::operator+=(AnyMap &&rhs)
{
    if (this != &rhs)
    {
        for (auto &pair : rhs.data)
            data[pair.first] = std::move(pair.second);
    }

    return (*this);
}

AnyMap &AnyMap::operator+=(const AnyMap &rhs)
{
    if (this != &rhs)
    {
        for (auto &pair : rhs.data)
        {
            auto pt = pair.second->clone();
            if (pt)
                data[pair.first] = pair.second->clone();
        }
    }

    return (*this);
}
