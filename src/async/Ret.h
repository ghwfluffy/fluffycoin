#pragma once

#include <boost/asio/awaitable.hpp>

#include <memory>

namespace fluffycoin::async
{

template<typename T>
using Ret = boost::asio::awaitable<T>;

template<typename T>
using RetPtr = boost::asio::awaitable<std::unique_ptr<T>>;

}
