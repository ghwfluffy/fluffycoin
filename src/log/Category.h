#pragma once

#include <typeinfo>
#include <type_traits>

namespace fluffycoin::log
{

/**
 * Common categories
 */
enum BasicCategory
{
    General,
    Comm,
    Db,
    Auth,
    Init,
};

/**
 * Log category utilities
 */
namespace Category
{
    template<typename Category>
    std::size_t catToInt(Category cat)
    {
        return typeid(Category).hash_code() + static_cast<std::size_t>(cat);
    }

    template<>
    std::size_t catToInt(std::size_t cat);

    void addInternal(std::size_t cat, const char *name);

    template<typename Category>
    void add(Category cat, const char *name)
    {
        addInternal(catToInt(cat), name);
    }

    const char *get(std::size_t);

    void init();
}

}
