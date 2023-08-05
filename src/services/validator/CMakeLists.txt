project(fluffycoin-validator)

fluffyservice(
    validator
    PRIVATE
        fluffycoin-log
        fluffycoin-utils
        fluffycoin-ossl
        fluffycoin-block
        fluffycoin-alg
        fluffycoin-svc
        fluffyproto-comm
        fluffyproto-p2p-v1-auth
        OpenSSL::Crypto
    NAMESPACES
        api/client
        api/server
)
