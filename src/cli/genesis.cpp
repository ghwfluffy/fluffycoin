#include <fluffycoin/block/Genesis.h>
#include <fluffycoin/block/Reconciliation.h>
#include <fluffycoin/block/Block.h>

#include <fluffycoin/ossl/Curve25519.h>

#include <fluffycoin/alg/info.h>
#include <fluffycoin/alg/Wallet.h>

#include <fluffycoin/utils/ArgParser.h>
#include <fluffycoin/utils/FileTools.h>

#include <fluffycoin/log/Log.h>

#include <termios.h>

using namespace fluffycoin;

namespace
{

bool readPassword(
    SafeData &password,
    bool bRequireVerify)
{
    // Don't show password
    struct termios tOrig = {};
    struct termios tSilent = {};
    tcgetattr(fileno(stdin), &tOrig);
    tSilent = tOrig;
    tSilent.c_lflag &= static_cast<tcflag_t>(~ECHO);
    tcsetattr(fileno(stdin), TCSANOW, &tSilent);

    // Restore stdin flags
    struct x {
        struct termios &tOrig;
        x(struct termios &tOrig) : tOrig(tOrig) {}
        ~x() { tcsetattr(fileno(stdin), TCSANOW, &tOrig); }
    };
    auto reset = std::make_unique<x>(tOrig);

    printf("Enter wallet password: ");
    fflush(stdout);

    password.resize(256);
    char *pszPassword = reinterpret_cast<char *>(password.data());
    if (fgets(pszPassword, static_cast<int>(password.length() - 1), stdin))
    {
        password.resize(strlen(pszPassword) - 1);
        pszPassword = reinterpret_cast<char *>(password.data());
    }
    printf("\n");
    if (!password.data()[0])
    {
        log::error("User canceled password entry.");
        return false;
    }

    // Verify match
    if (bRequireVerify)
    {
        printf("Verify password: ");
        fflush(stdout);

        SafeData verifyPassword;
        verifyPassword.resize(256);
        char *pszVerify = reinterpret_cast<char *>(verifyPassword.data());
        if (fgets(pszVerify, static_cast<int>(verifyPassword.length() - 1), stdin))
        {
            verifyPassword.resize(strlen(pszVerify) - 1);
            pszVerify = reinterpret_cast<char *>(verifyPassword.data());
        }
        printf("\n");
        if (password != verifyPassword)
        {
            log::error("Password mismatch.");
            return false;
        }
    }

    return true;
}

// Return codes
namespace error
{
    int iter__ = 0;
    #define R(x) const int x = iter__++
    R(Success);
    R(ArgumentParse);
    R(ArgumentInvalid);
    R(ArgumentMissing);
    R(ReadWallet);
    R(GetPassword);
    R(DecryptWallet);
    R(MakeKey);
    R(CreateOutputDir);
    R(WriteBlock);
    R(SerializeWallet);
    R(WriteWallet);
};

}

int main(int argc, const char *argv[])
{
    log::setConsole(true);

    // Defaults
    uint64_t greed = 1000;
    std::string outputDir = "~/.fluffycoin/blocks";
    std::string walletFile = "~/.fluffycoin/wallet.json";

    // Setup argument parser
    ArgParser parser;
    parser.addCategory("Wallet");
    parser.addParam('w', "wallet", "File to read and save wallet (Default: {})", walletFile);
    parser.addParam('W', "wallet-salt", "Salt for new wallet address (Default: Random)");
    parser.addParam('p', "wallet-password", "Password for wallet (Default: Prompt)");
    parser.addParam('g', "greed", "How many coins to mint (Default: {})", greed);
    parser.addParam('s', "seed", "Seed value (Optional)");
    parser.addParam('o', "output", "Output directory (Default {})", outputDir);

    // Parse
    Args args = parser.parse(argc, argv);
    if (args.hasError())
        return error::ArgumentParse;

    // Validate greed value
    if (args.hasArg("greed"))
        greed = static_cast<uint64_t>(args.getSizeT("greed"));

    if (!greed)
    {
        fprintf(stderr, "Invalid greed.\n");
        parser.printHelp();
        return error::ArgumentInvalid;
    }

    // Read in wallet
    if (args.hasArg("wallet"))
        walletFile = args.getArg("wallet");
    std::string walletContents;
    if (FileTools::exists(walletFile))
    {
        if (!FileTools::read(walletFile, walletContents))
        {
            log::error("Failed to read in wallet data from '{}'.", walletFile);
            return error::ReadWallet;
        }

        log::info("Read in wallet data from '{}'.", walletFile);
    }

    // Get wallet password
    SafeData password;
    if (args.hasArg("wallet-password"))
        password = SafeData(args.getArg("wallet-address"));
    else if (!readPassword(password, walletContents.empty()))
    {
        log::error("Failed to read password from command line.");
        return error::GetPassword;
    }

    // Decrypt wallet
    alg::Wallet wallet;
    if (!walletContents.empty() && !wallet.setString(walletContents, password))
    {
        log::error("Failed to decrypt wallet contents.");
        return error::DecryptWallet;
    }

    // Make a new key
    if (!wallet.makeNewKey(alg::Wallet::KeyUsage::Validator, args.getArg("wallet-salt")))
    {
        log::error("Failed to create validator wallet key.");
        return error::MakeKey;
    }
    else if (!wallet.makeNewKey(alg::Wallet::KeyUsage::Specie, args.getArg("wallet-salt")))
    {
        log::error("Failed to create creator wallet key.");
        return error::MakeKey;
    }

    // Create genesis block
    block::Genesis gen;
    gen.setProtocol(alg::PROTOCOL_VERSION);
    gen.setName(alg::BLOCKCHAIN_NAME);
    gen.setVersion(alg::BLOCKCHAIN_VERSION);
    gen.setCreation(block::Time::now());
    gen.setCreator(wallet.getLatestAddressBin(alg::Wallet::KeyUsage::Specie));
    gen.setValidatorKey(wallet.getLatestPublicKey(alg::Wallet::KeyUsage::Validator));
    gen.setStakeAddress(wallet.getLatestAddressBin(alg::Wallet::KeyUsage::Validator));
    gen.setSeed(args.getArg("seed"));

    block::Specie greedSpec;
    greedSpec.setCoins(greed);
    gen.setGreed(greedSpec);

    // Serialize genesis block
    BinData genesisData = gen.encode();

    // Make the first reconciliation block
    block::Reconciliation rec;
    rec.setProtocol(alg::PROTOCOL_VERSION);
    rec.setChainId(0);
    rec.setShardHashes({block::Hash(genesisData)});
    rec.setLeader(wallet.getLatestAddressBin(alg::Wallet::KeyUsage::Validator));
    rec.setSignature(ossl::Curve25519::sign(*wallet.getLatestKey(alg::Wallet::KeyUsage::Validator), rec.toContent()));

    // Vote for ourselves
    block::Validation validation;
    validation.setVerified(true);
    validation.setAddress(wallet.getLatestAddressBin(alg::Wallet::KeyUsage::Validator));
    validation.setSignature(rec.getSignature());
    rec.setVotes({validation});

    BinData recData = rec.encode();

    // Get output directory
    if (args.hasArg("output"))
        outputDir = args.getArg("output");
    if (!FileTools::isDir(outputDir) && !FileTools::createDir(outputDir))
    {
        log::error("Failed to create output directory '{}'.", outputDir);
        return error::CreateOutputDir;
    }

    // Write out blocks
    if (!FileTools::write(outputDir + "/00.der", genesisData))
    {
        log::error("Failed to write genesis block to filesystem.");
        return error::WriteBlock;
    }

    if (!FileTools::write(outputDir + "/01.der", recData))
    {
        log::error("Failed to write initial reconciliation block to filesystem.");
        return error::WriteBlock;
    }

    // Write out wallet
    std::string walletData = wallet.getString(password);
    if (walletData.empty())
    {
        log::error("Failed to serialize wallet.");
        return error::SerializeWallet;
    }

    if (!FileTools::write(walletFile, walletData))
    {
        log::error("Failed to write out wallet.");
        return error::WriteWallet;
    }

    printf("Success.\n");
    fflush(stdout);

    return error::Success;
}
