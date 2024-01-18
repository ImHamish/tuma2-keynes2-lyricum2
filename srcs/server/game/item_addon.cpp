#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "item.h"
#include "item_addon.h"
#ifdef ENABLE_REWARD_SYSTEM
#include "char.h"
#include "char_manager.h"
#endif

CItemAddonManager::CItemAddonManager()
{
}

CItemAddonManager::~CItemAddonManager()
{
}

#ifdef ENABLE_SWITCHBOT_WORLDARD
void CItemAddonManager::ApplyAddonTo(int iAddonType, LPITEM pItem, bool switchbot)
#else
void CItemAddonManager::ApplyAddonTo(int iAddonType, LPITEM pItem)
#endif
{
	if (!pItem)
	{
		sys_err("ITEM pointer null");
		return;
	}

	// TODO 일단 하드코딩으로 평타 스킬 수치 변경만 경우만 적용받게한다.

	int iSkillBonus = MINMAX(-30, (int) (gauss_random(0, 5) + 0.5f), 30);
	int iNormalHitBonus = 0;
	if (abs(iSkillBonus) <= 20)
		iNormalHitBonus = -2 * iSkillBonus + abs(number(-8, 8) + number(-8, 8)) + number(1, 4);
	else
		iNormalHitBonus = -2 * iSkillBonus + number(1, 5);

#ifdef ENABLE_REWARD_SYSTEM
	if(iNormalHitBonus>=59)
	{
		LPCHARACTER ch = pItem->GetOwner();
		if(ch)
			CHARACTER_MANAGER::Instance().SetRewardData(REWARD_AVERAGE,ch->GetName(), true);
	}
#endif

	pItem->RemoveAttributeType(APPLY_SKILL_DAMAGE_BONUS);
	pItem->RemoveAttributeType(APPLY_NORMAL_HIT_DAMAGE_BONUS);
#ifdef ENABLE_SWITCHBOT_WORLDARD
	if (switchbot){
		pItem->AddAttributeSwitchBot(APPLY_NORMAL_HIT_DAMAGE_BONUS, iNormalHitBonus);
		pItem->AddAttributeSwitchBot(APPLY_SKILL_DAMAGE_BONUS, iSkillBonus);
	}else{
		pItem->AddAttribute(APPLY_NORMAL_HIT_DAMAGE_BONUS, iNormalHitBonus);
		pItem->AddAttribute(APPLY_SKILL_DAMAGE_BONUS, iSkillBonus);	
	}
#else
	pItem->AddAttribute(APPLY_NORMAL_HIT_DAMAGE_BONUS, iNormalHitBonus);
	pItem->AddAttribute(APPLY_SKILL_DAMAGE_BONUS, iSkillBonus);
#endif
}
