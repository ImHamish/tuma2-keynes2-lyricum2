#include "stdafx.h"

#include "questlua.h"
#include "questmanager.h"
#include "desc_client.h"
#include "char.h"
#include "char_manager.h"
#include "utils.h"
#include "guild.h"
#include "guild_manager.h"

#undef sys_err
#if !defined(_WIN32)
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

namespace quest
{
	//
	// "guild" Lua functions
	//
	int guild_around_ranking_string(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		if (!ch->GetGuild())
			lua_pushstring(L,"");
		else
		{
			char szBuf[4096+1];
			CGuildManager::instance().GetAroundRankString(ch->GetGuild()->GetID(), szBuf, sizeof(szBuf));
			lua_pushstring(L, szBuf);
		}
		return 1;
	}

	int guild_high_ranking_string(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		DWORD dwMyGuild = 0;
		if (ch->GetGuild())
			dwMyGuild = ch->GetGuild()->GetID();

		char szBuf[4096+1];
		CGuildManager::instance().GetHighRankString(dwMyGuild, szBuf, sizeof(szBuf));
		lua_pushstring(L, szBuf);
		return 1;
	}

	int guild_get_ladder_point(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		if (!ch->GetGuild())
		{
			lua_pushnumber(L, -1);
		}
		else
		{
			lua_pushnumber(L, ch->GetGuild()->GetLadderPoint());
		}
		return 1;
	}

	int guild_get_rank(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		if (!ch->GetGuild())
		{
			lua_pushnumber(L, -1);
		}
		else
		{
			lua_pushnumber(L, CGuildManager::instance().GetRank(ch->GetGuild()));
		}
		return 1;
	}

	int guild_is_war(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (ch->GetGuild() && ch->GetGuild()->UnderWar((DWORD) lua_tonumber(L, 1)))
			lua_pushboolean(L, true);
		else
			lua_pushboolean(L, false);

		return 1;
	}

	int guild_name(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			return 0;
		}

		CGuild * pkGuild = CGuildManager::instance().FindGuild((DWORD) lua_tonumber(L, 1));

		if (pkGuild)
			lua_pushstring(L, pkGuild->GetName());
		else
			lua_pushstring(L, "");

		return 1;
	}

	int guild_level(lua_State* L)
	{
		luaL_checknumber(L, 1);

		CGuild * pkGuild = CGuildManager::instance().FindGuild((DWORD) lua_tonumber(L, 1));

		if (pkGuild)
			lua_pushnumber(L, pkGuild->GetLevel());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int guild_war_enter(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			return 0;
		}

		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		if (ch->GetGuild())
			ch->GetGuild()->GuildWarEntryAccept((DWORD) lua_tonumber(L, 1), ch);

		return 0;
	}

	int guild_get_any_war(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (ch->GetGuild())
			lua_pushnumber(L, ch->GetGuild()->UnderAnyWar());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int guild_get_name(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			lua_pushstring(L,  "");
			return 1;
		}

		CGuild * pkGuild = CGuildManager::instance().FindGuild((DWORD) lua_tonumber(L, 1));

		if (pkGuild)
			lua_pushstring(L, pkGuild->GetName());
		else
			lua_pushstring(L, "");

		return 1;
	}

	int guild_war_bet(lua_State* L)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("invalid argument");
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		TPacketGDGuildWarBet p;

		p.dwWarID = (DWORD) lua_tonumber(L, 1);
		strlcpy(p.szLogin, ch->GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
		p.dwGuild = (DWORD) lua_tonumber(L, 2);
		p.dwGold = (DWORD) lua_tonumber(L, 3);

		sys_log(0, "GUILD_WAR_BET: %s login %s war_id %u guild %u gold %u",
				ch->GetName(), p.szLogin, p.dwWarID, p.dwGuild, p.dwGold);

		db_clientdesc->DBPacket(HEADER_GD_GUILD_WAR_BET, 0, &p, sizeof(p));
		return 0;
	}

	int guild_is_bet(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			lua_pushboolean(L, true);
			return 1;
		}

		bool bBet = CGuildManager::instance().IsBet((DWORD) lua_tonumber(L, 1),
				CQuestManager::instance().GetCurrentCharacterPtr()->GetDesc()->GetAccountTable().login);

		lua_pushboolean(L, bBet);
		return 1;
	}

	int guild_get_warp_war_list(lua_State* L)
	{
		FBuildLuaGuildWarList f(L);
		CGuildManager::instance().for_each_war(f);
		return 1;
	}

	int guild_get_reserve_war_table(lua_State* L)
	{
		std::vector<CGuildWarReserveForGame *> & con = CGuildManager::instance().GetReserveWarRef();

		int i = 0;
		std::vector<CGuildWarReserveForGame *>::iterator it = con.begin();

		sys_log(0, "con.size(): %d", con.size());

		// stack : table1
		lua_newtable(L);

		while (it != con.end())
		{
			TGuildWarReserve * p = &(*(it++))->data;

			if (p->bType != GUILD_WAR_TYPE_BATTLE)
				continue;

			lua_newtable(L);

			sys_log(0, "con.size(): %u %u %u handi %d", p->dwID, p->dwGuildFrom, p->dwGuildTo, p->lHandicap);

			// stack : table1 table2
			lua_pushnumber(L, p->dwID);
			// stack : table1 table2 dwID
			lua_rawseti(L, -2, 1);

			// stack : table1 table2
			if (p->lPowerFrom > p->lPowerTo)
				lua_pushnumber(L, p->dwGuildFrom);
			else
				lua_pushnumber(L, p->dwGuildTo);
			// stack : table1 table2 guildfrom
			lua_rawseti(L, -2, 2);

			// stack : table1 table2
			if (p->lPowerFrom > p->lPowerTo)
				lua_pushnumber(L, p->dwGuildTo);
			else
				lua_pushnumber(L, p->dwGuildFrom);
			// stack : table1 table2 guildto
			lua_rawseti(L, -2, 3);

			lua_pushnumber(L, p->lHandicap);
			lua_rawseti(L, -2, 4);

			// stack : table1 table2
			lua_rawseti(L, -2, ++i);
		}

		return 1;
	}

	int guild_get_member_count(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch == NULL )
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		CGuild* pGuild = ch->GetGuild();

		if ( pGuild == NULL )
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		lua_pushnumber(L, pGuild->GetMemberCount());

		return 1;
	}

	int guild_change_master(lua_State* L)
	{
		// 리턴값
		//	0 : 입력한 이름이 잘못되었음 ( 문자열이 아님 )
		//	1 : 길드장이 아님
		//	2 : 지정한 이름의 길드원이 없음
		//	3 : 요청 성공
		//	4 : 길드가 없음

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();

		if ( pGuild != NULL )
		{
			if ( pGuild->GetMasterPID() == ch->GetPlayerID() )
			{
				if ( lua_isstring(L, 1) == false )
				{
					lua_pushnumber(L, 0);
				}
				else
				{
					bool ret = pGuild->ChangeMasterTo(pGuild->GetMemberPID(lua_tostring(L, 1)));

					lua_pushnumber(L, ret == false ? 2 : 3 );
				}
			}
			else
			{
				lua_pushnumber(L, 1);
			}
		}
		else
		{
			lua_pushnumber(L, 4);
		}

		return 1;
	}

	int guild_change_master_with_limit(lua_State* L)
	{
		// 인자
		//  arg0 : 새 길드장 이름
		//  arg1 : 새 길드장 레벨 제한
		//  arg2 : resign_limit 제한 시간
		//  arg3 : be_other_leader 제한 시간
		//  arg4 : be_other_member 제한 시간
		//  arg5 : 캐시템인가 아닌가
		//
		// 리턴값
		//	0 : 입력한 이름이 잘못되었음 ( 문자열이 아님 )
		//	1 : 길드장이 아님
		//	2 : 지정한 이름의 길드원이 없음
		//	3 : 요청 성공
		//	4 : 길드가 없음
		//	5 : 지정한 이름이 온라인이 아님
		//	6 : 지정한 캐릭터 레벨이 기준레벨보다 낮음
		//	7 : 새 길드장이 be_other_leader 제한에 걸림

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();

		if ( pGuild != NULL )
		{
			if ( pGuild->GetMasterPID() == ch->GetPlayerID() )
			{
				if ( lua_isstring(L, 1) == false )
				{
					lua_pushnumber(L, 0);
				}
				else
				{
					LPCHARACTER pNewMaster = CHARACTER_MANAGER::instance().FindPC( lua_tostring(L,1) );

					if ( pNewMaster != NULL )
					{
						if ( pNewMaster->GetLevel() < lua_tonumber(L, 2) )
						{
							lua_pushnumber(L, 6);
						}
						else
						{
							int nBeOtherLeader = pNewMaster->GetQuestFlag("change_guild_master.be_other_leader");
							CQuestManager::instance().GetPC( ch->GetPlayerID() );

							if ( lua_toboolean(L, 6) == true ) nBeOtherLeader = 0;

							if ( nBeOtherLeader > get_global_time() )
							{
								lua_pushnumber(L, 7);
							}
							else
							{
								bool ret = pGuild->ChangeMasterTo(pGuild->GetMemberPID(lua_tostring(L, 1)));

								if ( ret == false )
								{
									lua_pushnumber(L, 2);
								}
								else
								{
									lua_pushnumber(L, 3);

									pNewMaster->SetQuestFlag("change_guild_master.be_other_leader", 0);
									pNewMaster->SetQuestFlag("change_guild_master.be_other_member", 0);
									pNewMaster->SetQuestFlag("change_guild_master.resign_limit", (int)lua_tonumber(L, 3));

									ch->SetQuestFlag("change_guild_master.be_other_leader", (int)lua_tonumber(L, 4));
									ch->SetQuestFlag("change_guild_master.be_other_member", (int)lua_tonumber(L, 5));
									ch->SetQuestFlag("change_guild_master.resign_limit", 0);
								}
							}
						}
					}
					else
					{
						lua_pushnumber(L, 5);
					}
				}
			}
			else
			{
				lua_pushnumber(L, 1);
			}
		}
		else
		{
			lua_pushnumber(L, 4);
		}

		return 1;
	}

	int guild_get_sp0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();

		lua_pushnumber(L, (pGuild!=NULL)?pGuild->GetSP():0);
		return 1;
	}

	int guild_get_maxsp0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();

		lua_pushnumber(L, (pGuild!=NULL)?pGuild->GetMaxSP():0);
		return 1;
	}

	int guild_get_money0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();

		lua_pushnumber(L, (pGuild!=NULL)?pGuild->GetGuildMoney():0);
		return 1;
	}

	int guild_get_max_member0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();

		lua_pushnumber(L, (pGuild!=NULL)?pGuild->GetMaxMemberCount():0);
		return 1;
	}

	int guild_get_total_member_level0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();

		lua_pushnumber(L, (pGuild!=NULL)?pGuild->GetTotalLevel():0);
		return 1;
	}

	int guild_has_land0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();

		lua_pushboolean(L, (pGuild!=NULL)?pGuild->HasLand():false);
		return 1;
	}

	int guild_get_win_count0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();

		lua_pushnumber(L, (pGuild!=NULL)?pGuild->GetGuildWarWinCount():0);
		return 1;
	}

	int guild_get_draw_count0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();

		lua_pushnumber(L, (pGuild!=NULL)?pGuild->GetGuildWarDrawCount():0);
		return 1;
	}

	int guild_get_loss_count0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();

		lua_pushnumber(L, (pGuild!=NULL)?pGuild->GetGuildWarLossCount():0);
		return 1;
	}

	int guild_add_comment0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();
		if (pGuild)
			pGuild->AddComment(ch, std::string(lua_tostring(L, 1)));
		return 0;
	}

	int guild_get_skill_level0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();
		lua_pushnumber(L, (pGuild)?pGuild->GetSkillLevel(lua_tonumber(L, 1)):0);
		return 1;
	}

	int guild_set_skill_level0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();
		if (pGuild)
			pGuild->SetSkillLevel(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_isnumber(L, 3)?lua_tonumber(L, 3):0);
		return 0;
	}

	int guild_get_skill_point0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();
		lua_pushnumber(L, (pGuild)?pGuild->GetSkillPoint():0);
		return 1;
	}

	int guild_set_skill_point0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();
		if (pGuild)
			pGuild->SetSkillPoint(lua_tonumber(L, 1));
		return 0;
	}

	int guild_get_exp_level0(lua_State* L)
	{
		lua_pushnumber(L, guild_exp_table2[MINMAX(0, lua_tonumber(L, 1) ,GUILD_MAX_LEVEL)]);
		return 1;
	}

	int guild_offer_exp0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();

		if (!pGuild)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		DWORD offer = lua_tonumber(L, 1);

		if (pGuild->GetLevel() >= GUILD_MAX_LEVEL)
		{
			lua_pushboolean(L, false);
		}
		else
		{
			offer /= 100;
			offer *= 100;

			if (pGuild->OfferExp(ch, offer))
			{
				lua_pushboolean(L, true);
			}
			else
			{
				lua_pushboolean(L, false);
			}
		}
		return 1;
	}

	int guild_give_exp0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CGuild* pGuild = ch->GetGuild();

		if (!pGuild)
			return 0;

		pGuild->GuildPointChange(POINT_EXP, lua_tonumber(L, 1) / 100, true);
		return 0;
	}

	int guild_get_id(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		if (!ch)
		{
			sys_err("no current character.");
			lua_pushnumber(L, 0);
			return 1;
		}

		CGuild * guild = ch->GetGuild();
		if (!guild)
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		lua_pushnumber(L, guild->GetID());
		return 1;
	}

	void RegisterGuildFunctionTable()
	{
		luaL_reg guild_functions[] =
		{
			{ "get_rank",				guild_get_rank				},
			{ "get_ladder_point",		guild_get_ladder_point		},
			{ "high_ranking_string",	guild_high_ranking_string	},
			{ "around_ranking_string",	guild_around_ranking_string	},
			{ "name",					guild_name					},
			{ "level",					guild_level					},
			{ "is_war",					guild_is_war				},
			{ "war_enter",				guild_war_enter				},
			{ "get_any_war",			guild_get_any_war			},
			{ "get_reserve_war_table",	guild_get_reserve_war_table	},
			{ "get_name",				guild_get_name				},
			{ "war_bet",				guild_war_bet				},
			{ "is_bet",					guild_is_bet				},
			{ "get_warp_war_list",		guild_get_warp_war_list		},
			{ "get_member_count",		guild_get_member_count		},
			{ "change_master",			guild_change_master			},
			{ "change_master_with_limit",			guild_change_master_with_limit			},
			{ "get_sp0",				guild_get_sp0				},	// get guild sp [return lua number]
			{ "get_maxsp0",				guild_get_maxsp0			},	// get guild maxsp [return lua number]
			{ "get_money0",				guild_get_money0			},	// get money guild [return lua number]
			{ "get_max_member0",		guild_get_max_member0		},	// get max joinable members [return lua number]
			{ "get_total_member_level0",	guild_get_total_member_level0	},	// get the sum of all the members' level [return lua number]
			{ "has_land0",				guild_has_land0				},	// get whether guild has a land or not [return lua boolean]
			{ "get_win_count0",			guild_get_win_count0		},	// get guild wins [return lua number]
			{ "get_draw_count0",		guild_get_draw_count0		},	// get guild draws [return lua number]
			{ "get_loss_count0",		guild_get_loss_count0		},	// get guild losses [return lua number]
			{ "add_comment0",			guild_add_comment0			},	// add a comment into guild notice board [return nothing]
			{ "get_skill_level0",		guild_get_skill_level0		},	// get guild skill level [return lua number]
			{ "set_skill_level0",		guild_set_skill_level0		},	// set guild skill level [return nothing]
			{ "get_skill_point0",		guild_get_skill_point0		},	// get guild skill points [return lua number]
			{ "set_skill_point0",		guild_set_skill_point0		},	// set guild skill points [return nothing]
			{ "get_exp_level0",			guild_get_exp_level0		},	// get how much exp is necessary for such <level> [return lua number]
			{ "offer_exp0",				guild_offer_exp0			},	// give player's <exp> to guild [return lua boolean=successfulness]
			{ "give_exp0",				guild_give_exp0				},	// give <exp> to guild [return nothing]
			{"get_id", guild_get_id},
			{NULL, NULL}
		};

		CQuestManager::instance().AddLuaFunctionTable("guild", guild_functions);
	}
}

