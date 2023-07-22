FROM alpine:3.18
LABEL maintainer="GhwFluffy <ghw@ghwfluffy.com>"
LABEL description="Alpine build environment for fluffycoin binaries"

# Fluffycoin build dependencies
RUN set -eux; \
    apk add --no-cache \
        bash \
        cmake \
        g++ \
        ninja-build \
        ; \
    true

# Fluffycoin dependencies build dependencies
RUN set -eux; \
    apk add --no-cache \
        autoconf \
        automake \
        libtool \
        linux-headers \
        make \
        patch \
        ; \
    true

# Build environment
RUN set -eux; \
    ln -s /usr/lib/ninja-build/bin/ninja /usr/bin/ninja; \
    true

# Fluffycoin dependencies
RUN set -eux; \
    apk add --no-cache \
        fmt-dev \
        nlohmann-json \
        openssl-dev \
        ; \
    true
