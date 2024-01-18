#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "desc.h"
#include "desc_manager.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "battle.h"
#include "pvp.h"
#include "skill.h"
#include "start_position.h"
#include "profiler.h"
#include "cmd.h"
#include "dungeon.h"
#include "log.h"
#include "unique_item.h"
#include "priv_manager.h"
#include "db.h"
#include "vector.h"
#include "marriage.h"
#include "arena.h"
#include "regen.h"
#include "exchange.h"
#include "shop_manager.h"
#include "ani.h"
#include "BattleArena.h"
#include "packet.h"
#include "party.h"
#include "affect.h"
#include "guild.h"
#include "guild_manager.h"
#include "questmanager.h"
#include "questlua.h"
#ifdef __NEWPET_SYSTEM__
#include "New_PetSystem.h"
#endif
#include <random>
#include <algorithm>
#if defined(ENABLE_ULTIMATE_REGEN)
#include "new_mob_timer.h"
#endif

static DWORD __GetPartyExpNP(const DWORD level)
{
	if (!level || level > PLAYER_EXP_TABLE_MAX)
	{
		return 14000;
	}

	return party_exp_distribute_table[level];
}

static int __GetExpLossPerc(const DWORD level)
{
	if (!level || level > PLAYER_EXP_TABLE_MAX)
		return 1;
	return aiExpLossPercents[level];
}

DWORD AdjustExpByLevel(const LPCHARACTER ch, const DWORD exp)
{
	if (PLAYER_MAX_LEVEL_CONST < ch->GetLevel())
	{
		double ret = 0.95;
		double factor = 0.1;

		for (int32_t i=0 ; i < ch->GetLevel()-100 ; ++i)
		{
			if ( (i%10) == 0)
				factor /= 2.0;

			ret *= 1.0 - factor;
		}

		ret = ret * static_cast<double>(exp);

		if (ret < 1.0)
			return 1;

		return static_cast<DWORD>(ret);
	}

	return exp;
}

bool CHARACTER::CanBeginFight() const
{
	if (!CanMove())
		return false;

	return m_pointsInstant.position == POS_STANDING && !IsDead() && !IsStun();
}

void CHARACTER::BeginFight(LPCHARACTER pkVictim)
{
	SetVictim(pkVictim);
	SetNowWalking(false);
	SetPosition(POS_FIGHTING);
	SetNextStatePulse(1);
}

bool CHARACTER::CanFight() const
{
	return m_pointsInstant.position >= POS_FIGHTING ? true : false;
}

void CHARACTER::CreateFly(BYTE bType, LPCHARACTER pkVictim)
{
	TPacketGCCreateFly packFly;

	packFly.bHeader         = HEADER_GC_CREATE_FLY;
	packFly.bType           = bType;
	packFly.dwStartVID      = GetVID();
	packFly.dwEndVID        = pkVictim->GetVID();

	PacketAround(&packFly, sizeof(TPacketGCCreateFly));
}

void CHARACTER::DistributeSP(LPCHARACTER pkKiller, int iMethod)
{
	if (pkKiller->GetSP() >= pkKiller->GetMaxSP())
		return;

	bool bAttacking = (get_dword_time() - GetLastAttackTime()) < 3000;
	bool bMoving = (get_dword_time() - GetLastMoveTime()) < 3000;

	if (iMethod == 1)
	{
		int num = number(0, 3);

		if (!num)
		{
			int iLvDelta = GetLevel() - pkKiller->GetLevel();
			int iAmount = 0;

			if (iLvDelta >= 5)
				iAmount = 10;
			else if (iLvDelta >= 0)
				iAmount = 6;
			else if (iLvDelta >= -3)
				iAmount = 2;

			if (iAmount != 0)
			{
				iAmount += (iAmount * pkKiller->GetPoint(POINT_SP_REGEN)) / 100;

				if (iAmount >= 11)
					CreateFly(FLY_SP_BIG, pkKiller);
				else if (iAmount >= 7)
					CreateFly(FLY_SP_MEDIUM, pkKiller);
				else
					CreateFly(FLY_SP_SMALL, pkKiller);

				pkKiller->PointChange(POINT_SP, iAmount);
			}
		}
	}
	else
	{
		if (pkKiller->GetJob() == JOB_SHAMAN || (pkKiller->GetJob() == JOB_SURA && pkKiller->GetSkillGroup() == 2))
		{
			int iAmount;

			if (bAttacking)
				iAmount = 2 + GetMaxSP() / 100;
			else if (bMoving)
				iAmount = 3 + GetMaxSP() * 2 / 100;
			else
				iAmount = 10 + GetMaxSP() * 3 / 100; // 평상시

			iAmount += (iAmount * pkKiller->GetPoint(POINT_SP_REGEN)) / 100;
			pkKiller->PointChange(POINT_SP, iAmount);
		}
		else
		{
			int iAmount;

			if (bAttacking)
				iAmount = 2 + pkKiller->GetMaxSP() / 200;
			else if (bMoving)
				iAmount = 2 + pkKiller->GetMaxSP() / 100;
			else
			{
				// 평상시
				if (pkKiller->GetHP() < pkKiller->GetMaxHP())
					iAmount = 2 + (pkKiller->GetMaxSP() / 100); // 피 다 안찼을때
				else
					iAmount = 9 + (pkKiller->GetMaxSP() / 100); // 기본
			}

			iAmount += (iAmount * pkKiller->GetPoint(POINT_SP_REGEN)) / 100;
			pkKiller->PointChange(POINT_SP, iAmount);
		}
	}
}


bool CHARACTER::Attack(LPCHARACTER pkVictim, BYTE bType)
{
	if (test_server) {
		sys_log(0, "[TEST_SERVER] Attack : %s type %d, MobBattleType %d", GetName(), bType, !GetMobBattleType() ? 0 : GetMobAttackRange());
	}

	if (pkVictim == nullptr)
		return false;

	if (pkVictim->GetMyShop()) {
		return false;
	}

	if (!CanMove()
#if defined(ENABLE_NEW_ANTICHEAT_RULES)
 || IsObserverMode()
#endif
	) {
		return false;
	}

//#if defined(ENABLE_NEW_ANTICHEAT_RULES)
//	if (GetSectree() && pkVictim->GetSectree()) {
//		if (GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK) ||
//			pkVictim->GetSectree()->IsAttr(pkVictim->GetX(), pkVictim->GetY(), ATTR_BANPK)) {
//			const LPDESC d = GetDesc();
//			if (d) {
//				LogManager::instance().HackLog("ATTACK_IN_SAFEZONE_HACK", this);
//				d->DelayedDisconnect(3);
//
//				return false;
//			}
//		}
//	}
//#endif

	if (!battle_is_attackable(this, pkVictim)) {
		return false;
	}

	DWORD dwCurrentTime = get_dword_time();

	if (IsPC()) {
#if defined(ENABLE_NEW_ANTICHEAT_RULES)
		if (IS_SPEED_HACK(this, pkVictim, dwCurrentTime)) {
			return false;
		}
#endif

		if (bType == 0 && dwCurrentTime < GetSkipComboAttackByTime()) {
			return false;
		}

#ifdef __FIX_PRO_DAMAGE__
		if (bType == 0 && CheckSyncPosition()) {
			return false;
		}
#endif
	} else {
#ifdef USE_CAPTCHA_SYSTEM
        if (pkVictim->IsPC() &&
            pkVictim->IsWaitingForCaptcha())
        {
            return false;
        }
#endif
	}

#if !defined(ENABLE_NEW_ANTICHEAT_RULES)
	pkVictim->SetSyncOwner(this);

	if (pkVictim->CanBeginFight()) {
		pkVictim->BeginFight(this);
	}
#endif

	int32_t iRet;

	if (bType == 0)
	{
		switch (GetMobBattleType()) {
			case BATTLE_TYPE_MELEE:
			case BATTLE_TYPE_POWER:
			case BATTLE_TYPE_TANKER:
			case BATTLE_TYPE_SUPER_POWER:
			case BATTLE_TYPE_SUPER_TANKER: {
#if defined(ENABLE_NEW_ANTICHEAT_RULES)
				if (IsPC() && pkVictim && battle_distance_valid(this, pkVictim) == false) {
/* maybe false report
					SEND_REPORT_HACK(this, ANTICHEAT_TYPE_RANGE);
*/

					if (pkVictim->GetRaceNum() != 6118) {
						return false;
					}
				}
#endif

				iRet = battle_melee_attack(this, pkVictim);
				break;
			}
			case BATTLE_TYPE_RANGE: {
				FlyTarget(pkVictim->GetVID(), pkVictim->GetX(), pkVictim->GetY(), HEADER_CG_FLY_TARGETING);
				iRet = Shoot(0) ? BATTLE_DAMAGE : BATTLE_NONE;
				break;
			}
			case BATTLE_TYPE_MAGIC: {
				FlyTarget(pkVictim->GetVID(), pkVictim->GetX(), pkVictim->GetY(), HEADER_CG_FLY_TARGETING);
				iRet = Shoot(1) ? BATTLE_DAMAGE : BATTLE_NONE;
				break;
			}
			default: {
				sys_err("Unhandled battle type %d", GetMobBattleType());
				iRet = BATTLE_NONE;
				break;
			}
		}
	}
	else
	{
		if (IsPC() == true)
		{
			if (dwCurrentTime - m_dwLastSkillTime > 1500)
			{
				sys_log(1, "HACK: Too long skill using term. Name(%s) PID(%u) delta(%u)",
						GetName(), GetPlayerID(), (dwCurrentTime - m_dwLastSkillTime));
				return false;
			}
		}

		sys_log(1, "Attack call ComputeSkill %d %s", bType, pkVictim?pkVictim->GetName():"");
		iRet = ComputeSkill(bType, pkVictim);
	}

#if defined(ENABLE_NEW_ANTICHEAT_RULES)
	if (iRet != BATTLE_NONE)
	{
		pkVictim->SetSyncOwner(this);
		if (pkVictim->CanBeginFight())
			pkVictim->BeginFight(this);
	}
#endif

	if (iRet == BATTLE_DAMAGE || iRet == BATTLE_DEAD)
	{
		OnMove(true);
		pkVictim->OnMove();

		if (BATTLE_DEAD == iRet && IsPC()) {
			SetVictim(NULL);
		}

		return true;
	}

	return false;
}

void CHARACTER::DeathPenalty(BYTE bTown)
{
	sys_log(1, "DEATH_PERNALY_CHECK(%s) town(%d)", GetName(), bTown);

	Cube_close(this);
#ifdef __ATTR_TRANSFER_SYSTEM__
	AttrTransfer_close(this);
#endif
#ifdef ENABLE_ACCE_SYSTEM
	CloseAcce();
#endif
#if defined(ENABLE_CHANGELOOK)
	ChangeLookWindow(false, true);
#endif

	if (CBattleArena::instance().IsBattleArenaMap(GetMapIndex()) == true)
	{
		return;
	}

	if (GetLevel() < 10) {
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 412, "");
#endif
		return;
	}

	if (number(0, 2) == 1) {
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 412, "");
#endif
		return;
	}

#if defined(ENABLE_RESTRICT_STAFF_RESTRICTIONS)
	if (GetGMLevel() > GM_PLAYER) {
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 412, "");
#endif
		return;
	}
#endif

	if (IS_SET(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY))
	{
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);

		// NO_DEATH_PENALTY_BUG_FIX
		if (!bTown) // 국제 버전에서는 제자리 부활시만 용신의 가호를 사용한다. (마을 복귀시는 경험치 패널티 없음)
		{
			if (FindAffect(AFFECT_NO_DEATH_PENALTY))
			{
#if defined(ENABLE_TEXTS_RENEWAL)
				ChatPacketNew(CHAT_TYPE_INFO, 384, "");
#endif
				RemoveAffect(AFFECT_NO_DEATH_PENALTY);
				return;
			}
		}
		// END_OF_NO_DEATH_PENALTY_BUG_FIX

		int iLoss = ((GetNextExp() * __GetExpLossPerc(GetLevel())) / 100);

		iLoss = MIN(800000, iLoss);

		if (bTown)
			iLoss = 0;

		if (IsEquipUniqueItem(UNIQUE_ITEM_TEARDROP_OF_GODNESS))
			iLoss /= 2;

		sys_log(0, "DEATH_PENALTY(%s) EXP_LOSS: %d percent %d%%", GetName(), iLoss, __GetExpLossPerc(GetLevel()));

		PointChange(POINT_EXP, -iLoss, true);
	}
}

bool CHARACTER::IsStun() const
{
	if (IS_SET(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN))
		return true;

	return false;
}

EVENTFUNC(StunEvent)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "StunEvent> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}
	ch->m_pkStunEvent = NULL;
	ch->Dead();
	return 0;
}

void CHARACTER::Stun()
{
	if (IsStun())
		return;

	if (IsDead())
		return;

	if (!IsPC() && m_pkParty)
	{
		m_pkParty->SendMessage(this, PM_ATTACKED_BY, 0, 0);
	}

	sys_log(1, "%s: Stun %p", GetName(), this);

	PointChange(POINT_HP_RECOVERY, -GetPoint(POINT_HP_RECOVERY));
	PointChange(POINT_SP_RECOVERY, -GetPoint(POINT_SP_RECOVERY));

	CloseMyShop();

	event_cancel(&m_pkRecoveryEvent); // 회복 이벤트를 죽인다.

	TPacketGCStun pack;
	pack.header	= HEADER_GC_STUN;
	pack.vid	= m_vid;
	PacketAround(&pack, sizeof(pack));

	SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);

	if (m_pkStunEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkStunEvent = event_create(StunEvent, info, PASSES_PER_SEC(3));
}

EVENTINFO(SCharDeadEventInfo)
{
	bool isPC;
	uint32_t dwID;

	SCharDeadEventInfo()
	: isPC(0)
	, dwID(0)
	{
	}
};

EVENTFUNC(dead_event)
{
	const SCharDeadEventInfo* info = dynamic_cast<SCharDeadEventInfo*>(event->info);
	if (info == NULL)
	{
		sys_err( "dead_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = NULL;

	if (true == info->isPC)
	{
		ch = CHARACTER_MANAGER::instance().FindByPID( info->dwID );
	}
	else
	{
		ch = CHARACTER_MANAGER::instance().Find( info->dwID );
	}

	if (NULL == ch)
	{
		sys_err("DEAD_EVENT: cannot find char pointer with %s id(%d)", info->isPC ? "PC" : "MOB", info->dwID );
		return 0;
	}

	ch->m_pkDeadEvent = NULL;

	if (ch->GetDesc())
	{
		ch->GetDesc()->SetPhase(PHASE_GAME);

		ch->SetPosition(POS_STANDING);

#ifdef USE_AUTO_HUNT
        if (ch->GetAutoUseType(AUTO_ONOFF_START) && ch->GetAutoUseType(AUTO_ONOFF_RESTART))
        {
            if (ch->GetPremiumRemainSeconds(PREMIUM_AUTO_USE) > 0)
            {
                ch->RestartAtSamePos();
                ch->PointChange(POINT_HP, (ch->GetMaxHP() / 2) - ch->GetHP(), true);
                ch->DeathPenalty(0);
                ch->StartRecoveryEvent();
                ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
                return 0;
            }
        }
#endif

		PIXEL_POSITION pos;

		if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos))
			ch->WarpSet(pos.x, pos.y);
		else
		{
			sys_err("cannot find spawn position (name %s)", ch->GetName());
			ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
		}

		ch->PointChange(POINT_HP, (ch->GetMaxHP() / 2) - ch->GetHP(), true);

		ch->DeathPenalty(0);

		ch->StartRecoveryEvent();

		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
	}
	else
	{
		if (ch->IsMonster() == true)
		{
			if (ch->IsRevive() == false && ch->HasReviverInParty() == true)
			{
				ch->SetPosition(POS_STANDING);
				ch->SetHP(ch->GetMaxHP());

				ch->ViewReencode();

				ch->SetAggressive();
				ch->SetRevive(true);

				return 0;
			}
		}

		M2_DESTROY_CHARACTER(ch);
	}

	return 0;
}

bool CHARACTER::IsDead() const
{
	if (m_pointsInstant.position == POS_DEAD)
		return true;

	return false;
}

void CHARACTER::RewardGold(LPCHARACTER pkAttacker) {
	if (pkAttacker && pkAttacker->IsPC()) {
#ifdef ENABLE_BLOCK_MULTIFARM
		if (pkAttacker->FindAffect(AFFECT_DROP_BLOCK, APPLY_NONE)) {
			return;
		}
#endif

		if (IsStone()) {
			bool drop = true;
			int mylvl = pkAttacker->GetLevel(), targetlvl = GetLevel();
			if (mylvl > targetlvl) {
				drop = mylvl-targetlvl <= 15 ? true : false;
			}

			if (drop) {
/*
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
				if (GetMapIndex() < 10000) {
					if (CHARACTER_MANAGER::Instance().CheckEventIsActive(HALLOWEEN_EVENT) != NULL) {
						int32_t stoneLevel = GetLevel();
						uint8_t awardChance = 0, awardSkul = 0;

						if (stoneLevel >= 105) {
							awardChance = 15;
							awardSkul = 3;
						} else if (stoneLevel >= 90) {
							awardChance = 15;
							awardSkul = 2;
						} else if (stoneLevel >= 70) {
							awardChance = 15;
							awardSkul = 1;
						}

						if (awardSkul != 0 && awardChance >= number(1, 100)) {
							pkAttacker->PointChange(POINT_SKULL, awardSkul, true);
						}
					}
				}
#endif
*/

				uint32_t min = GetMobTable().dwGoldMin, max = GetMobTable().dwGoldMax;

				if (min == 0 || max == 0) {
					return;
				}

				if (min > max) {
					min = max - 1;
				}

				int gold = number(min, max);

				if (gold <= 0) {
					return;
				}

#if defined(ENABLE_EVENT_MANAGER)
				const auto event = CHARACTER_MANAGER::Instance().CheckEventIsActive(YANG_DROP_EVENT, pkAttacker->GetEmpire());
				if (event != NULL) {
					gold += (gold / 100) * event->value[0];
				}
#endif

				if (pkAttacker->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0 ||
					pkAttacker->IsEquipUniqueGroup(UNIQUE_GROUP_LUCKY_GOLD)) {
						gold += gold;
				}

				int32_t iDropDoubleYang = MIN(100, pkAttacker->GetPoint(POINT_GOLD_DOUBLE_BONUS));
				if (number(1, 100) <= iDropDoubleYang) {
					gold += gold;
				}

				if (gold <= 0) {
					return;
				}

#if defined(ENABLE_NEW_GOLD_LIMIT)
				pkAttacker->ChangeGold(gold);
#else
				pkAttacker->PointChange(POINT_GOLD, gold, true);
#endif
			}
		} else {
			// ADD_PREMIUM
			bool isAutoLoot =
				(pkAttacker->GetPremiumRemainSeconds(PREMIUM_AUTOLOOT) > 0 ||
				 pkAttacker->IsEquipUniqueGroup(UNIQUE_GROUP_AUTOLOOT))
				? true : false; // 제3의 손
			// END_OF_ADD_PREMIUM

			PIXEL_POSITION pos;

			if (!isAutoLoot)
				if (!SECTREE_MANAGER::instance().GetMovablePosition(GetMapIndex(), GetX(), GetY(), pos))
					return;

			int iTotalGold = 0;
			//
			// --------- 돈 드롭 확률 계산 ----------
			//
			int iGoldPercent = MobRankStats[GetMobRank()].iGoldPercent;

#ifdef ENABLE_EVENT_MANAGER
			if (pkAttacker->IsPC())
			{
				const auto event = CHARACTER_MANAGER::Instance().CheckEventIsActive(YANG_DROP_EVENT, pkAttacker->GetEmpire());
				if(event != NULL)
					iGoldPercent = iGoldPercent * (100 + (event->value[0]+CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD_DROP))) / 100;
				else
					iGoldPercent = iGoldPercent * (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD_DROP)) / 100;
			}
#else
			if (pkAttacker->IsPC())
				iGoldPercent = iGoldPercent * (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD_DROP)) / 100;
#endif

			if (pkAttacker->GetPoint(POINT_MALL_GOLDBONUS))
				iGoldPercent += (iGoldPercent * pkAttacker->GetPoint(POINT_MALL_GOLDBONUS) / 100);

			iGoldPercent = iGoldPercent * CHARACTER_MANAGER::instance().GetMobGoldDropRate(pkAttacker) / 100;

			// ADD_PREMIUM
			if (pkAttacker->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0 ||
				pkAttacker->IsEquipUniqueGroup(UNIQUE_GROUP_LUCKY_GOLD)) {
					iGoldPercent += iGoldPercent;
			}
			// END_OF_ADD_PREMIUM

			if (iGoldPercent > 100)
				iGoldPercent = 100;

			int iPercent;

			if (GetMobRank() >= MOB_RANK_BOSS)
				iPercent = ((iGoldPercent * PERCENT_LVDELTA_BOSS(pkAttacker->GetLevel(), GetLevel())) / 100);
			else
				iPercent = ((iGoldPercent * PERCENT_LVDELTA(pkAttacker->GetLevel(), GetLevel())) / 100);
			//int iPercent = CALCULATE_VALUE_LVDELTA(pkAttacker->GetLevel(), GetLevel(), iGoldPercent);

			if (number(1, 100) > iPercent)
				return;

			int iGoldMultipler = 1;

			if (1 == number(1, 50000)) // 1/50000 확률로 돈이 10배
				iGoldMultipler *= 10;
			else if (1 == number(1, 10000)) // 1/10000 확률로 돈이 5배
				iGoldMultipler *= 5;

			// 개인 적용
			if (pkAttacker->GetPoint(POINT_GOLD_DOUBLE_BONUS))
				if (number(1, 100) <= pkAttacker->GetPoint(POINT_GOLD_DOUBLE_BONUS))
					iGoldMultipler *= 2;

			//
			// --------- 돈 드롭 배수 결정 ----------
			//
			if (test_server)
				pkAttacker->ChatPacket(CHAT_TYPE_PARTY, "gold_mul %d rate %d", iGoldMultipler, CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker));

			//
			// --------- 실제 드롭 처리 -------------
			//
			LPITEM item;

			int iGold10DropPct = 100;
#ifdef ENABLE_EVENT_MANAGER
			const auto event = CHARACTER_MANAGER::Instance().CheckEventIsActive(YANG_DROP_EVENT, pkAttacker->GetEmpire());
			if(event != NULL)
				iGold10DropPct = (iGold10DropPct * 100) / (100 + event->value[0] + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD10_DROP));
			else
				iGold10DropPct = (iGold10DropPct * 100) / (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD10_DROP));
#else
			iGold10DropPct = (iGold10DropPct * 100) / (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD10_DROP));
#endif

			// MOB_RANK가 BOSS보다 높으면 무조건 돈폭탄
			if (GetMobRank() >= MOB_RANK_BOSS && !IsStone() && GetMobTable().dwGoldMax != 0)
			{
				if (1 == number(1, iGold10DropPct))
					iGoldMultipler *= 10; // 1% 확률로 돈 10배

				int iSplitCount = number(25, 35);

				for (int i = 0; i < iSplitCount; ++i)
				{
					int iGold = number(GetMobTable().dwGoldMin, GetMobTable().dwGoldMax) / iSplitCount;
					if (test_server)
						sys_log(0, "iGold %d", iGold);
					iGold = iGold * CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker) / 100;
					iGold *= iGoldMultipler;

					if (iGold == 0)
					{
						continue ;
					}

					if (test_server)
					{
						sys_log(0, "Drop Moeny MobGoldAmountRate %d %d", CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker), iGoldMultipler);
						sys_log(0, "Drop Money gold %d GoldMin %d GoldMax %d", iGold, GetMobTable().dwGoldMax, GetMobTable().dwGoldMax);
					}

					pkAttacker->GiveGold(iGold / iSplitCount);
					iTotalGold += iGold;
				}
			}
			else
			{
				int iGold = number(GetMobTable().dwGoldMin, GetMobTable().dwGoldMax);
				iGold = iGold * CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker) / 100;
				iGold *= iGoldMultipler;

				int iSplitCount;

				if (iGold >= 3)
					iSplitCount = number(1, 3);
				else if (GetMobRank() >= MOB_RANK_BOSS)
				{
					iSplitCount = number(3, 10);

					if ((iGold / iSplitCount) == 0)
						iSplitCount = 1;
				}
				else
					iSplitCount = 1;

				if (iGold != 0)
				{
					iTotalGold += iGold; // Total gold

					for (int i = 0; i < iSplitCount; ++i)
					{
						pkAttacker->GiveGold(iGold / iSplitCount);
#if defined(USE_BATTLEPASS)
						pkAttacker->UpdateExtBattlePassMissionProgress(YANG_COLLECT, iGold / iSplitCount, pkAttacker->GetMapIndex());
#endif
					}
				}
			}
		}

//		DBManager::instance().SendMoneyLog(MONEY_LOG_MONSTER, GetRaceNum(), iTotalGold);
	}
}

////template <typename T>
////CHARACTER* GetRandomCharacterFromWeightedTable(const std::vector<std::pair<CHARACTER*, T>>& table, const T& total) {
////	if (total <= 0)
////		return nullptr;
////
////	auto gen = std::default_random_engine(std::random_device()());
////	auto dis = std::uniform_int_distribution<T>(0, total);
////
////	auto n = dis(gen);
////	for (auto it = std::begin(table); it != std::end(table); ++it) {
////		if ((*it).second > n)
////			return (*it).first;
////
////		n -= (*it).second;
////	}
////
////	return nullptr;
////}

void CHARACTER::Reward(bool bItemDrop)
{
	//PROF_UNIT puReward("Reward");
	LPCHARACTER pkAttacker = DistributeExp();

	if (!pkAttacker)
		return;

	//PROF_UNIT pu1("r1");
	if (pkAttacker->IsPC())
	{
		if ((GetLevel() - pkAttacker->GetLevel()) >= -10)
		{
			if (pkAttacker->GetRealAlignment() < 0)
			{
				if (pkAttacker->IsEquipUniqueItem(UNIQUE_ITEM_FASTER_ALIGNMENT_UP_BY_KILL))
					pkAttacker->UpdateAlignment(14);
				else
					pkAttacker->UpdateAlignment(7);
			}
			else
				pkAttacker->UpdateAlignment(2);
		}

		pkAttacker->SetQuestNPCID(GetVID());
		quest::CQuestManager::instance().Kill(pkAttacker->GetPlayerID(), GetRaceNum());
		CHARACTER_MANAGER::instance().KillLog(GetRaceNum());
#if defined(USE_BATTLEPASS)
		pkAttacker->UpdateExtBattlePassMissionProgress(KILL_MONSTER, 1, GetRaceNum());
#endif

		if (!number(0, 9))
		{
			if (pkAttacker->GetPoint(POINT_KILL_HP_RECOVERY))
			{
				int iHP = pkAttacker->GetMaxHP() * pkAttacker->GetPoint(POINT_KILL_HP_RECOVERY) / 100;
				pkAttacker->PointChange(POINT_HP, iHP);
				CreateFly(FLY_HP_SMALL, pkAttacker);
			}

			if (pkAttacker->GetPoint(POINT_KILL_SP_RECOVER))
			{
				int iSP = pkAttacker->GetMaxSP() * pkAttacker->GetPoint(POINT_KILL_SP_RECOVER) / 100;
				pkAttacker->PointChange(POINT_SP, iSP);
				CreateFly(FLY_SP_SMALL, pkAttacker);
			}
		}
	}
	//pu1.Pop();
	
#ifdef ENABLE_BLOCK_MULTIFARM
	if (pkAttacker->FindAffect(AFFECT_DROP_BLOCK, APPLY_NONE)) {
		return;
	}
#endif
	
	if (!bItemDrop)
		return;

	PIXEL_POSITION pos = GetXYZ();

	if (!SECTREE_MANAGER::instance().GetMovablePosition(GetMapIndex(), pos.x, pos.y, pos))
		return;

	//
	// 돈 드롭
	//
	//PROF_UNIT pu2("r2");
	if (test_server)
		sys_log(0, "Drop money : Attacker %s", pkAttacker->GetName());
	RewardGold(pkAttacker);
	//pu2.Pop();

	//
	// 아이템 드롭
	//
	//PROF_UNIT pu3("r3");
	LPITEM item;

	static std::vector<LPITEM> s_vec_item;
	s_vec_item.clear();
	size_t item_rest_size = ITEM_MANAGER::instance().CreateDropItem(this, pkAttacker, s_vec_item);

	if (item_rest_size)
	{
		if (s_vec_item.size() == 0) {
			
		}
		else if (s_vec_item.size() == 1)
		{
			item = s_vec_item[0];
#ifdef USE_PICKUP_FILTER
            if (pkAttacker->GetFilterStatus(FILTER_TYPE_AUTO) && pkAttacker->CanPickItem(item))
            {
                pkAttacker->AutoGiveItem(item, false
#ifdef __HIGHLIGHT_SYSTEM__
, true
#endif
, true
);
            }
            else
            {
                item->AddToGround(GetMapIndex(), pos);

                if (CBattleArena::instance().IsBattleArenaMap(pkAttacker->GetMapIndex()) == false)
                {
                    item->SetOwnership(pkAttacker);
                }

                item->StartDestroyEvent();

                pos.x = number(-7, 7) * 20;
                pos.y = number(-7, 7) * 20;
                pos.x += GetX();
                pos.y += GetY();
            }
#else
			item->AddToGround(GetMapIndex(), pos);

			if (CBattleArena::instance().IsBattleArenaMap(pkAttacker->GetMapIndex()) == false)
			{
				item->SetOwnership(pkAttacker);
			}

			item->StartDestroyEvent();

			pos.x = number(-7, 7) * 20;
			pos.y = number(-7, 7) * 20;
			pos.x += GetX();
			pos.y += GetY();
#endif
		}
		else
		{
			int32_t iItemIdx = s_vec_item.size() - 1;

			std::priority_queue<std::pair<int, LPCHARACTER> > pq;

			int32_t total_dam = 0;

			for (auto it = m_map_kDamage.begin(); it != m_map_kDamage.end(); ++it)
			{
				int32_t iDamage = it->second.iTotalDamage;
				if (iDamage > 0)
				{
					LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(it->first);
					if (ch)
					{
						pq.push(std::make_pair(iDamage, ch));
						total_dam += iDamage;
					}
				}
			}

			std::vector<LPCHARACTER> v;

			while (!pq.empty() && pq.top().first * 10 >= total_dam)
			{
				v.push_back(pq.top().second);
				pq.pop();
			}

			if (v.empty())
			{
				while (iItemIdx >= 0)
				{
					item = s_vec_item[iItemIdx--];

					if (!item)
					{
						sys_err("item null in vector idx %d", iItemIdx + 1);
						continue;
					}

					item->AddToGround(GetMapIndex(), pos);
					item->StartDestroyEvent();

					pos.x = number(-7, 7) * 20;
					pos.y = number(-7, 7) * 20;
					pos.x += GetX();
					pos.y += GetY();
				}
			}
			else
			{
				std::vector<LPCHARACTER>::iterator it = v.begin();

				while (iItemIdx >= 0)
				{
					item = s_vec_item[iItemIdx--];

					if (!item)
					{
						sys_err("item null in vector idx %d", iItemIdx + 1);
						continue;
					}

					LPCHARACTER ch = *it;

					if (ch->GetParty())
					{
						ch = ch->GetParty()->GetNextOwnership(ch, GetX(), GetY());
					}

					if (!CBattleArena::instance().IsBattleArenaMap(ch->GetMapIndex()))
					{
						item->SetOwnership(ch);
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

                        ++it;

                        if (it == v.end())
                        {
                            it = v.begin();
                        }

                        continue;
                    }
#endif

					item->AddToGround(GetMapIndex(), pos);
					item->StartDestroyEvent();

					pos.x = number(-7, 7) * 20;
					pos.y = number(-7, 7) * 20;
					pos.x += GetX();
					pos.y += GetY();

					++it;

					if (it == v.end())
						it = v.begin();
				}
			}
		}
	}

	m_map_kDamage.clear();
}

struct TItemDropPenalty
{
	int iInventoryPct;		// Range: 1 ~ 1000
	int iInventoryQty;		// Range: --
	int iEquipmentPct;		// Range: 1 ~ 100
	int iEquipmentQty;		// Range: --
};

TItemDropPenalty aItemDropPenalty_kor[9] =
{
	{   0,   0,  0,  0 },	// 선왕
	{   0,   0,  0,  0 },	// 영웅
	{   0,   0,  0,  0 },	// 성자
	{   0,   0,  0,  0 },	// 지인
	{   0,   0,  0,  0 },	// 양민
	{  25,   1,  5,  1 },	// 낭인
	{  50,   2, 10,  1 },	// 악인
	{  75,   4, 15,  1 },	// 마두
	{ 100,   8, 20,  1 },	// 패왕
};

void CHARACTER::ItemDropPenalty(LPCHARACTER pkKiller)
{
#if defined(ENABLE_RESTRICT_STAFF_RESTRICTIONS)
	if (GetGMLevel() > GM_PLAYER)
	{
		return;
	}
#endif

	if (GetMyShop())
	{
		return;
	}

	if (GetLevel() < 50)
	{
		return;
	}

	const auto qc = quest::CQuestManager::instance().GetPCForce(GetPlayerID());
	if (!qc)
	{
		return;
	}

	if (qc->IsRunning())
	{
		return;
	}

	if (CBattleArena::instance().IsBattleArenaMap(GetMapIndex()) == true)
	{
		return;
	}

	struct TItemDropPenalty * table = &aItemDropPenalty_kor[0];

	int iAlignIndex;

	if (GetRealAlignment() >= 120000)
		iAlignIndex = 0;
	else if (GetRealAlignment() >= 80000)
		iAlignIndex = 1;
	else if (GetRealAlignment() >= 40000)
		iAlignIndex = 2;
	else if (GetRealAlignment() >= 10000)
		iAlignIndex = 3;
	else if (GetRealAlignment() >= 0)
		iAlignIndex = 4;
	else if (GetRealAlignment() > -40000)
		iAlignIndex = 5;
	else if (GetRealAlignment() > -80000)
		iAlignIndex = 6;
	else if (GetRealAlignment() > -120000)
		iAlignIndex = 7;
	else
		iAlignIndex = 8;

	std::vector<std::pair<LPITEM, int> > vec_item;
	LPITEM pkItem;
	int	i;
	bool isDropAllEquipments = false;

	TItemDropPenalty & r = table[iAlignIndex];
	sys_log(0, "%s align %d inven_pct %d equip_pct %d", GetName(), iAlignIndex, r.iInventoryPct, r.iEquipmentPct);

	bool bDropInventory = r.iInventoryPct >= number(1, 1000);
	bool bDropEquipment = r.iEquipmentPct >= number(1, 100);
	bool bDropAntiDropUniqueItem = false;

	if ((bDropInventory || bDropEquipment) && IsEquipUniqueItem(UNIQUE_ITEM_SKIP_ITEM_DROP_PENALTY))
	{
		bDropInventory = false;
		bDropEquipment = false;
		bDropAntiDropUniqueItem = true;
	}

	if (bDropInventory) // Drop Inventory
	{
		std::vector<uint16_t> vec_bSlots;

		for (i = 0; i < INVENTORY_MAX_NUM; ++i)
			if (GetInventoryItem(i))
				vec_bSlots.push_back(i);

		if (!vec_bSlots.empty())
		{
			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(vec_bSlots.begin(), vec_bSlots.end(), g);
			int iQty = MIN(vec_bSlots.size(), r.iInventoryQty);

			if (iQty)
				iQty = number(1, iQty);

			for (i = 0; i < iQty; ++i)
			{
				pkItem = GetInventoryItem(vec_bSlots[i]);

				if (IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_PKDROP))
					continue;

				SyncQuickslot(QUICKSLOT_TYPE_ITEM, vec_bSlots[i], 65535);
				vec_item.push_back(std::make_pair(pkItem->RemoveFromCharacter(), INVENTORY));
			}
		}

		else if (iAlignIndex == 8)
			isDropAllEquipments = true;
	}

	if (bDropEquipment) // Drop Equipment
	{
		std::vector<uint16_t> vec_bSlots;

		for (i = 0; i < WEAR_MAX_NUM; ++i)
			if (GetWear(i))
				vec_bSlots.push_back(i);

		if (!vec_bSlots.empty())
		{
			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(vec_bSlots.begin(), vec_bSlots.end(), g);
			int iQty;

			if (isDropAllEquipments)
				iQty = vec_bSlots.size();
			else
				iQty = MIN(vec_bSlots.size(), number(1, r.iEquipmentQty));

			if (iQty)
				iQty = number(1, iQty);

			for (i = 0; i < iQty; ++i)
			{
				pkItem = GetWear(vec_bSlots[i]);

				if (IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_PKDROP))
					continue;

				SyncQuickslot(QUICKSLOT_TYPE_ITEM, vec_bSlots[i], 65535);
				vec_item.push_back(std::make_pair(pkItem->RemoveFromCharacter(), EQUIPMENT));
			}
		}
	}

	if (bDropAntiDropUniqueItem)
	{
		LPITEM pkItem;

		pkItem = GetWear(WEAR_UNIQUE1);

		if (pkItem && pkItem->GetVnum() == UNIQUE_ITEM_SKIP_ITEM_DROP_PENALTY)
		{
			SyncQuickslot(QUICKSLOT_TYPE_ITEM, WEAR_UNIQUE1, 65535);
			vec_item.push_back(std::make_pair(pkItem->RemoveFromCharacter(), EQUIPMENT));
		}

		pkItem = GetWear(WEAR_UNIQUE2);

		if (pkItem && pkItem->GetVnum() == UNIQUE_ITEM_SKIP_ITEM_DROP_PENALTY)
		{
			SyncQuickslot(QUICKSLOT_TYPE_ITEM, WEAR_UNIQUE2, 65535);
			vec_item.push_back(std::make_pair(pkItem->RemoveFromCharacter(), EQUIPMENT));
		}
	}

	{
		PIXEL_POSITION pos;
		pos.x = GetX();
		pos.y = GetY();

		unsigned int i;

		for (i = 0; i < vec_item.size(); ++i)
		{
			LPITEM item = vec_item[i].first;
			int window = vec_item[i].second;

			item->AddToGround(GetMapIndex(), pos);
			item->StartDestroyEvent();

			sys_log(0, "DROP_ITEM_PK: %s %d %d from %s", item->GetName(), pos.x, pos.y, GetName());
			LogManager::instance().ItemLog(this, item, "DEAD_DROP", (window == INVENTORY) ? "INVENTORY" : ((window == EQUIPMENT) ? "EQUIPMENT" : ""));

			pos.x = GetX() + number(-7, 7) * 20;
			pos.y = GetY() + number(-7, 7) * 20;
		}
	}
}

class FPartyAlignmentCompute
{
	public:
		FPartyAlignmentCompute(int iAmount, int x, int y)
		{
			m_iAmount = iAmount;
			m_iCount = 0;
			m_iStep = 0;
			m_iKillerX = x;
			m_iKillerY = y;
		}

		void operator () (LPCHARACTER pkChr)
		{
			if (DISTANCE_APPROX(pkChr->GetX() - m_iKillerX, pkChr->GetY() - m_iKillerY) < PARTY_DEFAULT_RANGE)
			{
				if (m_iStep == 0)
				{
					++m_iCount;
				}
				else
				{
					pkChr->UpdateAlignment(m_iAmount / m_iCount);
				}
			}
		}

		int m_iAmount;
		int m_iCount;
		int m_iStep;

		int m_iKillerX;
		int m_iKillerY;
};



void CHARACTER::Dead(LPCHARACTER pkKiller, bool bImmediateDead)
{
	if (IsDead())
		return;
	
	if (GetInvincible())
		return;

	if (IsPC())
	{
#if !defined(USE_MOUNT_COSTUME_SYSTEM)
		UnMount(true);
#else
		if (IsHorseRiding())
		{
			StopRiding();
		}
#if !defined(USE_MOUNT_COSTUME_SYSTEM)
		else if (GetMountVnum()) {
			RemoveAffect(AFFECT_MOUNT_BONUS);

			m_dwMountVnum = 0;
			UnEquipSpecialRideUniqueItem();

			UpdatePacket();
		}
#endif
#endif
	}

	if (IsMonster() || IsStone())
	{
		LPDUNGEON dungeon = GetDungeon();
		if (dungeon)
		{
			dungeon->DecMonster();
		}
	}

	if (!pkKiller && m_dwKillerPID)
		pkKiller = CHARACTER_MANAGER::instance().FindByPID(m_dwKillerPID);

	m_dwKillerPID = 0; // 반드시 초기화 해야함 DO NOT DELETE THIS LINE UNLESS YOU ARE 1000000% SURE

	if (IsPC())
	{
		RemoveBadAffect();
	}

#if defined(ENABLE_ULTIMATE_REGEN)
	CNewMobTimer::Instance().Dead(this, pkKiller);
#endif

	bool isAgreedPVP = false;
	bool isUnderGuildWar = false;
	bool isDuel = false;

	if (pkKiller && pkKiller->IsPC())
	{
		if (pkKiller->m_pkChrTarget == this)
			pkKiller->SetTarget(NULL);

		isAgreedPVP = CPVPManager::instance().Dead(this, pkKiller->GetPlayerID());
		isDuel = CArenaManager::instance().OnDead(pkKiller, this);
#ifdef ENABLE_PVP_ADVANCED
		if (isAgreedPVP || isDuel)
		{	
			const char* szTableStaticPvP[] = {BLOCK_CHANGEITEM, BLOCK_BUFF, BLOCK_POTION, BLOCK_RIDE, BLOCK_PET, BLOCK_POLY, BLOCK_PARTY, BLOCK_EXCHANGE_, BET_WINNER, CHECK_IS_FIGHT};
				
			int betMoneyDead = GetQuestFlag(szTableStaticPvP[8]);
			int betMoneyKiller = pkKiller->GetQuestFlag(szTableStaticPvP[8]);
			
			if (betMoneyDead > 0 && betMoneyKiller > 0)
			{
#if defined(ENABLE_NEW_GOLD_LIMIT)
				pkKiller->ChangeGold(betMoneyDead*2);
#else
				pkKiller->PointChange(POINT_GOLD, betMoneyDead*2, true);
#endif
#if defined(ENABLE_TEXTS_RENEWAL)
				pkKiller->ChatPacketNew(CHAT_TYPE_INFO, 515, "%d", betMoneyDead);
#endif
			}
			
			for (unsigned int i = 0; i < _countof(szTableStaticPvP); i++) {	
				char pkCh_Buf[CHAT_MAX_LEN + 1], pkKiller_Buf[CHAT_MAX_LEN + 1];
				
				snprintf(pkCh_Buf, sizeof(pkCh_Buf), "BINARY_Duel_Delete");
				snprintf(pkKiller_Buf, sizeof(pkKiller_Buf), "BINARY_Duel_Delete");
				
				ChatPacket(CHAT_TYPE_COMMAND, pkCh_Buf);	
				SetQuestFlag(szTableStaticPvP[i], 0);
				
				pkKiller->ChatPacket(CHAT_TYPE_COMMAND, pkKiller_Buf);	
				pkKiller->SetQuestFlag(szTableStaticPvP[i], 0);	
			}
		}
#endif

		if (IsPC())
		{
			CGuild * g1 = GetGuild();
			CGuild * g2 = pkKiller->GetGuild();

			if (g1 && g2)
				if (g1->UnderWar(g2->GetID()))
					isUnderGuildWar = true;

			pkKiller->SetQuestNPCID(GetVID());
			quest::CQuestManager::instance().Kill(pkKiller->GetPlayerID(), quest::QUEST_NO_NPC);
			CGuildManager::instance().Kill(pkKiller, this);
		}
	}

#ifdef ENABLE_QUEST_DIE_EVENT
	//if (IsPC())
	//{
	//	if (pkKiller)
	//		SetQuestNPCID(pkKiller->GetVID());
	//	// quest::CQuestManager::instance().Die(GetPlayerID(), quest::QUEST_NO_NPC);
	//	quest::CQuestManager::instance().Die(GetPlayerID(), (pkKiller)?pkKiller->GetRaceNum():quest::QUEST_NO_NPC);
	//}
	if (IsPC())
	{
		if (pkKiller) {
			SetQuestNPCID(pkKiller->GetVID());
		}

		quest::CQuestManager::instance().Die(GetPlayerID(), (pkKiller) ? pkKiller->GetRaceNum() : quest::QUEST_NO_NPC);
	}
#endif

#ifdef ENABLE_RANKING
	if ((IsPC())) {
		if (((isAgreedPVP) || (isDuel)) && (pkKiller)) {
			SetRankPoints(1, pkKiller->GetRankPoints(1) + 1);
			pkKiller->SetRankPoints(0, pkKiller->GetRankPoints(0) + 1);
		}
		else if (isUnderGuildWar) {
			pkKiller->SetRankPoints(2, pkKiller->GetRankPoints(2) + 1);
		}
	}
	
	if (pkKiller) {
		if (pkKiller->IsPC()) {
			if (IsStone()) {
				if (pkKiller)
					pkKiller->SetRankPoints(5, pkKiller->GetRankPoints(5) + 1);
			}
			else if (IsMonster()) {
				if (GetMobRank() >= MOB_RANK_BOSS)
					pkKiller->SetRankPoints(7, pkKiller->GetRankPoints(7) + 1);
				else
					pkKiller->SetRankPoints(6, pkKiller->GetRankPoints(6) + 1);
			}
		}
	}
#endif

/*
	if (pkKiller &&
			!isAgreedPVP &&
			!isUnderGuildWar &&
			IsPC() &&
			!isDuel)
	{
		if (GetGMLevel() == GM_PLAYER || test_server)
		{
			ItemDropPenalty(pkKiller);
		}
	}
*/

#ifdef ENABLE_SKILLS_BUFF_ALTERNATIVE
	if (IsPC()) {
		if (pkKiller && !pkKiller->IsPC()) {
			pkKiller->SetTarget(NULL);
		}

		ClearAffectSkills();
	}
#endif

	SetPosition(POS_DEAD);
	ClearAffect(true);

	if (pkKiller && IsPC())
	{
		if (!pkKiller->IsPC())
		{
#ifdef ENABLE_REVIVE_WITH_HALF_HP_IF_MONSTER_KILLED_YOU
			SetDeadByMonster(true);
#endif

			sys_log(1, "DEAD: %s %p WITH PENALTY", GetName(), this);
			SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);
			LogManager::instance().CharLog(this, pkKiller->GetRaceNum(), "DEAD_BY_NPC", pkKiller->GetName());
		}
		else
		{
#ifdef ENABLE_REVIVE_WITH_HALF_HP_IF_MONSTER_KILLED_YOU
			SetDeadByMonster(false);
#endif
			sys_log(1, "DEAD_BY_PC: %s %p KILLER %s %p", GetName(), this, pkKiller->GetName(), get_pointer(pkKiller));
#if defined(USE_BATTLEPASS)
			pkKiller->UpdateExtBattlePassMissionProgress(KILL_PLAYER, 1, GetLevel());
#endif
			REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);

			if (GetEmpire() != pkKiller->GetEmpire())
			{
				int iEP = MIN(GetPoint(POINT_EMPIRE_POINT), pkKiller->GetPoint(POINT_EMPIRE_POINT));

				PointChange(POINT_EMPIRE_POINT, -(iEP / 10));
				pkKiller->PointChange(POINT_EMPIRE_POINT, iEP / 5);

				if (GetPoint(POINT_EMPIRE_POINT) < 10)
				{
					// TODO : 입구로 날리는 코드를 넣어야 한다.
				}

				char buf[256];
				snprintf(buf, sizeof(buf),
						"%d %d %d %s %d %d %d %s",
						GetEmpire(), GetAlignment(), GetPKMode(), GetName(),
						pkKiller->GetEmpire(), pkKiller->GetAlignment(), pkKiller->GetPKMode(), pkKiller->GetName());

				LogManager::instance().CharLog(this, pkKiller->GetPlayerID(), "DEAD_BY_PC", buf);
			}
			else
			{
				if (!isAgreedPVP && !isUnderGuildWar && !IsKillerMode() /*&& GetAlignment() >= 0*/ && !isDuel)
				{
					int iNoPenaltyProb = 0;

					if (pkKiller->GetAlignment() >= 0)	// 1/3 percent down
						iNoPenaltyProb = 33;
					else				// 4/5 percent down
						iNoPenaltyProb = 20;

					if (number(1, 100) < iNoPenaltyProb) {
#if defined(ENABLE_TEXTS_RENEWAL)
						pkKiller->ChatPacketNew(CHAT_TYPE_INFO, 413, "");
#endif
					} else {
						if (pkKiller->GetParty())
						{
							FPartyAlignmentCompute f(-20000, pkKiller->GetX(), pkKiller->GetY());
                            pkKiller->GetParty()->ForEachOnMapMember(f, pkKiller->GetMapIndex());

							if (f.m_iCount == 0)
								pkKiller->UpdateAlignment(-20000);
							else
							{
								sys_log(0, "ALIGNMENT PARTY count %d amount %d", f.m_iCount, f.m_iAmount);

								f.m_iStep = 1;
                                pkKiller->GetParty()->ForEachOnMapMember(f, pkKiller->GetMapIndex());
							}
						}
						else
							pkKiller->UpdateAlignment(-20000);
					}
				}

				char buf[256];
				snprintf(buf, sizeof(buf),
						"%d %d %d %s %d %d %d %s",
						GetEmpire(), GetAlignment(), GetPKMode(), GetName(),
						pkKiller->GetEmpire(), pkKiller->GetAlignment(), pkKiller->GetPKMode(), pkKiller->GetName());

				LogManager::instance().CharLog(this, pkKiller->GetPlayerID(), "DEAD_BY_PC", buf);
			}
		}
	}
	else
	{
		sys_log(1, "DEAD: %s %p", GetName(), this);
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);
	}

	ClearSync();

	//sys_log(1, "stun cancel %s[%d]", GetName(), (DWORD)GetVID());
	event_cancel(&m_pkStunEvent); // 기절 이벤트는 죽인다.

	if (IsPC())
	{
		m_dwLastDeadTime = get_dword_time();
		//SetKillerMode(pkKiller && pkKiller->IsPC());
		SetKillerMode(false);
		GetDesc()->SetPhase(PHASE_DEAD);
	}
	else
	{
		// 가드에게 공격받은 몬스터는 보상이 없어야 한다.
		if (!IS_SET(m_pointsInstant.instant_flag, INSTANT_FLAG_NO_REWARD))
		{
			if (!(pkKiller && pkKiller->IsPC() && pkKiller->GetGuild() && pkKiller->GetGuild()->UnderAnyWar(GUILD_WAR_TYPE_FIELD)))
			{
				// 부활하는 몬스터는 보상을 주지 않는다.
				if (GetMobTable().dwResurrectionVnum)
				{
					// DUNGEON_MONSTER_REBIRTH_BUG_FIX
					LPCHARACTER chResurrect = CHARACTER_MANAGER::instance().SpawnMob(GetMobTable().dwResurrectionVnum, GetMapIndex(), GetX(), GetY(), GetZ(), true, (int) GetRotation());
					if (GetDungeon() && chResurrect)
					{
						chResurrect->SetDungeon(GetDungeon());
					}
					// END_OF_DUNGEON_MONSTER_REBIRTH_BUG_FIX

					Reward(false);
				}
				else if (IsRevive() == true)
				{
					Reward(false);
				}
				else
				{
					Reward(true); // Drops gold, item, etc..
				}
			}
			else
			{
				if (pkKiller->m_dwUnderGuildWarInfoMessageTime < get_dword_time())
				{
					pkKiller->m_dwUnderGuildWarInfoMessageTime = get_dword_time() + 60000;
#if defined(ENABLE_TEXTS_RENEWAL)
					pkKiller->ChatPacketNew(CHAT_TYPE_INFO, 147, "");
#endif
				}
			}
		}
	}

	// BOSS_KILL_LOG
	if (GetMobRank() >= MOB_RANK_BOSS && pkKiller && pkKiller->IsPC())
	{
		char buf[51];
		snprintf(buf, sizeof(buf), "%d %ld", g_bChannel, pkKiller->GetMapIndex());
		if (IsStone())
			LogManager::instance().CharLog(pkKiller, GetRaceNum(), "STONE_KILL", buf);
		else
			LogManager::instance().CharLog(pkKiller, GetRaceNum(), "BOSS_KILL", buf);
	}
	// END_OF_BOSS_KILL_LOG

	TPacketGCDead pack;
	pack.header	= HEADER_GC_DEAD;
	pack.vid	= m_vid;
	PacketAround(&pack, sizeof(pack));

	REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);

	// 플레이어 캐릭터이면
	if (GetDesc() != NULL) {
		//
		// 클라이언트에 에펙트 패킷을 다시 보낸다.
		//
		auto it = m_list_pkAffect.begin();

		while (it != m_list_pkAffect.end())
			SendAffectAddPacket(GetDesc(), *it++);
	}

	//
	// Dead 이벤트 생성,
	//
	// Dead 이벤트에서는 몬스터의 경우 몇초 후에 Destroy 되도록 해주며,
	// PC의 경우 3분 있다가 마을에서 나오도록 해 준다. 3분 내에는 유저로부터
	// 마을에서 시작할 건지, 여기서 시작할 건지 결정을 받는다.
	if (isDuel == false)
	{
		if (m_pkDeadEvent)
		{
			sys_log(1, "DEAD_EVENT_CANCEL: %s %p %p", GetName(), this, get_pointer(m_pkDeadEvent));
			event_cancel(&m_pkDeadEvent);
		}

		if (IsStone())
		{
			ClearStone();
		}

		if (GetDungeon())
		{
			GetDungeon()->DeadCharacter(this);
		}

		SCharDeadEventInfo* pEventInfo = AllocEventInfo<SCharDeadEventInfo>();

		if (IsPC())
		{
			pEventInfo->isPC = true;
			pEventInfo->dwID = GetPlayerID();

			m_pkDeadEvent = event_create(dead_event, pEventInfo, PASSES_PER_SEC(180));
			sys_log(1, "DEAD_EVENT_CREATE: %s %p %p", GetName(), this, get_pointer(m_pkDeadEvent));
		}
		else
		{
			pEventInfo->isPC = false;
			pEventInfo->dwID = GetVID();

			if (IsRevive() == false && HasReviverInParty() == true)
			{
				m_pkDeadEvent = event_create(dead_event, pEventInfo, bImmediateDead ? 1 : PASSES_PER_SEC(3));
				sys_log(1, "DEAD_EVENT_CREATE: %s %p %p", GetName(), this, get_pointer(m_pkDeadEvent));
			}
#if defined(__DEFENSE_WAVE__)
			else if (GetRaceNum() >= 3950 && GetRaceNum() <= 3964)
			{
				m_pkDeadEvent = event_create(dead_event, pEventInfo, bImmediateDead ? 1 : PASSES_PER_SEC(3));
				sys_log(1, "DEAD_EVENT_CREATE: %s %p %p", GetName(), this, get_pointer(m_pkDeadEvent));
			}
#endif
			else
			{
				m_pkDeadEvent = event_create(dead_event, pEventInfo, 1);
				sys_log(1, "DEAD_EVENT_CREATE: %s %p %p", GetName(), this, get_pointer(m_pkDeadEvent));
			}
		}
	}

	if (GetDesc()) {
		if (m_pkExchange != nullptr)
		{
			m_pkExchange->Cancel();
			m_pkExchange = nullptr;
		}

#ifdef __ATTR_TRANSFER_SYSTEM__
		if (IsAttrTransferOpen() == true)
		{
			AttrTransfer_close(this);
		}
#endif

		if (IsCubeOpen() == true)
		{
			Cube_close(this);
		}

		CShopManager::instance().StopShopping(this);
		CloseMyShop();
		CloseSafebox();
#ifdef ENABLE_ACCE_SYSTEM
		CloseAcce();
#endif
#if defined(ENABLE_AURA_SYSTEM)
		if (IsAuraRefineWindowOpen())
		{
			AuraRefineWindowClose();
		}
#endif
#if defined(ENABLE_CHANGELOOK)
		if (isChangeLookOpened())
		{
			ChangeLookWindow(false, true);
		}
#endif
	}
}

struct FuncSetLastAttacked
{
	FuncSetLastAttacked(DWORD dwTime) : m_dwTime(dwTime)
	{
	}

	void operator () (LPCHARACTER ch)
	{
		ch->SetLastAttacked(m_dwTime);
	}

	DWORD m_dwTime;
};

void CHARACTER::SetLastAttacked(DWORD dwTime)
{
	assert(m_pkMobInst != NULL);

	m_pkMobInst->m_dwLastAttackedTime = dwTime;
	m_pkMobInst->m_posLastAttacked = GetXYZ();
}

void CHARACTER::SendDamagePacket(LPCHARACTER pAttacker, int Damage, BYTE DamageFlag) {
	if (IsPC() == true || (pAttacker->IsPC() == true && pAttacker->GetTarget() == this))
	{
		TPacketGCDamageInfo p;

		p.header = HEADER_GC_DAMAGE_INFO;
		p.dwVID = (DWORD)GetVID();
		p.flag = DamageFlag;
		p.damage = Damage;

		if (GetDesc() != NULL)
		{
			GetDesc()->Packet(&p, sizeof(TPacketGCDamageInfo));
		}

		if (pAttacker->GetDesc() != NULL)
		{
			pAttacker->GetDesc()->Packet(&p, sizeof(TPacketGCDamageInfo));
		}

		if (GetArenaObserverMode() == false && GetArena() != NULL) {
			GetArena()->SendPacketToObserver(&p, sizeof(TPacketGCDamageInfo));
		}
	}
}

//
// CHARACTER::Damage 메소드는 this가 데미지를 입게 한다.
//
// Arguments
//    pAttacker		: 공격자
//    dam		: 데미지
//    EDamageType	: 어떤 형식의 공격인가?
//
// Return value
//    true		: dead
//    false		: not dead yet
//

#ifdef __ENABLE_BERAN_ADDONS_
bool IsBeranMap(int lMapIndex)
{
	int lMinIndex = 208*10000, lMaxIndex = 208*10000 + 10000;
	if (((lMapIndex >= lMinIndex) && (lMapIndex < lMaxIndex)) || (lMapIndex == 208))
		return true;
	
	return false;
}
#endif

#ifdef __ENABLE_SPIDER_ADDONS_
bool IsSpiderMap(int lMapIndex)
{
	int lMinIndex = 217*10000, lMaxIndex = 217*10000 + 10000;
	if (((lMapIndex >= lMinIndex) && (lMapIndex < lMaxIndex)) || (lMapIndex == 217))
		return true;
	
	return false;
}
#endif

bool CHARACTER::Damage(LPCHARACTER pAttacker, int dam, EDamageType type) // returns true if dead
{
	if (GetInvincible())
		return false;

#ifdef __NEWPET_SYSTEM__
	if (IsImmortal())
		return false;
#endif

	if (pAttacker)
	{
		if (pAttacker->IsAffectFlag(AFF_GWIGUM) && !pAttacker->GetWear(WEAR_WEAPON))
		{
			pAttacker->RemoveAffect(SKILL_GWIGEOM);
			return false;
		}

		if (pAttacker->IsAffectFlag(AFF_GEOMGYEONG) && !pAttacker->GetWear(WEAR_WEAPON))
		{
			pAttacker->RemoveAffect(SKILL_GEOMKYUNG);
			return false;
		}
	}

	if ((IsPC() && IsAffectFlag(AFF_REVIVE_INVISIBLE)) || (pAttacker && (pAttacker->IsPC() && pAttacker->IsAffectFlag(AFF_REVIVE_INVISIBLE))))
		return false;

	if (pAttacker && IsStone() && pAttacker->IsPC())
	{
		if (GetEmpire() && GetEmpire() == pAttacker->GetEmpire())
		{
			SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
			return false;
		}
	}

#if defined(ENABLE_WORLDBOSS)
	if (IsWorldBossMonster()) {
#if defined(ENABLE_BLOCK_MULTIFARM)
		if (pAttacker && pAttacker->FindAffect(AFFECT_DROP_BLOCK, APPLY_NONE)) {
			SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
			return false;
		}
#endif

#if defined(ENABLE_HALLOWEEN_EVENT_2022) && !defined(USE_NO_HALLOWEEN_EVENT_2022_BONUSES)
        if (IsHalloweenMonster() && pAttacker && pAttacker->GetPoint(POINT_FEAR) < 25)
        {
            SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
            return false;
        }
#endif
    }
#endif

	if (DAMAGE_TYPE_MAGIC == type)
	{
		dam = (int)((float)dam * (100 + (pAttacker->GetPoint(POINT_MAGIC_ATT_BONUS_PER) + pAttacker->GetPoint(POINT_MELEE_MAGIC_ATT_BONUS_PER))) / 100.f + 0.5f);
	}

	// 평타가 아닐 때는 공포 처리
	if (type != DAMAGE_TYPE_NORMAL && type != DAMAGE_TYPE_NORMAL_RANGE)
	{
		if (IsAffectFlag(AFF_TERROR))
		{
			int pct = GetSkillPower(SKILL_TERROR) / 400;

			if (number(1, 100) <= pct)
				return false;
		}
	}

	int iCurHP = GetHP();
	int iCurSP = GetSP();

	bool IsCritical = false;
	bool IsPenetrate = false;
	bool IsDeathBlow = false;

	//PROF_UNIT puAttr("Attr");

	//
	// 마법형 스킬과, 레인지형 스킬은(궁자객) 크리티컬과, 관통공격 계산을 한다.
	// 원래는 하지 않아야 하는데 Nerf(다운밸런스)패치를 할 수 없어서 크리티컬과
	// 관통공격의 원래 값을 쓰지 않고, /2 이상하여 적용한다.
	//
	// 무사 이야기가 많아서 밀리 스킬도 추가
	//
	// 20091109 : 무사가 결과적으로 엄청나게 강해진 것으로 결론남, 독일 기준 무사 비율 70% 육박
	//
	
#if defined(ENABLE_DS_RUNE) || defined(ENABLE_MELEY_LAIR)
	int32_t itakehp = 0;
#endif
	
	if (type == DAMAGE_TYPE_MELEE || type == DAMAGE_TYPE_RANGE || type == DAMAGE_TYPE_MAGIC)
	{
		if (pAttacker)
		{
			// 크리티컬
			int iCriticalPct = pAttacker->GetPoint(POINT_CRITICAL_PCT);

			if (!IsPC()) {
				iCriticalPct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_CRITICAL_BONUS);
				iCriticalPct += pAttacker->GetPoint(POINT_PVM_CRITICAL_PCT);
			}
			
			if (iCriticalPct)
			{
				if (iCriticalPct >= 10) // 10보다 크면 5% + (4마다 1%씩 증가), 따라서 수치가 50이면 20%
					iCriticalPct = 5 + (iCriticalPct - 10) / 4;
				else // 10보다 작으면 단순히 반으로 깎음, 10 = 5%
					iCriticalPct /= 2;

				//크리티컬 저항 값 적용.
				iCriticalPct -= GetPoint(POINT_RESIST_CRITICAL);

				if (number(1, 100) <= iCriticalPct)
				{
					IsCritical = true;
					dam *= 2;
					EffectPacket(SE_CRITICAL);

					if (IsAffectFlag(AFF_MANASHIELD))
					{
						RemoveAffect(AFF_MANASHIELD);
					}
				}
			}

			// 관통공격
			int iPenetratePct = pAttacker->GetPoint(POINT_PENETRATE_PCT);

			if (!IsPC())
				iPenetratePct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_PENETRATE_BONUS);


			if (iPenetratePct)
			{
				{
					CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_RESIST_PENETRATE);

					if (NULL != pkSk)
					{
						pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_RESIST_PENETRATE) / 100.0f);

						iPenetratePct -= static_cast<int>(pkSk->kPointPoly.Eval());
					}
				}

				if (iPenetratePct >= 10)
				{
					// 10보다 크면 5% + (4마다 1%씩 증가), 따라서 수치가 50이면 20%
					iPenetratePct = 5 + (iPenetratePct - 10) / 4;
				}
				else
				{
					// 10보다 작으면 단순히 반으로 깎음, 10 = 5%
					iPenetratePct /= 2;
				}

				//관통타격 저항 값 적용.
				iPenetratePct -= GetPoint(POINT_RESIST_PENETRATE);

				if (number(1, 100) <= iPenetratePct)
				{
					IsPenetrate = true;
#if defined(ENABLE_TEXTS_RENEWAL)
					if (test_server) {
						ChatPacketNew(CHAT_TYPE_INFO, 257, "%d", GetPoint(POINT_DEF_GRADE) * (100 + GetPoint(POINT_DEF_BONUS)) / 100);
					}
#endif
					dam += GetPoint(POINT_DEF_GRADE) * (100 + GetPoint(POINT_DEF_BONUS)) / 100;

					if (IsAffectFlag(AFF_MANASHIELD))
					{
						RemoveAffect(AFF_MANASHIELD);
					}
				}
			}
		}
	}
	//
	// 콤보 공격, 활 공격, 즉 평타 일 때만 속성값들을 계산을 한다.
	//
	else if (type == DAMAGE_TYPE_NORMAL || type == DAMAGE_TYPE_NORMAL_RANGE)
	{
		if (type == DAMAGE_TYPE_NORMAL)
		{
			// 근접 평타일 경우 막을 수 있음
			if (GetPoint(POINT_BLOCK) && number(1, 100) <= GetPoint(POINT_BLOCK))
			{
#if defined(ENABLE_TEXTS_RENEWAL)
				if (test_server) {
					pAttacker->ChatPacketNew(CHAT_TYPE_INFO, 95, "%s#%d", GetName(), GetPoint(POINT_BLOCK));
					ChatPacketNew(CHAT_TYPE_INFO, 95, "%s#%d", pAttacker->GetName(), pAttacker->GetPoint(POINT_BLOCK));
				}
#endif
				SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
				return false;
			}
		}
		else if (type == DAMAGE_TYPE_NORMAL_RANGE)
		{
			// 원거리 평타의 경우 피할 수 있음
			if (GetPoint(POINT_DODGE) && number(1, 100) <= GetPoint(POINT_DODGE))
			{
#if defined(ENABLE_TEXTS_RENEWAL)
				if (test_server) {
					pAttacker->ChatPacketNew(CHAT_TYPE_INFO, 96, "%s#%d", GetName(), GetPoint(POINT_DODGE));
					ChatPacketNew(CHAT_TYPE_INFO, 96, "%s#%d", pAttacker->GetName(), pAttacker->GetPoint(POINT_DODGE));
				}
#endif
				SendDamagePacket(pAttacker, 0, DAMAGE_DODGE);
				return false;
			}
		}

#ifndef ENABLE_NO_MALUS_JEONGWIHON
		if (IsAffectFlag(AFF_JEONGWIHON))
			dam = (int) (dam * (100 + GetSkillPower(SKILL_JEONGWI) * 25 / 100) / 100);
#endif

		if (IsAffectFlag(AFF_TERROR))
			dam = (int) (dam * (95 - GetSkillPower(SKILL_TERROR) / 5) / 100);

		if (IsAffectFlag(AFF_HOSIN))
			dam = dam * (100 - GetPoint(POINT_RESIST_NORMAL_DAMAGE)) / 100;

		//
		// 공격자 속성 적용
		//
		if (pAttacker)
		{
			if (type == DAMAGE_TYPE_NORMAL)
			{
#if defined(ENABLE_WORLDBOSS)
				if (!pAttacker->IsWorldBossMonster()) {
#endif
				if (GetPoint(POINT_REFLECT_MELEE))
				{
					int reflectDamage = dam * GetPoint(POINT_REFLECT_MELEE) / 100;

					// NOTE: 공격자가 IMMUNE_REFLECT 속성을 갖고있다면 반사를 안 하는 게
					// 아니라 1/3 데미지로 고정해서 들어가도록 기획에서 요청.
					if (pAttacker->IsImmune(IMMUNE_REFLECT))
						reflectDamage = int(reflectDamage / 3.0f + 0.5f);

					pAttacker->Damage(this, reflectDamage, DAMAGE_TYPE_SPECIAL);
				}
#if defined(ENABLE_WORLDBOSS)
				}
#endif
			}

			// 크리티컬
			int iCriticalPct = pAttacker->GetPoint(POINT_CRITICAL_PCT);

			if (!IsPC()) {
				iCriticalPct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_CRITICAL_BONUS);
				iCriticalPct += pAttacker->GetPoint(POINT_PVM_CRITICAL_PCT);
			}
			
			if (iCriticalPct)
			{
				//크리티컬 저항 값 적용.
				iCriticalPct -= GetPoint(POINT_RESIST_CRITICAL);

				if (number(1, 100) <= iCriticalPct)
				{
					IsCritical = true;
					dam *= 2;
					EffectPacket(SE_CRITICAL);
				}
			}

			// 관통공격
			int iPenetratePct = pAttacker->GetPoint(POINT_PENETRATE_PCT);

			if (!IsPC())
				iPenetratePct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_PENETRATE_BONUS);

			{
				CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_RESIST_PENETRATE);

				if (NULL != pkSk)
				{
					pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_RESIST_PENETRATE) / 100.0f);

					iPenetratePct -= static_cast<int>(pkSk->kPointPoly.Eval());
				}
			}


			if (iPenetratePct)
			{

				//관통타격 저항 값 적용.
				iPenetratePct -= GetPoint(POINT_RESIST_PENETRATE);

				if (number(1, 100) <= iPenetratePct)
				{
					IsPenetrate = true;
					dam += GetPoint(POINT_DEF_GRADE) * (100 + GetPoint(POINT_DEF_BONUS)) / 100;
				}
			}

			int iStealHP_ptr = pAttacker->GetPoint(POINT_STEAL_HP);
			if (iStealHP_ptr) {
				if (number(1, 100) <= iStealHP_ptr) {
					int iHP = MIN(dam, MAX(0, GetHP())) * pAttacker->GetPoint(POINT_STEAL_HP) / 100;


					if ((pAttacker->GetHP() > 0) && (pAttacker->GetHP() + iHP < pAttacker->GetMaxHP()) && (GetHP() > 0) && (iHP > 0)) {
						CreateFly(FLY_HP_MEDIUM, pAttacker);
						pAttacker->PointChange(POINT_HP, iHP);
#if defined(ENABLE_DS_RUNE) || defined(ENABLE_MELEY_LAIR)
						int32_t racevnum = GetRaceNum();
						if (
#if defined(ENABLE_DS_RUNE)
						racevnum == 3996 || racevnum == 3997 || racevnum == 3998 || racevnum == 4011 || racevnum == 4012 || racevnum == 4013
#endif
#if defined(ENABLE_MELEY_LAIR)
#ifdef ENABLE_DS_RUNE
						 || racevnum == 6118
#else
						racevnum == 6118
#endif
#endif
						)
						{
							itakehp = iHP;
						}
						else
						{
#if defined(ENABLE_WORLDBOSS)
							if (!IsWorldBossMonster()) {
#endif
								PointChange(POINT_HP, -iHP);
#if defined(ENABLE_WORLDBOSS)
							}
#endif
						}
#else
#if defined(ENABLE_WORLDBOSS)
						if (!IsWorldBossMonster()) {
#endif
							PointChange(POINT_HP, -iHP);
#if defined(ENABLE_WORLDBOSS)
						}
#endif
#endif
					}
				}
			}

			int iStealSP_ptr = pAttacker->GetPoint(POINT_STEAL_SP);
			if (iStealSP_ptr) {
				if (IsPC() && pAttacker->IsPC()) {
					if (number(1, 100) <= iStealSP_ptr) {
						int iSP = MIN(dam, MAX(0, GetSP())) * pAttacker->GetPoint(POINT_STEAL_SP) / 100;


						if ((pAttacker->GetSP() > 0) && (pAttacker->GetSP() + iSP < pAttacker->GetMaxSP()) && (GetSP() > 0) && (iSP > 0))
						{
							CreateFly(FLY_SP_MEDIUM, pAttacker);
							pAttacker->PointChange(POINT_SP, iSP);
							PointChange(POINT_SP, -iSP);
						}
					}
				}
			}

			// 돈 스틸
			if (pAttacker->GetPoint(POINT_STEAL_GOLD))
			{
				if (number(1, 100) <= pAttacker->GetPoint(POINT_STEAL_GOLD))
				{
#if defined(ENABLE_NEW_GOLD_LIMIT)
					uint64_t iAmount = number(1, GetLevel());
					pAttacker->ChangeGold(iAmount);
#else
					uint32_t iAmount = number(1, GetLevel());
					pAttacker->PointChange(POINT_GOLD, iAmount);
#endif
					DBManager::instance().SendMoneyLog(MONEY_LOG_MISC, 1, iAmount);
				}
			}

			int iAbsoHP_ptr = pAttacker->GetPoint(POINT_HIT_HP_RECOVERY);
			if (iAbsoHP_ptr > 0) {
				if (number(1, 100) <= iAbsoHP_ptr) {
					int iHPAbso = MIN(dam, GetHP()) * pAttacker->GetPoint(POINT_HIT_HP_RECOVERY) / 100;
					if ((pAttacker->GetHP() > 0) && (pAttacker->GetHP() + iHPAbso < pAttacker->GetMaxHP()) && (GetHP() > 0) && (iHPAbso > 0)) {
						CreateFly(FLY_HP_SMALL, pAttacker);
						pAttacker->PointChange(POINT_HP, iHPAbso);
					}
				}
			}

			int iAbsoSP_ptr = pAttacker->GetPoint(POINT_HIT_SP_RECOVERY);
			if (iAbsoSP_ptr > 0) {
				if (number(1, 100) <= iAbsoSP_ptr) {
					int iSPAbso = MIN(dam, GetSP()) * pAttacker->GetPoint(POINT_HIT_SP_RECOVERY) / 100;
					if ((pAttacker->GetSP() > 0) && (pAttacker->GetSP() + iSPAbso < pAttacker->GetMaxSP()) && (GetSP() > 0) && (iSPAbso > 0)) {
						CreateFly(FLY_SP_SMALL, pAttacker);
						pAttacker->PointChange(POINT_SP, iSPAbso);
					}
				}
			}

			// 상대방의 마나를 없앤다.
			if (pAttacker->GetPoint(POINT_MANA_BURN_PCT))
			{
				if (number(1, 100) <= pAttacker->GetPoint(POINT_MANA_BURN_PCT))
					PointChange(POINT_SP, -50);
			}
		}
	}

	//
	// 평타 또는 스킬로 인한 보너스 피해/방어 계산
	//
	switch (type)
	{
		case DAMAGE_TYPE_NORMAL:
		case DAMAGE_TYPE_NORMAL_RANGE:
			{
				if (pAttacker) {
					if (pAttacker->GetPoint(POINT_NORMAL_HIT_DAMAGE_BONUS))
						dam = dam * (100 + pAttacker->GetPoint(POINT_NORMAL_HIT_DAMAGE_BONUS)) / 100;
#ifdef ENABLE_MEDI_PVM
					if (IsNPC())
						dam = dam * (100 + pAttacker->GetPoint(POINT_ATTBONUS_MEDI_PVM)) / 100;
#endif
				}
				
				dam = dam * (100 - MIN(99, GetPoint(POINT_NORMAL_HIT_DEFEND_BONUS))) / 100;
			}
			break;
		case DAMAGE_TYPE_MELEE:
		case DAMAGE_TYPE_RANGE:
		case DAMAGE_TYPE_FIRE:
		case DAMAGE_TYPE_ICE:
		case DAMAGE_TYPE_ELEC:
		case DAMAGE_TYPE_MAGIC:
			{
				if (pAttacker) {
					if (pAttacker->GetPoint(POINT_SKILL_DAMAGE_BONUS))
						dam = dam * (100 + pAttacker->GetPoint(POINT_SKILL_DAMAGE_BONUS)) / 100;
				}
				
				dam = dam * (100 - MIN(99, GetPoint(POINT_SKILL_DEFEND_BONUS))) / 100;
			}
			break;
		default:
			break;
	}

	//
	// 마나쉴드(흑신수호)
	//
	if (IsAffectFlag(AFF_MANASHIELD))
	{
		// POINT_MANASHIELD 는 작아질수록 좋다
		int iDamageSPPart = dam / 3;
		int iDamageToSP = iDamageSPPart * GetPoint(POINT_MANASHIELD) / 100;
		int iSP = GetSP();

		// SP가 있으면 무조건 데미지 절반 감소
		if (iDamageToSP <= iSP)
		{
			PointChange(POINT_SP, -iDamageToSP);
			dam -= iDamageSPPart;
		}
		else
		{
			// 정신력이 모자라서 피가 더 깍여야할떄
			PointChange(POINT_SP, -GetSP());
			dam -= iSP * 100 / MAX(GetPoint(POINT_MANASHIELD), 1);
		}
	}

	//
	// 전체 방어력 상승 (몰 아이템)
	//
	if (GetPoint(POINT_MALL_DEFBONUS) > 0)
	{
		int dec_dam = MIN(200, dam * GetPoint(POINT_MALL_DEFBONUS) / 100);
		dam -= dec_dam;
	}

	if (pAttacker)
	{
		//
		// 전체 공격력 상승 (몰 아이템)
		//
		if (pAttacker->GetPoint(POINT_MALL_ATTBONUS) > 0)
		{
			int add_dam = MIN(300, dam * pAttacker->GetLimitPoint(POINT_MALL_ATTBONUS) / 100);
			dam += add_dam;
		}

		if (pAttacker->IsPC())
		{
			int iEmpire = pAttacker->GetEmpire();
			long lMapIndex = pAttacker->GetMapIndex();
			int iMapEmpire = SECTREE_MANAGER::instance().GetEmpireFromMapIndex(lMapIndex);

			// 다른 제국 사람인 경우 데미지 10% 감소
			if (iEmpire && iMapEmpire && iEmpire != iMapEmpire)
			{
				dam = dam * 9 / 10;
			}

			if (!IsPC() && GetMonsterDrainSPPoint())
			{
				int iDrain = GetMonsterDrainSPPoint();

				if (iDrain <= pAttacker->GetSP())
					pAttacker->PointChange(POINT_SP, -iDrain);
				else
				{
					int iSP = pAttacker->GetSP();
					pAttacker->PointChange(POINT_SP, -iSP);
				}
			}

		}
		else if (pAttacker->IsGuardNPC())
		{
			SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_NO_REWARD);
			Stun();
			return true;
		}
	}
	//puAttr.Pop();

	if (!GetSectree() || GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK))
		return false;

	if (!IsPC())
	{
		if (m_pkParty && m_pkParty->GetLeader())
			m_pkParty->GetLeader()->SetLastAttacked(get_dword_time());
		else
			SetLastAttacked(get_dword_time());
	}

	if (IsStun())
	{
		Dead(pAttacker);
		return true;
	}

	if (IsDead())
		return true;

	// 독 공격으로 죽지 않도록 함.
	if (type == DAMAGE_TYPE_POISON)
	{
		if (GetHP() - dam <= 0)
		{
			dam = GetHP() - 1;
		}
	}

	// ------------------------
	// 독일 프리미엄 모드
	// -----------------------
	if (pAttacker && pAttacker->IsPC())
	{
		int iDmgPct = CHARACTER_MANAGER::instance().GetUserDamageRate(pAttacker);
		dam = dam * iDmgPct / 100;
	}

	// STONE SKIN : 피해 반으로 감소
	if (IsMonster() && IsStoneSkinner())
	{
		if (GetHPPct() < GetMobTable().bStoneSkinPoint)
			dam /= 2;
	}

	//PROF_UNIT puRest1("Rest1");
	if (pAttacker)
	{
		// DEATH BLOW : 확률 적으로 4배 피해 (!? 현재 이벤트나 공성전용 몬스터만 사용함)
		if (pAttacker->IsMonster() && pAttacker->IsDeathBlower())
		{
			if (pAttacker->IsDeathBlow())
			{
				if (number(1, 4) == GetJob())
				{
					IsDeathBlow = true;
					dam = dam * 4;
				}
			}
		}

		BYTE damageFlag = 0;

		if (type == DAMAGE_TYPE_POISON)
			damageFlag = DAMAGE_POISON;
		else
			damageFlag = DAMAGE_NORMAL;

		if (IsCritical == true)
			damageFlag |= DAMAGE_CRITICAL;

		if (IsPenetrate == true)
			damageFlag |= DAMAGE_PENETRATE;


		//최종 데미지 보정
		float damMul = this->GetDamMul();
		float tempDam = dam;
		dam = tempDam * damMul + 0.5f;

#if defined(ENABLE_WORLDBOSS)
		if (IsWorldBossMonster()) {
			dam = pAttacker->GetLevel() > PVP_LVL_LIMIT ? 2 : 1;
		}
#endif

#if defined(ENABLE_DS_RUNE) || defined(ENABLE_MELEY_LAIR)
		if (!IsPC() && pAttacker && pAttacker->IsPC())
		{
			int32_t racevnum = GetRaceNum();
			LPDUNGEON dungeon = GetDungeon();
			if (dungeon)
			{
#if defined(ENABLE_DS_RUNE)
				if (racevnum == 3996 || racevnum == 3997 || racevnum == 3998 || racevnum == 4011 || racevnum == 4012 || racevnum == 4013)
				{
					int32_t type = dungeon->GetFlag("type");
					int32_t step = dungeon->GetFlag("step");
					if (type == 2)
					{
						if (step == 0)
						{
							int32_t per = (GetMaxHP() / 100) * 60;
							if (GetHP()-dam <= per)
							{
								dungeon->SetFlag("step", 1);
								if (racevnum == 3997) {
									dungeon->SpawnRegen("data/dungeon/rune/regen2_type3a.txt");
								}
								else if (racevnum == 3998) {
									dungeon->SpawnRegen("data/dungeon/rune/regen3_type3a.txt");
								}
								else if (racevnum == 3996) {
									dungeon->SpawnRegen("data/dungeon/rune/regen4_type3a.txt");
								}

								dungeon->Notice(905, "");
								dungeon->Notice(906, "");

								if (GetHP() > per)
								{
									PointChange(POINT_HP, -(GetHP() - per), false);
								}
								else
								{
									PointChange(POINT_HP, (per - GetHP()), false);
								}

								SetInvincible(true);
								return false;
							}
						}
						else if (step == 2)
						{
							int32_t per = (GetMaxHP() / 100) * 20;
							if (GetHP()-dam <= per)
							{
								dungeon->SetFlag("step", 3);
								if (racevnum == 3997) {
									dungeon->SpawnRegen("data/dungeon/rune/regen2_type3b.txt");
								}
								else if (racevnum == 3998) {
									dungeon->SpawnRegen("data/dungeon/rune/regen3_type3b.txt");
								}
								else if (racevnum == 3996) {
									dungeon->SpawnRegen("data/dungeon/rune/regen4_type3b.txt");
								}

								dungeon->Notice(907, "");
								dungeon->Notice(906, "");

								if (GetHP() > per)
								{
									PointChange(POINT_HP, -(GetHP() - per), false);
								}
								else
								{
									PointChange(POINT_HP, (per - GetHP()), false);
								}

								SetAttMul(2.0f);
								SetDamMul(2.0f);
								SetInvincible(true);
								return false;
							}
						}
					}
					else if (type == 3 && step == 0)
					{
						LPPARTY party = pAttacker->GetParty();
						if (party)
						{
							if (party->GetLeaderPID() == pAttacker->GetPlayerID())
							{
								int32_t per = (GetMaxHP() / 100) * 70;
								if (GetHP()-dam <= per)
								{
									dungeon->SetFlag("step", 1);
									dungeon->Notice(908, "");
								}
							}
							else
							{
								return false;
							}
						}
						else
						{
							dungeon->SetFlag("step", 1);
						}
					}
					else if (type == 8)
					{
						if (step == 0)
						{
							int32_t per = (GetMaxHP() / 100) * 50;
							if (GetHP()-dam <= per)
							{
								dungeon->SetFlag("step", 1);
								dungeon->SpawnRegen("data/dungeon/rune/regen8.txt");

								dungeon->Notice(907, "");
								dungeon->Notice(906, "");

								if (GetHP() > per)
								{
									PointChange(POINT_HP, -(GetHP() - per), false);
								}
								else
								{
									PointChange(POINT_HP, (per - GetHP()), false);
								}

								IncreaseMobRigHP(20);
								SetInvincible(true);
								return false;
							}
						}
						else if (step == 2)
						{
							int32_t per = (GetMaxHP() / 100) * 10;
							if (GetHP()-dam <= per)
							{
								dungeon->SetFlag("step", 3);
								dungeon->SpawnRegen("data/dungeon/rune/regen9.txt");

								dungeon->Notice(905, "");
								dungeon->Notice(906, "");

								if (GetHP() > per)
								{
									PointChange(POINT_HP, -(GetHP() - per), false);
								}
								else
								{
									PointChange(POINT_HP, (per - GetHP()), false);
								}

								SetAttMul(2.0f);
								SetDamMul(2.0f);
								SetInvincible(true);
								return false;
							}
						}
					}

					if (itakehp != 0)
					{
						PointChange(POINT_HP, -itakehp);
					}
				}
#endif
#if defined(ENABLE_MELEY_LAIR)
				if (racevnum == 6118)
				{
					int32_t vid = GetVID();
					if (vid == dungeon->GetFlag("statue_vid1") || vid == dungeon->GetFlag("statue_vid2") || vid == dungeon->GetFlag("statue_vid3") || vid == dungeon->GetFlag("statue_vid4"))
					{
						int32_t floor = dungeon->GetFlag("floor");
						if (floor >= 1 && floor < 5)
						{
							int32_t per = (GetMaxHP() / 100) * 75;
							if (GetHP()-dam <= per)
							{
								dungeon->SetFlag("floor", floor + 1);

								if (GetHP() > per)
								{
									PointChange(POINT_HP, -(GetHP() - per), false);
								}
								else
								{
									PointChange(POINT_HP, (per - GetHP()), false);
								}

								SetInvincible(true);

								if (!FindAffect(AFFECT_STATUE))
								{
									AddAffect(AFFECT_STATUE, POINT_NONE, 0, AFF_STATUE1, 3600, 0, true);
								}

								if (floor == 4)
								{
									dungeon->KillAllMonsters();
									dungeon->ClearRegen();
								}

								return false;
							}
						}
						else if (floor >= 7 && floor < 11)
						{
							int32_t per = (GetMaxHP() / 100) * 50;
							if (GetHP()-dam <= per)
							{
								dungeon->SetFlag("floor", floor + 1);

								if (GetHP() > per)
								{
									PointChange(POINT_HP, -(GetHP() - per), false);
								}
								else
								{
									PointChange(POINT_HP, (per - GetHP()), false);
								}

								SetInvincible(true);

								if (!FindAffect(AFFECT_STATUE))
								{
									AddAffect(AFFECT_STATUE, POINT_NONE, 0, AFF_STATUE2, 3600, 0, true);
								}

								if (floor == 10)
								{
									dungeon->KillAllMonsters();
									dungeon->ClearRegen();
								}

								return false;
							}
						}
						else if (floor >= 13 && floor < 17)
						{
							int32_t per = (GetMaxHP() / 100) * 5;
							if (GetHP()-dam <= per)
							{
								dungeon->SetFlag("floor", floor + 1);

								if (GetHP() > per)
								{
									PointChange(POINT_HP, -(GetHP() - per), false);
								}
								else
								{
									PointChange(POINT_HP, (per - GetHP()), false);
								}

								SetInvincible(true);

								if (!FindAffect(AFFECT_STATUE))
								{
									AddAffect(AFFECT_STATUE, POINT_NONE, 0, AFF_STATUE3, 3600, 0, true);
								}

								if (floor == 17)
								{
									dungeon->KillAllMonsters();
								}

								return false;
							}
						}
					}

					if (itakehp != 0)
					{
						PointChange(POINT_HP, -itakehp);
					}
				}
#endif
			}
		}
#endif

		if (pAttacker)
			SendDamagePacket(pAttacker, dam, damageFlag);

		if (test_server)
		{
			int iTmpPercent = 0; // @fixme136
			if (GetMaxHP() >= 0)
				iTmpPercent = (GetHP() * 100) / GetMaxHP();

			if(pAttacker)
			{
				pAttacker->ChatPacket(CHAT_TYPE_INFO, "-> %s, DAM %d HP %d(%d%%) %s%s",
						GetName(),
						dam,
						GetHP(),
						iTmpPercent,
						IsCritical ? "crit " : "",
						IsPenetrate ? "pene " : "",
						IsDeathBlow ? "deathblow " : "");
			}

			ChatPacket(CHAT_TYPE_PARTY, "<- %s, DAM %d HP %d(%d%%) %s%s",
					pAttacker ? pAttacker->GetName() : 0,
					dam,
					GetHP(),
					iTmpPercent,
					IsCritical ? "crit " : "",
					IsPenetrate ? "pene " : "",
					IsDeathBlow ? "deathblow " : "");
		}
		
#ifdef ENABLE_RANKING
		if (pAttacker->IsPC()) {
			if (IsPC()) {
				switch (type) {
					case DAMAGE_TYPE_NORMAL:
					case DAMAGE_TYPE_NORMAL_RANGE: {
							if (dam > pAttacker->GetRankPoints(3))
								pAttacker->SetRankPoints(3, dam);
						}
						break;
					case DAMAGE_TYPE_MELEE:
					case DAMAGE_TYPE_RANGE:
					case DAMAGE_TYPE_FIRE:
					case DAMAGE_TYPE_ICE:
					case DAMAGE_TYPE_ELEC:
					case DAMAGE_TYPE_MAGIC: {
							if (dam > pAttacker->GetRankPoints(4))
								pAttacker->SetRankPoints(4, dam);
						}
						break;
					default:
						break;
				}
			}
			else if (IsMonster()) {
				if (GetMobRank() >= MOB_RANK_BOSS) {
					switch (type) {
						case DAMAGE_TYPE_NORMAL:
						case DAMAGE_TYPE_NORMAL_RANGE: {
								if (dam > pAttacker->GetRankPoints(8))
									pAttacker->SetRankPoints(8, dam);
							}
							break;
						case DAMAGE_TYPE_MELEE:
						case DAMAGE_TYPE_RANGE:
						case DAMAGE_TYPE_FIRE:
						case DAMAGE_TYPE_ICE:
						case DAMAGE_TYPE_ELEC:
						case DAMAGE_TYPE_MAGIC: {
								if (dam > pAttacker->GetRankPoints(9))
									pAttacker->SetRankPoints(9, dam);
							}
							break;
						default:
							break;
					}
				}
				else if (!IsStone()) {
					switch (type) {
						case DAMAGE_TYPE_NORMAL:
						case DAMAGE_TYPE_NORMAL_RANGE: {
								if (dam > pAttacker->GetRankPoints(18))
									pAttacker->SetRankPoints(18, dam);
							}
							break;
						case DAMAGE_TYPE_MELEE:
						case DAMAGE_TYPE_RANGE:
						case DAMAGE_TYPE_FIRE:
						case DAMAGE_TYPE_ICE:
						case DAMAGE_TYPE_ELEC:
						case DAMAGE_TYPE_MAGIC: {
								if (dam > pAttacker->GetRankPoints(19))
									pAttacker->SetRankPoints(19, dam);
							}
							break;
						default:
							break;
					}
				}
			}
		}
#endif
	}
#if defined(ENABLE_WORLDBOSS)
	else if (IsWorldBossMonster()) {
		dam = 0;
	}

	if (IsWorldBossMonster()) {
		if (dam == 1 || dam == 2) {
			pAttacker->PointChange(POINT_WB_POINTS, dam, true);
		} else {
			SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
			return false;
		}
	}
#endif

#if defined(USE_BATTLEPASS)
	if (type != DAMAGE_TYPE_POISON && pAttacker)
	{
		if (IsPC())
			pAttacker->UpdateExtBattlePassMissionProgress(DAMAGE_PLAYER, dam, GetLevel());
		else
			pAttacker->UpdateExtBattlePassMissionProgress(DAMAGE_MONSTER, dam, GetRaceNum());
	}
#endif

	//
	// !!!!!!!!! 실제 HP를 줄이는 부분 !!!!!!!!!
	//
	if (!cannot_dead)
	{
#ifdef __DUNGEON_INFO_SYSTEM__
		if (!IsPC() && pAttacker && pAttacker->IsPC())
		{
			pAttacker->SetQuestDamage(GetRaceNum(), dam);
			pAttacker->SetQuestNPCID(GetVID());
			quest::CQuestManager::instance().QuestDamage(pAttacker->GetPlayerID(), GetRaceNum());
		}
#endif

		if (GetHP() - dam <= 0) // @fixme137
			dam = GetHP();

		PointChange(POINT_HP, -dam, false);
	}

	//puRest1.Pop();

	//PROF_UNIT puRest2("Rest2");
	if (pAttacker && dam > 0 && IsNPC())
	{
		//PROF_UNIT puRest20("Rest20");
		TDamageMap::iterator it = m_map_kDamage.find(pAttacker->GetVID());

		if (it == m_map_kDamage.end())
		{
			m_map_kDamage.insert(TDamageMap::value_type(pAttacker->GetVID(), TBattleInfo(dam, 0)));
			it = m_map_kDamage.find(pAttacker->GetVID());
		}
		else
		{
			it->second.iTotalDamage += dam;
		}
		//puRest20.Pop();

		//PROF_UNIT puRest21("Rest21");
#ifdef __DEFENSE_WAVE__
	if (GetRaceNum() != 20434)
	{
		StartRecoveryEvent();
	}
#else
		StartRecoveryEvent();
#endif
		//puRest21.Pop();

		//PROF_UNIT puRest22("Rest22");
		UpdateAggrPointEx(pAttacker, type, dam, it->second);
		//puRest22.Pop();
	}
	//puRest2.Pop();

	//PROF_UNIT puRest3("Rest3");
	if (GetHP() <= 0)
	{
		if (IsPC())
			Stun();

		if (pAttacker && !pAttacker->IsNPC())
			m_dwKillerPID = pAttacker->GetPlayerID();
		else
			m_dwKillerPID = 0;

		if (!IsPC())
			Dead(pAttacker);
	}

#ifdef __DEFENSE_WAVE__
	if (GetRaceNum() == 20434)
	{
		LPDUNGEON dungeon = GetDungeon();
		if (dungeon)
		{
			dungeon->UpdateMastHP();
			if (dungeon->GetMast()->GetHP() <= 0)
			{
				dungeon->KillAll();
				dungeon->ClearRegen();
				dungeon->Notice(909, "");
				dungeon->Notice(910, "");
				dungeon->ExitAllLobby((10240 + 828) * 100, (16640 + 1268) * 100);
			}
		}
	}
#endif

	return false;
}

void CHARACTER::DistributeHP(LPCHARACTER pkKiller)
{
	if (pkKiller->GetDungeon()) // 던젼내에선 만두가나오지않는다
		return;
}

#define ENABLE_NEWEXP_CALCULATION
#ifdef ENABLE_NEWEXP_CALCULATION
#define NEW_GET_LVDELTA(me, victim) aiPercentByDeltaLev[MINMAX(0, (victim + 15) - me, MAX_EXP_DELTA_OF_LEV - 1)]

typedef long double rate_t;
static void GiveExp(LPCHARACTER from, LPCHARACTER to, int iExp)
{
	if (!to || !from) {
		return;
	}

	if (iExp <= 0)
	{
		if (test_server) {
			to->ChatPacket(CHAT_TYPE_INFO, "exp(%d) overflow", iExp);
		}

		return;
	}

	rate_t lvFactor = static_cast<rate_t>(NEW_GET_LVDELTA(to->GetLevel(), from->GetLevel())) / 100.0L;
	iExp *= lvFactor;

	int iBaseExp = iExp;
	if (iBaseExp <= 0) {
		return;
	}

	rate_t rateFactor = 100;

	rateFactor += CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT);

#ifdef ENABLE_EVENT_MANAGER
	const auto event = CHARACTER_MANAGER::Instance().CheckEventIsActive(EXP_EVENT, to->GetEmpire());
	if (event != 0) {
		rateFactor += event->value[0];
	}
#endif

	if (to->IsEquipUniqueItem(UNIQUE_ITEM_LARBOR_MEDAL)) {
		rateFactor += 20;
	}

	if (to->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_EXP)) {
		rateFactor += 50;
	}

	if (to->GetPremiumRemainSeconds(PREMIUM_EXP) > 0) {
		rateFactor += 50;
	}

	if (to->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_EXP)) {
		rateFactor += 50;
	}

	int32_t iExpBonus = MIN(200, to->GetPoint(POINT_EXP_DOUBLE_BONUS));
	if (iExpBonus > 0) {
		rateFactor += iExpBonus;
	}

	rateFactor += to->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_EXP_BONUS);
	rateFactor += to->GetPoint(POINT_RAMADAN_CANDY_BONUS_EXP);
	rateFactor += to->GetPoint(POINT_MALL_EXPBONUS);
	rateFactor += to->GetPoint(POINT_EXP);
	rateFactor = rateFactor * static_cast<rate_t>(CHARACTER_MANAGER::instance().GetMobExpRate(to))/100.0L;

	iExp *= (rateFactor / 100.0L);
	if (test_server) {
		to->ChatPacket(CHAT_TYPE_INFO, "base_exp(%d) * rate(%Lf) = exp(%d)", iBaseExp, rateFactor/100.0L, iExp);
	}

	iExp = MIN(to->GetNextExp() / 10, iExp);
	iExp = AdjustExpByLevel(to, iExp);

	if (iExp <= 0)
	{
		if (test_server) {
			to->ChatPacket(CHAT_TYPE_INFO, "exp(%d) overflow", iExp);
		}

		return;
	}

#if defined(__NEWPET_SYSTEM__)
	CNewPetSystem* petSystemNew = to->GetNewPetSystem();
	if (petSystemNew && petSystemNew->GetLevel() < 
#if defined(ENABLE_NEW_PET_EDITS)
100
#else
120
#endif
	) {
		if ((petSystemNew->IsActivePet()) && (petSystemNew->GetLevelStep() < 4))
		{
			int tmpexp = iExp * 9 / 20;
			if (petSystemNew->SetExp(tmpexp, 0) == true) {
				iExp -= tmpexp;
			}
		}
	}
#endif

	if (test_server) {
		to->ChatPacket(CHAT_TYPE_INFO, "exp+minGNE+adjust(%d)", iExp);
	}

	to->PointChange(POINT_EXP, iExp, true);
	from->CreateFly(FLY_EXP, to);
#if defined(USE_BATTLEPASS)
	to->UpdateExtBattlePassMissionProgress(EXP_COLLECT, iExp, from->GetRaceNum());
#endif

	{
		LPCHARACTER you = to->GetMarryPartner();
		if (you)
		{
			DWORD dwUpdatePoint = (2000.0L / to->GetLevel()/to->GetLevel()/3) * iExp;

			if (to->GetPremiumRemainSeconds(PREMIUM_MARRIAGE_FAST) > 0 ||
				you->GetPremiumRemainSeconds(PREMIUM_MARRIAGE_FAST) > 0) {
					dwUpdatePoint *= 3;
				}

			marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(to->GetPlayerID());
			if (pMarriage && pMarriage->IsNear()) {
				pMarriage->Update(dwUpdatePoint);
			}
		}
	}
}
#else
static void GiveExp(LPCHARACTER from, LPCHARACTER to, int iExp)
{
	// 레벨차 경험치 가감비율
	iExp = CALCULATE_VALUE_LVDELTA(to->GetLevel(), from->GetLevel(), iExp);

	int iBaseExp = iExp;

	// 점술, 회사 경험치 이벤트 적용
#ifdef ENABLE_EVENT_MANAGER
	const auto event = CHARACTER_MANAGER::Instance().CheckEventIsActive(EXP_EVENT, to->GetEmpire());
	if(event != 0)
		iExp = iExp * (100 + (event->value[0] + CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT))) / 100;
	else
		iExp = iExp * (100 + CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT)) / 100;
#else
	iExp = iExp * (100 + CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT)) / 100;
#endif

	// 게임내 기본 제공되는 경험치 보너스
	{
		// 노동절 메달
		if (to->IsEquipUniqueItem(UNIQUE_ITEM_LARBOR_MEDAL))
			iExp += iExp * 20 /100;

		// 사귀타워 경험치 보너스
		if (to->GetMapIndex() >= 660000 && to->GetMapIndex() < 670000)
			iExp += iExp * 20 / 100; // 1.2배 (20%)

		// 아이템 경험치 두배 속성
		if (to->GetPoint(POINT_EXP_DOUBLE_BONUS))
			if (number(1, 100) <= to->GetPoint(POINT_EXP_DOUBLE_BONUS))
				iExp += iExp * 30 / 100; // 1.3배 (30%)

		// 경험의 반지 (2시간짜리)
		if (to->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_EXP))
			iExp += iExp * 50 / 100;
	}

	// 아이템 몰 판매 경험치 보너스
	{
		// 아이템 몰: 경험치 결제
		if (to->GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
		{
			iExp += (iExp * 50 / 100);
		}

		if (to->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_EXP) == true)
		{
			iExp += (iExp * 50 / 100);
		}

		iExp += iExp * to->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_EXP_BONUS) / 100;
	}

	iExp += (iExp * to->GetPoint(POINT_RAMADAN_CANDY_BONUS_EXP)/100);
	iExp += (iExp * to->GetPoint(POINT_MALL_EXPBONUS)/100);
	iExp += (iExp * to->GetPoint(POINT_EXP)/100);

	if (test_server)
	{
		sys_log(0, "Bonus Exp : Ramadan Candy: %d MallExp: %d PointExp: %d",
				to->GetPoint(POINT_RAMADAN_CANDY_BONUS_EXP),
				to->GetPoint(POINT_MALL_EXPBONUS),
				to->GetPoint(POINT_EXP)
			   );
	}

	// 기획측 조정값 2005.04.21 현재 85%
	iExp = iExp * CHARACTER_MANAGER::instance().GetMobExpRate(to) / 100;

	// 경험치 한번 획득량 제한
	iExp = MIN(to->GetNextExp() / 10, iExp);

	if (test_server)
	{
		if (quest::CQuestManager::instance().GetEventFlag("exp_bonus_log") && iBaseExp>0)
			to->ChatPacket(CHAT_TYPE_INFO, "exp bonus %d%%", (iExp-iBaseExp)*100/iBaseExp);
		to->ChatPacket(CHAT_TYPE_INFO, "exp(%d) base_exp(%d)", iExp, iBaseExp);
	}

	iExp = AdjustExpByLevel(to, iExp);

#if defined(__NEWPET_SYSTEM__)
	CNewPetSystem* petSystemNew = to->GetNewPetSystem();
	if (petSystemNew && petSystemNew->GetLevel() < 
#if defined(ENABLE_NEW_PET_EDITS)
100
#else
120
#endif
	) {
		if ((petSystemNew->IsActivePet()) && (petSystemNew->GetLevelStep() < 4))
		{
			int tmpexp = iExp * 9 / 20;
			if (petSystemNew->SetExp(tmpexp, 0) == true) {
				iExp = iExp - tmpexp;
			}
		}
	}
#endif

	to->PointChange(POINT_EXP, iExp, true);
	from->CreateFly(FLY_EXP, to);

	{
		LPCHARACTER you = to->GetMarryPartner();
		// 부부가 서로 파티중이면 금슬이 오른다
		if (you)
		{
			// 1억이 100%
			DWORD dwUpdatePoint = 2000*iExp/to->GetLevel()/to->GetLevel()/3;

			if (to->GetPremiumRemainSeconds(PREMIUM_MARRIAGE_FAST) > 0 ||
					you->GetPremiumRemainSeconds(PREMIUM_MARRIAGE_FAST) > 0)
				dwUpdatePoint = (DWORD)(dwUpdatePoint * 3);

			marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(to->GetPlayerID());

			// DIVORCE_NULL_BUG_FIX
			if (pMarriage && pMarriage->IsNear())
				pMarriage->Update(dwUpdatePoint);
			// END_OF_DIVORCE_NULL_BUG_FIX
		}
	}
}
#endif

namespace NPartyExpDistribute
{
	struct FPartyTotaler
	{
		int		total;
		int		member_count;
		int		x, y;

		FPartyTotaler(LPCHARACTER center)
			: total(0), member_count(0), x(center->GetX()), y(center->GetY())
		{};

		void operator () (LPCHARACTER ch)
		{
			if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
			{
				total += __GetPartyExpNP(ch->GetLevel());

				++member_count;
			}
		}
	};

	struct FPartyDistributor
	{
		int		total;
		LPCHARACTER	c;
		int		x, y;
		DWORD		_iExp;
		int		m_iMode;
		int		m_iMemberCount;

		FPartyDistributor(LPCHARACTER center, int member_count, int total, DWORD iExp, int iMode)
			: total(total), c(center), x(center->GetX()), y(center->GetY()), _iExp(iExp), m_iMode(iMode), m_iMemberCount(member_count)
			{
				if (m_iMemberCount == 0)
					m_iMemberCount = 1;
			};

		void operator () (LPCHARACTER ch)
		{
			if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
			{
#if defined(USE_SPECIAL_EXP_FLAG)
				if (ch->GetLevel() >= 120 &&
					!c->IsSpecialExp())
				{
					return;
				}
#endif


				DWORD iExp2 = 0;

				switch (m_iMode)
				{
					case PARTY_EXP_DISTRIBUTION_NON_PARITY:
						iExp2 = (DWORD) (_iExp * ((float)__GetPartyExpNP(ch->GetLevel())) / total);
						break;
					case PARTY_EXP_DISTRIBUTION_PARITY:
						iExp2 = _iExp / m_iMemberCount;
						break;
					default:
						sys_err("Unknown party exp distribution mode %d", m_iMode);
						return;
				}

				GiveExp(c, ch, iExp2);
			}
		}
	};
}

typedef struct SDamageInfo
{
	int iDam;
	LPCHARACTER pAttacker;
	LPPARTY pParty;

	void Clear()
	{
		pAttacker = NULL;
		pParty = NULL;
	}

	inline void Distribute(LPCHARACTER ch, int iExp)
	{
		if (pAttacker)
		{
#if defined(USE_SPECIAL_EXP_FLAG)
			if (pAttacker->GetLevel() >= 120 &&
				!ch->IsSpecialExp())
			{
				return;
			}
#endif

			GiveExp(ch, pAttacker, iExp);
		}
		else if (pParty)
		{
			NPartyExpDistribute::FPartyTotaler f(ch);
            pParty->ForEachOnMapMember(f, ch->GetMapIndex());

			if (pParty->IsPositionNearLeader(ch))
			{
				iExp = iExp * (100 + pParty->GetExpBonusPercent()) / 100;
			}

			if (pParty->GetExpCentralizeCharacter())
			{
				LPCHARACTER tch = pParty->GetExpCentralizeCharacter();
				if (DISTANCE_APPROX(ch->GetX() - tch->GetX(), ch->GetY() - tch->GetY()) <= PARTY_DEFAULT_RANGE)
				{
					int iExpCenteralize = (int) (iExp * 0.05f);
					iExp -= iExpCenteralize;

					GiveExp(ch, pParty->GetExpCentralizeCharacter(), iExpCenteralize);
				}
			}

			NPartyExpDistribute::FPartyDistributor fDist(ch, f.member_count, f.total, iExp, pParty->GetExpDistributionMode());
            pParty->ForEachOnMapMember(fDist, ch->GetMapIndex());
		}
	}
} TDamageInfo;

LPCHARACTER CHARACTER::DistributeExp()
{
	int iExpToDistribute = GetExp();
	if (iExpToDistribute <= 0)
	{
		return NULL;
	}

	int	iTotalDam = 0;
	LPCHARACTER pkChrMostAttacked = NULL;
	int iMostDam = 0;

	typedef std::vector<TDamageInfo> TDamageInfoTable;
	TDamageInfoTable damage_info_table;
	std::map<LPPARTY, TDamageInfo> map_party_damage;

	damage_info_table.reserve(m_map_kDamage.size());

	TDamageMap::iterator it = m_map_kDamage.begin();

	// 일단 주위에 없는 사람을 걸러 낸다. (50m)
	while (it != m_map_kDamage.end())
	{
		const VID & c_VID = it->first;
		int iDam = it->second.iTotalDamage;

		++it;

		LPCHARACTER pAttacker = CHARACTER_MANAGER::instance().Find(c_VID);

		// NPC가 때리기도 하나? -.-;
		if (!pAttacker || pAttacker->IsNPC() || DISTANCE_APPROX(GetX()-pAttacker->GetX(), GetY()-pAttacker->GetY())>5000)
			continue;

		iTotalDam += iDam;
		if (!pkChrMostAttacked || iDam > iMostDam)
		{
			pkChrMostAttacked = pAttacker;
			iMostDam = iDam;
		}

		if (pAttacker->GetParty())
		{
			std::map<LPPARTY, TDamageInfo>::iterator it = map_party_damage.find(pAttacker->GetParty());
			if (it == map_party_damage.end())
			{
				TDamageInfo di;
				di.iDam = iDam;
				di.pAttacker = NULL;
				di.pParty = pAttacker->GetParty();
				map_party_damage.insert(std::make_pair(di.pParty, di));
			}
			else
			{
				it->second.iDam += iDam;
			}
		}
		else
		{
			TDamageInfo di;

			di.iDam = iDam;
			di.pAttacker = pAttacker;
			di.pParty = NULL;

			//sys_log(0, "__ pq_damage %s %d", pAttacker->GetName(), iDam);
			//pq_damage.push(di);
			damage_info_table.push_back(di);
		}
	}

	for (std::map<LPPARTY, TDamageInfo>::iterator it = map_party_damage.begin(); it != map_party_damage.end(); ++it)
	{
		damage_info_table.push_back(it->second);
		//sys_log(0, "__ pq_damage_party [%u] %d", it->second.pParty->GetLeaderPID(), it->second.iDam);
	}

	SetExp(0);
	//m_map_kDamage.clear();

	if (iTotalDam == 0)	// 데미지 준게 0이면 리턴
		return NULL;

/*
	if (m_pkChrStone)	// 돌이 있을 경우 경험치의 반을 돌에게 넘긴다.
	{
		//sys_log(0, "__ Give half to Stone : %d", iExpToDistribute>>1);
		int iExp = iExpToDistribute >> 1;
		m_pkChrStone->SetExp(m_pkChrStone->GetExp() + iExp);
		iExpToDistribute -= iExp;
	}
*/

	sys_log(1, "%s total exp: %d, damage_info_table.size() == %d, TotalDam %d",
			GetName(), iExpToDistribute, damage_info_table.size(), iTotalDam);
	//sys_log(1, "%s total exp: %d, pq_damage.size() == %d, TotalDam %d",
	//GetName(), iExpToDistribute, pq_damage.size(), iTotalDam);

	if (damage_info_table.empty())
		return NULL;

	// 제일 데미지를 많이 준 사람이 HP 회복을 한다.
	DistributeHP(pkChrMostAttacked);	// 만두 시스템

	{
		// 제일 데미지를 많이 준 사람이나 파티가 총 경험치의 20% + 자기가 때린만큼의 경험치를 먹는다.
		TDamageInfoTable::iterator di = damage_info_table.begin();
		{
			TDamageInfoTable::iterator it;

			for (it = damage_info_table.begin(); it != damage_info_table.end();++it)
			{
				if (it->iDam > di->iDam)
					di = it;
			}
		}

		int	iExp = iExpToDistribute / 5;
		iExpToDistribute -= iExp;

		float fPercent = (float) di->iDam / iTotalDam;

		if (fPercent > 1.0f)
		{
			sys_err("DistributeExp percent over 1.0 (fPercent %f name %s)", fPercent, di->pAttacker->GetName());
			fPercent = 1.0f;
		}

		iExp += (int) (iExpToDistribute * fPercent);

		//sys_log(0, "%s given exp percent %.1f + 20 dam %d", GetName(), fPercent * 100.0f, di.iDam);

		di->Distribute(this, iExp);

		// 100% 다 먹었으면 리턴한다.
		if (fPercent == 1.0f)
			return pkChrMostAttacked;

		di->Clear();
	}

	{
		// 남은 80%의 경험치를 분배한다.
		TDamageInfoTable::iterator it;

		for (it = damage_info_table.begin(); it != damage_info_table.end(); ++it)
		{
			TDamageInfo & di = *it;

			float fPercent = (float) di.iDam / iTotalDam;

			if (fPercent > 1.0f)
			{
				sys_err("DistributeExp percent over 1.0 (fPercent %f name %s)", fPercent, di.pAttacker->GetName());
				fPercent = 1.0f;
			}

			//sys_log(0, "%s given exp percent %.1f dam %d", GetName(), fPercent * 100.0f, di.iDam);
			di.Distribute(this, (int) (iExpToDistribute * fPercent));
		}
	}

	return pkChrMostAttacked;
}

// 화살 개수를 리턴해 줌
int CHARACTER::GetArrowAndBow(LPITEM * ppkBow, LPITEM * ppkArrow, int iArrowCount/* = 1 */)
{
	LPITEM pkBow;

	if (!(pkBow = GetWear(WEAR_WEAPON)) || pkBow->GetProto()->bSubType != WEAPON_BOW)
	{
		return 0;
	}

	LPITEM pkArrow;

	if (!(pkArrow = GetWear(WEAR_ARROW)) || pkArrow->GetType() != ITEM_WEAPON ||
			pkArrow->GetProto()->bSubType != WEAPON_ARROW)
	{
		return 0;
	}

	iArrowCount = MIN(iArrowCount, pkArrow->GetCount());

	*ppkBow = pkBow;
	*ppkArrow = pkArrow;

	return iArrowCount;
}

void CHARACTER::UseArrow(LPITEM pkArrow, DWORD dwArrowCount)
{
	int iCount = pkArrow->GetCount();
	DWORD dwVnum = pkArrow->GetVnum();
#if !defined(__INFINITE_ARROW__)
	iCount = iCount - MIN(iCount, dwArrowCount);
#endif
	pkArrow->SetCount(iCount);

	if (iCount == 0)
	{
		LPITEM pkNewArrow = FindSpecifyItem(dwVnum);

		sys_log(0, "UseArrow : FindSpecifyItem %u %p", dwVnum, get_pointer(pkNewArrow));

		if (pkNewArrow)
			EquipItem(pkNewArrow);
	}
}

class CFuncShoot
{
	public:
		LPCHARACTER	m_me;
		BYTE		m_bType;
		bool		m_bSucceed;

		CFuncShoot(LPCHARACTER ch, BYTE bType) : m_me(ch), m_bType(bType), m_bSucceed(FALSE)
		{
		}

		void operator () (DWORD dwTargetVID)
		{
			if (m_bType > 1)
			{
				if (g_bSkillDisable)
					return;

				m_me->m_SkillUseInfo[m_bType].SetMainTargetVID(dwTargetVID);
				/*if (m_bType == SKILL_BIPABU || m_bType == SKILL_KWANKYEOK)
				  m_me->m_SkillUseInfo[m_bType].ResetHitCount();*/
			}

			LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(dwTargetVID);

			if (!pkVictim)
				return;

			// 공격 불가
			if (!battle_is_attackable(m_me, pkVictim))
				return;

			if (m_me->IsNPC())
			{
				if (DISTANCE_APPROX(m_me->GetX() - pkVictim->GetX(), m_me->GetY() - pkVictim->GetY()) > 5000)
					return;
			}

#if defined(ENABLE_FIX_WAITHACK)
			if (m_me->IsPC() && m_bType > 0 && m_me->IsSkillCooldown(m_bType, static_cast<float> (m_me->GetSkillPower(m_bType) / 100.0f))) {
				return;
			}
#endif

			if (m_me->IsPC())
			{
				if (m_me->IsBowMode())
				{
					const int distance = DISTANCE_APPROX(m_me->GetX() - pkVictim->GetX(), m_me->GetY() - pkVictim->GetY());
					if (distance > m_me->GetBowRange())
					{
						if (test_server)
							m_me->ChatPacket(7, "You cannot shoot %u (%s) because is too far (%d > %d)", static_cast<uint32_t>(pkVictim->GetVID()), pkVictim->GetName(), distance, m_me->GetBowRange());
						return;
					}
				}
			}

			LPITEM pkBow, pkArrow;
			bool unknownType = false;

			switch (m_bType)
			{
				case 0: // 일반활
					{
						int iDam = 0;

						if (m_me->IsPC())
						{
							if (m_me->GetJob() != JOB_ASSASSIN)
								return;

							if (0 == m_me->GetArrowAndBow(&pkBow, &pkArrow))
								return;

							if (m_me->GetSkillGroup() != 0)
								if (!m_me->IsNPC() && m_me->GetSkillGroup() != 2)
								{
									if (m_me->GetSP() < 5)
										return;

									m_me->PointChange(POINT_SP, -5);
								}

							iDam = CalcArrowDamage(m_me, pkVictim, pkBow, pkArrow);
							m_me->UseArrow(pkArrow, 1);

#ifdef __FIX_PRO_DAMAGE__
							if (m_me->CheckSyncPosition()) {
								iDam = 0;
								return;
							}
#endif
						}
						else
							iDam = CalcMeleeDamage(m_me, pkVictim);

						NormalAttackAffect(m_me, pkVictim);

						// 데미지 계산
						long lValue = pkVictim->GetPoint(POINT_RESIST_BOW);
#ifdef ENABLE_NEW_BONUS_TALISMAN
						lValue -= m_me->GetPoint(POINT_ATTBONUS_IRR_FRECCIA);
#endif
#ifdef ENABLE_NEW_COMMON_BONUSES
						lValue -= m_me->GetPoint(POINT_IRR_WEAPON_DEFENSE);
#endif
						lValue = lValue < 0 ? 0 : lValue;
						iDam = iDam * (100 - lValue) / 100;
#ifdef ENABLE_SOUL_SYSTEM // Arrow ninja
						iDam += m_me->GetSoulItemDamage(pkVictim, iDam, RED_SOUL);
#endif


						//sys_log(0, "%s arrow %s dam %d", m_me->GetName(), pkVictim->GetName(), iDam);

						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						pkVictim->Damage(m_me, iDam, DAMAGE_TYPE_NORMAL_RANGE);
						// 타격치 계산부 끝
					}
					break;

				case 1: // 일반 마법
					{
						int iDam;

						if (m_me->IsPC())
							return;

						iDam = CalcMagicDamage(m_me, pkVictim);

						NormalAttackAffect(m_me, pkVictim);

#if defined(ENABLE_APPLY_MAGICRES_REDUCTION)
						int32_t resist_magic = MINMAX(0, pkVictim->GetPoint(POINT_RESIST_MAGIC), 100);
						if (resist_magic > 0) {
							resist_magic = m_me->GetJob() == JOB_SHAMAN ? int32_t(float(resist_magic) / 1.4f) : resist_magic;
							const int32_t resist_magic_reduction = MINMAX(0, (m_me->GetJob() == JOB_SURA) ? m_me->GetPoint(POINT_RESIST_MAGIC_REDUCTION) / 2 : m_me->GetPoint(POINT_RESIST_MAGIC_REDUCTION), 50);
							const int32_t total_res_magic = MINMAX(0, resist_magic - resist_magic_reduction, 100);
							iDam = iDam * (100 - total_res_magic) / 100;
						}
#else
						iDam = iDam * (100 - (int32_t)(pkVictim->GetPoint(POINT_RESIST_MAGIC) / 2)) / 100;
#endif

						//sys_log(0, "%s arrow %s dam %d", m_me->GetName(), pkVictim->GetName(), iDam);

						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						pkVictim->Damage(m_me, iDam, DAMAGE_TYPE_MAGIC);
						// 타격치 계산부 끝
					}
					break;

				case SKILL_YEONSA:	// 연사
					{
						//int iUseArrow = 2 + (m_me->GetSkillPower(SKILL_YEONSA) *6/100);
						int iUseArrow = 1;

						// 토탈만 계산하는경우
						{
							if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
							{
								m_me->OnMove(true);
								pkVictim->OnMove();

								if (pkVictim->CanBeginFight())
									pkVictim->BeginFight(m_me);

								m_me->ComputeSkill(m_bType, pkVictim);
								m_me->UseArrow(pkArrow, iUseArrow);

								if (pkVictim->IsDead())
									break;

							}
							else
								break;
						}
					}
					break;


				case SKILL_KWANKYEOK:
					{
						int iUseArrow = 1;

						if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
						{
							m_me->OnMove(true);
							pkVictim->OnMove();

							if (pkVictim->CanBeginFight())
								pkVictim->BeginFight(m_me);

							sys_log(0, "%s kwankeyok %s", m_me->GetName(), pkVictim->GetName());
							m_me->ComputeSkill(m_bType, pkVictim);
							m_me->UseArrow(pkArrow, iUseArrow);
						}
					}
					break;

				case SKILL_GIGUNG:
					{
						int iUseArrow = 1;
						if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
						{
							m_me->OnMove(true);
							pkVictim->OnMove();

							if (pkVictim->CanBeginFight())
							{
								pkVictim->BeginFight(m_me);
							}

							sys_log(0, "%s gigung %s", m_me->GetName(), pkVictim->GetName());

							m_me->ComputeSkill(m_bType, pkVictim);
							m_me->UseArrow(pkArrow, iUseArrow);
						}
					}

					break;
				case SKILL_HWAJO:
					{
						int iUseArrow = 1;
						if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
						{
							m_me->OnMove(true);
							pkVictim->OnMove();

							if (pkVictim->CanBeginFight())
								pkVictim->BeginFight(m_me);

							sys_log(0, "%s hwajo %s", m_me->GetName(), pkVictim->GetName());
							m_me->ComputeSkill(m_bType, pkVictim);
							m_me->UseArrow(pkArrow, iUseArrow);
						}
					}

					break;

				case SKILL_HORSE_WILDATTACK_RANGE:
					{
						int iUseArrow = 1;
						if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
						{
							m_me->OnMove(true);
							pkVictim->OnMove();

							if (pkVictim->CanBeginFight())
								pkVictim->BeginFight(m_me);

							sys_log(0, "%s horse_wildattack %s", m_me->GetName(), pkVictim->GetName());
							m_me->ComputeSkill(m_bType, pkVictim);
							m_me->UseArrow(pkArrow, iUseArrow);
						}
					}

					break;

				case SKILL_MARYUNG:
					//case SKILL_GUMHWAN:
				case SKILL_TUSOK:
				case SKILL_BIPABU:
				case SKILL_NOEJEON:
				case SKILL_GEOMPUNG:
				case SKILL_SANGONG:
				case SKILL_MAHWAN:
				case SKILL_PABEOB:
                case SKILL_PAERYONG:
					//case SKILL_CURSE:
					{
						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						sys_log(0, "%s - Skill %d -> %s", m_me->GetName(), m_bType, pkVictim->GetName());
						m_me->ComputeSkill(m_bType, pkVictim);
					}
					break;

				case SKILL_CHAIN:
					{
						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						sys_log(0, "%s - Skill %d -> %s", m_me->GetName(), m_bType, pkVictim->GetName());
						m_me->ComputeSkill(m_bType, pkVictim);

						// TODO 여러명에게 슉 슉 슉 하기
					}
					break;
				case SKILL_YONGBI:
					{
						m_me->OnMove(true);
					}
					break;
					/*case SKILL_BUDONG:
					  {
					  m_me->OnMove(true);
					  pkVictim->OnMove();

					  DWORD * pdw;
					  DWORD dwEI = AllocEventInfo(sizeof(DWORD) * 2, &pdw);
					  pdw[0] = m_me->GetVID();
					  pdw[1] = pkVictim->GetVID();

					  event_create(budong_event_func, dwEI, PASSES_PER_SEC(1));
					  }
					  break;*/

				default:
					sys_err("CFuncShoot: I don't know this type [%d] of range attack.", (int) m_bType);
					unknownType = true;
					break;
			}

			m_bSucceed = unknownType ? false : true;
		}
};

bool CHARACTER::Shoot(BYTE bType)
{
	sys_log(1, "Shoot %s type %u flyTargets.size %zu", GetName(), bType, m_vec_dwFlyTargets.size());

	const LPDESC d = GetDesc();

	if (!CanMove() ||
		IsObserverMode() ||
		(d && d->IsDisconnectEvent()))
	{
		return false;
	}

	CFuncShoot f(this, bType);

	if (m_dwFlyTargetID != 0)
	{
		f(m_dwFlyTargetID);
		m_dwFlyTargetID = 0;

		if (f.m_bSucceed)
		{
			if (d && bType == 0)
			{
				if (IsShootSpeedHack())
				{
					LogManager::Instance().HackLog("SHOOT_SPEED_HACK", this);
//					d->DelayedDisconnect(number(5, 10));
					return false;
				}
			}
		}

		return f.m_bSucceed;
	}

	if (bType == 0)
	{
		return false;
	}

	uint32_t dwCount = 0;

	for (const auto vid : m_vec_dwFlyTargets)
	{
		if (dwCount >= EBattleRange::BATTLE_SHOOT_MAX_COUNT)
		{
			LogManager::Instance().HackLog("SHOOT_HACK_COUNT_OVERFLOW", this);
//			d->DelayedDisconnect(number(5, 10));
			return false;
		}

		f(vid);
		if (f.m_bSucceed)
			dwCount++;
	}

	m_vec_dwFlyTargets.clear();
	return dwCount != 0;
}

void CHARACTER::FlyTarget(DWORD dwTargetVID, long x, long y, BYTE bHeader)
{
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(dwTargetVID);
	TPacketGCFlyTargeting pack;

	//pack.bHeader	= HEADER_GC_FLY_TARGETING;
	pack.bHeader	= (bHeader == HEADER_CG_FLY_TARGETING) ? HEADER_GC_FLY_TARGETING : HEADER_GC_ADD_FLY_TARGETING;
	pack.dwShooterVID	= GetVID();

	if (pkVictim)
	{
		if (IsPC())
		{
			if (!IsVictimInView(pkVictim) || !battle_is_attackable(this, pkVictim))
				return;
		}

		pack.dwTargetVID = pkVictim->GetVID();
		pack.x = pkVictim->GetX();
		pack.y = pkVictim->GetY();

		if (bHeader == HEADER_CG_FLY_TARGETING)
		{
			m_dwFlyTargetID = dwTargetVID;
			m_vec_dwFlyTargets.clear();
		}
		else
			m_vec_dwFlyTargets.emplace(dwTargetVID);
	}
	else
	{
		pack.dwTargetVID = 0;
		pack.x = x;
		pack.y = y;
	}

	sys_log(1, "FlyTarget %s vid %d x %d y %d", GetName(), pack.dwTargetVID, pack.x, pack.y);
	PacketAround(&pack, sizeof(pack), this);
}

LPCHARACTER CHARACTER::GetNearestVictim(LPCHARACTER pkChr)
{
	if (NULL == pkChr)
		pkChr = this;

	float fMinDist = 99999.0f;
	LPCHARACTER pkVictim = NULL;

	TDamageMap::iterator it = m_map_kDamage.begin();

	// 일단 주위에 없는 사람을 걸러 낸다.
	while (it != m_map_kDamage.end())
	{
		const VID & c_VID = it->first;
		++it;

		LPCHARACTER pAttacker = CHARACTER_MANAGER::instance().Find(c_VID);

		if (!pAttacker)
			continue;

		if (pAttacker->IsAffectFlag(AFF_EUNHYUNG) ||
				pAttacker->IsAffectFlag(AFF_INVISIBILITY) ||
				pAttacker->IsAffectFlag(AFF_REVIVE_INVISIBLE))
			continue;

		float fDist = DISTANCE_APPROX(pAttacker->GetX() - pkChr->GetX(), pAttacker->GetY() - pkChr->GetY());

		if (fDist < fMinDist)
		{
			pkVictim = pAttacker;
			fMinDist = fDist;
		}
	}

	return pkVictim;
}

void CHARACTER::SetVictim(LPCHARACTER pkVictim)
{
	if (!pkVictim)
	{
		if (0 != (DWORD)m_kVIDVictim)
			MonsterLog("공격 대상을 해제");

		m_kVIDVictim.Reset();
		battle_end(this);
	}
	else
	{
		if (m_kVIDVictim != pkVictim->GetVID())
			MonsterLog("공격 대상을 설정: %s", pkVictim->GetName());

		m_kVIDVictim = pkVictim->GetVID();
		m_dwLastVictimSetTime = get_dword_time();
	}
}

LPCHARACTER CHARACTER::GetVictim() const
{
	return CHARACTER_MANAGER::instance().Find(m_kVIDVictim);
}

LPCHARACTER CHARACTER::GetProtege() const // 보호해야 할 대상을 리턴
{
	if (m_pkChrStone)
		return m_pkChrStone;

	if (m_pkParty)
		return m_pkParty->GetLeader();

	return NULL;
}

int CHARACTER::GetAlignment() const
{
	return m_iAlignment;
}

int CHARACTER::GetRealAlignment() const
{
	return m_iRealAlignment;
}

void CHARACTER::ShowAlignment(bool bShow)
{
	if (bShow)
	{
		if (m_iAlignment != m_iRealAlignment)
		{
			m_iAlignment = m_iRealAlignment;
			UpdatePacket();
		}
	}
	else
	{
		if (m_iAlignment != 0)
		{
			m_iAlignment = 0;
			UpdatePacket();
		}
	}
}

void CHARACTER::UpdateAlignment(int iAmount)
{
	bool bShow = false;

	if (m_iAlignment == m_iRealAlignment)
		bShow = true;

	int i = m_iAlignment / 10;

	m_iRealAlignment = MINMAX(-200000, m_iRealAlignment + iAmount, 200000);

	if (bShow)
	{
		OnUpdateAlignment();

		m_iAlignment = m_iRealAlignment;

		if (i != m_iAlignment / 10)
			UpdatePacket();
	}
}

void CHARACTER::OnUpdateAlignment()
{
#ifdef ENABLE_AFFECT_ALIGNMENT

	if (m_iRealAlignment == 200000)
	{
		if (const auto aff = FindAffect(AFFECT_EXPAND_ALIGNMENT, POINT_ATTBONUS_HUMAN))
			RemoveAffect(aff);

		if (const auto aff = FindAffect(AFFECT_EXPAND_ALIGNMENT, POINT_MAX_HP))
			RemoveAffect(aff);

		if (!FindAffect(AFFECT_EXPAND_ALIGNMENT, POINT_ATTBONUS_MONSTER))
			AddAffect(AFFECT_EXPAND_ALIGNMENT, POINT_ATTBONUS_MONSTER, 25, 0, INFINITE_AFFECT_DURATION, 0, false);
	}
	else if (m_iRealAlignment <= -150000)
	{
		if (const auto aff = FindAffect(AFFECT_EXPAND_ALIGNMENT, POINT_ATTBONUS_MONSTER))
			RemoveAffect(aff);

		if (!FindAffect(AFFECT_EXPAND_ALIGNMENT, POINT_ATTBONUS_HUMAN))
			AddAffect(AFFECT_EXPAND_ALIGNMENT, POINT_ATTBONUS_HUMAN, 10, 0, INFINITE_AFFECT_DURATION, 0, false);

		if (!FindAffect(AFFECT_EXPAND_ALIGNMENT, POINT_MAX_HP))
			AddAffect(AFFECT_EXPAND_ALIGNMENT, POINT_MAX_HP, 1000, 0, INFINITE_AFFECT_DURATION, 0, false);
	}
	else
	{
		RemoveAffect(AFFECT_EXPAND_ALIGNMENT);
	}
#endif
}

void CHARACTER::SetKillerMode(bool isOn)
{
	if ((isOn ? ADD_CHARACTER_STATE_KILLER : 0) == IS_SET(m_bAddChrState, ADD_CHARACTER_STATE_KILLER))
		return;

	if (isOn)
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_KILLER);
	else
		REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_KILLER);

	m_iKillerModePulse = thecore_pulse();
	UpdatePacket();
	sys_log(0, "SetKillerMode Update %s[%d]", GetName(), GetPlayerID());
}

bool CHARACTER::IsKillerMode() const
{
	return IS_SET(m_bAddChrState, ADD_CHARACTER_STATE_KILLER);
}

void CHARACTER::UpdateKillerMode()
{
	if (!IsKillerMode())
		return;

	if (thecore_pulse() - m_iKillerModePulse >= PASSES_PER_SEC(30))
		SetKillerMode(false);
}

void CHARACTER::SetPKMode(BYTE bPKMode)
{
	if (bPKMode >= PK_MODE_MAX_NUM)
		return;

	if (m_bPKMode == bPKMode)
		return;

	if (bPKMode == PK_MODE_GUILD && !GetGuild())
		bPKMode = PK_MODE_FREE;

	m_bPKMode = bPKMode;
	UpdatePacket();
	sys_log(0, "PK_MODE: %s %d", GetName(), m_bPKMode);
}

BYTE CHARACTER::GetPKMode() const
{
	return m_bPKMode;
}

struct FuncForgetMyAttacker
{
	LPCHARACTER m_ch;
	FuncForgetMyAttacker(LPCHARACTER ch)
	{
		m_ch = ch;
	}
	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch->IsPC())
				return;
			if (ch->m_kVIDVictim == m_ch->GetVID())
				ch->SetVictim(NULL);
		}
	}
};

struct FuncAggregateMonster
{
	LPCHARACTER m_ch;
	FuncAggregateMonster(LPCHARACTER ch)
	{
		m_ch = ch;
	}
	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch->IsPC())
				return;
			if (!ch->IsMonster())
				return;
			if (ch->GetVictim())
				return;

			//if (number(1, 100) <= 50) // 임시로 50% 확률로 적을 끌어온다

			int max_distance = 5000;
			
			if (m_ch->IsPremiumPlayer())
				max_distance *= 2;

			if (DISTANCE_APPROX(ch->GetX() - m_ch->GetX(), ch->GetY() - m_ch->GetY()) < max_distance)
				if (ch->CanBeginFight())
					ch->BeginFight(m_ch);
		}
	}
};

struct FuncAggregateMonster2
{
	LPCHARACTER m_ch;
	FuncAggregateMonster2(LPCHARACTER ch)
	{
		m_ch = ch;
	}
	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch->IsPC())
				return;
			if (!ch->IsMonster())
				return;
			if (ch->GetVictim())
				return;

			//if (number(1, 100) <= 50) // 임시로 50% 확률로 적을 끌어온다
			if (DISTANCE_APPROX(ch->GetX() - m_ch->GetX(), ch->GetY() - m_ch->GetY()) < 9000)
				if (ch->CanBeginFight())
					ch->BeginFight(m_ch);
		}
	}
};

struct FuncAttractRanger
{
	LPCHARACTER m_ch;
	FuncAttractRanger(LPCHARACTER ch)
	{
		m_ch = ch;
	}

	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch->IsPC())
				return;
			if (!ch->IsMonster())
				return;
			if (ch->GetVictim() && ch->GetVictim() != m_ch)
				return;
			if (ch->GetMobAttackRange() > 150)
			{
				int iNewRange = 150;//(int)(ch->GetMobAttackRange() * 0.2);
				if (iNewRange < 150)
					iNewRange = 150;

				ch->AddAffect(AFFECT_BOW_DISTANCE, POINT_BOW_DISTANCE, iNewRange - ch->GetMobAttackRange(), AFF_NONE, 3*60, 0, false);
			}
		}
	}
};

struct FuncPullMonster
{
	LPCHARACTER m_ch;
	int m_iLength;
	FuncPullMonster(LPCHARACTER ch, int iLength = 300)
	{
		m_ch = ch;
		m_iLength = iLength;
	}

	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch->IsPC())
				return;
			if (!ch->IsMonster())
				return;
			//if (ch->GetVictim() && ch->GetVictim() != m_ch)
			//return;
			float fDist = DISTANCE_APPROX(m_ch->GetX() - ch->GetX(), m_ch->GetY() - ch->GetY());
			if (fDist > 3000 || fDist < 100)
				return;

			float fNewDist = fDist - m_iLength;
			if (fNewDist < 100)
				fNewDist = 100;

			float degree = GetDegreeFromPositionXY(ch->GetX(), ch->GetY(), m_ch->GetX(), m_ch->GetY());
			float fx;
			float fy;

			GetDeltaByDegree(degree, fDist - fNewDist, &fx, &fy);
			long tx = (long)(ch->GetX() + fx);
			long ty = (long)(ch->GetY() + fy);

			ch->Sync(tx, ty);
			ch->Goto(tx, ty);
			ch->CalculateMoveDuration();

			ch->SyncPacket();
		}
	}
};

void CHARACTER::ForgetMyAttacker()
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncForgetMyAttacker f(this);
		pSec->ForEachAround(f);
	}
	ReviveInvisible(5);
}

void CHARACTER::AggregateMonster()
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncAggregateMonster f(this);
		pSec->ForEachAround(f);
	}
}

void CHARACTER::AggregateMonster2()
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncAggregateMonster2 f(this);
		pSec->ForEachAround(f);
	}
}

void CHARACTER::AttractRanger()
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncAttractRanger f(this);
		pSec->ForEachAround(f);
	}
}

void CHARACTER::PullMonster()
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncPullMonster f(this);
		pSec->ForEachAround(f);
	}
}

void CHARACTER::UpdateAggrPointEx(LPCHARACTER pAttacker, EDamageType type, int dam, CHARACTER::TBattleInfo & info)
{
	// 특정 공격타입에 따라 더 올라간다
	switch (type)
	{
		case DAMAGE_TYPE_NORMAL_RANGE:
			dam = (int) (dam*1.2f);
			break;

		case DAMAGE_TYPE_RANGE:
			dam = (int) (dam*1.5f);
			break;

		case DAMAGE_TYPE_MAGIC:
			dam = (int) (dam*1.2f);
			break;

		default:
			break;
	}

	// 공격자가 현재 대상인 경우 보너스를 준다.
	if (pAttacker == GetVictim())
		dam = (int) (dam * 1.2f);

	info.iAggro += dam;

	if (info.iAggro < 0)
		info.iAggro = 0;

	//sys_log(0, "UpdateAggrPointEx for %s by %s dam %d total %d", GetName(), pAttacker->GetName(), dam, total);
	if (GetParty() && dam > 0 && type != DAMAGE_TYPE_SPECIAL)
	{
		LPPARTY pParty = GetParty();

		// 리더인 경우 영향력이 좀더 강하다
		int iPartyAggroDist = dam;

		if (pParty->GetLeaderPID() == GetVID())
			iPartyAggroDist /= 2;
		else
			iPartyAggroDist /= 3;

		pParty->SendMessage(this, PM_AGGRO_INCREASE, iPartyAggroDist, pAttacker->GetVID());
	}

	ChangeVictimByAggro(info.iAggro, pAttacker);
}

void CHARACTER::UpdateAggrPoint(LPCHARACTER pAttacker, EDamageType type, int dam)
{
	if (IsDead() || IsStun())
		return;

	TDamageMap::iterator it = m_map_kDamage.find(pAttacker->GetVID());

	if (it == m_map_kDamage.end())
	{
		m_map_kDamage.insert(TDamageMap::value_type(pAttacker->GetVID(), TBattleInfo(0, dam)));
		it = m_map_kDamage.find(pAttacker->GetVID());
	}

	UpdateAggrPointEx(pAttacker, type, dam, it->second);
}

void CHARACTER::ChangeVictimByAggro(int iNewAggro, LPCHARACTER pNewVictim)
{
	if (get_dword_time() - m_dwLastVictimSetTime < 3000) // 3초는 기다려야한다
		return;

	if (pNewVictim == GetVictim())
	{
		if (m_iMaxAggro < iNewAggro)
		{
			m_iMaxAggro = iNewAggro;
			return;
		}

		// Aggro가 감소한 경우
		TDamageMap::iterator it;
		TDamageMap::iterator itFind = m_map_kDamage.end();

		for (it = m_map_kDamage.begin(); it != m_map_kDamage.end(); ++it)
		{
			if (it->second.iAggro > iNewAggro)
			{
				LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(it->first);

				if (ch && !ch->IsDead() && DISTANCE_APPROX(ch->GetX() - GetX(), ch->GetY() - GetY()) < 5000)
				{
					itFind = it;
					iNewAggro = it->second.iAggro;
				}
			}
		}

		if (itFind != m_map_kDamage.end())
		{
			m_iMaxAggro = iNewAggro;
#ifdef __DEFENSE_WAVE__
			if (!IsDefanceWaweMastAttackMob(GetRaceNum()))
			{
				SetVictim(CHARACTER_MANAGER::instance().Find(itFind->first));
			}
#else
			SetVictim(CHARACTER_MANAGER::instance().Find(itFind->first));
#endif
			m_dwStateDuration = 1;
		}
	}
	else
	{
		if (m_iMaxAggro < iNewAggro)
		{
			m_iMaxAggro = iNewAggro;
#ifdef __DEFENSE_WAVE__
			if (!IsDefanceWaweMastAttackMob(GetRaceNum()))
			{
				SetVictim(pNewVictim);
			}
#else
			SetVictim(pNewVictim);
#endif
			m_dwStateDuration = 1;
		}
	}
}

bool CHARACTER::IsShootSpeedHack(BYTE bSec, BYTE bCount)
{
	const auto pulse = thecore_pulse();
	if (pulse > (GetShootSpeedPulse() + PASSES_PER_SEC(bSec)))
	{
		SetShootSpeedCount(0);
		SetShootSpeedPulse(pulse);
	}
	return IncreaseShootSpeedCount() >= bCount;
}

#ifdef __FIX_PRO_DAMAGE__
void CHARACTER::SetSyncPosition(long x, long y)
{
	long mx = GetX();
	long my = GetY();

	float fDist = DISTANCE_SQRT(mx - x, my - y);
	float motionSpeed = GetMoveMotionSpeed();

    int32_t iLimit = GetLimitPoint(POINT_MOV_SPEED) < 200 ? 200 : GetLimitPoint(POINT_MOV_SPEED);

	DWORD mduration = CalculateDuration(iLimit, (int32_t)((fDist / motionSpeed) * 1000.0f));
	DWORD mstart = get_dword_time();

	sync_hack = mstart + mduration;
}

bool CHARACTER::CheckSyncPosition(bool sync_check)
{
    uint32_t dwTime = get_dword_time();
	if (sync_check)
	{
		if (dwTime > sync_time)
		{
			int sync = sync_count;
			sync_count = 0;
			sync_time = dwTime + 1000;

			if (sync > 200) {
//			if (sync > 160) {
				sys_log(0, "#(HACK)# MOVE: %s sync hack (count: %d) Riding(%d)", GetName(), sync, IsRiding());

				if (test_server)
				{
					ChatPacket(CHAT_TYPE_INFO, "#(HACK)# sync hack count = %d", sync);
				}

				Show(GetMapIndex(), GetX(), GetY(), GetZ());
				Stop();
				return true;
			}
		}

		return false;
	}

	if (dwTime < sync_hack)
	{
        if ((sync_hack - dwTime) <= 350)
        {
            return false;
        }

		return true;
	}

	return false;
}
#endif
