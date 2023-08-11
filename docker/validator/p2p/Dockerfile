FROM alpine:3.18
LABEL maintainer="GhwFluffy <ghw@ghwfluffy.com>"
LABEL description="Fluffycoin p2p"
ENV LANG="en_US.utf8"
ENV LC_ALL="en_US.utf8"
ENV LANGUAGE="en_US:en"

# Fluffycoin dependencies
RUN set -eux; \
    apk add --no-cache \
        fmt \
        libgcc \
        libstdc++ \
        openssl \
        ; \
    true

# Runtime environment
RUN set -eux; \
    mkdir -p /var/log/fluffycoin

# Binaries
COPY ./dist/bin/fluffycoin-p2p /usr/bin/
COPY ./dist/lib/* /usr/lib/

# Entrypoint
ENTRYPOINT ["/usr/bin/fluffycoin-p2p"]
