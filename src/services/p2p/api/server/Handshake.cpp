#include <fluffycoin/p2p/api/server/Handshake.h>

#include <fluffycoin/p2p/StakeKey.h>
#include <fluffycoin/p2p/BruteForce.h>
#include <fluffycoin/p2p/PeerSessions.h>
#include <fluffycoin/p2p/ValidatorLookup.h>

#include <fluffycoin/svc/Log.h>

#include <fluffycoin/alg/PeerAuth.h>
#include <fluffycoin/alg/P2pProtocol.h>

#include <fluffycoin/ossl/Prng.h>

using namespace fluffycoin;
using namespace fluffycoin::p2p;
using namespace fluffycoin::p2p::api;
using namespace fluffycoin::p2p::api::server;

async::RetPtr<google::protobuf::Message> Handshake::process(
        svc::RequestScene &scene,
        fcpb::p2p::v1::auth::AuthenticateSession &handshake)
{
    // Check address against brute force map
    std::string clientAddress = handshake.auth_address();
    BruteForce &bforce = scene.utils().get<BruteForce>();
    if (!bforce.checkAddress(clientAddress))
    {
        scene.setError(log::Auth, ErrorCode::Blocked, "brute_force",
            "Refusing address '{}' due to brute force check.",
            clientAddress);
        co_return std::unique_ptr<google::protobuf::Message>();
    }

    // Sane validate supported protocol version
    int version = handshake.version();
    if (version <= 0)
    {
        scene.setError(svc::Log::Api, ErrorCode::ArgumentInvalid, "version",
            "Invalid protocol version {}.", version);
        bforce.addOffense(clientAddress);
        co_return std::unique_ptr<google::protobuf::Message>();
    }
    // Log if peers are using a newer protocol version
    else if (static_cast<unsigned int>(version) > alg::P2pProtocol::VERSION)
    {
        scene.log().traffic(svc::Log::Api, "Peer supports newer protocol version {}.", version);
    }

    // Sane validate nonce length
    BinData clientNonce = handshake.client_session_id();
    if (clientNonce.length() != alg::PeerAuth::AUTH_NONCE_LENGTH)
    {
        scene.setError(log::Auth, ErrorCode::ArgumentInvalid, "client_session_id",
            "Invalid client session ID length.");
        bforce.addOffense(clientAddress);
        co_return std::unique_ptr<google::protobuf::Message>();
    }

    // Lookup p2p info of client
    ValidatorInfo info = scene.utils().get<ValidatorLookup>().getValidator(clientAddress, scene.details());
    if (!scene.details().isOk())
        co_return std::unique_ptr<google::protobuf::Message>();

    // This p2p has coins staked?
    if (!info.isActive())
    {
        scene.setError(log::Auth, ErrorCode::NotAuthorized, "auth_address",
            "Validator '{}' is not currently active.", clientAddress);
        bforce.addOffense(clientAddress);
        co_return std::unique_ptr<google::protobuf::Message>();
    }

    // Generate a server nonce
    BinData serverNonce = ossl::Prng::rand(alg::PeerAuth::AUTH_NONCE_LENGTH);
    // Combine nonces to create HMAC key
    SafeData hmacKey = alg::PeerAuth::createMessageAuthKey(
        *scene.utils().get<StakeKey>().getKey(),
        info.getPubKey(),
        clientNonce,
        serverNonce);
    if (hmacKey.empty())
    {
        scene.setError(log::Auth, ErrorCode::InternalError, "derive",
            "Failed to derive message authentication key.");
        co_return std::unique_ptr<google::protobuf::Message>();
    }

    // Combine nonces to make session identifier
    std::string sessionId = alg::PeerAuth::createSessionId(clientNonce, serverNonce);

    // Log
    scene.log().traffic(log::Auth, "Started authentication session '{}' with peer '{}'.",
        sessionId, clientAddress);

    // Save the session info to verify later requests
    scene.utils().get<PeerSessions>().addSession(
        static_cast<unsigned int>(version),
        std::move(clientAddress),
        std::move(sessionId),
        std::move(hmacKey));

    // Send back the server nonce
    // They already have our public key
    auto rsp = std::make_unique<fcpb::p2p::v1::auth::AuthenticateSessionResponse>();
    rsp->set_version(static_cast<int>(alg::P2pProtocol::VERSION));
    rsp->set_server_session_id(serverNonce.data(), serverNonce.length());

    co_return rsp;
}
