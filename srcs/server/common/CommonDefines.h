#ifndef __INC_METIN2_COMMON_DEFINES_H__
#define __INC_METIN2_COMMON_DEFINES_H__

//////////////////////////////////////////////////////////////////////////
// ### General Features ###
#define ENABLE_FULL_NOTICE
#define ENABLE_PORT_SECURITY
#define ENABLE_BELT_INVENTORY_EX
enum eCommonDefines {
	MAP_ALLOW_LIMIT = 32, // 32 default
};
// ### General Features ###
//////////////////////////////////////////////////////////////////////////

#define ENABLE_PLAYER_PER_ACCOUNT5
#define ENABLE_DICE_SYSTEM
#define ENABLE_EXTEND_INVEN_SYSTEM

#define ENABLE_WEAPON_COSTUME_SYSTEM
// ### CommonDefines Systems ###
//////////////////////////////////////////////////////////////////////////

#define __DUNGEON_INFO_SYSTEM__
#define __ENABLE_BLOCK_EXP__
#define __INFINITE_ARROW__
#define __CMD_WARP_IN_DUNGEON__
#define __DEFENSE_WAVE__
#define __VERSION_162__
#ifdef __VERSION_162__
	#define HEALING_SKILL_VNUM 265
#endif

/******************************/
/*  WORLDART SYSTEMS  */
#define ENABLE_CUBE_RENEWAL_WORLDARD
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	#define ENABLE_CUBE_RENEWAL_COPY_WORLDARD		// COPY-PASTE attribute and stones trasfert
#endif

/****************************************/

#define	__ENABLE_RANGE_ALCHEMY__							// Open shop everywhere
#define __ENABLE_REFINE_ALCHEMY__							// Open Refine Window everywhere
#define	ENABLE_STRONG_BOSS									// Bonus strong against Boss
#define ENABLE_STRONG_METIN									// Bonus strong against Metin
#define ENABLE_RESIST_MONSTER								// New Bonus Resist Monster
#define ENABLE_ACCE_SYSTEM									// Sash System Metin2

#define ELEMENT_NEW_BONUSES									// Bonus Elemental Resistence (Fire, Ice, Dark, Earth, Elect, Wind)
#ifdef ELEMENT_NEW_BONUSES									//
#define ELEMENT_TARGET										// Target Element Resistence (Target Mob)
#define ENABLE_PENDANT										// New Item Talisman Resistence (Item Metin2 17.5)
#define ENABLE_NEW_BONUS_TALISMAN							// New Bonus DB for Talisman (Metin2 17.5)
#endif
#define __VIEW_TARGET_PLAYER_HP__							// Show Decimal HP
#ifdef __VIEW_TARGET_PLAYER_HP__							//
	#define __VIEW_TARGET_DECIMAL_HP__						//
#endif	
#define ENABLE_ITEMAWARD_REFRESH

#define ENABLE_CHANNEL_SWITCH_SYSTEM							// Instant Change Channel

#define __USE_ADD_WITH_ALL_ITEMS__							//Use Green Add & switch for all items
// #define __ENABLE_GREEN_ITEM_LVL_30__						//Use Green Add & switch for all items max lvl 30
#define __ENABLE_CAPITALE_MAP__								//New Coord Warp Map in Capital (index 214)


#define ENABLE_FEATURES_REFINE_SYSTEM						// Refine System
#define __OPEN_SAFEBOX_CLICK__								// Command to open Safebox from python
#define ENABLE_SORT_INVEN									// Sort Inventory

#define ENABLE_MULTI_LANGUAGE								// MultiLanguage
#ifdef ENABLE_MULTI_LANGUAGE
	#define ENABLE_MULTI_NAMES								//Multilanguage Translate Mob/Npc
#endif



//OFFLINESHOP
#define __ENABLE_NEW_OFFLINESHOP__
// #define DISABLE_OFFSHOP_OFFERS

#define ENABLE_NEW_OFFLINESHOP_LOGS
#ifdef __ENABLE_NEW_OFFLINESHOP__
#define ENABLE_NEW_SHOP_IN_CITIES
#endif

/*** Ikarus Defines ***/
//#define ENABLE_OFFLINESHOP_DEBUG
#ifdef ENABLE_OFFLINESHOP_DEBUG
#	ifdef __WIN32__
#		define OFFSHOP_DEBUG(fmt , ...) sys_log(0,"%s:%d >> " fmt , __FUNCTION__ , __LINE__, __VA_ARGS__)
#	else
#		define OFFSHOP_DEBUG(fmt , args...) sys_log(0,"%s:%d >> " fmt , __FUNCTION__ , __LINE__, ##args)
#	endif
#else
#	define OFFSHOP_DEBUG(...)   
#endif
#define ENABLE_ITEM_EXTRA_PROTO
#define ENABLE_RARITY_SYSTEM
#define ENABLE_NEW_EXTRA_BONUS
#define __FIX_PRO_DAMAGE__
#endif
