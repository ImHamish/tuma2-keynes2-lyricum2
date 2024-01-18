#include "stdafx.h"

#ifdef ENABLE_NEW_FISHING_SYSTEM
#include "utils.h"
#include "char.h"
#include "char_manager.h"
#include "config.h"
#include "desc.h"
#include "item.h"
#include "item_manager.h"
#include "unique_item.h"
#include "fishing.h"
#include "vector.h"
#include "packet.h"
#include "sectree.h"
#include "sectree_manager.h"

EVENTFUNC(fishing_event) {
	fishingnew_event_info * info = dynamic_cast<fishingnew_event_info *>(event->info);
	if (info == NULL) {
		return 0;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(info->pid);
	if (!ch) {
		return 0;
	}

	if (ch->GetFishCatch() >= FISHING_NEED_CATCH){
		ch->fishing_catch_decision(info->vnum);
		return 0;
	} else {
		LPITEM rod = ch->GetWear(WEAR_WEAPON);
		if (!(rod && rod->GetType() == ITEM_ROD)) {
			ch->fishing_new_stop();
			return 0;
		}

		if (info->sec == 1) {
			if (ch->FindAffect(AFFECT_FISH_MIND_PILL) || ch->GetPremiumRemainSeconds(PREMIUM_FISH_MIND) > 0 || ch->IsEquipUniqueGroup(UNIQUE_GROUP_FISH_MIND)) {
				TItemTable* pTable = ITEM_MANAGER::instance().GetTable(info->vnum);
				if (pTable) {
#if defined(ENABLE_TEXTS_RENEWAL)
#ifdef ENABLE_MULTI_NAMES
				BYTE lang = 0;
				LPDESC d = ch->GetDesc();
				if (d) {
					lang = d->GetLanguage();
				}
#endif
				ch->ChatPacketNew(CHAT_TYPE_INFO, 896, "%s", 
#ifdef ENABLE_MULTI_NAMES
				pTable->szLocaleName[lang]
#else
				pTable->szLocaleName
#endif
				);
#endif
				} else {
#if defined(ENABLE_TEXTS_RENEWAL)
				ch->ChatPacketNew(CHAT_TYPE_INFO, 897, "");
#endif
				}
			} else {
#if defined(ENABLE_TEXTS_RENEWAL)
				ch->ChatPacketNew(CHAT_TYPE_INFO, 897, "");
#endif
			}
		}

		DWORD failed = ch->GetFishCatchFailed();
		if (failed > 0) {
			info->sec += failed;
			ch->SetFishCatchFailed(0);
		}

		if (info->sec >= 15) {
			ch->fishing_new_stop();
			return 0;
		}

		++info->sec;
		return (PASSES_PER_SEC(1));
	}
};

void CHARACTER::fishing_new_start() {
	if (m_pkFishingNewEvent) {
		return;
	}

	LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());
	if (!pkSectreeMap) {
		return;
	}

	int x = GetX(), y = GetY();
	LPSECTREE tree = pkSectreeMap->Find(x, y);
	if (!tree) {
		return;
	}

	if (tree->IsAttr(x, y, ATTR_BLOCK)) {
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 894, "");
#endif
		return;
	}

	if (GetEmptyInventory(1) == -1) {
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 899, "");
#endif
		return;
	}

	LPITEM rod = GetWear(WEAR_WEAPON);
	if (!rod || rod->GetType() != ITEM_ROD) {
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 895, "");
#endif
		return;
	}

	if (rod->GetSocket(2) == 0) {
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 281, "");
#endif
		return;
	}

	float fx, fy;
	GetDeltaByDegree(GetRotation(), 400.0f, &fx, &fy);

	SetFishCatch(0);
	SetFishCatchFailed(0);
	SetLastCatchTime(0);

	DWORD dwVnum = rod->GetVnum();
	bool second = dwVnum >= 27400 && dwVnum <= 27490 ? false : true;

	fishingnew_event_info* info = AllocEventInfo<fishingnew_event_info>();
	info->pid = GetPlayerID();
	info->vnum = fishingnew::GetFishCatchedVnum(100, 15 + GetPoint(POINT_FISHING_RARE) + rod->GetSocket(2), second);
	info->chance = 0;
	info->sec = 1;
	m_pkFishingNewEvent = event_create(fishing_event, info, PASSES_PER_SEC(1));

	TPacketFishingNew p;
	p.header = HEADER_GC_FISHING_NEW;
	p.subheader = FISHING_SUBHEADER_NEW_START;
	p.vid = GetVID();
	p.dir = (BYTE)(GetRotation() / 5);
	p.need = FISHING_NEED_CATCH;
	p.count = 0;
	PacketAround(&p, sizeof(p));
}

void CHARACTER::fishing_new_stop() {
	if (!m_pkFishingNewEvent) {
		return;
	}

	event_cancel(&m_pkFishingNewEvent);
	m_pkFishingNewEvent = NULL;

	LPITEM rod = GetWear(WEAR_WEAPON);
	if (rod && rod->GetType() == ITEM_ROD) {
		rod->SetSocket(2, 0);
	}

	TPacketFishingNew p;
	p.header = HEADER_GC_FISHING_NEW;
	p.subheader = FISHING_SUBHEADER_NEW_STOP;
	p.vid = GetVID();
	p.dir = 0;
	p.need = 0;
	p.count = 0;
	PacketAround(&p, sizeof(p));
}

void CHARACTER::fishing_new_catch() {
	if (!m_pkFishingNewEvent) {
		return;
	}

	if (GetLastCatchTime() > get_global_time()) {
		return;
	}

	BYTE v = GetFishCatch() + 1;
	SetLastCatchTime(get_global_time() + 1);
	SetFishCatch(v);

	TPacketFishingNew p;
	p.header = HEADER_GC_FISHING_NEW;
	p.subheader = FISHING_SUBHEADER_NEW_CATCH;
	p.vid = GetVID();
	p.dir = 0;
	p.need = 0;
	p.count = v;
	PacketAround(&p, sizeof(p));
}

void CHARACTER::fishing_new_catch_failed() {
	if (!m_pkFishingNewEvent) {
		return;
	}

	SetFishCatchFailed(GetFishCatchFailed() + 1);

	TPacketFishingNew p;
	p.header = HEADER_GC_FISHING_NEW;
	p.subheader = FISHING_SUBHEADER_NEW_CATCH_FAILED;
	p.vid = GetVID();
	p.dir = 0;
	p.need = 0;
	p.count = 0;
	PacketAround(&p, sizeof(p));
}

void CHARACTER::fishing_catch_decision(DWORD itemVnum) {
	if (!m_pkFishingNewEvent) {
		return;
	}

	event_cancel(&m_pkFishingNewEvent);
	m_pkFishingNewEvent = NULL;

	LPITEM rod = GetWear(WEAR_WEAPON);
	if (!rod)
	{
		return;
	}

	if (rod->GetType() == ITEM_ROD)
	{
		if (rod->GetRefinedVnum()> 0 && rod->GetSocket(0) < rod->GetValue(2) && number(1, rod->GetValue(1)) == 1)
		{
			rod->SetSocket(0, rod->GetSocket(0) + 1);
#if defined(ENABLE_TEXTS_RENEWAL)
			ChatPacketNew(CHAT_TYPE_INFO, 283, "%d#%d", rod->GetSocket(0), rod->GetValue(2));
#endif
			if (rod->GetSocket(0) == rod->GetValue(2))
			{
#if defined(ENABLE_TEXTS_RENEWAL)
				ChatPacketNew(CHAT_TYPE_INFO, 279, "");
				ChatPacketNew(CHAT_TYPE_INFO, 280, "");
#endif
			}
		}

		rod->SetSocket(2, 0);
	}

	BYTE chance;
	switch (itemVnum) {
		case 27803:
		case 27806:
		case 27816:
		case 27807:
		case 27818:
		case 27805:
		case 27822:
		case 27823:
		case 27824:
		case 27825:
			{
				chance = 20;
			}
			break;
		case 27804:
		case 27811:
		case 27810:
		case 27809:
		case 27814:
		case 27812:
		case 27808:
		case 27826:
		case 27827:
		case 27813:
		case 27815:
		case 27819:
		case 27820:
		case 27821:
			{
				chance = 5;
			}
			break;
		default:
			{
				chance = 0;
			}
			break;
	}

	if (GetPoint(POINT_FISHING_RARE) > 0 && chance == 5) {
		chance += 20;
	}

	DWORD dwVnum = rod->GetVnum();
	if (dwVnum >= 27400 && dwVnum <= 27490) {
		chance += (rod->GetValue(0) / 10) * 2;
	} else {
		chance += rod->GetValue(0) / 10;
	}

	TPacketFishingNew p;
	p.header = HEADER_GC_FISHING_NEW;
	if (number(1, 100) >= chance) {
		p.subheader = FISHING_SUBHEADER_NEW_CATCH_FAIL;
	} else {
#ifdef ENABLE_RANKING
		SetRankPoints(14, GetRankPoints(14) + 1);
#endif
#if defined(USE_BATTLEPASS)
		UpdateExtBattlePassMissionProgress(FISH_FISHING, 1, itemVnum);
#endif
		p.subheader = FISHING_SUBHEADER_NEW_CATCH_SUCCESS;
		AutoGiveItem(itemVnum, 1, -1, false);

#if defined(ENABLE_NEW_FISH_EVENT)
#if defined(ENABLE_EVENT_MANAGER)
		if (CHARACTER_MANAGER::Instance().CheckEventIsActive(JIGSAW_EVENT) != NULL) {
#else
		if (quest::CQuestManager::instance().GetEventFlag("enable_fish_event")) {
#endif
			if (GetPoint(POINT_FISHING_RARE) > 0) {
				uint8_t r = number(1, 100);
				AutoGiveItem(r <= 10 ? ITEM_FISH_EVENT_BOX_SPECIAL : ITEM_FISH_EVENT_BOX, 1, -1, false);
			} else {
				AutoGiveItem(ITEM_FISH_EVENT_BOX, 5, -1, false);
			}
		}
#endif
	}

	p.vid = GetVID();
	p.dir = 0;
	p.need = 0;
	p.count = 0;
	PacketAround(&p, sizeof(p));
}
#endif
