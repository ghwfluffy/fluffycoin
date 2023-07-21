#pragma once

#include <fluffycoin/async/Socket.h>

#include <mutex>

namespace fluffycoin::async
{

struct UncontrolledSocketImpl;

/**
 * Some sockets we want to be able to poll asynchronously with ASIO
 * but we are not the ones who read the data off the socket. For
 * instance ZMQ sockets which read from the socket on background
 * threads.
 *
 * This mitigates the possibility that the thread which is out of
 * our control reads the data off the socket before we can call
 * read() which would cause read() not to trigger even though
 * there is data associated with the socket ready to be processed.
 */
class UncontrolledSocket
{
    public:
        UncontrolledSocket();
        UncontrolledSocket(
            boost::asio::io_context &asio,
            int iFD);
        UncontrolledSocket(UncontrolledSocket &&);
        UncontrolledSocket(const UncontrolledSocket &) = delete;
        UncontrolledSocket &operator=(UncontrolledSocket &&);
        UncontrolledSocket &operator=(const UncontrolledSocket &) = delete;
        ~UncontrolledSocket();

        operator bool() const;

        void read(
            Details &details,
            std::function<void(Details &)> callback);

    private:
        std::unique_ptr<UncontrolledSocketImpl> impl;
};

}
