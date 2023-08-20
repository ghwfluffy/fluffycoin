#pragma once

#include <fluffycoin/async/CallbackAwaiter.h>

namespace fluffycoin::async
{

// Helper structs
namespace internal
{

template <typename T>
struct tuple_to_awaitable;

template <typename... Ts>
struct tuple_to_awaitable<std::tuple<Ts...>> {
    using type = boost::asio::awaitable<std::tuple<std::remove_const_t<std::remove_reference_t<Ts>>...>>;
};

template <typename T>
struct tuple_to_awaiter;

template <typename... Ts>
struct tuple_to_awaiter<std::tuple<Ts...>> {
    using type = CallbackAwaiter<Ts...>;
};

}

namespace woc
{

template<typename... Args>
struct args {
    using ArgType = std::tuple<Args...>;
};

}

/**
 * Allows your coroutine to await on an async callback and return the callback parameters as a tuple
 *
 * Example:
 * \code{cpp}
 *     void doAsync(float argument, std::function<void(Details &, int)> callback);
 *     std::tuple<Details, int> result = co_await waitOnCallback<woc::args<Details &, int>>(doAsync, 2.0);
 * \endcode
 *
 * The memory for any lvalue references passed to the callback will be moved to the tuple
 */
template<typename CallbackArgs, typename Func, typename... FunctionArgs>
typename internal::tuple_to_awaitable<typename CallbackArgs::ArgType>::type
    waitOnCallback(Func func, FunctionArgs&&... args)
{
    using awaiter = internal::tuple_to_awaiter<typename CallbackArgs::ArgType>::type;
    awaiter t;
    func(std::forward<FunctionArgs>(args)..., t.f());
    typename awaiter::ResultType tupVal;
    tupVal = co_await t.run();
    co_return std::move(tupVal);
}

}
