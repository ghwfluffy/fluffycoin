#pragma once

#include <fluffycoin/utils/BinData.h>
#include <fluffycoin/utils/Details.h>

#include <fluffycoin/zmq/Context.h>

namespace fluffycoin::zmq
{

/**
 * Common routines used internally for the ZMQ classes
 */
namespace Utils
{
    int getFd(void *socket);

    void *bindSocket(
        const Context *ctx,
        int socketType,
        const std::string &host,
        uint16_t port,
        const BinData &serverKey,
        Details &details);

    void *connectSocket(
        const Context *ctx,
        int socketType,
        const std::string &host,
        uint16_t port,
        const BinData &serverKey,
        Details &details);
}

}
