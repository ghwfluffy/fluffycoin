#pragma once

#include <fluffycoin/svc/ServiceScene.h>

#include <fluffycoin/async/Ret.h>

#include <stdint.h>

namespace fluffycoin::curator
{

/**
 * Reads block in from filesystem and fill the database
 */
namespace LoadFromFs
{
    async::Ret<void> init(
        const svc::ServiceScene &ctx,
        uint64_t reloadFrom);
}

}
