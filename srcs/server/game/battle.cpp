#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "desc.h"
#include "char.h"
#include "char_manager.h"
#include "battle.h"
#include "item.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "vector.h"
#include "packet.h"
#include "pvp.h"
#include "profiler.h"
#include "guild.h"
#include "affect.h"
#include "unique_item.h"
#include "lua_incl.h"
#include "arena.h"
#include "sectree.h"
#include "ani.h"
#include "locale_service.h"
#include "../common/CommonDefines.h"
#include "log.h"
#if defined(ENABLE_ULTIMATE_REGEN)
#include "new_mob_timer.h"
#endif

int battle_hit(LPCHARACTER ch, LPCHARACTER victim, int & iRetDam);

#if !defined(ENABLE_NEW_ANTICHEAT_RULES)
bool battle_distance_valid_by_xy(long x, long y, long tx, long ty)
{
	long distance = DISTANCE_APPROX(x - tx, y - ty);

	if (distance > 170)
		return false;

	return true;
}
#endif

bool battle_distance_valid(LPCHARACTER ch, LPCHARACTER victim)
{
#if defined(ENABLE_NEW_ANTICHEAT_RULES)
	if (!ch || !victim) {
		return false;
	}

	long x = ch->GetX(), y = ch->GetY(), tx = victim->GetX(), ty = victim->GetY();
	long distance = DISTANCE_APPROX(tx - x, ty - y);

	long limit;

	uint32_t raceNum = victim->GetRaceNum();

	if (victim->IsPC()) {
#if defined(ENABLE_PVP_RENEWAL)
		limit = 450;
#else
		limit = 420;
#endif
	}
	else
	{
		if (victim->IsStone())
		{
			limit = ch->IsRiding() ? 350 : 260;

#if defined(ENABLE_HALLOWEEN_EVENT_2022)
			if (raceNum == 8731 || 
				raceNum == 8732 || 
				raceNum == 8733) {
				limit += 100;
			}
#endif
#if defined(__DEFENSE_WAVE__)
			if (raceNum == 3965) {
				limit += 100;
			}
#endif
		}
		else if (victim->IsMonster())
		{
			limit = ch->IsRiding() ? 400 : 300;

#if defined(ENABLE_WORLDBOSS)
			if (victim->IsWorldBossMonster())
			{
				limit += 500;
			}
#endif

#if defined(__DEFENSE_WAVE__)
			if (raceNum == 3960 || 
				raceNum == 3961 || 
				raceNum == 3962) {
				limit += 400;
			}
#endif

			if (raceNum == 768)
			{
				limit += 300;
			}
			else if (raceNum == 4524)
			{
				limit += 350;
			}
			else if (raceNum == 4525)
			{
				limit += 200;
			}
		}
		else
		{
			limit = 170;
		}
	}

	if (ch->IsPC() && victim->IsPC() && distance == 0) {
		return true;
	}

	return distance == 0 || distance > limit ? false : true;
#else
	return battle_distance_valid_by_xy(ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY());
#endif
}

void timed_event_cancel(LPCHARACTER ch)
{
	if (ch->m_pkTimedEvent)
	{
		event_cancel(&ch->m_pkTimedEvent);

#if defined(ENABLE_TEXTS_RENEWAL)
		if (ch->IsPC()) {
			ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_FightCancelled");
			ch->ChatPacketNew(CHAT_TYPE_INFO, 482, "");
		}
#else
		if (ch->IsPC()) {
			ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_FightCancelled");
		}
#endif
		return;
	}

	/* RECALL_DELAY
	   차후 전투로 인해 귀환부 딜레이가 취소 되어야 할 경우 주석 해제
	   if (ch->m_pk_RecallEvent)
	   {
	   event_cancel(&ch->m_pkRecallEvent);
	   return true;
	   }
	   END_OF_RECALL_DELAY */
}

bool battle_is_attackable(LPCHARACTER ch, LPCHARACTER victim)
{
	if (!ch || !victim)
		return false;

	if (ch == victim)
		return false;

	if (ch->IsStun() || ch->IsDead() || victim->IsDead())
		return false;

	if (victim->GetMyShop())
		return false;

	{
		LPSECTREE sectree = ch->GetSectree();
		if (sectree && sectree->IsAttr(ch->GetX(), ch->GetY(), ATTR_BANPK))
			return false;

		sectree = victim->GetSectree();
		if (sectree && sectree->IsAttr(victim->GetX(), victim->GetY(), ATTR_BANPK))
			return false;
	}

#if defined(ENABLE_ULTIMATE_REGEN)
	if (!CNewMobTimer::Instance().CheckDamage(ch, victim)) {
		return false;
	}
#endif

	if (ch->IsPC() && victim->IsPC())
	{
		CGuild* g1 = ch->GetGuild();
		CGuild* g2 = victim->GetGuild();

		if (g1 && g2)
		{
			if (g1->UnderWar(g2->GetID())) {
				ch->SetLastAttackPulse(get_global_time() + 5);
				return true;
			}
		}
	}

	if (CArenaManager::instance().CanAttack(ch, victim) == true) {
		ch->SetLastAttackPulse(get_global_time() + 5);
		return true;
	}

#ifdef __DEFENSE_WAVE__
	if (victim->GetRaceNum() == 20434 && ch->IsMonster())
	{
		return true;
	}
#endif

	const auto ret = CPVPManager::instance().CanAttack(ch, victim);
	if (ret && ch->IsPC() && victim->IsPC()) {
		ch->SetLastAttackPulse(get_global_time() + 5);
	}

	return ret;
}

int battle_melee_attack(LPCHARACTER ch, LPCHARACTER victim)
{
	if (test_server && ch->IsPC())
		sys_log(0, "battle_melee_attack : [%s] attack to [%s]", ch->GetName(), victim->GetName());

	if (!victim || ch == victim)
		return BATTLE_NONE;

	if (test_server&&ch->IsPC())
		sys_log(0, "battle_melee_attack : [%s] attack to [%s]", ch->GetName(), victim->GetName());

	if (!battle_is_attackable(ch, victim))
		return BATTLE_NONE;

	if (test_server&&ch->IsPC())
		sys_log(0, "battle_melee_attack : [%s] attack to [%s]", ch->GetName(), victim->GetName());

	int distance = DISTANCE_APPROX(ch->GetX() - victim->GetX(), ch->GetY() - victim->GetY());

	if (!victim->IsBuilding())
	{
#if defined(ENABLE_PVP_RENEWAL)
		int max;
		bool bMaxRange = true;
		if (ch->IsPC() && victim->IsPC()) {
			max = 450;
			bMaxRange = false;
		} else {
			max = 420;
		}
#else
		int max = 420;
#endif
		if (false == ch->IsPC())
		{
			max = (int) (ch->GetMobAttackRange() * 1.15f);
		}
		else
		{
			if (false == victim->IsPC() && BATTLE_TYPE_MELEE == victim->GetMobBattleType())
				max = MAX(350, (int) (victim->GetMobAttackRange() * 1.15f));
		}

#ifdef __DEFENSE_WAVE__
		if (ch->IsPC() && (victim->GetRaceNum() == 3960 || victim->GetRaceNum() == 3961 || victim->GetRaceNum() == 3962))
		{
			max += 400;
		}
#endif

#if defined(ENABLE_PVP_RENEWAL)
		if (bMaxRange && distance > max) {
			if (test_server) {
				sys_log(0, "VICTIM_FAR: %s distance: %d max: %d", ch->GetName(), distance, max);
			}

			return BATTLE_NONE;
		}
#else
		if (distance > max)
		{
			if (test_server)
				sys_log(0, "VICTIM_FAR: %s distance: %d max: %d", ch->GetName(), distance, max);

			return BATTLE_NONE;
		}
#endif
	}

	timed_event_cancel(ch);
	timed_event_cancel(victim);

	ch->SetPosition(POS_FIGHTING);
	ch->SetVictim(victim);

	const PIXEL_POSITION & vpos = victim->GetXYZ();
	ch->SetRotationToXY(vpos.x, vpos.y);

	int dam;
	int ret = battle_hit(ch, victim, dam);
	return (ret);
}

void battle_end_ex(LPCHARACTER ch)
{
	if (ch->IsPosition(POS_FIGHTING))
		ch->SetPosition(POS_STANDING);
}

void battle_end(LPCHARACTER ch)
{
	battle_end_ex(ch);
}

// AG = Attack Grade
// AL = Attack Limit
int CalcBattleDamage(int iDam, int iAttackerLev, int iVictimLev)
{
	if (iDam < 3)
		iDam = number(1, 5);

	//return CALCULATE_DAMAGE_LVDELTA(iAttackerLev, iVictimLev, iDam);
	return iDam;
}

int CalcMagicDamageWithValue(int iDam, LPCHARACTER pkAttacker, LPCHARACTER pkVictim)
{
	return CalcBattleDamage(iDam, pkAttacker->GetLevel(), pkVictim->GetLevel());
}

int CalcMagicDamage(LPCHARACTER pkAttacker, LPCHARACTER pkVictim)
{
	int iDam = 0;

	if (pkAttacker->IsNPC())
	{
		iDam = CalcMeleeDamage(pkAttacker, pkVictim, false, false);
	}

	iDam += pkAttacker->GetPoint(POINT_PARTY_ATTACKER_BONUS);

	return CalcMagicDamageWithValue(iDam, pkAttacker, pkVictim);
}

float CalcAttackRating(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, bool bIgnoreTargetRating)
{
	int iARSrc;
	int iERSrc;

	{
		int attacker_dx = pkAttacker->GetPolymorphPoint(POINT_DX);
		int attacker_lv = pkAttacker->GetLevel();

		int victim_dx = pkVictim->GetPolymorphPoint(POINT_DX);
		int victim_lv = pkAttacker->GetLevel();

		iARSrc = MIN(90, (attacker_dx * 4	+ attacker_lv * 2) / 6);
		iERSrc = MIN(90, (victim_dx	  * 4	+ victim_lv   * 2) / 6);
	}

	float fAR = ((float) iARSrc + 210.0f) / 300.0f; // fAR = 0.7 ~ 1.0

	if (bIgnoreTargetRating)
		return fAR;

	// ((Edx * 2 + 20) / (Edx + 110)) * 0.3
	float fER = ((float) (iERSrc * 2 + 5) / (iERSrc + 95)) * 3.0f / 10.0f;

	return fAR - fER;
}

int CalcAttBonus(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, int iAtk)
{
	// PvP에는 적용하지않음
	if (!pkVictim->IsPC())
		iAtk += pkAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_ATTACK_BONUS);

	// PvP에는 적용하지않음
	if (!pkAttacker->IsPC())
	{
		int iReduceDamagePct = pkVictim->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_TRANSFER_DAMAGE);
		iAtk = iAtk * (100 + iReduceDamagePct) / 100;
	}

	if (pkAttacker->IsNPC() && pkVictim->IsPC())
	{
		iAtk = (iAtk * CHARACTER_MANAGER::instance().GetMobDamageRate(pkAttacker)) / 100;
	}

	if (pkVictim->IsNPC())
	{
#ifdef ENABLE_DS_RUNE
		if (pkVictim->IsRaceFlag(RACE_FLAG_RUNE))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_RUNE_MONSTERS)) / 100;
#endif
		if (pkVictim->IsRaceFlag(RACE_FLAG_ANIMAL))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ANIMAL)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_UNDEAD))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_UNDEAD)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_DEVIL))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_DEVIL)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_HUMAN))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_HUMAN)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_ORC))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ORC)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_MILGYO))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_MILGYO)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_INSECT))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_INSECT)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_FIRE))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_FIRE)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_ICE))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ICE)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_DESERT))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_DESERT)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_TREE))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_TREE)) / 100;
#ifdef ELEMENT_NEW_BONUSES
		if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_ELEC))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ELEC)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_FIRE))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_FIRE)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_ICE))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ICE)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_WIND))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_WIND)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_EARTH))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_EARTH)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_DARK))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_DARK)) / 100;
#endif
		if (pkVictim->GetCharType() == CHAR_TYPE_STONE) {
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
			if (!pkVictim->IsHalloweenMonster()) {
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_METIN)) / 100;
			}
#else
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_METIN)) / 100;
#endif
		}
		else {
			if (pkVictim->GetMobRank() >= MOB_RANK_BOSS)
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
				if (!pkVictim->IsHalloweenMonster()) {
					iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_BOSS)) / 100;
				}
#else
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_BOSS)) / 100;
#endif
		}

#if defined(ENABLE_HALLOWEEN_EVENT_2022) && !defined(USE_NO_HALLOWEEN_EVENT_2022_BONUSES)
        if (pkVictim->IsHalloweenMonster())
        {
            iAtk += (iAtk * pkAttacker->GetPoint(POINT_FEAR)) / 100;
        }
#endif

#ifdef ENABLE_NO_ATTBONUS_MONSTER_FOR_STONES
		if (pkVictim->GetCharType() != CHAR_TYPE_STONE) {
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
			if (!pkVictim->IsHalloweenMonster()) {
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_MONSTER)) / 100;
			}
#else
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_MONSTER)) / 100;
#endif
		}
#else
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
		if (!pkVictim->IsHalloweenMonster()) {
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_MONSTER)) / 100;
		}
#else
		iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_MONSTER)) / 100;
#endif
#endif
	}
	else if (pkVictim->IsPC())
	{
#if defined(ENABLE_NEW_BONUS_TALISMAN)
		if (pkAttacker->GetPoint(POINT_ATTBONUS_HUMAN) > 0) {
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_HUMAN)) / 100;
		}

		int32_t iResistHumans = pkVictim->GetPoint(POINT_RESIST_MEZZIUOMINI);
		if (iResistHumans > 0) {
			iAtk -= (iAtk / 100) * iResistHumans;
		}
#else
		iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_HUMAN)) / 100;
#endif

		iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATT_RACES)) / 100;

		switch (pkVictim->GetJob())
		{
			case JOB_WARRIOR:
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_WARRIOR)) / 100;
				break;

			case JOB_ASSASSIN:
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ASSASSIN)) / 100;
				break;

			case JOB_SURA:
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_SURA)) / 100;
				break;

			case JOB_SHAMAN:
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_SHAMAN)) / 100;
				break;
		}
	}

	if (pkAttacker->IsPC() == true)
	{
#ifdef ENABLE_NEW_BONUS_TALISMAN
		iAtk -= (iAtk * pkVictim->GetPoint(POINT_DEF_TALISMAN)) / 100;
#endif

		iAtk -= (iAtk / 100) * pkVictim->GetPoint(POINT_RES_RACES);

		switch (pkAttacker->GetJob())
		{
			case JOB_WARRIOR:
				iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_WARRIOR)) / 100;
				break;

			case JOB_ASSASSIN:
				iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_ASSASSIN)) / 100;
				break;

			case JOB_SURA:
				iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_SURA)) / 100;
				break;

			case JOB_SHAMAN:
				iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_SHAMAN)) / 100;
				break;
		}
	}

#if defined(ENABLE_HALLOWEEN_EVENT_2022) && !defined(USE_NO_HALLOWEEN_EVENT_2022_BONUSES)
    if (pkAttacker->IsHalloweenMonster())
    {
        iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESISTANCE_FEAR)) / 100;
    }
#endif

#if defined(ENABLE_RESIST_MONSTER)
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
	if (!pkAttacker->IsHalloweenMonster()) {
		iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_MONSTER)) / 100;
	}
#else
	iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_MONSTER)) / 100;
#endif
#endif

#ifdef ELEMENT_TARGET
	//[ mob -> PC ] 원소 속성 방어 적용
	//2013/01/17
	//몬스터 속성공격 데미지의 30%에 해당하는 수치에만 저항이 적용됨.
	if (pkAttacker->IsNPC() && pkVictim->IsPC())
	{
#ifdef ENABLE_NEW_BONUS_TALISMAN
		iAtk -= (iAtk * 30 * pkVictim->GetPoint(POINT_DEF_TALISMAN))		/ 10000;
#endif
		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_ELEC))
			iAtk -= (iAtk * 30 * pkVictim->GetPoint(POINT_RESIST_ELEC))		/ 10000;
		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_FIRE))
			iAtk -= (iAtk * 30 * pkVictim->GetPoint(POINT_RESIST_FIRE))		/ 10000;
		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_ICE))
			iAtk -= (iAtk * 30 * pkVictim->GetPoint(POINT_RESIST_ICE))		/ 10000;
		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_WIND))
			iAtk -= (iAtk * 30 * pkVictim->GetPoint(POINT_RESIST_WIND))		/ 10000;
		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_EARTH))
			iAtk -= (iAtk * 30 * pkVictim->GetPoint(POINT_RESIST_EARTH))	/ 10000;
		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_DARK))
			iAtk -= (iAtk * 30 * pkVictim->GetPoint(POINT_RESIST_DARK))		/ 10000;//difesa
	}
#endif

	return iAtk;
}

void Item_GetDamage(LPITEM pkItem, int* pdamMin, int* pdamMax)
{
	*pdamMin = 0;
	*pdamMax = 1;

	if (!pkItem)
		return;

	switch (pkItem->GetType())
	{
		case ITEM_ROD:
		case ITEM_PICK:
			return;
	}

	if (pkItem->GetType() != ITEM_WEAPON)
		sys_err("Item_GetDamage - !ITEM_WEAPON vnum=%d, type=%d", pkItem->GetOriginalVnum(), pkItem->GetType());

	*pdamMin = pkItem->GetValue(3);
	*pdamMax = pkItem->GetValue(4);
}

int CalcMeleeDamage(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, bool bIgnoreDefense, bool bIgnoreTargetRating)
{
	LPITEM pWeapon = pkAttacker->GetWear(WEAR_WEAPON);
	bool bPolymorphed = pkAttacker->IsPolymorphed();

	if (pWeapon && !(bPolymorphed && !pkAttacker->IsPolyMaintainStat()))
	{
		if (pWeapon->GetType() != ITEM_WEAPON)
			return 0;

		switch (pWeapon->GetSubType())
		{
			case WEAPON_SWORD:
			case WEAPON_DAGGER:
			case WEAPON_TWO_HANDED:
			case WEAPON_BELL:
			case WEAPON_FAN:
			case WEAPON_MOUNT_SPEAR:
				break;
			case WEAPON_BOW:
				sys_err("CalcMeleeDamage should not handle bows (name: %s)", pkAttacker->GetName());
				return 0;
			default:
				return 0;
		}
	}

	int iDam = 0;
	float fAR = CalcAttackRating(pkAttacker, pkVictim, bIgnoreTargetRating);
	int iDamMin = 0, iDamMax = 0;

	// TESTSERVER_SHOW_ATTACKINFO
	int DEBUG_iDamCur = 0;
	int DEBUG_iDamBonus = 0;
	// END_OF_TESTSERVER_SHOW_ATTACKINFO

	if (bPolymorphed && !pkAttacker->IsPolyMaintainStat())
	{
		// MONKEY_ROD_ATTACK_BUG_FIX
		Item_GetDamage(pWeapon, &iDamMin, &iDamMax);
		// END_OF_MONKEY_ROD_ATTACK_BUG_FIX

		DWORD dwMobVnum = pkAttacker->GetPolymorphVnum();
		const CMob * pMob = CMobManager::instance().Get(dwMobVnum);

		if (pMob)
		{
			int iPower = pkAttacker->GetPolymorphPower();
			iDamMin += pMob->m_table.dwDamageRange[0] * iPower / 100;
			iDamMax += pMob->m_table.dwDamageRange[1] * iPower / 100;
		}
	}
	else if (pWeapon)
	{
		// MONKEY_ROD_ATTACK_BUG_FIX
		Item_GetDamage(pWeapon, &iDamMin, &iDamMax);
		// END_OF_MONKEY_ROD_ATTACK_BUG_FIX
	}
	else if (pkAttacker->IsNPC())
	{
		iDamMin = pkAttacker->GetMobDamageMin();
		iDamMax = pkAttacker->GetMobDamageMax();
	}

	iDam = number(iDamMin, iDamMax) * 2;

	// TESTSERVER_SHOW_ATTACKINFO
	DEBUG_iDamCur = iDam;
	// END_OF_TESTSERVER_SHOW_ATTACKINFO
	//
	int iAtk = 0;

	// level must be ignored when multiply by fAR, so subtract it before calculation.
	iAtk = pkAttacker->GetPoint(POINT_ATT_GRADE) + iDam - (pkAttacker->GetLevel() * 2);
	iAtk = (int) (iAtk * fAR);
	iAtk += pkAttacker->GetLevel() * 2; // and add again

	if (pWeapon)
	{
		iAtk += pWeapon->GetValue(5) * 2;

		// 2004.11.12.myevan.TESTSERVER_SHOW_ATTACKINFO
		DEBUG_iDamBonus = pWeapon->GetValue(5) * 2;
		///////////////////////////////////////////////
	}

	iAtk += pkAttacker->GetPoint(POINT_PARTY_ATTACKER_BONUS); // party attacker role bonus
	iAtk = (int) (iAtk * (100 + (pkAttacker->GetPoint(POINT_ATT_BONUS) + pkAttacker->GetPoint(POINT_MELEE_MAGIC_ATT_BONUS_PER))) / 100);

	iAtk = CalcAttBonus(pkAttacker, pkVictim, iAtk);

	int iDef = 0;

	if (!bIgnoreDefense)
	{
		iDef = (pkVictim->GetPoint(POINT_DEF_GRADE) * (100 + pkVictim->GetPoint(POINT_DEF_BONUS)) / 100);

		if (!pkAttacker->IsPC())
			iDef += pkVictim->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_DEFENSE_BONUS);
	}

	if (pkAttacker->IsNPC())
		iAtk = (int) (iAtk * pkAttacker->GetMobDamageMultiply());

	iDam = MAX(0, iAtk - iDef);

	if (test_server)
	{
		int DEBUG_iLV = pkAttacker->GetLevel()*2;
		int DEBUG_iST = int((pkAttacker->GetPoint(POINT_ATT_GRADE) - DEBUG_iLV) * fAR);
		int DEBUG_iPT = pkAttacker->GetPoint(POINT_PARTY_ATTACKER_BONUS);
		int DEBUG_iWP = 0;
		int DEBUG_iPureAtk = 0;
		int DEBUG_iPureDam = 0;
		char szRB[32] = "";
		char szGradeAtkBonus[32] = "";

		DEBUG_iWP = int(DEBUG_iDamCur * fAR);
		DEBUG_iPureAtk = DEBUG_iLV + DEBUG_iST + DEBUG_iWP+DEBUG_iDamBonus;
		DEBUG_iPureDam = iAtk - iDef;

		if (pkAttacker->IsNPC())
		{
			snprintf(szGradeAtkBonus, sizeof(szGradeAtkBonus), "=%d*%.1f", DEBUG_iPureAtk, pkAttacker->GetMobDamageMultiply());
			DEBUG_iPureAtk = int(DEBUG_iPureAtk * pkAttacker->GetMobDamageMultiply());
		}

		if (DEBUG_iDamBonus != 0)
			snprintf(szRB, sizeof(szRB), "+RB(%d)", DEBUG_iDamBonus);

		char szPT[32] = "";

		if (DEBUG_iPT != 0)
			snprintf(szPT, sizeof(szPT), ", PT=%d", DEBUG_iPT);

		char szUnknownAtk[32] = "";

		if (iAtk != DEBUG_iPureAtk)
			snprintf(szUnknownAtk, sizeof(szUnknownAtk), "+?(%d)", iAtk-DEBUG_iPureAtk);

		char szUnknownDam[32] = "";

		if (iDam != DEBUG_iPureDam)
			snprintf(szUnknownDam, sizeof(szUnknownDam), "+?(%d)", iDam-DEBUG_iPureDam);

		char szMeleeAttack[128];

		snprintf(szMeleeAttack, sizeof(szMeleeAttack),
				"%s(%d)-%s(%d)=%d%s, ATK=LV(%d)+ST(%d)+WP(%d)%s%s%s, AR=%.3g%s",
				pkAttacker->GetName(),
				iAtk,
				pkVictim->GetName(),
				iDef,
				iDam,
				szUnknownDam,
				DEBUG_iLV,
				DEBUG_iST,
				DEBUG_iWP,
				szRB,
				szUnknownAtk,
				szGradeAtkBonus,
				fAR,
				szPT);

		pkAttacker->ChatPacket(CHAT_TYPE_TALKING, "%s", szMeleeAttack);
		pkVictim->ChatPacket(CHAT_TYPE_TALKING, "%s", szMeleeAttack);
	}

	return CalcBattleDamage(iDam, pkAttacker->GetLevel(), pkVictim->GetLevel());
}

int CalcArrowDamage(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, LPITEM pkBow, LPITEM pkArrow, bool bIgnoreDefense)
{
	if (!pkBow || pkBow->GetType() != ITEM_WEAPON || pkBow->GetSubType() != WEAPON_BOW)
		return 0;

	if (!pkArrow)
		return 0;

	if (pkAttacker->IsStateMove())
		return 0;

	// 타격치 계산부
	int iDist = (int) (DISTANCE_SQRT(pkAttacker->GetX() - pkVictim->GetX(), pkAttacker->GetY() - pkVictim->GetY()));
	//int iGap = (iDist / 100) - 5 - pkBow->GetValue(5) - pkAttacker->GetPoint(POINT_BOW_DISTANCE);
	int iGap = (iDist / 100) - 5 - pkAttacker->GetPoint(POINT_BOW_DISTANCE);
	int iPercent = 100 - (iGap * 5);

	if (iPercent <= 0)
		return 0;
	else if (iPercent > 100)
		iPercent = 100;

	int iDam = 0;

	float fAR = CalcAttackRating(pkAttacker, pkVictim, false);
	iDam = number(pkBow->GetValue(3), pkBow->GetValue(4)) * 2 + pkArrow->GetValue(3);
	int iAtk;

	// level must be ignored when multiply by fAR, so subtract it before calculation.
	iAtk = pkAttacker->GetPoint(POINT_ATT_GRADE) + iDam - (pkAttacker->GetLevel() * 2);
	iAtk = (int) (iAtk * fAR);
	iAtk += pkAttacker->GetLevel() * 2; // and add again

	// Refine Grade
	iAtk += pkBow->GetValue(5) * 2;

	iAtk += pkAttacker->GetPoint(POINT_PARTY_ATTACKER_BONUS);
	iAtk = (int) (iAtk * (100 + (pkAttacker->GetPoint(POINT_ATT_BONUS) + pkAttacker->GetPoint(POINT_MELEE_MAGIC_ATT_BONUS_PER))) / 100);

	iAtk = CalcAttBonus(pkAttacker, pkVictim, iAtk);

	int iDef = 0;

	if (!bIgnoreDefense)
		iDef = (pkVictim->GetPoint(POINT_DEF_GRADE) * (100 + pkAttacker->GetPoint(POINT_DEF_BONUS)) / 100);

	if (pkAttacker->IsNPC())
		iAtk = (int) (iAtk * pkAttacker->GetMobDamageMultiply());

	iDam = MAX(0, iAtk - iDef);

	int iPureDam = iDam;

	iPureDam = (iPureDam * iPercent) / 100;

	if (test_server)
	{
		pkAttacker->ChatPacket(CHAT_TYPE_INFO, "ARROW %s -> %s, DAM %d DIST %d GAP %d %% %d",
				pkAttacker->GetName(),
				pkVictim->GetName(),
				iPureDam,
				iDist, iGap, iPercent);
	}

	return iPureDam;
	//return iDam;
}


void NormalAttackAffect(LPCHARACTER pkAttacker, LPCHARACTER pkVictim)
{
	// 독 공격은 특이하므로 특수 처리
	if (pkAttacker->GetPoint(POINT_POISON_PCT) && !pkVictim->IsAffectFlag(AFF_POISON))
	{
		if (number(1, 100) <= pkAttacker->GetPoint(POINT_POISON_PCT))
			pkVictim->AttackedByPoison(pkAttacker);
	}

	int iStunDuration = 2;
	if (pkAttacker->IsPC() && !pkVictim->IsPC())
		iStunDuration = 4;

	AttackAffect(pkAttacker, pkVictim, POINT_STUN_PCT, IMMUNE_STUN,  AFFECT_STUN, POINT_NONE,        0, AFF_STUN, iStunDuration, "STUN");
	AttackAffect(pkAttacker, pkVictim, POINT_SLOW_PCT, IMMUNE_SLOW,  AFFECT_SLOW, POINT_MOV_SPEED, -30, AFF_SLOW, 20,		"SLOW");
}

int battle_hit(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, int & iRetDam)
{
/*
	if (pkAttacker->IsStateMove())
		return (BATTLE_DAMAGE);
*/

	if (test_server)
		sys_log(0, "battle_hit : [%s] attack to [%s] : dam :%d", pkAttacker->GetName(), pkVictim->GetName(), iRetDam);

	int iDam = CalcMeleeDamage(pkAttacker, pkVictim);

	if (iDam <= 0)
		return (BATTLE_DAMAGE);

	NormalAttackAffect(pkAttacker, pkVictim);

	// 데미지 계산
	//iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST)) / 100;
	LPITEM pkWeapon = pkAttacker->GetWear(WEAR_WEAPON);

	if (pkWeapon)
		switch (pkWeapon->GetSubType())
		{
			case WEAPON_SWORD:
			{
				long lValue = pkVictim->GetPoint(POINT_RESIST_SWORD);
#ifdef ENABLE_NEW_BONUS_TALISMAN
				lValue -= pkAttacker->GetPoint(POINT_ATTBONUS_IRR_SPADA);
#endif
#ifdef ENABLE_NEW_COMMON_BONUSES
				lValue -= pkAttacker->GetPoint(POINT_IRR_WEAPON_DEFENSE);
#endif
				lValue = lValue < 0 ? 0 :  lValue;
				iDam = iDam * (100 - lValue) / 100;
				break;
			}
			case WEAPON_TWO_HANDED:
			{
				long lValue = pkVictim->GetPoint(POINT_RESIST_TWOHAND);
#ifdef ENABLE_NEW_BONUS_TALISMAN
				lValue -= pkAttacker->GetPoint(POINT_ATTBONUS_IRR_SPADONE);
#endif
#ifdef ENABLE_NEW_COMMON_BONUSES
				lValue -= pkAttacker->GetPoint(POINT_IRR_WEAPON_DEFENSE);
#endif
				lValue = lValue < 0 ? 0 :  lValue;
				iDam = iDam * (100 - lValue) / 100;
				break;
			}
			case WEAPON_DAGGER:
			{
				long lValue = pkVictim->GetPoint(POINT_RESIST_DAGGER);
#ifdef ENABLE_NEW_BONUS_TALISMAN
				lValue -= pkAttacker->GetPoint(POINT_ATTBONUS_IRR_PUGNALE);
#endif
#ifdef ENABLE_NEW_COMMON_BONUSES
				lValue -= pkAttacker->GetPoint(POINT_IRR_WEAPON_DEFENSE);
#endif
				lValue = lValue < 0 ? 0 :  lValue;
				iDam = iDam * (100 - lValue) / 100;
				break;
			}
			case WEAPON_BELL:
			{
				long lValue = pkVictim->GetPoint(POINT_RESIST_BELL);
#ifdef ENABLE_NEW_BONUS_TALISMAN
				lValue -= pkAttacker->GetPoint(POINT_ATTBONUS_IRR_CAMPANA);
#endif
#ifdef ENABLE_NEW_COMMON_BONUSES
				lValue -= pkAttacker->GetPoint(POINT_IRR_WEAPON_DEFENSE);
#endif
				lValue = lValue < 0 ? 0 :  lValue;
				iDam = iDam * (100 - lValue) / 100;
				break;
			}
			case WEAPON_FAN:
			{
				long lValue = pkVictim->GetPoint(POINT_RESIST_FAN);
#ifdef ENABLE_NEW_BONUS_TALISMAN
				lValue -= pkAttacker->GetPoint(POINT_ATTBONUS_IRR_VENTAGLIO);
#endif
#ifdef ENABLE_NEW_COMMON_BONUSES
				lValue -= pkAttacker->GetPoint(POINT_IRR_WEAPON_DEFENSE);
#endif
				lValue = lValue < 0 ? 0 :  lValue;
				iDam = iDam * (100 - lValue) / 100;
				break;
			}
			case WEAPON_BOW:
			{
				long lValue = pkVictim->GetPoint(POINT_RESIST_BOW);
#ifdef ENABLE_NEW_BONUS_TALISMAN
				lValue -= pkAttacker->GetPoint(POINT_ATTBONUS_IRR_FRECCIA);
#endif
#ifdef ENABLE_NEW_COMMON_BONUSES
				lValue -= pkAttacker->GetPoint(POINT_IRR_WEAPON_DEFENSE);
#endif
				lValue = lValue < 0 ? 0 :  lValue;
				iDam = iDam * (100 - lValue) / 100;
				break;
			}
			default:
				break;
		}


	//최종적인 데미지 보정. (2011년 2월 현재 대왕거미에게만 적용.)
	float attMul = pkAttacker->GetAttMul();
	float tempIDam = iDam;
	iDam = attMul * tempIDam + 0.5f;

#ifdef ENABLE_SOUL_SYSTEM
	iDam += pkAttacker->GetSoulItemDamage(pkVictim, iDam, RED_SOUL);
#endif

	iRetDam = iDam;

	//PROF_UNIT puDam("Dam");
	if (pkVictim->Damage(pkAttacker, iDam, DAMAGE_TYPE_NORMAL))
		return (BATTLE_DEAD);

	return (BATTLE_DAMAGE);
}

#if defined(ENABLE_NEW_ANTICHEAT_RULES)
uint32_t GET_ATTACK_SPEED(LPCHARACTER ch) {
	if (!ch) {
		return 1000;
	}

	int32_t bonus = ch->IsRiding() ? 65 + ch->GetPoint(POINT_ATT_SPEED) : 95 + ch->GetPoint(POINT_ATT_SPEED);
//	int32_t bonus = ch->IsRiding() ? 55 + ch->GetPoint(POINT_ATT_SPEED) : 85 + ch->GetPoint(POINT_ATT_SPEED);

	if (!ch->IsRiding() && ch->IsDaggerEquipped()) {
		bonus += 50;
	}

	uint32_t real_speed = ani_attack_speed(ch);
	uint32_t final_speed = (real_speed * 100) / bonus;

	return final_speed;
}

void SET_ATTACK_TIME(LPCHARACTER ch, LPCHARACTER victim, uint32_t current_time) {
	if (ch && ch->IsPC() && victim && ch != victim) {
		ch->m_kAttackLog.dwVID = victim->GetVID();
		ch->m_kAttackLog.dwTime = current_time;
	}
}

void SET_ATTACKED_TIME(LPCHARACTER ch, LPCHARACTER victim, uint32_t current_time) {
	if (ch && ch->IsPC() && victim && ch != victim) {
		victim->m_AttackedLog.dwPID = ch->GetPlayerID();
		victim->m_AttackedLog.dwAttackedTime = current_time;
	}
}

void SEND_REPORT_HACK(LPCHARACTER ch, uint8_t reason) {
	const LPDESC d = ch ? ch->GetDesc() : NULL;
	if (!d) {
		return;
	}

#ifdef USE_CAPTCHA_SYSTEM
    if (ch->IsWaitingForCaptcha())
    {
        return;
    }

    ch->SetWaitingForCaptcha(true);
#endif

	const char* szReason;

	switch (reason)
	{
	case ANTICHEAT_TYPE_ATTACKSPEED:
	{
		szReason = "SPEED_HACK";
		break;
	}
	case ANTICHEAT_TYPE_RANGE:
	{
		szReason = "RANGE_HACK";
		break;
	}
	default:
	{
		szReason = "UNKNOWN";
		break;
	}
	}

	LogManager::instance().HackReport(ch->GetPlayerID(), ch->GetName(), d->GetHostName(), szReason);

	TPacketGCAntiCheat p;

	p.bHeader = HEADER_GC_ANTICHEAT;
	p.bReason = reason;

	d->Packet(&p, sizeof(TPacketGCAntiCheat));

	d->DelayedDisconnect(3);
}

bool IS_SPEED_HACK(LPCHARACTER ch, LPCHARACTER victim, uint32_t current_time) {
	if (ch && ch->IsPC() && victim && ch != victim) {

		if (ch->m_kAttackLog.dwVID == victim->GetVID()) {
			if (current_time - ch->m_kAttackLog.dwTime < GET_ATTACK_SPEED(ch)) {
/* maybe false report
				INCREASE_SPEED_HACK_COUNT(ch);

				if (ch->m_speed_hack_count > 50) {
					SEND_REPORT_HACK(ch, ANTICHEAT_TYPE_ATTACKSPEED);
				}
*/

				SET_ATTACK_TIME(ch, victim, current_time);
				SET_ATTACKED_TIME(ch, victim, current_time);
				return true;
			}
		}

		if (victim->m_AttackedLog.dwPID == ch->GetPlayerID()) {
			if (current_time - victim->m_AttackedLog.dwAttackedTime < GET_ATTACK_SPEED(ch)) {
/* maybe false report
				INCREASE_SPEED_HACK_COUNT(ch);

				if (ch->m_speed_hack_count > 50) {
					SEND_REPORT_HACK(ch, ANTICHEAT_TYPE_ATTACKSPEED);
				}
*/

				SET_ATTACK_TIME(ch, victim, current_time);
				SET_ATTACKED_TIME(ch, victim, current_time);
				return true;
			}
		}

		SET_ATTACK_TIME(ch, victim, current_time);
		SET_ATTACKED_TIME(ch, victim, current_time);
	}

	return false;
}
#endif
