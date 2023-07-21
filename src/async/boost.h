#pragma once

#include <features.h>

/**
 * Forward declarations for boost.
 * Everything that uses boost should use PImpl to improve compile speeds.
 */
namespace boost
{

namespace asio
{
    class io_context;
}

}
