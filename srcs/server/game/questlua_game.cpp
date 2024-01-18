#include "stdafx.h"
#include "questlua.h"
#include "questmanager.h"
#include "desc_client.h"
#include "char.h"
#include "item_manager.h"
#include "item.h"
#include "cmd.h"
#include "packet.h"
#ifdef ENABLE_EVENT_MANAGER
#include "char_manager.h"
#endif
#ifdef ENABLE_DICE_SYSTEM
#include "party.h"
#endif

#undef sys_err
#if !defined(_WIN32)
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

#if !defined(ENABLE_INGAME_ITEMSHOP)
extern ACMD(do_in_game_mall);
#endif

namespace quest
{
	int game_set_event_flag(lua_State* L)
	{
		CQuestManager & q = CQuestManager::instance();

		if (lua_isstring(L,1) && lua_isnumber(L, 2))
			q.RequestSetEventFlag(lua_tostring(L,1), (int)lua_tonumber(L,2));

		return 0;
	}

	int game_get_event_flag(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (lua_isstring(L,1))
			lua_pushnumber(L, q.GetEventFlag(lua_tostring(L,1)));
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int game_request_make_guild(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDESC d = q.GetCurrentCharacterPtr()->GetDesc();
		if (d)
		{
			BYTE header = HEADER_GC_REQUEST_MAKE_GUILD;
			d->Packet(&header, 1);
		}
		return 0;
	}

	int game_get_safebox_level(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		lua_pushnumber(L, q.GetCurrentCharacterPtr()->GetSafeboxSize()/SAFEBOX_PAGE_SIZE);
		return 1;
	}

	int game_set_safebox_level(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		//q.GetCurrentCharacterPtr()->ChangeSafeboxSize(3*(int)lua_tonumber(L,-1));
		TSafeboxChangeSizePacket p;
		p.dwID = q.GetCurrentCharacterPtr()->GetDesc()->GetAccountTable().id;
		p.bSize = (int)lua_tonumber(L,-1);
		db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_CHANGE_SIZE,  q.GetCurrentCharacterPtr()->GetDesc()->GetHandle(), &p, sizeof(p));

		q.GetCurrentCharacterPtr()->SetSafeboxSize(SAFEBOX_PAGE_SIZE * (int)lua_tonumber(L,-1));
		return 0;
	}

	int game_open_safebox(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		ch->SetSafeboxOpenPosition();
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeSafeboxPassword");
		return 0;
	}

	int game_open_mall(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		ch->SetSafeboxOpenPosition();
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeMallPassword");
		return 0;
	}

	int game_drop_item(lua_State* L)
	{
		//
		// Syntax: game.drop_item(50050, 1)
		//
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		DWORD item_vnum = (DWORD) lua_tonumber(L, 1);
		int count = (int) lua_tonumber(L, 2);
		long x = ch->GetX();
		long y = ch->GetY();

		LPITEM item = ITEM_MANAGER::instance().CreateItem(item_vnum, count);

		if (!item)
		{
			sys_err("cannot create item vnum %d count %d", item_vnum, count);
			return 0;
		}

		PIXEL_POSITION pos;
		pos.x = x + number(-200, 200);
		pos.y = y + number(-200, 200);

		item->AddToGround(ch->GetMapIndex(), pos);
		item->StartDestroyEvent();

		return 0;
	}

	int game_drop_item_with_ownership(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		LPITEM item = NULL;
		switch (lua_gettop(L))
		{
		case 1:
			item = ITEM_MANAGER::instance().CreateItem((DWORD) lua_tonumber(L, 1));
			break;
		case 2:
		case 3:
			item = ITEM_MANAGER::instance().CreateItem((DWORD) lua_tonumber(L, 1), (int) lua_tonumber(L, 2));
			break;
		default:
			return 0;
		}

		if ( item == NULL )
		{
			return 0;
		}

#ifdef USE_PICKUP_FILTER
        if (ch->GetFilterStatus(FILTER_TYPE_AUTO) && ch->CanPickItem(item))
        {
            ch->AutoGiveItem(item, false
#ifdef __HIGHLIGHT_SYSTEM__
, true
#endif
, true
);

            return 0;
        }
#endif

		if (lua_isnumber(L, 3))
		{
			int sec = (int) lua_tonumber(L, 3);
			if (sec <= 0)
			{
				item->SetOwnership( ch );
			}
			else
			{
				item->SetOwnership( ch, sec );
			}
		}
		else
			item->SetOwnership( ch );

		PIXEL_POSITION pos;
		pos.x = ch->GetX() + number(-200, 200);
		pos.y = ch->GetY() + number(-200, 200);

		item->AddToGround(ch->GetMapIndex(), pos);
		item->StartDestroyEvent();

		return 0;
	}

#ifdef ENABLE_DICE_SYSTEM
	int game_drop_item_with_ownership_and_dice(lua_State* L)
	{
		LPITEM item = NULL;
		switch (lua_gettop(L))
		{
		case 1:
			item = ITEM_MANAGER::instance().CreateItem((DWORD) lua_tonumber(L, 1));
			break;
		case 2:
		case 3:
			item = ITEM_MANAGER::instance().CreateItem((DWORD) lua_tonumber(L, 1), (int) lua_tonumber(L, 2));
			break;
		default:
			return 0;
		}

		if ( item == NULL )
		{
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (ch->GetParty())
		{
			FPartyDropDiceRoll f(item, ch);
			f.Process(NULL);
		}

#ifdef USE_PICKUP_FILTER
        if (ch->GetFilterStatus(FILTER_TYPE_AUTO) && ch->CanPickItem(item))
        {
            ch->AutoGiveItem(item, false
#ifdef __HIGHLIGHT_SYSTEM__
, true
#endif
, true
);

            return 0;
        }
#endif

		if (lua_isnumber(L, 3))
		{
			int sec = (int) lua_tonumber(L, 3);
			if (sec <= 0)
			{
				item->SetOwnership( ch );
			}
			else
			{
				item->SetOwnership( ch, sec );
			}
		}
		else
			item->SetOwnership( ch );

		PIXEL_POSITION pos;
		pos.x = ch->GetX() + number(-200, 200);
		pos.y = ch->GetY() + number(-200, 200);

		item->AddToGround(ch->GetMapIndex(), pos);
		item->StartDestroyEvent();

		return 0;
	}
#endif

	int game_web_mall(lua_State* L)
	{
#if !defined(ENABLE_INGAME_ITEMSHOP)
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		
		if ( ch != NULL )
		{
			do_in_game_mall(ch, const_cast<char*>(""), 0, 0);
		}
#endif
		return 0;
	}

#ifdef ENABLE_EVENT_MANAGER
	int game_check_event(lua_State* L)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
		{
			lua_pushboolean(L, false);
			return 1;
		}
		auto it = CHARACTER_MANAGER::instance().CheckEventIsActive(lua_tonumber(L, 1), lua_tonumber(L, 2));
		lua_pushboolean(L, it != NULL);
		return 1;
	}
#endif
#ifdef ENABLE_REWARD_SYSTEM
	int game_set_reward(lua_State* L)
	{
		CQuestManager& qMgr = CQuestManager::instance();
		LPCHARACTER pkChar = qMgr.GetCurrentCharacterPtr();
		if (pkChar)
		{
			if (!lua_isnumber(L, 1))
				return 0;
			CHARACTER_MANAGER::instance().SetRewardData(lua_tonumber(L, 1), pkChar->GetName(), true);
		}
		return 0;
	}
#endif

#if defined(ENABLE_AURA_SYSTEM)
	int game_open_aura_absorb_window(lua_State* L) {
		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (NULL == ch) {
			sys_err("NULL POINT ERROR");
			return 0;
		}

		ch->OpenAuraRefineWindow(CQuestManager::instance().GetCurrentNPCCharacterPtr(), AURA_WINDOW_TYPE_ABSORB);
		return 0;
	}

	int game_open_aura_growth_window(lua_State* L) {
		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (NULL == ch) {
			sys_err("NULL POINT ERROR");
			return 0;
		}

		ch->OpenAuraRefineWindow(CQuestManager::instance().GetCurrentNPCCharacterPtr(), AURA_WINDOW_TYPE_GROWTH);
		return 0;
	}

	int game_open_aura_evolve_window(lua_State* L) {
		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (NULL == ch) {
			sys_err("NULL POINT ERROR");
			return 0;
		}

		ch->OpenAuraRefineWindow(CQuestManager::instance().GetCurrentNPCCharacterPtr(), AURA_WINDOW_TYPE_EVOLVE);
		return 0;
	}
#endif

#if defined(ENABLE_HALLOWEEN_EVENT_2022) && defined(ENABLE_EVENT_MANAGER)
	int game_get_halloween_event_status(lua_State* L)
	{
		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (NULL == ch) {
			sys_err("Character pointer is NULL.");
			return 0;
		}

		lua_pushnumber(L, CHARACTER_MANAGER::Instance().CheckEventIsActive(HALLOWEEN_EVENT) == NULL ? 0 : 1);
		return 1;
	}
#endif

#if defined(ENABLE_OKEY_CARD_GAME) && defined(ENABLE_EVENT_MANAGER)
	int game_get_okay_event_status(lua_State* L)
	{
		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (ch == NULL)
		{
			sys_err("Character pointer is NULL.");
			return 0;
		}

		lua_pushnumber(L, CHARACTER_MANAGER::Instance().CheckEventIsActive(OKEY_CARD_EVENT) == NULL ? 0 : 1);
		return 1;
	}
#endif

	void RegisterGameFunctionTable()
	{
		luaL_reg game_functions[] =
		{
#if defined(ENABLE_OKEY_CARD_GAME) && defined(ENABLE_EVENT_MANAGER)
			{"get_okay_event_status", game_get_okay_event_status},
#endif
#if defined(ENABLE_HALLOWEEN_EVENT_2022) && defined(ENABLE_EVENT_MANAGER)
			{"get_halloween_event_status", game_get_halloween_event_status},
#endif
#if defined(ENABLE_AURA_SYSTEM)
			{"open_aura_absorb_window", game_open_aura_absorb_window},
			{"open_aura_growth_window", game_open_aura_growth_window},
			{"open_aura_evolve_window", game_open_aura_evolve_window},
#endif
			{ "get_safebox_level",			game_get_safebox_level			},
			{ "request_make_guild",			game_request_make_guild			},
			{ "set_safebox_level",			game_set_safebox_level			},
			{ "open_safebox",				game_open_safebox				},
			{ "open_mall",					game_open_mall					},
			{ "get_event_flag",				game_get_event_flag				},
			{ "set_event_flag",				game_set_event_flag				},
			{ "drop_item",					game_drop_item					},
			{ "drop_item_with_ownership",	game_drop_item_with_ownership	},
#ifdef ENABLE_DICE_SYSTEM
			{ "drop_item_with_ownership_and_dice",	game_drop_item_with_ownership_and_dice	},
#endif
			{ "open_web_mall",				game_web_mall					},
#ifdef ENABLE_EVENT_MANAGER
			{ "check_event",		game_check_event			},
#endif
#ifdef ENABLE_REWARD_SYSTEM
			{ "set_reward",		game_set_reward			},
#endif
			{ NULL,					NULL				}
		};

		CQuestManager::instance().AddLuaFunctionTable("game", game_functions);
	}
}

