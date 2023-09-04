project(fluffycoin-curator)

fluffyservice(
    curator
    PRIVATE
        fluffycoin-log
        fluffycoin-utils
        fluffycoin-block
        fluffycoin-alg
        fluffycoin-db
        fluffycoin-svc
    NAMESPACES
        fs
)
