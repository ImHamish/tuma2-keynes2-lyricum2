#ifndef SHARE_LENGTH_HPP
#define SHARE_LENGTH_HPP

#if VSTD_HAS_PRAGMA_ONCE
#pragma once
#endif

//#ifdef ENABLE_MULTI_LANGUAGE
enum EM2Languages
{
    LANGUAGE_DEFAULT,                       // 0
    LANGUAGE_EN,                            // 1
    LANGUAGE_RO,                            // 2
    LANGUAGE_IT,                            // 3
    LANGUAGE_TR,                            // 4
    LANGUAGE_DE,                            // 5
    LANGUAGE_PL,                            // 6
    LANGUAGE_PT,                            // 7
    LANGUAGE_ES,                            // 8
    LANGUAGE_CZ,                            // 9
    LANGUAGE_HU,                            // 10
    LANGUAGE_MAX_NUM,                       // MAX NUM 10
    DEFAULT_LANGUAGE = LANGUAGE_EN,
};
//#endif

enum EM2Misc
{
    LOGIN_MAX_LEN                           = 30,
    PASSWD_MAX_LEN                          = 16,
};

enum EM2PointTypes
{
#ifdef USE_AUTO_HUNT
    POINT_AUTOHUNT_EXPIRE                   = 193,
#endif
};

enum EM2PremiumTypes
{
    PREMIUM_EXP,                            // 0
    PREMIUM_ITEM,                           // 1
    PREMIUM_SAFEBOX,                        // 2
    PREMIUM_AUTOLOOT,                       // 3
    PREMIUM_FISH_MIND,                      // 4
    PREMIUM_MARRIAGE_FAST,                  // 5
    PREMIUM_GOLD,                           // 6
//#ifdef ENABLE_VOTE4BONUS
    PREMIUM_BONUS1,                         // 7
    PREMIUM_BONUS2,                         // 8
    PREMIUM_BONUS3,                         // 9
//#endif
//#ifdef USE_AUTO_HUNT
    PREMIUM_AUTO_USE,                       // 10
//#endif
    PREMIUM_MAX_NUM,                        // 11
};

enum EM2Jobs
{
    JOB_WARRIOR,                            // 0
    JOB_ASSASSIN,                           // 1
    JOB_SURA,                               // 2
    JOB_SHAMAN,                             // 3
    JOB_MAX_NUM,                            // 4
};

#ifdef USE_AUTO_HUNT
enum EM2AutoUseType
{
    AUTO_ONOFF_START,                       // 0
    AUTO_ONOFF_ATTACK,                      // 1
    AUTO_ONOFF_RANGE,                       // 3
    AUTO_ONOFF_METINSTONES,                 // 3
    AUTO_ONOFF_POTION,                      // 4
    AUTO_ONOFF_SKILL,                       // 5
    AUTO_ONOFF_RESTART,                     // 6
    AUTO_ONOFF_MAX,                         // 7
};

enum EM2AutoSlotMax
{
    AUTO_SKILL_SLOT_MAX                     = 12,
    AUTO_POTION_SLOT_START                  = AUTO_SKILL_SLOT_MAX,
    AUTO_POTION_SLOT_MAX                    = AUTO_SKILL_SLOT_MAX + AUTO_POTION_SLOT_START,
};
#endif

#ifdef USE_PICKUP_FILTER
enum EFilterType
{
    FILTER_TYPE_AUTO,                       // 0
    FILTER_TYPE_WEAPON,                     // 1
    FILTER_TYPE_ARMOR_BODY,                 // 2
    FILTER_TYPE_ARMOR_HEAD,                 // 3
    FILTER_TYPE_OTHERS,                     // 4
    FILTER_TYPE_COSTUME,                    // 5
    FILTER_TYPE_DS,                         // 6
    FILTER_TYPE_UNIQUE,                     // 7
    FILTER_TYPE_MATERIAL_AND_METIN,         // 8
    FILTER_TYPE_POTION,                     // 9
    FILTER_TYPE_OTHERS2,                    // 10
    FILTER_TYPE_COMPANY,                    // 11
    FILTER_TYPE_BOOK,                       // 12
    FILTER_TYPE_OTHERS3,                    // 13
    FILTER_TYPE_EVENT,                      // 14
    FILTER_TYPE_MAX,                        // 15
};
#endif

#endif // SHARE_LENGTH_HPP
