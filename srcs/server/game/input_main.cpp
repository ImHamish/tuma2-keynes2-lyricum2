#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "utils.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "protocol.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "cmd.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "battle.h"
#include "exchange.h"
#include "questmanager.h"
#include "profiler.h"
#include "messenger_manager.h"
#include "party.h"
#include "p2p.h"
#include "affect.h"
#include "guild.h"
#include "guild_manager.h"
#include "log.h"
#include "banword.h"
#include "empire_text_convert.h"
#include "unique_item.h"
#include "building.h"
#include "locale_service.h"
#include "gm.h"
#include "spam.h"
#include "ani.h"
#include "motion.h"
#include "OXEvent.h"
#ifdef ENABLE_SWITCHBOT_WORLDARD
#include "switchbonus.h"
#endif
#include "locale_service.h"
#include "DragonSoul.h"
#ifdef __NEWPET_SYSTEM__
#include "New_PetSystem.h"
#endif
#include "belt_inventory_helper.h" // @fixme119
#include "../common/CommonDefines.h"

#include "input.h"

#ifdef ENABLE_FEATURES_REFINE_SYSTEM
	#include "refine.h"
#endif

#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
	#include "whisper_admin.h"
#endif
#ifdef __INGAME_WIKI__
#include "mob_manager.h"
#endif
#if defined(USE_BATTLEPASS)
#include "battlepass_manager.h"
#endif

#define ENABLE_CHECK_GHOSTMODE

static char	__escape_query[1024];
static int __deposit_limit()
{
	return (1000*10000); // 1천만
}

void SendBlockChatInfo(LPCHARACTER ch, int sec)
{
	if (sec <= 0)
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 473, "");
		return;
	}

	long hour = sec / 3600;
	sec -= hour * 3600;
	long min = (sec / 60);
	sec -= min * 60;
	if (hour > 0 && min > 0) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 475, "%d#%d#%d", hour, min, sec);
	}
	else if (hour > 0 && min == 0) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 476, "%d#%d", hour, sec);
	}
	else if (hour == 0 && min > 0) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 477, "%d#%d", min, sec);
	}
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 478, "%d", sec);
	}
}

EVENTINFO(spam_event_info)
{
	char host[MAX_HOST_LENGTH+1];

	spam_event_info()
	{
		::memset( host, 0, MAX_HOST_LENGTH+1 );
	}
};

typedef std::unordered_map<std::string, std::pair<unsigned int, LPEVENT> > spam_score_of_ip_t;
spam_score_of_ip_t spam_score_of_ip;

EVENTFUNC(block_chat_by_ip_event)
{
	spam_event_info* info = dynamic_cast<spam_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "block_chat_by_ip_event> <Factor> Null pointer" );
		return 0;
	}

	const char * host = info->host;

	spam_score_of_ip_t::iterator it = spam_score_of_ip.find(host);

	if (it != spam_score_of_ip.end())
	{
		it->second.first = 0;
		it->second.second = NULL;
	}

	return 0;
}

bool SpamBlockCheck(LPCHARACTER ch, const char* const buf, const size_t buflen)
{
	if (ch->GetLevel() < g_iSpamBlockMaxLevel)
	{
		spam_score_of_ip_t::iterator it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());

		if (it == spam_score_of_ip.end())
		{
			spam_score_of_ip.insert(std::make_pair(ch->GetDesc()->GetHostName(), std::make_pair(0, (LPEVENT) NULL)));
			it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());
		}

		if (it->second.second)
		{
			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);
			return true;
		}

		unsigned int score;
		const char * word = SpamManager::instance().GetSpamScore(buf, buflen, score);

		it->second.first += score;

		if (word)
			sys_log(0, "SPAM_SCORE: %s text: %s score: %u total: %u word: %s", ch->GetName(), buf, score, it->second.first, word);

		if (it->second.first >= g_uiSpamBlockScore)
		{
			spam_event_info* info = AllocEventInfo<spam_event_info>();
			strlcpy(info->host, ch->GetDesc()->GetHostName(), sizeof(info->host));

			it->second.second = event_create(block_chat_by_ip_event, info, PASSES_PER_SEC(g_uiSpamBlockDuration));
			sys_log(0, "SPAM_IP: %s for %u seconds", info->host, g_uiSpamBlockDuration);

			LogManager::instance().CharLog(ch, 0, "SPAM", word);

			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);

			return true;
		}
	}

	return false;
}

enum
{
	TEXT_TAG_PLAIN,
	TEXT_TAG_TAG, // ||
	TEXT_TAG_COLOR, // |cffffffff
	TEXT_TAG_HYPERLINK_START, // |H
	TEXT_TAG_HYPERLINK_END, // |h ex) |Hitem:1234:1:1:1|h
	TEXT_TAG_RESTORE_COLOR,
};

int GetTextTag(const char * src, int maxLen, int & tagLen, std::string & extraInfo)
{
	tagLen = 1;

	if (maxLen < 2 || *src != '|')
		return TEXT_TAG_PLAIN;

	const char * cur = ++src;

	if (*cur == '|') // ||는 |로 표시한다.
	{
		tagLen = 2;
		return TEXT_TAG_TAG;
	}
	else if (*cur == 'c') // color |cffffffffblahblah|r
	{
		tagLen = 2;
		return TEXT_TAG_COLOR;
	}
	else if (*cur == 'H') // hyperlink |Hitem:10000:0:0:0:0|h[이름]|h
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_START;
	}
	else if (*cur == 'h') // end of hyperlink
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_END;
	}

	return TEXT_TAG_PLAIN;
}

void GetTextTagInfo(const char * src, int src_len, int & hyperlinks, bool & colored)
{
	colored = false;
	hyperlinks = 0;

	int len;
	std::string extraInfo;

	for (int i = 0; i < src_len;)
	{
		int tag = GetTextTag(&src[i], src_len - i, len, extraInfo);

		if (tag == TEXT_TAG_HYPERLINK_START)
			++hyperlinks;

		if (tag == TEXT_TAG_COLOR)
			colored = true;

		i += len;
	}
}

int ProcessTextTag(LPCHARACTER ch, const char * c_pszText, size_t len)
{
	//개인상점중에 금강경을 사용할 경우
	//0 : 정상적으로 사용
	//1 : 금강경 부족
	//2 : 금강경이 있으나, 개인상점에서 사용중
	//3 : 기타
	//4 : 에러
	int hyperlinks;
	bool colored;

	GetTextTagInfo(c_pszText, len, hyperlinks, colored);

	if (colored == true && hyperlinks == 0)
		return 4;

	if (g_bDisablePrismNeed)
		return 0;

	int nPrismCount = ch->CountSpecifyItem(ITEM_PRISM);

	if (nPrismCount < hyperlinks)
		return 1;


	if (!ch->GetMyShop())
	{
		ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
		return 0;
	} else
	{
		int sellingNumber = ch->GetMyShop()->GetNumberByVnum(ITEM_PRISM);
		if(nPrismCount - sellingNumber < hyperlinks)
		{
			return 2;
		} else
		{
			ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
			return 0;
		}
	}

	return 4;
}

int32_t CInputMain::Whisper(LPCHARACTER ch, const char * data, size_t uiBytes) {
	const auto* pinfo = reinterpret_cast<const TPacketCGWhisper*>(data);

	if (uiBytes < pinfo->wSize) {
		return -1;
	}

	int32_t iExtraLen = pinfo->wSize - sizeof(TPacketCGWhisper);

	const LPDESC d = ch->GetDesc();
	if (!d) {
		return -1;
	}

	if (iExtraLen < 0) {
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

#if defined(ENABLE_RENEWAL_OX)
	if (ch->GetGMLevel() == GM_PLAYER && ch->GetMapIndex() == OX_MAP_INDEX) {
		return iExtraLen;
	}
#endif

	if (ch->GetLastPMPulse() < thecore_pulse())
		ch->ClearPMCounter();

	if (ch->GetPMCounter() > 3 && ch->GetLastPMPulse() > thecore_pulse())
	{
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	if (ch->FindAffect(AFFECT_BLOCK_CHAT))
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 639, "");
		return (iExtraLen);
	}

	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(pinfo->szNameTo);

	if (pkChr == ch)
		return (iExtraLen);

	ch->IncreasePMCounter();
	ch->SetLastPMPulse();

	LPDESC pkDesc = NULL;

	BYTE bOpponentEmpire = 0;

	if (test_server)
	{
		if (!pkChr)
			sys_log(0, "Whisper to %s(%s) from %s", "Null", pinfo->szNameTo, ch->GetName());
		else
			sys_log(0, "Whisper to %s(%s) from %s", pkChr->GetName(), pinfo->szNameTo, ch->GetName());
	}

	if (ch->IsBlockMode(BLOCK_WHISPER))
	{
		if (ch->GetDesc())
		{
			TPacketGCWhisper pack;
			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
			pack.wSize = sizeof(TPacketGCWhisper);
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			ch->GetDesc()->Packet(&pack, sizeof(pack));
		}
		
		return iExtraLen;
	}

	if (!pkChr)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

		if (pkCCI)
		{
			pkDesc = pkCCI->pkDesc;
			pkDesc->SetRelay(pinfo->szNameTo);
			bOpponentEmpire = pkCCI->bEmpire;

			if (test_server)
				sys_log(0, "Whisper to %s from %s (Channel %d Mapindex %d)", "Null", ch->GetName(), pkCCI->bChannel, pkCCI->lMapIndex);
		}
	}
	else
	{
		pkDesc = pkChr->GetDesc();
		bOpponentEmpire = pkChr->GetEmpire();
	}

	if (!pkDesc)
	{
		if (ch->GetDesc())
		{
#if defined(BL_OFFLINE_MESSAGE)
			const BYTE bDelay = 10;
			char msg[64];
			if (get_dword_time() - ch->GetLastOfflinePMTime() > bDelay * 1000)
			{
				char buf[CHAT_MAX_LEN + 1];
				strlcpy(buf, data + sizeof(TPacketCGWhisper), MIN(iExtraLen + 1, sizeof(buf)));
				const size_t buflen = strlen(buf);
				CBanwordManager::instance().ConvertString(buf, buflen);
				int processReturn = ProcessTextTag(ch, buf, buflen);
				
				if (0 != processReturn)
				{
					TItemTable* pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);
					if (pTable) {
#ifdef ENABLE_MULTI_NAMES
						int Lang = ch && ch->GetDesc() ? ch->GetDesc()->GetLanguage() : 0;
#endif
						ch->ChatPacketNew(CHAT_TYPE_INFO, 823, "%s",
#ifdef ENABLE_MULTI_NAMES
						pTable->szLocaleName[Lang]
#else
						pTable->szLocaleName
#endif
						);
					}
					
					return (iExtraLen);
				}
				
				if (buflen > 0) 
				{
					ch->SendOfflineMessage(pinfo->szNameTo, buf);
					snprintf(msg, sizeof(msg), "An offline message has been sent.");
				}
				else
					return (iExtraLen);
			}
			else
			{
				snprintf(msg, sizeof(msg), "You have to wait %d seconds for send offline message.", bDelay);
			}
			
			TPacketGCWhisper pack;
			int len = MIN(CHAT_MAX_LEN, strlen(msg) + 1);
			pack.bHeader = HEADER_GC_WHISPER;
			pack.wSize = static_cast<WORD>(sizeof(TPacketGCWhisper) + len);
			pack.bType = WHISPER_TYPE_OFFLINE;
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			
			TEMP_BUFFER buf;
			buf.write(&pack, sizeof(TPacketGCWhisper));
			buf.write(msg, len);
			ch->GetDesc()->Packet(buf.read_peek(), buf.size());

#else
			TPacketGCWhisper pack;
			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_NOT_EXIST;
			pack.wSize = sizeof(TPacketGCWhisper);
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			ch->GetDesc()->Packet(&pack, sizeof(TPacketGCWhisper));
			sys_log(0, "WHISPER: no player");
#endif
		}
	}
	else
	{
		if (ch->IsBlockMode(BLOCK_WHISPER))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
				pack.wSize = sizeof(TPacketGCWhisper);
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
		else if (pkChr && pkChr->IsBlockMode(BLOCK_WHISPER))
		{
			TPacketGCWhisper p;
			p.bHeader = HEADER_GC_WHISPER;
			p.bType = WHISPER_TYPE_TARGET_BLOCKED;
			p.wSize = sizeof(TPacketGCWhisper);
			strlcpy(p.szNameFrom, pinfo->szNameTo, sizeof(p.szNameFrom));

			d->Packet(&p, sizeof(p));
		}
#if defined(ENABLE_MESSENGER_BLOCK)
		else if (MessengerManager::instance().CheckMessengerList(ch->GetName(), pinfo->szNameTo, SYST_BLOCK)) {
			TPacketGCWhisper p;
			p.bHeader = HEADER_GC_WHISPER;
			p.bType = WHISPER_TYPE_FRIEND_BLOCKED;
			p.wSize = sizeof(TPacketGCWhisper);
			strlcpy(p.szNameFrom, pinfo->szNameTo, sizeof(p.szNameFrom));

			d->Packet(&p, sizeof(p));
		}
#endif
		else
		{
			BYTE bType = WHISPER_TYPE_NORMAL;

			char buf[CHAT_MAX_LEN + 1];
			strlcpy(buf, data + sizeof(TPacketCGWhisper), MIN(iExtraLen + 1, sizeof(buf)));
			const size_t buflen = strlen(buf);

			if (true == SpamBlockCheck(ch, buf, buflen))
			{
				if (!pkChr)
				{
					CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

					if (pkCCI)
					{
						pkDesc->SetRelay("");
					}
				}
				return iExtraLen;
			}

			CBanwordManager::instance().ConvertString(buf, buflen);

			if (g_bEmpireWhisper)
				if (!ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
					if (!(pkChr && pkChr->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)))
						if (bOpponentEmpire != ch->GetEmpire() && ch->GetEmpire() && bOpponentEmpire // 서로 제국이 다르면서
								&& ch->GetGMLevel() == GM_PLAYER && gm_get_level(pinfo->szNameTo) == GM_PLAYER) // 둘다 일반 플레이어이면
							// 이름 밖에 모르니 gm_get_level 함수를 사용
						{
							if (!pkChr)
							{
								// 다른 서버에 있으니 제국 표시만 한다. bType의 상위 4비트를 Empire번호로 사용한다.
								bType = ch->GetEmpire() << 4;
							}
							else
							{
								ConvertEmpireText(ch->GetEmpire(), buf, buflen, 10 + 2 * pkChr->GetSkillPower(SKILL_LANGUAGE1 + ch->GetEmpire() - 1)/*변환확률*/);
							}
						}

			int processReturn = ProcessTextTag(ch, buf, buflen);
			if (0!=processReturn)
			{
				if (ch->GetDesc())
				{
					TItemTable * pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

					if (pTable)
					{
#ifdef ENABLE_MULTI_NAMES
						int Lang = ch && ch->GetDesc() ? ch->GetDesc()->GetLanguage() : 0;
#endif
						ch->ChatPacketNew(CHAT_TYPE_INFO, 823, "%s",
#ifdef ENABLE_MULTI_NAMES
						pTable->szLocaleName[Lang]
#else
						pTable->szLocaleName
#endif
						);
					}
				}

				// 릴래이 상태일 수 있으므로 릴래이를 풀어준다.
				pkDesc->SetRelay("");
				return (iExtraLen);
			}

			if (ch->IsGM())
				bType = (bType & 0xF0) | WHISPER_TYPE_GM;

			if (buflen > 0)
			{
				TPacketGCWhisper pack;

				pack.bHeader = HEADER_GC_WHISPER;
				pack.wSize = sizeof(TPacketGCWhisper) + buflen;
				pack.bType = bType;
				strlcpy(pack.szNameFrom, ch->GetName(), sizeof(pack.szNameFrom));
				// desc->BufferedPacket을 하지 않고 버퍼에 써야하는 이유는
				// P2P relay되어 패킷이 캡슐화 될 수 있기 때문이다.
				TEMP_BUFFER tmpbuf;

				tmpbuf.write(&pack, sizeof(pack));
				tmpbuf.write(buf, buflen);

				pkDesc->Packet(tmpbuf.read_peek(), tmpbuf.size());

				// @warme006
				// sys_log(0, "WHISPER: %s -> %s : %s", ch->GetName(), pinfo->szNameTo, buf);
			}
		}
	}
	if(pkDesc)
		pkDesc->SetRelay("");

	return (iExtraLen);
}

struct RawPacketToCharacterFunc {
	const void* m_buf;
	int32_t m_buf_len;
#if defined(ENABLE_MESSENGER_BLOCK)
	const char* m_szName;
#endif

	RawPacketToCharacterFunc(const void * buf, int32_t buf_len
#if defined(ENABLE_MESSENGER_BLOCK)
, const char* name
#endif
	) : 
	m_buf(buf), m_buf_len(buf_len)
#if defined(ENABLE_MESSENGER_BLOCK)
, m_szName(name)
#endif
	{
	}

	void operator () (LPCHARACTER c)
	{
		const LPDESC d = c->GetDesc();

		if (!c->GetDesc()) {
			return;
		}

#if defined(ENABLE_MESSENGER_BLOCK)
		if (MessengerManager::instance().CheckMessengerList(c->GetName(), m_szName, SYST_BLOCK)) {
			return;
		}
#endif

		d->Packet(m_buf, m_buf_len);
	}
};

struct FEmpireChatPacket
{
	packet_chat& p;
	const char* orig_msg;
	int orig_len;
	char converted_msg[CHAT_MAX_LEN+1];

	BYTE bEmpire;
	int iMapIndex;
	int namelen;
	const char* m_szName;

	FEmpireChatPacket(packet_chat& p, const char* chat_msg, int len, BYTE bEmpire, int iMapIndex, const char* name)
		: p(p), orig_msg(chat_msg), orig_len(len), bEmpire(bEmpire), iMapIndex(iMapIndex), namelen(strlen(name)), m_szName(name)
	{
		memset( converted_msg, 0, sizeof(converted_msg) );
	}

	void operator () (LPDESC d)
	{
		const LPCHARACTER pkChar = d->GetCharacter();
		if (!pkChar || pkChar->GetMapIndex() != iMapIndex) {
			return;
		}

#if defined(ENABLE_MESSENGER_BLOCK)
		if (MessengerManager::instance().CheckMessengerList(pkChar->GetName(), m_szName, SYST_BLOCK)) {
			return;
		}
#endif

		d->BufferedPacket(&p, sizeof(packet_chat));

		if (d->GetEmpire() == bEmpire ||
			bEmpire == 0 ||
			d->GetCharacter()->GetGMLevel() > GM_PLAYER ||
			d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
		{
			d->Packet(orig_msg, orig_len);
		}
		else
		{
			size_t len = strlcpy(converted_msg, orig_msg, sizeof(converted_msg));

			if (len >= sizeof(converted_msg))
				len = sizeof(converted_msg) - 1;

			ConvertEmpireText(bEmpire, converted_msg + namelen, len - namelen, 10 + 2 * d->GetCharacter()->GetSkillPower(SKILL_LANGUAGE1 + bEmpire - 1));
			d->Packet(converted_msg, orig_len);
		}
	}
};

#ifdef __NEWPET_SYSTEM__
void CInputMain::BraveRequestPetName(LPCHARACTER ch, const char* c_pData)
{
	if (!ch->GetDesc()) { return; }
	int vid = ch->GetEggVid();
	if (vid == 0) { return; }

	TPacketCGRequestPetName* p = (TPacketCGRequestPetName*)c_pData;

	if (ch->GetGold() < 100000) 
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 768, "%d", 100000);
	}

	if (ch->CountSpecifyItem(vid) > 0 && check_name(p->petname) != 0) {
#ifdef ENABLE_NEW_PET_EDITS
		{
			char szQuery[256] = {0};
			snprintf(szQuery, sizeof(szQuery), "SELECT id FROM player.new_petsystem%s WHERE name='%s';", get_table_postfix(), p->petname);
			std::unique_ptr<SQLMsg> pRes(DBManager::instance().DirectQuery(szQuery));
			int iRes = pRes->Get()->uiNumRows;
			if (iRes > 0) {
				ch->ChatPacketNew(CHAT_TYPE_INFO, 50, "");
				return;
			}
		}
#endif
		
		DBManager::instance().SendMoneyLog(MONEY_LOG_QUEST, ch->GetPlayerID(), -100000);
#if defined(ENABLE_NEW_GOLD_LIMIT)
		ch->ChangeGold(-100000);
#else
		ch->PointChange(POINT_GOLD, -100000, true);
#endif
		ch->RemoveSpecifyItem(vid, 1);
		LPITEM item = ch->AutoGiveItem(vid + 300, 1);
#ifdef ENABLE_NEW_PET_EDITS
		int tmpskill[4] = { -1, -1, -1, -1 };
#else
		int tmpskill[4] = { 0, 0, 0, 0 };
		int tmpslot = number(1, 3);
		for (int i = 0; i < 4; ++i)
		{
			if (i > tmpslot - 1)
				tmpskill[i] = -1;
		}
#endif
		int tmpdur = 3 * 24 * 60;
		char szQuery1[1024];
		int hp[] = {30, 35, 40};
		int mostri[] = {10, 15, 20};
		int medi[] = {10, 15, 20};
		snprintf(szQuery1, sizeof(szQuery1), "INSERT INTO new_petsystem VALUES(%u,'%s', 1, 0, 0, 0, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, 0"
#ifdef ENABLE_NEW_PET_EDITS
		", %d"
#endif
		")", item->GetID(), p->petname, hp[number(0, 2)], mostri[number(0, 2)], medi[number(0, 2)], tmpskill[0], 0, tmpskill[1], 0, tmpskill[2], 0, tmpskill[3], 0, tmpdur, tmpdur, get_global_time());
		std::unique_ptr<SQLMsg> pmsg2(DBManager::instance().DirectQuery(szQuery1));
		ch->ChatPacketNew(CHAT_TYPE_INFO, 769, "");
	}
	else  {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 770, "");
	}
}
#endif

int CInputMain::Chat(LPDESC d, const char * data, size_t uiBytes)
{
	const TPacketCGChat* pinfo = reinterpret_cast<const TPacketCGChat*>(data);

	if (uiBytes < pinfo->size)
		return -1;

	const int iExtraLen = pinfo->size - sizeof(TPacketCGChat);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->size, uiBytes);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	const auto ch = d->GetCharacter();
	if (!ch)
		return iExtraLen;

	char buf[CHAT_MAX_LEN - (CHARACTER_NAME_MAX_LEN + 3) + 1];
	strlcpy(buf, data + sizeof(TPacketCGChat), MIN(iExtraLen + 1, sizeof(buf)));
	const size_t buflen = strlen(buf);

	if (buflen > 1 && *buf == '/')
	{
		interpret_command(ch, buf + 1, buflen - 1);
		return iExtraLen;
	}

#if defined(ENABLE_RENEWAL_OX)
	if (ch->GetGMLevel() == GM_PLAYER && ch->GetMapIndex() == OX_MAP_INDEX) {
		return iExtraLen;
	}
#endif

	// 채팅 금지 Affect 처리
	const CAffect* pAffect = ch->FindAffect(AFFECT_BLOCK_CHAT);

	if (pAffect != NULL)
	{
		SendBlockChatInfo(ch, pAffect->lDuration);
		return iExtraLen;
	}

	if (true == SpamBlockCheck(ch, buf, buflen))
	{
		return iExtraLen;
	}

	// @fixme133 begin
	CBanwordManager::instance().ConvertString(buf, buflen);

	int processReturn = ProcessTextTag(ch, buf, buflen);
	if (0!=processReturn)
	{
		const TItemTable* pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);
		if (NULL != pTable)
		{
#ifdef ENABLE_MULTI_NAMES
			int lang = 0;
			if (ch) {
				LPDESC desc = ch->GetDesc();
				lang = desc != NULL ? desc->GetLanguage() : 0;
			}
#endif
			ch->ChatPacketNew(CHAT_TYPE_INFO, 642, "%s", 
#ifdef ENABLE_MULTI_NAMES
			pTable->szLocaleName[lang]
#else
			pTable->szLocaleName
#endif
			);
		}

		return iExtraLen;
	}
	// @fixme133 end

	char chatbuf[CHAT_MAX_LEN + 1];
	//static const char* colorbuf[] = {"|cFFffa200|H|h[Staff]|h|r", "|cFFff0000|H|h[Shinsoo]|h|r", "|cFFffc700|H|h[Chunjo]|h|r", "|cFF000bff|H|h[Jinno]|h|r"};
#ifdef ENABLE_MULTI_LANGUAGE
	int len;

	if (pinfo->type == CHAT_TYPE_SHOUT) {
//		len = snprintf(chatbuf, sizeof(chatbuf), "|L%s|l%s |Hmsg:%s|h%s [PM]|h|r : %s", langName.c_str(), (ch->IsGM()?colorbuf[0]:colorbuf[MINMAX(0, ch->GetEmpire(), 3)]), ch->GetName(), ch->GetName(), buf);
		len = snprintf(chatbuf, sizeof(chatbuf), "|L%s|l|E%d|e |Hmsg:%s|h%s [PM]|h|r : %s", ch->GetCountryFlag().c_str(), ch->GetEmpire(), ch->GetName(), ch->GetName(), buf);
	} else {
//		len = snprintf(chatbuf, sizeof(chatbuf), "|L%s|l %s %s : %s", langName.c_str(), (ch->IsGM()?colorbuf[0]:colorbuf[MINMAX(0, ch->GetEmpire(), 3)]), ch->GetName(), buf);
		len = snprintf(chatbuf, sizeof(chatbuf), "|L%s|l|E%d|e %s : %s", ch->GetCountryFlag().c_str(), ch->GetEmpire(), ch->GetName(), buf);
	}
#else
	int len = snprintf(chatbuf, sizeof(chatbuf), "%s %s : %s", (ch->IsGM()?colorbuf[0]:colorbuf[MINMAX(0, ch->GetEmpire(), 3)]), ch->GetName(),buf);
#endif

	if (CHAT_TYPE_SHOUT == pinfo->type)
	{
		LogManager::instance().ShoutLog(g_bChannel, ch->GetEmpire(), chatbuf);
	}

	if (len < 0 || len >= (int) sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	if (pinfo->type == CHAT_TYPE_SHOUT)
	{
		// const int SHOUT_LIMIT_LEVEL = 15;

		if (ch->GetLevel() < g_iShoutLimitLevel) {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 411, "%d", g_iShoutLimitLevel);
			return (iExtraLen);
		}

		// if (thecore_heart->pulse - (int) ch->GetLastShoutPulse() < passes_per_sec * g_iShoutLimitTime)
		if (thecore_heart->pulse - (int) ch->GetLastShoutPulse() < passes_per_sec * 15)
			return (iExtraLen);

		ch->SetLastShoutPulse(thecore_heart->pulse);

		TPacketGGShout p;

		p.bHeader = HEADER_GG_SHOUT;
		p.bEmpire = ch->GetEmpire();
#if defined(ENABLE_MESSENGER_BLOCK)
		strlcpy(p.name, ch->GetName(), sizeof(p.name));
#endif
		strlcpy(p.szText, chatbuf, sizeof(p.szText));

		P2P_MANAGER::instance().Send(&p, sizeof(p));
		SendShout(chatbuf, ch->GetEmpire()
#if defined(ENABLE_MESSENGER_BLOCK)
, ch->GetName()
#endif
		);

#if defined(USE_BATTLEPASS)
		ch->UpdateExtBattlePassMissionProgress(CHAT_MESSAGE_SHOUT, 1, 0);
#endif

		return (iExtraLen);
	}

	TPacketGCChat pack_chat;

	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(TPacketGCChat) + len;
	pack_chat.type = pinfo->type;
	pack_chat.id = ch->GetVID();

	switch (pinfo->type)
	{
		case CHAT_TYPE_TALKING:
			{
				const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();

				std::for_each(c_ref_set.begin(), c_ref_set.end(),
						FEmpireChatPacket(pack_chat,
							chatbuf,
							len,
							(ch->GetGMLevel() > GM_PLAYER ||
							 ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)) ? 0 : ch->GetEmpire(),
							ch->GetMapIndex(), ch->GetName()));
			}
			break;

		case CHAT_TYPE_PARTY:
			{
				const LPPARTY pParty = ch->GetParty();

				if (!pParty) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 486, "");
				} else {
					TEMP_BUFFER tbuf;

					tbuf.write(&pack_chat, sizeof(pack_chat));
					tbuf.write(chatbuf, len);

					RawPacketToCharacterFunc f(tbuf.read_peek(), tbuf.size()
#if defined(ENABLE_MESSENGER_BLOCK)
, ch->GetName()
#endif
					);

					pParty->ForEachOnlineMember(f);
				}
			}
			break;

		case CHAT_TYPE_GUILD:
			{
				if (ch->GetGuild()) {
					ch->GetGuild()->Chat(chatbuf
#if defined(ENABLE_MESSENGER_BLOCK)
, ch->GetName()
#endif
					);
				}
				else {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 271, "");
				}
			}
			break;

		default:
			sys_err("Unknown chat type %d", pinfo->type);
			break;
	}

	return (iExtraLen);
}

void CInputMain::ItemUse(LPCHARACTER ch, const char * data)
{
	ch->UseItem(((struct command_item_use *) data)->Cell);
}

void CInputMain::ItemToItem(LPCHARACTER ch, const char * pcData)
{
	TPacketCGItemUseToItem * p = (TPacketCGItemUseToItem *) pcData;
	if (ch)
		ch->UseItem(p->Cell, p->TargetCell);
}

void CInputMain::ItemDrop(LPCHARACTER ch, const char * data)
{
	const auto* pInfo = reinterpret_cast<const TPacketCGItemDrop*>(data);
	if (!pInfo || !ch)
	{
		return;
	}

	ch->DropItem(pInfo->Cell);
}

void CInputMain::ItemDrop2(LPCHARACTER ch, const char * data)
{
	const auto* pInfo = reinterpret_cast<const TPacketCGItemDrop2*>(data);
	if (!pInfo || !ch)
	{
		return;
	}

	ch->DropItem(pInfo->Cell, pInfo->count);
}

void CInputMain::ItemMove(LPCHARACTER ch, const char * data)
{
	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (ch) {
		ch->MoveItem(pinfo->Cell, pinfo->CellTo, pinfo->count);
	}
}

void CInputMain::ItemPickup(LPCHARACTER ch, const char * data)
{
	struct command_item_pickup* pinfo = (struct command_item_pickup*) data;
	if (ch)
	{
		ch->PickupItem(pinfo->vid);
	}
}

void CInputMain::QuickslotAdd(LPCHARACTER ch, const char * data)
{
	TPacketQuickSlotAdd* pinfo = (TPacketQuickSlotAdd*) data;
	if (!pinfo || !ch) {
		return;
	}

	if (pinfo->slot.type == QUICKSLOT_TYPE_ITEM
#if defined(ENABLE_EXTRA_INVENTORY)
 || pinfo->slot.type == 12
#endif
	) {
#if defined(ENABLE_EXTRA_INVENTORY)
		uint8_t windowType = pinfo->slot.type == 12 ? EXTRA_INVENTORY : INVENTORY;
#else
		uint8_t windowType = INVENTORY;
#endif

		TItemPos srcCell(windowType, static_cast<uint16_t>(pinfo->slot.pos));

		const LPITEM pItem = ch->GetItem(srcCell);
		if (!pItem) {
			return;
		}

		if (pItem->GetType() != ITEM_USE && 
			pItem->GetType() != ITEM_QUEST) {
			return;
		}

#if defined(ENABLE_EXTRA_INVENTORY)
		if (windowType == EXTRA_INVENTORY && 
			pItem->GetType() == ITEM_USE && 
			pItem->GetSubType() == USE_POTION) {
			return;
		}
#endif
	}

	ch->SetQuickslot(pinfo->pos, pinfo->slot, true);
}

void CInputMain::QuickslotDelete(LPCHARACTER ch, const char * data)
{
	const auto* pinfo = reinterpret_cast<const TPacketQuickSlotDel*>(data);
	if (!pinfo || !ch) {
		return;
	}

	ch->DelQuickslot(pinfo->pos);
}

void CInputMain::QuickslotSwap(LPCHARACTER ch, const char * data)
{
	const auto* pinfo = reinterpret_cast<const TPacketQuickSlotSwap*>(data);
	if (!pinfo || !ch) {
		return;
	}

	ch->SwapQuickslot(pinfo->pos, pinfo->change_pos);
}

int32_t CInputMain::Messenger(LPCHARACTER ch, const char* c_pData, size_t uiBytes) {
	TPacketCGMessenger* p = (TPacketCGMessenger*) c_pData;

	if (uiBytes < sizeof(TPacketCGMessenger))
		return -1;

	c_pData += sizeof(TPacketCGMessenger);
	uiBytes -= sizeof(TPacketCGMessenger);

	switch (p->subheader)
	{
		case MESSENGER_SUBHEADER_CG_ADD_BY_VID:
			{
				if (uiBytes < sizeof(TPacketCGMessengerAddByVID))
					return -1;

				TPacketCGMessengerAddByVID * p2 = (TPacketCGMessengerAddByVID *) c_pData;
				LPCHARACTER ch_companion = CHARACTER_MANAGER::instance().Find(p2->vid);

				if (!ch_companion)
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch->IsObserverMode())
					return sizeof(TPacketCGMessengerAddByVID);

				if(ch_companion == ch)
					return sizeof(TPacketCGMessengerAddByVID);
				if (ch_companion->IsBlockMode(BLOCK_MESSENGER_INVITE))
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 370, "%s", ch_companion->GetName());
					return sizeof(TPacketCGMessengerAddByVID);
				}

				LPDESC d = ch_companion->GetDesc();

				if (!d)
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch->GetGMLevel() == GM_PLAYER && ch_companion->GetGMLevel() != GM_PLAYER)
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 184, "");
					return sizeof(TPacketCGMessengerAddByVID);
				}

				if (ch->GetDesc() == d) // 자신은 추가할 수 없다.
					return sizeof(TPacketCGMessengerAddByVID);

				MessengerManager::instance().RequestToAdd(ch, ch_companion);
				//MessengerManager::instance().AddToList(ch->GetName(), ch_companion->GetName());
			}
			return sizeof(TPacketCGMessengerAddByVID);

		case MESSENGER_SUBHEADER_CG_ADD_BY_NAME:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(name, c_pData, sizeof(name));

				if (ch->GetGMLevel() == GM_PLAYER && gm_get_level(name) != GM_PLAYER)
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 184, "");
					return CHARACTER_NAME_MAX_LEN;
				}

				LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);
				if (tch)
				{
					if (tch == ch) // 자신은 추가할 수 없다.
						return CHARACTER_NAME_MAX_LEN;

					if (tch->IsBlockMode(BLOCK_MESSENGER_INVITE) == true)
					{
						ch->ChatPacketNew(CHAT_TYPE_INFO, 370, "%s", tch->GetName());
					}
					else
					{
						// 메신저가 캐릭터단위가 되면서 변경
						MessengerManager::instance().RequestToAdd(ch, tch);
						//MessengerManager::instance().AddToList(ch->GetName(), tch->GetName());
					}
				}
				else {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 108, "%s", name);
				}
			}
			return CHARACTER_NAME_MAX_LEN;

		case MESSENGER_SUBHEADER_CG_REMOVE:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char char_name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(char_name, c_pData, sizeof(char_name));
				MessengerManager::instance().RemoveFromList(ch->GetName(), char_name);
				MessengerManager::instance().RemoveFromList(char_name, ch->GetName());
			}
			return CHARACTER_NAME_MAX_LEN;
#if defined(ENABLE_MESSENGER_BLOCK)
		case MESSENGER_SUBHEADER_CG_ADD_BLOCK_BY_VID: {
			if (uiBytes < sizeof(TPacketCGMessengerAddBlockByVID)) {
				return -1;
			}

			const TPacketCGMessengerAddBlockByVID* p2 = reinterpret_cast<const TPacketCGMessengerAddBlockByVID*>(c_pData);
			const LPCHARACTER ch_companion = CHARACTER_MANAGER::instance().Find(p2->vid);

			if (!ch_companion) {
				return sizeof(TPacketCGMessengerAddBlockByVID);
			}

			if (ch->IsObserverMode()) {
				return sizeof(TPacketCGMessengerAddBlockByVID);
			}

			if (ch_companion == ch)
				return sizeof(TPacketCGMessengerAddBlockByVID);

			if(!ch_companion->IsPC())
				return sizeof(TPacketCGMessengerAddBlockByVID);

			const LPDESC d = ch_companion->GetDesc();

			if (!d) {
				return sizeof(TPacketCGMessengerAddByVID);
			}

			if (ch->GetDesc() == d) {
				return sizeof(TPacketCGMessengerAddBlockByVID);
			}

/*
			if (ch->GetGuild() != NULL && 
				ch_companion->GetGuild() != NULL &&
				ch_companion->GetGuild() == ch->GetGuild()) {
				ch->ChatPacketNew(CHAT_TYPE_INFO, 1435, "");
				return sizeof(TPacketCGMessengerAddBlockByVID);
			}
*/

			if (MessengerManager::instance().CheckMessengerList(ch->GetName(), ch_companion->GetName(), SYST_FRIEND)) {
				ch->ChatPacketNew(CHAT_TYPE_INFO, 1436, "%s", ch_companion->GetName());
				return sizeof(TPacketCGMessengerAddBlockByVID);
			}

			if (MessengerManager::instance().CheckMessengerList(ch->GetName(), ch_companion->GetName(), SYST_BLOCK)) {
				ch->ChatPacketNew(CHAT_TYPE_INFO, 1437, "%s", ch_companion->GetName());
				return sizeof(TPacketCGMessengerAddBlockByVID);
			}

			if (!test_server) {
				if (ch_companion->GetGMLevel() != GM_PLAYER) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 1433, "");
					return sizeof(TPacketCGMessengerAddByVID);
				}
			}

			MessengerManager::instance().AddToBlockList(ch->GetName(), ch_companion->GetName());
			return sizeof(TPacketCGMessengerAddBlockByVID);
		}
		case MESSENGER_SUBHEADER_CG_ADD_BLOCK_BY_NAME: {
			if (uiBytes < CHARACTER_NAME_MAX_LEN) {
				return -1;
			}

			char name[CHARACTER_NAME_MAX_LEN + 1];
			strlcpy(name, c_pData, sizeof(name));

			if (!test_server) {
				if (gm_get_level(name) != GM_PLAYER) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 1433, "");
					return CHARACTER_NAME_MAX_LEN;
				}
			}

			const LPCHARACTER pkTargetChar = CHARACTER_MANAGER::instance().FindPC(name);

			if (!pkTargetChar) {
				ch->ChatPacketNew(CHAT_TYPE_INFO, 1434, "%s", name);
			} else {
				if (pkTargetChar == ch) {
					return CHARACTER_NAME_MAX_LEN;
				}

/*
				if (ch->GetGuild() != NULL && 
					pkTargetChar->GetGuild() != NULL &&
					pkTargetChar->GetGuild() == ch->GetGuild()) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 1435, "");
					return CHARACTER_NAME_MAX_LEN;
				}
*/

				if (MessengerManager::instance().CheckMessengerList(ch->GetName(), pkTargetChar->GetName(), SYST_FRIEND)) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 1436, "%s", pkTargetChar->GetName());
					return CHARACTER_NAME_MAX_LEN;
				}

				MessengerManager::instance().AddToBlockList(ch->GetName(), pkTargetChar->GetName());
			}

			return CHARACTER_NAME_MAX_LEN;
		}
		case MESSENGER_SUBHEADER_CG_REMOVE_BLOCK: {
			if (uiBytes < CHARACTER_NAME_MAX_LEN) {
				return -1;
			}

			char char_name[CHARACTER_NAME_MAX_LEN + 1];
			strlcpy(char_name, c_pData, sizeof(char_name));

			if (!MessengerManager::instance().CheckMessengerList(ch->GetName(), char_name, SYST_BLOCK)) {
				ch->ChatPacketNew(CHAT_TYPE_INFO, 1432, "%s", char_name);
				return CHARACTER_NAME_MAX_LEN;
			}

			MessengerManager::instance().RemoveFromBlockList(ch->GetName(), char_name);

			return CHARACTER_NAME_MAX_LEN;
		}
#endif
		default: {
			sys_err("CInputMain::Messenger : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
		}
	}

	return 0;
}

int CInputMain::Shop(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	TPacketCGShop * p = (TPacketCGShop *) data;

	if (uiBytes < sizeof(TPacketCGShop))
		return -1;

	if (test_server)
		sys_log(0, "CInputMain::Shop() ==> SubHeader %d", p->subheader);

	const char * c_pData = data + sizeof(TPacketCGShop);
	uiBytes -= sizeof(TPacketCGShop);

	switch (p->subheader)
	{
		case SHOP_SUBHEADER_CG_END:
			sys_log(1, "INPUT: %s SHOP: END", ch->GetName());
			CShopManager::instance().StopShopping(ch);
			return 0;

		case SHOP_SUBHEADER_CG_BUY:
			{
				if (uiBytes < sizeof(BYTE) + sizeof(BYTE))
					return -1;

				BYTE bPos = *(c_pData + 1);
				sys_log(1, "INPUT: %s SHOP: BUY %d", ch->GetName(), bPos);
				CShopManager::instance().Buy(ch, bPos);
				return (sizeof(BYTE) + sizeof(BYTE));
			}
#ifndef ENABLE_EXTRA_INVENTORY
		case SHOP_SUBHEADER_CG_SELL:
			{
				if (uiBytes < sizeof(BYTE))
					return -1;

				BYTE pos = *c_pData;

				sys_log(0, "INPUT: %s SHOP: SELL", ch->GetName());
				CShopManager::instance().Sell(ch, pos);
				return sizeof(BYTE);
			}
#endif
		case SHOP_SUBHEADER_CG_SELL2:
			{
				if (uiBytes < sizeof(BYTE)
#ifdef ENABLE_EXTRA_INVENTORY
				+ sizeof(WORD) 
#else
				+ sizeof(BYTE) 
#endif
				+ sizeof(WORD))
					return -1;
				
#ifdef ENABLE_EXTRA_INVENTORY
				BYTE window = *(c_pData);
				WORD cell = *(WORD*)(c_pData + 1);
#else
				BYTE pos = *(c_pData++);
#endif
				WORD count = *(WORD*)(c_pData + sizeof(WORD));

				sys_log(0, "INPUT: %s SHOP: SELL2", ch->GetName());
				CShopManager::instance().Sell(ch,
#ifdef ENABLE_EXTRA_INVENTORY
				TItemPos(window, cell), 
#else
				pos, 
#endif
				count);
				return sizeof(BYTE) 
#ifdef ENABLE_EXTRA_INVENTORY
				+ sizeof(WORD) 
#else
				+ sizeof(BYTE) 
#endif
				+ sizeof(WORD);
			}
#ifdef ENABLE_BUY_STACK_FROM_SHOP
		case SHOP_SUBHEADER_CG_BUY2:
			{
				size_t size = sizeof(uint8_t) + sizeof(uint8_t);
				if (uiBytes < size) {
					return -1;
				}

				uint8_t p = *(c_pData++);
				uint8_t c = *(c_pData);
				sys_log(1, "INPUT: %s SHOP: MULTIPLE BUY %d COUNT %d", ch->GetName(), p, c);
				CShopManager::instance().MultipleBuy(ch, p, c);
				return size;
			}
#endif
		default:
			sys_err("CInputMain::Shop : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}

void CInputMain::OnClick(LPCHARACTER ch, const char * data)
{
	struct command_on_click *	pinfo = (struct command_on_click *) data;
	LPCHARACTER			victim;

	if ((victim = CHARACTER_MANAGER::instance().Find(pinfo->vid)))
		victim->OnClick(ch);
	else if (test_server)
	{
		sys_err("CInputMain::OnClick %s.Click.NOT_EXIST_VID[%d]", ch->GetName(), pinfo->vid);
	}
}

#if defined(ENABLE_GAYA_RENEWAL)
int CInputMain::GemShop(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	TPacketCGGemShop * p = (TPacketCGGemShop *) data;

	if (uiBytes < sizeof(TPacketCGGemShop))
		return -1;

	const char * c_pData = data + sizeof(TPacketCGGemShop);
	uiBytes -= sizeof(TPacketCGGemShop);

	switch (p->subheader)
	{
		case GEM_SHOP_SUBHEADER_CG_BUY:
			{
				if (uiBytes < sizeof(BYTE))
					return -1;

				BYTE bPos = *c_pData;

				sys_log(1, "INPUT: %s GEM_SHOP: BUY %d", ch->GetName(), bPos);
				ch->GemShopBuy(bPos);
				return sizeof(BYTE);
			}
			
		case GEM_SHOP_SUBHEADER_CG_ADD:
			{
				if (uiBytes < sizeof(BYTE))
					return -1;

				BYTE bPos = *c_pData;

				sys_log(1, "INPUT: %s GEM_SHOP: ADD %d", ch->GetName(), bPos);
				ch->GemShopAdd(bPos);
				return sizeof(BYTE);
			}
			
		case GEM_SHOP_SUBHEADER_CG_REFRESH:
			sys_log(1, "INPUT: %s GEM_SHOP: REFRESH", ch->GetName());
			ch->RefreshGemShopWithItem();
			return 0;
			
		default:
			sys_err("CInputMain::GemShop() : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}
#endif

void CInputMain::Exchange(LPCHARACTER ch, const char * data)
{
	struct command_exchange * pinfo = (struct command_exchange *) data;
	LPCHARACTER	to_ch = NULL;

	if (!ch->CanHandleItem())
		return;

	const auto qc = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	if (!qc || qc->IsRunning())
		return;

#ifdef __ENABLE_NEW_OFFLINESHOP__
	if(ch->GetOfflineShopGuest() || ch->GetAuctionGuest())
		return;
#endif

	int iPulse = thecore_pulse();

	if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
	{
		if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			to_ch->ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", g_nPortalLimitTime);
			return;
		}

		if( true == to_ch->IsDead() )
		{
			return;
		}
	}

	sys_log(0, "CInputMain()::Exchange()  SubHeader %d ", pinfo->sub_header);

	if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", g_nPortalLimitTime);
		return;
	}


	switch (pinfo->sub_header)
	{
		case EXCHANGE_SUBHEADER_CG_START:	// arg1 == vid of target character
			if (!ch->GetExchange())
			{
				if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
				{
#if defined(ENABLE_RESTRICT_STAFF_RESTRICTIONS)
					uint8_t gm_level = ch->GetGMLevel();

					if (test_server == false && ((gm_level > GM_PLAYER && gm_level < GM_IMPLEMENTOR) ||
												(gm_level != GM_IMPLEMENTOR && to_ch->GetGMLevel() > GM_PLAYER && to_ch->GetGMLevel() < GM_IMPLEMENTOR))
					) {
						ch->ChatPacketNew(CHAT_TYPE_INFO, 1543, "");
						return;
					}
#endif

					if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
						ch->ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", g_nPortalLimitTime);
						return;
					}

					if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
						to_ch->ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", g_nPortalLimitTime);
						return;
					}

#if defined(ENABLE_NEW_GOLD_LIMIT)
					if (ch->GetGold() >= GOLD_MAX_MAX) {
						ch->ChatPacketNew(CHAT_TYPE_INFO, 406, "%llu", GOLD_MAX_MAX);
						return;
					}
#else
					if (ch->GetGold() >= GOLD_MAX) {
						ch->ChatPacketNew(CHAT_TYPE_INFO, 406, "%d", GOLD_MAX);
						return;
					}
#endif

					if (to_ch->IsPC())
					{
						if (quest::CQuestManager::instance().GiveItemToPC(ch->GetPlayerID(), to_ch))
						{
							sys_log(0, "Exchange canceled by quest %s %s", ch->GetName(), to_ch->GetName());
							return;
						}
					}


					if (ch->GetMyShop() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen()
#ifdef ENABLE_ACCE_SYSTEM
 || ch->IsAcceOpen()
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
 || ch->GetOfflineShopGuest() || ch->GetAuctionGuest()
#endif
#if defined(ENABLE_AURA_SYSTEM)
 || ch->IsAuraRefineWindowOpen()
#endif
#ifdef __ATTR_TRANSFER_SYSTEM__
 || ch->IsAttrTransferOpen()
#endif
#if defined(ENABLE_CHANGELOOK)
 || ch->isChangeLookOpened()
#endif
#if defined(USE_ATTR_6TH_7TH)
 || ch->IsOpenAttr67Add()
#endif
					)
					{
						ch->ChatPacketNew(CHAT_TYPE_INFO, 292, "");
						return;
					}

					ch->ExchangeStart(to_ch);
				}
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ITEM_ADD:	// arg1 == position of item, arg2 == position in exchange window
			if (ch->GetExchange())
			{
				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
#if defined(ITEM_CHECKINOUT_UPDATE)
					ch->GetExchange()->AddItem(pinfo->Pos, pinfo->arg2, pinfo->SelectPosAuto);
#else
					ch->GetExchange()->AddItem(pinfo->Pos, pinfo->arg2);
#endif
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ITEM_DEL:	// arg1 == position of item
			if (ch->GetExchange())
			{
				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->RemoveItem(pinfo->arg1);
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ELK_ADD:	// arg1 == amount of gold
			if (ch->GetExchange())
			{
#if defined(ENABLE_NEW_GOLD_LIMIT)
				const uint64_t nTotalGold = static_cast<uint64_t>(ch->GetExchange()->GetCompany()->GetOwner()->GetGold()) + static_cast<uint64_t>(pinfo->arg1);
				if (GOLD_MAX_MAX <= nTotalGold)
#else
				const uint32_t nTotalGold = static_cast<uint32_t>(ch->GetExchange()->GetCompany()->GetOwner()->GetGold()) + static_cast<uint32_t>(pinfo->arg1);
				if (GOLD_MAX <= nTotalGold)
#endif
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 226,
#if defined(ENABLE_NEW_GOLD_LIMIT)
					"%llu"
#else
					"%lu"
#endif
					, nTotalGold);
					return;
				}

				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
				{
					ch->GetExchange()->AddGold(pinfo->arg1);
				}
			}
			break;
		case EXCHANGE_SUBHEADER_CG_ACCEPT:	// arg1 == not used
			if (ch->GetExchange())
			{
				sys_log(0, "CInputMain()::Exchange() ==> ACCEPT ");
				ch->GetExchange()->Accept(true);
			}

			break;

		case EXCHANGE_SUBHEADER_CG_CANCEL:	// arg1 == not used
			if (ch->GetExchange())
			{
				ch->GetExchange()->Cancel();
				ch->SetExchange(nullptr);
			}
			break;
	}
}

void CInputMain::Position(LPCHARACTER ch, const char * data)
{
	struct command_position * pinfo = (struct command_position *) data;

	switch (pinfo->position)
	{
		case POSITION_GENERAL:
			ch->Standup();
			break;

		case POSITION_SITTING_CHAIR:
			ch->Sitdown(0);
			break;

		case POSITION_SITTING_GROUND:
			ch->Sitdown(1);
			break;
	}
}

static const int ComboSequenceBySkillLevel[3][8] =
{
	// 0   1   2   3   4   5   6   7
	{ 14, 15, 16, 17,  0,  0,  0,  0 },
	{ 14, 15, 16, 18, 20,  0,  0,  0 },
	{ 14, 15, 16, 18, 19, 17,  0,  0 },
};

void CInputMain::Move(LPCHARACTER ch, const char * data)
{
	if (!ch)
		return;

	if (!ch->CanMove())
		return;

	struct command_move * pinfo = (struct command_move *) data;
#if defined(ENABLE_NEW_ANTICHEAT_RULES)
	uint32_t a = 55184705, b = 85277383, c = pinfo->uX, d = pinfo->uY;
	uint32_t verify_x = pinfo->lX ^ a;
	verify_x += b;
	verify_x >>= 3;
	verify_x |= 0xAA11;
	verify_x ^= b;

	if (verify_x != c) {
		ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
		ch->Stop();
		return;
	}

	uint32_t verify_y = pinfo->lY ^ a;
	verify_y += b;
	verify_y >>= 3;
	verify_y |= 0xAA11;
	verify_y ^= b;

	if(verify_y != d)
	{
		ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
		ch->Stop();
		return;
	}
#endif

#ifdef USE_CAPTCHA_SYSTEM
    if (ch->IsWaitingForCaptcha())
    {
        ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
        ch->Stop();
        return;
    }
#endif

	if (pinfo->bFunc >= FUNC_MAX_NUM && !(pinfo->bFunc & 0x80))
	{
		sys_err("invalid move type: %s", ch->GetName());
		return;
	}

	//enum EMoveFuncType
	//{
	//	FUNC_WAIT,
	//	FUNC_MOVE,
	//	FUNC_ATTACK,
	//	FUNC_COMBO,
	//	FUNC_MOB_SKILL,
	//	_FUNC_SKILL,
	//	FUNC_MAX_NUM,
	//	FUNC_SKILL = 0x80,
	//};

	// 텔레포트 핵 체크

//	if (!test_server)
	{
		const float fDist = DISTANCE_SQRT((ch->GetX() - pinfo->lX) / 100, (ch->GetY() - pinfo->lY) / 100);
		if (((false == ch->IsRiding() && fDist > 30) || fDist > 60) && OXEVENT_MAP_INDEX != ch->GetMapIndex())
		{
			sys_log(0, "MOVE: %s trying to move too far (dist: %.1fm) Riding(%d)", ch->GetName(), fDist, ch->IsRiding());

			ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
			ch->Stop();
			return;
		}

#ifdef ENABLE_CHECK_GHOSTMODE
		if (ch->IsPC() && ch->IsDead())
		{
			sys_log(0, "MOVE: %s trying to move as dead", ch->GetName());

			ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
			ch->Stop();
			return;
		}
#endif

#ifdef __FIX_PRO_DAMAGE__
		if (OXEVENT_MAP_INDEX != ch->GetMapIndex() && ch->CheckSyncPosition(true))
		{
			sys_log(0, "#(HACK)# (%s) sync_check error", ch->GetName());
			ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
			ch->Stop();
			return;
		}
#endif

		DWORD dwCurTime = get_dword_time();
		if (ch->GetDesc()) {
			bool CheckSpeedHack = (false == ch->GetDesc()->IsHandshaking() && dwCurTime - ch->GetDesc()->GetClientTime() > 7000);
			if (CheckSpeedHack)
			{
				int iDelta = (int)(dwCurTime - pinfo->dwTime);
				int iServerDelta = (int)(dwCurTime - ch->GetDesc()->GetClientTime());
				if (iDelta >= 30000) {
					sys_log(0, "SPEEDHACK: slow timer name %s delta %d", ch->GetName(), iDelta);
					ch->GetDesc()->DelayedDisconnect(3);
				} else if (iDelta < -(iServerDelta / 50)) {
					sys_log(0, "SPEEDHACK: DETECTED! %s (delta %d %d)", ch->GetName(), iDelta, iServerDelta);
					ch->GetDesc()->DelayedDisconnect(3);
				}
			}

			//if (pinfo->bFunc == FUNC_COMBO && g_bCheckMultiHack)
			//{
			//	CheckComboHack(ch, pinfo->bArg, pinfo->dwTime, CheckSpeedHack); // 콤보 체크
			//}
		}
	}

	if (pinfo->bFunc == FUNC_MOVE)
	{
		if (ch->GetLimitPoint(POINT_MOV_SPEED) == 0)
			return;

		ch->SetRotation(pinfo->bRot * 5);	// 중복 코드
		ch->ResetStopTime();				// ""

		ch->Goto(pinfo->lX, pinfo->lY);
	}
	else
	{
		if (pinfo->bFunc == FUNC_ATTACK || pinfo->bFunc == FUNC_COMBO)
			ch->OnMove(true);
		else if (pinfo->bFunc & FUNC_SKILL)
		{
			const int MASK_SKILL_MOTION = 0x7F;
			unsigned int motion = pinfo->bFunc & MASK_SKILL_MOTION;

			if (!ch->IsUsableSkillMotion(motion))
			{
				ch->GetDesc()->DelayedDisconnect(number(150, 500));
			}

			ch->OnMove();
		}

		ch->SetRotation(pinfo->bRot * 5);
		ch->ResetStopTime();

#ifdef __FIX_PRO_DAMAGE__
		if (OXEVENT_MAP_INDEX != ch->GetMapIndex())
		{
			int fDist = DISTANCE_SQRT((ch->GetX() - pinfo->lX) / 100, (ch->GetY() - pinfo->lY) / 100);
			ch->SetSyncPosition(pinfo->lX, pinfo->lY);

			if (((false == ch->IsRiding() && fDist > 50) || fDist > 65)) {
//			if (((false == ch->IsRiding() && fDist > 25) || fDist > 40)) {
				sys_log(0, "#(HACK)# (%s) sync fDist = %d, limit = 16", ch->GetName(), fDist);
				ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
				ch->Stop();
				return;
			}

			int il = 0;
			if (((false == ch->IsRiding() && fDist > 40) || fDist > 55)) {
//			if (((false == ch->IsRiding() && fDist > 20) || fDist > 35)) {
				il = DISTANCE_SQRT((ch->GetX() - pinfo->lX) / 100, (ch->GetY() - pinfo->lY) / 100) * 3;
			}

			ch->CheckSyncPosition() ? il += 1 : il += 3;
			ch->SetSyncCount(il);
		}
#endif

		ch->Move(pinfo->lX, pinfo->lY);
		ch->Stop();
		ch->StopStaminaConsume();
	}

	TPacketGCMove pack;

	pack.bHeader      = HEADER_GC_MOVE;
	pack.bFunc        = pinfo->bFunc;
	pack.bArg         = pinfo->bArg;
	pack.bRot         = pinfo->bRot;
	pack.dwVID        = ch->GetVID();
	pack.lX           = pinfo->lX;
	pack.lY           = pinfo->lY;
	pack.dwTime       = pinfo->dwTime;
	pack.dwDuration   = (pinfo->bFunc == FUNC_MOVE) ? ch->GetCurrentMoveDuration() : 0;

	ch->PacketAround(&pack, sizeof(TPacketGCMove), ch);
/*
	if (pinfo->dwTime == 10653691) // 디버거 발견
	{
		if (ch->GetDesc()->DelayedDisconnect(number(15, 30)))
			LogManager::instance().HackLog("Debugger", ch);

	}
	else if (pinfo->dwTime == 10653971) // Softice 발견
	{
		if (ch->GetDesc()->DelayedDisconnect(number(15, 30)))
			LogManager::instance().HackLog("Softice", ch);
	}
*/
	/*
	sys_log(0,
			"MOVE: %s Func:%u Arg:%u Pos:%dx%d Time:%u Dist:%.1f",
			ch->GetName(),
			pinfo->bFunc,
			pinfo->bArg,
			pinfo->lX / 100,
			pinfo->lY / 100,
			pinfo->dwTime,
			fDist);
	*/
}

#ifdef __SKILL_COLOR_SYSTEM__
void CInputMain::SetSkillColor(LPCHARACTER ch, const char* pcData)
{
	if (!ch)
		return;
	
	TPacketCGSkillColor * p = (TPacketCGSkillColor*)pcData;
	if (p->skill >= ESkillColorLength::MAX_SKILL_COUNT)
		return;
	
	if ((p->col1 != 0) || (p->col2 != 0) || (p->col3 != 0) || (p->col4 != 0) || (p->col5 != 0)) {
		if (ch->CountSpecifyItem(164406) < 1) {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 16, "");
			return;
		} else {
			ch->RemoveSpecifyItem(164406, 1);
		}
	}
	
	DWORD data[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
	memcpy(data, ch->GetSkillColor(), sizeof(data));

	data[p->skill][0] = p->col1;
	data[p->skill][1] = p->col2;
	data[p->skill][2] = p->col3;
	data[p->skill][3] = p->col4;
	data[p->skill][4] = p->col5;

	ch->ChatPacketNew(CHAT_TYPE_INFO, 15, "");

	ch->SetSkillColor(data[0]);

	TSkillColor db_pack;
	memcpy(db_pack.dwSkillColor, data, sizeof(data));
	db_pack.player_id = ch->GetPlayerID();
	db_clientdesc->DBPacketHeader(HEADER_GD_SKILL_COLOR_SAVE, 0, sizeof(TSkillColor));
	db_clientdesc->Packet(&db_pack, sizeof(TSkillColor));
}
#endif

void CInputMain::Attack(LPCHARACTER ch, const BYTE header, const char* data)
{
	if (NULL == ch)
		return;

	struct type_identifier
	{
		BYTE header;
		BYTE type;
	};

	const struct type_identifier* const type = reinterpret_cast<const struct type_identifier*>(data);

#if defined(ENABLE_RENEWAL_OX)
	if (ch->GetMapIndex() == OX_MAP_INDEX) {
		if (ch->GetGMLevel() == GM_PLAYER) {
			return;
		}
	}
#endif

	if (type->type > 0)
	{
		if (false == ch->CanUseSkill(type->type))
		{
			return;
		}

		switch (type->type)
		{
			case SKILL_GEOMPUNG:
			case SKILL_SANGONG:
			case SKILL_YEONSA:
			case SKILL_KWANKYEOK:
			case SKILL_HWAJO:
			case SKILL_GIGUNG:
			case SKILL_PABEOB:
			case SKILL_MARYUNG:
			case SKILL_TUSOK:
			case SKILL_MAHWAN:
			case SKILL_BIPABU:
			case SKILL_NOEJEON:
			case SKILL_CHAIN:
			case SKILL_HORSE_WILDATTACK_RANGE:
				if (HEADER_CG_SHOOT != type->header)
				{
					return;
				}
				break;
		}
	}

	switch (header)
	{
		case HEADER_CG_ATTACK: {
			if (NULL == ch->GetDesc())
			{
				return;
			}

			const TPacketCGAttack* const packMelee = reinterpret_cast<const TPacketCGAttack*>(data);

#if defined(ENABLE_NEW_ANTICHEAT_RULES)
			uint32_t a = 429235637, b = 796398961;

			uint32_t verify_vid = packMelee->dwVID ^ a;
			verify_vid += b;
			verify_vid >>= 3;
			verify_vid |= 0xAA11;
			verify_vid ^= b;

			if(verify_vid != packMelee->dwVictimVIDVerify) {
				ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
				ch->Stop();
				return;
			}
#endif

#ifdef USE_CAPTCHA_SYSTEM
            if (ch->IsWaitingForCaptcha())
            {
                ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
                ch->Stop();
                return;
            }
#endif

			const LPDESC d = ch->GetDesc();
			if (d) {
				d->AssembleCRCMagicCube(packMelee->bCRCMagicCubeProcPiece, packMelee->bCRCMagicCubeFilePiece);
			}

			LPCHARACTER	victim = CHARACTER_MANAGER::instance().Find(packMelee->dwVID);

			if (NULL == victim || ch == victim) {
				return;
			}

			if (!ch->IsVictimInView(victim) || victim->GetMapIndex() != ch->GetMapIndex()) {
				if (test_server)
					ch->ChatPacket(CHAT_TYPE_INFO, "You cannot attack %u (%s) because not in view map", static_cast<uint32_t>(victim->GetVID()), victim->GetName());

				return;
			}

			switch (victim->GetCharType())
			{
				case CHAR_TYPE_NPC:
				case CHAR_TYPE_WARP:
				case CHAR_TYPE_GOTO:
					return;
			}

			if (packMelee->bType > 0)
			{
				if (false == ch->CheckSkillHitCount(packMelee->bType, victim->GetVID()))
				{
					return;
				}
			}
			else if (packMelee->bType == 0)
			{
				const float fHittingDistance = DISTANCE_APPROX((victim->GetX() - ch->GetX()) / 100, (victim->GetY() - ch->GetY()) / 100);
				static const float fMaximumHittingDistance = 7.0f;
				if (fHittingDistance > fMaximumHittingDistance)
				{
					if (test_server)
						ch->ChatPacket(CHAT_TYPE_INFO, "You cannot hit %u (%s) because is too far", static_cast<uint32_t>(victim->GetVID()), victim->GetName());
					return;
				}
			}

			ch->Attack(victim, packMelee->bType);

			break;
		}
		case HEADER_CG_SHOOT: {
			const TPacketCGShoot* const packShoot = reinterpret_cast<const TPacketCGShoot*>(data);

			ch->Shoot(packShoot->bType);
			break;
		}
		default: {
			break;
		}
	}
}

int CInputMain::SyncPosition(LPCHARACTER ch, const char * c_pcData, size_t uiBytes)
{
	const TPacketCGSyncPosition* pinfo = reinterpret_cast<const TPacketCGSyncPosition*>( c_pcData );

	if (uiBytes < pinfo->wSize)
		return -1;

	int iExtraLen = pinfo->wSize - sizeof(TPacketCGSyncPosition);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	if (0 != (iExtraLen % sizeof(TPacketCGSyncPositionElement)))
	{
		sys_err("invalid packet length %d (name: %s)", pinfo->wSize, ch->GetName());
		return iExtraLen;
	}

	int iCount = iExtraLen / sizeof(TPacketCGSyncPositionElement);

	if (iCount <= 0)
		return iExtraLen;

	static const int nCountLimit = 60;

	if( iCount > nCountLimit )
	{
		//LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );
		sys_err( "Too many SyncPosition Count(%d) from Name(%s)", iCount, ch->GetName() );
		//ch->GetDesc()->SetPhase(PHASE_CLOSE);
		//return -1;
		iCount = nCountLimit;
	}

	TEMP_BUFFER tbuf;
	LPBUFFER lpBuf = tbuf.getptr();

	TPacketGCSyncPosition * pHeader = (TPacketGCSyncPosition *) buffer_write_peek(lpBuf);
	buffer_write_proceed(lpBuf, sizeof(TPacketGCSyncPosition));

	const TPacketCGSyncPositionElement* e =
		reinterpret_cast<const TPacketCGSyncPositionElement*>(c_pcData + sizeof(TPacketCGSyncPosition));

	timeval tvCurTime;
	gettimeofday(&tvCurTime, NULL);

	for (int i = 0; i < iCount; ++i, ++e)
	{
		LPCHARACTER victim = CHARACTER_MANAGER::instance().Find(e->dwVID);

		if (!victim)
			continue;

		switch (victim->GetCharType())
		{
			case CHAR_TYPE_NPC:
			case CHAR_TYPE_WARP:
			case CHAR_TYPE_GOTO:
				continue;
		}

		// 소유권 검사
		if (!victim->SetSyncOwner(ch))
			continue;

		const float fDistWithSyncOwner = DISTANCE_SQRT( (victim->GetX() - ch->GetX()) / 100, (victim->GetY() - ch->GetY()) / 100 );
		static const float fLimitDistWithSyncOwner = 2500.f + 1000.f;
		// victim과의 거리가 2500 + a 이상이면 핵으로 간주.
		//	거리 참조 : 클라이언트의 __GetSkillTargetRange, __GetBowRange 함수
		//	2500 : 스킬 proto에서 가장 사거리가 긴 스킬의 사거리, 또는 활의 사거리
		//	a = POINT_BOW_DISTANCE 값... 인데 실제로 사용하는 값인지는 잘 모르겠음. 아이템이나 포션, 스킬, 퀘스트에는 없는데...
		//		그래도 혹시나 하는 마음에 버퍼로 사용할 겸해서 1000.f 로 둠...
		if (fDistWithSyncOwner > fLimitDistWithSyncOwner)
		{
			// g_iSyncHackLimitCount번 까지는 봐줌.
			if (ch->GetSyncHackCount() < 60)
			{
				ch->SetSyncHackCount(ch->GetSyncHackCount() + 1);
				continue;
			}
			else
			{
				LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

				sys_err( "Too far SyncPosition DistanceWithSyncOwner(%f)(%s) from Name(%s) CH(%d,%d) VICTIM(%d,%d) SYNC(%d,%d)",
					fDistWithSyncOwner, victim->GetName(), ch->GetName(), ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY(),
					e->lX, e->lY );

				ch->GetDesc()->SetPhase(PHASE_CLOSE);

				return -1;
			}
		}

		const float fDist = DISTANCE_SQRT( (victim->GetX() - e->lX) / 100, (victim->GetY() - e->lY) / 100 );
		static const long g_lValidSyncInterval = 100 * 1000; // 100ms
		const timeval &tvLastSyncTime = victim->GetLastSyncTime();
		timeval *tvDiff = timediff(&tvCurTime, &tvLastSyncTime);

		// SyncPosition을 악용하여 타유저를 이상한 곳으로 보내는 핵 방어하기 위하여,
		// 같은 유저를 g_lValidSyncInterval ms 이내에 다시 SyncPosition하려고 하면 핵으로 간주.
		if (tvDiff->tv_sec == 0 && tvDiff->tv_usec < g_lValidSyncInterval)
		{
			// g_iSyncHackLimitCount번 까지는 봐줌.
			if (ch->GetSyncHackCount() < 60)
			{
				ch->SetSyncHackCount(ch->GetSyncHackCount() + 1);
				continue;
			}
			else
			{
				LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

				sys_err( "Too often SyncPosition Interval(%ldms)(%s) from Name(%s) VICTIM(%d,%d) SYNC(%d,%d)",
					tvDiff->tv_sec * 1000 + tvDiff->tv_usec / 1000, victim->GetName(), ch->GetName(), victim->GetX(), victim->GetY(),
					e->lX, e->lY );

				ch->GetDesc()->SetPhase(PHASE_CLOSE);

				return -1;
			}
		}
		else if( fDist > 40.0f )
		{
			LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

			sys_err( "Too far SyncPosition Distance(%f)(%s) from Name(%s) CH(%d,%d) VICTIM(%d,%d) SYNC(%d,%d)",
				   	fDist, victim->GetName(), ch->GetName(), ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY(),
				  e->lX, e->lY );

			ch->GetDesc()->SetPhase(PHASE_CLOSE);

			return -1;
		}
		else
		{
			victim->SetLastSyncTime(tvCurTime);
			victim->Sync(e->lX, e->lY);
			buffer_write(lpBuf, e, sizeof(TPacketCGSyncPositionElement));
		}
	}

	if (buffer_size(lpBuf) != sizeof(TPacketGCSyncPosition))
	{
		pHeader->bHeader = HEADER_GC_SYNC_POSITION;
		pHeader->wSize = buffer_size(lpBuf);

		ch->PacketAround(buffer_read_peek(lpBuf), buffer_size(lpBuf), ch);
	}

	return iExtraLen;
}

void CInputMain::FlyTarget(LPCHARACTER ch, const char * pcData, BYTE bHeader)
{
	TPacketCGFlyTargeting * p = (TPacketCGFlyTargeting *) pcData;
	ch->FlyTarget(p->dwTargetVID, p->x, p->y, bHeader);
}

void CInputMain::UseSkill(LPCHARACTER ch, const char * pcData)
{
	if (!ch) {
		return;
	}

	TPacketCGUseSkill * p = (TPacketCGUseSkill *) pcData;
	ch->UseSkill(p->dwVnum, CHARACTER_MANAGER::instance().Find(p->dwVID));
}

void CInputMain::ScriptButton(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptButton * p = (TPacketCGScriptButton *) c_pData;
	sys_log(0, "QUEST ScriptButton pid %d idx %u", ch->GetPlayerID(), p->idx);

	quest::PC* pc = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	if (pc && pc->IsConfirmWait())
	{
		quest::CQuestManager::instance().Confirm(ch->GetPlayerID(), quest::CONFIRM_TIMEOUT);
	}
	else if (p->idx & 0x80000000)
	{
		//퀘스트 창에서 클릭시(__SelectQuest) 여기로
		quest::CQuestManager::Instance().QuestInfo(ch->GetPlayerID(), p->idx & 0x7fffffff);
	}
	else
	{
		quest::CQuestManager::Instance().QuestButton(ch->GetPlayerID(), p->idx);
	}
}

void CInputMain::ScriptAnswer(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptAnswer * p = (TPacketCGScriptAnswer *) c_pData;
	sys_log(0, "QUEST ScriptAnswer pid %d answer %d", ch->GetPlayerID(), p->answer);

	if (p->answer > 250) // 다음 버튼에 대한 응답으로 온 패킷인 경우
	{
		quest::CQuestManager::Instance().Resume(ch->GetPlayerID());
	}
	else // 선택 버튼을 골라서 온 패킷인 경우
	{
		quest::CQuestManager::Instance().Select(ch->GetPlayerID(),  p->answer);
	}
}


// SCRIPT_SELECT_ITEM
void CInputMain::ScriptSelectItem(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptSelectItem* p = (TPacketCGScriptSelectItem*) c_pData;
	sys_log(0, "QUEST ScriptSelectItem pid %d answer %d", ch->GetPlayerID(), p->selection);
	quest::CQuestManager::Instance().SelectItem(ch->GetPlayerID(), p->selection);
}
// END_OF_SCRIPT_SELECT_ITEM

void CInputMain::QuestInputString(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestInputString * p = (TPacketCGQuestInputString*) c_pData;

	char msg[65];
	strlcpy(msg, p->msg, sizeof(msg));
	sys_log(0, "QUEST InputString pid %u msg %s", ch->GetPlayerID(), msg);

	quest::CQuestManager::Instance().Input(ch->GetPlayerID(), msg);
}

void CInputMain::QuestConfirm(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestConfirm* p = (TPacketCGQuestConfirm*) c_pData;
	LPCHARACTER ch_wait = CHARACTER_MANAGER::instance().FindByPID(p->requestPID);
	if (p->answer)
		p->answer = quest::CONFIRM_YES;
	sys_log(0, "QuestConfirm from %s pid %u name %s answer %d", ch->GetName(), p->requestPID, (ch_wait)?ch_wait->GetName():"", p->answer);
	if (ch_wait)
	{
		quest::CQuestManager::Instance().Confirm(ch_wait->GetPlayerID(), (quest::EQuestConfirmType) p->answer, ch->GetPlayerID());
	}
}

void CInputMain::Target(LPCHARACTER ch, const char * pcData)
{
	TPacketCGTarget * p = (TPacketCGTarget *) pcData;

	building::LPOBJECT pkObj = building::CManager::instance().FindObjectByVID(p->dwVID);

	if (pkObj)
	{
		TPacketGCTarget pckTarget;
		pckTarget.header = HEADER_GC_TARGET;
		pckTarget.dwVID = p->dwVID;
		ch->GetDesc()->Packet(&pckTarget, sizeof(TPacketGCTarget));
	}
	else
		ch->SetTarget(CHARACTER_MANAGER::instance().Find(p->dwVID));
}

void CInputMain::Warp(LPCHARACTER ch, const char * pcData)
{
	ch->WarpEnd();
}

void CInputMain::SafeboxCheckin(LPCHARACTER ch, const char * c_pData)
{
	if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning() == true)
		return;

#if defined(ENABLE_RESTRICT_STAFF_RESTRICTIONS)
	uint8_t gm_level = ch->GetGMLevel();

	if (test_server == false && gm_level > GM_PLAYER && gm_level < GM_IMPLEMENTOR) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 1543, "");
		return;
	}
#endif

	TPacketCGSafeboxCheckin * p = (TPacketCGSafeboxCheckin *) c_pData;

	if (!ch->CanHandleItem())
		return;

#ifdef __ENABLE_NEW_OFFLINESHOP__
	if (ch->GetOfflineShopGuest() || ch->GetAuctionGuest())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot move the item if you have opened a private shop.");
		return;
	}
#endif

	CSafebox * pkSafebox = ch->GetSafebox();
	LPITEM pkItem = ch->GetItem(p->ItemPos);

	if (!pkSafebox || !pkItem)
		return;
#if defined(ITEM_CHECKINOUT_UPDATE)
	if (p->SelectPosAuto)
	{
		int AutoPos = pkSafebox->GetEmptySafebox(pkItem->GetSize());
		if (AutoPos == -1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "<SAFEBOX> You don't have enough space.");
			return;
		}
		p->bSafePos = AutoPos;
	}
#endif

	if (pkItem->IsEquipped())
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 1244, "");
		return;
	}

/*
#if defined(ENABLE_EXTRA_INVENTORY)
	if (pkItem->IsExtraItem())
		return;
#endif
*/

	if (pkItem->GetCell() >= INVENTORY_MAX_NUM && IS_SET(pkItem->GetFlag(), ITEM_FLAG_IRREMOVABLE))
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 640, "");
		return;
	}

	if (!pkSafebox->IsEmpty(p->bSafePos, pkItem->GetSize()))
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 641, "");
		return;
	}

	if (pkItem->GetVnum() == UNIQUE_ITEM_SAFEBOX_EXPAND)
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 187, "");
		return;
	}

	if( IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_SAFEBOX) )
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 187, "");
		return;
	}

	if (true == pkItem->isLocked())
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 187, "");
		return;
	}

	// @fixme140 BEGIN
	if (ITEM_BELT == pkItem->GetType() && CBeltInventoryHelper::IsExistItemInBeltInventory(ch))
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 385, "");
		return;
	}
	// @fixme140 END

	pkItem->RemoveFromCharacter();
#ifdef ENABLE_EXTRA_INVENTORY
	if (!pkItem->IsDragonSoul() && !pkItem->IsExtraItem())
#else
	if (!pkItem->IsDragonSoul())
#endif
	{
		ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, p->ItemPos.cell, 65535);
	}

	pkSafebox->Add(p->bSafePos, pkItem);

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX PUT", szHint);
}

void CInputMain::SafeboxCheckout(LPCHARACTER ch, const char * c_pData, bool bMall)
{
#if defined(ENABLE_RESTRICT_STAFF_RESTRICTIONS)
	uint8_t gm_level = ch->GetGMLevel();

	if (test_server == false && gm_level > GM_PLAYER && gm_level < GM_IMPLEMENTOR) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 1543, "");
		return;
	}
#endif

	TPacketCGSafeboxCheckout * p = (TPacketCGSafeboxCheckout *) c_pData;

	if (!ch->CanHandleItem())
		return;

#ifdef __ENABLE_NEW_OFFLINESHOP__
	if (ch->GetOfflineShopGuest() || ch->GetAuctionGuest())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot move the item if you have opened a private shop.");
		return;
	}
#endif

	CSafebox * pkSafebox;

	if (bMall)
		pkSafebox = ch->GetMall();
	else
		pkSafebox = ch->GetSafebox();

	if (!pkSafebox)
		return;

	LPITEM pkItem = pkSafebox->Get(p->bSafePos);

	if (!pkItem)
		return;
#if defined(ITEM_CHECKINOUT_UPDATE)
	if (p->SelectPosAuto)
	{
		int AutoPos = pkItem->IsDragonSoul() ? ch->GetEmptyDragonSoulInventory(pkItem) : ch->GetEmptyInventory(pkItem->GetSize());
		if (AutoPos == -1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You don't have enough space.");
			return;
		}
		p->ItemPos = TItemPos(pkItem->IsDragonSoul() ? DRAGON_SOUL_INVENTORY : INVENTORY, AutoPos);
	}
#endif
	if (!ch->IsEmptyItemGrid(p->ItemPos, pkItem->GetSize()))
		return;

	// 아이템 몰에서 인벤으로 옮기는 부분에서 용혼석 특수 처리
	// (몰에서 만드는 아이템은 item_proto에 정의된대로 속성이 붙기 때문에,
	//  용혼석의 경우, 이 처리를 하지 않으면 속성이 하나도 붙지 않게 된다.)
	if (pkItem->IsDragonSoul())
	{
		if (bMall)
		{
			DSManager::instance().DragonSoulItemInitialize(pkItem);
		}

		if (DRAGON_SOUL_INVENTORY != p->ItemPos.window_type)
		{
			ch->ChatPacketNew(CHAT_TYPE_INFO, 643, "");
			return;
		}

		TItemPos DestPos = p->ItemPos;
		if (!DSManager::instance().IsValidCellForThisItem(pkItem, DestPos))
		{
			int iCell = ch->GetEmptyDragonSoulInventory(pkItem);
			if (iCell < 0)
			{
				ch->ChatPacketNew(CHAT_TYPE_INFO, 644, "");
				return;
			}

			DestPos = TItemPos (DRAGON_SOUL_INVENTORY, iCell);
		}

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, DestPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
#ifdef ENABLE_EXTRA_INVENTORY
	else if(pkItem->IsExtraItem())
	{
		if (p->ItemPos.window_type != EXTRA_INVENTORY)
		{
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1292, "");
			return;
		}

		BYTE category = pkItem->GetExtraCategory();
		if (p->ItemPos.cell < category * EXTRA_INVENTORY_CATEGORY_MAX_NUM || p->ItemPos.cell >= (category + 1) * EXTRA_INVENTORY_CATEGORY_MAX_NUM) {
			return;
		}

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
#endif
	else
	{
#ifdef ENABLE_EXTRA_INVENTORY
		if (EXTRA_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacketNew(CHAT_TYPE_INFO, 645, "");
			return;
		}
#endif
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacketNew(CHAT_TYPE_INFO, 646, "");
			return;
		}
		// @fixme119
		if (p->ItemPos.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(pkItem))
		{
			ch->ChatPacketNew(CHAT_TYPE_INFO, 509, "");
			return;
		}

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}

	DWORD dwID = pkItem->GetID();
	db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_FLUSH, 0, sizeof(DWORD));
	db_clientdesc->Packet(&dwID, sizeof(DWORD));

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	if (bMall)
		LogManager::instance().ItemLog(ch, pkItem, "MALL GET", szHint);
	else
		LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX GET", szHint);
}

void CInputMain::SafeboxItemMove(LPCHARACTER ch, const char * data)
{
#if defined(ENABLE_RESTRICT_STAFF_RESTRICTIONS)
	uint8_t gm_level = ch->GetGMLevel();

	if (test_server == false && gm_level > GM_PLAYER && gm_level < GM_IMPLEMENTOR) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 1543, "");
		return;
	}
#endif

	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (!ch->CanHandleItem())
		return;

#ifdef __ENABLE_NEW_OFFLINESHOP__
	if (ch->GetOfflineShopGuest() || ch->GetAuctionGuest())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot move the item if you have opened a private shop.");
		return;
	}
#endif

	if (!ch->GetSafebox())
		return;

	ch->GetSafebox()->MoveItem(pinfo->Cell.cell, pinfo->CellTo.cell, pinfo->count);
}

void CInputMain::PartyInvite(LPCHARACTER pkChar, const char* data) {
	if (!pkChar->IsPC()) {
		return;
	}

	if (pkChar->GetArena()) {
		pkChar->ChatPacketNew(CHAT_TYPE_INFO, 303, "");
		return;
	}

	const auto* pInfo = reinterpret_cast<const TPacketCGPartyInvite*>(data);
	const LPCHARACTER pInvitee = CHARACTER_MANAGER::instance().Find(pInfo->vid);

	if (!pInvitee || !pInvitee->GetDesc()) {
		return;
	}

#if defined(ENABLE_MESSENGER_BLOCK)
	if (MessengerManager::instance().CheckMessengerList(pkChar->GetName(), pInvitee->GetName(), SYST_BLOCK))
	{
		pkChar->ChatPacketNew(CHAT_TYPE_INFO, 1429, "%s", pInvitee->GetName());
		return;
	}
#endif

	pkChar->PartyInvite(pInvitee);
}

void CInputMain::PartyInviteAnswer(LPCHARACTER ch, const char * c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 303, "");
		return;
	}

	TPacketCGPartyInviteAnswer * p = (TPacketCGPartyInviteAnswer*) c_pData;

	LPCHARACTER pInviter = CHARACTER_MANAGER::instance().Find(p->leader_vid);
	if (!pInviter || !pInviter->GetDesc()) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 217, "");
	}
	else if (!p->accept) {
		pInviter->PartyInviteDeny(ch->GetPlayerID());
	} else {
		pInviter->PartyInviteAccept(ch);
	}
}
// END_OF_PARTY_JOIN_BUG_FIX

void CInputMain::PartySetState(LPCHARACTER ch, const char* c_pData)
{
	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 208, "");
		return;
	}

	TPacketCGPartySetState* p = (TPacketCGPartySetState*) c_pData;

	if (!ch->GetParty())
		return;

	if (ch->GetParty()->GetLeaderPID() != ch->GetPlayerID())
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 206, "");
		return;
	}

	if (!ch->GetParty()->IsMember(p->pid))
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 207, "");
		return;
	}

	DWORD pid = p->pid;
	sys_log(0, "PARTY SetRole pid %d to role %d state %s", pid, p->byRole, p->flag ? "on" : "off");

	switch (p->byRole)
	{
		case PARTY_ROLE_NORMAL:
			break;

		case PARTY_ROLE_ATTACKER:
		case PARTY_ROLE_TANKER:
		case PARTY_ROLE_BUFFER:
		case PARTY_ROLE_SKILL_MASTER:
		case PARTY_ROLE_HASTE:
		case PARTY_ROLE_DEFENDER:
			if (ch->GetParty()->SetRole(pid, p->byRole, p->flag))
			{
				TPacketPartyStateChange pack;
				pack.dwLeaderPID = ch->GetPlayerID();
				pack.dwPID = p->pid;
				pack.bRole = p->byRole;
				pack.bFlag = p->flag;
				db_clientdesc->DBPacket(HEADER_GD_PARTY_STATE_CHANGE, 0, &pack, sizeof(pack));
			}
			break;
		default:
			sys_err("wrong byRole in PartySetState Packet name %s state %d", ch->GetName(), p->byRole);
			break;
	}
}

void CInputMain::PartyRemove(LPCHARACTER ch, const char* c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 303, "");
		return;
	}

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 208, "");
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 203, "");
		return;
	}

	TPacketCGPartyRemove* p = (TPacketCGPartyRemove*) c_pData;

	if (!ch->GetParty())
		return;

	LPPARTY pParty = ch->GetParty();
	if (pParty->GetLeaderPID() == ch->GetPlayerID())
	{
		if (!ch->GetDungeon()) {
			// 적룡성에서 파티장이 던젼 밖에서 파티 해산 못하게 막자
			if(pParty->IsPartyInDungeon(351))
			{
				ch->ChatPacketNew(CHAT_TYPE_INFO, 648, "");
				return;
			}

			// leader can remove any member
			if (p->pid == ch->GetPlayerID() || pParty->GetMemberCount() == 2)
			{
				// party disband
				CPartyManager::instance().DeleteParty(pParty);
			}
			else
			{
				LPCHARACTER B = CHARACTER_MANAGER::instance().FindByPID(p->pid);
				if (B) {
					//pParty->SendPartyRemoveOneToAll(B);
					B->ChatPacketNew(CHAT_TYPE_INFO, 216, "");
					//pParty->Unlink(B);
					//CPartyManager::instance().SetPartyMember(B->GetPlayerID(), NULL);
				}
				pParty->Quit(p->pid);
			}
		}
		else {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 205, "");
		}
	}
	else
	{
		if (p->pid == ch->GetPlayerID())
		{
			if (!ch->GetDungeon()) {
				if (pParty->GetMemberCount() == 2) {
					CPartyManager::instance().DeleteParty(pParty);
				} else {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 215, "");
					//pParty->SendPartyRemoveOneToAll(ch);
					pParty->Quit(ch->GetPlayerID());
					//pParty->SendPartyRemoveAllToOne(ch);
					//CPartyManager::instance().SetPartyMember(ch->GetPlayerID(), NULL);
				}
			}
			else {
				ch->ChatPacketNew(CHAT_TYPE_INFO, 204, "");
			}
		}
		else {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 197, "");
		}
	}
}

void CInputMain::AnswerMakeGuild(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGAnswerMakeGuild* p = (TPacketCGAnswerMakeGuild*) c_pData;

	if (ch->GetGold() < 200000) {
		return;
	}
	else if (ch->GetLevel() < 40) {
		return;
	}

	if (get_global_time() - ch->GetQuestFlag("guild_manage.new_disband_time") < CGuildManager::instance().GetDisbandDelay()) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 181, "%d", quest::CQuestManager::instance().GetEventFlag("guild_disband_delay"));
		return;
	}

	if (get_global_time() - ch->GetQuestFlag("guild_manage.new_withdraw_time") < CGuildManager::instance().GetWithdrawDelay()) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 179, "%d", quest::CQuestManager::instance().GetEventFlag("guild_withdraw_delay"));
		return;
	}

	if (ch->GetGuild())
		return;

	CGuildManager& gm = CGuildManager::instance();

	TGuildCreateParameter cp;
	memset(&cp, 0, sizeof(cp));

	cp.master = ch;
	strlcpy(cp.name, p->guild_name, sizeof(cp.name));

	if (cp.name[0] == 0 || !check_name(cp.name))
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 455, "");
		return;
	}

	DWORD dwGuildID = gm.CreateGuild(cp);

	if (dwGuildID)
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 125, "%s", cp.name);
		int GuildCreateFee = 200000;

#if defined(ENABLE_NEW_GOLD_LIMIT)
		ch->ChangeGold(-GuildCreateFee);
#else
		ch->PPointChange(POINT_GOLD, -GuildCreateFee);
#endif
		DBManager::instance().SendMoneyLog(MONEY_LOG_GUILD, ch->GetPlayerID(), -GuildCreateFee);

		char Log[128];
		snprintf(Log, sizeof(Log), "GUILD_NAME %s MASTER %s", cp.name, ch->GetName());
		LogManager::instance().CharLog(ch, 0, "MAKE_GUILD", Log);

		ch->RemoveSpecifyItem(GUILD_CREATE_ITEM_VNUM, 1);
		//ch->SendGuildName(dwGuildID);
	}
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 132, "");
	}
}

void CInputMain::PartyUseSkill(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGPartyUseSkill* p = (TPacketCGPartyUseSkill*) c_pData;
	if (!ch->GetParty())
		return;

	if (ch->GetPlayerID() != ch->GetParty()->GetLeaderPID())
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 211, "");
		return;
	}

	switch (p->bySkillIndex)
	{
		case PARTY_SKILL_HEAL:
			ch->GetParty()->HealParty();
			break;
		case PARTY_SKILL_WARP:
			{
				LPCHARACTER pch = CHARACTER_MANAGER::instance().Find(p->vid);
				if (pch) {
					ch->GetParty()->SummonToLeader(pch->GetPlayerID());
				}
				else {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 209, "");
				}
			}
			break;
		default:
			break;
	}
}

void CInputMain::PartyParameter(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGPartyParameter * p = (TPacketCGPartyParameter *) c_pData;

	if (ch->GetParty())
		ch->GetParty()->SetParameter(p->bDistributeMode);
}

#ifdef __INGAME_WIKI__
void CInputMain::RecvWikiPacket(LPCHARACTER ch, const char * c_pData)
{
	if (!ch || (ch && !ch->GetDesc()))
		return;

	if (!c_pData)
		return;

#if defined(ENABLE_RENEWAL_OX)
	if (ch->GetMapIndex() == OX_MAP_INDEX) {
		if (ch->GetGMLevel() == GM_PLAYER) {
			return;
		}
	}
#endif

	InGameWiki::TCGWikiPacket * p = nullptr;
	if (!(p = (InGameWiki::TCGWikiPacket *) c_pData))
		return;
	
	InGameWiki::TGCWikiPacket pack;
	pack.set_data_type(!p->is_mob ? InGameWiki::LOAD_WIKI_ITEM : InGameWiki::LOAD_WIKI_MOB);
	pack.increment_data_size(WORD(sizeof(InGameWiki::TGCWikiPacket)));
	
	if (pack.is_data_type(InGameWiki::LOAD_WIKI_ITEM))
	{
		const std::vector<CommonWikiData::TWikiItemOriginInfo>& originVec = ITEM_MANAGER::Instance().GetItemOrigin(p->vnum);
		const std::vector<CSpecialItemGroup::CSpecialItemInfo> _gV = ITEM_MANAGER::instance().GetWikiChestInfo(p->vnum);
		const std::vector<CommonWikiData::TWikiRefineInfo> _rV = ITEM_MANAGER::instance().GetWikiRefineInfo(p->vnum);
		const CommonWikiData::TWikiInfoTable* _wif = ITEM_MANAGER::instance().GetItemWikiInfo(p->vnum);
		
		if (!_wif)
			return;
		
		const size_t origin_size = originVec.size();
		const size_t chest_info_count = _wif->chest_info_count;
		const size_t refine_infos_count = _wif->refine_infos_count;
		const size_t buf_data_dize = sizeof(InGameWiki::TGCItemWikiPacket) +
								(origin_size * sizeof(CommonWikiData::TWikiItemOriginInfo)) +
								(chest_info_count * sizeof(CommonWikiData::TWikiChestInfo)) +
								(refine_infos_count * sizeof(CommonWikiData::TWikiRefineInfo));
		
		if (chest_info_count != _gV.size()) {
			sys_err("Item Vnum : %d || ERROR TYPE -> 1", p->vnum);
			return;
		}
		
		if (refine_infos_count != _rV.size()) {
			sys_err("Item Vnum : %d || ERROR TYPE -> 2", p->vnum);
			return;
		}
		
		pack.increment_data_size(WORD(buf_data_dize));
		
		TEMP_BUFFER buf;
		buf.write(&pack, sizeof(InGameWiki::TGCWikiPacket));
		
		InGameWiki::TGCItemWikiPacket data_packet;
		data_packet.mutable_wiki_info(*_wif);
		data_packet.set_origin_infos_count(origin_size);
		data_packet.set_vnum(p->vnum);
		data_packet.set_ret_id(p->ret_id);
		buf.write(&data_packet, sizeof(data_packet));
		
		{
			if (origin_size)
				for (int idx = 0; idx < (int)origin_size; ++idx)
					buf.write(&(originVec[idx]), sizeof(CommonWikiData::TWikiItemOriginInfo));
			
			if (chest_info_count > 0) {
				for (int idx = 0; idx < (int)chest_info_count; ++idx) {
					CommonWikiData::TWikiChestInfo write_struct(_gV[idx].vnum, _gV[idx].count);
					buf.write(&write_struct, sizeof(CommonWikiData::TWikiChestInfo));
				}
			}
			
			if (refine_infos_count > 0)
				for (int idx = 0; idx < (int)refine_infos_count; ++idx)
					buf.write(&(_rV[idx]), sizeof(CommonWikiData::TWikiRefineInfo));
		}
		
		ch->GetDesc()->Packet(buf.read_peek(), buf.size());
	}
	else
	{
		CMobManager::TMobWikiInfoVector& mobVec = CMobManager::instance().GetMobWikiInfo(p->vnum);
		const size_t _mobVec_size = mobVec.size();
		
		if (!_mobVec_size) {
			if (test_server)
				sys_log(0, "Mob Vnum: %d : || LOG TYPE -> 1", p->vnum);
			return;
		}
		
		const size_t buf_data_dize = (sizeof(InGameWiki::TGCMobWikiPacket) + (_mobVec_size * sizeof(CommonWikiData::TWikiMobDropInfo)));
		pack.increment_data_size(WORD(buf_data_dize));
		
		TEMP_BUFFER buf;
		buf.write(&pack, sizeof(InGameWiki::TGCWikiPacket));
		
		InGameWiki::TGCMobWikiPacket data_packet;
		data_packet.set_drop_info_count(_mobVec_size);
		data_packet.set_vnum(p->vnum);
		data_packet.set_ret_id(p->ret_id);
		buf.write(&data_packet, sizeof(InGameWiki::TGCMobWikiPacket));
		
		{
			if (_mobVec_size) {
				for (int idx = 0; idx < (int)_mobVec_size; ++idx) {
					CommonWikiData::TWikiMobDropInfo write_struct(mobVec[idx].vnum, mobVec[idx].count);
					buf.write(&write_struct, sizeof(CommonWikiData::TWikiMobDropInfo));
				}
			}
		}
		
		ch->GetDesc()->Packet(buf.read_peek(), buf.size());
	}
}
#endif

size_t GetSubPacketSize(const GUILD_SUBHEADER_CG& header)
{
	switch (header)
	{
		case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:				return sizeof(int);
		case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:				return sizeof(int);
		case GUILD_SUBHEADER_CG_ADD_MEMBER:					return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_REMOVE_MEMBER:				return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:			return 10;
		case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:		return sizeof(BYTE) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_OFFER:						return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_CHARGE_GSP:					return sizeof(int);
		case GUILD_SUBHEADER_CG_POST_COMMENT:				return 1;
		case GUILD_SUBHEADER_CG_DELETE_COMMENT:				return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_REFRESH_COMMENT:			return 0;
		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_USE_SKILL:					return sizeof(TPacketCGGuildUseSkill);
		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_CHANGE_SYMBOL:		return 0;
	}

	return 0;
}

int CInputMain::Guild(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	if (uiBytes < sizeof(TPacketCGGuild))
		return -1;

	const TPacketCGGuild* p = reinterpret_cast<const TPacketCGGuild*>(data);
	const char* c_pData = data + sizeof(TPacketCGGuild);

	uiBytes -= sizeof(TPacketCGGuild);

	const GUILD_SUBHEADER_CG SubHeader = static_cast<GUILD_SUBHEADER_CG>(p->subheader);
	const size_t SubPacketLen = GetSubPacketSize(SubHeader);

	if (uiBytes < SubPacketLen)
	{
		return -1;
	}

	CGuild* pGuild = ch->GetGuild();

	if (NULL == pGuild)
	{
		if (SubHeader != GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER)
		{
			ch->ChatPacketNew(CHAT_TYPE_INFO, 138, "");
			return SubPacketLen;
		}
	}

	switch (SubHeader)
	{
		case GUILD_SUBHEADER_CG_CHANGE_SYMBOL:
		{
			pGuild->ChangeGuildSymbol(ch);
			return SubPacketLen;
		}

		case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:
			{
				// by mhh : 길드자금은 당분간 넣을 수 없다.
				return SubPacketLen;

				const int gold = MIN(*reinterpret_cast<const int*>(c_pData), __deposit_limit());

				if (gold < 0)
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 170, "");
					return SubPacketLen;
				}

				if (ch->GetGold() < gold)
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 126, "");
					return SubPacketLen;
				}

				pGuild->RequestDepositMoney(ch, gold);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:
			{
				// by mhh : 길드자금은 당분간 뺄 수 없다.
				return SubPacketLen;

				const int gold = MIN(*reinterpret_cast<const int*>(c_pData), 500000);

				if (gold < 0)
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 170, "");
					return SubPacketLen;
				}

				pGuild->RequestWithdrawMoney(ch, gold);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_ADD_MEMBER:
			{
				const DWORD vid = *reinterpret_cast<const DWORD*>(c_pData);
				LPCHARACTER newmember = CHARACTER_MANAGER::instance().Find(vid);

				if (!newmember)
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 128, "");
					return SubPacketLen;
				}

				if (!ch->IsPC() || !newmember->IsPC()) {
					return SubPacketLen;
				}

#if defined(ENABLE_MESSENGER_BLOCK)
				if (MessengerManager::instance().CheckMessengerList(ch->GetName(), newmember->GetName(), SYST_BLOCK)) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 1429, "%s", newmember->GetName());
					return SubPacketLen;
				}
#endif

				pGuild->Invite(ch, newmember);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REMOVE_MEMBER:
			{
				if (pGuild->UnderAnyWar() != 0)
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 649, "");
					return SubPacketLen;
				}

				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				LPCHARACTER member = CHARACTER_MANAGER::instance().FindByPID(pid);

				if (member)
				{
					if (member->GetGuild() != pGuild)
					{
						ch->ChatPacketNew(CHAT_TYPE_INFO, 161, "");
						return SubPacketLen;
					}

					if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
					{
						ch->ChatPacketNew(CHAT_TYPE_INFO, 139, "");
						return SubPacketLen;
					}

					member->SetQuestFlag("guild_manage.new_withdraw_time", get_global_time());
					pGuild->RequestRemoveMember(member->GetPlayerID());

					if (g_bGuildInviteLimit)
					{
						DBManager::instance().Query("REPLACE INTO guild_invite_limit VALUES(%d, %d)", pGuild->GetID(), get_global_time());
					}
				}
				else
				{
					if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
					{
						ch->ChatPacketNew(CHAT_TYPE_INFO, 139, "");
						return SubPacketLen;
					}

					if (pGuild->RequestRemoveMember(pid)) {
						ch->ChatPacketNew(CHAT_TYPE_INFO, 129, "");
					} else {
						ch->ChatPacketNew(CHAT_TYPE_INFO, 128, "");
					}
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:
			{
				char gradename[GUILD_GRADE_NAME_MAX_LEN + 1];
				strlcpy(gradename, c_pData + 1, sizeof(gradename));

				const TGuildMember * m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 175, "");
				} else if (*c_pData == GUILD_LEADER_GRADE) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 143, "");
				}
				else if (!check_name(gradename)) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 171, "");
				}
				else {
					pGuild->ChangeGradeName(*c_pData, gradename);
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:
			{
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());
				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 174, "");
				} else if (*c_pData == GUILD_LEADER_GRADE) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 142, "");
				}
				else {
					pGuild->ChangeGradeAuth(*c_pData, *(c_pData + 1));
				}
			}
			return SubPacketLen;
		case GUILD_SUBHEADER_CG_OFFER:
			{
				DWORD offer = *reinterpret_cast<const DWORD*>(c_pData);

				if (pGuild->GetLevel() >= GUILD_MAX_LEVEL)
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 650, "%d", GUILD_MAX_LEVEL);
				}
				else
				{
					offer /= 100;
					offer *= 100;
					if (pGuild->OfferExp(ch, offer)) {
						ch->ChatPacketNew(CHAT_TYPE_INFO, 121, "%u", offer);
					} else {
						ch->ChatPacketNew(CHAT_TYPE_INFO, 122, "");
					}
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHARGE_GSP:
			{
				const int offer = *reinterpret_cast<const int*>(c_pData);
				const int gold = offer * 100;

				if (offer < 0 || gold < offer || gold < 0 || ch->GetGold() < gold)
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 151, "");
					return SubPacketLen;
				}

				if (!pGuild->ChargeSP(ch, offer)) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 164, "");
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_POST_COMMENT:
			{
				const size_t length = *c_pData;

				if (length > GUILD_COMMENT_MAX_LEN)
				{
					// 잘못된 길이.. 끊어주자.
					sys_err("POST_COMMENT: %s comment too long (length: %u)", ch->GetName(), length);
					ch->GetDesc()->SetPhase(PHASE_CLOSE);
					return -1;
				}

				if (uiBytes < 1 + length)
					return -1;

				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (length && !pGuild->HasGradeAuth(m->grade, GUILD_AUTH_NOTICE) && *(c_pData + 1) == '!')
				{
					ch->ChatPacketNew(CHAT_TYPE_INFO, 127, "");
				}
				else
				{
					std::string str(c_pData + 1, length);
					pGuild->AddComment(ch, str);
				}

				return (1 + length);
			}

		case GUILD_SUBHEADER_CG_DELETE_COMMENT:
			{
				const DWORD comment_id = *reinterpret_cast<const DWORD*>(c_pData);

				pGuild->DeleteComment(ch, comment_id);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REFRESH_COMMENT:
			pGuild->RefreshComment(ch);
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:
			{
				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE grade = *(c_pData + sizeof(DWORD));
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 176, "");
				} else if (ch->GetPlayerID() == pid) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 143, "");
				} else if (grade == 1) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 141, "");
				} else {
					pGuild->ChangeMemberGrade(pid, grade);
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_USE_SKILL:
			{
				const TPacketCGGuildUseSkill* p = reinterpret_cast<const TPacketCGGuildUseSkill*>(c_pData);

				pGuild->UseSkill(p->dwVnum, ch, p->dwPID);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:
			{
				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE is_general = *(c_pData + sizeof(DWORD));
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 150, "");
				} else if (!pGuild->ChangeMemberGeneral(pid, is_general)) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 149, "");
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:
			{
				const DWORD guild_id = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE accept = *(c_pData + sizeof(DWORD));

				CGuild * g = CGuildManager::instance().FindGuild(guild_id);

				if (g)
				{
					if (accept)
						g->InviteAccept(ch);
					else
						g->InviteDeny(ch->GetPlayerID());
				}
			}
			return SubPacketLen;

	}

	return 0;
}

void CInputMain::Fishing(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGFishing* p = (TPacketCGFishing*)c_pData;
	ch->SetRotation(p->dir * 5);
	ch->fishing();
	return;
}

void CInputMain::ItemGive(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGGiveItem* p = (TPacketCGGiveItem*) c_pData;
	LPCHARACTER to_ch = CHARACTER_MANAGER::instance().Find(p->dwTargetVID);

	if (to_ch) {
#if defined(ENABLE_RESTRICT_STAFF_RESTRICTIONS)
		uint8_t gm_level = ch->GetGMLevel();

		if (test_server == false && ((gm_level > GM_PLAYER && gm_level < GM_IMPLEMENTOR) ||
									(gm_level != GM_IMPLEMENTOR && to_ch->GetGMLevel() > GM_PLAYER && to_ch->GetGMLevel() < GM_IMPLEMENTOR))
		) {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1543, "");
			return;
		}
#endif

		ch->GiveItem(to_ch, p->ItemPos);
	}
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 403, "");
	}
}

void CInputMain::Hack(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGHack * p = (TPacketCGHack *) c_pData;
	if (!p || !strlen(p->szBuf))
	{
		return;
	}

	char buf[sizeof(p->szBuf)];
	strlcpy(buf, p->szBuf, sizeof(buf));

	char __escape_string[1024];
	DBManager::instance().EscapeString(__escape_string, sizeof(__escape_string), buf, strlen(p->szBuf));

	LogManager::instance().HackLog(__escape_string, ch);

	ch->GetDesc()->SetPhase(PHASE_CLOSE);
}

int CInputMain::MyShop(LPCHARACTER ch, const char * c_pData, size_t uiBytes)
{
	TPacketCGMyShop * p = (TPacketCGMyShop *) c_pData;
	int iExtraLen = p->bCount * sizeof(TShopItemTable);

	if (uiBytes < sizeof(TPacketCGMyShop) + iExtraLen)
		return -1;

#if defined(ENABLE_NEW_GOLD_LIMIT)
	if (ch->GetGold() >= GOLD_MAX_MAX)
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 226, "%llu", GOLD_MAX_MAX);
		return (iExtraLen);
	}
#else
	if (ch->GetGold() >= GOLD_MAX)
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 226, "%d", GOLD_MAX);
		return (iExtraLen);
	}
#endif

	if (ch->IsStun() || ch->IsDead())
		return (iExtraLen);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen()
#ifdef ENABLE_ACCE_SYSTEM
 || ch->IsAcceOpen()
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
 || ch->GetOfflineShopGuest() || ch->GetAuctionGuest()
#endif
#if defined(ENABLE_AURA_SYSTEM)
 || ch->IsAuraRefineWindowOpen()
#endif
#ifdef __ATTR_TRANSFER_SYSTEM__
 || ch->IsAttrTransferOpen()
#endif
#if defined(ENABLE_CHANGELOOK)
 || ch->isChangeLookOpened()
#endif
#if defined(USE_ATTR_6TH_7TH)
 || ch->IsOpenAttr67Add()
#endif
	)
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 292, "");
		return (iExtraLen);
	}

	sys_log(0, "MyShop count %d", p->bCount);
	ch->OpenMyShop(p->szSign, (TShopItemTable *) (c_pData + sizeof(TPacketCGMyShop)), p->bCount);
	return (iExtraLen);
}

void CInputMain::Refine(LPCHARACTER ch, const char* c_pData)
{
#if defined(ENABLE_RESTRICT_STAFF_RESTRICTIONS)
	uint8_t gm_level = ch->GetGMLevel();

	if (test_server == false && gm_level > GM_PLAYER && gm_level < GM_IMPLEMENTOR) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 1543, "");
		ch->ClearRefineMode();
		return;
	}
#endif

	const TPacketCGRefine* p = reinterpret_cast<const TPacketCGRefine*>(c_pData);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen()
#ifdef ENABLE_ACCE_SYSTEM
 || ch->IsAcceOpen()
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
 || ch->GetOfflineShopGuest() || ch->GetAuctionGuest()
#endif
#if defined(ENABLE_AURA_SYSTEM)
 || ch->IsAuraRefineWindowOpen()
#endif
#ifdef __ATTR_TRANSFER_SYSTEM__
 || ch->IsAttrTransferOpen()
#endif
#if defined(ENABLE_CHANGELOOK)
 || ch->isChangeLookOpened()
#endif
#if defined(USE_ATTR_6TH_7TH)
 || ch->IsOpenAttr67Add()
#endif
	)
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 502, "");
		ch->ClearRefineMode();
		return;
	}

	if (p->type == 255)
	{
		// DoRefine Cancel
		ch->ClearRefineMode();
		return;
	}

	if (p->pos >= INVENTORY_MAX_NUM)
	{
		ch->ClearRefineMode();
		return;
	}

	LPITEM item = ch->GetInventoryItem(p->pos);
	if (!item)
	{
		ch->ClearRefineMode();
		return;
	}

#if defined(ENABLE_AURA_SYSTEM)
	if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_AURA) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 427, "");
		ch->ClearRefineMode();
		return;
	}
#endif

#ifdef ENABLE_FEATURES_REFINE_SYSTEM
	if (!CRefineManager::instance().GetPercentage(ch, p->lLow, p->lMedium, p->lExtra, p->lTotal, item))
	{
		ch->ClearRefineMode();
		return;
	}

	CRefineManager::instance().Increase(ch, p->lLow, p->lMedium, p->lExtra);
#endif

	ch->SetRefineTime();

	if (p->type == REFINE_TYPE_NORMAL)
	{
		sys_log (0, "refine_type_noraml");
		ch->DoRefine(item);
	}
	else if (p->type == REFINE_TYPE_SCROLL || p->type == REFINE_TYPE_HYUNIRON || p->type == REFINE_TYPE_MUSIN || p->type == REFINE_TYPE_BDRAGON)
	{
		sys_log (0, "refine_type_scroll, ...");
		ch->DoRefineWithScroll(item);
	}

#ifdef ENABLE_SOUL_SYSTEM
	else if (p->type == REFINE_TYPE_SOUL)
	{
		sys_log (0, "refine_type_soul, ...");
		ch->DoRefineItemSoul(item);
	}
#endif
	else if (p->type == REFINE_TYPE_MONEY_ONLY) {
		if (item) {
			if (ch->GetQuestFlag("deviltower_zone.can_refine"))
			{
				if (ch->DoRefine(item, true)) {
					ch->SetQuestFlag("deviltower_zone.can_refine", 0);
				}
			}
			else {
				ch->ChatPacketNew(CHAT_TYPE_INFO, 361, "");
			}
		}
	}

	ch->ClearRefineMode();
}



#ifdef ENABLE_ACCE_SYSTEM
void CInputMain::Acce(LPCHARACTER pkChar, const char* c_pData) {
	quest::PC * pPC = quest::CQuestManager::instance().GetPCForce(pkChar->GetPlayerID());
	if (pPC->IsRunning()) {
		return;
	}

	const TPacketAcce* pInfo = reinterpret_cast<const TPacketAcce*>(c_pData);

	switch (pInfo->subheader) {
		case ACCE_SUBHEADER_CG_CLOSE: {
			pkChar->CloseAcce();
			break;
		}
		case ACCE_SUBHEADER_CG_ADD: {
			pkChar->AddAcceMaterial(pInfo->tPos, pInfo->bPos);
			break;
		}
		case ACCE_SUBHEADER_CG_REMOVE: {
			pkChar->RemoveAcceMaterial(pInfo->bPos);
			break;
		}
		case ACCE_SUBHEADER_CG_REFINE: {
			pkChar->RefineAcceMaterials();
			break;
		}
		default: {
			break;
		}
	}
}
#endif

#if defined(ENABLE_CHANGELOOK)
void CInputMain::ChangeLook(LPCHARACTER pkChar, const char* c_pData) {
	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(pkChar->GetPlayerID());
	if (pPC->IsRunning()) {
		return;
	}

	const TPacketChangeLook* pInfo = reinterpret_cast<const TPacketChangeLook*>(c_pData);

	switch (pInfo->bSubHeader) {
		case SUBHEADER_CHANGE_LOOK_CLOSE: {
			pkChar->ChangeLookWindow(false);
			break;
		}
		case SUBHEADER_CHANGE_LOOK_ADD: {
			pkChar->AddChangeLookMaterial(pInfo->tPos, pInfo->bPos);
			break;
		}
		case SUBHEADER_CHANGE_LOOK_REMOVE: {
			pkChar->RemoveChangeLookMaterial(pInfo->bPos);
			break;
		}
		case SUBHEADER_CHANGE_LOOK_REFINE: {
			pkChar->RefineChangeLookMaterials();
			break;
		}
		default: {
			break;
		}
	}
}
#endif

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
void CInputMain::CubeRenewalSend(LPCHARACTER ch, const char* data)
{
	struct packet_send_cube_renewal * pinfo = (struct packet_send_cube_renewal *) data;
	switch (pinfo->subheader)
	{
		case CUBE_RENEWAL_SUB_HEADER_MAKE_ITEM:
		{

			int index_item = pinfo->index_item;
			int count_item = pinfo->count_item;
			int index_item_improve = pinfo->index_item_improve;

			Cube_Make(ch,index_item,count_item,index_item_improve);
		}
		break;

		case CUBE_RENEWAL_SUB_HEADER_CLOSE:
		{
			Cube_close(ch);
		}
		break;
	}
}
#endif


#ifdef __ENABLE_NEW_OFFLINESHOP__
#include "new_offlineshop.h"
#include "new_offlineshop_manager.h"
template <class T>
bool CanDecode(T* p, int buffleft) {
	return buffleft >= (int)sizeof(T);
}

template <class T>
const char* Decode(T*& pObj, const char* data, int* pbufferLeng = nullptr, int* piBufferLeft=nullptr)
{
	pObj = (T*) data;

	if(pbufferLeng)
		*pbufferLeng += sizeof(T);

	if(piBufferLeft)
		*piBufferLeft -= sizeof(T);

	return data + sizeof(T);
}

int OfflineshopPacketCreateNewShop(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopCreate* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	offlineshop::TShopInfo& rShopInfo = pack->shop;

	//fix flooding
	if (rShopInfo.dwCount > 500 || rShopInfo.dwCount == 0) {
		sys_err("tried to open a shop with 500+ items.");
		return -1;
	}

	std::vector<offlineshop::TShopItemInfo> vec;
	vec.reserve(rShopInfo.dwCount);

	offlineshop::TShopItemInfo* pItem=nullptr;


	for (DWORD i = 0; i < rShopInfo.dwCount; ++i)
	{
		if(!CanDecode(pItem, iBufferLeft))
			return -1;

		data = Decode(pItem, data, &iExtra, &iBufferLeft);
		vec.push_back(*pItem);
	}

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopCreateNewClientPacket(ch, rShopInfo, vec)) {
		if (ch) {
			offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_CREATE_SHOP);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "RefreshOfflineShop");
		}
	}
	
	return iExtra;
}


int OfflineshopPacketChangeShopName(LPCHARACTER ch, const char* data, int iBufferLeft)
{
#if defined(DISABLE_OFFLINESHOP_CHANGENAME)
	return -1;
#endif

	offlineshop::TSubPacketCGShopChangeName* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopChangeNameClientPacket(ch, pack->szName))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_CHANGE_NAME);

	return iExtra;
}

#if defined(ENABLE_OFFLINESHOP_REWORK)
int OfflineshopPacketExtendTime(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopExtendTime* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopExtendTimeClientPacket(ch, pack->dwTime))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_CHANGE_NAME);

	return iExtra;
}
#endif

int OfflineshopPacketForceCloseShop(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopForceCloseClientPacket(ch))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_FORCE_CLOSE);

	return 0;
}


int OfflineshopPacketRequestShopList(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopRequestListClientPacket(ch);
	return 0;
}


int OfflineshopPacketOpenShop(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopOpen* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopOpenClientPacket(ch,pack->dwOwnerID))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_OPEN_SHOP);

	return iExtra;
}


int OfflineshopPacketOpenShowOwner(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopOpenMyShopClientPacket(ch))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_OPEN_SHOP_OWNER);

	return 0;
}


int OfflineshopPacketBuyItem(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopBuyItem* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopBuyItemClientPacket(ch , pack->dwOwnerID, pack->dwItemID, pack->bIsSearch, pack->TotalPriceSeen)) //patch seen price check
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_BUY_ITEM);

	return iExtra;
}


int OfflineshopPacketAddItem(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGAddItem* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);
	
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopAddItemClientPacket(ch, pack->pos, pack->price))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_ADD_ITEM);

	return iExtra;
}


int OfflineshopPacketRemoveItem(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGRemoveItem* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopRemoveItemClientPacket(ch, pack->dwItemID))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_REMOVE_ITEM);

	return iExtra;
}


int OfflineshopPacketEditItem(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGEditItem* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopEditItemClientPacket(ch, pack->dwItemID, pack->price))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_EDIT_ITEM);

	return iExtra;
}


int OfflineshopPacketFilterRequest(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGFilterRequest* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopFilterRequestClientPacket(ch, pack->filter))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_FILTER);

	return iExtra;
}


int OfflineshopPacketCreateOffer(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGOfferCreate* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopCreateOfferClientPacket(ch, pack->offer))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_CREATE_OFFER);

	return iExtra;
}


int OfflineshopPacketAcceptOffer(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGOfferAccept* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopAcceptOfferClientPacket(ch, pack->dwOfferID))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_ACCEPT_OFFER);

	return iExtra;
}



int OfflineshopPacketOfferCancel(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGOfferCancel* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopCancelOfferClientPacket(ch, pack->dwOfferID, pack->dwOwnerID))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_ACCEPT_OFFER);

	return iExtra;
}


int OfflineshopPacketOfferListRequest(LPCHARACTER ch)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvOfferListRequestPacket(ch);
	return 0;
}


int OfflineshopPacketOpenSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopSafeboxOpenClientPacket(ch))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_OPEN_SAFEBOX);

	return 0;
}


int OfflineshopPacketCloseBoard(LPCHARACTER ch)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvCloseBoardClientPacket(ch);
	return 0;
}

int OfflineshopPacketCloseMyAuction(LPCHARACTER ch)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvCloseMyAuction(ch);
	return 0;
}

int OfflineshopPacketGetItemSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopSafeboxGetItem* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopSafeboxGetItemClientPacket(ch, pack->dwItemID))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_SAFEBOX_GET_ITEM);

	return iExtra;

}


int OfflineshopPacketGetValutesSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopSafeboxGetValutes* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopSafeboxGetValutesClientPacket(ch, pack->valutes))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_SAFEBOX_GET_VALUTES);

	return iExtra;
}


int OfflineshopPacketCloseSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopSafeboxCloseClientPacket(ch))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_SAFEBOX_CLOSE);

	return 0;
}


int OfflineshopPacketListRequest(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvAuctionListRequestClientPacket(ch))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_AUCTION_CANNOT_SEND_LIST);

	return 0;
}



int OfflineshopPacketOpenAuctionRequest(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGAuctionOpenRequest* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvAuctionOpenRequestClientPacket(ch, pack->dwOwnerID))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_AUCTION_CANNOT_OPEN_AUCTION);

	return iExtra;
}



int OfflineshopPacketOpenMyAuctionRequest(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvMyAuctionOpenRequestClientPacket(ch))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_AUCTION_CANNOT_SEND_LIST);

	return 0;
}



int OfflineshopPacketCreateAuction(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGAuctionCreate* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvAuctionCreateClientPacket(ch, pack->dwDuration, pack->init_price, pack->pos))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_AUCTION_CANNOT_CREATE_AUCTION);

	return iExtra;
}



int OfflineshopPacketAddOffer(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGAuctionAddOffer* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvAuctionAddOfferClientPacket(ch, pack->dwOwnerID, pack->price))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_AUCTION_CANNOT_ADD_OFFER);

	return iExtra;
}



int OfflineshopPacketExitFromAuction(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGAuctionExitFrom* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvAuctionExitFromAuction(ch, pack->dwOwnerID);
	return iExtra;
}


#ifdef ENABLE_NEW_SHOP_IN_CITIES
int OfflineshopPacketClickEntity(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopClickEntity* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopClickEntity(ch, pack->dwShopVID);
	return iExtra;
}

#endif



int OfflineshopPacket(const char* data , LPCHARACTER ch, long iBufferLeft)
{
	unsigned int iBufferLeftCompare = iBufferLeft;
	if(iBufferLeftCompare < sizeof(TPacketCGNewOfflineShop))
		return -1;

	TPacketCGNewOfflineShop* pPack=nullptr;
	iBufferLeft -= sizeof(TPacketCGNewOfflineShop);
	data = Decode(pPack, data);

#if defined(ENABLE_RENEWAL_OX)
	if (ch && ch->GetMapIndex() == OX_MAP_INDEX) {
		if (ch->GetGMLevel() == GM_PLAYER) {
			return -1;
		}
	}
#endif

	switch (pPack->bSubHeader)
	{
	case offlineshop::SUBHEADER_CG_SHOP_CREATE_NEW:				return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketCreateNewShop(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_CHANGE_NAME:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketChangeShopName(ch,data,iBufferLeft);
#if defined(ENABLE_OFFLINESHOP_REWORK)
	case offlineshop::SUBHEADER_CG_SHOP_EXTEND_TIME:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketExtendTime(ch,data,iBufferLeft);
#endif
	case offlineshop::SUBHEADER_CG_SHOP_FORCE_CLOSE:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketForceCloseShop(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_REQUEST_SHOPLIST:		return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketRequestShopList(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_OPEN:					return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketOpenShop(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_OPEN_OWNER:				return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketOpenShowOwner(ch,data,iBufferLeft);
	
	case offlineshop::SUBHEADER_CG_SHOP_BUY_ITEM:				return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketBuyItem(ch, data , iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_ADD_ITEM:				return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketAddItem(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_REMOVE_ITEM:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketRemoveItem(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_EDIT_ITEM:				return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketEditItem(ch,data,iBufferLeft);
	
	case offlineshop::SUBHEADER_CG_SHOP_FILTER_REQUEST:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketFilterRequest(ch,data,iBufferLeft);
	
	case offlineshop::SUBHEADER_CG_SHOP_OFFER_CREATE:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketCreateOffer(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_OFFER_ACCEPT:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketAcceptOffer(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_REQUEST_OFFER_LIST:		return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketOfferListRequest(ch);
	case offlineshop::SUBHEADER_CG_SHOP_OFFER_CANCEL:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketOfferCancel(ch, data,iBufferLeft);

	case offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_OPEN:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketOpenSafebox(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_GET_ITEM:		return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketGetItemSafebox(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_GET_VALUTES:	return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketGetValutesSafebox(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_CLOSE:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketCloseSafebox(ch,data,iBufferLeft);

	case offlineshop::SUBHEADER_CG_AUCTION_LIST_REQUEST:		return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketListRequest(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_AUCTION_OPEN_REQUEST:		return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketOpenAuctionRequest(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_MY_AUCTION_OPEN_REQUEST:		return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketOpenMyAuctionRequest(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_CREATE_AUCTION:				return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketCreateAuction(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_AUCTION_ADD_OFFER:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketAddOffer(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_EXIT_FROM_AUCTION:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketExitFromAuction(ch, data, iBufferLeft);

	case offlineshop::SUBHEADER_CG_CLOSE_BOARD:					return /*sizeof(TPacketCGNewOfflineshop) +*/ OfflineshopPacketCloseBoard(ch);
#ifdef ENABLE_NEW_SHOP_IN_CITIES
	case offlineshop::SUBHEADER_CG_CLICK_ENTITY:				return /*sizeof(TPacketCGNewOfflineshop) +*/ OfflineshopPacketClickEntity(ch, data, iBufferLeft);
#endif
	case offlineshop::SUBHEADER_CG_AUCTION_CLOSE:
		return /*sizeof(TPacketCGNewOfflineshop) +*/ OfflineshopPacketCloseMyAuction(ch);

	default:
		sys_err("UNKNOWN SUBHEADER %d ",pPack->bSubHeader);
		return -1;
	}

}
#endif

void CInputMain::ItemDestroy(LPCHARACTER ch, const char * data)
{
	struct command_item_destroy * pinfo = (struct command_item_destroy *) data;
	if (ch) {
		ch->DestroyItem(pinfo->Cell);
	}
}

#if defined(ENABLE_EXCHANGE_FRAGMENTS)
void CInputMain::FragmentsExchange(LPCHARACTER ch, const char * data)
{
	const TPacketExchangeFragments* pinfo = reinterpret_cast<const TPacketExchangeFragments*>(data);
	if (ch) {
		ch->ExchangeForFragments(pinfo->itemPos);
	}
}
#endif

void CInputMain::ItemDivision(LPCHARACTER ch, const char * data)
{
	struct command_item_division * pinfo = (struct command_item_division *) data;
	if (ch) {
		ch->ItemDivision(pinfo->pos);
	}
}

#ifdef ENABLE_NEW_FISHING_SYSTEM
void CInputMain::FishingNew(LPCHARACTER ch, const char* c_pData)
{
	if (!ch) {
		return;
	}

	TPacketFishingNew* p = (TPacketFishingNew*)c_pData;
	switch (p->subheader) {
		case FISHING_SUBHEADER_NEW_START:
			{
				ch->SetRotation(p->dir * 5);
				ch->fishing_new_start();
			}
			break;
		case FISHING_SUBHEADER_NEW_STOP:
			{
				ch->SetRotation(p->dir * 5);
				ch->fishing_new_stop();
			}
			break;
		case FISHING_SUBHEADER_NEW_CATCH:
			{
				ch->fishing_new_catch();
			}
			break;
		case FISHING_SUBHEADER_NEW_CATCH_FAILED:
			{
				ch->fishing_new_catch_failed();
			}
			break;
		default:
			return;
	}
}
#endif

#if defined(USE_BATTLEPASS)
int CInputMain::ReciveExtBattlePassActions(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	if (uiBytes < sizeof(TPacketCGExtBattlePassAction))
	{
		return -1;
	}

	const auto* p = reinterpret_cast<const TPacketCGExtBattlePassAction*>(data);
	if (!p) {
		return -1;
	}

	uiBytes -= sizeof(TPacketCGExtBattlePassAction);
	int32_t ret = 0;

	switch (p->bAction)
	{
		case 1:
		{
			CBattlePassManager::instance().BattlePassRequestOpen(ch);
			break;
		}
		case 2:
		{
			if (get_dword_time() < ch->GetLastReciveExtBattlePassOpenRanking()) {
				ch->ChatPacketNew(CHAT_TYPE_NOTICE, 1407, "%d", ((ch->GetLastReciveExtBattlePassOpenRanking() - get_dword_time()) / 1000) + 1);
				break;
			}

			ch->SetLastReciveExtBattlePassOpenRanking(get_dword_time() + 10000);

			BYTE bBattlePassID = CBattlePassManager::instance().GetBattlePassID();
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT player_name, battlepass_id, UNIX_TIMESTAMP(start_time), "
																			"UNIX_TIMESTAMP(end_time) FROM player.battlepass_playerindex "
																			"WHERE battlepass_id = %d and battlepass_completed = 1 and not "
																			"player_name LIKE '[%%' ORDER BY (UNIX_TIMESTAMP(end_time)-UNIX_TIMESTAMP(start_time)) "
																			"ASC LIMIT 40", bBattlePassID));
			if (pMsg->uiSQLErrno) {
				break;
			}

			MYSQL_ROW row;
			while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
			{
				TPacketGCExtBattlePassRanking pack;
				pack.bHeader = HEADER_GC_EXT_BATTLE_PASS_SEND_RANKING;
				strlcpy(pack.szPlayerName, row[0], sizeof(pack.szPlayerName));
				str_to_number(pack.bBattlePassID, row[1]);
				str_to_number(pack.dwStartTime, row[2]);
				str_to_number(pack.dwEndTime, row[3]);

				if (ch->GetDesc()) {
					ch->GetDesc()->Packet(&pack, sizeof(pack));
				}
			}
			break;
		}
		case 10:
		{
			CBattlePassManager::instance().BattlePassRequestReward(ch);
			break;
		}
		default:
		{
			break;
		}
	}

	return ret;
}

int CInputMain::ReciveExtBattlePassPremiumItem(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	TPacketCGExtBattlePassSendPremiumItem* p = (TPacketCGExtBattlePassSendPremiumItem*)data;

	if (uiBytes < sizeof(TPacketCGExtBattlePassSendPremiumItem))
		return -1;

	const char* c_pData = data + sizeof(TPacketCGExtBattlePassSendPremiumItem);
	uiBytes -= sizeof(TPacketCGExtBattlePassSendPremiumItem);

	LPITEM item = ch->GetInventoryItem(p->iSlotIndex);
	if (item != NULL && item->GetVnum() == 70611)
	{
		ch->PointChange(POINT_BATTLE_PASS_PREMIUM_ID, CBattlePassManager::instance().GetBattlePassID());
		CBattlePassManager::instance().BattlePassRequestOpen(ch);
		item->SetCount(item->GetCount() - 1);
		ch->ChatPacketNew(CHAT_TYPE_INFO, 1401, "");
	}
	return 0;
}
#endif

#ifdef ENABLE_SWITCHBOT_WORLDARD
void CInputMain::SwitchBotSend(LPCHARACTER ch, const char * data)
{
#if defined(ENABLE_RENEWAL_OX)
	if (ch->GetGMLevel() == GM_PLAYER && ch->GetMapIndex() == OX_MAP_INDEX) {
		return;
	}
#endif

	struct packet_send_switchbot * pinfo = (struct packet_send_switchbot *) data;
	switch (pinfo->subheader)
	{
		case SWITCH_SUB_HEADER_CHANGE:
		{
			if (CSwitchBonus::Instance().CheckElementsSB(ch))
				return;

			CSwitchBonus::Instance().ChangeSB(ch,pinfo->aAttr,pinfo->vnum_change);
		}
		break;

		case SWITCH_SUB_HEADER_OPEN:
		{
			CSwitchBonus::Instance().OpenSB(ch);
		}
		break;

		case SWITCH_SUB_HEADER_CLOSE:
		{
			CSwitchBonus::Instance().CloseSB(ch);
		}
		break;
	}
}
#endif

#ifdef USE_CAPTCHA_SYSTEM
void CInputMain::Captcha(LPCHARACTER pkChar, const char* c_pData)
{
    if (!pkChar)
    {
        return;
    }

    const auto* pInfo = reinterpret_cast<const TPacketCGCaptcha*>(c_pData);
    pkChar->ValidateCaptcha(pInfo->code);
}
#endif

#if defined(ENABLE_NEW_FISH_EVENT)
int32_t CInputMain::FishEvent(LPCHARACTER ch, const char* data, size_t uiBytes) {
	if (uiBytes < sizeof(TPacketCGFishEvent)) {
		return -1;
	}

	const auto* pInfo = reinterpret_cast<const TPacketCGFishEvent*>(data);
	const char* c_pData = data + sizeof(TPacketCGFishEvent);
	uiBytes -= sizeof(TPacketCGFishEvent);

	switch (pInfo->bSubheader) {
		case FISH_EVENT_SUBHEADER_BOX_USE: {
				if (uiBytes < sizeof(uint8_t) + sizeof(WORD)) {
					return -1;
				}

				uint8_t bWindow = *(c_pData);
				WORD wCell = *(WORD*)(c_pData + 1);

				if (test_server) {
					sys_log(0, "INPUT: %s FISH_EVENT: USE_BOX", ch->GetName());
				}

				ch->FishEventUseBox(TItemPos(bWindow, wCell));

				return (sizeof(uint8_t) + sizeof(WORD));
		}
		case FISH_EVENT_SUBHEADER_SHAPE_ADD: {
				if (uiBytes < sizeof(uint8_t)) {
					return -1;
				}

				uint8_t shapePos = *c_pData;

				if (test_server) {
				sys_log(0, "INPUT: %s FISH_EVENT: ADD_SHAPE", ch->GetName());
				}

				ch->FishEventAddShape(shapePos);

				return sizeof(uint8_t);
		}
		default: {
			sys_err("CInputMain::FishEvent : Unknown subheader %d : %s", pInfo->bSubheader, ch->GetName());
			break;
		}
	}

	return 0;
}
#endif

#if defined(ENABLE_LETTERS_EVENT)
void CInputMain::TakeLettersReward(LPCHARACTER pkChar) {
	if (!pkChar || !pkChar->GetDesc()) {
		return;
	}

	pkChar->CollectReward();
}
#endif

#if defined(ENABLE_AURA_SYSTEM)
size_t GetAuraSubPacketLength(const EPacketCGAuraSubHeader& SubHeader) {
	size_t ret = 0;

	switch (SubHeader) {
		case AURA_SUBHEADER_CG_REFINE_CHECKIN: {
			ret = sizeof(TSubPacketCGAuraRefineCheckIn);
			break;
		}
		case AURA_SUBHEADER_CG_REFINE_CHECKOUT: {
			ret = sizeof(TSubPacketCGAuraRefineCheckOut);
			break;
		}
		case AURA_SUBHEADER_CG_REFINE_ACCEPT: {
			ret = sizeof(TSubPacketCGAuraRefineAccept);
			break;
		}
		default: {
			break;
		}
	}

	return ret;
}

int32_t CInputMain::Aura(LPCHARACTER ch, const char* data, size_t uiBytes) {
	if (uiBytes < sizeof(TPacketCGAura)) {
		return -1;
	}

	const TPacketCGAura* pinfo = reinterpret_cast<const TPacketCGAura*>(data);
	const char* c_pData = data + sizeof(TPacketCGAura);

	uiBytes -= sizeof(TPacketCGAura);

	const EPacketCGAuraSubHeader SubHeader = static_cast<EPacketCGAuraSubHeader>(pinfo->bSubHeader);
	const size_t SubPacketLength = GetAuraSubPacketLength(SubHeader);
	if (uiBytes < SubPacketLength) {
		sys_err("invalid aura subpacket length (sublen %d size %u buffer %u)", SubPacketLength, sizeof(TPacketCGAura), uiBytes);
		return -1;
	}

	switch (SubHeader) {
		case AURA_SUBHEADER_CG_REFINE_CHECKIN:
			{
				const TSubPacketCGAuraRefineCheckIn* sp = reinterpret_cast<const TSubPacketCGAuraRefineCheckIn*>(c_pData);
				ch->AuraRefineWindowCheckIn(sp->byAuraRefineWindowType, sp->AuraCell, sp->ItemCell);
			}
			return SubPacketLength;
		case AURA_SUBHEADER_CG_REFINE_CHECKOUT:
			{
				const TSubPacketCGAuraRefineCheckOut* sp = reinterpret_cast<const TSubPacketCGAuraRefineCheckOut*>(c_pData);
				ch->AuraRefineWindowCheckOut(sp->byAuraRefineWindowType, sp->AuraCell);
			}
			return SubPacketLength;
		case AURA_SUBHEADER_CG_REFINE_ACCEPT:
			{
				const TSubPacketCGAuraRefineAccept* sp = reinterpret_cast<const TSubPacketCGAuraRefineAccept*>(c_pData);
				ch->AuraRefineWindowAccept(sp->byAuraRefineWindowType);
			}
			return SubPacketLength;
		case AURA_SUBHEADER_CG_REFINE_CANCEL:
			{
				ch->AuraRefineWindowClose();
			}
			return SubPacketLength;
	}

	return 0;
}
#endif

#if defined(USE_ATTR_6TH_7TH)
void CInputMain::Attr67Add(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGAttr67Add* pkPacket = (TPacketCGAttr67Add*)c_pData;
	switch (pkPacket->bySubHeader)
	{
		case SUBHEADER_CG_ATTR67_ADD_CLOSE:
		{
			ch->SetOpenAttr67Add(false);
			break;
		}
		case SUBHEADER_CG_ATTR67_ADD_OPEN:
		{
			if (!ch->IsOpenAttr67Add())
				ch->SetOpenAttr67Add(true);
			break;
		}
		case SUBHEADER_CG_ATTR67_ADD_REGIST:
		{
			if (ch->IsOpenAttr67Add())
				ch->Attr67Add(pkPacket->Attr67AddData);
			break;
		}
		default:
		{
			break;
		}
	}
}
#endif

#ifdef USE_MULTIPLE_OPENING
void CInputMain::MultipleOpening(LPCHARACTER ch, const char* c_pData)
{
    const LPDESC d = ch ? ch->GetDesc() : nullptr;
    if (!d)
    {
        return;
    }

    const auto* info = reinterpret_cast<const TPacketCGMultipleOpening*>(c_pData);
    if (!info)
    {
        return;
    }

    ch->MultipleOpening(info->pos, info->count);
}
#endif

int CInputMain::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

#if defined(ENABLE_ANTI_FLOOD)
	if (bHeader != HEADER_CG_ITEM_USE &&
		bHeader != HEADER_CG_ITEM_PICKUP &&
		bHeader != HEADER_CG_MOVE
	) {
		if (thecore_pulse() > ch->GetAntiFloodPulse(FLOOD_PACKETS) + PASSES_PER_SEC(1)) {
			ch->SetAntiFloodCount(FLOOD_PACKETS, 0);
			ch->SetAntiFloodPulse(FLOOD_PACKETS, thecore_pulse());
		}

		if (ch->IncreaseAntiFloodCount(FLOOD_PACKETS) >= 300) {
			const LPDESC d = ch->GetDesc();
			if (d) {
				d->SetPhase(PHASE_CLOSE);
				sys_err("Player: %s host(%s) header(%d) is trying to flood with the packets.",
						ch->GetName(), inet_ntoa(d->GetAddr().sin_addr), bHeader);
				return -1;
			}
		}
	}
#endif

	int iExtraLen = 0;

	if (test_server && bHeader != HEADER_CG_MOVE)
		sys_log(0, "CInputMain::Analyze() ==> Header [%d] ", bHeader);

	switch (bHeader)
	{
#if defined(USE_ATTR_6TH_7TH)
		case HEADER_CG_ATTR67_ADD:
			Attr67Add(ch, c_pData);
			break;
#endif
#if defined(ENABLE_AURA_SYSTEM)
		case HEADER_CG_AURA:
			if ((iExtraLen = Aura(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif
#if defined(ENABLE_LETTERS_EVENT)
		case HEADER_CG_TAKE_LETTERS_REWARD:
			TakeLettersReward(ch);
			break;
#endif
#if defined(ENABLE_NEW_FISH_EVENT)
		case HEADER_CG_FISH_EVENT_SEND:
			if ((iExtraLen = FishEvent(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif
#ifdef USE_CAPTCHA_SYSTEM
        case HEADER_CG_CAPTCHA:
            Captcha(ch, c_pData);
            break;
#endif
#if defined(USE_BATTLEPASS)
		case HEADER_CG_EXT_BATTLE_PASS_ACTION:
			if ((iExtraLen = ReciveExtBattlePassActions(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
			
		case HEADER_CG_EXT_SEND_BP_PREMIUM_ITEM:
			if ((iExtraLen = ReciveExtBattlePassPremiumItem(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

		case HEADER_CG_PONG:
			Pong(d);
			break;

		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;

		case HEADER_CG_CHAT:
			if (test_server)
			{
				char* pBuf = (char*)c_pData;
				sys_log(0, "%s", pBuf + sizeof(TPacketCGChat));
			}

			if ((iExtraLen = Chat(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_WHISPER:
			if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_MOVE:
			Move(ch, c_pData);
			// @fixme103 (removed CheckClientVersion since useless in here)
			break;

		case HEADER_CG_CHARACTER_POSITION:
			Position(ch, c_pData);
			break;

		case HEADER_CG_ITEM_USE:
			if (!ch->IsObserverMode())
				ItemUse(ch, c_pData);
			break;

		case HEADER_CG_ITEM_DROP:
			if (!ch->IsObserverMode())
			{
				ItemDrop(ch, c_pData);
			}
			break;

#ifdef ENABLE_ACCE_SYSTEM
		case HEADER_CG_ACCE: {
			Acce(ch, c_pData);
			break;
		}
#endif
#if defined(ENABLE_CHANGELOOK)
		case HEADER_CG_CHANGE_LOOK: {
			ChangeLook(ch, c_pData);
			break;
		}
#endif
		case HEADER_CG_ITEM_DROP2:
			if (!ch->IsObserverMode())
				ItemDrop2(ch, c_pData);
			break;
		case HEADER_CG_ITEM_DESTROY:
			if (!ch->IsObserverMode())
				ItemDestroy(ch, c_pData);
			break;
#if defined(ENABLE_EXCHANGE_FRAGMENTS)
		case HEADER_CG_EXCHANGE_FRAGMENTS:
			if (!ch->IsObserverMode())
				FragmentsExchange(ch, c_pData);
			break;
#endif
		case HEADER_CG_ITEM_DIVISION:
			{
				if (!ch->IsObserverMode())
					ItemDivision(ch, c_pData);
			}
			break;
		case HEADER_CG_ITEM_MOVE:
			if (!ch->IsObserverMode())
				ItemMove(ch, c_pData);
			break;
		case HEADER_CG_ITEM_PICKUP:
			if (!ch->IsObserverMode())
				ItemPickup(ch, c_pData);
			break;

		case HEADER_CG_ITEM_USE_TO_ITEM:
			if (!ch->IsObserverMode())
				ItemToItem(ch, c_pData);
			break;
		case HEADER_CG_ITEM_GIVE:
			if (!ch->IsObserverMode())
				ItemGive(ch, c_pData);
			break;

		case HEADER_CG_EXCHANGE:
			if (!ch->IsObserverMode())
				Exchange(ch, c_pData);
			break;

		case HEADER_CG_ATTACK:
		case HEADER_CG_SHOOT:
			if (!ch->IsObserverMode())
			{
				Attack(ch, bHeader, c_pData);
			}
			break;

		case HEADER_CG_USE_SKILL:
			if (!ch->IsObserverMode())
				UseSkill(ch, c_pData);
			break;

#ifdef __SKILL_COLOR_SYSTEM__
		case HEADER_CG_SKILL_COLOR:
			SetSkillColor(ch, c_pData);
			break;
#endif
#ifdef ENABLE_OPENSHOP_PACKET
		case HEADER_CG_OPENSHOP: {
				TPacketOpenShop* p = reinterpret_cast<TPacketOpenShop*>((void*)c_pData);
				if (p->shopid > 0) {
					if (!(ch->IsObserverMode() || ch->IsOpenSafebox() || ch->GetExchange() || ch->IsCubeOpen() || ch->IsStun() || ch->IsDead()
#ifdef ENABLE_ACCE_SYSTEM
 || ch->IsAcceOpen()
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
 || ch->GetOfflineShopGuest() || ch->GetAuctionGuest()
#endif
#if defined(ENABLE_AURA_SYSTEM)
 || ch->IsAuraRefineWindowOpen()
#endif
#ifdef __ATTR_TRANSFER_SYSTEM__
 || ch->IsAttrTransferOpen()
#endif
#if defined(ENABLE_CHANGELOOK)
 || ch->isChangeLookOpened()
#endif
#if defined(USE_ATTR_6TH_7TH)
 || ch->IsOpenAttr67Add()
#endif
					)) {
						LPSHOP shop = CShopManager::instance().Get(p->shopid);
						if (shop) {
							shop->AddGuest(ch, 0, false);
							ch->SetShopOwner(NULL);
						}
					}
				}
			} break;
#endif
		case HEADER_CG_QUICKSLOT_ADD:
			QuickslotAdd(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_DEL:
			QuickslotDelete(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_SWAP:
			QuickslotSwap(ch, c_pData);
			break;

		case HEADER_CG_SHOP:
			if ((iExtraLen = Shop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#if defined(ENABLE_GAYA_RENEWAL)
		case HEADER_CG_GEM_SHOP:
			if ((iExtraLen = GemShop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif
		case HEADER_CG_MESSENGER:
			if ((iExtraLen = Messenger(ch, c_pData, m_iBufferLeft))<0)
				return -1;
			break;

		case HEADER_CG_ON_CLICK:
			OnClick(ch, c_pData);
			break;

		case HEADER_CG_SYNC_POSITION:
			if ((iExtraLen = SyncPosition(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_ADD_FLY_TARGETING:
		case HEADER_CG_FLY_TARGETING:
			FlyTarget(ch, c_pData, bHeader);
			break;

		case HEADER_CG_SCRIPT_BUTTON:
			ScriptButton(ch, c_pData);
			break;

			// SCRIPT_SELECT_ITEM
		case HEADER_CG_SCRIPT_SELECT_ITEM:
			ScriptSelectItem(ch, c_pData);
			break;
			// END_OF_SCRIPT_SELECT_ITEM

		case HEADER_CG_SCRIPT_ANSWER:
			ScriptAnswer(ch, c_pData);
			break;

		case HEADER_CG_QUEST_INPUT_STRING:
			QuestInputString(ch, c_pData);
			break;

		case HEADER_CG_QUEST_CONFIRM:
			QuestConfirm(ch, c_pData);
			break;

		case HEADER_CG_TARGET:
			Target(ch, c_pData);
			break;

		case HEADER_CG_WARP:
			Warp(ch, c_pData);
			break;

		case HEADER_CG_SAFEBOX_CHECKIN:
			SafeboxCheckin(ch, c_pData);
			break;

		case HEADER_CG_SAFEBOX_CHECKOUT:
			SafeboxCheckout(ch, c_pData, false);
			break;

		case HEADER_CG_SAFEBOX_ITEM_MOVE:
			SafeboxItemMove(ch, c_pData);
			break;

		case HEADER_CG_MALL_CHECKOUT:
			SafeboxCheckout(ch, c_pData, true);
			break;

		case HEADER_CG_PARTY_INVITE:
			PartyInvite(ch, c_pData);
			break;

		case HEADER_CG_PARTY_REMOVE:
			PartyRemove(ch, c_pData);
			break;

		case HEADER_CG_PARTY_INVITE_ANSWER:
			PartyInviteAnswer(ch, c_pData);
			break;

		case HEADER_CG_PARTY_SET_STATE:
			PartySetState(ch, c_pData);
			break;

		case HEADER_CG_PARTY_USE_SKILL:
			PartyUseSkill(ch, c_pData);
			break;

		case HEADER_CG_PARTY_PARAMETER:
			PartyParameter(ch, c_pData);
			break;
#ifdef __INGAME_WIKI__
		case InGameWiki::HEADER_CG_WIKI:
			RecvWikiPacket(ch, c_pData);
			break;
#endif
		case HEADER_CG_ANSWER_MAKE_GUILD:
#ifdef ENABLE_NEWGUILDMAKE
			ch->ChatPacket(CHAT_TYPE_INFO, "<%s> AnswerMakeGuild disabled", __FUNCTION__);
#else
			AnswerMakeGuild(ch, c_pData);
#endif
			break;

		case HEADER_CG_GUILD:
			if ((iExtraLen = Guild(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_FISHING:
			Fishing(ch, c_pData);
			break;
		case HEADER_CG_HACK:
			Hack(ch, c_pData);
			break;
			
#ifdef __NEWPET_SYSTEM__
		case HEADER_CG_PetSetName:
			BraveRequestPetName(ch, c_pData);
			break;
#endif
		case HEADER_CG_MYSHOP:
			if ((iExtraLen = MyShop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_REFINE:
			Refine(ch, c_pData);
			break;

#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
		case HEADER_CG_WHISPER_ADMIN:
			CWhisperAdmin::instance().Manager(ch, c_pData);
			break;
#endif

		case HEADER_CG_CLIENT_VERSION:
			Version(ch, c_pData);
			break;

#ifdef ENABLE_SWITCHBOT_WORLDARD
		case HEADER_CG_SWITCHBOT:
			{
				SwitchBotSend(ch, c_pData);
				break;
			}
#endif
		case HEADER_CG_DRAGON_SOUL_REFINE:
			{
				TPacketCGDragonSoulRefine* p = reinterpret_cast <TPacketCGDragonSoulRefine*>((void*)c_pData);
				switch(p->bSubType)
				{
				case DS_SUB_HEADER_CLOSE:
					ch->DragonSoul_RefineWindow_Close();
					break;
				case DS_SUB_HEADER_DO_REFINE_GRADE:
					{
						DSManager::instance().DoRefineGrade(ch, p->ItemGrid);
					}
					break;
				case DS_SUB_HEADER_DO_REFINE_STEP:
					{
						DSManager::instance().DoRefineStep(ch, p->ItemGrid);
					}
					break;
				case DS_SUB_HEADER_DO_REFINE_STRENGTH:
					{
						DSManager::instance().DoRefineStrength(ch, p->ItemGrid);
					}
					break;
				}
			}
			break;
#ifdef ENABLE_DS_REFINE_ALL
		case HEADER_CG_DRAGON_SOUL_REFINE_ALL: {
			TPacketDragonSoulRefineAll* p = reinterpret_cast <TPacketDragonSoulRefineAll*>((void*)c_pData);
			DSManager::instance().DoRefineAll(ch, p->subheader, p->type, p->grade);
		} break;
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
		case HEADER_CG_NEW_OFFLINESHOP:
			if((iExtraLen = OfflineshopPacket(c_pData, ch, m_iBufferLeft))< 0)
				return -1;
			break;
#endif
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
		case HEADER_CG_CUBE_RENEWAL:
			CubeRenewalSend(ch, c_pData);
			break;
#endif
#ifdef __ENABLE_PREMIUM_PLAYERS__
	case HEADER_CG_PREMIUM_PLAYERS:
		RecvPremiumPlayersPacket(ch, c_pData);
		break;
#endif
#ifdef ENABLE_NEW_FISHING_SYSTEM
		case HEADER_CG_FISHING_NEW:
			{
				FishingNew(ch, c_pData);
			}
			break;
#endif
#ifdef USE_MULTIPLE_OPENING
        case HEADER_CG_MULTI_CHESTS_OPEN:
        {
            MultipleOpening(ch, c_pData);
            break;
        }
#endif

		case HEADER_CG_REQ_MAP_BOSS_INFO:
			RequestMapBossInfo(ch);
			break;

		case HEADER_CG_WHISPER_INFO:
			WhisperInfo(ch, c_pData);
			break;
	}

	return (iExtraLen);
}

int CInputDead::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		return -1;
	}

	int iExtraLen = 0;

	switch (bHeader)
	{
		case HEADER_CG_PONG:
			Pong(d);
			break;

		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;

		case HEADER_CG_CHAT:
			if ((iExtraLen = Chat(d, c_pData, m_iBufferLeft)) < 0)
				return -1;

			break;

		case HEADER_CG_WHISPER:
			if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;

			break;

		case HEADER_CG_HACK:
			Hack(ch, c_pData);
			break;

		case HEADER_CG_REQ_MAP_BOSS_INFO:
			RequestMapBossInfo(ch);
			break;

		case HEADER_CG_WHISPER_INFO:
			WhisperInfo(ch, c_pData);
			break;

		default:
			return -1;
	}

	return (iExtraLen);
}

#ifdef __ENABLE_PREMIUM_PLAYERS__
void CInputMain::RecvPremiumPlayersPacket(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGPremiumPlayers* p = reinterpret_cast<const TPacketCGPremiumPlayers*>(c_pData);
	switch (p->bySubHeader)
	{
		case PREMIUM_PLAYERS_SUBHEADER_CG_OPEN:
			ch->PremiumPlayersOpenPacket();
			break;
		case PREMIUM_PLAYERS_SUBHEADER_CG_LIST:
			//ch->PremiumPlayersListPacket();
			break;
		case PREMIUM_PLAYERS_SUBHEADER_CG_ACTIVATE:
			ch->PremiumPlayersActivatePacket();
			break;
		default:
			break;
	}
}
#endif

void CInputMain::RequestMapBossInfo(const LPCHARACTER ch)
{
	const LPDESC desc = ch ? ch->GetDesc() : nullptr;
	if (!desc)
		return;

	const auto map_index = ch->GetMapIndex();

	if (g_BossPosition.find(map_index) == g_BossPosition.end())
		return;

	const auto& pos = g_BossPosition[map_index];
	if (pos.empty())
		return;

	SPacketGCBossPosition packet = {};
	packet.header = HEADER_GC_BOSS_POSITION;
	packet.size = sizeof(packet) + (sizeof(SBossPosition) * pos.size());
	desc->BufferedPacket(&packet, sizeof(packet));

	for (const auto& e : pos)
		desc->Packet(e, sizeof(SBossPosition));
}

void CInputMain::WhisperInfo(const LPCHARACTER ch, const char* c_pData)
{
	const LPDESC desc = ch ? ch->GetDesc() : nullptr;
	if (!desc)
		return;

	const auto p = reinterpret_cast<const SPacketCGWhisperInfo*>(c_pData);

	if (strlen(p->name) == 0)
		return;

	std::string country_flag;
	uint8_t empire;

	const auto tch = CHARACTER_MANAGER::instance().FindPC(p->name);
	if (!tch)
	{
		const CCI* pkCCI = P2P_MANAGER::instance().Find(p->name);
		if (!pkCCI)
			return;

		country_flag = pkCCI->country_flag;
		empire = pkCCI->bEmpire;
	}
	else
	{
		country_flag = tch->GetCountryFlag();
		empire = tch->GetEmpire();
	}

	if (country_flag.empty() || empire == 0)
		return;

	SPacketGCWhisperInfo pgc;
	pgc.header = HEADER_GC_WHISPER_INFO;
	strlcpy(pgc.name, p->name, sizeof(pgc.name));
	strlcpy(pgc.country_flag, country_flag.c_str(), sizeof(pgc.country_flag));
	pgc.empire = empire;
	desc->Packet(&pgc, sizeof(pgc));
}