#include <fluffycoin/services/validator/api/server/Handshake.h>

#include <fluffycoin/utils/BinData.h>

using namespace fluffycoin;
using namespace fluffycoin::validator;
using namespace fluffycoin::validator::api;
using namespace fluffycoin::validator::api::server;

void Handshake::process(
        svc::RequestScene &scene,
        fcpb::p2p::v1::auth::AuthenticateSession &handshake,
        svc::ApiResponseCallback callback)
{
    (void)scene;
    (void)callback;

    BinData clientNonce = handshake.client_session_id();
    std::string clientAddress = handshake.auth_address();

    // TODO
    // Check against a brute force list if we trust this address
    // Get the peer address/port from the socket
    // Check against a brute force list if we trust this address/port
    // Lookup the information stub about the peer from their address
    // Generate a server nonce
    // Perform ECDH to create the secret HMAC key
    // Save to our auth map with expiration
    // Send back the server nonce and our public key

    // TODO: Remove server auth from responses, server auth is all based on CurveMQ
#if 0
    // TODO: Should server auth all be based on CurveZMQ
    // TODO: If so, then don't send back server key, and don't set auth on server responses
    // TODO: Also do we really need the client to auth every message then, or just the handshake
    // TODO: You should be able to get a TCP disconnect and resume on the same handshake/clientid?

    // Option A:
    // Validators: peer p2p all uses curveMQ
    // client signs all their messages to verify who they are against their handshaked session
    // client already knows server's key, they got it from peers or they got it from the registry

    // Kiosks: These accept clear messages. Same as registry?

    // Registry: This is just an HTTP/S server who exposes known peer information
#endif
}
