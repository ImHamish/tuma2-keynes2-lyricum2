#if !defined(__INC_METIN_II_COMMON_H__)
#define __INC_METIN_II_COMMON_H__

/// NEFOLOSITE														///
/*
// Server & Client

*/

/*
// Server

*/

/*
// Client

*/
/// Server - Nefolosite




/// FOLOSITE														///
// Server & Client
#define USE_BATTLEPASS
#define USE_MOUNT_COSTUME_SYSTEM									// 
#define ENABLE_OFFLINESHOP_REWORK									// 
#if defined(ENABLE_OFFLINESHOP_REWORK)
#define DISABLE_OFFLINESHOP_CHANGENAME								// 
#define DISABLE_OFFLINESHOP_STRICTRULES								// 
#endif
#define USE_SOCKET_MAX_NUM 4										// 
#define USE_ATTR_6TH_7TH											// 
// #define USE_9TH_SKILL												// 
#define USE_SPECIAL_EXP_FLAG										// 

// Server
#if defined(USE_BATTLEPASS)
#define RESTRICT_COMMAND_GET_INFO			GM_LOW_WIZARD			// 
#define RESTRICT_COMMAND_SET_MISSION		GM_IMPLEMENTOR			// 
#define RESTRICT_COMMAND_PREMIUM_ACTIVATE	GM_IMPLEMENTOR			// 
#endif
#define NEW_PERMANENT_CAPE											// 70038 cape permanenta
#define USE_ANTI_EXP												// 
#define USE_NEWHP_REFRESH											// 
#define USE_DELAY_ON_AUTOPOTIONS_HP_SP 100							// 100 millisecunde
#define USE_WHEEL_OF_FORTUNE										// 
#define USE_AURA_ATTR_REMOVER										// 
#define USE_REQUIRMENTS_TO_OPEN_DS_CHESTS 1001						// value5 - group
#define USE_NO_ITEM_USE_IN_OX										// 
#define USE_DEADTIME_CHECK 3                                        //

// Client
#define USE_AUTO_REFINE												// 
#define USE_NPC_WEAR_ITEM											// 
#define USE_RACE_HEIGHT												// 
#define USE_FOG_FIX													// 
#define USE_ENVIRONMENT_OPTIONS										// 
#define USE_SCREEN_LIMIT											// 
#define USE_ANI_IMAGE_RENEWAL										// 
#define USE_DMG_RENEWAL												// 
#define USE_WIKI_ADDONS												// 
#define USE_NEW_ENCHANT_ATTR										// 
#define ENABLE_METINSTONE_SIZE										// 

#endif // __INC_METIN_II_COMMON_H__
