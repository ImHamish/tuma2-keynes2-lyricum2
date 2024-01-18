#ifndef __INC_METIN_II_CHAR_H__
#define __INC_METIN_II_CHAR_H__

#include <unordered_map>

#include "../common/stl.h"
#include "entity.h"
#include "FSM.h"
#include "horse_rider.h"
#include "vid.h"
#include "constants.h"
#include "affect.h"
#include "affect_flag.h"
#ifndef ENABLE_CUBE_RENEWAL_WORLDARD
#include "cube.h"
#else
#include "cuberenewal.h"
#endif
#include "mining.h"

#include "utils.h"

#include "../common/CommonDefines.h"
#ifdef __ATTR_TRANSFER_SYSTEM__
#include "attr_transfer.h"
#endif

#define ENABLE_ANTI_CMD_FLOOD
#define ENABLE_OPEN_SHOP_WITH_ARMOR
enum eMountType {MOUNT_TYPE_NONE=0, MOUNT_TYPE_NORMAL=1, MOUNT_TYPE_COMBAT=2, MOUNT_TYPE_MILITARY=3};
eMountType GetMountLevelByVnum(DWORD dwMountVnum, bool IsNew);
const DWORD GetRandomSkillVnum(BYTE bJob = JOB_MAX_NUM);


class CBuffOnAttributes;
class CPetSystem;
#ifdef __NEWPET_SYSTEM__
class CNewPetSystem;
#endif

#ifdef __ENABLE_NEW_OFFLINESHOP__
namespace offlineshop
{
	class CShop;
	class CShopSafebox;
	class CAuction;
}
#endif
#define INSTANT_FLAG_DEATH_PENALTY		(1 << 0)
#define INSTANT_FLAG_SHOP			(1 << 1)
#define INSTANT_FLAG_EXCHANGE			(1 << 2)
#define INSTANT_FLAG_STUN			(1 << 3)
#define INSTANT_FLAG_NO_REWARD			(1 << 4)

#define AI_FLAG_NPC				(1 << 0)
#define AI_FLAG_AGGRESSIVE			(1 << 1)
#define AI_FLAG_HELPER				(1 << 2)
#define AI_FLAG_STAYZONE			(1 << 3)


#define SET_OVER_TIME(ch, time)	(ch)->SetOverTime(time)

extern int g_nPortalLimitTime;

enum
{
	MAIN_RACE_WARRIOR_M,
	MAIN_RACE_ASSASSIN_W,
	MAIN_RACE_SURA_M,
	MAIN_RACE_SHAMAN_W,
	MAIN_RACE_WARRIOR_W,
	MAIN_RACE_ASSASSIN_M,
	MAIN_RACE_SURA_W,
	MAIN_RACE_SHAMAN_M,
	MAIN_RACE_MAX_NUM,
};

enum
{
	POISON_LENGTH = 30,
	STAMINA_PER_STEP = 1,
	SAFEBOX_PAGE_SIZE = 9,
	AI_CHANGE_ATTACK_POISITION_TIME_NEAR = 10000,
	AI_CHANGE_ATTACK_POISITION_TIME_FAR = 1000,
	AI_CHANGE_ATTACK_POISITION_DISTANCE = 100,
	SUMMON_MONSTER_COUNT = 3,
};

enum EFlyType
{
	FLY_NONE,
	FLY_EXP,
	FLY_HP_MEDIUM,
	FLY_HP_BIG,
	FLY_SP_SMALL,
	FLY_SP_MEDIUM,
	FLY_SP_BIG,
	FLY_FIREWORK1,
	FLY_FIREWORK2,
	FLY_FIREWORK3,
	FLY_FIREWORK4,
	FLY_FIREWORK5,
	FLY_FIREWORK6,
	FLY_FIREWORK_CHRISTMAS,
	FLY_CHAIN_LIGHTNING,
	FLY_HP_SMALL,
	FLY_SKILL_MUYEONG,
};

enum EDamageType
{
	DAMAGE_TYPE_NONE,
	DAMAGE_TYPE_NORMAL,
	DAMAGE_TYPE_NORMAL_RANGE,
	//스킬
	DAMAGE_TYPE_MELEE,
	DAMAGE_TYPE_RANGE,
	DAMAGE_TYPE_FIRE,
	DAMAGE_TYPE_ICE,
	DAMAGE_TYPE_ELEC,
	DAMAGE_TYPE_MAGIC,
	DAMAGE_TYPE_POISON,
	DAMAGE_TYPE_SPECIAL,
};

enum DamageFlag
{
	DAMAGE_NORMAL	= (1 << 0),
	DAMAGE_POISON	= (1 << 1),
	DAMAGE_DODGE	= (1 << 2),
	DAMAGE_BLOCK	= (1 << 3),
	DAMAGE_PENETRATE= (1 << 4),
	DAMAGE_CRITICAL = (1 << 5),
};

enum EPointTypes
{
	POINT_NONE,                 // 0
	POINT_LEVEL,                // 1
	POINT_VOICE,                // 2
	POINT_EXP,                  // 3
	POINT_NEXT_EXP,             // 4
	POINT_HP,                   // 5
	POINT_MAX_HP,               // 6
	POINT_SP,                   // 7
	POINT_MAX_SP,               // 8
	POINT_STAMINA,              // 9  스테미너
	POINT_MAX_STAMINA,          // 10 최대 스테미너

	POINT_GOLD = 11,
	POINT_ST,                   // 12 근력
	POINT_HT,                   // 13 체력
	POINT_DX,                   // 14 민첩성
	POINT_IQ,                   // 15 정신력
	POINT_DEF_GRADE,		// 16 ...
	POINT_ATT_SPEED,            // 17 공격속도
	POINT_ATT_GRADE,		// 18 공격력 MAX
	POINT_MOV_SPEED,            // 19 이동속도
	POINT_CLIENT_DEF_GRADE,	// 20 방어등급
	POINT_CASTING_SPEED,        // 21 주문속도 (쿨다운타임*100) / (100 + 이값) = 최종 쿨다운 타임
	POINT_MAGIC_ATT_GRADE,      // 22 마법공격력
	POINT_MAGIC_DEF_GRADE,      // 23 마법방어력
	POINT_EMPIRE_POINT,         // 24 제국점수
	POINT_LEVEL_STEP,           // 25 한 레벨에서의 단계.. (1 2 3 될 때 보상, 4 되면 레벨 업)
	POINT_STAT,                 // 26 능력치 올릴 수 있는 개수
	POINT_SUB_SKILL,		// 27 보조 스킬 포인트
	POINT_SKILL,		// 28 액티브 스킬 포인트
	POINT_WEAPON_MIN,		// 29 무기 최소 데미지
	POINT_WEAPON_MAX,		// 30 무기 최대 데미지
	POINT_PLAYTIME,             // 31 플레이시간
	POINT_HP_REGEN,             // 32 HP 회복률
	POINT_SP_REGEN,             // 33 SP 회복률

	POINT_BOW_DISTANCE,         // 34 활 사정거리 증가치 (meter)

	POINT_HP_RECOVERY,          // 35 체력 회복 증가량
	POINT_SP_RECOVERY,          // 36 정신력 회복 증가량

	POINT_POISON_PCT,           // 37 독 확률
	POINT_STUN_PCT,             // 38 기절 확률
	POINT_SLOW_PCT,             // 39 슬로우 확률
	POINT_CRITICAL_PCT,         // 40 크리티컬 확률
	POINT_PENETRATE_PCT,        // 41 관통타격 확률
	POINT_CURSE_PCT,            // 42 저주 확률

	POINT_ATTBONUS_HUMAN,       // 43 인간에게 강함
	POINT_ATTBONUS_ANIMAL,      // 44 동물에게 데미지 % 증가
	POINT_ATTBONUS_ORC,         // 45 웅귀에게 데미지 % 증가
	POINT_ATTBONUS_MILGYO,      // 46 밀교에게 데미지 % 증가
	POINT_ATTBONUS_UNDEAD,      // 47 시체에게 데미지 % 증가
	POINT_ATTBONUS_DEVIL,       // 48 마귀(악마)에게 데미지 % 증가
	POINT_ATTBONUS_INSECT,      // 49 벌레족
	POINT_ATTBONUS_FIRE,        // 50 화염족
	POINT_ATTBONUS_ICE,         // 51 빙설족
	POINT_ATTBONUS_DESERT,      // 52 사막족
	POINT_ATTBONUS_MONSTER,     // 53 모든 몬스터에게 강함
	POINT_ATTBONUS_WARRIOR,     // 54 무사에게 강함
	POINT_ATTBONUS_ASSASSIN,	// 55 자객에게 강함
	POINT_ATTBONUS_SURA,		// 56 수라에게 강함
	POINT_ATTBONUS_SHAMAN,		// 57 무당에게 강함
	POINT_ATTBONUS_TREE,     	// 58 나무에게 강함 20050729.myevan UNUSED5

	POINT_RESIST_WARRIOR,		// 59 무사에게 저항
	POINT_RESIST_ASSASSIN,		// 60 자객에게 저항
	POINT_RESIST_SURA,			// 61 수라에게 저항
	POINT_RESIST_SHAMAN,		// 62 무당에게 저항

	POINT_STEAL_HP,             // 63 생명력 흡수
	POINT_STEAL_SP,             // 64 정신력 흡수

	POINT_MANA_BURN_PCT,        // 65 마나 번

	/// 피해시 보너스 ///

	POINT_DAMAGE_SP_RECOVER,    // 66 공격당할 시 정신력 회복 확률

	POINT_BLOCK,                // 67 블럭율
	POINT_DODGE,                // 68 회피율

	POINT_RESIST_SWORD,         // 69
	POINT_RESIST_TWOHAND,       // 70
	POINT_RESIST_DAGGER,        // 71
	POINT_RESIST_BELL,          // 72
	POINT_RESIST_FAN,           // 73
	POINT_RESIST_BOW,           // 74  화살   저항   : 대미지 감소
	POINT_RESIST_FIRE,          // 75  화염   저항   : 화염공격에 대한 대미지 감소
	POINT_RESIST_ELEC,          // 76  전기   저항   : 전기공격에 대한 대미지 감소
	POINT_RESIST_MAGIC,         // 77  술법   저항   : 모든술법에 대한 대미지 감소
	POINT_RESIST_WIND,          // 78  바람   저항   : 바람공격에 대한 대미지 감소

	POINT_REFLECT_MELEE,        // 79 공격 반사

	/// 특수 피해시 ///
	POINT_REFLECT_CURSE,		// 80 저주 반사
	POINT_POISON_REDUCE,		// 81 독데미지 감소

	/// 적 소멸시 ///
	POINT_KILL_SP_RECOVER,		// 82 적 소멸시 MP 회복
	POINT_EXP_DOUBLE_BONUS,		// 83
	POINT_GOLD_DOUBLE_BONUS,		// 84
	POINT_ITEM_DROP_BONUS,		// 85

	/// 회복 관련 ///
	POINT_POTION_BONUS,			// 86
	POINT_KILL_HP_RECOVERY,		// 87

	POINT_IMMUNE_STUN,			// 88
	POINT_IMMUNE_SLOW,			// 89
	POINT_IMMUNE_FALL,			// 90
	//////////////////

	POINT_PARTY_ATTACKER_BONUS,		// 91
	POINT_PARTY_TANKER_BONUS,		// 92

	POINT_ATT_BONUS,			// 93
	POINT_DEF_BONUS,			// 94

	POINT_ATT_GRADE_BONUS,		// 95
	POINT_DEF_GRADE_BONUS,		// 96
	POINT_MAGIC_ATT_GRADE_BONUS,	// 97
	POINT_MAGIC_DEF_GRADE_BONUS,	// 98

	POINT_RESIST_NORMAL_DAMAGE,		// 99

	POINT_HIT_HP_RECOVERY,		// 100
	POINT_HIT_SP_RECOVERY, 		// 101
	POINT_MANASHIELD,			// 102 흑신수호 스킬에 의한 마나쉴드 효과 정도

	POINT_PARTY_BUFFER_BONUS,		// 103
	POINT_PARTY_SKILL_MASTER_BONUS,	// 104

	POINT_HP_RECOVER_CONTINUE,		// 105
	POINT_SP_RECOVER_CONTINUE,		// 106

	POINT_STEAL_GOLD,			// 107
	POINT_POLYMORPH,			// 108 변신한 몬스터 번호
#if defined(USE_MOUNT_COSTUME_SYSTEM)
	POINT_MOUNT_OLD,				// 109
#else
	POINT_MOUNT,					// 109
#endif
	POINT_PARTY_HASTE_BONUS,		// 110
	POINT_PARTY_DEFENDER_BONUS,		// 111
	POINT_STAT_RESET_COUNT,		// 112 피의 단약 사용을 통한 스텟 리셋 포인트 (1당 1포인트 리셋가능)

	POINT_HORSE_SKILL,			// 113

	POINT_MALL_ATTBONUS,		// 114 공격력 +x%
	POINT_MALL_DEFBONUS,		// 115 방어력 +x%
	POINT_MALL_EXPBONUS,		// 116 경험치 +x%
	POINT_MALL_ITEMBONUS,		// 117 아이템 드롭율 x/10배
	POINT_MALL_GOLDBONUS,		// 118 돈 드롭율 x/10배

	POINT_MAX_HP_PCT,			// 119 최대생명력 +x%
	POINT_MAX_SP_PCT,			// 120 최대정신력 +x%

	POINT_SKILL_DAMAGE_BONUS,		// 121 스킬 데미지 *(100+x)%
	POINT_NORMAL_HIT_DAMAGE_BONUS,	// 122 평타 데미지 *(100+x)%

	// DEFEND_BONUS_ATTRIBUTES
	POINT_SKILL_DEFEND_BONUS,		// 123 스킬 방어 데미지
	POINT_NORMAL_HIT_DEFEND_BONUS,	// 124 평타 방어 데미지
	// END_OF_DEFEND_BONUS_ATTRIBUTES

	POINT_PC_UNUSED01,		// 125 PC방 전용 경험치 보너스
	POINT_PC_UNUSED02,		// 126 PC방 전용 드롭률 보너스
	POINT_RAMADAN_CANDY_BONUS_EXP,			// 라마단 사탕 경험치 증가용

	POINT_ENERGY = 128,					// 128 기력

	// 기력 ui 용.
	// 서버에서 쓰지 않기만, 클라이언트에서 기력의 끝 시간을 POINT로 관리하기 때문에 이렇게 한다.
	// 아 부끄럽다
	POINT_ENERGY_END_TIME = 129,					// 129 기력 종료 시간

	POINT_COSTUME_ATTR_BONUS = 130,
	POINT_MAGIC_ATT_BONUS_PER = 131,
	POINT_MELEE_MAGIC_ATT_BONUS_PER = 132,

	// 추가 속성 저항
	POINT_RESIST_ICE = 133,          //   냉기 저항   : 얼음공격에 대한 대미지 감소
	POINT_RESIST_EARTH = 134,        //   대지 저항   : 얼음공격에 대한 대미지 감소
	POINT_RESIST_DARK = 135,         //   어둠 저항   : 얼음공격에 대한 대미지 감소

	POINT_RESIST_CRITICAL = 136,		// 크리티컬 저항	: 상대의 크리티컬 확률을 감소
	POINT_RESIST_PENETRATE = 137,		// 관통타격 저항	: 상대의 관통타격 확률을 감소
#ifdef ENABLE_ACCE_SYSTEM
	POINT_ACCEDRAIN_RATE = 143,
#endif
#ifdef ELEMENT_NEW_BONUSES
	POINT_ATTBONUS_ELEC = 146,
	POINT_ATTBONUS_WIND = 147,
	POINT_ATTBONUS_EARTH = 148,
	POINT_ATTBONUS_DARK = 149,
#ifdef ENABLE_NEW_BONUS_TALISMAN
	POINT_ATTBONUS_IRR_SPADA = 150,
	POINT_ATTBONUS_IRR_SPADONE = 151,
	POINT_ATTBONUS_IRR_PUGNALE = 152,
	POINT_ATTBONUS_IRR_FRECCIA = 153,
	POINT_ATTBONUS_IRR_VENTAGLIO = 154,
	POINT_ATTBONUS_IRR_CAMPANA = 155,
	POINT_RESIST_MEZZIUOMINI = 156,
	POINT_DEF_TALISMAN = 157,
	POINT_ATTBONUS_FORT_ZODIAC = 158,
#endif
#endif
#ifdef ENABLE_STRONG_METIN
	POINT_ATTBONUS_METIN = 159,
#endif
#ifdef ENABLE_STRONG_BOSS
	POINT_ATTBONUS_BOSS = 160,
#endif
#ifdef ENABLE_RESIST_MONSTER
	POINT_RESIST_MONSTER = 161,
#endif
#ifdef ENABLE_MEDI_PVM
	POINT_ATTBONUS_MEDI_PVM = 162,
#endif
#if defined(ENABLE_GAYA_RENEWAL)
	POINT_GEM = 163,
#endif
	POINT_PVM_CRITICAL_PCT = 170,
#ifdef ENABLE_DS_RUNE
	POINT_RUNE_MONSTERS = 171,
#endif
#ifdef ENABLE_NEW_COMMON_BONUSES
	POINT_DOUBLE_DROP_ITEM = 172,
	POINT_IRR_WEAPON_DEFENSE = 173,
#endif
	POINT_FISHING_RARE = 174,
#ifdef ENABLE_NEW_USE_POTION
	POINT_PARTY_DROPEXP = 175,
#endif
#if defined(USE_BATTLEPASS)
	POINT_BATTLE_PASS_PREMIUM_ID = 176,
#endif
#if defined(USE_MOUNT_COSTUME_SYSTEM)
	POINT_MOUNT = 183,
#endif
	POINT_ATT_RACES = 184,
	POINT_RES_RACES = 185,
#if defined(ENABLE_PETSKIN)
	POINT_PETSKIN = 186,
#endif
#if defined(ENABLE_MOUNTSKIN)
	POINT_MOUNTSKIN = 187,
#endif
#ifdef ENABLE_HALLOWEEN_EVENT_2022
#ifndef USE_NO_HALLOWEEN_EVENT_2022_BONUSES
    POINT_FEAR                      = 188,
    POINT_RESISTANCE_FEAR           = 189,
#endif
    POINT_SKULL                     = 190,
#endif
#if defined(ENABLE_WORLDBOSS)
	POINT_WB_POINTS = 191,
#endif
#if defined(USE_ANTI_EXP)
	POINT_ANTI_EXP = 192,
#endif
	//POINT_MAX_NUM = 129	common/length.h
};

enum EPKModes
{
	PK_MODE_PEACE,
	PK_MODE_REVENGE,
	PK_MODE_FREE,
	PK_MODE_PROTECT,
	PK_MODE_GUILD,
	PK_MODE_MAX_NUM
};

enum EPositions
{
	POS_DEAD,
	POS_SLEEPING,
	POS_RESTING,
	POS_SITTING,
	POS_FISHING,
	POS_FIGHTING,
	POS_MOUNTING,
	POS_STANDING
};

enum EBlockAction
{
	BLOCK_EXCHANGE		= (1 << 0),
	BLOCK_PARTY_INVITE		= (1 << 1),
	BLOCK_GUILD_INVITE		= (1 << 2),
	BLOCK_WHISPER		= (1 << 3),
	BLOCK_MESSENGER_INVITE	= (1 << 4),
	BLOCK_PARTY_REQUEST		= (1 << 5),
};

// <Factor> Dynamically evaluated CHARACTER* equivalent.
// Referring to SCharDeadEventInfo.
struct DynamicCharacterPtr {
	DynamicCharacterPtr() : is_pc(false), id(0) {}
	DynamicCharacterPtr(const DynamicCharacterPtr& o)
		: is_pc(o.is_pc), id(o.id) {}

	// Returns the LPCHARACTER found in CHARACTER_MANAGER.
	LPCHARACTER Get() const;
	// Clears the current settings.
	void Reset() {
		is_pc = false;
		id = 0;
	}

	// Basic assignment operator.
	DynamicCharacterPtr& operator=(const DynamicCharacterPtr& rhs) {
		is_pc = rhs.is_pc;
		id = rhs.id;
		return *this;
	}
	// Supports assignment with LPCHARACTER type.
	DynamicCharacterPtr& operator=(LPCHARACTER character);
	// Supports type casting to LPCHARACTER.
	operator LPCHARACTER() const {
		return Get();
	}

	bool is_pc;
	uint32_t id;
};

typedef struct character_point
{
	long points[POINT_MAX_NUM];
	BYTE			job;
	BYTE			voice;

	BYTE			level;
	DWORD			exp;
#if defined(ENABLE_NEW_GOLD_LIMIT)
	uint64_t gold;
#else
	uint32_t gold;
#endif
#if defined(ENABLE_WORLDBOSS)
	uint32_t wbpoints;
#endif
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
	uint32_t skull;
#endif
#if defined(ENABLE_GAYA_RENEWAL)
	long gem;
#endif
	int				hp;
	int				sp;

	int				iRandomHP;
	int				iRandomSP;

	int				stamina;

	BYTE			skill_group;
#if defined(USE_BATTLEPASS)
	int battle_pass_premium_id;
#endif
} CHARACTER_POINT;

typedef struct character_point_instant
{
	struct SPlayerSlot
	{
		LPITEM pItems[INVENTORY_AND_EQUIP_SLOT_MAX];
		WORD bItemGrid[INVENTORY_AND_EQUIP_SLOT_MAX];

		LPITEM pDSItems[DRAGON_SOUL_INVENTORY_MAX_NUM];
		WORD wDSItemGrid[DRAGON_SOUL_INVENTORY_MAX_NUM];
		LPENTITY m_pDragonSoulRefineWindowOpener;

#if defined(ENABLE_EXTRA_INVENTORY)
		LPITEM pExtraItems[EXTRA_INVENTORY_MAX_NUM];
		WORD wExtraItemGrid[EXTRA_INVENTORY_MAX_NUM];
#endif

#if defined(ENABLE_SWITCHBOT_WORLDARD)
		LPITEM pSBItems[SWITCHBOT_SLOT_MAX];
#endif

		LPITEM pCubeItems[CUBE_MAX_NUM];
		LPCHARACTER pCubeNpc;

#if defined(__ATTR_TRANSFER_SYSTEM__)
		LPITEM pAttrTransferItems[MAX_ATTR_TRANSFER_SLOT];
		LPCHARACTER pAttrTransferNpc;
#endif

#if defined(ENABLE_ACCE_SYSTEM)
		LPITEM pAcceMaterials[ACCE_WINDOW_MAX_MATERIALS];
#endif

#if defined(ENABLE_CHANGELOOK)
		LPITEM pChangeLookMaterials[CHANGE_LOOK_WINDOW_MAX_MATERIALS];
#endif
#if defined(ENABLE_AURA_SYSTEM)
		LPENTITY m_pAuraRefineWindowOpener;
#endif

#if defined(USE_ATTR_6TH_7TH)
		LPITEM pAttr67AddItem;
#endif
	};

	long points[POINT_MAX_NUM];
	float			fRot;

	int				iMaxHP;
	int				iMaxSP;

	long			position;

	long			instant_flag;
	DWORD			dwAIFlag;
	DWORD			dwImmuneFlag;
	DWORD			dwLastShoutPulse;

	uint32_t parts[PART_MAX_NUM];

	LPCHARACTER			battle_victim;

	BYTE			gm_level;

	BYTE			bBasePart;	// 평상복 번호

	int				iMaxStamina;

	BYTE			bBlockMode;

	int				iDragonSoulActiveDeck;
	std::unique_ptr<SPlayerSlot> playerSlots;
} CHARACTER_POINT_INSTANT;

#define TRIGGERPARAM		LPCHARACTER ch, LPCHARACTER causer

typedef struct trigger
{
	BYTE	type;
	int		(*func) (TRIGGERPARAM);
	long	value;
} TRIGGER;

class CTrigger
{
	public:
		CTrigger() : bType(0), pFunc(NULL)
		{
		}

		BYTE	bType;
		int	(*pFunc) (TRIGGERPARAM);
};

EVENTINFO(char_event_info)
{
	DynamicCharacterPtr ch;
};

struct TSkillUseInfo
{
	int	    iHitCount;
	int	    iMaxHitCount;
	int	    iSplashCount;
	DWORD   dwNextSkillUsableTime;
	int	    iRange;
	bool    bUsed;
#if defined(ENABLE_FIX_WAITHACK)
	bool bSkillCD;
	DWORD dwHitCount;
#endif
	DWORD   dwVID;
	bool    isGrandMaster;

	std::unordered_map<DWORD, size_t> TargetVIDMap;

	TSkillUseInfo()
		: iHitCount(0), iMaxHitCount(0), iSplashCount(0), dwNextSkillUsableTime(0), iRange(0), bUsed(false),
#if defined(ENABLE_FIX_WAITHACK)
 bSkillCD(false), dwHitCount(0),
#endif
		dwVID(0), isGrandMaster(false)
   	{}

#if defined(ENABLE_FIX_WAITHACK)
	bool IsSkillCooldown(DWORD dwVnum, float fSkillPower);
#endif
	bool HitOnce(DWORD dwVnum = 0);

	bool    UseSkill(bool isGrandMaster, DWORD vid, DWORD dwCooltime, int splashcount = 1, int hitcount = -1, int range = -1);
	DWORD   GetMainTargetVID() const	{ return dwVID; }
	void    SetMainTargetVID(DWORD vid) { dwVID=vid; }
	void    ResetHitCount() { if (iSplashCount) { iHitCount = iMaxHitCount; iSplashCount--; } }
};

typedef struct packet_party_update TPacketGCPartyUpdate;
class CExchange;
class CSkillProto;
class CParty;
class CDungeon;
class CWarMap;
class CAffect;
class CGuild;
class CSafebox;
class CArena;

class CShop;
typedef class CShop * LPSHOP;

class CMob;
class CMobInstance;
typedef struct SMobSkillInfo TMobSkillInfo;

//SKILL_POWER_BY_LEVEL
extern int GetSkillPowerByLevelFromType(int job, int skillgroup, int skilllevel);
//END_SKILL_POWER_BY_LEVEL

namespace marriage
{
	class WeddingMap;
}
enum e_overtime
{
	OT_NONE,
	OT_3HOUR,
	OT_5HOUR,
};

#if defined(ENABLE_OKEY_CARD_GAME)
typedef struct SOkeyCard
{
	uint32_t type;
	uint32_t value;
} TOkeyCard;

typedef struct SOkeyCardInfo
{
	TOkeyCard cards_in_hand[MAX_OKEY_CARDS_IN_HAND];
	TOkeyCard cards_in_field[MAX_OKEY_CARDS_IN_FIELD];
	uint32_t cards_left;
	uint32_t field_points;
	uint32_t points;
} TOkeyCardInfo;
#endif

class CHARACTER : public CEntity, public CFSM, public CHorseRider
{
	public:
		void ClearPMCounter(void)       { m_iPMCounter = 0; }
		void IncreasePMCounter(void)    { m_iPMCounter++; }
		void SetLastPMPulse(void);
		int  GetPMCounter(void)   const { return m_iPMCounter; }
		int  GetLastPMPulse(void) const { return m_iLastPMPulse; }

	protected:
		int m_iLastPMPulse;
		int m_iPMCounter;

	protected:
		//////////////////////////////////////////////////////////////////////////////////
		// Entity 관련
		virtual void	EncodeInsertPacket(LPENTITY entity);
		virtual void	EncodeRemovePacket(LPENTITY entity);
		//////////////////////////////////////////////////////////////////////////////////

	public:
		LPCHARACTER			FindCharacterInView(const char * name, bool bFindPCOnly);
		bool IsVictimInView(LPCHARACTER victim) const;
		void				UpdatePacket();

		//////////////////////////////////////////////////////////////////////////////////
		// FSM (Finite State Machine) 관련
	protected:
		CStateTemplate<CHARACTER>	m_stateMove;
		CStateTemplate<CHARACTER>	m_stateBattle;
		CStateTemplate<CHARACTER>	m_stateIdle;

	public:
		virtual void		StateMove();
		virtual void		StateBattle();
		virtual void		StateIdle();
		virtual void		StateFlag();
		virtual void		StateFlagBase();
		void				StateHorse();

	protected:
		// STATE_IDLE_REFACTORING
		void				__StateIdle_Monster();
		void				__StateIdle_Stone();
		void				__StateIdle_NPC();
		// END_OF_STATE_IDLE_REFACTORING

	public:
		DWORD GetAIFlag() const	{ return m_pointsInstant.dwAIFlag; }

		void				SetAggressive();
		bool				IsAggressive() const;

		void				SetCoward();
		bool				IsCoward() const;
		void				CowardEscape();

		void				SetNoAttackShinsu();
		bool				IsNoAttackShinsu() const;

		void				SetNoAttackChunjo();
		bool				IsNoAttackChunjo() const;

		void				SetNoAttackJinno();
		bool				IsNoAttackJinno() const;

		void				SetAttackMob();
		bool				IsAttackMob() const;

		virtual void			BeginStateEmpty();
		virtual void			EndStateEmpty() {}

		void				RestartAtSamePos();

	protected:
		uint32_t m_dwStateDuration;

	public:
		CHARACTER();
		virtual ~CHARACTER();

		void			Create(const char * c_pszName, DWORD vid, bool isPC);
		void			Destroy();

		void			Disconnect(const char * c_pszReason);

	protected:
		void			Initialize();

	public:
		DWORD			GetPlayerID() const	{ return m_dwPlayerID; }

		void			SetPlayerProto(const TPlayerTable * table);
		void			CreatePlayerProto(TPlayerTable & tab);	// 저장 시 사용

		void			SetProto(const CMob * c_pkMob);
		WORD			GetRaceNum() const;

		void			Save();		// DelayedSave
		void			SaveReal();	// 실제 저장
		void			FlushDelayedSaveItem();

#ifdef ENABLE_MULTI_NAMES
		const char *	GetName(BYTE lang = DEFAULT_LANGUAGE) const;
#else
		const char *	GetName() const;
#endif

		const VID &		GetVID() const		{ return m_vid;		}

		void			SetName(const std::string& name) { m_stName = name; }

		void			SetRace(BYTE race);
		bool			ChangeSex();

		bool IsFemale() const;
		bool IsMale() const;

		DWORD			GetAID() const;
		int				GetChangeEmpireCount() const;
		void			SetChangeEmpireCount();
		int				ChangeEmpire(BYTE empire);

		BYTE			GetJob() const;
		BYTE			GetCharType() const;

		bool			IsPC() const		{ return GetDesc() ? true : false; }
		bool			IsNPC()	const		{ return m_bCharType != CHAR_TYPE_PC; }
		bool			IsMonster()	const	{ return m_bCharType == CHAR_TYPE_MONSTER; }
		bool			IsStone() const		{ return m_bCharType == CHAR_TYPE_STONE; }
		bool			IsDoor() const		{ return m_bCharType == CHAR_TYPE_DOOR; }
		bool			IsBuilding() const	{ return m_bCharType == CHAR_TYPE_BUILDING;  }
		bool			IsWarp() const		{ return m_bCharType == CHAR_TYPE_WARP; }
		bool			IsGoto() const		{ return m_bCharType == CHAR_TYPE_GOTO; }
//		bool			IsPet() const		{ return m_bCharType == CHAR_TYPE_PET; }

		DWORD			GetLastShoutPulse() const	{ return m_pointsInstant.dwLastShoutPulse; }
		void			SetLastShoutPulse(DWORD pulse) { m_pointsInstant.dwLastShoutPulse = pulse; }
		int				GetLevel() const		{ return m_points.level;	}
		void			SetLevel(BYTE level);

		BYTE			GetGMLevel() const;
		BOOL 			IsGM() const;
		void			SetGMLevel();

		DWORD			GetExp() const		{ return m_points.exp;	}
		void			SetExp(DWORD exp)	{ m_points.exp = exp;	}

#ifdef __ENABLE_BLOCK_EXP__
		bool			Block_Exp;
#endif

		DWORD			GetNextExp() const;
#ifdef __NEWPET_SYSTEM__
		DWORD			PetGetNextExp() const;
#endif
		LPCHARACTER		DistributeExp();	// 제일 많이 때린 사람을 리턴한다.
		void			DistributeHP(LPCHARACTER pkKiller);
		void			DistributeSP(LPCHARACTER pkKiller, int iMethod=0);

		void			SetPosition(int pos);
		bool			IsPosition(int pos) const	{ return m_pointsInstant.position == pos ? true : false; }
		int				GetPosition() const		{ return m_pointsInstant.position; }

		void SetPart(uint8_t, uint32_t);
		uint32_t GetPart(uint8_t) const;
		uint32_t GetOriginalPart(uint8_t) const;

		void			SetHP(int hp)		{ m_points.hp = hp; }
		int				GetHP() const		{ return m_points.hp; }

		void			SetSP(int sp)		{ m_points.sp = sp; }
		int				GetSP() const		{ return m_points.sp; }

		void			SetStamina(int stamina)	{ m_points.stamina = stamina; }
		int				GetStamina() const		{ return m_points.stamina; }

		void			SetMaxHP(int iVal)	{ m_pointsInstant.iMaxHP = iVal; }

		int32_t GetMaxHP() const { return m_pointsInstant.iMaxHP; };

		void			SetMaxSP(int iVal)	{ m_pointsInstant.iMaxSP = iVal; }
		int				GetMaxSP() const	{ return m_pointsInstant.iMaxSP; }

		void			SetMaxStamina(int iVal)	{ m_pointsInstant.iMaxStamina = iVal; }
		int				GetMaxStamina() const	{ return m_pointsInstant.iMaxStamina; }

		void			SetRandomHP(int v)	{ m_points.iRandomHP = v; }
		void			SetRandomSP(int v)	{ m_points.iRandomSP = v; }

		int				GetRandomHP() const	{ return m_points.iRandomHP; }
		int				GetRandomSP() const	{ return m_points.iRandomSP; }

		int				GetHPPct() const;
		int				GetSPPct() const;

		void			SetRealPoint(BYTE idx, int val);
		int				GetRealPoint(BYTE idx) const;

		void			SetPoint(BYTE idx, int val);
		int				GetPoint(BYTE idx) const;
		int				GetLimitPoint(BYTE idx) const;
		int				GetPolymorphPoint(BYTE idx) const;

		const TMobTable &	GetMobTable() const;
		BYTE				GetMobRank() const;
		BYTE				GetMobBattleType() const;
		BYTE				GetMobSize() const;
		DWORD				GetMobDamageMin() const;
		DWORD				GetMobDamageMax() const;
		WORD				GetMobAttackRange() const;
		DWORD				GetMobDropItemVnum() const;
		float				GetMobDamageMultiply() const;

		// NEWAI
		bool			IsBerserker() const;
		bool			IsBerserk() const;
		void			SetBerserk(bool mode);

		bool			IsStoneSkinner() const;

		bool			IsGodSpeeder() const;
		bool			IsGodSpeed() const;
		void			SetGodSpeed(bool mode);

		bool			IsDeathBlower() const;
		bool			IsDeathBlow() const;

		bool			IsReviver() const;
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
		bool HasSpawnAnimation() const { return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_SPAWN_ANIMATION); };
		bool IsHalloweenMonster() const { return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_HALLOWEEN); };
#endif
#if defined(ENABLE_WORLDBOSS)
		bool IsWorldBossMonster() const { return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_WORLDBOSS); };
#endif
		bool			HasReviverInParty() const;
		bool			IsRevive() const;
		void			SetRevive(bool mode);
		// NEWAI END

		bool			IsRaceFlag(DWORD dwBit) const;
		bool			IsSummonMonster() const;
		DWORD			GetSummonVnum() const;

		DWORD			GetPolymorphItemVnum() const;
		DWORD			GetMonsterDrainSPPoint() const;

		void			MainCharacterPacket();	// 내가 메인캐릭터라고 보내준다.

		void			ComputePoints();
		void			ComputeBattlePoints();
		void			PointChange(BYTE type, int amount, bool bAmount = false, bool bBroadcast = false
#ifdef __ENABLE_BLOCK_EXP__
, bool bForceExp = false
);
#endif
		void			PointsPacket();
		void			ApplyPoint(BYTE bApplyType, int iVal);
#ifdef __NEWPET_SYSTEM__
		void			SendPetLevelUpEffect(int vid, int type, int value, int amount);
#endif		
		void			CheckMaximumPoints();	// HP, SP 등의 현재 값이 최대값 보다 높은지 검사하고 높다면 낮춘다.

		bool			Show(long lMapIndex, long x, long y, long z = LONG_MAX, bool bShowSpawnMotion = false);

		void			Sitdown(int is_ground);
		void			Standup();

#ifdef ENABLE_ANCIENT_PYRAMID
		void			SetRotation(float fRot, bool bForce = false);
#else
		void			SetRotation(float fRot);
#endif
		void			SetRotationToXY(long x, long y);
		float			GetRotation() const	{ return m_pointsInstant.fRot; }

		void			MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion * packet);
		void			Motion(BYTE motion, LPCHARACTER victim = NULL);

		void			ChatPacket(BYTE type, const char *format, ...);
		void			SendGreetMessage();

		void			ResetPoint(int iLv);

		void			SetBlockMode(BYTE bFlag);
		void			SetBlockModeForce(BYTE bFlag);
		bool			IsBlockMode(BYTE bFlag) const	{ return (m_pointsInstant.bBlockMode & bFlag)?true:false; }

		bool			IsPolymorphed() const		{ return m_dwPolymorphRace>0; }
		bool			IsPolyMaintainStat() const	{ return m_bPolyMaintainStat; } // 이전 스텟을 유지하는 폴리모프.
		void			SetPolymorph(DWORD dwRaceNum, bool bMaintainStat = false);
		DWORD			GetPolymorphVnum() const	{ return m_dwPolymorphRace; }
		int				GetPolymorphPower() const;

		// FISING
		void			fishing();
		bool			IsNearWater() const;
		void			fishing_take();
		// END_OF_FISHING

		// MINING
		void			mining(LPCHARACTER chLoad);
		void			mining_cancel();
		void			mining_take();
		// END_OF_MINING

		void			ResetPlayTime(DWORD dwTimeRemain = 0);

		void			CreateFly(BYTE bType, LPCHARACTER pkVictim);

		void			ResetChatCounter();
		BYTE			IncreaseChatCounter();
		BYTE			GetChatCounter() const;

		void			ResetMountCounter();
		BYTE			IncreaseMountCounter();
		BYTE			GetMountCounter() const;

	protected:
		DWORD			m_dwPolymorphRace;
		bool			m_bPolyMaintainStat;
		DWORD			m_dwLoginPlayTime;
		DWORD			m_dwPlayerID;
		VID				m_vid;
		std::string		m_stName;
#ifdef __NEWPET_SYSTEM__
		BYTE			m_stImmortalSt;
#endif
		BYTE			m_bCharType;
#ifdef __NEWPET_SYSTEM__
		DWORD			m_newpetskillcd[4];
#endif
		CHARACTER_POINT		m_points;
		CHARACTER_POINT_INSTANT	m_pointsInstant;

		int				m_iMoveCount;
		DWORD			m_dwPlayStartTime;
		BYTE			m_bAddChrState;
		bool			m_bSkipSave;
		std::string		m_stMobile;
		char			m_szMobileAuth[5];
		BYTE			m_bChatCounter;

		// End of Basic Points

		//////////////////////////////////////////////////////////////////////////////////
		// Move & Synchronize Positions
		//////////////////////////////////////////////////////////////////////////////////
	public:
		long	SetInvincible(bool arg);
		bool	GetInvincible();
		long	IncreaseMobHP(long lArg);
		long	IncreaseMobRigHP(long lArg);

	private:
		bool	isInvincible;

	public:
		bool			IsStateMove() const			{ return IsState((CState&)m_stateMove); }
		bool			IsStateIdle() const			{ return IsState((CState&)m_stateIdle); }
		bool			IsWalking() const			{ return m_bNowWalking || GetStamina()<=0; }
		void			SetWalking(bool bWalkFlag)	{ m_bWalking=bWalkFlag; }
		void			SetNowWalking(bool bWalkFlag);
		void			ResetWalking()			{ SetNowWalking(m_bWalking); }

		bool			Goto(long x, long y);	// 바로 이동 시키지 않고 목표 위치로 BLENDING 시킨다.
		void			Stop();

		bool			CanMove() const;		// 이동할 수 있는가?

		void			SyncPacket();
		bool			Sync(long x, long y);	// 실제 이 메소드로 이동 한다 (각 종 조건에 의한 이동 불가가 없음)
		bool			Move(long x, long y);	// 조건을 검사하고 Sync 메소드를 통해 이동 한다.
		void			OnMove(bool bIsAttack = false);	// 움직일때 불린다. Move() 메소드 이외에서도 불릴 수 있다.
		DWORD			GetMotionMode() const;
		float			GetMoveMotionSpeed() const;
		float			GetMoveSpeed() const;
		void			CalculateMoveDuration();
		void			SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime=0, int iRot=-1);
		DWORD			GetCurrentMoveDuration() const	{ return m_dwMoveDuration; }
		DWORD			GetWalkStartTime() const	{ return m_dwWalkStartTime; }
		DWORD			GetLastMoveTime() const		{ return m_dwLastMoveTime; }
		DWORD			GetLastAttackTime() const	{ return m_dwLastAttackTime; }

		void			SetLastAttacked(DWORD time);	// 마지막으로 공격받은 시간 및 위치를 저장함

		bool			SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList = true);
		bool			IsSyncOwner(LPCHARACTER ch) const;

		bool			WarpSet(long x, long y, long lRealMapIndex = 0);
		
		
#ifdef ENABLE_CHANNEL_SWITCH_SYSTEM
		bool			SwitchChannel(long newAddr, WORD newPort);
		bool			StartChannelSwitch(long newAddr, WORD newPort);
#endif
		
		void			SetWarpLocation(long lMapIndex, long x, long y);
		void			WarpEnd();
		const PIXEL_POSITION & GetWarpPosition() const { return m_posWarp; }
		bool			WarpToPID(DWORD dwPID, bool checkWarp = true);

		void			SaveExitLocation();
		void			ExitToSavedLocation();

		void			StartStaminaConsume();
		void			StopStaminaConsume();
		bool			IsStaminaConsume() const;
		bool			IsStaminaHalfConsume() const;

		void			ResetStopTime();
		DWORD			GetStopTime() const;

	protected:
		void			ClearSync();

		float m_fSyncTime;
		LPCHARACTER		m_pkChrSyncOwner;
		CHARACTER_LIST	m_kLst_pkChrSyncOwned;	// 내가 SyncOwner인 자들

		PIXEL_POSITION	m_posDest;
		PIXEL_POSITION	m_posStart;
		PIXEL_POSITION	m_posWarp;
		long			m_lWarpMapIndex;

		PIXEL_POSITION	m_posExit;
		long			m_lExitMapIndex;

		DWORD			m_dwMoveStartTime;
		DWORD			m_dwMoveDuration;

		DWORD			m_dwLastMoveTime;
		DWORD			m_dwLastAttackTime;
		DWORD			m_dwWalkStartTime;
		DWORD			m_dwStopTime;

		bool			m_bWalking;
		bool			m_bNowWalking;
		bool			m_bStaminaConsume;
		// End

	public:
		void SyncQuickslot(uint8_t, uint16_t, uint16_t);
		bool GetQuickslot(uint8_t pos, TQuickSlot**);
		bool SetQuickslot(uint8_t pos, TQuickSlot& rSlot, bool bPacket = false);
		bool DelQuickslot(uint8_t pos);
		bool SwapQuickslot(uint8_t, uint8_t);
		void ChainQuickslotItem(LPITEM, uint8_t, uint8_t);

#ifdef __ENABLE_NEW_OFFLINESHOP__
	public:
		offlineshop::CShop*		GetOfflineShop() {return m_pkOfflineShop;}
		void					SetOfflineShop(offlineshop::CShop* pkShop) {m_pkOfflineShop = pkShop;}
		
		offlineshop::CShop*		GetOfflineShopGuest() const {return m_pkOfflineShopGuest;}
		void					SetOfflineShopGuest(offlineshop::CShop* pkShop) {m_pkOfflineShopGuest = pkShop;}
		
		offlineshop::CShopSafebox*
								GetShopSafebox() {return m_pkShopSafebox;}
		void					SetShopSafebox(offlineshop::CShopSafebox* pk);

		void					SetAuction(offlineshop::CAuction* pk) {m_pkAuction = pk;}
		void					SetAuctionGuest(offlineshop::CAuction* pk) {m_pkAuctionGuest = pk;}

		offlineshop::CAuction*	GetAuction() {return m_pkAuction;}
		offlineshop::CAuction*	GetAuctionGuest() const {return m_pkAuctionGuest;}


		//offlineshop-updated 05/08/19
		void					SetLookingOfflineshopOfferList(bool is) { m_bIsLookingOfflineshopOfferList = is; }
		bool					IsLookingOfflineshopOfferList() { return m_bIsLookingOfflineshopOfferList; }
		int						GetOfflineShopUseTime() const { return m_iOfflineShopUseTime; }
		void					SetOfflineShopUseTime() { m_iOfflineShopUseTime = thecore_pulse(); }

	private:
		offlineshop::CShop*			m_pkOfflineShop;
		offlineshop::CShop*			m_pkOfflineShopGuest;
		offlineshop::CShopSafebox*	m_pkShopSafebox;
		offlineshop::CAuction*		m_pkAuction;
		offlineshop::CAuction*		m_pkAuctionGuest;

		//offlineshop-updated 05/08/19
		bool	m_bIsLookingOfflineshopOfferList;
		// patch with warp check
		int		m_iOfflineShopUseTime = 0;
#endif

	protected:
		TQuickSlot m_quickslot[QUICKSLOT_MAX_NUM];
#if defined(ENABLE_GAYA_RENEWAL)
		TPlayerGemItems*	m_gemItems;
#endif

		////////////////////////////////////////////////////////////////////////////////////////
		// Affect
	public:
		void			StartAffectEvent();
		void			ClearAffect(bool bSave=false);
		void			ComputeAffect(CAffect * pkAff, bool bAdd);
		bool			AddAffect(DWORD dwType, BYTE bApplyOn, long lApplyValue, DWORD dwFlag, long lDuration, long lSPCost, bool bOverride, bool IsCube = false);
		void			RefreshAffect();
		bool			RemoveAffect(DWORD dwType);
		bool			IsAffectFlag(DWORD dwAff) const;

		bool			UpdateAffect();	// called from EVENT
		int				ProcessAffect();

		void			LoadAffect(DWORD dwCount, TPacketAffectElement * pElements);
		void			SaveAffect();

		// Affect loading이 끝난 상태인가?
		bool			IsLoadedAffect() const	{ return m_bIsLoadedAffect; }

		bool			IsGoodAffect(BYTE bAffectType) const;

		void			RemoveGoodAffect();
		void			RemoveBadAffect();

		CAffect *		FindAffect(DWORD dwType, BYTE bApply=APPLY_NONE) const;
		const std::list<CAffect *> & GetAffectContainer() const	{ return m_list_pkAffect; }
		bool			RemoveAffect(CAffect * pkAff);

	protected:
		bool			m_bIsLoadedAffect;
		TAffectFlag		m_afAffectFlag;
		std::list<CAffect *>	m_list_pkAffect;

#ifdef ENABLE_SKILLS_BUFF_ALTERNATIVE
	public:
		void						ClearAffectSkills();
		void						SaveAffectSkills(DWORD dwType, BYTE bApplyOn, long lApplyValue, DWORD dwFlag, long lDuration, long lSPCost);
		void						LoadAffectSkills();

	protected:
		std::vector<TAffectSkills>	m_list_pkAffectSkills;
#endif

	public:
		// PARTY_JOIN_BUG_FIX
		void			SetParty(LPPARTY pkParty);
		LPPARTY			GetParty() const	{ return m_pkParty; }

		bool			RequestToParty(LPCHARACTER leader);
		void			DenyToParty(LPCHARACTER member);
		void			AcceptToParty(LPCHARACTER member);

		/// 자신의 파티에 다른 character 를 초대한다.
		/**
		 * @param	pchInvitee 초대할 대상 character. 파티에 참여 가능한 상태이어야 한다.
		 *
		 * 양측 character 의 상태가 파티에 초대하고 초대받을 수 있는 상태가 아니라면 초대하는 캐릭터에게 해당하는 채팅 메세지를 전송한다.
		 */
		void			PartyInvite(LPCHARACTER pchInvitee);

		/// 초대했던 character 의 수락을 처리한다.
		/**
		 * @param	pchInvitee 파티에 참여할 character. 파티에 참여가능한 상태이어야 한다.
		 *
		 * pchInvitee 가 파티에 가입할 수 있는 상황이 아니라면 해당하는 채팅 메세지를 전송한다.
		 */
		void			PartyInviteAccept(LPCHARACTER pchInvitee);

		/// 초대했던 character 의 초대 거부를 처리한다.
		/**
		 * @param [in]	dwPID 초대 했던 character 의 PID
		 */
		void			PartyInviteDeny(DWORD dwPID);

		bool			BuildUpdatePartyPacket(TPacketGCPartyUpdate & out);
		int				GetLeadershipSkillLevel() const;

		bool			CanSummon(int iLeaderShip);

		void			SetPartyRequestEvent(LPEVENT pkEvent) { m_pkPartyRequestEvent = pkEvent; }

	protected:

		/// 파티에 가입한다.
		/**
		 * @param	pkLeader 가입할 파티의 리더
		 */
		void			PartyJoin(LPCHARACTER pkLeader);

		/**
		 * 파티 가입을 할 수 없을 경우의 에러코드.
		 * Error code 는 시간에 의존적인가에 따라 변경가능한(mutable) type 과 정적(static) type 으로 나뉜다.
		 * Error code 의 값이 PERR_SEPARATOR 보다 낮으면 변경가능한 type 이고 높으면 정적 type 이다.
		 */
		enum PartyJoinErrCode {
			PERR_NONE		= 0,	///< 처리성공
			PERR_SERVER,			///< 서버문제로 파티관련 처리 불가
			PERR_DUNGEON,			///< 캐릭터가 던전에 있음
			PERR_OBSERVER,			///< 관전모드임
			PERR_LVBOUNDARY,		///< 상대 캐릭터와 레벨차이가 남
			PERR_LOWLEVEL,			///< 상대파티의 최고레벨보다 30레벨 낮음
			PERR_HILEVEL,			///< 상대파티의 최저레벨보다 30레벨 높음
			PERR_ALREADYJOIN,		///< 파티가입 대상 캐릭터가 이미 파티중
			PERR_PARTYISFULL,		///< 파티인원 제한 초과
			PERR_SEPARATOR,			///< Error type separator.
			PERR_DIFFEMPIRE,		///< 상대 캐릭터와 다른 제국임
			PERR_MAX				///< Error code 최고치. 이 앞에 Error code 를 추가한다.
		};

		/// 파티 가입이나 결성 가능한 조건을 검사한다.
		/**
		 * @param 	pchLeader 파티의 leader 이거나 초대한 character
		 * @param	pchGuest 초대받는 character
		 * @return	모든 PartyJoinErrCode 가 반환될 수 있다.
		 */
		static PartyJoinErrCode	IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);

		/// 파티 가입이나 결성 가능한 동적인 조건을 검사한다.
		/**
		 * @param 	pchLeader 파티의 leader 이거나 초대한 character
		 * @param	pchGuest 초대받는 character
		 * @return	mutable type 의 code 만 반환한다.
		 */
		static PartyJoinErrCode	IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);

		LPPARTY			m_pkParty;
		DWORD			m_dwLastDeadTime;
		LPEVENT			m_pkPartyRequestEvent;

		/**
		 * 파티초청 Event map.
		 * key: 초대받은 캐릭터의 PID
		 * value: event의 pointer
		 *
		 * 초대한 캐릭터들에 대한 event map.
		 */
		typedef std::map< DWORD, LPEVENT >	EventMap;
		EventMap		m_PartyInviteEventMap;

		// END_OF_PARTY_JOIN_BUG_FIX

		////////////////////////////////////////////////////////////////////////////////////////
		// Dungeon
	public:
		void			SetDungeon(LPDUNGEON pkDungeon);
		LPDUNGEON		GetDungeon() const	{ return m_pkDungeon; }
		LPDUNGEON		GetDungeonForce() const;
	protected:
		LPDUNGEON	m_pkDungeon;
		int			m_iEventAttr;

		////////////////////////////////////////////////////////////////////////////////////////
		// Guild
	public:
		void			SetGuild(CGuild * pGuild);
		CGuild*			GetGuild() const	{ return m_pGuild; }

		void			SetWarMap(CWarMap* pWarMap);
		CWarMap*		GetWarMap() const	{ return m_pWarMap; }

	protected:
		CGuild *		m_pGuild;
		DWORD			m_dwUnderGuildWarInfoMessageTime;
		CWarMap *		m_pWarMap;

		////////////////////////////////////////////////////////////////////////////////////////
		// Item related
	public:
		bool			CanHandleItem(bool bSkipRefineCheck = false, bool bSkipObserver = false); // 아이템 관련 행위를 할 수 있는가?

		bool			IsItemLoaded() const	{ return m_bItemLoaded; }
		void			SetItemLoaded()	{ m_bItemLoaded = true; }

		void			ClearItem();

#ifdef ENABLE_SORT_INVEN	
		void			EditMyInven();
		void			EditMyExtraInven();
#endif
		
#ifdef __HIGHLIGHT_SYSTEM__
		void			SetItem(TItemPos Cell, LPITEM item, bool isHighLight = false);
#else
		void			SetItem(TItemPos Cell, LPITEM item);
#endif
		LPITEM			GetItem(TItemPos Cell) const;
		LPITEM			GetInventoryItem(WORD wCell) const;
#ifdef ENABLE_EXTRA_INVENTORY
		LPITEM			GetExtraInventoryItem(WORD wCell) const;
		void			SetNextSortExtraInventoryPulse(int pulse) { m_sortExtraInventoryPulse = pulse; }
		int				GetSortExtraInventoryPulse() { return m_sortExtraInventoryPulse; }
		int				m_sortExtraInventoryPulse;
#endif
		bool			IsEmptyItemGrid(TItemPos Cell, BYTE size, int iExceptionCell = -1) const;

		void			SetWear(BYTE bCell, LPITEM item);
		LPITEM			GetWear(BYTE bCell) const;

		// MYSHOP_PRICE_LIST
		void			UseSilkBotary(void); 		/// 비단 보따리 아이템의 사용

		/// DB 캐시로 부터 받아온 가격정보 리스트를 유저에게 전송하고 보따리 아이템 사용을 처리한다.
		/**
		 * @param [in] p	가격정보 리스트 패킷
		 *
		 * 접속한 후 처음 비단 보따리 아이템 사용 시 UseSilkBotary 에서 DB 캐시로 가격정보 리스트를 요청하고
		 * 응답받은 시점에 이 함수에서 실제 비단보따리 사용을 처리한다.
		 */
		void			UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p);
		// END_OF_MYSHOP_PRICE_LIST

		bool			UseItemEx(LPITEM item, TItemPos DestCell);
		bool			UseItem(TItemPos Cell, TItemPos DestCell = NPOS);

		// ADD_REFINE_BUILDING
		bool			IsRefineThroughGuild() const;
		CGuild *		GetRefineGuild() const;
#if defined(ENABLE_NEW_GOLD_LIMIT)
		uint64_t ComputeRefineFee(uint64_t cost, uint8_t multiply = 5) const;
		void PayRefineFee(uint64_t);
#else
		uint32_t ComputeRefineFee(uint32_t cost, uint8_t multiply = 5) const;
		void PayRefineFee(uint32_t);
#endif
		void			SetRefineNPC(LPCHARACTER ch);
		// END_OF_ADD_REFINE_BUILDING

		bool			RefineItem(LPITEM pkItem, LPITEM pkTarget);
		bool			DropItem(TItemPos Cell, WORD bCount = 0);
		bool			DestroyItem(TItemPos Cell);
		void			ItemDivision(TItemPos Cell);
		bool			GiveRecallItem(LPITEM item);
		void			ProcessRecallItem(LPITEM item);

		//	void			PotionPacket(int iPotionType);
		void			EffectPacket(int enumEffectType);
		void			SpecificEffectPacket(const char filename[128]);

		// ADD_MONSTER_REFINE
		bool			DoRefine(LPITEM item, bool bMoneyOnly = false);
		// END_OF_ADD_MONSTER_REFINE

		bool			DoRefineWithScroll(LPITEM item);
		bool			RefineInformation(BYTE bCell, BYTE bType, int iAdditionalCell = -1);

		void			SetRefineMode(int iAdditionalCell = -1);
		void			ClearRefineMode();

		bool			GiveItem(LPCHARACTER victim, TItemPos Cell);
		bool			CanReceiveItem(LPCHARACTER from, LPITEM item) const;
		void			ReceiveItem(LPCHARACTER from, LPITEM item);
		bool			GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector <DWORD> &dwItemVnums,
							std::vector <DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count);

		bool			MoveItem(TItemPos pos, TItemPos change_pos, WORD count);
		bool			PickupItem(DWORD vid);
		bool			EquipItem(LPITEM item, int iCandidateCell = -1);
		bool			UnequipItem(LPITEM item);

		// 현재 item을 착용할 수 있는 지 확인하고, 불가능 하다면 캐릭터에게 이유를 알려주는 함수
		bool			CanEquipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);

		// 착용중인 item을 벗을 수 있는 지 확인하고, 불가능 하다면 캐릭터에게 이유를 알려주는 함수
		bool			CanUnequipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);

		bool			SwapItem(BYTE bCell, BYTE bDestCell);
		
		LPITEM			AutoGiveItem(DWORD dwItemVnum, WORD bCount = 1, int iRarePct = -1, bool bMsg = true
#ifdef __HIGHLIGHT_SYSTEM__
		, bool isHighLight = true
#endif
		);
		void AutoGiveItem(LPITEM item, bool longOwnerShip = false
#ifdef __HIGHLIGHT_SYSTEM__
, bool isHighLight = true
#endif
#ifdef USE_PICKUP_FILTER
, bool bWithMsg = false
#endif
		);
#ifdef ENABLE_DS_REFINE_ALL
		bool	AutoGiveDS(LPITEM item, bool longOwnerShip = false);
#endif
		bool			CanTakeInventoryItem(LPITEM item, TItemPos* pos);
		
#ifdef ENABLE_EXTRA_INVENTORY
		int				GetEmptyExtraInventory(LPITEM pItem) const;
		int				GetEmptyExtraInventory(BYTE size, BYTE category) const;
#endif
		
		int				GetEmptyInventory(BYTE size) const;
		int				GetEmptyDragonSoulInventory(LPITEM pItem) const;
#if defined(USE_REQUIRMENTS_TO_OPEN_DS_CHESTS)
		bool GetEmptyInEachDsAntiqueCategory() const;
#endif
		void			CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const;

		int				CountEmptyInventory() const;

		int 			CountSpecifyItemRenewal(DWORD vnum) const;
		
		int				CountSpecifyItem(DWORD vnum) const;
		void			RemoveSpecifyItem(DWORD vnum, int count = 1, bool cuberenewal = false);
		LPITEM FindSpecifyItem(DWORD vnum) const;
		LPITEM FindItemByID(DWORD id) const;

		int				CountSpecifyTypeItem(BYTE type) const;
		void			RemoveSpecifyTypeItem(BYTE type, int count = 1);

		bool			IsEquipUniqueItem(DWORD dwItemVnum) const;

		// CHECK_UNIQUE_GROUP
		bool			IsEquipUniqueGroup(DWORD dwGroupVnum) const;
		// END_OF_CHECK_UNIQUE_GROUP

		void			SendEquipment(LPCHARACTER ch);
		// End of Item

	protected:

		/// 한 아이템에 대한 가격정보를 전송한다.
		/**
		 * @param [in]	dwItemVnum 아이템 vnum
		 * @param [in]	dwItemPrice 아이템 가격
		 */
#if defined(ENABLE_NEW_GOLD_LIMIT)
		void SendMyShopPriceListCmd(DWORD dwItemVnum, uint64_t dwItemPrice);
#else
		void SendMyShopPriceListCmd(DWORD dwItemVnum, uint32_t dwItemPrice);
#endif
		bool			m_bNoOpenedShop;	///< 이번 접속 후 개인상점을 연 적이 있는지의 여부(열었던 적이 없다면 true)

		bool			m_bItemLoaded;
		int				m_iRefineAdditionalCell;
		bool			m_bUnderRefine;
		DWORD			m_dwRefineNPCVID;

	public:
#if defined(ENABLE_NEW_GOLD_LIMIT)
		uint64_t GetGold() const { return m_points.gold; }
		void SetGold(uint64_t gold) { m_points.gold = gold; }
		uint64_t GetAllowedGold() const;
		void GiveGold(uint64_t);
		void ChangeGold(int64_t);
#else
		uint32_t GetGold() const { return m_points.gold; }
		void SetGold(uint32_t gold) { m_points.gold = gold; }
		uint32_t GetAllowedGold() const;
		void GiveGold(uint32_t);
#endif
#if defined(ENABLE_GAYA_RENEWAL)
		long GetGem() const { return m_points.gem; }
		void SetGem(long gem) { m_points.gem = gem; }
#endif
#ifdef ENABLE_PVP_ADVANCED
		int				GetDuel(const char* type) const;
		void			SetDuel(const char* type, int value);
#endif

		// End of Money

		////////////////////////////////////////////////////////////////////////////////////////
		// Shop related
	public:
		void			SetShop(LPSHOP pkShop);
		LPSHOP			GetShop() const { return m_pkShop; }
		void			ShopPacket(BYTE bSubHeader);

		void			SetShopOwner(LPCHARACTER ch) { m_pkChrShopOwner = ch; }
		LPCHARACTER		GetShopOwner() const { return m_pkChrShopOwner;}

		void OpenMyShop(const char*, TShopItemTable*, uint8_t);
		LPSHOP			GetMyShop() const { return m_pkMyShop; }
		void			CloseMyShop();
#ifdef ENABLE_PVP_ADVANCED
		void			DestroyPvP();
#endif

	protected:

		LPSHOP			m_pkShop;
		LPSHOP			m_pkMyShop;
		std::string		m_stShopSign;
		LPCHARACTER		m_pkChrShopOwner;
		// End of shop

#ifdef __SKILL_COLOR_SYSTEM__
	public:
		void			SetSkillColor(DWORD* dwSkillColor);
		DWORD* GetSkillColor() { return m_dwSkillColor[0]; }

	protected:
		DWORD			m_dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif

		////////////////////////////////////////////////////////////////////////////////////////
		// Exchange related
	public:
		bool			ExchangeStart(LPCHARACTER victim);
		void			SetExchange(CExchange * pkExchange);
		CExchange *		GetExchange() const	{ return m_pkExchange;	}
#if defined(ENABLE_GAYA_RENEWAL)
		void OpenGemShop();
		void RefreshGemShop();
		void OpenGemShopFirstTime();
		void RefreshGemShopItems();
		void RefreshGemShopWithItem();
		void GemShopBuy(BYTE bPos);
		void GemShopAdd(BYTE bPos);
		bool CreateGaya(int glimmerstone_count, LPITEM metinstone_item, int cost, int pct);
#endif

	protected:
		CExchange *		m_pkExchange;
		// End of Exchange

#ifdef __DUNGEON_INFO_SYSTEM__
	public:
		void				SetQuestDamage(int race, int dmg);
		int					GetQuestDamage(int race);

	private:
		std::map<int, int>	dungeonDamage;
#endif

		////////////////////////////////////////////////////////////////////////////////////////
		// Battle
	public:
		struct TBattleInfo
		{
			int64_t iTotalDamage;
			int iAggro;

			TBattleInfo(int64_t iTot, int iAggr)
				: iTotalDamage(iTot), iAggro(iAggr)
				{}
		};
		typedef std::map<VID, TBattleInfo>	TDamageMap;

		typedef struct SAttackLog
		{
			DWORD	dwVID;
			DWORD	dwTime;
		} AttackLog;


#ifdef __ENABLE_BERAN_ADDONS__
		bool				IsBeranMap(int lMapIndex);
#endif


		bool				Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL);
		bool				__Profile__Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL);
		void				DeathPenalty(BYTE bExpLossPercent);
		void				ReviveInvisible(int iDur);

		bool				Attack(LPCHARACTER pkVictim, BYTE bType = 0);
		bool				IsAlive() const		{ return m_pointsInstant.position == POS_DEAD ? false : true; }
		bool				CanFight() const;

		bool				CanBeginFight() const;
		void				BeginFight(LPCHARACTER pkVictim); // pkVictimr과 싸우기 시작한다. (강제적임, 시작할 수 있나 체크하려면 CanBeginFight을 사용)

		bool				CounterAttack(LPCHARACTER pkChr); // 반격하기 (몬스터만 사용)

		bool				IsStun() const;
		void				Stun();
		bool				IsDead() const;
		void				Dead(LPCHARACTER pkKiller = NULL, bool bImmediateDead=false);
#ifdef __NEWPET_SYSTEM__
		void				SetImmortal(int st) { m_stImmortalSt = st; };
		bool				IsImmortal() { return 1 == m_stImmortalSt; };
		void				SetNewPetSkillCD(int s, DWORD time) { m_newpetskillcd[s] = time; };
		DWORD				GetNewPetSkillCD(int s) { return m_newpetskillcd[s]; };
#endif
		void				Reward(bool bItemDrop);
		void				RewardGold(LPCHARACTER pkAttacker);

		bool				Shoot(BYTE bType);
		void				FlyTarget(DWORD dwTargetVID, long x, long y, BYTE bHeader);

		void				ForgetMyAttacker();
		void				AggregateMonster();
		void				AggregateMonster2();
		void				AttractRanger();
		void				PullMonster();

		[[nodiscard]] int GetBowRange() const;
		int					GetArrowAndBow(LPITEM * ppkBow, LPITEM * ppkArrow, int iArrowCount = 1);
		void				UseArrow(LPITEM pkArrow, DWORD dwArrowCount);

		void				AttackedByPoison(LPCHARACTER pkAttacker);
		void				RemovePoison();
		void				AttackedByFire(LPCHARACTER pkAttacker, int amount, int count);
		void				RemoveFire();

		void				UpdateAlignment(int iAmount);
		int					GetAlignment() const;

		//선악치 얻기
		int					GetRealAlignment() const;
		void				ShowAlignment(bool bShow);

		void				SetKillerMode(bool bOn);
		bool				IsKillerMode() const;
		void				UpdateKillerMode();

		BYTE				GetPKMode() const;
		void				SetPKMode(BYTE bPKMode);

		void				ItemDropPenalty(LPCHARACTER pkKiller);

		void				UpdateAggrPoint(LPCHARACTER ch, EDamageType type, int dam);

		//
		// HACK
		//
	public:
		void SetComboSequence(BYTE seq);
		BYTE GetComboSequence() const;

		void SetLastComboTime(DWORD time);
		DWORD GetLastComboTime() const;

		int GetValidComboInterval() const;
		void SetValidComboInterval(int interval);

		BYTE GetComboIndex() const;

		void IncreaseComboHackCount(int k = 1);
		void ResetComboHackCount();
		void SkipComboAttackByTime(int interval);
		DWORD GetSkipComboAttackByTime() const;

	protected:
		BYTE m_bComboSequence;
		DWORD m_dwLastComboTime;
		int m_iValidComboInterval;
		BYTE m_bComboIndex;
		int m_iComboHackCount;
		DWORD m_dwSkipComboAttackByTime;

	protected:
		void OnUpdateAlignment();
		void				UpdateAggrPointEx(LPCHARACTER ch, EDamageType type, int dam, TBattleInfo & info);
		void				ChangeVictimByAggro(int iNewAggro, LPCHARACTER pNewVictim);

		DWORD				m_dwFlyTargetID;
		std::set<DWORD>	m_vec_dwFlyTargets;
		TDamageMap			m_map_kDamage;	// 어떤 캐릭터가 나에게 얼마만큼의 데미지를 주었는가?
//		AttackLog			m_kAttackLog;
		DWORD				m_dwKillerPID;

		int					m_iAlignment;		// Lawful/Chaotic value -200000 ~ 200000
		int					m_iRealAlignment;

		int					m_iKillerModePulse;
		BYTE				m_bPKMode;

		// Aggro
		DWORD				m_dwLastVictimSetTime;
		int					m_iMaxAggro;
		// End of Battle

		// Stone
	public:
		void				SetStone(LPCHARACTER pkChrStone);
		void				ClearStone();
		void				DetermineDropMetinStone();
		DWORD				GetDropMetinStoneVnum() const { return m_dwDropMetinStone; }
		BYTE				GetDropMetinStonePct() const { return m_bDropMetinStonePct; }
		void				DetermineDropMetinStofa();
		DWORD				GetDropMetinStofaVnum() const { return m_dwDropMetinStofa; }
		BYTE				GetDropMetinStofaPct() const { return m_bDropMetinStofaPct; }

	protected:
		LPCHARACTER			m_pkChrStone;		// 나를 스폰한 돌
		CHARACTER_SET		m_set_pkChrSpawnedBy;	// 내가 스폰한 놈들
		DWORD				m_dwDropMetinStone;
		BYTE				m_bDropMetinStonePct;
		DWORD				m_dwDropMetinStofa;
		BYTE				m_bDropMetinStofaPct;

#ifdef ENABLE_RANKING
	protected:
		long long	m_lRankPoints[RANKING_MAX_CATEGORIES];

	public:
		long long	GetRankPoints(int iArg);
		void		SetRankPoints(int iArg, long long lPoint);
		void		RankingSubcategory(int iArg);
#endif
#ifdef ENABLE_ATTR_COSTUMES
	public:
		void	SetAttrDialogRemove(int iArg)	{ attrdialog_remove = iArg; }
		int		GetAttrDialogRemove() const { return attrdialog_remove; }

	protected:
		int	attrdialog_remove;
#endif
#ifdef ENABLE_NEW_PET_EDITS
	public:
		void	SetPetEnchant(int iArg)	{ petenchant = iArg; }
		int		GetPetEnchant() const { return petenchant; }

	protected:
		int	petenchant;
#endif
	public:
		enum
		{
			SKILL_UP_BY_POINT,
			SKILL_UP_BY_BOOK,
			SKILL_UP_BY_TRAIN,

			// ADD_GRANDMASTER_SKILL
			SKILL_UP_BY_QUEST,
			// END_OF_ADD_GRANDMASTER_SKILL
		};

		void				SkillLevelPacket();
		void				SkillLevelUp(DWORD dwVnum, BYTE bMethod = SKILL_UP_BY_POINT);
		bool				SkillLevelDown(DWORD dwVnum);
		// ADD_GRANDMASTER_SKILL
		bool				UseSkill(DWORD dwVnum, LPCHARACTER pkVictim, bool bUseGrandMaster = true);
#if defined(ENABLE_FIX_WAITHACK)
		bool IsSkillCooldown(DWORD dwVnum, float fSkillPower) { return m_SkillUseInfo[dwVnum].IsSkillCooldown(dwVnum, fSkillPower) ? true : false; }
#endif
		void				ResetSkill();
		void				SetSkillLevel(DWORD dwVnum, BYTE bLev);
		int					GetUsedSkillMasterType(DWORD dwVnum);

		bool				IsLearnableSkill(DWORD dwSkillVnum) const;
		// END_OF_ADD_GRANDMASTER_SKILL

		bool				CheckSkillHitCount(const BYTE SkillID, const VID dwTargetVID);
		bool				CanUseSkill(DWORD dwSkillVnum) const;
		bool				IsUsableSkillMotion(DWORD dwMotionIndex) const;
		int					GetSkillLevel(DWORD dwVnum) const;
		int					GetSkillMasterType(DWORD dwVnum) const;
		int					GetSkillPower(DWORD dwVnum, BYTE bLevel = 0) const;

		time_t				GetSkillNextReadTime(DWORD dwVnum) const;
		void				SetSkillNextReadTime(DWORD dwVnum, time_t time);
		void				SkillLearnWaitMoreTimeMessage(DWORD dwVnum);

		void				ComputePassiveSkill(DWORD dwVnum);
#ifdef USE_NEW_GYEONGGONG_SKILL
		int					ComputeGyeongGongSkill(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#endif
		int					ComputeSkill(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#ifdef GROUP_BUFF
		int					ComputeSkillParty(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#endif
		int					ComputeSkillAtPosition(DWORD dwVnum, const PIXEL_POSITION& posTarget, BYTE bSkillLevel = 0);
		void				ComputeSkillPoints();

		void				SetSkillGroup(BYTE bSkillGroup);
		BYTE				GetSkillGroup() const		{ return m_points.skill_group; }

		int					ComputeCooltime(int time);

		void				GiveRandomSkillBook();

		void				DisableCooltime();
		bool				LearnSkillByBook(DWORD dwSkillVnum, BYTE bProb = 0);
		bool				LearnGrandMasterSkill(DWORD dwSkillVnum);

	private:
		bool				m_bDisableCooltime;
		DWORD				m_dwLastSkillTime;	///< 마지막으로 skill 을 쓴 시간(millisecond).
		// End of Skill

		// MOB_SKILL
	public:
		bool				HasMobSkill() const;
		size_t				CountMobSkill() const;
		const TMobSkillInfo * GetMobSkill(unsigned int idx) const;
		bool				CanUseMobSkill(unsigned int idx) const;
		bool				UseMobSkill(unsigned int idx);
		void				ResetMobSkillCooltime();
	protected:
		DWORD				m_adwMobSkillCooltime[MOB_SKILL_MAX_NUM];
		// END_OF_MOB_SKILL

		// for SKILL_MUYEONG
	public:
		void				StartMuyeongEvent();
		void				StopMuyeongEvent();
#ifdef USE_NEW_GYEONGGONG_SKILL
		void				StartGyeongGongEvent();
		void				StopGyeongGongEvent();
#endif

	private:
		LPEVENT				m_pkMuyeongEvent;
#ifdef USE_NEW_GYEONGGONG_SKILL
		LPEVENT				m_pkGyeongGongEvent;
#endif

		// for SKILL_CHAIN lighting
	public:
		int					GetChainLightningIndex() const { return m_iChainLightingIndex; }
		void				IncChainLightningIndex() { ++m_iChainLightingIndex; }
		void				AddChainLightningExcept(LPCHARACTER ch) { m_setExceptChainLighting.insert(ch); }
		void				ResetChainLightningIndex() { m_iChainLightingIndex = 0; m_setExceptChainLighting.clear(); }
		int					GetChainLightningMaxCount() const;
		const CHARACTER_SET& GetChainLightingExcept() const { return m_setExceptChainLighting; }

	private:
		int					m_iChainLightingIndex;
		CHARACTER_SET m_setExceptChainLighting;

		// for SKILL_EUNHYUNG
	public:
		void				SetAffectedEunhyung();
		void				ClearAffectedEunhyung() { m_dwAffectedEunhyungLevel = 0; }
		bool				GetAffectedEunhyung() const { return m_dwAffectedEunhyungLevel; }

	private:
		DWORD				m_dwAffectedEunhyungLevel;

		//
		// Skill levels
		//
	protected:
		TPlayerSkill*					m_pSkillLevels;
		std::unordered_map<BYTE, int>		m_SkillDamageBonus;
		std::map<int, TSkillUseInfo>	m_SkillUseInfo;

		////////////////////////////////////////////////////////////////////////////////////////
		// AI related
	public:
		void			AssignTriggers(const TMobTable * table);
		LPCHARACTER		GetVictim() const;	// 공격할 대상 리턴
		void			SetVictim(LPCHARACTER pkVictim);
		LPCHARACTER		GetNearestVictim(LPCHARACTER pkChr);
		LPCHARACTER		GetProtege() const;	// 보호해야 할 대상 리턴

		bool			Follow(LPCHARACTER pkChr, float fMinimumDistance = 150.0f);
		bool			Return();
		bool			IsGuardNPC() const;
		bool			IsChangeAttackPosition(LPCHARACTER target) const;
		void			ResetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time() - AI_CHANGE_ATTACK_POISITION_TIME_NEAR;}
		void			SetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time();}

		bool			OnIdle();

		void			OnAttack(LPCHARACTER pkChrAttacker);
		void			OnClick(LPCHARACTER pkChrCauser);

		VID				m_kVIDVictim;

	protected:
		DWORD			m_dwLastChangeAttackPositionTime;
		CTrigger		m_triggerOnClick;
		// End of AI

		////////////////////////////////////////////////////////////////////////////////////////
		// Target
	protected:
		LPCHARACTER				m_pkChrTarget;		// 내 타겟
		CHARACTER_SET	m_set_pkChrTargetedBy;	// 나를 타겟으로 가지고 있는 사람들

	public:
		void				SetTarget(LPCHARACTER pkChrTarget);
		void				BroadcastTargetPacket();
		void				ClearTarget();
		void				CheckTarget();
		LPCHARACTER			GetTarget() const { return m_pkChrTarget; }

		////////////////////////////////////////////////////////////////////////////////////////
		// Safebox
	public:
		int					GetSafeboxSize() const;
		void				QuerySafeboxSize();
		void				SetSafeboxSize(int size);

		CSafebox *			GetSafebox() const;
		void				LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem * pItems);
		void				ChangeSafeboxSize(BYTE bSize);
		void				CloseSafebox();

		/// 창고 열기 요청
		/**
		 * @param [in]	pszPassword 1자 이상 6자 이하의 창고 비밀번호
		 *
		 * DB 에 창고열기를 요청한다.
		 * 창고는 중복으로 열지 못하며, 최근 창고를 닫은 시간으로 부터 10초 이내에는 열 지 못한다.
		 */
		void				ReqSafeboxLoad(const char* pszPassword);

		/// 창고 열기 요청의 취소
		/**
		 * ReqSafeboxLoad 를 호출하고 CloseSafebox 하지 않았을 때 이 함수를 호출하면 창고를 열 수 있다.
		 * 창고열기의 요청이 DB 서버에서 실패응답을 받았을 경우 이 함수를 사용해서 요청을 할 수 있게 해준다.
		 */
		void				CancelSafeboxLoad( void ) { m_bOpeningSafebox = false; }

		void				SetMallLoadTime(int t) { m_iMallLoadTime = t; }
		int					GetMallLoadTime() const { return m_iMallLoadTime; }

		CSafebox *			GetMall() const;
		void				LoadMall(int iItemCount, TPlayerItem * pItems);
		void				CloseMall();

		void				SetSafeboxOpenPosition();
		float				GetDistanceFromSafeboxOpen() const;

	protected:
		CSafebox *			m_pkSafebox;
		int					m_iSafeboxSize;
		int					m_iSafeboxLoadTime;
		bool				m_bOpeningSafebox;	///< 창고가 열기 요청 중이거나 열려있는가 여부, true 일 경우 열기요청이거나 열려있음.

		CSafebox *			m_pkMall;
		int					m_iMallLoadTime;

		PIXEL_POSITION		m_posSafeboxOpen;

		////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////
		// Mounting
	public:
		void MountVnum(uint32_t vnum);
		DWORD				GetMountVnum() const { return m_dwMountVnum; }
		DWORD				GetLastMountTime() const { return m_dwMountTime; }

		bool				CanUseHorseSkill();

		// Horse
		virtual	void		SetHorseLevel(int iLevel);

		virtual	bool		StartRiding();
		virtual	bool		StopRiding();

		virtual	DWORD		GetMyHorseVnum() const;

		virtual	void		HorseDie();
		virtual bool		ReviveHorse();

		virtual void		SendHorseInfo();
		virtual	void		ClearHorseInfo();

		void				HorseSummon(bool bSummon, bool bFromFar = false, DWORD dwVnum = 0, const char* name = 0);

		LPCHARACTER			GetHorse() const			{ return m_chHorse; }	 // 현재 소환중인 말
		LPCHARACTER			GetRider() const; // rider on horse
		void				SetRider(LPCHARACTER ch);

		bool				IsRiding() const;
#ifdef __ATTR_TRANSFER_SYSTEM__
	public:
		LPITEM* GetAttrTransferItem() { return m_pointsInstant.playerSlots ? m_pointsInstant.playerSlots->pAttrTransferItems : nullptr; }
		bool IsAttrTransferOpen() const { return (m_pointsInstant.playerSlots ? m_pointsInstant.playerSlots->pAttrTransferNpc != nullptr : false); }
		void SetAttrTransferNpc(LPCHARACTER npc) { m_pointsInstant.playerSlots->pAttrTransferNpc = npc; }
		bool CanDoAttrTransfer() const;
#endif
#ifdef __PET_SYSTEM__
	public:
		CPetSystem*			GetPetSystem()				{ return m_petSystem; }

	protected:
		CPetSystem*			m_petSystem;

	public:
#endif

#ifdef __NEWPET_SYSTEM__
	public:
		CNewPetSystem*			GetNewPetSystem() { return m_newpetSystem; }

	protected:
		CNewPetSystem*			m_newpetSystem;

	public:
#endif

	protected:
		LPCHARACTER			m_chHorse;
		LPCHARACTER			m_chRider;

		DWORD				m_dwMountVnum;
		DWORD				m_dwMountTime;

		BYTE				m_bSendHorseLevel;
		BYTE				m_bSendHorseHealthGrade;
		BYTE				m_bSendHorseStaminaGrade;

		////////////////////////////////////////////////////////////////////////////////////////
		// Detailed Log
	public:
		void				DetailLog() { m_bDetailLog = !m_bDetailLog; }
		void				ToggleMonsterLog();
		void				MonsterLog(const char* format, ...);
	private:
		bool				m_bDetailLog;
		bool				m_bMonsterLog;

		////////////////////////////////////////////////////////////////////////////////////////
		// Empire

	public:
		void 				SetEmpire(BYTE bEmpire);
		BYTE				GetEmpire() const { return m_bEmpire; }

	protected:
		BYTE				m_bEmpire;

		////////////////////////////////////////////////////////////////////////////////////////
		// Regen
	public:
		void				SetRegen(LPREGEN pkRegen);

	protected:
		PIXEL_POSITION			m_posRegen;
		float				m_fRegenAngle;
		LPREGEN				m_pkRegen;
		size_t				regen_id_; // to help dungeon regen identification
		// End of Regen

		////////////////////////////////////////////////////////////////////////////////////////
		// Resists & Proofs
	public:
		bool				CannotMoveByAffect() const;	// 특정 효과에 의해 움직일 수 없는 상태인가?
		bool				IsImmune(DWORD dwImmuneFlag);
		void				SetImmuneFlag(DWORD dw) { m_pointsInstant.dwImmuneFlag = dw; }

	protected:
		void				ApplyMobAttribute(const TMobTable* table);
		// End of Resists & Proofs

		////////////////////////////////////////////////////////////////////////////////////////
		// QUEST
		//
	public:
		void				SetQuestNPCID(DWORD vid);
		DWORD				GetQuestNPCID() const { return m_dwQuestNPCVID; }
		LPCHARACTER			GetQuestNPC() const;

		void				SetQuestItemPtr(LPITEM item);
		void				ClearQuestItemPtr();
		LPITEM GetQuestItemPtr() const;

		void				SetQuestBy(DWORD dwQuestVnum)	{ m_dwQuestByVnum = dwQuestVnum; }
		DWORD				GetQuestBy() const			{ return m_dwQuestByVnum; }

		int					GetQuestFlag(const std::string& flag) const;
		void				SetQuestFlag(const std::string& flag, int value);

		void				ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID);

	private:
		DWORD				m_dwQuestNPCVID;
		DWORD				m_dwQuestByVnum;
		uint32_t m_dwQuestItemVID;
		LPITEM m_pQuestItem;

		// Events
	public:
		bool				StartStateMachine(int iPulse = 1);
		void				StopStateMachine();
		void				UpdateStateMachine(DWORD dwPulse);
		void				SetNextStatePulse(int iPulseNext);

		// 캐릭터 인스턴스 업데이트 함수. 기존엔 이상한 상속구조로 CFSM::Update 함수를 호출하거나 UpdateStateMachine 함수를 사용했는데, 별개의 업데이트 함수 추가함.
		void				UpdateCharacter(DWORD dwPulse);

	protected:
		DWORD				m_dwNextStatePulse;

		// Marriage
	public:
		LPCHARACTER			GetMarryPartner() const;
		void				SetMarryPartner(LPCHARACTER ch);
		int					GetMarriageBonus(DWORD dwItemVnum, bool bSum = true);

		void				SetWeddingMap(marriage::WeddingMap* pMap);
		marriage::WeddingMap* GetWeddingMap() const { return m_pWeddingMap; }

	private:
		marriage::WeddingMap* m_pWeddingMap;
		LPCHARACTER			m_pkChrMarried;

		// Warp Character
	public:
		void				StartWarpNPCEvent();

	public:
		void				StartSaveEvent();
		void				StartRecoveryEvent();
		void				StartDestroyWhenIdleEvent();

		LPEVENT				m_pkDeadEvent;
		LPEVENT				m_pkStunEvent;
		LPEVENT				m_pkSaveEvent;
		LPEVENT				m_pkRecoveryEvent;
		LPEVENT				m_pkTimedEvent;
		LPEVENT				m_pkFishingEvent;
		LPEVENT				m_pkAffectEvent;
		LPEVENT				m_pkPoisonEvent;

		LPEVENT				m_pkFireEvent;
		LPEVENT				m_pkWarpNPCEvent;
		//DELAYED_WARP
		//END_DELAYED_WARP

		// MINING
		LPEVENT				m_pkMiningEvent;
		// END_OF_MINING

		LPEVENT				m_pkDestroyWhenIdleEvent;
		LPEVENT				m_pkPetSystemUpdateEvent;
#ifdef __NEWPET_SYSTEM__
		LPEVENT				m_pkNewPetSystemUpdateEvent;
		LPEVENT				m_pkNewPetSystemExpireEvent;
#endif
#ifdef __ENABLE_PREMIUM_PLAYERS__
		LPEVENT 			m_pkPremiumPlayersUpdateEvent;
#endif
		bool IsWarping() const { return m_bWarping; }

		bool				m_bHasPoisoned;

		const CMob *		m_pkMobData;
		CMobInstance *		m_pkMobInst;

		std::map<int, LPEVENT> m_mapMobSkillEvent;

		friend struct FuncSplashDamage;
		friend struct FuncSplashAffect;
		friend class CFuncShoot;

    private:
        int32_t m_aiPremiumTimes[PREMIUM_MAX_NUM];
		bool m_bWarping;
    public:
        int32_t GetPremiumRemainSeconds(uint8_t t) const;

		// CHANGE_ITEM_ATTRIBUTES
		// static const DWORD		msc_dwDefaultChangeItemAttrCycle;	///< 디폴트 아이템 속성변경 가능 주기
		static const char		msc_szLastChangeItemAttrFlag[];		///< 최근 아이템 속성을 변경한 시간의 Quest Flag 이름
		// static const char		msc_szChangeItemAttrCycleFlag[];		///< 아이템 속성병경 가능 주기의 Quest Flag 이름
		// END_OF_CHANGE_ITEM_ATTRIBUTES

		// NEW_HAIR_STYLE_ADD
	public :
		bool ItemProcess_Hair(LPITEM item, int iDestCell);
		// END_NEW_HAIR_STYLE_ADD

	public :
		void ClearSkill();
		void ClearSubSkill();

		// RESET_ONE_SKILL
		bool ResetOneSkill(DWORD dwVnum);
		// END_RESET_ONE_SKILL

	private :
		void SendDamagePacket(LPCHARACTER pAttacker, int Damage, BYTE DamageFlag);

	// ARENA
	private :
		CArena *m_pArena;
		bool m_ArenaObserver;
		int m_nPotionLimit;

	public :
		void 	SetArena(CArena* pArena) { m_pArena = pArena; }
		void	SetArenaObserverMode(bool flag) { m_ArenaObserver = flag; }

		CArena* GetArena() const { return m_pArena; }
		bool	GetArenaObserverMode() const { return m_ArenaObserver; }

		void	SetPotionLimit(int count) { m_nPotionLimit = count; }
		int		GetPotionLimit() const { return m_nPotionLimit; }
	// END_ARENA

		//PREVENT_TRADE_WINDOW
	public:
		bool	IsOpenSafebox() const { return m_isOpenSafebox ? true : false; }
		void 	SetOpenSafebox(bool b) { m_isOpenSafebox = b; }

		int		GetSafeboxLoadTime() const { return m_iSafeboxLoadTime; }
		void	SetSafeboxLoadTime() { m_iSafeboxLoadTime = thecore_pulse(); }
		//END_PREVENT_TRADE_WINDOW
	private:
		bool	m_isOpenSafebox;

	public:
		int		GetSkillPowerByLevel(int level, bool bMob = false) const;

		//PREVENT_REFINE_HACK
		int		GetRefineTime() const { return m_iRefineTime; }
		void	SetRefineTime() { m_iRefineTime = thecore_pulse(); }
		int		m_iRefineTime;
		//END_PREVENT_REFINE_HACK

		//RESTRICT_USE_SEED_OR_MOONBOTTLE
		int 	GetUseSeedOrMoonBottleTime() const { return m_iSeedTime; }
		void  	SetUseSeedOrMoonBottleTime() { m_iSeedTime = thecore_pulse(); }
		int 	m_iSeedTime;
		//END_RESTRICT_USE_SEED_OR_MOONBOTTLE

		//PREVENT_PORTAL_AFTER_EXCHANGE
		int		GetExchangeTime() const { return m_iExchangeTime; }
		void	SetExchangeTime() { m_iExchangeTime = thecore_pulse(); }
		int		m_iExchangeTime;
		//END_PREVENT_PORTAL_AFTER_EXCHANGE

		int 	m_iMyShopTime;
		int		GetMyShopTime() const	{ return m_iMyShopTime; }
		void	SetMyShopTime() { m_iMyShopTime = thecore_pulse(); }

		// Hack 방지를 위한 체크.
		bool	IsHack(bool bSendMsg = true, bool bCheckShopOwner = true, int limittime = g_nPortalLimitTime);

		void Say(const std::string & s);

	public:
		bool ItemProcess_Polymorph(LPITEM item);

		// by mhh
		LPITEM* GetCubeItem() { return m_pointsInstant.playerSlots ? m_pointsInstant.playerSlots->pCubeItems : nullptr; }
		bool IsCubeOpen() const { return (m_pointsInstant.playerSlots ? m_pointsInstant.playerSlots->pCubeNpc != nullptr : false); }
		void SetCubeNpc(LPCHARACTER npc) { m_pointsInstant.playerSlots->pCubeNpc = npc; }
		bool CanDoCube() const;

	private:
		//중국 전용
		//18세 미만 전용
		//3시간 : 50 % 5 시간 0%
		e_overtime m_eOverTime;

	public:
		bool IsOverTime(e_overtime e) const { return (e == m_eOverTime); }
		void SetOverTime(e_overtime e) { m_eOverTime = e; }

	private:
		int		m_deposit_pulse;

	public:
		void	UpdateDepositPulse();
		bool	CanDeposit() const;

	private:
		void	__OpenPrivateShop();

	public:
		struct AttackedLog
		{
			DWORD 	dwPID;
			DWORD	dwAttackedTime;

			AttackedLog() : dwPID(0), dwAttackedTime(0)
			{
			}
		};

		AttackLog	m_kAttackLog;
		AttackedLog m_AttackedLog;
		int			m_speed_hack_count;

	private :
		std::string m_strNewName;

	public :
		const std::string GetNewName() const { return this->m_strNewName; }
		void SetNewName(const std::string name) { this->m_strNewName = name; }

	public :
		void GoHome();

	private :
		std::set<DWORD>	m_known_guild;

	public :
		void SendGuildName(CGuild* pGuild);
		void SendGuildName(DWORD dwGuildID);

	private :
		DWORD m_dwLogOffInterval;

	public :
		DWORD GetLogOffInterval() const { return m_dwLogOffInterval; }

#if defined(ENABLE_GAYA_RENEWAL)
	private :
		DWORD m_dwGemNextRefresh;
	public :
		DWORD GetGemNextRefresh() const { return m_dwGemNextRefresh; }
#endif

	public:
		bool UnEquipSpecialRideUniqueItem();
		bool CanWarp() const;

	private:
		DWORD m_dwLastGoldDropTime;
		DWORD m_dwLastBoxUseTime;
		DWORD m_dwLastBuySellTime;
	public:
		DWORD GetLastBuySellTime() const { return m_dwLastBuySellTime; }
		void SetLastBuySellTime(DWORD dwLastBuySellTime) { m_dwLastBuySellTime = dwLastBuySellTime; }
	public:
		void AutoRecoveryItemProcess (const EAffectTypes);
#ifdef ENABLE_RECALL
		void AutoRecallProcess();
#endif

	public:
		void BuffOnAttr_AddBuffsFromItem(LPITEM pItem);
		void BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem);

	private:
		void BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue);
		void BuffOnAttr_ClearAll();

		typedef std::map <BYTE, CBuffOnAttributes*> TMapBuffOnAttrs;
		TMapBuffOnAttrs m_map_buff_on_attrs;
		// 무적 : 원활한 테스트를 위하여.
	public:
		void SetArmada() { cannot_dead = true; }
		void ResetArmada() { cannot_dead = false; }
	private:
		bool cannot_dead;
#ifdef __PET_SYSTEM__
	private:
		bool m_bIsPet;
	public:
		void SetPet() { m_bIsPet = true; }
		bool IsPet() { return m_bIsPet; }
#endif
#ifdef __ENABLE_PREMIUM_PLAYERS__
public:
	void PremiumPlayersOpenPacket();
	void PremiumPlayersListPacket();
	void PremiumPlayersActivatePacket();
	
	void StartPremiumPlayersUpdateEvent();
	void StopPremiumPlayersUpdateEvent();
	
	bool IsPremiumPlayer() { return (GetPremiumPlayer() == 1 ? true : false); }
	
	BYTE GetPremiumPlayer() const { return m_byPremium; }
	void SetPremiumPlayer(BYTE byValue);
	
	int GetPremiumPlayerTimer() const { return m_iPremiumTime; }
	void SetPremiumPlayerTimer(int iTime);
	
	void CheckPremiumPlayersAffects();
	
protected:
	BYTE m_byPremium;
	long int m_iPremiumTime;
#endif

#ifdef __NEWPET_SYSTEM__
	private:
		bool m_bIsNewPet;
		int m_eggvid;
	public:
		void SetNewPet() { m_bIsNewPet = true; }
		bool IsNewPet() const { return m_bIsNewPet ? true : false; }
		void SetEggVid(int vid) { m_eggvid = vid; }
		int GetEggVid() { return m_eggvid; }

#endif

	//최종 데미지 보정.
	private:
		float m_fAttMul;
		float m_fDamMul;
	public:
		float GetAttMul() { return this->m_fAttMul; }
		void SetAttMul(float newAttMul) {this->m_fAttMul = newAttMul; }
		float GetDamMul() { return this->m_fDamMul; }
		void SetDamMul(float newDamMul) {this->m_fDamMul = newDamMul; }

	private:
		bool IsValidItemPosition(TItemPos Pos) const;

	public:
		//용혼석

		// 캐릭터의 affect, quest가 load 되기 전에 DragonSoul_Initialize를 호출하면 안된다.
		// affect가 가장 마지막에 로드되어 LoadAffect에서 호출함.
		void	DragonSoul_Initialize();

		int		DragonSoul_GetActiveDeck() const;
		bool	DragonSoul_IsDeckActivated() const;
		bool	DragonSoul_ActivateDeck(int deck_idx);

		void	DragonSoul_DeactivateAll();
		// 반드시 ClearItem 전에 불러야 한다.
		// 왜냐하면....
		// 용혼석 하나 하나를 deactivate할 때마다 덱에 active인 용혼석이 있는지 확인하고,
		// active인 용혼석이 하나도 없다면, 캐릭터의 용혼석 affect와, 활성 상태를 제거한다.
		//
		// 하지만 ClearItem 시, 캐릭터가 착용하고 있는 모든 아이템을 unequip하는 바람에,
		// 용혼석 Affect가 제거되고, 결국 로그인 시, 용혼석이 활성화되지 않는다.
		// (Unequip할 때에는 로그아웃 상태인지, 아닌지 알 수 없다.)
		// 용혼석만 deactivate시키고 캐릭터의 용혼석 덱 활성 상태는 건드리지 않는다.
		void	DragonSoul_CleanUp();
		// 용혼석 강화창
	public:
		bool		DragonSoul_RefineWindow_Open(LPENTITY pEntity);
		bool		DragonSoul_RefineWindow_Close();
		LPENTITY DragonSoul_RefineWindow_GetOpener()
		{
			return m_pointsInstant.playerSlots ? 
				m_pointsInstant.playerSlots->m_pDragonSoulRefineWindowOpener : 
				nullptr;
		}

		bool		DragonSoul_RefineWindow_CanRefine();
#if defined(BL_OFFLINE_MESSAGE)
	protected:
		DWORD				dwLastOfflinePMTime;
	public:
		DWORD				GetLastOfflinePMTime() const { return dwLastOfflinePMTime; }
		void				SetLastOfflinePMTime() { dwLastOfflinePMTime = get_dword_time(); }
		void				SendOfflineMessage(const char* To, const char* Message);
		void				ReadOfflineMessages();
#endif
		//독일 선물 기능 패킷 임시 저장
	private:
		unsigned int itemAward_vnum;
		char		 itemAward_cmd[20];
		//bool		 itemAward_flag;
	public:
		unsigned int GetItemAward_vnum() { return itemAward_vnum; }
		char*		 GetItemAward_cmd() { return itemAward_cmd;	  }
		//bool		 GetItemAward_flag() { return itemAward_flag; }
		void		 SetItemAward_vnum(unsigned int vnum) { itemAward_vnum = vnum; }
		void		 SetItemAward_cmd(char* cmd) { strcpy(itemAward_cmd,cmd); }
		//void		 SetItemAward_flag(bool flag) { itemAward_flag = flag; }
#ifdef ENABLE_ANTI_CMD_FLOOD
	private:
		int m_dwCmdAntiFloodPulse;
		DWORD m_dwCmdAntiFloodCount;
	public:
		int GetCmdAntiFloodPulse(){return m_dwCmdAntiFloodPulse;}
		DWORD GetCmdAntiFloodCount(){return m_dwCmdAntiFloodCount;}
		DWORD IncreaseCmdAntiFloodCount(){return ++m_dwCmdAntiFloodCount;}
		void SetCmdAntiFloodPulse(int dwPulse){m_dwCmdAntiFloodPulse=dwPulse;}
		void SetCmdAntiFloodCount(DWORD dwCount){m_dwCmdAntiFloodCount=dwCount;}
#endif
	private:
		// SyncPosition을 악용하여 타유저를 이상한 곳으로 보내는 핵 방어하기 위하여,
		// SyncPosition이 일어날 때를 기록.
		timeval		m_tvLastSyncTime;
		int			m_iSyncHackCount;
	public:
		void			SetLastSyncTime(const timeval &tv) { memcpy(&m_tvLastSyncTime, &tv, sizeof(timeval)); }
		const timeval&	GetLastSyncTime() { return m_tvLastSyncTime; }
		void			SetSyncHackCount(int iCount) { m_iSyncHackCount = iCount;}
		int				GetSyncHackCount() { return m_iSyncHackCount; }

#ifdef ENABLE_ACCE_SYSTEM
	protected:
		bool	m_bAcceCombination, m_bAcceAbsorption;

	public:
		bool	isAcceOpened(bool bCombination) {return bCombination ? m_bAcceCombination : m_bAcceAbsorption;}
		void	OpenAcce(bool bCombination);
		void	CloseAcce();
		void	ClearAcceMaterials();
		bool	CleanAcceAttr(LPITEM pkItem, LPITEM pkTarget);
		LPITEM* GetAcceMaterials() { return m_pointsInstant.playerSlots ? m_pointsInstant.playerSlots->pAcceMaterials : nullptr; }
		bool	AcceIsSameGrade(long lGrade);
		DWORD	GetAcceCombinePrice(long lGrade
#ifdef ENABLE_STOLE_COSTUME
		, bool isCostume
#endif
		);
		void	GetAcceCombineResult(DWORD & dwItemVnum, DWORD & dwMinAbs, DWORD & dwMaxAbs);
		BYTE	CheckEmptyMaterialSlot();
		void	AddAcceMaterial(TItemPos tPos, BYTE bPos);
		void	RemoveAcceMaterial(BYTE bPos);
		BYTE	CanRefineAcceMaterials();
		void	RefineAcceMaterials();
		bool	IsAcceOpen() const {return m_bAcceCombination || m_bAcceAbsorption;}
#endif

#ifdef ENABLE_SOUL_SYSTEM
	public:
		bool 		DoRefineItemSoul(LPITEM item);
		int 		GetSoulItemDamage(LPCHARACTER pkVictim, int iDamage, BYTE bSoulType);
#endif

#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
	std::string GetLang();
#endif
#if defined(ENABLE_TEXTS_RENEWAL)
	public:
		void ChatPacketNew(BYTE type, DWORD idx, const char * format, ...);
#endif
#ifdef ENABLE_NEW_FISHING_SYSTEM
	public:
		LPEVENT m_pkFishingNewEvent;

		void fishing_new_start();
		void fishing_new_stop();
		void fishing_new_catch();
		void fishing_new_catch_failed();
		void fishing_catch_decision(DWORD itemVnum);
		void SetFishCatch(int i) { m_bFishCatch = i; }
		BYTE GetFishCatch() { return m_bFishCatch; }
		void SetLastCatchTime(DWORD i) { m_dwLastCatch = i; }
		int GetLastCatchTime() { return m_dwLastCatch; }
		void SetFishCatchFailed(int i) { m_dwCatchFailed = i; }
		BYTE GetFishCatchFailed() { return m_dwCatchFailed; }

	private:
		BYTE m_bFishCatch;
		DWORD m_dwCatchFailed;
		int m_dwLastCatch;
#endif
	public:
		int		GetGoToXYTime() const { return m_iGoToXYTime; }
		void	SetGoToXYTime() { m_iGoToXYTime = thecore_pulse(); }

	protected:
		int		m_iGoToXYTime;

#ifdef ENABLE_SORT_INVEN
	public:
		int		GetSortInv1Time() const { return m_iSortInv1Time; }
		void	SetSortInv1Time() { m_iSortInv1Time = thecore_pulse(); }
		int		GetSortInv2Time() const { return m_iSortInv2Time; }
		void	SetSortInv2Time() { m_iSortInv2Time = thecore_pulse(); }

	protected:
		int		m_iSortInv1Time;
		int		m_iSortInv2Time;
#endif

#ifdef ENABLE_REVIVE_WITH_HALF_HP_IF_MONSTER_KILLED_YOU
	public:
		bool	GetDeadByMonster() const { return m_deadByMonster; }
		void	SetDeadByMonster(bool flag) { m_deadByMonster = flag; }

	protected:
		bool	m_deadByMonster;
#endif

#ifdef ENABLE_SPAM_CHECK
	public:
		int32_t	GetLastUnlock() const { return m_iLastUnlock; }
		void	SetLastUnlock() { m_iLastUnlock = get_global_time() + 3; }
		int32_t	GetLastDSREfine() const { return m_iLastDSRefine; }
		void	SetLastDSREfine() { m_iLastDSRefine = get_global_time() + 3; }

	protected:
		int32_t	m_iLastUnlock, m_iLastDSRefine;
#endif

	public:
#ifdef ENABLE_BIOLOGIST_UI
		void CheckBiologistReward();
#endif
#ifdef ENABLE_BLOCK_MULTIFARM
		void BlockProcessed();
		void BlockDrop();
		void UnblockDrop();
		void SetDropStatus();
#endif
#ifdef __DEFENSE_WAVE__
		bool IsDefanceWaweMastAttackMob(int32_t vnum) const { return (vnum >= 3401 && vnum <= 3405) || (vnum >= 3601 && vnum <= 3605) || (vnum >= 3950 && vnum <= 3964); }
#endif

	protected:
#ifdef ENABLE_BLOCK_MULTIFARM
		LPEVENT m_pkDropEvent;
#endif

#ifdef ENABLE_USEITEM_COOLDOWN
	private:
		int m_dwItemUseAntiFloodPulse;
		DWORD m_dwItemUseAntiFloodCount;

	public:
		int GetItemUseAntiFloodPulse(){return m_dwItemUseAntiFloodPulse;}
		DWORD GetItemUseAntiFloodCount(){return m_dwItemUseAntiFloodCount;}
		DWORD IncreaseItemUseAntiFloodCount(){return ++m_dwItemUseAntiFloodCount;}
		void SetItemUseAntiFloodPulse(int dwPulse){m_dwItemUseAntiFloodPulse=dwPulse;}
		void SetItemUseAntiFloodCount(DWORD dwCount){m_dwItemUseAntiFloodCount=dwCount;}
#endif

#if defined(ENABLE_ANTI_FLOOD)
	protected:
		int32_t m_antifloodpulse[MAX_FLOOD_TYPE];
		uint32_t m_antifloodcount[MAX_FLOOD_TYPE];

	public:
		int32_t GetAntiFloodPulse(uint8_t type);
		uint32_t GetAntiFloodCount(uint8_t type);
		uint32_t IncreaseAntiFloodCount(uint8_t type);
		void SetAntiFloodPulse(uint8_t type, int32_t pulse);
		void SetAntiFloodCount(uint8_t type, uint32_t count);
#endif

#if defined(USE_BATTLEPASS)
	protected:
		DWORD	m_dwLastReciveExtBattlePassInfoTime;
		DWORD	m_dwLastExtBattlePassOpenRankingTime;

    private:
        bool                                                    m_bIsLoadedExtBattlePass;
        std::list<std::unique_ptr<TPlayerExtBattlePassMission>> m_listExtBattlePass;

	public:
		void LoadExtBattlePass(DWORD, TPlayerExtBattlePassMission*);
		DWORD GetExtBattlePassMissionProgress(BYTE, BYTE);
		bool IsExtBattlePassCompletedMission(BYTE, BYTE);
		bool IsExtBattlePassRegistered(DWORD);
		void UpdateExtBattlePassMissionProgress(DWORD, DWORD, DWORD, bool isOverride = false);
		void SetExtBattlePassMissionProgress(DWORD, DWORD, DWORD);

		bool IsLoadedExtBattlePass() const { return m_bIsLoadedExtBattlePass; }
		DWORD GetExtBattlePassPremiumID() const { return m_points.battle_pass_premium_id; }
		void SetExtBattlePassPremiumID(DWORD id)	{ m_points.battle_pass_premium_id = id;}

		void SetLastReciveExtBattlePassInfoTime(DWORD time);
		DWORD GetLastReciveExtBattlePassInfoTime() const { return m_dwLastReciveExtBattlePassInfoTime; }
		void SetLastReciveExtBattlePassOpenRanking(DWORD time);
		DWORD GetLastReciveExtBattlePassOpenRanking() const { return m_dwLastExtBattlePassOpenRankingTime; }
#endif

#if defined(ENABLE_DS_REFINE_ALL)
	private:
		bool m_isdsrefiningall;

	public:
		bool IsDSRefiningAll() { return m_isdsrefiningall; }
		void SetDSRefiningAll(bool value) { m_isdsrefiningall = value; }
#endif

#ifdef ENABLE_SWITCHBOT_WORLDARD
	public:
		bool			IsSBOpen() const { return m_isOpenSB ? true : false; }
		void 			SetSBOpen(bool b) { m_isOpenSB = b; }
		LPITEM 			GetSwitchBotItem(WORD wCell) const;

	private:
		bool 			m_isOpenSB;
#endif

	public:
		DWORD GetShootSpeedPulse() const { return m_dwShootSpeedPulse; }
		DWORD GetShootSpeedCount() const { return m_dwShootSpeedCount; }
		void SetShootSpeedPulse(DWORD dwShootSpeedPulse) { m_dwShootSpeedPulse = dwShootSpeedPulse; }
		void SetShootSpeedCount(DWORD dwShootSpeedCount) { m_dwShootSpeedCount = dwShootSpeedCount; }
		DWORD IncreaseShootSpeedCount() { return ++m_dwShootSpeedCount; }
		bool IsShootSpeedHack(BYTE bSec = 2, BYTE bCount = 5);
	private:
		DWORD m_dwShootSpeedPulse;
		DWORD m_dwShootSpeedCount;

	public:
		uint32_t GetLastMoveItemTime() const { return m_dwLastMoveItemTime; }
		bool IsLastMoveItemTime() const;
		void SetLastMoveItemTime(uint32_t dwLastMoveItemTime) { m_dwLastMoveItemTime = dwLastMoveItemTime; }
	private:
		uint32_t m_dwLastMoveItemTime;

#if defined(ENABLE_EXCHANGE_FRAGMENTS)
	public:
		void ExchangeForFragments(TItemPos);
#endif

	protected:
#ifdef USE_CAPTCHA_SYSTEM
        LPEVENT                 m_CaptchaTimer;
#endif
#if defined(ENABLE_NEW_FISH_EVENT)
		TPlayerFishEventSlot* m_fishSlots;
#endif
#if defined(ENABLE_CHANGELOOK)
		bool m_bChangeLook;
#endif
#ifdef __FIX_PRO_DAMAGE__
		DWORD sync_hack;
		int sync_count;
		int sync_time;
		DWORD bow_time;
#endif
		int32_t m_iLastAttackPulse{ 0 };
#if defined(ENABLE_OKEY_CARD_GAME)
		TOkeyCardInfo character_cards;
		TOkeyCard randomized_cards[MAX_OKEY_CARDS];
#endif

	private:
#ifdef USE_CAPTCHA_SYSTEM
        bool        m_isWaitingForCaptcha;
        uint16_t    m_Captcha;
#endif
#if defined(ENABLE_NEW_FISH_EVENT)
		uint32_t m_dwFishUseCount;
		uint8_t m_bFishAttachedShape;
#endif
#if defined(ENABLE_HIDE_COSTUME_SYSTEM)
		uint8_t m_PartStatus[HIDE_PART_MAX];
#endif
#if defined(ENABLE_NEW_ANTICHEAT_RULES)
		bool m_bIsDaggerEquipped;
#endif
#if defined(USE_ATTR_6TH_7TH)
		bool m_bIsOpenAttr67Add;
#endif

	public:
		bool IsBowMode() const;
#ifdef USE_CAPTCHA_SYSTEM
        void        LoadCaptchaTimer();
        bool        IsWaitingForCaptcha() { return m_isWaitingForCaptcha; };
        void        SetWaitingForCaptcha(bool bValue) { m_isWaitingForCaptcha = bValue; };
        uint16_t    GetCaptcha() { return m_Captcha; };
        void        ValidateCaptcha(uint16_t);
        void        CleanCaptchaTimer();
#endif
#if defined(ENABLE_NEW_FISH_EVENT)
		void FishEventGeneralInfo();
		void FishEventUseBox(TItemPos);
		bool FishEventIsValidPosition(uint8_t, uint8_t);
		void FishEventPlaceShape(uint8_t, uint8_t);
		void FishEventAddShape(uint8_t);
		void FishEventCheckEnd();
		uint32_t GetFishEventUseCount() const { return m_dwFishUseCount; }
		void FishEventIncreaseUseCount() { m_dwFishUseCount++; }
		uint8_t GetFishAttachedShape() const { return m_bFishAttachedShape; }
		void SetFishAttachedShape(uint8_t bShape) { m_bFishAttachedShape = bShape; }
#endif
#if defined(ENABLE_LETTERS_EVENT)
		void SendLettersEventInfo();
		void CollectReward();
#endif
#if defined(ENABLE_HIDE_COSTUME_SYSTEM)
		void SetPartStatus(uint8_t);
		uint8_t GetPartStatus(uint8_t) const;
#endif
#if defined(ENABLE_PETSKIN)
		void UpdatePetSkin();
#endif
#if defined(ENABLE_CHANGELOOK)
		bool isChangeLookOpened() const { return m_bChangeLook; }
		void ChangeLookWindow(bool bOpen = false, bool bRequest = false);
		void ClearChangeLookWindowMaterials();
		LPITEM* GetChangeLookWindowMaterials() { return m_pointsInstant.playerSlots ? m_pointsInstant.playerSlots->pChangeLookMaterials : nullptr; }
		uint8_t CheckChangeLookEmptyMaterialSlot();
		void AddChangeLookMaterial(TItemPos, uint8_t);
		void RemoveChangeLookMaterial(uint8_t);
		void RefineChangeLookMaterials();
		bool CleanTransmutation(LPITEM, LPITEM);
		void ClearChangeLookWindow();
#endif
#if defined(ENABLE_NEW_ANTICHEAT_RULES)
		bool IsDaggerEquipped() const { return m_bIsDaggerEquipped; };
		void SetDaggerEquipped(bool bValue) { m_bIsDaggerEquipped = bValue; };
#endif
#ifdef __FIX_PRO_DAMAGE__
		void SetSyncPosition(long x, long y);
		bool CheckSyncPosition(bool sync_check = false);
		void SetSyncCount(int count) { sync_count += count; }
		DWORD GetBowTime() {return bow_time;}
		void SetBowTime(DWORD t) {bow_time = t;}
#endif
#if defined(ENABLE_HALLOWEEN_EVENT_2022) && defined(ENABLE_EVENT_MANAGER)
		void SendHalloweenEventInfo();
		long GetSkull() const { return m_points.skull; }
		void SetSkull(long value) { m_points.skull = value; }
#endif
#if defined(ENABLE_TRACK_WINDOW)
		void GetDungeonCooldown(WORD);
		void GetDungeonCooldownTest(WORD, int32_t, bool);
#endif
#if defined(ENABLE_WORLDBOSS)
		long GetWbPoints() const { return m_points.wbpoints; }
		void SetWbPoints(long value) { m_points.wbpoints = value; }
#endif
		int GetLastAttackPulse() const { return m_iLastAttackPulse; }
		void SetLastAttackPulse(int pulse) { m_iLastAttackPulse = pulse; }
#if defined(ENABLE_OKEY_CARD_GAME)
#if defined(ENABLE_EVENT_MANAGER)
		void SendOkeyGameEventInfo();
#endif
		void Cards_open(uint32_t);
		void Cards_clean_list();
		int32_t GetEmptySpaceInHand();
		void Cards_pullout();
		void RandomizeCards();
		bool CardWasRandomized(uint8_t, uint8_t);
		void SendUpdatedInformations();
		void SendReward();
		void CardsDestroy(uint32_t);
		void CardsAccept(uint32_t);
		void CardsRestore(uint32_t);
		int32_t GetEmptySpaceInField();
		uint32_t GetAllCardsCount();
		bool TypesAreSame();
		bool ValuesAreSame();
		bool CardsMatch();
		uint32_t GetLowestCard();
		bool CheckReward();
		void CheckCards();
		void RestoreField();
		void ResetField();
		void CardsEnd();
		void GetGlobalRank(const char*, const char*, char*, size_t);
		void GetRundRank(const char*, const char*, char*, size_t);
#endif
		bool IsWearingDress() const;
		void UnMount(bool bUnequipItem = false);
#if defined(USE_ATTR_6TH_7TH)
		bool IsOpenAttr67Add() const { return m_bIsOpenAttr67Add ? true : false; }
		void SetOpenAttr67Add(bool bOpen) { m_bIsOpenAttr67Add = bOpen; }
		LPITEM GetAttr67AddItem(BYTE byCell = 0) const;
		bool Attr67Add(const TAttr67AddData kAttr67AddData);
#endif
#if defined(USE_SPECIAL_EXP_FLAG)
		bool IsSpecialExp() const { return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_SPECIALEXP); };
#endif

#if defined(ENABLE_AURA_SYSTEM)
	private:
		uint8_t m_bAuraRefineWindowType;
		bool m_bAuraRefineWindowOpen;
		TItemPos m_pAuraRefineWindowItemSlot[AURA_SLOT_MAX];
		TAuraRefineInfo m_bAuraRefineInfo[AURA_REFINE_INFO_SLOT_MAX];

	protected:
		uint8_t __GetAuraAbsorptionRate(uint8_t, uint8_t) const;
		TAuraRefineInfo __GetAuraRefineInfo(TItemPos);
		TAuraRefineInfo __CalcAuraRefineInfo(TItemPos, TItemPos);
		TAuraRefineInfo __GetAuraEvolvedRefineInfo(TItemPos);

	public:
		void OpenAuraRefineWindow(LPENTITY, EAuraWindowType);
		bool IsAuraRefineWindowOpen() const { return  m_bAuraRefineWindowOpen; }
		uint8_t GetAuraRefineWindowType() const { return  m_bAuraRefineWindowType; }
		LPENTITY GetAuraRefineWindowOpener()
		{
			return m_pointsInstant.playerSlots ? 
				m_pointsInstant.playerSlots->m_pAuraRefineWindowOpener : nullptr;
		}
		bool IsAuraRefineWindowCanRefine();
		void AuraRefineWindowCheckIn(uint8_t, TItemPos, TItemPos);
		void AuraRefineWindowCheckOut(uint8_t, TItemPos);
		void AuraRefineWindowAccept(uint8_t);
		void AuraRefineWindowClose();
#endif

	private:
		std::map<std::string, int32_t>  m_protection_Time;

	public:
		void SetProtectTime(const std::string& flagname, int32_t value);
		int32_t GetProtectTime(const std::string& flagname) const;

#if defined(ENABLE_INGAME_ITEMSHOP) || defined(USE_WHEEL_OF_FORTUNE)
	public:
		int32_t GetDragonCoin(bool bForceRefresh = false);
		void SetDragonCoin(int32_t amount);
		void SetDragonCoinMinus(int32_t amount);

	private:
		int32_t m_iCoins;
		bool m_bCoinsLoaded;
#endif

#if defined(USE_DEADTIME_CHECK)
    public:
        DWORD   GetDeadTime() const { return m_dwLastDeadTime + (1000 * USE_DEADTIME_CHECK); };
#endif

#ifdef USE_AUTO_HUNT
    private:
        bool m_abAutoUseType[AUTO_ONOFF_MAX];

    public:
        void SetAutoUseType(const uint8_t t, const bool v);
        bool GetAutoUseType(const uint8_t t);
#endif

#ifdef USE_PICKUP_FILTER
    protected:
        std::unordered_map<uint8_t, TFilterTable>   m_mapFilterTable;
        bool                                        m_bFilterStatus[FILTER_TYPE_MAX];

    public:
        size_t      GetFilterTableSize() const;
        void        AddFilterTable(const uint8_t bIdx, TFilterTable tFilter);
#if defined(USE_AUTO_HUNT) || defined(__ENABLE_PREMIUM_PLAYERS__)
        void        ClearFilterTable(bool bExpire = false);
#else
        void        ClearFilterTable();
#endif
        bool        CanPickItem(LPITEM pItem);
        void        SetFilterStatus(uint8_t bType, bool bFlag);
        bool        GetFilterStatus(uint8_t bType) const;
#endif

#ifdef USE_AUTO_AGGREGATE
    protected:
        LPEVENT     m_pAutoAggregateEvent;
        bool        m_bAutoAggregate;

    public:
        void        SetPremiumBraveryCape(const bool bFlag);
        bool        GetPremiumBraveryCape() const;
        void        StopPremiumBraveryCape(bool bJustClear = false);
        void        GetBraveCapeCMDCompare(const char* c_pszArgument);
#endif

#ifdef USE_MULTIPLE_OPENING
    public:
        bool MultipleAutoGiveItem(LPITEM pItem, uint32_t& count, std::map<LPITEM, uint32_t>& mItems);
        bool MultipleGiveItemFromSpecialItemGroup(uint32_t dwGroupNum, uint32_t& count);
        void MultipleOpening(TItemPos pos, uint32_t count);
#endif

public:
	void SetCountryFlag(const char* country_flag) { m_country_flag = country_flag; }
	auto& GetCountryFlag() const { return m_country_flag; }
private:
	std::string m_country_flag;
};

ESex GET_SEX(LPCHARACTER ch);

#ifdef USE_CAPTCHA_SYSTEM
EVENTINFO(captcha_event_info)
{
    DynamicCharacterPtr ch;
    bool                disconnect;
};
#endif

#ifdef ENABLE_BLOCK_MULTIFARM
EVENTINFO(drop_event_info) {
	DynamicCharacterPtr ch;
	time_t time;
	bool drop;
};
#endif

#ifdef ENABLE_NEW_FISHING_SYSTEM
EVENTINFO(fishingnew_event_info)
{
	DWORD pid, vnum, chance, sec;
	fishingnew_event_info() : pid(0), vnum(0), chance(0), sec(0) {}
};
#endif
#endif
