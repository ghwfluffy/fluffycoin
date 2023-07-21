#pragma once

#include <fluffycoin/async/boost.h>

#include <memory>

namespace fluffycoin::async
{

// Private implementation
struct SchedulerImpl;

/**
 * Runs the Boost ASIO event loop.
 *
 * This class is not reentrant. Initialize desired threads before starting.
 */
class Scheduler
{
    public:
        Scheduler();
        Scheduler(Scheduler &&) = delete;
        Scheduler(const Scheduler &) = delete;
        Scheduler &operator=(Scheduler &&) = delete;
        Scheduler &operator=(const Scheduler &) = delete;
        ~Scheduler();

        void setThreads(unsigned int threads);

        void start();
        void stop();

        boost::asio::io_context &asio();

    private:
        std::unique_ptr<SchedulerImpl> impl;
};

}
