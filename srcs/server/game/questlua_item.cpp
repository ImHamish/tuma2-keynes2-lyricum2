#include "stdafx.h"
#include "questmanager.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "over9refine.h"
#include "log.h"
#include "db.h"

#undef sys_err
#if !defined(_WIN32)
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

namespace quest
{
	//
	// "item" Lua functions
	//

	int item_get_cell(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (q.GetCurrentItem())
		{
			lua_pushnumber(L, q.GetCurrentItem()->GetCell());
		}
		else
			lua_pushnumber(L, 0);
		return 1;
	}

	int item_select_cell(lua_State* L)
	{
		lua_pushboolean(L, 0);
		if (!lua_isnumber(L, 1))
		{
			return 1;
		}
		DWORD cell = (DWORD) lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPITEM item = ch ? ch->GetInventoryItem(cell) : NULL;

		if (!item)
		{
			return 1;
		}

		CQuestManager::instance().SetCurrentItem(item);
		lua_pushboolean(L, 1);

		return 1;
	}

	int item_select(lua_State* L)
	{
		lua_pushboolean(L, 0);
		if (!lua_isnumber(L, 1))
		{
			return 1;
		}
		DWORD id = (DWORD) lua_tonumber(L, 1);
		LPITEM item = ITEM_MANAGER::instance().Find(id);

		if (!item)
		{
			return 1;
		}

		CQuestManager::instance().SetCurrentItem(item);
		lua_pushboolean(L, 1);

		return 1;
	}

	int item_get_id(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (q.GetCurrentItem())
		{
			lua_pushnumber(L, q.GetCurrentItem()->GetID());
		}
		else
			lua_pushnumber(L, 0);
		return 1;
	}

	int item_remove(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();
		if (item != NULL) {
			if (q.GetCurrentCharacterPtr() == item->GetOwner()) {
				ITEM_MANAGER::instance().RemoveItem(item);
			} else {
				sys_err("Tried to remove invalid item %p", get_pointer(item));
			}
			q.ClearCurrentItem();
		}

		return 0;
	}

	int item_get_socket(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		if (q.GetCurrentItem() && lua_isnumber(L, 1))
		{
			int idx = (int) lua_tonumber(L, 1);
			if (idx < 0 || idx >= ITEM_SOCKET_MAX_NUM)
				lua_pushnumber(L,0);
			else
				lua_pushnumber(L, q.GetCurrentItem()->GetSocket(idx));
		}
		else
		{
			lua_pushnumber(L,0);
		}
		return 1;
	}

	int item_set_socket(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		if (q.GetCurrentItem() && lua_isnumber(L,1) && lua_isnumber(L,2))
		{
			int idx = (int) lua_tonumber(L, 1);
			int value = (int) lua_tonumber(L, 2);
			if (idx >=0 && idx < ITEM_SOCKET_MAX_NUM)
				q.GetCurrentItem()->SetSocket(idx, value);
		}
		return 0;
	}

	int item_get_vnum(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetVnum());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_has_flag(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (!lua_isnumber(L, 1))
		{
			sys_err("flag is not a number.");
			lua_pushboolean(L, 0);
			return 1;
		}

		if (!item)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		long lCheckFlag = (long) lua_tonumber(L, 1);
		lua_pushboolean(L, IS_SET(item->GetFlag(), lCheckFlag));

		return 1;
	}

	int item_get_value(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (!item)
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		if (!lua_isnumber(L, 1))
		{
			sys_err("index is not a number");
			lua_pushnumber(L, 0);
			return 1;
		}

		int index = (int) lua_tonumber(L, 1);

		if (index < 0 || index >= ITEM_VALUES_MAX_NUM)
		{
			sys_err("index(%d) is out of range (0..%d)", index, ITEM_VALUES_MAX_NUM);
			lua_pushnumber(L, 0);
		}
		else
			lua_pushnumber(L, item->GetValue(index));

		return 1;
	}

	int item_set_value(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (!item)
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		if (false == (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)))
		{
			sys_err("index is not a number");
			lua_pushnumber(L, 0);
			return 1;
		}

		item->SetForceAttribute(
			lua_tonumber(L, 1),		// index
			lua_tonumber(L, 2),		// apply type
			lua_tonumber(L, 3)		// apply value
		);

		return 0;
	}

	int item_get_name(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();
		if (item)
			lua_pushstring(L, item->GetName());
		else
			lua_pushstring(L, "");

		return 1;
	}

	int item_get_size(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetSize());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_get_count(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetCount());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_get_type(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetType());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_get_sub_type(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetSubType());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_get_refine_vnum(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetRefinedVnum());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_next_refine_vnum(lua_State* L)
	{
		DWORD vnum = 0;
		if (lua_isnumber(L, 1))
			vnum = (DWORD) lua_tonumber(L, 1);

		TItemTable* pTable = ITEM_MANAGER::instance().GetTable(vnum);
		if (pTable)
		{
			lua_pushnumber(L, pTable->dwRefinedVnum);
		}
		else
		{
			sys_err("Cannot find item table of vnum %u", vnum);
			lua_pushnumber(L, 0);
		}
		return 1;
	}

	int item_get_level(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetRefineLevel());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_can_over9refine(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if ( item == NULL ) return 0;

		lua_pushnumber(L, COver9RefineManager::instance().canOver9Refine(item->GetVnum()));

		return 1;
	}

	int item_change_to_over9(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if ( ch == NULL || item == NULL ) return 0;

		lua_pushboolean(L, COver9RefineManager::instance().Change9ToOver9(ch, item));

		return 1;
	}

	int item_over9refine(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if ( ch == NULL || item == NULL ) return 0;

		lua_pushboolean(L, COver9RefineManager::instance().Over9Refine(ch, item));

		return 1;
	}

	int item_get_over9_material_vnum(lua_State* L)
	{
		if ( lua_isnumber(L, 1) == true )
		{
			lua_pushnumber(L, COver9RefineManager::instance().GetMaterialVnum((DWORD)lua_tonumber(L, 1)));
			return 1;
		}

		return 0;
	}

	int item_get_level_limit(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (q.GetCurrentItem())
		{
			if (q.GetCurrentItem()->GetType() != ITEM_WEAPON && q.GetCurrentItem()->GetType() != ITEM_ARMOR)
			{
				return 0;
			}
			lua_pushnumber(L, q.GetCurrentItem() -> GetLevelLimit());
			return 1;
		}
		return 0;
	}

#ifdef __NEWPET_SYSTEM__
	int item_pet_death(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		DWORD itemid = 0;
		if (q.GetCurrentItem())
		{
			itemid = q.GetCurrentItem()->GetID();
			char szQuery1[1024];
			snprintf(szQuery1, sizeof(szQuery1), "SELECT duration FROM new_petsystem WHERE id = %u LIMIT 1", itemid);
			std::unique_ptr<SQLMsg> pmsg2(DBManager::instance().DirectQuery(szQuery1));
			if (pmsg2->Get()->uiNumRows > 0) {
				MYSQL_ROW row = mysql_fetch_row(pmsg2->Get()->pSQLResult);
				lua_pushboolean(L, atoi(row[0]) <= 0);
				return 0;
			}
			else{
				lua_pushboolean(L, false);
				sys_err("[NewPetSystem]Error no item founded!On item.pet.death");
				return 0;
			}
					
		}
		return 0;
	}

	int item_pet_revive(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		DWORD itemid = 0;
		if (q.GetCurrentItem())
		{
			itemid = q.GetCurrentItem()->GetID();
			std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE new_petsystem SET duration =(tduration/2) WHERE id = %d", itemid));
		}
		return 0;
	}
#endif

	int item_start_realtime_expire(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM pItem = q.GetCurrentItem();

		if (pItem)
		{
			pItem->StartRealTimeExpireEvent();
			return 1;
		}

		return 0;
	}

	int item_copy_and_give_before_remove(lua_State* L)
	{
		lua_pushboolean(L, 0);
		if (!lua_isnumber(L, 1))
			return 1;

		DWORD vnum = (DWORD)lua_tonumber(L, 1);

		CQuestManager& q = CQuestManager::instance();
		LPITEM pItem = q.GetCurrentItem();
		LPCHARACTER pChar = q.GetCurrentCharacterPtr();

		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(pItem, pkNewItem);
			LogManager::instance().ItemLog(pChar, pkNewItem, "COPY SUCCESS", pkNewItem->GetName());

			BYTE bCell = pItem->GetCell();

			ITEM_MANAGER::instance().RemoveItem(pItem, "REMOVE (COPY SUCCESS)");

			pkNewItem->AddToCharacter(pChar, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			pkNewItem->AttrLog();

			// ����!
			lua_pushboolean(L, 1);
		}

		return 1;
	}

	int item_get_wearflag0(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetWearFlag());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_has_wearflag0(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (!lua_isnumber(L, 1))
		{
			sys_err("wearflag is not a number.");
			lua_pushboolean(L, 0);
			return 1;
		}

		if (item)
			lua_pushboolean(L, IS_SET(item->GetWearFlag(), (long)lua_tonumber(L, 1)));
		else
			lua_pushboolean(L, false);

		return 1;
	}


	int item_get_antiflag0(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetAntiFlag());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_has_antiflag0(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (!lua_isnumber(L, 1))
		{
			sys_err("antiflag is not a number.");
			lua_pushboolean(L, false);
			return 1;
		}

		if (item)
			lua_pushboolean(L, IS_SET(item->GetAntiFlag(), static_cast<DWORD>(lua_tonumber(L, 1))));
		else
			lua_pushboolean(L, false);

		return 1;
	}


	int item_get_immuneflag0(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetImmuneFlag());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_has_immuneflag0(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (!lua_isnumber(L, 1))
		{
			sys_err("immuneflag is not a number.");
			lua_pushboolean(L, false);
			return 1;
		}

		if (item)
			lua_pushboolean(L, IS_SET(item->GetImmuneFlag(), static_cast<DWORD>(lua_tonumber(L, 1))));
		else
			lua_pushboolean(L, false);

		return 1;
	}


#define NS_ITEM_GETMODE0(x)	\
		int x = 0;\
		if(lua_isnumber(L, 1))\
			x = MINMAX(0, lua_tonumber(L, 1), 2);

	int item_add_attr0(lua_State* L)
	{
		NS_ITEM_GETMODE0(m_mode);

		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if(item)
		{
			int m_count = 0;
			int m_reqsf = 1;
			if (lua_isnumber(L, 2))
				m_reqsf = lua_tonumber(L, 2);

			if (m_mode==1 || m_mode==0)
			{
				m_count = ITEM_ATTRIBUTE_NORM_NUM - item->GetAttributeCount();
				if (m_count>m_reqsf && m_reqsf!=0)
					m_count = m_reqsf;
				for (int i=0; i<m_count; i++)
					item->AddAttribute();
			}
			if (m_mode==2 || m_mode==0)
			{
				m_count = ITEM_ATTRIBUTE_RARE_NUM - item->GetRareAttrCount();
				if (m_count>m_reqsf && m_reqsf!=0)
					m_count = m_reqsf;
				for (int i=0; i<m_count; i++)
					item->AddRareAttribute();
			}
		}
		return 0;
	}

	int item_change_attr0(lua_State* L)
	{
		NS_ITEM_GETMODE0(m_mode);

		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if(item)
		{
			if (m_mode==0 || m_mode==1)
				item->ChangeAttribute();
			if (m_mode==0 || m_mode==2)
				item->ChangeRareAttribute();
		}
		return 0;
	}

	int item_clear_attr0(lua_State* L)
	{
		NS_ITEM_GETMODE0(m_mode);

		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		int m_start = 0;
		int m_end = ITEM_ATTRIBUTE_MAX_NUM;

		if (m_mode==1)
			m_end = ITEM_ATTRIBUTE_NORM_NUM;
		else if (m_mode==2)
			m_start = ITEM_ATTRIBUTE_NORM_NUM;

		for (int i=m_start; i<m_end; i++)
			item->SetForceAttribute(i, 0, 0);
		return 0;
	}

	int item_count_attr0(lua_State* L)
	{
		NS_ITEM_GETMODE0(m_mode);

		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if(item)
		{
			if (m_mode==1)
				lua_pushnumber(L, item->GetAttributeCount());
			else if(m_mode==2)
				lua_pushnumber(L, item->GetRareAttrCount());
			else //0
			{
				lua_newtable(L);
				{
					lua_pushnumber(L, item->GetAttributeCount());
					lua_rawseti(L, -2, 1);
				}
				{
					lua_pushnumber(L, item->GetRareAttrCount());
					lua_rawseti(L, -2, 2);
				}
			}
		}
		else
			lua_pushnumber(L, 0.0);

		return 1;
	}

	int item_get_attr0(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		TPlayerItemAttribute m_attr;
		if (item)
		{
			// it returns a table like:
			// {id, value, id, value, id, value, id, value, id, value, id, value, id, value}
			// es. {1, 1000, 2, 500, 73, 15, 23, 20, 0, 0, 71, 15, 72, 15}
			lua_newtable(L);
			for (int i=0; i<ITEM_ATTRIBUTE_MAX_NUM; i++)
			{
				m_attr = item->GetAttribute(i);
				// push type
				lua_pushnumber(L, m_attr.bType);
				lua_rawseti(L, -2, (i*2)+1);
				// push value
				lua_pushnumber(L, m_attr.sValue);
				lua_rawseti(L, -2, (i*2)+2);
			}
		}
		else
			lua_pushnumber(L, 0.0);

		return 1;
	}

	int item_set_attr0(lua_State* L)
	{
		if (!lua_istable(L, 1))
			return 0;

		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		int m_attr[ITEM_ATTRIBUTE_MAX_NUM*2] = {0};
		int m_idx = 0;
		// start
		lua_pushnil(L);
		while (lua_next(L, 1) && m_idx<(ITEM_ATTRIBUTE_MAX_NUM*2))
		{
			m_attr[m_idx++] = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		// end
		for (int i=0; i<ITEM_ATTRIBUTE_MAX_NUM; i++)
			item->SetForceAttribute(i, m_attr[(i*2)+0], m_attr[(i*2)+1]);
		return 0;
	}

	int item_set_count0(lua_State* L)
	{
		if(!lua_isnumber(L, 1))
			return 0;

		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			item->SetCount(lua_tonumber(L, 1));
			//item->SetCount(MINMAX(1, lua_tonumber(L, 1), g_bItemCountLimit));

		return 0;
	}

	// int item_equip_to0(lua_State* L)
	// {
		// CQuestManager& q = CQuestManager::instance();
		// LPITEM item = q.GetCurrentItem();
		// LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		// lua_pushboolean((item && ch)?item->EquipTo(ch, lua_tonumber(L, 1)):false);

		// return 1;
	// }

	// int item_unequip0(lua_State* L)
	// {
		// CQuestManager& q = CQuestManager::instance();
		// LPITEM item = q.GetCurrentItem();

		// lua_pushboolean(L, (item)?item->Unequip():false);

		// return 1;
	// }

	int item_is_available0(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		lua_pushboolean(L, item!=NULL);
		return 1;
	}

#if defined(ENABLE_CHANGELOOK)
	int item_is_transmulated(lua_State* L)
	{
		CQuestManager& qMgr = CQuestManager::instance();
		LPITEM pkItem = qMgr.GetCurrentItem();

		if (pkItem) {
			lua_pushboolean(L, pkItem->GetTransmutation() != 0 ? true : false);
		} else {
			lua_pushboolean(L, false);
		}

		return 1;
	}

	int item_set_transmutation(lua_State* L)
	{
		CQuestManager& qMgr = CQuestManager::instance();
		LPITEM pkItem = qMgr.GetCurrentItem();

		if (pkItem && lua_isnumber(L, 1)) {
			uint32_t dwTransmutation = (uint32_t)lua_tonumber(L, 1);
			pkItem->SetTransmutation(dwTransmutation);
		}

		return 0;
	}

	int item_get_transmutation(lua_State* L)
	{
		CQuestManager& qMgr = CQuestManager::instance();
		LPITEM pkItem = qMgr.GetCurrentItem();

		lua_pushnumber(L, pkItem ? pkItem->GetTransmutation() : 0);

		return 1;
	}
#endif

	void RegisterITEMFunctionTable()
	{

		luaL_reg item_functions[] =
		{
#if defined(ENABLE_CHANGELOOK)
			{"is_transmulated", item_is_transmulated},
			{"set_transmutation", item_set_transmutation},
			{"get_transmutation", item_get_transmutation},
#endif
			{ "get_id",		item_get_id		},
			{ "get_cell",		item_get_cell		},
			{ "select",		item_select		},
			{ "select_cell",	item_select_cell	},
			{ "remove",		item_remove		},
			{ "get_socket",		item_get_socket		},
			{ "set_socket",		item_set_socket		},
			{ "get_vnum",		item_get_vnum		},
			{ "has_flag",		item_has_flag		},
			{ "get_value",		item_get_value		},
			{ "set_value",		item_set_value		},
			{ "get_name",		item_get_name		},
			{ "get_size",		item_get_size		},
			{ "get_count",		item_get_count		},
			{ "get_type",		item_get_type		},
			{ "get_sub_type",	item_get_sub_type	},
			{ "get_refine_vnum",	item_get_refine_vnum	},
			{ "get_level",		item_get_level		},
			{ "next_refine_vnum",	item_next_refine_vnum	},
			{ "can_over9refine",	item_can_over9refine	},
			{ "change_to_over9",		item_change_to_over9	},
			{ "over9refine",		item_over9refine	},
			{ "get_over9_material_vnum",		item_get_over9_material_vnum	},
			{ "get_level_limit", 				item_get_level_limit },
			{ "start_realtime_expire", 			item_start_realtime_expire },
			{ "copy_and_give_before_remove",	item_copy_and_give_before_remove},
#ifdef __NEWPET_SYSTEM__
			{ "petdeath",						item_pet_death},
			{ "petrevive",						item_pet_revive},
#endif		
			{ "get_wearflag0",			item_get_wearflag0},	// [return lua number]
			{ "has_wearflag0",			item_has_wearflag0},	// [return lua boolean]
			{ "get_antiflag0",			item_get_antiflag0},	// [return lua number]
			{ "has_antiflag0",			item_has_antiflag0},	// [return lua boolean]
			{ "get_immuneflag0",		item_get_immuneflag0},	// [return lua number]
			{ "has_immuneflag0",		item_has_immuneflag0},	// [return lua boolean]
			// item.add_attr0(0|1|2[, cnt]) -- (0: baseeraro, 1: base, 2: raro)
			// item.add_attr0(0) -- add one 1-5 and one 6-7 bonus
			// item.add_attr0(0, 0) -- add all 1-7 bonuses
			// item.add_attr0(1|2) -- add one 1-5|6-7 bonus
			// item.add_attr0(1|2, 0) -- add all 1-5|6-7 bonuses
			// item.add_attr0(1|2, 4) -- add four 1-5|6-7 bonuses
			{ "add_attr0",			item_add_attr0},
			// item.change_attr0(0|1|2) -- (0: baseerari, 1: base, 2: rari)
			{ "change_attr0",		item_change_attr0},
			// item.clear_attr0(0|1|2) -- (0: baseerari, 1: base, 2: rari)
			{ "clear_attr0",		item_clear_attr0},
			// item.count_attr0(0|1|2) -- (0: [cnt(base), cnt(rari)], 1: cnt(base), 2: cnt(rari))
			{ "count_attr0",		item_count_attr0},
			// item.get_attr0() -- return a table containing all the item attrs {1,11,2,22,...,7,77}
			{ "get_attr0",			item_get_attr0},	// [return lua table]
			// item.set_attr0({1,11,2,22,...,7,77}) use a table to set the item attrs
			{ "set_attr0",			item_set_attr0},	// [return nothing]
			// item.set_count(count)
			{ "set_count0",			item_set_count0},	// [return nothing]
			// { "equip_to0",			item_equip_to0},	// [return lua boolean=successfulness]
			// { "unequip0",			item_unequip0},		// [return lua boolean=successfulness]
			{ "is_available0",		item_is_available0	},	// [return lua boolean]
			{ NULL,			NULL			}
		};
		CQuestManager::instance().AddLuaFunctionTable("item", item_functions);
	}
}
