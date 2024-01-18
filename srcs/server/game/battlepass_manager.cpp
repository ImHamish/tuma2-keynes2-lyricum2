#include "stdafx.h"

#if defined(USE_BATTLEPASS)
#include "battlepass_manager.h"
#include "p2p.h"
#include "locale_service.h"
#include "char.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "questmanager.h"
#include "questlua.h"
#include "start_position.h"
#include "char_manager.h"
#include "item_manager.h"
#include "sectree_manager.h"
#include "regen.h"
#include "log.h"
#include "db.h"
#include "target.h"
#include "party.h"

#include <string>
#include <algorithm>

const std::string g_astMissionType[MISSION_TYPE_MAX] = {
	"",
	"KILL_MONSTER",
	"KILL_PLAYER",
	"DAMAGE_MONSTER",
	"DAMAGE_PLAYER",
	"BP_ITEM_USE",
	"BP_ITEM_SELL",
	"BP_ITEM_CRAFT",
	"BP_ITEM_REFINE",
	"BP_ITEM_DESTROY",
	"BP_ITEM_COLLECT",
	"FISH_FISHING",
	"FISH_GRILL",
	"FISH_CATCH",
	"EXP_COLLECT",
	"YANG_COLLECT",
	"GUILD_PLAY_GUILDWAR",
	"GUILD_SPENT_EXP",
	"GAYA_CRAFT_GAYA",
	"GAYA_BUY_ITEM_GAYA_COST",
	"COMPLETE_DUNGEON",
	"CHAT_MESSAGE_SHOUT",
};

CBattlePassManager::CBattlePassManager()
{
	m_pLoader = nullptr;

	m_dwActiveBattlePassID = 0;
	m_dwBattlePassStartTime = 0;
	m_dwBattlePassEndTime = 0;
}

CBattlePassManager::~CBattlePassManager()
{
	if (m_pLoader)
	{
		delete m_pLoader;
		m_pLoader = nullptr;
	}

	m_dwActiveBattlePassID = 0;
	m_dwBattlePassStartTime = 0;
	m_dwBattlePassEndTime = 0;
}

bool CBattlePassManager::InitializeBattlePass()
{
	char szBattlePassFileName[256];
	snprintf(szBattlePassFileName, sizeof(szBattlePassFileName), "%s/battle_pass.txt", LocaleService_GetBasePath().c_str());

	m_pLoader = new CGroupTextParseTreeLoader;
	CGroupTextParseTreeLoader& normal_loader = *m_pLoader;

	if (false == normal_loader.Load(szBattlePassFileName))
	{
		sys_err("battlepass.txt initialize failure (path incorrect?)");
		return false;
	}

	if (!ReadBattlePassGroup())
	{
		return false;
	}

	if (!ReadBattlePassMissions())
	{
		return false;
	}

	CheckBattlePassTimes();

	return true;
}

bool CBattlePassManager::ReadBattlePassGroup()
{
	std::string stName;

	CGroupNode* pGroupNode = m_pLoader->GetGroup("battlepass");

	if (nullptr == pGroupNode)
	{
		sys_err("battle_pass.txt a besoin d'un groupe de battle pass.");
		return false;
	}

	int32_t n = pGroupNode->GetRowCount();
	if (0 == n)
	{
		sys_err("Le group battle pass est vide.");
		return false;
	}

	std::set<uint8_t> setIDs;

	for (int32_t i = 0; i < n; i++)
	{
		const CGroupNode::CGroupNodeRow* pRow;
		pGroupNode->GetRow(i, &pRow);

		std::string stBattlePassName;
		uint8_t battlePassId;

		std::vector<TExtBattlePassTimeTable> timeVector;
		TExtBattlePassTimeTable activeTimes;

		if (!pRow->GetValue("battlepassname", stBattlePassName))
		{
			sys_err("In Group BattlePass, No BattlePassName column.");
			return false;
		}

		if (!pRow->GetValue("battlepassid", battlePassId))
		{
			sys_err("In Group BattlePass, %s's ID is invalid", stBattlePassName.c_str());
			return false;
		}

		if (!pRow->GetValue("starttime", activeTimes.dwStartTime))
		{
			sys_err("In Group BattlePass, %s's StartTime is invalid", stBattlePassName.c_str());
			return false;
		}

		if (!pRow->GetValue("endtime", activeTimes.dwEndTime))
		{
			sys_err("In Group BattlePass, %s's EndTime is invalid", stBattlePassName.c_str());
			return false;
		}

		if (setIDs.end() != setIDs.find(battlePassId))
		{
			sys_err("In Group BattlePass, duplicated id exist.");
			return false;
		}

		setIDs.insert(battlePassId);

		m_map_battle_pass_name.insert(TMapBattlePassName::value_type(battlePassId, stBattlePassName));

		activeTimes.bBattlePassId = battlePassId;

		timeVector.push_back(activeTimes);
		m_map_battle_pass_times.insert(TMapBattlePassTimeTable::value_type(battlePassId, timeVector));
	}

	return true;
}

bool CBattlePassManager::ReadBattlePassMissions()
{
	auto it = m_map_battle_pass_name.begin();
	while (it != m_map_battle_pass_name.end())
	{
		std::string battlePassName = (it++)->second;

		CGroupNode* pGroupNode = m_pLoader->GetGroup(battlePassName.c_str());

		if (nullptr == pGroupNode)
		{
			sys_err("battle_pass.txt need group %s.", battlePassName.c_str());
			return false;
		}

		int32_t n = pGroupNode->GetChildNodeCount();
		if (n < 2)
		{
			sys_err("Group %s need to have at least one grup for Reward and one Mission. Row: %d", battlePassName.c_str(), n);
			return false;
		}
		
		{
			CGroupNode* pChild;
			if (nullptr == (pChild = pGroupNode->GetChildNode("reward")))
			{
				sys_err("In Group %s, Reward group is not defined.", battlePassName.c_str());
				return false;
			}

			int32_t m = pChild->GetRowCount();
			std::vector<TExtBattlePassRewardItem> rewardVector;

			for (int32_t j = 1; j <= m; j++) {
				std::stringstream ss;
				ss << j;
				const CGroupNode::CGroupNodeRow* pRow = nullptr;

				pChild->GetRow(ss.str(), &pRow);
				if (nullptr == pRow)
				{
					sys_err("In Group %s, subgroup Reward, No %d row.", battlePassName.c_str(), j);
					return false;
				}

				TExtBattlePassRewardItem itemReward;

				if (!pRow->GetValue("itemvnum", itemReward.dwVnum))
				{
					sys_err("In Group %s, subgroup Reward, ItemVnum is empty.", battlePassName.c_str());
					return false;
				}

				if (!pRow->GetValue("itemcount", itemReward.bCount))
				{
					sys_err("In Group %s, subgroup Reward, ItemCount is empty.", battlePassName.c_str());
					return false;
				}

				rewardVector.push_back(itemReward);
			}

			m_map_battle_pass_reward.insert(TMapBattlePassReward::value_type(battlePassName.c_str(), rewardVector));
		}

		std::vector<TExtBattlePassMissionInfo> missionInfoVector;

		for (int i = 1; i < n; i++) {
			std::stringstream ss;
			ss << "mission_" << i;

			CGroupNode* pChild;
			if (nullptr == (pChild = pGroupNode->GetChildNode(ss.str())))
			{
				sys_err("In Group %s, %s subgroup is not defined.", battlePassName.c_str(), ss.str().c_str());
				return false;
			}

			int32_t m = pChild->GetRowCount();

			std::string stMissionSearch[] = { "", "" };
			bool bAlreadySearched = false;
			uint8_t bRewardContor = 0;
			TExtBattlePassMissionInfo missionInfo;
			memset(&missionInfo, 0, sizeof(TExtBattlePassMissionInfo));

			missionInfo.bMissionIndex = i;
			
			for (int32_t j = 0; j < m; j++) {
				const CGroupNode::CGroupNodeRow* pRow = nullptr;

				pChild->GetRow(j, &pRow);
				if (nullptr == pRow)
				{
					sys_err("In Group %s and subgroup %s nullptr row.", battlePassName.c_str(), ss.str().c_str());
					return false;
				}

				std::string stInfoDesc;
				if (!pRow->GetValue("infodesc", stInfoDesc))
				{
					sys_err("In Group %s and subgroup %s InfoDesc does not exist.", battlePassName.c_str(), ss.str().c_str());
					return false;
				}

				if (stInfoDesc == "type") {
					std::string stInfoName;
					if (!pRow->GetValue("infoname", stInfoName))
					{
						sys_err("In Group %s and subgroup %s InfoName does not exist.", battlePassName.c_str(), ss.str().c_str());
						return false;
					}

					missionInfo.bMissionType = GetMissionTypeByName(stInfoName);
				}

				if (missionInfo.bMissionType <= MISSION_TYPE_NONE
					|| missionInfo.bMissionType >= MISSION_TYPE_MAX)
				{
					sys_err("In Group %s and subgroup %s Wrong mission type: %d.", battlePassName.c_str(), ss.str().c_str(), missionInfo.bMissionType);
					return false;
				}

				if (!bAlreadySearched)
				{
					GetMissionSearchName(missionInfo.bMissionType, &stMissionSearch[0], &stMissionSearch[1]);
					bAlreadySearched = true;
				}

				for (int k = 0; k < 2; k++)
				{
					if (stMissionSearch[k] != "")
					{
						if (stInfoDesc == stMissionSearch[k])
						{
							if (!pRow->GetValue("infoname", missionInfo.dwMissionInfo[k]))
							{
								sys_err("In Group %s and subgroup %s InfoDesc %s InfoName does not exist.", battlePassName.c_str(), ss.str().c_str(), stMissionSearch[k].c_str());
								return false;
							}

							stMissionSearch[k] = "";
						}
					}
				}

				if (bRewardContor >= 3)
				{
					sys_err("In Group %s and subgroup %s More than 3 rewards.", battlePassName.c_str(), ss.str().c_str());
					return false;
				}

				if (isdigit(*stInfoDesc.c_str()))
				{
					uint32_t dwVnum = atoi(stInfoDesc.c_str());
					uint32_t bCount = 1;

					if (!pRow->GetValue("infoname", bCount))
					{
						sys_err("In Group %s and subgroup %s Wrong ItemCount.", battlePassName.c_str(), ss.str().c_str());
						return false;
					}

					missionInfo.aRewardList[bRewardContor].dwVnum = dwVnum;
					missionInfo.aRewardList[bRewardContor].bCount = bCount;
					bRewardContor++;
				}
			}

			missionInfoVector.push_back(missionInfo);
		}

		m_map_battle_pass_mission_info.insert(TMapBattleMissionInfo::value_type(battlePassName.c_str(), missionInfoVector));
	}

	return true;
}

void CBattlePassManager::CheckBattlePassTimes()
{
	bool findActiveBattlePass = false;
	int32_t cachedBattlePassID, cachedStartTime, cachedEndTime;

	for (auto it = m_map_battle_pass_times.begin(); it != m_map_battle_pass_times.end(); ++it)
	{
		std::vector<TExtBattlePassTimeTable> timeInfo = it->second;
		if (time(0) > timeInfo[0].dwStartTime && time(0) <timeInfo[0].dwEndTime)
		{
			findActiveBattlePass = true;
			cachedBattlePassID = timeInfo[0].bBattlePassId;
			cachedStartTime = timeInfo[0].dwStartTime;
			cachedEndTime = timeInfo[0].dwEndTime;
		}
	}

	if (findActiveBattlePass)
	{
		m_dwActiveBattlePassID = cachedBattlePassID;
		m_dwBattlePassStartTime = cachedStartTime;
		m_dwBattlePassEndTime = cachedEndTime;
	}
	else
	{
		m_dwActiveBattlePassID = 0;
		m_dwBattlePassStartTime = 0;
		m_dwBattlePassEndTime = 0;
	}
}

uint8_t CBattlePassManager::GetMissionIndex(uint8_t bMissionType, uint32_t dwCondition)
{
	bool isMinMissionType = false;
	if (bMissionType == 2
		|| bMissionType == 4
		|| bMissionType == 20)
	{
		isMinMissionType = true;
	}

	if (m_dwActiveBattlePassID == 0)
	{
		return 0;
	}

	auto it_normal = m_map_battle_pass_name.find(m_dwActiveBattlePassID);
	if (it_normal != m_map_battle_pass_name.end())
	{
		std::string battlePassName = it_normal->second;

		TMapBattleMissionInfo::iterator itNormalInfo = m_map_battle_pass_mission_info.find(battlePassName);
		std::vector<TExtBattlePassMissionInfo> normalMissionInfo = itNormalInfo->second;

		for (int i = 0; i < normalMissionInfo.size(); i++)
		{
			if (normalMissionInfo[i].dwMissionInfo[0] != 0
				&& isMinMissionType == false)
			{
				if (normalMissionInfo[i].bMissionType == bMissionType
					&& normalMissionInfo[i].dwMissionInfo[0] == dwCondition)
				{
					return normalMissionInfo[i].bMissionIndex;
				}
			}
			else
			{
				if (normalMissionInfo[i].bMissionType == bMissionType)
				{
					return normalMissionInfo[i].bMissionIndex;
				}
			}
		}

		return 0;
	}

	return 0;
}

uint8_t CBattlePassManager::GetMissionTypeByName(std::string stMissionName)
{
	for (int32_t i = 0; i < MISSION_TYPE_MAX; i++)
	{
		if (g_astMissionType[i] == stMissionName)
		{
			return i;
		}
	}

	return 0;
}

uint8_t CBattlePassManager::GetMissionTypeByIndex(uint32_t dwIndex)
{
	if (m_dwActiveBattlePassID == 0)
	{
		return 0;
	}

	auto it_normal = m_map_battle_pass_name.find(m_dwActiveBattlePassID);
	if (it_normal != m_map_battle_pass_name.end())
	{
		std::string battlePassName = it_normal->second;

		TMapBattleMissionInfo::iterator itNormalInfo = m_map_battle_pass_mission_info.find(battlePassName);
		std::vector<TExtBattlePassMissionInfo> normalMissionInfo = itNormalInfo->second;

		for (int32_t i = 0; i < normalMissionInfo.size(); i++)
		{
			if (normalMissionInfo[i].dwMissionInfo[0] != 0 && normalMissionInfo[i].bMissionIndex == dwIndex)
			{
				return normalMissionInfo[i].bMissionType;
			}
		}

		return 0;
	}

	return 0;
}

std::string CBattlePassManager::GetMissionNameByType(uint8_t bType)
{
	for (int32_t i = 0; i < MISSION_TYPE_MAX; i++)
	{
		if (i == bType)
		{
			return g_astMissionType[i];
		}
	}

	return "";
}

std::string CBattlePassManager::GetBattlePassNameByID(uint8_t bID)
{
	auto it = m_map_battle_pass_name.find(bID);
	return it == m_map_battle_pass_name.end() ? "" : it->second;
}

void CBattlePassManager::GetMissionSearchName(uint8_t bMissionType, std::string* st_name_1, std::string* st_name_2)
{
	switch (bMissionType)
	{
		case KILL_MONSTER:
		case DAMAGE_MONSTER:
		{
			*st_name_1 = "mob_vnum";
			*st_name_2 = "value";
			break;
		}
		case KILL_PLAYER:
		case DAMAGE_PLAYER:
		{
			*st_name_1 = "min_level";
			*st_name_2 = "value";
			break;
		}
		case BP_ITEM_USE:
		case BP_ITEM_SELL:
		case BP_ITEM_CRAFT:
		case BP_ITEM_REFINE:
		case BP_ITEM_DESTROY:
		case BP_ITEM_COLLECT:
		case FISH_FISHING:
		case FISH_GRILL:
		case FISH_CATCH:
		{
			*st_name_1 = "item_vnum";
			*st_name_2 = "count";
			break;
		}
		case EXP_COLLECT:
		{
			*st_name_1 = "mob_vnum";
			*st_name_2 = "value";
			break;
		}
		case YANG_COLLECT:
		{
			*st_name_1 = "map_idx";
			*st_name_2 = "value";
			break;
		}
		case GUILD_PLAY_GUILDWAR:
		{
			*st_name_1 = "war_type";
			*st_name_2 = "count";
			break;
		}
		case GUILD_SPENT_EXP:
		case GAYA_CRAFT_GAYA:
		case GAYA_BUY_ITEM_GAYA_COST:
		case CHAT_MESSAGE_SHOUT:
		{
			*st_name_1 = "";
			*st_name_2 = "value";
			break;
		}
		case COMPLETE_DUNGEON:
		{
			*st_name_1 = "dungeon_id";
			*st_name_2 = "count";
			break;
		}
		default:
		{
			*st_name_1 = "";
			*st_name_2 = "";
			break;
		}
	}
}

void CBattlePassManager::BattlePassRequestOpen(LPCHARACTER pkChar, bool bAfterOpen)
{
	const LPDESC d = pkChar ? pkChar->GetDesc() : nullptr;
	if (!d)
	{
		return;
	}

	if (get_dword_time() < pkChar->GetLastReciveExtBattlePassInfoTime())
	{
		return;
	}

	pkChar->SetLastReciveExtBattlePassInfoTime(get_dword_time() + 1000);

	if (m_dwActiveBattlePassID != 0)
	{
		{
			const char* season_name;
			auto it = m_map_battle_pass_name.find(m_dwActiveBattlePassID);
			if (it != m_map_battle_pass_name.end())
			{
				season_name = it->second.c_str();
			}
			else
			{
				season_name = "NoName";
			}

			for (auto it = m_map_battle_pass_times.begin(); it != m_map_battle_pass_times.end(); ++it)
			{
				std::vector<TExtBattlePassTimeTable> timeInfo = it->second;
				if (!timeInfo.empty())
				{
					TPacketGCExtBattlePassGeneralInfo packet;
					packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_GENERAL_INFO;
					strcpy(packet.szSeasonName, season_name);
					packet.dwBattlePassID = timeInfo[0].bBattlePassId;
					packet.dwBattlePassStartTime = timeInfo[0].dwStartTime;
					packet.dwBattlePassEndTime = timeInfo[0].dwEndTime;
					d->Packet(&packet, sizeof(TPacketGCExtBattlePassGeneralInfo));
				}
			}
		}

		{
			auto it_normal = m_map_battle_pass_name.find(m_dwActiveBattlePassID);
			if (it_normal != m_map_battle_pass_name.end())
			{
				std::string battlePassName = it_normal->second;

				TMapBattleMissionInfo::iterator itNormalInfo = m_map_battle_pass_mission_info.find(battlePassName);
				auto itNormalReward = m_map_battle_pass_reward.find(battlePassName);

				std::vector<TExtBattlePassMissionInfo> normalMissionInfo = itNormalInfo->second;
				std::vector<TExtBattlePassRewardItem> normalRewardInfo = itNormalReward->second;

				for (int32_t i = 0; i < normalMissionInfo.size(); i++)
				{
					normalMissionInfo[i].dwMissionInfo[2] = pkChar->GetExtBattlePassMissionProgress(normalMissionInfo[i].bMissionIndex, normalMissionInfo[i].bMissionType);
				}

				if (!normalMissionInfo.empty())
				{
					TPacketGCExtBattlePassMissionInfo packet;
					packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_INFO;
					packet.wSize = sizeof(packet) + sizeof(TExtBattlePassMissionInfo) * normalMissionInfo.size();
					packet.wRewardSize = sizeof(TExtBattlePassRewardItem) * normalRewardInfo.size();
					packet.dwBattlePassID = m_dwActiveBattlePassID;

					d->BufferedPacket(&packet, sizeof(packet));
					d->BufferedPacket(&normalMissionInfo[0], sizeof(TExtBattlePassMissionInfo) * normalMissionInfo.size());
					d->Packet(&normalRewardInfo[0], sizeof(TExtBattlePassRewardItem) * normalRewardInfo.size());
				}
			}
		}
	}

	if (bAfterOpen)
	{
		TPacketGCExtBattlePassOpen packet;
		packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_OPEN;
		d->Packet(&packet, sizeof(packet));
	}
}

void CBattlePassManager::BattlePassRewardMission(LPCHARACTER pkChar, uint32_t bBattlePassId, uint8_t bMissionIndex)
{
	const LPDESC d = pkChar ? pkChar->GetDesc() : nullptr;
	if (!d)
	{
		return;
	}

	auto it = m_map_battle_pass_name.find(bBattlePassId);
	if (it == m_map_battle_pass_name.end())
	{
		return;
	}

	std::string battlePassName = it->second;
	auto itInfo = m_map_battle_pass_mission_info.find(battlePassName);
	if (itInfo == m_map_battle_pass_mission_info.end())
	{
		return;
	}

	std::vector<TExtBattlePassMissionInfo> missionInfo = itInfo->second;
	for (int32_t i = 0; i < missionInfo.size(); i++)
	{
		if (missionInfo[i].bMissionIndex == bMissionIndex)
		{
			for (int32_t j = 0; j < 3; j++)
			{
				if (missionInfo[i].aRewardList[j].dwVnum && missionInfo[i].aRewardList[j].bCount > 0)
				{
					pkChar->AutoGiveItem(missionInfo[i].aRewardList[j].dwVnum, missionInfo[i].aRewardList[j].bCount);
				}
			}

			break;
		}
	}
}

void CBattlePassManager::BattlePassRequestReward(LPCHARACTER pkChar)
{
	const LPDESC d = pkChar ? pkChar->GetDesc() : nullptr;
	if (!d)
	{
		return;
	}

	uint8_t bBattlePassID = GetBattlePassID();
	if (bBattlePassID != pkChar->GetExtBattlePassPremiumID())
	{
		return;
	}

	auto it = m_map_battle_pass_name.find(bBattlePassID);
	if (it == m_map_battle_pass_name.end())
	{
		return;
	}

	std::string battlePassName = it->second;
	auto itInfo = m_map_battle_pass_mission_info.find(battlePassName);
	if (itInfo == m_map_battle_pass_mission_info.end())
	{
		return;
	}

	std::vector<TExtBattlePassMissionInfo> missionInfo = itInfo->second;

	for (int32_t i = 0; i < missionInfo.size(); i++)
	{
		if (!pkChar->IsExtBattlePassCompletedMission(missionInfo[i].bMissionIndex, missionInfo[i].bMissionType))
		{
			pkChar->ChatPacketNew(CHAT_TYPE_NOTICE, 1403, "");
			return;
		}
	}

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT battlepass_completed FROM player.battlepass_playerindex WHERE player_id = %u AND battlepass_id = %u", pkChar->GetPlayerID(), bBattlePassID));
	if (pMsg->uiSQLErrno)
	{
		return;
	}

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	int32_t bHasRecived;
	str_to_number(bHasRecived, row[0]);

	if (bHasRecived != 0)
	{
		pkChar->ChatPacketNew(CHAT_TYPE_NOTICE, 1406, "");
		return;
	}

	DBManager::instance().DirectQuery("UPDATE player.battlepass_playerindex SET battlepass_completed = 1, end_time = NOW() WHERE player_id = %u and battlepass_id = %u", pkChar->GetPlayerID(), bBattlePassID);

	if (bBattlePassID)
	{
		BattlePassReward(pkChar, bBattlePassID);
	}
}

void CBattlePassManager::BattlePassReward(LPCHARACTER pkChar, uint8_t bBattlePassID)
{
	auto it = m_map_battle_pass_name.find(bBattlePassID);
	if (it == m_map_battle_pass_name.end())
	{
		return;
	}

	std::string battlePassName = it->second;
	auto itReward = m_map_battle_pass_reward.find(battlePassName);
	if (itReward == m_map_battle_pass_reward.end())
	{
		return;
	}

	std::vector<TExtBattlePassRewardItem> rewardInfo = itReward->second;
	for (int32_t i = 0; i < rewardInfo.size(); i++)
	{
		pkChar->AutoGiveItem(rewardInfo[i].dwVnum, rewardInfo[i].bCount);
	}

	pkChar->EffectPacket(SE_EFFECT_BP_COMPLETED);
}

bool CBattlePassManager::BattlePassMissionGetInfo(uint8_t bMissionIndex, uint8_t bBattlePassId, uint8_t bMissionType, DWORD* dwFirstInfo, DWORD* dwSecondInfo)
{
	auto it = m_map_battle_pass_name.find(bBattlePassId);
	if (it == m_map_battle_pass_name.end())
	{
		return false;
	}

	std::string battlePassName = it->second;
	auto itInfo = m_map_battle_pass_mission_info.find(battlePassName);

	if (itInfo == m_map_battle_pass_mission_info.end())
	{
		return false;
	}

	std::vector<TExtBattlePassMissionInfo> missionInfo = itInfo->second;
	for (int32_t i = 0; i < missionInfo.size(); i++)
	{
		if (missionInfo[i].bMissionIndex == bMissionIndex && missionInfo[i].bMissionType == bMissionType)
		{
			*dwFirstInfo = missionInfo[i].dwMissionInfo[0];
			*dwSecondInfo = missionInfo[i].dwMissionInfo[1];
			return true;
		}
	}

	return false;
}
#endif
