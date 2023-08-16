#pragma once

#include <fluffycoin/svc/RequestScene.h>

#include <fcpb/p2p/v1/auth/AuthenticateSession.pb.h>

#include <boost/asio/awaitable.hpp>

namespace fluffycoin::p2p::api::server
{

/**
 * Peer handshake to establish a mutually authenticated connection
 */
namespace Handshake
{
    boost::asio::awaitable<std::unique_ptr<google::protobuf::Message>> process(
        svc::RequestScene &scene,
        fcpb::p2p::v1::auth::AuthenticateSession &handshake);
}

}
