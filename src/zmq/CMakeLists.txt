project(fluffycoin-zmq)

fluffylib(
    zmq
    PUBLIC
        fluffycoin-log
        fluffycoin-utils
        fluffycoin-ossl
        libsodium
        libzmq
)
