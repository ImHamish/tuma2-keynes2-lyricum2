#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "desc.h"
#include "char.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "party.h"
#include "regen.h"
#include "p2p.h"
#include "dungeon.h"
#include "db.h"
#include "config.h"
#include "questmanager.h"
#include "questlua.h"
#include "locale_service.h"
#include "shutdown_manager.h"
#include "../common/CommonDefines.h"
#if defined(ENABLE_INGAME_ITEMSHOP)
#include "log.h"
#endif
#include "desc_manager.h"
#include <iterator>

CHARACTER_MANAGER::CHARACTER_MANAGER() :
	m_iVIDCount(0),
	m_pkChrSelectedStone(NULL),
	m_bUsePendingDestroy(false)
{
	m_iMobItemRate = 100;
	m_iMobDamageRate = 100;
	m_iMobGoldAmountRate = 100;
	m_iMobGoldDropRate = 100;
	m_iMobExpRate = 100;

	m_iMobItemRatePremium = 100;
	m_iMobGoldAmountRatePremium = 100;
	m_iMobGoldDropRatePremium = 100;
	m_iMobExpRatePremium = 100;

	m_iUserDamageRate = 100;
	m_iUserDamageRatePremium = 100;
}

CHARACTER_MANAGER::~CHARACTER_MANAGER()
{
	Destroy();
}

void CHARACTER_MANAGER::Destroy()
{
	auto it = m_map_pkChrByVID.begin();
	while (it != m_map_pkChrByVID.end()) {
		LPCHARACTER ch = it->second;
		M2_DESTROY_CHARACTER(ch); // m_map_pkChrByVID is changed here
		it = m_map_pkChrByVID.begin();
	}

#if defined(ENABLE_INGAME_ITEMSHOP)
	m_IShopManager.clear();
#endif
}

void CHARACTER_MANAGER::GracefulShutdown()
{
	NAME_MAP::iterator it = m_map_pkPCChr.begin();

	while (it != m_map_pkPCChr.end())
		(it++)->second->Disconnect("GracefulShutdown");
}

DWORD CHARACTER_MANAGER::AllocVID()
{
	++m_iVIDCount;
	return m_iVIDCount;
}

LPCHARACTER CHARACTER_MANAGER::CreateCharacter(const char * name, DWORD dwPID)
{
	DWORD dwVID = AllocVID();

#ifdef M2_USE_POOL
	LPCHARACTER ch = pool_.Construct();
#else
	LPCHARACTER ch = M2_NEW CHARACTER;
#endif
	ch->Create(name, dwVID, dwPID ? true : false);

//	if (dwVID != ch->GetVID()) {
//		--m_iVIDCount;
//		M2_DESTROY_CHARACTER(ch);
//		return NULL;
//	}

	m_map_pkChrByVID.insert(std::make_pair(dwVID, ch));

	if (dwPID)
	{
		char szName[CHARACTER_NAME_MAX_LEN + 1];
		str_lower(name, szName, sizeof(szName));

		m_map_pkPCChr.insert(NAME_MAP::value_type(szName, ch));
		m_map_pkChrByPID.insert(std::make_pair(dwPID, ch));
	}

	return (ch);
}

#ifndef DEBUG_ALLOC
void CHARACTER_MANAGER::DestroyCharacter(LPCHARACTER ch)
#else
void CHARACTER_MANAGER::DestroyCharacter(LPCHARACTER ch, const char* file, size_t line)
#endif
{
	if (!ch)
		return;

	// <Factor> Check whether it has been already deleted or not.
	auto it = m_map_pkChrByVID.find(ch->GetVID());
	if (it == m_map_pkChrByVID.end()) {
		sys_err("[CHARACTER_MANAGER::DestroyCharacter] <Factor> %d not found", (DWORD)(ch->GetVID()));
		return; // prevent duplicated destrunction
	}

	// 던전에 소속된 몬스터는 던전에서도 삭제하도록.
#ifdef __NEWPET_SYSTEM__
	if (ch->IsNPC() && !ch->IsPet() && !ch->IsNewPet() && ch->GetRider() == NULL)
#else
	if (ch->IsNPC() && !ch->IsPet() && ch->GetRider() == NULL)
#endif
	{
		if (ch->GetDungeon())
		{
			ch->GetDungeon()->DeadCharacter(ch);
		}
	}

	if (m_bUsePendingDestroy)
	{
		m_set_pkChrPendingDestroy.insert(ch);
		return;
	}

	CHARACTER_SET::iterator it2 = m_set_pkChrForDelayedSave.find(ch);
	if (it2 != m_set_pkChrForDelayedSave.end()) {
		ch->SaveReal();
		m_set_pkChrForDelayedSave.erase(it2);
	}

	m_map_pkChrByVID.erase(it);

	if (true == ch->IsPC())
	{
		char szName[CHARACTER_NAME_MAX_LEN + 1];

		str_lower(ch->GetName(), szName, sizeof(szName));

		NAME_MAP::iterator it = m_map_pkPCChr.find(szName);

		if (m_map_pkPCChr.end() != it)
			m_map_pkPCChr.erase(it);
	}

	if (0 != ch->GetPlayerID())
	{
		auto it = m_map_pkChrByPID.find(ch->GetPlayerID());

		if (m_map_pkChrByPID.end() != it)
		{
			m_map_pkChrByPID.erase(it);
		}
	}

	if (ch->IsPC()) {
		auto it = m_set_pkChrForDelayedSave.find(ch);
		if (m_set_pkChrForDelayedSave.end() != it)
		{
			ch->SaveReal();
			m_set_pkChrForDelayedSave.erase(it);
		}
	}

	UnregisterRaceNumMap(ch);

	RemoveFromStateList(ch);

#ifdef M2_USE_POOL
	pool_.Destroy(ch);
#else
#ifndef DEBUG_ALLOC
	M2_DELETE(ch);
#else
	M2_DELETE_EX(ch, file, line);
#endif
#endif
}

LPCHARACTER CHARACTER_MANAGER::Find(DWORD dwVID)
{
	auto it = m_map_pkChrByVID.find(dwVID);

	if (m_map_pkChrByVID.end() == it)
		return NULL;

	// <Factor> Added sanity check
	LPCHARACTER found = it->second;
	if (found != NULL && dwVID != (DWORD)found->GetVID()) {
		sys_err("[CHARACTER_MANAGER::Find] <Factor> %u != %u", dwVID, (DWORD)found->GetVID());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::Find(const VID & vid)
{
	LPCHARACTER tch = Find((DWORD) vid);

	if (!tch || tch->GetVID() != vid)
		return NULL;

	return tch;
}

LPCHARACTER CHARACTER_MANAGER::FindByPID(DWORD dwPID)
{
	auto it = m_map_pkChrByPID.find(dwPID);

	if (m_map_pkChrByPID.end() == it)
		return NULL;

	// <Factor> Added sanity check
	LPCHARACTER found = it->second;
	if (found != NULL && dwPID != found->GetPlayerID()) {
		sys_err("[CHARACTER_MANAGER::FindByPID] <Factor> %u != %u", dwPID, found->GetPlayerID());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::FindPC(const char * name)
{
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	str_lower(name, szName, sizeof(szName));
	NAME_MAP::iterator it = m_map_pkPCChr.find(szName);

	if (it == m_map_pkPCChr.end())
		return NULL;

	// <Factor> Added sanity check
	LPCHARACTER found = it->second;
	if (found != NULL && strncasecmp(szName, found->GetName(), CHARACTER_NAME_MAX_LEN) != 0) {
		sys_err("[CHARACTER_MANAGER::FindPC] <Factor> %s != %s", name, found->GetName());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::SpawnMobRandomPosition(DWORD dwVnum, long lMapIndex)
{
	const CMob * pkMob = CMobManager::instance().Get(dwVnum);

	if (!pkMob)
	{
		sys_err("no mob data for vnum %u", dwVnum);
		return NULL;
	}

	if (!map_allow_find(lMapIndex))
	{
		sys_err("not allowed map %u", lMapIndex);
		return NULL;
	}

	LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);
	if (pkSectreeMap == NULL) {
		return NULL;
	}

	int i;
	long x, y;
	for (i=0; i<2000; i++)
	{
		x = number(1, (pkSectreeMap->m_setting.iWidth / 100)  - 1) * 100 + pkSectreeMap->m_setting.iBaseX;
		y = number(1, (pkSectreeMap->m_setting.iHeight / 100) - 1) * 100 + pkSectreeMap->m_setting.iBaseY;
		//LPSECTREE tree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);
		LPSECTREE tree = pkSectreeMap->Find(x, y);

		if (!tree)
			continue;

		DWORD dwAttr = tree->GetAttribute(x, y);

		if (IS_SET(dwAttr, ATTR_BLOCK | ATTR_OBJECT))
			continue;

		if (IS_SET(dwAttr, ATTR_BANPK))
			continue;

		break;
	}

	if (i == 2000)
	{
		sys_err("cannot find valid location");
		return NULL;
	}

	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "SpawnMobRandomPosition: cannot create monster at non-exist sectree %d x %d (map %d)", x, y, lMapIndex);
		return NULL;
	}

#ifdef ENABLE_MULTI_NAMES
	LPCHARACTER ch = CHARACTER_MANAGER::instance().CreateCharacter(pkMob->m_table.szLocaleName[DEFAULT_LANGUAGE]);
#else
	LPCHARACTER ch = CHARACTER_MANAGER::instance().CreateCharacter(pkMob->m_table.szLocaleName);
#endif

	if (!ch)
	{
		sys_log(0, "SpawnMobRandomPosition: cannot create new character");
		return NULL;
	}

	ch->SetProto(pkMob);

	// if mob is npc with no empire assigned, assign to empire of map
	if (pkMob->m_table.bType == CHAR_TYPE_NPC)
		if (ch->GetEmpire() == 0)
			ch->SetEmpire(SECTREE_MANAGER::instance().GetEmpireFromMapIndex(lMapIndex));

	ch->SetRotation(number(0, 360));

	if (!ch->Show(lMapIndex, x, y, 0, false))
	{
		M2_DESTROY_CHARACTER(ch);
		sys_err(0, "SpawnMobRandomPosition: cannot show monster");
		return NULL;
	}

	return (ch);
}

LPCHARACTER CHARACTER_MANAGER::SpawnMob(DWORD dwVnum, long lMapIndex, long x, long y, long z, bool bSpawnMotion, int iRot, bool bShow)
{
	const CMob * pkMob = CMobManager::instance().Get(dwVnum);
	if (!pkMob)
	{
		sys_err("SpawnMob: no mob data for vnum %u", dwVnum);
		return NULL;
	}

	if (!(pkMob->m_table.bType == CHAR_TYPE_NPC || pkMob->m_table.bType == CHAR_TYPE_WARP || pkMob->m_table.bType == CHAR_TYPE_GOTO) || mining::IsVeinOfOre (dwVnum))
	{
		LPSECTREE tree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

		if (!tree)
		{
			sys_log(0, "no sectree for spawn at %d %d mobvnum %d mapindex %d", x, y, dwVnum, lMapIndex);
			return NULL;
		}

		DWORD dwAttr = tree->GetAttribute(x, y);

		bool is_set = false;

		if ( mining::IsVeinOfOre (dwVnum) ) is_set = IS_SET(dwAttr, ATTR_BLOCK);
		else is_set = IS_SET(dwAttr, ATTR_BLOCK | ATTR_OBJECT);

		if ( is_set )
		{
			// SPAWN_BLOCK_LOG
			static bool s_isLog=quest::CQuestManager::instance().GetEventFlag("spawn_block_log");
			static DWORD s_nextTime=get_global_time()+10000;

			DWORD curTime=get_global_time();

			if (curTime>s_nextTime)
			{
				s_nextTime=curTime;
				s_isLog=quest::CQuestManager::instance().GetEventFlag("spawn_block_log");

			}

			if (s_isLog)
				sys_log(0, "SpawnMob: BLOCKED position for spawn %s %u at %d %d (attr %u)", pkMob->m_table.szName, dwVnum, x, y, dwAttr);
			// END_OF_SPAWN_BLOCK_LOG
			return NULL;
		}

		if (IS_SET(dwAttr, ATTR_BANPK))
		{
			//sys_err("SpawnMob: BAN_PK position for mob spawn %s %u at %d %d", pkMob->m_table.szName, dwVnum, x, y);
			sys_log(0, "SpawnMob: BAN_PK position for mob spawn %s %u at %d %d", pkMob->m_table.szName, dwVnum, x, y);
			return NULL;
		}
	}

	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		//sys_err("SpawnMob: cannot create monster at non-exist sectree %d x %d (map %d)", x, y, lMapIndex);
		sys_log(0, "SpawnMob: cannot create monster at non-exist sectree %d x %d (map %d)", x, y, lMapIndex);
		return NULL;
	}

#ifdef ENABLE_MULTI_NAMES
	LPCHARACTER ch = CHARACTER_MANAGER::instance().CreateCharacter(pkMob->m_table.szLocaleName[DEFAULT_LANGUAGE]);
#else
	LPCHARACTER ch = CHARACTER_MANAGER::instance().CreateCharacter(pkMob->m_table.szLocaleName);
#endif

	if (!ch)
	{
		//sys_err("SpawnMob: cannot create new character");
		sys_log(0, "SpawnMob: cannot create new character");
		return NULL;
	}

	if (iRot == -1)
		iRot = number(0, 360);

	ch->SetProto(pkMob);

	// if mob is npc with no empire assigned, assign to empire of map
	if (pkMob->m_table.bType == CHAR_TYPE_NPC)
		if (ch->GetEmpire() == 0)
			ch->SetEmpire(SECTREE_MANAGER::instance().GetEmpireFromMapIndex(lMapIndex));

#ifdef ENABLE_ANCIENT_PYRAMID
	ch->SetRotation(iRot, true);
#else
	ch->SetRotation(iRot);
#endif

	if (bShow && !ch->Show(lMapIndex, x, y, z, bSpawnMotion))
	{
		M2_DESTROY_CHARACTER(ch);
		sys_log(0, "SpawnMob: cannot show monster");
		return NULL;
	}

	return (ch);
}

LPCHARACTER CHARACTER_MANAGER::SpawnMobRange(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, bool bIsException, bool bSpawnMotion, bool bAggressive )
{
	const CMob * pkMob = CMobManager::instance().Get(dwVnum);

	if (!pkMob)
		return NULL;

	if (pkMob->m_table.bType == CHAR_TYPE_STONE)
		bSpawnMotion = true;

	int i = 16;

	while (i--)
	{
		int x = number(sx, ex);
		int y = number(sy, ey);
		/*
		   if (bIsException)
		   if (is_regen_exception(x, y))
		   continue;
		 */
		LPCHARACTER ch = SpawnMob(dwVnum, lMapIndex, x, y, 0, bSpawnMotion);

		if (ch)
		{
			sys_log(1, "MOB_SPAWN: %s(%d) %dx%d", ch->GetName(), (DWORD) ch->GetVID(), ch->GetX(), ch->GetY());
			if ( bAggressive )
				ch->SetAggressive();
			return (ch);
		}
	}

	return NULL;
}

void CHARACTER_MANAGER::SelectStone(LPCHARACTER pkChr)
{
	m_pkChrSelectedStone = pkChr;
}

bool CHARACTER_MANAGER::SpawnMoveGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, int tx, int ty, LPREGEN pkRegen, bool bAggressive_)
{
	
	if (dwVnum == 0)
		return false;
	
	CMobGroup * pkGroup = CMobManager::Instance().GetGroup(dwVnum);

	if (!pkGroup)
	{
		sys_err("NOT_EXIST_GROUP_VNUM(%u) Map(%u) ", dwVnum, lMapIndex);
		return false;
	}

	LPCHARACTER pkChrMaster = NULL;
	LPPARTY pkParty = NULL;

	const std::vector<DWORD> & c_rdwMembers = pkGroup->GetMemberVector();

	bool bSpawnedByStone = false;
	bool bAggressive = bAggressive_;

	if (m_pkChrSelectedStone)
	{
		bSpawnedByStone = true;
		if (m_pkChrSelectedStone->GetDungeon())
			bAggressive = true;
	}

	for (DWORD i = 0; i < c_rdwMembers.size(); ++i)
	{
		LPCHARACTER tch = SpawnMobRange(c_rdwMembers[i], lMapIndex, sx, sy, ex, ey, true, bSpawnedByStone);

		if (!tch)
		{
			if (i == 0)
				return false;

			continue;
		}

		sx = tch->GetX() - number(300, 500);
		sy = tch->GetY() - number(300, 500);
		ex = tch->GetX() + number(300, 500);
		ey = tch->GetY() + number(300, 500);

		if (m_pkChrSelectedStone)
			tch->SetStone(m_pkChrSelectedStone);
		else if (pkParty)
		{
			pkParty->Join(tch->GetVID());
			pkParty->Link(tch);
		}
		else if (!pkChrMaster)
		{
			pkChrMaster = tch;
			pkChrMaster->SetRegen(pkRegen);

			pkParty = CPartyManager::instance().CreateParty(pkChrMaster);
		}
		if (bAggressive)
			tch->SetAggressive();

		if (tch->Goto(tx, ty))
			tch->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	}

	return true;
}

bool CHARACTER_MANAGER::SpawnGroupGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen, bool bAggressive_, LPDUNGEON pDungeon)
{
	const DWORD dwGroupID = CMobManager::Instance().GetGroupFromGroupGroup(dwVnum);

	if( dwGroupID != 0 )
	{
		return SpawnGroup(dwGroupID, lMapIndex, sx, sy, ex, ey, pkRegen, bAggressive_, pDungeon);
	}
	else
	{
		sys_err( "NOT_EXIST_GROUP_GROUP_VNUM(%u) MAP(%ld)", dwVnum, lMapIndex );
		return false;
	}
}

LPCHARACTER CHARACTER_MANAGER::SpawnGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen, bool bAggressive_, LPDUNGEON pDungeon)
{
	
	if (dwVnum == 0)
		return NULL;
	
	CMobGroup * pkGroup = CMobManager::Instance().GetGroup(dwVnum);

	if (!pkGroup)
	{
		sys_err("NOT_EXIST_GROUP_VNUM(%u) Map(%u) ", dwVnum, lMapIndex);
		return NULL;
	}

	LPCHARACTER pkChrMaster = NULL;
	LPPARTY pkParty = NULL;

	const std::vector<DWORD> & c_rdwMembers = pkGroup->GetMemberVector();

	bool bSpawnedByStone = false;
	bool bAggressive = bAggressive_;

	if (m_pkChrSelectedStone)
	{
		bSpawnedByStone = true;

		if (m_pkChrSelectedStone->GetDungeon())
			bAggressive = true;
	}

	LPCHARACTER chLeader = NULL;

	for (DWORD i = 0; i < c_rdwMembers.size(); ++i)
	{
		LPCHARACTER tch = SpawnMobRange(c_rdwMembers[i], lMapIndex, sx, sy, ex, ey, true, bSpawnedByStone);

		if (!tch)
		{
			if (i == 0)
				return NULL;

			continue;
		}

		if (i == 0)
			chLeader = tch;

		tch->SetDungeon(pDungeon);

		sx = tch->GetX() - number(300, 500);
		sy = tch->GetY() - number(300, 500);
		ex = tch->GetX() + number(300, 500);
		ey = tch->GetY() + number(300, 500);

		if (m_pkChrSelectedStone)
			tch->SetStone(m_pkChrSelectedStone);
		else if (pkParty)
		{
			pkParty->Join(tch->GetVID());
			pkParty->Link(tch);
		}
		else if (!pkChrMaster)
		{
			pkChrMaster = tch;
			pkChrMaster->SetRegen(pkRegen);

			pkParty = CPartyManager::instance().CreateParty(pkChrMaster);
		}

		if (bAggressive)
			tch->SetAggressive();
	}

	return chLeader;
}

struct FuncUpdateAndResetChatCounter
{
	void operator () (LPCHARACTER ch)
	{
		ch->ResetChatCounter();
		ch->CFSM::Update();
	}
};

void CHARACTER_MANAGER::Update(int iPulse)
{
	BeginPendingDestroy();

	if (!m_map_pkPCChr.empty()) {
		const auto resetChatCounter = !(iPulse % PASSES_PER_SEC(5));

		std::for_each(m_map_pkPCChr.begin(), m_map_pkPCChr.end(),
			[&resetChatCounter, &iPulse](const auto& v)
			{
				LPCHARACTER ch = v.second;
				if (ch != nullptr)
				{
					if (resetChatCounter)
					{
						ch->ResetChatCounter();
						ch->CFSM::Update();
					}

					ch->UpdateCharacter(iPulse);
				}
			}
		);
	}

	if (!m_set_pkChrState.empty()) {
		std::for_each(m_set_pkChrState.begin(), m_set_pkChrState.end(), [iPulse](LPCHARACTER ch) { if (ch != nullptr) { ch->UpdateStateMachine(iPulse); } });
	}

	if (0 == (iPulse % PASSES_PER_SEC(3600))) {
		for (auto it = m_map_dwMobKillCount.begin(); it != m_map_dwMobKillCount.end(); ++it) {
			DBManager::instance().SendMoneyLog(MONEY_LOG_MONSTER_KILL, it->first, it->second);
		}

		m_map_dwMobKillCount.clear();
	}

	if (test_server && 0 == (iPulse % PASSES_PER_SEC(60)))
		sys_log(0, "CHARACTER COUNT vid %zu pid %zu", m_map_pkChrByVID.size(), m_map_pkChrByPID.size());

	FlushPendingDestroy();

	// ShutdownManager Update
	CShutdownManager::Instance().Update();
}

void CHARACTER_MANAGER::ProcessDelayedSave()
{
	CHARACTER_SET::iterator it = m_set_pkChrForDelayedSave.begin();
	while (it != m_set_pkChrForDelayedSave.end())
	{
		LPCHARACTER pkChr = *it++;
		pkChr->SaveReal();
	}

	m_set_pkChrForDelayedSave.clear();
}

bool CHARACTER_MANAGER::AddToStateList(LPCHARACTER ch)
{
	assert(ch != NULL);

	CHARACTER_SET::iterator it = m_set_pkChrState.find(ch);

	if (it == m_set_pkChrState.end())
	{
		m_set_pkChrState.insert(ch);
		return true;
	}

	return false;
}

void CHARACTER_MANAGER::RemoveFromStateList(LPCHARACTER ch)
{
	CHARACTER_SET::iterator it = m_set_pkChrState.find(ch);

	if (it != m_set_pkChrState.end())
	{
		//sys_log(0, "RemoveFromStateList %p", ch);
		m_set_pkChrState.erase(it);
	}
}

void CHARACTER_MANAGER::DelayedSave(LPCHARACTER ch) {
	m_set_pkChrForDelayedSave.insert(ch);
}

bool CHARACTER_MANAGER::FlushDelayedSave(LPCHARACTER ch)
{
	CHARACTER_SET::iterator it = m_set_pkChrForDelayedSave.find(ch);

	if (it == m_set_pkChrForDelayedSave.end())
		return false;

	m_set_pkChrForDelayedSave.erase(it);
	ch->SaveReal();
	return true;
}

void CHARACTER_MANAGER::RegisterForMonsterLog(LPCHARACTER ch)
{
	m_set_pkChrMonsterLog.insert(ch);
}

void CHARACTER_MANAGER::UnregisterForMonsterLog(LPCHARACTER ch)
{
	m_set_pkChrMonsterLog.erase(ch);
}

void CHARACTER_MANAGER::PacketMonsterLog(LPCHARACTER ch, const void* buf, int size)
{
	for (auto it = m_set_pkChrMonsterLog.begin(); it!=m_set_pkChrMonsterLog.end();++it)
	{
		LPCHARACTER c = *it;

		if (ch && DISTANCE_APPROX(c->GetX()-ch->GetX(), c->GetY()-ch->GetY())>6000)
			continue;

		LPDESC d = c->GetDesc();

		if (d)
			d->Packet(buf, size);
	}
}

void CHARACTER_MANAGER::KillLog(DWORD dwVnum)
{
	const DWORD SEND_LIMIT = 10000;

	auto it = m_map_dwMobKillCount.find(dwVnum);

	if (it == m_map_dwMobKillCount.end())
		m_map_dwMobKillCount.insert(std::make_pair(dwVnum, 1));
	else
	{
		++it->second;

		if (it->second > SEND_LIMIT)
		{
			DBManager::instance().SendMoneyLog(MONEY_LOG_MONSTER_KILL, it->first, it->second);
			m_map_dwMobKillCount.erase(it);
		}
	}
}

void CHARACTER_MANAGER::RegisterRaceNum(DWORD dwVnum)
{
	m_set_dwRegisteredRaceNum.insert(dwVnum);
}

void CHARACTER_MANAGER::RegisterRaceNumMap(LPCHARACTER ch)
{
	DWORD dwVnum = ch->GetRaceNum();

	if (m_set_dwRegisteredRaceNum.find(dwVnum) != m_set_dwRegisteredRaceNum.end())
	{
		sys_log(0, "RegisterRaceNumMap %s %u", ch->GetName(), dwVnum);
		m_map_pkChrByRaceNum[dwVnum].insert(ch);
	}
}

void CHARACTER_MANAGER::UnregisterRaceNumMap(LPCHARACTER ch)
{
	DWORD dwVnum = ch->GetRaceNum();

	auto it = m_map_pkChrByRaceNum.find(dwVnum);

	if (it != m_map_pkChrByRaceNum.end())
		it->second.erase(ch);
}

bool CHARACTER_MANAGER::GetCharactersByRaceNum(DWORD dwRaceNum, CharacterVectorInteractor & i)
{
	std::map<DWORD, CHARACTER_SET>::iterator it = m_map_pkChrByRaceNum.find(dwRaceNum);

	if (it == m_map_pkChrByRaceNum.end())
		return false;

	i = it->second;
	return true;
}

#define FIND_JOB_WARRIOR_0	(1 << 3)
#define FIND_JOB_WARRIOR_1	(1 << 4)
#define FIND_JOB_WARRIOR_2	(1 << 5)
#define FIND_JOB_WARRIOR	(FIND_JOB_WARRIOR_0 | FIND_JOB_WARRIOR_1 | FIND_JOB_WARRIOR_2)
#define FIND_JOB_ASSASSIN_0	(1 << 6)
#define FIND_JOB_ASSASSIN_1	(1 << 7)
#define FIND_JOB_ASSASSIN_2	(1 << 8)
#define FIND_JOB_ASSASSIN	(FIND_JOB_ASSASSIN_0 | FIND_JOB_ASSASSIN_1 | FIND_JOB_ASSASSIN_2)
#define FIND_JOB_SURA_0		(1 << 9)
#define FIND_JOB_SURA_1		(1 << 10)
#define FIND_JOB_SURA_2		(1 << 11)
#define FIND_JOB_SURA		(FIND_JOB_SURA_0 | FIND_JOB_SURA_1 | FIND_JOB_SURA_2)
#define FIND_JOB_SHAMAN_0	(1 << 12)
#define FIND_JOB_SHAMAN_1	(1 << 13)
#define FIND_JOB_SHAMAN_2	(1 << 14)
#define FIND_JOB_SHAMAN		(FIND_JOB_SHAMAN_0 | FIND_JOB_SHAMAN_1 | FIND_JOB_SHAMAN_2)

//
// (job+1)*3+(skill_group)
//
LPCHARACTER CHARACTER_MANAGER::FindSpecifyPC(unsigned int uiJobFlag, long lMapIndex, LPCHARACTER except, int iMinLevel, int iMaxLevel)
{
	LPCHARACTER chFind = NULL;
	int n = 0;

	for (auto it = m_map_pkChrByPID.begin(); it != m_map_pkChrByPID.end(); ++it)
	{
		LPCHARACTER ch = it->second;

		if (ch == except)
			continue;

		if (ch->GetLevel() < iMinLevel)
			continue;

		if (ch->GetLevel() > iMaxLevel)
			continue;

		if (ch->GetMapIndex() != lMapIndex)
			continue;

		if (uiJobFlag)
		{
			unsigned int uiChrJob = (1 << ((ch->GetJob() + 1) * 3 + ch->GetSkillGroup()));

			if (!IS_SET(uiJobFlag, uiChrJob))
				continue;
		}

		if (!chFind || number(1, ++n) == 1)
			chFind = ch;
	}

	return chFind;
}

int CHARACTER_MANAGER::GetMobItemRate(LPCHARACTER ch)
{
	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_ITEM) > 0)
		return m_iMobItemRatePremium;
	return m_iMobItemRate;
}

int CHARACTER_MANAGER::GetMobDamageRate(LPCHARACTER ch)
{
	return m_iMobDamageRate;
}

int CHARACTER_MANAGER::GetMobGoldAmountRate(LPCHARACTER ch)
{
	if ( !ch )
		return m_iMobGoldAmountRate;

	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0)
		return m_iMobGoldAmountRatePremium;
	return m_iMobGoldAmountRate;
}

int CHARACTER_MANAGER::GetMobGoldDropRate(LPCHARACTER ch)
{
	if (!ch) {
		return m_iMobGoldDropRate;
	}

	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0) {
		return m_iMobGoldDropRatePremium;
	}

	return m_iMobGoldDropRate;
}

int CHARACTER_MANAGER::GetMobExpRate(LPCHARACTER ch)
{
	if (!ch) {
		return m_iMobExpRate;
	}

	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_EXP) > 0) {
		return m_iMobExpRatePremium;
	}

	return m_iMobExpRate;
}

int	CHARACTER_MANAGER::GetUserDamageRate(LPCHARACTER ch)
{
	if (!ch)
		return m_iUserDamageRate;

	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
		return m_iUserDamageRatePremium;

	return m_iUserDamageRate;
}

void CHARACTER_MANAGER::SendScriptToMap(long lMapIndex, const std::string & s)
{
	LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);

	if (NULL == pSecMap)
		return;

	struct packet_script p;

	p.header = HEADER_GC_SCRIPT;
	p.skin = 1;
	p.src_size = s.size();

	quest::FSendPacket f;
	p.size = p.src_size + sizeof(struct packet_script);
	f.buf.write(&p, sizeof(struct packet_script));
	f.buf.write(&s[0], s.size());

	pSecMap->for_each(f);
}

bool CHARACTER_MANAGER::BeginPendingDestroy()
{
	if (m_bUsePendingDestroy)
		return false;

	m_bUsePendingDestroy = true;
	return true;
}

void CHARACTER_MANAGER::FlushPendingDestroy()
{
	m_bUsePendingDestroy = false;

	if (!m_set_pkChrPendingDestroy.empty())
	{
		sys_log(0, "FlushPendingDestroy size %d", m_set_pkChrPendingDestroy.size());

		CHARACTER_SET::iterator it = m_set_pkChrPendingDestroy.begin(),
			end = m_set_pkChrPendingDestroy.end();
		for ( ; it != end; ++it) {
			M2_DESTROY_CHARACTER(*it);
		}

		m_set_pkChrPendingDestroy.clear();
	}
}

CharacterVectorInteractor::CharacterVectorInteractor(const CHARACTER_SET & r) : m_bMyBegin(false)
{
	using namespace std;

	reserve(r.size());
	insert(end(), r.begin(), r.end());

	if (CHARACTER_MANAGER::instance().BeginPendingDestroy())
		m_bMyBegin = true;
}

CharacterVectorInteractor::~CharacterVectorInteractor()
{
	if (m_bMyBegin)
		CHARACTER_MANAGER::instance().FlushPendingDestroy();
}


#ifdef ENABLE_EVENT_MANAGER
#include "item_manager.h"
#include "item.h"
#include "desc_client.h"
#include "desc_manager.h"
void CHARACTER_MANAGER::ClearEventData()
{
	m_eventData.clear();
}
void CHARACTER_MANAGER::CheckBonusEvent(LPCHARACTER ch)
{
	const TEventManagerData* eventPtr = CheckEventIsActive(BONUS_EVENT, ch->GetEmpire());
	if (eventPtr)
		ch->ApplyPoint(eventPtr->value[0], eventPtr->value[1]);
}
const TEventManagerData* CHARACTER_MANAGER::CheckEventIsActive(BYTE eventIndex, BYTE empireIndex)
{
	const time_t cur_Time = time(NULL);
	const struct tm vKey = *localtime(&cur_Time);

	for(auto it = m_eventData.begin();it!=m_eventData.end();++it)
	{
		const auto& dayIndex = it->first;
		const auto& dayVector = it->second;
		for (DWORD j=0;j<dayVector.size();++j)
		{
			const auto & eventData = dayVector[j];
			if (eventData.eventIndex == eventIndex)
			{
				if (eventData.channelFlag != 0)
					if (eventData.channelFlag != g_bChannel)
						continue;
				if (eventData.empireFlag != 0 && empireIndex != 0)
					if (eventData.empireFlag != empireIndex)
						continue;

				if(eventData.eventStatus == true)
					return &eventData;
				//if (cur_Time >= eventData.startTime && cur_Time <= eventData.endTime)
				//	return &eventData;
			}
		}
	}
	return NULL;
}
void CHARACTER_MANAGER::CheckEventForDrop(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<LPITEM>& vec_item)
{
	if (!pkChr)
	{
		return;
	}

	uint32_t mapIdx = pkChr->GetMapIndex();
	static uint32_t idxNotPvmMaps[] = { 1, 21, 41 };
	if (std::count(std::begin(idxNotPvmMaps), std::end(idxNotPvmMaps), mapIdx) > 0)
	{
		return;
	}

	const BYTE killerEmpire = pkKiller->GetEmpire();
	const TEventManagerData* eventPtr = NULL;
	LPITEM rewardItem = NULL;

	if (pkChr->IsStone())
	{
		eventPtr = CheckEventIsActive(DOUBLE_METIN_LOOT_EVENT, killerEmpire);
		if (eventPtr)
		{
			const int prob = number(1, 100);
			const int success_prob = eventPtr->value[3];
			if (prob < success_prob)
			{
				std::vector<LPITEM> m_cache;
				for (DWORD j=0;j<vec_item.size();++j)
				{
					const auto vItem = vec_item[j];
					rewardItem = ITEM_MANAGER::Instance().CreateItem(vItem->GetVnum(), vItem->GetCount(), 0, true);
					if (rewardItem) m_cache.emplace_back(rewardItem);
				}
				for (DWORD j=0;j<m_cache.size();++j)
					vec_item.emplace_back(m_cache[j]);
			}
		}
	}
	else if (pkChr->GetMobRank() >= MOB_RANK_BOSS)
	{
		eventPtr = CheckEventIsActive(DOUBLE_BOSS_LOOT_EVENT, killerEmpire);
		if (eventPtr)
		{
			const int prob = number(1, 100);
			const int success_prob = eventPtr->value[3];
			if (prob < success_prob)
			{
				std::vector<LPITEM> m_cache;
				for (DWORD j=0;j<vec_item.size();++j)
				{
					const auto vItem = vec_item[j];
					rewardItem = ITEM_MANAGER::Instance().CreateItem(vItem->GetVnum(), vItem->GetCount(), 0, true);
					if (rewardItem) m_cache.emplace_back(rewardItem);
				}
				for (DWORD j=0;j<m_cache.size();++j)
					vec_item.emplace_back(m_cache[j]);
			}
		}
	}

	eventPtr = CheckEventIsActive(DOUBLE_MISSION_BOOK_EVENT, killerEmpire);
	if (eventPtr)
	{
		const int prob = number(1, 100);
		const int success_prob = eventPtr->value[3];
		if (prob < success_prob)
		{
			// If you have different book index put here!
			constexpr DWORD m_lbookItems[] = { 50300, 50301, 50302 };
			std::vector<LPITEM> m_cache;
			for (DWORD j=0;j<vec_item.size();++j)
			{
				const auto vItem = vec_item[j];

				const DWORD itemVnum = vItem->GetVnum();
				for (DWORD x=0;x<_countof(m_lbookItems);++x)
				{
					if (m_lbookItems[x] == itemVnum)
					{
						rewardItem = ITEM_MANAGER::Instance().CreateItem(itemVnum, vItem->GetCount(), 0, true);
						if (rewardItem) m_cache.emplace_back(rewardItem);
						break;
					}
				}
				for (DWORD j=0;j<m_cache.size();++j)
					vec_item.emplace_back(m_cache[j]);
			}
		}
	}

	eventPtr = CheckEventIsActive(DUNGEON_TICKET_LOOT_EVENT, killerEmpire);
	if (eventPtr)
	{
		const int prob = number(1, 100);
		const int success_prob = eventPtr->value[3];
		if (prob < success_prob)
		{
			// If you have different book index put here!
			constexpr DWORD m_lticketItems[] = { 71201 };
			std::vector<LPITEM> m_cache;
			for (DWORD j=0;j<vec_item.size();++j)
			{
				const auto vItem = vec_item[j];
				const DWORD itemVnum = vItem->GetVnum();
				for (DWORD x=0;x<_countof(m_lticketItems);++x)
				{
					if (m_lticketItems[x] == itemVnum)
					{
						rewardItem = ITEM_MANAGER::Instance().CreateItem(itemVnum, vItem->GetCount(), 0, true);
						if (rewardItem) m_cache.emplace_back(rewardItem);
						break;
					}
				}
				for (DWORD j=0;j<m_cache.size();++j)
					vec_item.emplace_back(m_cache[j]);
			}
		}
	}
	eventPtr = CheckEventIsActive(MOONLIGHT_EVENT, killerEmpire);
	if (eventPtr)
	{
		const int prob = number(1, 100);
		const int success_prob = eventPtr->value[3];
		if (prob < success_prob)
		{
			// If your moonlight item vnum is different change 50011!
			LPITEM item = ITEM_MANAGER::Instance().CreateItem(50011, 1, 0, true);
			if (item) vec_item.emplace_back(item);
		}
	}

#if defined(ENABLE_LETTERS_EVENT)
	eventPtr = CheckEventIsActive(LETTERS_EVENT, killerEmpire);
	if (eventPtr)
	{
		const uint8_t prob = number(1, 100);
		const int32_t success_prob = eventPtr->value[3];
		if (prob < success_prob)
		{
			const uint32_t vnum = FIRST_LETTER_VNUM + number(0, MAX_RANGE_LETTERS);
			LPITEM item = ITEM_MANAGER::Instance().CreateItem(vnum, 1, 0, true);
			if (item) {
				vec_item.emplace_back(item);
			}
		}
	}
#endif

#if defined(ENABLE_HALLOWEEN_EVENT_2022)
	eventPtr = CheckEventIsActive(HALLOWEEN_EVENT, killerEmpire);
	if (eventPtr)
	{
		const uint8_t prob = number(1, 100);

		if (prob < (int32_t)(eventPtr->value[2]))
		{
			LPITEM item = ITEM_MANAGER::Instance().CreateItem(HALLOWEEN_COMMON_DROP_VNUM, 1, 0, true);
			if (item)
			{
				vec_item.emplace_back(item);
			}
		}
	}
#endif

#if defined(ENABLE_OKEY_CARD_GAME)
	eventPtr = CheckEventIsActive(OKEY_CARD_EVENT, killerEmpire);
	if (eventPtr)
	{
		if (number(1, 100) < eventPtr->value[3])
		{
			LPITEM item = ITEM_MANAGER::Instance().CreateItem(OKEY_CARD_ITEMVNUM, 1, 0, true);
			if (item)
			{
				vec_item.emplace_back(item);
			}
		}
	}
#endif
}

bool CHARACTER_MANAGER::CompareEventSendData(TEMP_BUFFER* buf)
{
	const uint8_t dayCount = m_eventData.size();
	const uint8_t subIndex = EVENT_MANAGER_LOAD;
	const int32_t cur_Time = time(NULL);

	TPacketGCEventManager p;
	p.header = HEADER_GC_EVENT_MANAGER;
	p.size = sizeof(TPacketGCEventManager) + 
			sizeof(uint8_t)+
			sizeof(uint8_t)+
			sizeof(int32_t);

	for (auto it = m_eventData.begin();it != m_eventData.end();++it)
	{
		const auto& dayIndex = it->first;
		const auto& dayData = it->second;
		const uint8_t dayEventCount = dayData.size();
		p.size += sizeof(uint8_t) + sizeof(uint8_t) + (dayEventCount * sizeof(TEventManagerData));
	}

	buf->write(&p, sizeof(TPacketGCEventManager));
	buf->write(&subIndex, sizeof(uint8_t));
	buf->write(&dayCount, sizeof(uint8_t));
	buf->write(&cur_Time, sizeof(int32_t));

	for(auto it = m_eventData.begin();it != m_eventData.end();++it)
	{
		const auto& dayIndex = it->first;
		const auto& dayData = it->second;
		const uint8_t dayEventCount = dayData.size();
		buf->write(&dayIndex, sizeof(uint8_t));
		buf->write(&dayEventCount, sizeof(uint8_t));
		if (dayEventCount > 0) {
			buf->write(dayData.data(), dayEventCount * sizeof(TEventManagerData));
		}
	}

	return true;
}

void CHARACTER_MANAGER::UpdateAllPlayerEventData()
{
	TEMP_BUFFER buf;
	if (CompareEventSendData(&buf)) {
		const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
		for (auto it = c_ref_set.begin(); it != c_ref_set.end(); ++it)
		{
			LPCHARACTER ch = (*it)->GetCharacter();
			if (!ch)
				continue;
			(*it)->Packet(buf.read_peek(), buf.size());
		}
	}
}

void CHARACTER_MANAGER::SendDataPlayer(LPCHARACTER ch)
{
	const LPDESC d = ch->GetDesc();
	if (!d) {
		return;
	}

	TEMP_BUFFER buf;
	if (CompareEventSendData(&buf)) {
		d->Packet(buf.read_peek(), buf.size());
	}
}
bool CHARACTER_MANAGER::CloseEventManuel(BYTE eventIndex)
{
	auto eventPtr = CheckEventIsActive(eventIndex);
	if (eventPtr != NULL)
	{
		const BYTE subHeader = EVENT_MANAGER_REMOVE_EVENT;
		db_clientdesc->DBPacketHeader(HEADER_GD_EVENT_MANAGER, 0, sizeof(BYTE)+sizeof(WORD));
		db_clientdesc->Packet(&subHeader, sizeof(BYTE));
		db_clientdesc->Packet(&eventPtr->eventID, sizeof(WORD));
		return true;
	}
	return false;
}
void CHARACTER_MANAGER::SetEventStatus(const WORD eventID, const bool eventStatus, const int endTime)
{
	//eventStatus - 0-deactive  // 1-active

	TEventManagerData* eventData = NULL;
	for (auto it = m_eventData.begin(); it != m_eventData.end(); ++it)
	{
		if (it->second.size())
		{
			for (DWORD j = 0; j < it->second.size(); ++j)
			{
				TEventManagerData& pData = it->second[j];
				if (pData.eventID == eventID)
				{
					eventData = &pData;
					break;
				}
			}
		}
	}
	if (eventData == NULL)
		return;
	eventData->eventStatus = eventStatus;
	eventData->endTime = endTime;

	// Auto open&close notice
	const std::map<BYTE, std::pair<DWORD, DWORD>> m_eventText = {
		//Here
		{BONUS_EVENT,{1446,2052}},
		{DOUBLE_BOSS_LOOT_EVENT,{1448,1449}},
		{DOUBLE_METIN_LOOT_EVENT,{1450,1451}},
		{DOUBLE_MISSION_BOOK_EVENT,{1452,1453}},
		{DUNGEON_COOLDOWN_EVENT,{1454,1455}},
		{DUNGEON_TICKET_LOOT_EVENT,{1456,1457}},
		{MOONLIGHT_EVENT,{1458,1459}},
#if defined(ENABLE_NEW_FISH_EVENT)
		{JIGSAW_EVENT, {1426, 1427}},
#endif
#if defined(ENABLE_LETTERS_EVENT)
		{LETTERS_EVENT, {1438, 1439}},
#endif
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
		{HALLOWEEN_EVENT, {1553, 1554}},
#endif
#if defined(ENABLE_OKEY_CARD_GAME)
		{OKEY_CARD_EVENT, {1613, 1614}},
#endif
	};

#if defined(ENABLE_NEW_FISH_EVENT)
	bool bIsNewFishEvent = false;
#endif
#if defined(ENABLE_LETTERS_EVENT)
	bool bIsLettersEvent = false;
#endif
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
	bool bIsHalloweenEvent = false;
#endif
#if defined(ENABLE_OKEY_CARD_GAME)
	bool bIsOkeyGame = false;
#endif

	const auto it = m_eventText.find(eventData->eventIndex);
	if (it != m_eventText.end())
	{
		if (eventStatus)
		{
			//SendNotice(it->second.first.c_str());
#if defined(ENABLE_NEW_FISH_EVENT)
			if (it->first == JIGSAW_EVENT) {
				bIsNewFishEvent = true;
			}
#endif

#if defined(ENABLE_LETTERS_EVENT)
			if (it->first == LETTERS_EVENT) {
				bIsLettersEvent = true;
			}
#endif

#if defined(ENABLE_HALLOWEEN_EVENT_2022)
			if (it->first == HALLOWEEN_EVENT) {
				bIsHalloweenEvent = true;
			}
#endif

#if defined(ENABLE_OKEY_CARD_GAME)
			if (it->first == OKEY_CARD_EVENT) {
				bIsOkeyGame = true;
			}
#endif

			SendNoticeNew(CHAT_TYPE_NOTICE, 0, 0, it->second.first, "");
		}
		else
		{
			//SendNotice(it->second.second.c_str());
#if defined(ENABLE_NEW_FISH_EVENT)
			if (it->first == JIGSAW_EVENT) {
				bIsNewFishEvent = true;
			}
#endif

#if defined(ENABLE_LETTERS_EVENT)
			if (it->first == LETTERS_EVENT) {
				bIsLettersEvent = true;
			}
#endif

#if defined(ENABLE_HALLOWEEN_EVENT_2022)
			if (it->first == HALLOWEEN_EVENT) {
				bIsHalloweenEvent = true;
			}
#endif

#if defined(ENABLE_OKEY_CARD_GAME)
			if (it->first == OKEY_CARD_EVENT) {
				bIsOkeyGame = true;
			}
#endif
			SendNoticeNew(CHAT_TYPE_NOTICE, 0, 0, it->second.second, "");
		}
	}

	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();

	// Bonus event update status
	if (eventData->eventIndex == BONUS_EVENT)
	{
		for (auto itDesc = c_ref_set.begin(); itDesc != c_ref_set.end(); ++itDesc)
		{
			LPCHARACTER ch = (*itDesc)->GetCharacter();
			if (!ch)
				continue;
			if (eventData->empireFlag != 0)
				if (eventData->empireFlag != ch->GetEmpire())
					continue;
			if (eventData->channelFlag != 0)
				if (eventData->channelFlag != g_bChannel)
					return;
			if (!eventStatus)
			{
				const long value = eventData->value[1];
				ch->ApplyPoint(eventData->value[0], -value);
			}
			ch->ComputePoints();
		}
	}

	const int now = time(0);
	const BYTE subIndex = EVENT_MANAGER_EVENT_STATUS;
	
	TPacketGCEventManager p;
	p.header = HEADER_GC_EVENT_MANAGER;
	p.size = sizeof(TPacketGCEventManager)+sizeof(BYTE)+sizeof(WORD)+sizeof(bool)+sizeof(int)+sizeof(int);

	TEMP_BUFFER buf;
	buf.write(&p, sizeof(TPacketGCEventManager));
	buf.write(&subIndex, sizeof(BYTE));
	buf.write(&eventData->eventID, sizeof(WORD));
	buf.write(&eventData->eventStatus, sizeof(bool));
	buf.write(&eventData->endTime, sizeof(int));
	buf.write(&now, sizeof(int));

	for (auto it = c_ref_set.begin(); it != c_ref_set.end(); ++it)
	{
		LPCHARACTER ch = (*it)->GetCharacter();
		if (!ch)
			continue;

		(*it)->Packet(buf.read_peek(), buf.size());
#if defined(ENABLE_NEW_FISH_EVENT)
		if (bIsNewFishEvent == true) {
			ch->FishEventGeneralInfo();
		}
#endif

#if defined(ENABLE_LETTERS_EVENT)
		if (bIsLettersEvent == true) {
			ch->SendLettersEventInfo();
		}
#endif

#if defined(ENABLE_HALLOWEEN_EVENT_2022)
		if (bIsHalloweenEvent == true) {
			ch->SendHalloweenEventInfo();
		}
#endif

#if defined(ENABLE_OKEY_CARD_GAME)
		if (bIsOkeyGame == true) {
			ch->SendOkeyGameEventInfo();
		}
#endif
	}
}

void CHARACTER_MANAGER::SetEventData(BYTE dayIndex, const std::vector<TEventManagerData>& m_data)
{
	const auto it = m_eventData.find(dayIndex);
	if (it == m_eventData.end())
		m_eventData.emplace(dayIndex, m_data);
	else
	{
		it->second.clear();
		for (DWORD j=0;j<m_data.size();++j)
			it->second.emplace_back(m_data[j]);
	}
}
#endif

#ifdef ENABLE_REWARD_SYSTEM
const char* GetRewardIndexToString(BYTE rewardIndex)
{
	std::map<BYTE, std::string> rewardNames = {
		{REWARD_105,"REWARD_105"},
		{REWARD_PET_115,"REWARD_PET_115"},
		{REWARD_120,"REWARD_120"},
		{REWARD_LEGENDARY_SKILL,"REWARD_LEGENDARY_SKILL"},
		{REWARD_LEGENDARY_SKILL_SET,"REWARD_LEGENDARY_SKILL_SET"},
		{REWARD_KILL_DRAGON,"REWARD_KILL_DRAGON"},
		{REWARD_KILL_RAZADOR,"REWARD_KILL_RAZADOR"},
		{REWARD_KILL_NEMERE,"REWARD_KILL_NEMERE"},
		{REWARD_OFFLINESHOP_SLOT,"REWARD_OFFLINESHOP_SLOT"},
		{REWARD_INVENTORY_SLOT,"REWARD_INVENTORY_SLOT"},
		{REWARD_AVERAGE,"REWARD_AVERAGE"},
		{REWARD_ELEMENT,"REWARD_ELEMENT"},
		{REWARD_BATTLEPASS,"REWARD_BATTLEPASS"},
		{REWARD_CUSTOM_SASH,"REWARD_CUSTOM_SASH"},
		{REWARD_AURA,"REWARD_AURA"},
		{REWARD_ENERGY,"REWARD_ENERGY"},
		{REWARD_112_BIO,"REWARD_112_BIO"},
		{REWARD_120_BIO,"REWARD_120_BIO"},
		{REWARD_LEADER_SHIP,"REWARD_LEADER_SHIP"},
		{REWARD_BUFFI_LEGENDARY_SKILL,"REWARD_BUFFI_LEGENDARY_SKILL"},
	};
	auto it = rewardNames.find(rewardIndex);
	if (it != rewardNames.end())
		return it->second.c_str();
	return 0;
}
BYTE GetRewardIndex(const char* szRewardName)
{
	std::map<std::string, BYTE> rewardNames = {
		{"REWARD_105",REWARD_105},
		{"REWARD_PET_115",REWARD_PET_115},
		{"REWARD_120",REWARD_120},
		{"REWARD_LEGENDARY_SKILL",REWARD_LEGENDARY_SKILL},
		{"REWARD_LEGENDARY_SKILL_SET",REWARD_LEGENDARY_SKILL_SET},
		{"REWARD_KILL_DRAGON",REWARD_KILL_DRAGON},
		{"REWARD_KILL_RAZADOR",REWARD_KILL_RAZADOR},
		{"REWARD_KILL_NEMERE",REWARD_KILL_NEMERE},
		{"REWARD_OFFLINESHOP_SLOT",REWARD_OFFLINESHOP_SLOT},
		{"REWARD_INVENTORY_SLOT",REWARD_INVENTORY_SLOT},
		{"REWARD_AVERAGE",REWARD_AVERAGE},
		{"REWARD_ELEMENT",REWARD_ELEMENT},
		{"REWARD_BATTLEPASS",REWARD_BATTLEPASS},
		{"REWARD_CUSTOM_SASH",REWARD_CUSTOM_SASH},
		{"REWARD_AURA",REWARD_AURA},
		{"REWARD_ENERGY",REWARD_ENERGY},
		{"REWARD_112_BIO",REWARD_112_BIO},
		{"REWARD_120_BIO",REWARD_120_BIO},
		{"REWARD_LEADER_SHIP",REWARD_LEADER_SHIP},
		{"REWARD_BUFFI_LEGENDARY_SKILL",REWARD_BUFFI_LEGENDARY_SKILL},
	};
	auto it = rewardNames.find(szRewardName);
	if (it != rewardNames.end())
		return it->second;
	return 0;
}
void CHARACTER_MANAGER::LoadRewardData()
{
	m_rewardData.clear();

	char szQuery[QUERY_MAX_LEN];
	snprintf(szQuery, sizeof(szQuery), "SELECT rewardIndex, lc_text, playerName, itemVnum0, itemCount0, itemVnum1, itemCount1, itemVnum2, itemCount2 FROM player.reward_table");
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));

	if (pMsg->Get()->uiNumRows != 0)
	{
		MYSQL_ROW row = NULL;
		for (int n = 0; (row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != NULL; ++n)
		{
			int col = 0;
			TRewardInfo p;
			p.m_rewardItems.clear();

			char rewardName[50];
			DWORD rewardIndex;
			strlcpy(rewardName, row[col++], sizeof(rewardName));
			rewardIndex = GetRewardIndex(rewardName);

			strlcpy(p.lc_text, row[col++], sizeof(p.lc_text));
			strlcpy(p.playerName, row[col++], sizeof(p.playerName));
			for (BYTE j = 0; j < 3; ++j)
			{
				DWORD itemVnum, itemCount;
				str_to_number(itemVnum, row[col++]);
				str_to_number(itemCount, row[col++]);
				p.m_rewardItems.emplace_back(itemVnum, itemCount);
			}
			m_rewardData.emplace(rewardIndex, p);
		}
	}
}
bool CHARACTER_MANAGER::IsRewardEmpty(BYTE rewardIndex)
{
	auto it = m_rewardData.find(rewardIndex);
	if (it != m_rewardData.end())
	{
		if (strcmp(it->second.playerName, "") == 0)
			return true;
	}
	return false;
}
void CHARACTER_MANAGER::SendRewardInfo(LPCHARACTER ch)
{
	ch->SetProtectTime("RewardInfo", 1);
	std::string cmd="";//12
	if (m_rewardData.size())
	{
		for (auto it = m_rewardData.begin(); it != m_rewardData.end(); ++it)
		{
			if (strlen(it->second.playerName) > 1)
			{
				char text[60];
				snprintf(text, sizeof(text), "%d|%s#", it->first, it->second.playerName);
				cmd += text;
			}
			if (strlen(cmd.c_str()) + 12 > CHAT_MAX_LEN - 30)
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, "RewardInfo %s", cmd.c_str());
				cmd = "";
			}
		}
		if (strlen(cmd.c_str()) > 1)
			ch->ChatPacket(CHAT_TYPE_COMMAND, "RewardInfo %s", cmd.c_str());
	}
}

struct RewardForEach
{
	void operator() (LPDESC d)
	{
		LPCHARACTER ch = d->GetCharacter();
		if (ch == NULL)
			return;
		else if (ch->GetProtectTime("RewardInfo") != 1)
			return;
		CHARACTER_MANAGER::Instance().SendRewardInfo(ch);
	}
};
void CHARACTER_MANAGER::SetRewardData(BYTE rewardIndex, const char* playerName, bool isP2P)
{
	if (!IsRewardEmpty(rewardIndex))
		return;

	auto it = m_rewardData.find(rewardIndex);
	if (it == m_rewardData.end())
		return;
	TRewardInfo& rewardInfo = it->second;
	strlcpy(rewardInfo.playerName, playerName, sizeof(rewardInfo.playerName));

	if (isP2P)
	{
		LPCHARACTER ch = FindPC(playerName);
		if (ch)
		{
			for (DWORD j = 0; j < rewardInfo.m_rewardItems.size(); ++j)
				ch->AutoGiveItem(rewardInfo.m_rewardItems[j].first, rewardInfo.m_rewardItems[j].second);
		}

		DWORD lc_number;
		str_to_number(lc_number, it->second.lc_text);
		BroadcastNoticeNew(CHAT_TYPE_NOTICE, 0, 0, lc_number, "%s", playerName);

		//char msg[CHAT_MAX_LEN+1];
		//snprintf(msg, sizeof(msg), LC_TEXT(), playerName);
		//snprintf(msg, sizeof(msg), rewardInfo.lc_text, playerName);
		//BroadcastNotice(msg);

		TPacketGGRewardInfo p;
		p.bHeader = HEADER_GG_REWARD_INFO;
		p.rewardIndex = rewardIndex;
		strlcpy(p.playerName, playerName, sizeof(p.playerName));
		P2P_MANAGER::Instance().Send(&p, sizeof(p));

		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery), "UPDATE player.reward_table SET playerName = '%s' WHERE lc_text = '%s'", playerName, rewardInfo.lc_text);
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	}
	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), RewardForEach());
}
#endif

#if defined(ENABLE_INGAME_ITEMSHOP)
void CHARACTER_MANAGER::LoadItemShopLogReal(LPCHARACTER ch, const char* c_pData)
{
	if (!ch)
		return;

	BYTE subIndex = ITEMSHOP_LOG;

	const int logCount = *(int*)c_pData;
	c_pData += sizeof(int);
	std::vector<TIShopLogData> m_vec;
	if (logCount)
	{
		for (DWORD j = 0; j < (uint32_t)(logCount); ++j)
		{
			const TIShopLogData logData = *(TIShopLogData*)c_pData;
			m_vec.emplace_back(logData);
			c_pData += sizeof(TIShopLogData);
		}
	}

	TPacketGCItemShop p;
	p.header = HEADER_GC_ITEMSHOP;
	p.size = sizeof(TPacketGCItemShop) + sizeof(BYTE) + sizeof(int) + (sizeof(TIShopLogData) * logCount);

	TEMP_BUFFER buf;
	buf.write(&p, sizeof(TPacketGCItemShop));
	buf.write(&subIndex, sizeof(BYTE));
	buf.write(&logCount, sizeof(int));
	if(logCount)
		buf.write(m_vec.data(), sizeof(TIShopLogData)* logCount);
	
	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}
void CHARACTER_MANAGER::LoadItemShopLog(LPCHARACTER ch)
{
	BYTE subIndex = ITEMSHOP_LOG;
	DWORD accountID = ch->GetDesc()->GetAccountTable().id;

	db_clientdesc->DBPacketHeader(HEADER_GD_ITEMSHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE)+sizeof(DWORD));
	db_clientdesc->Packet(&subIndex, sizeof(BYTE));
	db_clientdesc->Packet(&accountID, sizeof(DWORD));
}
void CHARACTER_MANAGER::LoadItemShopData(LPCHARACTER ch, bool isAll)
{
	TEMP_BUFFER buf;
	TPacketGCItemShop p;
	p.header = HEADER_GC_ITEMSHOP;
	
	long long dragonCoin = ch->GetDragonCoin();

	if (isAll)
	{

		int calculateSize = 0;
		BYTE subIndex = ITEMSHOP_LOAD;
		calculateSize += sizeof(BYTE);

		calculateSize += sizeof(long long);//dragon coin
		calculateSize += sizeof(int);//updatetime

		int categoryTotalSize = m_IShopManager.size();
		calculateSize += sizeof(int);

		if (m_IShopManager.size())
		{
			for (auto it = m_IShopManager.begin(); it != m_IShopManager.end(); ++it)
			{
				BYTE categoryIndex = it->first;
				calculateSize += sizeof(BYTE);
				BYTE categorySize = it->second.size();
				calculateSize += sizeof(BYTE);

				if (it->second.size())
				{
					for (auto itEx = it->second.begin(); itEx != it->second.end(); ++itEx)
					{
						BYTE categorySubIndex = itEx->first;
						calculateSize += sizeof(BYTE);
						BYTE categorySubSize = itEx->second.size();
						calculateSize += sizeof(BYTE);
						if (categorySubSize)
							calculateSize += sizeof(TIShopData) * categorySubSize;
					}
				}
			}
		}

		
		p.size = sizeof(TPacketGCItemShop) + calculateSize;

		
		buf.write(&p, sizeof(TPacketGCItemShop));
		buf.write(&subIndex, sizeof(BYTE));
		buf.write(&dragonCoin, sizeof(long long));
		buf.write(&itemshopUpdateTime, sizeof(int));
		buf.write(&categoryTotalSize, sizeof(int));

		if (m_IShopManager.size())
		{
			for (auto it = m_IShopManager.begin(); it != m_IShopManager.end(); ++it)
			{
				BYTE categoryIndex = it->first;
				buf.write(&categoryIndex, sizeof(BYTE));
				BYTE categorySize = it->second.size();
				buf.write(&categorySize, sizeof(BYTE));
				if (it->second.size())
				{
					for (auto itEx = it->second.begin(); itEx != it->second.end(); ++itEx)
					{
						BYTE categorySubIndex = itEx->first;
						buf.write(&categorySubIndex, sizeof(BYTE));
						BYTE categorySubSize = itEx->second.size();
						buf.write(&categorySubSize, sizeof(BYTE));
						if (categorySubSize)
							buf.write(itEx->second.data(), sizeof(TIShopData) * categorySubSize);
					}
				}
			}
		}
	}
	else
	{
		p.size = sizeof(TPacketGCItemShop) + sizeof(BYTE)+sizeof(int)+sizeof(int);
		buf.write(&p, sizeof(TPacketGCItemShop));
		BYTE subIndex = ITEMSHOP_LOAD;
		buf.write(&subIndex, sizeof(BYTE));
		buf.write(&dragonCoin, sizeof(long long));
		buf.write(&itemshopUpdateTime, sizeof(int));
		int categoryTotalSize = 9999;
		buf.write(&categoryTotalSize, sizeof(int));
	}
	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}
void CHARACTER_MANAGER::LoadItemShopBuyReal(LPCHARACTER ch, const char* c_pData)
{
	if (!ch)
		return;
	
	const BYTE returnType = *(BYTE*)c_pData;
	c_pData += sizeof(BYTE);

	if (returnType == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Not_Enough_Dragon_Coins");
		return;
	}
	else if (returnType == 1)
	{
		const int weekMaxCount = *(int*)c_pData;
		c_pData += sizeof(int);
		ch->ChatPacket(CHAT_TYPE_INFO, "Weekly Purchase limit %d exceeded.", weekMaxCount);
		return;
	}
	else if (returnType == 2)
	{
		const int monthMaxCount = *(int*)c_pData;
		c_pData += sizeof(int);
		ch->ChatPacket(CHAT_TYPE_INFO, "Monthly Purchase limit %d exceeded.", monthMaxCount);
		return;
	}

	const bool isOpenLog = *(bool*)c_pData;
	c_pData += sizeof(bool);

	const DWORD itemVnum = *(DWORD*)c_pData;
	c_pData += sizeof(DWORD);

	const int itemCount = *(int*)c_pData;
	c_pData += sizeof(int);

	const long long itemPrice = *(long long*)c_pData;
	c_pData += sizeof(long long);
	
	long long dragonCoin = ch->GetDragonCoin(true);
	if (dragonCoin < itemPrice)
		return;

	LPITEM item = ITEM_MANAGER::instance().CreateItem(50001, itemCount, 0, true);
	if (item) {
		item->SetSocket(0, itemVnum);

		int iEmptyPos = ch->GetEmptyInventory(item->GetSize());
		if (iEmptyPos != -1) {
			item->SetSocket(0, itemVnum);
			ch->SetDragonCoinMinus((int)itemPrice);
			item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			LogManager::instance().ItemLog(ch, item, "ITEMSHOP_BUY", item->GetName());
		} else {
			M2_DESTROY_ITEM(item);
			return;
		}
	} else {
		return;
	}

	TEMP_BUFFER buf;
	TPacketGCItemShop p;
	p.header = HEADER_GC_ITEMSHOP;
	p.size = sizeof(TPacketGCItemShop) + sizeof(BYTE) + sizeof(long long) + sizeof(bool);

	if(isOpenLog)
		p.size += sizeof(TIShopLogData);

	BYTE subIndex = ITEMSHOP_DRAGONCOIN;
	dragonCoin -= itemPrice;

	buf.write(&p, sizeof(TPacketGCItemShop));
	buf.write(&subIndex, sizeof(BYTE));
	buf.write(&dragonCoin, sizeof(long long));
	buf.write(&isOpenLog, sizeof(bool));
	if (isOpenLog)
	{
		const TIShopLogData logData = *(TIShopLogData*)c_pData;
		c_pData += sizeof(TIShopLogData);

		buf.write(&logData, sizeof(TIShopLogData));
	}
	ch->GetDesc()->Packet(buf.read_peek(), buf.size());

	ch->ChatPacket(CHAT_TYPE_INFO, "You bought %dx item from Itemshop for %lld coins.", itemCount, itemPrice);
}
void CHARACTER_MANAGER::LoadItemShopBuy(LPCHARACTER ch, int itemID, int itemCount)
{
	if (itemCount <= 0) {
		return;
	}

	if ((const uint32_t)(itemCount) > 20)
		return;

	if (m_IShopManager.size())
	{
		for (auto it = m_IShopManager.begin(); it != m_IShopManager.end(); ++it)
		{
			if (it->second.size())
			{
				for (auto itEx = it->second.begin(); itEx != it->second.end(); ++itEx)
				{
					if (itEx->second.size())
					{
						for (auto itReal = itEx->second.begin(); itReal != itEx->second.end(); ++itReal)
						{
							const TIShopData& itemData = *itReal;
							if (itemData.id == (const uint32_t)(itemID))
							{
								LPITEM buyItem = ITEM_MANAGER::instance().CreateItem(50001, itemCount, 0, true);
								if (!buyItem)
									return;
								
								buyItem->SetSocket(0, itemData.itemVnum);

								int iEmptyPos = ch->GetEmptyInventory(buyItem->GetSize());
								if (iEmptyPos != -1)
								{
									M2_DESTROY_ITEM(buyItem);
								}
								else
								{
									M2_DESTROY_ITEM(buyItem);
									ch->ChatPacket(CHAT_TYPE_INFO, "Unfortunately you don't have enough space in your inventory.");
									return;
								}

								DWORD dragonCoin = ch->GetDragonCoin();
								long long itemPrice = itemData.itemPrice*itemCount;
								if (itemData.discount > 0)
									itemPrice = long((float(itemData.itemPrice) / 100.0) * float(100 - itemData.discount));

								if (itemPrice > dragonCoin)
								{
									ch->ChatPacket(CHAT_TYPE_INFO, "Not_Enough_Dragon_Coins");
									return;
								}

								DWORD accountID = ch->GetDesc()->GetAccountTable().id;
								BYTE subIndex = ITEMSHOP_BUY;
								char playerName[CHARACTER_NAME_MAX_LEN + 1];
								strlcpy(playerName, ch->GetName(), sizeof(playerName));

								char ipAdress[16];
								strlcpy(ipAdress, ch->GetDesc()->GetHostName(), sizeof(ipAdress));

								TEMP_BUFFER buf;
								buf.write(&subIndex, sizeof(BYTE));
								buf.write(&accountID, sizeof(DWORD));
								buf.write(&playerName, sizeof(playerName));
								buf.write(&ipAdress, sizeof(ipAdress));
								buf.write(&itemID, sizeof(int));
								buf.write(&itemCount, sizeof(int));
								bool isLogOpen = (ch->GetProtectTime("itemshop.log") == 1)?true:false;
								buf.write(&isLogOpen, sizeof(bool));

								db_clientdesc->DBPacketHeader(HEADER_GD_ITEMSHOP, ch->GetDesc()->GetHandle(), buf.size());
								db_clientdesc->Packet(buf.read_peek(), buf.size());

								return;
							}
						}
					}
				}
			}
		}
	}


}
void RefreshItemShop(LPDESC d)
{
	LPCHARACTER ch = d->GetCharacter();
	if (!ch)
		return;
	if (ch->GetProtectTime("itemshop.load") == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Itemshop was updated.");
		CHARACTER_MANAGER::Instance().LoadItemShopData(ch, true);
	}
}

void CHARACTER_MANAGER::LoadItemShopData(const char* c_pData)
{
	m_IShopManager.clear();

	const int updateTime = *(int*)c_pData;
	c_pData += sizeof(int);

	const bool isManuelUpdate = *(bool*)c_pData;
	c_pData += sizeof(bool);

	const int categoryTotalSize = *(int*)c_pData;
	c_pData += sizeof(int);

	itemshopUpdateTime = updateTime;

	for (uint32_t j = 0; j < (uint32_t)(categoryTotalSize); ++j)
	{
		const BYTE categoryIndex = *(BYTE*)c_pData;
		c_pData += sizeof(BYTE);
		const BYTE categorySize = *(BYTE*)c_pData;
		c_pData += sizeof(BYTE);

		std::map<BYTE, std::vector<TIShopData>> m_map;
		m_map.clear();

		for (DWORD x = 0; x < categorySize; ++x)
		{
			const BYTE categorySubIndex = *(BYTE*)c_pData;
			c_pData += sizeof(BYTE);

			const BYTE categorySubSize = *(BYTE*)c_pData;
			c_pData += sizeof(BYTE);

			std::vector<TIShopData> m_vec;
			m_vec.clear();

			for (DWORD b = 0; b < categorySubSize; ++b)
			{
				const TIShopData itemData = *(TIShopData*)c_pData;

				m_vec.emplace_back(itemData);
				c_pData += sizeof(TIShopData);
			}

			if(m_vec.size())
				m_map.emplace(categorySubIndex, m_vec);
		}
		if(m_map.size())
			m_IShopManager.emplace(categoryIndex, m_map);
	}

	if (isManuelUpdate)
	{
		const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_ref_set.begin(), c_ref_set.end(), RefreshItemShop);
	}
}
#endif
