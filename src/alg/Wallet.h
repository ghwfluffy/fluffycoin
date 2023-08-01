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
        Wallet() = default;
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
            Pkcs8
        };

        struct Entry
        {
            uint64_t time = 0;
            EncFormat format = EncFormat::None;
            std::string address;
            std::string privKey;
        };

        ossl::EvpPkeyPtr getLatestKey() const;
        std::string getLatestAddress() const;
        BinData getLatestAddressBin() const;
        BinData getLatestPublicKey() const;

        ossl::EvpPkeyPtr getKey(const std::string &address);

        ossl::EvpPkeyPtr makeNewKey(const std::string &addressSalt = std::string());

        // Front=Newest
        const std::list<Entry> &getKeys() const;

        bool setString(const std::string &value, const SafeData &password);
        std::string getString(const SafeData &password = SafeData()) const;

        void addKey(const EVP_PKEY &key, const std::string &address);

    private:
        std::list<Entry> keys;
        SafeData password;
};

}
