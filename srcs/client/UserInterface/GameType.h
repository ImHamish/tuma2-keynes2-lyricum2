#pragma once
#include "../GameLib/ItemData.h"

enum EEquipWindowType {
	WND_EQUIP_NORMAL,
	WND_EQUIP_COSTUMES,
#if defined(ENABLE_PENDANT_RENEWAL)
	WND_EQUIP_PENDANT,
#endif
	WND_EQUIP_MAX
};

struct SAffects
{
	enum
	{
		AFFECT_MAX_NUM = 32,
	};

	SAffects() : dwAffects(0) {}
	SAffects(const DWORD & c_rAffects)
	{
		__SetAffects(c_rAffects);
	}
	int operator = (const DWORD & c_rAffects)
	{
		__SetAffects(c_rAffects);
	}

	BOOL IsAffect(BYTE byIndex)
	{
		return dwAffects & (1 << byIndex);
	}

	void __SetAffects(const DWORD & c_rAffects)
	{
		dwAffects = c_rAffects;
	}

	DWORD dwAffects;
};

#if defined(ENABLE_HIDE_COSTUME_SYSTEM)
enum EMaxHiddenParts {
	HIDE_PART_BODY,
	HIDE_PART_HAIR,
#if defined(ENABLE_ACCE_SYSTEM)
	HIDE_PART_SASH,
#endif
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
	HIDE_PART_WEAPON,
#endif
#if defined(ENABLE_AURA_SYSTEM)
	HIDE_PART_AURA,
#endif
#if defined(ENABLE_PETSKIN)
	HIDE_PETSKIN,
#endif
#if defined(ENABLE_MOUNTSKIN)
	HIDE_MOUNTSKIN,
#endif
#if defined(ENABLE_ACCE_SYSTEM)
	HIDE_STOLE_PART,
#endif
	HIDE_PART_MAX,
};
#endif

#ifdef WJ_ENABLE_TRADABLE_ICON
enum ETopWindowTypes
{
	ON_TOP_WND_NONE,
	ON_TOP_WND_SHOP,
	ON_TOP_WND_EXCHANGE,
	ON_TOP_WND_SAFEBOX,
	ON_TOP_WND_PRIVATE_SHOP,
	ON_TOP_WND_ITEM_COMB,
	ON_TOP_WND_PET_FEED,
#if defined(ENABLE_NEW_FISH_EVENT)
	ON_TOP_WND_FISH_EVENT,
#endif
#if defined(ENABLE_CHANGELOOK)
	ON_TOP_WND_CHANGE_LOOK,
#endif
#if defined(USE_ATTR_6TH_7TH)
	ON_TOP_WND_ATTR67,
#endif
	ON_TOP_WND_MAX,
};
#endif

#if defined(USE_ATTR_6TH_7TH)
enum EAttr67
{
	ATTR67_ADD_SLOT_MAX = 1,
};
#endif

extern std::string g_strGuildSymbolPathName;

const DWORD c_Name_Max_Length = 64;
const DWORD c_FileName_Max_Length = 128;
const DWORD c_Short_Name_Max_Length = 32;

const DWORD c_Inventory_Page_Column = 5;
const DWORD c_Inventory_Page_Row = 9;
const DWORD c_Inventory_Page_Size = c_Inventory_Page_Column*c_Inventory_Page_Row; // x*y
#ifdef ENABLE_EXTEND_INVEN_SYSTEM
const DWORD c_Inventory_Page_Count = 4;
#else
const DWORD c_Inventory_Page_Count = 2;
#endif
const DWORD c_ItemSlot_Count = c_Inventory_Page_Size * c_Inventory_Page_Count;
const DWORD c_Equipment_Count = 12;

const DWORD c_Equipment_Start = c_ItemSlot_Count;

const DWORD c_Equipment_Body	= c_Equipment_Start + 0;
const DWORD c_Equipment_Head	= c_Equipment_Start + 1;
const DWORD c_Equipment_Shoes	= c_Equipment_Start + 2;
const DWORD c_Equipment_Wrist	= c_Equipment_Start + 3;
const DWORD c_Equipment_Weapon	= c_Equipment_Start + 4;
const DWORD c_Equipment_Neck	= c_Equipment_Start + 5;
const DWORD c_Equipment_Ear		= c_Equipment_Start + 6;
const DWORD c_Equipment_Unique1	= c_Equipment_Start + 7;
const DWORD c_Equipment_Unique2	= c_Equipment_Start + 8;
const DWORD c_Equipment_Arrow	= c_Equipment_Start + 9;
const DWORD c_Equipment_Shield	= c_Equipment_Start + 10;
#ifdef ENABLE_STOLE_REAL
const DWORD c_Equipment_Stole = c_Equipment_Start + 18;
#endif
#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
const DWORD c_New_Equipment_Start = c_Equipment_Start + 21;
const DWORD c_New_Equipment_Count = 3;
const DWORD c_Equipment_Ring1 = c_New_Equipment_Start + 0;
const DWORD c_Equipment_Ring2 = c_New_Equipment_Start + 1;
const DWORD c_Equipment_Belt  = c_New_Equipment_Start + 2;
#endif
#if defined(ENABLE_PENDANT_RENEWAL)
const uint16_t c_EquipmentTalisman1Start = c_Equipment_Start + CItemData::WEAR_TALISMAN1_ICE;
const uint16_t c_EquipmentTalisman1End = c_Equipment_Start + CItemData::WEAR_TALISMAN1_ELEC;
const uint16_t c_EquipmentTalisman2Start = c_Equipment_Start + CItemData::WEAR_TALISMAN2_ICE;
const uint16_t c_EquipmentTalisman2End = c_Equipment_Start + CItemData::WEAR_TALISMAN2_ELEC;
#endif

enum EDragonSoulDeckType
{
	DS_DECK_1,
	DS_DECK_2,
	DS_DECK_MAX_NUM = 2,
};


#ifdef ENABLE_EXTRA_INVENTORY
const DWORD c_Extra_Inventory_Page_Column = 5;
const DWORD c_Extra_Inventory_Page_Row = 9;
const DWORD c_Extra_Inventory_Page_Size = c_Extra_Inventory_Page_Column * c_Extra_Inventory_Page_Row;
const DWORD c_Extra_Inventory_Page_Count = 20;
const DWORD c_Extra_Inventory_Count = c_Extra_Inventory_Page_Size * c_Extra_Inventory_Page_Count;
const DWORD c_Extra_Inventory_Category_Count = 5;
#endif

enum EDragonSoulGradeTypes
{
	DRAGON_SOUL_GRADE_NORMAL,
	DRAGON_SOUL_GRADE_BRILLIANT,
	DRAGON_SOUL_GRADE_RARE,
	DRAGON_SOUL_GRADE_ANCIENT,
	DRAGON_SOUL_GRADE_LEGENDARY,
#ifdef ENABLE_DS_GRADE_MYTH
	DRAGON_SOUL_GRADE_MYTH,
#endif
	DRAGON_SOUL_GRADE_MAX,
};

enum EDragonSoulStepTypes
{
	DRAGON_SOUL_STEP_LOWEST,
	DRAGON_SOUL_STEP_LOW,
	DRAGON_SOUL_STEP_MID,
	DRAGON_SOUL_STEP_HIGH,
	DRAGON_SOUL_STEP_HIGHEST,
	DRAGON_SOUL_STEP_MAX,
};

#ifdef ENABLE_COSTUME_SYSTEM
	const DWORD c_Costume_Slot_Start	= c_Equipment_Start + 19;	// [주의] 숫자(19) 하드코딩 주의. 현재 서버에서 코스츔 슬롯은 19부터임. 서버 common/length.h 파일의 EWearPositions 열거형 참고.
	const DWORD	c_Costume_Slot_Body		= c_Costume_Slot_Start + 0;
	const DWORD	c_Costume_Slot_Hair		= c_Costume_Slot_Start + 1;
#if defined(USE_MOUNT_COSTUME_SYSTEM)
	const DWORD	c_Costume_Slot_Mount = c_Costume_Slot_Start + 2;
#endif
#ifdef ENABLE_ACCE_SYSTEM
	const DWORD	c_Costume_Slot_Acce		= c_Costume_Slot_Start + 3;
#endif

#if defined(ENABLE_WEAPON_COSTUME_SYSTEM) || defined(ENABLE_ACCE_SYSTEM)
	const DWORD c_Costume_Slot_Count = 4;
#elif defined(USE_MOUNT_COSTUME_SYSTEM)
	const DWORD c_Costume_Slot_Count = 3;
#else
	const DWORD c_Costume_Slot_Count = 2;
#endif

	const DWORD c_Costume_Slot_End		= c_Costume_Slot_Start + c_Costume_Slot_Count;

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	const DWORD	c_Costume_Slot_Weapon	= c_Costume_Slot_End + 1;
#endif
#if defined(ENABLE_PETSKIN)
const DWORD c_Costume_PetSkin = c_Equipment_Start + 26;
#endif
#if defined(ENABLE_MOUNTSKIN)
const DWORD c_Costume_MountSkin = c_Equipment_Start + 27;
#endif
#ifdef ENABLE_COSTUME_EFFECT
const DWORD c_Costume_EffectBody = c_Equipment_Start + 28;
const DWORD c_Costume_EffectWeapon = c_Equipment_Start + 29;
#endif
#if defined(ENABLE_AURA_SYSTEM)
const DWORD c_Costume_Slot_Aura = c_Equipment_Start + 30;
#endif
#endif

// [주의] 숫자(32) 하드코딩 주의. 현재 서버에서 용혼석 슬롯은 32부터임.
// 서버 common/length.h 파일의 EWearPositions 열거형이 32까지 확장될 것을 염두하고(32 이상은 확장 하기 힘들게 되어있음.),
// 그 이후부터를 용혼석 장착 슬롯으로 사용.
const DWORD c_Wear_Max = 64;

const DWORD c_DragonSoul_Equip_Start = c_ItemSlot_Count + c_Wear_Max;
const DWORD c_DragonSoul_Equip_Slot_Max = 6;
const DWORD c_DragonSoul_Equip_End = c_DragonSoul_Equip_Start + c_DragonSoul_Equip_Slot_Max * DS_DECK_MAX_NUM;

// NOTE: 2013년 2월 5일 현재... 용혼석 데크는 2개가 존재하는데, 향후 확장 가능성이 있어서 3개 데크 여유분을 할당 해 둠. 그 뒤 공간은 벨트 인벤토리로 사용
const DWORD c_DragonSoul_Equip_Reserved_Count = c_DragonSoul_Equip_Slot_Max * 3;

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	// 벨트 아이템이 제공하는 인벤토리
	const DWORD c_Belt_Inventory_Slot_Start = c_DragonSoul_Equip_End + c_DragonSoul_Equip_Reserved_Count;
	const DWORD c_Belt_Inventory_Width = 4;
	const DWORD c_Belt_Inventory_Height= 4;
	const DWORD c_Belt_Inventory_Slot_Count = c_Belt_Inventory_Width * c_Belt_Inventory_Height;
	const DWORD c_Belt_Inventory_Slot_End = c_Belt_Inventory_Slot_Start + c_Belt_Inventory_Slot_Count;

	const DWORD c_Inventory_Count	= c_Belt_Inventory_Slot_End;
#else
	const DWORD c_Inventory_Count	= c_DragonSoul_Equip_End;
#endif

// 용혼석 전용 인벤토리
const DWORD c_DragonSoul_Inventory_Start = 0;
const DWORD c_DragonSoul_Inventory_Box_Size = 32;
const DWORD c_DragonSoul_Inventory_Count = 300 + 25 + (CItemData::DS_SLOT_NUM_TYPES * DRAGON_SOUL_GRADE_MAX * c_DragonSoul_Inventory_Box_Size);
const DWORD c_DragonSoul_Inventory_End = c_DragonSoul_Inventory_Start + c_DragonSoul_Inventory_Count;

enum ESlotType
{
	SLOT_TYPE_NONE,
	SLOT_TYPE_INVENTORY,
	SLOT_TYPE_SKILL,
	SLOT_TYPE_EMOTION,
	SLOT_TYPE_SHOP,
	SLOT_TYPE_EXCHANGE_OWNER,
	SLOT_TYPE_EXCHANGE_TARGET,
	SLOT_TYPE_QUICK_SLOT,
	SLOT_TYPE_SAFEBOX,
	SLOT_TYPE_PRIVATE_SHOP,
	SLOT_TYPE_MALL,
	SLOT_TYPE_DRAGON_SOUL_INVENTORY,
#ifdef ENABLE_EXTRA_INVENTORY
	SLOT_TYPE_EXTRA_INVENTORY,
#endif
#ifdef ENABLE_SWITCHBOT_WORLDARD
	SLOT_TYPE_SWITCHBOT,
#endif
#if defined(ENABLE_AURA_SYSTEM)
	SLOT_TYPE_AURA,
#endif
#if defined(ENABLE_NEW_FISH_EVENT)
	SLOT_TYPE_FISH_EVENT,
#endif
#ifdef USE_AUTO_HUNT
    SLOT_TYPE_AUTO,
#endif
    SLOT_TYPE_MAX,
};

#if defined(ENABLE_AURA_SYSTEM)
const uint8_t c_AuraMaxLevel = 250;

enum EAuraRefineInfoSlot {
	AURA_REFINE_INFO_SLOT_CURRENT,
	AURA_REFINE_INFO_SLOT_NEXT,
	AURA_REFINE_INFO_SLOT_EVOLVED,
	AURA_REFINE_INFO_SLOT_MAX
};

enum EAuraWindowType {
	AURA_WINDOW_TYPE_ABSORB,
	AURA_WINDOW_TYPE_GROWTH,
	AURA_WINDOW_TYPE_EVOLVE,
	AURA_WINDOW_TYPE_MAX,
};

enum EAuraSlotType {
	AURA_SLOT_MAIN,
	AURA_SLOT_SUB,
	AURA_SLOT_RESULT,
	AURA_SLOT_MAX
};

enum EAuraRefineInfoType {
	AURA_REFINE_INFO_STEP,
	AURA_REFINE_INFO_LEVEL_MIN,
	AURA_REFINE_INFO_LEVEL_MAX,
	AURA_REFINE_INFO_NEED_EXP,
	AURA_REFINE_INFO_MATERIAL_VNUM,
	AURA_REFINE_INFO_MATERIAL_COUNT,
	AURA_REFINE_INFO_NEED_GOLD,
	AURA_REFINE_INFO_EVOLVE_PCT,
	AURA_REFINE_INFO_MAX
};
#endif

enum EWindows
{
	RESERVED_WINDOW,
	INVENTORY,				// 기본 인벤토리. (45칸 짜리가 2페이지 존재 = 90칸)
	EQUIPMENT,
	SAFEBOX,
	MALL,
	DRAGON_SOUL_INVENTORY,
	BELT_INVENTORY,			// NOTE: W2.1 버전에 새로 추가되는 벨트 슬롯 아이템이 제공하는 벨트 인벤토리
#ifdef ENABLE_EXTRA_INVENTORY
	EXTRA_INVENTORY,
#endif
#ifdef ENABLE_SWITCHBOT_WORLDARD
	SWITCHBOT,
#endif
#if defined(ENABLE_AURA_SYSTEM)
	AURA_REFINE,
#endif
#if defined(USE_ATTR_6TH_7TH)
	ATTR67_ADD,
#endif
	GROUND,					// NOTE: 2013년 2월5일 현재까지 unused.. 왜 있는거지???
	WINDOW_TYPE_MAX,
};

#if defined(ENABLE_NEW_FISH_EVENT)
enum EFishEventInfo {
	FISH_EVENT_SHAPE_NONE,
	FISH_EVENT_SHAPE_1,
	FISH_EVENT_SHAPE_2,
	FISH_EVENT_SHAPE_3,
	FISH_EVENT_SHAPE_4,
	FISH_EVENT_SHAPE_5,
	FISH_EVENT_SHAPE_6,
	FISH_EVENT_SHAPE_7,
	FISH_EVENT_SHAPE_MAX_NUM,
};
#endif

enum EDSInventoryMaxNum
{
	DS_INVENTORY_MAX_NUM = c_DragonSoul_Inventory_Count,
	DS_REFINE_WINDOW_MAX_NUM = 15,
};
#ifdef ENABLE_SWITCHBOT_WORLDARD
enum SwitchBotMaxNum
{
	SWITCHBOT_COUNT = 1,
};
#endif
#pragma pack (push, 1)
#define WORD_MAX 0xffff

typedef struct SItemPos
{
	BYTE window_type;
	WORD cell;
    SItemPos ()
    {
		window_type =     INVENTORY;
		cell = WORD_MAX;
    }
	SItemPos (BYTE _window_type, WORD _cell)
    {
        window_type = _window_type;
        cell = _cell;
    }

	// 기존에 cell의 형을 보면 BYTE가 대부분이지만, oi
	// 어떤 부분은 int, 어떤 부분은 WORD로 되어있어,
	// 가장 큰 자료형인 int로 받는다.
  //  int operator=(const int _cell)
  //  {
		//window_type = INVENTORY;
  //      cell = _cell;
  //      return cell;
  //  }
	bool IsValidCell()
	{
		switch (window_type)
		{
		case INVENTORY:
			return cell < c_Inventory_Count;
			break;
		case EQUIPMENT:
			return cell < c_DragonSoul_Equip_End;
			break;
		case DRAGON_SOUL_INVENTORY:
			return cell < (DS_INVENTORY_MAX_NUM);
			break;
#ifdef ENABLE_EXTRA_INVENTORY
		case EXTRA_INVENTORY:
			return cell < c_Extra_Inventory_Count;
			break;
#endif
#ifdef ENABLE_SWITCHBOT_WORLDARD
		case SWITCHBOT:
			return cell < SWITCHBOT_COUNT;
			break;
#endif
#if defined(USE_ATTR_6TH_7TH)
		case ATTR67_ADD:
			return cell < ATTR67_ADD_SLOT_MAX;
#endif
		default:
			return false;
			break;
		}
	}
	bool IsEquipCell()
	{
		switch (window_type)
		{
		case INVENTORY:
		case EQUIPMENT:
			return (c_Equipment_Start + c_Wear_Max > cell) && (c_Equipment_Start <= cell);
			break;

		case BELT_INVENTORY:
		case DRAGON_SOUL_INVENTORY:
#if defined(ENABLE_EXTRA_INVENTORY)
		case EXTRA_INVENTORY:
#endif
			return false;
			break;

		default:
			return false;
		}
	}

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	bool IsBeltInventoryCell()
	{
		bool bResult = c_Belt_Inventory_Slot_Start <= cell && c_Belt_Inventory_Slot_End > cell;
		return bResult;
	}
#endif

	bool IsNPOS()
	{
		return (window_type == RESERVED_WINDOW && cell == WORD_MAX);
	}

	bool operator==(const struct SItemPos& rhs) const
	{
		return (window_type == rhs.window_type) && (cell == rhs.cell);
	}

	bool operator!=(const struct SItemPos& rhs) const
	{
		return (window_type != rhs.window_type) || (cell != rhs.cell);
	}

	bool operator<(const struct SItemPos& rhs) const
	{
		return (window_type < rhs.window_type) || ((window_type == rhs.window_type) && (cell < rhs.cell));
	}
} TItemPos;

const TItemPos NPOS(RESERVED_WINDOW, WORD_MAX);
#pragma pack(pop)

const DWORD c_QuickBar_Line_Count = 3;
const DWORD c_QuickBar_Slot_Count = 12;

const float c_Idle_WaitTime = 5.0f;

const int c_Monster_Race_Start_Number = 6;
const int c_Monster_Model_Start_Number = 20001;

const float c_fAttack_Delay_Time = 0.2f;
const float c_fHit_Delay_Time = 0.1f;
const float c_fCrash_Wave_Time = 0.2f;
const float c_fCrash_Wave_Distance = 3.0f;

const float c_fHeight_Step_Distance = 50.0f;

enum
{
	DISTANCE_TYPE_FOUR_WAY,
	DISTANCE_TYPE_EIGHT_WAY,
	DISTANCE_TYPE_ONE_WAY,
	DISTANCE_TYPE_MAX_NUM,
};

const float c_fMagic_Script_Version = 1.0f;
const float c_fSkill_Script_Version = 1.0f;
const float c_fMagicSoundInformation_Version = 1.0f;
const float c_fBattleCommand_Script_Version = 1.0f;
const float c_fEmotionCommand_Script_Version = 1.0f;
const float c_fActive_Script_Version = 1.0f;
const float c_fPassive_Script_Version = 1.0f;

// Used by PushMove
const float c_fWalkDistance = 175.0f;
const float c_fRunDistance = 310.0f;

#define FILE_MAX_LEN 128

enum
{
#if defined(USE_SOCKET_MAX_NUM)
	ITEM_SOCKET_SLOT_MAX_NUM			= USE_SOCKET_MAX_NUM,
#else
	ITEM_SOCKET_SLOT_MAX_NUM			= 3,
#endif

	// refactored attribute slot begin
	ITEM_ATTRIBUTE_SLOT_NORM_NUM	= 5,
	ITEM_ATTRIBUTE_SLOT_RARE_NUM	= 2,

	ITEM_ATTRIBUTE_SLOT_NORM_START	= 0,
	ITEM_ATTRIBUTE_SLOT_NORM_END	= ITEM_ATTRIBUTE_SLOT_NORM_START + ITEM_ATTRIBUTE_SLOT_NORM_NUM,

	ITEM_ATTRIBUTE_SLOT_RARE_START	= ITEM_ATTRIBUTE_SLOT_NORM_END,
	ITEM_ATTRIBUTE_SLOT_RARE_END	= ITEM_ATTRIBUTE_SLOT_RARE_START + ITEM_ATTRIBUTE_SLOT_RARE_NUM,

	ITEM_ATTRIBUTE_SLOT_MAX_NUM		= ITEM_ATTRIBUTE_SLOT_RARE_END, // 7
	// refactored attribute slot end
};

#pragma pack(push)
#pragma pack(1)

typedef struct SQuickSlot
{
	uint8_t type;
	uint16_t pos;
} TQuickSlot;

#if defined(USE_ATTR_6TH_7TH)
typedef struct SAttr67AddData
{
	SAttr67AddData() : wRegistItemPos(0), byMaterialCount(0), wSupportItemPos(0), bySupportItemCount(0) {}
	uint16_t wRegistItemPos;
	uint8_t byMaterialCount;
	uint16_t wSupportItemPos;
	uint8_t bySupportItemCount;
} TAttr67AddData;
#endif

typedef struct TPlayerItemAttribute
{
    BYTE        bType;
    short       sValue;
} TPlayerItemAttribute;

typedef struct packet_item
{
	DWORD		vnum;
	WORD count;
	DWORD		flags;
	DWORD		anti_flags;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#if defined(ENABLE_CHANGELOOK)
	uint32_t transmutation;
#endif
#ifdef ATTR_LOCK
	short	lockedattr;
#endif
} TItemData;

#ifdef ENABLE_BUY_WITH_ITEM
typedef struct SShopItemPriceData
{
	DWORD		vnum;
	DWORD		count;
} TShopItemPriceData;
#endif

typedef struct packet_shop_item
{
	DWORD		vnum;
#if defined(ENABLE_NEW_GOLD_LIMIT)
	uint64_t price;
#else
	uint32_t price;
#endif
	WORD count;
#ifdef ENABLE_BUY_WITH_ITEM
	TShopItemPriceData	itemprice[MAX_SHOP_PRICES];
#endif
	BYTE		display_pos;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#if defined(ENABLE_CHANGELOOK)
	uint32_t transmutation;
#endif
#ifdef ATTR_LOCK
	short	lockedattr;
#endif
} TShopItemData;

#if defined(ENABLE_GAYA_RENEWAL)
typedef struct SGemShopItem
{
	BYTE	slotIndex;
	BYTE	status;
	
	DWORD	dwVnum;
	BYTE	bCount;
	DWORD	dwPrice;
} TGemShopItem;
#endif

#if defined(USE_BATTLEPASS)
typedef struct SExtBattlePassRewardItem
{
	DWORD dwVnum;
	DWORD bCount;
} TExtBattlePassRewardItem;

typedef struct SExtBattlePassMissionInfo
{
	BYTE bMissionIndex;
	BYTE bMissionType;
	DWORD dwMissionInfo[3];
	TExtBattlePassRewardItem aRewardList[3];
} TExtBattlePassMissionInfo;

typedef struct SExtBattlePassRanking
{
	BYTE bPos;
	char playerName[24 + 1];
	DWORD dwFinishTime;
} TExtBattlePassRanking;
#endif

#pragma pack(pop)

inline float GetSqrtDistance(int ix1, int iy1, int ix2, int iy2) // By sqrt
{
	float dx, dy;

	dx = float(ix1 - ix2);
	dy = float(iy1 - iy2);

	return sqrtf(dx*dx + dy*dy);
}

// DEFAULT_FONT
void DefaultFont_Startup();
void DefaultFont_Cleanup();
void DefaultFont_SetName(const char * c_szFontName);
CResource* DefaultFont_GetResource();
CResource* DefaultItalicFont_GetResource();
// END_OF_DEFAULT_FONT

void SetGuildSymbolPath(const char * c_szPathName);
const char * GetGuildSymbolFileName(DWORD dwGuildID);
BYTE SlotTypeToInvenType(BYTE bSlotType);
BYTE ApplyTypeToPointType(BYTE bApplyType);
#if defined(ENABLE_AURA_SYSTEM)
#if defined(ENABLE_NEW_GOLD_LIMIT)
extern const uint64_t* GetAuraRefineInfo(uint8_t);
#else
extern const uint32_t* GetAuraRefineInfo(uint8_t);
#endif
#endif
