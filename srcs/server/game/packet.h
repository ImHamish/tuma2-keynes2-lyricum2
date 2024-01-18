#ifndef __INC_PACKET_H__
#define __INC_PACKET_H__
#include "stdafx.h"
#include "../common/tables.h"

enum
{
	HEADER_CG_HANDSHAKE = 0xf0,
	HEADER_CG_PONG = 0xef,
	HEADER_CG_TIME_SYNC = 0xed,
	HEADER_CG_KEY_AGREEMENT = 0xec,

	HEADER_CG_LOGIN = 0x20,
	HEADER_CG_ATTACK = 49,
	HEADER_CG_CHAT					= 3,
	HEADER_CG_CHARACTER_CREATE = 0x19,
	HEADER_CG_CHARACTER_DELETE		= 5,
	HEADER_CG_CHARACTER_SELECT		= 6,
	HEADER_CG_MOVE					= 7,
	HEADER_CG_SYNC_POSITION			= 8,
	HEADER_CG_ENTERGAME				= 10,

	HEADER_CG_ITEM_USE				= 11,
	HEADER_CG_ITEM_DROP				= 12,
	HEADER_CG_ITEM_MOVE				= 13,
	HEADER_CG_ITEM_PICKUP			= 15,

	HEADER_CG_QUICKSLOT_ADD = 16,
	HEADER_CG_QUICKSLOT_DEL = 17,
	HEADER_CG_QUICKSLOT_SWAP = 18,

	HEADER_CG_WHISPER				= 19,
	HEADER_CG_ITEM_DROP2			= 20,
	HEADER_CG_ITEM_DESTROY = 21,
	HEADER_CG_ITEM_DIVISION = 22,
	HEADER_CG_REQ_MAP_BOSS_INFO = 23,

	HEADER_CG_ON_CLICK				= 26,
	HEADER_CG_EXCHANGE				= 27,
	HEADER_CG_CHARACTER_POSITION		= 28,
	HEADER_CG_SCRIPT_ANSWER			= 29,
	HEADER_CG_QUEST_INPUT_STRING		= 30,
	HEADER_CG_QUEST_CONFIRM			= 31,
	HEADER_CG_LOGIN3 = 33,
	HEADER_CG_SHOP				= 50,
	HEADER_CG_FLY_TARGETING			= 51,
	HEADER_CG_USE_SKILL = 42,
#if defined(USE_BATTLEPASS)
	HEADER_CG_EXT_BATTLE_PASS_ACTION = 43,
	HEADER_CG_EXT_SEND_BP_PREMIUM_ITEM = 44,
#endif
#ifdef USE_CAPTCHA_SYSTEM
    HEADER_CG_CAPTCHA                           = 47,
#endif
#if defined(USE_ATTR_6TH_7TH)
	HEADER_CG_ATTR67_ADD = 48,
#endif
	HEADER_CG_ADD_FLY_TARGETING			= 53,
	HEADER_CG_SHOOT				= 54,
	HEADER_CG_MYSHOP				= 55,

#ifdef __SKILL_COLOR_SYSTEM__
	HEADER_CG_SKILL_COLOR			= 56,
#endif
#ifdef ENABLE_OPENSHOP_PACKET
	HEADER_CG_OPENSHOP = 57,
#endif
#if defined(ENABLE_GAYA_RENEWAL)
	HEADER_CG_GEM_SHOP = 58,
#endif
	HEADER_CG_ITEM_USE_TO_ITEM			= 60,
	HEADER_CG_TARGET			 	= 61,
	HEADER_CG_TEXT				= 64,
	//HEADER_CG_TEXT				= 64,	// @ 로 시작되면 텍스트를 파싱한다.
	HEADER_CG_WARP				= 65,
	HEADER_CG_SCRIPT_BUTTON			= 66,
	HEADER_CG_MESSENGER				= 67,

	HEADER_CG_MALL_CHECKOUT			= 69,
	HEADER_CG_SAFEBOX_CHECKIN			= 70,	// 아이템을 창고에 넣넎는다.
	HEADER_CG_SAFEBOX_CHECKOUT			= 71,	// 아이템을 창고로 부터 빼온다.

	HEADER_CG_PARTY_INVITE			= 72,
	HEADER_CG_PARTY_INVITE_ANSWER		= 73,
	HEADER_CG_PARTY_REMOVE			= 74,
	HEADER_CG_PARTY_SET_STATE                   = 75,
	HEADER_CG_PARTY_USE_SKILL			= 76,
	HEADER_CG_SAFEBOX_ITEM_MOVE			= 77,
	HEADER_CG_PARTY_PARAMETER			= 78,
#if defined(ENABLE_AURA_SYSTEM)
	HEADER_CG_AURA = 79,
#endif
	HEADER_CG_GUILD				= 80,
	HEADER_CG_ANSWER_MAKE_GUILD			= 81,

	HEADER_CG_FISHING				= 82,

	HEADER_CG_ITEM_GIVE				= 83,
	HEADER_CG_EMPIRE				= 90,

	HEADER_CG_REFINE				= 96,

	HEADER_CG_MARK_LOGIN			= 100,
	HEADER_CG_MARK_CRCLIST			= 101,
	HEADER_CG_MARK_UPLOAD			= 102,
	HEADER_CG_MARK_IDXLIST			= 104,

	HEADER_CG_HACK					= 105,
	HEADER_CG_CHANGE_NAME			= 106,
	HEADER_CG_CHANGE_COUNTRY_FLAG	= 107,
	HEADER_CG_LOGIN2 = 109,
	HEADER_CG_DUNGEON				= 110,
	HEADER_CG_GUILD_SYMBOL_UPLOAD	= 112,
	HEADER_CG_SYMBOL_CRC			= 113,

	// SCRIPT_SELECT_ITEM
	HEADER_CG_SCRIPT_SELECT_ITEM	= 114,
	// END_OF_SCRIPT_SELECT_ITEM

#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
	HEADER_CG_WHISPER_ADMIN	= 220,
#endif


#ifdef __ENABLE_NEW_OFFLINESHOP__
	HEADER_CG_NEW_OFFLINESHOP		= 119,
#endif
#if defined(ENABLE_EXCHANGE_FRAGMENTS)
	HEADER_CG_EXCHANGE_FRAGMENTS = 118,
#endif
#ifdef ENABLE_SWITCHBOT_WORLDARD
		HEADER_CG_SWITCHBOT = 171,
#endif
//	HEADER_CG_ROULETTE				= 200,
	//NOTE : 이런 개XXX 정말 이거 Packet설계한 사람은 누구냐. 이렇게 코딩하고 밥이 넘어가나.
	//enum을 별도로 구별을 하던가. 아님 namepsace로 구별을 하던가..
	//정말 packet generator까지는 바라지도 않는다. 이런 씨XX
	//이러다가 숫자 겹치면 누가 책임지는데???
	HEADER_CG_DRAGON_SOUL_REFINE			= 205,
	HEADER_CG_STATE_CHECKER					= 206,
#ifdef ENABLE_DS_REFINE_ALL
	HEADER_CG_DRAGON_SOUL_REFINE_ALL = 207,
#endif
#if defined(ENABLE_LETTERS_EVENT)
	HEADER_CG_TAKE_LETTERS_REWARD = 212,
#endif
#if defined(ENABLE_CHANGELOOK)
	HEADER_CG_CHANGE_LOOK = 213,
#endif
	HEADER_CG_WHISPER_INFO = 214,
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	HEADER_CG_CUBE_RENEWAL 						= 215,
#endif
#ifdef ENABLE_NEW_FISHING_SYSTEM
	HEADER_CG_FISHING_NEW = 216, 
#endif
#if defined(ENABLE_NEW_FISH_EVENT)
	HEADER_CG_FISH_EVENT_SEND = 219,
#endif
	HEADER_CG_CLIENT_VERSION = 0xfd,
	HEADER_CG_CLIENT_VERSION2 = 0xf1,

	/********************************************************/
	HEADER_GC_KEY_AGREEMENT_COMPLETED = 0xeb,

	HEADER_GC_KEY_AGREEMENT = 0xec,
	HEADER_GC_TIME_SYNC = 0xed,
	HEADER_GC_PHASE = 0xee,
	HEADER_GC_BINDUDP = 0xef,
	HEADER_GC_HANDSHAKE = 0xf0,

	HEADER_GC_CHARACTER_ADD			= 1,
	HEADER_GC_CHARACTER_DEL			= 2,
	HEADER_GC_MOVE					= 3,
	HEADER_GC_CHAT					= 4,
	HEADER_GC_SYNC_POSITION			= 5,

	HEADER_GC_LOGIN_SUCCESS			= 6,
	HEADER_GC_LOGIN_SUCCESS_NEWSLOT	= 32,
	HEADER_GC_LOGIN_FAILURE			= 7,

	HEADER_GC_CHARACTER_CREATE_SUCCESS		= 8,
	HEADER_GC_CHARACTER_CREATE_FAILURE		= 9,
	HEADER_GC_CHARACTER_DELETE_SUCCESS		= 10,
	HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID	= 11,

	HEADER_GC_ATTACK				= 12,
	HEADER_GC_STUN				= 13,
	HEADER_GC_DEAD				= 14,

	HEADER_GC_CHARACTER_POINTS			= 16,
	HEADER_GC_CHARACTER_POINT_CHANGE		= 17,
	HEADER_GC_CHANGE_SPEED			= 18,
	HEADER_GC_CHARACTER_UPDATE			= 19,
	HEADER_GC_CHARACTER_UPDATE_NEW		= 24,

	HEADER_GC_ITEM_DEL				= 20,
	HEADER_GC_ITEM_SET				= 21,
	HEADER_GC_ITEM_USE				= 22,
	HEADER_GC_ITEM_DROP				= 23,
	HEADER_GC_ITEM_UPDATE			= 25,

	HEADER_GC_ITEM_GROUND_ADD			= 26,
	HEADER_GC_ITEM_GROUND_DEL			= 27,

	HEADER_GC_QUICKSLOT_ADD = 28,
	HEADER_GC_QUICKSLOT_DEL = 29,
	HEADER_GC_QUICKSLOT_SWAP = 30,

	HEADER_GC_ITEM_OWNERSHIP			= 31,
#ifdef USE_CAPTCHA_SYSTEM
    HEADER_GC_CAPTCHA                           = 33,
#endif
	HEADER_GC_WHISPER				= 34,
#if defined(ENABLE_CLOSE_GAMECLIENT_CMD)
	HEADER_GC_CLOSECLIENT = 35,
#endif
	HEADER_GC_MOTION				= 36,
	HEADER_GC_PARTS				= 37,

	HEADER_GC_SHOP				= 38,
	HEADER_GC_SHOP_SIGN				= 39,

	HEADER_GC_DUEL_START				= 40,
	HEADER_GC_PVP                               = 41,
	HEADER_GC_EXCHANGE				= 42,
	HEADER_GC_CHARACTER_POSITION		= 43,

	HEADER_GC_PING				= 44,
	HEADER_GC_SCRIPT				= 45,
	HEADER_GC_QUEST_CONFIRM			= 46,
#ifdef ENABLE_EVENT_MANAGER
	HEADER_GC_EVENT_MANAGER = 47,
#endif
#if defined(ENABLE_NEW_ANTICHEAT_RULES)
	HEADER_GC_ANTICHEAT = 48,
#endif
#ifdef __NEWPET_SYSTEM__
	HEADER_CG_PetSetName = 146,
#endif
	HEADER_GC_MOUNT				= 61,
	HEADER_GC_OWNERSHIP				= 62,
	HEADER_GC_TARGET			 	= 63,
	HEADER_GC_WARP				= 65,

	HEADER_GC_ADD_FLY_TARGETING			= 69,
	HEADER_GC_CREATE_FLY			= 70,
	HEADER_GC_FLY_TARGETING			= 71,
	HEADER_GC_SKILL_LEVEL_OLD			= 72,
	HEADER_GC_SKILL_LEVEL			= 76,

	HEADER_GC_MESSENGER				= 74,
	HEADER_GC_GUILD				= 75,

	HEADER_GC_PARTY_INVITE			= 77,
	HEADER_GC_PARTY_ADD				= 78,
	HEADER_GC_PARTY_UPDATE			= 79,
	HEADER_GC_PARTY_REMOVE			= 80,
	HEADER_GC_QUEST_INFO			= 81,
	HEADER_GC_REQUEST_MAKE_GUILD		= 82,
	HEADER_GC_PARTY_PARAMETER			= 83,

	HEADER_GC_SAFEBOX_SET			= 85,
	HEADER_GC_SAFEBOX_DEL			= 86,
	HEADER_GC_SAFEBOX_WRONG_PASSWORD		= 87,
	HEADER_GC_SAFEBOX_SIZE			= 88,

	HEADER_GC_FISHING				= 89,

	HEADER_GC_EMPIRE				= 90,

	HEADER_GC_PARTY_LINK			= 91,
	HEADER_GC_PARTY_UNLINK			= 92,
#if defined(ENABLE_INGAME_ITEMSHOP)
	HEADER_GC_ITEMSHOP = 93,
#endif
	HEADER_GC_VIEW_EQUIP = 99,

	HEADER_GC_MARK_BLOCK			= 100,
	HEADER_GC_MARK_IDXLIST			= 102,

	HEADER_GC_TIME					= 106,
	HEADER_GC_CHANGE_NAME			= 107,
	HEADER_GC_CHANGE_COUNTRY_FLAG	= 109,
	HEADER_GC_DUNGEON				= 110,

	HEADER_GC_WALK_MODE				= 111,
	HEADER_GC_SKILL_GROUP			= 112,
	HEADER_GC_MAIN_CHARACTER			= 113,

	//	HEADER_GC_USE_POTION			= 114,
	HEADER_GC_SEPCIAL_EFFECT		= 114,

	HEADER_GC_NPC_POSITION			= 115,
	HEADER_GC_LOGIN_KEY				= 118,
	HEADER_GC_REFINE_INFORMATION = 119,
	HEADER_GC_CHANNEL				= 121,

	// 122 HEADER_GC_MALL_OPEN
	HEADER_GC_TARGET_UPDATE			= 123,
	HEADER_GC_TARGET_DELETE			= 124,
	HEADER_GC_TARGET_CREATE			= 125,

	HEADER_GC_AFFECT_ADD			= 126,
	HEADER_GC_AFFECT_REMOVE			= 127,

	HEADER_GC_MALL_OPEN				= 122,
	HEADER_GC_MALL_SET				= 128,
	HEADER_GC_MALL_DEL				= 129,

	HEADER_GC_LAND_LIST				= 130,
	HEADER_GC_LOVER_INFO			= 131,
	HEADER_GC_LOVE_POINT_UPDATE			= 132,

	HEADER_GC_SYMBOL_DATA			= 133,

	// MINING
	HEADER_GC_DIG_MOTION			= 134,
	// END_OF_MINING

	HEADER_GC_DAMAGE_INFO           = 135,
	HEADER_GC_CHAR_ADDITIONAL_INFO	= 136,

	// SUPPORT_BGM
	HEADER_GC_MAIN_CHARACTER3_BGM		= 137,
	HEADER_GC_MAIN_CHARACTER4_BGM_VOL	= 138,
	// END_OF_SUPPORT_BGM

	HEADER_GC_AUTH_SUCCESS			= 150,
#if defined(ENABLE_TEXTS_RENEWAL)
	HEADER_GC_CHAT_NEW = 155,
#endif
#if defined(ENABLE_GAYA_RENEWAL)
	HEADER_GC_GEM_SHOP_OPEN = 156,
	HEADER_GC_GEM_SHOP_REFRESH = 157,
#endif
	// ROULETTE
	HEADER_GC_ROULETTE					= 200,
	// END_ROULETTE
	//
	HEADER_GC_SPECIFIC_EFFECT				= 208,

	HEADER_GC_DRAGON_SOUL_REFINE			= 209,
	HEADER_GC_RESPOND_CHANNELSTATUS			= 210,
#if defined(ENABLE_NEW_GOLD_LIMIT)
	HEADER_GC_CHARACTER_GOLD = 213,
	HEADER_GC_CHARACTER_GOLD_CHANGE = 216,
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
	HEADER_GC_NEW_OFFLINESHOP				= 214,
#endif

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	HEADER_GC_CUBE_RENEWAL = 217,
#endif
#if defined(ENABLE_CHANGELOOK)
	HEADER_GC_CHANGE_LOOK = 219,
#endif
#if defined(ENABLE_AURA_SYSTEM)
	HEADER_GC_AURA = 221,
#endif
#ifdef ENABLE_ATLAS_BOSS
	HEADER_GC_BOSS_POSITION = 222,
#endif
#ifdef ENABLE_NEW_FISHING_SYSTEM
	HEADER_GC_FISHING_NEW = 223, 
#endif
#if defined(ENABLE_NEW_FISH_EVENT)
	HEADER_GC_FISH_EVENT_INFO = 224,
#endif
#if defined(USE_BATTLEPASS)
	HEADER_GC_EXT_BATTLE_PASS_OPEN = 225,
	HEADER_GC_EXT_BATTLE_PASS_GENERAL_INFO = 226,
	HEADER_GC_EXT_BATTLE_PASS_MISSION_INFO = 227,
	HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE = 228,
	HEADER_GC_EXT_BATTLE_PASS_SEND_RANKING = 229,
#endif
	HEADER_GC_BOSS_POSITION = 230,
	HEADER_GC_WHISPER_INFO = 231,
	/////////////////////////////////////////////////////////////////////////////

#ifdef ENABLE_SWITCHBOT_WORLDARD
    HEADER_GC_SWITCHBOT						= 171,
#endif
	HEADER_GG_LOGIN				= 1,
	HEADER_GG_LOGOUT				= 2,
	HEADER_GG_RELAY				= 3,
	HEADER_GG_NOTICE				= 4,
	HEADER_GG_SHUTDOWN				= 5,
	HEADER_GG_GUILD				= 6,
	HEADER_GG_DISCONNECT			= 7,	// 누군가의 접속을 강제로 끊을 때
	HEADER_GG_SHOUT				= 8,
	HEADER_GG_SETUP				= 9,
	HEADER_GG_MESSENGER_ADD                     = 10,
	HEADER_GG_MESSENGER_REMOVE                  = 11,
	HEADER_GG_FIND_POSITION			= 12,
	HEADER_GG_WARP_CHARACTER			= 13,
	HEADER_GG_MESSENGER_MOBILE			= 14,
	HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX		= 15,
	HEADER_GG_TRANSFER				= 16,
	HEADER_GG_RELOAD_CRC_LIST			= 19,
	HEADER_GG_LOGIN_PING			= 20,
	HEADER_GG_CHECK_CLIENT_VERSION		= 21,
	HEADER_GG_BLOCK_CHAT			= 22,

	HEADER_GG_CHECK_AWAKENESS		= 29,
#ifdef ENABLE_FULL_NOTICE
	HEADER_GG_BIG_NOTICE			= 30,
#endif
#if defined(ENABLE_MESSENGER_BLOCK)
	HEADER_GG_MESSENGER_BLOCK = 31,
#endif
#if defined(ENABLE_TEXTS_RENEWAL)
	HEADER_GG_CHAT_NEW = 32,
#endif
#if defined(ENABLE_OFFLINESHOP_REWORK)
	HEADER_GG_OFFLINE_SHOP_NOTIFICATION	= 33,
#endif
#if defined(ENABLE_ULTIMATE_REGEN)
	HEADER_GG_NEW_REGEN = 40,
#endif
#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
	HEADER_GG_WHISPER_SYSTEM = 47,
#endif
#ifdef ENABLE_REWARD_SYSTEM
	HEADER_GG_REWARD_INFO = 48,
#endif
#if defined(ENABLE_CLOSE_GAMECLIENT_CMD)
	HEADER_GG_CLOSECLIENT = 49,
#endif
};

#pragma pack(1)
#if defined(ENABLE_ULTIMATE_REGEN)
enum {
	NEW_REGEN_LOAD,
	NEW_REGEN_REFRESH,
};

typedef struct SGGPacketNewRegen {
	uint8_t header;
	uint8_t subHeader;
	WORD id;
	bool isAlive;
} TGGPacketNewRegen;
#endif

typedef struct SPacketGGSetup
{
	BYTE	bHeader;
	WORD	wPort;
	BYTE	bChannel;
} TPacketGGSetup;

typedef struct SPacketGGLogin
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD	dwPID;
	BYTE	bEmpire;
	long	lMapIndex;
	BYTE	bChannel;
	char country_flag[3 + 1];
} TPacketGGLogin;

typedef struct SPacketGGLogout
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGLogout;

typedef struct SPacketGGRelay
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lSize;
} TPacketGGRelay;

typedef struct SPacketGGNotice
{
	BYTE	bHeader;
	long	lSize;
} TPacketGGNotice;

typedef struct SPacketGGShutdown
{
	BYTE	bHeader;
} TPacketGGShutdown;

typedef struct SPacketGGGuild
{
	BYTE	bHeader;
	BYTE	bSubHeader;
	DWORD	dwGuild;
} TPacketGGGuild;

enum
{
	GUILD_SUBHEADER_GG_CHAT,
	GUILD_SUBHEADER_GG_SET_MEMBER_COUNT_BONUS,
};

typedef struct SPacketGGGuildChat
{
	uint8_t bHeader;
	uint8_t bSubHeader;
	uint8_t dwGuild;
#if defined(ENABLE_MESSENGER_BLOCK)
	char name[CHARACTER_NAME_MAX_LEN + 1];
#endif
	char szText[CHAT_MAX_LEN + 1];
} TPacketGGGuildChat;

typedef struct SPacketGGParty
{
	BYTE	header;
	BYTE	subheader;
	DWORD	pid;
	DWORD	leaderpid;
} TPacketGGParty;

enum
{
	PARTY_SUBHEADER_GG_CREATE,
	PARTY_SUBHEADER_GG_DESTROY,
	PARTY_SUBHEADER_GG_JOIN,
	PARTY_SUBHEADER_GG_QUIT,
};

typedef struct SPacketGGDisconnect
{
	BYTE	bHeader;
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGDisconnect;

typedef struct SPacketGGShout {
	uint8_t bHeader;
	uint8_t bEmpire;
#if defined(ENABLE_MESSENGER_BLOCK)
	char name[CHARACTER_NAME_MAX_LEN + 1];
#endif
	char szText[CHAT_MAX_LEN + 1];
} TPacketGGShout;

typedef struct SMessengerData
{
	char        szMobile[MOBILE_MAX_LEN + 1];
} TMessengerData;

typedef struct SPacketGGMessenger
{
	BYTE        bHeader;
	char        szAccount[CHARACTER_NAME_MAX_LEN + 1];
	char        szCompanion[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGMessenger;

typedef struct SPacketGGMessengerMobile
{
	BYTE        bHeader;
	char        szName[CHARACTER_NAME_MAX_LEN + 1];
	char        szMobile[MOBILE_MAX_LEN + 1];
} TPacketGGMessengerMobile;

typedef struct SPacketGGFindPosition
{
	BYTE header;
	DWORD dwFromPID; // 저 위치로 워프하려는 사람
	DWORD dwTargetPID; // 찾는 사람
} TPacketGGFindPosition;

typedef struct SPacketGGWarpCharacter
{
	BYTE header;
	DWORD pid;
	long x;
	long y;
#ifdef __CMD_WARP_IN_DUNGEON__
	int mapIndex;
#endif
} TPacketGGWarpCharacter;

//  HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX	    = 15,

typedef struct SPacketGGGuildWarMapIndex
{
	BYTE bHeader;
	DWORD dwGuildID1;
	DWORD dwGuildID2;
	long lMapIndex;
} TPacketGGGuildWarMapIndex;

typedef struct SPacketGGTransfer
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lX, lY;
} TPacketGGTransfer;

typedef struct SPacketGGLoginPing
{
	BYTE	bHeader;
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGLoginPing;

typedef struct SPacketGGBlockChat
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lBlockDuration;
} TPacketGGBlockChat;

/* 클라이언트 측에서 보내는 패킷 */

typedef struct command_text
{
	BYTE	bHeader;
} TPacketCGText;

/* 로그인 (1) */
typedef struct command_handshake
{
	BYTE	bHeader;
	DWORD	dwHandshake;
	DWORD	dwTime;
	long	lDelta;
} TPacketCGHandshake;

typedef struct command_login
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
} TPacketCGLogin;

typedef struct command_login2
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	DWORD	dwLoginKey;
	DWORD	adwClientKey[4];
} TPacketCGLogin2;
typedef struct command_login3
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
	DWORD	adwClientKey[4];
#ifdef ENABLE_HWID
	char hwid[HWID_LENGTH + 1];
#endif
#if defined(ENABLE_FILES_CHECK)
	char key[128 + 1];
#endif
#if defined(ENABLE_NEW_ANTICHEAT_RULES)
	uint32_t loginHash;
#endif
#ifdef ENABLE_MULTI_LANGUAGE
    uint8_t lang;
#endif
} TPacketCGLogin3;

#ifdef __NEWPET_SYSTEM__
typedef struct packet_RequestPetName
{
	BYTE byHeader;
	char petname[13];

}TPacketCGRequestPetName;
#endif

typedef struct packet_login_key
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
} TPacketGCLoginKey;

typedef struct command_player_select
{
	BYTE	header;
	BYTE	index;
} TPacketCGPlayerSelect;

typedef struct command_player_delete
{
	BYTE	header;
	BYTE	index;
	char	private_code[8];
} TPacketCGPlayerDelete;

typedef struct command_player_create
{
	BYTE	header;
	BYTE	index;
	char	name[CHARACTER_NAME_MAX_LEN + 1];
	WORD	job;
	BYTE	shape;
	BYTE	Con;
	BYTE	Int;
	BYTE	Str;
	BYTE	Dex;
	char country_flag[3 + 1];
} TPacketCGPlayerCreate;

typedef struct command_player_create_success
{
	BYTE			header;
	BYTE			bAccountCharacterIndex;
	TSimplePlayer	player;
} TPacketGCPlayerCreateSuccess;

typedef struct SPacketCGAttack
{
	uint8_t bHeader;
	uint8_t bType;
	uint32_t dwVID;
	uint8_t bCRCMagicCubeProcPiece;
	uint8_t bCRCMagicCubeFilePiece;
#if defined(ENABLE_NEW_ANTICHEAT_RULES)
	uint32_t dwVictimVIDVerify;
#endif
} TPacketCGAttack;

enum EMoveFuncType
{
	FUNC_WAIT,
	FUNC_MOVE,
	FUNC_ATTACK,
	FUNC_COMBO,
	FUNC_MOB_SKILL,
	_FUNC_SKILL,
	FUNC_MAX_NUM,
	FUNC_SKILL = 0x80,
};

// 이동
typedef struct command_move
{
	BYTE	bHeader;
	BYTE	bFunc;
	BYTE	bArg;
	BYTE	bRot;
	long	lX;
	long	lY;
	DWORD	dwTime;
#if defined(ENABLE_NEW_ANTICHEAT_RULES)
	long uX;
	long uY;
#endif
} TPacketCGMove;

typedef struct command_sync_position_element
{
	DWORD	dwVID;
	long	lX;
	long	lY;
} TPacketCGSyncPositionElement;

// 위치 동기화
typedef struct command_sync_position	// 가변 패킷
{
	BYTE	bHeader;
	WORD	wSize;
} TPacketCGSyncPosition;

/* 채팅 (3) */
typedef struct command_chat	// 가변 패킷
{
	BYTE	header;
	WORD	size;
	BYTE	type;
} TPacketCGChat;

/* 귓속말 */
typedef struct command_whisper
{
	BYTE	bHeader;
	WORD	wSize;
	char 	szNameTo[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisper;

typedef struct command_entergame
{
	BYTE	header;
} TPacketCGEnterGame;

typedef struct command_item_use
{
	BYTE 	header;
	TItemPos 	Cell;
} TPacketCGItemUse;

typedef struct command_item_use_to_item
{
	BYTE	header;
	TItemPos	Cell;
	TItemPos	TargetCell;
} TPacketCGItemUseToItem;

typedef struct command_item_drop
{
	BYTE 	header;
	TItemPos 	Cell;
} TPacketCGItemDrop;

typedef struct command_item_drop2
{
	BYTE 	header;
	TItemPos 	Cell;
	WORD count;
} TPacketCGItemDrop2;

typedef struct command_item_destroy
{
	BYTE		header;
	TItemPos	Cell;
} TPacketCGItemDestroy;

typedef struct command_item_division
{
	BYTE		header;
	TItemPos	pos;
} TPacketCGItemDivision;

typedef struct command_item_move
{
	BYTE 	header;
	TItemPos	Cell;
	TItemPos	CellTo;
	WORD count;
} TPacketCGItemMove;

typedef struct command_item_pickup
{
	BYTE 	header;
	DWORD	vid;
} TPacketCGItemPickup;

enum
{
	SHOP_SUBHEADER_CG_END,
	SHOP_SUBHEADER_CG_BUY,
	SHOP_SUBHEADER_CG_SELL,
	SHOP_SUBHEADER_CG_SELL2
#ifdef ENABLE_BUY_STACK_FROM_SHOP
	,SHOP_SUBHEADER_CG_BUY2
#endif
};

typedef struct command_shop_buy
{
	BYTE	count;
} TPacketCGShopBuy;

typedef struct command_shop_sell
{
	BYTE	pos;
	BYTE	count;
} TPacketCGShopSell;

typedef struct command_shop
{
	BYTE	header;
	BYTE	subheader;
} TPacketCGShop;

typedef struct command_on_click
{
	BYTE	header;
	DWORD	vid;
} TPacketCGOnClick;

enum
{
	EXCHANGE_SUBHEADER_CG_START,	/* arg1 == vid of target character */
	EXCHANGE_SUBHEADER_CG_ITEM_ADD,	/* arg1 == position of item */
	EXCHANGE_SUBHEADER_CG_ITEM_DEL,	/* arg1 == position of item */
	EXCHANGE_SUBHEADER_CG_ELK_ADD,	/* arg1 == amount of gold */
	EXCHANGE_SUBHEADER_CG_ACCEPT,	/* arg1 == not used */
	EXCHANGE_SUBHEADER_CG_CANCEL,	/* arg1 == not used */
};

typedef struct command_exchange
{
	BYTE	header;
	BYTE	sub_header;
#if defined(ENABLE_NEW_GOLD_LIMIT)
	uint64_t arg1;
#else
	uint32_t arg1;
#endif
	BYTE arg2;
	TItemPos	Pos;
#if defined(ITEM_CHECKINOUT_UPDATE)
	bool	SelectPosAuto;
#endif
} TPacketCGExchange;

typedef struct command_position
{
	BYTE	header;
	BYTE	position;
} TPacketCGPosition;

typedef struct command_script_answer
{
	BYTE	header;
	BYTE	answer;
	//char	file[32 + 1];
	//BYTE	answer[16 + 1];
} TPacketCGScriptAnswer;


typedef struct command_script_button
{
	BYTE        header;
	unsigned int	idx;
} TPacketCGScriptButton;

typedef struct command_quest_input_string
{
	BYTE header;
	char msg[64+1];
} TPacketCGQuestInputString;

typedef struct command_quest_confirm
{
	BYTE header;
	BYTE answer;
	DWORD requestPID;
} TPacketCGQuestConfirm;

/*
 * 서버 측에서 보내는 패킷
 */
typedef struct packet_quest_confirm
{
	BYTE header;
	char msg[64+1];
	long timeout;
	DWORD requestPID;
} TPacketGCQuestConfirm;

typedef struct packet_handshake
{
	BYTE	bHeader;
	DWORD	dwHandshake;
	DWORD	dwTime;
	long	lDelta;
} TPacketGCHandshake;

enum EPhase
{
	PHASE_CLOSE,
	PHASE_HANDSHAKE,
	PHASE_LOGIN,
	PHASE_SELECT,
	PHASE_LOADING,
	PHASE_GAME,
	PHASE_DEAD,

	PHASE_CLIENT_CONNECTING,
	PHASE_DBCLIENT,
	PHASE_P2P,
	PHASE_AUTH,
	PHASE_GUILD_MARK,
};

typedef struct packet_phase
{
	BYTE	header;
	BYTE	phase;
} TPacketGCPhase;

typedef struct packet_bindudp
{
	BYTE	header;
	DWORD	addr;
	WORD	port;
} TPacketGCBindUDP;

enum
{
	LOGIN_FAILURE_ALREADY	= 1,
	LOGIN_FAILURE_ID_NOT_EXIST	= 2,
	LOGIN_FAILURE_WRONG_PASS	= 3,
	LOGIN_FAILURE_FALSE		= 4,
	LOGIN_FAILURE_NOT_TESTOR	= 5,
	LOGIN_FAILURE_NOT_TEST_TIME	= 6,
	LOGIN_FAILURE_FULL		= 7
};

typedef struct packet_login_success
{
	BYTE		bHeader;
	TSimplePlayer	players[PLAYER_PER_ACCOUNT];
	DWORD		guild_id[PLAYER_PER_ACCOUNT];
	char		guild_name[PLAYER_PER_ACCOUNT][GUILD_NAME_MAX_LEN+1];

	DWORD		handle;
	DWORD		random_key;
} TPacketGCLoginSuccess;

typedef struct packet_auth_success
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
	BYTE	bResult;
} TPacketGCAuthSuccess;

typedef struct packet_login_failure
{
	BYTE	header;
	char	szStatus[ACCOUNT_STATUS_MAX_LEN + 1];
} TPacketGCLoginFailure;

typedef struct packet_create_failure
{
	BYTE	header;
	BYTE	bType;
} TPacketGCCreateFailure;

enum
{
	ADD_CHARACTER_STATE_DEAD		= (1 << 0),
	ADD_CHARACTER_STATE_SPAWN		= (1 << 1),
	ADD_CHARACTER_STATE_GUNGON		= (1 << 2),
	ADD_CHARACTER_STATE_KILLER		= (1 << 3),
	ADD_CHARACTER_STATE_PARTY		= (1 << 4),
};

enum ECharacterEquipmentPart
{
	CHR_EQUIPPART_ARMOR,
	CHR_EQUIPPART_WEAPON,
	CHR_EQUIPPART_HEAD,
	CHR_EQUIPPART_HAIR,
#ifdef ENABLE_ACCE_SYSTEM
	CHR_EQUIPPART_ACCE,
#endif
#ifdef ENABLE_COSTUME_EFFECT
	CHR_EQUIPPART_EFFECT_BODY,
	CHR_EQUIPPART_EFFECT_WEAPON,
#endif
#if defined(ENABLE_AURA_SYSTEM)
	CHR_EQUIPPART_AURA,
#endif
	CHR_EQUIPPART_NUM,
};

typedef struct packet_add_char
{
	BYTE	header;
	DWORD	dwVID;
	float	angle;
	long	x;
	long	y;
	long	z;
	BYTE	bType;
	WORD	wRaceNum;
	WORD	bMovingSpeed;
	WORD	bAttackSpeed;
	BYTE	bStateFlag;
	DWORD	dwAffectFlag[2];
#ifdef ENABLE_MULTI_NAMES
	bool transname;
#endif
} TPacketGCCharacterAdd;

typedef struct packet_char_additional_info
{
	BYTE    header;
	DWORD   dwVID;
	char    name[CHARACTER_NAME_MAX_LEN + 1];
	uint32_t awPart[CHR_EQUIPPART_NUM];
	BYTE	bEmpire;
	DWORD   dwGuildID;
	DWORD   dwLevel;
	short	sAlignment;
	BYTE	bPKMode;
	DWORD	dwMountVnum;
	
#ifdef __ENABLE_PREMIUM_PLAYERS__
	BYTE 	byPremium;
	long int iPremiumTime;
#endif
	
#ifdef __SKILL_COLOR_SYSTEM__
	DWORD	dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif

	char country_flag[3 + 1];
	BYTE bGuildLeaderGrade;
} TPacketGCCharacterAdditionalInfo;

typedef struct packet_update_char
{
	BYTE	header;
	DWORD	dwVID;

	uint32_t awPart[CHR_EQUIPPART_NUM];
	WORD	bMovingSpeed;
	WORD	bAttackSpeed;

	BYTE	bStateFlag;
	DWORD	dwAffectFlag[2];

	DWORD	dwGuildID;
	short	sAlignment;
	BYTE	bPKMode;
	DWORD	dwMountVnum;
#ifdef __ENABLE_PREMIUM_PLAYERS__
	BYTE 	byPremium;
	long int iPremiumTime;
#endif
#ifdef __SKILL_COLOR_SYSTEM__
	DWORD	dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif
	BYTE bGuildLeaderGrade;
} TPacketGCCharacterUpdate;

typedef struct packet_del_char
{
	BYTE	header;
	DWORD	id;
} TPacketGCCharacterDelete;

#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
typedef struct SPacketCGWhisperAdmin
{
	BYTE	header;
	char	szText[512 + 1];
	char	szLang[2 + 1];
	int		color;
} TPacketCGWhisperAdmin;

typedef struct SPacketGGWhisperSystem
{
	BYTE	bHeader;
	long	lSize;
} TPacketGGWhisperSystem;
#endif

typedef struct packet_chat	// 가변 패킷
{
	BYTE	header;
	WORD	size;
	BYTE	type;
	DWORD	id;
	BYTE	bEmpire;
} TPacketGCChat;

typedef struct packet_whisper	// 가변 패킷
{
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bType;
	char	szNameFrom[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCWhisper;

typedef struct packet_main_character
{
	BYTE        header;
	DWORD	dwVID;
	WORD	wRaceNum;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter;

// SUPPORT_BGM
typedef struct packet_main_character3_bgm
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE    header;
	DWORD	dwVID;
	WORD	wRaceNum;
	char	szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char	szBGMName[MUSIC_NAME_LEN + 1];
	long	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter3_BGM;

typedef struct packet_main_character4_bgm_vol
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE    header;
	DWORD	dwVID;
	WORD	wRaceNum;
	char	szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char	szBGMName[MUSIC_NAME_LEN + 1];
	float	fBGMVol;
	long	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter4_BGM_VOL;
// END_OF_SUPPORT_BGM

typedef struct packet_points
{
	uint8_t header;
	int32_t points[POINT_MAX_NUM];
} TPacketGCPoints;

typedef struct packet_skill_level
{
	BYTE		bHeader;
	TPlayerSkill	skills[SKILL_MAX_NUM];
} TPacketGCSkillLevel;

typedef struct packet_point_change
{
	uint8_t header;
	uint32_t dwVID;
	uint8_t type;
	long amount;
	long value;
} TPacketGCPointChange;

typedef struct packet_stun
{
	BYTE	header;
	DWORD	vid;
} TPacketGCStun;

typedef struct packet_dead
{
	BYTE	header;
	DWORD	vid;
} TPacketGCDead;

struct TPacketGCItemDelDeprecated
{
	BYTE	header;
	TItemPos Cell;
	DWORD	vnum;
	WORD count;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#if defined(ENABLE_CHANGELOOK)
	uint32_t transmutation;
#endif
#ifdef ATTR_LOCK
	short	lockedattr;
#endif
};

typedef struct packet_item_set
{
	BYTE	header;
	TItemPos Cell;
	DWORD	vnum;
	WORD count;
	DWORD	flags;
	DWORD	anti_flags;
	bool	highlight;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#if defined(ENABLE_CHANGELOOK)
	uint32_t transmutation;
#endif
#ifdef ATTR_LOCK
	short	lockedattr;
#endif
} TPacketGCItemSet;

typedef struct packet_item_del
{
	BYTE	header;
	BYTE	pos;
} TPacketGCItemDel;

struct packet_item_use
{
	BYTE	header;
	TItemPos Cell;
	DWORD	ch_vid;
	DWORD	victim_vid;
	DWORD	vnum;
};

struct packet_item_move
{
	BYTE	header;
	TItemPos Cell;
	TItemPos CellTo;
};

typedef struct packet_item_update
{
	BYTE	header;
	TItemPos Cell;
	WORD count;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#if defined(ENABLE_CHANGELOOK)
	uint32_t transmutation;
#endif
#ifdef ATTR_LOCK
	short	lockedattr;
#endif
} TPacketGCItemUpdate;

typedef struct packet_item_ground_add
{
	BYTE	bHeader;
	long 	x, y, z;
	DWORD	dwVID;
	DWORD	dwVnum;
} TPacketGCItemGroundAdd;

typedef struct packet_item_ownership
{
	BYTE	bHeader;
	DWORD	dwVID;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCItemOwnership;

typedef struct packet_item_ground_del
{
	BYTE	bHeader;
	DWORD	dwVID;
} TPacketGCItemGroundDel;

struct packet_motion
{
	BYTE	header;
	DWORD	vid;
	DWORD	victim_vid;
	WORD	motion;
};

enum EPacketShopSubHeaders
{
	SHOP_SUBHEADER_GC_START,
	SHOP_SUBHEADER_GC_END,
	SHOP_SUBHEADER_GC_UPDATE_ITEM,
	SHOP_SUBHEADER_GC_UPDATE_PRICE,
	SHOP_SUBHEADER_GC_OK,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY,
#ifdef ENABLE_BUY_WITH_ITEM
	SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM,
#endif
#if defined(ENABLE_WORLDBOSS)
	SHOP_SUBHEADER_GC_NOT_ENOUGH_WB_POINTS,
#endif
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
	SHOP_SUBHEADER_GC_NOT_ENOUGH_SKULLS,
#endif
	SHOP_SUBHEADER_GC_SOLDOUT,
	SHOP_SUBHEADER_GC_INVENTORY_FULL,
	SHOP_SUBHEADER_GC_INVALID_POS,
	SHOP_SUBHEADER_GC_SOLD_OUT,
	SHOP_SUBHEADER_GC_START_EX,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX,
};

struct packet_shop_item
{
	DWORD vnum;
#if defined(ENABLE_NEW_GOLD_LIMIT)
	uint64_t price;
#else
	uint32_t price;
#endif
	WORD count;
#ifdef ENABLE_BUY_WITH_ITEM
	TShopItemPrice	itemprice[MAX_SHOP_PRICES];
#endif
	BYTE display_pos;
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#if defined(ENABLE_CHANGELOOK)
	uint32_t transmutation;
#endif
#ifdef ATTR_LOCK
	short lockedattr;
#endif
};

typedef struct packet_shop_start
{
	DWORD   owner_vid;
	struct packet_shop_item	items[SHOP_HOST_ITEM_MAX_NUM];
} TPacketGCShopStart;

typedef struct packet_shop_start_ex // 다음에 TSubPacketShopTab* shop_tabs 이 따라옴.
{
	typedef struct sub_packet_shop_tab
	{
		char name[SHOP_TAB_NAME_MAX];
		BYTE coin_type;
		packet_shop_item items[SHOP_HOST_ITEM_MAX_NUM];
	} TSubPacketShopTab;
	DWORD owner_vid;
	BYTE shop_tab_count;
} TPacketGCShopStartEx;

typedef struct packet_shop_update_item
{
	BYTE			pos;
	struct packet_shop_item	item;
} TPacketGCShopUpdateItem;

typedef struct packet_shop_update_price
{
#if defined(ENABLE_NEW_GOLD_LIMIT)
	uint64_t iPrice;
#else
	uint32_t iPrice;
#endif
} TPacketGCShopUpdatePrice;

typedef struct packet_shop	// 가변 패킷
{
	BYTE        header;
	WORD	size;
	BYTE        subheader;
} TPacketGCShop;

struct packet_exchange
{
	BYTE	header;
	BYTE	sub_header;
	BYTE	is_me;
#if defined(ENABLE_NEW_GOLD_LIMIT)
	uint64_t arg1;
#else
	uint32_t arg1;
#endif
	TItemPos	arg2;	// cell
	DWORD	arg3;	// count
#ifdef WJ_ENABLE_TRADABLE_ICON
	TItemPos	arg4;
#endif
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#if defined(ENABLE_CHANGELOOK)
	uint32_t dwTransmutation;
#endif
#ifdef ATTR_LOCK
	short	lockedattr;
#endif
};

enum EPacketTradeSubHeaders
{
	EXCHANGE_SUBHEADER_GC_START,	/* arg1 == vid */
	EXCHANGE_SUBHEADER_GC_ITEM_ADD,	/* arg1 == vnum  arg2 == pos  arg3 == count */
	EXCHANGE_SUBHEADER_GC_ITEM_DEL,
	EXCHANGE_SUBHEADER_GC_GOLD_ADD,	/* arg1 == gold */
	EXCHANGE_SUBHEADER_GC_ACCEPT,	/* arg1 == accept */
	EXCHANGE_SUBHEADER_GC_END,		/* arg1 == not used */
	EXCHANGE_SUBHEADER_GC_ALREADY,	/* arg1 == not used */
	EXCHANGE_SUBHEADER_GC_LESS_GOLD,	/* arg1 == not used */
};

struct packet_position
{
	BYTE	header;
	DWORD	vid;
	BYTE	position;
};

typedef struct packet_ping
{
	BYTE	header;
} TPacketGCPing;

struct packet_script
{
	BYTE	header;
	WORD	size;
	BYTE	skin;
	WORD	src_size;
};

typedef struct packet_change_speed
{
	BYTE		header;
	DWORD		vid;
	WORD		moving_speed;
} TPacketGCChangeSpeed;

struct packet_mount
{
	BYTE	header;
	DWORD	vid;
	DWORD	mount_vid;
	BYTE	pos;
	DWORD	x, y;
};

typedef struct packet_move
{
	BYTE		bHeader;
	BYTE		bFunc;
	BYTE		bArg;
	BYTE		bRot;
	DWORD		dwVID;
	long		lX;
	long		lY;
	DWORD		dwTime;
	DWORD		dwDuration;
} TPacketGCMove;

// 소유권
typedef struct packet_ownership
{
	BYTE		bHeader;
	DWORD		dwOwnerVID;
	DWORD		dwVictimVID;
} TPacketGCOwnership;

// 위치 동기화 패킷의 bCount 만큼 붙는 단위
typedef struct packet_sync_position_element
{
	DWORD	dwVID;
	long	lX;
	long	lY;
} TPacketGCSyncPositionElement;

// 위치 동기화
typedef struct packet_sync_position	// 가변 패킷
{
	BYTE	bHeader;
	WORD	wSize;	// 개수 = (wSize - sizeof(TPacketGCSyncPosition)) / sizeof(TPacketGCSyncPositionElement)
} TPacketGCSyncPosition;

typedef struct packet_fly
{
	BYTE	bHeader;
	BYTE	bType;
	DWORD	dwStartVID;
	DWORD	dwEndVID;
} TPacketGCCreateFly;

typedef struct command_fly_targeting
{
	BYTE		bHeader;
	DWORD		dwTargetVID;
	long		x, y;
} TPacketCGFlyTargeting;

typedef struct packet_fly_targeting
{
	BYTE		bHeader;
	DWORD		dwShooterVID;
	DWORD		dwTargetVID;
	long		x, y;
} TPacketGCFlyTargeting;

typedef struct packet_shoot
{
	BYTE		bHeader;
	BYTE		bType;
} TPacketCGShoot;

typedef struct packet_duel_start
{
	BYTE	header;
	WORD	wSize;	// DWORD가 몇개? 개수 = (wSize - sizeof(TPacketGCPVPList)) / 4
} TPacketGCDuelStart;

enum EPVPModes
{
	PVP_MODE_NONE,
	PVP_MODE_AGREE,
	PVP_MODE_FIGHT,
	PVP_MODE_REVENGE
};

typedef struct packet_pvp
{
	BYTE        bHeader;
	DWORD       dwVIDSrc;
	DWORD       dwVIDDst;
	BYTE        bMode;	// 0 이면 끔, 1이면 켬
} TPacketGCPVP;

typedef struct command_use_skill
{
	BYTE	bHeader;
	DWORD	dwVnum;
	DWORD	dwVID;
} TPacketCGUseSkill;

typedef struct command_target
{
	BYTE	header;
	DWORD	dwVID;
} TPacketCGTarget;

typedef struct packet_target
{
	BYTE	header;
	DWORD	dwVID;
	BYTE	bHPPercent;
#ifdef __VIEW_TARGET_DECIMAL_HP__
	int		iMinHP;
	int		iMaxHP;
#endif
#ifdef ELEMENT_TARGET
	BYTE	bElement;
#endif
} TPacketGCTarget;

typedef struct packet_warp
{
	BYTE	bHeader;
	long	lX;
	long	lY;
	long	lAddr;
	WORD	wPort;
} TPacketGCWarp;

typedef struct command_warp
{
	BYTE	bHeader;
} TPacketCGWarp;

struct packet_quest_info
{
	BYTE header;
	WORD size;
	WORD index;
	BYTE flag;
	char	szTitle[30 + 1];
	BYTE	isBegin;
	char	szClockName[16 + 1];
	int		iClockValue;
	char	szCounterName[16 + 1];
	int		iCounterValue;
	char	szIconFileName[24 + 1];
};

enum
{
#if defined(ENABLE_MESSENGER_TEAM)
	MESSENGER_SUBHEADER_GC_TEAM,
	MESSENGER_SUBHEADER_GC_TEAM_UPDATE,
#endif
	MESSENGER_SUBHEADER_GC_LIST,
	MESSENGER_SUBHEADER_GC_LOGIN,
	MESSENGER_SUBHEADER_GC_LOGOUT,
	MESSENGER_SUBHEADER_GC_INVITE,
	MESSENGER_SUBHEADER_GC_MOBILE,
	MESSENGER_SUBHEADER_GC_REMOVE_FRIEND,
#if defined(ENABLE_MESSENGER_BLOCK)
	MESSENGER_SUBHEADER_GC_BLOCK_LIST,
	MESSENGER_SUBHEADER_GC_BLOCK_LOGIN,
	MESSENGER_SUBHEADER_GC_BLOCK_LOGOUT,
	MESSENGER_SUBHEADER_GC_BLOCK_INVITE,
#endif
};

#if defined(ENABLE_MESSENGER_TEAM)
typedef struct STeamMessenger
{
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	bool IsConnected;
} TTeamMessenger;
#endif

typedef struct packet_messenger
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCMessenger;

typedef struct packet_messenger_guild_list
{
	BYTE connected;
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildList;

typedef struct packet_messenger_guild_login
{
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildLogin;

typedef struct packet_messenger_guild_logout
{
	BYTE length;

	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildLogout;

typedef struct packet_messenger_list_offline
{
	BYTE connected; // always 0
	BYTE length;
} TPacketGCMessengerListOffline;

typedef struct packet_messenger_list_online
{
	BYTE connected; // always 1
	BYTE length;
} TPacketGCMessengerListOnline;

enum
{
	MESSENGER_SUBHEADER_CG_ADD_BY_VID,
	MESSENGER_SUBHEADER_CG_ADD_BY_NAME,
	MESSENGER_SUBHEADER_CG_REMOVE,
	MESSENGER_SUBHEADER_CG_INVITE_ANSWER,
#if defined(ENABLE_MESSENGER_BLOCK)
	MESSENGER_SUBHEADER_CG_ADD_BLOCK_BY_VID,
	MESSENGER_SUBHEADER_CG_ADD_BLOCK_BY_NAME,
	MESSENGER_SUBHEADER_CG_REMOVE_BLOCK,
#endif
};

typedef struct command_messenger
{
	BYTE header;
	BYTE subheader;
} TPacketCGMessenger;

typedef struct command_messenger_add_by_vid
{
	DWORD vid;
} TPacketCGMessengerAddByVID;

typedef struct command_messenger_add_by_name
{
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketCGMessengerAddByName;

typedef struct command_messenger_remove
{
	char login[LOGIN_MAX_LEN+1];
	//DWORD account;
} TPacketCGMessengerRemove;

typedef struct command_safebox_checkout
{
	BYTE	bHeader;
	BYTE	bSafePos;
	TItemPos	ItemPos;
#if defined(ITEM_CHECKINOUT_UPDATE)
	bool	SelectPosAuto;
#endif
} TPacketCGSafeboxCheckout;

typedef struct command_safebox_checkin
{
	BYTE	bHeader;
	BYTE	bSafePos;
	TItemPos	ItemPos;
#if defined(ITEM_CHECKINOUT_UPDATE)
	bool	SelectPosAuto;
#endif
} TPacketCGSafeboxCheckin;

///////////////////////////////////////////////////////////////////////////////////
// Party

typedef struct command_party_parameter
{
	BYTE	bHeader;
	BYTE	bDistributeMode;
} TPacketCGPartyParameter;

typedef struct paryt_parameter
{
	BYTE	bHeader;
	BYTE	bDistributeMode;
} TPacketGCPartyParameter;

typedef struct packet_party_add
{
	BYTE	header;
	DWORD	pid;
	char	name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCPartyAdd;

typedef struct command_party_invite
{
	BYTE	header;
	DWORD	vid;
} TPacketCGPartyInvite;

typedef struct packet_party_invite
{
	BYTE	header;
	DWORD	leader_vid;
} TPacketGCPartyInvite;

typedef struct command_party_invite_answer
{
	BYTE	header;
	DWORD	leader_vid;
	BYTE	accept;
} TPacketCGPartyInviteAnswer;

typedef struct packet_party_update
{
	BYTE	header;
	DWORD	pid;
	BYTE	role;
	BYTE	percent_hp;
	short	affects[7];
} TPacketGCPartyUpdate;

typedef struct packet_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketGCPartyRemove;

typedef struct packet_party_link
{
	BYTE header;
	DWORD pid;
	DWORD vid;
} TPacketGCPartyLink;

typedef struct packet_party_unlink
{
	BYTE header;
	DWORD pid;
	DWORD vid;
} TPacketGCPartyUnlink;

typedef struct command_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketCGPartyRemove;

typedef struct command_party_set_state
{
	BYTE header;
	DWORD pid;
	BYTE byRole;
	BYTE flag;
} TPacketCGPartySetState;

enum
{
	PARTY_SKILL_HEAL = 1,
	PARTY_SKILL_WARP = 2
};

typedef struct command_party_use_skill
{
	BYTE header;
	BYTE bySkillIndex;
	DWORD vid;
} TPacketCGPartyUseSkill;

typedef struct packet_safebox_size
{
	BYTE bHeader;
	BYTE bSize;
} TPacketCGSafeboxSize;

typedef struct packet_safebox_wrong_password
{
	BYTE	bHeader;
} TPacketCGSafeboxWrongPassword;

typedef struct command_empire
{
	BYTE	bHeader;
	BYTE	bEmpire;
} TPacketCGEmpire;

typedef struct packet_empire
{
	BYTE	bHeader;
	BYTE	bEmpire;
} TPacketGCEmpire;

enum
{
	SAFEBOX_MONEY_STATE_SAVE,
	SAFEBOX_MONEY_STATE_WITHDRAW,
};

typedef struct command_safebox_money
{
	BYTE        bHeader;
	BYTE        bState;
	long	lMoney;
} TPacketCGSafeboxMoney;

typedef struct packet_safebox_money_change
{
	BYTE	bHeader;
	long	lMoney;
} TPacketGCSafeboxMoneyChange;

// Guild

enum
{
	GUILD_SUBHEADER_GC_LOGIN,
	GUILD_SUBHEADER_GC_LOGOUT,
	GUILD_SUBHEADER_GC_LIST,
	GUILD_SUBHEADER_GC_GRADE,
	GUILD_SUBHEADER_GC_ADD,
	GUILD_SUBHEADER_GC_REMOVE,
	GUILD_SUBHEADER_GC_GRADE_NAME,
	GUILD_SUBHEADER_GC_GRADE_AUTH,
	GUILD_SUBHEADER_GC_INFO,
	GUILD_SUBHEADER_GC_COMMENTS,
	GUILD_SUBHEADER_GC_CHANGE_EXP,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_GC_SKILL_INFO,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_GC_GUILD_INVITE,
	GUILD_SUBHEADER_GC_WAR,
	GUILD_SUBHEADER_GC_GUILD_NAME,
	GUILD_SUBHEADER_GC_GUILD_WAR_LIST,
	GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST,
	GUILD_SUBHEADER_GC_WAR_SCORE,
	GUILD_SUBHEADER_GC_MONEY_CHANGE,
	GUILD_SUBHEADER_GC_SYMBOL_AUTH_DATA,
	GUILD_SUBHEADER_GC_SYMBOL_LOGIN,
};

enum GUILD_SUBHEADER_CG
{
	GUILD_SUBHEADER_CG_ADD_MEMBER,
	GUILD_SUBHEADER_CG_REMOVE_MEMBER,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY,
	GUILD_SUBHEADER_CG_OFFER,
	GUILD_SUBHEADER_CG_POST_COMMENT,
	GUILD_SUBHEADER_CG_DELETE_COMMENT,
	GUILD_SUBHEADER_CG_REFRESH_COMMENT,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_CG_USE_SKILL,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER,
	GUILD_SUBHEADER_CG_CHARGE_GSP,
	GUILD_SUBHEADER_CG_DEPOSIT_MONEY,
	GUILD_SUBHEADER_CG_WITHDRAW_MONEY,
	GUILD_SUBHEADER_CG_CHANGE_SYMBOL
};

typedef struct packet_guild
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCGuild;

typedef struct packet_guild_name_t
{
	BYTE header;
	WORD size;
	BYTE subheader;
	DWORD	guildID;
	char	guildName[GUILD_NAME_MAX_LEN];
} TPacketGCGuildName;

typedef struct packet_guild_war
{
	DWORD	dwGuildSelf;
	DWORD	dwGuildOpp;
	BYTE	bType;
	BYTE 	bWarState;
} TPacketGCGuildWar;

typedef struct command_guild
{
	BYTE header;
	BYTE subheader;
} TPacketCGGuild;

typedef struct command_guild_answer_make_guild
{
	BYTE header;
	char guild_name[GUILD_NAME_MAX_LEN+1];
} TPacketCGAnswerMakeGuild;

typedef struct command_guild_use_skill
{
	DWORD	dwVnum;
	DWORD	dwPID;
} TPacketCGGuildUseSkill;

// Guild Mark
typedef struct command_mark_login
{
	BYTE    header;
	DWORD   handle;
	DWORD   random_key;
	DWORD dwHandshake;
} TPacketCGMarkLogin;

typedef struct packet_guild_symbol_auth
{
	uint8_t nonce[24];
	uint8_t ciphertext[32];
} TPacketGuildSymbolAuth;

typedef struct command_mark_upload
{
	BYTE	header;
	DWORD	gid, pid;
	BYTE	image[16*12*4];
} TPacketCGMarkUpload;

typedef struct command_mark_idxlist
{
	BYTE	header;
} TPacketCGMarkIDXList;

typedef struct command_mark_crclist
{
	BYTE	header;
	BYTE	imgIdx;
	DWORD	crclist[80];
} TPacketCGMarkCRCList;

typedef struct packet_mark_idxlist
{
	BYTE    header;
	DWORD	bufSize;
	WORD	count;
	//뒤에 size * (WORD + WORD)만큼 데이터 붙음
} TPacketGCMarkIDXList;

typedef struct packet_mark_block
{
	BYTE	header;
	DWORD	bufSize;
	BYTE	imgIdx;
	DWORD	count;
	// 뒤에 64 x 48 x 픽셀크기(4바이트) = 12288만큼 데이터 붙음
} TPacketGCMarkBlock;

typedef struct command_symbol_upload
{
	BYTE	header;
	WORD	size;
	DWORD	guild_id;
} TPacketCGGuildSymbolUpload;

typedef struct command_symbol_crc
{
	BYTE header;
	DWORD guild_id;
	DWORD crc;
	DWORD size;
} TPacketCGSymbolCRC;

typedef struct packet_symbol_data
{
	BYTE header;
	WORD size;
	DWORD guild_id;
} TPacketGCGuildSymbolData;

// Fishing

typedef struct command_fishing
{
	BYTE header;
	BYTE dir;
} TPacketCGFishing;

typedef struct packet_fishing
{
	BYTE header;
	BYTE subheader;
	DWORD info;
	BYTE dir;
} TPacketGCFishing;

enum
{
	FISHING_SUBHEADER_GC_START,
	FISHING_SUBHEADER_GC_STOP,
	FISHING_SUBHEADER_GC_REACT,
	FISHING_SUBHEADER_GC_SUCCESS,
	FISHING_SUBHEADER_GC_FAIL,
	FISHING_SUBHEADER_GC_FISH,
};

typedef struct command_give_item
{
	BYTE byHeader;
	DWORD dwTargetVID;
	TItemPos ItemPos;
	WORD byItemCount;
} TPacketCGGiveItem;

typedef struct SPacketCGHack
{
	BYTE	bHeader;
	char	szBuf[255 + 1];
} TPacketCGHack;

// SubHeader - Dungeon
enum
{
	DUNGEON_SUBHEADER_GC_TIME_ATTACK_START = 0,
	DUNGEON_SUBHEADER_GC_DESTINATION_POSITION = 1,
};

typedef struct packet_dungeon
{
	BYTE bHeader;
	WORD size;
	BYTE subheader;
} TPacketGCDungeon;

typedef struct packet_dungeon_dest_position
{
	long x;
	long y;
} TPacketGCDungeonDestPosition;

typedef struct SPacketGCShopSign
{
	BYTE	bHeader;
	DWORD	dwVID;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
} TPacketGCShopSign;

typedef struct SPacketCGMyShop
{
	BYTE	bHeader;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
	BYTE	bCount;
} TPacketCGMyShop;

typedef struct SPacketGCTime
{
	BYTE	bHeader;
	time_t	time;
} TPacketGCTime;

enum
{
	WALKMODE_RUN,
	WALKMODE_WALK,
};

typedef struct SPacketGCWalkMode
{
	BYTE	header;
	DWORD	vid;
	BYTE	mode;
} TPacketGCWalkMode;

typedef struct SPacketGCChangeSkillGroup
{
	BYTE        header;
	BYTE        skill_group;
} TPacketGCChangeSkillGroup;

typedef struct SPacketCGRefine
{
	uint8_t header;
	uint8_t pos;
	uint8_t type;
#ifdef ENABLE_FEATURES_REFINE_SYSTEM
	uint8_t lLow;
	uint8_t lMedium;
	uint8_t lExtra;
	uint8_t lTotal;
#endif
} TPacketCGRefine;

typedef struct SPacketCGRequestRefineInfo
{
	uint8_t header;
	uint8_t pos;
} TPacketCGRequestRefineInfo;

typedef struct SPacketGCRefineInformaion
{
	uint8_t header;
	uint8_t type;
	uint8_t pos;
	uint32_t src_vnum;
	uint32_t result_vnum;
	uint8_t material_count;
#if defined(ENABLE_NEW_GOLD_LIMIT)
	uint64_t cost;
#else
	uint32_t cost;
#endif
	uint8_t prob;
	TRefineMaterial materials[REFINE_MATERIAL_MAX_NUM];
} TPacketGCRefineInformation;

struct TNPCPosition
{
	BYTE bType;
#ifdef ENABLE_MULTI_NAMES
	DWORD	name;
#else
	char	name[CHARACTER_NAME_MAX_LEN+1];
#endif
	long x;
	long y;
};

typedef struct SPacketGCNPCPosition
{
	BYTE header;
	WORD size;
	WORD count;

	// array of TNPCPosition
} TPacketGCNPCPosition;

typedef struct SPacketGCSpecialEffect
{
	BYTE header;
	BYTE type;
	DWORD vid;
} TPacketGCSpecialEffect;

typedef struct SPacketCGChangeName
{
	BYTE header;
	BYTE index;
	char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketCGChangeName;

typedef struct SPacketGCChangeName
{
	BYTE header;
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCChangeName;


typedef struct command_client_version
{
	BYTE header;
	char filename[32+1];
	char timestamp[32+1];
} TPacketCGClientVersion;

typedef struct command_client_version2
{
	BYTE header;
	char filename[32+1];
	char timestamp[32+1];
} TPacketCGClientVersion2;

typedef struct packet_channel
{
	BYTE header;
	BYTE channel;
} TPacketGCChannel;

typedef struct SEquipmentItemSet
{
	uint16_t				pos;
	uint32_t				vnum;
	uint16_t				count;
	long					alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TEquipmentItemSet;

typedef struct packet_view_equip
{
	uint8_t				bHeader;
	uint32_t			dwVid;
	TEquipmentItemSet	tEquip[23];
} TPacketViewEquip;

typedef struct
{
	DWORD	dwID;
	long	x, y;
	long	width, height;
	DWORD	dwGuildID;
} TLandPacketElement;

typedef struct packet_land_list
{
	BYTE	header;
	WORD	size;
} TPacketGCLandList;

typedef struct
{
	BYTE	bHeader;
	long	lID;
	char	szName[32+1];
	DWORD	dwVID;
	BYTE	bType;
} TPacketGCTargetCreate;

typedef struct
{
	BYTE	bHeader;
	long	lID;
	long	lX, lY;
} TPacketGCTargetUpdate;

typedef struct
{
	BYTE	bHeader;
	long	lID;
} TPacketGCTargetDelete;

typedef struct
{
	BYTE		bHeader;
	TPacketAffectElement elem;
} TPacketGCAffectAdd;

typedef struct
{
	BYTE	bHeader;
	DWORD	dwType;
	BYTE	bApplyOn;
} TPacketGCAffectRemove;

typedef struct packet_lover_info
{
	BYTE header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	BYTE love_point;
} TPacketGCLoverInfo;

typedef struct packet_love_point_update
{
	BYTE header;
	BYTE love_point;
} TPacketGCLovePointUpdate;

// MINING
typedef struct packet_dig_motion
{
	BYTE header;
	DWORD vid;
	DWORD target_vid;
	BYTE count;
} TPacketGCDigMotion;
// END_OF_MINING

// SCRIPT_SELECT_ITEM
typedef struct command_script_select_item
{
	BYTE header;
	DWORD selection;
} TPacketCGScriptSelectItem;
// END_OF_SCRIPT_SELECT_ITEM

typedef struct packet_damage_info
{
	BYTE header;
	DWORD dwVID;
	BYTE flag;
	int damage;
} TPacketGCDamageInfo;

typedef struct SPacketGGCheckAwakeness
{
	BYTE bHeader;
} TPacketGGCheckAwakeness;

#if defined(ENABLE_OFFLINESHOP_REWORK)
typedef struct SPacketGGOfflineShopNotification
{
	BYTE		bHeader;
	int			ownerId;
	int			vnum;
	long long	price;
	WORD		count;
} TPacketGGOfflineShopNotification;
#endif

#ifdef _IMPROVED_PACKET_ENCRYPTION_
struct TPacketKeyAgreement
{
	static const int MAX_DATA_LEN = 512;
	BYTE bHeader;
	WORD wAgreedLength;
	WORD wDataLength;
	BYTE data[MAX_DATA_LEN];
};

struct TPacketKeyAgreementCompleted
{
	BYTE bHeader;
	BYTE data[3]; // dummy (not used)
};

#endif // _IMPROVED_PACKET_ENCRYPTION_

#define MAX_EFFECT_FILE_NAME 128
typedef struct SPacketGCSpecificEffect
{
	BYTE header;
	DWORD vid;
	char effect_file[MAX_EFFECT_FILE_NAME];
} TPacketGCSpecificEffect;

// 용혼석
enum EDragonSoulRefineWindowRefineType
{
	DragonSoulRefineWindow_UPGRADE,
	DragonSoulRefineWindow_IMPROVEMENT,
	DragonSoulRefineWindow_REFINE,
};

enum EPacketCGDragonSoulSubHeaderType
{
	DS_SUB_HEADER_OPEN,
	DS_SUB_HEADER_CLOSE,
	DS_SUB_HEADER_DO_REFINE_GRADE,
	DS_SUB_HEADER_DO_REFINE_STEP,
	DS_SUB_HEADER_DO_REFINE_STRENGTH,
	DS_SUB_HEADER_REFINE_FAIL,
	DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE,
	DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL,
	DS_SUB_HEADER_REFINE_SUCCEED,
};
typedef struct SPacketCGDragonSoulRefine
{
	SPacketCGDragonSoulRefine() : header (HEADER_CG_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos ItemGrid[DRAGON_SOUL_REFINE_GRID_SIZE];
} TPacketCGDragonSoulRefine;

#ifdef ENABLE_DS_REFINE_ALL
typedef struct SPacketDragonSoulRefineAll {
	uint8_t header, subheader, type, grade;
} TPacketDragonSoulRefineAll;
#endif

typedef struct SPacketGCDragonSoulRefine
{
	SPacketGCDragonSoulRefine() : header(HEADER_GC_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos Pos;
} TPacketGCDragonSoulRefine;

typedef struct SPacketCGStateCheck
{
	BYTE header;
	uint32_t key;
	uint32_t index;
} TPacketCGStateCheck;

typedef struct SPacketGCStateCheck
{
	BYTE header;
	uint32_t key;
	uint32_t index;
	unsigned char state;
} TPacketGCStateCheck;

#ifdef ENABLE_RANKING
enum
{
	MAX_RANKING_LIST = 51,
	HEADER_GC_RANKING_SEND = 218,
};

typedef struct SPacketGCList
{
	int		iPosition, iRealPosition, iLevel, iPoints;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCList;

typedef struct SPacketGCRankingTable {
	SPacketGCRankingTable() : bHeader(HEADER_GC_RANKING_SEND)
	{}
	BYTE 			bHeader;
	TPacketGCList	list[MAX_RANKING_LIST];
} TPacketGCRankingTable;
#endif

#ifdef __ENABLE_NEW_OFFLINESHOP__

//ACTIONS PACKETS
typedef struct
{
	uint8_t bHeader;
	int32_t wSize;
	uint8_t bSubHeader;
} TPacketGCNewOfflineshop;

typedef struct {
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketCGNewOfflineShop;

namespace offlineshop
{
	//common
	typedef struct SFilterInfo {
		BYTE		bType;
		BYTE		bSubType;

		char		szName[ITEM_NAME_MAX_LEN];
		TPriceInfo	priceStart, priceEnd;
		int			iLevelStart, iLevelEnd;

		DWORD		dwWearFlag;
		TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_NORM_NUM];
		int         iRarity = -1;
#if defined(ENABLE_OFFLINESHOP_REWORK)
		bool		bPlayerName;
#endif
	}TFilterInfo;


	typedef struct SShopItemInfo {
		TItemPos	pos;
		TPriceInfo	price;
	}TShopItemInfo;

	//AUCTION
	typedef struct SAuctionListElement {
		TAuctionInfo	auction;
		TPriceInfo		actual_best;
		DWORD			dwOfferCount;
	}TAuctionListElement;

	//offlineshop-updated 03/08/19
	typedef struct SMyOfferExtraInfo {
		TItemInfo item;
		char szShopName[OFFLINE_SHOP_NAME_MAX_LEN];
	} TMyOfferExtraInfo;


	//GAME TO CLIENT
	enum eSubHeaderGC
	{
		SUBHEADER_GC_SHOP_LIST,
		SUBHEADER_GC_SHOP_OPEN,
		SUBHEADER_GC_SHOP_OPEN_OWNER,
		SUBHEADER_GC_SHOP_OPEN_OWNER_NO_SHOP,
		SUBHEADER_GC_SHOP_CLOSE,
		SUBHEADER_GC_SHOP_BUY_ITEM_FROM_SEARCH,

		SUBHEADER_GC_OFFER_LIST,

		SUBHEADER_GC_SHOP_FILTER_RESULT,
		SUBHEADER_GC_SHOP_SAFEBOX_REFRESH,

		//AUCTION
		SUBHEADER_GC_AUCTION_LIST,
		SUBHEADER_GC_OPEN_MY_AUCTION,
		SUBHEADER_GC_OPEN_MY_AUCTION_NO_AUCTION,
		SUBHEADER_GC_OPEN_AUCTION,
#ifdef ENABLE_NEW_SHOP_IN_CITIES
		SUBHEADER_GC_INSERT_SHOP_ENTITY,
		SUBHEADER_GC_REMOVE_SHOP_ENTITY,
#endif
#if defined(ENABLE_OFFLINESHOP_REWORK)
		SUBHEADER_GC_NOTIFICATION,
#endif
	};


	typedef struct SSubPacketGCShopList {
		DWORD	dwShopCount;
	} TSubPacketGCShopList;


	typedef struct SSubPacketGCShopOpen {
		TShopInfo	shop;

	} TSubPacketGCShopOpen;


	typedef struct SSubPacketGCShopOpenOwner {
		TShopInfo	shop;
		DWORD		dwSoldCount;
		DWORD		dwOfferCount;

	} TSubPacketGCShopOpenOwner;



	typedef struct SSubPacketGCShopBuyItemFromSearch {
		DWORD dwOwnerID;
		DWORD dwItemID;
	}TSubPacketGCShopBuyItemFromSearch;


	typedef struct SSubPacketGCShopFilterResult {
		DWORD dwCount;
	} TSubPacketGCShopFilterResult;


	typedef struct SSubPacketGCShopOfferList {
		DWORD dwOfferCount;

	} TSubPacketGCShopOfferList;



	typedef struct SSubPacketGCShopSafeboxRefresh {
		TValutesInfo	valute;
		DWORD			dwItemCount;

	}TSubPacketGCShopSafeboxRefresh;

	//AUCTION
	typedef struct SSubPacketGCAuctionList {
		DWORD dwCount;
		bool bOwner;
	}TSubPacketGCAuctionList;



	typedef struct SSubPacketGCAuctionOpen {
		TAuctionInfo auction;
		DWORD dwOfferCount;


	}TSubPacketGCAuctionOpen;
#ifdef ENABLE_NEW_SHOP_IN_CITIES
	typedef struct SSubPacketGCInsertShopEntity {
		DWORD	dwVID;
		char	szName[OFFLINE_SHOP_NAME_MAX_LEN];
		int		iType;

		long 	x, y, z;
	} TSubPacketGCInsertShopEntity;


	typedef struct SSubPacketGCRemoveShopEntity {
		DWORD dwVID;
	} TSubPacketGCRemoveShopEntity;
#endif

#if defined(ENABLE_OFFLINESHOP_REWORK)
	typedef struct {
		DWORD dwItemID;
		long long dwItemPrice;
		WORD dwItemCount;
	} TSubPacketGCNotification;
	
	typedef struct
	{
		DWORD dwTime;
	}TSubPacketCGShopExtendTime;
#endif

	// CLIENT TO GAME
	enum eSubHeaderCG
	{
		SUBHEADER_CG_SHOP_CREATE_NEW,
		SUBHEADER_CG_SHOP_CHANGE_NAME,
#if defined(ENABLE_OFFLINESHOP_REWORK)
		SUBHEADER_CG_SHOP_EXTEND_TIME,
#endif
		SUBHEADER_CG_SHOP_FORCE_CLOSE,
		SUBHEADER_CG_SHOP_REQUEST_SHOPLIST,
		SUBHEADER_CG_SHOP_OPEN,
		SUBHEADER_CG_SHOP_OPEN_OWNER,
		SUBHEADER_CG_SHOP_BUY_ITEM,

		SUBHEADER_CG_SHOP_ADD_ITEM,
		SUBHEADER_CG_SHOP_REMOVE_ITEM,
		SUBHEADER_CG_SHOP_EDIT_ITEM,

		SUBHEADER_CG_SHOP_FILTER_REQUEST,

		SUBHEADER_CG_SHOP_OFFER_CREATE,
		SUBHEADER_CG_SHOP_OFFER_ACCEPT,
		SUBHEADER_CG_SHOP_OFFER_CANCEL,
		SUBHEADER_CG_SHOP_REQUEST_OFFER_LIST,

		SUBHEADER_CG_SHOP_SAFEBOX_OPEN,
		SUBHEADER_CG_SHOP_SAFEBOX_GET_ITEM,
		SUBHEADER_CG_SHOP_SAFEBOX_GET_VALUTES,
		SUBHEADER_CG_SHOP_SAFEBOX_CLOSE,

		//AUCTION
		SUBHEADER_CG_AUCTION_LIST_REQUEST,
		SUBHEADER_CG_AUCTION_OPEN_REQUEST,
		SUBHEADER_CG_MY_AUCTION_OPEN_REQUEST,
		SUBHEADER_CG_CREATE_AUCTION,
		SUBHEADER_CG_AUCTION_ADD_OFFER,
		SUBHEADER_CG_EXIT_FROM_AUCTION,

		SUBHEADER_CG_CLOSE_BOARD,
#ifdef ENABLE_NEW_SHOP_IN_CITIES
		SUBHEADER_CG_CLICK_ENTITY,
#endif
		SUBHEADER_CG_AUCTION_CLOSE,
	};




	typedef struct SSubPacketCGShopCreate
	{
		TShopInfo shop;
	}TSubPacketCGShopCreate;


	typedef struct SSubPacketCGShopChangeName
	{
		char szName[OFFLINE_SHOP_NAME_MAX_LEN];
	}TSubPacketCGShopChangeName;



	typedef struct SSubPacketCGShopOpen
	{
		DWORD dwOwnerID;
	}TSubPacketCGShopOpen;



	typedef struct SSubPacketCGAddItem
	{
		TItemPos	pos;
		TPriceInfo  price;
	}TSubPacketCGAddItem;


	typedef struct SSubPacketCGRemoveItem
	{
		DWORD dwItemID;
	}TSubPacketCGRemoveItem;



	typedef struct SSubPacketCGEditItem
	{
		DWORD dwItemID;
		TPriceInfo price;
	}TSubPacketCGEditItem;



	typedef struct SSubPacketCGFilterRequest
	{
		TFilterInfo filter;
	}TSubPacketCGFilterRequest;



	typedef struct SSubPacketCGOfferCreate
	{
		TOfferInfo offer;
	}TSubPacketCGOfferCreate;


	typedef struct SSubPacketCGOfferAccept
	{
		DWORD dwOfferID;
	}TSubPacketCGOfferAccept;



	typedef struct SSubPacketCGOfferCancel
	{
		DWORD dwOfferID;
		DWORD dwOwnerID;
	}TSubPacketCGOfferCancel;


	typedef struct SSubPacketCGShopSafeboxGetItem
	{
		DWORD dwItemID;
	}TSubPacketCGShopSafeboxGetItem;


	typedef struct SSubPacketCGShopSafeboxGetValutes
	{
		TValutesInfo valutes;
	}TSubPacketCGShopSafeboxGetValutes;

	typedef struct SSubPacketCGShopBuyItem
	{
		DWORD dwOwnerID;
		DWORD dwItemID;
		bool  bIsSearch;
		long long TotalPriceSeen;
	}TSubPacketCGShopBuyItem;

	//AUCTION
	typedef struct SSubPacketCGAuctionOpenRequest {
		DWORD dwOwnerID;
	} TSubPacketCGAuctionOpenRequest;

	typedef struct SSubPacketCGAuctionCreate {
		DWORD		dwDuration;
		TItemPos	pos;

		TPriceInfo	init_price;
	} TSubPacketCGAuctionCreate;


	typedef struct SSubPacketCGAuctionAddOffer {
		DWORD		dwOwnerID;
		TPriceInfo	price;
	}TSubPacketCGAuctionAddOffer;

	typedef struct SSubPacketCGAuctionExitFrom {
		DWORD dwOwnerID;
	} TSubPacketCGAuctionExitFrom;



#ifdef ENABLE_NEW_SHOP_IN_CITIES
	typedef struct SSubPacketCGShopClickEntity {
		DWORD dwShopVID;
	} TSubPacketCGShopClickEntity;
#endif

}


#endif




#ifdef ENABLE_ACCE_SYSTEM
enum
{
	HEADER_CG_ACCE = 211,
	HEADER_GC_ACCE = 215,
	ACCE_SUBHEADER_GC_OPEN = 0,
	ACCE_SUBHEADER_GC_CLOSE,
	ACCE_SUBHEADER_GC_ADDED,
	ACCE_SUBHEADER_GC_REMOVED,
	ACCE_SUBHEADER_CG_REFINED,
	ACCE_SUBHEADER_CG_CLOSE = 0,
	ACCE_SUBHEADER_CG_ADD,
	ACCE_SUBHEADER_CG_REMOVE,
	ACCE_SUBHEADER_CG_REFINE,
};

typedef struct SPacketAcce
{
	BYTE	header;
	BYTE	subheader;
	bool	bWindow;
	DWORD	dwPrice;
	BYTE	bPos;
	TItemPos	tPos;
	DWORD	dwItemVnum;
	DWORD	dwMinAbs;
	DWORD	dwMaxAbs;
} TPacketAcce;
#endif

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD

enum
{
	CUBE_RENEWAL_SUB_HEADER_OPEN_RECEIVE,
	CUBE_RENEWAL_SUB_HEADER_CLEAR_DATES_RECEIVE,
	CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE,
	CUBE_RENEWAL_SUB_HEADER_DATES_LOADING,

	CUBE_RENEWAL_SUB_HEADER_MAKE_ITEM,
	CUBE_RENEWAL_SUB_HEADER_CLOSE,
};

typedef struct  packet_send_cube_renewal
{
	BYTE header;
	BYTE subheader;
	DWORD index_item;
	DWORD count_item;
	DWORD index_item_improve;
}TPacketCGCubeRenewalSend;


typedef struct dates_cube_renewal
{
	DWORD npc_vnum;
	DWORD index;

	DWORD vnum_reward;
	int count_reward;

	bool item_reward_stackable;

	DWORD vnum_material_1;
	int count_material_1;

	DWORD vnum_material_2;
	int count_material_2;

	DWORD vnum_material_3;
	int count_material_3;

	DWORD vnum_material_4;
	int count_material_4;

	DWORD vnum_material_5;
	int count_material_5;

	unsigned long long gold;
	int percent;
#if defined(ENABLE_GAYA_RENEWAL)
	int gaya;
#endif
#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
	DWORD allowCopy;
#endif

	char category[100];
}TInfoDateCubeRenewal;

typedef struct packet_receive_cube_renewal
{
	packet_receive_cube_renewal(): header(HEADER_GC_CUBE_RENEWAL)
	{}

	BYTE header;
	BYTE subheader;
	TInfoDateCubeRenewal date_cube_renewal;
}TPacketGCCubeRenewalReceive;
#endif

#ifdef __SKILL_COLOR_SYSTEM__
typedef struct packet_skill_color
{
	BYTE		bheader;
	BYTE		skill;
	DWORD		col1;
	DWORD		col2;
	DWORD		col3;
	DWORD		col4;
	DWORD		col5;
} TPacketCGSkillColor;
#endif

#ifdef ENABLE_ATLAS_BOSS
typedef struct SPacketGCBossPosition
{
	BYTE	bHeader;
	WORD	wSize;
	WORD	wCount;
} TPacketGCBossPosition;

struct TBossPosition
{
	BYTE	bType;
#ifdef ENABLE_MULTI_NAMES
	DWORD	szName;
#else
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
#endif
	long	lX;
	long	lY;
	long	lTime;
};
#endif

#if defined(ENABLE_TEXTS_RENEWAL)
typedef struct SPacketGCChatNew {
	BYTE header;
	WORD size;
	BYTE type;
	DWORD idx;
} TPacketGCChatNew;

typedef struct SPacketGGChatNew {
	BYTE header;
	BYTE type;
	BYTE empire;
	long mapidx;
	DWORD idx;
	WORD size;
} TPacketGGChatNew;
#endif

#ifdef ENABLE_NEW_FISHING_SYSTEM
enum {
	FISHING_SUBHEADER_NEW_START = 0,
	FISHING_SUBHEADER_NEW_STOP = 1,
	FISHING_SUBHEADER_NEW_CATCH = 2,
	FISHING_SUBHEADER_NEW_CATCH_FAIL = 3,
	FISHING_SUBHEADER_NEW_CATCH_SUCCESS = 4,
	FISHING_SUBHEADER_NEW_CATCH_FAILED = 5,
};

typedef struct SPacketFishingNew
{
	BYTE header;
	BYTE subheader;
	DWORD vid;
	int dir;
	BYTE need;
	BYTE count;
} TPacketFishingNew;
#endif

#ifdef ENABLE_OPENSHOP_PACKET
typedef struct SPacketOpenShop {
	uint8_t header;
	int32_t shopid;
} TPacketOpenShop;
#endif

#if defined(ENABLE_EVENT_MANAGER)
typedef struct SPacketGCEventManager
{
	uint8_t header;
	int32_t size;
} TPacketGCEventManager;
#endif

#if defined(ENABLE_INGAME_ITEMSHOP)
typedef struct SPacketGCItemShop
{
	BYTE	header;
	DWORD	size;
} TPacketGCItemShop;
#endif

#ifdef ENABLE_REWARD_SYSTEM
typedef struct SPacketGGRewardInfo
{
	BYTE	bHeader;
	BYTE	rewardIndex;
	char	playerName[CHARACTER_NAME_MAX_LEN+1];
} TPacketGGRewardInfo;
#endif

#if defined(USE_BATTLEPASS)
typedef struct SPacketCGExtBattlePassAction
{
	BYTE bHeader;
	BYTE bAction;
} TPacketCGExtBattlePassAction;

typedef struct SPacketCGExtBattlePassSendPremiumItem
{
	BYTE bHeader;
	int iSlotIndex;
} TPacketCGExtBattlePassSendPremiumItem;

typedef struct SPacketGCExtBattlePassOpen
{
	BYTE bHeader;
} TPacketGCExtBattlePassOpen;

typedef struct SPacketGCExtBattlePassGeneralInfo
{
	BYTE bHeader;
	char szSeasonName[64+1];
	DWORD dwBattlePassID;
	DWORD dwBattlePassStartTime;
	DWORD dwBattlePassEndTime;
} TPacketGCExtBattlePassGeneralInfo;

typedef struct SPacketGCExtBattlePassMissionInfo
{
	BYTE bHeader;
	WORD wSize;
	WORD wRewardSize;
	DWORD dwBattlePassID;
} TPacketGCExtBattlePassMissionInfo;

typedef struct SPacketGCExtBattlePassMissionUpdate
{
	BYTE bHeader;
	BYTE bMissionIndex;
	BYTE bMissionType;
	DWORD dwNewProgress;
} TPacketGCExtBattlePassMissionUpdate;

typedef struct SPacketGCExtBattlePassRanking
{
	BYTE bHeader;
	char szPlayerName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE bBattlePassID;
	DWORD dwStartTime;
	DWORD dwEndTime;
} TPacketGCExtBattlePassRanking;
#endif

#if defined(ENABLE_GAYA_RENEWAL)
typedef struct command_gem_shop
{
	BYTE		header;
	BYTE		subheader;
} TPacketCGGemShop;

typedef struct command_gem_shop_open
{
	BYTE	header;
	int     nextRefreshTime;
	TGemShopItem shopItems[GEM_SLOTS_MAX_NUM];
} TPacketGCGemShopOpen;

typedef struct command_gem_shop_refresh
{
	BYTE	header;
	int		nextRefreshTime;
	TGemShopItem shopItems[GEM_SLOTS_MAX_NUM];
} TPacketGCGemShopRefresh;

enum
{
	GEM_SHOP_SUBHEADER_CG_BUY,
	GEM_SHOP_SUBHEADER_CG_ADD,
	GEM_SHOP_SUBHEADER_CG_REFRESH,
};
#endif

#ifdef ENABLE_SWITCHBOT_WORLDARD
enum
{
	SWITCH_SUB_HEADER_CHANGE,
	SWITCH_SUB_HEADER_CLOSE,
	SWITCH_SUB_HEADER_OPEN,

	SWITCH_RECEIVE_OPEN,
	SWITCH_RECEIVE_CLEAR_INFO,
	SWITCH_RECEIVE_INFO_BONUS_ITEM,
	SWITCH_RECEIVE_INFO_EXTRA,
};

typedef struct packet_send_switchbot
{
	BYTE header;
	BYTE subheader;
	TPlayerItemAttribute aAttr[5];

	int vnum_change;

} TPacketCGSwitchBotSend;

typedef struct SPacketGCSwitchBotReceive
{
	SPacketGCSwitchBotReceive() : header(HEADER_GC_SWITCHBOT)
	{}
	BYTE header;
	BYTE bSubType;
	int  id_bonus;
	int  value_bonus_0;
	int  value_bonus_1;
	int  value_bonus_2;
	int  value_bonus_3;
	int  value_bonus_4;

} TPacketGCSwitchBotReceive;
#endif

#if defined(ENABLE_EXCHANGE_FRAGMENTS)
typedef struct SPacketExchangeFragments
{
	BYTE bHeader;
	TItemPos itemPos;
} TPacketExchangeFragments;
#endif

#ifdef USE_CAPTCHA_SYSTEM
typedef struct SPacketGCCaptcha
{
    uint8_t     bHeader;
    uint16_t    code;
    uint8_t     limit;
} TPacketGCCaptcha;

typedef struct SPacketCGCaptcha
{
    uint8_t     bHeader;
    uint16_t    code;
} TPacketCGCaptcha;
#endif

#if defined(ENABLE_NEW_ANTICHEAT_RULES)
enum EAntiCheatSubHeaders {
	ANTICHEAT_TYPE_ATTACKSPEED,
	ANTICHEAT_TYPE_RANGE,
};

typedef struct SPacketGCAntiCheat {
	uint8_t bHeader;
	uint8_t bReason;
} TPacketGCAntiCheat;
#endif

typedef struct SPacketEmpty
{
	uint8_t bHeader;
} TPacketEmpty;

#if defined(ENABLE_NEW_FISH_EVENT)
enum {
	FISH_EVENT_SUBHEADER_BOX_USE,
	FISH_EVENT_SUBHEADER_SHAPE_ADD,
	FISH_EVENT_SUBHEADER_GC_REWARD,
	FISH_EVENT_SUBHEADER_GC_ENABLE,
};

typedef struct SPacketGCFishEvent {
	uint8_t bHeader;
	uint8_t bSubheader;
} TPacketCGFishEvent;

typedef struct SPacketGCFishEventInfo {
	uint8_t bHeader;
	uint8_t bSubheader;
	uint32_t dwFirstArg;
	uint32_t dwSecondArg;
} TPacketGCFishEventInfo;
#endif

#if defined(ENABLE_MESSENGER_BLOCK)
typedef struct SPacketGGMessengerBlock {
	uint8_t bHeader;
	uint8_t bAdd;
	char szAccount[CHARACTER_NAME_MAX_LEN + 1];
	char szCompanion[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGMessengerBlock;

typedef struct SPacketGCMessengerBlockList {
	uint8_t connected;
	uint8_t length;
} TPacketGCMessengerBlockList;

typedef struct command_messenger_add_block_by_vid {
	uint32_t vid;
} TPacketCGMessengerAddBlockByVID;

typedef struct command_messenger_add_block_by_name {
	uint8_t length;
} TPacketCGMessengerAddBlockByName;

typedef struct command_messenger_remove_block {
	char szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGMessengerRemoveBlock;
#endif

#if defined(ENABLE_AURA_SYSTEM)
typedef struct SItemData {
	uint32_t vnum;
	WORD count;
	uint32_t flags;
	uint32_t anti_flags;
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#if defined(ENABLE_CHANGELOOK)
	uint32_t transmutation;
#endif
#ifdef ATTR_LOCK
	short lockedattr;
#endif
} TItemData;

enum EPacketGCAuraSubHeader {
	AURA_SUBHEADER_GC_OPEN,
	AURA_SUBHEADER_GC_CLOSE,
	AURA_SUBHEADER_GC_SET_ITEM,
	AURA_SUBHEADER_GC_CLEAR_SLOT,
	AURA_SUBHEADER_GC_CLEAR_ALL,
	AURA_SUBHEADER_GC_CLEAR_RIGHT,
	AURA_SUBHEADER_GC_REFINE_INFO,
};

typedef struct SSubPacketGCAuraOpenClose {
	uint8_t bAuraWindowType;
} TSubPacketGCAuraOpenClose;

typedef struct SSubPacketGCAuraSetItem {
	TItemPos Cell;
	TItemPos AuraCell;
	TItemData pItem;
} TSubPacketGCAuraSetItem;

typedef struct SSubPacketGCAuraClearSlot {
	uint8_t bAuraSlotPos;
} TSubPacketGCAuraClearSlot;

typedef struct SSubPacketGCAuraRefineInfo {
	uint8_t bAuraRefineInfoType;
	uint8_t bAuraRefineInfoLevel;
	uint8_t bAuraRefineInfoExpPercent;
} TSubPacketGCAuraRefineInfo;

enum EPacketCGAuraSubHeader {
	AURA_SUBHEADER_CG_REFINE_CHECKIN,
	AURA_SUBHEADER_CG_REFINE_CHECKOUT,
	AURA_SUBHEADER_CG_REFINE_ACCEPT,
	AURA_SUBHEADER_CG_REFINE_CANCEL,
};

typedef struct SSubPacketCGAuraRefineCheckIn {
	TItemPos ItemCell;
	TItemPos AuraCell;
	uint8_t byAuraRefineWindowType;
} TSubPacketCGAuraRefineCheckIn;

typedef struct SSubPacketCGAuraRefineCheckOut {
	TItemPos AuraCell;
	uint8_t byAuraRefineWindowType;
} TSubPacketCGAuraRefineCheckOut;

typedef struct SSubPacketCGAuraRefineAccept {
	uint8_t byAuraRefineWindowType;
} TSubPacketCGAuraRefineAccept;

typedef struct SPacketGCAura {
	SPacketGCAura() : bHeader(HEADER_GC_AURA) {}
	uint8_t bHeader;
	WORD wSize;
	uint8_t bSubHeader;
} TPacketGCAura;

typedef struct SPacketCGAura {
	SPacketCGAura() : bHeader(HEADER_CG_AURA) {}
	uint8_t bHeader;
	WORD wSize;
	uint8_t bSubHeader;
} TPacketCGAura;
#endif

#if defined(ENABLE_CHANGELOOK)
enum EChangeLookSubHeader {
	SUBHEADER_CHANGE_LOOK_OPEN = 0,
	SUBHEADER_CHANGE_LOOK_CLOSE,
	SUBHEADER_CHANGE_LOOK_ADD,
	SUBHEADER_CHANGE_LOOK_REMOVE,
	SUBHEADER_CHANGE_LOOK_REFINE,
};

typedef struct SPacketChangeLook {
	uint8_t bHeader;
	uint8_t bSubHeader;
	int64_t dwCost;
	uint8_t bPos;
	TItemPos tPos;
} TPacketChangeLook;
#endif

#if defined(ENABLE_CLOSE_GAMECLIENT_CMD)
typedef struct SPacketGGCloseClient {
	uint8_t bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGCloseClient;
#endif

#if defined(ENABLE_NEW_GOLD_LIMIT)
typedef struct packet_gold
{
	uint8_t header;
	uint64_t gold;
} TPacketGCGold;

typedef struct packet_gold_change
{
	uint8_t header;
	uint32_t dwVID;
	int64_t amount;
	uint64_t value;
} TPacketGCGoldChange;
#endif

typedef struct SPacketQuickSlotAdd
{
	uint8_t header;
	uint8_t pos;
	TQuickSlot slot;
} TPacketQuickSlotAdd;

typedef struct SPacketCGQuickSlotDel
{
	uint8_t header;
	uint8_t pos;
} TPacketQuickSlotDel;

typedef struct SPacketCGQuickSlotSwap
{
	uint8_t header;
	uint8_t pos;
	uint8_t change_pos;
} TPacketQuickSlotSwap;

#if defined(USE_ATTR_6TH_7TH)
enum EAttr67AddSubHeader
{
	SUBHEADER_CG_ATTR67_ADD_CLOSE,
	SUBHEADER_CG_ATTR67_ADD_OPEN,
	SUBHEADER_CG_ATTR67_ADD_REGIST,
};

typedef struct SPacketCGAttr67Add
{
	uint8_t byHeader;
	uint8_t bySubHeader;
	TAttr67AddData Attr67AddData;
} TPacketCGAttr67Add;
#endif
#ifdef __ENABLE_PREMIUM_PLAYERS__
enum PremiumPlayersHeader
{
	HEADER_CG_PREMIUM_PLAYERS = 177,
	HEADER_GC_PREMIUM_PLAYERS = 142,

	PREMIUM_PLAYERS_SUBHEADER_CG_OPEN = 0,
	PREMIUM_PLAYERS_SUBHEADER_CG_LIST = 1,
	PREMIUM_PLAYERS_SUBHEADER_CG_ACTIVATE = 2,

	PREMIUM_PLAYERS_SUBHEADER_GC_OPEN = 0,
	PREMIUM_PLAYERS_SUBHEADER_GC_LIST = 1,
	PREMIUM_PLAYERS_SUBHEADER_GC_ACTIVATE = 2,
};

typedef struct SPacketCGPremiumPlayers
{
	SPacketCGPremiumPlayers() : 
		byHeader(HEADER_CG_PREMIUM_PLAYERS)
	{}
	BYTE byHeader;
	BYTE bySubHeader;
} TPacketCGPremiumPlayers;

typedef struct SPacketGCPremiumPlayers
{
	SPacketGCPremiumPlayers() :
		byHeader(HEADER_GC_PREMIUM_PLAYERS)
	{}
	BYTE byHeader;
	BYTE bySubHeader;
	BYTE byPos;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCPremiumPlayers;
#endif

#ifdef USE_MULTIPLE_OPENING
typedef struct SPacketCGMultipleOpening
{
    uint8_t     header;
    TItemPos    pos;
    uint16_t    count;
} TPacketCGMultipleOpening;
#endif

struct SBossPosition
{
	uint32_t vnum;
	int32_t x, y;
	uint32_t time;
	uint32_t real_time;
	bool is_alive;
	SBossPosition() = default;
	SBossPosition(const uint32_t vnum, const int32_t x, const int32_t y, const uint32_t time) : vnum(vnum), x(x), y(y),
		time(time),
		real_time(0),
		is_alive(false)
	{
	}

	bool operator==(const SBossPosition& rhs) const
	{
		return vnum == rhs.vnum && x == rhs.x && y == rhs.y && time == rhs.time;
	}

	bool operator!=(const SBossPosition& rhs) const
	{
		return !(*this == rhs);
	}
};

struct SPacketGCBossPosition
{
	uint8_t header;
	uint16_t size;
};

typedef struct SPacketCGChangeCountryFlag
{
	BYTE header;
	BYTE index;
	char country_flag[3 + 1];
} TPacketCGChangeCountryFlag;

typedef struct SPacketGCChangeCountryFlag
{
	BYTE header;
	BYTE index;
	char country_flag[3 + 1];
} TPacketGCChangeCountryFlag;

struct SPacketCGWhisperInfo
{
	uint8_t	header;
	char name[CHARACTER_NAME_MAX_LEN + 1];

	SPacketCGWhisperInfo() : header(HEADER_CG_WHISPER_INFO), name{}
	{
	}
};

struct SPacketGCWhisperInfo
{
	uint8_t	header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	char country_flag[3 + 1];
	uint8_t empire;

	SPacketGCWhisperInfo() : header(HEADER_GC_WHISPER_INFO), name{}, country_flag{}, empire(0)
	{
	}
};

#pragma pack()
#endif
