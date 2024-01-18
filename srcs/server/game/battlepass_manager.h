#pragma once

#if defined(USE_BATTLEPASS)
#ifndef __INC_METIN_II_BATTLE_PASS_H__
#define __INC_METIN_II_BATTLE_PASS_H__

#include "../common/stl.h"
#include "../common/length.h"
#include "../common/tables.h"
#include "group_text_parse_tree.h"
#include "packet.h"

class CBattlePassManager : public singleton<CBattlePassManager>
{
	public:
		CBattlePassManager();
		virtual ~CBattlePassManager();

		bool InitializeBattlePass();

		bool ReadBattlePassGroup();
		bool ReadBattlePassMissions();

		void CheckBattlePassTimes();
		BYTE GetMissionIndex(uint8_t, uint32_t);
		BYTE GetMissionTypeByName(std::string);
		uint8_t GetMissionTypeByIndex(uint32_t);
		std::string GetMissionNameByType(BYTE);
		
		std::string GetBattlePassNameByID(BYTE);

		void GetMissionSearchName(BYTE, std::string*, std::string*);

		void BattlePassRequestOpen(LPCHARACTER, bool bAfterOpen = true);
		void BattlePassRewardMission(LPCHARACTER, uint32_t, uint8_t);
		bool BattlePassMissionGetInfo(uint8_t, uint8_t, uint8_t, DWORD*, DWORD*);

		void BattlePassRequestReward(LPCHARACTER);
		void BattlePassReward(LPCHARACTER, BYTE);

		DWORD GetBattlePassID() const { return m_dwActiveBattlePassID; }
		DWORD GetBattlePassStartTime() const { return m_dwBattlePassStartTime; }
		DWORD GetBattlePassEndTime() const { return m_dwBattlePassEndTime; }

	protected:
		DWORD	m_dwBattlePassStartTime;
		DWORD	m_dwBattlePassEndTime;
		DWORD	m_dwActiveBattlePassID;

	private:
		CGroupTextParseTreeLoader* m_pLoader;

		typedef std::map <BYTE, std::string> TMapBattlePassName;
		typedef std::map <std::string, std::vector<TExtBattlePassRewardItem>> TMapBattlePassReward;
		typedef std::map <std::string, std::vector<TExtBattlePassMissionInfo>> TMapBattleMissionInfo;

		TMapBattlePassName m_map_battle_pass_name;
		TMapBattlePassReward m_map_battle_pass_reward;
		TMapBattleMissionInfo m_map_battle_pass_mission_info;

		typedef std::map <BYTE, std::vector<TExtBattlePassTimeTable>> TMapBattlePassTimeTable;
		TMapBattlePassTimeTable m_map_battle_pass_times;
};
#endif
#endif
