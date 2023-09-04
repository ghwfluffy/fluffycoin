#pragma once

#include <fluffycoin/utils/BinData.h>

#include <openssl/asn1.h>

#include <list>
#include <string>

#include <stdint.h>

namespace fluffycoin
{

namespace ossl
{

BinData toBin(const ASN1_STRING &t);
void fromBin(ASN1_STRING &t, const BinData &data);

std::string toString(const ASN1_STRING &t);
void fromString(ASN1_STRING &t, const std::string &str);

uint32_t toUInt32(const ASN1_INTEGER &t);
void fromUInt32(ASN1_INTEGER &t, uint32_t ui);

uint64_t toUInt64(const ASN1_INTEGER &t);
void fromUInt64(ASN1_INTEGER &t, uint64_t ui);

int toNid(const ASN1_OBJECT &t);
void fromNid(ASN1_OBJECT *&pt, int nid);

#define FLUFFYCOIN_DEFINE_STACK_FUNCTIONS(Type) \
    SKM_DEFINE_STACK_OF(Type, Type, Type) \
 \
    template<typename BlockType> \
    static void to##Type##Stack(asn1::stack_st_##Type &st, const std::list<BlockType> &objs) \
    { \
        while (asn1::sk_##Type##_num(&(st)) > 0) \
            asn1::Type##_free(sk_##Type##_pop(&st)); \
        for (const BlockType &cur : (objs)) \
        { \
            asn1::Type *pt = asn1::Type##_new(); \
            cur.toAsn1(*pt); \
            asn1::sk_##Type##_push(&(st), pt); \
        } \
    } \
 \
    template<typename BlockType> \
    static void from##Type##Stack(std::list<BlockType> &objs, const asn1::stack_st_##Type &st) \
    { \
        objs.clear(); \
        for (int i = 0; i < asn1::sk_##Type##_num(&(st)); i++) \
        { \
            asn1::Type *pt = asn1::sk_##Type##_value(&(st), i); \
            BlockType cur; \
            cur.fromAsn1(*pt); \
            objs.push_back(std::move(cur)); \
        } \
    }

#define FLUFFYCOIN_DEFINE_STACK_FUNCTIONS_VEC(Type) \
    SKM_DEFINE_STACK_OF(Type, Type, Type) \
 \
    template<typename BlockType> \
    static void to##Type##Stack(asn1::stack_st_##Type &st, const std::vector<BlockType> &objs) \
    { \
        while (asn1::sk_##Type##_num(&(st)) > 0) \
            asn1::Type##_free(sk_##Type##_pop(&st)); \
        for (const BlockType &cur : (objs)) \
        { \
            asn1::Type *pt = asn1::Type##_new(); \
            cur.toAsn1(*pt); \
            asn1::sk_##Type##_push(&(st), pt); \
        } \
    } \
 \
    template<typename BlockType> \
    static void from##Type##Stack(std::vector<BlockType> &objs, const asn1::stack_st_##Type &st) \
    { \
        objs.clear(); \
        objs.reserve(static_cast<size_t>(asn1::sk_##Type##_num(&(st)))); \
        for (int i = 0; i < asn1::sk_##Type##_num(&(st)); i++) \
        { \
            asn1::Type *pt = asn1::sk_##Type##_value(&(st), i); \
            BlockType cur; \
            cur.fromAsn1(*pt); \
            objs.push_back(std::move(cur)); \
        } \
    }
}

}
