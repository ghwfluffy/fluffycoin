#pragma once

#include <memory>

#define FC_OSSL_UPTR_DELETER(x) \
    namespace fluffycoin \
    { \
    namespace ossl \
    { \
        struct fc_ssl_ptr_free##x \
        { \
            void operator()(x *pt) \
            { \
                x##_free(pt); \
            } \
        }; \
    } \
    }

#define FC_OSSL_UPTR(x) \
    FC_OSSL_UPTR_DELETER(x) \
    namespace fluffycoin \
    { \
    namespace ossl \
    { \
        typedef std::unique_ptr<x, fluffycoin::ossl::fc_ssl_ptr_free##x> x##_uptr; \
    } \
    }
