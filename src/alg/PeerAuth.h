#pragma once

#include <fluffycoin/utils/SafeData.h>

#include <openssl/evp.h>

#include <string>

namespace fluffycoin::alg
{

/**
 * Algorithms for authenticating peer-to-peer messages
 */
namespace PeerAuth
{
    constexpr const size_t AUTH_NONCE_LENGTH = 8;

    SafeData createMessageAuthKey(
        const EVP_PKEY &priv,
        const BinData &pub,
        const BinData &clientNonce,
        const BinData &serverNonce);

    std::string createSessionId(
        const BinData &clientNonce,
        const BinData &serverNonce);
}

}
