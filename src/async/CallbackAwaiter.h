#pragma once

#include <tuple>
#include <atomic>
#include <coroutine>
#include <type_traits>

#include <boost/asio/awaitable.hpp>

namespace fluffycoin::async
{

/**
 * Allows converting callback-based async functions
 * into a coroutine awaitable. The callback-based async function
 * should be executing on the same strand as the awaiter
 * or a theoretical deadlock is possible.
 *
 * @template Args The arguments of the callback async function
 */
template<typename... Args>
struct CallbackAwaiter
{
    using ResultType = std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...>;
    ResultType results;

    boost::asio::awaitable<std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...>> run()
    {
        co_return std::move(results);
    }

    auto f()
    {
        return [this](Args&&... args)
        {
            // Save result
            set(std::forward<Args>(args)...);
            isComplete = true;
            // Unwait
            if (isWaiting)
                handle.resume();
        };
    };

    void set(Args... args)
    {
        results = ResultType(std::move(args)...);
    }

    std::atomic<bool> isWaiting = false;
    std::atomic<bool> isComplete = false;
    std::coroutine_handle<> handle;

    // Always suspend
    bool await_ready() const noexcept { return !isComplete; }

    void await_suspend(std::coroutine_handle<> handle) noexcept
    {
        this->handle = std::move(handle);
        isWaiting = true;
    }

    ResultType await_resume() noexcept
    {
        return std::move(results);
    }
};

}
