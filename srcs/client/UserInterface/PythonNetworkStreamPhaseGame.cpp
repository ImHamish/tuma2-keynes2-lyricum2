#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "Packet.h"

#include "PythonGuild.h"
#include "PythonCharacterManager.h"
#include "PythonPlayer.h"
#include "PythonBackground.h"
#include "PythonMiniMap.h"
#include "PythonTextTail.h"
#include "PythonItem.h"
#include "PythonChat.h"
#include "PythonShop.h"
#include "PythonExchange.h"
#include "PythonQuest.h"
#include "PythonEventManager.h"
#include "PythonMessenger.h"
#include "PythonApplication.h"

#include "../EterPack/EterPackManager.h"
#include "../gamelib/ItemManager.h"

#include "AbstractApplication.h"
#include "AbstractCharacterManager.h"
#include "InstanceBase.h"

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
#include "PythonCubeRenewal.h"
#endif

#include "ProcessCRC.h"
#ifdef INGAME_WIKI
#include "../GameLib/in_game_wiki.h"
#endif
#if defined(ENABLE_NEW_ANTICHEAT_RULES)
#include "Resource.h"
#endif

void CPythonNetworkStream::__RefreshAlignmentWindow()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshAlignment", Py_BuildValue("()"));
}

void CPythonNetworkStream::__RefreshTargetBoardByVID(DWORD dwVID)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoardByVID", Py_BuildValue("(i)", dwVID));
}

void CPythonNetworkStream::__RefreshTargetBoardByName(const char * c_szName)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoardByName", Py_BuildValue("(s)", c_szName));
}

void CPythonNetworkStream::__RefreshTargetBoard()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoard", Py_BuildValue("()"));
}

void CPythonNetworkStream::__RefreshGuildWindowGradePage()
{
	m_isRefreshGuildWndGradePage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowSkillPage()
{
	m_isRefreshGuildWndSkillPage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowMemberPageGradeComboBox()
{
	m_isRefreshGuildWndMemberPageGradeComboBox=true;
}

void CPythonNetworkStream::__RefreshGuildWindowMemberPage()
{
	m_isRefreshGuildWndMemberPage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowBoardPage()
{
	m_isRefreshGuildWndBoardPage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowInfoPage()
{
	m_isRefreshGuildWndInfoPage=true;
}

void CPythonNetworkStream::__RefreshMessengerWindow()
{
	m_isRefreshMessengerWnd=true;
}

void CPythonNetworkStream::__RefreshSafeboxWindow()
{
	m_isRefreshSafeboxWnd=true;
}

void CPythonNetworkStream::__RefreshMallWindow()
{
	m_isRefreshMallWnd=true;
}

void CPythonNetworkStream::__RefreshSkillWindow()
{
	m_isRefreshSkillWnd=true;
}

void CPythonNetworkStream::__RefreshExchangeWindow()
{
	m_isRefreshExchangeWnd=true;
}

void CPythonNetworkStream::__RefreshStatus()
{
	m_isRefreshStatus=true;
}

void CPythonNetworkStream::__RefreshCharacterWindow()
{
	m_isRefreshCharacterWnd=true;
}

void CPythonNetworkStream::__RefreshInventoryWindow()
{
	m_isRefreshInventoryWnd=true;
}

void CPythonNetworkStream::__RefreshEquipmentWindow()
{
	m_isRefreshEquipmentWnd=true;
}

void CPythonNetworkStream::__SetGuildID(DWORD id)
{
	if (m_dwGuildID != id)
	{
		m_dwGuildID = id;
		IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();

		for (int i = 0; i < PLAYER_PER_ACCOUNT4; ++i)
			if (!strncmp(m_akSimplePlayerInfo[i].szName, rkPlayer.GetName(), CHARACTER_NAME_MAX_LEN))
			{
				m_adwGuildID[i] = id;

				std::string  guildName;
				if (CPythonGuild::Instance().GetGuildName(id, &guildName))
				{
					m_astrGuildName[i] = guildName;
				}
				else
				{
					m_astrGuildName[i] = "";
				}
			}
	}
}

struct PERF_PacketInfo
{
	DWORD dwCount;
	DWORD dwTime;

	PERF_PacketInfo()
	{
		dwCount=0;
		dwTime=0;
	}
};


#ifdef __PERFORMANCE_CHECK__

class PERF_PacketTimeAnalyzer
{
	public:
		~PERF_PacketTimeAnalyzer()
		{
			FILE* fp=fopen("perf_dispatch_packet_result.txt", "w");

			for (std::map<DWORD, PERF_PacketInfo>::iterator i=m_kMap_kPacketInfo.begin(); i!=m_kMap_kPacketInfo.end(); ++i)
			{
				if (i->second.dwTime>0)
					fprintf(fp, "header %d: count %d, time %d, tpc %d\n", i->first, i->second.dwCount, i->second.dwTime, i->second.dwTime/i->second.dwCount);
			}
			fclose(fp);
		}

	public:
		std::map<DWORD, PERF_PacketInfo> m_kMap_kPacketInfo;
};

PERF_PacketTimeAnalyzer gs_kPacketTimeAnalyzer;

#endif

void CPythonNetworkStream::NotifyHack(const char* c_szMsg)
{
	if (!m_kQue_stHack.empty())
		if (c_szMsg == m_kQue_stHack.back())
			return;

	m_kQue_stHack.push_back(c_szMsg);
}

bool CPythonNetworkStream::__SendHack(const char* c_szMsg)
{
	Tracen(c_szMsg);

	TPacketCGHack kPacketHack;
	kPacketHack.bHeader = HEADER_CG_HACK;
	strncpy(kPacketHack.szBuf, c_szMsg, sizeof(kPacketHack.szBuf) - 1);

	if (!Send(sizeof(kPacketHack), &kPacketHack))
		return false;

	return SendSequence();
}

// Game Phase ---------------------------------------------------------------------------
void CPythonNetworkStream::GamePhase()
{
	if (!m_kQue_stHack.empty())
	{
		__SendHack(m_kQue_stHack.front().c_str());
		m_kQue_stHack.pop_front();
	}

	TPacketHeader header = 0;
	bool ret = true;

#ifdef __PERFORMANCE_CHECK__
	DWORD timeBeginDispatch=timeGetTime();

	static std::map<DWORD, PERF_PacketInfo> kMap_kPacketInfo;
	kMap_kPacketInfo.clear();
#endif

	const DWORD MAX_RECV_COUNT = 8 * 2;
	const DWORD SAFE_RECV_BUFSIZE = 8192 * 2;
	DWORD dwRecvCount = 0;

	while (ret)
	{
		if (dwRecvCount++ >= MAX_RECV_COUNT-1 && 
			GetRecvBufferSize() < SAFE_RECV_BUFSIZE
			&& m_strPhase == "Game") {
				break;
			}

		if (!CheckPacket(&header))
			break;

#ifdef __PERFORMANCE_CHECK__
		DWORD timeBeginPacket=timeGetTime();
#endif

#if defined(_DEBUG)
		Tracenf("RECV HEADER: %u (phase: %s)", header, m_strPhase.c_str());
#endif

#if defined(ENABLE_CLOSE_GAMECLIENT_CMD)
		if (header == HEADER_GC_CLOSECLIENT) {
			RecvCloseClient();
			break;
		}
#endif

#if defined(ENABLE_NEW_ANTICHEAT_RULES)
		if (header == HEADER_GC_ANTICHEAT) {
			RecvAntiCheat();
			break;
		}
#endif

		switch (header)
		{
#if defined(ENABLE_AURA_SYSTEM)
			case HEADER_GC_AURA:
				ret = RecvAuraPacket();
				break;
#endif
			case HEADER_GC_OBSERVER_ADD:
				ret = RecvObserverAddPacket();
				break;
			case HEADER_GC_OBSERVER_REMOVE:
				ret = RecvObserverRemovePacket();
				break;
			case HEADER_GC_OBSERVER_MOVE:
				ret = RecvObserverMovePacket();
				break;
			case HEADER_GC_WARP:
				ret = RecvWarpPacket();
				break;

			case HEADER_GC_PHASE:
				ret = RecvPhasePacket();
#ifdef USE_CAPTCHA_SYSTEM
                if (!CPythonApplication::Instance().IsWindowVisisble())
                {
                    CPythonApplication::Instance().SetWindowInvisible(false);
                }
#endif
				return; // 도중에 Phase 가 바뀌면 일단 무조건 GamePhase 탈출 - [levites]
				break;

			case HEADER_GC_PVP:
				ret = RecvPVPPacket();
				break;

			case HEADER_GC_DUEL_START:
				ret = RecvDuelStartPacket();
				break;

			case HEADER_GC_CHARACTER_ADD:
 				ret = RecvCharacterAppendPacket();
				break;

			case HEADER_GC_CHAR_ADDITIONAL_INFO:
				ret = RecvCharacterAdditionalInfo();
				break;

			case HEADER_GC_CHARACTER_ADD2:
				ret = RecvCharacterAppendPacketNew();
				break;

			case HEADER_GC_CHARACTER_UPDATE:
				ret = RecvCharacterUpdatePacket();
				break;

			case HEADER_GC_CHARACTER_UPDATE2:
				ret = RecvCharacterUpdatePacketNew();
				break;

			case HEADER_GC_CHARACTER_DEL:
				ret = RecvCharacterDeletePacket();
				break;

			case HEADER_GC_CHAT:
				ret = RecvChatPacket();
				break;

			case HEADER_GC_SYNC_POSITION:
				ret = RecvSyncPositionPacket();
				break;

			case HEADER_GC_OWNERSHIP:
				ret = RecvOwnerShipPacket();
				break;

			case HEADER_GC_WHISPER:
				ret = RecvWhisperPacket();
				break;

			case HEADER_GC_CHARACTER_MOVE:
				ret = RecvCharacterMovePacket();
				break;

			// Position
			case HEADER_GC_CHARACTER_POSITION:
				ret = RecvCharacterPositionPacket();
				break;

			// Battle Packet
			case HEADER_GC_STUN:
				ret = RecvStunPacket();
				break;

			case HEADER_GC_DEAD:
				ret = RecvDeadPacket();
				break;
			case HEADER_GC_PLAYER_POINT_CHANGE:
				ret = RecvPointChange();
				break;
#if defined(ENABLE_NEW_GOLD_LIMIT)
			case HEADER_GC_CHARACTER_GOLD_CHANGE:
				if (RecvGoldChange())
					return;
				break;
			case HEADER_GC_CHARACTER_GOLD:
				ret = __RecvPlayerGold();
				break;
#endif
			case HEADER_GC_ITEM_SET:
				ret = RecvItemSetPacket();
				break;

			case HEADER_GC_ITEM_SET2:
				ret = RecvItemSetPacket2();
				break;

			case HEADER_GC_ITEM_USE:
				ret = RecvItemUsePacket();
				break;

			case HEADER_GC_ITEM_UPDATE:
				ret = RecvItemUpdatePacket();
				break;

			case HEADER_GC_ITEM_GROUND_ADD:
				ret = RecvItemGroundAddPacket();
				break;

			case HEADER_GC_ITEM_GROUND_DEL:
				ret = RecvItemGroundDelPacket();
				break;

			case HEADER_GC_ITEM_OWNERSHIP:
				ret = RecvItemOwnership();
				break;

			case HEADER_GC_QUICKSLOT_ADD: {
				ret = RecvQuickSlotAddPacket();
				break;
			}
			case HEADER_GC_QUICKSLOT_DEL: {
				ret = RecvQuickSlotDelPacket();
				break;
			}
			case HEADER_GC_QUICKSLOT_SWAP: {
				ret = RecvQuickSlotMovePacket();
				break;
			}
			case HEADER_GC_MOTION:
				ret = RecvMotionPacket();
				break;

			case HEADER_GC_SHOP:
				ret = RecvShopPacket();
				break;

			case HEADER_GC_SHOP_SIGN:
				ret = RecvShopSignPacket();
				break;

			case HEADER_GC_EXCHANGE:
				ret = RecvExchangePacket();
				break;

			case HEADER_GC_QUEST_INFO:
				ret = RecvQuestInfoPacket();
				break;

			case HEADER_GC_REQUEST_MAKE_GUILD:
				ret = RecvRequestMakeGuild();
				break;

			case HEADER_GC_PING:
				ret = RecvPingPacket();
				break;

			case HEADER_GC_SCRIPT:
				ret = RecvScriptPacket();
				break;

			case HEADER_GC_QUEST_CONFIRM:
				ret = RecvQuestConfirmPacket();
				break;

			case HEADER_GC_TARGET:
				ret = RecvTargetPacket();
				break;
			case HEADER_GC_DAMAGE_INFO:
				ret = RecvDamageInfoPacket();
				break;

			case HEADER_GC_MOUNT:
				ret = RecvMountPacket();
				break;

			case HEADER_GC_CHANGE_SPEED:
				ret = RecvChangeSpeedPacket();
				break;

			case HEADER_GC_PLAYER_POINTS:
				ret = __RecvPlayerPoints();
				break;

			case HEADER_GC_CREATE_FLY:
				ret = RecvCreateFlyPacket();
				break;

			case HEADER_GC_FLY_TARGETING:
				ret = RecvFlyTargetingPacket();
				break;

			case HEADER_GC_ADD_FLY_TARGETING:
				ret = RecvAddFlyTargetingPacket();
				break;

			case HEADER_GC_SKILL_LEVEL:
				ret = RecvSkillLevel();
				break;

			case HEADER_GC_SKILL_LEVEL_NEW:
				ret = RecvSkillLevelNew();
				break;

			case HEADER_GC_MESSENGER:
				ret = RecvMessenger();
				break;

			case HEADER_GC_GUILD:
				ret = RecvGuild();
				break;

			case HEADER_GC_PARTY_INVITE:
				ret = RecvPartyInvite();
				break;

			case HEADER_GC_PARTY_ADD:
				ret = RecvPartyAdd();
				break;

			case HEADER_GC_PARTY_UPDATE:
				ret = RecvPartyUpdate();
				break;

			case HEADER_GC_PARTY_REMOVE:
				ret = RecvPartyRemove();
				break;

			case HEADER_GC_PARTY_LINK:
				ret = RecvPartyLink();
				break;

			case HEADER_GC_PARTY_UNLINK:
				ret = RecvPartyUnlink();
				break;

			case HEADER_GC_PARTY_PARAMETER:
				ret = RecvPartyParameter();
				break;

			case HEADER_GC_SAFEBOX_SET:
				ret = RecvSafeBoxSetPacket();
				break;

			case HEADER_GC_SAFEBOX_DEL:
				ret = RecvSafeBoxDelPacket();
				break;

			case HEADER_GC_SAFEBOX_WRONG_PASSWORD:
				ret = RecvSafeBoxWrongPasswordPacket();
				break;

			case HEADER_GC_SAFEBOX_SIZE:
				ret = RecvSafeBoxSizePacket();
				break;

			case HEADER_GC_SAFEBOX_MONEY_CHANGE:
				ret = RecvSafeBoxMoneyChangePacket();
				break;

			case HEADER_GC_FISHING:
				ret = RecvFishing();
				break;

			case HEADER_GC_DUNGEON:
				ret = RecvDungeon();
				break;

			case HEADER_GC_TIME:
				ret = RecvTimePacket();
				break;

			case HEADER_GC_WALK_MODE:
				ret = RecvWalkModePacket();
				break;

			case HEADER_GC_CHANGE_SKILL_GROUP:
				ret = RecvChangeSkillGroupPacket();
				break;

			case HEADER_GC_REFINE_INFORMATION:
				ret = RecvRefineInformationPacket();
				break;

			case HEADER_GC_SEPCIAL_EFFECT:
				ret = RecvSpecialEffect();
				break;

			case HEADER_GC_NPC_POSITION:
				ret = RecvNPCList();
				break;

			case HEADER_GC_CHANNEL:
				ret = RecvChannelPacket();
				break;

			case HEADER_GC_VIEW_EQUIP:
				ret = RecvViewEquipPacket();
				break;

			case HEADER_GC_LAND_LIST:
				ret = RecvLandPacket();
				break;

			//case HEADER_GC_TARGET_CREATE:
			//	ret = RecvTargetCreatePacket();
			//	break;

			case HEADER_GC_TARGET_CREATE_NEW:
				ret = RecvTargetCreatePacketNew();
				break;

			case HEADER_GC_TARGET_UPDATE:
				ret = RecvTargetUpdatePacket();
				break;

			case HEADER_GC_TARGET_DELETE:
				ret = RecvTargetDeletePacket();
				break;

			case HEADER_GC_AFFECT_ADD:
				ret = RecvAffectAddPacket();
				break;

			case HEADER_GC_AFFECT_REMOVE:
				ret = RecvAffectRemovePacket();
				break;

			case HEADER_GC_MALL_OPEN:
				ret = RecvMallOpenPacket();
				break;

			case HEADER_GC_MALL_SET:
				ret = RecvMallItemSetPacket();
				break;

			case HEADER_GC_MALL_DEL:
				ret = RecvMallItemDelPacket();
				break;

			case HEADER_GC_LOVER_INFO:
				ret = RecvLoverInfoPacket();
				break;

			case HEADER_GC_LOVE_POINT_UPDATE:
				ret = RecvLovePointUpdatePacket();
				break;

			case HEADER_GC_DIG_MOTION:
				ret = RecvDigMotionPacket();
				break;

			case HEADER_GC_HANDSHAKE:
				RecvHandshakePacket();
				return;
				break;

			case HEADER_GC_HANDSHAKE_OK:
				RecvHandshakeOKPacket();
				return;
				break;
#ifdef _IMPROVED_PACKET_ENCRYPTION_
			case HEADER_GC_KEY_AGREEMENT:
				RecvKeyAgreementPacket();
				return;
				break;

			case HEADER_GC_KEY_AGREEMENT_COMPLETED:
				RecvKeyAgreementCompletedPacket();
				return;
				break;
#endif
			case HEADER_GC_SPECIFIC_EFFECT:
				ret = RecvSpecificEffect();
				break;
			case HEADER_GC_DRAGON_SOUL_REFINE:
				ret = RecvDragonSoulRefine();
				break;
#if defined(ENABLE_GAYA_RENEWAL)
			case HEADER_GC_GEM_SHOP_OPEN:
				ret = RecvGemShopOpen();
				break;
			case HEADER_GC_GEM_SHOP_REFRESH:
				ret = RecvGemShopRefresh();
				break;
#endif
#ifdef ENABLE_SWITCHBOT_WORLDARD
			case HEADER_GC_SWITCHBOT:
				ret = RecvSwitchBot();
				break;
#endif

#if defined(USE_BATTLEPASS)
			case HEADER_GC_EXT_BATTLE_PASS_OPEN:
			{
				ret = RecvExtBattlePassOpenPacket();
				break;
			}
			case HEADER_GC_EXT_BATTLE_PASS_GENERAL_INFO:
			{
				ret = RecvExtBattlePassGeneralInfoPacket();
				break;
			}
			case HEADER_GC_EXT_BATTLE_PASS_MISSION_INFO:
			{
				ret = RecvExtBattlePassMissionInfoPacket();
				break;
			}
			case HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE:
			{
				ret = RecvExtBattlePassMissionUpdatePacket();
				break;
			}
			case HEADER_GC_EXT_BATTLE_PASS_SEND_RANKING:
			{
				ret = RecvExtBattlePassRankingPacket();
				break;
			}
#endif

#ifdef INGAME_WIKI
			case InGameWiki::HEADER_GC_WIKI:
				ret = RecvWikiPacket();
				break;
#endif

#ifdef ENABLE_PREMIUM_PLAYERS
			case HEADER_GC_PREMIUM_PLAYERS:
				ret = RecvPremiumPlayersPacket();
				break;
#endif
#ifdef ENABLE_RANKING
			case HEADER_GC_RANKING_SEND:
				ret = RecvRankingTable();
				break;
#endif
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
			case HEADER_GC_CUBE_RENEWAL:
				ret = RecvCubeRenewalPacket();
				break;	
#endif
#ifdef ENABLE_ACCE_SYSTEM
			case HEADER_GC_ACCE:
				ret = RecvAccePacket();
				break;
#endif
#ifdef ENABLE_ATLAS_BOSS
			case HEADER_GC_BOSS_POSITION:
				ret = RecvBossList();
				break;
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
			case HEADER_GC_NEW_OFFLINESHOP:
				ret = RecvOfflineshopPacket();
				break;
#endif
#if defined(ENABLE_TEXTS_RENEWAL)
			case HEADER_GC_CHAT_NEW:
				ret = RecvChatPacketNew();
				break;
#endif
#ifdef ENABLE_NEW_FISHING_SYSTEM
			case HEADER_GC_FISHING_NEW:
				ret = RecvFishingNew();
				break;
#endif
#if defined(ENABLE_EVENT_MANAGER)
			case HEADER_GC_EVENT_MANAGER:
				ret = RecvEventManager();
				break;
#endif
#if defined(ENABLE_INGAME_ITEMSHOP)
			case HEADER_GC_ITEMSHOP:
				ret = RecvItemShop();
				break;
#endif
#ifdef USE_CAPTCHA_SYSTEM
            case HEADER_GC_CAPTCHA:
                ret = RecvCaptcha();
                break;
#endif
#if defined(ENABLE_NEW_FISH_EVENT)
			case HEADER_GC_FISH_EVENT_INFO:
				ret = RecvFishEventInfo();
				break;
#endif
#if defined(ENABLE_CHANGELOOK)
			case HEADER_GC_CHANGE_LOOK:
				ret = RecvChangeLookPacket();
				break;
#endif
			case HEADER_GC_BOSS_POSITION:
				ret = RecvBossPositionPacket();
				break;

			case HEADER_GC_WHISPER_INFO:
				ret = RecvWhisperInfo();
				break;

			default:
				ret = RecvDefaultPacket(header);
				break;
		}
#ifdef __PERFORMANCE_CHECK__
		DWORD timeEndPacket=timeGetTime();

		{
			PERF_PacketInfo& rkPacketInfo=kMap_kPacketInfo[header];
			rkPacketInfo.dwCount++;
			rkPacketInfo.dwTime+=timeEndPacket-timeBeginPacket;
		}

		{
			PERF_PacketInfo& rkPacketInfo=gs_kPacketTimeAnalyzer.m_kMap_kPacketInfo[header];
			rkPacketInfo.dwCount++;
			rkPacketInfo.dwTime+=timeEndPacket-timeBeginPacket;
		}
#endif
	}

#ifdef __PERFORMANCE_CHECK__
	DWORD timeEndDispatch=timeGetTime();

	if (timeEndDispatch-timeBeginDispatch>2)
	{
		static FILE* fp=fopen("perf_dispatch_packet.txt", "w");

		fprintf(fp, "delay %d\n", timeEndDispatch-timeBeginDispatch);
		for (std::map<DWORD, PERF_PacketInfo>::iterator i=kMap_kPacketInfo.begin(); i!=kMap_kPacketInfo.end(); ++i)
		{
			if (i->second.dwTime>0)
				fprintf(fp, "header %d: count %d, time %d\n", i->first, i->second.dwCount, i->second.dwTime);
		}
		fputs("=====================================================\n", fp);
		fflush(fp);
	}
#endif

	if (!ret)
		RecvErrorPacket(header);

	static DWORD s_nextRefreshTime = ELTimer_GetMSec();

	DWORD curTime = ELTimer_GetMSec();
	if (s_nextRefreshTime > curTime)
		return;



	if (m_isRefreshCharacterWnd)
	{
		m_isRefreshCharacterWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshCharacter", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshEquipmentWnd)
	{
		m_isRefreshEquipmentWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshEquipment", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshInventoryWnd)
	{
		m_isRefreshInventoryWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshInventory", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshExchangeWnd)
	{
		m_isRefreshExchangeWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshExchange", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshSkillWnd)
	{
		m_isRefreshSkillWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSkill", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshSafeboxWnd)
	{
		m_isRefreshSafeboxWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSafebox", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshMallWnd)
	{
		m_isRefreshMallWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshMall", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshStatus)
	{
		m_isRefreshStatus=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshStatus", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshMessengerWnd)
	{
		m_isRefreshMessengerWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshMessenger", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndInfoPage)
	{
		m_isRefreshGuildWndInfoPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildInfoPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndBoardPage)
	{
		m_isRefreshGuildWndBoardPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildBoardPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndMemberPage)
	{
		m_isRefreshGuildWndMemberPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildMemberPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndMemberPageGradeComboBox)
	{
		m_isRefreshGuildWndMemberPageGradeComboBox=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildMemberPageGradeComboBox", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndSkillPage)
	{
		m_isRefreshGuildWndSkillPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildSkillPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndGradePage)
	{
		m_isRefreshGuildWndGradePage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGradePage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}
}

void CPythonNetworkStream::__InitializeGamePhase()
{
	__ServerTimeSync_Initialize();

	m_isRefreshStatus=false;
	m_isRefreshCharacterWnd=false;
	m_isRefreshEquipmentWnd=false;
	m_isRefreshInventoryWnd=false;
	m_isRefreshExchangeWnd=false;
	m_isRefreshSkillWnd=false;
	m_isRefreshSafeboxWnd=false;
	m_isRefreshMallWnd=false;
	m_isRefreshMessengerWnd=false;
	m_isRefreshGuildWndInfoPage=false;
	m_isRefreshGuildWndBoardPage=false;
	m_isRefreshGuildWndMemberPage=false;
	m_isRefreshGuildWndMemberPageGradeComboBox=false;
	m_isRefreshGuildWndSkillPage=false;
	m_isRefreshGuildWndGradePage=false;

	m_EmoticonStringVector.clear();

	m_pInstTarget = NULL;
}

void CPythonNetworkStream::Warp(LONG lGlobalX, LONG lGlobalY)
{
	CPythonBackground& rkBgMgr=CPythonBackground::Instance();
	rkBgMgr.SetShadowLevel(CPythonBackground::SHADOW_NONE);
	rkBgMgr.Destroy();
	rkBgMgr.Create();
	rkBgMgr.Warp(lGlobalX, lGlobalY);
	//rkBgMgr.SetShadowLevel(CPythonBackground::SHADOW_ALL);
	rkBgMgr.RefreshShadowLevel();

	// NOTE : Warp 했을때 CenterPosition의 Height가 0이기 때문에 카메라가 땅바닥에 박혀있게 됨
	//        움직일때마다 Height가 갱신 되기 때문이므로 맵을 이동하면 Position을 강제로 한번
	//        셋팅해준다 - [levites]
	LONG lLocalX = lGlobalX;
	LONG lLocalY = lGlobalY;
	__GlobalPositionToLocalPosition(lLocalX, lLocalY);
	float fHeight = CPythonBackground::Instance().GetHeight(float(lLocalX), float(lLocalY));

	IAbstractApplication& rkApp=IAbstractApplication::GetSingleton();
	rkApp.SetCenterPosition(float(lLocalX), float(lLocalY), fHeight);
}

void CPythonNetworkStream::__LeaveGamePhase()
{
	CInstanceBase::ClearPVPKeySystem();

	__ClearNetworkActorManager();

	m_bComboSkillFlag = FALSE;

	IAbstractCharacterManager& rkChrMgr=IAbstractCharacterManager::GetSingleton();
	rkChrMgr.Destroy();

	CPythonItem& rkItemMgr=CPythonItem::Instance();
	rkItemMgr.Destroy();
#ifdef __ENABLE_NEW_OFFLINESHOP__
#ifdef ENABLE_NEW_SHOP_IN_CITIES
	CPythonOfflineshop::instance().DeleteEntities();
#endif
#endif
}

void CPythonNetworkStream::SetGamePhase()
{
	if ("Game"!=m_strPhase)
		m_phaseLeaveFunc.Run();

	Tracen("");
	Tracen("## Network - Game Phase ##");
	Tracen("");

	m_strPhase = "Game";

	m_dwChangingPhaseTime = ELTimer_GetMSec();
	m_phaseProcessFunc.Set(this, &CPythonNetworkStream::GamePhase);
	m_phaseLeaveFunc.Set(this, &CPythonNetworkStream::__LeaveGamePhase);

	// Main Character 등록O

	IAbstractPlayer & rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.SetMainCharacterIndex(GetMainActorVID());

#if defined(ENABLE_TRACK_WINDOW)
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "TrackWindowUpdate", Py_BuildValue("()"));
#endif

	__RefreshStatus();
}

bool CPythonNetworkStream::RecvObserverAddPacket()
{
	TPacketGCObserverAdd kObserverAddPacket;
	if (!Recv(sizeof(kObserverAddPacket), &kObserverAddPacket))
		return false;

	CPythonMiniMap::Instance().AddObserver(
		kObserverAddPacket.vid,
		kObserverAddPacket.x*100.0f,
		kObserverAddPacket.y*100.0f);

	return true;
}

bool CPythonNetworkStream::RecvObserverRemovePacket()
{
	TPacketGCObserverAdd kObserverRemovePacket;
	if (!Recv(sizeof(kObserverRemovePacket), &kObserverRemovePacket))
		return false;

	CPythonMiniMap::Instance().RemoveObserver(
		kObserverRemovePacket.vid
	);

	return true;
}

bool CPythonNetworkStream::RecvObserverMovePacket()
{
	TPacketGCObserverMove kObserverMovePacket;
	if (!Recv(sizeof(kObserverMovePacket), &kObserverMovePacket))
		return false;

	CPythonMiniMap::Instance().MoveObserver(
		kObserverMovePacket.vid,
		kObserverMovePacket.x*100.0f,
		kObserverMovePacket.y*100.0f);

	return true;
}


bool CPythonNetworkStream::RecvWarpPacket()
{
	TPacketGCWarp kWarpPacket;

	if (!Recv(sizeof(kWarpPacket), &kWarpPacket))
		return false;

	__DirectEnterMode_Set(m_dwSelectedCharacterIndex);

	CNetworkStream::Connect((DWORD)kWarpPacket.lAddr, kWarpPacket.wPort);

	return true;
}

bool CPythonNetworkStream::RecvDuelStartPacket()
{
	TPacketGCDuelStart kDuelStartPacket;
	if (!Recv(sizeof(kDuelStartPacket), &kDuelStartPacket))
		return false;

	DWORD count = (kDuelStartPacket.wSize - sizeof(kDuelStartPacket))/sizeof(DWORD);

	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();

	CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
	{
		TraceError("CPythonNetworkStream::RecvDuelStartPacket - MainCharacter is NULL");
		return false;
	}
	DWORD dwVIDSrc = pkInstMain->GetVirtualID();
	DWORD dwVIDDest;

	for ( DWORD i = 0; i < count; i++)
	{
		Recv(sizeof(dwVIDDest),&dwVIDDest);
		CInstanceBase::InsertDUELKey(dwVIDSrc,dwVIDDest);
	}

	if(count == 0)
		pkInstMain->SetDuelMode(CInstanceBase::DUEL_CANNOTATTACK);
	else
		pkInstMain->SetDuelMode(CInstanceBase::DUEL_START);

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));

	rkChrMgr.RefreshAllPCTextTail();

	return true;
}

bool CPythonNetworkStream::RecvPVPPacket()
{
	TPacketGCPVP kPVPPacket;
	if (!Recv(sizeof(kPVPPacket), &kPVPPacket))
		return false;

	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();
	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();

	switch (kPVPPacket.bMode)
	{
		case PVP_MODE_AGREE:
			rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);

			// 상대가 나(Dst)에게 동의를 구했을때
			if (rkPlayer.IsMainCharacterIndex(kPVPPacket.dwVIDDst))
				rkPlayer.RememberChallengeInstance(kPVPPacket.dwVIDSrc);

			// 상대에게 동의를 구한 동안에는 대결 불능
			if (rkPlayer.IsMainCharacterIndex(kPVPPacket.dwVIDSrc))
				rkPlayer.RememberCantFightInstance(kPVPPacket.dwVIDDst);
			break;
		case PVP_MODE_REVENGE:
		{
			rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);

			DWORD dwKiller = kPVPPacket.dwVIDSrc;
			DWORD dwVictim = kPVPPacket.dwVIDDst;

			// 내(victim)가 상대에게 복수할 수 있을때
			if (rkPlayer.IsMainCharacterIndex(dwVictim))
				rkPlayer.RememberRevengeInstance(dwKiller);

			// 상대(victim)가 나에게 복수하는 동안에는 대결 불능
			if (rkPlayer.IsMainCharacterIndex(dwKiller))
				rkPlayer.RememberCantFightInstance(dwVictim);
			break;
		}

		case PVP_MODE_FIGHT:
			rkChrMgr.InsertPVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDSrc);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDDst);
			break;
		case PVP_MODE_NONE:
			rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDSrc);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDDst);
			break;
	}

	// NOTE : PVP 토글시 TargetBoard 를 업데이트 합니다.
	__RefreshTargetBoardByVID(kPVPPacket.dwVIDSrc);
	__RefreshTargetBoardByVID(kPVPPacket.dwVIDDst);

	return true;
}

// DELETEME
/*
void CPythonNetworkStream::__SendWarpPacket()
{
	TPacketCGWarp kWarpPacket;
	kWarpPacket.bHeader=HEADER_GC_WARP;
	if (!Send(sizeof(kWarpPacket), &kWarpPacket))
	{
		return;
	}
}
*/

bool CPythonNetworkStream::SendMessengerAddByVIDPacket(DWORD vid)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_ADD_BY_VID;
	if (!Send(sizeof(packet), &packet))
		return false;
	if (!Send(sizeof(vid), &vid))
		return false;
	return SendSequence();
}

#ifdef NEW_PET_SYSTEM
bool CPythonNetworkStream::PetSetNamePacket(const char * petname)
{
	TPacketCGRequestPetName PetSetName;
	PetSetName.byHeader = HEADER_CG_PetSetName;
	strncpy(PetSetName.petname, petname, 12);
	PetSetName.petname[12] = '\0';

	if (!Send(sizeof(PetSetName), &PetSetName))
		return false;

	return SendSequence();
}
#endif

bool CPythonNetworkStream::SendMessengerAddByNamePacket(const char * c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_ADD_BY_NAME;
	if (!Send(sizeof(packet), &packet))
		return false;
	char szName[CHARACTER_NAME_MAX_LEN];
	strncpy(szName, c_szName, CHARACTER_NAME_MAX_LEN-1);
	szName[CHARACTER_NAME_MAX_LEN-1] = '\0'; // #720: 메신저 이름 관련 버퍼 오버플로우 버그 수정

	if (!Send(sizeof(szName), &szName))
		return false;
	Tracef(" SendMessengerAddByNamePacket : %s\n", c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerRemovePacket(const char * c_szKey, const char * c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_REMOVE;
	if (!Send(sizeof(packet), &packet))
		return false;
	char szKey[CHARACTER_NAME_MAX_LEN];
	strncpy(szKey, c_szKey, CHARACTER_NAME_MAX_LEN-1);
	if (!Send(sizeof(szKey), &szKey))
		return false;
	__RefreshTargetBoardByName(c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendCharacterStatePacket(const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg)
{
	if (!__CanActMainInstance())
		return true;

	if (fDstRot < 0.0f)
		fDstRot = 360 + fDstRot;
	else if (fDstRot > 360.0f)
		fDstRot = fmodf(fDstRot, 360.0f);

	// TODO: 나중에 패킷이름을 바꾸자
	TPacketCGMove kStatePacket;
	kStatePacket.bHeader = HEADER_CG_CHARACTER_MOVE;
	kStatePacket.bFunc = eFunc;
	kStatePacket.bArg = uArg;
	kStatePacket.bRot = fDstRot/5.0f;
	kStatePacket.lX = long(c_rkPPosDst.x);
	kStatePacket.lY = long(c_rkPPosDst.y);
	kStatePacket.dwTime = ELTimer_GetServerMSec();

	assert(kStatePacket.lX >= 0 && kStatePacket.lX < 204800);

	__LocalPositionToGlobalPosition(kStatePacket.lX, kStatePacket.lY);
#if defined(ENABLE_NEW_ANTICHEAT_RULES)
NANOBEGIN

	uint32_t a = 55184705, b = 85277383, c = kStatePacket.lX, d = kStatePacket.lY;
	uint32_t verify_x = kStatePacket.lX ^ a;
	verify_x += b;
	verify_x >>= 3;
	verify_x |= 0xAA11;
	verify_x ^= b;

	uint32_t verify_y = kStatePacket.lY ^ a;
	verify_y += b;
	verify_y >>= 3;
	verify_y |= 0xAA11;
	verify_y ^= b;

NANOEND

	kStatePacket.uX = verify_x;
	kStatePacket.uY = verify_y;
#endif

	if (!Send(sizeof(kStatePacket), &kStatePacket))
	{
		Tracenf("CPythonNetworkStream::SendCharacterStatePacket(dwCmdTime=%u, fDstPos=(%f, %f), fDstRot=%f, eFunc=%d uArg=%d) - PACKET SEND ERROR",
			kStatePacket.dwTime,
			float(kStatePacket.lX),
			float(kStatePacket.lY),
			fDstRot,
			kStatePacket.bFunc,
			kStatePacket.bArg);
		return false;
	}

	return SendSequence();
}

// NOTE : SlotIndex는 임시
bool CPythonNetworkStream::SendUseSkillPacket(DWORD dwSkillIndex, DWORD dwTargetVID)
{
	if (m_strPhase != "Game") // FIXME
		return true;

	TPacketCGUseSkill UseSkillPacket;
	UseSkillPacket.bHeader = HEADER_CG_USE_SKILL;
	UseSkillPacket.dwVnum = dwSkillIndex;
	UseSkillPacket.dwTargetVID = dwTargetVID;
	if (!Send(sizeof(TPacketCGUseSkill), &UseSkillPacket))
	{
		Tracen("CPythonNetworkStream::SendUseSkillPacket - SEND PACKET ERROR");
		return false;
	}

	return SendSequence();
}

std::vector<std::string> emotionsList = {
	"/clap",
	"/dance1",
	"/dance2",
	"/dance3",
	"/dance4",
	"/dance5",
	"/dance6",
	"/congratulation",
	"/forgive",
	"/angry",
	"/attractive",
	"/sad",
	"/shy",
	"/cheerup",
	"/banter",
	"/joy",
	"/cheer1",
	"/cheer2",
	"/kiss",
	"/french_kiss",
	"/slap",
	"/selfie",
	"/dance7",
	"/doze",
	"/exercise",
	"/pushup",
};

bool CPythonNetworkStream::SendChatPacket(const char * c_szChat, BYTE byType)
{
	if (strlen(c_szChat) == 0)
		return true;

	if (strlen(c_szChat) >= 512)
		return true;

	if (m_strPhase != "Game") // FIXME
		return true;
	if (c_szChat[0] == '/')
	{
		if (__IsPlayerAttacking()) {
			std::string commandName = c_szChat;
			transform(commandName.begin(), commandName.end(), commandName.begin(),
				[](unsigned char c) { return std::tolower(c); });
	
			for (auto const& emotion : emotionsList) {
				std::string cnvEmotion = emotion;
				transform(cnvEmotion.begin(), cnvEmotion.end(), cnvEmotion.begin(),
					[](unsigned char c) { return std::tolower(c); });
	
				if (!commandName.compare(0, cnvEmotion.length(), cnvEmotion.c_str())) {
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_EmotionsFailed", Py_BuildValue("(i)", 0));
					return true;
				}
			}
		}

		if (1 == strlen(c_szChat))
		{
			if (!m_strLastCommand.empty())
				c_szChat = m_strLastCommand.c_str();
		}
		else
		{
			m_strLastCommand = c_szChat;
		}
	}

	if (ClientCommand(c_szChat))
		return true;

	int iTextLen = strlen(c_szChat) + 1;
	TPacketCGChat ChatPacket;
	ChatPacket.header = HEADER_CG_CHAT;
	ChatPacket.length = sizeof(ChatPacket) + iTextLen;
	ChatPacket.type = byType;

	if (!Send(sizeof(ChatPacket), &ChatPacket))
		return false;

	if (!Send(iTextLen, c_szChat))
		return false;

	return SendSequence();
}

//////////////////////////////////////////////////////////////////////////
// Emoticon
void CPythonNetworkStream::RegisterEmoticonString(const char * pcEmoticonString)
{
	if (m_EmoticonStringVector.size() >= CInstanceBase::EMOTICON_NUM)
	{
		TraceError("Can't register emoticon string... vector is full (size:%d)", m_EmoticonStringVector.size() );
		return;
	}
	m_EmoticonStringVector.push_back(pcEmoticonString);
}

bool CPythonNetworkStream::ParseEmoticon(const char* pChatMsg, uint32_t* pdwEmoticon)
{
	for (DWORD dwEmoticonIndex = 0; dwEmoticonIndex < m_EmoticonStringVector.size() ; ++dwEmoticonIndex)
	{
		if (strlen(pChatMsg) > m_EmoticonStringVector[dwEmoticonIndex].size())
			continue;

		const char * pcFind = strstr(pChatMsg, m_EmoticonStringVector[dwEmoticonIndex].c_str());

		if (pcFind != pChatMsg)
			continue;

		*pdwEmoticon = dwEmoticonIndex;

		return true;
	}

	return false;
}
// Emoticon
//////////////////////////////////////////////////////////////////////////

void CPythonNetworkStream::__ConvertEmpireText(DWORD dwEmpireID, char* szText)
{
	if (dwEmpireID<1 || dwEmpireID>3)
		return;

	UINT uHanPos;

	STextConvertTable& rkTextConvTable=m_aTextConvTable[dwEmpireID-1];

	BYTE* pbText=(BYTE*)szText;
	while (*pbText)
	{
		if (*pbText & 0x80)
		{
			if (pbText[0]>=0xb0 && pbText[0]<=0xc8 && pbText[1]>=0xa1 && pbText[1]<=0xfe)
			{
				uHanPos=(pbText[0]-0xb0)*(0xfe-0xa1+1)+(pbText[1]-0xa1);
				pbText[0]=rkTextConvTable.aacHan[uHanPos][0];
				pbText[1]=rkTextConvTable.aacHan[uHanPos][1];
			}
			pbText+=2;
		}
		else
		{
			if (*pbText>='a' && *pbText<='z')
			{
				*pbText=rkTextConvTable.acLower[*pbText-'a'];
			}
			else if (*pbText>='A' && *pbText<='Z')
			{
				*pbText=rkTextConvTable.acUpper[*pbText-'A'];
			}
			pbText++;
		}
	}
}

bool CPythonNetworkStream::RecvChatPacket()
{
	TPacketGCChat p;
	if (!Recv(sizeof(p), &p))
	{
		TraceError("Cannot recv TPacketGCChat");
		return false;
	}

	uint32_t size = p.size - sizeof(p);
	char szBuf[1024 + 1];

	if (size > 0)
	{
		if (!Recv(size, szBuf))
		{
			TraceError("Cannot recv TPacketGCChat size %d", size);
			return false;
		}
	}
	else
	{
		strcpy(szBuf, "");
	}

	szBuf[size] = '\0';

	if (p.type >= CHAT_TYPE_MAX_NUM)
	{
		return true;
	}

	if (LocaleService_IsEUROPE() && GetDefaultCodePage() == 1256)
	{
		char* p = strchr(szBuf, ':');
		if (p && p[1] == ' ')
		{
			p[1] = 0x08;
		}
	}

	if (p.type == CHAT_TYPE_COMMAND)
	{
		ServerCommand(szBuf);
		return true;
	}

	SRecvChat sChat;
	sChat.type = p.type;
	sChat.idx = 0;
	sChat.vid = p.dwVID;
	strncpy(sChat.szBuf, szBuf, sizeof(sChat.szBuf));

	m_ChatQueue.push(sChat);

	if (m_ChatQueue.size() > 20) {
		m_ChatQueue.pop();
	}

	return true;
}

bool CPythonNetworkStream::RecvWhisperPacket()
{
	TPacketGCWhisper whisperPacket;
    char buf[512 + 1];

	if (!Recv(sizeof(whisperPacket), &whisperPacket))
		return false;

	assert(whisperPacket.wSize - sizeof(whisperPacket) < 512);

	if (!Recv(whisperPacket.wSize - sizeof(whisperPacket), &buf))
		return false;

	buf[whisperPacket.wSize - sizeof(whisperPacket)] = '\0';
#ifndef ENABLE_MULTI_LANGUAGE
	static char line[256];
#endif
	if (CPythonChat::WHISPER_TYPE_CHAT == whisperPacket.bType || CPythonChat::WHISPER_TYPE_GM == whisperPacket.bType
#if defined(BL_OFFLINE_MESSAGE)
		|| CPythonChat::WHISPER_TYPE_OFFLINE == whisperPacket.bType
#endif
	)
	{
#ifdef ENABLE_MULTI_LANGUAGE
		std::string chatStr = whisperPacket.szNameFrom;
		chatStr += " : ";
		chatStr += buf;
		std::string replace = "";
		int firstDelPos = chatStr.find("[{");
		int secondDelPos = chatStr.find("}]");
		int vnum = 0;
		while (firstDelPos > 0 && secondDelPos > 0) {
			replace = chatStr.substr(firstDelPos + 1, secondDelPos-firstDelPos);
			vnum = stoi(chatStr.substr(firstDelPos+2, secondDelPos-firstDelPos-2));
			CItemManager::Instance().SelectItemData(vnum);
			CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();

			std::string vnumStr = std::to_string(vnum);
			chatStr.replace(firstDelPos + 1, secondDelPos-firstDelPos, vnumStr.c_str());
			if (pItemData) {
				vnumStr = pItemData->GetName();
			}

			firstDelPos = chatStr.find("[-");
			secondDelPos = chatStr.find("-]");
			if (firstDelPos > 0 && secondDelPos > 0) {
				replace = chatStr.substr(firstDelPos + 1, secondDelPos-firstDelPos);
				chatStr.replace(firstDelPos + 1, secondDelPos-firstDelPos, vnumStr.c_str());
			}

			firstDelPos = chatStr.find("[{");
			secondDelPos = chatStr.find("}]");
		}
#else
		_snprintf(line, sizeof(line), "%s : %s", whisperPacket.szNameFrom, buf);
#endif
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisper", Py_BuildValue("(iss)", (int) whisperPacket.bType, whisperPacket.szNameFrom, 
#ifdef ENABLE_MULTI_LANGUAGE
		chatStr.c_str()
#else
		line
#endif
		));
	}
	else if (CPythonChat::WHISPER_TYPE_SYSTEM == whisperPacket.bType || CPythonChat::WHISPER_TYPE_ERROR == whisperPacket.bType)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisperSystemMessage", Py_BuildValue("(iss)", (int) whisperPacket.bType, whisperPacket.szNameFrom, buf));
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisperError", Py_BuildValue("(iss)", (int) whisperPacket.bType, whisperPacket.szNameFrom, buf));
	}

	return true;
}

bool CPythonNetworkStream::SendWhisperPacket(const char * name, const char * c_szChat)
{
	if (strlen(c_szChat) >= 255)
		return true;

	int iTextLen = strlen(c_szChat) + 1;
	TPacketCGWhisper WhisperPacket;
	WhisperPacket.bHeader = HEADER_CG_WHISPER;
	WhisperPacket.wSize = sizeof(WhisperPacket) + iTextLen;

	strncpy(WhisperPacket.szNameTo, name, sizeof(WhisperPacket.szNameTo) - 1);

	if (!Send(sizeof(WhisperPacket), &WhisperPacket))
		return false;

	if (!Send(iTextLen, c_szChat))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendMobileMessagePacket(const char * name, const char * c_szChat)
{
	//int iTextLen = strlen(c_szChat) + 1;
	//TPacketCGSMS SMSPacket;
	//SMSPacket.bHeader = HEADER_CG_SMS;
	//SMSPacket.wSize = sizeof(SMSPacket) + iTextLen;
	//
	//strncpy(SMSPacket.szNameTo, name, sizeof(SMSPacket.szNameTo) - 1);
	//
	//if (!Send(sizeof(SMSPacket), &SMSPacket))
	//	return false;
	//
	//if (!Send(iTextLen, c_szChat))
	//	return false;
	//
	//return SendSequence();
	return true;
}

bool CPythonNetworkStream::RecvPointChange()
{
	TPacketGCPointChange PointChange;

	if (!Recv(sizeof(TPacketGCPointChange), &PointChange))
	{
		Tracen("Recv Point Change Packet Error");
		return false;
	}

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	rkChrMgr.ShowPointEffect(PointChange.Type, PointChange.dwVID);

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();

	// 자신의 Point가 변경되었을 경우..
	if (pInstance && PointChange.dwVID == pInstance->GetVirtualID())
	{
		CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
		rkPlayer.SetStatus(PointChange.Type, PointChange.value);

		switch (PointChange.Type)
		{
			case POINT_STAT_RESET_COUNT:
				__RefreshStatus();
				break;
			case POINT_PLAYTIME:
				m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].dwPlayMinutes = PointChange.value;
				break;
			case POINT_LEVEL:
				m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byLevel = PointChange.value;
				__RefreshStatus();
				__RefreshSkillWindow();
				break;
			case POINT_ST:
				m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byST = PointChange.value;
				__RefreshStatus();
				__RefreshSkillWindow();
				break;
			case POINT_DX:
				m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byDX = PointChange.value;
				__RefreshStatus();
				__RefreshSkillWindow();
				break;
			case POINT_HT:
				m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byHT = PointChange.value;
				__RefreshStatus();
				__RefreshSkillWindow();
				break;
			case POINT_IQ:
				m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byIQ = PointChange.value;
				__RefreshStatus();
				__RefreshSkillWindow();
				break;
			case POINT_SKILL:
			case POINT_SUB_SKILL:
			case POINT_HORSE_SKILL:
				__RefreshSkillWindow();
				break;
			case POINT_ENERGY:
				if (PointChange.value == 0)
				{
					rkPlayer.SetStatus(POINT_ENERGY_END_TIME, 0);
				}
				__RefreshStatus();
				break;
			default:
				__RefreshStatus();
				break;
		}
	}
#ifdef ENABLE_TEXT_LEVEL_REFRESH
	else
	{
		// the /advance command will provide no global refresh! it sends the pointchange only to the specific player and not all
		pInstance = CPythonCharacterManager::Instance().GetInstancePtr(PointChange.dwVID);
		if (pInstance && PointChange.Type == POINT_LEVEL && pInstance->GetLevel() != PointChange.value)
		{
			pInstance->SetLevel(PointChange.value);
			pInstance->UpdateTextTailLevel(PointChange.value);
		}
	}
#endif

	return true;
}

bool CPythonNetworkStream::RecvStunPacket()
{
	TPacketGCStun StunPacket;

	if (!Recv(sizeof(StunPacket), &StunPacket))
	{
		Tracen("CPythonNetworkStream::RecvStunPacket Error");
		return false;
	}

	//Tracef("RecvStunPacket %d\n", StunPacket.vid);

	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase * pkInstSel = rkChrMgr.GetInstancePtr(StunPacket.vid);

	if (pkInstSel)
	{
		if (CPythonCharacterManager::Instance().GetMainInstancePtr()==pkInstSel)
			pkInstSel->Die();
		else
			pkInstSel->Stun();
	}

	return true;
}

bool CPythonNetworkStream::RecvDeadPacket()
{
	TPacketGCDead DeadPacket;
	if (!Recv(sizeof(DeadPacket), &DeadPacket))
	{
		Tracen("CPythonNetworkStream::RecvDeadPacket Error");
		return false;
	}

	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase * pkChrInstSel = rkChrMgr.GetInstancePtr(DeadPacket.vid);
	if (pkChrInstSel)
	{
		CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
		if (pkInstMain==pkChrInstSel)
		{
			Tracenf("On MainActor");
			if (false == pkInstMain->GetDuelMode())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnGameOver", Py_BuildValue("()"));
			}
			CPythonPlayer::Instance().NotifyDeadMainCharacter();
		}

		pkChrInstSel->Die();
	}

	return true;
}

bool CPythonNetworkStream::SendCharacterPositionPacket(BYTE iPosition)
{
	TPacketCGPosition PositionPacket;

	PositionPacket.header = HEADER_CG_CHARACTER_POSITION;
	PositionPacket.position = iPosition;

	if (!Send(sizeof(TPacketCGPosition), &PositionPacket))
	{
		Tracen("Send Character Position Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendOnClickPacket(DWORD vid)
{
	if (m_strPhase != "Game") // FIXME
		return true;
	TPacketCGOnClick OnClickPacket;
	OnClickPacket.header	= HEADER_CG_ON_CLICK;
	OnClickPacket.vid		= vid;

	if (!Send(sizeof(OnClickPacket), &OnClickPacket))
	{
		Tracen("Send On_Click Packet Error");
		return false;
	}

	Tracef("SendOnClickPacket\n");
	return SendSequence();
}

bool CPythonNetworkStream::RecvCharacterPositionPacket()
{
	TPacketGCPosition PositionPacket;

	if (!Recv(sizeof(TPacketGCPosition), &PositionPacket))
		return false;

	CInstanceBase * pChrInstance = CPythonCharacterManager::Instance().GetInstancePtr(PositionPacket.vid);

	if (!pChrInstance)
		return true;

	//pChrInstance->UpdatePosition(PositionPacket.position);

	return true;
}

bool CPythonNetworkStream::RecvMotionPacket()
{
	TPacketGCMotion MotionPacket;

	if (!Recv(sizeof(TPacketGCMotion), &MotionPacket))
		return false;

	CInstanceBase * pMainInstance = CPythonCharacterManager::Instance().GetInstancePtr(MotionPacket.vid);
	CInstanceBase * pVictimInstance = NULL;

	if (0 != MotionPacket.victim_vid)
		pVictimInstance = CPythonCharacterManager::Instance().GetInstancePtr(MotionPacket.victim_vid);

	if (!pMainInstance)
		return false;

	return true;
}

bool CPythonNetworkStream::RecvShopPacket()
{
	std::vector<char> vecBuffer;
	vecBuffer.clear();

	TPacketGCShop  packet_shop;
	if (!Recv(sizeof(packet_shop), &packet_shop))
		return false;

	int iSize = packet_shop.size - sizeof(packet_shop);
	if (iSize > 0)
	{
		vecBuffer.resize(iSize);
		if (!Recv(iSize, &vecBuffer[0]))
			return false;
	}
	
	switch (packet_shop.subheader)
	{
		case SHOP_SUBHEADER_GC_START:
			{
				CPythonShop::Instance().Clear();

				DWORD dwVID = *(DWORD *)&vecBuffer[0];

				TPacketGCShopStart * pShopStartPacket = (TPacketGCShopStart *)&vecBuffer[4];
				for (BYTE iItemIndex = 0; iItemIndex < SHOP_HOST_ITEM_MAX_NUM; ++iItemIndex)
				{
					CPythonShop::Instance().SetItemData(iItemIndex, pShopStartPacket->items[iItemIndex]);
				}

				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartShop", Py_BuildValue("(i)", dwVID));
			}
			break;

		case SHOP_SUBHEADER_GC_START_EX:
			{
				CPythonShop::Instance().Clear();

				TPacketGCShopStartEx * pShopStartPacket = (TPacketGCShopStartEx *)&vecBuffer[0];
				size_t read_point = sizeof(TPacketGCShopStartEx);

				DWORD dwVID = pShopStartPacket->owner_vid;
				BYTE shop_tab_count = pShopStartPacket->shop_tab_count;

				CPythonShop::instance().SetTabCount(shop_tab_count);

				for (size_t i = 0; i < shop_tab_count; i++)
				{
					TPacketGCShopStartEx::TSubPacketShopTab* pPackTab = (TPacketGCShopStartEx::TSubPacketShopTab*)&vecBuffer[read_point];
					read_point += sizeof(TPacketGCShopStartEx::TSubPacketShopTab);

					CPythonShop::instance().SetTabCoinType((BYTE)i, pPackTab->coin_type);
					CPythonShop::instance().SetTabName((BYTE)i, pPackTab->name);

					struct packet_shop_item* item = &pPackTab->items[0];

					for (BYTE j = 0; j < SHOP_HOST_ITEM_MAX_NUM; j++)
					{
						TShopItemData* itemData = (item + j);
						CPythonShop::Instance().SetItemData((BYTE)i, j, *itemData);
					}
				}

				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartShop", Py_BuildValue("(i)", dwVID));
			}
			break;
		case SHOP_SUBHEADER_GC_END:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "EndShop", Py_BuildValue("()"));
			break;
		case SHOP_SUBHEADER_GC_UPDATE_ITEM:
			{
				TPacketGCShopUpdateItem * pShopUpdateItemPacket = (TPacketGCShopUpdateItem *)&vecBuffer[0];
				CPythonShop::Instance().SetItemData(pShopUpdateItemPacket->pos, pShopUpdateItemPacket->item);
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshShop", Py_BuildValue("()"));
			}
			break;
		case SHOP_SUBHEADER_GC_UPDATE_PRICE:
			{
#if defined(ENABLE_NEW_GOLD_LIMIT)
				PyObject *args = PyTuple_New(1);
				PyTuple_SetItem(args, 0, PyLong_FromLongLong(*(uint64_t *)&vecBuffer[0]));
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetShopSellingPrice", args);
#else
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetShopSellingPrice", Py_BuildValue("(i)", *(uint32_t *)&vecBuffer[0]));
#endif
			}
			break;
		case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY"));
			break;
#ifdef ENABLE_BUY_WITH_ITEM
		case SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_ITEM"));
			break;
#endif
		case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY_EX"));
			break;
		case SHOP_SUBHEADER_GC_SOLDOUT:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "SOLDOUT"));
			break;
		case SHOP_SUBHEADER_GC_INVENTORY_FULL:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVENTORY_FULL"));
			break;
		case SHOP_SUBHEADER_GC_INVALID_POS:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVALID_POS"));
			break;
#if defined(ENABLE_WORLDBOSS)
		case SHOP_SUBHEADER_GC_NOT_ENOUGH_WB_POINTS: {
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_WB_POINTS"));
			break;
		}
#endif
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
		case SHOP_SUBHEADER_GC_NOT_ENOUGH_SKULLS: {
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_SKULLS"));
			break;
		}
#endif
		default:
			TraceError("CPythonNetworkStream::RecvShopPacket: Unknown subheader\n");
			break;
	}

	return true;
}

bool CPythonNetworkStream::RecvExchangePacket()
{
	TPacketGCExchange exchange_packet;

	if (!Recv(sizeof(exchange_packet), &exchange_packet))
		return false;

	switch (exchange_packet.subheader)
	{
		case EXCHANGE_SUBHEADER_GC_START:
			CPythonExchange::Instance().Clear();
			CPythonExchange::Instance().Start();
			CPythonExchange::Instance().SetSelfName(CPythonPlayer::Instance().GetName());
#ifdef ENABLE_NEW_EXCHANGE_WINDOW
			CPythonExchange::Instance().SetSelfRace(CPythonPlayer::Instance().GetRace());
			CPythonExchange::Instance().SetSelfLevel(CPythonPlayer::Instance().GetStatus(POINT_LEVEL));
#endif

#ifdef ENABLE_LEVEL_IN_TRADE
			//CPythonExchange::Instance().SetSelfLevel(CPythonPlayer::Instance().GetLevel());
			CPythonExchange::Instance().SetSelfLevel(CPythonPlayer::Instance().GetStatus(POINT_LEVEL));
#endif

			{
				CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetInstancePtr(exchange_packet.arg1);

				if (pCharacterInstance)
				{
					CPythonExchange::Instance().SetTargetName(pCharacterInstance->GetNameString());
#ifdef ENABLE_NEW_EXCHANGE_WINDOW
					CPythonExchange::Instance().SetTargetRace(pCharacterInstance->GetRace());
					CPythonExchange::Instance().SetTargetLevel(pCharacterInstance->GetLevel());
#endif

#ifdef ENABLE_LEVEL_IN_TRADE
					CPythonExchange::Instance().SetTargetLevel(pCharacterInstance->GetLevel());
#endif
				}
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartExchange", Py_BuildValue("()"));
			break;

		case EXCHANGE_SUBHEADER_GC_ITEM_ADD:
			if (exchange_packet.is_me)
			{
				int iSlotIndex = exchange_packet.arg2.cell;
				CPythonExchange::Instance().SetItemToSelf(iSlotIndex, exchange_packet.arg1, (WORD) exchange_packet.arg3);
				for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
					CPythonExchange::Instance().SetItemMetinSocketToSelf(iSlotIndex, i, exchange_packet.alValues[i]);
				for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
					CPythonExchange::Instance().SetItemAttributeToSelf(iSlotIndex, j, exchange_packet.aAttr[j].bType, exchange_packet.aAttr[j].sValue);

#if defined(ENABLE_CHANGELOOK)
				CPythonExchange::Instance().SetItemTransmutation(iSlotIndex, exchange_packet.transmutation, true);
#endif
#ifdef ATTR_LOCK
				CPythonExchange::Instance().SetItemAttrLocked(iSlotIndex, exchange_packet.lockedattr, true);
#endif
#ifdef WJ_ENABLE_TRADABLE_ICON
				PyObject *args = PyTuple_New(3);
				PyTuple_SetItem(args, 0, PyInt_FromLong(exchange_packet.arg4.window_type));
				PyTuple_SetItem(args, 1, PyInt_FromLong(exchange_packet.arg4.cell));
				PyTuple_SetItem(args, 2, PyInt_FromLong(iSlotIndex));
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AddItemToExchange", args);
#endif
			}
			else
			{
				int iSlotIndex = exchange_packet.arg2.cell;
				CPythonExchange::Instance().SetItemToTarget(iSlotIndex, exchange_packet.arg1, (WORD) exchange_packet.arg3);

				for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
					CPythonExchange::Instance().SetItemMetinSocketToTarget(iSlotIndex, i, exchange_packet.alValues[i]);

				for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
					CPythonExchange::Instance().SetItemAttributeToTarget(iSlotIndex, j, exchange_packet.aAttr[j].bType, exchange_packet.aAttr[j].sValue);

#if defined(ENABLE_CHANGELOOK)
				CPythonExchange::Instance().SetItemTransmutation(iSlotIndex, exchange_packet.transmutation, false);
#endif
#ifdef ATTR_LOCK
				CPythonExchange::Instance().SetItemAttrLocked(iSlotIndex, exchange_packet.lockedattr, false);
#endif
			}

			__RefreshExchangeWindow();
			__RefreshInventoryWindow();
			break;

		case EXCHANGE_SUBHEADER_GC_ITEM_DEL:
			if (exchange_packet.is_me)
			{
				CPythonExchange::Instance().DelItemOfSelf((BYTE) exchange_packet.arg1);
			}
			else
			{
				CPythonExchange::Instance().DelItemOfTarget((BYTE) exchange_packet.arg1);
			}
			__RefreshExchangeWindow();
			__RefreshInventoryWindow();
			break;

		case EXCHANGE_SUBHEADER_GC_ELK_ADD:
			if (exchange_packet.is_me)
				CPythonExchange::Instance().SetElkToSelf(exchange_packet.arg1);
			else
				CPythonExchange::Instance().SetElkToTarget(exchange_packet.arg1);

			__RefreshExchangeWindow();
			break;

		case EXCHANGE_SUBHEADER_GC_ACCEPT:
			if (exchange_packet.is_me)
			{
				CPythonExchange::Instance().SetAcceptToSelf((BYTE) exchange_packet.arg1);
			}
			else
			{
				CPythonExchange::Instance().SetAcceptToTarget((BYTE) exchange_packet.arg1);
			}
			__RefreshExchangeWindow();
			break;

		case EXCHANGE_SUBHEADER_GC_END:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "EndExchange", Py_BuildValue("()"));
			__RefreshInventoryWindow();
			CPythonExchange::Instance().End();
			break;

		case EXCHANGE_SUBHEADER_GC_ALREADY:
			Tracef("trade_already");
			break;

		case EXCHANGE_SUBHEADER_GC_LESS_ELK:
			Tracef("trade_less_elk");
			break;
	};

	return true;
}

bool CPythonNetworkStream::RecvQuestInfoPacket()
{
//	TPacketGCQuestInfo QuestInfo;
//	if (!Peek(sizeof(TPacketGCQuestInfo), &QuestInfo)) {
//		TraceError("RecvQuestInfoPacket:: error 1");
//		return false;
//	}
//
//	if (!Peek(QuestInfo.size)) {
//		TraceError("RecvQuestInfoPacket:: error 2");
//		return false;
//	}
//
//	Recv(sizeof(TPacketGCQuestInfo));
//
//	const BYTE & c_rFlag = QuestInfo.flag;
//	enum
//	{
//		QUEST_PACKET_TYPE_NONE,
//		QUEST_PACKET_TYPE_BEGIN,
//		QUEST_PACKET_TYPE_UPDATE,
//		QUEST_PACKET_TYPE_END,
//	};
//
//	BYTE byQuestPacketType = QUEST_PACKET_TYPE_NONE;
//	if (0 != (c_rFlag & QUEST_SEND_IS_BEGIN)) {
//		BYTE isBegin = QuestInfo.isBegin;
//		if (isBegin)
//			byQuestPacketType = QUEST_PACKET_TYPE_BEGIN;
//		else
//			byQuestPacketType = QUEST_PACKET_TYPE_END;
//	}
//	else {
//		byQuestPacketType = QUEST_PACKET_TYPE_UPDATE;
//	}
//
//	char szTitle[30 + 1] = "";
//	char szClockName[16 + 1] = "";
//	int iClockValue = 0;
//	char szCounterName[16 + 1] = "";
//	int iCounterValue = 0;
//	char szIconFileName[24 + 1] = "";
//
//	if (0 != (c_rFlag & QUEST_SEND_TITLE)) {
//		strncpy(szTitle, QuestInfo.szTitle, sizeof(szTitle));
//		szTitle[30]='\0';
//	}
//
//	if (0 != (c_rFlag & QUEST_SEND_CLOCK_NAME)) {
//		strncpy(szClockName, QuestInfo.szClockName, sizeof(szClockName));
//		szClockName[16]='\0';
//	}
//
//	if (0 != (c_rFlag & QUEST_SEND_CLOCK_VALUE)) {
//		iClockValue = QuestInfo.iClockValue;
//	}
//
//	if (0 != (c_rFlag & QUEST_SEND_COUNTER_NAME)) {
//		strncpy(szCounterName, QuestInfo.szCounterName, sizeof(szCounterName));
//		szCounterName[16]='\0';
//	}
//
//	if (0 != (c_rFlag & QUEST_SEND_COUNTER_VALUE)) {
//		iCounterValue = QuestInfo.iCounterValue;
//	}
//
//	if (0 != (c_rFlag & QUEST_SEND_ICON_FILE)) {
//		strncpy(szIconFileName, QuestInfo.szIconFileName, sizeof(szIconFileName));
//		szIconFileName[24]='\0';
//	}
//	// Recv Data End
//
//	CPythonQuest& rkQuest=CPythonQuest::Instance();
//
//	// Process Start
//	if (QUEST_PACKET_TYPE_END == byQuestPacketType) {
//		rkQuest.DeleteQuestInstance(QuestInfo.index);
//	}
//	else if (QUEST_PACKET_TYPE_UPDATE == byQuestPacketType)
//	{
//		if (!rkQuest.IsQuest(QuestInfo.index))
//		{
//			rkQuest.MakeQuest(QuestInfo.index);
//		}
//
//		if (strlen(szTitle) > 0)
//			rkQuest.SetQuestTitle(QuestInfo.index, szTitle);
//		if (strlen(szClockName) > 0)
//			rkQuest.SetQuestClockName(QuestInfo.index, szClockName);
//		if (strlen(szCounterName) > 0)
//			rkQuest.SetQuestCounterName(QuestInfo.index, szCounterName);
//		if (strlen(szIconFileName) > 0)
//			rkQuest.SetQuestIconFileName(QuestInfo.index, szIconFileName);
//		if (c_rFlag & QUEST_SEND_CLOCK_VALUE)
//			rkQuest.SetQuestClockValue(QuestInfo.index, iClockValue);
//		if (c_rFlag & QUEST_SEND_COUNTER_VALUE)
//			rkQuest.SetQuestCounterValue(QuestInfo.index, iCounterValue);
//	}
//	else if (QUEST_PACKET_TYPE_BEGIN == byQuestPacketType)
//	{
//		CPythonQuest::SQuestInstance QuestInstance;
//		QuestInstance.dwIndex = QuestInfo.index;
//		QuestInstance.strTitle = szTitle;
//		QuestInstance.strClockName = szClockName;
//		QuestInstance.iClockValue = iClockValue;
//		QuestInstance.strCounterName = szCounterName;
//		QuestInstance.iCounterValue = iCounterValue;
//		QuestInstance.strIconFileName = szIconFileName;
//		CPythonQuest::Instance().RegisterQuestInstance(QuestInstance);
//	}
//	// Process Start End
//
//	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshQuest", Py_BuildValue("()"));
//	return true;
	TPacketGCQuestInfo QuestInfo;

	if (!Peek(sizeof(TPacketGCQuestInfo), &QuestInfo))
	{
		Tracen("Recv Quest Info Packet Error #1");
		return false;
	}

	if (!Peek(QuestInfo.size))
	{
		Tracen("Recv Quest Info Packet Error #2");
		return false;
	}

	Recv(sizeof(TPacketGCQuestInfo));

	const BYTE& c_rFlag = QuestInfo.flag;

	enum
	{
		QUEST_PACKET_TYPE_NONE,
		QUEST_PACKET_TYPE_BEGIN,
		QUEST_PACKET_TYPE_UPDATE,
		QUEST_PACKET_TYPE_END,
	};

	BYTE byQuestPacketType = QUEST_PACKET_TYPE_NONE;

	if (0 != (c_rFlag & QUEST_SEND_IS_BEGIN))
	{
		BYTE isBegin;
		if (!Recv(sizeof(isBegin), &isBegin))
			return false;

		if (isBegin)
			byQuestPacketType = QUEST_PACKET_TYPE_BEGIN;
		else
			byQuestPacketType = QUEST_PACKET_TYPE_END;
	}
	else
	{
		byQuestPacketType = QUEST_PACKET_TYPE_UPDATE;
	}

	// Recv Data Start
	char szTitle[30 + 1] = "";
	char szClockName[16 + 1] = "";
	int iClockValue = 0;
	char szCounterName[16 + 1] = "";
	int iCounterValue = 0;
	char szIconFileName[24 + 1] = "";

	if (0 != (c_rFlag & QUEST_SEND_TITLE))
	{
		if (!Recv(sizeof(szTitle), &szTitle))
			return false;

		szTitle[30] = '\0';
	}
	if (0 != (c_rFlag & QUEST_SEND_CLOCK_NAME))
	{
		if (!Recv(sizeof(szClockName), &szClockName))
			return false;

		szClockName[16] = '\0';
	}
	if (0 != (c_rFlag & QUEST_SEND_CLOCK_VALUE))
	{
		if (!Recv(sizeof(iClockValue), &iClockValue))
			return false;
	}
	if (0 != (c_rFlag & QUEST_SEND_COUNTER_NAME))
	{
		if (!Recv(sizeof(szCounterName), &szCounterName))
			return false;

		szCounterName[16] = '\0';
	}
	if (0 != (c_rFlag & QUEST_SEND_COUNTER_VALUE))
	{
		if (!Recv(sizeof(iCounterValue), &iCounterValue))
			return false;
	}
	if (0 != (c_rFlag & QUEST_SEND_ICON_FILE))
	{
		if (!Recv(sizeof(szIconFileName), &szIconFileName))
			return false;

		szIconFileName[24] = '\0';
	}
	// Recv Data End

	CPythonQuest& rkQuest = CPythonQuest::Instance();

	// Process Start
	if (QUEST_PACKET_TYPE_END == byQuestPacketType)
	{
		rkQuest.DeleteQuestInstance(QuestInfo.index);
	}
	else if (QUEST_PACKET_TYPE_UPDATE == byQuestPacketType)
	{
		if (!rkQuest.IsQuest(QuestInfo.index))
		{
			rkQuest.MakeQuest(QuestInfo.index);
		}

		if (strlen(szTitle) > 0)
			rkQuest.SetQuestTitle(QuestInfo.index, szTitle);
		if (strlen(szClockName) > 0)
			rkQuest.SetQuestClockName(QuestInfo.index, szClockName);
		if (strlen(szCounterName) > 0)
			rkQuest.SetQuestCounterName(QuestInfo.index, szCounterName);
		if (strlen(szIconFileName) > 0)
			rkQuest.SetQuestIconFileName(QuestInfo.index, szIconFileName);

		if (c_rFlag & QUEST_SEND_CLOCK_VALUE)
			rkQuest.SetQuestClockValue(QuestInfo.index, iClockValue);
		if (c_rFlag & QUEST_SEND_COUNTER_VALUE)
			rkQuest.SetQuestCounterValue(QuestInfo.index, iCounterValue);
	}
	else if (QUEST_PACKET_TYPE_BEGIN == byQuestPacketType)
	{
		CPythonQuest::SQuestInstance QuestInstance;
		QuestInstance.dwIndex = QuestInfo.index;
		QuestInstance.strTitle = szTitle;
		QuestInstance.strClockName = szClockName;
		QuestInstance.iClockValue = iClockValue;
		QuestInstance.strCounterName = szCounterName;
		QuestInstance.iCounterValue = iCounterValue;
		QuestInstance.strIconFileName = szIconFileName;
		// CPythonQuest::Instance().RegisterQuestInstance(QuestInstance);
		rkQuest.RegisterQuestInstance(QuestInstance);
	}
	// Process Start End

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshQuest", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvQuestConfirmPacket()
{
	TPacketGCQuestConfirm kQuestConfirmPacket;
	if (!Recv(sizeof(kQuestConfirmPacket), &kQuestConfirmPacket))
	{
		Tracen("RecvQuestConfirmPacket Error");
		return false;
	}

	PyObject * poArg = Py_BuildValue("(sii)", kQuestConfirmPacket.msg, kQuestConfirmPacket.timeout, kQuestConfirmPacket.requestPID);
 	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OnQuestConfirm", poArg);
	return true;
}

bool CPythonNetworkStream::RecvRequestMakeGuild()
{
	TPacketGCBlank blank;
	if (!Recv(sizeof(blank), &blank))
	{
		Tracen("RecvRequestMakeGuild Packet Error");
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AskGuildName", Py_BuildValue("()"));

	return true;
}

void CPythonNetworkStream::ToggleGameDebugInfo()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ToggleDebugInfo", Py_BuildValue("()"));
}

bool CPythonNetworkStream::SendExchangeStartPacket(DWORD vid)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_START;
	packet.arg1			= vid;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_start_packet Error\n");
		return false;
	}

	Tracef("send_trade_start_packet   vid %d \n", vid);
	return SendSequence();
}

#if defined(ENABLE_NEW_GOLD_LIMIT)
bool CPythonNetworkStream::SendExchangeElkAddPacket(uint64_t elk)
#else
bool CPythonNetworkStream::SendExchangeElkAddPacket(uint32_t elk)
#endif
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ELK_ADD;
	packet.arg1			= elk;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_elk_add_packet Error\n");
		return false;
	}

	return SendSequence();
}

#if defined(ITEM_CHECKINOUT_UPDATE)
bool CPythonNetworkStream::SendExchangeItemAddPacket(TItemPos ItemPos, BYTE byDisplayPos, bool SelectPosAuto)
#else
bool CPythonNetworkStream::SendExchangeItemAddPacket(TItemPos ItemPos, BYTE byDisplayPos)
#endif
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ITEM_ADD;
	packet.Pos			= ItemPos;
	packet.arg2			= byDisplayPos;
#if defined(ITEM_CHECKINOUT_UPDATE)
	packet.SelectPosAuto = SelectPosAuto;
#endif

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_item_add_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeItemDelPacket(BYTE pos)
{
	assert(!"Can't be called function - CPythonNetworkStream::SendExchangeItemDelPacket");
	return true;

	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ITEM_DEL;
	packet.arg1			= pos;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_item_del_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeAcceptPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ACCEPT;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_accept_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeExitPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_CANCEL;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_exit_packet Error\n");
		return false;
	}

	return SendSequence();
}

#ifdef ENABLE_SWITCHBOT_WORLDARD

bool CPythonNetworkStream::SendSwitchBotOpen()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGSwitchBotSend	packet;

	packet.header		= HEADER_CG_SWITCHBOT;
	packet.subheader	= SWITCH_SUB_HEADER_OPEN;

	if (!Send(sizeof(TPacketCGSwitchBotSend), &packet))
	{
		Tracef("SendSwitchBotOpen Error\n");
		return false;
	}

	return SendSequence();
}


bool CPythonNetworkStream::SendSwitchBotClose()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGSwitchBotSend	packet;

	packet.header		= HEADER_CG_SWITCHBOT;
	packet.subheader	= SWITCH_SUB_HEADER_CLOSE;

	if (!Send(sizeof(TPacketCGSwitchBotSend), &packet))
	{
		Tracef("SendSwitchBotClose Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSwitchBotChange(PyObject* attr,int vnum_change)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGSwitchBotSend	packet;

	packet.header		= HEADER_CG_SWITCHBOT;
	packet.subheader	= SWITCH_SUB_HEADER_CHANGE;
	

	PyObject* w0;
	PyObject* wa_type;
	PyObject* wa_value;

	int size_list = PyList_Size(attr);

	if(size_list < 0 || size_list > 5)
	{
		return true;
	}

	for (int i = 0; i < size_list; ++i )
	{
		w0 			= PyList_GetItem(attr, i);
		wa_type		= PyList_GetItem(w0, 0);
		wa_value 	= PyList_GetItem(w0, 1);

		packet.aAttr[i].bType 	= PyInt_AsLong(wa_type);
		packet.aAttr[i].sValue 	= PyInt_AsLong(wa_value);
	}

	packet.vnum_change = vnum_change;


	if (!Send(sizeof(TPacketCGSwitchBotSend), &packet))
	{
		Tracef("SendSwitchBotChange Error\n");
		return false;
	}

	return SendSequence();
}


#endif

// PointReset 개임시
bool CPythonNetworkStream::SendPointResetPacket()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartPointReset", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::__IsPlayerAttacking()
{
	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
		return false;

	if (!pkInstMain->IsAttacking())
		return false;

	return true;
}

bool CPythonNetworkStream::RecvScriptPacket()
{
	TPacketGCScript ScriptPacket;

	if (!Recv(sizeof(TPacketGCScript), &ScriptPacket))
	{
		TraceError("RecvScriptPacket_RecvError");
		return false;
	}

	if (ScriptPacket.size < sizeof(TPacketGCScript))
	{
		TraceError("RecvScriptPacket_SizeError");
		return false;
	}

	ScriptPacket.size -= sizeof(TPacketGCScript);

	static std::string str;
	str = "";
	str.resize(ScriptPacket.size+1);

	if (!Recv(ScriptPacket.size, &str[0]))
		return false;

	str[str.size()-1] = '\0';

	int iIndex = CPythonEventManager::Instance().RegisterEventSetFromString(str);

	if (-1 != iIndex)
	{
		CPythonEventManager::Instance().SetVisibleLineCount(iIndex, 30);
		CPythonNetworkStream::Instance().OnScriptEventStart(ScriptPacket.skin,iIndex);
	}

	return true;
}

bool CPythonNetworkStream::SendScriptAnswerPacket(int iAnswer)
{
	TPacketCGScriptAnswer ScriptAnswer;

	ScriptAnswer.header = HEADER_CG_SCRIPT_ANSWER;
	ScriptAnswer.answer = (BYTE) iAnswer;
	if (!Send(sizeof(TPacketCGScriptAnswer), &ScriptAnswer))
	{
		Tracen("Send Script Answer Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendScriptButtonPacket(unsigned int iIndex)
{
	TPacketCGScriptButton ScriptButton;

	ScriptButton.header = HEADER_CG_SCRIPT_BUTTON;
	ScriptButton.idx = iIndex;
	if (!Send(sizeof(TPacketCGScriptButton), &ScriptButton))
	{
		Tracen("Send Script Button Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAnswerMakeGuildPacket(const char * c_szName)
{
	TPacketCGAnswerMakeGuild Packet;

	Packet.header = HEADER_CG_ANSWER_MAKE_GUILD;
	strncpy(Packet.guild_name, c_szName, GUILD_NAME_MAX_LEN);
	Packet.guild_name[GUILD_NAME_MAX_LEN] = '\0';

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracen("SendAnswerMakeGuild Packet Error");
		return false;
	}

// 	Tracef(" SendAnswerMakeGuildPacket : %s", c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendQuestInputStringPacket(const char * c_szString)
{
	TPacketCGQuestInputString Packet;
	Packet.bHeader = HEADER_CG_QUEST_INPUT_STRING;
	strncpy(Packet.szString, c_szString, QUEST_INPUT_STRING_MAX_NUM);

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracen("SendQuestInputStringPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuestConfirmPacket(BYTE byAnswer, DWORD dwPID)
{
	TPacketCGQuestConfirm kPacket;
	kPacket.header = HEADER_CG_QUEST_CONFIRM;
	kPacket.answer = byAnswer;
	kPacket.requestPID = dwPID;

	if (!Send(sizeof(kPacket), &kPacket))
	{
		Tracen("SendQuestConfirmPacket Error");
		return false;
	}

	Tracenf(" SendQuestConfirmPacket : %d, %d", byAnswer, dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::RecvSkillCoolTimeEnd()
{
	TPacketGCSkillCoolTimeEnd kPacketSkillCoolTimeEnd;
	if (!Recv(sizeof(kPacketSkillCoolTimeEnd), &kPacketSkillCoolTimeEnd))
	{
		Tracen("CPythonNetworkStream::RecvSkillCoolTimeEnd - RecvError");
		return false;
	}

	CPythonPlayer::Instance().EndSkillCoolTime(kPacketSkillCoolTimeEnd.bSkill);

	return true;
}

bool CPythonNetworkStream::RecvSkillLevel()
{
	TPacketGCSkillLevel packet;
	if (!Recv(sizeof(TPacketGCSkillLevel), &packet))
	{
		Tracen("CPythonNetworkStream::RecvSkillLevel - RecvError");
		return false;
	}

	DWORD dwSlotIndex;

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		if (rkPlayer.GetSkillSlotIndex(i, &dwSlotIndex))
			rkPlayer.SetSkillLevel(dwSlotIndex, packet.abSkillLevels[i]);
	}

	__RefreshSkillWindow();
	__RefreshStatus();
	Tracef(" >> RecvSkillLevel\n");
	return true;
/*
	assert(!"CPythonNetworkStream::RecvSkillLevel - Don't use this function");
	TPacketGCSkillLevel packet;
	if (!Recv(sizeof(TPacketGCSkillLevel), &packet))
	{
		Tracen("CPythonNetworkStream::RecvSkillLevel - RecvError");
		return false;
	}

	DWORD dwSlotIndex;

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		if (rkPlayer.GetSkillSlotIndex(i, &dwSlotIndex))
			rkPlayer.SetSkillLevel(dwSlotIndex, packet.abSkillLevels[i]);
	}

	__RefreshSkillWindow();
	__RefreshStatus();
	Tracef(" >> RecvSkillLevel\n");
	return true;
*/
}

bool CPythonNetworkStream::RecvSkillLevelNew()
{
	TPacketGCSkillLevelNew packet;

	if (!Recv(sizeof(TPacketGCSkillLevelNew), &packet))
	{
		Tracen("CPythonNetworkStream::RecvSkillLevelNew - RecvError");
		return false;
	}

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	

	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		TPlayerSkill & rPlayerSkill = packet.skills[i];
		
		rkPlayer.SetSkillLevel_(i, rPlayerSkill.bMasterType, rPlayerSkill.bLevel);
	}

	__RefreshSkillWindow();
	__RefreshStatus();
	//Tracef(" >> RecvSkillLevelNew\n");
	return true;
}


bool CPythonNetworkStream::RecvDamageInfoPacket()
{
	TPacketGCDamageInfo DamageInfoPacket;

	if (!Recv(sizeof(TPacketGCDamageInfo), &DamageInfoPacket))
	{
		Tracen("Recv Target Packet Error");
		return false;
	}

	CInstanceBase * pInstTarget = CPythonCharacterManager::Instance().GetInstancePtr(DamageInfoPacket.dwVID);
	bool bSelf = (pInstTarget == CPythonCharacterManager::Instance().GetMainInstancePtr());
	bool bTarget = (pInstTarget==m_pInstTarget);
	if (pInstTarget) {
		if(DamageInfoPacket.damage >= 0)
			pInstTarget->AddDamageEffect(DamageInfoPacket.damage,DamageInfoPacket.flag,bSelf,bTarget);
	}

	return true;
}

bool CPythonNetworkStream::RecvTargetPacket()
{
	TPacketGCTarget TargetPacket;

	if (!Recv(sizeof(TPacketGCTarget), &TargetPacket))
	{
		Tracen("Recv Target Packet Error");
		return false;
	}

	CInstanceBase * pInstPlayer = CPythonCharacterManager::Instance().GetMainInstancePtr();
	CInstanceBase * pInstTarget = CPythonCharacterManager::Instance().GetInstancePtr(TargetPacket.dwVID);
	if (pInstPlayer && pInstTarget)
	{
		if (!pInstTarget->IsDead())
		{

#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
			if (pInstTarget->IsBuilding())
#else
			if (pInstTarget->IsPC() || pInstTarget->IsBuilding())
#endif
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoardIfDifferent", Py_BuildValue("(i)", TargetPacket.dwVID));
			else if (pInstPlayer->CanViewTargetHP(*pInstTarget))
#ifdef ENABLE_VIEW_TARGET_DECIMAL_HP
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetHPTargetBoard", Py_BuildValue(
#ifdef ENABLE_VIEW_ELEMENT
				"(iiiii)",
#else
				"(iiii)",
#endif
				TargetPacket.dwVID, TargetPacket.bHPPercent, TargetPacket.iMinHP, TargetPacket.iMaxHP
#ifdef ENABLE_VIEW_ELEMENT
				, TargetPacket.bElement
#endif
				));
#else
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetHPTargetBoard", Py_BuildValue(
#ifdef ENABLE_VIEW_ELEMENT
				"(iii)",
#else
				"(ii)",
#endif
				TargetPacket.dwVID, TargetPacket.bHPPercent
#ifdef ENABLE_VIEW_ELEMENT
				, TargetPacket.bElement
#endif
				));
#endif
			else
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));

			m_pInstTarget = pInstTarget;
		}
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));
	}

	return true;
}

bool CPythonNetworkStream::RecvMountPacket()
{
	TPacketGCMount MountPacket;

	if (!Recv(sizeof(TPacketGCMount), &MountPacket))
	{
		Tracen("Recv Mount Packet Error");
		return false;
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(MountPacket.vid);

	if (pInstance)
	{
		// Mount
		if (0 != MountPacket.mount_vid)
		{
//			pInstance->Ride(MountPacket.pos, MountPacket.mount_vid);
		}
		// Unmount
		else
		{
//			pInstance->Unride(MountPacket.pos, MountPacket.x, MountPacket.y);
		}
	}

	if (CPythonPlayer::Instance().IsMainCharacterIndex(MountPacket.vid))
	{
//		CPythonPlayer::Instance().SetRidingVehicleIndex(MountPacket.mount_vid);
	}

	return true;
}

bool CPythonNetworkStream::RecvChangeSpeedPacket()
{
	TPacketGCChangeSpeed SpeedPacket;

	if (!Recv(sizeof(TPacketGCChangeSpeed), &SpeedPacket))
	{
		Tracen("Recv Speed Packet Error");
		return false;
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(SpeedPacket.vid);

	if (!pInstance)
		return true;

//	pInstance->SetWalkSpeed(SpeedPacket.walking_speed);
//	pInstance->SetRunSpeed(SpeedPacket.running_speed);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Recv

bool CPythonNetworkStream::SendAttackPacket(
#if defined(ENABLE_NEW_ATTACK_METHOD)
uint32_t type,
#endif
UINT uMotAttack, DWORD dwVIDVictim
#if defined(ENABLE_NEW_ATTACK_METHOD)
, uint32_t crc32
#endif
)
{
#if defined(ENABLE_NEW_ATTACK_METHOD)
	if (type != 3 || crc32 != 213334) {
		return true;
	}
#endif

	if (!__CanActMainInstance()) {
		return true;
	}

#if defined(ENABLE_NEW_ANTICHEAT_RULES)
	if (!__IsPlayerAttacking()) {
		return true;
	}
#endif

#if defined(ATTACK_TIME_LOG)
	static DWORD prevAttackTime = timeGetTime();
	DWORD curTime = timeGetTime();
	TraceError("TIME: %.4f(%.4f) ATTACK_PACKET: %d TARGET: %d", curTime/1000.0f, (curTime-prevTime)/1000.0f, uMotAttack, dwVIDVictim);
	prevAttackTime = curTime;
#endif

	TPacketCGAttack p;

	p.bHeader = HEADER_CG_ATTACK;
	p.bType = uMotAttack;
	p.dwVID = dwVIDVictim;
#if defined(ENABLE_NEW_ANTICHEAT_RULES)
	uint32_t a = 429235637, b = 796398961;
	uint32_t verify_vid = dwVIDVictim ^ a;
	verify_vid += b;
	verify_vid >>= 3;
	verify_vid |= 0xAA11;
	verify_vid ^= b;

	p.dwVictimVIDVerify = verify_vid;
#endif

	if (!SendSpecial(sizeof(TPacketCGAttack), &p)) {
		Tracen("Send Battle Attack Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSpecial(int nLen, void * pvBuf)
{
	BYTE bHeader = *(BYTE *) pvBuf;

	switch (bHeader)
	{
		case HEADER_CG_ATTACK:
			{
				TPacketCGAttack * pkPacketAtk = (TPacketCGAttack *) pvBuf;
				pkPacketAtk->bCRCMagicCubeProcPiece = GetProcessCRCMagicCubePiece();
				pkPacketAtk->bCRCMagicCubeFilePiece = GetProcessCRCMagicCubePiece();
				return Send(nLen, pvBuf);
			}
			break;
	}

	return Send(nLen, pvBuf);
}

bool CPythonNetworkStream::RecvAddFlyTargetingPacket()
{
	TPacketGCFlyTargeting kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
		return false;

	__GlobalPositionToLocalPosition(kPacket.lX, kPacket.lY);

	Tracef("VID [%d] Added to target settings\n",kPacket.dwShooterVID);

	CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	CInstanceBase * pShooter = rpcm.GetInstancePtr(kPacket.dwShooterVID);

	if (!pShooter)
	{
#ifndef _DEBUG
		TraceError("CPythonNetworkStream::RecvFlyTargetingPacket() - dwShooterVID[%d] NOT EXIST", kPacket.dwShooterVID);
#endif
		return true;
	}

	CInstanceBase * pTarget = rpcm.GetInstancePtr(kPacket.dwTargetVID);

	if (kPacket.dwTargetVID && pTarget)
	{
		pShooter->GetGraphicThingInstancePtr()->AddFlyTarget(pTarget->GetGraphicThingInstancePtr());
	}
	else
	{
		float h = CPythonBackground::Instance().GetHeight(kPacket.lX,kPacket.lY) + 60.0f; // TEMPORARY HEIGHT
		pShooter->GetGraphicThingInstancePtr()->AddFlyTarget(D3DXVECTOR3(kPacket.lX,kPacket.lY,h));
		//pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(kPacket.kPPosTarget.x,kPacket.kPPosTarget.y,);
	}

	return true;
}

bool CPythonNetworkStream::RecvFlyTargetingPacket()
{
	TPacketGCFlyTargeting kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
		return false;

	__GlobalPositionToLocalPosition(kPacket.lX, kPacket.lY);

	//Tracef("CPythonNetworkStream::RecvFlyTargetingPacket - VID [%d]\n",kPacket.dwShooterVID);

	CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	CInstanceBase * pShooter = rpcm.GetInstancePtr(kPacket.dwShooterVID);

	if (!pShooter)
	{
#ifdef _DEBUG
		TraceError("CPythonNetworkStream::RecvFlyTargetingPacket() - dwShooterVID[%d] NOT EXIST", kPacket.dwShooterVID);
#endif
		return true;
	}

	CInstanceBase * pTarget = rpcm.GetInstancePtr(kPacket.dwTargetVID);

	if (kPacket.dwTargetVID && pTarget)
	{
		pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(pTarget->GetGraphicThingInstancePtr());
	}
	else
	{
		float h = CPythonBackground::Instance().GetHeight(kPacket.lX, kPacket.lY) + 60.0f; // TEMPORARY HEIGHT
		pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(D3DXVECTOR3(kPacket.lX,kPacket.lY,h));
		//pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(kPacket.kPPosTarget.x,kPacket.kPPosTarget.y,);
	}

	return true;
}

bool CPythonNetworkStream::SendShootPacket(UINT uSkill)
{
	TPacketCGShoot kPacketShoot;
	kPacketShoot.bHeader=HEADER_CG_SHOOT;
	kPacketShoot.bType=uSkill;

	if (!Send(sizeof(kPacketShoot), &kPacketShoot))
	{
		Tracen("SendShootPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAddFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition & kPPosTarget)
{
	TPacketCGFlyTargeting packet;

	//CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	packet.bHeader	= HEADER_CG_ADD_FLY_TARGETING;
	packet.dwTargetVID = dwTargetVID;
	packet.lX = kPPosTarget.x;
	packet.lY = kPPosTarget.y;

	__LocalPositionToGlobalPosition(packet.lX, packet.lY);

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send FlyTargeting Packet Error");
		return false;
	}

	return SendSequence();
}


bool CPythonNetworkStream::SendFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition & kPPosTarget)
{
	TPacketCGFlyTargeting packet;

	//CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	packet.bHeader	= HEADER_CG_FLY_TARGETING;
	packet.dwTargetVID = dwTargetVID;
	packet.lX = kPPosTarget.x;
	packet.lY = kPPosTarget.y;

	__LocalPositionToGlobalPosition(packet.lX, packet.lY);

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send FlyTargeting Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvCreateFlyPacket()
{
	TPacketGCCreateFly kPacket;
	if (!Recv(sizeof(TPacketGCCreateFly), &kPacket))
		return false;

	CFlyingManager& rkFlyMgr = CFlyingManager::Instance();
	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();

	CInstanceBase * pkStartInst = rkChrMgr.GetInstancePtr(kPacket.dwStartVID);
	CInstanceBase * pkEndInst = rkChrMgr.GetInstancePtr(kPacket.dwEndVID);
	if (!pkStartInst || !pkEndInst)
		return true;

	rkFlyMgr.CreateIndexedFly(kPacket.bType, pkStartInst->GetGraphicThingInstancePtr(), pkEndInst->GetGraphicThingInstancePtr());

	return true;
}

bool CPythonNetworkStream::SendTargetPacket(DWORD dwVID)
{
	if (m_strPhase != "Game")
		return false;
	TPacketCGTarget packet;
	packet.header = HEADER_CG_TARGET;
	packet.dwVID = dwVID;

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send Target Packet Error");
		return false;
	}

	return SendSequence();
}

#ifdef ENABLE_SKILL_COLOR_SYSTEM
bool CPythonNetworkStream::SendSkillColorPacket(BYTE bSkillSlot, DWORD dwColor1, DWORD dwColor2, DWORD dwColor3, DWORD dwColor4, DWORD dwColor5)
{
	TPacketCGSkillColor pack;
	pack.bheader = HEADER_CG_SKILL_COLOR;
	pack.skill = bSkillSlot;
	pack.col1 = dwColor1;
	pack.col2 = dwColor2;
	pack.col3 = dwColor3;
	pack.col4 = dwColor4;
	pack.col5 = dwColor5;

	if (!Send(sizeof(pack), &pack))
	{
		Tracen("Send Skill Color Packet Error");
		return false;
	}

	return SendSequence();
}
#endif

bool CPythonNetworkStream::SendSyncPositionElementPacket(DWORD dwVictimVID, DWORD dwVictimX, DWORD dwVictimY)
{
	TPacketCGSyncPositionElement kSyncPos;
	kSyncPos.dwVID=dwVictimVID;
	kSyncPos.lX=dwVictimX;
	kSyncPos.lY=dwVictimY;

	__LocalPositionToGlobalPosition(kSyncPos.lX, kSyncPos.lY);

	if (!Send(sizeof(kSyncPos), &kSyncPos))
	{
		Tracen("CPythonNetworkStream::SendSyncPositionElementPacket - ERROR");
		return false;
	}

	return true;
}

bool CPythonNetworkStream::RecvMessenger()
{
	TPacketGCMessenger p;
	if (!Recv(sizeof(p), &p))
		return false;

	int iSize = p.size - sizeof(p);
	char char_name[24+1];

	switch (p.subheader)
	{
		case MESSENGER_SUBHEADER_GC_LIST:
		{
			TPacketGCMessengerListOnline on;
			while(iSize)
			{
				if (!Recv(sizeof(TPacketGCMessengerListOffline),&on))
					return false;

				if (!Recv(on.length, char_name))
					return false;

				char_name[on.length] = 0;

				if (on.connected & MESSENGER_CONNECTED_STATE_ONLINE)
					CPythonMessenger::Instance().OnFriendLogin(char_name);
				else
					CPythonMessenger::Instance().OnFriendLogout(char_name);
				
				if (on.connected & MESSENGER_CONNECTED_STATE_MOBILE)
					CPythonMessenger::Instance().SetMobile(char_name, TRUE);

				iSize -= sizeof(TPacketGCMessengerListOffline);
				iSize -= on.length;
			}
			break;
		}

		case MESSENGER_SUBHEADER_GC_LOGIN:
		{
			TPacketGCMessengerLogin p;
			if (!Recv(sizeof(p),&p))
				return false;
			if (!Recv(p.length, char_name))
				return false;
			char_name[p.length] = 0;
			CPythonMessenger::Instance().OnFriendLogin(char_name);
			__RefreshTargetBoardByName(char_name);
			break;
		}

		case MESSENGER_SUBHEADER_GC_LOGOUT:
		{
			TPacketGCMessengerLogout logout;
			if (!Recv(sizeof(logout),&logout))
				return false;
			if (!Recv(logout.length, char_name))
				return false;
			char_name[logout.length] = 0;
			CPythonMessenger::Instance().OnFriendLogout(char_name);
			break;
		}

		case MESSENGER_SUBHEADER_GC_MOBILE:
		{
			BYTE byState; // 모바일 번호가 없어졌는지 플래그
			BYTE byLength;
			if (!Recv(sizeof(byState), &byState))
				return false;
			if (!Recv(sizeof(byLength), &byLength))
				return false;
			if (!Recv(byLength, char_name))
				return false;
			char_name[byLength] = 0;
			CPythonMessenger::Instance().SetMobile(char_name, byState);
			break;
		}
#if defined(ENABLE_MESSENGER_TEAM)
		case MESSENGER_SUBHEADER_GC_TEAM:
		{
			TTeamMessenger TeamMessenger;
			CPythonMessenger::Instance().ClearTeamList();

			while (iSize)
			{
				if (!Recv(sizeof(TTeamMessenger), &TeamMessenger))
				{
					return false;
				}

				CPythonMessenger::Instance().LoadTeamList(TeamMessenger);

				iSize -= sizeof(TTeamMessenger);
			}

			break;
		}
		case MESSENGER_SUBHEADER_GC_TEAM_UPDATE:
		{
			TTeamMessenger TeamMessenger;
			if (!Recv(sizeof(TTeamMessenger), &TeamMessenger))
			{
				return false;
			}

			CPythonMessenger::Instance().UpdateTeam(TeamMessenger);

			break;
		}
#endif
		case MESSENGER_SUBHEADER_GC_REMOVE_FRIEND:
		{
			BYTE bLength;
			if (!Recv(sizeof(bLength), &bLength))
				return false;

			if (!Recv(bLength, char_name))
				return false;

			char_name[bLength] = 0;
			CPythonMessenger::Instance().RemoveFriend(char_name);
			__RefreshTargetBoardByName(char_name);
			break;
		}
#if defined(ENABLE_MESSENGER_BLOCK)
		case MESSENGER_SUBHEADER_GC_BLOCK_LIST:
		{
			TPacketGCMessengerBlockList onn;

			while (iSize) {
				if (!Recv(sizeof(TPacketGCMessengerBlockList), &onn)) {
					return false;
				}

				if (!Recv(onn.length, char_name)) {
					return false;
				}

				char_name[onn.length] = 0;

				if (onn.connected & MESSENGER_CONNECTED_STATE_ONLINE) {
					CPythonMessenger::Instance().OnBlockLogin(char_name);
				} else {
					CPythonMessenger::Instance().OnBlockLogout(char_name);
				}

				iSize -= sizeof(TPacketGCMessengerBlockList);
				iSize -= onn.length;
			}

			break;
		}
		case MESSENGER_SUBHEADER_GC_BLOCK_LOGIN:
		{
			TPacketGCMessengerLogin pp;
			if (!Recv(sizeof(pp),&pp)) {
				return false;
			}

			if (!Recv(pp.length, char_name)) {
				return false;
			}

			char_name[pp.length] = 0;

			CPythonMessenger::Instance().OnBlockLogin(char_name);
			__RefreshTargetBoardByName(char_name);

			break;
		}
		case MESSENGER_SUBHEADER_GC_BLOCK_LOGOUT:
		{
			TPacketGCMessengerLogout logout2;
			if (!Recv(sizeof(logout2),&logout2)) {
				return false;
			}

			if (!Recv(logout2.length, char_name)) {
				return false;
			}

			char_name[logout2.length] = 0;
			CPythonMessenger::Instance().OnBlockLogout(char_name);

			break;
		}
#endif
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Party

bool CPythonNetworkStream::SendPartyInvitePacket(DWORD dwVID)
{
	TPacketCGPartyInvite kPartyInvitePacket;
	kPartyInvitePacket.header = HEADER_CG_PARTY_INVITE;
	kPartyInvitePacket.vid = dwVID;

	if (!Send(sizeof(kPartyInvitePacket), &kPartyInvitePacket))
	{
		Tracenf("CPythonNetworkStream::SendPartyInvitePacket [%ud] - PACKET SEND ERROR", dwVID);
		return false;
	}

	Tracef(" << SendPartyInvitePacket : %d\n", dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyInviteAnswerPacket(DWORD dwLeaderVID, BYTE byAnswer)
{
	TPacketCGPartyInviteAnswer kPartyInviteAnswerPacket;
	kPartyInviteAnswerPacket.header = HEADER_CG_PARTY_INVITE_ANSWER;
	kPartyInviteAnswerPacket.leader_pid = dwLeaderVID;
	kPartyInviteAnswerPacket.accept = byAnswer;

	if (!Send(sizeof(kPartyInviteAnswerPacket), &kPartyInviteAnswerPacket))
	{
		Tracenf("CPythonNetworkStream::SendPartyInviteAnswerPacket [%ud %ud] - PACKET SEND ERROR", dwLeaderVID, byAnswer);
		return false;
	}

	Tracef(" << SendPartyInviteAnswerPacket : %d, %d\n", dwLeaderVID, byAnswer);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyRemovePacket(DWORD dwPID)
{
	TPacketCGPartyRemove kPartyInviteRemove;
	kPartyInviteRemove.header = HEADER_CG_PARTY_REMOVE;
	kPartyInviteRemove.pid = dwPID;

	if (!Send(sizeof(kPartyInviteRemove), &kPartyInviteRemove))
	{
		Tracenf("CPythonNetworkStream::SendPartyRemovePacket [%ud] - PACKET SEND ERROR", dwPID);
		return false;
	}

	Tracef(" << SendPartyRemovePacket : %d\n", dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartySetStatePacket(DWORD dwVID, BYTE byState, BYTE byFlag)
{
	TPacketCGPartySetState kPartySetState;
	kPartySetState.byHeader = HEADER_CG_PARTY_SET_STATE;
	kPartySetState.dwVID = dwVID;
	kPartySetState.byState = byState;
	kPartySetState.byFlag = byFlag;

	if (!Send(sizeof(kPartySetState), &kPartySetState))
	{
		Tracenf("CPythonNetworkStream::SendPartySetStatePacket(%ud, %ud) - PACKET SEND ERROR", dwVID, byState);
		return false;
	}

	Tracef(" << SendPartySetStatePacket : %d, %d, %d\n", dwVID, byState, byFlag);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyUseSkillPacket(BYTE bySkillIndex, DWORD dwVID)
{
	TPacketCGPartyUseSkill kPartyUseSkill;
	kPartyUseSkill.byHeader = HEADER_CG_PARTY_USE_SKILL;
	kPartyUseSkill.bySkillIndex = bySkillIndex;
	kPartyUseSkill.dwTargetVID = dwVID;

	if (!Send(sizeof(kPartyUseSkill), &kPartyUseSkill))
	{
		Tracenf("CPythonNetworkStream::SendPartyUseSkillPacket(%ud, %ud) - PACKET SEND ERROR", bySkillIndex, dwVID);
		return false;
	}

	Tracef(" << SendPartyUseSkillPacket : %d, %d\n", bySkillIndex, dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyParameterPacket(BYTE byDistributeMode)
{
	TPacketCGPartyParameter kPartyParameter;
	kPartyParameter.bHeader = HEADER_CG_PARTY_PARAMETER;
	kPartyParameter.bDistributeMode = byDistributeMode;

	if (!Send(sizeof(kPartyParameter), &kPartyParameter))
	{
		Tracenf("CPythonNetworkStream::SendPartyParameterPacket(%d) - PACKET SEND ERROR", byDistributeMode);
		return false;
	}

	Tracef(" << SendPartyParameterPacket : %d\n", byDistributeMode);
	return SendSequence();
}

bool CPythonNetworkStream::RecvPartyInvite()
{
	TPacketGCPartyInvite kPartyInvitePacket;
	if (!Recv(sizeof(kPartyInvitePacket), &kPartyInvitePacket))
		return false;

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kPartyInvitePacket.leader_pid);
	if (!pInstance)
	{
		TraceError(" CPythonNetworkStream::RecvPartyInvite - Failed to find leader instance [%d]\n", kPartyInvitePacket.leader_pid);
		return true;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RecvPartyInviteQuestion", Py_BuildValue("(is)", kPartyInvitePacket.leader_pid, pInstance->GetNameString()));
	Tracef(" >> RecvPartyInvite : %d, %s\n", kPartyInvitePacket.leader_pid, pInstance->GetNameString());

	return true;
}

bool CPythonNetworkStream::RecvPartyAdd()
{
	TPacketGCPartyAdd kPartyAddPacket;
	if (!Recv(sizeof(kPartyAddPacket), &kPartyAddPacket))
		return false;

	CPythonPlayer::Instance().AppendPartyMember(kPartyAddPacket.pid, kPartyAddPacket.name);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AddPartyMember", Py_BuildValue("(is)", kPartyAddPacket.pid, kPartyAddPacket.name));
	Tracef(" >> RecvPartyAdd : %d, %s\n", kPartyAddPacket.pid, kPartyAddPacket.name);

	return true;
}

bool CPythonNetworkStream::RecvPartyUpdate()
{
	TPacketGCPartyUpdate kPartyUpdatePacket;
	if (!Recv(sizeof(kPartyUpdatePacket), &kPartyUpdatePacket))
		return false;

	CPythonPlayer::TPartyMemberInfo * pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(kPartyUpdatePacket.pid, &pPartyMemberInfo))
		return true;

	BYTE byOldState = pPartyMemberInfo->byState;

	CPythonPlayer::Instance().UpdatePartyMemberInfo(kPartyUpdatePacket.pid, kPartyUpdatePacket.state, kPartyUpdatePacket.percent_hp);
	for (int i = 0; i < PARTY_AFFECT_SLOT_MAX_NUM; ++i)
	{
		CPythonPlayer::Instance().UpdatePartyMemberAffect(kPartyUpdatePacket.pid, i, kPartyUpdatePacket.affects[i]);
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UpdatePartyMemberInfo", Py_BuildValue("(i)", kPartyUpdatePacket.pid));

	// 만약 리더가 바뀌었다면, TargetBoard 의 버튼을 업데이트 한다.
	DWORD dwVID;
	if (CPythonPlayer::Instance().PartyMemberPIDToVID(kPartyUpdatePacket.pid, &dwVID))
	if (byOldState != kPartyUpdatePacket.state)
	{
		__RefreshTargetBoardByVID(dwVID);
	}

// 	Tracef(" >> RecvPartyUpdate : %d, %d, %d\n", kPartyUpdatePacket.pid, kPartyUpdatePacket.state, kPartyUpdatePacket.percent_hp);

	return true;
}

bool CPythonNetworkStream::RecvPartyRemove()
{
	TPacketGCPartyRemove kPartyRemovePacket;
	if (!Recv(sizeof(kPartyRemovePacket), &kPartyRemovePacket))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RemovePartyMember", Py_BuildValue("(i)", kPartyRemovePacket.pid));
	Tracef(" >> RecvPartyRemove : %d\n", kPartyRemovePacket.pid);

	return true;
}

bool CPythonNetworkStream::RecvPartyLink()
{
	TPacketGCPartyLink kPartyLinkPacket;
	if (!Recv(sizeof(kPartyLinkPacket), &kPartyLinkPacket))
		return false;

	CPythonPlayer::Instance().LinkPartyMember(kPartyLinkPacket.pid, kPartyLinkPacket.vid);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "LinkPartyMember", Py_BuildValue("(ii)", kPartyLinkPacket.pid, kPartyLinkPacket.vid));
	Tracef(" >> RecvPartyLink : %d, %d\n", kPartyLinkPacket.pid, kPartyLinkPacket.vid);

	return true;
}

bool CPythonNetworkStream::RecvPartyUnlink()
{
	TPacketGCPartyUnlink kPartyUnlinkPacket;
	if (!Recv(sizeof(kPartyUnlinkPacket), &kPartyUnlinkPacket))
		return false;

	CPythonPlayer::Instance().UnlinkPartyMember(kPartyUnlinkPacket.pid);

	if (CPythonPlayer::Instance().IsMainCharacterIndex(kPartyUnlinkPacket.vid))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UnlinkAllPartyMember", Py_BuildValue("()"));
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UnlinkPartyMember", Py_BuildValue("(i)", kPartyUnlinkPacket.pid));
	}

	Tracef(" >> RecvPartyUnlink : %d, %d\n", kPartyUnlinkPacket.pid, kPartyUnlinkPacket.vid);

	return true;
}

bool CPythonNetworkStream::RecvPartyParameter()
{
	TPacketGCPartyParameter kPartyParameterPacket;
	if (!Recv(sizeof(kPartyParameterPacket), &kPartyParameterPacket))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ChangePartyParameter", Py_BuildValue("(i)", kPartyParameterPacket.bDistributeMode));
	Tracef(" >> RecvPartyParameter : %d\n", kPartyParameterPacket.bDistributeMode);

	return true;
}

// Party
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Guild

bool CPythonNetworkStream::SendGuildAddMemberPacket(DWORD dwVID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_ADD_MEMBER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwVID), &dwVID))
		return false;

	Tracef(" SendGuildAddMemberPacket\n", dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildRemoveMemberPacket(DWORD dwPID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REMOVE_MEMBER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwPID), &dwPID))
		return false;

	Tracef(" SendGuildRemoveMemberPacket %d\n", dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeGradeNamePacket(BYTE byGradeNumber, const char * c_szName)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(byGradeNumber), &byGradeNumber))
		return false;

	char szName[GUILD_GRADE_NAME_MAX_LEN+1];
	strncpy(szName, c_szName, GUILD_GRADE_NAME_MAX_LEN);
	szName[GUILD_GRADE_NAME_MAX_LEN] = '\0';

	if (!Send(sizeof(szName), &szName))
		return false;

	Tracef(" SendGuildChangeGradeNamePacket %d, %s\n", byGradeNumber, c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeGradeAuthorityPacket(BYTE byGradeNumber, BYTE byAuthority)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(byGradeNumber), &byGradeNumber))
		return false;
	if (!Send(sizeof(byAuthority), &byAuthority))
		return false;

	Tracef(" SendGuildChangeGradeAuthorityPacket %d, %d\n", byGradeNumber, byAuthority);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildOfferPacket(DWORD dwExperience)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_OFFER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwExperience), &dwExperience))
		return false;

	Tracef(" SendGuildOfferPacket %d\n", dwExperience);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildPostCommentPacket(const char * c_szMessage)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_POST_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	BYTE bySize = BYTE(strlen(c_szMessage)) + 1;
	if (!Send(sizeof(bySize), &bySize))
		return false;
	if (!Send(bySize, c_szMessage))
		return false;

	Tracef(" SendGuildPostCommentPacket %d, %s\n", bySize, c_szMessage);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildDeleteCommentPacket(DWORD dwIndex)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_DELETE_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwIndex), &dwIndex))
		return false;

	Tracef(" SendGuildDeleteCommentPacket %d\n", dwIndex);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildRefreshCommentsPacket(DWORD dwHighestIndex)
{
	static DWORD s_LastTime = timeGetTime() - 1001;

	if (timeGetTime() - s_LastTime < 1000)
		return true;
	s_LastTime = timeGetTime();

	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REFRESH_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	Tracef(" SendGuildRefreshCommentPacket %d\n", dwHighestIndex);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeMemberGradePacket(DWORD dwPID, BYTE byGrade)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwPID), &dwPID))
		return false;
	if (!Send(sizeof(byGrade), &byGrade))
		return false;

	Tracef(" SendGuildChangeMemberGradePacket %d, %d\n", dwPID, byGrade);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildUseSkillPacket(DWORD dwSkillID, DWORD dwTargetVID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_USE_SKILL;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwSkillID), &dwSkillID))
		return false;
	if (!Send(sizeof(dwTargetVID), &dwTargetVID))
		return false;

	Tracef(" SendGuildUseSkillPacket %d, %d\n", dwSkillID, dwTargetVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeMemberGeneralPacket(DWORD dwPID, BYTE byFlag)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwPID), &dwPID))
		return false;
	if (!Send(sizeof(byFlag), &byFlag))
		return false;

	Tracef(" SendGuildChangeMemberGeneralFlagPacket %d, %d\n", dwPID, byFlag);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildInviteAnswerPacket(DWORD dwGuildID, BYTE byAnswer)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwGuildID), &dwGuildID))
		return false;
	if (!Send(sizeof(byAnswer), &byAnswer))
		return false;

	Tracef(" SendGuildInviteAnswerPacket %d, %d\n", dwGuildID, byAnswer);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChargeGSPPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHARGE_GSP;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwMoney), &dwMoney))
		return false;

	Tracef(" SendGuildChargeGSPPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildDepositMoneyPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_DEPOSIT_MONEY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwMoney), &dwMoney))
		return false;

	Tracef(" SendGuildDepositMoneyPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildWithdrawMoneyPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_WITHDRAW_MONEY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwMoney), &dwMoney))
		return false;

	Tracef(" SendGuildWithdrawMoneyPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeSymbol()
{
	TPacketCGGuild packet;
	packet.byHeader = HEADER_CG_GUILD;
	packet.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_SYMBOL;
	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvGuild()
{
    TPacketGCGuild GuildPacket;
	if (!Recv(sizeof(GuildPacket), &GuildPacket))
		return false;

	switch(GuildPacket.subheader)
	{
		case GUILD_SUBHEADER_GC_LOGIN:
		{
			DWORD dwPID;
			if (!Recv(sizeof(DWORD), &dwPID))
				return false;

			// Messenger
			CPythonGuild::TGuildMemberData * pGuildMemberData;
			if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pGuildMemberData))
				if (0 != pGuildMemberData->strName.compare(CPythonPlayer::Instance().GetName()))
					CPythonMessenger::Instance().LoginGuildMember(pGuildMemberData->strName.c_str());

			//Tracef(" <Login> %d\n", dwPID);
			break;
		}
		case GUILD_SUBHEADER_GC_LOGOUT:
		{
			DWORD dwPID;
			if (!Recv(sizeof(DWORD), &dwPID))
				return false;

			// Messenger
			CPythonGuild::TGuildMemberData * pGuildMemberData;
			if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pGuildMemberData))
				if (0 != pGuildMemberData->strName.compare(CPythonPlayer::Instance().GetName()))
					CPythonMessenger::Instance().LogoutGuildMember(pGuildMemberData->strName.c_str());

			//Tracef(" <Logout> %d\n", dwPID);
			break;
		}
		case GUILD_SUBHEADER_GC_REMOVE:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;

			// Main Player 일 경우 DeleteGuild
			if (CPythonGuild::Instance().IsMainPlayer(dwPID))
			{
				CPythonGuild::Instance().Destroy();
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "DeleteGuild", Py_BuildValue("()"));
				CPythonMessenger::Instance().RemoveAllGuildMember();
				__SetGuildID(0);
				__RefreshMessengerWindow();
				__RefreshTargetBoard();
				__RefreshCharacterWindow();
			}
			else
			{
				// Get Member Name
				std::string strMemberName = "";
				CPythonGuild::TGuildMemberData * pData;
				if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pData))
				{
					strMemberName = pData->strName;
					CPythonMessenger::Instance().RemoveGuildMember(pData->strName.c_str());
				}

				CPythonGuild::Instance().RemoveMember(dwPID);

				// Refresh
				__RefreshTargetBoardByName(strMemberName.c_str());
				__RefreshGuildWindowMemberPage();
			}

			Tracef(" <Remove> %d\n", dwPID);
			break;
		}
		case GUILD_SUBHEADER_GC_LIST:
		{
			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);

			for (; iPacketSize > 0;)
			{
				TPacketGCGuildSubMember memberPacket;
				if (!Recv(sizeof(memberPacket), &memberPacket))
					return false;

				char szName[CHARACTER_NAME_MAX_LEN+1] = "";
				if (memberPacket.byNameFlag)
				{
					if (!Recv(sizeof(szName), &szName))
						return false;

					iPacketSize -= CHARACTER_NAME_MAX_LEN+1;
				}
				else
				{
					CPythonGuild::TGuildMemberData * pMemberData;
					if (CPythonGuild::Instance().GetMemberDataPtrByPID(memberPacket.pid, &pMemberData))
					{
						strncpy(szName, pMemberData->strName.c_str(), CHARACTER_NAME_MAX_LEN);
					}
				}

				//Tracef(" <List> %d : %s, %d (%d, %d, %d)\n", memberPacket.pid, szName, memberPacket.byGrade, memberPacket.byJob, memberPacket.byLevel, memberPacket.dwOffer);

				CPythonGuild::SGuildMemberData GuildMemberData;
				GuildMemberData.dwPID = memberPacket.pid;
				GuildMemberData.byGrade = memberPacket.byGrade;
				GuildMemberData.strName = szName;
				GuildMemberData.byJob = memberPacket.byJob;
				GuildMemberData.byLevel = memberPacket.byLevel;
				GuildMemberData.dwOffer = memberPacket.dwOffer;
				GuildMemberData.byGeneralFlag = memberPacket.byIsGeneral;
				CPythonGuild::Instance().RegisterMember(GuildMemberData);

				// Messenger
				if (strcmp(szName, CPythonPlayer::Instance().GetName()))
					CPythonMessenger::Instance().AppendGuildMember(szName);

				__RefreshTargetBoardByName(szName);

				iPacketSize -= sizeof(memberPacket);
			}

			__RefreshGuildWindowInfoPage();
			__RefreshGuildWindowMemberPage();
			__RefreshMessengerWindow();
			__RefreshCharacterWindow();
			break;
		}
		case GUILD_SUBHEADER_GC_GRADE:
		{
			BYTE byCount;
			if (!Recv(sizeof(byCount), &byCount))
				return false;

			for (BYTE i = 0; i < byCount; ++ i)
			{
				BYTE byIndex;
				if (!Recv(sizeof(byCount), &byIndex))
					return false;
				TPacketGCGuildSubGrade GradePacket;
				if (!Recv(sizeof(GradePacket), &GradePacket))
					return false;

				CPythonGuild::Instance().SetGradeData(byIndex, CPythonGuild::SGuildGradeData(GradePacket.auth_flag, GradePacket.grade_name));
				//Tracef(" <Grade> [%d/%d] : %s, %d\n", byIndex, byCount, GradePacket.grade_name, GradePacket.auth_flag);
			}
			__RefreshGuildWindowGradePage();
			__RefreshGuildWindowMemberPageGradeComboBox();
			break;
		}
		case GUILD_SUBHEADER_GC_GRADE_NAME:
		{
			BYTE byGradeNumber;
			if (!Recv(sizeof(byGradeNumber), &byGradeNumber))
				return false;

			char szGradeName[GUILD_GRADE_NAME_MAX_LEN+1] = "";
			if (!Recv(sizeof(szGradeName), &szGradeName))
				return false;

			CPythonGuild::Instance().SetGradeName(byGradeNumber, szGradeName);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGrade", Py_BuildValue("()"));

			Tracef(" <Change Grade Name> %d, %s\n", byGradeNumber, szGradeName);
			__RefreshGuildWindowGradePage();
			__RefreshGuildWindowMemberPageGradeComboBox();
			break;
		}
		case GUILD_SUBHEADER_GC_GRADE_AUTH:
		{
			BYTE byGradeNumber;
			if (!Recv(sizeof(byGradeNumber), &byGradeNumber))
				return false;
			BYTE byAuthorityFlag;
			if (!Recv(sizeof(byAuthorityFlag), &byAuthorityFlag))
				return false;

			CPythonGuild::Instance().SetGradeAuthority(byGradeNumber, byAuthorityFlag);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGrade", Py_BuildValue("()"));

			Tracef(" <Change Grade Authority> %d, %d\n", byGradeNumber, byAuthorityFlag);
			__RefreshGuildWindowGradePage();
			break;
		}
		case GUILD_SUBHEADER_GC_INFO:
		{
			TPacketGCGuildInfo GuildInfo;
			if (!Recv(sizeof(GuildInfo), &GuildInfo))
				return false;

			CPythonGuild::Instance().EnableGuild();
			CPythonGuild::TGuildInfo & rGuildInfo = CPythonGuild::Instance().GetGuildInfoRef();
			strncpy(rGuildInfo.szGuildName, GuildInfo.name, GUILD_NAME_MAX_LEN);
			rGuildInfo.szGuildName[GUILD_NAME_MAX_LEN] = '\0';

			rGuildInfo.dwGuildID = GuildInfo.guild_id;
			rGuildInfo.dwMasterPID = GuildInfo.master_pid;
			rGuildInfo.dwGuildLevel = GuildInfo.level;
			rGuildInfo.dwCurrentExperience = GuildInfo.exp;
			rGuildInfo.dwCurrentMemberCount = GuildInfo.member_count;
			rGuildInfo.dwMaxMemberCount = GuildInfo.max_member_count;
			rGuildInfo.dwGuildMoney = GuildInfo.gold;
			rGuildInfo.bHasLand = GuildInfo.hasLand;

			//Tracef(" <Info> %s, %d, %d : %d\n", GuildInfo.name, GuildInfo.master_pid, GuildInfo.level, rGuildInfo.bHasLand);
			__RefreshGuildWindowInfoPage();
			break;
		}
		case GUILD_SUBHEADER_GC_COMMENTS:
		{
			BYTE byCount;
			if (!Recv(sizeof(byCount), &byCount))
				return false;

			CPythonGuild::Instance().ClearComment();
			//Tracef(" >>> Comments Count : %d\n", byCount);

			for (BYTE i = 0; i < byCount; ++i)
			{
				DWORD dwCommentID;
				if (!Recv(sizeof(dwCommentID), &dwCommentID))
					return false;

				char szName[CHARACTER_NAME_MAX_LEN+1] = "";
				if (!Recv(sizeof(szName), &szName))
					return false;

				char szComment[GULID_COMMENT_MAX_LEN+1] = "";
				if (!Recv(sizeof(szComment), &szComment))
					return false;

				//Tracef(" [Comment-%d] : %s, %s\n", dwCommentID, szName, szComment);
				CPythonGuild::Instance().RegisterComment(dwCommentID, szName, szComment);
			}

			__RefreshGuildWindowBoardPage();
			break;
		}
		case GUILD_SUBHEADER_GC_CHANGE_EXP:
		{
			BYTE byLevel;
			if (!Recv(sizeof(byLevel), &byLevel))
				return false;
			DWORD dwEXP;
			if (!Recv(sizeof(dwEXP), &dwEXP))
				return false;
			CPythonGuild::Instance().SetGuildEXP(byLevel, dwEXP);
			Tracef(" <ChangeEXP> %d, %d\n", byLevel, dwEXP);
			__RefreshGuildWindowInfoPage();
			break;
		}
		case GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;
			BYTE byGrade;
			if (!Recv(sizeof(byGrade), &byGrade))
				return false;
			CPythonGuild::Instance().ChangeGuildMemberGrade(dwPID, byGrade);
			Tracef(" <ChangeMemberGrade> %d, %d\n", dwPID, byGrade);
			__RefreshGuildWindowMemberPage();
			break;
		}
		case GUILD_SUBHEADER_GC_SKILL_INFO:
		{
			CPythonGuild::TGuildSkillData & rSkillData = CPythonGuild::Instance().GetGuildSkillDataRef();
			if (!Recv(sizeof(rSkillData.bySkillPoint), &rSkillData.bySkillPoint))
				return false;
			if (!Recv(sizeof(rSkillData.bySkillLevel), rSkillData.bySkillLevel))
				return false;
			if (!Recv(sizeof(rSkillData.wGuildPoint), &rSkillData.wGuildPoint))
				return false;
			if (!Recv(sizeof(rSkillData.wMaxGuildPoint), &rSkillData.wMaxGuildPoint))
				return false;

			Tracef(" <SkillInfo> %d / %d, %d\n", rSkillData.bySkillPoint, rSkillData.wGuildPoint, rSkillData.wMaxGuildPoint);
			__RefreshGuildWindowSkillPage();
			break;
		}
		case GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;
			BYTE byFlag;
			if (!Recv(sizeof(byFlag), &byFlag))
				return false;

			CPythonGuild::Instance().ChangeGuildMemberGeneralFlag(dwPID, byFlag);
			Tracef(" <ChangeMemberGeneralFlag> %d, %d\n", dwPID, byFlag);
			__RefreshGuildWindowMemberPage();
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_INVITE:
		{
			DWORD dwGuildID;
			if (!Recv(sizeof(dwGuildID), &dwGuildID))
				return false;
			char szGuildName[GUILD_NAME_MAX_LEN+1];
			if (!Recv(GUILD_NAME_MAX_LEN, &szGuildName))
				return false;

			szGuildName[GUILD_NAME_MAX_LEN] = 0;

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RecvGuildInviteQuestion", Py_BuildValue("(is)", dwGuildID, szGuildName));
			Tracef(" <Guild Invite> %d, %s\n", dwGuildID, szGuildName);
			break;
		}
		case GUILD_SUBHEADER_GC_WAR:
		{
			TPacketGCGuildWar kGuildWar;
			if (!Recv(sizeof(kGuildWar), &kGuildWar))
				return false;

			switch (kGuildWar.bWarState)
			{
				case GUILD_WAR_SEND_DECLARE:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_SEND_DECLARE\n");
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnSendDeclare",
						Py_BuildValue("(i)", kGuildWar.dwGuildOpp)
					);
					break;
				case GUILD_WAR_RECV_DECLARE:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_RECV_DECLARE\n");
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnRecvDeclare",
						Py_BuildValue("(ii)", kGuildWar.dwGuildOpp, kGuildWar.bType)
					);
					break;
				case GUILD_WAR_ON_WAR:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_ON_WAR : %d, %d\n", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp);
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnStart",
						Py_BuildValue("(ii)", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp)
					);
					CPythonGuild::Instance().StartGuildWar(kGuildWar.dwGuildOpp);
					break;
				case GUILD_WAR_END:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_END\n");
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnEnd",
						Py_BuildValue("(ii)", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp)
					);
					CPythonGuild::Instance().EndGuildWar(kGuildWar.dwGuildOpp);
					break;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_NAME:
		{
			DWORD dwID;
			char szGuildName[GUILD_NAME_MAX_LEN+1];

			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);

			int nItemSize = sizeof(dwID) + GUILD_NAME_MAX_LEN;

			assert(iPacketSize%nItemSize==0 && "GUILD_SUBHEADER_GC_GUILD_NAME");

			for (; iPacketSize > 0;)
			{
				if (!Recv(sizeof(dwID), &dwID))
					return false;

				if (!Recv(GUILD_NAME_MAX_LEN, &szGuildName))
					return false;

				szGuildName[GUILD_NAME_MAX_LEN] = 0;

				//Tracef(" >> GulidName [%d : %s]\n", dwID, szGuildName);
				CPythonGuild::Instance().RegisterGuildName(dwID, szGuildName);
				iPacketSize -= nItemSize;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_WAR_LIST:
		{
			DWORD dwSrcGuildID;
			DWORD dwDstGuildID;

			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);
			int nItemSize = sizeof(dwSrcGuildID) + sizeof(dwDstGuildID);

			assert(iPacketSize%nItemSize==0 && "GUILD_SUBHEADER_GC_GUILD_WAR_LIST");

			for (; iPacketSize > 0;)
			{
				if (!Recv(sizeof(dwSrcGuildID), &dwSrcGuildID))
					return false;

				if (!Recv(sizeof(dwDstGuildID), &dwDstGuildID))
					return false;

				Tracef(" >> GulidWarList [%d vs %d]\n", dwSrcGuildID, dwDstGuildID);
				CInstanceBase::InsertGVGKey(dwSrcGuildID, dwDstGuildID);
				CPythonCharacterManager::Instance().ChangeGVG(dwSrcGuildID, dwDstGuildID);
				iPacketSize -= nItemSize;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST:
		{
			DWORD dwSrcGuildID;
			DWORD dwDstGuildID;

			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);
			int nItemSize = sizeof(dwSrcGuildID) + sizeof(dwDstGuildID);

			assert(iPacketSize%nItemSize==0 && "GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST");

			for (; iPacketSize > 0;)
			{

				if (!Recv(sizeof(dwSrcGuildID), &dwSrcGuildID))
					return false;

				if (!Recv(sizeof(dwDstGuildID), &dwDstGuildID))
					return false;

				Tracef(" >> GulidWarEndList [%d vs %d]\n", dwSrcGuildID, dwDstGuildID);
				CInstanceBase::RemoveGVGKey(dwSrcGuildID, dwDstGuildID);
				CPythonCharacterManager::Instance().ChangeGVG(dwSrcGuildID, dwDstGuildID);
				iPacketSize -= nItemSize;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_WAR_POINT:
		{
			TPacketGuildWarPoint GuildWarPoint;
			if (!Recv(sizeof(GuildWarPoint), &GuildWarPoint))
				return false;

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
				"BINARY_GuildWar_OnRecvPoint",
				Py_BuildValue("(iii)", GuildWarPoint.dwGainGuildID, GuildWarPoint.dwOpponentGuildID, GuildWarPoint.lPoint)
			);
			break;
		}
		case GUILD_SUBHEADER_GC_MONEY_CHANGE:
		{
			DWORD dwMoney;
			if (!Recv(sizeof(dwMoney), &dwMoney))
				return false;

			CPythonGuild::Instance().SetGuildMoney(dwMoney);

			__RefreshGuildWindowInfoPage();
			Tracef(" >> Guild Money Change : %d\n", dwMoney);
			break;
		}

		case GUILD_SUBHEADER_GC_SYMBOL_AUTH_DATA:
		{
			TPacketGuildSymbolAuth sub_packet;
			if (!Recv(sizeof(sub_packet), &sub_packet))
				return false;

			CGuildMarkUploader::Instance().SetSymbolAuthData(&sub_packet);

			break;
		}

		case GUILD_SUBHEADER_GC_SYMBOL_LOGIN:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;

			const auto errorCode = UploadMark(dwPID, GetMarkImageFileName().c_str());
			if (errorCode != ERROR_NONE)
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_GuildSymbolError", Py_BuildValue("(i)", errorCode));
			break;
		}

	}

	return true;
}

// Guild
//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
// Fishing

bool CPythonNetworkStream::SendFishingPacket(int iRotation)
{
	BYTE byHeader = HEADER_CG_FISHING;
	if (!Send(sizeof(byHeader), &byHeader))
		return false;
	BYTE byPacketRotation = iRotation / 5;
	if (!Send(sizeof(BYTE), &byPacketRotation))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendGiveItemPacket(DWORD dwTargetVID, TItemPos ItemPos, int iItemCount)
{
	TPacketCGGiveItem GiveItemPacket;
	GiveItemPacket.byHeader = HEADER_CG_GIVE_ITEM;
	GiveItemPacket.dwTargetVID = dwTargetVID;
	GiveItemPacket.ItemPos = ItemPos;
	GiveItemPacket.byItemCount = iItemCount;

	if (!Send(sizeof(GiveItemPacket), &GiveItemPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvFishing()
{
	TPacketGCFishing FishingPacket;
	if (!Recv(sizeof(FishingPacket), &FishingPacket))
		return false;

	CInstanceBase * pFishingInstance = NULL;
	if (FISHING_SUBHEADER_GC_FISH != FishingPacket.subheader)
	{
		pFishingInstance = CPythonCharacterManager::Instance().GetInstancePtr(FishingPacket.info);
		if (!pFishingInstance)
			return true;
	}

	switch (FishingPacket.subheader)
	{
		case FISHING_SUBHEADER_GC_START:
			pFishingInstance->StartFishing(float(FishingPacket.dir) * 5.0f);
			break;
		case FISHING_SUBHEADER_GC_STOP:
			{
				if (pFishingInstance->IsFishing()) {
					pFishingInstance->StopFishing();
				}

#ifdef ENABLE_NEW_FISHING_SYSTEM
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingStop", Py_BuildValue("()"));
#endif
			}
			break;
		case FISHING_SUBHEADER_GC_REACT:
			if (pFishingInstance->IsFishing())
			{
				pFishingInstance->SetFishEmoticon(); // Fish Emoticon
				pFishingInstance->ReactFishing();
			}
			break;
		case FISHING_SUBHEADER_GC_SUCCESS:
			pFishingInstance->CatchSuccess();
			break;
		case FISHING_SUBHEADER_GC_FAIL:
			pFishingInstance->CatchFail();
			if (pFishingInstance == CPythonCharacterManager::Instance().GetMainInstancePtr())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingFailure", Py_BuildValue("()"));
			}
			break;
		case FISHING_SUBHEADER_GC_FISH:
		{
			DWORD dwFishID = FishingPacket.info;

			if (0 == FishingPacket.info)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingNotifyUnknown", Py_BuildValue("()"));
				return true;
			}

			CItemData * pItemData;
			if (!CItemManager::Instance().GetItemDataPointer(dwFishID, &pItemData))
				return true;

			CInstanceBase * pMainInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
			if (!pMainInstance)
				return true;

			if (pMainInstance->IsFishing())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingNotify", Py_BuildValue("(is)", CItemData::ITEM_TYPE_FISH == pItemData->GetType(), pItemData->GetName()));
			}
			else
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingSuccess", Py_BuildValue("(is)", CItemData::ITEM_TYPE_FISH == pItemData->GetType(), pItemData->GetName()));
			}
			break;
		}
	}

	return true;
}
// Fishing
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Dungeon
bool CPythonNetworkStream::RecvDungeon()
{
	TPacketGCDungeon DungeonPacket;
	if (!Recv(sizeof(DungeonPacket), &DungeonPacket))
		return false;

	switch (DungeonPacket.subheader)
	{
		case DUNGEON_SUBHEADER_GC_TIME_ATTACK_START:
		{
			break;
		}
		case DUNGEON_SUBHEADER_GC_DESTINATION_POSITION:
		{
			uint32_t ulx, uly;
			if (!Recv(sizeof(ulx), &ulx))
				return false;
			if (!Recv(sizeof(uly), &uly))
				return false;

			CPythonPlayer::Instance().SetDungeonDestinationPosition(ulx, uly);
			break;
		}
	}

	return true;
}
// Dungeon
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// MyShop
bool CPythonNetworkStream::SendBuildPrivateShopPacket(const char * c_szName, const std::vector<TShopItemTable> & c_rSellingItemStock)
{
	TPacketCGMyShop packet;
	packet.bHeader = HEADER_CG_MYSHOP;
	strncpy(packet.szSign, c_szName, SHOP_SIGN_MAX_LEN);
	packet.bCount = (BYTE)c_rSellingItemStock.size();

	if (!Send(sizeof(packet), &packet))
		return false;

	for (std::vector<TShopItemTable>::const_iterator itor = c_rSellingItemStock.begin(); itor < c_rSellingItemStock.end(); ++itor)
	{
		const TShopItemTable & c_rItem = *itor;
		if (!Send(sizeof(c_rItem), &c_rItem))
			return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvShopSignPacket()
{
	TPacketGCShopSign p;
	if (!Recv(sizeof(TPacketGCShopSign), &p))
		return false;

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();

	if (0 == strlen(p.szSign))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"BINARY_PrivateShop_Disappear",
			Py_BuildValue("(i)", p.dwVID)
		);

		if (rkPlayer.IsMainCharacterIndex(p.dwVID))
			rkPlayer.ClosePrivateShop();
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_PrivateShop_Appear", Py_BuildValue("(is)", p.dwVID, p.szSign));

		if (rkPlayer.IsMainCharacterIndex(p.dwVID))
			rkPlayer.OpenPrivateShop();
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////

bool CPythonNetworkStream::RecvTimePacket()
{
	TPacketGCTime TimePacket;
	if (!Recv(sizeof(TimePacket), &TimePacket))
		return false;

	IAbstractApplication& rkApp=IAbstractApplication::GetSingleton();
	rkApp.SetServerTime(TimePacket.time);

	return true;
}

bool CPythonNetworkStream::RecvWalkModePacket()
{
	TPacketGCWalkMode WalkModePacket;
	if (!Recv(sizeof(WalkModePacket), &WalkModePacket))
		return false;

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(WalkModePacket.vid);
	if (pInstance)
	{
		if (WALKMODE_RUN == WalkModePacket.mode)
		{
			pInstance->SetRunMode();
		}
		else
		{
			pInstance->SetWalkMode();
		}
	}

	return true;
}

bool CPythonNetworkStream::RecvChangeSkillGroupPacket()
{
	TPacketGCChangeSkillGroup ChangeSkillGroup;
	if (!Recv(sizeof(ChangeSkillGroup), &ChangeSkillGroup))
		return false;

	m_dwMainActorSkillGroup = ChangeSkillGroup.skill_group;

	CPythonPlayer::Instance().NEW_ClearSkillData();
	__RefreshCharacterWindow();
	return true;
}

void CPythonNetworkStream::__TEST_SetSkillGroupFake(int iIndex)
{
	m_dwMainActorSkillGroup = DWORD(iIndex);

	CPythonPlayer::Instance().NEW_ClearSkillData();
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshCharacter", Py_BuildValue("()"));
}

#ifdef ENABLE_FEATURES_REFINE_SYSTEM
bool CPythonNetworkStream::SendRefinePacket(BYTE byPos, BYTE byType, BYTE lLow, BYTE lMedium, BYTE lExtra, BYTE lTotal)
{
	TPacketCGRefine kRefinePacket;
	kRefinePacket.header = HEADER_CG_REFINE;
	kRefinePacket.pos = byPos;
	kRefinePacket.type = byType;
#ifdef ENABLE_FEATURES_REFINE_SYSTEM
	kRefinePacket.lLow = lLow;
	kRefinePacket.lMedium = lMedium;
	kRefinePacket.lExtra = lExtra;
	kRefinePacket.lTotal = lTotal;
#endif

	if (!Send(sizeof(kRefinePacket), &kRefinePacket))
		return false;

	return SendSequence();
}
#else
bool CPythonNetworkStream::SendRefinePacket(BYTE byPos, BYTE byType)
{
	TPacketCGRefine kRefinePacket;
	kRefinePacket.header = HEADER_CG_REFINE;
	kRefinePacket.pos = byPos;
	kRefinePacket.type = byType;

	if (!Send(sizeof(kRefinePacket), &kRefinePacket))
		return false;

	return SendSequence();
}
#endif

bool CPythonNetworkStream::SendSelectItemPacket(DWORD dwItemPos)
{
	TPacketCGScriptSelectItem kScriptSelectItem;
	kScriptSelectItem.header = HEADER_CG_SCRIPT_SELECT_ITEM;
	kScriptSelectItem.selection = dwItemPos;

	if (!Send(sizeof(kScriptSelectItem), &kScriptSelectItem))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvRefineInformationPacket()
{
	TPacketGCRefineInformation kRefineInfoPacket;
	if (!Recv(sizeof(TPacketGCRefineInformation), &kRefineInfoPacket)) {
		return false;
	}

	TRefineTable& rkRefineTable = kRefineInfoPacket.refine_table;
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
		"OpenRefineDialog",
#if defined(ENABLE_NEW_GOLD_LIMIT)
		Py_BuildValue("(iiLii)",
#else
		Py_BuildValue("(iiiii)",
#endif
			kRefineInfoPacket.pos,
			kRefineInfoPacket.refine_table.result_vnum,
			rkRefineTable.cost,
			rkRefineTable.prob,
			kRefineInfoPacket.type)
		);

	for (uint8_t i = 0; i < rkRefineTable.material_count; ++i) {
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendMaterialToRefineDialog", Py_BuildValue("(ii)", rkRefineTable.materials[i].vnum, rkRefineTable.materials[i].count));
	}

#if defined(_DEBUG)
	Tracef(" >> RecvRefineInformationPacket(pos=%d, result_vnum=%d, "
#if defined(ENABLE_NEW_GOLD_LIMIT)
		"cost=%llu, "
#else
		"cost=%lu, "
#endif
		"prob=%d, type=%d)\n",
		kRefineInfoPacket.pos,
		kRefineInfoPacket.refine_table.result_vnum,
		rkRefineTable.cost,
		rkRefineTable.prob,
		kRefineInfoPacket.type);
#endif

	return true;
}

bool CPythonNetworkStream::RecvNPCList()
{
	TPacketGCNPCPosition kNPCPosition;
	if (!Recv(sizeof(kNPCPosition), &kNPCPosition))
		return false;

	assert(int(kNPCPosition.size)-sizeof(kNPCPosition) == kNPCPosition.count*sizeof(TNPCPosition) && "HEADER_GC_NPC_POSITION");

	CPythonMiniMap::Instance().ClearAtlasMarkInfo();

	for (int i = 0; i < kNPCPosition.count; ++i)
	{
		TNPCPosition NPCPosition;
		if (!Recv(sizeof(TNPCPosition), &NPCPosition))
			return false;

#ifdef ENABLE_MULTI_NAMES
		const char * szName;
		CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
		if (!rkNonPlayer.GetName(NPCPosition.name, &szName)) {
			szName = "";
		}

		CPythonMiniMap::Instance().RegisterAtlasMark(NPCPosition.bType, szName, NPCPosition.x, NPCPosition.y);
#else
		CPythonMiniMap::Instance().RegisterAtlasMark(NPCPosition.bType, NPCPosition.name, NPCPosition.x, NPCPosition.y);
#endif
	}

	return true;
}

bool CPythonNetworkStream::__SendCRCReportPacket()
{
	/*
	DWORD dwProcessCRC = 0;
	DWORD dwFileCRC = 0;
	CFilename exeFileName;
	//LPCVOID c_pvBaseAddress = NULL;

	GetExeCRC(dwProcessCRC, dwFileCRC);

	CFilename strRootPackFileName = CEterPackManager::Instance().GetRootPacketFileName();
	strRootPackFileName.ChangeDosPath();

	TPacketCGCRCReport kReportPacket;

	kReportPacket.header = HEADER_CG_CRC_REPORT;
	kReportPacket.byPackMode = CEterPackManager::Instance().GetSearchMode();
	kReportPacket.dwBinaryCRC32 = dwFileCRC;
	kReportPacket.dwProcessCRC32 = dwProcessCRC;
	kReportPacket.dwRootPackCRC32 = GetFileCRC32(strRootPackFileName.c_str());

	if (!Send(sizeof(kReportPacket), &kReportPacket))
		Tracef("SendClientReportPacket Error");

	return SendSequence();
	*/
	return true;
}

bool CPythonNetworkStream::SendClientVersionPacket()
{
	std::string filename;

	GetExcutedFileName(filename);

	filename = CFileNameHelper::NoPath(filename);
	CFileNameHelper::ChangeDosPath(filename);

	if (LocaleService_IsEUROPE() && false == LocaleService_IsYMIR())
	{
		TPacketCGClientVersion2 kVersionPacket;
		kVersionPacket.header = HEADER_CG_CLIENT_VERSION2;
		strncpy(kVersionPacket.filename, filename.c_str(), sizeof(kVersionPacket.filename)-1);
		strncpy(kVersionPacket.timestamp, "1215955205", sizeof(kVersionPacket.timestamp)-1); // # python time.time 앞자리
		//strncpy(kVersionPacket.timestamp, __TIMESTAMP__, sizeof(kVersionPacket.timestamp)-1); // old_string_ver
		//strncpy(kVersionPacket.timestamp, "1218055205", sizeof(kVersionPacket.timestamp)-1); // new_future
		//strncpy(kVersionPacket.timestamp, "1214055205", sizeof(kVersionPacket.timestamp)-1); // old_past

		if (!Send(sizeof(kVersionPacket), &kVersionPacket))
			Tracef("SendClientReportPacket Error");
	}
	else
	{
		TPacketCGClientVersion kVersionPacket;
		kVersionPacket.header = HEADER_CG_CLIENT_VERSION;
		strncpy(kVersionPacket.filename, filename.c_str(), sizeof(kVersionPacket.filename)-1);
		strncpy(kVersionPacket.timestamp, __TIMESTAMP__, sizeof(kVersionPacket.timestamp)-1);

		if (!Send(sizeof(kVersionPacket), &kVersionPacket))
			Tracef("SendClientReportPacket Error");
	}
	return SendSequence();
}

bool CPythonNetworkStream::RecvAffectAddPacket()
{
	TPacketGCAffectAdd kAffectAdd;
	if (!Recv(sizeof(kAffectAdd), &kAffectAdd))
		return false;

	TPacketAffectElement & rkElement = kAffectAdd.elem;
	if (rkElement.bPointIdxApplyOn == POINT_ENERGY)
	{
		CPythonPlayer::instance().SetStatus (POINT_ENERGY_END_TIME, CPythonApplication::Instance().GetServerTimeStamp() + rkElement.lDuration);
		__RefreshStatus();
	}
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_NEW_AddAffect", Py_BuildValue("(iiii)", rkElement.dwType, rkElement.bPointIdxApplyOn, rkElement.lApplyValue, rkElement.lDuration));

	return true;
}

bool CPythonNetworkStream::RecvAffectRemovePacket()
{
	TPacketGCAffectRemove kAffectRemove;
	if (!Recv(sizeof(kAffectRemove), &kAffectRemove))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_NEW_RemoveAffect", Py_BuildValue("(ii)", kAffectRemove.dwType, kAffectRemove.bApplyOn));

	return true;
}

bool CPythonNetworkStream::RecvChannelPacket()
{
	TPacketGCChannel kChannelPacket;
	if (!Recv(sizeof(kChannelPacket), &kChannelPacket))
		return false;

#ifdef ENABLE_INGAME_CHCHANGE
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OnChannelPacket", Py_BuildValue("(i)", kChannelPacket.channel));
#endif

	//Tracef(" >> CPythonNetworkStream::RecvChannelPacket(channel=%d)\n", kChannelPacket.channel);

	return true;
}

bool CPythonNetworkStream::RecvViewEquipPacket()
{
	TPacketViewEquip p;
	if (!Recv(sizeof(TPacketViewEquip), &p))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenEquipmentDialog",
		Py_BuildValue("(i)", p.dwVid));

	for (uint8_t i = 0; i < 23; ++i)
	{
		TEquipmentItemSet& rItemSet = p.tEquip[i];

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogItem",
			Py_BuildValue("(iiii)", p.dwVid, rItemSet.pos, rItemSet.vnum, rItemSet.count));

		for (uint8_t j = 0; j < ITEM_SOCKET_SLOT_MAX_NUM; ++j)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogSocket",
				Py_BuildValue("(iiii)", p.dwVid, rItemSet.pos, j, rItemSet.alSockets[j]));
		}

		for (uint8_t k = 0; k < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++k)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogAttr",
				Py_BuildValue("(iiiii)", p.dwVid, rItemSet.pos, k, rItemSet.aAttr[k].bType, rItemSet.aAttr[k].sValue));
		}
	}

	return true;
}

bool CPythonNetworkStream::RecvLandPacket()
{
	TPacketGCLandList kLandList;
	if (!Recv(sizeof(kLandList), &kLandList))
		return false;

	std::vector<DWORD> kVec_dwGuildID;

	CPythonMiniMap & rkMiniMap = CPythonMiniMap::Instance();
	CPythonBackground & rkBG = CPythonBackground::Instance();
	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();

	rkMiniMap.ClearGuildArea();
	rkBG.ClearGuildArea();

	int iPacketSize = (kLandList.size - sizeof(TPacketGCLandList));
	for (; iPacketSize > 0; iPacketSize-=sizeof(TLandPacketElement))
	{
		TLandPacketElement kElement;
		if (!Recv(sizeof(TLandPacketElement), &kElement))
			return false;

		rkMiniMap.RegisterGuildArea(kElement.dwID,
									kElement.dwGuildID,
									kElement.x,
									kElement.y,
									kElement.width,
									kElement.height);

		if (pMainInstance)
		if (kElement.dwGuildID == pMainInstance->GetGuildID())
		{
			rkBG.RegisterGuildArea(kElement.x,
								   kElement.y,
								   kElement.x+kElement.width,
								   kElement.y+kElement.height);
		}

		if (0 != kElement.dwGuildID)
			kVec_dwGuildID.push_back(kElement.dwGuildID);
	}
	// @fixme006
	if (kVec_dwGuildID.size()>0)
		__DownloadSymbol(kVec_dwGuildID);

	return true;
}

bool CPythonNetworkStream::RecvTargetCreatePacket()
{
	TPacketGCTargetCreate kTargetCreate;
	if (!Recv(sizeof(kTargetCreate), &kTargetCreate))
		return false;

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName);

//#ifdef _DEBUG
//	char szBuf[256+1];
//	_snprintf(szBuf, sizeof(szBuf), "타겟이 생성 되었습니다 [%d:%s]", kTargetCreate.lID, kTargetCreate.szTargetName);
//	CPythonChat::Instance().AppendChat(CHAT_TYPE_NOTICE, szBuf);
//	Tracef(" >> RecvTargetCreatePacket %d : %s\n", kTargetCreate.lID, kTargetCreate.szTargetName);
//#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenAtlasWindow", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvTargetCreatePacketNew()
{
	TPacketGCTargetCreateNew kTargetCreate;
	if (!Recv(sizeof(kTargetCreate), &kTargetCreate))
		return false;

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	CPythonBackground & rkpyBG = CPythonBackground::Instance();
	if (CREATE_TARGET_TYPE_LOCATION == kTargetCreate.byType)
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName);
	}
	else
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName, kTargetCreate.dwVID);
		rkpyBG.CreateTargetEffect(kTargetCreate.lID, kTargetCreate.dwVID);
	}

//#ifdef _DEBUG
//	char szBuf[256+1];
//	_snprintf(szBuf, sizeof(szBuf), "캐릭터 타겟이 생성 되었습니다 [%d:%s:%d]", kTargetCreate.lID, kTargetCreate.szTargetName, kTargetCreate.dwVID);
//	CPythonChat::Instance().AppendChat(CHAT_TYPE_NOTICE, szBuf);
//	Tracef(" >> RecvTargetCreatePacketNew %d : %d/%d\n", kTargetCreate.lID, kTargetCreate.byType, kTargetCreate.dwVID);
//#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenAtlasWindow", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvTargetUpdatePacket()
{
	TPacketGCTargetUpdate kTargetUpdate;
	if (!Recv(sizeof(kTargetUpdate), &kTargetUpdate))
		return false;

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.UpdateTarget(kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);

	CPythonBackground & rkpyBG = CPythonBackground::Instance();
	rkpyBG.CreateTargetEffect(kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);

//#ifdef _DEBUG
//	char szBuf[256+1];
//	_snprintf(szBuf, sizeof(szBuf), "타겟의 위치가 갱신 되었습니다 [%d:%d/%d]", kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);
//	CPythonChat::Instance().AppendChat(CHAT_TYPE_NOTICE, szBuf);
//	Tracef(" >> RecvTargetUpdatePacket %d : %d, %d\n", kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);
//#endif

	return true;
}

bool CPythonNetworkStream::RecvTargetDeletePacket()
{
	TPacketGCTargetDelete kTargetDelete;
	if (!Recv(sizeof(kTargetDelete), &kTargetDelete))
		return false;

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.DeleteTarget(kTargetDelete.lID);

	CPythonBackground & rkpyBG = CPythonBackground::Instance();
	rkpyBG.DeleteTargetEffect(kTargetDelete.lID);

//#ifdef _DEBUG
//	Tracef(" >> RecvTargetDeletePacket %d\n", kTargetDelete.lID);
//#endif

	return true;
}

bool CPythonNetworkStream::RecvLoverInfoPacket()
{
	TPacketGCLoverInfo kLoverInfo;
	if (!Recv(sizeof(kLoverInfo), &kLoverInfo))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_LoverInfo", Py_BuildValue("(si)", kLoverInfo.szName, kLoverInfo.byLovePoint));
#ifdef _DEBUG
	Tracef("RECV LOVER INFO : %s, %d\n", kLoverInfo.szName, kLoverInfo.byLovePoint);
#endif
	return true;
}

bool CPythonNetworkStream::RecvLovePointUpdatePacket()
{
	TPacketGCLovePointUpdate kLovePointUpdate;
	if (!Recv(sizeof(kLovePointUpdate), &kLovePointUpdate))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_UpdateLovePoint", Py_BuildValue("(i)", kLovePointUpdate.byLovePoint));
#ifdef _DEBUG
	Tracef("RECV LOVE POINT UPDATE : %d\n", kLovePointUpdate.byLovePoint);
#endif
	return true;
}

bool CPythonNetworkStream::RecvDigMotionPacket()
{
	TPacketGCDigMotion kDigMotion;
	if (!Recv(sizeof(kDigMotion), &kDigMotion))
		return false;

#ifdef _DEBUG
	Tracef(" Dig Motion [%d/%d]\n", kDigMotion.vid, kDigMotion.count);
#endif

	IAbstractCharacterManager& rkChrMgr=IAbstractCharacterManager::GetSingleton();
	CInstanceBase * pkInstMain = rkChrMgr.GetInstancePtr(kDigMotion.vid);
	CInstanceBase * pkInstTarget = rkChrMgr.GetInstancePtr(kDigMotion.target_vid);
	if (NULL == pkInstMain)
		return true;

	if (pkInstTarget)
		pkInstMain->NEW_LookAtDestInstance(*pkInstTarget);

	for (int i = 0; i < kDigMotion.count; ++i)
		pkInstMain->PushOnceMotion(CRaceMotionData::NAME_DIG);

	return true;
}


// 용혼석 강화
bool CPythonNetworkStream::SendDragonSoulRefinePacket(BYTE bRefineType, TItemPos* pos)
{
	TPacketCGDragonSoulRefine pk;
	pk.header = HEADER_CG_DRAGON_SOUL_REFINE;
	pk.bSubType = bRefineType;
	memcpy (pk.ItemGrid, pos, sizeof (TItemPos) * DS_REFINE_WINDOW_MAX_NUM);
	if (!Send(sizeof (pk), &pk))
	{
		return false;
	}
	return true;
}

#ifdef ENABLE_DS_REFINE_ALL
bool CPythonNetworkStream::SendDragonSoulRefineAllPacket(uint8_t subheader, uint8_t type, uint8_t grade) {
	TPacketDragonSoulRefineAll p;
	p.header = HEADER_CG_DRAGON_SOUL_REFINE_ALL;
	p.subheader = subheader;
	p.type = type;
	p.grade = grade;

	if (!Send(sizeof(p), &p)) {
		return false;
	} else {
		return true;
	}
}
#endif

#ifdef ENABLE_RANKING
bool CPythonNetworkStream::RecvRankingTable()
{
	TPacketGCRankingTable p;
	if (!Recv(sizeof(p), &p))
		return false;
	
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RankingClearData", Py_BuildValue("()"));
	for (int i = 0; i < MAX_RANKING_LIST; i++) {
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RankingAddRank", Py_BuildValue("(iiisi)", p.list[i].iPosition, p.list[i].iLevel, p.list[i].iPoints, p.list[i].szName, p.list[i].iRealPosition));
	}
	
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RankingRefresh", Py_BuildValue("()"));
	return true;
}
#endif

#ifdef ENABLE_ACCE_SYSTEM
bool CPythonNetworkStream::RecvAccePacket(bool bReturn)
{
	TPacketAcce sPacket;
	if (!Recv(sizeof(sPacket), &sPacket))
		return bReturn;

	bReturn = true;
	switch (sPacket.subheader)
	{
		case ACCE_SUBHEADER_GC_OPEN:
			CPythonAcce::Instance().Clear();
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 1, sPacket.bWindow));
			break;


		case ACCE_SUBHEADER_GC_CLOSE:
			CPythonAcce::Instance().Clear();
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 2, sPacket.bWindow));
			break;


		case ACCE_SUBHEADER_GC_ADDED:
			CPythonAcce::Instance().AddMaterial(sPacket.dwPrice, sPacket.bPos, sPacket.tPos);
			if (sPacket.bPos == 1)
			{
				CPythonAcce::Instance().AddResult(sPacket.dwItemVnum, sPacket.dwMinAbs, sPacket.dwMaxAbs);
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AlertAcce", Py_BuildValue("(b)", sPacket.bWindow));
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 3, sPacket.bWindow));
			break;



		case ACCE_SUBHEADER_GC_REMOVED:
			CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, sPacket.bPos);
			if (sPacket.bPos == 0)
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, 1);

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 4, sPacket.bWindow));
			break;



		case ACCE_SUBHEADER_CG_REFINED:
			if (sPacket.dwMaxAbs == 0)
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, 1);
			else
			{
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, 0);
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, 1);
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 4, sPacket.bWindow));
			break;



		default:
			TraceError("CPythonNetworkStream::RecvAccePacket: unknown subheader %d\n.", sPacket.subheader);
			break;
	}

	return bReturn;
}

bool CPythonNetworkStream::SendAcceClosePacket()
{
	if (!__CanActMainInstance())
		return true;

	TItemPos tPos;
	tPos.window_type	= INVENTORY;
	tPos.cell			= 0;

	TPacketAcce sPacket;
	sPacket.header		= HEADER_CG_ACCE;
	sPacket.subheader	= ACCE_SUBHEADER_CG_CLOSE;
	sPacket.dwPrice		= 0;
	sPacket.bPos		= 0;
	sPacket.tPos		= tPos;
	sPacket.dwItemVnum	= 0;
	sPacket.dwMinAbs	= 0;
	sPacket.dwMaxAbs	= 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceAddPacket(TItemPos tPos, BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketAcce sPacket;
	sPacket.header		= HEADER_CG_ACCE;
	sPacket.subheader	= ACCE_SUBHEADER_CG_ADD;
	sPacket.dwPrice		= 0;
	sPacket.bPos		= bPos;
	sPacket.tPos		= tPos;
	sPacket.dwItemVnum	= 0;
	sPacket.dwMinAbs	= 0;
	sPacket.dwMaxAbs	= 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceRemovePacket(BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header		= HEADER_CG_ACCE;
	sPacket.subheader	= ACCE_SUBHEADER_CG_REMOVE;
	sPacket.dwPrice		= 0;
	sPacket.bPos		= bPos;
	sPacket.tPos		= tPos;
	sPacket.dwItemVnum	= 0;
	sPacket.dwMinAbs	= 0;
	sPacket.dwMaxAbs	= 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceRefinePacket()
{
	if (!__CanActMainInstance())
		return true;

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header		= HEADER_CG_ACCE;
	sPacket.subheader	= ACCE_SUBHEADER_CG_REFINE;
	sPacket.dwPrice		= 0;
	sPacket.bPos		= 0;
	sPacket.tPos		= tPos;
	sPacket.dwItemVnum	= 0;
	sPacket.dwMinAbs	= 0;
	sPacket.dwMaxAbs	= 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}
#endif

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD

bool CPythonNetworkStream::CubeRenewalMakeItem(int index_item, int count_item, int index_item_improve)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGCubeRenewalSend	packet;

	packet.header		= HEADER_CG_CUBE_RENEWAL;
	packet.subheader	= CUBE_RENEWAL_SUB_HEADER_MAKE_ITEM;
	packet.index_item 	= index_item;
	packet.count_item   = count_item;
	packet.index_item_improve	= index_item_improve;

	if (!Send(sizeof(TPacketCGCubeRenewalSend), &packet))
	{
		Tracef("CPythonNetworkStream::CubeRenewalMakeItem Error\n");
		return false;
	}

	return SendSequence();
}
bool CPythonNetworkStream::CubeRenewalClose()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGCubeRenewalSend	packet;

	packet.header		= HEADER_CG_CUBE_RENEWAL;
	packet.subheader	= CUBE_RENEWAL_SUB_HEADER_CLOSE;

	if (!Send(sizeof(TPacketCGCubeRenewalSend), &packet))
	{
		Tracef("CPythonNetworkStream::CubeRenewalClose Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvCubeRenewalPacket()
{

	TPacketGCCubeRenewalReceive CubeRenewalPacket;

	if (!Recv(sizeof(CubeRenewalPacket), &CubeRenewalPacket))
		return false;

	switch (CubeRenewalPacket.subheader)
	{

		case CUBE_RENEWAL_SUB_HEADER_OPEN_RECEIVE:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_CUBE_RENEWAL_OPEN", Py_BuildValue("()"));
		}
		break;

		case CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE:
		{
			CPythonCubeRenewal::Instance().ReceiveList(CubeRenewalPacket.date_cube_renewal);
		}
		break;

		case CUBE_RENEWAL_SUB_HEADER_DATES_LOADING:
		{
			CPythonCubeRenewal::Instance().LoadingList();
		}
		break;

		case CUBE_RENEWAL_SUB_HEADER_CLEAR_DATES_RECEIVE:
		{
			CPythonCubeRenewal::Instance().ClearList();
		}
		break;
	}

	return true;
}

#endif

#ifdef ENABLE_ATLAS_BOSS
bool CPythonNetworkStream::RecvBossList()
{
	TPacketGCBossPosition packet1;
	if (!Recv(sizeof(packet1), &packet1))
		return false;

	assert(int(packet1.wSize) - sizeof(packet1) == packet1.wCount * sizeof(TBossPosition) && "HEADER_GC_BOSS_POSITION");
	CPythonMiniMap::Instance().ClearAtlasMarkInfoBoss();
	for (int i = 0; i < packet1.wCount; ++i)
	{

		TBossPosition packet2;
		if (!Recv(sizeof(TBossPosition), &packet2))
			return false;

#ifdef ENABLE_MULTI_NAMES
		const char * name;
		CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();
		if (!rkNonPlayer.GetName(packet2.szName, &name)) {
			name = "";
		}

		CPythonMiniMap::Instance().RegisterAtlasMarkBoss(CActorInstance::TYPE_NPC, name, packet2.lX, packet2.lY, packet2.lTime);
#else
		CPythonMiniMap::Instance().RegisterAtlasMarkBoss(CActorInstance::TYPE_NPC, packet2.szName, packet2.lX, packet2.lY, packet2.lTime);
#endif
	}

	return true;
}
#endif

#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
bool CPythonNetworkStream::SendWhisperAdminPacket(const char* c_szText, const char* c_szLang, int color)
{
	TPacketCGWhisperAdmin p;
	p.header = HEADER_CG_WHISPER_ADMIN;
	strncpy(p.szText, c_szText, sizeof(p.szText));
	strncpy(p.szLang, c_szLang, sizeof(p.szLang));
	p.color = color;

	if (!Send(sizeof(p), &p))
		return false;

	return SendSequence();
}
#endif

#if defined(ENABLE_TEXTS_RENEWAL)
bool CPythonNetworkStream::RecvChatPacketNew()
{
	TPacketGCChatNew p;
	if (!Recv(sizeof(p), &p))
	{
		TraceError("Cannot recv TPacketGCChatNew");
		return false;
	}

	WORD size = p.size - sizeof(p);
	char szBuf[1024 + 1] = {};

	if (size > 0)
	{
		if (!Recv(size, szBuf))
		{
			TraceError("Cannot recv TPacketGCChatNew size %d", size);
			return false;
		}
	}
	else
	{
		strcpy(szBuf, "");
	}

	szBuf[size] = '\0';

	if (p.idx == 0)
	{
		return true;
	}

	bool ret = true;

	switch (p.type)
	{
	case CHAT_TYPE_INFO:
	case CHAT_TYPE_NOTICE:
	case CHAT_TYPE_BIG_NOTICE:
#if defined(ENABLE_DICE_SYSTEM)
	case CHAT_TYPE_DICE_INFO:
#endif
#if defined(ENABLE_NEW_CHAT)
	case CHAT_TYPE_INFO_EXP:
	case CHAT_TYPE_INFO_ITEM:
	case CHAT_TYPE_INFO_VALUE:
#endif
	case CHAT_TYPE_DIALOG:
	{
		ret = false;
		break;
	}
	default:
	{
		break;
	}
	}

	if (ret == true)
	{
		return true;
	}

	SRecvChat sChat;
	sChat.type = p.type;
	sChat.idx = p.idx;
	sChat.vid = 0;
	strncpy(sChat.szBuf, szBuf, sizeof(sChat.szBuf));

	m_ChatQueue.push(sChat);

	if (m_ChatQueue.size() > 20) {
		m_ChatQueue.pop();
	}

	return true;
}
#endif

#ifdef ENABLE_NEW_FISHING_SYSTEM
bool CPythonNetworkStream::SendFishingPacketNew(int r, int i)
{
	TPacketFishingNew p;
	p.header = HEADER_CG_FISHING_NEW;
	switch (i) {
		case 0:
			{
				p.subheader = FISHING_SUBHEADER_NEW_STOP;
			}
			break;
		case 1:
			{
				p.subheader = FISHING_SUBHEADER_NEW_START;
			}
			break;
		case 2:
			{
				p.subheader = FISHING_SUBHEADER_NEW_CATCH;
			}
			break;
		case 3:
			{
				p.subheader = FISHING_SUBHEADER_NEW_CATCH_FAILED;
			}
			break;
		default:
			return false;
	}
	p.vid = 0;
	p.dir = r / 5;
	p.need = 0;
	p.count = 0;

	if (!Send(sizeof(p), &p))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvFishingNew()
{
	TPacketFishingNew p;
	if (!Recv(sizeof(p), &p))
		return false;

	CInstanceBase * pFishingInstance = CPythonCharacterManager::Instance().GetInstancePtr(p.vid);
	if (!pFishingInstance) {
		return true;
	}

	switch (p.subheader)
	{
		case FISHING_SUBHEADER_NEW_START:
			{
				if (pFishingInstance->IsFishing()) {
					if (pFishingInstance == CPythonCharacterManager::Instance().GetMainInstancePtr()) {
						SendFishingPacketNew(0, 0);
					}
					return true;
				} else {
					pFishingInstance->StartFishing(float(p.dir) * 5.0f);
					if (pFishingInstance == CPythonCharacterManager::Instance().GetMainInstancePtr()) {
						PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingStart", Py_BuildValue("(ii)", p.count, p.need));
					}
				}
			}
			break;
		case FISHING_SUBHEADER_NEW_STOP:
			{
				if (pFishingInstance->IsFishing()) {
					pFishingInstance->StopFishing();
				}

				if (pFishingInstance == CPythonCharacterManager::Instance().GetMainInstancePtr()) {
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingStop", Py_BuildValue("()"));
				}
			}
			break;
		case FISHING_SUBHEADER_NEW_CATCH:
			{
				if (pFishingInstance == CPythonCharacterManager::Instance().GetMainInstancePtr()) {
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingCatch", Py_BuildValue("(i)", p.count));
				}
			}
			break;
		case FISHING_SUBHEADER_NEW_CATCH_FAILED:
			{
				if (pFishingInstance == CPythonCharacterManager::Instance().GetMainInstancePtr()) {
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingCatchFailed", Py_BuildValue("()"));
				}
			}
			break;
		case FISHING_SUBHEADER_NEW_CATCH_FAIL:
		case FISHING_SUBHEADER_NEW_CATCH_SUCCESS:
			{
				if (pFishingInstance == CPythonCharacterManager::Instance().GetMainInstancePtr()) {
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingStop", Py_BuildValue("()"));
				}

				pFishingInstance->SetFishEmoticon();
				if (p.subheader == FISHING_SUBHEADER_NEW_CATCH_SUCCESS) {
					pFishingInstance->CatchSuccess();
				} else {
					pFishingInstance->CatchFail();
				}
			}
			break;
	}

	return true;
}
#endif
#ifdef ENABLE_PREMIUM_PLAYERS
bool CPythonNetworkStream::SendPremiumPlayersOpenRequest()
{
	TPacketCGPremiumPlayers kSendPremiumPlayers;
	kSendPremiumPlayers.bySubHeader = PREMIUM_PLAYERS_SUBHEADER_CG_OPEN;

	if (!Send(sizeof(TPacketCGPremiumPlayers), &kSendPremiumPlayers))
		return false;

	return true;
}

bool CPythonNetworkStream::SendPremiumPlayersListRequest()
{
	TPacketCGPremiumPlayers kSendPremiumPlayers;
	kSendPremiumPlayers.bySubHeader = PREMIUM_PLAYERS_SUBHEADER_CG_LIST;

	if (!Send(sizeof(TPacketCGPremiumPlayers), &kSendPremiumPlayers))
		return false;

	return true;
}

bool CPythonNetworkStream::SendPremiumPlayersActivateRequest()
{
	TPacketCGPremiumPlayers kSendPremiumPlayers;
	kSendPremiumPlayers.bySubHeader = PREMIUM_PLAYERS_SUBHEADER_CG_ACTIVATE;

	if (!Send(sizeof(TPacketCGPremiumPlayers), &kSendPremiumPlayers))
		return false;

	return true;
}

bool CPythonNetworkStream::RecvPremiumPlayersPacket()
{
	TPacketGCPremiumPlayers kRecvPremiumPlayersPacket;
	if (!Recv(sizeof(TPacketGCPremiumPlayers), &kRecvPremiumPlayersPacket))
	{
		Tracenf("RecvPremiumPlayersPacket System Packet Error");
		return false;
	}

	switch (kRecvPremiumPlayersPacket.bySubHeader)
	{
	case PREMIUM_PLAYERS_SUBHEADER_GC_OPEN:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RecvPremiumPlayersOpenPacket", Py_BuildValue("()"));
		break;
	case PREMIUM_PLAYERS_SUBHEADER_GC_LIST:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RecvPremiumPlayersListPacket", Py_BuildValue("(is)", kRecvPremiumPlayersPacket.byPos, kRecvPremiumPlayersPacket.szName));
	}
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RecvPremiumPlayersListOpenPacket", Py_BuildValue("()"));
	break;
	case PREMIUM_PLAYERS_SUBHEADER_GC_ACTIVATE:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RecvPremiumPlayersActivatePacket", Py_BuildValue("()"));
		break;
	default:
		break;
	}

	return true;
}
#endif
#ifdef INGAME_WIKI
extern PyObject* wikiModule;

bool CPythonNetworkStream::SendWikiRequestInfo(uint64_t retID, DWORD vnum, bool isMob)
{
	InGameWiki::TCGWikiPacket pack;
	pack.vnum = vnum;
	pack.is_mob = isMob;
	pack.ret_id = retID;

	if (!Send(sizeof(InGameWiki::TCGWikiPacket), &pack))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvWikiPacket()
{
	InGameWiki::TGCWikiPacket pack;
	if (!Recv(sizeof(InGameWiki::TGCWikiPacket), &pack))
		return false;

	WORD iPacketSize = pack.size - sizeof(InGameWiki::TGCWikiPacket);
	if (iPacketSize <= 0)
		return false;

	uint64_t ret_id = 0;
	DWORD data_vnum = 0;
	
	if (pack.is_data_type(InGameWiki::LOAD_WIKI_ITEM))
	{
		const size_t recv_size = sizeof(InGameWiki::TGCItemWikiPacket);
		iPacketSize -= WORD(recv_size);
		
		InGameWiki::TGCItemWikiPacket item_data;
		if (!Recv(recv_size, &item_data))
			return false;
		
		ret_id = item_data.ret_id;
		data_vnum = item_data.vnum;
		
		CItemData* pData = nullptr;
		if (!CItemManager::instance().GetItemDataPointer(item_data.vnum, &pData)) {
			TraceError("Cant get pointer from item -> %d", item_data.vnum);
			return false;
		}
		
		auto recv_wiki = item_data.wiki_info;
		auto wikiInfo = pData->GetWikiTable();
		
		const int origin_size = item_data.origin_infos_count;
		const int chest_info_count = recv_wiki.chest_info_count;
		const int refine_infos_count = recv_wiki.refine_infos_count;
		
		bool already = wikiInfo->isSet;
		
		if (!already) {
			wikiInfo->isSet = true;
			wikiInfo->hasData = true;
			wikiInfo->bIsCommon = recv_wiki.is_common;
			wikiInfo->dwOrigin = recv_wiki.origin_vnum;
			wikiInfo->maxRefineLevel = refine_infos_count;
		}
		
		{
			if (!already) {
				wikiInfo->pOriginInfo.clear();
			}
			const size_t origin_info_recv_base_size = sizeof(CommonWikiData::TWikiItemOriginInfo);
			for (int idx = 0; idx < origin_size; ++idx) {
				CommonWikiData::TWikiItemOriginInfo origin_data;
				if (!Recv(origin_info_recv_base_size, &origin_data))
					return false;

				if (!already) {
					wikiInfo->pOriginInfo.emplace_back(origin_data);
				}

				iPacketSize -= WORD(origin_info_recv_base_size);
			}
			
			if (!already) {
				wikiInfo->pChestInfo.clear();
			}
			const size_t chest_info_recv_base_size = sizeof(CommonWikiData::TWikiChestInfo);
			for (int idx = 0; idx < chest_info_count; ++idx) {
				CommonWikiData::TWikiChestInfo chest_data;
				if (!Recv(chest_info_recv_base_size, &chest_data)) {
					return false;
				}

				if (!already) {
					wikiInfo->pChestInfo.emplace_back(chest_data);
				}

				iPacketSize -= WORD(chest_info_recv_base_size);
			}
			
			if (!already) {
				wikiInfo->pRefineData.clear();
			}
			const size_t refine_info_recv_base_size = sizeof(CommonWikiData::TWikiRefineInfo);
			for (int idx = 0; idx < refine_infos_count; ++idx) {
				CommonWikiData::TWikiRefineInfo refine_info_data;
				if (!Recv(refine_info_recv_base_size, &refine_info_data))
					return false;

				if (!already) {
					wikiInfo->pRefineData.emplace_back(refine_info_data);
				}

				iPacketSize -= WORD(refine_info_recv_base_size);
			}
		}
		
		if (iPacketSize != 0)
			return false;
	}
	else
	{
		const size_t recv_size = sizeof(InGameWiki::TGCMobWikiPacket);
		iPacketSize -= WORD(recv_size);
		
		InGameWiki::TGCMobWikiPacket mob_data;
		if (!Recv(recv_size, &mob_data))
			return false;
		
		ret_id = mob_data.ret_id;
		data_vnum = mob_data.vnum;
		const int drop_info_count = mob_data.drop_info_count;
		
		CPythonNonPlayer::TWikiInfoTable* mobData = nullptr;
		if (!(mobData = CPythonNonPlayer::instance().GetWikiTable(mob_data.vnum))) {
			TraceError("Cant get mob data from monster -> %d", mob_data.vnum);
			return false;
		}
		
		bool already = mobData->isSet;
		
		if (!already) {
			mobData->isSet = (drop_info_count > 0);
		}
		
		{
			if (!already) {
				mobData->dropList.clear();
			}
			const size_t mob_drop_info_recv_base_size = sizeof(CommonWikiData::TWikiMobDropInfo);
			
			for (int idx = 0; idx < drop_info_count; ++idx) {
				CommonWikiData::TWikiMobDropInfo drop_data;
				if (!Recv(mob_drop_info_recv_base_size, &drop_data))
					return false;
				
				if (!already) {
					mobData->dropList.push_back(drop_data);
				}
				iPacketSize -= WORD(mob_drop_info_recv_base_size);
			}
		}
		
		if (iPacketSize != 0)
			return false;
	}
	
	if (wikiModule)
		PyCallClassMemberFunc(wikiModule, "BINARY_LoadInfo", Py_BuildValue("(Li)", (long long)ret_id, data_vnum));
	
	return true;
//	return SendSequence();
}
#endif

#ifdef ENABLE_OPENSHOP_PACKET
bool CPythonNetworkStream::SendOpenShopPacket(int32_t shopid) {
	if (shopid <= 0) {
		Tracen("SendOpenShopPacket Error");
		return true;
	}

	TPacketOpenShop pack;
	pack.header = HEADER_CG_OPENSHOP;
	pack.shopid = shopid;

	if (!Send(sizeof(pack), &pack)) {
		Tracen("SendOpenShopPacket Error");
		return false;
	}

	return SendSequence();
}
#endif

#if defined(ENABLE_EVENT_MANAGER)
bool CPythonNetworkStream::RecvEventManager()
{
	TPacketGCEventManager p;
	if (!Recv(sizeof(TPacketGCEventManager), &p))
	{
		return false;
	}

	uint8_t subIndex;
	if (!Recv(sizeof(uint8_t), &subIndex))
	{
		return false;
	}

	if (subIndex == EVENT_MANAGER_LOAD)
	{
		uint8_t dayCount;
		if (!Recv(sizeof(uint8_t), &dayCount))
		{
			return false;
		}

		int32_t serverTime;
		if (!Recv(sizeof(int32_t), &serverTime))
		{
			return false;
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ClearEventManager", Py_BuildValue("()"));

		const int32_t now = time(0);
		const int32_t summerTime = 0;
//		const int32_t summerTime = 60 * 60 * 2;
		const int32_t differenceTime = (serverTime - now) == 0 ? (serverTime - now) + summerTime : (serverTime - now) + summerTime;

		for (uint8_t j = 0; j < dayCount; ++j)
		{
			uint8_t dayIndex;
			if (!Recv(sizeof(uint8_t), &dayIndex))
			{
				return false;
			}

			uint8_t dayEventCount;
			if (!Recv(sizeof(uint8_t), &dayEventCount))
			{
				return false;
			}

			if (dayEventCount > 0)
			{
				std::vector<TEventManagerData> m_vec;
				m_vec.resize(dayEventCount);

				if (!Recv(dayEventCount * sizeof(TEventManagerData), &m_vec[0]))
				{
					return false;
				}

				for (const auto& eventPtr : m_vec)
				{
					const time_t startTime = ((eventPtr.startTime - serverTime) + now) + differenceTime;
					const time_t endTime = eventPtr.endTime == 0 ? 0 : ((eventPtr.endTime - serverTime) + now) + differenceTime;

					const struct tm vEventStartKey = *localtime(&startTime);
					const struct tm vEventEndKey = *localtime(&endTime);

					char startTimeText[24];
					snprintf(startTimeText, sizeof(startTimeText), "%d-%02d-%02d %02d:%02d:%02d", vEventStartKey.tm_year + 1900, vEventStartKey.tm_mon + 1, vEventStartKey.tm_mday, vEventStartKey.tm_hour, vEventStartKey.tm_min, vEventStartKey.tm_sec);

					char endTimeText[24];
					snprintf(endTimeText, sizeof(endTimeText), "%d-%02d-%02d %02d:%02d:%02d", vEventEndKey.tm_year + 1900, vEventEndKey.tm_mon + 1, vEventEndKey.tm_mday, vEventEndKey.tm_hour, vEventEndKey.tm_min, vEventEndKey.tm_sec);

					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendEvent", Py_BuildValue("(iiissiiiiiiiii)", dayIndex, eventPtr.eventID, eventPtr.eventIndex, startTimeText, endTimeText, eventPtr.empireFlag, eventPtr.channelFlag, eventPtr.value[0], eventPtr.value[1], eventPtr.value[2], eventPtr.value[3], eventPtr.startTime - serverTime, eventPtr.endTime == 0 ? 0 : eventPtr.endTime - serverTime, eventPtr.eventStatus));
				}
			}
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshEventManager", Py_BuildValue("()"));
	}
	else if (subIndex == EVENT_MANAGER_EVENT_STATUS)
	{
		uint16_t eventID;
		if (!Recv(sizeof(uint16_t), &eventID))
		{
			return false;
		}

		bool eventStatus;
		if (!Recv(sizeof(bool), &eventStatus))
		{
			return false;
		}

		int32_t endTime;
		if (!Recv(sizeof(int32_t), &endTime))
		{
			return false;
		}

		int32_t serverTime;
		if (!Recv(sizeof(int32_t), &serverTime))
		{
			return false;
		}

		const int32_t now = time(0);
		const int32_t summerTime = 60 * 60 * 2;
		const int32_t differenceTime = (serverTime - now) == 0 ? (serverTime - now) + summerTime : (serverTime - now) + summerTime;

		const time_t endTimeReal = endTime == 0 ? 0 : (endTime - serverTime) + now + differenceTime;
		const struct tm vEventEndKey = *localtime(&endTimeReal);

		char endTimeText[24];
		snprintf(endTimeText, sizeof(endTimeText), "%d-%02d-%02d %02d:%02d:%02d", vEventEndKey.tm_year + 1900, vEventEndKey.tm_mon + 1, vEventEndKey.tm_mday, vEventEndKey.tm_hour, vEventEndKey.tm_min, vEventEndKey.tm_sec);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshEventStatus", Py_BuildValue("(iiis)", eventID, eventStatus, endTime == 0 ? 0 : endTime - serverTime, endTimeText));
	}

	return true;
}
#endif

#if defined(USE_BATTLEPASS)
bool CPythonNetworkStream::SendExtBattlePassAction(BYTE bAction)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExtBattlePassAction packet;
	packet.bHeader = HEADER_CG_EXT_BATTLE_PASS_ACTION;
	packet.bAction = bAction;

	if (!Send(sizeof(TPacketCGExtBattlePassAction), &packet))
	{
		Tracef("SendExtBattlePassAction Send Packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExtBattlePassItem(int slotindex)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExtBattlePassSendPremiumItem packet;
	packet.bHeader = HEADER_CG_EXT_SEND_BP_PREMIUM_ITEM;
	packet.iSlotIndex = slotindex;

	if (!Send(sizeof(TPacketCGExtBattlePassSendPremiumItem), &packet))
	{
		Tracef("SendExtBattlePassPremiumItem Send Packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvExtBattlePassOpenPacket()
{
	SPacketGCExtBattlePassOpen packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ExtOpenBattlePass", Py_BuildValue("()"));

	return true;
}

bool CPythonNetworkStream::RecvExtBattlePassGeneralInfoPacket()
{
	TPacketGCExtBattlePassGeneralInfo packet;
	if (!Recv(sizeof(packet), &packet))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ExtBattlePassAddGeneralInfo", Py_BuildValue("(siii)", packet.szSeasonName, packet.dwBattlePassID, packet.dwBattlePassStartTime, packet.dwBattlePassEndTime));
	return true;
}

bool CPythonNetworkStream::RecvExtBattlePassMissionInfoPacket()
{
	TPacketGCExtBattlePassMissionInfo packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	packet.wSize -= sizeof(packet);

	while (packet.wSize > 0)
	{
		TExtBattlePassMissionInfo missionInfo;
		if (!Recv(sizeof(missionInfo), &missionInfo))
			return false;

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ExtBattlePassAddMission", Py_BuildValue("(iiiiii)",
			packet.dwBattlePassID, missionInfo.bMissionIndex, missionInfo.bMissionType, missionInfo.dwMissionInfo[0], missionInfo.dwMissionInfo[1], missionInfo.dwMissionInfo[2]));

		for (int i = 0; i < 3; ++i)
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ExtBattlePassAddMissionReward", Py_BuildValue("(iiiii)",
				 packet.dwBattlePassID, missionInfo.bMissionIndex, missionInfo.bMissionType, missionInfo.aRewardList[i].dwVnum, missionInfo.aRewardList[i].bCount));

		packet.wSize -= sizeof(missionInfo);
	}

    PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ExtBattlePassClearReward", Py_BuildValue("(i)", packet.dwBattlePassID));

	while (packet.wRewardSize > 0)
	{
		TExtBattlePassRewardItem rewardInfo;
		if (!Recv(sizeof(rewardInfo), &rewardInfo))
			return false;

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ExtBattlePassAddReward", Py_BuildValue("(iii)", packet.dwBattlePassID, rewardInfo.dwVnum, rewardInfo.bCount));

		packet.wRewardSize -= sizeof(rewardInfo);
	}
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ExtOpenBattlePass", Py_BuildValue("()"));

	return true;
}

bool CPythonNetworkStream::RecvExtBattlePassMissionUpdatePacket()
{
	TPacketGCExtBattlePassMissionUpdate packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ExtBattlePassUpdate", Py_BuildValue("(iii)", packet.bMissionIndex, packet.bMissionType, packet.dwNewProgress));

	return true;
}

bool CPythonNetworkStream::RecvExtBattlePassRankingPacket()
{
	TPacketGCExtBattlePassRanking packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ExtBattlePassAddRanklistEntry", Py_BuildValue("(siii)",
		packet.szPlayerName, packet.bBattlePassID, packet.dwStartTime, packet.dwEndTime
	));

	return true;
}
#endif

#if defined(ENABLE_INGAME_CHCHANGE)
bool CPythonNetworkStream::SendChangeChannel(BYTE ch) {
	TPacketCGChangeChannel kPacketCGChangeChannel;
	kPacketCGChangeChannel.header = HEADER_CG_CHANGE_CHANNEL;
	kPacketCGChangeChannel.channel = ch;

	if (!Send(sizeof(kPacketCGChangeChannel), &kPacketCGChangeChannel)) {
		return false;
	}

	return SendSequence();
}
#endif

#if defined(ENABLE_INGAME_ITEMSHOP)
bool CPythonNetworkStream::RecvItemShop()
{
	TPacketGCItemShop p;
	if (!Recv(sizeof(TPacketGCItemShop), &p))
		return false;

	BYTE subIndex;
	if (!Recv(sizeof(BYTE), &subIndex))
		return false;

	switch (subIndex)
	{
	case ITEMSHOP_DRAGONCOIN:
	{
		long long dragonCoin;
		if (!Recv(sizeof(long long), &dragonCoin))
			return false;
		bool isLogOpen;
		if (!Recv(sizeof(bool), &isLogOpen))
			return false;

		if (isLogOpen)
		{
			TIShopLogData logData;
			if (!Recv(sizeof(TIShopLogData), &logData))
				return false;
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendLog", Py_BuildValue("(sissiiL)", logData.buyDate, logData.buyTime, logData.playerName, logData.ipAdress, logData.itemVnum, logData.itemCount, logData.itemPrice));
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopSetDragonCoin", Py_BuildValue("(L)", dragonCoin));
	}
	break;
	case ITEMSHOP_LOG:
	{
		int logCount;
		if (!Recv(sizeof(int), &logCount))
			return false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopHideLoading", Py_BuildValue("()"));
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopLogClear", Py_BuildValue("()"));
		if (logCount)
		{
			std::vector<TIShopLogData> m_vec;
			m_vec.resize(logCount);
			if (!Recv(sizeof(TIShopLogData) * logCount, &m_vec[0]))
				return false;
			for (DWORD j = 0; j < m_vec.size(); ++j)
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendLog", Py_BuildValue("(sissiiL)", m_vec[j].buyDate, m_vec[j].buyTime, m_vec[j].playerName, m_vec[j].ipAdress, m_vec[j].itemVnum, m_vec[j].itemCount, m_vec[j].itemPrice));
		}
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopPurchasesWindow", Py_BuildValue("()"));
	}
	break;
	case ITEMSHOP_LOAD:
	{
		long long dragonCoin;
		if (!Recv(sizeof(long long), &dragonCoin))
			return false;
		int updateTime;
		if (!Recv(sizeof(int), &updateTime))
			return false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopSetDragonCoin", Py_BuildValue("(L)", dragonCoin));

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopHideLoading", Py_BuildValue("()"));
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopClear", Py_BuildValue("(i)", updateTime));

		int categoryTotalSize;
		if (!Recv(sizeof(int), &categoryTotalSize))
			return false;

		if (categoryTotalSize == 9999)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopOpenMainPage", Py_BuildValue("()"));
			return true;
		}

		for (DWORD j = 0; j < categoryTotalSize; ++j)
		{
			BYTE categoryIndex, categorySize;
			if (!Recv(sizeof(BYTE), &categoryIndex))
				return false;
			if (!Recv(sizeof(BYTE), &categorySize))
				return false;
			for (DWORD x = 0; x < categorySize; ++x)
			{
				BYTE categorySubIndex, categorySubSize;
				if (!Recv(sizeof(BYTE), &categorySubIndex))
					return false;
				if (!Recv(sizeof(BYTE), &categorySubSize))
					return false;

				std::vector<TIShopData> m_vec;
				if (categorySubSize)
				{
					m_vec.resize(categorySubSize);
					if (!Recv(sizeof(TIShopData) * categorySubSize, &m_vec[0]))
						return false;
					for (DWORD k = 0; k < m_vec.size(); ++k)
					{
						const TIShopData& shopData = m_vec[k];
						PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopAppendItem", Py_BuildValue("(iiiiLiiiiL)",
							categoryIndex, categorySubIndex, shopData.id, shopData.itemVnum, shopData.itemPrice, shopData.discount, shopData.offerTime, shopData.topSellingIndex, shopData.addedTime,
							shopData.sellCount));
					}
				}
			}
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ItemShopOpenMainPage", Py_BuildValue("()"));
		}
	}
	break;
	}
	return true;
}
#endif

#ifdef USE_CAPTCHA_SYSTEM
static int s_captcha_idx = 0;
bool CPythonNetworkStream::RecvCaptcha()
{
    TPacketGCCaptcha p;
    if (!Recv(sizeof(p), &p))
    {
        return false;
    }

	char filename[56];
	snprintf(filename, sizeof(filename), "%d%u", s_captcha_idx, GetCurrentProcessId());

    if (p.code == 0 &&
        p.limit == 0)
    {
        CPythonApplication::Instance().SetWindowInvisible(false);
    }
    else
    {
        if (p.code > 0 &&
            CPythonApplication::Instance().SaveImage(filename, p.code) == true)
        {
            CPythonApplication::Instance().SetWindowInvisible(true);
        }
    }

    PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
                            "BINARY_RecvCaptcha",
                            Py_BuildValue("(iis)",
                            p.code > 0 ? -1 : 0,
                            p.limit, filename));
	s_captcha_idx++;
    return true;
}

bool CPythonNetworkStream::SendCaptcha(uint16_t code)
{
    TPacketCGCaptcha p;
    p.bHeader = HEADER_CG_CAPTCHA;
    p.code = code;

    if (!Send(sizeof(p), &p))
    {
        return false;
    }

    return SendSequence();
}
#endif

#if defined(ENABLE_CLOSE_GAMECLIENT_CMD)
void CPythonNetworkStream::RecvCloseClient() {
	SetOffLinePhase();
	Disconnect();

	CPythonApplication::Instance().Exit();
}
#endif

#if defined(ENABLE_MESSENGER_BLOCK)
bool CPythonNetworkStream::SendMessengerAddBlockByVIDPacket(uint32_t vid) {
	TPacketCGMessenger p;
	p.header = HEADER_CG_MESSENGER;
	p.subheader = MESSENGER_SUBHEADER_CG_ADD_BLOCK_BY_VID;

	if (!Send(sizeof(p), &p)) {
		return false;
	}

	if (!Send(sizeof(vid), &vid)) {
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerAddBlockByNamePacket(const char* c_szName) {
	TPacketCGMessenger p;
	p.header = HEADER_CG_MESSENGER;
	p.subheader = MESSENGER_SUBHEADER_CG_ADD_BLOCK_BY_NAME;

	if (!Send(sizeof(p), &p)) {
		return false;
	}

	char szName[CHARACTER_NAME_MAX_LEN];
	strncpy(szName, c_szName, CHARACTER_NAME_MAX_LEN - 1);
	szName[CHARACTER_NAME_MAX_LEN - 1] = '\0';

	if (!Send(sizeof(szName), &szName)) {
		return false;
	}

#if defined(_DEBUG)
	Tracef("SendMessengerAddBlockByNamePacket: %s\n", c_szName);
#endif

	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerRemoveBlockPacket(const char* c_szKey, const char* c_szName) {
	TPacketCGMessenger p;
	p.header = HEADER_CG_MESSENGER;
	p.subheader = MESSENGER_SUBHEADER_CG_REMOVE_BLOCK;

	if (!Send(sizeof(p), &p)) {
		return false;
	}

	char szKey[CHARACTER_NAME_MAX_LEN];
	strncpy(szKey, c_szKey, CHARACTER_NAME_MAX_LEN - 1);
	if (!Send(sizeof(szKey), &szKey)) {
		return false;
	}

	__RefreshTargetBoardByName(c_szName);
	return SendSequence();
}
#endif

#if defined(ENABLE_LETTERS_EVENT)
bool CPythonNetworkStream::SendTakeLettersEventReward() {
	TPacketEmpty p;
	p.bHeader = HEADER_CG_TAKE_LETTERS_REWARD;

	if (!Send(sizeof(p), &p)) {
		return false;
	}

	return SendSequence();
}
#endif

#if defined(ENABLE_AURA_SYSTEM)
bool CPythonNetworkStream::RecvAuraPacket() {
	TPacketGCAura kAuraPacket;

	if (!Recv(sizeof(TPacketGCAura), &kAuraPacket)) {
		Tracef("Aura Packet Error SubHeader %u\n", kAuraPacket.bSubHeader);
		return false;
	}

	int32_t iPacketSize = int32_t(kAuraPacket.wSize) - sizeof(TPacketGCAura);

	switch (kAuraPacket.bSubHeader)
	{
		case AURA_SUBHEADER_GC_OPEN:
		case AURA_SUBHEADER_GC_CLOSE: {
			if (iPacketSize > 0) {
				TSubPacketGCAuraOpenClose kSubPacket;
				assert(iPacketSize % sizeof(TSubPacketGCAuraOpenClose) == 0 && "AURA_SUBHEADER_GC_OPENCLOSE");
				if (!Recv(sizeof(TSubPacketGCAuraOpenClose), &kSubPacket)) {
					return false;
				}

				if (kAuraPacket.bSubHeader == AURA_SUBHEADER_GC_OPEN) {
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AuraWindowOpen", Py_BuildValue("(i)", kSubPacket.bAuraWindowType));
				}
				else if (kAuraPacket.bSubHeader == AURA_SUBHEADER_GC_CLOSE) {
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AuraWindowClose", Py_BuildValue("()"));
					CPythonPlayer::instance().DelAuraItemData(AURA_SLOT_MAIN);
					CPythonPlayer::instance().DelAuraItemData(AURA_SLOT_SUB);
					CPythonPlayer::instance().DelAuraItemData(AURA_SLOT_RESULT);
				}

				CPythonPlayer::instance().SetAuraRefineWindowOpen(kSubPacket.bAuraWindowType);
			}
			else {
				TraceError(" RecvAuraPacket Error 0x04100%u0F", kAuraPacket.bSubHeader);
			}

			break;
		}
		case AURA_SUBHEADER_GC_SET_ITEM: {
			if (iPacketSize > 0) {
				TSubPacketGCAuraSetItem kSubPacket;
				assert(iPacketSize % sizeof(TSubPacketGCAuraSetItem) == 0 && "AURA_SUBHEADER_GC_SET_ITEM");
				if (!Recv(sizeof(TSubPacketGCAuraSetItem), &kSubPacket)) {
					return false;
				}

				TItemData kItemData;
				kItemData.vnum = kSubPacket.pItem.vnum;
				kItemData.count = kSubPacket.pItem.count;
				for (int iSocket = 0; iSocket < ITEM_SOCKET_SLOT_MAX_NUM; ++iSocket) {
					kItemData.alSockets[iSocket] = kSubPacket.pItem.alSockets[iSocket];
				}

				for (int iAttr = 0; iAttr < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++iAttr) {
					kItemData.aAttr[iAttr] = kSubPacket.pItem.aAttr[iAttr];
				}

				if (kSubPacket.Cell.IsValidCell() && !kSubPacket.Cell.IsEquipCell()) {
					CPythonPlayer::instance().SetActivatedAuraSlot(BYTE(kSubPacket.AuraCell.cell), kSubPacket.Cell);
				}

				CPythonPlayer::instance().SetAuraItemData(BYTE(kSubPacket.AuraCell.cell), kItemData);
			} else {
				TraceError(" RecvAuraPacket Error 0x040%uBABE", kAuraPacket.bSubHeader);
			}

			break;
		}
		case AURA_SUBHEADER_GC_CLEAR_SLOT: {
			if (iPacketSize > 0) {
				TSubPacketGCAuraClearSlot kSubPacket;
				assert(iPacketSize % sizeof(TSubPacketGCAuraClearSlot) == 0 && "AURA_SUBHEADER_GC_CLEAR_SLOT");
				if (!Recv(sizeof(TSubPacketGCAuraClearSlot), &kSubPacket)) {
					return false;
				}

				CPythonPlayer::instance().DelAuraItemData(kSubPacket.bAuraSlotPos);
			} else {
				TraceError(" RecvAuraPacket Error 0x04FF0%uAA", kAuraPacket.bSubHeader);
			}

			break;
		}
		case AURA_SUBHEADER_GC_CLEAR_ALL: {
			CPythonPlayer::instance().DelAuraItemData(AURA_SLOT_MAIN);
			CPythonPlayer::instance().DelAuraItemData(AURA_SLOT_SUB);
			CPythonPlayer::instance().DelAuraItemData(AURA_SLOT_RESULT);
			break;
		}
		case AURA_SUBHEADER_GC_CLEAR_RIGHT: {
			if (iPacketSize == 0) {
				CPythonPlayer::instance().DelAuraItemData(AURA_SLOT_SUB);
			} else {
				TraceError("invalid packet size %d", iPacketSize);
			}

			break;
		}
		case AURA_SUBHEADER_GC_REFINE_INFO: {
			if (iPacketSize > 0) {
				for (auto i = 0; iPacketSize > 0; ++i) {
					assert(iPacketSize % sizeof(TSubPacketGCAuraRefineInfo) == 0 && "AURA_SUBHEADER_GC_REFINE_INFO");
					TSubPacketGCAuraRefineInfo kSubPacket;
					if (!Recv(sizeof(TSubPacketGCAuraRefineInfo), &kSubPacket)) {
						return false;
					}

					CPythonPlayer::instance().SetAuraRefineInfo(kSubPacket.bAuraRefineInfoType, kSubPacket.bAuraRefineInfoLevel, kSubPacket.bAuraRefineInfoExpPercent);
					iPacketSize -= sizeof(TSubPacketGCAuraRefineInfo);
				}
			}
			else {
				TraceError(" RecvAuraPacket Error 0x04000%FF", kAuraPacket.bSubHeader);
			}

			break;
		}
		default: {
			break;
		}
	}

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::SendAuraRefineCheckIn(TItemPos InventoryCell, TItemPos AuraCell, uint8_t byAuraRefineWindowType) {
	__PlayInventoryItemDropSound(InventoryCell);

	TPacketCGAura kAuraPacket;
	kAuraPacket.wSize = sizeof(TPacketCGAura) + sizeof(TSubPacketCGAuraRefineCheckIn);
	kAuraPacket.bSubHeader = AURA_SUBHEADER_CG_REFINE_CHECKIN;

	TSubPacketCGAuraRefineCheckIn kAuraSubPacket;
	kAuraSubPacket.ItemCell = InventoryCell;
	kAuraSubPacket.AuraCell = AuraCell;
	kAuraSubPacket.byAuraRefineWindowType = byAuraRefineWindowType;

	if (!Send(sizeof(TPacketCGAura), &kAuraPacket)) {
		return false;
	}

	if (!Send(sizeof(TSubPacketCGAuraRefineCheckIn), &kAuraSubPacket)) {
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAuraRefineCheckOut(TItemPos AuraCell, uint8_t byAuraRefineWindowType) {
	TPacketCGAura kAuraPacket;
	kAuraPacket.wSize = sizeof(TPacketCGAura) + sizeof(TSubPacketCGAuraRefineCheckOut);
	kAuraPacket.bSubHeader = AURA_SUBHEADER_CG_REFINE_CHECKOUT;

	TSubPacketCGAuraRefineCheckOut kAuraSubPacket;
	kAuraSubPacket.AuraCell = AuraCell;
	kAuraSubPacket.byAuraRefineWindowType = byAuraRefineWindowType;

	if (!Send(sizeof(TPacketCGAura), &kAuraPacket)) {
		return false;
	}

	if (!Send(sizeof(TSubPacketCGAuraRefineCheckOut), &kAuraSubPacket)) {
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAuraRefineAccept(uint8_t byAuraRefineWindowType) {
	TPacketCGAura kAuraPacket;
	kAuraPacket.wSize = sizeof(TPacketCGAura) + sizeof(TSubPacketCGAuraRefineAccept);
	kAuraPacket.bSubHeader = AURA_SUBHEADER_CG_REFINE_ACCEPT;

	TSubPacketCGAuraRefineAccept kAuraSubPacket;
	kAuraSubPacket.byAuraRefineWindowType = byAuraRefineWindowType;

	if (!Send(sizeof(TPacketCGAura), &kAuraPacket)) {
		return false;
	}

	if (!Send(sizeof(TSubPacketCGAuraRefineAccept), &kAuraSubPacket)) {
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAuraRefineCancel() {
	TPacketCGAura kAuraPacket;
	kAuraPacket.wSize = sizeof(TPacketCGAura);
	kAuraPacket.bSubHeader = AURA_SUBHEADER_CG_REFINE_CANCEL;

	if (!Send(sizeof(TPacketCGAura), &kAuraPacket)) {
		return false;
	}

	return SendSequence();
}
#endif

#if defined(ENABLE_CHANGELOOK)
bool CPythonNetworkStream::RecvChangeLookPacket() {
	TPacketChangeLook p;
	if (!Recv(sizeof(TPacketChangeLook), &p)) {
		return false;
	}

	switch (p.bSubHeader) {
		case SUBHEADER_CHANGE_LOOK_OPEN: {
			CPythonChangeLook::Instance().Clear();
			CPythonChangeLook::Instance().SetCost(p.dwCost);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActChangeLook", Py_BuildValue("(i)", 1));

			break;
		}
		case SUBHEADER_CHANGE_LOOK_CLOSE: {
			CPythonChangeLook::Instance().Clear();
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActChangeLook", Py_BuildValue("(i)", 2));

			break;
		}
		case SUBHEADER_CHANGE_LOOK_ADD: {
			CPythonChangeLook::Instance().AddMaterial(p.bPos, p.tPos);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActChangeLook", Py_BuildValue("(i)", 3));
			if (p.bPos == 1) {
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AlertChangeLook", Py_BuildValue("()"));
			}

			break;
		}
		case SUBHEADER_CHANGE_LOOK_REMOVE: {
			if (p.bPos == 1 || p.bPos == 2) {
				CPythonChangeLook::Instance().RemoveMaterial(p.bPos);
			} else {
				CPythonChangeLook::Instance().RemoveAllMaterials();
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActChangeLook", Py_BuildValue("(i)", 4));

			break;
		}
		case SUBHEADER_CHANGE_LOOK_REFINE: {
			CPythonChangeLook::Instance().RemoveAllMaterials();
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActChangeLook", Py_BuildValue("(i)", 4));

			break;
		}
		default: {
			TraceError("CPythonNetworkStream::RecvChangeLookPacket: unknown subheader %d\n.", p.bSubHeader);
			break;
		}
	}

	return true;
}

bool CPythonNetworkStream::SendChangeLookClosePacket() {
	if (!__CanActMainInstance()) {
		return true;
	}

	TPacketChangeLook p;

	p.bHeader = HEADER_CG_CHANGE_LOOK;
	p.bSubHeader = SUBHEADER_CHANGE_LOOK_CLOSE;
	p.dwCost = 0;
	p.bPos = 0;
	p.tPos = TItemPos(INVENTORY, 0);

	if (!Send(sizeof(TPacketChangeLook), &p)) {
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendChangeLookAddPacket(TItemPos tPos, uint8_t bPos) {
	if (!__CanActMainInstance()) {
		return true;
	}

	TPacketChangeLook p;

	p.bHeader = HEADER_CG_CHANGE_LOOK;
	p.bSubHeader = SUBHEADER_CHANGE_LOOK_ADD;
	p.dwCost = 0;
	p.bPos = bPos;
	p.tPos = tPos;

	if (!Send(sizeof(TPacketChangeLook), &p)) {
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendChangeLookRemovePacket(uint8_t bPos) {
	if (!__CanActMainInstance()) {
		return true;
	}

	TPacketChangeLook p;

	p.bHeader = HEADER_CG_CHANGE_LOOK;
	p.bSubHeader = SUBHEADER_CHANGE_LOOK_REMOVE;
	p.dwCost = 0;
	p.bPos = bPos;
	p.tPos = TItemPos(INVENTORY, 0);

	if (!Send(sizeof(TPacketChangeLook), &p)) {
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendChangeLookRefinePacket() {
	if (!__CanActMainInstance()) {
		return true;
	}

	TPacketChangeLook p;

	p.bHeader = HEADER_CG_CHANGE_LOOK;
	p.bSubHeader = SUBHEADER_CHANGE_LOOK_REFINE;
	p.dwCost = 0;
	p.bPos = 0;
	p.tPos = TItemPos(INVENTORY, 0);

	if (!Send(sizeof(TPacketChangeLook), &p))
		return false;

	return SendSequence();
}
#endif

#if defined(ENABLE_NEW_ANTICHEAT_RULES)
void CPythonNetworkStream::RecvAntiCheat() {
	TPacketGCAntiCheat p;

	if (Recv(sizeof(TPacketGCAntiCheat), &p)) {
		uint8_t reason = p.bReason;

		SetOffLinePhase();
		Disconnect();

		switch (reason) {
			case ANTICHEAT_TYPE_ATTACKSPEED:
			case ANTICHEAT_TYPE_RANGE: {
				MessageBoxA(NULL, "A cheat was detected, this information was saved!", "Keynes2", MB_ICONSTOP);
				break;
			}
			default: {
				MessageBoxA(NULL, "Unknown error!", "Keynes2", MB_ICONSTOP);
				break;
			}
		}

		exit(0);
	} else {
		SetOffLinePhase();
		Disconnect();

		CPythonApplication::Instance().Exit();
	}
}
#endif

#if defined(ENABLE_NEW_GOLD_LIMIT)
bool CPythonNetworkStream::RecvGoldChange()
{
	TPacketGCGoldChange GoldChange;
	if (!Recv(sizeof(TPacketGCGoldChange), &GoldChange))
	{
		Tracen("Recv Gold Change Packet Error");
		return false;
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
	if (pInstance)
	{
		if (GoldChange.dwVID == pInstance->GetVirtualID())
		{
			CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
			rkPlayer.SetGold(GoldChange.value);
		}
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshStatus", Py_BuildValue("()"));
	return true;
}
#endif

#if defined(USE_ATTR_6TH_7TH)
bool CPythonNetworkStream::SendAttr67AddPacket(const TAttr67AddData* pkAttr67AddData, bool bAddPacket)
{
	TPacketCGAttr67Add packet;
	packet.byHeader = HEADER_CG_ATTR67_ADD;
	if (bAddPacket)
	{
		packet.bySubHeader = SUBHEADER_CG_ATTR67_ADD_REGIST;
		memcpy(&packet.Attr67AddData, pkAttr67AddData, sizeof(TAttr67AddData));
	}
	else
	{
		packet.bySubHeader = SUBHEADER_CG_ATTR67_ADD_OPEN;
		memset(&packet.Attr67AddData, 0, sizeof(TAttr67AddData));
	}

	if (!Send(sizeof(packet), &packet))
	{
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAttr67ClosePacket()
{
	TPacketCGAttr67Add packet;
	packet.byHeader = HEADER_CG_ATTR67_ADD;
	packet.bySubHeader = SUBHEADER_CG_ATTR67_ADD_CLOSE;
	memset(&packet.Attr67AddData, 0, sizeof(TAttr67AddData));

	if (!Send(sizeof(packet), &packet))
	{
		return false;
	}

	return SendSequence();
}
#endif

bool CPythonNetworkStream::SendReqMapBossInfo()
{
	BYTE bHeader = HEADER_CG_REQ_MAP_BOSS_INFO;
	if (!Send(sizeof(bHeader), &bHeader))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvBossPositionPacket() 
{
	SPacketGCBossPosition packet{};
	if (!Recv(sizeof(packet), &packet))
		return false;

	packet.size -= sizeof(SPacketGCBossPosition);

	CPythonMiniMap::Instance().ClearBossMarkInfo();

	if (packet.size > 0) {
		if (const auto count = packet.size / sizeof(SBossPosition); count > 0) {
			std::vector<SBossPosition> bossPosition(count);
			if (!Recv(packet.size, bossPosition.data()))
				return false;

			auto& monster = CPythonNonPlayer::Instance();
			for (const auto& v : bossPosition) {
				if (!monster.GetTable(v.vnum))
					continue;

				CPythonMiniMap::Instance().RegisterBossAtlasInfo(v.vnum, v.x, v.y, v.time, v.real_time, v.is_alive);
			}
		}
	}
	return true;
}

bool CPythonNetworkStream::SendWhisperInfo(const char* name)
{
	SPacketCGWhisperInfo packet;
	packet.header = HEADER_CG_WHISPER_INFO;
	strncpy(packet.name, name, CHARACTER_NAME_MAX_LEN);

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvWhisperInfo()
{
	SPacketGCWhisperInfo packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisperInfo", Py_BuildValue("(ssi)", packet.name, packet.country_flag, packet.empire));

	return true;
}