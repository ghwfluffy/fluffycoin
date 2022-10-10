#pragma once

#include <memory>

#define FLUFFYCOIN_DEFINE_UPTR(x) \
    struct fc_ssl_ptr_free##x \
    { \
        void operator()(x *pt) \
        { \
            x##_free(pt); \
        } \
    }; \
    typedef std::unique_ptr<x, fc_ssl_ptr_free##x> x##_uptr;

