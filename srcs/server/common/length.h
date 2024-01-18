#ifndef __INC_METIN_II_LENGTH_H__
#define __INC_METIN_II_LENGTH_H__

#include "CommonDefines.h"
#include "service.h"

#define WORD_MAX 0xffff

#if defined(ENABLE_NEW_GOLD_LIMIT)
#define GOLD_MAX_MAX 1000000000000001LL
#endif

#if defined(USE_ATTR_6TH_7TH)
enum EAttr67Add
{
	ATTR67_ADD_SLOT_MAX = 1,
	ATTR67_MATERIAL_MAX_COUNT = 10,
	ATTR67_SUPPORT_MAX_COUNT = 5,
	ATTR67_SUCCESS_PER_MATERIAL = 2,
	ATTR67_ADD_WAIT_TIME = 60 * 60 * 8,
};
#endif

#if defined(ENABLE_OKEY_CARD_GAME)
enum EOkeyCardGame
{
	MAX_OKEY_CARDS_IN_HAND = 5,
	MAX_OKEY_CARDS_IN_FIELD = 3,
	MAX_OKEY_CARDS = 24,
	OKEY_CARD_PLAY_PRICE = 50000000,
	OKEY_CARD_ITEMVNUM = 79505,
	OKEY_CARD_SET_ITEMVNUM = 79506,
	OKEY_CARD_GOLDEN_PRIZE_ITEMVNUM = 71194,
	OKEY_CARD_SILVER_PRIZE_ITEMVNUM = 71195,
	OKEY_CARD_BRONZE_PRIZE_ITEMVNUM = 71196,
};
#endif

#ifdef ENABLE_REWARD_SYSTEM
enum
{
	REWARD_105 = 1,
	REWARD_PET_115=2,
	REWARD_120=3,
	REWARD_LEGENDARY_SKILL=4,
	REWARD_LEGENDARY_SKILL_SET=5,
	REWARD_KILL_DRAGON=6,
	REWARD_KILL_RAZADOR=7,
	REWARD_KILL_NEMERE=8,
	REWARD_OFFLINESHOP_SLOT=9,
	REWARD_INVENTORY_SLOT=10,
	REWARD_AVERAGE=11,
	REWARD_ELEMENT=12,
	REWARD_BATTLEPASS=13,
	REWARD_CUSTOM_SASH=14,
	REWARD_AURA=15,
	REWARD_ENERGY=16,
	REWARD_112_BIO=17,
	REWARD_120_BIO=18,
	REWARD_LEADER_SHIP=19,
	REWARD_BUFFI_LEGENDARY_SKILL=20,
	REWARD_MAX,
};
#endif

#if defined(ENABLE_WORLDBOSS)
enum EWbInfo {
	PLAYER_CREATE_WB_POINTS = 0,
	MAX_WB_POINTS = 999999999,
	WB_POINTS_ITEMVNUM = 30930,
	WB_MAP_INDEX = 221,
	PVP_LVL_LIMIT = 74,
};
#endif

#if defined(ENABLE_HALLOWEEN_EVENT_2022)
enum EHalloweenInfo {
	CHEST_HALLOWEEN_VNUM = 71052,
	TICKET_JACK_VNUM = 71053,
	HALLOWEEN_SHOP_VNUM1 = 61,
	HALLOWEEN_SHOP_VNUM2 = 62,
	HALLOWEEN_COMMON_DROP_VNUM = 71058,
};

enum ESkullCurrencyInfo {
	PLAYER_CREATE_SKULL = 0,
	MAX_SKULL = 999999999,
	SKULL_ITEMVNUM = 30909,
};
#endif

#if defined(ENABLE_NEW_ANTICHEAT_RULES)
enum EAntiCheatInfo {
	METINSTONE_CHECK_VNUM = 8110,
};
#endif

#if defined(ENABLE_CHANGELOOK)
enum EChangeLookInfo {
	CHANGE_LOOK_WINDOW_MAX_MATERIALS = 3,
	CHANGE_LOOK_TRANSMUTATION_PRICE = 100000000,
};
#endif

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

#if defined(ENABLE_MESSENGER_BLOCK)
enum BlockSystem {
	SYST_BLOCK,
	SYST_FRIEND
};
#endif

#if defined(ENABLE_ANTI_FLOOD)
enum EFloodTypes {
	FLOOD_BUY_SHOP,
	FLOOD_SELL_SHOP,
	FLOOD_USE_ITEM,
	FLOOD_PACKETS,
	FLOOD_CMD,
#if defined(ENABLE_LETTERS_EVENT)
	FLOOD_LETTERS_REWARD,
#endif
#if defined(ENABLE_HIDE_COSTUME_SYSTEM)
	FLOOD_HIDE_PARTS,
#endif
	FLOOD_AGGREGATE_ITEM,
#if defined(ENABLE_PENDANT_RENEWAL)
	COOLDOWN_PENDANT_TYPE,
#endif
#if defined(USE_MOUNT_COSTUME_SYSTEM)
	FLOOD_MOUNT_ITEM,
#endif
#if defined(__ENABLE_NEW_OFFLINESHOP__)
	FLOOD_OFFSHOP_REMOVEITEM,
	FLOOD_OFFSHOP_EDITITEM,
	FLOOD_OFFSHOP_WITHDRAWITEM,
	FLOOD_OFFSHOP_WITHDRAWYANG,
#endif
#if defined(USE_DELAY_ON_AUTOPOTIONS_HP_SP)
	FLOOD_AUTOPOTIONS_HP,
	FLOOD_AUTOPOTIONS_SP,
#endif
#if defined(USE_WHEEL_OF_FORTUNE)
	FLOOD_WHEEL,
#endif
	FLOOD_DROPITEM,
	FLOOD_USE_SWITCHBOT,
#ifdef USE_MULTIPLE_OPENING
    MULTIPLE_OPENING_COOLDOWN,
#endif
	MAX_FLOOD_TYPE
};
#endif

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

#if defined(ENABLE_PENDANT_RENEWAL)
enum EPendantType {
	PENDANT_TYPE_PVM = 0,
	PENDANT_TYPE_PVP,
	PENDANT_TYPE_MAX
};
#endif

enum EMisc {
#if defined(USE_WHEEL_OF_FORTUNE)
	WHEEL_SPIN_COINS_COST = 30,
#endif
	SUMMON_MOB_VALUE3_VNUM = 81,
#if defined(ENABLE_RENEWAL_OX) || defined(USE_NO_ITEM_USE_IN_OX)
	OX_MAP_INDEX = 113,
#endif
#if defined(ENABLE_LETTERS_EVENT)
	FIRST_LETTER_VNUM = 79000,
	MAX_RANGE_LETTERS = 4,
#endif
#if defined(ENABLE_NEW_FISH_EVENT)
	FISH_EVENT_SLOTS_NUM = 24,
	ITEM_FISH_EVENT_BOX = 25106,
	ITEM_FISH_EVENT_BOX_SPECIAL = 25107,
#endif
#ifdef USE_CAPTCHA_SYSTEM
    CAPTCHA_TIME_MIN                        = 600,
    CAPTCHA_TIME_MAX                        = 720,
    CAPTCHA_MAX_ALLOWED_TIME                = 30,
#endif
	ITEM_MAX_COUNT			= 2000,
	MAX_HOST_LENGTH			= 15,
	IP_ADDRESS_LENGTH		= 15,
#ifdef ENABLE_PLAYER_PER_ACCOUNT5
	PLAYER_PER_ACCOUNT		= 5,
#else
	PLAYER_PER_ACCOUNT		= 4,
#endif
	ACCOUNT_STATUS_MAX_LEN	= 8,
	CHARACTER_NAME_MAX_LEN	= 24,
	SHOP_SIGN_MAX_LEN		= 32,
	INVENTORY_PAGE_COLUMN	= 5, // 5 (default)
	INVENTORY_PAGE_ROW		= 9, // 9 (default)
	INVENTORY_PAGE_SIZE		= INVENTORY_PAGE_COLUMN*INVENTORY_PAGE_ROW,
#ifdef ENABLE_EXTEND_INVEN_SYSTEM
	INVENTORY_PAGE_COUNT	= 4, // 2 (default)
#else
	INVENTORY_PAGE_COUNT	= 2, // 2 (default)
#endif

	INVENTORY_MAX_NUM		= INVENTORY_PAGE_SIZE*INVENTORY_PAGE_COUNT, // 90 (default)
#ifdef ENABLE_EXTRA_INVENTORY
	EXTRA_INVENTORY_PAGE_COLUMN = 5,
	EXTRA_INVENTORY_PAGE_ROW = 9,
	EXTRA_INVENTORY_PAGE_SIZE = EXTRA_INVENTORY_PAGE_COLUMN * EXTRA_INVENTORY_PAGE_ROW,
	EXTRA_INVENTORY_PAGE_COUNT = 20,
	EXTRA_INVENTORY_MAX_NUM = EXTRA_INVENTORY_PAGE_SIZE * EXTRA_INVENTORY_PAGE_COUNT,
	EXTRA_INVENTORY_CATEGORY_MAX_NUM = EXTRA_INVENTORY_PAGE_SIZE * 4,
#endif
	
	
	ABILITY_MAX_NUM			= 50,
	EMPIRE_MAX_NUM			= 4,
	BANWORD_MAX_LEN			= 24,
	SMS_MAX_LEN				= 80,
	MOBILE_MAX_LEN			= 32,
	SOCIAL_ID_MAX_LEN		= 18,

	GUILD_NAME_MAX_LEN		= 12,

	SHOP_HOST_ITEM_MAX_NUM	= 40,	/* 호스트의 최대 아이템 개수 */
	SHOP_GUEST_ITEM_MAX_NUM = 18,	/* 게스트의 최대 아이템 개수 */

	SHOP_PRICELIST_MAX_NUM	= 40,	///< 개인상점 가격정보 리스트에서 유지할 가격정보의 최대 갯수

	CHAT_MAX_LEN			= 512,

	QUICKSLOT_MAX_NUM = 36,

	JOURNAL_MAX_NUM			= 2,

	QUERY_MAX_LEN			= 8192,

	FILE_MAX_LEN			= 128,
	PLAYER_EXP_TABLE_MAX	= 120,
	PLAYER_MAX_LEVEL_CONST	= 250,

	GUILD_MAX_LEVEL			= 20,
	MOB_MAX_LEVEL			= 100,

	ATTRIBUTE_MAX_VALUE		= 20,
	CHARACTER_PATH_MAX_NUM	= 64,
	SKILL_MAX_NUM			= 255,
	SKILLBOOK_DELAY_MIN		= 64800,
	SKILLBOOK_DELAY_MAX		= 108000,
	SKILL_MAX_LEVEL			= 40,

	APPLY_NAME_MAX_LEN		= 32,
	EVENT_FLAG_NAME_MAX_LEN = 32,

	MOB_SKILL_MAX_NUM		= 5,

	POINT_MAX_NUM = 255,
	DRAGON_SOUL_BOX_SIZE = 32,
	DRAGON_SOUL_BOX_COLUMN_NUM = 8,
	DRAGON_SOUL_BOX_ROW_NUM = DRAGON_SOUL_BOX_SIZE / DRAGON_SOUL_BOX_COLUMN_NUM,
	DRAGON_SOUL_REFINE_GRID_SIZE = 15,
	MAX_AMOUNT_OF_MALL_BONUS	= 20,
	WEAR_MAX_NUM = 64,
#if defined(ENABLE_GAYA_RENEWAL)
	GEM_SLOTS_MAX_NUM = 9,
	GEM_REFRESH_ITEM_VNUM = 39063,
	GEM_UNLOCK_ITEM_VNUM = 39064,
	GEM_GLIMMERSTONE_VNUM = 50926,
	GEM_REFRESH_PULSE = 1,
	GEM_MAX = 100000,
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
	OFFLINE_SHOP_NAME_MAX_LEN			= 40 + CHARACTER_NAME_MAX_LEN +1,
	OFFLINESHOP_DURATION_MAX_MINUTES	= 8* 24 * 60,
	OFFLINESHOP_MAX_SEARCH_RESULT		= 250,
	OFFLINESHOP_SECONDS_PER_SEARCH=3,
	OFFLINESHOP_SECONDS_DELAY_CHANGE_NAME = 15,
	OFFLINESHOP_SECONDS_DELAY_EDIT_ITEM_PRICE = 10,
	//*new check about auction offers
	OFFLINESHOP_SECONDS_PER_OFFER		= 7,
	OFFLINESHOP_MAX_ITEM_NUM = 160,
#endif
	GOLD_MAX = 2000000000,

	SHOP_TAB_NAME_MAX = 32,
	SHOP_TAB_COUNT_MAX = 3,

	BELT_INVENTORY_SLOT_WIDTH = 4,
	BELT_INVENTORY_SLOT_HEIGHT= 4,

	BELT_INVENTORY_SLOT_COUNT = BELT_INVENTORY_SLOT_WIDTH * BELT_INVENTORY_SLOT_HEIGHT,
#ifdef ENABLE_RANKING
	RANKING_MAX_CATEGORIES = 20,
#endif

/**
	 **** 현재까지 할당 된 아이템 영역 정리 (DB상 Item Position) ****
	+------------------------------------------------------+ 0
	| 캐릭터 기본 인벤토리 (45칸 * 2페이지) 90칸           |
	+------------------------------------------------------+ 90 = INVENTORY_MAX_NUM(90)
	| 캐릭터 장비 창 (착용중인 아이템) 32칸                |
	+------------------------------------------------------+ 122 = INVENTORY_MAX_NUM(90) + WEAR_MAX_NUM(32)
	| 용혼석 장비 창 (착용중인 용혼석) 12칸                |
	+------------------------------------------------------+ 134 = 122 + DS_SLOT_MAX(6) * DRAGON_SOUL_DECK_MAX_NUM(2)
	| 용혼석 장비 창 예약 (아직 미사용) 18칸               |
	+------------------------------------------------------+ 152 = 134 + DS_SLOT_MAX(6) * DRAGON_SOUL_DECK_RESERVED_MAX_NUM(3)
	| 벨트 인벤토리 (벨트 착용시에만 벨트 레벨에 따라 활성)|
	+------------------------------------------------------+ 168 = 152 + BELT_INVENTORY_SLOT_COUNT(16) = INVENTORY_AND_EQUIP_CELL_MAX
	| 미사용                                               |
	+------------------------------------------------------+ ??
*/
#ifdef __ENABLE_NEW_OFFLINESHOP__
#ifdef __ENABLE_CHEQUE_SYSTEM__
	YANG_PER_CHEQUE = 100000000,
#endif
#endif
#ifdef ENABLE_BUY_WITH_ITEM
	MAX_SHOP_PRICES = 5,
#endif
#ifdef ENABLE_HWID
	HWID_LENGTH = 64,
#endif
};

enum EWearPositions
{
	WEAR_BODY,		// 0
	WEAR_HEAD,		// 1
	WEAR_FOOTS,		// 2
	WEAR_WRIST,		// 3
	WEAR_WEAPON,	// 4
	WEAR_NECK,		// 5
	WEAR_EAR,		// 6
	WEAR_UNIQUE1,	// 7
	WEAR_UNIQUE2,	// 8
	WEAR_ARROW,		// 9
	WEAR_SHIELD,	// 10
	WEAR_ABILITY1,  // 11
	WEAR_ABILITY2,  // 12
	WEAR_ABILITY3,  // 13
	WEAR_ABILITY4,  // 14
	WEAR_ABILITY5,  // 15
	WEAR_ABILITY6,  // 16
	WEAR_ABILITY7,  // 17
#ifndef ENABLE_STOLE_REAL
	WEAR_COSTUME_STOLE,
#else
	WEAR_ABILITY8,
#endif
	WEAR_COSTUME_BODY,	// 19
	WEAR_COSTUME_HAIR,	// 20

	WEAR_RING1,			// 21	: 신규 반지슬롯1 (왼쪽)
#if defined(USE_MOUNT_COSTUME_SYSTEM)
	WEAR_COSTUME_MOUNT = WEAR_RING1,
#endif
	WEAR_RING2,			// 22	: 신규 반지슬롯2 (오른쪽)
#ifdef ENABLE_ACCE_SYSTEM
	WEAR_COSTUME_ACCE = WEAR_RING2, // costume_acce == ring2
#endif

	WEAR_BELT,			// 23	: 신규 벨트슬롯

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	WEAR_COSTUME_WEAPON,
#endif

#ifdef ENABLE_PENDANT
	WEAR_PENDANT,		// 24
#endif
#if defined(ENABLE_PETSKIN)
	WEAR_PETSKIN = 26,
#endif
#if defined(ENABLE_MOUNTSKIN)
	WEAR_MOUNTSKIN = 27,
#endif
#ifdef ENABLE_COSTUME_EFFECT
	WEAR_COSTUME_EFFECT_BODY = 28,
	WEAR_COSTUME_EFFECT_WEAPON = 29,
#endif
#if defined(ENABLE_AURA_SYSTEM)
	WEAR_COSTUME_AURA = 30,
#endif
#if defined(ENABLE_PENDANT_RENEWAL)
	WEAR_TALISMAN1_ICE				= 31,
	WEAR_TALISMAN1_FIRE				= 32,
	WEAR_TALISMAN1_WIND				= 33,
	WEAR_TALISMAN1_EARTH			= 34,
	WEAR_TALISMAN1_DARK				= 35,
	WEAR_TALISMAN1_ELEC				= 36,
	WEAR_TALISMAN2_ICE				= 37,
	WEAR_TALISMAN2_FIRE				= 38,
	WEAR_TALISMAN2_WIND				= 39,
	WEAR_TALISMAN2_EARTH			= 40,
	WEAR_TALISMAN2_DARK				= 41,
	WEAR_TALISMAN2_ELEC				= 42,
#endif
	WEAR_MAX						= 64,
};

enum EDragonSoulDeckType
{
	DRAGON_SOUL_DECK_0,
	DRAGON_SOUL_DECK_1,
	DRAGON_SOUL_DECK_MAX_NUM = 2,

	DRAGON_SOUL_DECK_RESERVED_MAX_NUM = 3,	// NOTE: 중요! 아직 사용중이진 않지만, 3페이지 분량을 예약 해 둠. DS DECK을 늘릴 경우 반드시 그 수만큼 RESERVED에서 차감해야 함!
};

enum ESex
{
	SEX_MALE,
	SEX_FEMALE
};

enum EDirection
{
	DIR_NORTH,
	DIR_NORTHEAST,
	DIR_EAST,
	DIR_SOUTHEAST,
	DIR_SOUTH,
	DIR_SOUTHWEST,
	DIR_WEST,
	DIR_NORTHWEST,
	DIR_MAX_NUM
};

#define ABILITY_MAX_LEVEL	10  /* 기술 최대 레벨 */

enum EAbilityDifficulty
{
	DIFFICULTY_EASY,
	DIFFICULTY_NORMAL,
	DIFFICULTY_HARD,
	DIFFICULTY_VERY_HARD,
	DIFFICULTY_NUM_TYPES
};

enum EAbilityCategory
{
	CATEGORY_PHYSICAL,	/* 신체적 어빌리티 */
	CATEGORY_MENTAL,	/* 정신적 어빌리티 */
	CATEGORY_ATTRIBUTE,	/* 능력 어빌리티 */
	CATEGORY_NUM_TYPES
};

enum ESkillGroups
{
	SKILL_GROUP_MAX_NUM = 2,
};

enum ERaceFlags
{
	RACE_FLAG_ANIMAL	= (1 << 0),
	RACE_FLAG_UNDEAD	= (1 << 1),
	RACE_FLAG_DEVIL		= (1 << 2),
	RACE_FLAG_HUMAN		= (1 << 3),
	RACE_FLAG_ORC		= (1 << 4),
	RACE_FLAG_MILGYO	= (1 << 5),
	RACE_FLAG_INSECT	= (1 << 6),
	RACE_FLAG_FIRE		= (1 << 7),
	RACE_FLAG_ICE		= (1 << 8),
	RACE_FLAG_DESERT	= (1 << 9),
	RACE_FLAG_TREE		= (1 << 10),
#ifdef ELEMENT_TARGET
	RACE_FLAG_ATT_ELEC	= (1 << 11),
	RACE_FLAG_ATT_FIRE	= (1 << 12),
	RACE_FLAG_ATT_ICE	= (1 << 13),
	RACE_FLAG_ATT_WIND	= (1 << 14),
	RACE_FLAG_ATT_EARTH	= (1 << 15),
	RACE_FLAG_ATT_DARK	= (1 << 16),
#endif
};

enum ELoads
{
	LOAD_NONE,
	LOAD_LIGHT,
	LOAD_NORMAL,
	LOAD_HEAVY,
	LOAD_MASSIVE
};

enum
{
	QUICKSLOT_TYPE_NONE,
	QUICKSLOT_TYPE_ITEM,
	QUICKSLOT_TYPE_SKILL,
	QUICKSLOT_TYPE_COMMAND,
#if defined(ENABLE_EXTRA_INVENTORY)
	QUICKSLOT_TYPE_ITEM_EXTRA,
#endif
	QUICKSLOT_TYPE_MAX_NUM,
};

enum EParts
{
	PART_MAIN,
	PART_WEAPON,
	PART_HEAD,
	PART_HAIR,

#ifdef ENABLE_ACCE_SYSTEM
	PART_ACCE,
#endif
#ifdef ENABLE_COSTUME_EFFECT
	PART_EFFECT_BODY,
	PART_EFFECT_WEAPON,
#endif
#if defined(ENABLE_AURA_SYSTEM)
	PART_AURA,
#endif
	PART_MAX_NUM,
	PART_WEAPON_SUB,
};

enum EChatType
{
	CHAT_TYPE_TALKING,	/* 그냥 채팅 */
	CHAT_TYPE_INFO,	/* 정보 (아이템을 집었다, 경험치를 얻었다. 등) */
	CHAT_TYPE_NOTICE,	/* 공지사항 */
	CHAT_TYPE_PARTY,	/* 파티말 */
	CHAT_TYPE_GUILD,	/* 길드말 */
	CHAT_TYPE_COMMAND,	/* 일반 명령 */
	CHAT_TYPE_SHOUT,	/* 외치기 */
	CHAT_TYPE_WHISPER,
	CHAT_TYPE_BIG_NOTICE,
#ifdef ENABLE_DICE_SYSTEM
	CHAT_TYPE_DICE_INFO, //11
#endif
#ifdef ENABLE_NEW_CHAT
	CHAT_TYPE_INFO_EXP,
	CHAT_TYPE_INFO_ITEM,
	CHAT_TYPE_INFO_VALUE,
#endif
	CHAT_TYPE_DIALOG,
	CHAT_TYPE_MAX_NUM
};

enum EWhisperType
{
	WHISPER_TYPE_NORMAL = 0,
	WHISPER_TYPE_NOT_EXIST = 1,
	WHISPER_TYPE_TARGET_BLOCKED = 2,
	WHISPER_TYPE_SENDER_BLOCKED = 3,
	WHISPER_TYPE_ERROR = 4,
	WHISPER_TYPE_GM = 5,
#if defined(BL_OFFLINE_MESSAGE)
	WHISPER_TYPE_OFFLINE = 6,
#endif
#if defined(ENABLE_MESSENGER_BLOCK)
	WHISPER_TYPE_FRIEND_BLOCKED = 7,
#endif
	WHISPER_TYPE_SYSTEM = 0xFF
};

enum ECharacterPosition
{
	POSITION_GENERAL,
	POSITION_BATTLE,
	POSITION_DYING,
	POSITION_SITTING_CHAIR,
	POSITION_SITTING_GROUND,
	POSITION_INTRO,
	POSITION_MAX_NUM
};

enum EGMLevels
{
	GM_PLAYER,
	GM_LOW_WIZARD,
	GM_WIZARD,
	GM_HIGH_WIZARD,
	GM_GOD,
	GM_IMPLEMENTOR,
	GM_DISABLE,
};

enum EMobRank
{
	MOB_RANK_PAWN,
	MOB_RANK_S_PAWN,
	MOB_RANK_KNIGHT,
	MOB_RANK_S_KNIGHT,
	MOB_RANK_BOSS,
	MOB_RANK_KING,
	MOB_RANK_MAX_NUM
};

enum ECharType
{
	CHAR_TYPE_MONSTER,
	CHAR_TYPE_NPC,
	CHAR_TYPE_STONE,
	CHAR_TYPE_WARP,
	CHAR_TYPE_DOOR,
	CHAR_TYPE_BUILDING,
	CHAR_TYPE_PC,
	CHAR_TYPE_POLYMORPH_PC,
	CHAR_TYPE_HORSE,
	CHAR_TYPE_GOTO
};

enum EBattleType
{
	BATTLE_TYPE_MELEE,
	BATTLE_TYPE_RANGE,
	BATTLE_TYPE_MAGIC,
	BATTLE_TYPE_SPECIAL,
	BATTLE_TYPE_POWER,
	BATTLE_TYPE_TANKER,
	BATTLE_TYPE_SUPER_POWER,
	BATTLE_TYPE_SUPER_TANKER,
	BATTLE_TYPE_MAX_NUM
};

enum EApplyTypes
{
	APPLY_NONE,			// 0
	APPLY_MAX_HP,		// 1
	APPLY_MAX_SP,		// 2
	APPLY_CON,			// 3
	APPLY_INT,			// 4
	APPLY_STR,			// 5
	APPLY_DEX,			// 6
	APPLY_ATT_SPEED,	// 7
	APPLY_MOV_SPEED,	// 8
	APPLY_CAST_SPEED,	// 9
	APPLY_HP_REGEN,		// 10
	APPLY_SP_REGEN,		// 11
	APPLY_POISON_PCT,	// 12
	APPLY_STUN_PCT,		// 13
	APPLY_SLOW_PCT,		// 14
	APPLY_CRITICAL_PCT,		// 15
	APPLY_PENETRATE_PCT,	// 16
	APPLY_ATTBONUS_HUMAN,	// 17
	APPLY_ATTBONUS_ANIMAL,	// 18
	APPLY_ATTBONUS_ORC,		// 19
	APPLY_ATTBONUS_MILGYO,	// 20
	APPLY_ATTBONUS_UNDEAD,	// 21
	APPLY_ATTBONUS_DEVIL,	// 22
	APPLY_STEAL_HP,			// 23
	APPLY_STEAL_SP,			// 24
	APPLY_MANA_BURN_PCT,	// 25
	APPLY_DAMAGE_SP_RECOVER,	// 26
	APPLY_BLOCK,			// 27
	APPLY_DODGE,			// 28
	APPLY_RESIST_SWORD,		// 29
	APPLY_RESIST_TWOHAND,	// 30
	APPLY_RESIST_DAGGER,	// 31
	APPLY_RESIST_BELL,		// 32
	APPLY_RESIST_FAN,		// 33
	APPLY_RESIST_BOW,		// 34
	APPLY_RESIST_FIRE,		// 35
	APPLY_RESIST_ELEC,		// 36
	APPLY_RESIST_MAGIC,		// 37
	APPLY_RESIST_WIND,		// 38
	APPLY_REFLECT_MELEE,	// 39
	APPLY_REFLECT_CURSE,	// 40
	APPLY_POISON_REDUCE,	// 41
	APPLY_KILL_SP_RECOVER,	// 42
	APPLY_EXP_DOUBLE_BONUS,	// 43
	APPLY_GOLD_DOUBLE_BONUS,	// 44
	APPLY_ITEM_DROP_BONUS,	// 45
	APPLY_POTION_BONUS,		// 46
	APPLY_KILL_HP_RECOVER,	// 47
	APPLY_IMMUNE_STUN,		// 48
	APPLY_IMMUNE_SLOW,		// 49
	APPLY_IMMUNE_FALL,		// 50
	APPLY_SKILL,			// 51
	APPLY_BOW_DISTANCE,		// 52
	APPLY_ATT_GRADE_BONUS,	// 53
	APPLY_DEF_GRADE_BONUS,	// 54
	APPLY_MAGIC_ATT_GRADE,	// 55
	APPLY_MAGIC_DEF_GRADE,	// 56
	APPLY_CURSE_PCT,		// 57
	APPLY_MAX_STAMINA,		// 58
	APPLY_ATTBONUS_WARRIOR,	// 59
	APPLY_ATTBONUS_ASSASSIN,	// 60
	APPLY_ATTBONUS_SURA,	// 61
	APPLY_ATTBONUS_SHAMAN,	// 62
	APPLY_ATTBONUS_MONSTER,	// 63
	APPLY_MALL_ATTBONUS,			// 64 공격력 +x%
	APPLY_MALL_DEFBONUS,			// 65 방어력 +x%
	APPLY_MALL_EXPBONUS,			// 66 경험치 +x%
	APPLY_MALL_ITEMBONUS,			// 67 아이템 드롭율 x/10배
	APPLY_MALL_GOLDBONUS,			// 68 돈 드롭율 x/10배
	APPLY_MAX_HP_PCT,				// 69 최대 생명력 +x%
	APPLY_MAX_SP_PCT,				// 70 최대 정신력 +x%
	APPLY_SKILL_DAMAGE_BONUS,		// 71 스킬 데미지 * (100+x)%
	APPLY_NORMAL_HIT_DAMAGE_BONUS,	// 72 평타 데미지 * (100+x)%
	APPLY_SKILL_DEFEND_BONUS,		// 73 스킬 데미지 방어 * (100-x)%
	APPLY_NORMAL_HIT_DEFEND_BONUS,	// 74 평타 데미지 방어 * (100-x)%
	APPLY_PC_UNUSED01,		// 75 PC방 아이템 EXP 보너스
	APPLY_PC_UNUSED02,		// 76 PC방 아이템 드롭율 보너스

	APPLY_EXTRACT_HP_PCT,			// 77 사용시 HP 소모

	APPLY_RESIST_WARRIOR,			// 78 무사에게 저항
	APPLY_RESIST_ASSASSIN,			// 79 자객에게 저항
	APPLY_RESIST_SURA,				// 80 수라에게 저항
	APPLY_RESIST_SHAMAN,			// 81 무당에게 저항
	APPLY_ENERGY,					// 82 기력
	APPLY_DEF_GRADE,				// 83 방어력. DEF_GRADE_BONUS는 클라에서 두배로 보여지는 의도된 버그(...)가 있다.
	APPLY_COSTUME_ATTR_BONUS,		// 84 코스튬 아이템에 붙은 속성치 보너스
	APPLY_MAGIC_ATTBONUS_PER,		// 85 마법 공격력 +x%
	APPLY_MELEE_MAGIC_ATTBONUS_PER,			// 86 마법 + 밀리 공격력 +x%

	APPLY_RESIST_ICE,		// 87 냉기 저항
	APPLY_RESIST_EARTH,		// 88 대지 저항
	APPLY_RESIST_DARK,		// 89 어둠 저항

	APPLY_ANTI_CRITICAL_PCT,	//90 크리티컬 저항
	APPLY_ANTI_PENETRATE_PCT,	//91 관통타격 저항

#ifdef ELEMENT_NEW_BONUSES
#ifdef ENABLE_NEW_BONUS_TALISMAN
	APPLY_ATTBONUS_IRR_SPADA		= 97,
	APPLY_ATTBONUS_IRR_SPADONE		= 98,
	APPLY_ATTBONUS_IRR_PUGNALE		= 99,
	APPLY_ATTBONUS_IRR_FRECCIA		= 100,
	APPLY_ATTBONUS_IRR_VENTAGLIO	= 101,
	APPLY_ATTBONUS_IRR_CAMPANA		= 102,
	APPLY_RESIST_MEZZIUOMINI		= 103,
	APPLY_DEF_TALISMAN				= 104,
	APPLY_ATTBONUS_INSECT			= 105,
	APPLY_ATTBONUS_DESERT			= 106,
	APPLY_ATTBONUS_FORT_ZODIAC		= 107,
#endif
	APPLY_ATTBONUS_ELEC				= 108,
	APPLY_ATTBONUS_FIRE				= 109,
	APPLY_ATTBONUS_ICE				= 110,
	APPLY_ATTBONUS_WIND				= 111,
	APPLY_ATTBONUS_EARTH			= 112,
	APPLY_ATTBONUS_DARK				= 113,
#endif

#ifdef ENABLE_ACCE_SYSTEM
	APPLY_ACCEDRAIN_RATE			= 114,			//115
#endif
#ifdef ENABLE_STRONG_METIN
	APPLY_ATTBONUS_METIN 			= 116,
#endif
#ifdef ENABLE_STRONG_BOSS
	APPLY_ATTBONUS_BOSS 			= 117,
#endif
#ifdef ENABLE_RESIST_MONSTER
	APPLY_RESIST_MONSTER			= 118,
#endif
#ifdef ENABLE_MEDI_PVM
	APPLY_ATTBONUS_MEDI_PVM = 119,
#endif
	APPLY_PVM_CRITICAL_PCT = 120,
#ifdef ENABLE_NEW_COMMON_BONUSES
	APPLY_DOUBLE_DROP_ITEM = 122,
	APPLY_IRR_WEAPON_DEFENSE = 123,
#endif
	APPLY_DESIGN_1 = 124,
	APPLY_FISHING_RARE = 125,
#ifdef ENABLE_NEW_USE_POTION
	APPLY_PARTY_DROPEXP = 126,
#endif
#if defined(USE_MOUNT_COSTUME_SYSTEM)
	APPLY_MOUNT = 127, 							// 127
#else
	APPLY_UNUSED127 = 127,						// 127
#endif
	APPLY_ATT_RACES = 128,
	APPLY_RES_RACES = 129,
#if defined(ENABLE_PETSKIN)
	APPLY_PETSKIN = 130,						// 130
#else
	APPLY_UNUSED130 = 130,						// 130
#endif
#if defined(ENABLE_MOUNTSKIN)
	APPLY_MOUNTSKIN = 131,						// 131
#else
	APPLY_UNUSED131 = 131,						// 131
#endif
#if defined(ENABLE_HALLOWEEN_EVENT_2022) && !defined(USE_NO_HALLOWEEN_EVENT_2022_BONUSES)
    APPLY_FEAR                                  = 132,
    APPLY_RESISTANCE_FEAR                       = 133,
#else
    APPLY_UNUSED132                             = 132,
    APPLY_UNUSED133                             = 133,
#endif
#if defined(USE_ANTI_EXP)
	APPLY_ANTI_EXP			= 134,
#else
	APPLY_UNUSED134			= 134,
#endif
	MAX_APPLY_NUM = 135,
};

enum EOnClickEvents
{
	ON_CLICK_NONE,
	ON_CLICK_SHOP,
	ON_CLICK_TALK,
	ON_CLICK_MAX_NUM
};

enum EOnIdleEvents
{
	ON_IDLE_NONE,
	ON_IDLE_GENERAL,
	ON_IDLE_MAX_NUM
};

enum EWindows
{
	RESERVED_WINDOW,
	INVENTORY,
	EQUIPMENT,
	SAFEBOX,
	MALL,
	DRAGON_SOUL_INVENTORY,
	BELT_INVENTORY,
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
	GROUND,
#ifdef __ENABLE_NEW_OFFLINESHOP__
	NEW_OFFSHOP,
	SHOP_SAFEBOX,
#endif
	WINDOW_TYPE_MAX,
};

enum EMobSizes
{
	MOBSIZE_RESERVED,
	MOBSIZE_SMALL,
	MOBSIZE_MEDIUM,
	MOBSIZE_BIG
};

enum EAIFlags
{
	AIFLAG_AGGRESSIVE					= (1 << 0),
	AIFLAG_NOMOVE						= (1 << 1),
	AIFLAG_COWARD						= (1 << 2),
	AIFLAG_NOATTACKSHINSU				= (1 << 3),
	AIFLAG_NOATTACKJINNO				= (1 << 4),
	AIFLAG_NOATTACKCHUNJO				= (1 << 5),
	AIFLAG_ATTACKMOB					= (1 << 6 ),
	AIFLAG_BERSERK						= (1 << 7),
	AIFLAG_STONESKIN					= (1 << 8),
	AIFLAG_GODSPEED						= (1 << 9),
	AIFLAG_DEATHBLOW					= (1 << 10),
	AIFLAG_REVIVE						= (1 << 11),
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
	AIFLAG_SPAWN_ANIMATION				= (1 << 12),
	AIFLAG_HALLOWEEN					= (1 << 13),
#endif
#if defined(ENABLE_WORLDBOSS)
	AIFLAG_WORLDBOSS					= (1 << 14),
#endif
#if defined(USE_SPECIAL_EXP_FLAG)
	AIFLAG_SPECIALEXP					= (1 << 15),
#endif
};

enum EMobStatType
{
	MOB_STATTYPE_POWER,
	MOB_STATTYPE_TANKER,
	MOB_STATTYPE_SUPER_POWER,
	MOB_STATTYPE_SUPER_TANKER,
	MOB_STATTYPE_RANGE,
	MOB_STATTYPE_MAGIC,
	MOB_STATTYPE_MAX_NUM
};

enum EImmuneFlags
{
	IMMUNE_STUN		= (1 << 0),
	IMMUNE_SLOW		= (1 << 1),
	IMMUNE_FALL		= (1 << 2),
	IMMUNE_CURSE	= (1 << 3),
	IMMUNE_POISON	= (1 << 4),
	IMMUNE_TERROR	= (1 << 5),
	IMMUNE_REFLECT	= (1 << 6),
};

enum EMobEnchants
{
	MOB_ENCHANT_CURSE,
	MOB_ENCHANT_SLOW,
	MOB_ENCHANT_POISON,
	MOB_ENCHANT_STUN,
	MOB_ENCHANT_CRITICAL,
	MOB_ENCHANT_PENETRATE,
	MOB_ENCHANTS_MAX_NUM
};

enum EMobResists
{
	MOB_RESIST_SWORD,
	MOB_RESIST_TWOHAND,
	MOB_RESIST_DAGGER,
	MOB_RESIST_BELL,
	MOB_RESIST_FAN,
	MOB_RESIST_BOW,
	MOB_RESIST_FIRE,
	MOB_RESIST_ELECT,
	MOB_RESIST_MAGIC,
	MOB_RESIST_WIND,
	MOB_RESIST_POISON,
	MOB_RESISTS_MAX_NUM
};

enum
{
	SKILL_ATTR_TYPE_NORMAL = 1,
	SKILL_ATTR_TYPE_MELEE,
	SKILL_ATTR_TYPE_RANGE,
	SKILL_ATTR_TYPE_MAGIC
		/*
		   SKILL_ATTR_TYPE_FIRE,
		   SKILL_ATTR_TYPE_ICE,
		   SKILL_ATTR_TYPE_ELEC,
		   SKILL_ATTR_TYPE_DARK,
		   */
};

enum
{
	SKILL_NORMAL,
	SKILL_MASTER,
	SKILL_GRAND_MASTER,
	SKILL_PERFECT_MASTER,
};

enum EGuildWarType
{
	GUILD_WAR_TYPE_FIELD,
	GUILD_WAR_TYPE_BATTLE,
	GUILD_WAR_TYPE_FLAG,
	GUILD_WAR_TYPE_MAX_NUM
};

enum EGuildWarState
{
	GUILD_WAR_NONE,
	GUILD_WAR_SEND_DECLARE,
	GUILD_WAR_REFUSE,
	GUILD_WAR_RECV_DECLARE,
	GUILD_WAR_WAIT_START,
	GUILD_WAR_CANCEL,
	GUILD_WAR_ON_WAR,
	GUILD_WAR_END,
	GUILD_WAR_OVER,
	GUILD_WAR_RESERVE,

	GUILD_WAR_DURATION = 30*60, // 1시간
	GUILD_WAR_WIN_POINT = 1000,
	GUILD_WAR_LADDER_HALF_PENALTY_TIME = 12*60*60,
};

enum EAttributeSet
{
	ATTRIBUTE_SET_WEAPON,
	ATTRIBUTE_SET_BODY,
	ATTRIBUTE_SET_WRIST,
	ATTRIBUTE_SET_FOOTS,
	ATTRIBUTE_SET_NECK,
	ATTRIBUTE_SET_HEAD,
	ATTRIBUTE_SET_SHIELD,
	ATTRIBUTE_SET_EAR,
#if defined(ENABLE_PENDANT) && defined(ENABLE_NEW_BONUS_TALISMAN)
	ATTRIBUTE_SET_PENDANT,
#endif
#ifdef ENABLE_ATTR_COSTUMES
	ATTRIBUTE_SET_COSTUME_BODY,
	ATTRIBUTE_SET_COSTUME_HAIR,
	ATTRIBUTE_SET_COSTUME_WEAPON,
#ifdef ENABLE_STOLE_COSTUME
	ATTRIBUTE_SET_COSTUME_STOLE,
#endif
#endif
	ATTRIBUTE_SET_MAX_NUM
};

enum EPrivType
{
	PRIV_NONE,
	PRIV_ITEM_DROP,
	PRIV_GOLD_DROP,
	PRIV_GOLD10_DROP,
	PRIV_EXP_PCT,
	MAX_PRIV_NUM,
};

enum EMoneyLogType
{
	MONEY_LOG_RESERVED,
	MONEY_LOG_MONSTER,
	MONEY_LOG_SHOP,
	MONEY_LOG_REFINE,
	MONEY_LOG_QUEST,
	MONEY_LOG_GUILD,
	MONEY_LOG_MISC,
	MONEY_LOG_MONSTER_KILL,
	MONEY_LOG_DROP,
	MONEY_LOG_TYPE_MAX_NUM,
};

enum SPECIAL_EFFECT
{
	SE_NONE,

	SE_HPUP_RED,
	SE_SPUP_BLUE,
	SE_SPEEDUP_GREEN,
	SE_DXUP_PURPLE,
	SE_CRITICAL,
	SE_PENETRATE,
	SE_BLOCK,
	SE_DODGE,
	SE_CHINA_FIREWORK,
	SE_SPIN_TOP,
	SE_SUCCESS,
	SE_FAIL,
	SE_FR_SUCCESS,
	SE_LEVELUP_ON_14_FOR_GERMANY,
	SE_LEVELUP_UNDER_15_FOR_GERMANY,
	SE_PERCENT_DAMAGE1,
	SE_PERCENT_DAMAGE2,
	SE_PERCENT_DAMAGE3,

	SE_AUTO_HPUP,
	SE_AUTO_SPUP,

	SE_EQUIP_RAMADAN_RING,		// 라마단 초승달의 반지(71135) 착용할 때 이펙트 (발동이펙트임, 지속이펙트 아님)
	SE_EQUIP_HALLOWEEN_CANDY,		// 할로윈 사탕을 착용(-_-;)한 순간에 발동하는 이펙트
	SE_EQUIP_HAPPINESS_RING,		// 크리스마스 행복의 반지(71143) 착용할 때 이펙트 (발동이펙트임, 지속이펙트 아님)
	SE_EQUIP_LOVE_PENDANT,		// 발렌타인 사랑의 팬던트(71145) 착용할 때 이펙트 (발동이펙트임, 지속이펙트 아님)

#ifdef ENABLE_ACCE_SYSTEM
	SE_EFFECT_ACCE_SUCCEDED,
	SE_EFFECT_ACCE_EQUIP,
#endif
#ifdef __VERSION_162__
	SE_EFFECT_HEALER,
#endif
#ifdef __EFFETTO_MANTELLO__
	SE_MANTELLO,
#endif
#if defined(USE_BATTLEPASS)
	SE_EFFECT_BP_MISSION_COMPLETED,
	SE_EFFECT_BP_COMPLETED,
#endif
};

#include "item_length.h"

enum EDragonSoulRefineWindowSize
{
	DRAGON_SOUL_REFINE_GRID_MAX = 15,
};

enum EMisc2
{
	DRAGON_SOUL_EQUIP_SLOT_START = INVENTORY_MAX_NUM + WEAR_MAX_NUM, // 219
	DRAGON_SOUL_EQUIP_SLOT_END = DRAGON_SOUL_EQUIP_SLOT_START + (DS_SLOT_MAX * DRAGON_SOUL_DECK_MAX_NUM), // 224
	DRAGON_SOUL_EQUIP_RESERVED_SLOT_END = DRAGON_SOUL_EQUIP_SLOT_END + (DS_SLOT_MAX * DRAGON_SOUL_DECK_RESERVED_MAX_NUM),

	BELT_INVENTORY_SLOT_START = DRAGON_SOUL_EQUIP_RESERVED_SLOT_END,
	BELT_INVENTORY_SLOT_END = BELT_INVENTORY_SLOT_START + BELT_INVENTORY_SLOT_COUNT,

	INVENTORY_AND_EQUIP_SLOT_MAX = BELT_INVENTORY_SLOT_END,
};

#ifdef ENABLE_SWITCHBOT_WORLDARD
enum SwitchBotMaxNum
{
	SWITCHBOT_SLOT_MAX = 1,
};
#endif

#pragma pack(push, 1)

typedef struct SItemPos
{
	BYTE window_type;
	WORD cell;
    SItemPos ()
    {
        window_type = INVENTORY;
		cell = WORD_MAX;
    }

	SItemPos (BYTE _window_type, WORD _cell)
    {
        window_type = _window_type;
        cell = _cell;
    }

	bool IsValidItemPosition() const
	{
		switch (window_type)
		{
		case RESERVED_WINDOW:
			return false;
		case INVENTORY:
		case EQUIPMENT:
		case BELT_INVENTORY:
			return cell < INVENTORY_AND_EQUIP_SLOT_MAX;
		case DRAGON_SOUL_INVENTORY:
			return cell < (DRAGON_SOUL_INVENTORY_MAX_NUM);
		// 동적으로 크기가 정해지는 window는 valid 체크를 할 수가 없다.
		case SAFEBOX:
		case MALL:
			return false;
#ifdef ENABLE_EXTRA_INVENTORY
		case EXTRA_INVENTORY:
			return cell < (EXTRA_INVENTORY_MAX_NUM);
#endif
#ifdef ENABLE_SWITCHBOT_WORLDARD
		case SWITCHBOT:
			return cell < SWITCHBOT_SLOT_MAX;
#endif
#if defined(USE_ATTR_6TH_7TH)
		case ATTR67_ADD:
			return cell < ATTR67_ADD_SLOT_MAX;
#endif
		default:
			return false;
		}
		return false;
	}

	bool IsEquipPosition() const
	{
		return ((INVENTORY == window_type || EQUIPMENT == window_type) && cell >= INVENTORY_MAX_NUM && cell < INVENTORY_MAX_NUM + WEAR_MAX_NUM)
			|| IsDragonSoulEquipPosition();
	}

	bool IsDragonSoulEquipPosition() const
	{
#ifdef ENABLE_EXTRA_INVENTORY
		if (window_type == EXTRA_INVENTORY)
			return false;
#endif
		
		return (DRAGON_SOUL_EQUIP_SLOT_START <= cell) && (DRAGON_SOUL_EQUIP_SLOT_END > cell);
	}

	bool IsBeltInventoryPosition() const
	{
#ifdef ENABLE_EXTRA_INVENTORY
		if (window_type == EXTRA_INVENTORY)
			return false;
#endif
		
		return (BELT_INVENTORY_SLOT_START <= cell) && (BELT_INVENTORY_SLOT_END > cell);
	}
	bool IsDefaultInventoryPosition() const
	{
		return INVENTORY == window_type && cell < INVENTORY_MAX_NUM;
	}

#ifdef ENABLE_EXTRA_INVENTORY
    bool IsDefaultExtraInventoryPosition() const
    {
        return EXTRA_INVENTORY == window_type && cell < EXTRA_INVENTORY_MAX_NUM;
    }
#endif

#if defined(ENABLE_SWITCHBOT_WORLDARD)
	bool IsSwitchBotPosition() const
	{
		return SWITCHBOT == window_type && cell < SWITCHBOT_SLOT_MAX;
	}
#endif

	bool operator==(const struct SItemPos& rhs) const
	{
		return (window_type == rhs.window_type) && (cell == rhs.cell);
	}
	bool operator<(const struct SItemPos& rhs) const
	{
		return (window_type < rhs.window_type) || ((window_type == rhs.window_type) && (cell < rhs.cell));
	}

#if defined(ENABLE_AURA_SYSTEM)
	bool operator!=(const struct SItemPos& rhs) const
	{
		return (window_type != rhs.window_type) || (cell != rhs.cell);
	}
#endif
	
	bool IsSamePosition(const SItemPos& other) const
	{
		return *this == other
			|| ((INVENTORY == window_type || EQUIPMENT == window_type)
				&& (INVENTORY == other.window_type || EQUIPMENT == other.window_type)
				&& cell == other.cell);
	}
#ifdef USE_ATTR_6TH_7TH
	bool IsUse67AttrPosition() const
	{
		return ATTR67_ADD == window_type && cell < ATTR67_ADD_SLOT_MAX;
	}
#endif

} TItemPos;

const TItemPos NPOS (RESERVED_WINDOW, WORD_MAX);

typedef enum
{
	SHOP_COIN_TYPE_GOLD, // DEFAULT VALUE
	SHOP_COIN_TYPE_SECONDARY_COIN,
} EShopCoinType;

#ifdef ENABLE_STOLE_REAL
#define WEAR_COSTUME_ACCE_SLOT	18
#else
#define WEAR_COSTUME_ACCE_SLOT	22
#endif

#ifdef ENABLE_BIOLOGIST_UI
enum biologAffect {
	AFFECT_BIOLOGIST_1 = 651,
	AFFECT_BIOLOGIST_2 = 652,
	AFFECT_BIOLOGIST_3 = 653,
	AFFECT_BIOLOGIST_4 = 654,
	AFFECT_BIOLOGIST_5 = 655,
	AFFECT_BIOLOGIST_6 = 656,
	AFFECT_BIOLOGIST_7 = 657,
	AFFECT_BIOLOGIST_8 = 658,
	AFFECT_BIOLOGIST_9 = 659,
	AFFECT_BIOLOGIST_10 = 660,
	AFFECT_BIOLOGIST_11 = 661,
	AFFECT_BIOLOGIST_12 = 662,
	AFFECT_BIOLOGIST_13 = 663,
	AFFECT_BIOLOGIST_14 = 664
};

static const long biologistMissionInfo[14][15] =
{
    // missioneID: 0
    {
        30006,                        // il vnum del item richiesto per la missione.
        10,                            // il contaggio dei item richiesti per completare la missione.
        600,
        APPLY_ATT_SPEED,                // il primo bonus ricompensa.
        10,                        // il primo valore del bonus ricompensa.
        APPLY_NONE,                    // il secondo bonus ricompensa.
        0,                            // il secondo valore del bonus ricompensa.
        APPLY_NONE,                    // il terzo bonus ricompensa.
        0,                            // il terzo valore del bonus ricompensa.
        APPLY_NONE,                    // il quarto bonus ricompensa.
        0,                            // il quarto valore del bonus ricompensa.
        0,                            // 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
        30,                            // il livello minimo per fare la missione.
        75,                            // % di successo per la consegna.
        AFFECT_BIOLOGIST_1,            // l'affetto su cui va stoccato il bonus.
    },
    // missioneID: 1
    {
        30047,                        // il vnum del item richiesto per la missione.
        15,                            // il contaggio dei item richiesti per completare la missione.
        900,
        APPLY_DEF_GRADE_BONUS,        // il primo bonus ricompensa.
        60,                            // il primo valore del bonus ricompensa.
        APPLY_NONE,                    // il secondo bonus ricompensa.
        0,                            // il secondo valore del bonus ricompensa.
        APPLY_NONE,                    // il terzo bonus ricompensa.
        0,                            // il terzo valore del bonus ricompensa.
        APPLY_NONE,                    // il quarto bonus ricompensa.
        0,                            // il quarto valore del bonus ricompensa.
        0,                            // 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
        40,                            // il livello minimo per fare la missione.
        70,                            // % di successo per la consegna.
        AFFECT_BIOLOGIST_2,            // l'affetto su cui va stoccato il bonus.
    },
    // missioneID: 2
    {
        30015,                        // il vnum del item richiesto per la missione.
        20,                            // il contaggio dei item richiesti per completare la missione.
        1200,                        // il tempo d'attessa per consegnare l'item (in secondi).
        APPLY_ATT_GRADE_BONUS,        // il primo bonus ricompensa.
        50,                            // il primo valore del bonus ricompensa.
        APPLY_NONE,                    // il secondo bonus ricompensa.
        0,                            // il secondo valore del bonus ricompensa.
        APPLY_NONE,                    // il terzo bonus ricompensa.
        0,                            // il terzo valore del bonus ricompensa.
        APPLY_NONE,                    // il quarto bonus ricompensa.
        0,                            // il quarto valore del bonus ricompensa.
        0,                            // 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
        50,                            // il livello minimo per fare la missione.
        65,                            // % di successo per la consegna.
        AFFECT_BIOLOGIST_3,            // l'affetto su cui va stoccato il bonus.
    },
    // missioneID: 3
    {
        30050,                        // il vnum del item richiesto per la missione.
        25,                            // il contaggio dei item richiesti per completare la missione.
        1500,                        // il tempo d'attessa per consegnare l'item (in secondi).
        APPLY_ATTBONUS_MONSTER,    // il primo bonus ricompensa.
        10,                            // il primo valore del bonus ricompensa.
        APPLY_NONE,                    // il secondo bonus ricompensa.
        0,                            // il secondo valore del bonus ricompensa.
        APPLY_NONE,                    // il terzo bonus ricompensa.
        0,                            // il terzo valore del bonus ricompensa.
        APPLY_NONE,                    // il quarto bonus ricompensa.
        0,                            // il quarto valore del bonus ricompensa.
        0,                            // 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
        60,                            // il livello minimo per fare la missione.
        50,                            // % di successo per la consegna.
        AFFECT_BIOLOGIST_4,            // l'affetto su cui va stoccato il bonus.
    },
    // missioneID: 4
    {
        30165,                        // il vnum del item richiesto per la missione.
        30,                            // il contaggio dei item richiesti per completare la missione.
        1800,                        // il tempo d'attessa per consegnare l'item (in secondi).
        APPLY_RESIST_MONSTER,        // il primo bonus ricompensa.
        10,                            // il primo valore del bonus ricompensa.
        APPLY_NONE,                    // il secondo bonus ricompensa.
        0,                            // il secondo valore del bonus ricompensa.
        APPLY_NONE,                    // il terzo bonus ricompensa.
        0,                            // il terzo valore del bonus ricompensa.
        APPLY_NONE,                    // il quarto bonus ricompensa.
        0,                            // il quarto valore del bonus ricompensa.
        0,                            // 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
        70,                            // il livello minimo per fare la missione.
        50,                            // % di successo per la consegna.
        AFFECT_BIOLOGIST_5,            // l'affetto su cui va stoccato il bonus.
    },
    // missioneID: 5
    {
        30166,                        // il vnum del item richiesto per la missione.
        35,                            // il contaggio dei item richiesti per completare la missione.
        2100,                        // il tempo d'attessa per consegnare l'item (in secondi).
        APPLY_MALL_ATTBONUS,        // il primo bonus ricompensa.
        15,                            // il primo valore del bonus ricompensa.
        APPLY_NONE,                    // il secondo bonus ricompensa.
        0,                            // il secondo valore del bonus ricompensa.
        APPLY_NONE,                    // il terzo bonus ricompensa.
        0,                            // il terzo valore del bonus ricompensa.
        APPLY_NONE,                    // il quarto bonus ricompensa.
        0,                            // il quarto valore del bonus ricompensa.
        0,                            // 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
        80,                            // il livello minimo per fare la missione.
        50,                            // % di successo per la consegna.
        AFFECT_BIOLOGIST_6,            // l'affetto su cui va stoccato il bonus.
    },
    // missioneID: 6
    {
        30167,                        // il vnum del item richiesto per la missione.
        40,                            // il contaggio dei item richiesti per completare la missione.
        2400,                        // il tempo d'attessa per consegnare l'item (in secondi).
        APPLY_RES_RACES,        // il primo bonus ricompensa.
        10,                            // il primo valore del bonus ricompensa.
        APPLY_NONE,                    // il secondo bonus ricompensa.
        0,                            // il secondo valore del bonus ricompensa.
        APPLY_NONE,                    // il terzo bonus ricompensa.
        0,                            // il terzo valore del bonus ricompensa.
        APPLY_NONE,                    // il quarto bonus ricompensa.
        0,                            // il quarto valore del bonus ricompensa.
        0,                            // 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
        85,                            // il livello minimo per fare la missione.
        50,                            // % di successo per la consegna.
        AFFECT_BIOLOGIST_7,            // l'affetto su cui va stoccato il bonus.
    },
    // missioneID: 7
    {
        30168,                        // il vnum del item richiesto per la missione.
        50,                            // il contaggio dei item richiesti per completare la missione.
        2700,                        // il tempo d'attessa per consegnare l'item (in secondi).
        APPLY_ATT_RACES,        // il primo bonus ricompensa.
        8,                            // il primo valore del bonus ricompensa.
        APPLY_NONE,                    // il secondo bonus ricompensa.
        0,                            // il secondo valore del bonus ricompensa.
        APPLY_NONE,                    // il terzo bonus ricompensa.
        0,                            // il terzo valore del bonus ricompensa.
        APPLY_NONE,                    // il quarto bonus ricompensa.
        0,                            // il quarto valore del bonus ricompensa.
        0,                            // 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
        90,                            // il livello minimo per fare la missione.
        50,                            // % di successo per la consegna.
        AFFECT_BIOLOGIST_8,            // l'affetto su cui va stoccato il bonus.
    },
    // missioneID: 8
    {
        30251,                        // il vnum del item richiesto per la missione.
        10,                            // il contaggio dei item richiesti per completare la missione.
        3000,                        // il tempo d'attessa per consegnare l'item (in secondi).
        APPLY_MAX_HP,        // il primo bonus ricompensa.
        1000,                            // il primo valore del bonus ricompensa.
        APPLY_ATT_GRADE_BONUS,    // il secondo bonus ricompensa.
        60,                            // il secondo valore del bonus ricompensa.
        APPLY_DEF_GRADE_BONUS,        // il terzo bonus ricompensa.
        80,                            // il terzo valore del bonus ricompensa.
        APPLY_NONE,        // il quarto bonus ricompensa.
        0,                            // il quarto valore del bonus ricompensa.
        1,                            // 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
        100,                            // il livello minimo per fare la missione.
        50,                            // % di successo per la consegna.
        AFFECT_BIOLOGIST_9,            // l'affetto su cui va stoccato il bonus.
    },
    // missioneID: 9
    {
        30252,                        // il vnum del item richiesto per la missione.
        10,                            // il contaggio dei item richiesti per completare la missione.
        3600,                        // il tempo d'attessa per consegnare l'item (in secondi).
        APPLY_MAX_HP,        // il primo bonus ricompensa.
        1500,                            // il primo valore del bonus ricompensa.
        APPLY_ATT_GRADE_BONUS,        // il secondo bonus ricompensa.
        100,                            // il secondo valore del bonus ricompensa.
        APPLY_DEF_GRADE_BONUS,            // il terzo bonus ricompensa.
        150,                            // il terzo valore del bonus ricompensa.
        APPLY_NONE,        // il quarto bonus ricompensa.
        0,                            // il quarto valore del bonus ricompensa.
        1,                            // 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
        105,                        // il livello minimo per fare la missione.
        50,                            // % di successo per la consegna.
        AFFECT_BIOLOGIST_10,        // l'affetto su cui va stoccato il bonus.
    },
    // missioneID: 10
    {
        30002,                        // il vnum del item richiesto per la missione.
        10,                            // il contaggio dei item richiesti per completare la missione.
        5400,                        // il tempo d'attessa per consegnare l'item (in secondi).
        APPLY_MAX_HP,                // il primo bonus ricompensa.
        2000,                        // il primo valore del bonus ricompensa.
        APPLY_ATTBONUS_MONSTER,        // il secondo bonus ricompensa.
        20,                        // il secondo valore del bonus ricompensa.
        APPLY_ATT_GRADE_BONUS,        // il terzo bonus ricompensa.
        120,                            // il terzo valore del bonus ricompensa.
        APPLY_NONE,                    // il quarto bonus ricompensa.
        0,                            // il quarto valore del bonus ricompensa.
        1,                            // 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
        110,                        // il livello minimo per fare la missione.
        50,                            // % di successo per la consegna.
        AFFECT_BIOLOGIST_11,        // l'affetto su cui va stoccato il bonus.
    },
    // missioneID: 11
    {
        30005,                        // il vnum del item richiesto per la missione.
        20,                            // il contaggio dei item richiesti per completare la missione.
        10800,                        // il tempo d'attessa per consegnare l'item (in secondi).
        APPLY_ATTBONUS_MONSTER,                // il primo bonus ricompensa.
        10,                        // il primo valore del bonus ricompensa.
        APPLY_ATTBONUS_HUMAN,        // il secondo bonus ricompensa.
        10,                        // il secondo valore del bonus ricompensa.
        APPLY_SKILL_DAMAGE_BONUS,        // il terzo bonus ricompensa.
        10,                            // il terzo valore del bonus ricompensa.
        APPLY_NONE,                    // il quarto bonus ricompensa.
        0,                            // il quarto valore del bonus ricompensa.
        0,                            // 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
        120,                        // il livello minimo per fare la missione.
        50,                            // % di successo per la consegna.
        AFFECT_BIOLOGIST_12,        // l'affetto su cui va stoccato il bonus.
    },
    // missioneID: 12
    {
        30004,                        // il vnum del item richiesto per la missione.
        40,                            // il contaggio dei item richiesti per completare la missione.
        4200,                        // il tempo d'attessa per consegnare l'item (in secondi).
        APPLY_ATTBONUS_HUMAN,        // il primo bonus ricompensa.
        10,                            // il primo valore del bonus ricompensa.
        APPLY_PVM_CRITICAL_PCT,        // il secondo bonus ricompensa.
        10,                            // il secondo valore del bonus ricompensa.
        APPLY_NONE,                    // il terzo bonus ricompensa.
        0,                            // il terzo valore del bonus ricompensa.
        APPLY_NONE,                    // il quarto bonus ricompensa.
        0,                            // il quarto valore del bonus ricompensa.
        0,                            // 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
        125,                        // il livello minimo per fare la missione.
        50,                            // % di successo per la consegna.
        AFFECT_BIOLOGIST_13,        // l'affetto su cui va stoccato il bonus.
    },
    // missioneID: 13
    {
        30005,                        // il vnum del item richiesto per la missione.
        45,                            // il contaggio dei item richiesti per completare la missione.
        4500,                        // il tempo d'attessa per consegnare l'item (in secondi).
        APPLY_ATTBONUS_MONSTER,        // il primo bonus ricompensa.
        10,                            // il primo valore del bonus ricompensa.
        APPLY_RESIST_MEZZIUOMINI,    // il secondo bonus ricompensa.
        10,                            // il secondo valore del bonus ricompensa.
        APPLY_NONE,                    // il terzo bonus ricompensa.
        0,                            // il terzo valore del bonus ricompensa.
        APPLY_NONE,                    // il quarto bonus ricompensa.
        0,                            // il quarto valore del bonus ricompensa.
        0,                            // 0 = tutti i bonus verrano dati al player | 1 = il player deve scegliere uno bonus (piu avra la posibilita di cambiarlo tramite: 164401).
        130,                        // il livello minimo per fare la missione.
        50,                            // % di successo per la consegna.
        AFFECT_BIOLOGIST_14,        // l'affetto su cui va stoccato il bonus.
    }
};
#endif

#if defined(USE_BATTLEPASS)
enum EExtBattlePassType
{
	BATTLEPASS_NORMAL,
	BATTLEPASS_PREMIUM,
	BATTLEPASS_EVENT,

	BATTLEPASS_TYPE_MAX,
};

enum EExtBattleMissionPassType
{
	MISSION_TYPE_NONE,

	KILL_MONSTER,
	KILL_PLAYER,

	DAMAGE_MONSTER,
	DAMAGE_PLAYER,

	BP_ITEM_USE,
	BP_ITEM_SELL,
	BP_ITEM_CRAFT,
	BP_ITEM_REFINE,
	BP_ITEM_DESTROY,
	BP_ITEM_COLLECT,

	FISH_FISHING,
	FISH_GRILL,
	FISH_CATCH,
	
	EXP_COLLECT,
	YANG_COLLECT,
	
	GUILD_PLAY_GUILDWAR,
	GUILD_SPENT_EXP,
	
	GAYA_CRAFT_GAYA,
	GAYA_BUY_ITEM_GAYA_COST,

	COMPLETE_DUNGEON,
	CHAT_MESSAGE_SHOUT,
	MISSION_TYPE_MAX,
};
#endif

#if defined(ENABLE_AURA_SYSTEM)
typedef struct SAuraRefineInfo {
	uint8_t bAuraRefineInfoLevel;
	uint8_t bAuraRefineInfoExpPercent;
} TAuraRefineInfo;

enum EAuraMisc {
	AURA_MAX_LEVEL = 250,
	AURA_REFINE_MAX_DISTANCE = 1000,
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

enum EAuraGradeType {
	AURA_GRADE_NONE,
	AURA_GRADE_ORDINARY,
	AURA_GRADE_SIMPLE,
	AURA_GRADE_NOBLE,
	AURA_GRADE_SPARKLING,
	AURA_GRADE_MAGNIFICENT,
	AURA_GRADE_RADIANT,
	AURA_GRADE_MAX_NUM,
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

enum ERefineInfoType {
	AURA_REFINE_INFO_SLOT_CURRENT,
	AURA_REFINE_INFO_SLOT_NEXT,
	AURA_REFINE_INFO_SLOT_EVOLVED,
	AURA_REFINE_INFO_SLOT_MAX
};
#endif

#pragma pack(pop)

#endif
