#pragma once

#include <fluffycoin/svc/RequestScene.h>
#include <fluffycoin/svc/ApiResponseCallback.h>

#include <fcpb/p2p/v1/auth/AuthenticateSession.pb.h>

#include <boost/asio/awaitable.hpp>

namespace fluffycoin::p2p::api::server
{

/**
 * Peer handshake to establish a mutually authenticated connection
 */
namespace Handshake
{
    boost::asio::awaitable<void> process(
        svc::RequestScene &scene,
        fcpb::p2p::v1::auth::AuthenticateSession &handshake,
        svc::ApiResponseCallback callback);
}

}
