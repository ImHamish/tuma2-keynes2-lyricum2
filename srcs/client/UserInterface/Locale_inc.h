#pragma once

#define ENABLE_REWARD_SYSTEM
#define ENABLE_SWITCHBOT_WORLDARD
#define ENABLE_NEW_GAMEOPTION
#define LINK_IN_CHAT
#define ENABLE_MILES_CHECK
#define ENABLE_PYLIB_CHECK

/* da rimuovere dopo i test */
//#define ENABLE_LITTLE_PACK

/* non usati al momento per via di problemi */
#define INGAME_WIKI
#ifdef INGAME_WIKI
//#define ENABLE_INGAME_WIKI_HEARDERS_WRONGSIZE
	/*
		Only define this if you have wolfman in you server, for more
			informations reed wikipedia system guide!
	*/
#endif

/* common */
#define ENABLE_EXTRA_INVENTORY

//////////////////////////////////////////////////////////////////////////
// ### Default Ymir Macros ###
#define ENABLE_RANKING
#define LOCALE_SERVICE_EUROPE
#define ENABLE_COSTUME_SYSTEM
#define ENABLE_ENERGY_SYSTEM
#define ENABLE_DRAGON_SOUL_SYSTEM
#define ENABLE_NEW_EQUIPMENT_SYSTEM
#define ENABLE_BUY_WITH_ITEM
#ifdef ENABLE_BUY_WITH_ITEM
	#define MAX_SHOP_PRICES 5
#endif
// ### Default Ymir Macros ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### New From LocaleInc ###
#define ENABLE_PACK_GET_CHECK
//#define ENABLE_CANSEEHIDDENTHING_FOR_GM
#define ENABLE_PROTOSTRUCT_AUTODETECT

#define ATTR_LOCK

#define ENABLE_PLAYER_PER_ACCOUNT5
#define ENABLE_LEVEL_IN_TRADE
#define ENABLE_DICE_SYSTEM
#define ENABLE_EXTEND_INVEN_SYSTEM
#define ENABLE_SLOT_WINDOW_EX
#define ENABLE_TEXT_LEVEL_REFRESH
#define ENABLE_USE_COSTUME_ATTR

#define WJ_SHOW_MOB_INFO
#ifdef WJ_SHOW_MOB_INFO
#define ENABLE_SHOW_MOBAIFLAG
#define ENABLE_SHOW_MOBLEVEL
#endif
#define ENABLE_MAGIC_REDUCTION_SYSTEM
#define ENABLE_WEAPON_COSTUME_SYSTEM
#define ENABLE_DEFENSE_WAVE
#define VERSION_162_ENABLED
#define NEW_PET_SYSTEM
#define ENABLE_WHISPER_ADMIN_SYSTEM
#define ENABLE_MELEY_LAIR_DUNGEON
#ifdef ENABLE_MELEY_LAIR_DUNGEON
	#define MELEY_LAIR_DUNGEON_STATUE 6118
#endif
#define ENABLE_CUBE_RENEWAL_WORLDARD
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	// #define ENABLE_CUBE_RENEWAL_GEM_WORLDARD		//Unused - Adapted with ENABLE_GAYA_RENEWAL (Don't Active this define!!)
	#define ENABLE_CUBE_RENEWAL_COPY_WORLDARD
#endif
#define ENABLE_CAPITALE_SYSTEM				// Expanded Capital System
#define	__ENABLE_RANGE_ALCHEMY__			// Open shop everywhere
#define __ENABLE_REFINE_ALCHEMY__			// Open Refine Window everywhere
#define	ENABLE_STRONG_BOSS					// Bonus strong against Boss
#define ENABLE_STRONG_METIN					// Bonus strong against Metin
#define ENABLE_RESIST_MONSTER				// New Bonus Resist Monster
#define ENABLE_MEDI_PVM						// New Bonus Medi Pvm
#define ELEMENT_NEW_BONUSES					// Bonus Elemental Resistence (Fire, Ice, Dark, Earth, Elect, Wind)
#ifdef ELEMENT_NEW_BONUSES					//
#define ENABLE_PENDANT						// New Item Talisman Resistence (Item Metin2 17.5)
#define ENABLE_NEW_BONUS_TALISMAN			// New Bonus DB for Talisman (Metin2 17.5)
#endif
#define ENABLE_VIEW_TARGET_PLAYER_HP		// Show Decimal HP
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP			//
#define ENABLE_VIEW_TARGET_DECIMAL_HP		//
#endif
// #define __ENABLE_INGAME_CHCHANGE__			// Instant Change Channel
#define ENABLE_DATETIME_UNDER_MINIMAP		// DateTime (ONly PythonApplicationModule.cpp)
#define ENABLE_SORT_INVEN					// Sort Inventory (Only PythonApplicationModule.cpp)
#define ENABLE_SOUL_SYSTEM					//System Anima
// if is define ENABLE_ACCE_SYSTEM the players can use shoulder sash 
// if you want to use object scaling function you must defined ENABLE_OBJ_SCALLING
#define ENABLE_ACCE_SYSTEM
#define ENABLE_OBJ_SCALLING
#define	ENABLE_FEATURES_REFINE_SYSTEM		// Refine System
#define ENABLE_SKILL_COLOR_SYSTEM 			// Skill color
#ifdef ENABLE_SKILL_COLOR_SYSTEM
#define ENABLE_5LAYER_SKILL_COLOR 			// Enable 5 layers for skill color
#endif
#define ENABLE_CONFIG_MODULE 					// Enable configuration module for saving settings
#define ENABLE_NEW_EXCHANGE_WINDOW
#define ENABLE_PVP_ADVANCED
#define EQUIP_ENABLE_VIEW_SASH
#define ENABLE_MULTI_LANGUAGE				// MultiLanguage
//OFFSHOP
#define __ENABLE_EMOJI_SYSTEM__

#define ENABLE_OFFLINESHOP_DEBUG
#define ENABLE_NEW_SHOP_IN_CITIES
#define BLOCK_AUTO_ATTACK_PLAYER


//HOTTIES IKARUS STUFFS
#define ENABLE_ITEM_EXTRA_PROTO
#define ENABLE_RARITY_SYSTEM
#define ENABLE_NEW_EXTRA_BONUS

#define ENABLE_ATTR_TRANSFER_SYSTEM
#define ENABLE_ATTR_COSTUMES
#if defined(ENABLE_OFFLINESHOP_DEBUG) && defined(_DEBUG)
#define OFFSHOP_DEBUG(fmt , ...) Tracenf("%s:%d >> " fmt , __FUNCTION__ , __LINE__, __VA_ARGS__)
#else
#define OFFSHOP_DEBUG(...)
#endif

#define ENABLE_DUNGEON_INFO_SYSTEM
#define ENABLE_DS_SET
#define ENABLE_DS_ENCHANT
#define ENABLE_HIGHLIGHT_SYSTEM
#define ENABLE_NEW_PET_EDITS
#define ENABLE_REMOTE_ATTR_SASH_REMOVE
#define __ENABLE_NEW_EFFECT_STOLE__
//#define ENABLE_ATLAS_BOSS
#define ENABLE_STOLE_REAL
#define ENABLE_STOLE_COSTUME
#define ENABLE_COSTUME_EFFECT
#define ENABLE_EFFECT_WEAPON_COSTUME
#define ENABLE_BLOCK_MULTIFARM
#define BL_OFFLINE_MESSAGE
#define ENABLE_NEW_USE_POTION
#define __EFFETTO_MANTELLO__
#define ENABLE_LIMIT_PUSH_DEST
#define ENABLE_NEW_RESTART
#define DISABLE_MENU_IF_KEY_F10
#define ENABLE_VIEW_ELEMENT
#define ENABLE_RECALL
#define ENABLE_NEW_COMMON_BONUSES
#define ENABLE_PERSPECTIVE_VIEW
#define ENABLE_UI_EXTRA
#define ENABLE_NEW_CHAT
#define ENABLE_DS_GRADE_MYTH
#define ENABLE_NO_COLLISION
#define ENABLE_INFINITE_RAFINES
#define ENABLE_BIOLOGIST_UI
#define ENABLE_EMPIRE_EFFECT_LIMITED
#define ENABLE_DS_POTION_DIFFRENT
#define ENABLE_NEW_FISHING_SYSTEM
#define WJ_ENABLE_TRADABLE_ICON
#define ENABLE_3D_MODELS_TEXTURE_FIX
#define ENABLE_SAVECAMERA_PREFERENCES
//#define ENABLE_MAP_PERFORMANCE
#define ENABLE_GENDER_ALIGNMENT
#define OUTLINE_NAMES_TEXTLINE
#define ENABLE_MULTI_NAMES
#define NORMAL_ROTATION_SPEED 2400.0f
#define MOUNT_ROTATION_SPEED 1200.0f
#define ENABLE_CHOOSE_DOCTRINE_GUI
#define ENABLE_ITEMSHOP_ITEM
#define SPECIAL_ACTION_START_INDEX 101
#define ENABLE_DS_REFINE_ALL
#define ENABLE_NEW_ATTACK_METHOD
#define ENABLE_BUY_STACK_FROM_SHOP
#ifdef ENABLE_BUY_STACK_FROM_SHOP
#define MULTIPLE_BUY_LIMIT 100
#else
#define MULTIPLE_BUY_LIMIT 0
#endif
#define ENABLE_OPENSHOP_PACKET
#define ENABLE_HWID
#define ENABLE_AFFECT_ALIGNMENT

/* debug
#define __PERFORMANCE_CHECKER__
#define _PACKETDUMP
*/

// #define ENABLE_SLOT_COVER_TRAFFIC_LIGHTS