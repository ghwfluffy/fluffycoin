#pragma once

#include <fluffycoin/block/Time.h>
#include <fluffycoin/block/Address.h>
#include <fluffycoin/block/Specie.h>

#include <fluffycoin/utils/BinData.h>

#include <string>
#include <stdint.h>

#include <openssl/asn1.h>

namespace fluffycoin
{

namespace asn1
{
    typedef struct Genesis_st Genesis;
    DECLARE_ASN1_FUNCTIONS(Genesis)
}

namespace block
{

/**
 * The first block in the blockchain
 */
class Genesis
{
    public:
        Genesis();
        Genesis(Genesis &&) = default;
        Genesis(const Genesis &) = default;
        Genesis &operator=(Genesis &&) = default;
        Genesis &operator=(const Genesis &) = default;
        ~Genesis() = default;

        uint32_t getProtocol() const;
        void setProtocol(uint32_t protocol);

        const std::string &getName() const;
        void setName(std::string name);

        uint32_t getVersion() const;
        void setVersion(uint32_t version);

        const Time &getCreation() const;
        void setCreation(Time time);

        const Address &getCreator() const;
        void setCreator(Address creator);

        const Specie &getGreed() const;
        void setGreed(Specie specie);

        const BinData &getSeed() const;
        void setSeed(BinData data);

        void toASN1(asn1::Genesis &) const;
        void fromASN1(const asn1::Genesis &);

    private:
        uint32_t protocol;
        std::string name;
        uint32_t version;
        Time creation;
        Address creator;
        Specie greed;
        BinData seed;
};

}

}
