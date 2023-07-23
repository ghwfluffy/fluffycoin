#pragma once

#include <fluffycoin/svc/ServiceScene.h>
#include <fluffycoin/svc/ApiHandlerMap.h>
#include <fluffycoin/svc/IAuthenticator.h>

#include <fluffycoin/async/UncontrolledSocket.h>
#include <fluffycoin/zmq/Server.h>

#include <atomic>

#include <stdint.h>

namespace fluffycoin::svc
{

/**
 * Opens the API port and listens and handled requests.
 */
class ApiPort
{
    public:
        ApiPort(
            const ServiceScene &ctx,
            const zmq::Context &zmqCtx,
            const ApiHandlerMap &handlers);
        ApiPort(ApiPort &&) = delete;
        ApiPort(const ApiPort &) = delete;
        ApiPort &operator=(ApiPort &&) = delete;
        ApiPort &operator=(const ApiPort &) = delete;
        ~ApiPort() = default;

        void setAuthenticator(
            IAuthenticator *authenticator);

        bool openPort(
            uint16_t port,
            const BinData &serverKey = BinData());

    private:
        void read(
            Details &details);

        void handleRequest(
            const zmq::ClientId &client,
            const BinData &msgData);

        void handleResponse(
            RequestScene &scene,
            const zmq::ClientId &client,
            std::unique_ptr<google::protobuf::Message> msg);

        const ServiceScene &ctx;
        const ApiHandlerMap &handlers;
        async::UncontrolledSocket socket;
        IAuthenticator *authenticator;

        zmq::Server server;

        std::atomic<uint64_t> requestId;
};

}
