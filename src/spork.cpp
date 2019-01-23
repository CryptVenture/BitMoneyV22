// Copyright (c) 2014-2016 The Dash developers
// Copyright (c) 2016-2018 The PIVX developers
// Copyright (c) 2018-2018 The BitMoney developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "spork.h"
#include "base58.h"
#include "key.h"
#include "main.h"
#include "masternode-budget.h"
#include "net.h"
#include "protocol.h"
#include "sync.h"
#include "sporkdb.h"
#include "util.h"

using namespace std;
using namespace boost;

class CSporkMessage;
class CSporkManager;

CSporkManager sporkManager;

std::map<uint256, CSporkMessage> mapSporks;
std::map<int, CSporkMessage> mapSporksActive;

// BitMoney: on startup load spork values from previous session if they exist in the sporkDB
void LoadSporksFromDB()
{
    for (int i = SPORK_START; i <= SPORK_END; ++i) {
        // Since not all spork IDs are in use, we have to exclude undefined IDs
        std::string strSpork = sporkManager.GetSporkNameByID(i);
        if (strSpork == "Unknown") continue;

        // attempt to read spork from sporkDB
        CSporkMessage spork;
        if (!pSporkDB->ReadSpork(i, spork)) {
            LogPrintf("%s : no previous value for %s found in database\n", __func__, strSpork);
            continue;
        }

        // add spork to memory
        mapSporks[spork.GetHash()] = spork;
        mapSporksActive[spork.nSporkID] = spork;
        std::time_t result = spork.nValue;
        // If SPORK Value is greater than 1,000,000 assume it's actually a Date and then convert to a more readable format
        if (spork.nValue > 1000000) {
            LogPrintf("%s : loaded spork %s with value %d : %s", __func__,
                      sporkManager.GetSporkNameByID(spork.nSporkID), spork.nValue,
                      std::ctime(&result));
        } else {
            LogPrintf("%s : loaded spork %s with value %d\n", __func__,
                      sporkManager.GetSporkNameByID(spork.nSporkID), spork.nValue);
        }
    }
}

void ProcessSpork(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)
{
    if (fLiteMode) return; //disable all obfuscation/masternode related functionality

    if (strCommand == "spork") {
        //LogPrintf("ProcessSpork::spork\n");
        CDataStream vMsg(vRecv);
        CSporkMessage spork;
        vRecv >> spork;

        if (chainActive.Tip() == NULL) return;

        // Ignore spork messages about unknown/deleted sporks
        std::string strSpork = sporkManager.GetSporkNameByID(spork.nSporkID);
        if (strSpork == "Unknown") return;

        uint256 hash = spork.GetHash();
        if (mapSporksActive.count(spork.nSporkID)) {
            if (mapSporksActive[spork.nSporkID].nTimeSigned >= spork.nTimeSigned) {
                if (fDebug) LogPrintf("%s : seen %s block %d \n", __func__, hash.ToString(), chainActive.Tip()->nHeight);
                return;
            } else {
                if (fDebug) LogPrintf("%s : got updated spork %s block %d \n", __func__, hash.ToString(), chainActive.Tip()->nHeight);
            }
        }

        LogPrintf("%s : new %s ID %d Time %d bestHeight %d\n", __func__, hash.ToString(), spork.nSporkID, spork.nValue, chainActive.Tip()->nHeight);

        if (spork.nTimeSigned >= Params().NewSporkStart()) {
            if (!sporkManager.CheckSignature(spork, true)) {
                LogPrintf("%s : PREMATURE - Invalid Signature\n", __func__);
                Misbehaving(pfrom->GetId(), 100);
                return;
            }
        }

        if (!sporkManager.CheckSignature(spork)) {
            LogPrintf("%s : Invalid Signature\n", __func__);
            Misbehaving(pfrom->GetId(), 100);
            return;
        }

        mapSporks[hash] = spork;
        mapSporksActive[spork.nSporkID] = spork;
        sporkManager.Relay(spork);

        // BitMoney: add to spork database.
        pSporkDB->WriteSpork(spork.nSporkID, spork);
    }
    if (strCommand == "getsporks") {
        std::map<int, CSporkMessage>::iterator it = mapSporksActive.begin();

        while (it != mapSporksActive.end()) {
            pfrom->PushMessage("spork", it->second);
            it++;
        }
    }
}


// grab the value of the spork on the network, or the default
int64_t GetSporkValue(int nSporkID)
{
    int64_t r = -1;

    if (mapSporksActive.count(nSporkID)) {
        r = mapSporksActive[nSporkID].nValue;
    } else {
        if (nSporkID == SPORK_2_SWIFTTX) r = SPORK_2_SWIFTTX_DEFAULT;
        if (nSporkID == SPORK_3_SWIFTTX_BLOCK_FILTERING) r = SPORK_3_SWIFTTX_BLOCK_FILTERING_DEFAULT;
        if (nSporkID == SPORK_5_MAX_VALUE) r = SPORK_5_MAX_VALUE_DEFAULT;
        if (nSporkID == SPORK_7_MASTERNODE_SCANNING) r = SPORK_7_MASTERNODE_SCANNING_DEFAULT;
        if (nSporkID == SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT) r = SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT_DEFAULT;
        if (nSporkID == SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT) r = SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT_DEFAULT;
        if (nSporkID == SPORK_10_MASTERNODE_PAY_UPDATED_NODES) r = SPORK_10_MASTERNODE_PAY_UPDATED_NODES_DEFAULT;
        if (nSporkID == SPORK_13_ENABLE_SUPERBLOCKS) r = SPORK_13_ENABLE_SUPERBLOCKS_DEFAULT;
        if (nSporkID == SPORK_14_NEW_PROTOCOL_ENFORCEMENT) r = SPORK_14_NEW_PROTOCOL_ENFORCEMENT_DEFAULT;
        if (nSporkID == SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2) r = SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2_DEFAULT;
        if (nSporkID == SPORK_16_ZEROCOIN_MAINTENANCE_MODE) r = SPORK_16_ZEROCOIN_MAINTENANCE_MODE_DEFAULT;
		if (nSporkID == SPORK_17_MAX_BLOCK_COINSIZE) r = SPORK_17_MAX_BLOCK_COINSIZE_DEFAULT;
		if (nSporkID == SPORK_18_MIN_BLOCK_COINSIZE) r = SPORK_18_MIN_BLOCK_COINSIZE_DEFAULT;
		if (nSporkID == SPORK_19_CURRENT_BLOCK_1_COINSIZE) r = SPORK_19_CURRENT_BLOCK_1_COINSIZE_DEFAULT;
		if (nSporkID == SPORK_20_CURRENT_BLOCK_2_COINSIZE) r = SPORK_20_CURRENT_BLOCK_2_COINSIZE_DEFAULT;
		if (nSporkID == SPORK_21_CURRENT_DYNBLOCK_START) r = SPORK_21_CURRENT_DYNBLOCK_START_DEFAULT;
		if (nSporkID == SPORK_22_CURRENT_DYNBLOCK_END) r = SPORK_22_CURRENT_DYNBLOCK_END_DEFAULT;

        if (r == -1) LogPrintf("%s : Unknown Spork %d\n", __func__, nSporkID);
    }

    return r;
}

// grab the spork value, and see if it's off
bool IsSporkActive(int nSporkID)
{
    int64_t r = GetSporkValue(nSporkID);
    if (r == -1) return false;
    return r < GetTime();
}


void ReprocessBlocks(int nBlocks)
{
    std::map<uint256, int64_t>::iterator it = mapRejectedBlocks.begin();
    while (it != mapRejectedBlocks.end()) {
        //use a window twice as large as is usual for the nBlocks we want to reset
        if ((*it).second > GetTime() - (nBlocks * 60 * 5)) {
            BlockMap::iterator mi = mapBlockIndex.find((*it).first);
            if (mi != mapBlockIndex.end() && (*mi).second) {
                LOCK(cs_main);

                CBlockIndex* pindex = (*mi).second;
                LogPrintf("ReprocessBlocks - %s\n", (*it).first.ToString());

                CValidationState state;
                ReconsiderBlock(state, pindex);
            }
        }
        ++it;
    }

    CValidationState state;
    {
        LOCK(cs_main);
        DisconnectBlocksAndReprocess(nBlocks);
    }

    if (state.IsValid()) {
        ActivateBestChain(state);
    }
}

bool CSporkManager::CheckSignature(CSporkMessage& spork, bool fCheckSigner)
{
    //note: need to investigate why this is failing
    std::string strMessage = std::to_string(spork.nSporkID) + std::to_string(spork.nValue) + std::to_string(spork.nTimeSigned);

	CPubKey pubkeyold(ParseHex(Params().SporkKeyOld()));
    CPubKey pubkeynew(ParseHex(Params().SporkKey()));
	//get additional spork keys
	CPubKey pubkey2(ParseHex(Params().SporkKey_2()));
	CPubKey pubkey3(ParseHex(Params().SporkKey_3()));
    std::string errorMessage = "";

	//Some sporks can be signed with main key, some with key 2 and some with key 3

	bool fValidWithOldKey = false;
	bool fValidWithNewKey = false;
	bool fValidWithKeyTwo = false;
	bool fValidWithKeyThree = false;

	//LogPrintf("Spork ID to process : %s\n", std::to_string(spork.nSporkID));

	if (spork.nSporkID == SPORK_2_SWIFTTX)
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_2_SWIFTTX : Needs Sign - %s\n", Params().SporkKey());
	}
	else if (spork.nSporkID == SPORK_3_SWIFTTX_BLOCK_FILTERING)
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_3_SWIFTTX_BLOCK_FILTERING : Needs Sign - %s\n", Params().SporkKey());
	}
	else if (spork.nSporkID == SPORK_5_MAX_VALUE)
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_5_MAX_VALUE : Needs Sign - %s\n", Params().SporkKey());
	}
	else if (spork.nSporkID == SPORK_7_MASTERNODE_SCANNING)
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_7_MASTERNODE_SCANNING : Needs Sign - %s\n", Params().SporkKey());
	}
	else if (spork.nSporkID == SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT)
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT : Needs Sign - %s\n", Params().SporkKey());
	}
	else if (spork.nSporkID == SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT)
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT : Needs Sign - %s\n", Params().SporkKey());
	}
	else if (spork.nSporkID == SPORK_10_MASTERNODE_PAY_UPDATED_NODES)
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_10_MASTERNODE_PAY_UPDATED_NODES : Needs Sign - %s\n", Params().SporkKey());
	}
	else if (spork.nSporkID == SPORK_13_ENABLE_SUPERBLOCKS) 
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_13_ENABLE_SUPERBLOCKS : Needs Sign - %s\n", Params().SporkKey());
	}
	else if (spork.nSporkID == SPORK_14_NEW_PROTOCOL_ENFORCEMENT)
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_14_NEW_PROTOCOL_ENFORCEMENT : Needs Sign - %s\n", Params().SporkKey());
	}
	else if (spork.nSporkID == SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2)
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2 : Needs Sign - %s\n", Params().SporkKey());
	}
	else if (spork.nSporkID == SPORK_16_ZEROCOIN_MAINTENANCE_MODE)
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_16_ZEROCOIN_MAINTENANCE_MODE : Needs Sign - %s\n", Params().SporkKey());
	}
	else if (spork.nSporkID == SPORK_17_MAX_BLOCK_COINSIZE)
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_17_MAX_BLOCK_COINSIZE : Needs Sign - %s\n", Params().SporkKey());
	}
	else if (spork.nSporkID == SPORK_18_MIN_BLOCK_COINSIZE)
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_18_MIN_BLOCK_COINSIZE : Needs Sign - %s\n", Params().SporkKey());
	}
	else if (spork.nSporkID == SPORK_19_CURRENT_BLOCK_1_COINSIZE)
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_19_CURRENT_BLOCK_1_COINSIZE : Needs Sign - %s\n", Params().SporkKey());
	}
	else if (spork.nSporkID == SPORK_20_CURRENT_BLOCK_2_COINSIZE)
	{
		fValidWithKeyTwo = obfuScationSigner.VerifyMessage(pubkey2, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_20_CURRENT_BLOCK_2_COINSIZE : Needs Sign - %s\n", Params().SporkKey_2());
	}
	else if (spork.nSporkID == SPORK_21_CURRENT_DYNBLOCK_START)
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_21_CURRENT_DYNBLOCK_START : Needs Sign - %s\n", Params().SporkKey());
	}
	else if (spork.nSporkID == SPORK_22_CURRENT_DYNBLOCK_END)
	{
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		//LogPrintf("CheckSignature : Got SPORK - SPORK_22_CURRENT_DYNBLOCK_END : Needs Sign - %s\n", Params().SporkKey());
	}
	else
	{
		
		fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig, strMessage, errorMessage);
		fValidWithKeyTwo = obfuScationSigner.VerifyMessage(pubkey2, spork.vchSig, strMessage, errorMessage);
		fValidWithKeyThree = obfuScationSigner.VerifyMessage(pubkey3, spork.vchSig, strMessage, errorMessage);
		
		//Check if this spork has been signed with the old key
		if (GetAdjustedTime() < Params().RejectOldSporkKey() || spork.nTimeSigned < Params().RejectOldSporkKey())
		{
			fValidWithOldKey = obfuScationSigner.VerifyMessage(pubkeyold, spork.vchSig, strMessage, errorMessage);
		}
		else
		{
			fValidWithOldKey = false;
		}
		//LogPrintf("%s : Spork ID %d Value %d Error if any: %s\n", __func__, std::to_string(spork.nSporkID), std::to_string(spork.nValue), errorMessage);
	}

    //bool fValidWithNewKey = obfuScationSigner.VerifyMessage(pubkeynew, spork.vchSig,strMessage, errorMessage);

	//Last resort, check other spork keys

	/*if (!fValidWithNewKey)
	{
		fValidWithKeyTwo = obfuScationSigner.VerifyMessage(pubkey2, spork.vchSig, strMessage, errorMessage);
		if (!fValidWithNewKey && !fValidWithKeyTwo)
		{
			fValidWithKeyThree = obfuScationSigner.VerifyMessage(pubkey3, spork.vchSig, strMessage, errorMessage);
		}
	}*/

    if ((fCheckSigner) && (!fValidWithNewKey && !fValidWithKeyTwo && !fValidWithKeyThree && !fValidWithOldKey))
        return false;

    // See if window is open that allows for old spork key to sign messages
    if ((!fValidWithNewKey && !fValidWithKeyTwo && !fValidWithKeyThree) && (GetAdjustedTime() < Params().RejectOldSporkKey() || spork.nTimeSigned < Params().RejectOldSporkKey())) {
       // CPubKey pubkeyold(ParseHex(Params().SporkKeyOld()));

		return obfuScationSigner.VerifyMessage(pubkeyold, spork.vchSig, strMessage, errorMessage);
    }

	if (fValidWithNewKey)
	{
		//LogPrintf("Valid with primary key");
		return fValidWithNewKey;
	}
	else if (fValidWithKeyTwo)
	{
		//LogPrintf("Valid with secondary key");
		return fValidWithKeyTwo;
	}
	else if (fValidWithKeyThree)
	{
		//LogPrintf("Valid with tertiary key");
		return fValidWithKeyThree;
	}
	else
	{
		return fValidWithNewKey;
	}
}

bool CSporkManager::Sign(CSporkMessage& spork)
{
    std::string strMessage = std::to_string(spork.nSporkID) + std::to_string(spork.nValue) + std::to_string(spork.nTimeSigned);

    CKey key2;
    CPubKey pubkey2;
    std::string errorMessage = "";

    if (!obfuScationSigner.SetKey(strMasterPrivKey, errorMessage, key2, pubkey2)) {
        LogPrintf("CMasternodePayments::Sign - ERROR: Invalid masternodeprivkey: '%s'\n", errorMessage);
        return false;
    }

    if (!obfuScationSigner.SignMessage(strMessage, errorMessage, spork.vchSig, key2)) {
        LogPrintf("CMasternodePayments::Sign - Sign message failed");
        return false;
    }

    if (!obfuScationSigner.VerifyMessage(pubkey2, spork.vchSig, strMessage, errorMessage)) {
        LogPrintf("CMasternodePayments::Sign - Verify message failed");
        return false;
    }

	//Check if this private key has permissions to sign this spork
	if (sporkManager.CheckSignature(spork, true))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CSporkManager::UpdateSpork(int nSporkID, int64_t nValue)
{
    CSporkMessage msg;
    msg.nSporkID = nSporkID;
    msg.nValue = nValue;
    msg.nTimeSigned = GetTime();

    if (Sign(msg)) {
        Relay(msg);
        mapSporks[msg.GetHash()] = msg;
        mapSporksActive[nSporkID] = msg;
        return true;
    }

    return false;
}

void CSporkManager::Relay(CSporkMessage& msg)
{
    CInv inv(MSG_SPORK, msg.GetHash());
    RelayInv(inv);
}

bool CSporkManager::SetPrivKey(std::string strPrivKey)
{
    CSporkMessage msg;

    // Test signing successful, proceed
    strMasterPrivKey = strPrivKey;

    Sign(msg);

    if (CheckSignature(msg, true)) {
        LogPrintf("CSporkManager::SetPrivKey - Successfully initialized as spork signer\n");
        return true;
    } else {
		LogPrintf("CSporkManager::SetPrivKey - Issue with your provided spork credentials\n");
        return false;
    }
}

int CSporkManager::GetSporkIDByName(std::string strName)
{
    if (strName == "SPORK_2_SWIFTTX") return SPORK_2_SWIFTTX;
    if (strName == "SPORK_3_SWIFTTX_BLOCK_FILTERING") return SPORK_3_SWIFTTX_BLOCK_FILTERING;
    if (strName == "SPORK_5_MAX_VALUE") return SPORK_5_MAX_VALUE;
    if (strName == "SPORK_7_MASTERNODE_SCANNING") return SPORK_7_MASTERNODE_SCANNING;
    if (strName == "SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT") return SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT;
    if (strName == "SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT") return SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT;
    if (strName == "SPORK_10_MASTERNODE_PAY_UPDATED_NODES") return SPORK_10_MASTERNODE_PAY_UPDATED_NODES;
    if (strName == "SPORK_13_ENABLE_SUPERBLOCKS") return SPORK_13_ENABLE_SUPERBLOCKS;
    if (strName == "SPORK_14_NEW_PROTOCOL_ENFORCEMENT") return SPORK_14_NEW_PROTOCOL_ENFORCEMENT;
    if (strName == "SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2") return SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2;
    if (strName == "SPORK_16_ZEROCOIN_MAINTENANCE_MODE") return SPORK_16_ZEROCOIN_MAINTENANCE_MODE;
	if (strName == "SPORK_17_MAX_BLOCK_COINSIZE") return SPORK_17_MAX_BLOCK_COINSIZE;
	if (strName == "SPORK_18_MIN_BLOCK_COINSIZE") return SPORK_18_MIN_BLOCK_COINSIZE;
	if (strName == "SPORK_19_CURRENT_BLOCK_1_COINSIZE") return SPORK_19_CURRENT_BLOCK_1_COINSIZE;
	if (strName == "SPORK_20_CURRENT_BLOCK_2_COINSIZE") return SPORK_20_CURRENT_BLOCK_2_COINSIZE;
	if (strName == "SPORK_21_CURRENT_DYNBLOCK_START") return SPORK_21_CURRENT_DYNBLOCK_START;
	if (strName == "SPORK_22_CURRENT_DYNBLOCK_END") return SPORK_22_CURRENT_DYNBLOCK_END;

    return -1;
}

std::string CSporkManager::GetSporkNameByID(int id)
{
    if (id == SPORK_2_SWIFTTX) return "SPORK_2_SWIFTTX";
    if (id == SPORK_3_SWIFTTX_BLOCK_FILTERING) return "SPORK_3_SWIFTTX_BLOCK_FILTERING";
    if (id == SPORK_5_MAX_VALUE) return "SPORK_5_MAX_VALUE";
    if (id == SPORK_7_MASTERNODE_SCANNING) return "SPORK_7_MASTERNODE_SCANNING";
    if (id == SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT) return "SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT";
    if (id == SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT) return "SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT";
    if (id == SPORK_10_MASTERNODE_PAY_UPDATED_NODES) return "SPORK_10_MASTERNODE_PAY_UPDATED_NODES";
    if (id == SPORK_13_ENABLE_SUPERBLOCKS) return "SPORK_13_ENABLE_SUPERBLOCKS";
    if (id == SPORK_14_NEW_PROTOCOL_ENFORCEMENT) return "SPORK_14_NEW_PROTOCOL_ENFORCEMENT";
    if (id == SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2) return "SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2";
    if (id == SPORK_16_ZEROCOIN_MAINTENANCE_MODE) return "SPORK_16_ZEROCOIN_MAINTENANCE_MODE";
	if (id == SPORK_17_MAX_BLOCK_COINSIZE) return "SPORK_17_MAX_BLOCK_COINSIZE";
	if (id == SPORK_18_MIN_BLOCK_COINSIZE) return "SPORK_18_MIN_BLOCK_COINSIZE";
	if (id == SPORK_19_CURRENT_BLOCK_1_COINSIZE) return "SPORK_19_CURRENT_BLOCK_1_COINSIZE";
	if (id == SPORK_20_CURRENT_BLOCK_2_COINSIZE) return "SPORK_20_CURRENT_BLOCK_2_COINSIZE";
	if (id == SPORK_21_CURRENT_DYNBLOCK_START) return "SPORK_21_CURRENT_DYNBLOCK_START";
	if (id == SPORK_22_CURRENT_DYNBLOCK_END) return "SPORK_22_CURRENT_DYNBLOCK_END";

    return "Unknown";
}
