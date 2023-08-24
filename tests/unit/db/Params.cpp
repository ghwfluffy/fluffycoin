#include <fluffytest/db/Params.h>

using namespace fluffytest::db;

namespace
{

std::string gparams;

}

const std::string &Params::get()
{
    return gparams;
}

void Params::set(const char *params)
{
    if (params)
        gparams = params;
}
