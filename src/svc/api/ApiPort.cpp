#include <fluffycoin/svc/ApiPort.h>
#include <fluffycoin/svc/Log.h>

#include <fluffycoin/pb/Json.h>
#include <fluffycoin/pb/Catalog.h>
#include <fluffycoin/pb/MsgInfo.h>
#include <fluffycoin/pb/AnyMessage.h>

#include <fcpb/comm/Request.pb.h>
#include <fcpb/comm/Response.pb.h>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>

using namespace fluffycoin;
using namespace fluffycoin::svc;

ApiPort::ApiPort(
    const ServiceScene &ctx,
    const zmq::Context &zmqCtx,
    const ApiHandlerMap &handlers)
        : ctx(ctx)
        , handlers(handlers)
        , server(zmqCtx)
{
    authenticator = nullptr;
    requestId = 0;
}

void ApiPort::setAuthenticator(
    IAuthenticator *authenticator)
{
    this->authenticator = authenticator;
}

bool ApiPort::openPort(
    uint16_t port,
    const BinData &serverKey)
{
    // Bind port
    Details details;
    server.bind(std::string(), port, serverKey, details);

    // Give to boost
    if (details.isOk())
    {
        int fd = server.getFd();
        socket = async::UncontrolledSocket(ctx.asio, fd);
        socket.read(details, std::bind(&ApiPort::read, this, std::placeholders::_1));
    }

    return details.isOk();
}

void ApiPort::read(
    Details &details)
{
    // Something low level went wrong
    // This is gonna be fatal
    if (!details.isOk())
    {
        details.setError(Log::Api, ErrorCode::SocketError, "api_port_read",
            "Received error when reading on API port.");
        return;
    }

    // Try to read in a message
    zmq::ClientId clientId;
    BinData message;
    bool bHasMessage = server.recv(clientId, message, details);
    if (!details.isOk())
    {
        details.setError(Log::Api, ErrorCode::ReadError, "api_port_read",
            "Received error when reading message on API port.");
        return;
    }

    // Have a message, process it
    if (bHasMessage)
        handleRequest(clientId, message);

    // Queue next read
    socket.read(details, std::bind(&ApiPort::read, this, std::placeholders::_1));
}

void ApiPort::handleRequest(
    const zmq::ClientId &client,
    const BinData &msgData)
{
    // Setup the request scene
    std::unique_ptr<RequestScene> pscene = std::make_unique<RequestScene>(ctx);
    RequestScene &scene = *pscene;

    // Assign a request ID
    scene.log().addContext("request_id", ++requestId);

    // Interpret google protobuf
    std::unique_ptr<fcpb::comm::Request> envelope =
        std::make_unique<fcpb::comm::Request>();
    pb::Json::fromJson(msgData.data(), msgData.length(), *envelope, scene.details());

    // Pull out request message from envelope
    std::unique_ptr<google::protobuf::Message> request;
    if (scene.isOk())
    {
        pb::AnyMessage::fromAny(envelope->body(), request, scene.details());
        // Log
        if (scene.isOk())
            scene.log().traffic(Log::Api, "Received request '{}'.", pb::MsgInfo::getType(*request));
    }

    // Verify authentication
    if (scene.isOk())
    {
        // Perform auth
        if (envelope->has_auth() && authenticator)
            authenticator->authenticateScene(scene, envelope->auth());
        // Let authenticator decide if we need auth for this request
        else if (authenticator)
            authenticator->noAuth(scene);
        // Why is there an auth without an authenticator
        else if (envelope->has_auth())
            scene.log().debug(Log::Api, "Received message with authentication parameters but API port has no authenticator.");
    }

    // Find the appropriate request handler
    const ApiHandlerMap::Handler *handler = nullptr;
    if (scene.isOk())
    {
        size_t reqType = pb::Catalog::getTypeId(*request);
        handler = handlers.get(reqType);
        if (!handler)
        {
            scene.setError(
                ErrorCode::UnknownCommand, "request_type",
                "Unknown request type '%s'.",
                pb::MsgInfo::getType(*request).c_str());
        }
    }
    // Add request to scene
    scene.setRequest(std::move(request));

    // Do not process, respond error
    if (!scene.isOk())
    {
        handleResponse(scene, client, std::unique_ptr<google::protobuf::Message>());
        return;
    }

    // Save memory
    ctx.trove.add(std::move(pscene));

    // Handle request as coroutine
    boost::asio::co_spawn(
        ctx.asio,
        (*handler)(scene, ApiResponseCallback(
            // Programmer will call this when processing is complete
            [this, client, &scene](std::unique_ptr<google::protobuf::Message> msg) -> void
            {
                handleResponse(scene, client, std::move(msg));
            },
            // If this gets triggered, programmer fked up and some code path leads to a dead end
            [this, client, &scene]() -> void
            {
                if (!scene.isOk())
                {
                    scene.setError(
                        ErrorCode::InternalError, "no_response",
                        "Request handling ended prematurely.");
                }
                handleResponse(scene, client, std::unique_ptr<google::protobuf::Message>());
            })
        ),
        boost::asio::detached);
}

void ApiPort::handleResponse(
    RequestScene &scene,
    const zmq::ClientId &client,
    std::unique_ptr<google::protobuf::Message> msg)
{
    fcpb::comm::Response rsp;
    scene.log().traffic(Log::Api, "Responding to request '{}' with {}.",
        pb::MsgInfo::getType(scene.req<google::protobuf::Message>()),
        msg ? pb::MsgInfo::getType(*msg) : std::string("null"));

    // Serialize message body (if any)
    if (scene.isOk() && msg)
        pb::AnyMessage::toAny(*msg, *rsp.mutable_body());

    // Serialize authentication response
    if (authenticator)
        authenticator->setResponseAuth(scene, *rsp.mutable_auth());

    // Serialize error state
    if (scene.isOk())
        rsp.set_code(static_cast<int>(ErrorCode::None));
    else
    {
        const std::string &errorField = scene.details().error().getField();
        if (!errorField.empty())
            rsp.set_error_field(errorField);
        const std::string &errorMsg = scene.details().error().getStr();
        if (!errorMsg.empty())
            rsp.set_error_string(errorMsg);
        rsp.set_code(static_cast<int>(scene.details().error().getCode()));
    }

    // Serialize response
    Details replyDetails;
    replyDetails.log() = scene.log();
    std::string rspJson;
    pb::Json::toJson(rsp, rspJson, replyDetails);

    // Respond over API
    server.reply(client, rspJson, replyDetails);

    // Done with memory
    ctx.trove.cleanup(scene);
}
