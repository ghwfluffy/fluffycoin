#include <fluffycoin/alg/Wallet.h>
#include <fluffycoin/alg/Address.h>
#include <fluffycoin/ossl/Curve25519.h>
#include <fluffycoin/ossl/PbkdfKey.h>
#include <fluffycoin/ossl/Pkcs8Key.h>
#include <fluffycoin/ossl/Prng.h>
#include <fluffycoin/utils/Base64.h>
#include <fluffycoin/log/Log.h>

#include <nlohmann/json.hpp>

#include <chrono>

using namespace fluffycoin;
using namespace fluffycoin::alg;

namespace
{

constexpr const unsigned int VERSION = 1;

ossl::EvpPkeyPtr decrypt(
    Wallet::EncFormat format,
    const std::string &keyString,
    const SafeData &password)
{
    // Decode key
    SafeData originalBin = Base64::decode(keyString);

    // Decrypt key
    ossl::EvpPkeyPtr key;
    switch (format)
    {
        case Wallet::EncFormat::Clear:
            key = ossl::Curve25519::fromPrivate(originalBin);
            break;
        case Wallet::EncFormat::FcPbkdf:
            key = ossl::PbkdfKey::unwrap(originalBin, password);
            break;
        case Wallet::EncFormat::Pkcs8:
            key = ossl::Pkcs8Key::unwrap(originalBin, password);
            break;
        default:
        case Wallet::EncFormat::None:
            break;
    }

    return key;
}

std::string to_string(Wallet::EncFormat format)
{
    switch (format)
    {
        case Wallet::EncFormat::Clear:
            return "clear";
        case Wallet::EncFormat::FcPbkdf:
            return "fcpbkdf";
        case Wallet::EncFormat::Pkcs8:
            return "pkcs8";
        default:
        case Wallet::EncFormat::None:
            break;
    }

    return "none";
}

void from_string(const std::string &str, Wallet::EncFormat &format)
{
    format = Wallet::EncFormat::None;
    if (str == "clear")
        format = Wallet::EncFormat::Clear;
    else if (str == "pkcs8")
        format = Wallet::EncFormat::Pkcs8;
    else if (str == "fcpbkdf")
        format = Wallet::EncFormat::FcPbkdf;
}

bool retranslate(
    Wallet::EncFormat format,
    Wallet::EncFormat newFormat,
    unsigned int kdfIters,
    const std::string &original,
    std::string &translated,
    const SafeData &password,
    const SafeData &newPassword)
{
    bool bRet = true;

    // Decrypt key
    ossl::EvpPkeyPtr key = decrypt(format, original, password);
    if (!key)
        bRet = false;

    // Re-encrypt
    BinData newEnc;
    if (bRet)
    {
        switch (newFormat)
        {
            case Wallet::EncFormat::FcPbkdf:
                newEnc = ossl::PbkdfKey::wrap(*key, newPassword, kdfIters);
                break;
            case Wallet::EncFormat::Pkcs8:
                newEnc = ossl::Pkcs8Key::wrap(*key, newPassword, kdfIters);
                break;
            default:
            case Wallet::EncFormat::None:
            case Wallet::EncFormat::Clear:
                break;
        }

        bRet = !newEnc.empty();
    }

    // Output
    if (bRet)
    {
        translated = Base64::encode(newEnc);
        bRet = !translated.empty();
    }

    return bRet;
}

}

Wallet::Wallet()
{
    defaultFormat = EncFormat::FcPbkdf;
    kdfIters = ossl::PbkdfKey::KDF_ITERS_DEFAULT;
}

void Wallet::setEncFormat(EncFormat format, unsigned int iters)
{
    this->kdfIters = iters;
    this->defaultFormat = format;
}

ossl::EvpPkeyPtr Wallet::getLatestKey() const
{
    ossl::EvpPkeyPtr key;
    if (!keys.empty())
    {
        const Entry &entry = keys.front();
        key = decrypt(entry.format, entry.privKey, this->password);
    }

    return key;
}

std::string Wallet::getLatestAddress() const
{
    std::string ret;
    if (!keys.empty())
        ret = keys.front().address;
    return ret;
}

BinData Wallet::getLatestAddressBin() const
{
    return Address::unprintable(getLatestAddress());
}

BinData Wallet::getLatestPublicKey() const
{
    ossl::EvpPkeyPtr key = getLatestKey();
    return key ? ossl::Curve25519::toPublic(*key) : BinData();
}

ossl::EvpPkeyPtr Wallet::makeNewKey(const std::string &addressSalt)
{
    ossl::EvpPkeyPtr key = ossl::Curve25519::generate();
    if (key)
    {
        std::string address;
        if (addressSalt.empty())
            address = Address::printable(Address::generate(*key));
        else
            address = Address::printable(Address::generate(*key, addressSalt));
        addKey(*key, address);
    }

    return key;
}

const std::list<Wallet::Entry> &Wallet::getKeys() const
{
    return keys;
}

bool Wallet::setString(const std::string &value, const SafeData &password)
{
    keys.clear();
    this->password = password;

    bool success = false;
    try {
        nlohmann::ordered_json json = nlohmann::ordered_json::parse(value);

        // Pull out configurations
        const nlohmann::json &config = json["config"];
        unsigned int version = config["version"].get<unsigned int>();
        if (version > VERSION)
            log::error("Unsupported wallet version {}.", version);

        kdfIters = config["iters"].get<unsigned int>();
        ::from_string(config["defaultFormat"].get<std::string>(), defaultFormat);

        // Pull out keys
        bool bPwVerify = false;
        unsigned int entryNum = 0;
        for (auto &key : json["keys"])
        {
            std::string address;
            try {
                address.clear();
                entryNum++;

                // Decode JSON elements
                nlohmann::json jsonEntry = key;
                address = jsonEntry["address"].get<std::string>();
                std::string privKey = jsonEntry["priv"].get<std::string>();
                EncFormat format = EncFormat::None;
                ::from_string(jsonEntry["format"].get<std::string>(), format);
                uint64_t time = jsonEntry["time"].get<uint64_t>();
                if (format == EncFormat::None)
                    throw std::runtime_error("Invalid encryption format");

                // Just verify first entry against password
                if (!bPwVerify)
                {
                    ossl::EvpPkeyPtr key = decrypt(format, privKey, password);
                    if (!key)
                        throw std::runtime_error("Invalid password.");
                    bPwVerify = true;
                }

                // Save entry
                Entry entry;
                entry.time = time;
                entry.format = format;
                entry.address = std::move(address);
                entry.privKey = std::move(privKey);
                keys.push_back(std::move(entry));
            } catch (const std::exception &e) {
                throw std::runtime_error(
                    fmt::format("Failed to decode wallet entry {} address '{}': {}",
                    entryNum,
                    address,
                    e.what()));
            }
        }

        success = true;
    } catch (const std::exception &e) {
        log::error("Failed to decode wallet JSON: {}", e.what());
    }

    return success;
}

std::string Wallet::getString(const SafeData &password) const
{
    std::string ret;
    try {
        // Set configurations
        nlohmann::ordered_json json;
        nlohmann::ordered_json config;
        config["version"] = VERSION;
        config["iters"] = kdfIters;
        config["defaultFormat"] = ::to_string(defaultFormat);
        json["config"] = std::move(config);

        // Serialize each key
        std::list<nlohmann::json> entries;
        for (const Entry &entry : keys)
        {
            nlohmann::json json;
            json["address"] = entry.address;
            json["time"] = entry.time;
            // Export in same format
            if (password.empty() || password == this->password)
            {
                json["priv"] = entry.privKey;
                json["format"] = ::to_string(entry.format);
            }
            else
            {
                // Retranslate to new password
                std::string translated;
                if (!retranslate(entry.format, defaultFormat, kdfIters, entry.privKey, translated, this->password, password))
                {
                    throw std::runtime_error(
                        fmt::format("Failed to retranslate key {} under new password.",
                        entry.address));
                }

                json["priv"] = std::move(translated);
                json["format"] = ::to_string(defaultFormat);
            }

            entries.push_back(std::move(json));
        }

        // Set keys
        json["keys"] = std::move(entries);
        // Serialize
        ret = json.dump();
    } catch (const std::exception &e) {
        log::error("Failed to encode wallet JSON: {}", e.what());
    }

    return ret;
}

bool Wallet::addKey(const EVP_PKEY &key, const std::string &address)
{
    bool ok = true;

    if (!Address::verify(address, key))
        ok = log::error("Failed to add key to wallet. Address mismatch.");

    if (ok)
    {
        Entry newEntry;
        newEntry.time = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());

        // Assign temporary random password
        if (password.empty())
            password = ossl::Prng::rand(16);

        newEntry.address = address;
        switch (defaultFormat)
        {
            case Wallet::EncFormat::Clear:
                newEntry.format = EncFormat::Clear;
                newEntry.privKey = Base64::encode(ossl::Curve25519::toPrivate(key));
                break;
            case Wallet::EncFormat::Pkcs8:
                newEntry.format = EncFormat::Pkcs8;
                newEntry.privKey = Base64::encode(ossl::Pkcs8Key::wrap(key, password, kdfIters));
                break;
            default:
            case Wallet::EncFormat::None:
            case Wallet::EncFormat::FcPbkdf:
                newEntry.format = EncFormat::FcPbkdf;
                newEntry.privKey = Base64::encode(ossl::PbkdfKey::wrap(key, password, kdfIters));
                break;
        }

        // Success?
        if (newEntry.privKey.empty())
            ok = log::error("Failed to password encrypt wallet ket.");

        // Add to wallet
        if (ok)
            keys.push_front(std::move(newEntry));
    }

    return ok;
}
