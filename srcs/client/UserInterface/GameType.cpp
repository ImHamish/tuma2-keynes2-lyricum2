#include "StdAfx.h"
#include "GameType.h"
#include "Packet.h"

std::string g_strResourcePath = "d:/ymir work/";
std::string g_strImagePath = "d:/ymir work/ui/";

std::string g_strGuildSymbolPathName = "mark/10/";

// DEFAULT_FONT
static std::string gs_strDefaultFontName = "굴림체:12.fnt";
static std::string gs_strDefaultItalicFontName = "굴림체:12i.fnt";
static CResource* gs_pkDefaultFont = NULL;
static CResource* gs_pkDefaultItalicFont = NULL;

static bool gs_isReloadDefaultFont = false;

void DefaultFont_Startup()
{
	gs_pkDefaultFont = NULL;
}

void DefaultFont_Cleanup()
{
	if (gs_pkDefaultFont)
		gs_pkDefaultFont->Release();
}

void DefaultFont_SetName(const char * c_szFontName)
{
	gs_strDefaultFontName = c_szFontName;
	gs_strDefaultFontName += ".fnt";

	gs_strDefaultItalicFontName = c_szFontName;
	if(strchr(c_szFontName, ':'))
		gs_strDefaultItalicFontName += "i";
	gs_strDefaultItalicFontName += ".fnt";

	gs_isReloadDefaultFont = true;
}

bool ReloadDefaultFonts()
{
	CResourceManager& rkResMgr = CResourceManager::Instance();

	gs_isReloadDefaultFont = false;

	CResource* pkNewFont = rkResMgr.GetResourcePointer(gs_strDefaultFontName.c_str());
	pkNewFont->AddReference();
	if (gs_pkDefaultFont)
		gs_pkDefaultFont->Release();
	gs_pkDefaultFont = pkNewFont;

	CResource* pkNewItalicFont = rkResMgr.GetResourcePointer(gs_strDefaultItalicFontName.c_str());
	pkNewItalicFont->AddReference();
	if (gs_pkDefaultItalicFont)
		gs_pkDefaultItalicFont->Release();
	gs_pkDefaultItalicFont = pkNewItalicFont;

	return true;
}

CResource* DefaultFont_GetResource()
{
	if (!gs_pkDefaultFont || gs_isReloadDefaultFont)
		ReloadDefaultFonts();
	return gs_pkDefaultFont;
}

CResource* DefaultItalicFont_GetResource()
{
	if (!gs_pkDefaultItalicFont || gs_isReloadDefaultFont)
		ReloadDefaultFonts();
	return gs_pkDefaultItalicFont;
}

// END_OF_DEFAULT_FONT

void SetGuildSymbolPath(const char * c_szPathName)
{
	g_strGuildSymbolPathName = "mark/";
	g_strGuildSymbolPathName += c_szPathName;
	g_strGuildSymbolPathName += "/";
}

const char * GetGuildSymbolFileName(DWORD dwGuildID)
{
	return _getf("%s%03d.jpg", g_strGuildSymbolPathName.c_str(), dwGuildID);
}

BYTE c_aSlotTypeToInvenType[SLOT_TYPE_MAX] =
{
	RESERVED_WINDOW,		// SLOT_TYPE_NONE
	INVENTORY,				// SLOT_TYPE_INVENTORY
	RESERVED_WINDOW,		// SLOT_TYPE_SKILL
	RESERVED_WINDOW,		// SLOT_TYPE_EMOTION
	RESERVED_WINDOW,		// SLOT_TYPE_SHOP
	RESERVED_WINDOW,		// SLOT_TYPE_EXCHANGE_OWNER
	RESERVED_WINDOW,		// SLOT_TYPE_EXCHANGE_TARGET
	RESERVED_WINDOW,		// SLOT_TYPE_QUICK_SLOT
	RESERVED_WINDOW,		// SLOT_TYPE_SAFEBOX	<- SAFEBOX, MALL의 경우 하드 코딩되어있는 LEGACY 코드를 유지함.
	RESERVED_WINDOW,		// SLOT_TYPE_PRIVATE_SHOP
	RESERVED_WINDOW,		// SLOT_TYPE_MALL		<- SAFEBOX, MALL의 경우 하드 코딩되어있는 LEGACY 코드를 유지함.
	DRAGON_SOUL_INVENTORY,	// SLOT_TYPE_DRAGON_SOUL_INVENTORY
#ifdef ENABLE_EXTRA_INVENTORY
	EXTRA_INVENTORY,		// SLOT_TYPE_EXTRA_INVENTORY <- SPECIAL_SLOT
#endif
#ifdef ENABLE_SWITCHBOT_WORLDARD
	SWITCHBOT,				// SLOT_TYPE_SWITCHBOT
#endif
#if defined(ENABLE_AURA_SYSTEM)
	AURA_REFINE,			// SLOT_TYPE_AURA
#endif
#ifdef ENABLE_NEW_FISH_EVENT
    RESERVED_WINDOW,        // SLOT_TYPE_FISH_EVENT
#endif
#ifdef USE_AUTO_HUNT
    RESERVED_WINDOW,        // SLOT_TYPE_AUTO
#endif
};

BYTE SlotTypeToInvenType(BYTE bSlotType)
{
	if (bSlotType >= SLOT_TYPE_MAX)
		return RESERVED_WINDOW;
	else
		return c_aSlotTypeToInvenType[bSlotType];
}
typedef struct SApplyInfo
{
	BYTE	bPointType;
} TApplyInfo;

const TApplyInfo aApplyInfo[CItemData::MAX_APPLY_NUM] =
{
	{ POINT_NONE,						},
	{ POINT_MAX_HP,		        		},
	{ POINT_MAX_SP,		        		},
	{ POINT_HT,			        		},
	{ POINT_IQ,			        		},
	{ POINT_ST,			        		},
	{ POINT_DX,			        		},
	{ POINT_ATT_SPEED,		    		},
	{ POINT_MOV_SPEED,		    		},
	{ POINT_CASTING_SPEED,	    		},
	{ POINT_HP_REGEN,					},
	{ POINT_SP_REGEN,					},
	{ POINT_POISON_PCT,		    		},
	{ POINT_STUN_PCT,		    		},
	{ POINT_SLOW_PCT,		    		},
	{ POINT_CRITICAL_PCT,				},
	{ POINT_PENETRATE_PCT,				},
	{ POINT_ATTBONUS_HUMAN,				},
	{ POINT_ATTBONUS_ANIMAL,			},
	{ POINT_ATTBONUS_ORC,				},
	{ POINT_ATTBONUS_MILGYO,			},
	{ POINT_ATTBONUS_UNDEAD,			},
	{ POINT_ATTBONUS_DEVIL,				},
	{ POINT_STEAL_HP,					},
	{ POINT_STEAL_SP,					},
	{ POINT_MANA_BURN_PCT,				},
	{ POINT_DAMAGE_SP_RECOVER,			},
	{ POINT_BLOCK,		        		},
	{ POINT_DODGE,		        		},
	{ POINT_RESIST_SWORD,				},
	{ POINT_RESIST_TWOHAND,				},
	{ POINT_RESIST_DAGGER,				},
	{ POINT_RESIST_BELL,				},
	{ POINT_RESIST_FAN,					},
	{ POINT_RESIST_BOW,					},
	{ POINT_RESIST_FIRE,				},
	{ POINT_RESIST_ELEC,				},
	{ POINT_RESIST_MAGIC,				},
	{ POINT_RESIST_WIND,				},
	{ POINT_REFLECT_MELEE,				},
	{ POINT_REFLECT_CURSE,				},
	{ POINT_POISON_REDUCE,				},
	{ POINT_KILL_SP_RECOVER,			},
	{ POINT_EXP_DOUBLE_BONUS,			},
	{ POINT_GOLD_DOUBLE_BONUS,			},
	{ POINT_ITEM_DROP_BONUS,			},
	{ POINT_POTION_BONUS,				},
	{ POINT_KILL_HP_RECOVER,			},
	{ POINT_IMMUNE_STUN,				},
	{ POINT_IMMUNE_SLOW,				},
	{ POINT_IMMUNE_FALL,				},
	{ POINT_NONE,						},
	{ POINT_BOW_DISTANCE,				},
	{ POINT_ATT_GRADE_BONUS,			},
	{ POINT_DEF_GRADE_BONUS,			},
	{ POINT_MAGIC_ATT_GRADE_BONUS,		},
	{ POINT_MAGIC_DEF_GRADE_BONUS,		},
	{ POINT_CURSE_PCT,					},
	{ POINT_MAX_STAMINA					},
	{ POINT_ATTBONUS_WARRIOR			},
	{ POINT_ATTBONUS_ASSASSIN			},
	{ POINT_ATTBONUS_SURA				},
	{ POINT_ATTBONUS_SHAMAN				},
	{ POINT_ATTBONUS_MONSTER			},
	{ POINT_ATT_BONUS					},
	{ POINT_MALL_DEFBONUS				},
	{ POINT_MALL_EXPBONUS				},
	{ POINT_MALL_ITEMBONUS				},
	{ POINT_MALL_GOLDBONUS				},
	{ POINT_MAX_HP_PCT					},
	{ POINT_MAX_SP_PCT					},
	{ POINT_SKILL_DAMAGE_BONUS			},
	{ POINT_NORMAL_HIT_DAMAGE_BONUS		},
	{ POINT_SKILL_DEFEND_BONUS			},
	{ POINT_NORMAL_HIT_DEFEND_BONUS		},
	{ POINT_PC_UNUSED01			},
	{ POINT_PC_UNUSED02			},
	{ POINT_NONE,						},
	{ POINT_RESIST_WARRIOR,				},
	{ POINT_RESIST_ASSASSIN,			},
	{ POINT_RESIST_SURA,				},
	{ POINT_RESIST_SHAMAN,				},
	{ POINT_ENERGY						},
	{ POINT_DEF_GRADE					},
	{ POINT_COSTUME_ATTR_BONUS			},
	{ POINT_MAGIC_ATT_BONUS_PER 		},
	{ POINT_MELEE_MAGIC_ATT_BONUS_PER	},
	{ POINT_RESIST_ICE,					},
	{ POINT_RESIST_EARTH,				},
	{ POINT_RESIST_DARK,				},
	{ POINT_RESIST_CRITICAL,			},
	{ POINT_RESIST_PENETRATE,			},
};

BYTE ApplyTypeToPointType(BYTE bApplyType)
{
	if (bApplyType >= CItemData::MAX_APPLY_NUM)
		return POINT_NONE;
	else
		return aApplyInfo[bApplyType].bPointType;
}

#if defined(ENABLE_AURA_SYSTEM)
#if defined(ENABLE_NEW_GOLD_LIMIT)
static uint64_t s_aiAuraRefineInfo[CItemData::AURA_GRADE_MAX_NUM][AURA_REFINE_INFO_MAX] = 
#else
static uint32_t s_aiAuraRefineInfo[CItemData::AURA_GRADE_MAX_NUM][AURA_REFINE_INFO_MAX] = 
#endif
{
//  [ Grade | LevelMin | LevelMax | NeedEXP | EvolMaterial | EvolMaterialCount | EvolCost | EvolPCT ]
	{1,       1,         49,        250,      59996,         20,                 50000000000, 100},
	{2,       50,        99,        500,      59996,         40,                 100000000000, 100},
	{3,       100,       149,       1000,     59996,         60,                 200000000000, 100},
	{4,       150,       199,       2000,     59996,         80,                 300000000000, 100},
	{5,       200,       249,       4000,     59996,         100,                400000000000, 100},
	{6,       250,       250,       0,        0,             0,                  0,            0},
	{0,       0,         0,         0,        0,             0,                  0,            0}
};

#if defined(ENABLE_NEW_GOLD_LIMIT)
const uint64_t* GetAuraRefineInfo(uint8_t bLevel)
#else
const uint32_t* GetAuraRefineInfo(uint8_t bLevel)
#endif
{
	if (bLevel > 250) {
		return NULL;
	}

	for (int32_t i = 0; i < CItemData::AURA_GRADE_MAX_NUM + 1; ++i) {
		if (bLevel >= s_aiAuraRefineInfo[i][AURA_REFINE_INFO_LEVEL_MIN] && bLevel <= s_aiAuraRefineInfo[i][AURA_REFINE_INFO_LEVEL_MAX]) {
			return s_aiAuraRefineInfo[i];
		}
	}

	return NULL;
}
#endif
