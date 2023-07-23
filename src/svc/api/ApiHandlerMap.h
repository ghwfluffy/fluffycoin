#pragma once

#include <fluffycoin/svc/RequestScene.h>
#include <fluffycoin/svc/ApiResponseCallback.h>

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
        using Handler = std::function<void(
            svc::RequestScene &,
            svc::ApiResponseCallback)>;

        const Handler *get(size_t reqType) const;

        template<typename Request>
        void add(std::function<void(svc::RequestScene &, Request &, svc::ApiResponseCallback)> reqHandler)
        {
            add(
                typeid(Request).hash_code(),
                [reqHandler]
                (svc::RequestScene &scene, svc::ApiResponseCallback apiCallback) -> void
                {
                    reqHandler(scene, scene.req<Request>(), std::move(apiCallback));
                });
        }

    private:
        void add(size_t reqType, Handler handler);

        std::map<size_t, Handler> handlers;
};

}
