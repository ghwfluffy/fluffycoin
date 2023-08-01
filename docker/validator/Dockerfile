FROM alpine:3.18
LABEL maintainer="GhwFluffy <ghw@ghwfluffy.com>"
LABEL description="Fluffycoin validator node"
ENV LANG="en_US.utf8"
ENV LC_ALL="en_US.utf8"
ENV LANGUAGE="en_US:en"

# Fluffycoin dependencies
RUN set -eux; \
    apk add --no-cache \
        openssl \
        ; \
    true

# Binaries
COPY ./dist/bin/fluffycoin-genesis /usr/bin/
COPY ./dist/lib/* /usr/lib/

# Entrypoint
ENTRYPOINT /usr/bin/fluffycoin-genesis
