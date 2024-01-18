#include "stdafx.h"
#include "switchbonus.h"
#include "char.h"
#include "locale_service.h"
#include "packet.h"
#include "desc_client.h"
#include "item_manager.h"
#include "item.h"
#include "item_manager.h"
#include "locale_service.h"
#include "config.h"
#include "questmanager.h"

CSwitchBonus::CSwitchBonus()
{
}

CSwitchBonus::~CSwitchBonus()
{
	
}

bool CSwitchBonus::CheckElementsSB(LPCHARACTER ch){
	if (ch->GetExchange() || ch->GetShop() || ch->GetMyShop() || ch->IsOpenSafebox() || ch->IsCubeOpen()
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
	) {
		return true;
	}

	return false;
}

bool CSwitchBonus::CanMoveIntoSB(LPITEM item)
{
	if (!item) {
		return false;
	}

	if(item->GetType()==ITEM_WEAPON && item->GetSubType() != WEAPON_ARROW){return true;}
	if(item->GetType()==ITEM_ARMOR){return true;}

	if(item->GetType() == ITEM_COSTUME)
	{
		if (item->GetSubType() == COSTUME_BODY || item->GetSubType() == COSTUME_HAIR || item->GetSubType() == COSTUME_WEAPON)
			return true;
	}
	
	return false;
}

void CSwitchBonus::OpenSB(LPCHARACTER ch)
{
	if (ch->IsSBOpen()){
		return;
	}

	ch->SetSBOpen(true);
	SendSBPackets(ch,SWITCH_RECEIVE_OPEN);

	LPITEM item = ch->GetSwitchBotItem(0);
	if(item){
		SendSBItem(ch,item);
	}
}

void CSwitchBonus::CloseSB(LPCHARACTER ch)
{
	ch->SetSBOpen(false);
}

void CSwitchBonus::ClearSB(LPCHARACTER ch)
{
	SendSBPackets(ch,SWITCH_RECEIVE_CLEAR_INFO);
}

void CSwitchBonus::SendSBItem(LPCHARACTER ch,LPITEM item)
{
	if(!item)
		return;
	
	ClearSB(ch);
	int iAttributeSet = item->GetAttributeSetIndex();
	for (int b = 0; b < MAX_APPLY_NUM; ++b)	
	{
		const TItemAttrTable & r = g_map_itemAttr[b];
		if (r.bMaxLevelBySet[iAttributeSet]){
			SendSBPackets(ch,SWITCH_RECEIVE_INFO_BONUS_ITEM, b, r.lValues[0], r.lValues[1], r.lValues[2], r.lValues[3], r.lValues[4]);
		}
	}

	TItemTable const * pProto = item->GetProto();
	if(pProto && pProto->sAddonType)
	{
		SendSBPackets(ch,SWITCH_RECEIVE_INFO_EXTRA);
	}
	
}

bool CSwitchBonus::IsItemZodiac(DWORD vnum)
{
	static const DWORD ITEMS_ZODIAC[] = {
	300,301,302,303,304,305,306,307,308,309,329,339,349,359,
	310,311,312,313,314,315,316,317,318,319,369,379,389,399,
	1180,1181,1182,1183,1184,1185,1186,1187,1188,1189,1199,1209,1219,1229,
	2200,2201,2202,2203,2204,2205,2206,2207,2208,2209,2219,2229,2239,2249,
	3220,3221,3222,3223,3224,3225,3226,3227,3228,3229,3239,3249,3259,3269,
	5160,5161,5162,5163,5164,5165,5166,5167,5168,5169,5179,5189,5199,5209,
	7300,7301,7302,7303,7304,7305,7306,7307,7308,7309,7319,7329,7339,7349,
	19290,19291,19292,19293,19294,19295,19296,19297,19298,19299,19309,19310,19311,19312,
	19490,19491,19492,19493,19494,19495,19496,19497,19498,19499,19509,19510,19511,19512,
	19690,19691,19692,19693,19694,19695,19696,19697,19698,19699,19709,19710,19711,19712,
	19890,19891,19892,19893,19894,19895,19896,19897,19898,19899,19909,19910,19911,19912,
	};
	
	for (int a = 0; a <sizeof(ITEMS_ZODIAC)/sizeof(*ITEMS_ZODIAC); ++a)
	{
		DWORD vnum_get = ITEMS_ZODIAC[a];
			
		if (vnum_get == vnum){
			return true;
		}
	}
	
	return false;
}


bool CSwitchBonus::CheckVnumsChange(LPITEM item, int vnum_change)
{
	if (!item)
		return false;

	const int vnum_item_list[8] =
	{
		SWITCHER_NORMAL_VNUM,
		SWITCHER_LIMIT_VNUM,
		SWITCHER_LIMIT2_VNUM,
		SWITCHER_COSTUME_VNUM,
		SWITCHER_COSTUME2_VNUM,
		SWITCHER_PENDANT_VNUM,
		SWITCHER_PENDANT2_VNUM,
		SWITCHER_ZODIAC_VNUM,
	};

	bool check = false;

	for (int a = 0; a < 8; ++a)
	{
		if (vnum_item_list[a] == vnum_change)
			check = true;
	}

	switch (vnum_change)
	{
	case SWITCHER_NORMAL_VNUM:
	{
		if (item->GetType() == ITEM_COSTUME)
			check = false;
	}
	break;

	case SWITCHER_LIMIT_VNUM:
	case SWITCHER_LIMIT2_VNUM:
	{
		if (!item->CheckItemUseLevel(40) || item->GetType() == ITEM_COSTUME)
			check = false;
	}
	break;

	case SWITCHER_COSTUME_VNUM:
	case SWITCHER_COSTUME2_VNUM:
	{
		if (item->GetType() != ITEM_COSTUME)
			check = false;
	}
	break;

	case SWITCHER_PENDANT_VNUM:
	case SWITCHER_PENDANT2_VNUM:
	{
		if (item->GetType() != ITEM_ARMOR && item->GetSubType() != ARMOR_PENDANT)
			check = false;
	}
	break;
	
	case SWITCHER_ZODIAC_VNUM:
	{
		if (!IsItemZodiac(item->GetVnum()))
			check = false;
	}
	break;
	
	default:
		check = true;
		break;
	}

	return check;
}

void CSwitchBonus::ChangeSB(LPCHARACTER ch,const TPlayerItemAttribute* aAttr, int vnum_change)
{
	LPITEM item = ch->GetSwitchBotItem(0);

	if(!item)
		return;

	if (!ch->IsSBOpen()){
		return;
	}


#if defined(ENABLE_ANTI_FLOOD)
	if (thecore_pulse() > ch->GetAntiFloodPulse(FLOOD_USE_SWITCHBOT) + PASSES_PER_SEC(1)) {
		ch->SetAntiFloodCount(FLOOD_USE_SWITCHBOT, 0);
		ch->SetAntiFloodPulse(FLOOD_USE_SWITCHBOT, thecore_pulse());
	}

	if (ch->IncreaseAntiFloodCount(FLOOD_USE_SWITCHBOT) >= 5)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Autoclicker is not allowed...");
		return;
	}
#endif

	if (item->CheckBonusSwitchBot(aAttr)) {
		ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_SWITCHBOT_ALREADY");
		return;
	}

	if (!CheckVnumsChange(item, vnum_change))
		return;

	int count_change = 	ch->CountSpecifyItem(vnum_change)	;
	int remove_total = 0;
	int max_changes = COUNT_CHANGE_ITEM;
	bool change_succefull = false;

	if (count_change > 0)
	{
		if(count_change < max_changes){
			remove_total = count_change;
		}else{
			remove_total = max_changes;
		}

		int result = item->ChangeAttributeSwtichBot(remove_total,aAttr,&change_succefull);
		item->UpdatePacket();
		item->Save();

		if (result > 0) {
			if (change_succefull) {
				ch->ChatPacket(CHAT_TYPE_COMMAND, "SWITCHBOT_SUCCESFULL %d", result);
			} else {
				ch->ChatPacketNew(CHAT_TYPE_INFO, 1530, "%d", result);
			}

			ch->RemoveSpecifyItem(vnum_change,result);
		} else {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1531, "");
		}
	} else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 1529, "");
	}
}


void CSwitchBonus::SendSBPackets(LPCHARACTER ch,BYTE subheader, int id_bonus, int value_bonus_0, int value_bonus_1, int value_bonus_2, int value_bonus_3, int value_bonus_4)
{
	TPacketGCSwitchBotReceive pack;
	pack.bSubType = subheader;


	if(subheader == SWITCH_RECEIVE_INFO_BONUS_ITEM)
	{
		pack.id_bonus = id_bonus;
		pack.value_bonus_0 = value_bonus_0;
		pack.value_bonus_1 = value_bonus_1;
		pack.value_bonus_2 = value_bonus_2;
		pack.value_bonus_3 = value_bonus_3;
		pack.value_bonus_4 = value_bonus_4;

	}

	LPDESC d = ch->GetDesc();

	if (NULL == d)
	{
		sys_err ("User CSwitchBonus::SendSBPackets (%s)'s DESC is NULL POINT.", ch->GetName());
		return ;
	}

	d->Packet(&pack, sizeof(pack));
}
