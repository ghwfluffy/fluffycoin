#include <fluffycoin/async/Scheduler.h>
#include <fluffycoin/log/Log.h>

#include <boost/asio/io_context.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <atomic>
#include <thread>
#include <list>

using namespace fluffycoin;
using namespace fluffycoin::async;

namespace fluffycoin::async
{

struct SchedulerImpl
{
    std::atomic<bool> running = false;
    unsigned int numThreads = 1;
    boost::asio::io_context context;
    std::list<std::thread> threads;
    std::unique_ptr<boost::asio::deadline_timer> timer;

    void run();
    void execTimer(const boost::system::error_code &e);
};

}

void SchedulerImpl::run()
{
    log::info("Async scheduler thread started.");

    while (running)
        context.run();

    log::info("Async scheduler thread ended.");
}

void SchedulerImpl::execTimer(const boost::system::error_code &e)
{
    if (e == boost::asio::error::operation_aborted)
        log::info("Async scheduler stopped.");
    else
        timer->async_wait(std::bind(&SchedulerImpl::execTimer, this, std::placeholders::_1));
}

Scheduler::Scheduler()
        : impl(std::make_unique<SchedulerImpl>())
{
}

Scheduler::~Scheduler()
{
}

void Scheduler::setThreads(unsigned int threads)
{
    impl->numThreads = threads;

    // Hopefully we're not running yet
    if (impl->running && threads != impl->threads.size())
    {
        if (threads < impl->threads.size())
            start();
        else
            log::error("Async scheduler threads need to be set before starting scheduler.");
    }
}

void Scheduler::start()
{
    impl->running = true;

    // Timer makes it so that io_context::run does not exit while the scheduler is running
    if (!impl->timer)
    {
        impl->timer = std::make_unique<boost::asio::deadline_timer>(impl->context, boost::posix_time::minutes(60));
        impl->execTimer(boost::system::error_code());
    }

    // Start threads
    // TODO
    while (impl->threads.size() < impl->numThreads)
        impl->threads.emplace_back(&SchedulerImpl::run, impl.get());
}

void Scheduler::stop()
{
    impl->running = false;
    if (impl->timer)
    {
        impl->timer->cancel();
        impl->timer.reset();
    }
    impl->context.stop();
    for (auto &ptThread : impl->threads)
        ptThread.join();
    impl->threads.clear();
}

boost::asio::io_context &Scheduler::asio()
{
    return impl->context;
}
