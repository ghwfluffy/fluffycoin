#pragma once

#include <fluffycoin/svc/ServiceScene.h>

#include <stdint.h>

namespace fluffycoin::curator
{

/**
 * Reads block in from filesystem and fill the database
 */
namespace LoadFromFs
{
    void init(
        const svc::ServiceScene &ctx,
        uint64_t reloadFrom);
}

}
