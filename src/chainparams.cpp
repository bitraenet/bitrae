// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2020 The Bitcoin Core developers
// Copyright (c) 2024 The Bitrae Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/merkle.h>
#include <hash.h> // for signet block challenge hash
#include <tinyformat.h>
#include <util/system.h>
#include <util/strencodings.h>
#include <versionbitsinfo.h>

#include <assert.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "BBC News 02/Apr/2024 Google to delete records from Incognito tracking";
    const CScript genesisOutputScript = CScript() << ParseHex("046b20c56e93c1eed7fc875822e4dbde51afec4dce774b3669edbb467609ea1eeb1cf9fed4e7949edbefa22e8626abe682ecfdf30a5387de16b275344e1fd27f89") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
 /**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */
 
class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = CBaseChainParams::MAIN;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 3360000;
        consensus.BIP16Height = 0; // Activating P2SH from the genesis block to immediately support complex transaction types like multisig.
        consensus.BIP34Height = 0; // Enforces block height in coinbase transactions from the start to prevent chain splits and duplicate transactions.
        consensus.BIP34Hash = uint256S("0xd0ea3e7f3b5322bb1c774d57fecac4f145cd2ddba2b64015c9502f40f542b10f"); // Specific hash for reference.
        consensus.BIP65Height = 0; // Enables OP_CHECKLOCKTIMEVERIFY allowing transactions to be timelocked, enhancing conditional transaction capabilities.
        consensus.BIP66Height = 0; // Enforces strict DER encoding for signatures from block 0, enhancing network security against malleability attacks.
        consensus.CSVHeight = 0; // Enables CheckSequenceVerify at block 0, allowing for relative timelock features which improve smart contract functionality.
        consensus.SegwitHeight = 0; // Activates Segregated Witness at block 0, fixing transaction malleability and increasing block capacity.
        consensus.MinBIP9WarningHeight = 9216; // Sets minimum height for warning of BIP9 soft forks which might activate soon.
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 0.875 * 24 * 60 * 60; // 21 Hours
        consensus.nPowTargetSpacing = 0.625 * 60; // Bitrae: 37 Seconds
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 6129; // 75% of 8172
        consensus.nMinerConfirmationWindow = 8172; // nPowTargetTimespan / nPowTargetSpacing * 4
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartHeight = 0; // Activating Taproot from genesis to leverage advanced privacy and efficiency features immediately.
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeoutHeight = 0; // No timeout as activation is immediate and permanent from the start.

        // Deployment of MWEB (LIP-0002, LIP-0003, and LIP-0004) - Disable MWEB by assigning unreachable start and timeout heights
        consensus.vDeployments[Consensus::DEPLOYMENT_MWEB].bit = 4;
        consensus.vDeployments[Consensus::DEPLOYMENT_MWEB].nStartHeight = 99999999; // Set to an unreachable block height
        consensus.vDeployments[Consensus::DEPLOYMENT_MWEB].nTimeoutHeight = 99999999; // Set to the same unreachable block height

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000000001c482b9f166ec90");
        
        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x9b867ea38530bb6a9275791b7f1c27b5f3156d7bc8a0f3546a845101ebb6c7fc"); // Block 550000

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xef;
        pchMessageStart[1] = 0xd3;
        pchMessageStart[2] = 0xab;
        pchMessageStart[3] = 0xcd;
        nDefaultPort = 6333;
        nPruneAfterHeight = 400000;
        m_assumed_blockchain_size = 20; // Reduce the initial blockchain size estimate to 20 GB
        m_assumed_chain_state_size = 1; // Start with a 1 GB estimate for the state database

        genesis = CreateGenesisBlock(1712059200, 2087712026, 0x1e0ffff0, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0xd0ea3e7f3b5322bb1c774d57fecac4f145cd2ddba2b64015c9502f40f542b10f"));
        assert(genesis.hashMerkleRoot == uint256S("0x2ab18a3c50add0c9824cc7644ff6a8c50cf09a31faf32a1715e96563e309c07d"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as an addrfetch if they don't support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.emplace_back("dnsseed.bitraetools.com");
        vSeeds.emplace_back("dnsseed.bitraepool.com");
        vSeeds.emplace_back("dnsseed.bitraeseed.co.uk");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,25);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
        base58Prefixes[SCRIPT_ADDRESS2] = std::vector<unsigned char>(1,60);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,135);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        bech32_hrp = "btre";
        mweb_hrp = "btremweb";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_main), std::end(chainparams_seed_main));

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        m_is_test_chain = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {  0, uint256S("0xd0ea3e7f3b5322bb1c774d57fecac4f145cd2ddba2b64015c9502f40f542b10f")},
                {1000, uint256S("0x4203a542ff56022e1717901eb8574bb048b7539f2af7b1b956b3d0b775572437")},
                {10000, uint256S("0xb6c0a4452354764f0ac6a957bacd0daf2a7a4a5dde6289d7183e0c1b5885ee16")},
                {20000, uint256S("0xf4a4b4507dd6d5b08135e110249a15817fc112d1db9ef81bf1cc7a331f49e752")},
                {50000, uint256S("0x76afb0471020fef8e5887a7669d06bd95093c45f85678c42556149ed91470a48")},
                {70000, uint256S("0x6f7f6e4215a7371593dbfa66269811347cfe6d129c39e0b9d0cf0a63857d1845")},
                {100000, uint256S("0x1048f4ba25f16120133428a7febcc2d6b0892917f526fcaecc9e69d50f952139")},
                {120000, uint256S("0x7654c07fe285cf05b3d7b4802e62b8f2bf17868fd37425b1efd23de428e9723b")},
                {150000, uint256S("0x1022538a7d0505a0b81c1a64112a9756ab6a6d9ff5a9123f0752a67ca9b1fc72")},
                {170000, uint256S("0xa91a5100ff5ec308f3fce9de65d923d727965b3bb5232c299aa6a28dfafa6a74")},
                {200000, uint256S("0xcb28d70566952d744010ca7faa120c2c4fa0606d3ff5a2a75f900b2542aebb61")},
                {220000, uint256S("0xb7e78b72e3df8e3cd46be5b47bd223c82c92eba53bd43070ce2dfd508c55095b")},
                {250000, uint256S("0xa4e11747b8f820220a4d3254201b69f88038d7f1dab92d277151c04d4a327066")},
                {270000, uint256S("0x049f55bcfd90d5144ad199a1b2157b0e797a18da8de61224be6d293526ad1d71")},
                {300000, uint256S("0x9715cc464fe3b66a3a89c0b578457453c744d05d24f700f5da18e5e58b517b90")},
                {320000, uint256S("0x4b653c324b9b93a6fb7dbb16eaea8cf62cbd99d20a27a5572066e5d73e4dce7e")},
                {350000, uint256S("0x1ad904130d3ce0e9cf62369107d1689112c129db1383796691144d065a866abc")},
                {353328, uint256S("0x94b57e37c9be44f684ddc2bc91e1522522625764b7a5f827d32c068ae7373a0c")},
                {400000, uint256S("0xb4622a2b844c49f1e717bc687d78264c8a4cbeaf45cca488af732465e06ef501")},
                {450000, uint256S("0x31a599762e764d032166ff72a01a83d65ada0fefb12d77304a41ea3c232ef9b6")},
                {500000, uint256S("0xb05a1963980e6f73f8eae4302e236a0e65d7040a463ff0b5c1766967bbd59445")},
                {550000, uint256S("0x9b867ea38530bb6a9275791b7f1c27b5f3156d7bc8a0f3546a845101ebb6c7fc")},
                {595000, uint256S("0xb680976cad9d1ee4e02ce9295d97dd902ffa4a39e04c812b689eb4ce2c47257e")},
            }
        };

        chainTxData = ChainTxData{
            // Data from rpc: getchaintxstats
            /* nTime    */ 1748548335,
            /* nTxCount */ 622490,
            /* dTxRate  */ 0.02748970738209688
        };
    }
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = CBaseChainParams::TESTNET;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 3360000;
        consensus.BIP16Height = 0; // always enforce P2SH BIP16 on testnet
        consensus.BIP34Height = 0; // Enforces block height in coinbase transactions from the start to prevent chain splits and duplicate transactions.
        consensus.BIP34Hash = uint256S("0x2a8a85a61f33d3d35bb20f1c7c1a311e4a88605ff563fa4a0be2a0844a9a3245"); // 0
        consensus.BIP65Height = 0; // Enables OP_CHECKLOCKTIMEVERIFY allowing transactions to be timelocked, enhancing conditional transaction capabilities
        consensus.BIP66Height = 0; // Enforces strict DER encoding for signatures from block 0, enhancing network security against malleability attacks.
        consensus.CSVHeight = 0; // Enables CheckSequenceVerify at block 0, allowing for relative timelock features which improve smart contract functionality.
        consensus.SegwitHeight = 0; // Activates Segregated Witness at block 0, fixing transaction malleability and increasing block capacity.
        consensus.MinBIP9WarningHeight = 9216; // Sets minimum height for warning of BIP9 soft forks which might activate soon.
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 0.875 * 24 * 60 * 60; // 21 Hours
        consensus.nPowTargetSpacing = 0.625 * 60; // Bitrae: 37 Seconds
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 6129; // 75% for testchains
        consensus.nMinerConfirmationWindow = 8172; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartHeight = 0; // Activating Taproot from genesis to leverage advanced privacy and efficiency features immediately.
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeoutHeight = 0; // No timeout as activation is immediate and permanent from the start.

        // Deployment of MWEB (LIP-0002, LIP-0003, and LIP-0004) - Disable MWEB by assigning unreachable start and timeout heights
        consensus.vDeployments[Consensus::DEPLOYMENT_MWEB].bit = 4;
        consensus.vDeployments[Consensus::DEPLOYMENT_MWEB].nStartHeight = 99999999; // Set to an unreachable block height
        consensus.vDeployments[Consensus::DEPLOYMENT_MWEB].nTimeoutHeight = 99999999; // Set to the same unreachable block height

        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000000000000");
        consensus.defaultAssumeValid = uint256S("0x2a8a85a61f33d3d35bb20f1c7c1a311e4a88605ff563fa4a0be2a0844a9a3245"); // 0

        pchMessageStart[0] = 0xa3;
        pchMessageStart[1] = 0xea;
        pchMessageStart[2] = 0xbe;
        pchMessageStart[3] = 0xa8;
        nDefaultPort = 16333;
        nPruneAfterHeight = 4000;
        m_assumed_blockchain_size = 4;
        m_assumed_chain_state_size = 1;

        genesis = CreateGenesisBlock(1712059140, 388919574, 0x1e0ffff0, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x2a8a85a61f33d3d35bb20f1c7c1a311e4a88605ff563fa4a0be2a0844a9a3245"));
        assert(genesis.hashMerkleRoot == uint256S("0x2ab18a3c50add0c9824cc7644ff6a8c50cf09a31faf32a1715e96563e309c07d"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("testnet-seed.bitraetools.com");
        vSeeds.emplace_back("seed-b.bitraepool.com");
        vSeeds.emplace_back("dnsseed-testnet.bitraeseed.co.uk");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,85);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SCRIPT_ADDRESS2] = std::vector<unsigned char>(1,122);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,144);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tbtre";
        mweb_hrp = "tmweb";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_test), std::end(chainparams_seed_test));

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        m_is_test_chain = true;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {0, uint256S("0x2a8a85a61f33d3d35bb20f1c7c1a311e4a88605ff563fa4a0be2a0844a9a3245")},
            }
        };


        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 
            /* nTime    */ 1712059140,
            /* nTxCount */ 0,
            /* dTxRate  */ 1.0,
        };
    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    explicit CRegTestParams(const ArgsManager& args) {
        strNetworkID =  CBaseChainParams::REGTEST;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 2400;
        consensus.BIP16Height = 0;
        consensus.BIP34Height = 0; // BIP34 activated on regtest (Used in functional tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 0; // BIP65 activated on regtest (Used in functional tests)
        consensus.BIP66Height = 0; // BIP66 activated on regtest (Used in functional tests)
        consensus.CSVHeight = 0; // CSV activated on regtest (Used in rpc activation tests)
        consensus.SegwitHeight = 0; // SEGWIT is always activated on regtest unless overridden
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 0.875 * 24 * 60 * 60; // 21 Hours
        consensus.nPowTargetSpacing = 0.625 * 60; // Bitrae: 37 Seconds
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 432; // 75% for testchains
        consensus.nMinerConfirmationWindow = 576; // Faster than normal for regtest (576 instead of 8064)

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // Deployment of MWEB (LIP-0002 and LIP-0003)
        consensus.vDeployments[Consensus::DEPLOYMENT_MWEB].bit = 4;
        consensus.vDeployments[Consensus::DEPLOYMENT_MWEB].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_MWEB].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        pchMessageStart[0] = 0xae;
        pchMessageStart[1] = 0xd0;
        pchMessageStart[2] = 0xfa;
        pchMessageStart[3] = 0xcf;
        nDefaultPort = 16444;
        nPruneAfterHeight = 4000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        UpdateActivationParametersFromArgs(args);

        genesis = CreateGenesisBlock(1712059120, 388919580, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x945cec48e26601001df43d9b26c577fff67fe7a46947fcf9f75f9fc6ecddf802"));
        assert(genesis.hashMerkleRoot == uint256S("0x2ab18a3c50add0c9824cc7644ff6a8c50cf09a31faf32a1715e96563e309c07d"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        fRequireStandard = true;
        m_is_test_chain = true;
        m_is_mockable_chain = true;

        checkpointData = {
            {
                {0, uint256S("945cec48e26601001df43d9b26c577fff67fe7a46947fcf9f75f9fc6ecddf802")},
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,85);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SCRIPT_ADDRESS2] = std::vector<unsigned char>(1,122);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,144);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "rbtre";
        mweb_hrp = "tmweb";
    }

    /**
     * Allows modifying the Version Bits regtest parameters.
     */
    void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout, int64_t nStartHeight, int64_t nTimeoutHeight)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
        consensus.vDeployments[d].nStartHeight = nStartHeight;
        consensus.vDeployments[d].nTimeoutHeight = nTimeoutHeight;
    }
    void UpdateActivationParametersFromArgs(const ArgsManager& args);
};

void CRegTestParams::UpdateActivationParametersFromArgs(const ArgsManager& args)
{
    if (args.IsArgSet("-segwitheight")) {
        int64_t height = args.GetArg("-segwitheight", consensus.SegwitHeight);
        if (height < -1 || height >= std::numeric_limits<int>::max()) {
            throw std::runtime_error(strprintf("Activation height %ld for segwit is out of valid range. Use -1 to disable segwit.", height));
        } else if (height == -1) {
            LogPrintf("Segwit disabled for testing\n");
            height = std::numeric_limits<int>::max();
        }
        consensus.SegwitHeight = static_cast<int>(height);
    }

    if (!args.IsArgSet("-vbparams")) return;

    for (const std::string& strDeployment : args.GetArgs("-vbparams")) {
        std::vector<std::string> vDeploymentParams;
        boost::split(vDeploymentParams, strDeployment, boost::is_any_of(":"));
        if (vDeploymentParams.size() < 3 || 5 < vDeploymentParams.size()) {
            throw std::runtime_error("Version bits parameters malformed, expecting deployment:start:end[:heightstart:heightend]");
        }
        int64_t nStartTime, nTimeout, nStartHeight, nTimeoutHeight;
        if (!ParseInt64(vDeploymentParams[1], &nStartTime)) {
            throw std::runtime_error(strprintf("Invalid nStartTime (%s)", vDeploymentParams[1]));
        }
        if (!ParseInt64(vDeploymentParams[2], &nTimeout)) {
            throw std::runtime_error(strprintf("Invalid nTimeout (%s)", vDeploymentParams[2]));
        }
        if (vDeploymentParams.size() > 3 && !ParseInt64(vDeploymentParams[3], &nStartHeight)) {
            throw std::runtime_error(strprintf("Invalid nStartHeight (%s)", vDeploymentParams[3]));
        }
        if (vDeploymentParams.size() > 4 && !ParseInt64(vDeploymentParams[4], &nTimeoutHeight)) {
            throw std::runtime_error(strprintf("Invalid nTimeoutHeight (%s)", vDeploymentParams[4]));
        }
        bool found = false;
        for (int j=0; j < (int)Consensus::MAX_VERSION_BITS_DEPLOYMENTS; ++j) {
            if (vDeploymentParams[0] == VersionBitsDeploymentInfo[j].name) {
                UpdateVersionBitsParameters(Consensus::DeploymentPos(j), nStartTime, nTimeout, nStartHeight, nTimeoutHeight);
                found = true;
                LogPrintf("Setting version bits activation parameters for %s to start=%ld, timeout=%ld, start_height=%d, timeout_height=%d\n", vDeploymentParams[0], nStartTime, nTimeout, nStartHeight, nTimeoutHeight);
                break;
            }
        }
        if (!found) {
            throw std::runtime_error(strprintf("Invalid deployment (%s)", vDeploymentParams[0]));
        }
    }
}

static std::unique_ptr<const CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<const CChainParams> CreateChainParams(const ArgsManager& args, const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN) {
        return std::unique_ptr<CChainParams>(new CMainParams());
    } else if (chain == CBaseChainParams::TESTNET) {
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    } else if (chain == CBaseChainParams::SIGNET) {
        return std::unique_ptr<CChainParams>(new CTestNetParams()); // TODO: Support SigNet
    } else if (chain == CBaseChainParams::REGTEST) {
        return std::unique_ptr<CChainParams>(new CRegTestParams(args));
    }
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(gArgs, network);
}
