// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2018 The PIVX developers
// Copyright (c) 2018-2018 The BitMoney developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "libzerocoin/Params.h"
#include "chainparams.h"
#include "random.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7 * 24 * 60 * 60;
    for (unsigned int i = 0; i < count; i++) {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

//   What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions
static Checkpoints::MapCheckpoints mapCheckpoints =
boost::assign::map_list_of
(0, uint256("0x000006889bd568a836dd1a31a3f93949eaa24368f6dd98dc31edd44375578c2f"))
(22, uint256("0x45b8362f68be8d0948b9f48ffbd2af8598740ec6e1a920e2885bd6453b136a05"))
(26, uint256("0x3f50d50d8885a7393460f539fa42a4e12268a6efaf399a268709df5d1b08e972"))
(120, uint256("0xe9aa0316df6bc4020ea4a7161a6d4f2eaac075104d77d2806075364ae8bbc847"))
(142, uint256("0x3c113e8f036d16afd5096cf2c7522c4cae648bb18887e20fa67b1baf1036b0f4"))
(198, uint256("0x3bc3c68f21a860c5e835840f2dc0d1b0555170ac9ba38e869c63b4ad3e3740eb"))
(256, uint256("0xf7b0dab209a9f000c7f25d11fc89f70b2eae368ae400b15c42aab19766d220c6"));

static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
	1539182060, // * UNIX timestamp of last checkpoint block
    0,    // * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the SetBestChain debug.log lines)
    2000        // * estimated number of transactions per day after checkpoint
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of(0, uint256("0x001"));
static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
	1538783565,
    0,
    250};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of(0, uint256("0x001"));
static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
	1538783565,
    0,
    100};

libzerocoin::ZerocoinParams* CChainParams::Zerocoin_Params(bool useModulusV1) const
{
    assert(this);
    static CBigNum bnHexModulus = 0;
    if (!bnHexModulus)
        bnHexModulus.SetHex(zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParamsHex = libzerocoin::ZerocoinParams(bnHexModulus);
    static CBigNum bnDecModulus = 0;
    if (!bnDecModulus)
        bnDecModulus.SetDec(zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParamsDec = libzerocoin::ZerocoinParams(bnDecModulus);

    if (useModulusV1)
        return &ZCParamsHex;

    return &ZCParamsDec;
}

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0xf9;
        pchMessageStart[1] = 0xcd; //0xcd
        pchMessageStart[2] = 0x3b;
        pchMessageStart[3] = 0x68;
        vAlertPubKey = ParseHex("04bef0e73664d22c8b0be13ecaab85b68553b55d5ce8d17df0da1d9381c12bd57f5fb0e6db32850822600413b50bc131a3459439957280342d5aa5f4d7a3816c1b");
        nDefaultPort = 49444;
        bnProofOfWorkLimit = ~uint256(0) >> 20; //20 BitMoney starting difficulty is 1 / 2^12
        nSubsidyHalvingInterval = 210000;
        nMaxReorganizationDepth = 100;
		nEnforceBlockUpgradeMajority = 750;
		nRejectBlockOutdatedMajority = 950;
		nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nTargetTimespan = 1 * 30; // BitMoney: 0.5 minutes
        nTargetSpacingSlowLaunch = 10 * 60;  // BitMoney: 10 minutes (Slow launch - Block 288)
        nTargetSpacing = 1 * 60;  // BitMoney: 60 Seconds
        nMaturity = 19;
        nMasternodeCountDrift = 20;
        nMaxMoneyOut = 70000000000 * COIN;
		strDeveloperFeePayee = "PCZwyuXpcHaNjqGeqbeYrfrhpkpSfAqDiS";

        /** Height or Time Based Activations **/
        nLastPOWBlock = 100;
        nModifierUpdateBlock = 151000;  //66070
        nZerocoinStartHeight = 601;  //601
		nZerocoinStartTime = 1539191495; //1541030400; // old time 1 - nov - 2018  //1539191495
        nBlockEnforceSerialRange = 150000; //Enforce serial range starting this block //65000
        nBlockRecalculateAccumulators = 151000; //Trigger a recalculation of accumulators  //66060
        nBlockFirstFraudulent = ~1; //First block that bad serials emerged
        nBlockLastGoodCheckpoint = 601; //Last valid accumulator checkpoint  //66000
        nBlockEnforceInvalidUTXO = ~1; //Start enforcing the invalid UTXO's
        nInvalidAmountFiltered = 0 * COIN; //Amount of invalid coins filtered through exchanges, that should be considered valid
        nBlockZerocoinV2 = 999999999; //!> The block that zerocoin v2 becomes active - DONT ACTIVATE NOW
        nEnforceNewSporkKey = 1548237600; //!> Sporks signed after (GMT): 01/23/2019 @ 10:00am (UTC) must use the new spork key
        nRejectOldSporkKey = 1548331200; //!> Fully reject old spork key after release - 01/24/2019 @ 12:00pm (UTC)

        /**
         * Build the genesis block. Note that the output of the genesis coinbase cannot
         * be spent as it did not originally exist in the database.
         *
         * CBlock(hash=00000ffd590b14, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=e0028e, nTime=1390095618, nBits=1e0ffff0, nNonce=28917698, vtx=1)
         *   CTransaction(hash=e0028e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
         *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d01044c5957697265642030392f4a616e2f3230313420546865204772616e64204578706572696d656e7420476f6573204c6976653a204f76657273746f636b2e636f6d204973204e6f7720416363657074696e6720426974636f696e73)
         *     CTxOut(nValue=50.00000000, scriptPubKey=0xA9037BAC7050C479B121CF)
         *   vMerkleTree: e0028e
         */
        const char* pszTimestamp = "BitMoney V2 Phoenix is Born - Jacob is Gone";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 1 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("0408287b53c96f466269c6b9f759d1d108f86eabff18afc539e22a2a3b77b5ba90229662fe6cd0d4e79b29bb64b8b0fd7b0635e4df767551e990c4915f5cf7387a") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime = 1538783565;
        genesis.nBits = 0x1e0ffff0;
        genesis.nNonce = 4974290;

        hashGenesisBlock = uint256("0x000006889bd568a836dd1a31a3f93949eaa24368f6dd98dc31edd44375578c2f");
        /*if (false && genesis.GetHash() != hashGenesisBlock)
        {
            printf("recalculating params for mainnet.\n");
            printf("old mainnet genesis nonce: %s\n");
            std::string nNonce = "nNonce is " + std::to_string(genesis.nNonce);
            std::cout << nNonce << '\n';
            printf("old mainnet genesis hash:  %s\n", hashGenesisBlock.ToString().c_str());
            // deliberately empty for loop finds nonce value.
            for(genesis.nNonce == 0; genesis.GetHash() > bnProofOfWorkLimit; genesis.nNonce++){ }
            printf("new mainnet genesis merkle root: %s\n", genesis.hashMerkleRoot.ToString().c_str());
            printf("new mainnet genesis nonce: %s\n");
            std::string nNoncenew = "nNonce is " + std::to_string(genesis.nNonce);
            std::cout << nNoncenew << '\n';
            printf("new mainnet genesis hash: %s\n", genesis.GetHash().ToString().c_str());
        }*/

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x000006889bd568a836dd1a31a3f93949eaa24368f6dd98dc31edd44375578c2f"));
        assert(genesis.hashMerkleRoot == uint256("0x69ce58136f49e4b752e4a9d932dca99dea3d0e6847f6e685bf595c7709623dca"));

		//Activate SEED NODES before release

		vSeeds.push_back(CDNSSeedData("seed1.bitmoney.ws", "seed1.bitmoney.ws"));
		vSeeds.push_back(CDNSSeedData("seed2.bitmoney.ws", "seed2.bitmoney.ws"));
		vSeeds.push_back(CDNSSeedData("seed3.bitmoney.ws", "seed3.bitmoney.ws"));
        vSeeds.push_back(CDNSSeedData("95.179.193.119", "95.179.193.119"));
        vSeeds.push_back(CDNSSeedData("45.32.176.66", "45.32.176.66"));
        vSeeds.push_back(CDNSSeedData("95.179.197.142", "95.179.197.142"));
        vSeeds.push_back(CDNSSeedData("95.179.200.18", "95.179.200.18"));


        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 55); // C
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 108); // x
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 117); // c
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x06)(0x3F)(0x44)(0x23).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x09)(0x61)(0x22)(0x2D).convert_to_container<std::vector<unsigned char> >();
        // 	BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md 1135
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x20)(0x01)(0x1f)(0x6c).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = false;
        fHeadersFirstSyncingActive = false;

        nPoolMaxTransactions = 3;

		strSporkKey = "04548ec8e0136777852ced29129e45fdcff01f695976d9966cb861c890154bedad59a6b898339144c3d5665e54eb5a8b3f687230bc695d11cd083079205eabc584";
		strSporkKey_2 = "04e880a2839ac5a8772b82c2dc13103f92a8912da56a1ad363cb7541f854283bedc7eb1801946d68284dac1988c9e1380241bc01f76f4b01a72e2defd1d84196b1";
		strSporkKey_3 = "042ccf59ed0313097fdccfa87a0153d22f52b3418f42cf36d40736df99d6b2a64545b934070fe33aa2a296e7e17cdab7955c633bbddda73326cd7dbc84894ca8dd";

        strSporkKeyOld = "03c4f9d42586766f08a576c1c12bc4341243cd0d38540d88930e758d01bc614a7b";

		strObfuscationPoolDummyAddress = "PXH4xkfoACR8AVAx6hcY6rffKA5469sUfm";
        nStartMasternodePayments = 1532051032 + 6000; //Wed, 25 Jun 2014 20:36:16 GMT


		//"3785524595953099322030732332417894458952917640589980078881207353759424988688386699981570393730824892087899162469882525415785524785289163704263291597544460443726041076141223833161564919404643857902138248291130192507925412909438492815092632739634710919166008236248628130546586216626488936071191175595779635584877393518161136875379867586369555122943618257682825799572973753218370457550967629699253346127753734336109339963527239980354928530464514295907134933417753890367970148170564589709666774509976914660606920158313723919703144860304708145527388806227671807184713449580486445785596185702636737328751814488125406310677632757161204499446370161625795696701358967520133492763442414328924438951252213139531436612023129897446744323433745151303272805298064300014173023402799470598967668515296320686855307868435064743315986582624234356406272803739765923537847442494084441183663658533174512687180200435388704141146199678454023251018053";

        /** Zerocoin */
		zerocoinModulus =  "37855245959530993220307323324178944589529176405899800788812073537594249886883866999815703937308248"
			 "9208789916246988252541578552478528916370426329159754446044372604107614122383316156491940464385790213824829113019"
			 "2507925412909438492815092632739634710919166008236248628130546586216626488936071191175595779635584877393518161136"
			 "8753798675863695551229436182576828257995729737532183704575509676296992533461277537343361093399635272399803549285"
			 "3046451429590713493341775389036797014817056458970966677450997691466060692015831372391970314486030470814552738880"
			 "6227671807184713449580486445785596185702636737328751814488125406310677632757161204499446370161625795696701358967"
			 "5201334927634424143289244389512522131395314366120231298974467443234337451513032728052980643000141730234027994705"
		     "9896766851529632068685530786843506474331598658262423435640627280373976592353784744249408444118366365853317451268"
             "7180200435388704141146199678454023251018053";
		
		nMaxZerocoinSpendsPerTransaction = 7; // Assume about 20kb each
        nMinZerocoinMintFee = 1 * CENT; //high fee required for zerocoin mints
        nMintRequiredConfirmations = 20; //the maximum amount of confirmations until accumulated in 19
        nRequiredAccumulation = 1;
        nDefaultSecurityLevel = 100; //full security level for accumulators
        nZerocoinHeaderVersion = 4; //Block headers must be this version once zerocoin is active
        nZerocoinRequiredStakeDepth = 200; //The required confirmations for a zBIT to be stakable

        nBudget_Fee_Confirmations = 6; // Number of confirmations for the finalization fee
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
		pchMessageStart[0] = 0xc7;
		pchMessageStart[1] = 0xff;
		pchMessageStart[2] = 0x2d;
		pchMessageStart[3] = 0x4f;
        vAlertPubKey = ParseHex("04a62416422cc32e2295112f0803ab1c594e0b787c9f21b432e44cf37bf1cbf77cb0248dabb3cea6917780e3d0ee4ffb7753517c55a94b98f779ee918449f0d5b3");
        nDefaultPort = 49448;
        nEnforceBlockUpgradeMajority = 4320; // 75%
        nRejectBlockOutdatedMajority = 5472; // 95%
        nToCheckBlockUpgradeMajority = 5760; // 4 days
        nMinerThreads = 0;
        nTargetTimespan = 1 * 60; // BitMoney: 1 day
        nTargetSpacing = 1 * 60;  // BitMoney: 1 minute
        nLastPOWBlock = 200;
        nMaturity = 15;
        nMasternodeCountDrift = 4;
        nModifierUpdateBlock = 51197; //approx Mon, 17 Apr 2017 04:00:00 GMT
        nMaxMoneyOut = 43199500 * COIN;
        nZerocoinStartHeight = 201576;
        nZerocoinStartTime = 1524711188;
        nBlockEnforceSerialRange = 1; //Enforce serial range starting this block
        nBlockRecalculateAccumulators = 9908000; //Trigger a recalculation of accumulators
        nBlockFirstFraudulent = 9891737; //First block that bad serials emerged
        nBlockLastGoodCheckpoint = 9891730; //Last valid accumulator checkpoint
        nBlockEnforceInvalidUTXO = ~1; //Start enforcing the invalid UTXO's
        nInvalidAmountFiltered = ~1; //Amount of invalid coins filtered through exchanges, that should be considered valid
        nBlockZerocoinV2 = 20000; //!> The block that zerocoin v2 becomes active
        nEnforceNewSporkKey = 1521604800; //!> Sporks signed after Wednesday, March 21, 2018 4:00:00 AM GMT must use the new spork key
        nRejectOldSporkKey = 1522454400; //!> Reject old spork key after Saturday, March 31, 2018 12:00:00 AM GMT

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1515616140;
        genesis.nNonce = 79855;

	       hashGenesisBlock = genesis.GetHash();
        //assert(hashGenesisBlock == uint256("0x000007cff63ef602a51bf074e384b3516f0dd202f14d52f7c8c9b1af9423ab2e"));

        vFixedSeeds.clear();
        vSeeds.clear();
		//vSeeds.push_back(CDNSSeedData("45.32.176.66", "45.32.176.66"));
		//vSeeds.push_back(CDNSSeedData("95.179.197.142", "95.179.197.142"));
		//vSeeds.push_back(CDNSSeedData("95.179.200.18", "95.179.200.18"));


        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 139); // Testnet BitMoney addresses start with 'x' or 'y'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 19);  // Testnet BitMoney script addresses start with '8' or '9'
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 239);     // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        // Testnet BitMoney BIP32 pubkeys start with 'DRKV'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x3a)(0x80)(0x61)(0xa0).convert_to_container<std::vector<unsigned char> >();
        // Testnet BitMoney BIP32 prvkeys start with 'DRKP'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x3a)(0x80)(0x58)(0x37).convert_to_container<std::vector<unsigned char> >();
        // Testnet BitMoney BIP44 coin type is '1' (All coin's testnet default)
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 2;
        strSporkKey = "04aa681dd00d9c55d4c28367757b461f78d452de73d63678c04f62ae7047707a58c0477311c6f047056a9a02e904513b9ee80a993ac97667cf27ad07549f2d42f7";
		strObfuscationPoolDummyAddress = "PXH4xkfoACR8AVAx6hcY6rffKA5469sUfm";
        strSporkKeyOld = "04348C2F50F90267E6ABAE92E9A36E6CA60983E28E741F8E7277B11A747463AC48178A5075C5A9319D4A38";
        nStartMasternodePayments = 1420837558; //Fri, 09 Jan 2015 21:05:58 GMT
        nBudget_Fee_Confirmations = 3; // Number of confirmations for the finalization fee. We have to make this very short
                                       // here because we only have a 8 block finalization window on testnet
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        strNetworkID = "regtest";
		pchMessageStart[0] = 0xc2;
		pchMessageStart[1] = 0xfd;
		pchMessageStart[2] = 0x5c;
		pchMessageStart[3] = 0x1f;
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetTimespan = 24 * 60 * 60; // BitMoney: 1 day
        nTargetSpacing = 1 * 60;        // BitMoney: 1 minutes
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        genesis.nTime = 1515524400;
        genesis.nBits = 0x1e0ffff0;
        genesis.nNonce = 732084;

        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 49442;
        //assert(hashGenesisBlock == uint256("0x4f023a2120d9127b21bbad01724fdb79b519f593f2a85b60d3d79160ec5f29df"));

        vFixedSeeds.clear(); //! Testnet mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Testnet mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams
{
public:
    CUnitTestParams()
    {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 49441;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Unit test mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    //! Published setters to allow changing values in unit test cases
    virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval) { nSubsidyHalvingInterval = anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority) { nEnforceBlockUpgradeMajority = anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority) { nRejectBlockOutdatedMajority = anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority) { nToCheckBlockUpgradeMajority = anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks) { fDefaultConsistencyChecks = afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) { fAllowMinDifficultyBlocks = afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams* pCurrentParams = 0;

CModifiableParams* ModifiableParams()
{
    assert(pCurrentParams);
    assert(pCurrentParams == &unitTestParams);
    return (CModifiableParams*)&unitTestParams;
}

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    case CBaseChainParams::UNITTEST:
        return unitTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
