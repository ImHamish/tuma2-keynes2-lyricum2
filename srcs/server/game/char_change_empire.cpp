
#include "stdafx.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "db.h"
#include "guild_manager.h"
#include "marriage.h"
#include "../common/CommonDefines.h"
#include "desc.h"

int CHARACTER::ChangeEmpire(BYTE empire)
{
	if (GetEmpire() == empire)
		return 1;

	if (GetParty() != nullptr)
		return 4;

	char szQuery[1024+1];
	DWORD dwAID;
	DWORD dwPID[4];
	memset(dwPID, 0, sizeof(dwPID));

	{
		// 1. 내 계정의 모든 pid를 얻어 온다
#ifdef ENABLE_PLAYER_PER_ACCOUNT5
		snprintf(szQuery, sizeof(szQuery),
				"SELECT id, pid1, pid2, pid3, pid4, pid5 FROM player_index%s WHERE pid1=%u OR pid2=%u OR pid3=%u OR pid4=%u OR pid5=%u AND empire=%u",
				get_table_postfix(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetEmpire());
#else
		snprintf(szQuery, sizeof(szQuery),
				"SELECT id, pid1, pid2, pid3, pid4 FROM player_index%s WHERE pid1=%u OR pid2=%u OR pid3=%u OR pid4=%u AND empire=%u",
				get_table_postfix(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetEmpire());
#endif
		std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));
		if (msg->Get()->uiNumRows == 0) {
			return 0;
		}

		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);
		str_to_number(dwAID, row[0]);
		str_to_number(dwPID[0], row[1]);
		str_to_number(dwPID[1], row[2]);
		str_to_number(dwPID[2], row[3]);
		str_to_number(dwPID[3], row[4]);
	}

	const int loop = 4;

	{
		// 2. 각 캐릭터의 길드 정보를 얻어온다.
		//   한 캐릭터라도 길드에 가입 되어 있다면, 제국 이동을 할 수 없다.
		DWORD dwGuildID[4];
		CGuild * pGuild[4];
		for (int i = 0; i < loop; ++i) {
			snprintf(szQuery, sizeof(szQuery), "SELECT guild_id FROM guild_member%s WHERE pid=%u", get_table_postfix(), dwPID[i]);
			std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));
			if (msg->Get()->uiNumRows > 0) {
				MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);
				str_to_number(dwGuildID[i], row[0]);
				pGuild[i] = CGuildManager::instance().FindGuild(dwGuildID[i]);
				if (pGuild[i] != NULL) {
					return 2;
				}
			} else {
				dwGuildID[i] = 0;
				pGuild[i] = NULL;
			}
		}
	}

	{
		// 3. 각 캐릭터의 결혼 정보를 얻어온다.
		//   한 캐릭터라도 결혼 상태라면 제국 이동을 할 수 없다.
		for (int i = 0; i < loop; ++i)
		{
			if (marriage::CManager::instance().IsEngagedOrMarried(dwPID[i]) == true)
				return 3;
		}
	}

	{
		// 4. db의 제국 정보를 업데이트 한다.
#ifdef ENABLE_PLAYER_PER_ACCOUNT5
		snprintf(szQuery, sizeof(szQuery), "UPDATE player_index%s SET empire=%u WHERE pid1=%u OR pid2=%u OR pid3=%u OR pid4=%u OR pid5=%u AND empire=%u",
				get_table_postfix(), empire, GetPlayerID(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetEmpire());
#else
		snprintf(szQuery, sizeof(szQuery), "UPDATE player_index%s SET empire=%u WHERE pid1=%u OR pid2=%u OR pid3=%u OR pid4=%u AND empire=%u",
				get_table_postfix(), empire, GetPlayerID(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetEmpire());
#endif
		std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));
		if (msg->Get()->uiAffectedRows > 0)
		{
			SetChangeEmpireCount();

			const LPDESC d = GetDesc();
			if (d) {
				d->SetNewEmpire(empire);
			}

			SetEmpire(empire);
//			RestartAtSamePos();
			UpdatePacket();
	
			return 999;
		}
	}

	return 0;
}

int CHARACTER::GetChangeEmpireCount() const
{
	DWORD dwAID = GetAID();
	if (dwAID == 0)
		return 0;

	char szQuery[1024+1];

	snprintf(szQuery, sizeof(szQuery), "SELECT change_count FROM change_empire WHERE account_id = %u", dwAID);
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));
	if (msg->Get()->uiNumRows == 0) {
		return 0;
	}

	MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);
	DWORD count = 0;
	str_to_number(count, row[0]);
	return count;
}

void CHARACTER::SetChangeEmpireCount()
{
	char szQuery[1024+1];

	DWORD dwAID = GetAID();

	if (dwAID == 0) return;

	int count = GetChangeEmpireCount();

	if (count == 0)
	{
		count++;
		snprintf(szQuery, sizeof(szQuery), "INSERT INTO change_empire VALUES(%u, %d, NOW())", dwAID, count);
	}
	else
	{
		count++;
		snprintf(szQuery, sizeof(szQuery), "UPDATE change_empire SET change_count=%d WHERE account_id=%u", count, dwAID);
	}

	std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery(szQuery));
}

DWORD CHARACTER::GetAID() const
{
	return GetDesc() ? GetDesc()->GetAccountTable().id : 0;
}
