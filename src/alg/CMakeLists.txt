project(fluffycoin-alg)

fluffylib(
    alg
    PUBLIC
        fluffycoin-log
        fluffycoin-utils
        fluffycoin-ossl
        OpenSSL::Crypto
)
