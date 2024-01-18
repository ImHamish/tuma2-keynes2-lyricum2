#include "StdAfx.h"
#include "PythonPlayerEventHandler.h"
#include "PythonApplication.h"
#include "PythonItem.h"
#include "../eterbase/Timer.h"

#include "AbstractPlayer.h"
#include "../gamelib/GameLibDefines.h"

#if defined(ENABLE_ULTIMATE_REGEN)
#include "../eterPack/EterPackManager.h"

void localToGlobal(long& X, long& Y) {
	CPythonBackground::Instance().LocalPositionToGlobalPosition(X, Y);
	X *= 100;
	Y *= 100;
}

void positionToGlobal(const float& eX, const float& eY, long& X, long& Y) {
	PR_FLOAT_TO_INT(eX, X);
	PR_FLOAT_TO_INT(eY, Y);
	X /= 100;
	Y /= 100;
	localToGlobal(X, Y);
}

int32_t DISTANCE_APPROX_SERVER(int32_t dx, int32_t dy) {
	int32_t min, max;
	if (dx < 0) {
		dx = -dx;
	}

	if (dy < 0) {
		dy = -dy;
	}

	if (dx < dy) {
		min = dx;
		max = dy;
	} else {
		min = dy;
		max = dx;
	}

	return (((max << 8) + (max << 3) - (max << 4) - (max << 1) +(min << 7) - (min << 5) + (min << 3) - (min << 1)) >> 8);
}

bool CPythonPlayer::CheckBossSafeRange() {
	if (m_eventData.size()) {
		auto mainInstance = NEW_GetMainActorPtr();
		if (mainInstance) {
			TPixelPosition kPPosCur;
			mainInstance->NEW_GetPixelPosition(&kPPosCur);

			long mainX, mainY;
			positionToGlobal(kPPosCur.x, kPPosCur.y, mainX, mainY);

			const time_t cur_Time = time(NULL);
			const struct tm vKey = *localtime(&cur_Time);
			int32_t myear = vKey.tm_year;
			int32_t mmon = vKey.tm_mon;
			int32_t mday = vKey.tm_mday;
			int32_t yday = vKey.tm_yday;
			int32_t day = vKey.tm_wday;
			int32_t hour = vKey.tm_hour;
			int32_t minute = vKey.tm_min;
			int32_t second = vKey.tm_sec;

			for (auto it = m_eventData.begin(); it != m_eventData.end(); ++it) {
				TNewRegen& newRegen = *it;
				if (newRegen.day != 0) {
					const int32_t day = vKey.tm_wday + 1;
					if (day != newRegen.day)
						continue;
				}

				long posGlobalX = newRegen.x, posGlobalY = newRegen.y;
				localToGlobal(posGlobalX, posGlobalY);
				int32_t fDist = DISTANCE_APPROX_SERVER(mainX - posGlobalX, mainY - posGlobalY);

				if (fDist <= newRegen.safeRange) {
					std::vector<CInstanceBase*> m_Data;
					CPythonCharacterManager::Instance().GetMobWithVnum(newRegen.mob_vnum, m_Data);
					if (m_Data.size()) {
						for (auto itBoss = m_Data.begin(); itBoss != m_Data.end(); ++itBoss) {
							CInstanceBase* bossInstance = *itBoss;
							if (bossInstance) {
								TPixelPosition kPPosBoss;
								bossInstance->NEW_GetPixelPosition(&kPPosBoss);
								long bossX, bossY;
								positionToGlobal(kPPosBoss.x, kPPosBoss.y, bossX, bossY);
								fDist = DISTANCE_APPROX_SERVER(mainX - bossX, mainY - bossY);
								if (fDist <= newRegen.safeRange) {
									return true;
								}
							}
						}
					} else {
						const int32_t nowTime = time(0);
						if (newRegen.leftTime < 0) {
							bool isSucces = false;
							const int32_t now = time(0) + 2;
							const uint8_t regenHour = newRegen.hour;
							const uint8_t regenMinute = newRegen.minute;
							const uint8_t regenSecond = newRegen.second;

							while (true) {
								++second;
								if (second == 60) {
									minute += 1;
									second = 0;

									if (minute == 60) {
										minute = 0;
										hour += 1;

										if (hour == 24) {
											hour = 0;
											day += 1;
											if (day == 7) {
												day = 0;
											}

											mday += 1;
											yday += 1;

											if (mday == 32) {
												mday = 1;
												mmon += 1;

												if (mmon == 12) {
													mmon = 0;
													myear += 1;
													yday = 0;
												}
											}
										}
									}
								}

								if (second == 0) {
									if (newRegen.hours_range) {
										if ((hour % newRegen.hours_range) == 0 && regenMinute == minute) {
											isSucces = true;
											break;
										}
									}

									if (newRegen.minute_range) {
										if ((minute % newRegen.minute_range) == 0) {
											isSucces = true;
											break;
										}
									}
								}

								if (regenHour == hour && regenMinute == minute && regenSecond == second) {
									if (newRegen.day) {
										if (newRegen.day - 1 == day) {
											isSucces = true;
											break;
										}
									} else {
										isSucces = true;
										break;
									}
								}

								if (time(0) > now) {
									break;
								}
							}

							if (isSucces) {
								std::tm tm{};
								tm.tm_year = myear;
								tm.tm_mon = mmon;
								tm.tm_mday = mday;
								tm.tm_isdst = 0;
								tm.tm_wday = day;
								tm.tm_hour = hour;
								tm.tm_min = minute;
								tm.tm_sec = second;
								tm.tm_yday = yday;
								const time_t t = mktime(&tm);
								newRegen.leftTime = t;

								if (newRegen.leftTime-nowTime >= 0 && newRegen.leftTime-nowTime <= 30) {
									return true;
								}
							} else {
								return false;
							}
						} else {
							if (newRegen.leftTime-nowTime >= 0 && newRegen.leftTime-nowTime <= 30) {
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

bool CPythonPlayer::LoadNewRegen() {
	m_eventData.clear();

	CMappedFile file;
	LPCVOID pData;

	if (!CFileSystem::Instance().Get(file, "data/maps/newregen.txt", &pData, __FUNCTION__)) {
		return false;
	}

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(file.Size(), pData);
	const std::string mapName = CPythonBackground::Instance().GetWarpMapName();
	CTokenVector TokenVector;

	for (uint32_t i = 0; i < textFileLoader.GetLineCount(); ++i) {
		if (!textFileLoader.SplitLine(i, &TokenVector, "\t")) {
			continue;
		} else if (TokenVector.size() != 16) {
			continue;
		} else if (mapName != TokenVector[2]) {
			continue;
		}

		TNewRegen data;
		data.x = atoi(TokenVector[3].c_str());
		data.y = atoi(TokenVector[4].c_str());
		data.day = atoi(TokenVector[7].c_str());
		data.hour = atoi(TokenVector[8].c_str());
		data.minute = atoi(TokenVector[9].c_str());
		data.second = atoi(TokenVector[10].c_str());
		data.safeRange = atoi(TokenVector[12].c_str());
		if (data.safeRange == 0) {
			continue;
		}

		data.hours_range = atoi(TokenVector[13].c_str());
		data.minute_range = atoi(TokenVector[14].c_str());
		data.mob_vnum = atoi(TokenVector[15].c_str());
		data.leftTime = -1;
		m_eventData.emplace_back(data);
	}

	return true;
}
#endif

const uint32_t POINT_MAGIC_NUMBER = 0xe73ac1da;

void CPythonPlayer::SPlayerStatus::SetPoint(UINT ePoint, long lPoint)
{
	m_alPoint[ePoint] = lPoint ^ POINT_MAGIC_NUMBER;
}

long CPythonPlayer::SPlayerStatus::GetPoint(UINT ePoint)
{
	return m_alPoint[ePoint] ^ POINT_MAGIC_NUMBER;
}

bool CPythonPlayer::AffectIndexToSkillIndex(DWORD dwAffectIndex, DWORD * pdwSkillIndex)
{
	if (m_kMap_dwAffectIndexToSkillIndex.end() == m_kMap_dwAffectIndexToSkillIndex.find(dwAffectIndex))
		return false;

	*pdwSkillIndex = m_kMap_dwAffectIndexToSkillIndex[dwAffectIndex];
	return true;
}

bool CPythonPlayer::AffectIndexToSkillSlotIndex(UINT uAffect, DWORD* pdwSkillSlotIndex)
{
	DWORD dwSkillIndex=m_kMap_dwAffectIndexToSkillIndex[uAffect];

	return GetSkillSlotIndex(dwSkillIndex, pdwSkillSlotIndex);
}

bool CPythonPlayer::__GetPickedActorPtr(CInstanceBase** ppkInstPicked)
{
	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase* pkInstPicked=rkChrMgr.OLD_GetPickedInstancePtr();
	if (!pkInstPicked)
		return false;

	*ppkInstPicked=pkInstPicked;
	return true;
}

bool CPythonPlayer::__GetPickedActorID(DWORD* pdwActorID)
{
	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	return rkChrMgr.OLD_GetPickedInstanceVID(pdwActorID);
}

bool CPythonPlayer::__GetPickedItemID(DWORD* pdwItemID)
{
	CPythonItem& rkItemMgr=CPythonItem::Instance();
	return rkItemMgr.GetPickedItemID(pdwItemID);
}

bool CPythonPlayer::__GetPickedGroundPos(TPixelPosition* pkPPosPicked)
{
	CPythonBackground& rkBG=CPythonBackground::Instance();

	TPixelPosition kPPosPicked;
	if (rkBG.GetPickingPoint(pkPPosPicked))
	{
		pkPPosPicked->y=-pkPPosPicked->y;
		return true;
	}

	return false;
}

void CPythonPlayer::NEW_GetMainActorPosition(TPixelPosition* pkPPosActor)
{
	TPixelPosition kPPosMainActor;

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	CInstanceBase * pInstance = rkPlayer.NEW_GetMainActorPtr();
	if (pInstance)
	{
		pInstance->NEW_GetPixelPosition(pkPPosActor);
	}
	else
	{
		CPythonApplication::Instance().GetCenterPosition(pkPPosActor);
	}
}



bool CPythonPlayer::RegisterEffect(DWORD dwEID, const char* c_szFileName, bool isCache)
{
	if (dwEID>=EFFECT_NUM)
		return false;

	CEffectManager& rkEftMgr=CEffectManager::Instance();
	rkEftMgr.RegisterEffect2(c_szFileName, &m_adwEffect[dwEID], isCache);
	return true;
}

void CPythonPlayer::NEW_ShowEffect(int dwEID, TPixelPosition kPPosDst)
{
	if (dwEID>=EFFECT_NUM)
		return;

	D3DXVECTOR3 kD3DVt3Pos(kPPosDst.x, -kPPosDst.y, kPPosDst.z);
	D3DXVECTOR3 kD3DVt3Dir(0.0f, 0.0f, 1.0f);

	CEffectManager& rkEftMgr=CEffectManager::Instance();
	rkEftMgr.CreateEffect(m_adwEffect[dwEID], kD3DVt3Pos, kD3DVt3Dir);
}

CInstanceBase* CPythonPlayer::NEW_FindActorPtr(DWORD dwVID)
{
	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	return rkChrMgr.GetInstancePtr(dwVID);
}

CInstanceBase* CPythonPlayer::NEW_GetMainActorPtr()
{
	return NEW_FindActorPtr(m_dwMainCharacterIndex);
}

///////////////////////////////////////////////////////////////////////////////////////////


void CPythonPlayer::Update()
{
	NEW_RefreshMouseWalkingDirection();

	CPythonPlayerEventHandler& rkPlayerEventHandler=CPythonPlayerEventHandler::GetSingleton();
	rkPlayerEventHandler.FlushVictimList();

	if (m_isDestPosition)
	{
		CInstanceBase * pInstance = NEW_GetMainActorPtr();
		if (pInstance)
		{
			TPixelPosition PixelPosition;
			pInstance->NEW_GetPixelPosition(&PixelPosition);

			if (abs(int(PixelPosition.x) - m_ixDestPos) + abs(int(PixelPosition.y) - m_iyDestPos) < 10000)
			{
				m_isDestPosition = FALSE;
			}
			else
			{
				if (CTimer::Instance().GetCurrentMillisecond() - m_iLastAlarmTime > 20000)
				{
					AlarmHaveToGo();
				}
			}
		}
	}

	if (m_isConsumingStamina)
	{
		float fElapsedTime = CTimer::Instance().GetElapsedSecond();
		m_fCurrentStamina -= (fElapsedTime * m_fConsumeStaminaPerSec);

		SetStatus(POINT_STAMINA, DWORD(m_fCurrentStamina));

		PyCallClassMemberFunc(m_ppyGameWindow, "RefreshStamina", Py_BuildValue("()"));
	}

#ifdef USE_AUTO_HUNT
    __AutoProcess();
#endif

	__Update_AutoAttack();
	__Update_NotifyGuildAreaEvent();
}

bool CPythonPlayer::__IsUsingChargeSkill()
{
	CInstanceBase * pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return false;

	if (__CheckDashAffect(*pkInstMain))
		return true;

	if (MODE_USE_SKILL != m_eReservedMode)
		return false;

	if (m_dwSkillSlotIndexReserved >= SKILL_MAX_NUM)
		return false;

	TSkillInstance & rkSkillInst = m_playerStatus.aSkill[m_dwSkillSlotIndexReserved];

	CPythonSkill::TSkillData * pSkillData;
	if (!CPythonSkill::Instance().GetSkillData(rkSkillInst.dwIndex, &pSkillData))
		return false;

	return pSkillData->IsChargeSkill() ? true : false;
}

void CPythonPlayer::__Update_AutoAttack()
{
	if (0 == m_dwAutoAttackTargetVID)
		return;

	CInstanceBase * pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return;

	// 탄환격 쓰고 달려가는 도중에는 스킵
	if (__IsUsingChargeSkill())
		return;

	CInstanceBase* pkInstVictim=NEW_FindActorPtr(m_dwAutoAttackTargetVID);
	if (!pkInstVictim)
	{
		__ClearAutoAttackTargetActorID();
	}
	else
	{
		if (pkInstVictim->IsDead())
		{
			__ClearAutoAttackTargetActorID();
		}
		else if (pkInstMain->IsMountingHorse() && !pkInstMain->CanAttackHorseLevel())
		{
			__ClearAutoAttackTargetActorID();
		}
		else if (pkInstMain->IsAttackableInstance(*pkInstVictim))
		{
			if (pkInstMain->IsBowMode())
			{
				if (!__CanShot(*pkInstMain, *pkInstVictim))
				{
					return;
				}
			}

			if (pkInstMain->IsSleep())
			{
				//TraceError("SKIP_AUTO_ATTACK_IN_SLEEPING");
			}
			else
			{
				__ReserveClickActor(m_dwAutoAttackTargetVID);
			}
		}
	}
}

void CPythonPlayer::__Update_NotifyGuildAreaEvent()
{
	CInstanceBase * pkInstMain = NEW_GetMainActorPtr();
	if (pkInstMain)
	{
		TPixelPosition kPixelPosition;
		pkInstMain->NEW_GetPixelPosition(&kPixelPosition);

		DWORD dwAreaID = CPythonMiniMap::Instance().GetGuildAreaID(
			ULONG(kPixelPosition.x), ULONG(kPixelPosition.y));

		if (dwAreaID != m_inGuildAreaID)
		{
			if (0xffffffff != dwAreaID)
			{
				PyCallClassMemberFunc(m_ppyGameWindow, "BINARY_Guild_EnterGuildArea", Py_BuildValue("(i)", dwAreaID));
			}
			else
			{
				PyCallClassMemberFunc(m_ppyGameWindow, "BINARY_Guild_ExitGuildArea", Py_BuildValue("(i)", dwAreaID));
			}

			m_inGuildAreaID = dwAreaID;
		}
	}
}

void CPythonPlayer::SetMainCharacterIndex(int iIndex)
{
	m_dwMainCharacterIndex = iIndex;

	CInstanceBase* pkInstMain=NEW_GetMainActorPtr();
	if (pkInstMain)
	{
		CPythonPlayerEventHandler& rkPlayerEventHandler=CPythonPlayerEventHandler::GetSingleton();
		pkInstMain->SetEventHandler(&rkPlayerEventHandler);
	}
}

DWORD CPythonPlayer::GetMainCharacterIndex()
{
	return m_dwMainCharacterIndex;
}

bool CPythonPlayer::IsMainCharacterIndex(DWORD dwIndex)
{
	return (m_dwMainCharacterIndex == dwIndex);
}

DWORD CPythonPlayer::GetGuildID()
{
	CInstanceBase* pkInstMain=NEW_GetMainActorPtr();
	if (!pkInstMain)
		return 0xffffffff;

	return pkInstMain->GetGuildID();
}

void CPythonPlayer::SetWeaponPower(DWORD dwMinPower, DWORD dwMaxPower, DWORD dwMinMagicPower, DWORD dwMaxMagicPower, DWORD dwAddPower)
{
	m_dwWeaponMinPower=dwMinPower;
	m_dwWeaponMaxPower=dwMaxPower;
	m_dwWeaponMinMagicPower=dwMinMagicPower;
	m_dwWeaponMaxMagicPower=dwMaxMagicPower;
	m_dwWeaponAddPower=dwAddPower;

	__UpdateBattleStatus();
}

void CPythonPlayer::SetRace(DWORD dwRace)
{
	m_dwRace=dwRace;
}

DWORD CPythonPlayer::GetRace()
{
	return m_dwRace;
}

DWORD CPythonPlayer::__GetRaceStat()
{
	switch (GetRace())
	{
		case MAIN_RACE_WARRIOR_M:
		case MAIN_RACE_WARRIOR_W:
			return GetStatus(POINT_ST);
			break;
		case MAIN_RACE_ASSASSIN_M:
		case MAIN_RACE_ASSASSIN_W:
			return GetStatus(POINT_DX);
			break;
		case MAIN_RACE_SURA_M:
		case MAIN_RACE_SURA_W:
			return GetStatus(POINT_ST);
			break;
		case MAIN_RACE_SHAMAN_M:
		case MAIN_RACE_SHAMAN_W:
			return GetStatus(POINT_IQ);
			break;
	}
	return GetStatus(POINT_ST);
}

DWORD CPythonPlayer::__GetLevelAtk()
{
	return 2*GetStatus(POINT_LEVEL);
}

DWORD CPythonPlayer::__GetStatAtk()
{
	return (4*GetStatus(POINT_ST)+2*__GetRaceStat())/3;
}

DWORD CPythonPlayer::__GetWeaponAtk(DWORD dwWeaponPower)
{
	return 2*dwWeaponPower;
}

DWORD CPythonPlayer::__GetTotalAtk(DWORD dwWeaponPower, DWORD dwRefineBonus)
{
	DWORD dwLvAtk=__GetLevelAtk();
	DWORD dwStAtk=__GetStatAtk();

	/////

	DWORD dwWepAtk;
	DWORD dwTotalAtk;

	if (LocaleService_IsCHEONMA())
	{
		dwWepAtk = __GetWeaponAtk(dwWeaponPower+dwRefineBonus);
		dwTotalAtk = dwLvAtk+(dwStAtk+dwWepAtk)*(GetStatus(POINT_DX)+210)/300;
	}
	else
	{
		int hr = __GetHitRate();
		dwWepAtk = __GetWeaponAtk(dwWeaponPower+dwRefineBonus);
		dwTotalAtk = dwLvAtk+(dwStAtk+dwWepAtk)*hr/100;
	}

	return dwTotalAtk;
}

DWORD CPythonPlayer::__GetHitRate()
{
	int src = 0;

	if (LocaleService_IsCHEONMA())
	{
		src = GetStatus(POINT_DX);
	}
	else
	{
		src = (GetStatus(POINT_DX) * 4 + GetStatus(POINT_LEVEL) * 2)/6;
	}

	return 100*(min(90, src)+210)/300;
}

DWORD CPythonPlayer::__GetEvadeRate()
{
	return 30*(2*GetStatus(POINT_DX)+5)/(GetStatus(POINT_DX)+95);
}

void CPythonPlayer::__UpdateBattleStatus()
{
	m_playerStatus.SetPoint(POINT_NONE, 0);
	m_playerStatus.SetPoint(POINT_EVADE_RATE, __GetEvadeRate());
	m_playerStatus.SetPoint(POINT_HIT_RATE, __GetHitRate());
	m_playerStatus.SetPoint(POINT_MIN_WEP, m_dwWeaponMinPower+m_dwWeaponAddPower);
	m_playerStatus.SetPoint(POINT_MAX_WEP, m_dwWeaponMaxPower+m_dwWeaponAddPower);
	m_playerStatus.SetPoint(POINT_MIN_MAGIC_WEP, m_dwWeaponMinMagicPower+m_dwWeaponAddPower);
	m_playerStatus.SetPoint(POINT_MAX_MAGIC_WEP, m_dwWeaponMaxMagicPower+m_dwWeaponAddPower);
	m_playerStatus.SetPoint(POINT_MIN_ATK, __GetTotalAtk(m_dwWeaponMinPower, m_dwWeaponAddPower));
	m_playerStatus.SetPoint(POINT_MAX_ATK, __GetTotalAtk(m_dwWeaponMaxPower, m_dwWeaponAddPower));
}

void CPythonPlayer::SetStatus(DWORD dwType, long lValue)
{
	if (dwType >= POINT_MAX_NUM)
	{
		assert(!" CPythonPlayer::SetStatus - Strange Status Type!");
		Tracef("CPythonPlayer::SetStatus - Set Status Type Error\n");
		return;
	}

	if (dwType == POINT_LEVEL)
	{
		CInstanceBase* pkPlayer = NEW_GetMainActorPtr();

		if (pkPlayer && pkPlayer->GetLevel() != lValue)
		{
#ifdef ENABLE_TEXT_LEVEL_REFRESH
			// basically, just for the /level command to refresh locally
			pkPlayer->SetLevel(lValue);
#endif
			pkPlayer->UpdateTextTailLevel(lValue);
		}
	}

	switch (dwType)
	{
		case POINT_MIN_WEP:
		case POINT_MAX_WEP:
		case POINT_MIN_ATK:
		case POINT_MAX_ATK:
		case POINT_HIT_RATE:
		case POINT_EVADE_RATE:
		case POINT_LEVEL:
		case POINT_ST:
		case POINT_DX:
		case POINT_IQ:
			m_playerStatus.SetPoint(dwType, lValue);
			__UpdateBattleStatus();
			break;
		default:
			m_playerStatus.SetPoint(dwType, lValue);
			break;
	}
}

int CPythonPlayer::GetStatus(DWORD dwType)
{
	if (dwType >= POINT_MAX_NUM)
	{
		assert(!" CPythonPlayer::GetStatus - Strange Status Type!");
		Tracef("CPythonPlayer::GetStatus - Get Status Type Error\n");
		return 0;
	}

	return m_playerStatus.GetPoint(dwType);
}

const char* CPythonPlayer::GetName()
{
	return m_stName.c_str();
}

void CPythonPlayer::SetName(const char *name)
{
	m_stName = name;
}

void CPythonPlayer::NotifyDeletingCharacterInstance(DWORD dwVID)
{
	if (m_dwMainCharacterIndex == dwVID)
		m_dwMainCharacterIndex = 0;
}

void CPythonPlayer::NotifyCharacterDead(DWORD dwVID)
{
	if (__IsSameTargetVID(dwVID))
	{
		SetTarget(0);
	}
}

void CPythonPlayer::NotifyCharacterUpdate(DWORD dwVID)
{
	if (__IsSameTargetVID(dwVID))
	{
		CInstanceBase * pMainInstance = NEW_GetMainActorPtr();
		CInstanceBase * pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(dwVID);
		if (pMainInstance && pTargetInstance)
		{
			if (!pMainInstance->IsTargetableInstance(*pTargetInstance))
			{
				SetTarget(0);
				PyCallClassMemberFunc(m_ppyGameWindow, "CloseTargetBoard", Py_BuildValue("()"));
			}
			else
			{
				PyCallClassMemberFunc(m_ppyGameWindow, "RefreshTargetBoardByVID", Py_BuildValue("(i)", dwVID));
			}
		}
	}
}

void CPythonPlayer::NotifyDeadMainCharacter()
{
	__ClearReservedAction();
	__ClearAutoAttackTargetActorID();
	__ClearTarget();
}

void CPythonPlayer::NotifyChangePKMode()
{
	PyCallClassMemberFunc(m_ppyGameWindow, "OnChangePKMode", Py_BuildValue("()"));
}


void CPythonPlayer::MoveItemData(TItemPos SrcCell, TItemPos DstCell)
{
	if (!SrcCell.IsValidCell() || !DstCell.IsValidCell())
		return;

	TItemData src_item(*GetItemData(SrcCell));
	TItemData dst_item(*GetItemData(DstCell));
	SetItemData(DstCell, src_item);
	SetItemData(SrcCell, dst_item);
}

const TItemData * CPythonPlayer::GetItemData(TItemPos Cell) const
{
	if (!Cell.IsValidCell())
		return NULL;

	switch (Cell.window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		return &m_playerStatus.aItem[Cell.cell];
	case DRAGON_SOUL_INVENTORY:
		return &m_playerStatus.aDSItem[Cell.cell];	
#ifdef ENABLE_EXTRA_INVENTORY
	case EXTRA_INVENTORY:
		return &m_playerStatus.aExtraItem[Cell.cell];
#endif
#ifdef ENABLE_SWITCHBOT_WORLDARD
	case SWITCHBOT:
		return &m_playerStatus.aSBItem[Cell.cell];
#endif
#if defined(USE_ATTR_6TH_7TH)
	case ATTR67_ADD:
		return &m_playerStatus.aAttr67AddItem;
#endif
	default:
		return NULL;
	}
}

void CPythonPlayer::SetItemData(TItemPos Cell, const TItemData & c_rkItemInst)
{
	if (!Cell.IsValidCell())
		return;

	if (c_rkItemInst.vnum != 0)
	{
		CItemData * pItemData;
		if (!CItemManager::Instance().GetItemDataPointer(c_rkItemInst.vnum, &pItemData))
		{
			TraceError("CPythonPlayer::SetItemData(window_type : %d, dwSlotIndex=%d, itemIndex=%d) - Failed to item data\n", Cell.window_type, Cell.cell, c_rkItemInst.vnum);
			return;
		}
	}

	switch (Cell.window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		m_playerStatus.aItem[Cell.cell] = c_rkItemInst;
		break;
	case DRAGON_SOUL_INVENTORY:
		m_playerStatus.aDSItem[Cell.cell] = c_rkItemInst;
		break;
#ifdef ENABLE_EXTRA_INVENTORY
	case EXTRA_INVENTORY:
		m_playerStatus.aExtraItem[Cell.cell] = c_rkItemInst;
		break;
#endif
#ifdef ENABLE_SWITCHBOT_WORLDARD
	case SWITCHBOT:
		m_playerStatus.aSBItem[Cell.cell] = c_rkItemInst;
		break;
#endif
#if defined(USE_ATTR_6TH_7TH)
	case ATTR67_ADD:
		m_playerStatus.aAttr67AddItem = c_rkItemInst;
		break;
#endif
	}

}

#if defined(ENABLE_GAYA_RENEWAL)
void CPythonPlayer::SetGemShopItemData(BYTE slotIndex, const TGemShopItem & rItemInstance)
{
	m_GemItemsMap.insert(std::make_pair(slotIndex, rItemInstance));
}

bool CPythonPlayer::GetGemShopItemData(BYTE slotIndex,const TGemShopItem ** ppGemItemInfo)
{
	std::map<BYTE, TGemShopItem>::iterator itor = m_GemItemsMap.find(slotIndex);

	if (m_GemItemsMap.end() == itor)
		return false;

	*ppGemItemInfo = &(itor->second);

	return true;
}
#endif

DWORD CPythonPlayer::GetItemIndex(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;

	return GetItemData(Cell)->vnum;
}

DWORD CPythonPlayer::GetItemFlags(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;
	const TItemData * pItem = GetItemData(Cell);
	assert (pItem != NULL);
	return pItem->flags;
}

DWORD CPythonPlayer::GetItemAntiFlags(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;

	const TItemData * pItem = GetItemData(Cell);
	assert(pItem != NULL);
	return pItem->anti_flags;
}

BYTE CPythonPlayer::GetItemTypeBySlot(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;

	CItemData * pItemDataPtr = NULL;
	if (CItemManager::Instance().GetItemDataPointer(GetItemIndex(Cell), &pItemDataPtr)) {
		return pItemDataPtr->GetType();
	} else {
		return 0;
	}
}

BYTE CPythonPlayer::GetItemSubTypeBySlot(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;
	
	CItemData * pItemDataPtr = NULL;
	if (CItemManager::Instance().GetItemDataPointer(GetItemIndex(Cell), &pItemDataPtr)) {
		return pItemDataPtr->GetSubType();
	} else {
		return 0;
	}
}

DWORD CPythonPlayer::GetItemCount(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;
	const TItemData * pItem = GetItemData(Cell);
	if (pItem == NULL)
		return 0;
	else
		return pItem->count;
}

DWORD CPythonPlayer::GetItemCountByVnum(DWORD dwVnum)
{
	if (dwVnum == 0) {
		return 0;
	}
	DWORD dwCount = 0;

	for (int i = 0; i < c_Inventory_Count; ++i)
	{
		const TItemData & c_rItemData = m_playerStatus.aItem[i];
		if (c_rItemData.vnum == dwVnum)
		{
			dwCount += c_rItemData.count;
		}
	}

	return dwCount;
}


#if defined(ENABLE_EXTRA_INVENTORY)
uint32_t CPythonPlayer::GetItemCountbyVnumExtraInventory(uint32_t dwVnum)
{
	if (dwVnum == 0)
	{
		return 0;
	}

	uint32_t dwCount = 0;

	for (uint16_t i = 0; i < c_Extra_Inventory_Count; ++i)
	{
		const TItemData & c_rItemData = m_playerStatus.aExtraItem[i];
		if (c_rItemData.vnum == dwVnum)
		{
			dwCount += c_rItemData.count;
		}
	}

	return dwCount;
}
#endif


DWORD CPythonPlayer::GetItemMetinSocket(TItemPos Cell, DWORD dwMetinSocketIndex)
{
	if (!Cell.IsValidCell())
		return 0;

	if (dwMetinSocketIndex >= ITEM_SOCKET_SLOT_MAX_NUM)
		return 0;

	return GetItemData(Cell)->alSockets[dwMetinSocketIndex];
}

void CPythonPlayer::GetItemAttribute(TItemPos Cell, DWORD dwAttrSlotIndex, BYTE * pbyType, short * psValue)
{
	*pbyType = 0;
	*psValue = 0;

	if (!Cell.IsValidCell())
		return;

	if (dwAttrSlotIndex >= ITEM_ATTRIBUTE_SLOT_MAX_NUM)
		return;

	*pbyType = GetItemData(Cell)->aAttr[dwAttrSlotIndex].bType;
	*psValue = GetItemData(Cell)->aAttr[dwAttrSlotIndex].sValue;
}

#ifdef ATTR_LOCK
void CPythonPlayer::SetItemAttrLocked(TItemPos Cell, short dwIndex)
{
	if (!Cell.IsValidCell())
		return;

	(const_cast <TItemData*>(GetItemData(Cell)))->lockedattr = dwIndex;
	PyCallClassMemberFunc(m_ppyGameWindow, "RefreshInventory", Py_BuildValue("()"));
}

short CPythonPlayer::GetItemAttrLocked(TItemPos Cell)
{
	if (Cell.IsValidCell())
	{
		const TItemData* pkItem = GetItemData(Cell);
		if (pkItem)
			return pkItem->lockedattr;
	}

	return -1;
}
#endif

void CPythonPlayer::SetItemCount(TItemPos Cell, WORD byCount)
{
	if (!Cell.IsValidCell())
		return;

	(const_cast <TItemData *>(GetItemData(Cell)))->count = byCount;
	PyCallClassMemberFunc(m_ppyGameWindow, "RefreshInventory", Py_BuildValue("()"));
}

#if defined(ENABLE_CHANGELOOK)
void CPythonPlayer::SetItemTransmutation(TItemPos Cell, uint32_t dwVnum) {
	if (!Cell.IsValidCell()) {
		return;
	}

	(const_cast <TItemData*>(GetItemData(Cell)))->transmutation = dwVnum;
	PyCallClassMemberFunc(m_ppyGameWindow, "RefreshInventory", Py_BuildValue("()"));
}

uint32_t CPythonPlayer::GetItemTransmutation(TItemPos Cell) {
	if (Cell.IsValidCell()) {
		const TItemData* pkItem = GetItemData(Cell);
		return pkItem ? pkItem->transmutation : 0;
	}

	return 0;
}
#endif

void CPythonPlayer::SetItemMetinSocket(TItemPos Cell, DWORD dwMetinSocketIndex, DWORD dwMetinNumber)
{
	if (!Cell.IsValidCell())
		return;
	if (dwMetinSocketIndex >= ITEM_SOCKET_SLOT_MAX_NUM)
		return;

	(const_cast <TItemData *>(GetItemData(Cell)))->alSockets[dwMetinSocketIndex] = dwMetinNumber;
}

void CPythonPlayer::SetItemAttribute(TItemPos Cell, DWORD dwAttrIndex, BYTE byType, short sValue)
{
	if (!Cell.IsValidCell())
		return;
	if (dwAttrIndex >= ITEM_ATTRIBUTE_SLOT_MAX_NUM)
		return;

	(const_cast <TItemData *>(GetItemData(Cell)))->aAttr[dwAttrIndex].bType = byType;
	(const_cast <TItemData *>(GetItemData(Cell)))->aAttr[dwAttrIndex].sValue = sValue;
}

int32_t CPythonPlayer::GetQuickPage()
{
	return m_playerStatus.lQuickPageIndex;
}

void CPythonPlayer::SetQuickPage(int32_t nQuickPageIndex)
{
	if (nQuickPageIndex < 0)
	{
		m_playerStatus.lQuickPageIndex = QUICKSLOT_MAX_LINE + nQuickPageIndex;
	}
	else if (nQuickPageIndex >= QUICKSLOT_MAX_LINE)
	{
		m_playerStatus.lQuickPageIndex = nQuickPageIndex % QUICKSLOT_MAX_LINE;
	}
	else
	{
		m_playerStatus.lQuickPageIndex = nQuickPageIndex;
	}

	PyCallClassMemberFunc(m_ppyGameWindow, "RefreshInventory", Py_BuildValue("()"));
}

uint32_t CPythonPlayer::LocalQuickSlotIndexToGlobalQuickSlotIndex(uint32_t dwLocalSlotIndex)
{
	return m_playerStatus.lQuickPageIndex * QUICKSLOT_MAX_COUNT_PER_LINE + dwLocalSlotIndex;
}

void CPythonPlayer::GetGlobalQuickSlotData(uint32_t dwGlobalSlotIndex, uint8_t* pdwWndType, uint16_t* pdwWndItemPos)
{
	TQuickSlot& rkQuickSlot = __RefGlobalQuickSlot(dwGlobalSlotIndex);
	*pdwWndType = rkQuickSlot.type;
	*pdwWndItemPos = rkQuickSlot.pos;
}

void CPythonPlayer::GetLocalQuickSlotData(uint32_t dwSlotPos, uint8_t* pdwWndType, uint16_t* pdwWndItemPos)
{
	TQuickSlot& rkQuickSlot = __RefLocalQuickSlot(dwSlotPos);
	*pdwWndType = rkQuickSlot.type;
	*pdwWndItemPos = rkQuickSlot.pos;
}

TQuickSlot & CPythonPlayer::__RefLocalQuickSlot(int32_t SlotIndex)
{
	return __RefGlobalQuickSlot(LocalQuickSlotIndexToGlobalQuickSlotIndex(SlotIndex));
}

TQuickSlot & CPythonPlayer::__RefGlobalQuickSlot(int32_t SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= QUICKSLOT_MAX_NUM)
	{
		static TQuickSlot s_kQuickSlot;
		s_kQuickSlot.type = 0;
		s_kQuickSlot.pos = 0;

		return s_kQuickSlot;
	}

	return m_playerStatus.aQuickSlot[SlotIndex];
}

void CPythonPlayer::RemoveQuickSlotByValue(int32_t iType, int32_t iPosition)
{
	for (uint8_t i = 0; i < QUICKSLOT_MAX_NUM; ++i)
	{
		if (iType == m_playerStatus.aQuickSlot[i].type) {
			if (iPosition == m_playerStatus.aQuickSlot[i].pos) {
				CPythonNetworkStream::Instance().SendQuickSlotDelPacket(i);
			}
		}
	}
}

char CPythonPlayer::IsItem(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;

	return 0 != GetItemData(Cell)->vnum;
}

void CPythonPlayer::RequestMoveGlobalQuickSlotToLocalQuickSlot(uint32_t dwGlobalSrcSlotIndex, uint32_t dwLocalDstSlotIndex)
{
	uint32_t dwGlobalDstSlotIndex=LocalQuickSlotIndexToGlobalQuickSlotIndex(dwLocalDstSlotIndex);

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendQuickSlotMovePacket((uint8_t)dwGlobalSrcSlotIndex, (uint8_t)dwGlobalDstSlotIndex);
}

void CPythonPlayer::RequestAddLocalQuickSlot(uint32_t dwLocalSlotIndex, uint8_t dwWndType, uint16_t dwWndItemPos)
{
	if (dwLocalSlotIndex >= QUICKSLOT_MAX_COUNT_PER_LINE)
	{
		return;
	}

	uint32_t dwGlobalSlotIndex=LocalQuickSlotIndexToGlobalQuickSlotIndex(dwLocalSlotIndex);

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendQuickSlotAddPacket((uint8_t)dwGlobalSlotIndex, (uint8_t)dwWndType, (uint16_t)dwWndItemPos);
}

void CPythonPlayer::RequestAddToEmptyLocalQuickSlot(uint8_t dwWndType, uint16_t dwWndItemPos)
{
	for (int32_t i = 0; i < QUICKSLOT_MAX_COUNT_PER_LINE; ++i)
	{
		TQuickSlot& rkQuickSlot=__RefLocalQuickSlot(i);

		if (0 == rkQuickSlot.type)
		{
			uint32_t dwGlobalQuickSlotIndex=LocalQuickSlotIndexToGlobalQuickSlotIndex(i);

			CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
			rkNetStream.SendQuickSlotAddPacket((uint8_t)dwGlobalQuickSlotIndex, (uint8_t)dwWndType, (uint16_t)dwWndItemPos);
			return;
		}
	}
}

void CPythonPlayer::RequestDeleteGlobalQuickSlot(uint32_t dwGlobalSlotIndex)
{
	if (dwGlobalSlotIndex >= QUICKSLOT_MAX_COUNT)
	{
		return;
	}

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendQuickSlotDelPacket((uint8_t)dwGlobalSlotIndex);
}

void CPythonPlayer::RequestUseLocalQuickSlot(uint32_t dwLocalSlotIndex)
{
	if (dwLocalSlotIndex >= QUICKSLOT_MAX_COUNT_PER_LINE) {
		return;
	}

	uint8_t dwRegisteredType;
	uint16_t dwRegisteredItemPos;
	GetLocalQuickSlotData(dwLocalSlotIndex, &dwRegisteredType, &dwRegisteredItemPos);

	switch (dwRegisteredType)
	{
#if defined(ENABLE_EXTRA_INVENTORY)
		case 4: {
			CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
			rkNetStream.SendItemUsePacket(TItemPos(EXTRA_INVENTORY, dwRegisteredItemPos));
			break;
		}
#endif
		case SLOT_TYPE_INVENTORY: {
			CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
			rkNetStream.SendItemUsePacket(TItemPos(INVENTORY, dwRegisteredItemPos));
			break;
		}
		case SLOT_TYPE_SKILL: {
			ClickSkillSlot(dwRegisteredItemPos);
			break;
		}
		case SLOT_TYPE_EMOTION: {
			PyCallClassMemberFunc(m_ppyGameWindow, "BINARY_ActEmotion", Py_BuildValue("(i)", dwRegisteredItemPos));
			break;
		}
		default: {
			break;
		}
	}
}

void CPythonPlayer::AddQuickSlot(int32_t QuickSlotIndex, uint8_t IconType, uint16_t IconPosition)
{
	if (QuickSlotIndex < 0 || QuickSlotIndex >= QUICKSLOT_MAX_NUM)
	{
		return;
	}

	m_playerStatus.aQuickSlot[QuickSlotIndex].type = IconType;
	m_playerStatus.aQuickSlot[QuickSlotIndex].pos = IconPosition;
}

void CPythonPlayer::DeleteQuickSlot(int32_t QuickSlotIndex)
{
	if (QuickSlotIndex < 0 || QuickSlotIndex >= QUICKSLOT_MAX_NUM)
		return;

	m_playerStatus.aQuickSlot[QuickSlotIndex].type = 0;
	m_playerStatus.aQuickSlot[QuickSlotIndex].pos = 0;
}

void CPythonPlayer::MoveQuickSlot(int32_t Source, int32_t Target)
{
	if (Source < 0 || Source >= QUICKSLOT_MAX_NUM)
		return;

	if (Target < 0 || Target >= QUICKSLOT_MAX_NUM)
		return;

	TQuickSlot& rkSrcSlot=__RefGlobalQuickSlot(Source);
	TQuickSlot& rkDstSlot=__RefGlobalQuickSlot(Target);

	std::swap(rkSrcSlot, rkDstSlot);
}

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
bool CPythonPlayer::IsBeltInventorySlot(TItemPos Cell)
{
	return Cell.IsBeltInventoryCell();
}
#endif

bool CPythonPlayer::IsInventorySlot(TItemPos Cell)
{
	return !Cell.IsEquipCell() && Cell.IsValidCell();
}

bool CPythonPlayer::IsEquipmentSlot(TItemPos Cell)
{
	return Cell.IsEquipCell();
}

bool CPythonPlayer::IsEquipItemInSlot(TItemPos Cell)
{
	if (!Cell.IsEquipCell())
	{
		return false;
	}

	const TItemData * pData = GetItemData(Cell);

	if (NULL == pData)
	{
		return false;
	}

	DWORD dwItemIndex = pData->vnum;

	CItemManager::Instance().SelectItemData(dwItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
	{
		TraceError("Failed to find ItemData - CPythonPlayer::IsEquipItem(window_type=%d, iSlotindex=%d)\n", Cell.window_type, Cell.cell);
		return false;
	}

	return pItemData->IsEquipment() ? true : false;
}


void CPythonPlayer::SetSkill(DWORD dwSlotIndex, DWORD dwSkillIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return;

	m_playerStatus.aSkill[dwSlotIndex].dwIndex = dwSkillIndex;
	m_skillSlotDict[dwSkillIndex] = dwSlotIndex;
}

int CPythonPlayer::GetSkillIndex(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0;

	return m_playerStatus.aSkill[dwSlotIndex].dwIndex;
}

bool CPythonPlayer::GetSkillSlotIndex(DWORD dwSkillIndex, DWORD* pdwSlotIndex)
{
	std::map<DWORD, DWORD>::iterator f=m_skillSlotDict.find(dwSkillIndex);
	if (m_skillSlotDict.end()==f)
	{
		return false;
	}

	*pdwSlotIndex=f->second;

	return true;
}

int CPythonPlayer::GetSkillGrade(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0;

	return m_playerStatus.aSkill[dwSlotIndex].iGrade;
}

int CPythonPlayer::GetSkillLevel(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0;

	return m_playerStatus.aSkill[dwSlotIndex].iLevel;
}

float CPythonPlayer::GetSkillCurrentEfficientPercentage(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0;

	return m_playerStatus.aSkill[dwSlotIndex].fcurEfficientPercentage;
}

float CPythonPlayer::GetSkillNextEfficientPercentage(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0;

	return m_playerStatus.aSkill[dwSlotIndex].fnextEfficientPercentage;
}

void CPythonPlayer::SetSkillLevel(DWORD dwSlotIndex, DWORD dwSkillLevel)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return;

	m_playerStatus.aSkill[dwSlotIndex].iGrade = -1;
	m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel;
//	assert(!"CPythonPlayer::SetSkillLevel - Don't use this function");
//	if (dwSlotIndex >= SKILL_MAX_NUM)
//		return;
//
//	m_playerStatus.aSkill[dwSlotIndex].iGrade = -1;
//	m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel;
}

void CPythonPlayer::SetSkillLevel_(DWORD dwSkillIndex, DWORD dwSkillGrade, DWORD dwSkillLevel)
{
	DWORD dwSlotIndex;
	if (!GetSkillSlotIndex(dwSkillIndex, &dwSlotIndex))
		return;

	if (dwSlotIndex >= SKILL_MAX_NUM)
		return;

	switch (dwSkillGrade)
	{
		case 0:
			m_playerStatus.aSkill[dwSlotIndex].iGrade = dwSkillGrade;
			m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel;
			break;
		case 1:
			m_playerStatus.aSkill[dwSlotIndex].iGrade = dwSkillGrade;
			m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel-20+1;
			break;
		case 2:
			m_playerStatus.aSkill[dwSlotIndex].iGrade = dwSkillGrade;
			m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel-30+1;
			break;
		case 3:
			m_playerStatus.aSkill[dwSlotIndex].iGrade = dwSkillGrade;
			m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel-40+1;
			break;
	}

	const DWORD SKILL_MAX_LEVEL = 40;





	if (dwSkillLevel>SKILL_MAX_LEVEL)
	{
		m_playerStatus.aSkill[dwSlotIndex].fcurEfficientPercentage = 0.0f;
		m_playerStatus.aSkill[dwSlotIndex].fnextEfficientPercentage = 0.0f;

		TraceError("CPythonPlayer::SetSkillLevel(SlotIndex=%d, SkillLevel=%d)", dwSlotIndex, dwSkillLevel);
		return;
	}

	m_playerStatus.aSkill[dwSlotIndex].fcurEfficientPercentage	= LocaleService_GetSkillPower(dwSkillLevel)/100.0f;
	m_playerStatus.aSkill[dwSlotIndex].fnextEfficientPercentage = LocaleService_GetSkillPower(dwSkillLevel+1)/100.0f;

}

void CPythonPlayer::SetSkillCoolTime(DWORD dwSkillIndex)
{
	DWORD dwSlotIndex;
	if (!GetSkillSlotIndex(dwSkillIndex, &dwSlotIndex))
	{
		Tracenf("CPythonPlayer::SetSkillCoolTime(dwSkillIndex=%d) - FIND SLOT ERROR", dwSkillIndex);
		return;
	}

	if (dwSlotIndex>=SKILL_MAX_NUM)
	{
		Tracenf("CPythonPlayer::SetSkillCoolTime(dwSkillIndex=%d) - dwSlotIndex=%d/%d OUT OF RANGE", dwSkillIndex, dwSlotIndex, SKILL_MAX_NUM);
		return;
	}

	m_playerStatus.aSkill[dwSlotIndex].isCoolTime=true;
}

void CPythonPlayer::EndSkillCoolTime(DWORD dwSkillIndex)
{
	DWORD dwSlotIndex;
	if (!GetSkillSlotIndex(dwSkillIndex, &dwSlotIndex))
	{
		Tracenf("CPythonPlayer::EndSkillCoolTime(dwSkillIndex=%d) - FIND SLOT ERROR", dwSkillIndex);
		return;
	}

	if (dwSlotIndex>=SKILL_MAX_NUM)
	{
		Tracenf("CPythonPlayer::EndSkillCoolTime(dwSkillIndex=%d) - dwSlotIndex=%d/%d OUT OF RANGE", dwSkillIndex, dwSlotIndex, SKILL_MAX_NUM);
		return;
	}

	m_playerStatus.aSkill[dwSlotIndex].isCoolTime=false;
}

float CPythonPlayer::GetSkillCoolTime(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0.0f;

	return m_playerStatus.aSkill[dwSlotIndex].fCoolTime;
}

float CPythonPlayer::GetSkillElapsedCoolTime(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0.0f;

	return CTimer::Instance().GetCurrentSecond() - m_playerStatus.aSkill[dwSlotIndex].fLastUsedTime;
}

void CPythonPlayer::__ActivateSkillSlot(DWORD dwSlotIndex)
{
	if (dwSlotIndex>=SKILL_MAX_NUM)
	{
		Tracenf("CPythonPlayer::ActivavteSkill(dwSlotIndex=%d/%d) - OUT OF RANGE", dwSlotIndex, SKILL_MAX_NUM);
		return;
	}

	m_playerStatus.aSkill[dwSlotIndex].bActive = TRUE;
	PyCallClassMemberFunc(m_ppyGameWindow, "ActivateSkillSlot", Py_BuildValue("(i)", dwSlotIndex));
}

void CPythonPlayer::__DeactivateSkillSlot(DWORD dwSlotIndex)
{
	if (dwSlotIndex>=SKILL_MAX_NUM)
	{
		Tracenf("CPythonPlayer::DeactivavteSkill(dwSlotIndex=%d/%d) - OUT OF RANGE", dwSlotIndex, SKILL_MAX_NUM);
		return;
	}

	m_playerStatus.aSkill[dwSlotIndex].bActive = FALSE;
	PyCallClassMemberFunc(m_ppyGameWindow, "DeactivateSkillSlot", Py_BuildValue("(i)", dwSlotIndex));
}

BOOL CPythonPlayer::IsSkillCoolTime(DWORD dwSlotIndex)
{
	if (!__CheckRestSkillCoolTime(dwSlotIndex))
		return FALSE;

	return TRUE;
}

BOOL CPythonPlayer::IsSkillActive(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return FALSE;

	return m_playerStatus.aSkill[dwSlotIndex].bActive;
}

BOOL CPythonPlayer::IsToggleSkill(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return FALSE;

	DWORD dwSkillIndex = m_playerStatus.aSkill[dwSlotIndex].dwIndex;

	CPythonSkill::TSkillData * pSkillData;
	if (!CPythonSkill::Instance().GetSkillData(dwSkillIndex, &pSkillData))
		return FALSE;

	return pSkillData->IsToggleSkill();
}

void CPythonPlayer::SetPlayTime(DWORD dwPlayTime)
{
	m_dwPlayTime = dwPlayTime;
}

DWORD CPythonPlayer::GetPlayTime()
{
	return m_dwPlayTime;
}

#ifdef USE_QUICK_PICKUP
void CPythonPlayer::SendClickItemPacket(uint32_t dwID, bool bFromPickup/* = false*/)
#else
void CPythonPlayer::SendClickItemPacket(uint32_t dwIID)
#endif
{
    if (IsObserverMode())
    {
        return;
    }

    static uint32_t s_dwNextTCPTime = 0;
    uint32_t dwCurTime = ELTimer_GetMSec();

#ifdef USE_QUICK_PICKUP
    if (bFromPickup == true || dwCurTime >= s_dwNextTCPTime)
#else
    if (dwCurTime >= s_dwNextTCPTime)
#endif
    {
#ifdef USE_QUICK_PICKUP
        if (bFromPickup == false)
        {
            s_dwNextTCPTime = dwCurTime + 500;
        }
#else
        s_dwNextTCPTime = dwCurTime + 500;
#endif

        const char* c_szOwnerName;
        if (!CPythonItem::Instance().GetOwnership(dwID, &c_szOwnerName))
        {
            return;
        }

        if (strlen(c_szOwnerName) > 0)
        {
            if (strcmp(c_szOwnerName, GetName()) != 0)
            {
                CItemData * pItemData;
                if (!CItemManager::Instance().GetItemDataPointer(CPythonItem::Instance().GetVirtualNumberOfGroundItem(dwID), &pItemData))
                {
//                    Tracenf("CPythonPlayer::SendClickItemPacket(v=%d) : Non-exist item.", dwID);
                    return;
                }

                if (!IsPartyMemberByName(c_szOwnerName))
                {
                    PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotPickItem", Py_BuildValue("()"));
                    return;
                }
            }
        }

        CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
        rkNetStream.SendItemPickUpPacket(dwID);
    }
}

void CPythonPlayer::__SendClickActorPacket(CInstanceBase& rkInstVictim)
{
	// 말을 타고 광산을 캐는 것에 대한 예외 처리
	CInstanceBase* pkInstMain=NEW_GetMainActorPtr();
	if (pkInstMain)
	if (pkInstMain->IsHoldingPickAxe())
	if (pkInstMain->IsMountingHorse())
	if (rkInstVictim.IsResource())
	{
		PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotMining", Py_BuildValue("()"));
		return;
	}

	static DWORD s_dwNextTCPTime = 0;

	DWORD dwCurTime=ELTimer_GetMSec();

	if (dwCurTime >= s_dwNextTCPTime)
	{
		s_dwNextTCPTime=dwCurTime+1000;

		CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();

		DWORD dwVictimVID=rkInstVictim.GetVirtualID();
		rkNetStream.SendOnClickPacket(dwVictimVID);
	}
}

void CPythonPlayer::ActEmotion(DWORD dwEmotionID)
{
	CInstanceBase * pkInstTarget = __GetAliveTargetInstancePtr();
	if (!pkInstTarget)
	{
		PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotShotError", Py_BuildValue("(is)", GetMainCharacterIndex(), "NEED_TARGET"));
		return;
	}

	CPythonNetworkStream::Instance().SendChatPacket(_getf("/kiss %s", pkInstTarget->GetNameString()));
}

void CPythonPlayer::StartEmotionProcess()
{
	__ClearReservedAction();
	__ClearAutoAttackTargetActorID();

	m_bisProcessingEmotion = TRUE;
}

void CPythonPlayer::EndEmotionProcess()
{
	m_bisProcessingEmotion = FALSE;
}

BOOL CPythonPlayer::__IsProcessingEmotion()
{
	return m_bisProcessingEmotion;
}

// Dungeon
void CPythonPlayer::SetDungeonDestinationPosition(int ix, int iy)
{
	m_isDestPosition = TRUE;
	m_ixDestPos = ix;
	m_iyDestPos = iy;

	AlarmHaveToGo();
}

void CPythonPlayer::AlarmHaveToGo()
{
	m_iLastAlarmTime = CTimer::Instance().GetCurrentMillisecond();

	/////

	CInstanceBase * pInstance = NEW_GetMainActorPtr();
	if (!pInstance)
		return;

	TPixelPosition PixelPosition;
	pInstance->NEW_GetPixelPosition(&PixelPosition);

	float fAngle = GetDegreeFromPosition2(PixelPosition.x, PixelPosition.y, float(m_ixDestPos), float(m_iyDestPos));
	fAngle = fmod(540.0f - fAngle, 360.0f);
	D3DXVECTOR3 v3Rotation(0.0f, 0.0f, fAngle);

	PixelPosition.y *= -1.0f;

	CEffectManager::Instance().RegisterEffect("d:/ymir work/effect/etc/compass/appear_middle.mse");
	CEffectManager::Instance().CreateEffect("d:/ymir work/effect/etc/compass/appear_middle.mse", PixelPosition, v3Rotation);
}

// Party
void CPythonPlayer::ExitParty()
{
	m_PartyMemberMap.clear();

	CPythonCharacterManager::Instance().RefreshAllPCTextTail();
}

void CPythonPlayer::AppendPartyMember(DWORD dwPID, const char * c_szName)
{
	m_PartyMemberMap.insert(std::make_pair(dwPID, TPartyMemberInfo(dwPID, c_szName)));
}

void CPythonPlayer::LinkPartyMember(DWORD dwPID, DWORD dwVID)
{
	TPartyMemberInfo * pPartyMemberInfo;
	if (!GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
	{
		TraceError(" CPythonPlayer::LinkPartyMember(dwPID=%d, dwVID=%d) - Failed to find party member", dwPID, dwVID);
		return;
	}

	pPartyMemberInfo->dwVID = dwVID;

	CInstanceBase * pInstance = NEW_FindActorPtr(dwVID);
	if (pInstance)
		pInstance->RefreshTextTail();
}

void CPythonPlayer::UnlinkPartyMember(DWORD dwPID)
{
	TPartyMemberInfo * pPartyMemberInfo;
	if (!GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
	{
		TraceError(" CPythonPlayer::UnlinkPartyMember(dwPID=%d) - Failed to find party member", dwPID);
		return;
	}

	pPartyMemberInfo->dwVID = 0;
}

void CPythonPlayer::UpdatePartyMemberInfo(DWORD dwPID, BYTE byState, BYTE byHPPercentage)
{
	TPartyMemberInfo * pPartyMemberInfo;
	if (!GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
	{
		TraceError(" CPythonPlayer::UpdatePartyMemberInfo(dwPID=%d, byState=%d, byHPPercentage=%d) - Failed to find character", dwPID, byState, byHPPercentage);
		return;
	}

	pPartyMemberInfo->byState = byState;
	pPartyMemberInfo->byHPPercentage = byHPPercentage;
}

void CPythonPlayer::UpdatePartyMemberAffect(DWORD dwPID, BYTE byAffectSlotIndex, short sAffectNumber)
{
	if (byAffectSlotIndex >= PARTY_AFFECT_SLOT_MAX_NUM)
	{
		TraceError(" CPythonPlayer::UpdatePartyMemberAffect(dwPID=%d, byAffectSlotIndex=%d, sAffectNumber=%d) - Strange affect slot index", dwPID, byAffectSlotIndex, sAffectNumber);
		return;
	}

	TPartyMemberInfo * pPartyMemberInfo;
	if (!GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
	{
		TraceError(" CPythonPlayer::UpdatePartyMemberAffect(dwPID=%d, byAffectSlotIndex=%d, sAffectNumber=%d) - Failed to find character", dwPID, byAffectSlotIndex, sAffectNumber);
		return;
	}

	pPartyMemberInfo->sAffects[byAffectSlotIndex] = sAffectNumber;
}

void CPythonPlayer::RemovePartyMember(DWORD dwPID)
{
	DWORD dwVID = 0;
	TPartyMemberInfo * pPartyMemberInfo;
	if (GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
	{
		dwVID = pPartyMemberInfo->dwVID;
	}

	m_PartyMemberMap.erase(dwPID);

	if (dwVID > 0)
	{
		CInstanceBase * pInstance = NEW_FindActorPtr(dwVID);
		if (pInstance)
			pInstance->RefreshTextTail();
	}
}

bool CPythonPlayer::IsPartyMemberByVID(DWORD dwVID)
{
	std::map<DWORD, TPartyMemberInfo>::iterator itor = m_PartyMemberMap.begin();
	for (; itor != m_PartyMemberMap.end(); ++itor)
	{
		TPartyMemberInfo & rPartyMemberInfo = itor->second;
		if (dwVID == rPartyMemberInfo.dwVID)
			return true;
	}

	return false;
}

bool CPythonPlayer::IsPartyMemberByName(const char * c_szName)
{
	std::map<DWORD, TPartyMemberInfo>::iterator itor = m_PartyMemberMap.begin();
	for (; itor != m_PartyMemberMap.end(); ++itor)
	{
		TPartyMemberInfo & rPartyMemberInfo = itor->second;
		if (0 == rPartyMemberInfo.strName.compare(c_szName))
			return true;
	}

	return false;
}

bool CPythonPlayer::GetPartyMemberPtr(DWORD dwPID, TPartyMemberInfo ** ppPartyMemberInfo)
{
	std::map<DWORD, TPartyMemberInfo>::iterator itor = m_PartyMemberMap.find(dwPID);

	if (m_PartyMemberMap.end() == itor)
		return false;

	*ppPartyMemberInfo = &(itor->second);

	return true;
}

bool CPythonPlayer::PartyMemberPIDToVID(DWORD dwPID, DWORD * pdwVID)
{
	std::map<DWORD, TPartyMemberInfo>::iterator itor = m_PartyMemberMap.find(dwPID);

	if (m_PartyMemberMap.end() == itor)
		return false;

	const TPartyMemberInfo & c_rPartyMemberInfo = itor->second;
	*pdwVID = c_rPartyMemberInfo.dwVID;

	return true;
}

bool CPythonPlayer::PartyMemberVIDToPID(DWORD dwVID, DWORD * pdwPID)
{
	std::map<DWORD, TPartyMemberInfo>::iterator itor = m_PartyMemberMap.begin();
	for (; itor != m_PartyMemberMap.end(); ++itor)
	{
		TPartyMemberInfo & rPartyMemberInfo = itor->second;
		if (dwVID == rPartyMemberInfo.dwVID)
		{
			*pdwPID = rPartyMemberInfo.dwPID;
			return true;
		}
	}

	return false;
}

bool CPythonPlayer::IsSamePartyMember(DWORD dwVID1, DWORD dwVID2)
{
	return (IsPartyMemberByVID(dwVID1) && IsPartyMemberByVID(dwVID2));
}

// PVP
void CPythonPlayer::RememberChallengeInstance(DWORD dwVID)
{
	m_RevengeInstanceSet.erase(dwVID);
	m_ChallengeInstanceSet.insert(dwVID);
}
void CPythonPlayer::RememberRevengeInstance(DWORD dwVID)
{
	m_ChallengeInstanceSet.erase(dwVID);
	m_RevengeInstanceSet.insert(dwVID);
}
void CPythonPlayer::RememberCantFightInstance(DWORD dwVID)
{
	m_CantFightInstanceSet.insert(dwVID);
}
void CPythonPlayer::ForgetInstance(DWORD dwVID)
{
	m_ChallengeInstanceSet.erase(dwVID);
	m_RevengeInstanceSet.erase(dwVID);
	m_CantFightInstanceSet.erase(dwVID);
}

bool CPythonPlayer::IsChallengeInstance(DWORD dwVID)
{
	return m_ChallengeInstanceSet.end() != m_ChallengeInstanceSet.find(dwVID);
}
bool CPythonPlayer::IsRevengeInstance(DWORD dwVID)
{
	return m_RevengeInstanceSet.end() != m_RevengeInstanceSet.find(dwVID);
}
bool CPythonPlayer::IsCantFightInstance(DWORD dwVID)
{
	return m_CantFightInstanceSet.end() != m_CantFightInstanceSet.find(dwVID);
}

void CPythonPlayer::OpenPrivateShop()
{
	m_isOpenPrivateShop = TRUE;
}
void CPythonPlayer::ClosePrivateShop()
{
	m_isOpenPrivateShop = FALSE;
}

bool CPythonPlayer::IsOpenPrivateShop()
{
	return m_isOpenPrivateShop;
}

void CPythonPlayer::SetObserverMode(bool isEnable)
{
	m_isObserverMode=isEnable;
}

bool CPythonPlayer::IsObserverMode()
{
	return m_isObserverMode;
}


BOOL CPythonPlayer::__ToggleCoolTime()
{
	m_sysIsCoolTime = 1 - m_sysIsCoolTime;
	return m_sysIsCoolTime;
}

BOOL CPythonPlayer::__ToggleLevelLimit()
{
	m_sysIsLevelLimit = 1 - m_sysIsLevelLimit;
	return m_sysIsLevelLimit;
}

void CPythonPlayer::StartStaminaConsume(DWORD dwConsumePerSec, DWORD dwCurrentStamina)
{
	m_isConsumingStamina = TRUE;
	m_fConsumeStaminaPerSec = float(dwConsumePerSec);
	m_fCurrentStamina = float(dwCurrentStamina);

	SetStatus(POINT_STAMINA, dwCurrentStamina);
}

void CPythonPlayer::StopStaminaConsume(DWORD dwCurrentStamina)
{
	m_isConsumingStamina = FALSE;
	m_fConsumeStaminaPerSec = 0.0f;
	m_fCurrentStamina = float(dwCurrentStamina);

	SetStatus(POINT_STAMINA, dwCurrentStamina);
}

DWORD CPythonPlayer::GetPKMode()
{
	CInstanceBase * pInstance = NEW_GetMainActorPtr();
	if (!pInstance)
		return 0;

	return pInstance->GetPKMode();
}

void CPythonPlayer::SetMobileFlag(BOOL bFlag)
{
	m_bMobileFlag = bFlag;
	PyCallClassMemberFunc(m_ppyGameWindow, "RefreshMobile", Py_BuildValue("()"));
}

BOOL CPythonPlayer::HasMobilePhoneNumber()
{
	return m_bMobileFlag;
}

void CPythonPlayer::SetGameWindow(PyObject * ppyObject)
{
	m_ppyGameWindow = ppyObject;
}

void CPythonPlayer::NEW_ClearSkillData(bool bAll)
{
	std::map<DWORD, DWORD>::iterator it;

	for (it = m_skillSlotDict.begin(); it != m_skillSlotDict.end();)
	{
		if (bAll || __GetSkillType(it->first) == CPythonSkill::SKILL_TYPE_ACTIVE)
			it = m_skillSlotDict.erase(it);
		else
			++it;
	}

	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		ZeroMemory(&m_playerStatus.aSkill[i], sizeof(TSkillInstance));
	}

	for (int j = 0; j < SKILL_MAX_NUM; ++j)
	{
		// 2004.09.30.myevan.스킬갱신시 스킬 포인트업[+] 버튼이 안나와 처리
		m_playerStatus.aSkill[j].iGrade = 0;
		m_playerStatus.aSkill[j].fcurEfficientPercentage=0.0f;
		m_playerStatus.aSkill[j].fnextEfficientPercentage=0.05f;
	}

	if (m_ppyGameWindow)
		PyCallClassMemberFunc(m_ppyGameWindow, "BINARY_CheckGameButton", Py_BuildNone());
}

void CPythonPlayer::ClearSkillDict()
{
	// ClearSkillDict
	m_skillSlotDict.clear();

	// Game End - Player Data Reset
	m_isOpenPrivateShop = false;
	m_isObserverMode = false;

	m_isConsumingStamina = FALSE;
	m_fConsumeStaminaPerSec = 0.0f;
	m_fCurrentStamina = 0.0f;

	m_bMobileFlag = FALSE;

	__ClearAutoAttackTargetActorID();
}

void CPythonPlayer::Clear()
{
#ifdef USE_AUTO_HUNT
	ClearAutoAllSlot();
	m_bAutoUseOnOff = false;
#endif

	memset(&m_playerStatus, 0, sizeof(m_playerStatus));
	NEW_ClearSkillData(true);

	m_bisProcessingEmotion = FALSE;

	m_dwSendingTargetVID = 0;
	m_fTargetUpdateTime = 0.0f;

	// Test Code for Status Interface
	m_stName = "";
	m_dwMainCharacterIndex = 0;
	m_dwRace = 0;
	m_dwWeaponMinPower = 0;
	m_dwWeaponMaxPower = 0;
	m_dwWeaponMinMagicPower = 0;
	m_dwWeaponMaxMagicPower = 0;
	m_dwWeaponAddPower = 0;

	/////
	m_MovingCursorPosition = TPixelPosition(0, 0, 0);
	m_fMovingCursorSettingTime = 0.0f;

	m_eReservedMode = MODE_NONE;
	m_fReservedDelayTime = 0.0f;
	m_kPPosReserved = TPixelPosition(0, 0, 0);
	m_dwVIDReserved = 0;
	m_dwIIDReserved = 0;
	m_dwSkillSlotIndexReserved = 0;
	m_dwSkillRangeReserved = 0;

	m_isUp = false;
	m_isDown = false;
	m_isLeft = false;
	m_isRight = false;
	m_isSmtMov = false;
	m_isDirMov = false;
	m_isDirKey = false;
	m_isAtkKey = false;

	m_isCmrRot = true;
	m_fCmrRotSpd = 20.0f;

	m_iComboOld = 0;

	m_dwVIDPicked=0;
	m_dwIIDPicked=0;

	m_dwcurSkillSlotIndex = DWORD(-1);

	m_dwTargetVID = 0;
	m_dwTargetEndTime = 0;

	m_PartyMemberMap.clear();

	m_ChallengeInstanceSet.clear();
	m_RevengeInstanceSet.clear();

	m_isOpenPrivateShop = false;
	m_isObserverMode = false;
#if defined(ENABLE_CHANGELOOK)
	m_isOpenChangeLookWindow = false;
#endif

	m_isConsumingStamina = FALSE;
	m_fConsumeStaminaPerSec = 0.0f;
	m_fCurrentStamina = 0.0f;

	m_inGuildAreaID = 0xffffffff;

	m_bMobileFlag = FALSE;
#if defined(ENABLE_GAYA_RENEWAL)
	m_GemItemsMap.clear();
#endif

#ifdef ENABLE_AURA_SYSTEM
	m_AuraItemInstanceVector.clear();
	m_AuraItemInstanceVector.resize(AURA_SLOT_MAX);
	for (auto i = 0; i < m_AuraItemInstanceVector.size(); ++i) {
		TItemData& rkAuraItemInstance = m_AuraItemInstanceVector[i];
		ZeroMemory(&rkAuraItemInstance, sizeof(TItemData));
	}

	m_bAuraWindowOpen = false;
	m_bOpenedAuraWindowType = AURA_WINDOW_TYPE_MAX;

	for (auto j = 0; j < AURA_SLOT_MAX; ++j) {
		m_AuraRefineActivatedCell[j] = NPOS;
	}

	ZeroMemory(&m_bAuraRefineInfo, sizeof(TAuraRefineInfo));
#endif

	__ClearAutoAttackTargetActorID();

#ifdef USE_PICKUP_FILTER
    ClearFilterTable();
#endif
}

#if defined(ENABLE_AURA_SYSTEM)
void CPythonPlayer::__ClearAuraRefineWindow() {
	for (uint8_t i = 0; i < AURA_SLOT_MAX; ++i) {
		if (!m_AuraRefineActivatedCell[i].IsNPOS()) {
			m_AuraRefineActivatedCell[i] = NPOS;
		}
	}
}

void CPythonPlayer::SetAuraRefineWindowOpen(uint8_t wndType) {
	m_bOpenedAuraWindowType = wndType;
	m_bAuraWindowOpen = AURA_WINDOW_TYPE_MAX != wndType;

	if (!m_bAuraWindowOpen) {
		__ClearAuraRefineWindow();
	}
}

bool CPythonPlayer::IsAuraRefineWindowEmpty() {
	for (auto i = 0; i < m_AuraItemInstanceVector.size(); ++i) {
		if (m_AuraItemInstanceVector[i].vnum) {
			return false;
		}
	}

	return true;
}

void CPythonPlayer::SetAuraRefineInfo(uint8_t bAuraRefineInfoSlot, uint8_t bAuraRefineInfoLevel, uint8_t bAuraRefineInfoExpPercent)
{
	if (bAuraRefineInfoSlot >= AURA_REFINE_INFO_SLOT_MAX) {
		return;
	}

	m_bAuraRefineInfo[bAuraRefineInfoSlot].bAuraRefineInfoLevel = bAuraRefineInfoLevel;
	m_bAuraRefineInfo[bAuraRefineInfoSlot].bAuraRefineInfoExpPercent = bAuraRefineInfoExpPercent;
}

uint8_t CPythonPlayer::GetAuraRefineInfoLevel(uint8_t bAuraRefineInfoSlot) {
	return bAuraRefineInfoSlot >= AURA_REFINE_INFO_SLOT_MAX ? 0 : m_bAuraRefineInfo[bAuraRefineInfoSlot].bAuraRefineInfoLevel;
}

uint8_t CPythonPlayer::GetAuraRefineInfoExpPct(uint8_t bAuraRefineInfoSlot) {
	return bAuraRefineInfoSlot >= AURA_REFINE_INFO_SLOT_MAX ? 0 : m_bAuraRefineInfo[bAuraRefineInfoSlot].bAuraRefineInfoExpPercent;
}

void CPythonPlayer::SetAuraItemData(uint8_t bySlotIndex, const TItemData& rItemInstance) {
	if (bySlotIndex >= m_AuraItemInstanceVector.size()) {
		TraceError("CPythonPlayer::SetAuraItemData(bySlotIndex=%u) - Strange slot index", bySlotIndex);
		return;
	}

	m_AuraItemInstanceVector[bySlotIndex] = rItemInstance;
}

void CPythonPlayer::DelAuraItemData(uint8_t bySlotIndex) {
	if (bySlotIndex >= AURA_SLOT_MAX || bySlotIndex >= (uint8_t)m_AuraItemInstanceVector.size()) {
		TraceError("CPythonPlayer::DelAuraItemData(bySlotIndex=%u) - Strange slot index", bySlotIndex);
		return;
	}

	TItemData& rInstance = m_AuraItemInstanceVector[bySlotIndex];
	CItemData* pItemData;

	if (CItemManager::instance().GetItemDataPointer(rInstance.vnum, &pItemData)) {
		if (bySlotIndex == AURA_SLOT_MAIN || bySlotIndex == AURA_SLOT_SUB) {
			DelAuraItemData(AURA_SLOT_RESULT);
		}

		if (bySlotIndex != AURA_SLOT_RESULT) {
			m_AuraRefineActivatedCell[bySlotIndex] = NPOS;
		}

		ZeroMemory(&rInstance, sizeof(TItemData));
	}
}

uint8_t CPythonPlayer::FineMoveAuraItemSlot()
{
	for (auto i = 0; i < m_AuraItemInstanceVector.size(); ++i) {
		if (!m_AuraItemInstanceVector[i].vnum) {
			return (uint8_t)i;
		}
	}

	return AURA_SLOT_MAX;
}

uint8_t CPythonPlayer::GetAuraCurrentItemSlotCount() {
	uint8_t bCurCount = 0;

	for (auto i = 0; i < AURA_SLOT_MAX; ++i) {
		if (m_AuraItemInstanceVector[i].vnum) {
			++bCurCount;
		}
	}

	return bCurCount;
}

BOOL CPythonPlayer::GetAuraItemDataPtr(uint8_t bySlotIndex, TItemData** ppInstance) {
	if (bySlotIndex >= m_AuraItemInstanceVector.size()) {
		TraceError("CPythonPlayer::GetAuraItemDataPtr(bySlotIndex=%u) - Strange slot index", bySlotIndex);
		return FALSE;
	}

	*ppInstance = &m_AuraItemInstanceVector[bySlotIndex];

	return TRUE;
}

void CPythonPlayer::SetActivatedAuraSlot(uint8_t bySlotIndex, TItemPos ItemCell) {
	if (bySlotIndex >= AURA_SLOT_MAX) {
		return;
	}

	m_AuraRefineActivatedCell[bySlotIndex] = ItemCell;
}

uint8_t CPythonPlayer::FindActivatedAuraSlot(TItemPos ItemCell) {
	for (uint8_t i = AURA_SLOT_MAIN; i < AURA_SLOT_MAX; ++i) {
		if (m_AuraRefineActivatedCell[i] == ItemCell) {
			return uint8_t(i);
		}
	}

	return AURA_SLOT_MAX;
}

TItemPos CPythonPlayer::FindUsingAuraSlot(uint8_t bySlotIndex) {
	if (bySlotIndex >= AURA_SLOT_MAX) {
		return NPOS;
	}

	return m_AuraRefineActivatedCell[bySlotIndex];
}
#endif

#if defined(ENABLE_NEW_GOLD_LIMIT)
void CPythonPlayer::SPlayerStatus::SetGold(uint64_t gold)
{
	m_ullGold = gold;
}

uint64_t CPythonPlayer::SPlayerStatus::GetGold()
{
	return m_ullGold;
}

void CPythonPlayer::SetGold(uint64_t value)
{
	m_playerStatus.SetGold(value);
}

uint64_t CPythonPlayer::GetGold()
{
	return m_playerStatus.GetGold();
}
#endif

#ifdef USE_AUTO_HUNT
bool CPythonPlayer::GetAutoUseOnOff() const
{
    return m_bAutoUseOnOff;
}

void CPythonPlayer::SetAutoUseOnOff(const bool v)
{
    if (m_bAutoUseOnOff == true)
    {
        CInstanceBase* pkInstMain = NEW_GetMainActorPtr();
        if (pkInstMain)
        {
            __ClearReservedAction();
            __ClearTarget();
            __ClearAutoAttackTargetActorID();

            pkInstMain->NEW_Stop();
        }
    }

    m_bAutoUseOnOff = v;
}

void CPythonPlayer::__AutoProcess()
{
    CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
    if (rkChrMgr.GetAutoOnOff() != true)
    {
        return;
    }

    if (GetAutoUseOnOff() != true)
    {
        return;
    }

    if (rkChrMgr.GetAutoSystemByType(AUTO_ONOFF_ATTACK) == true)
    {
        __AutoAttack();
    }

    if (CPythonPlayer::Instance().GetStatus(POINT_AUTOHUNT_EXPIRE) > 0)
    {
        if (rkChrMgr.GetAutoSystemByType(AUTO_ONOFF_POTION) == true)
        {
            __AutoPotion();
        }

        if (rkChrMgr.GetAutoSystemByType(AUTO_ONOFF_SKILL) == true)
        {
            __AutoSkill();
        }
    }
}

void CPythonPlayer::__AutoAttack()
{
    auto pkInstMain = NEW_GetMainActorPtr();
    if (!pkInstMain)
    {
        return;
    }

    if (pkInstMain->IsAttacking())
    {
        return;
    }

    if (pkInstMain->IsDead() ||
        pkInstMain->IsAffect(CInstanceBase::AFFECT_REVIVE_INVISIBILITY))
    {
        return;
    }

    if (m_dwAutoAttackTargetVID != 0)
    {
        CInstanceBase* pkInstVictim = NEW_FindActorPtr(m_dwAutoAttackTargetVID);
        if (!pkInstVictim ||
            pkInstMain == pkInstVictim ||
            pkInstVictim->IsDead() ||
            (!pkInstVictim->IsEnemy() &&
            !pkInstVictim->IsStone()))
        {
            __ClearAutoAttackTargetActorID();
        }
    }

    CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();

    float fMinDistance = 4100.0f;
    float fMinFocusRadius = 4000.0f;
    float fLimit = 600.0f;

    TPixelPosition rkLastPixelPos, kMainPixelPos, rkTargetPos;

    CInstanceBase* pNearInst = nullptr;

    auto it = rkChrMgr.CharacterInstanceBegin();
    auto itEnd = rkChrMgr.CharacterInstanceEnd();
    for (it = rkChrMgr.CharacterInstanceBegin(); it != itEnd; ++it)
    {
        rkLastPixelPos = rkChrMgr.GetLastPixelPosition();
        pkInstMain->NEW_GetPixelPosition(&kMainPixelPos);

        auto pkInstEach = *it;
        if (!pkInstEach ||
            pkInstEach == pkInstMain ||
            pkInstEach->IsDead())
        {
            continue;
        }

        if (rkChrMgr.GetAutoSystemByType(AUTO_ONOFF_METINSTONES) == true)
        {
            if (!pkInstEach->IsStone())
            {
                continue;
            }
        }
        else
        {
            if (!pkInstEach->IsEnemy() && !pkInstEach->IsStone())
            {
                continue;
            }
        }

        if (rkChrMgr.GetAutoSystemByType(AUTO_ONOFF_RANGE) == true)
        {
            if (fabs(rkLastPixelPos.x - kMainPixelPos.x) > fLimit ||
                fabs(rkLastPixelPos.y - kMainPixelPos.y) > fLimit)
            {
                if (!pkInstMain->IsWalking())
                {
                    pkInstMain->NEW_MoveToDestPixelPositionDirection(rkLastPixelPos);
                }

                continue;
            }

            pkInstEach->NEW_GetPixelPosition(&rkTargetPos);

            if (fabs(rkLastPixelPos.x - rkTargetPos.x) < fMinFocusRadius &&
                fabs(rkLastPixelPos.y - rkTargetPos.y) < fMinFocusRadius)
            {
                float fDistance = pkInstMain->GetDistance(pkInstEach);
                if (fDistance < fMinDistance)
                {
                    fMinDistance = fDistance;
                    pNearInst = pkInstEach;
                }
            }

            if (fabs(rkLastPixelPos.x - kMainPixelPos.x) > fMinFocusRadius ||
                fabs(rkLastPixelPos.y - kMainPixelPos.y) > fMinFocusRadius)
            {
                if (!pkInstMain->IsWalking())
                {
                    pkInstMain->NEW_MoveToDestPixelPositionDirection(rkLastPixelPos);
                }

                continue;
            }
        }
        else
        {
            const auto fDistance = pkInstMain->GetDistance(pkInstEach);
            if (fDistance < fMinDistance)
            {
                fMinDistance = fDistance;
                pNearInst = pkInstEach;
            }
        }
    }

    if (pNearInst &&
        !pNearInst->IsDead())
    {
        m_dwAutoAttackTargetVID = pNearInst->GetVirtualID();
    }
}

void CPythonPlayer::__AutoPotion()
{
    CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();

    CItemData* pItemData = nullptr;
    for (uint8_t bSlotIndex = AUTO_POTION_SLOT_START; bSlotIndex < AUTO_POTION_SLOT_MAX; bSlotIndex++)
    {
        uint32_t c_dwItemSlotIndex = GetAutoAttachedSlotIndex(bSlotIndex);
        if (c_dwItemSlotIndex == USHRT_MAX)
        {
            continue;
        }

        uint32_t dwItemIndex = CPythonPlayer::Instance().GetItemIndex(TItemPos(INVENTORY, c_dwItemSlotIndex));
        if (dwItemIndex == 0)
        {
            continue;
        }

        if (!CItemManager::Instance().GetItemDataPointer(dwItemIndex, &pItemData))
        {
            TraceError("Cannot find item: %u", dwItemIndex);
            break;
        }

        if (std::count(std::begin(st_dwAutoHuntAllowedItems), std::end(st_dwAutoHuntAllowedItems), dwItemIndex) == 0)
        {
            continue;
        }

        const uint32_t c_dwSlotCooldown = GetAutoSlotCoolTime(bSlotIndex);
        const float c_fCurrentSec = CTimer::Instance().GetCurrentSecond();
        const float c_fLastUsedTime = GetAutoSlotLastUsedTime(bSlotIndex);

        switch (dwItemIndex)
        {
            case 70038:
            case 76007:
            {
                float fElapsedTime = c_fCurrentSec - c_fLastUsedTime;
                if (!c_fLastUsedTime || fElapsedTime >= static_cast<float>(c_dwSlotCooldown))
                {
                    rkNetStream.SendItemUsePacket(TItemPos(INVENTORY, c_dwItemSlotIndex));
                    SetAutoSlotLastUsedTime(bSlotIndex);
                }

                break;
            }
            default:
            {
                break;
            }
        }
    }
}

void CPythonPlayer::__AutoSkill()
{
    uint32_t dwSkillIndex = 0;

    for (uint8_t bSlotIndex = 0; bSlotIndex < AUTO_SKILL_SLOT_MAX; bSlotIndex++)
    {
        dwSkillIndex = GetAutoAttachedSlotIndex(bSlotIndex);
        if (dwSkillIndex == 0 ||
            dwSkillIndex > SKILL_MAX_NUM)
        {
            continue;
        }

        const float c_fCurrentSec = CTimer::Instance().GetCurrentSecond();
        if (!__CheckRestSkillCoolTime(dwSkillIndex))
        {
            float fElapsedTime = c_fCurrentSec - m_playerStatus.aSkill[dwSkillIndex].fLastUsedTime;
            if (fElapsedTime >= GetAutoSlotCoolTime(bSlotIndex))
            {
                if (IsToggleSkill(dwSkillIndex))
                {
                    if (!IsSkillActive(dwSkillIndex))
                    {
                        __UseSkill(dwSkillIndex);
                    }
                }
                else
                {
                    __UseSkill(dwSkillIndex);
                }
            }
        }
    }
}

uint32_t CPythonPlayer::GetAutoAttachedSlotIndex(const uint8_t bSlot)
{
    if (bSlot >= AUTO_POTION_SLOT_MAX)
    {
        return 0;
    }

    return dwAutoAttachedSlotIndex[bSlot];
}

void CPythonPlayer::SetAutoAttachedSlotIndex(const uint8_t bSlot,
                                                const uint32_t dwIndex)
{
    if (bSlot >= AUTO_POTION_SLOT_MAX)
    {
        return;
    }

    if (bSlot < AUTO_SKILL_SLOT_MAX)
    {
        if (dwIndex == 0)
        {
            dwAutoAttachedSlotIndex[bSlot] = 0;
            return;
        }

        for (uint8_t bSlotIndex = 0; bSlotIndex < AUTO_SKILL_SLOT_MAX; bSlotIndex++)
        {
            if (dwAutoAttachedSlotIndex[bSlotIndex] == dwIndex)
            {
                dwAutoAttachedSlotIndex[bSlotIndex] = 0;
            }
        }
    }
    else
    {
        if (dwIndex == USHRT_MAX)
        {
            dwAutoAttachedSlotIndex[bSlot] = USHRT_MAX;
            return;
        }

        if (dwIndex && dwIndex != USHRT_MAX)
        {
            uint32_t dwItemIndex = CPythonPlayer::Instance().GetItemIndex(TItemPos(INVENTORY, dwIndex));
            if (dwItemIndex == 0)
            {
                return;
            }

           CItemData* pItemData = nullptr;
           if (!CItemManager::Instance().GetItemDataPointer(dwItemIndex, &pItemData))
           {
               TraceError("Cannot find item data %u", dwItemIndex);
               return;
           }

           if (std::count(std::begin(st_dwAutoHuntAllowedItems), std::end(st_dwAutoHuntAllowedItems), dwItemIndex) == 0)
           {
               return;
           }
        }

        for (uint8_t bSlotIndex = AUTO_POTION_SLOT_START; bSlotIndex < AUTO_POTION_SLOT_MAX; bSlotIndex++)
        {
            if (dwAutoAttachedSlotIndex[bSlotIndex] == dwIndex)
            {
                dwAutoAttachedSlotIndex[bSlotIndex] = USHRT_MAX;
            }
        }
    }

    dwAutoAttachedSlotIndex[bSlot] = dwIndex;
    dwAutoCooldownSlotIndex[bSlot] = 0;
}

void CPythonPlayer::ClearAutoSkillSlot(int32_t iSlot/* = -1*/)
{
    if (iSlot < 0)
    {
        for (uint8_t i = 0; i < AUTO_SKILL_SLOT_MAX; i++)
        {
            dwAutoAttachedSlotIndex[i] = 0;
            m_afAutoSlotLastUsedTime[i] = 0;
        }
    }
    else if (iSlot < AUTO_SKILL_SLOT_MAX)
    {
        dwAutoAttachedSlotIndex[iSlot] = 0;
        m_afAutoSlotLastUsedTime[iSlot] = 0;
    }
}

void CPythonPlayer::ClearAutoPotionSlot(int32_t iSlot/* = -1*/)
{
    if (iSlot < 0)
    {
        for (uint8_t i = AUTO_POTION_SLOT_START; i < AUTO_POTION_SLOT_MAX; i++)
        {
            dwAutoAttachedSlotIndex[i] = USHRT_MAX;
            m_afAutoSlotLastUsedTime[i] = 0;
        }
    }
    else if (iSlot >= AUTO_POTION_SLOT_START &&
            iSlot < AUTO_POTION_SLOT_MAX)
    {
        dwAutoAttachedSlotIndex[iSlot] = USHRT_MAX;
        m_afAutoSlotLastUsedTime[iSlot] = 0;
    }
}

void CPythonPlayer::ClearAutoAllSlot()
{
    ClearAutoSkillSlot();
    ClearAutoPotionSlot();
}

float CPythonPlayer::GetAutoSlotLastUsedTime(const uint8_t bSlot) const
{
    if (bSlot >= AUTO_POTION_SLOT_MAX)
    {
        return 0.0f;
    }

    return m_afAutoSlotLastUsedTime[bSlot];
}

void CPythonPlayer::SetAutoSlotLastUsedTime(const uint8_t bSlot)
{
    if (bSlot >= AUTO_POTION_SLOT_MAX)
    {
        return;
    }

    m_afAutoSlotLastUsedTime[bSlot] = CTimer::Instance().GetCurrentSecond();
}

uint32_t CPythonPlayer::GetAutoSlotCoolTime(const uint8_t bSlot)
{
    if (bSlot >= AUTO_POTION_SLOT_MAX)
    {
        return 0;
    }

    return CheckAutoSlotCoolTime(bSlot, GetAutoAttachedSlotIndex(bSlot), dwAutoCooldownSlotIndex[bSlot]);
}

void CPythonPlayer::SetAutoSlotCoolTime(const uint8_t bSlot,
                                        const uint32_t dwCooldown)
{
    if (bSlot >= AUTO_POTION_SLOT_MAX)
    {
        return;
    }

    dwAutoCooldownSlotIndex[bSlot] = dwCooldown;
}

uint32_t CPythonPlayer::CheckAutoSlotCoolTime(const uint8_t c_bSlotIndex,
                                                const uint32_t dwIndex/* = 0*/,
                                                const uint32_t dwCooldown/* = 0*/)
{
    if (c_bSlotIndex < AUTO_SKILL_SLOT_MAX)
    {
        if (dwIndex == 0 ||
            dwIndex > SKILL_MAX_NUM)
        {
            return 0;
        }

        TSkillInstance& rkSkillInst = m_playerStatus.aSkill[dwIndex];

        CPythonSkill::TSkillData* pSkillData;
        if (!CPythonSkill::Instance().GetSkillData(rkSkillInst.dwIndex, &pSkillData))
        {
            TraceError("Cannot find skill: %u", rkSkillInst.dwIndex);
            return 0;
        }

        const uint32_t dwSkillCooldown = pSkillData->GetSkillCoolTime(float(rkSkillInst.fcurEfficientPercentage));

        return dwCooldown < dwSkillCooldown ?
                dwSkillCooldown : dwCooldown;
    }
    else
    {
        if (dwIndex == USHRT_MAX)
        {
            return 0;
        }

        uint32_t dwItemIndex = CPythonPlayer::Instance().GetItemIndex(TItemPos(INVENTORY, dwIndex));
        if (dwItemIndex == 0)
        {
            return 0;
        }

        CItemData* pItemData = nullptr;
        if (!CItemManager::Instance().GetItemDataPointer(dwItemIndex, &pItemData))
        {
            TraceError("Cannot find item data %u", dwItemIndex);
            return 0;
        }

        if (std::count(std::begin(st_dwAutoHuntAllowedItems), std::end(st_dwAutoHuntAllowedItems), dwItemIndex) == 0)
        {
            return 0;
        }

        switch (dwItemIndex)
        {
            case 70038:
            case 76007:
            {
                return dwCooldown == 0 ? 60 : dwCooldown;
            }
            default:
            {
                return 0;
            }
        }
    }

    return 0;
}

void CPythonPlayer::ClearTarget()
{
    auto* pkInstMain = NEW_GetMainActorPtr();
    if (pkInstMain)
    {
        pkInstMain->NEW_Stop();
        __ClearReservedAction();
    }
}
#endif

#ifdef USE_PICKUP_FILTER
size_t CPythonPlayer::GetFilterTableSize() const
{
    return m_mapFilterTable.size();
}

void CPythonPlayer::AddFilterTable(const uint8_t bIdx, TFilterTable tFilter)
{
    auto it = m_mapFilterTable.find(bIdx);
    if (it != m_mapFilterTable.end())
    {
        m_mapFilterTable.erase(it);
    }

    m_mapFilterTable[bIdx] = tFilter;
    SetFilterStatus(bIdx, true);
}

void CPythonPlayer::ClearFilterTable()
{
    m_mapFilterTable.clear();
    std::memset(m_bFilterStatus, true, sizeof(m_bFilterStatus));
    m_bFilterStatus[FILTER_TYPE_AUTO] = false;
}

bool CPythonPlayer::CanPickItem(uint32_t dwItemVnum)
{
    if (dwItemVnum == 0)
    {
        return false;
    }

    CItemData* pItem;
    if (!CItemManager::instance().GetItemDataPointer(dwItemVnum, &pItem))
    {
        return false;
    }

    uint8_t bItemType = pItem->GetType();
    uint8_t bItemSubType = pItem->GetSubType();

    int8_t bFilterType = -1;

    if (bItemType == CItemData::ITEM_TYPE_ARMOR)
    {
        if (bItemSubType == CItemData::WEAPON_ARROW)
        {
            bFilterType = FILTER_TYPE_OTHERS3;
        }
#ifdef ENABLE_QUIVER_SYSTEM
        else if (bItemSubType == CItemData::WEAPON_QUIVER)
        {
            bFilterType = FILTER_TYPE_OTHERS3;
        }
#endif
        else
        {
            bFilterType = FILTER_TYPE_WEAPON;
        }
    }
    else if (bItemType == CItemData::ITEM_TYPE_ARMOR)
    {
        if (bItemSubType == CItemData::ARMOR_BODY)
        {
            bFilterType = FILTER_TYPE_ARMOR_BODY;
        }
        else if (bItemSubType == CItemData::ARMOR_HEAD)
        {
            bFilterType = FILTER_TYPE_ARMOR_HEAD;
        }
        else
        {
            bFilterType = FILTER_TYPE_OTHERS;
        }
    }
    else if (bItemType == CItemData::ITEM_TYPE_COSTUME)
    {
#ifdef USE_MOUNT_COSTUME_SYSTEM
        if (bItemSubType == CItemData::COSTUME_MOUNT)
        {
            bFilterType = FILTER_TYPE_COMPANY;
        }
        else
        {
#endif
#ifdef ENABLE_PETSKIN
        if (bItemSubType == CItemData::COSTUME_PETSKIN)
        {
            bFilterType = FILTER_TYPE_COMPANY;
        }
        else
        {
#endif
#ifdef ENABLE_MOUNTSKIN
        if (bItemSubType == CItemData::COSTUME_MOUNTSKIN)
        {
            bFilterType = FILTER_TYPE_COMPANY;
        }
        else
        {
#endif
        bFilterType = FILTER_TYPE_COSTUME;
#ifdef ENABLE_MOUNTSKIN
        }
#endif
#ifdef ENABLE_PETSKIN
        }
#endif
#ifdef USE_MOUNT_COSTUME_SYSTEM
        }
#endif
    }
    else if (bItemType == CItemData::ITEM_TYPE_DS)
    {
        bFilterType = FILTER_TYPE_DS;
    }
    else if (bItemType == CItemData::ITEM_TYPE_UNIQUE)
    {
        bFilterType = FILTER_TYPE_UNIQUE;
    }
    else if (bItemType == CItemData::ITEM_TYPE_MATERIAL || bItemType == CItemData::ITEM_TYPE_METIN)
    {
        bFilterType = FILTER_TYPE_MATERIAL_AND_METIN;
    }
#ifdef ENABLE_NEW_USE_POTION
    else if (bItemType == CItemData::ITEM_TYPE_BLEND || (bItemType == CItemData::ITEM_TYPE_USE && (bItemSubType == CItemData::USE_POTION || bItemSubType == CItemData::USE_NEW_POTIION)))
#else
    else if (bItemType == CItemData::ITEM_TYPE_BLEND || (bItemType == CItemData::ITEM_TYPE_USE && bItemSubType == CItemData::USE_POTION))
#endif
    {
        bFilterType = FILTER_TYPE_POTION;
    }
    else if (bItemType == CItemData::ITEM_TYPE_FISH || (bItemType == CItemData::ITEM_TYPE_USE && bItemSubType == CItemData::USE_BAIT))
    {
        bFilterType = FILTER_TYPE_OTHERS2;
    }
    else if (bItemType == CItemData::ITEM_TYPE_SKILLBOOK)
    {
        bFilterType = FILTER_TYPE_BOOK;
    }
    else if (bItemType == CItemData::ITEM_TYPE_GIFTBOX)
    {
        bFilterType = FILTER_TYPE_OTHERS3;
    }
    else if ((bItemType == CItemData::ITEM_TYPE_ROD || bItemType == CItemData::ITEM_TYPE_PICK || bItemType == CItemData::ITEM_TYPE_BELT) && !IsPotionConsumables(dwItemVnum))
    {
        bFilterType = FILTER_TYPE_OTHERS;
    }
    else
    {
        if (IsCorDraconistItem(dwItemVnum))
        {
            bFilterType = FILTER_TYPE_DS;
        }
        else if (IsUniqueConsumablesItem(dwItemVnum))
        {
            bFilterType = FILTER_TYPE_UNIQUE;
        }
        else if (IsIncreasesChangeItem(dwItemVnum))
        {
            bFilterType = FILTER_TYPE_MATERIAL_AND_METIN;
        }
        else if (IsPotionConsumables(dwItemVnum) || IsHairDyesItem(dwItemVnum))
        {
            bFilterType = FILTER_TYPE_POTION;
        }
        else if (IsOreStones(dwItemVnum))
        {
            bFilterType = FILTER_TYPE_OTHERS2;
        }
        else if (IsEventItem(dwItemVnum))
        {
            bFilterType = FILTER_TYPE_EVENT;
        }
        else if (IsMarriageItem(dwItemVnum) || IsParchmentItem(dwItemVnum) || IsGroupItem(dwItemVnum) || IsTransformItem(dwItemVnum))
        {
            bFilterType = FILTER_TYPE_OTHERS3;
        }
        else if (bItemType == CItemData::ITEM_TYPE_USE && bItemSubType == CItemData::USE_TUNING)
        {
            bFilterType = FILTER_TYPE_OTHERS3;
        }
    }

    if (bFilterType == -1)
    {
        return true;
    }

    if (!GetFilterStatus(bFilterType))
    {
        return false;
    }

    auto it = m_mapFilterTable.find(bFilterType);
    if (it == m_mapFilterTable.end())
    {
        return true;
    }

    if (it->second.bRefineNeed)
    {
        if (!pItem->FilterCheckRefine(it->second.bRefine[0], it->second.bRefine[1]))
        {
            return false;
        }
    }

    if (it->second.bLevelNeed)
    {
        if (!pItem->FilterCheckLevel(it->second.bLevel[0], it->second.bLevel[1]))
        {
            return false;
        }
    }

    if (it->second.bJobNeed)
    {
        if (bFilterType == FILTER_TYPE_BOOK && dwItemVnum == 50300)
        {
            return true;
/*
            if (pItem->GetSocket(0) != 0)
            {
                const CSkillProto* pSkill = CSkillManager::instance().Get(pItem->GetSocket(0));
                if (pSkill)
                {
                    if (pSkill->dwType - 1 >= 0
                        && pSkill->dwType - 1 <= 3)
                    {
                        return it->second.bJobs[pSkill->dwType - 1];
                    }
                }
            }
*/
        }
        else
        {
            if (!it->second.bJobs[0])
            {
                if (!pItem->IsAntiFlag(CItemData::ITEM_ANTIFLAG_WARRIOR))
                {
                    return false;
                }
            }

            if (!it->second.bJobs[1])
            {
                if (!pItem->IsAntiFlag(CItemData::ITEM_ANTIFLAG_ASSASSIN))
                {
                    return false;
                }
            }

            if (!it->second.bJobs[2])
            {
                if (!pItem->IsAntiFlag(CItemData::ITEM_ANTIFLAG_SURA))
                {
                    return false;
                }
            }

            if (!it->second.bJobs[3])
            {
                if (!pItem->IsAntiFlag(CItemData::ITEM_ANTIFLAG_SHAMAN))
                {
                    return false;
                }
            }
        }
    }

    if (bFilterType == FILTER_TYPE_OTHERS)
    {
        if (bItemType == CItemData::ITEM_TYPE_ARMOR)
        {
            if (bItemSubType == CItemData::ARMOR_FOOTS)
            {
                return it->second.bSubs[0];
            }
            else if (bItemSubType == CItemData::ARMOR_WRIST)
            {
                return it->second.bSubs[2];
            }
            else if (bItemSubType == CItemData::ARMOR_NECK)
            {
                return it->second.bSubs[3];
            }
            else if (bItemSubType == CItemData::ARMOR_EAR)
            {
                return it->second.bSubs[4];
            }
            else if (bItemSubType == CItemData::ARMOR_SHIELD)
            {
                return it->second.bSubs[5];
            }
#ifdef ENABLE_PENDANT
            else if (bItemSubType == CItemData::ARMOR_PENDANT)
            {
                return it->second.bSubs[7];
            }
#endif
        }
        else if (bItemType == CItemData::ITEM_TYPE_BELT)
        {
            return it->second.bSubs[1];
        }
        else if (bItemType == CItemData::ITEM_TYPE_ROD)
        {
            return it->second.bSubs[8];
        }
        else if (bItemType == CItemData::ITEM_TYPE_PICK)
        {
            return it->second.bSubs[9];
        }
    }
    else if (bFilterType == FILTER_TYPE_COSTUME)
    {
        if (bItemSubType == CItemData::COSTUME_BODY)
        {
            return it->second.bSubs[1];
        }
        else if (bItemSubType == CItemData::COSTUME_HAIR)
        {
            return it->second.bSubs[2];
        }
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
        else if (bItemSubType == CItemData::COSTUME_WEAPON)
        {
            return it->second.bSubs[0];
        }
#endif
#ifdef ENABLE_ACCE_SYSTEM
        else if (bItemSubType == CItemData::COSTUME_ACCE)
        {
            return it->second.bSubs[3];
        }
#endif
#ifdef ENABLE_STOLE_COSTUME
        else if (bItemSubType == CItemData::COSTUME_STOLE)
        {
            return it->second.bSubs[3];
        }
#endif
#ifdef ENABLE_AURA_SYSTEM
        else if (bItemSubType == CItemData::COSTUME_AURA)
        {
            return it->second.bSubs[4];
        }
#endif

        return it->second.bSubs[5];
    }
    else if (bFilterType == FILTER_TYPE_DS)
    {
        if (IsCorDraconistItem(dwItemVnum))
        {
            return it->second.bSubs[0];
        }

        return it->second.bSubs[1];
    }
    else if (bFilterType == FILTER_TYPE_UNIQUE)
    {
        if (IsUniqueConsumablesItem(dwItemVnum))
        {
            return it->second.bSubs[0];
        }

        return it->second.bSubs[1];
    }
    else if (bFilterType == FILTER_TYPE_MATERIAL_AND_METIN)
    {
        if (IsUniqueConsumablesItem(dwItemVnum))
        {
            return it->second.bSubs[3];
        }
        else if (bItemType == CItemData::ITEM_TYPE_METIN)
        {
            return it->second.bSubs[1];
        }
        else if (bItemType == CItemData::ITEM_TYPE_MATERIAL && bItemSubType == CItemData::MATERIAL_LEATHER)
        {
            return it->second.bSubs[0];
        }

        return it->second.bSubs[2];
    }
    else if (bFilterType == FILTER_TYPE_POTION)
    {
        if (IsPotionConsumables(dwItemVnum))
        {
            return it->second.bSubs[0];
        }
        else if (IsHairDyesItem(dwItemVnum) == true)
        {
            return it->second.bSubs[1];
        }

        return it->second.bSubs[2];
    }
    else if (bFilterType == FILTER_TYPE_OTHERS2)
    {
        if (IsOreStones(dwItemVnum) == true)
        {
            return it->second.bSubs[1];
        }
        else if (bItemType == CItemData::ITEM_TYPE_USE && bItemSubType == CItemData::USE_BAIT)
        {
            return it->second.bSubs[0];
        }

        return it->second.bSubs[2];
    }
    else if (bFilterType == FILTER_TYPE_COMPANY)
    {
        if (bItemType == CItemData::ITEM_TYPE_COSTUME)
        {
#ifdef USE_MOUNT_COSTUME_SYSTEM
            if (bItemSubType == CItemData::COSTUME_MOUNT)
            {
                return it->second.bSubs[1];
            }
#endif

#ifdef ENABLE_PETSKIN
            if (bItemSubType == CItemData::COSTUME_PETSKIN)
            {
                return it->second.bSubs[1];
            }
#endif

#ifdef ENABLE_MOUNTSKIN
            if (bItemSubType == CItemData::COSTUME_MOUNTSKIN)
            {
                return it->second.bSubs[1];
            }
#endif
        }

        return it->second.bSubs[2];
    }
    else if (bFilterType == FILTER_TYPE_BOOK)
    {
        if (dwItemVnum != 50300)
        {
            return it->second.bSubs[0];
        }
    }
    else if (bFilterType == FILTER_TYPE_OTHERS3)
    {
        if (bItemType == CItemData::ITEM_TYPE_GIFTBOX)
        {
            return it->second.bSubs[0];
        }
        else if (IsMarriageItem(dwItemVnum))
        {
            return it->second.bSubs[1];
        }
        else if (IsParchmentItem(dwItemVnum))
        {
            return it->second.bSubs[2];
        }
        else if (IsGroupItem(dwItemVnum))
        {
            return it->second.bSubs[3];
        }
        else if (IsTransformItem(dwItemVnum))
        {
            return it->second.bSubs[4];
        }
        else if (bItemType == CItemData::ITEM_TYPE_USE && bItemSubType == CItemData::USE_TUNING)
        {
            return it->second.bSubs[5];
        }
#ifdef ENABLE_QUIVER_SYSTEM
        else if (bItemType == CItemData::ITEM_TYPE_ARMOR && (bItemSubType == CItemData::WEAPON_ARROW || bItemSubType == CItemData::WEAPON_QUIVER))
#else
        else if (bItemType == CItemData::ITEM_TYPE_ARMOR && bItemSubType == CItemData::WEAPON_ARROW)
#endif
        {
            return it->second.bSubs[6];
        }

        return it->second.bSubs[7];
    }

    return true;
}

void CPythonPlayer::SetFilterStatus(uint8_t bType, bool bFlag)
{
    if (bType >= FILTER_TYPE_MAX)
    {
        return;
    }

    if (bType == FILTER_TYPE_AUTO)
    {
        if (bFlag)
        {
            if (!m_PickupEventHandle)
            {
                static constexpr int EventsPerSecond = 4;
                static constexpr double SecondsPerEvent = 1.0 / EventsPerSecond;
                static Event::Event<>::EventFunc EventFunc = [](const auto& Info/*Unused Info*/)
                {
                    if (CPythonPlayer::Instance().GetFilterStatus(FILTER_TYPE_AUTO))
                    {
                        CPythonPlayer::Instance().PickAllCloseItems();
                    }

                    auto PickUpEvent = Event::Event<>::Make(EventFunc);
                    auto Handle = Event::AddEvent(SecondsPerEvent, PickUpEvent);
                    CPythonPlayer::Instance().SetPickUpEventHandle(Handle);
                };

                auto PickUpEvent = Event::Event<>::Make(EventFunc);
                m_PickupEventHandle = Event::AddEvent(SecondsPerEvent, PickUpEvent);
            }
        }
        else
        {
            if (m_PickupEventHandle)
            {
                Event::DeleteEvent(m_PickupEventHandle);
                m_PickupEventHandle = nullptr;
            }
        }
    }

    m_bFilterStatus[bType] = bFlag;
}

bool CPythonPlayer::GetFilterStatus(uint8_t bType) const
{
    if (bType >= FILTER_TYPE_MAX)
    {
        return false;
    }

    return m_bFilterStatus[bType];
}

void CPythonPlayer::SetPickUpEventHandle(const Event::EventHandle& Handle)
{
    m_PickupEventHandle = Handle;
}
#endif

CPythonPlayer::CPythonPlayer(void)
{
#ifdef USE_PICKUP_FILTER
    if (m_PickupEventHandle)
    {
        Event::DeleteEvent(m_PickupEventHandle);
    }

    m_PickupEventHandle = nullptr;
#endif



	SetMovableGroundDistance(40.0f);

	// AffectIndex To SkillIndex
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_JEONGWI), 3));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_GEOMGYEONG), 4));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_CHEONGEUN), 19));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_GYEONGGONG), 49));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_EUNHYEONG), 34));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_GONGPO), 64));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_JUMAGAP), 65));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_HOSIN), 94));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_BOHO), 95));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_KWAESOK), 110));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_GICHEON), 96));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_JEUNGRYEOK), 111));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_PABEOP), 66));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_FALLEN_CHEONGEUN), 19));
	/////
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_GWIGEOM), 63));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_MUYEONG), 78));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_HEUKSIN), 79));

	m_ppyGameWindow = NULL;

	m_sysIsCoolTime = TRUE;
	m_sysIsLevelLimit = TRUE;
	m_dwPlayTime = 0;

	m_aeMBFButton[MBT_LEFT]=CPythonPlayer::MBF_SMART;
	m_aeMBFButton[MBT_RIGHT]=CPythonPlayer::MBF_CAMERA;
	m_aeMBFButton[MBT_MIDDLE]=CPythonPlayer::MBF_CAMERA;

	memset(m_adwEffect, 0, sizeof(m_adwEffect));

	m_isDestPosition = FALSE;
	m_ixDestPos = 0;
	m_iyDestPos = 0;
	m_iLastAlarmTime = 0;
	Clear();
}

CPythonPlayer::~CPythonPlayer(void)
{
#ifdef USE_PICKUP_FILTER
    if (m_PickupEventHandle)
    {
        Event::DeleteEvent(m_PickupEventHandle);
    }

    m_PickupEventHandle = nullptr;
#endif

#ifdef USE_AUTO_HUNT
    ClearAutoAllSlot();
#endif
}
