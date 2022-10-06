project(fluffycoin-block)

fluffylib(
    block
    PUBLIC
        fluffycoin-log
        fluffycoin-utils
        OpenSSL::Crypto
)
