#pragma once

#include <fluffycoin/ossl/EvpPkeyPtr.h>
#include <fluffycoin/utils/SafeData.h>

#include <list>
#include <string>

namespace fluffycoin::alg
{

/**
 * Multiple private keys that make up a wallet
 */
class Wallet
{
    public:
        Wallet();
        Wallet(Wallet &&) = default;
        Wallet(const Wallet &) = default;
        Wallet &operator=(Wallet &&) = default;
        Wallet &operator=(const Wallet &) = default;
        ~Wallet() = default;

        enum class EncFormat
        {
            None = 0,
            Clear,
            FcPbkdf,
            Pkcs8,
        };

        enum class KeyUsage
        {
            None = 0,
            Specie,
            Validator,
        };

        struct Entry
        {
            uint64_t time = 0;
            KeyUsage usage = KeyUsage::None;
            EncFormat format = EncFormat::None;
            std::string address;
            std::string privKey;
        };

        void setEncFormat(EncFormat format, unsigned int iters);

        ossl::EvpPkeyPtr getLatestKey(KeyUsage usage) const;
        std::string getLatestAddress(KeyUsage usage) const;
        BinData getLatestAddressBin(KeyUsage usage) const;
        BinData getLatestPublicKey(KeyUsage usage) const;

        ossl::EvpPkeyPtr getKey(const std::string &address);

        ossl::EvpPkeyPtr makeNewKey(KeyUsage usage, const std::string &addressSalt = std::string());

        // Front=Newest
        const std::list<Entry> &getKeys() const;

        bool setString(const std::string &value, const SafeData &password);
        std::string getString(const SafeData &password = SafeData()) const;

        bool addKey(KeyUsage usage, const EVP_PKEY &key, const std::string &address);

    private:
        std::list<Entry> keys;
        SafeData password;
        EncFormat defaultFormat;
        unsigned int kdfIters;
};

}
