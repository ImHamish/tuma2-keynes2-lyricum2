#ifndef SHARE_ITEMCONSTANTS_HPP
#define SHARE_ITEMCONSTANTS_HPP

#if VSTD_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <map>

#ifdef USE_AUTO_HUNT
static uint32_t st_dwAutoHuntAllowedItems[] =
{
    70038,                                          // Bravery Cape
    76007,                                          // Bravery Cape
};
#endif

#ifdef IS_SERVER
#ifdef USE_AUTO_AGGREGATE
static uint32_t st_dwUnallowedAggregateMapIndex[] =
{
    113,
};

static uint32_t st_dwUnallowedAggregateMonster[] =
{
//#ifdef ENABLE_MELEY_LAIR
    6193,
//#endif
//#ifdef __DEFENSE_WAVE__
    3962,
//#endif
//#ifdef ENABLE_ANCIENT_PYRAMID
    4158,
//#endif
};
#endif
#endif

#ifdef USE_MULTIPLE_OPENING
static const uint32_t ar_sAllowedMultipleOpeningItemVnums[] =
{
    27987,
};
#endif

#endif // SHARE_ITEMCONSTANTS_HPP
