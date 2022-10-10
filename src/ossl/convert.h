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
    template<typename BlockType> \
    static void to##Type##Stack(asn1::stack_st_##Type &st, const std::list<BlockType> &objs) \
    { \
        asn1::sk_##Type##_pop_free(&(st), asn1::Type##_free); \
        for (const BlockType &cur : (objs)) \
        { \
            asn1::Type *pt = asn1::Type##_new(); \
            cur.toASN1(*pt); \
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
            cur.fromASN1(*pt); \
            objs.emplace_back(std::move(cur)); \
        } \
    }
}

}
