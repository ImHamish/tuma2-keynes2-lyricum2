#include "stdafx.h"
#include "../common/stl.h"
#include "constants.h"
#include "packet_info.h"
#ifdef __INGAME_WIKI__
#include "../common/in_game_wiki.h"
#endif

CPacketInfo::CPacketInfo()
	: m_pCurrentPacket(NULL), m_dwStartTime(0)
{
}

CPacketInfo::~CPacketInfo()
{
	auto it = m_pPacketMap.begin();
	for ( ; it != m_pPacketMap.end(); ++it) {
		M2_DELETE(it->second);
	}
}

void CPacketInfo::Set(int header, int iSize, const char* c_pszName, bool bSeq)
{
	if (m_pPacketMap.find(header) != m_pPacketMap.end())
	{
		fprintf(stderr, "The header(%d) is already registered.\n", header);
		abort();
	}

	TPacketElement * element = M2_NEW TPacketElement;

	element->iSize = iSize;
	element->stName.assign(c_pszName);
	element->iCalled = 0;
	element->dwLoad = 0;
#if defined(ENABLE_SEQUENCE)
	element->bSequencePacket = bSeq;

	if (element->bSequencePacket)
		element->iSize += sizeof(BYTE);
#endif

	m_pPacketMap.insert(std::map<int, TPacketElement *>::value_type(header, element));
}

bool CPacketInfo::Get(int header, int * size, const char ** c_ppszName)
{
	std::map<int, TPacketElement *>::iterator it = m_pPacketMap.find(header);

	if (it == m_pPacketMap.end())
		return false;

	*size = it->second->iSize;
	*c_ppszName = it->second->stName.c_str();

	m_pCurrentPacket = it->second;
	return true;
}

#if defined(ENABLE_SEQUENCE)
bool CPacketInfo::IsSequence(int header)
{
	TPacketElement * pkElement = GetElement(header);
	return pkElement ? pkElement->bSequencePacket : false;
}

void CPacketInfo::SetSequence(int header, bool bSeq)
{
	TPacketElement * pkElem = GetElement(header);

	if (pkElem)
	{
		if (bSeq)
		{
			if (!pkElem->bSequencePacket)
				pkElem->iSize++;
		}
		else
		{
			if (pkElem->bSequencePacket)
				pkElem->iSize--;
		}

		pkElem->bSequencePacket = bSeq;
	}
}
#endif

TPacketElement * CPacketInfo::GetElement(int header)
{
	std::map<int, TPacketElement *>::iterator it = m_pPacketMap.find(header);

	if (it == m_pPacketMap.end())
		return NULL;

	return it->second;
}

void CPacketInfo::Start()
{
	assert(m_pCurrentPacket != NULL);
	m_dwStartTime = get_dword_time();
}

void CPacketInfo::End()
{
	++m_pCurrentPacket->iCalled;
	m_pCurrentPacket->dwLoad += get_dword_time() - m_dwStartTime;
}

void CPacketInfo::Log(const char * c_pszFileName)
{
	FILE * fp;

	fp = fopen(c_pszFileName, "w");

	if (!fp)
		return;

	std::map<int, TPacketElement *>::iterator it = m_pPacketMap.begin();

	fprintf(fp, "Name             Called     Load       Ratio\n");

	while (it != m_pPacketMap.end())
	{
		TPacketElement * p = it->second;
		++it;

		fprintf(fp, "%-16s %-10d %-10u %.2f\n",
				p->stName.c_str(),
				p->iCalled,
				p->dwLoad,
				p->iCalled != 0 ? (float) p->dwLoad / p->iCalled : 0.0f);
	}

	fclose(fp);
}
///---------------------------------------------------------

CPacketInfoCG::CPacketInfoCG()
{
#if defined(ENABLE_CHANGELOOK)
	Set(HEADER_CG_CHANGE_LOOK, sizeof(TPacketChangeLook), "ChangeLook", true);
#endif
#if defined(ENABLE_AURA_SYSTEM)
	Set(HEADER_CG_AURA, sizeof(TPacketCGAura), "Aura", true);
#endif
#if defined(ENABLE_LETTERS_EVENT)
	Set(HEADER_CG_TAKE_LETTERS_REWARD, sizeof(TPacketEmpty), "LettersReward", true);
#endif
#if defined(ENABLE_NEW_FISH_EVENT)
	Set(HEADER_CG_FISH_EVENT_SEND, sizeof(TPacketCGFishEvent), "FishEvent", true);
#endif
#ifdef USE_CAPTCHA_SYSTEM
	Set(HEADER_CG_CAPTCHA, sizeof(TPacketCGCaptcha), "Captcha", true);
#endif
#if defined(USE_ATTR_6TH_7TH)
	Set(HEADER_CG_ATTR67_ADD, sizeof(TPacketCGAttr67Add), "Attr67Add", true);
#endif
#if defined(ENABLE_EXCHANGE_FRAGMENTS)
	Set(HEADER_CG_EXCHANGE_FRAGMENTS, sizeof(TPacketExchangeFragments), "ExchangeFragments", true);
#endif
#if defined(USE_BATTLEPASS)
	Set(HEADER_CG_EXT_BATTLE_PASS_ACTION, sizeof(TPacketCGExtBattlePassAction), "ReciveExtBattlePassActions", true);
	Set(HEADER_CG_EXT_SEND_BP_PREMIUM_ITEM, sizeof(TPacketCGExtBattlePassSendPremiumItem), "ReciveExtBattlePassPremiumItem", true);
#endif
	Set(HEADER_CG_TEXT, sizeof(TPacketCGText), "Text", false);
	Set(HEADER_CG_HANDSHAKE, sizeof(TPacketCGHandshake), "Handshake", false);
	Set(HEADER_CG_TIME_SYNC, sizeof(TPacketCGHandshake), "TimeSync", true);
	Set(HEADER_CG_MARK_LOGIN, sizeof(TPacketCGMarkLogin), "MarkLogin", false);
	Set(HEADER_CG_MARK_IDXLIST, sizeof(TPacketCGMarkIDXList), "MarkIdxList", false);
	Set(HEADER_CG_MARK_CRCLIST, sizeof(TPacketCGMarkCRCList), "MarkCrcList", false);
	Set(HEADER_CG_MARK_UPLOAD, sizeof(TPacketCGMarkUpload), "MarkUpload", false);
#ifdef _IMPROVED_PACKET_ENCRYPTION_
	Set(HEADER_CG_KEY_AGREEMENT, sizeof(TPacketKeyAgreement), "KeyAgreement", false);
#endif

	Set(HEADER_CG_GUILD_SYMBOL_UPLOAD, sizeof(TPacketCGGuildSymbolUpload), "SymbolUpload", false);
	Set(HEADER_CG_SYMBOL_CRC, sizeof(TPacketCGSymbolCRC), "SymbolCRC", false);
	Set(HEADER_CG_LOGIN, sizeof(TPacketCGLogin), "Login", true);
	Set(HEADER_CG_LOGIN2, sizeof(TPacketCGLogin2), "Login2", true);
	Set(HEADER_CG_LOGIN3, sizeof(TPacketCGLogin3), "Login3", true);
	Set(HEADER_CG_ATTACK, sizeof(TPacketCGAttack), "Attack", true);
	Set(HEADER_CG_CHAT, sizeof(TPacketCGChat), "Chat", true);
	Set(HEADER_CG_WHISPER, sizeof(TPacketCGWhisper), "Whisper", true);

	Set(HEADER_CG_CHARACTER_SELECT, sizeof(TPacketCGPlayerSelect), "Select", true);
	Set(HEADER_CG_CHARACTER_CREATE, sizeof(TPacketCGPlayerCreate), "Create", true);
	Set(HEADER_CG_CHARACTER_DELETE, sizeof(TPacketCGPlayerDelete), "Delete", true);
	Set(HEADER_CG_ENTERGAME, sizeof(TPacketCGEnterGame), "EnterGame", true);

	Set(HEADER_CG_ITEM_USE, sizeof(TPacketCGItemUse), "ItemUse", true);
	Set(HEADER_CG_ITEM_DROP, sizeof(TPacketCGItemDrop), "ItemDrop", true);
	Set(HEADER_CG_ITEM_DROP2, sizeof(TPacketCGItemDrop2), "ItemDrop2", true);
	Set(HEADER_CG_ITEM_DESTROY, sizeof(TPacketCGItemDestroy), "ItemDestroy", true);
	Set(HEADER_CG_ITEM_DIVISION, sizeof(TPacketCGItemDivision), "ItemDivision", true);
	Set(HEADER_CG_ITEM_MOVE, sizeof(TPacketCGItemMove), "ItemMove", true);
	Set(HEADER_CG_ITEM_PICKUP, sizeof(TPacketCGItemPickup), "ItemPickup", true);

	Set(HEADER_CG_QUICKSLOT_ADD, sizeof(TPacketQuickSlotAdd), "QuickslotAdd", true);
	Set(HEADER_CG_QUICKSLOT_DEL, sizeof(TPacketQuickSlotDel), "QuickslotDel", true);
	Set(HEADER_CG_QUICKSLOT_SWAP, sizeof(TPacketQuickSlotSwap), "QuickslotSwap", true);

	Set(HEADER_CG_SHOP, sizeof(TPacketCGShop), "Shop", true);
#if defined(ENABLE_GAYA_RENEWAL)
	Set(HEADER_CG_GEM_SHOP, sizeof(TPacketCGGemShop), "GemShop", true);
#endif
	Set(HEADER_CG_ON_CLICK, sizeof(TPacketCGOnClick), "OnClick", true);
	Set(HEADER_CG_EXCHANGE, sizeof(TPacketCGExchange), "Exchange", true);
	Set(HEADER_CG_CHARACTER_POSITION, sizeof(TPacketCGPosition), "Position", true);
	Set(HEADER_CG_SCRIPT_ANSWER, sizeof(TPacketCGScriptAnswer), "ScriptAnswer", true);
	Set(HEADER_CG_SCRIPT_BUTTON, sizeof(TPacketCGScriptButton), "ScriptButton", true);
	Set(HEADER_CG_QUEST_INPUT_STRING, sizeof(TPacketCGQuestInputString), "QuestInputString", true);
	Set(HEADER_CG_QUEST_CONFIRM, sizeof(TPacketCGQuestConfirm), "QuestConfirm", true);

	Set(HEADER_CG_MOVE, sizeof(TPacketCGMove), "Move", true);
	Set(HEADER_CG_SYNC_POSITION, sizeof(TPacketCGSyncPosition), "SyncPosition", true);

	Set(HEADER_CG_FLY_TARGETING, sizeof(TPacketCGFlyTargeting), "FlyTarget", true);
	Set(HEADER_CG_ADD_FLY_TARGETING, sizeof(TPacketCGFlyTargeting), "AddFlyTarget", true);
	Set(HEADER_CG_SHOOT, sizeof(TPacketCGShoot), "Shoot", true);

	Set(HEADER_CG_USE_SKILL, sizeof(TPacketCGUseSkill), "UseSkill", true);

	Set(HEADER_CG_ITEM_USE_TO_ITEM, sizeof(TPacketCGItemUseToItem), "UseItemToItem", true);
	Set(HEADER_CG_TARGET, sizeof(TPacketCGTarget), "Target", true);
	Set(HEADER_CG_WARP, sizeof(TPacketCGWarp), "Warp", true);
	Set(HEADER_CG_MESSENGER, sizeof(TPacketCGMessenger), "Messenger", true);

	Set(HEADER_CG_PARTY_REMOVE, sizeof(TPacketCGPartyRemove), "PartyRemove", true);
	Set(HEADER_CG_PARTY_INVITE, sizeof(TPacketCGPartyInvite), "PartyInvite", true);
	Set(HEADER_CG_PARTY_INVITE_ANSWER, sizeof(TPacketCGPartyInviteAnswer), "PartyInviteAnswer", true);
	Set(HEADER_CG_PARTY_SET_STATE, sizeof(TPacketCGPartySetState), "PartySetState", true);
	Set(HEADER_CG_PARTY_USE_SKILL, sizeof(TPacketCGPartyUseSkill), "PartyUseSkill", true);
	Set(HEADER_CG_PARTY_PARAMETER, sizeof(TPacketCGPartyParameter), "PartyParam", true);

	Set(HEADER_CG_EMPIRE, sizeof(TPacketCGEmpire), "Empire", true);
	Set(HEADER_CG_SAFEBOX_CHECKOUT, sizeof(TPacketCGSafeboxCheckout), "SafeboxCheckout", true);
	Set(HEADER_CG_SAFEBOX_CHECKIN, sizeof(TPacketCGSafeboxCheckin), "SafeboxCheckin", true);

	Set(HEADER_CG_SAFEBOX_ITEM_MOVE, sizeof(TPacketCGItemMove), "SafeboxItemMove", true);
#ifdef __INGAME_WIKI__
	Set(InGameWiki::HEADER_CG_WIKI, sizeof(InGameWiki::TCGWikiPacket), "RecvWikiPacket", true);
#endif
#ifdef __ENABLE_PREMIUM_PLAYERS__
	Set(HEADER_CG_PREMIUM_PLAYERS, sizeof(TPacketCGPremiumPlayers), "RecvPremiumPlayersPacket");
#endif
	Set(HEADER_CG_GUILD, sizeof(TPacketCGGuild), "Guild", true);
	Set(HEADER_CG_ANSWER_MAKE_GUILD, sizeof(TPacketCGAnswerMakeGuild), "AnswerMakeGuild", true);

	Set(HEADER_CG_FISHING, sizeof(TPacketCGFishing), "Fishing", true);
	Set(HEADER_CG_ITEM_GIVE, sizeof(TPacketCGGiveItem), "ItemGive", true);
	Set(HEADER_CG_HACK, sizeof(TPacketCGHack), "Hack", true);

#ifdef __NEWPET_SYSTEM__
	Set(HEADER_CG_PetSetName, sizeof(TPacketCGRequestPetName), "BraveRequestPetName", true);
#endif
	Set(HEADER_CG_MYSHOP, sizeof(TPacketCGMyShop), "MyShop", true);
	Set(HEADER_CG_REFINE, sizeof(TPacketCGRefine), "Refine", true);
	Set(HEADER_CG_CHANGE_NAME, sizeof(TPacketCGChangeName), "ChangeName", true);

	Set(HEADER_CG_CLIENT_VERSION, sizeof(TPacketCGClientVersion), "Version", true);
	Set(HEADER_CG_CLIENT_VERSION2, sizeof(TPacketCGClientVersion2), "Version", true);
	Set(HEADER_CG_PONG, sizeof(BYTE), "Pong", true);
	Set(HEADER_CG_MALL_CHECKOUT, sizeof(TPacketCGSafeboxCheckout), "MallCheckout", true);

	Set(HEADER_CG_SCRIPT_SELECT_ITEM, sizeof(TPacketCGScriptSelectItem), "ScriptSelectItem", true);

	Set(HEADER_CG_DRAGON_SOUL_REFINE, sizeof(TPacketCGDragonSoulRefine), "DragonSoulRefine", false);
#ifdef ENABLE_DS_REFINE_ALL
	Set(HEADER_CG_DRAGON_SOUL_REFINE_ALL, sizeof(TPacketDragonSoulRefineAll), "DragonSoulRefineAll", false);
#endif
	Set(HEADER_CG_STATE_CHECKER, sizeof(BYTE), "ServerStateCheck", false);

#ifdef ENABLE_SWITCHBOT_WORLDARD
	Set(HEADER_CG_SWITCHBOT, sizeof(TPacketCGSwitchBotSend), "SwitchBotSend", true);
#endif
#ifdef ENABLE_OPENSHOP_PACKET
	Set(HEADER_CG_OPENSHOP, sizeof(TPacketOpenShop), "OpenShop", true);
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
	Set(HEADER_CG_NEW_OFFLINESHOP, sizeof(TPacketCGNewOfflineShop), "NewOfflineshop", false);
#endif


#ifdef ENABLE_ACCE_SYSTEM
	Set(HEADER_CG_ACCE, sizeof(TPacketAcce), "Acce", true);
#endif

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	Set(HEADER_CG_CUBE_RENEWAL, sizeof(TPacketCGCubeRenewalSend), "CubeRenewalSend", true);
#endif
#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
	Set(HEADER_CG_WHISPER_ADMIN, sizeof(TPacketCGWhisperAdmin), "TPacketCGWhisperAdmin", true);
#endif
#ifdef ENABLE_NEW_FISHING_SYSTEM
	Set(HEADER_CG_FISHING_NEW, sizeof(TPacketFishingNew), "PacketFishingNew", true);
#endif
#ifdef USE_MULTIPLE_OPENING
    Set(HEADER_CG_MULTI_CHESTS_OPEN, sizeof(TPacketCGMultipleOpening), "MultipleOpening", true);
#endif

	Set(HEADER_CG_REQ_MAP_BOSS_INFO, sizeof(BYTE), "RequestMapBossInfo", true);
	Set(HEADER_CG_CHANGE_COUNTRY_FLAG, sizeof(TPacketCGChangeCountryFlag), "ChangeCountryFlag", true);
	Set(HEADER_CG_WHISPER_INFO, sizeof(SPacketCGWhisperInfo), "WhisperInfo", true);
}

CPacketInfoCG::~CPacketInfoCG()
{
	Log("packet_info.txt");
}

////////////////////////////////////////////////////////////////////////////////
CPacketInfoGG::CPacketInfoGG()
{
#if defined(ENABLE_ULTIMATE_REGEN)
	Set(HEADER_GG_NEW_REGEN, sizeof(TGGPacketNewRegen), "NewRegen", false);
#endif
#if defined(ENABLE_CLOSE_GAMECLIENT_CMD)
	Set(HEADER_GG_CLOSECLIENT, sizeof(TPacketGGCloseClient), "CloseClient", false);
#endif
#if defined(ENABLE_MESSENGER_BLOCK)
	Set(HEADER_GG_MESSENGER_BLOCK, sizeof(TPacketGGMessengerBlock), "MessengerBlock", false);
#endif
	Set(HEADER_GG_SETUP,		sizeof(TPacketGGSetup),		"Setup", false);
	Set(HEADER_GG_LOGIN,		sizeof(TPacketGGLogin),		"Login", false);
	Set(HEADER_GG_LOGOUT,		sizeof(TPacketGGLogout),	"Logout", false);
	Set(HEADER_GG_RELAY,		sizeof(TPacketGGRelay),		"Relay", false);
	Set(HEADER_GG_NOTICE,		sizeof(TPacketGGNotice),	"Notice", false);
#ifdef ENABLE_FULL_NOTICE
	Set(HEADER_GG_BIG_NOTICE,	sizeof(TPacketGGNotice),	"BigNotice", false);
#endif
	Set(HEADER_GG_SHUTDOWN,		sizeof(TPacketGGShutdown),	"Shutdown", false);
	Set(HEADER_GG_GUILD,		sizeof(TPacketGGGuild),		"Guild", false);
	Set(HEADER_GG_SHOUT,		sizeof(TPacketGGShout),		"Shout", false);
	Set(HEADER_GG_DISCONNECT,	    	sizeof(TPacketGGDisconnect),	"Disconnect", false);
	Set(HEADER_GG_MESSENGER_ADD,	sizeof(TPacketGGMessenger),	"MessengerAdd", false);
	Set(HEADER_GG_MESSENGER_REMOVE,	sizeof(TPacketGGMessenger),	"MessengerRemove", false);
	Set(HEADER_GG_FIND_POSITION,	sizeof(TPacketGGFindPosition),	"FindPosition", false);
	Set(HEADER_GG_WARP_CHARACTER,	sizeof(TPacketGGWarpCharacter),	"WarpCharacter", false);
	Set(HEADER_GG_MESSENGER_MOBILE,	sizeof(TPacketGGMessengerMobile), "MessengerMobile", false);
	Set(HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX, sizeof(TPacketGGGuildWarMapIndex), "GuildWarMapIndex", false);
	Set(HEADER_GG_TRANSFER,		sizeof(TPacketGGTransfer),	"Transfer", false);
	Set(HEADER_GG_RELOAD_CRC_LIST,	sizeof(BYTE),			"ReloadCRCList", false);
	Set(HEADER_GG_CHECK_CLIENT_VERSION, sizeof(BYTE),			"CheckClientVersion", false);
	Set(HEADER_GG_LOGIN_PING,		sizeof(TPacketGGLoginPing),	"LoginPing", false);

	// BLOCK_CHAT
	Set(HEADER_GG_BLOCK_CHAT,		sizeof(TPacketGGBlockChat),	"BlockChat", false);
	// END_OF_BLOCK_CHAT
#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
	Set(HEADER_GG_WHISPER_SYSTEM,		sizeof(TPacketGGWhisperSystem),	"TPacketGGWhisperSystem", false);
#endif
	Set(HEADER_GG_CHECK_AWAKENESS,		sizeof(TPacketGGCheckAwakeness),	"CheckAwakeness",		false);
#if defined(ENABLE_OFFLINESHOP_REWORK)
	Set(HEADER_GG_OFFLINE_SHOP_NOTIFICATION,sizeof(TPacketGGOfflineShopNotification),"OfflineShopNotification", false);
#endif
#if defined(ENABLE_TEXTS_RENEWAL)
	Set(HEADER_GG_CHAT_NEW, sizeof(TPacketGGChatNew), "ChatNew", false);
#endif
#ifdef ENABLE_REWARD_SYSTEM
	Set(HEADER_GG_REWARD_INFO, sizeof(TPacketGGRewardInfo), "RewardInfo", false);
#endif
}

CPacketInfoGG::~CPacketInfoGG()
{
	Log("p2p_packet_info.txt");
}

