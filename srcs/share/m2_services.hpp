#ifndef SHARE_SERVICES_HPP
#define SHARE_SERVICES_HPP

#if VSTD_HAS_PRAGMA_ONCE
#pragma once
#endif

//////////////////////////////////////////////////////////////////////
/// Server & Client //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#define IS_EMPIREMMORPG                                             // 
#define USE_CATEGORIZED_INVENTORY                                   // 
#define USE_BATTLEPASS                                              // 
#define USE_MOUNT_COSTUME_SYSTEM                                    // 
#define ENABLE_OFFLINESHOP_REWORK                                   // 
#if defined(ENABLE_OFFLINESHOP_REWORK)
#define DISABLE_OFFLINESHOP_CHANGENAME                              // 
#define DISABLE_OFFLINESHOP_STRICTRULES                             // 
#endif
#define USE_SOCKET_MAX_NUM 4                                        // 
#define USE_ATTR_6TH_7TH                                            // 
#define USE_SPECIAL_EXP_FLAG                                        // 
#define USE_AUTO_HUNT                                               // 
#ifdef USE_AUTO_HUNT
#define USE_AUTO_AGGREGATE                                          // 
#endif
#define USE_SKILL_BOOK_BONUS_100_PERCENT                            // 
#define USE_NO_HALLOWEEN_EVENT_2022_BONUSES                         // 
#define USE_PICKUP_FILTER                                           // 
#define USE_PICKUP_INCREASED_RANGE                                  // 
//#define USE_9TH_SKILL                                             // 
//#define USE_NEW_GYEONGGONG_SKILL                                  // 
#define USE_CRASH_RPT                                               // 
#ifdef USE_CRASH_RPT
#define CRASH_RPT_EMAIL_TO "keynesmt2@gmail.com"
#define CRASH_RPT_EMAIL_SUBJECT "Crash Report"
#define CRASH_RPT_URL "http://patch.keynes2.online/crashrpt/rpt.php"
#endif
#define USE_MULTIPLE_OPENING                                        // 



//////////////////////////////////////////////////////////////////////
/// Server ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#if defined(USE_BATTLEPASS)
#define RESTRICT_COMMAND_GET_INFO           GM_LOW_WIZARD           // 
#define RESTRICT_COMMAND_SET_MISSION        GM_IMPLEMENTOR          // 
#define RESTRICT_COMMAND_PREMIUM_ACTIVATE   GM_IMPLEMENTOR          // 
#endif
#define USE_ANTI_EXP                                                // 
#define USE_NEWHP_REFRESH                                           // 
#define USE_DELAY_ON_AUTOPOTIONS_HP_SP 100                          // 100 millisecunde
#define USE_WHEEL_OF_FORTUNE                                        // 
#define USE_REQUIRMENTS_TO_OPEN_DS_CHESTS 1001                      // value5 - group
#define USE_NO_ITEM_USE_IN_OX                                       // 
#define USE_DEADTIME_CHECK 3                                        //
#define USE_SAME_TIMED_TIME_IF_POS_FIGHTING                         // 
#define USE_NO_CLEAR_SPEED_AND_ATTACK_AFFECT_AT_PC_DIE              // 
#define USE_REWARD_AT_FIRST_LOGIN                                   // 
// #define USE_AURA_ATTR_REMOVER                                    // 



//////////////////////////////////////////////////////////////////////
/// Client ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#ifndef APP_NAME
#define APP_NAME "Keynes2"
#endif
#define USE_AUTO_REFINE                                             // 
#define USE_NPC_WEAR_ITEM                                           // 
#define USE_RACE_HEIGHT                                             // 
#define USE_FOG_FIX                                                 // 
#define USE_ENVIRONMENT_OPTIONS                                     // 
#define USE_SCREEN_LIMIT                                            // 
#define USE_ANI_IMAGE_RENEWAL                                       // 
#define USE_DMG_RENEWAL                                             // 
#define USE_WIKI_ADDONS                                             // 
#define USE_NEW_ENCHANT_ATTR                                        // 
#define ENABLE_METINSTONE_SIZE                                      // 
#define ENABLE_WINDOW_SLIDE_EFFECT                                  // 
#ifdef USE_PICKUP_FILTER
#define USE_PICKUP_FILTER_ICO_AS_AFFECT                             // 
#endif
#define USE_QUICK_PICKUP                                            // 
#ifdef USE_AUTO_HUNT
#define USE_AUTO_HUNT_NO_POTIONS                                    // 
#endif

#endif // SHARE_SERVICES_HPP
