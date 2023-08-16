#pragma once

#include <fluffycoin/svc/RequestScene.h>

#include <fluffycoin/pb/Catalog.h>

#include <boost/asio/awaitable.hpp>

#include <google/protobuf/message.h>

#include <functional>

namespace fluffycoin::svc
{

/**
 * Holds a map of API request types (by protobuf request class)
 * to the function that handles the API request. This is initalized
 * by microservices on startup for the API requests they handle.
 */
class ApiHandlerMap
{
    public:
        using AwaitResponse = boost::asio::awaitable<
            std::unique_ptr<google::protobuf::Message>>;

        using Handler = std::function<AwaitResponse(svc::RequestScene &)>;

        const Handler *get(size_t reqType) const;

        template<typename Request>
        void add(std::function<AwaitResponse(svc::RequestScene &, Request &)> reqHandler)
        {
            pb::Catalog::registerMsg<Request>();
            add(
                typeid(Request).hash_code(),
                [reqHandler]
                (svc::RequestScene &scene) -> AwaitResponse
                {
                    auto rsp = co_await reqHandler(scene, scene.req<Request>());
                    co_return rsp;
                });
        }

    private:
        void add(size_t reqType, Handler handler);

        std::map<size_t, Handler> handlers;
};

}
