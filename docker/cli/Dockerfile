FROM alpine:3.18
LABEL maintainer="GhwFluffy <ghw@ghwfluffy.com>"
LABEL description="Fluffycoin CLI"
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
    apk add --no-cache \
        bash \
        ; \
    true

# Binaries
COPY ./dist/lib/* /usr/lib/
COPY ./dist/bin/fluffycoin-genesis /usr/bin/
COPY ./docker/cli/entrypoint.sh /usr/bin/cli-entrypoint

# Entrypoint
ENTRYPOINT /usr/bin/cli-entrypoint
