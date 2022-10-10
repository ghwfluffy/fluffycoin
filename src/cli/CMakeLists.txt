project(fluffycoin-cli)

fluffybin(
    genesis
    PRIVATE
        fluffycoin-log
        fluffycoin-utils
        fluffycoin-ossl
        fluffycoin-block
        OpenSSL::Crypto
)
