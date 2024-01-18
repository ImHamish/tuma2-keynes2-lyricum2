#ifndef SHARE_VNUMHELPER_HPP
#define SHARE_VNUMHELPER_HPP

#if VSTD_HAS_PRAGMA_ONCE
#pragma once
#endif

#ifdef USE_PICKUP_FILTER
static const bool IsPotionConsumables(const uint32_t dwVnum)
{
    return dwVnum >= 50801 && dwVnum <= 50826 ? true : false;
};

static const bool IsCorDraconistItem(const uint32_t dwVnum)
{
    return dwVnum >= 51501 && dwVnum <= 51638 ? true : false;
};

static const bool IsUniqueConsumablesItem(const uint32_t dwVnum)
{
    return false;
};

static const bool IsIncreasesChangeItem(const uint32_t dwVnum)
{
    return false;
};

static const bool IsHairDyesItem(const uint32_t dwVnum)
{
    if (dwVnum >= 70201 && dwVnum <= 70208)
    {
        return true;
    }

    if (dwVnum >= 71075 && dwVnum <= 71079)
    {
        return true;
    }

    return false;
};

static const bool IsOreStones(const uint32_t dwVnum)
{
    return dwVnum >= 50601 && dwVnum <= 50619 ? true : false;
};

static const bool IsEventItem(const uint32_t dwVnum)
{
    return false;
};

static const bool IsMarriageItem(const uint32_t dwVnum)
{
    return false;
};

static const bool IsParchmentItem(const uint32_t dwVnum)
{
    switch (dwVnum)
    {
        case 25040:
        case 25041:
        case 25042:
        case 25100:
        case 90926:
        case 25043:
        case 39014:
        case 39008:
        case 39009:
        case 39007:
        case 39016:
        case 39015:
        case 39027:
        case 39033:
        case 50067:
        case 50068:
        case 50263:
        case 50264:
        case 55008:
        case 56002:
        case 56003:
        case 56004:
        case 56005:
        case 56006:
        case 56007:
        case 56008:
        case 56009:
        case 70602:
        case 70603:
        case 71000:
        case 71001:
        case 71002:
        case 71003:
        case 71021:
        case 71032:
        case 71100:
        case 22000:
        case 22001:
        case 22010:
        case 22011:
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
};

static const bool IsGroupItem(const uint32_t dwVnum)
{
    return false;
};

static const bool IsTransformItem(const uint32_t dwVnum)
{
    return false;
};
#endif

#endif // SHARE_VNUMHELPER_HPP
