#pragma once

#include <fluffycoin/async/boost.h>
#include <fluffycoin/utils/Details.h>
#include <fluffycoin/utils/BinData.h>

#include <memory>
#include <functional>

namespace fluffycoin::async
{

struct SocketImpl;

/**
 * Wrap around a posix file descriptor and provide async functionality.
 * Only one timeout can be active at a time. A timeout will cancel all
 * pending asynchronous actions.
 */
class Socket
{
    public:
        Socket();
        Socket(
            boost::asio::io_context &asio,
            int iFD,
            bool bClose = false);
        Socket(Socket &&);
        Socket(const Socket &) = delete;
        Socket &operator=(Socket &&);
        Socket &operator=(const Socket &) = delete;
        ~Socket();

        operator bool() const;

        void read(
            Details &details,
            std::function<void(Details &)> callback,
            unsigned int timeout = 0);

        void write(
            Details &details,
            BinData data,
            std::function<void(Details &)> callback,
            unsigned int timeout = 0);

        void wait(
            Details &details,
            std::function<void(Details &)> callback,
            unsigned int timeout = 0);

    private:
        std::unique_ptr<SocketImpl> impl;
};

}
