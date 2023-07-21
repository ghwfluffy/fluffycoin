#pragma once

namespace fluffycoin::zmq
{

class ContextImpl;

/**
 * ZeroMQ context. ZeroMQ starts 2 threads in the background for this context.
 */
class Context
{
    public:
        Context();
        Context(Context &&);
        Context(const Context &) = delete;
        Context &operator=(Context &&);
        Context &operator=(const Context &) = delete;
        ~Context();

        operator void *() const;

    private:
        void cleanup();

        void *ctx;
};

}
