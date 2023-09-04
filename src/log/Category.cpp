#include <fluffycoin/log/Category.h>

#include <map>

using namespace fluffycoin;

namespace
{

std::map<std::size_t, const char *> &categories()
{
    static std::map<std::size_t, const char *> categories;
    return categories;
}

}

void log::Category::init()
{
    log::Category::add(log::General, "GENERAL");
    log::Category::add(log::Comm, "COMM");
    log::Category::add(log::Db, "DATABASE");
    log::Category::add(log::Auth, "AUTH");
    log::Category::add(log::Init, "INIT");
}

void log::Category::addInternal(std::size_t cat, const char *name)
{
    categories()[cat] = name;
}

const char *log::Category::get(std::size_t cat)
{
    if (!cat)
        cat = catToInt(log::General);
    auto &cats = categories();
    auto iter = cats.find(cat);
    return iter == cats.end() ? "UNKNOWN" : iter->second;
}

template<>
std::size_t log::Category::catToInt(std::size_t cat)
{
    return cat;
}
