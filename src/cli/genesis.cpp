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

    // Setup argument parser
    ArgParser parser;
    parser.addCategory("Wallet");
    parser.addParam('w', "wallet", "File to read and save wallet");
    parser.addParam('W', "wallet-salt", "Salt for new wallet address");
    parser.addParam('p', "wallet-password", "Password for wallet (Default: Prompt)");
    parser.addParam('g', "greed", "How many coins to mint");
    parser.addParam('s', "seed", "Seed value (Optional)");
    parser.addParam('o', "output", "Output directory (Default ~/.fluffycoin/blocks)");

    // Parse
    Args args = parser.parse(argc, argv);
    if (args.hasError())
        return error::ArgumentParse;

    // Check required parameters
    if (!args.hasArg("wallet") || !args.hasArg("greed"))
    {
        fprintf(stderr, "Missing required arguments.\n");
        parser.printHelp();
        return error::ArgumentMissing;
    }

    // Read in wallet
    std::string walletContents;
    if (FileTools::exists(args.getArg("wallet")))
    {
        if (!FileTools::read(args.getArg("wallet"), walletContents))
        {
            log::error("Failed to read in wallet data from '{}'.", args.getArg("wallet"));
            return error::ReadWallet;
        }

        log::info("Read in wallet data from '{}'.", args.getArg("wallet"));
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
    if (!wallet.makeNewKey(args.getArg("wallet-salt")))
    {
        log::error("Failed to create wallet key.");
        return error::MakeKey;
    }

    // Create genesis block
    block::Genesis gen;
    gen.setProtocol(alg::PROTOCOL_VERSION);
    gen.setName(alg::BLOCKCHAIN_NAME);
    gen.setVersion(alg::BLOCKCHAIN_VERSION);
    gen.setCreation(block::Time::now());
    gen.setCreator(wallet.getLatestAddressBin());
    gen.setCreatorKey(wallet.getLatestPublicKey());
    gen.setSeed(args.getArg("seed"));

    block::Specie greed;
    greed.setCoins(static_cast<uint64_t>(args.getSizeT("greed")));
    gen.setGreed(greed);

    // Serialize genesis block
    BinData genesisData = gen.encode();

    // Make the first reconciliation block
    block::Reconciliation rec;
    rec.setProtocol(alg::PROTOCOL_VERSION);
    rec.setChainId(0);
    rec.setShardHashes({block::Hash(genesisData)});
    rec.setLeader(wallet.getLatestAddressBin());
    rec.setSignature(ossl::Curve25519::sign(*wallet.getLatestKey(), rec.toContent()));

    // Vote for ourselves
    block::Validation validation;
    validation.setVerified(true);
    validation.setAddress(wallet.getLatestAddressBin());
    validation.setSignature(rec.getSignature());
    rec.setVotes({validation});

    BinData recData = rec.encode();

    // Get output directory
    std::string outputDir = args.getArg("output");
    if (outputDir.empty())
        outputDir = "~/.fluffycoin/blocks";
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

    if (!FileTools::write(args.getArg("wallet"), walletData))
    {
        log::error("Failed to write out wallet.");
        return error::WriteWallet;
    }

    printf("Success.\n");
    fflush(stdout);

    return error::Success;
}
