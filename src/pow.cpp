// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Copyright (c) 2024 The Bitrae Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>
#include <assert.h>
#include <algorithm>

// ---------------------------------------------------------------
// LWMA v3 (Zawy12) per-block difficulty adjustment
// - Linear Weighted Moving Average over the last N blocks
// - Clamp solvetimes to limit timestamp manipulation
// - Average targets across the same window
// - Activation is height-gated; legacy path remains pre-fork
// - Based on Zawy’s LWMA v3 notes: https://github.com/zawy12/difficulty-algorithms
//
// Bitrae additions:
// - Per-block adjustment clamp (consensus param: nLWMAMaxAdjustFactor) to prevent abrupt swings
// - Preserve testnet/regtest min-difficulty "late block" rule even after LWMA activates
// ---------------------------------------------------------------
static unsigned int GetNextWorkRequired_LWMA(const CBlockIndex* pindexLast,
                                             const CBlockHeader* /*pblock*/,
                                             const Consensus::Params& params)
{
    assert(pindexLast != nullptr);

    const arith_uint256 powLimit = UintToArith256(params.powLimit);
    const unsigned int powLimitCompact = powLimit.GetCompact();

    const int64_t T = params.nPowTargetSpacing;          // target spacing (37s for Bitrae)
    const int N     = std::max(params.nLWMAWindow, 10);  // safety floor

    // If chain is too short, return powLimit.
    if (pindexLast->nHeight < N) {
        return powLimitCompact;
    }

    // Accumulators
    // sum_{i=1..N} i * solvetime_i (newest block has largest weight i=N)
    int64_t sumWeightedSolveTimes = 0;
    const int64_t sumWeights = (int64_t)N * (N + 1) / 2; // N(N+1)/2
    arith_uint256 avgTarget = arith_uint256(); // zero-init

    const CBlockIndex* block = pindexLast;

    for (int i = 1; i <= N; ++i) {
        const CBlockIndex* prev = block->pprev;
        if (!prev) {
            return powLimitCompact;
        }

        // Solve time between consecutive blocks
        int64_t solvetime = block->GetBlockTime() - prev->GetBlockTime();

        // Clamp to limit timestamp abuse. Allow some negative values to blunt manipulation.
        if (solvetime >  6 * T) solvetime =  6 * T;
        if (solvetime < -6 * T) solvetime = -6 * T;
        if (solvetime < 1)      solvetime =  1;

        sumWeightedSolveTimes += (int64_t)i * solvetime;

        // Accumulate targets (compact -> arith)
        arith_uint256 target; target.SetCompact(block->nBits);
        avgTarget += target;

        block = prev;
    }

    // Mean target across the window
    avgTarget /= N;

    // Normalization for linear weights: k = T * sum_{i=1..N} i
    const int64_t k = T * sumWeights;
    if (k <= 0) {
        return powLimitCompact;
    }

    // nextTarget = avgTarget * (sumWeightedSolveTimes / k)
    // Use integer math with scaling to reduce precision loss.
    const int64_t SCALE = 1000000; // 1e6
    int64_t num = sumWeightedSolveTimes;
    if (num < 1) num = 1;

    arith_uint256 nextTarget = avgTarget;
    nextTarget *= (uint64_t)(num * SCALE);
    nextTarget /= (uint64_t)(k * SCALE);

    // --- Bitrae safety clamp: limit per-block adjustment (prevents abrupt difficulty swings) ---
    // Clamp nextTarget relative to last block's target.
    // The clamp factor is consensus-controlled via params.nLWMAMaxAdjustFactor:
    //   - MAX_ADJ = 4 means difficulty can at most 4x harder (target/4) or 4x easier (target*4) in one block.
    //   - MAX_ADJ <= 0 disables the clamp (useful for regtest/experiments; not recommended for production).
    {
        const int64_t MAX_ADJ = params.nLWMAMaxAdjustFactor;

        if (MAX_ADJ > 0) {
            arith_uint256 lastTarget; lastTarget.SetCompact(pindexLast->nBits);

            // minTarget = lastTarget / MAX_ADJ  (harder)
            arith_uint256 minTarget = lastTarget / (uint64_t)MAX_ADJ;
            if (minTarget == 0) minTarget = arith_uint256(1);

            // maxTarget = lastTarget * MAX_ADJ  (easier)
            arith_uint256 maxTarget = lastTarget * (uint64_t)MAX_ADJ;

            // Also respect powLimit on the easy side
            if (maxTarget > powLimit) maxTarget = powLimit;

            if (nextTarget < minTarget) nextTarget = minTarget;
            if (nextTarget > maxTarget) nextTarget = maxTarget;
        }
    }

    if (nextTarget > powLimit) nextTarget = powLimit;

    return nextTarget.GetCompact();
}

// ---------------------------------------------------------------
// Legacy Litecoin-style periodic retarget (kept pre-activation)
// ---------------------------------------------------------------
unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < params.nPowTargetTimespan/4)
        nActualTimespan = params.nPowTargetTimespan/4;
    if (nActualTimespan > params.nPowTargetTimespan*4)
        nActualTimespan = params.nPowTargetTimespan*4;

    // Retarget
    arith_uint256 bnNew;
    arith_uint256 bnOld;
    bnNew.SetCompact(pindexLast->nBits);
    bnOld = bnNew;

    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    bool fShift = bnNew.bits() > bnPowLimit.bits() - 1;
    if (fShift) bnNew >>= 1;
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;
    if (fShift) bnNew <<= 1;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);

    const unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // -----------------------------------------------------------
    // Apply testnet/regtest "min-difficulty if blocks are late" rule even AFTER LWMA activates.
    // This is intentionally gated by fPowAllowMinDifficultyBlocks (false on mainnet).
    // -----------------------------------------------------------
    if (params.fPowAllowMinDifficultyBlocks) {
        // If the new block's timestamp is more than 2 * target spacing,
        // allow mining of a min-difficulty block to unstick the chain.
        if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing * 2) {
            return nProofOfWorkLimit;
        }
    }

    // Choose activation height based on network behavior (testnet-like vs mainnet-like).
    // IMPORTANT: This must NOT depend on fPowAllowMinDifficultyBlocks, because you may want
    // "testnet-like" activation heights while disabling the min-difficulty rule for clean LWMA tests.
    const bool isTestnetLike = params.fPowIsTestnetLike;
    const int  lwmaHeight    = isTestnetLike ? params.nLWMAHeightTestnet : params.nLWMAHeight;

    // Switch to LWMA at activation
    if (pindexLast->nHeight + 1 >= lwmaHeight) {
        return GetNextWorkRequired_LWMA(pindexLast, pblock, params);
    }

    // Legacy path (pre-activation)
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Only change once per difficulty adjustment interval
    if ((pindexLast->nHeight + 1) % params.DifficultyAdjustmentInterval() != 0)
    {
        if (params.fPowAllowMinDifficultyBlocks)
        {
            // (Delay case handled above already, but keep the classic behavior here)
            // Return the last non-special-min-difficulty-rules block
            const CBlockIndex* pindex = pindexLast;
            while (pindex->pprev &&
                   pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 &&
                   pindex->nBits == nProofOfWorkLimit)
            {
                pindex = pindex->pprev;
            }
            return pindex->nBits;
        }
        return pindexLast->nBits;
    }

    // Go back by what we want to be a full period
    int blockstogoback = params.DifficultyAdjustmentInterval() - 1;
    if ((pindexLast->nHeight + 1) != params.DifficultyAdjustmentInterval())
        blockstogoback = params.DifficultyAdjustmentInterval();

    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; pindexFirst && i < blockstogoback; i++)
        pindexFirst = pindexFirst->pprev;

    assert(pindexFirst);

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}

