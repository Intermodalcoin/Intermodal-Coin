// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 5000;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (0,	       Params().HashGenesisBlock() )
		(100, uint256("0x67fcc63c864e6afec93a9d236111995eec43b6e680146b7d06ee316019435039"))
		(500, uint256("8a4ec57370422defcbe7ac25f995582c0b6d1f85617aedbb4917354e2aec28b8"))
		(1000, uint256("ad9681dcf9ddc74cf6b7c29bff0d01a3e58b7fe9388f6baa66852c4bd35e7410"))
		(2500, uint256("c6e65f65f0446a276e92600fc963abea63327490e6d1822dba473a2fc7dd1155"))
		(5000, uint256("d51100c5e497531fdcfb20c18405d73a15170b0fdba8643d31c71dde060c32ed"))
		(10000, uint256("177b5394559971b8d5998618fa776579661da75f983cb914e169aae2a6ca85ac"))
		(15000, uint256("806cef87ba34a3ccffb6fa763b604d8042150a6515f6e85fb9018210543f4c0b"))
		(50000, uint256("95046a8e94aeab9d52e808f70c6a5a1d204c9c3c0d633538f3489d1fd27c2f99"))
		(100000, uint256("28b899161dd751e7078571a9856323a45833dfc4f74d0738e7ad9b294b23fab3"))
		(150000, uint256("13a00139a3d936ba644fb40815f5e912ec85a9663464f39b8e0eece9ffda7a4c"))
		(175000, uint256("c8f7b036ca014bac34806a2faa5ef761d82f57869ecdaef2b9bc3adc7d942754"))
    ;


    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();
        if (nHeight <= pindexSync->nHeight){
            return false;
        }
        return true;
    }
}
