#include "stdafx.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "packet.h"
#include "guild.h"
#include "vector.h"
#include "questmanager.h"
#include "item.h"
#include "horsename_manager.h"
#include "locale_service.h"
#include "arena.h"
#include "desc.h"

#include "../common/VnumHelper.h"

bool CHARACTER::StartRiding()
{
	if (IsRiding()) {
		return false;
	}

#ifdef BLOCK_RIDING_INSIDE_WAR
	if (GetWarMap()) {
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 852, "");
#endif
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
		if (IsRiding())
			StopRiding();
		return false;
	}
#endif

	if (g_NoMountAtGuildWar && GetWarMap())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
		if (IsRiding())
			StopRiding();
		return false;
	}

	if (IsDead() == true)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 356, "");
#endif
		return false;
	}

	if (IsPolymorphed())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 355, "");
#endif
		return false;
	}

	// 턱시도 입은 상태의 말 타기 금지
	LPITEM armor = GetWear(WEAR_BODY);

	if (armor && (armor->GetVnum() >= 11901 && armor->GetVnum() <= 11904))
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 410, "");
#endif
		return false;
	}

	// @warme005
	if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
		return false;


	DWORD dwMountVnum = m_chHorse ? m_chHorse->GetRaceNum() : GetMyHorseVnum();

	if (false == CHorseRider::StartRiding())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		if (GetHorseLevel() <= 0) {
			ChatPacketNew(CHAT_TYPE_INFO, 333, "");
		} else if (GetHorseHealth() <= 0) {
			ChatPacketNew(CHAT_TYPE_INFO, 335, "");
		} else if (GetHorseStamina() <= 0) {
			ChatPacketNew(CHAT_TYPE_INFO, 334, "");
		}
#endif
		return false;
	}

	// 소환한 말 없애고
	HorseSummon(false);

	MountVnum(dwMountVnum);

	if(test_server)
		sys_log(0, "Ride Horse : %s ", GetName());

	return true;
}

bool CHARACTER::StopRiding()
{
	if (CHorseRider::StopRiding())
	{
		quest::CQuestManager::instance().Unmount(GetPlayerID());

		if (!IsDead() && !IsStun())
		{
			DWORD dwOldVnum = GetMountVnum();
			MountVnum(0);

			HorseSummon(true, false, dwOldVnum);
		}
		else
		{
			m_dwMountVnum = 0;
			ComputePoints();
			UpdatePacket();
		}

		PointChange(POINT_ST, 0);
		PointChange(POINT_DX, 0);
		PointChange(POINT_HT, 0);
		PointChange(POINT_IQ, 0);

		return true;
	}

	return false;
}

EVENTFUNC(horse_dead_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "horse_dead_event> <Factor> Null pointer" );
		return 0;
	}

	// <Factor>
	LPCHARACTER ch = info->ch;
	if (ch == NULL) {
		return 0;
	}
	ch->HorseSummon(false);
	return 0;
}

void CHARACTER::SetRider(LPCHARACTER ch)
{
	if (m_chRider)
		m_chRider->ClearHorseInfo();

	m_chRider = ch;

	if (m_chRider)
		m_chRider->SendHorseInfo();
}

LPCHARACTER CHARACTER::GetRider() const
{
	return m_chRider;
}


void CHARACTER::HorseSummon(bool bSummon, bool bFromFar, DWORD dwVnum, const char* pPetName)
{
	if ( bSummon )
	{
		if( m_chHorse != NULL )
			return;

		if (GetHorseLevel() <= 0)
			return;

		if (IsRiding())
			return;

		sys_log(0, "HorseSummon : %s lv:%d bSummon:%d fromFar:%d", GetName(), GetLevel(), bSummon, bFromFar);

		long x = GetX();
		long y = GetY();

		if (GetHorseHealth() <= 0)
			bFromFar = false;

		if (bFromFar)
		{
			x += (number(0, 1) * 2 - 1) * number(2000, 2500);
			y += (number(0, 1) * 2 - 1) * number(2000, 2500);
		}
		else
		{
			x += number(-100, 100);
			y += number(-100, 100);
		}

		m_chHorse = CHARACTER_MANAGER::instance().SpawnMob(
				(0 == dwVnum) ? GetMyHorseVnum() : dwVnum,
				GetMapIndex(),
				x, y,
				GetZ(), false, (int)(GetRotation()+180), false);

		if (!m_chHorse)
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ChatPacketNew(CHAT_TYPE_INFO, 328, "");
#endif
			return;
		}

		if (GetHorseHealth() <= 0)
		{
			// 죽은거처럼 있게 하는 처리
			m_chHorse->SetPosition(POS_DEAD);

			// 일정시간있다 사라지게 하자.
			char_event_info* info = AllocEventInfo<char_event_info>();
			info->ch = this;
			m_chHorse->m_pkDeadEvent = event_create(horse_dead_event, info, PASSES_PER_SEC(60));
		}

		m_chHorse->SetLevel(GetHorseLevel());

		const char* pHorseName = CHorseNameManager::instance().GetHorseName(GetPlayerID());

		if ( pHorseName != NULL && strlen(pHorseName) != 0 )
		{
			m_chHorse->m_stName = pHorseName;
		}
		else
		{
			BYTE bLang = 0;
			if (GetDesc()) {
				bLang = GetDesc()->GetLanguage(); 
			}
			
			m_chHorse->m_stName = GetName();
			m_chHorse->m_stName += " ";
			m_chHorse->m_stName += m_horseText[bLang];
		}

		if (!m_chHorse->Show(GetMapIndex(), x, y, GetZ()))
		{
			M2_DESTROY_CHARACTER(m_chHorse);
			sys_err("cannot show monster");
			m_chHorse = NULL;
			return;
		}

		if ((GetHorseHealth() <= 0))
		{
			TPacketGCDead pack;
			pack.header	= HEADER_GC_DEAD;
			pack.vid    = m_chHorse->GetVID();
			PacketAround(&pack, sizeof(pack));
		}

		m_chHorse->SetRider(this);
		m_chHorse->SetObserverMode(IsObserverMode());
	}
	else
	{
		if (!m_chHorse)
			return;

		LPCHARACTER chHorse = m_chHorse;

		chHorse->SetRider(NULL); // m_chHorse assign to NULL

		// 말시체가 소환되어 있을때 상점 열면 bFromFar를 false로 만들어 말 시체를 사라지게 한다.
		if ((GetHorseHealth() <= 0))
			bFromFar = false;

		if (!bFromFar)
		{
			M2_DESTROY_CHARACTER(chHorse);
		}
		else
		{
			// 멀어지면서 사라지는 처리 하기
			chHorse->SetNowWalking(false);
			float fx, fy;
			chHorse->SetRotation(GetDegreeFromPositionXY(chHorse->GetX(), chHorse->GetY(), GetX(), GetY())+180);
			GetDeltaByDegree(chHorse->GetRotation(), 3500, &fx, &fy);
			chHorse->Goto((long)(chHorse->GetX()+fx), (long) (chHorse->GetY()+fy));
			chHorse->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
		}

		m_chHorse = NULL;

	}
}

DWORD CHARACTER::GetMyHorseVnum() const
{
	int delta = 0;

	if (GetGuild())
	{
		++delta;

		if (GetGuild()->GetMasterPID() == GetPlayerID())
			++delta;
	}

	return c_aHorseStat[GetHorseLevel()].iNPCRace + delta;
}

void CHARACTER::HorseDie()
{
	CHorseRider::HorseDie();
	HorseSummon(false);
}

bool CHARACTER::ReviveHorse()
{
	if (CHorseRider::ReviveHorse())
	{
		HorseSummon(false);
		HorseSummon(true);
		return true;
	}
	return false;
}

void CHARACTER::ClearHorseInfo()
{
	if (!IsHorseRiding())
	{
		ChatPacket(CHAT_TYPE_COMMAND, "hide_horse_state");

		m_bSendHorseLevel = 0;
		m_bSendHorseHealthGrade = 0;
		m_bSendHorseStaminaGrade = 0;
	}

	m_chHorse = NULL;

}

void CHARACTER::SendHorseInfo()
{
	if (m_chHorse || IsHorseRiding())
	{
		int iHealthGrade;
		int iStaminaGrade;
		/*
		   HP
3: 70% < ~ <= 100%
2: 30% < ~ <= 70%
1:  0% < ~ <= 30%
0: 사망

STM

3: 71% < ~ <= 100%
2: 31% < ~ <= 70%
1: 10% < ~ <= 30%
0:	 ~ <= 10%
		 */
		if (GetHorseHealth() == 0)
			iHealthGrade = 0;
		else if (GetHorseHealth() * 10 <= GetHorseMaxHealth() * 3)
			iHealthGrade = 1;
		else if (GetHorseHealth() * 10 <= GetHorseMaxHealth() * 7)
			iHealthGrade = 2;
		else
			iHealthGrade = 3;

		if (GetHorseStamina() * 10 <= GetHorseMaxStamina())
			iStaminaGrade = 0;
		else if (GetHorseStamina() * 10 <= GetHorseMaxStamina() * 3)
			iStaminaGrade = 1;
		else if (GetHorseStamina() * 10 <= GetHorseMaxStamina() * 7)
			iStaminaGrade = 2;
		else
			iStaminaGrade = 3;

		if (m_bSendHorseLevel != GetHorseLevel() ||
				m_bSendHorseHealthGrade != iHealthGrade ||
				m_bSendHorseStaminaGrade != iStaminaGrade)
		{
			ChatPacket(CHAT_TYPE_COMMAND, "horse_state %d %d %d", GetHorseLevel(), iHealthGrade, iStaminaGrade);

			m_bSendHorseLevel = GetHorseLevel();
			m_bSendHorseHealthGrade = iHealthGrade;
			m_bSendHorseStaminaGrade = iStaminaGrade;
		}
	}
}

bool CHARACTER::CanUseHorseSkill()
{
	if (IsRiding())
	{
		if (GetHorseGrade() == 3)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

void CHARACTER::SetHorseLevel(int iLevel)
{
	CHorseRider::SetHorseLevel(iLevel);
	SetSkillLevel(SKILL_HORSE, GetHorseLevel());
}

