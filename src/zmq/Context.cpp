#include <fluffycoin/zmq/Context.h>
#include <fluffycoin/log/Log.h>

#include <zmq.h>

#include <string.h>

using namespace fluffycoin;
using namespace fluffycoin::zmq;

Context::Context()
{
    ctx = zmq_ctx_new();
    if (!ctx)
        log::error(log::Comm, "Failed to create ZMQ context: ({}) {}.", errno, strerror(errno));
}

Context::Context(Context &&rhs)
{
    ctx = nullptr;
    operator=(std::move(rhs));
}

Context &Context::operator=(Context &&rhs)
{
    if (this != &rhs)
    {
        cleanup();
        ctx = rhs.ctx;
        rhs.ctx = nullptr;
    }
    return (*this);
}

Context::~Context()
{
    cleanup();
}

void Context::cleanup()
{
    if (ctx)
    {
        int ret = zmq_ctx_shutdown(ctx);
        if (ret != 0)
            log::error(log::Comm, "Failed to shutdown ZMQ context: ({}) {}.", errno, strerror(errno));

        zmq_ctx_destroy(ctx);
        if (ret != 0)
            log::error(log::Comm, "Failed to destroy ZMQ context: ({}) {}.", errno, strerror(errno));

        ctx = nullptr;
    }
}

Context::operator void*() const
{
    return ctx;
}
