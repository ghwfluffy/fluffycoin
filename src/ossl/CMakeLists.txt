project(fluffycoin-ossl)

fluffylib(
    ossl
    PUBLIC
        fluffycoin-log
        fluffycoin-utils
        OpenSSL::Crypto
)
