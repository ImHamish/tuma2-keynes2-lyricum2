#include "stdafx.h"
#ifdef __FreeBSD__
#include <md5.h>
#else
#include "../libthecore/xmd5.h"
#endif

#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "affect.h"
#include "pvp.h"
#include "start_position.h"
#include "party.h"
#include "guild_manager.h"
#include "p2p.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "war_map.h"
#include "questmanager.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "item.h"
#include "arena.h"
#include "buffer_manager.h"
#include "unique_item.h"
#include "log.h"
#include "../common/VnumHelper.h"
#include "shop.h"
#include "shop_manager.h"
#ifdef __NEWPET_SYSTEM__
#include "New_PetSystem.h"
#endif
#if defined(ENABLE_TRACK_WINDOW)
#include "new_mob_timer.h"
#endif

ACMD(do_user_horse_ride)
{
	if (ch->IsObserverMode())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsHorseRiding() == false)
	{
		if (ch->GetMountVnum()) {
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 532, "");
#endif
			return;
		}

		if (ch->GetHorse() == NULL)
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 332, "");
#endif
			return;
		}

		ch->StartRiding();
	}
	else
	{
		ch->StopRiding();
	}
}

ACMD(do_user_horse_back)
{
	if (!ch)
		return;

	if (ch->GetHorse() != NULL)
	{
		ch->HorseSummon(false);
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 331, "");
#endif
	}
	else if (ch->IsHorseRiding() == true)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 330, "");
#endif
	}
	else
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 332, "");
#endif
	}
}

ACMD(do_user_horse_feed)
{
	// 개인상점을 연 상태에서는 말 먹이를 줄 수 없다.
	if (ch->GetMyShop())
		return;

	if (ch->GetHorse() == NULL)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		if (ch->IsHorseRiding() == false) {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 332, "");
		}
		else {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 336, "");
		}
#endif
		return;
	}

	DWORD dwFood = ch->GetHorseGrade() + 50054 - 1;


	if (ch->CountSpecifyItem(dwFood) > 0)
	{
		ch->RemoveSpecifyItem(dwFood, 1);
		ch->FeedHorse();
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 112, "%s", 
#ifdef ENABLE_MULTI_NAMES
		ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName[ch->GetDesc()->GetLanguage()]
#else
		ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName
#endif
		);
#endif
	}
	else
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 111, "%s", 
#ifdef ENABLE_MULTI_NAMES
		ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName[ch->GetDesc()->GetLanguage()]
#else
		ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName
#endif
		);
#endif
	}

}

#define MAX_REASON_LEN		128

EVENTINFO(TimedEventInfo)
{
	DynamicCharacterPtr ch;
	int		subcmd;
	int         	left_second;
	char		szReason[MAX_REASON_LEN];

	TimedEventInfo()
	: ch()
	, subcmd( 0 )
	, left_second( 0 )
	{
		::memset( szReason, 0, MAX_REASON_LEN );
	}
};

struct SendDisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetCharacter())
		{
			if (d->GetCharacter()->GetGMLevel() == GM_PLAYER)
				d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "quit Shutdown(SendDisconnectFunc)");
		}
	}
};

struct DisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetType() == DESC_TYPE_CONNECTOR)
			return;

		if (d->IsPhase(PHASE_P2P))
			return;

		if (d->GetCharacter())
			d->GetCharacter()->Disconnect("Shutdown(DisconnectFunc)");

		d->SetPhase(PHASE_CLOSE);
	}
};

EVENTINFO(shutdown_event_data)
{
	int seconds;

	shutdown_event_data()
	: seconds( 0 )
	{
	}
};

EVENTFUNC(shutdown_event)
{
	shutdown_event_data* info = dynamic_cast<shutdown_event_data*>( event->info );

	if ( info == NULL )
	{
		sys_err( "shutdown_event> <Factor> Null pointer" );
		return 0;
	}

	int * pSec = & (info->seconds);

	if (*pSec < 0)
	{
		sys_log(0, "shutdown_event sec %d", *pSec);

		if (--*pSec == -10)
		{
			const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), DisconnectFunc());
			return passes_per_sec;
		}
		else if (*pSec < -10)
			return 0;

		return passes_per_sec;
	}
	else if (*pSec == 0)
	{
		const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_set_desc.begin(), c_set_desc.end(), SendDisconnectFunc());
		g_bNoMoreClient = true;
		--*pSec;
		return passes_per_sec;
	}
	else
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		SendNoticeNew(CHAT_TYPE_NOTICE, 0, 0, 577, "%d", *pSec);
#endif
		--*pSec;
		return passes_per_sec;
	}
}

void Shutdown(int iSec)
{
	if (g_bNoMoreClient)
	{
		thecore_shutdown();
		return;
	}

	CWarMapManager::instance().OnShutdown();
#if defined(ENABLE_TEXTS_RENEWAL)
	SendNoticeNew(CHAT_TYPE_NOTICE, 0, 0, 578, "%d", iSec);
#endif
	shutdown_event_data* info = AllocEventInfo<shutdown_event_data>();
	info->seconds = iSec;

	event_create(shutdown_event, info, 1);
}

ACMD(do_shutdown)
{
	if (NULL == ch)
	{
		sys_err("Accept shutdown command from %s.", ch->GetName());
	}
	TPacketGGShutdown p;
	p.bHeader = HEADER_GG_SHUTDOWN;
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShutdown));

	Shutdown(10);
}


#ifdef ENABLE_CHANNEL_SWITCH_SYSTEM
ACMD(do_change_channel)
{
	if (!ch)
		return;

	if (ch->IsWarping())
	{
		return;
	}

	if (!ch->CanWarp())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 234, "10");
#endif
		return;
	}

	if (ch->m_pkTimedEvent)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 482, "");
#endif
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 716, "");
#endif
		return;
	}

	if (g_bChannel == 99)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 719, "");
#endif
		return;
	}

	int32_t channel;
	str_to_number(channel, arg1);

	if (channel < 1 || channel > 6)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 717, "");
#endif
		return;
	}

	if (channel == g_bChannel)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 718, "%d", g_bChannel);
#endif
		return;
	}

	if (ch->GetDungeon())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 720, "");
#endif
		return;
	}

	TPacketChangeChannel p;
	p.channel = channel;
	p.lMapIndex = ch->GetMapIndex();

	db_clientdesc->DBPacket(HEADER_GD_FIND_CHANNEL, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}
#endif

#ifdef ENABLE_SORT_INVEN
ACMD(do_item_check)
{
	ch->EditMyInven();
}


ACMD(do_sort_extra_inventory)
{
	ch->EditMyExtraInven();
}

#endif

EVENTFUNC(timed_event)
{
	TimedEventInfo * info = dynamic_cast<TimedEventInfo *>( event->info );

	if ( info == NULL )
	{
		sys_err( "timed_event> <Factor> Null pointer" );
		return 0;
	}

	const LPCHARACTER ch = info->ch;
	if (ch == NULL) {
		return 0;
	}

	const LPDESC d = ch->GetDesc();

	if (info->left_second <= 0)
	{
#ifdef USE_SAME_TIMED_TIME_IF_POS_FIGHTING
        uint8_t bSeconds = 3;
#else
        uint8_t bSeconds = !ch->IsPosition(POS_FIGHTING) ? 3 : 10;
#endif

        if (ch->IsHack(false, true, bSeconds)
#ifdef __ATTR_TRANSFER_SYSTEM__
            || ch->IsAttrTransferOpen()
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
            || ch->GetOfflineShopGuest() || ch->GetAuctionGuest()
#endif
            || ch->GetExchange()
            || ch->IsOpenSafebox()
            || ch->IsCubeOpen()
            || ch->GetShop()
            || ch->GetMyShop())
        {
            ch->m_pkTimedEvent = nullptr;
            ch->ChatPacketNew(CHAT_TYPE_INFO, 1811, "");
            return 0;
        }

		ch->m_pkTimedEvent = NULL;

		if(info->subcmd == SCMD_LOGOUT || info->subcmd == SCMD_QUIT)
		{
			d->SetLoginKeyExpired(true);
		}
		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
			case SCMD_QUIT:
			case SCMD_PHASE_SELECT:
				{
					TPacketNeedLoginLogInfo acc_info;
					acc_info.dwPlayerID = ch->GetDesc()->GetAccountTable().id;

					db_clientdesc->DBPacket( HEADER_GD_VALID_LOGOUT, 0, &acc_info, sizeof(acc_info) );

					LogManager::instance().DetailLoginLog( false, ch );
				}
				break;
		}

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
				if (d)
					d->SetPhase(PHASE_CLOSE);
				break;

			case SCMD_QUIT:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "quit");
				if (d)
					d->DelayedDisconnect(1);
				break;

			case SCMD_PHASE_SELECT:
				{
					ch->Disconnect("timed_event - SCMD_PHASE_SELECT");

					if (d)
					{
						d->SetPhase(PHASE_SELECT);
					}
				}
				break;
		}

		return 0;
	}
	else
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 103, "%d", info->left_second);
#endif
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}

ACMD(do_cmd)
{
	if (ch->m_pkTimedEvent)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 482, "");
#endif
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

#if defined(ENABLE_TEXTS_RENEWAL)
	switch (subcmd)
	{
		case SCMD_LOGOUT:
			ch->ChatPacketNew(CHAT_TYPE_INFO, 326, "");
			break;
		case SCMD_QUIT:
			ch->ChatPacketNew(CHAT_TYPE_INFO, 240, "");
			break;
		case SCMD_PHASE_SELECT:
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 483, "");
#endif
			break;
	}
#endif

	const int nExitLimitTime = 10;

	if (ch->IsHack(false, true, nExitLimitTime) &&
	   	(!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
		case SCMD_QUIT:
		case SCMD_PHASE_SELECT:
			{
				TimedEventInfo* info = AllocEventInfo<TimedEventInfo>();

#ifdef USE_SAME_TIMED_TIME_IF_POS_FIGHTING
                info->left_second = 3;
#else
                if (ch->IsPosition(POS_FIGHTING))
                {
                    info->left_second = 10;
                }
                else
                {
                    info->left_second = 3;
                }
#endif

				info->ch		= ch;
				info->subcmd		= subcmd;
				strlcpy(info->szReason, argument, sizeof(info->szReason));

				ch->m_pkTimedEvent	= event_create(timed_event, info, 1);
			}
			break;
	}
}

ACMD(do_fishing)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	ch->SetRotation(atof(arg1));
	ch->fishing();
}

ACMD(do_console)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ConsoleEnable");
}

ACMD(do_restart)
{
	if (false == ch->IsDead())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
		ch->StartRecoveryEvent();
		return;
	}

#if defined(USE_DEADTIME_CHECK)
    if (get_dword_time() < ch->GetDeadTime())
    {
        uint32_t dwLastTime = ch->GetDeadTime() - get_dword_time();
        if (dwLastTime > 0)
        {
#if defined(ENABLE_TEXTS_RENEWAL)
            ch->ChatPacketNew(CHAT_TYPE_INFO, 469, "%u",
                ((uint32_t)(dwLastTime / 1000) == 0 ? 1 : (uint32_t)(dwLastTime / 1000)));
#else
            ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can do this again in %u second(s)."),
                ((uint32_t)(dwLastTime / 1000) == 0 ? 1 : (uint32_t)(dwLastTime / 1000)));
#endif

            return;
        }
    }
#endif

	if (NULL == ch->m_pkDeadEvent) {
		return;
	}

	if (!ch->CanWarp()) {
		return;
	}

	if (ch->IsHack())
	{
		if (subcmd == SCMD_RESTART_TOWN)
		{
			return;
		}
		else if (subcmd != SCMD_RESTART_TOWN && (!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
		{
			return;
		}
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");

	ch->GetDesc()->SetPhase(PHASE_GAME);
	ch->SetPosition(POS_STANDING);
	ch->StartRecoveryEvent();


	int32_t mapidx = ch->GetMapIndex();

	if (ch->GetWarMap() && !ch->IsObserverMode())
	{
		CWarMap * pMap = ch->GetWarMap();
		DWORD dwGuildOpponent = pMap ? pMap->GetGuildOpponent(ch) : 0;
		if (dwGuildOpponent)
		{
			switch (subcmd)
			{
				case SCMD_RESTART_TOWN:
					{
						sys_log(0, "do_restart: restart town");

						PIXEL_POSITION pos;
						if (CWarMapManager::instance().GetStartPosition(mapidx, ch->GetGuild()->GetID() < dwGuildOpponent ? 0 : 1, pos))
						{
							ch->Show(mapidx, pos.x, pos.y);
						}
						else
						{
							ch->ExitToSavedLocation();
						}

						ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
						ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
						ch->ReviveInvisible(5);
					}
					break;

				case SCMD_RESTART_HERE:
					{
						sys_log(0, "do_restart: restart here");
						ch->RestartAtSamePos();
						ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
						ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
						ch->ReviveInvisible(5);
#ifdef ENABLE_SKILLS_BUFF_ALTERNATIVE
						ch->LoadAffectSkills();
#endif
					}
					break;
				default:
					{
						sys_err(0, "do_restart: unknown method for %s", ch->GetName());
					}
					break;
			}

			return;
		}
	}

	switch (subcmd)
	{
		case SCMD_RESTART_TOWN:
			{
				sys_log(0, "do_restart: restart town");

				bool wasDungeon = false;
				bool showed = false;
				if (mapidx >= 10000)
				{
					LPDUNGEON dungeon = CDungeonManager::instance().FindByMapIndex(mapidx);
					if (dungeon)
					{
						if (mapidx >= 2160000 && mapidx < 2170000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
									{
										ch->Show(mapidx, 445000, 1228200);
										showed = true;
									}
									break;
								case 2:
									{
										ch->Show(mapidx, 391700, 1293200);
										showed = true;
									}
									break;
								case 3:
									{
										ch->Show(mapidx, 443400, 1269800);
										showed = true;
									}
									break;
								case 4:
									{
										ch->Show(mapidx, 314700, 1318700);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 2080000 && mapidx < 2090000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 843500, 1066800);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 2170000 && mapidx < 2180000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 87900, 614700);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 3550000 && mapidx < 3560000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
									{
										ch->Show(mapidx, 216600, 266700);
										showed = true;
									}
									break;
								case 2:
									{
										ch->Show(mapidx, 218600, 348900);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 3530000 && mapidx < 3540000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 166500, 522100);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 3520000 && mapidx < 3530000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
								case 2:
									{
										ch->Show(mapidx, 588100, 180400);
										showed = true;
									}
									break;
								case 3:
									{
										ch->Show(mapidx, 554000, 207000);
										showed = true;
									}
									break;
								case 4:
								case 5:
									{
										ch->Show(mapidx, 569100, 223000);
										showed = true;
									}
									break;
								case 6:
									{
										ch->Show(mapidx, 586600, 206800);
										showed = true;
									}
									break;
								case 7:
								case 8:
									{
										ch->Show(mapidx, 596900, 222500);
										showed = true;
									}
									break;
								case 9:
									{
										ch->Show(mapidx, 604700, 192600);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 3570000 && mapidx < 3580000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
									{
										ch->Show(mapidx, 905100, 2261700);
										showed = true;
									}
									break;
								case 2:
								case 3:
									{
										ch->Show(mapidx, 926600, 2262100);
										showed = true;
									}
									break;
								case 4:
								case 5:
									{
										ch->Show(mapidx, 953600, 2260800);
										showed = true;
									}
									break;
								case 6:
									{
										ch->Show(mapidx, 913700, 2355800);
										showed = true;
									}
									break;
								case 7:
									{
										ch->Show(mapidx, 975900, 2365500);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 3510000 && mapidx < 3520000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
								case 2:
								case 3:
								case 4:
								case 5:
								case 6:
								case 7:
									{
										ch->Show(mapidx, 776600, 671900);
										showed = true;
									}
									break;
								case 8:
									{
										ch->Show(mapidx, 810900, 686700);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 2180000 && mapidx < 2190000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
									{
										ch->Show(mapidx, 624500, 1415200);
										showed = true;
									}
									break;
								case 2:
									{
										ch->Show(mapidx, 627300, 1446800);
										showed = true;
									}
									break;
								case 3:
									{
										ch->Show(mapidx, 673200, 1444000);
										showed = true;
									}
									break;
								case 4:
									{
										ch->Show(mapidx, 655400, 1421200);
										showed = true;
									}
									break;
								case 5:
									{
										ch->Show(mapidx, 695500, 1421300);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 270000 && mapidx < 280000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 942000, 127700);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 2090000 && mapidx < 2100000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 853700, 1416400);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 2100000 && mapidx < 2110000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 782400, 1502100);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 660000 && mapidx < 670000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
									{
										ch->Show(mapidx, 377400, 2704000);
										showed = true;
									}
									break;
								case 2:
									{
										ch->Show(mapidx, 378200, 2680300);
										showed = true;
									}
									break;
								case 3:
								case 4:
									{
										ch->Show(mapidx, 401700, 2728500);
										showed = true;
									}
									break;
								case 5:
									{
										ch->Show(mapidx, 401700, 2705700);
										showed = true;
									}
									break;
								case 6:
								case 7:
									{
										ch->Show(mapidx, 402200, 2682300);
										showed = true;
									}
									break;
								case 8:
								case 9:
								case 10:
									{
										ch->Show(mapidx, 423800, 2729400);
										showed = true;
									}
									break;
								case 11:
								case 12:
									{
										ch->Show(mapidx, 423800, 2705900);
										showed = true;
									}
									break;
								case 13:
									{
										ch->Show(mapidx, 423800, 2681100);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 3560000 && mapidx < 3570000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 1528200, 2318700);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 2120000 && mapidx < 2130000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 320000, 1529000);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 260000 && mapidx < 270000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
									{
										ch->Show(mapidx, 54500, 2268000);
										showed = true;
									}
									break;
								case 2:
									{
										ch->Show(mapidx, 19400, 2306600);
										showed = true;
									}
									break;
								case 3:
									{
										ch->Show(mapidx, 110500, 2295900);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 250000 && mapidx < 260000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
									{
										ch->Show(mapidx, 2320600, 3077800);
										showed = true;
									}
									break;
								case 2:
									{
										ch->Show(mapidx, 2351500, 3141500);
										showed = true;
									}
									break;
								default:
									{
										ch->ExitToSavedLocation();
									}
									break;
							}
						}
						else if (mapidx >= 1360000 && mapidx < 1370000)
						{
							wasDungeon = true;
							int32_t floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
								{
									ch->Show(mapidx, (768+128)*100, (16384+67)*100);
									showed = true;
									break;
								}
								case 2:
								{
									ch->Show(mapidx, (768+384)*100, (16384+67)*100);
									showed = true;
									break;
								}
								case 3:
								{
									ch->Show(mapidx, (768+640)*100, (16384+67)*100);
									showed = true;
									break;
								}
								case 4:
								{
									ch->Show(mapidx, (768+128)*100, (16384+690)*100);
									showed = true;
									break;
								}
								default:
								{
									ch->ExitToSavedLocation();
									break;
								}
							}
						}
						else if (mapidx >= 1560000 && mapidx < 1570000)
						{
							wasDungeon = true;
							int floor = dungeon->GetFlag("floor");
							if (floor != 0)
							{
								ch->Show(mapidx, 3608300, 2270300);
								showed = true;
							}
							else
							{
								ch->ExitToSavedLocation();
							}
						}
						else if (mapidx >= 1570000 && mapidx < 1580000)
						{
							wasDungeon = true;
							int floor = dungeon->GetFlag("floor");
							switch (floor)
							{
								case 1:
								case 2:
								case 3:
								{
									ch->Show(mapidx, 403700, 2276600);
									showed = true;
									break;
								}
								case 4:
								case 5:
								case 6:
								case 7:
								case 8:
								case 9:
								case 10:
								case 11:
								case 12:
								case 13:
								case 14:
								case 15:
								case 16:
								case 17:
								case 18:
								case 19:
								case 20:
								case 21:
								case 22:
								case 23:
								{
									ch->Show(mapidx, 440300, 2268500);
									showed = true;
									break;
								}
								default:
								{
									ch->ExitToSavedLocation();
									break;
								}
							}
						}
					}
				}

				if (!wasDungeon)
				{
					PIXEL_POSITION pos;
					if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(mapidx, ch->GetEmpire(), pos))
					{
						ch->WarpSet(pos.x, pos.y);
					}
					else
					{
						ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
					}
				}

				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->DeathPenalty(1);
				if (showed)
				{
#ifdef ENABLE_SKILLS_BUFF_ALTERNATIVE
					ch->LoadAffectSkills();
#endif
				}
			}
			break;
		case SCMD_RESTART_HERE:
			{
				sys_log(0, "do_restart: restart here");

				ch->RestartAtSamePos();
#ifdef ENABLE_REVIVE_WITH_HALF_HP_IF_MONSTER_KILLED_YOU
				ch->PointChange(POINT_HP, ch->GetDeadByMonster() ? (ch->GetMaxHP() - ch->GetHP()) / 2 : 50 - ch->GetHP());
#else
				ch->PointChange(POINT_HP, 50 - ch->GetHP());
#endif
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->DeathPenalty(0);
				ch->ReviveInvisible(5);
#ifdef ENABLE_SKILLS_BUFF_ALTERNATIVE
				ch->LoadAffectSkills();
#endif
			}
			break;
		default:
			{
				sys_err(0, "do_restart: unknown method for %s", ch->GetName());
			}
			break;
	}
}

#define MAX_STAT g_iStatusPointSetMaxValue

ACMD(do_stat_reset)
{
	ch->PointChange(POINT_STAT_RESET_COUNT, 12 - ch->GetPoint(POINT_STAT_RESET_COUNT));
}

ACMD(do_stat_minus)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 312, "");
#endif
		return;
	}

	if (ch->GetPoint(POINT_STAT_RESET_COUNT) <= 0)
		return;

	if (!strcmp(arg1, "st"))
	{
		if (ch->GetRealPoint(POINT_ST) <= JobInitialPoints[ch->GetJob()].st)
			return;

		ch->SetRealPoint(POINT_ST, ch->GetRealPoint(POINT_ST) - 1);
		ch->SetPoint(POINT_ST, ch->GetPoint(POINT_ST) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_ST, 0);
	}
	else if (!strcmp(arg1, "dx"))
	{
		if (ch->GetRealPoint(POINT_DX) <= JobInitialPoints[ch->GetJob()].dx)
			return;

		ch->SetRealPoint(POINT_DX, ch->GetRealPoint(POINT_DX) - 1);
		ch->SetPoint(POINT_DX, ch->GetPoint(POINT_DX) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_DX, 0);
	}
	else if (!strcmp(arg1, "ht"))
	{
		if (ch->GetRealPoint(POINT_HT) <= JobInitialPoints[ch->GetJob()].ht)
			return;

		ch->SetRealPoint(POINT_HT, ch->GetRealPoint(POINT_HT) - 1);
		ch->SetPoint(POINT_HT, ch->GetPoint(POINT_HT) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_HT, 0);
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (!strcmp(arg1, "iq"))
	{
		if (ch->GetRealPoint(POINT_IQ) <= JobInitialPoints[ch->GetJob()].iq)
			return;

		ch->SetRealPoint(POINT_IQ, ch->GetRealPoint(POINT_IQ) - 1);
		ch->SetPoint(POINT_IQ, ch->GetPoint(POINT_IQ) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_IQ, 0);
		ch->PointChange(POINT_MAX_SP, 0);
	}
	else
		return;

	ch->PointChange(POINT_STAT, +1);
	ch->PointChange(POINT_STAT_RESET_COUNT, -1);
	ch->ComputePoints();
}

ACMD(do_stat)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 312, "");
#endif
		return;
	}

	if (ch->GetPoint(POINT_STAT) <= 0)
		return;

	BYTE idx = 0;

	if (!strcmp(arg1, "st"))
		idx = POINT_ST;
	else if (!strcmp(arg1, "dx"))
		idx = POINT_DX;
	else if (!strcmp(arg1, "ht"))
		idx = POINT_HT;
	else if (!strcmp(arg1, "iq"))
		idx = POINT_IQ;
	else
		return;

	if (ch->GetRealPoint(idx) >= MAX_STAT)
		return;

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + 1);
	ch->SetPoint(idx, ch->GetPoint(idx) + 1);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (idx == POINT_HT)
	{
		ch->PointChange(POINT_MAX_SP, 0);
	}

	ch->PointChange(POINT_STAT, -1);
	ch->ComputePoints();
}

#ifdef ENABLE_PVP_ADVANCED
#include <string>
#include <algorithm>
const char* szTableStaticPvP[] = {BLOCK_CHANGEITEM, BLOCK_BUFF, BLOCK_POTION, BLOCK_RIDE, BLOCK_PET, BLOCK_POLY, BLOCK_PARTY, BLOCK_EXCHANGE_, BET_WINNER, CHECK_IS_FIGHT};

ACMD(do_pvp)
{
	if (!ch)
		return;
	
	if (ch->GetArena() != NULL || CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
		return;
	}

	char arg1[256], arg2[256], arg3[256], arg4[256], arg5[256], arg6[256], arg7[256], arg8[256], arg9[256], arg10[256];
	
	pvp_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3), arg4, sizeof(arg4), arg5, sizeof(arg5), arg6, sizeof(arg6), arg7, sizeof(arg7), arg8, sizeof(arg8), arg9, sizeof(arg9), arg10, sizeof(arg10));	
	
	DWORD vid = 0;
	str_to_number(vid, arg1);	
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
		return;

	if (pkVictim->IsNPC())
		return;

#if defined(ENABLE_MESSENGER_BLOCK)
	if (MessengerManager::instance().CheckMessengerList(ch->GetName(), pkVictim->GetName(), SYST_BLOCK))
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 1429, "%s", pkVictim->GetName());
#else
		ch->ChatPacket(CHAT_TYPE_INFO, "You can't perform this action because %s blocked you.", pkVictim->GetName());
#endif
		return;
	}
#endif

	if (pkVictim->GetArena() != NULL) {
		return;
	}

	int mytime = pkVictim->GetQuestFlag("pvp.timed");
	int itime = mytime <= 0 ? 0 : mytime - get_global_time();
	if (itime > 0) {
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 888, "%d", itime);
		pkVictim->ChatPacketNew(CHAT_TYPE_DIALOG, 889, "%s", ch->GetName());
#endif
		return;
	}
	else {
		pkVictim->SetQuestFlag("pvp.timed", get_global_time() + 30);
	}

	if (ch->GetExchange() || pkVictim->GetExchange())
	{
		CPVPManager::instance().Decline(ch, pkVictim);
		CPVPManager::instance().Decline(pkVictim, ch);
		return;
	}
	
	if (*arg2 && !strcmp(arg2, "accept"))
	{	
#if defined(ENABLE_NEW_GOLD_LIMIT)
		uint64_t chA_nBetMoney = ch->GetQuestFlag(szTableStaticPvP[8]);
		uint64_t chB_nBetMoney = pkVictim->GetQuestFlag(szTableStaticPvP[8]);
		uint64_t limit = 2000000000;
#else
		uint32_t chA_nBetMoney = ch->GetQuestFlag(szTableStaticPvP[8]);
		uint32_t chB_nBetMoney = pkVictim->GetQuestFlag(szTableStaticPvP[8]);
		uint32_t limit = 2000000000;
#endif

		if ((ch->GetGold() < chA_nBetMoney) || (pkVictim->GetGold() < chB_nBetMoney ) || (chA_nBetMoney > limit) || (chB_nBetMoney > limit)) {
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 722, "");
			pkVictim->ChatPacketNew(CHAT_TYPE_INFO, 722, "");
#endif
			CPVPManager::instance().Decline(ch, pkVictim);
			CPVPManager::instance().Decline(pkVictim, ch);
			return;
		}

		ch->SetDuel("IsFight", 1);
		pkVictim->SetDuel("IsFight", 1);
		
		if (chA_nBetMoney > 0 && chA_nBetMoney > 0)
		{
#if defined(ENABLE_NEW_GOLD_LIMIT)
			ch->ChangeGold(-chA_nBetMoney);
			pkVictim->ChangeGold(-chB_nBetMoney);
#else
			ch->PointChange(POINT_GOLD, -chA_nBetMoney, true);
			pkVictim->PointChange(POINT_GOLD, -chB_nBetMoney, true);
#endif
		}

		CPVPManager::instance().Insert(ch, pkVictim);
		return;
	}
	
	int m_BlockChangeItem = 0, m_BlockBuff = 0, m_BlockPotion = 0, m_BlockRide = 0, m_BlockPet = 0, m_BlockPoly = 0, m_BlockParty = 0, m_BlockExchange = 0;

#if defined(ENABLE_NEW_GOLD_LIMIT)
	uint64_t m_BetMoney = 0;
#else
	uint32_t m_BetMoney = 0;
#endif

	str_to_number(m_BlockChangeItem, arg2);
	str_to_number(m_BlockBuff, arg3);
	str_to_number(m_BlockPotion, arg4);
	str_to_number(m_BlockRide, arg5);
	str_to_number(m_BlockPet, arg6);
	str_to_number(m_BlockPoly, arg7);
	str_to_number(m_BlockParty, arg8);
	str_to_number(m_BlockExchange, arg9);
	str_to_number(m_BetMoney, arg10);
	
	if (!isdigit(*arg2) && !isdigit(*arg3) && !isdigit(*arg4) && !isdigit(*arg5) && !isdigit(*arg6) && !isdigit(*arg7) && !isdigit(*arg8) && !isdigit(*arg9) && !isdigit(*arg10))
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 874, "");
#endif
		return;
	}

	if (m_BetMoney < 0)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 875, "");
#endif
		return;
	}

#if defined(ENABLE_NEW_GOLD_LIMIT)
	if (m_BetMoney >= GOLD_MAX_MAX)
#else
	if (m_BetMoney >= GOLD_MAX)
#endif
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 876, "");
#endif
		return;
	}
	
	if (ch->GetGold() < m_BetMoney)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 877, "");
#endif
		return;
	}

#if defined(ENABLE_NEW_GOLD_LIMIT)
	if ((ch->GetGold() + m_BetMoney) > GOLD_MAX_MAX)
#else
	if ((ch->GetGold() + m_BetMoney) > GOLD_MAX)
#endif
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 878, "");
#endif
		return;
	}

#if defined(ENABLE_NEW_GOLD_LIMIT)
	if ((pkVictim->GetGold() + m_BetMoney) > GOLD_MAX_MAX)
#else
	if ((pkVictim->GetGold() + m_BetMoney) > GOLD_MAX)
#endif
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 878, "");
#endif
		return;
	}
	
	if (pkVictim->GetGold() < m_BetMoney)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 879, "");
#endif
		return;
	}
	
	if (*arg1 && *arg2 && *arg3 && *arg4 && *arg5 && *arg6 && *arg7 && *arg8 && *arg9 && *arg10)
	{
		ch->SetDuel("BlockChangeItem", m_BlockChangeItem);			ch->SetDuel("BlockBuff", m_BlockBuff);
		ch->SetDuel("BlockPotion", m_BlockPotion);					ch->SetDuel("BlockRide", m_BlockRide);
		ch->SetDuel("BlockPet", m_BlockPet);						ch->SetDuel("BlockPoly", m_BlockPoly);	
		ch->SetDuel("BlockParty", m_BlockParty);					ch->SetDuel("BlockExchange", m_BlockExchange);
		ch->SetDuel("BetMoney", m_BetMoney);

		pkVictim->SetDuel("BlockChangeItem", m_BlockChangeItem);	pkVictim->SetDuel("BlockBuff", m_BlockBuff);
		pkVictim->SetDuel("BlockPotion", m_BlockPotion);			pkVictim->SetDuel("BlockRide", m_BlockRide);
		pkVictim->SetDuel("BlockPet", m_BlockPet);					pkVictim->SetDuel("BlockPoly", m_BlockPoly);	
		pkVictim->SetDuel("BlockParty", m_BlockParty);				pkVictim->SetDuel("BlockExchange", m_BlockExchange);
		pkVictim->SetDuel("BetMoney", m_BetMoney);
			
		CPVPManager::instance().Insert(ch, pkVictim); 
	}
}

ACMD(do_pvp_advanced)
{   
	if (!ch)
		return;

	if (ch->GetArena() != NULL || CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
		return;
	}
	
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
		return;
	
	if (pkVictim->IsNPC())
		return;
	
	if (pkVictim->GetArena() != NULL) {
		return;
	}
	
	if (ch->GetQuestFlag(szTableStaticPvP[9]) > 0)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 882, "");
#endif
		return;
	}
	
	if (pkVictim->GetQuestFlag(szTableStaticPvP[9]) > 0)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_DIALOG, 882, "");
#endif
		return;
	}
	
	int statusEq = pkVictim->GetQuestFlag(BLOCK_EQUIPMENT_);
	
	CGuild * g = pkVictim->GetGuild();

	const char* m_Name = pkVictim->GetName();	
	const char* m_GuildName = "-";
		
	int m_Vid = pkVictim->GetVID();	
	int m_Level = pkVictim->GetLevel();
	int m_PlayTime = pkVictim->GetRealPoint(POINT_PLAYTIME);
	int m_MaxHP = pkVictim->GetMaxHP();
	int m_MaxSP = pkVictim->GetMaxSP();
	
	DWORD m_Race = pkVictim->GetRaceNum();
	
	if (g)
	{ 
		ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_Duel_GetInfo %d %s %s %d %d %d %d %d", m_Vid, m_Name, g->GetName(), m_Level, m_Race, m_PlayTime, m_MaxHP, m_MaxSP);
		
		if (statusEq < 1)
			pkVictim->SendEquipment(ch);
	}
	else { 
		ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_Duel_GetInfo %d %s %s %d %d %d %d %d", m_Vid, m_Name, m_GuildName, m_Level, m_Race, m_PlayTime, m_MaxHP, m_MaxSP);
		
		if (statusEq < 1)
			pkVictim->SendEquipment(ch);
	}
}

ACMD(do_decline_pvp)
{
	if (!ch)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	
	if (!*arg1)
		return;
	
	DWORD vid = 0;
	str_to_number(vid, arg1);
	
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);
	
	if (!pkVictim)
		return;
	
	if (pkVictim->IsNPC())
		return;
	
	CPVPManager::instance().Decline(ch, pkVictim);
	ch->SetQuestFlag("pvp.timed", 0);
	pkVictim->SetQuestFlag("pvp.timed", 0);
}

ACMD(do_block_equipment)
{
	if (!ch)
		return;

	char arg1[256];
	one_argument (argument, arg1, sizeof(arg1));
	
	if (!ch->IsPC() || NULL == ch)
		return;
	
	int statusEq = ch->GetQuestFlag(BLOCK_EQUIPMENT_);
	
	if (!strcmp(arg1, "BLOCK"))
	{
		if (statusEq > 0)
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 11, "");
#endif
		}
		else {
			ch->SetQuestFlag(BLOCK_EQUIPMENT_, 1);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "equipview 1");
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 12, "");
#endif
		}
	}
	else if (!strcmp(arg1, "UNBLOCK"))
	{
		if (statusEq != 0) {
			ch->SetQuestFlag(BLOCK_EQUIPMENT_, 0);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "equipview 0");
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 14, "");
#endif
		}
#if defined(ENABLE_TEXTS_RENEWAL)
		else {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 13, "");
		}
#endif
	}
}
#endif

ACMD(do_guildskillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch->GetGuild())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 138, "");
#endif
		return;
	}

	CGuild* g = ch->GetGuild();
	TGuildMember* gm = g->GetMember(ch->GetPlayerID());
	if (gm->grade == GUILD_LEADER_GRADE)
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		g->SkillLevelUp(vnum);
	}
#if defined(ENABLE_TEXTS_RENEWAL)
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 890, "");
	}
#endif
}

ACMD(do_skillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vnum = 0;
	str_to_number(vnum, arg1);

	if (true == ch->CanUseSkill(vnum))
	{
		ch->SkillLevelUp(vnum);
	}
	else
	{
		switch(vnum)
		{
			case SKILL_HORSE_WILDATTACK:
			case SKILL_HORSE_CHARGE:
			case SKILL_HORSE_ESCAPE:
			case SKILL_HORSE_WILDATTACK_RANGE:

			case SKILL_7_A_ANTI_TANHWAN:
			case SKILL_7_B_ANTI_AMSEOP:
			case SKILL_7_C_ANTI_SWAERYUNG:
			case SKILL_7_D_ANTI_YONGBI:

			case SKILL_8_A_ANTI_GIGONGCHAM:
			case SKILL_8_B_ANTI_YEONSA:
			case SKILL_8_C_ANTI_MAHWAN:
			case SKILL_8_D_ANTI_BYEURAK:

			case SKILL_ADD_HP:
			case SKILL_RESIST_PENETRATE:

				ch->SkillLevelUp(vnum);
				break;
		}
	}
}

//
// @version	05/06/20 Bang2ni - 커맨드 처리 Delegate to CHARACTER class
//
ACMD(do_safebox_close)
{
	ch->CloseSafebox();
}

//
// @version	05/06/20 Bang2ni - 커맨드 처리 Delegate to CHARACTER class
//
ACMD(do_safebox_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	ch->ReqSafeboxLoad(arg1);
}

ACMD(do_safebox_change_password)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || strlen(arg1)>6)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 188, "");
#endif
		return;
	}

	if (!*arg2 || strlen(arg2)>6)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 188, "");
#endif
		return;
	}

	TSafeboxChangePasswordPacket p;

	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szOldPassword, arg1, sizeof(p.szOldPassword));
	strlcpy(p.szNewPassword, arg2, sizeof(p.szNewPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_CHANGE_PASSWORD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || strlen(arg1) > 6)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 188, "");
#endif
		return;
	}

	int iPulse = thecore_pulse();

	if (ch->GetMall())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 189, "");
#endif
		return;
	}

	if (iPulse - ch->GetMallLoadTime() < passes_per_sec * 10) // 10초에 한번만 요청 가능
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 190, "");
#endif
		return;
	}

	ch->SetMallLoadTime(iPulse);

	TSafeboxLoadPacket p;
	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, ch->GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, arg1, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_MALL_LOAD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_close)
{
	if (ch->GetMall())
	{
		ch->SetMallLoadTime(thecore_pulse());
		ch->CloseMall();
		ch->Save();
	}
}

ACMD(do_ungroup)
{
	if (!ch->GetParty())
		return;

	if (!CPartyManager::instance().IsEnablePCParty())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 208, "");
#endif
		return;
	}

	if (ch->GetDungeon())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 202, "");
#endif
		return;
	}

	LPPARTY pParty = ch->GetParty();

	if (pParty->GetMemberCount() == 2)
	{
		// party disband
		CPartyManager::instance().DeleteParty(pParty);
	}
	else
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 215, "");
#endif
		//pParty->SendPartyRemoveOneToAll(ch);
		pParty->Quit(ch->GetPlayerID());
		//pParty->SendPartyRemoveAllToOne(ch);
	}
}

ACMD(do_close_shop)
{
	if (ch->GetMyShop())
	{
		ch->CloseMyShop();
		return;
	}
}

ACMD(do_set_walk_mode)
{
	ch->SetNowWalking(true);
	ch->SetWalking(true);
}

ACMD(do_set_run_mode)
{
	ch->SetNowWalking(false);
	ch->SetWalking(false);
}

ACMD(do_war)
{
	//내 길드 정보를 얻어오고
	CGuild * g = ch->GetGuild();

	if (!g)
		return;

	//전쟁중인지 체크한번!
	if (g->UnderAnyWar())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 167, "");
#endif
		return;
	}

	//파라메터를 두배로 나누고
	char arg1[256], arg2[256];
	DWORD type = GUILD_WAR_TYPE_FIELD; //fixme102 base int modded uint
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	if (*arg2)
	{
		str_to_number(type, arg2);
		if (type < 0) {
			return;
		}

		if (type >= GUILD_WAR_TYPE_MAX_NUM)
			type = GUILD_WAR_TYPE_FIELD;
	}

	//길드의 마스터 아이디를 얻어온뒤
	DWORD gm_pid = g->GetMasterPID();

	//마스터인지 체크(길전은 길드장만이 가능)
	if (gm_pid != ch->GetPlayerID())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 144, "");
#endif
		return;
	}

	//상대 길드를 얻어오고
	CGuild * opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 130, "");
#endif
		return;
	}

	//상대길드와의 상태 체크
	switch (g->GetGuildWarState(opp_g->GetID()))
	{
		case GUILD_WAR_NONE:
			{
				if (opp_g->UnderAnyWar())
				{
#if defined(ENABLE_TEXTS_RENEWAL)
					ch->ChatPacketNew(CHAT_TYPE_INFO, 157, "");
#endif
					return;
				}

				int iWarPrice = KOR_aGuildWarInfo[type].iWarPrice;

				if (g->GetGuildMoney() < iWarPrice)
				{
#if defined(ENABLE_TEXTS_RENEWAL)
					ch->ChatPacketNew(CHAT_TYPE_INFO, 172, "");
#endif
					return;
				}

				if (opp_g->GetGuildMoney() < iWarPrice)
				{
#if defined(ENABLE_TEXTS_RENEWAL)
					ch->ChatPacketNew(CHAT_TYPE_INFO, 160, "");
#endif
					return;
				}
			}
			break;

		case GUILD_WAR_SEND_DECLARE:
			{
#if defined(ENABLE_TEXTS_RENEWAL)
				ch->ChatPacketNew(CHAT_TYPE_INFO, 438, "");
#endif
				return;
			}
			break;

		case GUILD_WAR_RECV_DECLARE:
			{
				if (opp_g->UnderAnyWar())
				{
#if defined(ENABLE_TEXTS_RENEWAL)
					ch->ChatPacketNew(CHAT_TYPE_INFO, 157, "");
#endif
					g->RequestRefuseWar(opp_g->GetID());
					return;
				}
			}
			break;

		case GUILD_WAR_RESERVE:
			{
#if defined(ENABLE_TEXTS_RENEWAL)
				ch->ChatPacketNew(CHAT_TYPE_INFO, 169, "");
#endif
				return;
			}
			break;

		case GUILD_WAR_END:
			return;

		default:
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 168, "");
#endif
			g->RequestRefuseWar(opp_g->GetID());
			return;
	}

	if (!g->CanStartWar(type))
	{
		// 길드전을 할 수 있는 조건을 만족하지않는다.
		if (g->GetLadderPoint() == 0)
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 159, "");
#endif
			sys_log(0, "GuildWar.StartError.NEED_LADDER_POINT");
		}
		else if (g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 145, "%d", GUILD_WAR_MIN_MEMBER_COUNT);
#endif
			sys_log(0, "GuildWar.StartError.NEED_MINIMUM_MEMBER[%d]", GUILD_WAR_MIN_MEMBER_COUNT);
		}
		else
		{
			sys_log(0, "GuildWar.StartError.UNKNOWN_ERROR");
		}
		return;
	}

	// 필드전 체크만 하고 세세한 체크는 상대방이 승낙할때 한다.
	if (!opp_g->CanStartWar(GUILD_WAR_TYPE_FIELD))
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		if (opp_g->GetLadderPoint() == 0) {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 153, "%s", opp_g->GetName());
		} else if (opp_g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT) {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 158, "");
		}
#endif
		return;
	}

	do
	{
		if (g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(g->GetMasterPID());

		if (pCCI != NULL)
			break;

#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 507, "");
#endif
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	do
	{
		if (opp_g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(opp_g->GetMasterPID());

		if (pCCI != NULL)
			break;

#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 507, "");
#endif
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	g->RequestDeclareWar(opp_g->GetID(), type);
}

ACMD(do_nowar)
{
	CGuild* g = ch->GetGuild();
	if (!g)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 144, "");
#endif
		return;
	}

	CGuild* opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 130, "");
#endif
		return;
	}
	
	g->RequestRefuseWar(opp_g->GetID());
}

ACMD(do_detaillog)
{
	ch->DetailLog();
}

ACMD(do_monsterlog)
{
	ch->ToggleMonsterLog();
}

ACMD(do_pkmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE mode = 0;
	str_to_number(mode, arg1);

	if (mode == PK_MODE_PROTECT)
		return;

	if (ch->GetLevel() < PK_PROTECT_LEVEL && mode != 0)
		return;

	ch->SetPKMode(mode);
}

ACMD(do_messenger_auth)
{
	if (ch->GetArena())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
		return;
	}

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	const char answer = LOWER(*arg1);
	// @fixme130 AuthToAdd void -> bool
	const bool bIsDenied = answer != 'y';
	const bool bIsAdded = MessengerManager::instance().AuthToAdd(ch->GetName(), arg2, bIsDenied); // DENY
	if (bIsAdded && bIsDenied)
	{
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);
#if defined(ENABLE_TEXTS_RENEWAL)
		if (tch) {
			tch->ChatPacketNew(CHAT_TYPE_INFO, 107, "%s", ch->GetName());
		}
#endif
	}
}

ACMD(do_setblockmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		BYTE flag = 0;
		str_to_number(flag, arg1);
		ch->SetBlockMode(flag);
	}
}

ACMD(do_unmount)
{
#if defined(USE_MOUNT_COSTUME_SYSTEM)
	if (const LPITEM pCostumeMount = ch->GetWear(WEAR_COSTUME_MOUNT))
	{
		if (ch->UnequipItem(pCostumeMount) == false)
		{
			return;
		}
	}
#endif

	ch->UnMount(true);
}

ACMD(do_observer_exit)
{
	if (ch->IsObserverMode())
	{
		if (ch->GetWarMap())
			ch->SetWarMap(NULL);

		if (ch->GetArena() != NULL || ch->GetArenaObserverMode() == true)
		{
			ch->SetArenaObserverMode(false);

			if (ch->GetArena() != NULL)
				ch->GetArena()->RemoveObserver(ch->GetPlayerID());

			ch->SetArena(NULL);
			ch->WarpSet(ARENA_RETURN_POINT_X(ch->GetEmpire()), ARENA_RETURN_POINT_Y(ch->GetEmpire()));
		}
		else
		{
			ch->ExitToSavedLocation();
		}
		ch->SetObserverMode(false);
	}
}

ACMD(do_view_equip)
{
	if (ch->GetGMLevel() <= GM_PLAYER)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		DWORD vid = 0;
		str_to_number(vid, arg1);
		const LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

		if (!tch)
			return;

		if (!tch->IsPC())
			return;

		tch->SendEquipment(ch);
	}
}

ACMD(do_party_request)
{
	if (ch->GetArena())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
		return;
	}

	if (ch->GetParty())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 441, "");
#endif
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	const LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		if (!ch->RequestToParty(tch))
			ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

ACMD(do_party_request_accept)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	const LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->AcceptToParty(tch);
}

ACMD(do_party_request_deny)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	const LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->DenyToParty(tch);
}

// LUA_ADD_GOTO_INFO
struct GotoInfo
{
	std::string 	st_name;

	BYTE 	empire;
	int 	mapIndex;
	DWORD 	x, y;

	GotoInfo()
	{
		st_name 	= "";
		empire 		= 0;
		mapIndex 	= 0;

		x = 0;
		y = 0;
	}

	GotoInfo(const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void operator = (const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void __copy__(const GotoInfo& c_src)
	{
		st_name 	= c_src.st_name;
		empire 		= c_src.empire;
		mapIndex 	= c_src.mapIndex;

		x = c_src.x;
		y = c_src.y;
	}
};

#ifdef __ATTR_TRANSFER_SYSTEM__
ACMD(do_attr_transfer)
{
	if (!ch->CanDoAttrTransfer())
		return;
	
	sys_log(1, "%s has used an Attr Transfer command: %s.", ch->GetName(), argument);
	
	int w_index = 0, i_index = 0;
	const char *line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));
	if (0 == arg1[0]) {
		return;
	}
	
	const std::string& strArg1 = std::string(arg1);
	if (strArg1 == "open")
	{
		AttrTransfer_open(ch);
		return;
	}
	else if (strArg1 == "close")
	{
		AttrTransfer_close(ch);
		return;
	}
	else if (strArg1 == "make")
	{
		AttrTransfer_make(ch);
		return;
	}
	else if (strArg1 == "add")
	{
		if (0 == arg2[0] || !isdigit(*arg2) || 0 == arg3[0] || !isdigit(*arg3))
			return;
		
		str_to_number(w_index, arg2);
		str_to_number(i_index, arg3);
		AttrTransfer_add_item(ch, w_index, i_index);
		return;
	}
	else if (strArg1 == "delete")
	{
		if (0 == arg2[0] || !isdigit(*arg2))
			return;
		
		str_to_number(w_index, arg2);
		AttrTransfer_delete_item(ch, w_index);
		return;
	}
	
	switch (LOWER(arg1[0]))
	{
		case 'o':
			AttrTransfer_open(ch);
			break;
		case 'c':
			AttrTransfer_close(ch);
			break;
		case 'm':
			AttrTransfer_make(ch);
			break;
		case 'a':
			{
				if (0 == arg2[0] || !isdigit(*arg2) || 0 == arg3[0] || !isdigit(*arg3))
					return;
				
				str_to_number(w_index, arg2);
				str_to_number(i_index, arg3);
				AttrTransfer_add_item(ch, w_index, i_index);
			}
			break;
		case 'd':
			{
				if (0 == arg2[0] || !isdigit(*arg2))
					return;
				
				str_to_number(w_index, arg2);
				AttrTransfer_delete_item(ch, w_index);
			}
			break;
		default:
			return;
	}
}
#endif

ACMD(do_inventory)
{
	int	index = 0;
	int	count		= 1;

	char arg1[256];
	char arg2[256];

	LPITEM	item;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1) {
		return;
	}

	if (!*arg2)
	{
		index = 0;
		str_to_number(count, arg1);
	}
	else
	{
		str_to_number(index, arg1); index = MIN(index, INVENTORY_MAX_NUM);
		str_to_number(count, arg2); count = MIN(count, INVENTORY_MAX_NUM);
	}

	for (int i = 0; i < count; ++i)
	{
		if (index >= INVENTORY_MAX_NUM)
			break;

		item = ch->GetInventoryItem(index);
#if defined(ENABLE_TEXTS_RENEWAL)
		if (item) {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 727, "%d#%s", index, item->GetName());
		} else {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 728, "%d", index);
		}
#endif
		++index;
	}
}

//gift notify quest command
ACMD(do_gift)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "gift");
}

#ifdef __NEWPET_SYSTEM__
ACMD(do_CubePetAdd) {

	int pos = 0;
	int invpos = 0;

	const char *line;
	char arg1[256], arg2[256], arg3[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (0 == arg1[0])
		return;
	const std::string& strArg1 = std::string(arg1);
	switch (LOWER(arg1[0]))
	{
	case 'a':	// add cue_index inven_index
	{
		if (0 == arg2[0] || !isdigit(*arg2) ||
			0 == arg3[0] || !isdigit(*arg3))
			return;

		str_to_number(pos, arg2);
		str_to_number(invpos, arg3);

	}
	break;

	default:
		return;
	}

	if (ch->GetNewPetSystem()->IsActivePet())
		ch->GetNewPetSystem()->SetItemCube(pos, invpos);
	else
		return;

}

ACMD(do_PetSkill) {
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;

	DWORD skillslot = 0;
	str_to_number(skillslot, arg1);
	if (skillslot > 3 || skillslot < 0)
		return;

	if (ch->GetNewPetSystem()->IsActivePet()) { 
		ch->GetNewPetSystem()->DoPetSkill(skillslot);
	}
#if defined(ENABLE_TEXTS_RENEWAL)
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 729, "");
	}
#endif
}

#ifdef ENABLE_NEW_PET_EDITS
ACMD(do_PetIncreaseSkill) {
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	
	if ((!*arg1) || (!*arg2))
		return;
	
	int iSlot = atoi(arg1), iType = atoi(arg2);
	if (!ch->GetNewPetSystem())
		return;
	
	if (ch->GetNewPetSystem()->IsActivePet())
		ch->GetNewPetSystem()->IncreasePetSkill(iSlot, iType);
}
#endif

ACMD(do_FeedCubePet) {
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;

	DWORD feedtype = 0;
	str_to_number(feedtype, arg1);
	if (ch->GetNewPetSystem()->IsActivePet()) {
		ch->GetNewPetSystem()->ItemCubeFeed(feedtype);
	}
#if defined(ENABLE_TEXTS_RENEWAL)
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 729, "");
	}
#endif
}

ACMD(do_PetEvo) {

	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()
#ifdef __ENABLE_NEW_OFFLINESHOP__
		|| ch->GetOfflineShopGuest() || ch->GetAuctionGuest()
#endif
	) {
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 730, "");
#endif
		return;
	}
	if (ch->GetNewPetSystem()->IsActivePet()) {
		int tmpevo = ch->GetNewPetSystem()->GetEvolution();
		if (((tmpevo == 0) && (ch->GetNewPetSystem()->GetLevel() >= 40)) || ((tmpevo == 1) && (ch->GetNewPetSystem()->GetLevel() >= 60)) || ((tmpevo == 2) && (ch->GetNewPetSystem()->GetLevel() >= 80))) {
#ifdef ENABLE_NEW_PET_EDITS
			if (ch->GetNewPetSystem()->GetExp() < ch->GetNewPetSystem()->GetNextExpFromMob()) {
#if defined(ENABLE_TEXTS_RENEWAL)
				ch->ChatPacketNew(CHAT_TYPE_INFO, 59, "");
#endif
				return;
			}
#endif
			
			bool bRet = false;
			DWORD dwItemVnum1 = 55003 + tmpevo;
			if (ch->CountSpecifyItem(dwItemVnum1) < 10) {
#if defined(ENABLE_TEXTS_RENEWAL)
				ch->ChatPacketNew(CHAT_TYPE_INFO, 60, "%d#%s", 10, 
#ifdef ENABLE_MULTI_NAMES
				ITEM_MANAGER::instance().GetTable(dwItemVnum1)->szLocaleName[ch->GetDesc()->GetLanguage()]
#else
				ITEM_MANAGER::instance().GetTable(dwItemVnum1)->szLocaleName
#endif
				);
#endif
				bRet = true;
			}
			
			DWORD dwItemVnum2 = 27992 + tmpevo;
			if (!bRet && ch->CountSpecifyItem(dwItemVnum2) < 10) {
#if defined(ENABLE_TEXTS_RENEWAL)
				ch->ChatPacketNew(CHAT_TYPE_INFO, 60, "%d#%s", 10, 
#ifdef ENABLE_MULTI_NAMES
				ITEM_MANAGER::instance().GetTable(dwItemVnum2)->szLocaleName[ch->GetDesc()->GetLanguage()]
#else
				ITEM_MANAGER::instance().GetTable(dwItemVnum2)->szLocaleName
#endif
				);
#endif
				bRet = true;
			}
			
			DWORD dwItemVnum3 = 86056 + tmpevo;
			if (!bRet && ch->CountSpecifyItem(dwItemVnum3) < 3) {
#if defined(ENABLE_TEXTS_RENEWAL)
				ch->ChatPacketNew(CHAT_TYPE_INFO, 60, "%d#%s", 3, 
#ifdef ENABLE_MULTI_NAMES
				ITEM_MANAGER::instance().GetTable(dwItemVnum3)->szLocaleName[ch->GetDesc()->GetLanguage()]
#else
				ITEM_MANAGER::instance().GetTable(dwItemVnum3)->szLocaleName
#endif
				);
#endif
				bRet = true;
			}
			
			if (bRet)
				return;
			
			ch->RemoveSpecifyItem(dwItemVnum1, 10);
			ch->RemoveSpecifyItem(dwItemVnum2, 10);
			ch->RemoveSpecifyItem(dwItemVnum3, 3);
			ch->GetNewPetSystem()->IncreasePetEvolution();
		}
		else {
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 730, "");
#endif
			return;
		}
	}
#if defined(ENABLE_TEXTS_RENEWAL)
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 729, "");
	}
#endif

}

#endif

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
ACMD(do_cube)
{

	const char *line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (0 == arg1[0])
	{
		return;
	}

	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			Cube_open(ch);
			break;

		default:
			return;
	}
}
#else
ACMD(do_cube)
{
	if (!ch->CanDoCube())
		return;

	int cube_index = 0, inven_index = 0;
#ifdef ENABLE_EXTRA_INVENTORY
	BYTE window_type = 0;
	char arg1[256], arg2[256], arg3[256], arg4[256];

	two_arguments(two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2)), arg3, sizeof(arg3), arg4, sizeof(arg4));
#else
	const char *line;
	char arg1[256], arg2[256], arg3[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));
#endif


	if (0 == arg1[0]) {
		return;
	}

	const std::string& strArg1 = std::string(arg1);

	// r_info (request information)
	// /cube r_info     ==> (Client -> Server) 현재 NPC가 만들 수 있는 레시피 요청
	//					    (Server -> Client) /cube r_list npcVNUM resultCOUNT 123,1/125,1/128,1/130,5
	//
	// /cube r_info 3   ==> (Client -> Server) 현재 NPC가 만들수 있는 레시피 중 3번째 아이템을 만드는 데 필요한 정보를 요청
	// /cube r_info 3 5 ==> (Client -> Server) 현재 NPC가 만들수 있는 레시피 중 3번째 아이템부터 이후 5개의 아이템을 만드는 데 필요한 재료 정보를 요청
	//					   (Server -> Client) /cube m_info startIndex count 125,1|126,2|127,2|123,5&555,5&555,4/120000@125,1|126,2|127,2|123,5&555,5&555,4/120000
	//
	if (strArg1 == "r_info")
	{
		if (0 == arg2[0])
			Cube_request_result_list(ch);
		else
		{
			if (isdigit(*arg2))
			{
				int listIndex = 0, requestCount = 1;
				str_to_number(listIndex, arg2);

				if (0 != arg3[0] && isdigit(*arg3))
					str_to_number(requestCount, arg3);

				Cube_request_material_info(ch, listIndex, requestCount);
			}
		}

		return;
	}

	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			Cube_open(ch);
			break;

		case 'c':	// close
			Cube_close(ch);
			break;

		case 'l':	// list
			Cube_show_list(ch);
			break;

		case 'a':	// add cue_index inven_index
			{
#ifdef ENABLE_EXTRA_INVENTORY
				if (arg2[0] == 0 || !isdigit(*arg2) || arg3[0] == 0 || !isdigit(*arg3) || arg4[0] == 0 || !isdigit(*arg4))
#else
				if (0 == arg2[0] || !isdigit(*arg2) ||
					0 == arg3[0] || !isdigit(*arg3))
#endif
					return;

				str_to_number(cube_index, arg2);
				str_to_number(inven_index, arg3);
#ifdef ENABLE_EXTRA_INVENTORY
				str_to_number(window_type, arg4);
				Cube_add_item(ch, cube_index, inven_index, window_type);
#else
				Cube_add_item (ch, cube_index, inven_index);
#endif
			}
			break;

		case 'd':	// delete
			{
				if (0 == arg2[0] || !isdigit(*arg2))
					return;

				str_to_number(cube_index, arg2);
				Cube_delete_item (ch, cube_index);
			}
			break;

		case 'm':	// make
			if (0 != arg2[0])
			{
				while (true == Cube_make(ch))
					sys_log(0, "cube make success");
			}
			else
				Cube_make(ch);
			break;

		default:
			return;
	}
}
#endif

#if !defined(ENABLE_INGAME_ITEMSHOP)
ACMD(do_in_game_mall)
{
	if (!ch)
		return;

	char buf[512+1];
	char sas[33];
	MD5_CTX ctx;
	const char sas_key[] = "24124fasfwgt3tg!@#$@!%SAF";
	
	char language[3];
	strcpy(language, "en");//If you have multilanguage, update this
	
	snprintf(buf, sizeof(buf), "%u%u%s", ch->GetPlayerID(), ch->GetAID(), sas_key);

	MD5Init(&ctx);
	MD5Update(&ctx, (const unsigned char *) buf, strlen(buf));
#ifdef __FreeBSD__
	MD5End(&ctx, sas);
#else
	static const char hex[] = "0123456789abcdef";
	unsigned char digest[16];
	MD5Final(digest, &ctx);
	int i;
	for (i = 0; i < 16; ++i) {
		sas[i+i] = hex[digest[i] >> 4];
		sas[i+i+1] = hex[digest[i] & 0x0f];
	}
	sas[i+i] = '\0';
#endif

	// snprintf(buf, sizeof(buf), "mall https://www.%s/shop?pid=%u&lang=%s&sid=%d&sas=%s",
			// g_strWebMallURL.c_str(), ch->GetPlayerID(), language, g_server_id, sas);
	snprintf(buf, sizeof(buf), "mall https://keynes2.online/shop/coins?pid=%u&lang=%s&sas=%s",
			ch->GetPlayerID(), language, sas);
	ch->ChatPacket(CHAT_TYPE_COMMAND, buf);

//	char buf[512+1];
//	char sas[33];
//	MD5_CTX ctx;
//	const char secretKey[] = "base64:vKqgEB0ho5Swmzh+bQTAmBoWpOk8z2yIFaxsIJMOzvE=";
//	const char websiteUrl[] = "https://wonder2.org";
//	snprintf(buf, sizeof(buf), "%u%s", ch->GetAID(), secretKey);
//	MD5Init(&ctx);
//	MD5Update(&ctx, (const unsigned char *) buf, strlen(buf));
//#ifdef __FreeBSD__
//	MD5End(&ctx, sas);
//#else
//	static const char hex[] = "0123456789abcdef";
//	unsigned char digest[16];
//	MD5Final(digest, &ctx);
//	int i;
//	for (i = 0; i < 16; ++i) {
//		sas[i+i] = hex[digest[i] >> 4];
//		sas[i+i+1] = hex[digest[i] & 0x0f];
//	}
//	
//	sas[i+i] = '\0';
//#endif
//
//#ifdef ENABLE_MULTI_LANGUAGE
//	BYTE lang = ch->GetDesc() ? ch->GetDesc()->GetLanguage() : 0;
//	std::string str_lang;
//	switch (lang) {
//		case LANGUAGE_RO: {
//			str_lang = "ro";
//			break;
//		}
//		case LANGUAGE_IT: {
//			str_lang = "it";
//			break;
//		}
//		case LANGUAGE_TR: {
//			str_lang = "tr";
//			break;
//		}
//		case LANGUAGE_DE: {
//			str_lang = "de";
//			break;
//		}
//		case LANGUAGE_PL: {
//			str_lang = "pl";
//			break;
//		}
//		case LANGUAGE_PT: {
//			str_lang = "pt";
//			break;
//		}
//		case LANGUAGE_ES: {
//			str_lang = "es";
//			break;
//		}
//		case LANGUAGE_CZ: {
//			str_lang = "cz";
//			break;
//		}
//		case LANGUAGE_HU: {
//			str_lang = "hu";
//			break;
//		}
//		default: {
//			str_lang = "en";
//			break;
//		}
//	}
//
//	snprintf(buf, sizeof(buf), "mall %s/in-game-shop?aid=%u&secret=%s&lang=%s", websiteUrl, ch->GetAID(), sas, str_lang.c_str());
//#else
//	snprintf(buf, sizeof(buf), "mall %s/in-game-shop?aid=%u&secret=%s", websiteUrl, ch->GetAID(), sas);
//#endif
//	ch->ChatPacket(CHAT_TYPE_COMMAND, buf);
// }
 #endif

// 주사위
ACMD(do_dice)
{
#if defined(ENABLE_TEXTS_RENEWAL)
	char arg1[256], arg2[256];
	int start = 1, end = 100;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (*arg1 && *arg2)
	{
		start = atoi(arg1);
		end = atoi(arg2);
	}
	else if (*arg1 && !*arg2)
	{
		start = 1;
		end = atoi(arg1);
	}

	end = MAX(start, end);
	start = MIN(start, end);

	int n = number(start, end);
	if (ch->GetParty()) {
		ch->GetParty()->ChatPacketToAllMemberNew(
#ifdef ENABLE_DICE_SYSTEM
		CHAT_TYPE_DICE_INFO
#else
		CHAT_TYPE_INFO
#endif
		, 544, "%s#%d#%d#%d", ch->GetName(), n, start, end);
	} else {
		ch->ChatPacketNew(
#ifdef ENABLE_DICE_SYSTEM
		CHAT_TYPE_DICE_INFO
#else
		CHAT_TYPE_INFO
#endif
		, 545, "%d#%d#%d", n, start, end);
	}
#endif
}

ACMD(do_click_safebox)
{
	if (ch->GetDungeon() || ch->GetWarMap())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 731, "");
#endif
		return;
	}

	ch->SetSafeboxOpenPosition();
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeSafeboxPassword");
}
ACMD(do_force_logout)
{
	LPDESC pDesc=DESC_MANAGER::instance().FindByCharacterName(ch->GetName());
	if (!pDesc)
		return;
	pDesc->DelayedDisconnect(0);
}

ACMD(do_click_mall)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeMallPassword");
}

ACMD(do_ride)
{
	if (ch->IsDead() || 
		ch->IsStun() || 
		ch->IsObserverMode() ||
		ch->IsWearingDress())
	{
		return;
	}

	if (ch->GetLastAttackPulse() > get_global_time())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Asteapta %d secunde pentru a calari.", (ch->GetLastAttackPulse() - get_global_time()));
		return;
	}

#ifdef ENABLE_PVP_ADVANCED	
	if ((ch->GetDuel("BlockRide")))
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 516, "");
		return;
	}
#endif

	if (ch->IsHorseRiding())
	{
		ch->StopRiding();
		return;
	}

	if (ch->GetMountVnum())
	{
		do_unmount(ch, NULL, 0, 0);
		return;
	}

	if (ch->GetHorse() != NULL)
	{
		ch->StartRiding();
		return;
	}
	
	LPITEM pItem = nullptr;
	for (uint16_t i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		pItem = ch->GetInventoryItem(i);
		if (!pItem)
		{
			continue;
		}

		if (pItem->IsRideItem())
		{
			ch->UseItem(TItemPos(INVENTORY, i));
			return;
		}
	}

#if defined(ENABLE_TEXTS_RENEWAL)
	ch->ChatPacketNew(CHAT_TYPE_INFO, 5, "");
#endif
}

#ifdef __ENABLE_RANGE_ALCHEMY__
ACMD(do_extend_range_npc)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	
	if (!*arg1)
		return;
	
	
	DWORD vnum = 0;
	str_to_number(vnum, arg1);
	
	if (ch->IsDead())
		return;
	
	if (ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || ch->IsOpenSafebox() || ch->IsCubeOpen()
#ifdef __ENABLE_NEW_OFFLINESHOP__
		|| ch->GetOfflineShopGuest() || ch->GetAuctionGuest()
#endif
	)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 735, "");
#endif
		return;
	}
	
	LPSHOP shop = CShopManager::instance().Get(vnum);
	
	if(!shop)
		return;
	
	ch->SetShopOwner(ch);
	shop->AddGuest(ch, 0, false);
	
}
#endif



#ifdef __ENABLE_REFINE_ALCHEMY__
ACMD(do_refine_window_alchemy) {
	ch->DragonSoul_RefineWindow_Open(ch);
}
#endif

#ifdef ENABLE_EVENT_MANAGER
ACMD(do_event_manager)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "info")
	{
		CHARACTER_MANAGER::Instance().SendDataPlayer(ch);
	}
	else if (vecArgs[1] == "remove")
	{
		if (!ch->IsGM())
			return;

		if (vecArgs.size() < 3) { 
			
			ch->ChatPacket(CHAT_TYPE_INFO, "put the event index!!");
			return; 
		}

		BYTE removeIndex;
		str_to_number(removeIndex, vecArgs[2].c_str());

		if(CHARACTER_MANAGER::Instance().CloseEventManuel(removeIndex))
			ch->ChatPacket(CHAT_TYPE_INFO, "successfuly remove!");
		else
			ch->ChatPacket(CHAT_TYPE_INFO, "dont has any event!");
	}
	else if (vecArgs[1] == "update")
	{
		if (!ch->IsGM())
			return;
		const BYTE subHeader = EVENT_MANAGER_UPDATE;
		//db_clientdesc->DBPacketHeader(HEADER_GD_EVENT_MANAGER, 0, sizeof(BYTE));
		//db_clientdesc->Packet(&subHeader, sizeof(BYTE));
		db_clientdesc->DBPacket(HEADER_GD_EVENT_MANAGER, 0, &subHeader, sizeof(BYTE));

		ch->ChatPacket(CHAT_TYPE_INFO, "successfully update!");
	}
}
#endif

#ifdef ENABLE_REWARD_SYSTEM
ACMD(do_update_reward_data)
{
	CHARACTER_MANAGER::Instance().SendRewardInfo(ch);
}
#endif

#if defined(ENABLE_INGAME_ITEMSHOP)
ACMD(do_in_game_buycoins)
{    
    char buf[512+1];
    char sas[33];
    MD5_CTX ctx;

    const char secretKey[] = "base64:mSC3kg7/kknQMy7I/BkoPh9wHqTKBfSkNBKK22eV0yI="; // change with the one in the .ENV file
    const char websiteUrl[] = "https://keynes2.online"; // WITHOUT "/" at the end!!!

    snprintf(buf, sizeof(buf), "%u%s", ch->GetAID(), secretKey);

    MD5Init(&ctx);
    MD5Update(&ctx, (const unsigned char *) buf, strlen(buf));
#ifdef __FreeBSD__
    MD5End(&ctx, sas);
#else
    static const char hex[] = "0123456789abcdef";
    unsigned char digest[16];
    MD5Final(digest, &ctx);
    int i;
    for (i = 0; i < 16; ++i) {
        sas[i+i] = hex[digest[i] >> 4];
        sas[i+i+1] = hex[digest[i] & 0x0f];
    }
    sas[i+i] = '\0';
#endif

    snprintf(buf, sizeof(buf), "buycoins %s/in-game-shop?aid=%u&secret=%s", websiteUrl, ch->GetAID(), sas);

    ch->ChatPacket(CHAT_TYPE_COMMAND, buf);
}

ACMD(do_ishop)
{
    std::vector<std::string> vecArgs;
    split_argument(argument, vecArgs);
    if (vecArgs.size() < 2) {
        return;
    }

    else if (vecArgs[1] == "data")
    {
        if (ch->GetProtectTime("itemshop.load") == 1)
            return;
        ch->SetProtectTime("itemshop.load", 1);
        if (vecArgs.size() < 3) { return; }
        int updateTime;
        str_to_number(updateTime, vecArgs[2].c_str());
        CHARACTER_MANAGER::Instance().LoadItemShopData(ch, CHARACTER_MANAGER::Instance().GetItemShopUpdateTime() != updateTime);
    }
    else if (vecArgs[1] == "log")
    {
        if (ch->GetProtectTime("itemshop.log") == 1)
            return;
        ch->SetProtectTime("itemshop.log", 1);

        CHARACTER_MANAGER::Instance().LoadItemShopLog(ch);
    }
    else if (vecArgs[1] == "buy")
    {
        if (vecArgs.size() < 4) { return; }
        int itemID;
        str_to_number(itemID, vecArgs[2].c_str());
        int itemCount;
        str_to_number(itemCount, vecArgs[3].c_str());
        CHARACTER_MANAGER::Instance().LoadItemShopBuy(ch, itemID, itemCount);
    }
}
#endif

#if defined(ENABLE_HIDE_COSTUME_SYSTEM)
ACMD(do_hide_costume) {
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1) {
		return;
	}

	uint8_t part = 0;
	str_to_number(part, arg1);

	if (part >= HIDE_PART_MAX) {
		return;
	}

	ch->SetPartStatus(part);
}
#endif

#if defined(ENABLE_TRACK_WINDOW)
ACMD(do_track_window) {
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);

	if (vecArgs.size() < 2) {
		return;
	}
	else if (vecArgs[1] == "load") {
		if (ch->GetProtectTime("track_dungeon") == 1) {
			return;
		}

		ch->GetDungeonCooldown(WORD_MAX);
		for (uint8_t i = 2; i < vecArgs.size(); ++i) {
			WORD globalBossID;
			if (!str_to_number(globalBossID, vecArgs[i].c_str())) {
				continue;
			}

			CNewMobTimer::Instance().GetTrackData(ch, globalBossID);
		}

		ch->SetProtectTime("track_dungeon", 1);
	} else if (vecArgs[1] == "reenter") {
		if (!ch->IsGM()) {
			return;
		}

		if (vecArgs.size() < 4) {
			return;
		}

		WORD testVnum;
		if (!str_to_number(testVnum, vecArgs[2].c_str())) {
			return;
		}

		int32_t testTime;
		if (!str_to_number(testTime, vecArgs[3].c_str())) {
			return;
		}

		ch->GetDungeonCooldownTest(testVnum, testTime, false);
	} else if (vecArgs[1] == "cooldown") {
		if (!ch->IsGM()) {
			return;
		}

		if (vecArgs.size() < 4) {
			return;
		}

		WORD testVnum;
		if (!str_to_number(testVnum, vecArgs[2].c_str())) {
			return;
		}

		int testTime;
		if (!str_to_number(testTime, vecArgs[3].c_str())) {
			return;
		}

		ch->GetDungeonCooldownTest(testVnum, testTime, true);
	} else if (vecArgs[1] == "teleport") {
		if (vecArgs.size() < 3) {
			return;
		}

		WORD mobIndex;
		if (!str_to_number(mobIndex, vecArgs[2].c_str())) {
			return;
		}

		std::map<WORD, std::pair<std::pair<long, long>,std::pair<WORD, BYTE>>> m_TeleportData = {
			//{mobindex - {{X, Y}, {PORT, MINLVL}}},
			{4902, { {24576 + 234, 35840 + 200}, {0, 50} }},
			{4900, { {24576 + 234, 35840 + 200}, {0, 50} }},
			{4903, { {24576 + 234, 35840 + 200}, {0, 50} }},
/*
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
			{4903, { {24576 + 234, 35840 + 200}, {0, 75} }},
#endif
*/
		};

		const auto it = m_TeleportData.find(mobIndex);
		if (it != m_TeleportData.end()) {
			if (ch->GetLevel() < it->second.second.second) {
#if defined(ENABLE_TEXTS_RENEWAL)
				ch->ChatPacketNew(CHAT_TYPE_INFO, 1591, "%d", it->second.second.second);
#else
				ch->ChatPacket(CHAT_TYPE_INFO, "You need to be at least level %d.", it->second.second.second);
#endif
				return;
			}

			if (!ch->CanWarp()) {
#if defined(ENABLE_TEXTS_RENEWAL)
				ch->ChatPacketNew(CHAT_TYPE_INFO, 1590, "");
#else
				ch->ChatPacket(CHAT_TYPE_INFO, "You cannot be teleporter right now, try again in 10 seconds!");
#endif
				return;
			}

			ch->WarpSet(it->second.first.first * 100, it->second.first.second * 100, it->second.second.first);
		}
	}
}
#endif

#if defined(ENABLE_OKEY_CARD_GAME)
ACMD(do_cards)
{
	const char *line;
	char arg1[256], arg2[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	switch (LOWER(arg1[0]))
	{
	case 'o':
	{
		if (isdigit(*arg2))
		{
			uint32_t myArg;
			str_to_number(myArg, arg2);
			ch->Cards_open(myArg);
		}
		break;
	}
	case 'p':
	{
		ch->Cards_pullout();
		break;
	}
	case 'e':
	{
		ch->CardsEnd();
		break;
	}
	case 'd':
	{
		if (isdigit(*arg2))
		{
			uint32_t myArg;
			str_to_number(myArg, arg2);
			ch->CardsDestroy(myArg);
		}
		break;
	}
	case 'a':
	{
		if (isdigit(*arg2))
		{
			uint32_t myArg;
			str_to_number(myArg, arg2);
			ch->CardsAccept(myArg);
		}
		break;
	}
	case 'r':
	{
		if (isdigit(*arg2))
		{
			uint32_t myArg;
			str_to_number(myArg, arg2);
			ch->CardsRestore(myArg);
		}
		break;
	}
	default:
	{
		break;
	}
	}
}
#endif

#if defined(USE_WHEEL_OF_FORTUNE)
ACMD(do_wheel_of_fortune)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);

	if (!ch->IsPC())
	{
		return;
	}

	if (vecArgs.size() < 2)
	{
		return;
	}
	else if (vecArgs[1] == "start")
	{
		if (ch->IsDead() ||
			ch->IsStun() ||
			ch->IsHack())
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1717, "");
#else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot spin the Wheel of Fortune in this moment."));
#endif
			return;
		}

		if (!ch->CanWarp())
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1718, "");
#else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Try again in few seconds."));
#endif
			return;
		}

#if defined(ENABLE_ANTI_FLOOD)
		uint8_t limit = 15;
		if (thecore_pulse() < ch->GetAntiFloodPulse(FLOOD_WHEEL) + limit)
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1719, "%u", limit);
#else
			ch->ChatPacket(CHAT_TYPE_INFO, "You can do this each %u milliseconds.", limit);
#endif
			return;
		}

		ch->SetAntiFloodPulse(FLOOD_WHEEL, thecore_pulse());
#endif

		if (ch->GetProtectTime("WheelWorking") != 0)
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1720, "");
#else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The Wheel of Fortune is already spining, please wait!"));
#endif
			return;
		}

		uint64_t dragonCoin = ch->GetDragonCoin(true);

		if (dragonCoin < WHEEL_SPIN_COINS_COST)
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1721, "%u", WHEEL_SPIN_COINS_COST);
#else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need %u coins to spin the Wheel of Fortune!"), WHEEL_SPIN_COINS_COST);
#endif
			return;
		}

		ch->SetDragonCoinMinus(static_cast<int32_t>(WHEEL_SPIN_COINS_COST));

		std::vector<std::pair<long, long>> m_important_item = {
			{70611, 1},
			{89005, 1},
			{59001, 1},
			{50514, 1},
			{50136, 1},
			{40004, 1},
			{51002, 1},
		};

		std::map<std::pair<long, long>, int> m_normal_item = {
			{{80008,1}, 30},
			{{50259,250}, 30},
			{{90027,5000}, 30},
			{{90027,2500}, 30},
			{{86062,10}, 30},
			{{86062,25}, 30},
			{{50512,1}, 30},
			{{90026,3}, 30},
			{{71057,1}, 30},
			{{59998,2}, 30},
			{{50203,2}, 30},
			{{30806,2}, 30},
			{{70403,1}, 30},
			{{72326,1}, 30},
			{{70604,2}, 30},
			{{70605,2}, 30},
			{{70606,2}, 30},
			{{72346,3}, 30},
			{{72067,5}, 30},
			{{70401,50}, 30},
			{{72349,5}, 30},
			{{59995,5}, 30},
			{{50926,250}, 30},
			{{40143,5}, 30},
			{{164406,5}, 30}
		};

		std::vector<std::pair<long, long>> m_send_items;
		if (m_important_item.size())
		{
			size_t random = number(0, m_important_item.size()-1);
			m_send_items.emplace_back(m_important_item[random].first, m_important_item[random].second);
		}

		while (true)
		{
			for (auto it = m_normal_item.begin(); it != m_normal_item.end(); ++it)
			{
				uint8_t randomEx = number(0,4);
				if (randomEx == 4)
				{
					uint8_t random = number(0,100);
					if (it->second >= random)
					{
						auto itFind = std::find(m_send_items.begin(), m_send_items.end(), it->first);
						if (itFind == m_send_items.end())
						{
							m_send_items.emplace_back(it->first.first, it->first.second);
							if (m_send_items.size() >= 10)
							{
								break;
							}
						}
					}
				}
			}

			if (m_send_items.size() >= 10)
			{
				break;
			}
		}

		std::string cmd_wheel = "";
		if (m_send_items.size())
		{
			for (auto it = m_send_items.begin(); it != m_send_items.end(); ++it)
			{
				cmd_wheel += std::to_string(it->first);
				cmd_wheel += "|";
				cmd_wheel += std::to_string(it->second);
				cmd_wheel += "#";
			}
		}

		uint8_t luckyWheel = number(0, 9);
		if (luckyWheel == 0)
		{
			if (number(0,1) == 0)
			{
				luckyWheel = number(0, 9);
			}
		}

		ch->SetProtectTime("WheelLuckyIndex", luckyWheel);
		ch->SetProtectTime("WheelLuckyItemVnum", m_send_items[luckyWheel].first);
		ch->SetProtectTime("WheelLuckyItemCount", m_send_items[luckyWheel].second);

		ch->SetProtectTime("WheelWorking", 1);

		ch->ChatPacket(CHAT_TYPE_COMMAND, "SetItemData %s", cmd_wheel.c_str());
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OnSetWhell %d", luckyWheel);
	}
	else if (vecArgs[1] == "done")
	{
		if (ch->GetProtectTime("WheelWorking") == 0)
		{
			return;
		}

		ch->AutoGiveItem(ch->GetProtectTime("WheelLuckyItemVnum"), ch->GetProtectTime("WheelLuckyItemCount"));

		ch->ChatPacket(CHAT_TYPE_COMMAND, "GetGiftData %d %d", ch->GetProtectTime("WheelLuckyItemVnum"), ch->GetProtectTime("WheelLuckyItemCount"));

		ch->SetProtectTime("WheelLuckyIndex", 0);
		ch->SetProtectTime("WheelLuckyItemVnum", 0);
		ch->SetProtectTime("WheelLuckyItemCount", 0);
		ch->SetProtectTime("WheelWorking", 0);
	}
}
#endif

#ifdef USE_AUTO_HUNT
ACMD(do_auto_onoff)
{
    std::vector<std::string> vecArgs;
    split_argument(argument, vecArgs);

    if (vecArgs.size() < 7)
    {
        return;
    }

    uint8_t bStart, bAttack, bMetinStones, bRange, bPotion, bSkill, bRestart;

    str_to_number(bStart, vecArgs[1].c_str());
    str_to_number(bAttack, vecArgs[2].c_str());
    str_to_number(bMetinStones, vecArgs[3].c_str());
    str_to_number(bRange, vecArgs[4].c_str());
    str_to_number(bPotion, vecArgs[5].c_str());
    str_to_number(bSkill, vecArgs[6].c_str());
    str_to_number(bRestart, vecArgs[7].c_str());

    ch->SetAutoUseType(AUTO_ONOFF_START, bStart == 0 ? false : true);
    ch->SetAutoUseType(AUTO_ONOFF_ATTACK, bAttack == 0 ? false : true);
    ch->SetAutoUseType(AUTO_ONOFF_METINSTONES, bMetinStones == 0 ? false : true);
    ch->SetAutoUseType(AUTO_ONOFF_RANGE, bRange == 0 ? false : true);

    if (ch->GetPremiumRemainSeconds(PREMIUM_AUTO_USE) > 0)
    {
        ch->SetAutoUseType(AUTO_ONOFF_POTION, bPotion == 0 ? false : true);
        ch->SetAutoUseType(AUTO_ONOFF_SKILL, bSkill == 0 ? false : true);
        ch->SetAutoUseType(AUTO_ONOFF_RESTART, bRestart == 0 ? false : true);
    }
    else
    {
        for (uint8_t i = AUTO_ONOFF_POTION; i < AUTO_ONOFF_MAX; ++i)
        {
            ch->SetAutoUseType(i, false);
        }
    }

    ch->ChatPacket(CHAT_TYPE_COMMAND, "auto_onoff %u", bStart);
}
#endif

#ifdef USE_PICKUP_FILTER
ACMD(do_filter)
{
#ifdef USE_AUTO_HUNT
    if (ch->GetPremiumRemainSeconds(PREMIUM_AUTO_USE) < 1)
    {
        return;
    }
#endif

    std::vector<std::string> vecArgs;
    split_argument(argument, vecArgs);

    if (vecArgs.size() < 2)
    {
        return;
    }

    if (vecArgs[1] == "clear")
    {
        ch->ClearFilterTable();
    }
    else if (vecArgs[1] == "unset")
    {
        if (vecArgs.size() < 3)
        {
            return; 
        }

        uint8_t bType;
        if (!str_to_number(bType, vecArgs[2].c_str()))
        {
            return;
        }

        ch->SetFilterStatus(bType, false);
    }
    else if (vecArgs[1] == "set")
    {
        uint8_t bIdx;
        if (!str_to_number(bIdx, vecArgs[2].c_str()))
        {
            return;
        }

        if (bIdx == FILTER_TYPE_AUTO)
        {
            if (vecArgs.size() < 3)
            {
                return; 
            }

            uint8_t bAutoPickup;
            if (!str_to_number(bAutoPickup, vecArgs[3].c_str()))
            {
                return;
            }

            ch->SetFilterStatus(bIdx, bAutoPickup == 0 ? false : true);
            return;
        }

        if (vecArgs.size() < 9)
        {
            return; 
        }

        if (bIdx >= FILTER_TYPE_MAX)
        {
            return;
        }

        uint8_t bJobNeed;
        if (!str_to_number(bJobNeed, vecArgs[3].c_str()))
        {
            return;
        }

        uint8_t bRefineNeed;
        if (!str_to_number(bRefineNeed, vecArgs[4].c_str()))
        {
            return;
        }

        uint8_t bLevelNeed;
        if (!str_to_number(bLevelNeed, vecArgs[5].c_str()))
        {
            return;
        }

        TFilterTable t;
        t.bJobNeed = bJobNeed == 1 ? true : false;
        t.bRefineNeed = bRefineNeed == 1 ? true : false;
        t.bLevelNeed = bLevelNeed == 1 ? true : false;

        std::vector<std::string> vecJobs;
        split_argument(vecArgs[6].c_str(), vecJobs, "-");

        if (vecJobs.size() != 4)
        {
            return;
        }

        for (uint8_t j = 0; j < 4; ++j)
        {
            uint8_t bJob;
            if (!str_to_number(bJob, vecJobs[j].c_str()))
            {
                return;
            }

            t.bJobs[j] = bJob == 1 ? true : false;
        }

        std::vector<std::string> vecRefine;
        split_argument(vecArgs[7].c_str(), vecRefine, "-");

        if (vecRefine.size() != 2)
        {
            return;
        }

        std::vector<std::string> vecLevel;
        split_argument(vecArgs[8].c_str(), vecLevel, "-");

        if (vecLevel.size() != 2)
        {
            return;
        }

        for (uint8_t j = 0; j < 2; ++j)
        {
            if (!str_to_number(t.bRefine[j], vecRefine[j].c_str()) || !str_to_number(t.bLevel[j], vecLevel[j].c_str()))
            {
                return;
            }
        }

        memset(&t.bSubs, true, sizeof(t.bSubs));

        if (vecArgs.size() == 10)
        {
            std::vector<std::string> vecSubStatus;
            split_argument(vecArgs[9].c_str(), vecSubStatus, "-");

            for (uint16_t j = 0; j < vecSubStatus.size(); ++j)
            {
                uint8_t bStatus;
                if (!str_to_number(bStatus, vecSubStatus[j].c_str()))
                {
                    break;
                }

                if (bStatus != 1)
                {
                    t.bSubs[j] = false;
                }
            }
        }

        ch->AddFilterTable(bIdx, t);
    }
}
#endif

#ifdef USE_AUTO_AGGREGATE
ACMD(do_auto_aggregate)
{
    ch->GetBraveCapeCMDCompare(argument);
}
#endif
