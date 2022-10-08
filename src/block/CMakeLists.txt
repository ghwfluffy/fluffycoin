project(fluffycoin-block)

fluffylib(
    block
    PUBLIC
        fluffycoin-log
        fluffycoin-utils
        fluffycoin-ossl
        OpenSSL::Crypto
)
