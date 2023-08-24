#pragma once

#include <string>

/**
 * Postgres parameters for integration tests
 */
namespace fluffytest::db::Params
{
    const std::string &get();
    void set(const char *params);
}
