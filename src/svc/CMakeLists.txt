project(fluffycoin-svc)

fluffylib(
    svc
    PUBLIC
        fluffycoin-log
        fluffycoin-utils
        fluffycoin-ossl
        fluffycoin-zmq
        fluffycoin-async
        fluffycoin-pb
        fluffyproto-comm
)
