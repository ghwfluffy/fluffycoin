FROM alpine:3.18
LABEL maintainer="GhwFluffy <ghw@ghwfluffy.com>"
LABEL description="Fluffycoin curator"
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
        postgresql15-client \
        ; \
    true

# Runtime environment
RUN set -eux; \
    mkdir -p /var/log/fluffycoin; \
    mkdir -p /usr/share/fluffycoin/sql; \
    true

# Binaries
COPY ./dist/bin/fluffycoin-curator /usr/bin/
COPY ./dist/lib/* /usr/lib/

# Schema
COPY ./doc/schema.sql /usr/share/fluffycoin/sql/

# Persistent data
VOLUME /var/log/fluffycoin

# Entrypoint
COPY entrypoint /
ENTRYPOINT ["/entrypoint"]
