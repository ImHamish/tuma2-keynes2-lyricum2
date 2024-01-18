#include "stdafx.h"

#include "config.h"
#include "questmanager.h"
#include "char.h"

#undef sys_err
#if !defined(_WIN32)
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

namespace quest
{
	int ds_open_refine_window(lua_State* L)
	{
		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (NULL == ch)
		{
			sys_err ("NULL POINT ERROR");
			return 0;
		}

		ch->DragonSoul_RefineWindow_Open(CQuestManager::instance().GetCurrentNPCCharacterPtr());
		return 0;
	}

	void RegisterDragonSoulFunctionTable()
	{
		luaL_reg ds_functions[] =
		{
			{ "open_refine_window"	, ds_open_refine_window },
			{ NULL					, NULL					}
		};

		CQuestManager::instance().AddLuaFunctionTable("ds", ds_functions);
	}
};
