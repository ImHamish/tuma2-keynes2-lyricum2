#include "StdAfx.h"
#if defined(ENABLE_CLIENT_OPTIMIZATION)
#include "PythonNetworkStream.h"
#include "PythonPlayer.h"
#include "PythonPlayerSettingsModule.h"
#include "PythonPlayerSettingsConstants.h"

#include "../EterLib/ResourceManager.h"
#include "../EterPack/EterPackManager.h"
#include "../GameLib/RaceManager.h"
#if defined(ENABLE_DOGMODE_RENEWAL)
#include "PythonSystem.h"
#endif

extern const DWORD c_iSkillIndex_Language1;
extern const DWORD c_iSkillIndex_Language2;
extern const DWORD c_iSkillIndex_Language3;

CPythonPlayerSettingsModule::CPythonPlayerSettingsModule()
= default;

CPythonPlayerSettingsModule::~CPythonPlayerSettingsModule()
= default;

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__SetIntroMotions(CRaceData * pRaceData, const std::string & c_rstrFolderName) -> void
{
	static auto& race_manager = CRaceManager::Instance();
	race_manager.SetPathName(string_path({ c_rstrFolderName, "intro/" }).c_str());
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_INTRO_WAIT, "wait.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_INTRO_SELECTED, "selected.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_INTRO_NOT_SELECTED, "not_selected.msa");
}

/**
* \brief
* \param pRaceData
* \param wMotionMode
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__SetGeneralMotions(CRaceData * pRaceData, const uint16_t wMotionMode, const std::string & c_rstrFolderName) -> void
{
	static auto& race_manager = CRaceManager::Instance();
	race_manager.SetPathName(c_rstrFolderName.c_str());
	pRaceData->RegisterMotionMode(wMotionMode);

	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WALK, "walk.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_RUN, "run.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE, "damage.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_FLYING, "damage_flying.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_STAND_UP, "falling_stand.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_FLYING_BACK, "back_damage_flying.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_STAND_UP_BACK, "back_falling_stand.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DEAD, "dead.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DIG, "dig.msa");
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadFishing(CRaceData * pRaceData, const std::string & c_rstrFolderName) -> void
{
	static auto& race_manager = CRaceManager::Instance();
	race_manager.SetPathName(string_path({ c_rstrFolderName, "fishing/" }).c_str());
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_FISHING);

	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_WAIT, "wait.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_WALK, "walk.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_RUN, "run.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_FISHING_THROW, "throw.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_FISHING_WAIT, "fishing_wait.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_FISHING_STOP, "fishing_cancel.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_FISHING_REACT, "fishing_react.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_FISHING_CATCH, "fishing_catch.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_FISHING_FAIL, "fishing_fail.msa");
}

/**
* \brief
* \param pRaceData
* \param wMotionMode
*/
auto CPythonPlayerSettingsModule::__LoadCombo(CRaceData * pRaceData, const uint16_t wMotionMode) -> void
{
	if (wMotionMode == CRaceMotionData::MODE_BOW)
	{
		pRaceData->ReserveComboAttack(CRaceMotionData::MODE_BOW, COMBO_TYPE_1, 1);
		pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BOW, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
		return;
	}

	// Combo Type 1
	pRaceData->ReserveComboAttack(wMotionMode, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_1, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_4);
	// Combo Type 2
	pRaceData->ReserveComboAttack(wMotionMode, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_2, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_2, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_2, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_2, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_2, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_7);
	// Combo Type 3
	pRaceData->ReserveComboAttack(wMotionMode, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_6);

	if (wMotionMode == CRaceMotionData::MODE_DUALHAND_SWORD)
		pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_6, CRaceMotionData::NAME_COMBO_ATTACK_8);
	else
		pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_6, CRaceMotionData::NAME_COMBO_ATTACK_4);
}

/**
* \brief
* \param pRaceData
* \param wMotionMode
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadWeaponMotion(CRaceData * pRaceData, const uint16_t wMotionMode, const std::string & c_rstrFolderName) -> void
{
	static auto& race_manager = CRaceManager::Instance();
	const auto& bJobIndex = RaceToJob(pRaceData->GetRaceIndex());

	// Set path name
	race_manager.SetPathName(c_rstrFolderName.c_str());

	// Register motion mode
	pRaceData->RegisterMotionMode(wMotionMode);
	
	switch (bJobIndex)
	{
		case NRaceData::JOB_WARRIOR:
		{
			const auto one_hand = wMotionMode == CRaceMotionData::MODE_ONEHAND_SWORD;
			__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait.msa", one_hand ? 50 : 70);
			__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait_1.msa", one_hand ? 50 : 30);
		}
		break;
	case NRaceData::JOB_ASSASSIN:
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait.msa", 70);
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait_1.msa", 30);
		break;
	case NRaceData::JOB_SURA:
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE, "damage.msa");
		break;
	case NRaceData::JOB_SHAMAN:
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait.msa");
		break;
	}

	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WALK, "walk.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_RUN, "run.msa");

	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE, "damage.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50);

	if (wMotionMode == CRaceMotionData::MODE_BOW)
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa");
	else
	{
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_4, "combo_04.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_5, "combo_05.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_6, "combo_06.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_7, "combo_07.msa");

		if (bJobIndex == NRaceData::JOB_ASSASSIN && wMotionMode == CRaceMotionData::MODE_DUALHAND_SWORD)
			__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_8, "combo_08.msa");
	}

	// Load combo
	__LoadCombo(pRaceData, wMotionMode);
}

/**
* \brief
* \param pRaceData
* \param wMotionMode
* \param wMotionIndex
* \param c_szFileName
* \param byPercentage
*/
auto CPythonPlayerSettingsModule::__RegisterCacheMotionData(CRaceData * pRaceData, const uint16_t wMotionMode, const uint16_t wMotionIndex, const char * c_szFileName, const uint8_t byPercentage) -> void
{
	static auto& race_manager = CRaceManager::Instance();
	const auto& pkMotionThing = pRaceData->RegisterMotionData(wMotionMode, wMotionIndex, race_manager.GetFullPathFileName(c_szFileName), byPercentage);
	if (pkMotionThing)
	{
		static auto& resource_manager = CResourceManager::Instance();
		resource_manager.LoadStaticCache(pkMotionThing->GetFileName());
	}
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadGuildSkill(CRaceData * pRaceData, const std::string & c_rstrFolderName) -> void
{
	static auto& race_manager = CRaceManager::Instance();
	race_manager.SetPathName(string_path({ c_rstrFolderName, "skill/" }).c_str());
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, GUILD_SKILL_DRAGONBLOOD, "guild_yongsinuipi.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, GUILD_SKILL_DRAGONBLESS, "guild_yongsinuichukbok.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, GUILD_SKILL_BLESSARMOR, "guild_seonghwigap.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, GUILD_SKILL_SPPEDUP, "guild_gasokhwa.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, GUILD_SKILL_DRAGONWRATH, "guild_yongsinuibunno.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, GUILD_SKILL_MAGICUP, "guild_jumunsul.msa");
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadHorse(CRaceData * pRaceData, const std::string & c_rstrFolderName) -> void
{
	static auto& race_manager = CRaceManager::Instance();
	const auto& bJobIndex = RaceToJob(pRaceData->GetRaceIndex());

	race_manager.SetPathName(string_path({ c_rstrFolderName, "horse/" }).c_str());
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait.msa", 90);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WALK, "walk.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_RUN, "run.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DEAD, "dead.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, HORSE_SKILL_CHARGE, "skill_charge.msa");

	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait_1.msa", 9);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait_2.msa", 1);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DAMAGE, "damage.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DAMAGE_BACK, "damage.msa");

	if (bJobIndex == NRaceData::JOB_SHAMAN || bJobIndex == NRaceData::JOB_SURA)
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, HORSE_SKILL_SPLASH, "skill_splash.msa");

	for (const auto & info : m_vecHorse)
	{
		if (info.bJobIndex == bJobIndex)
		{
			for (const auto & motion : info.vecMotions)
			{
				race_manager.SetPathName(string_path({ c_rstrFolderName, GetHorsePathNameByMode(motion) }).c_str());
				pRaceData->RegisterMotionMode(motion);

				__RegisterCacheMotionData(pRaceData, motion, CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa");
				__RegisterCacheMotionData(pRaceData, motion, CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa");
				__RegisterCacheMotionData(pRaceData, motion, CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa");

				pRaceData->ReserveComboAttack(motion, COMBO_TYPE_1, 3);
				pRaceData->RegisterComboAttack(motion, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
				pRaceData->RegisterComboAttack(motion, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
				pRaceData->RegisterComboAttack(motion, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);

				__RegisterCacheMotionData(pRaceData, motion, HORSE_SKILL_WILDATTACK, "skill_wildattack.msa");
				if (bJobIndex == NRaceData::JOB_WARRIOR || bJobIndex == NRaceData::JOB_ASSASSIN)
					__RegisterCacheMotionData(pRaceData, motion, HORSE_SKILL_SPLASH, "skill_splash.msa");
			}
			break;
		}
	}

	if (bJobIndex == NRaceData::JOB_ASSASSIN)
	{
		race_manager.SetPathName(string_path({ c_rstrFolderName, "horse_bow/" }).c_str());
		pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_BOW);

		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_WAIT, "wait.msa", 90);
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_WAIT, "wait_1.msa", 9);
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_WAIT, "wait_2.msa", 1);
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_RUN, "run.msa");
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_DAMAGE, "damage.msa");
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_DEAD, "dead.msa");
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa");
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, HORSE_SKILL_WILDATTACK, "skill_wildattack.msa");
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, HORSE_SKILL_SPLASH, "skill_splash.msa");

		pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_BOW, COMBO_TYPE_1, 1);
		pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_BOW, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	}
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadGameWarriorEx(CRaceData * pRaceData, const std::string & c_rstrFolderName) -> void
{
	// ONEHAND_SWORD BATTLE
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_ONEHAND_SWORD, string_path({ c_rstrFolderName, "onehand_sword/" }));
	// TWOHAND_SWORD BATTLE
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_TWOHAND_SWORD, string_path({ c_rstrFolderName, "twohand_sword/" }));
	// Bone
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON, "equip_right_hand");
#if defined(ENABLE_ACCE_SYSTEM)
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_ACCE, "Bip01 Spine2");
#endif
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadGameAssassinEx(CRaceData * pRaceData, const std::string & c_rstrFolderName)  -> void
{
	// ONEHAND_SWORD BATTLE
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_ONEHAND_SWORD, string_path({ c_rstrFolderName, "onehand_sword/" }));
	// DUALHAND_SWORD BATTLE
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_DUALHAND_SWORD, string_path({ c_rstrFolderName, "dualhand_sword/" }));
	// BOW BATTLE
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_BOW, string_path({ c_rstrFolderName, "bow/" }));
	// Bone
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON, "equip_right");
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON_LEFT, "equip_left");
#if defined(ENABLE_ACCE_SYSTEM)
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_ACCE, "Bip01 Spine2");
#endif
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadGameSuraEx(CRaceData * pRaceData, const std::string & c_rstrFolderName) -> void
{
	// ONEHAND_SWORD BATTLE
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_ONEHAND_SWORD, string_path({ c_rstrFolderName, "onehand_sword/" }));
	// Bone
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON, "equip_right");
#if defined(ENABLE_ACCE_SYSTEM)
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_ACCE, "Bip01 Spine2");
#endif
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadGameShamanEx(CRaceData * pRaceData, const std::string & c_rstrFolderName) -> void
{
	// Fan
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_FAN, string_path({ c_rstrFolderName, "fan/" }));
	// Bell
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_BELL, string_path({ c_rstrFolderName, "Bell/" }));
	// Bone
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON, "equip_right");
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON_LEFT, "equip_left");
#if defined(ENABLE_ACCE_SYSTEM)
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_ACCE, "Bip01 Spine2");
#endif
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadGameRace(CRaceData * pRaceData, const std::string & c_rstrFolderName) -> void
{
	const auto& bJobIndex = RaceToJob(pRaceData->GetRaceIndex());

	{
		__SetGeneralMotions(pRaceData, CRaceMotionData::MODE_GENERAL, string_path({ c_rstrFolderName, "general/" }));

		if (bJobIndex == NRaceData::JOB_WARRIOR || bJobIndex == NRaceData::JOB_ASSASSIN)
		{
			pRaceData->SetMotionRandomWeight(CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_WAIT, 0, 70);
			__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_WAIT, "wait_1.msa", 30);
		}

		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa", 50);
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack_1.msa", 50);

		pRaceData->ReserveComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, 1);
		pRaceData->RegisterComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	}


	switch (pRaceData->GetRaceIndex())
	{
		case MAIN_RACE_WARRIOR_M:
		case MAIN_RACE_WARRIOR_W:
			__LoadGameWarriorEx(pRaceData, c_rstrFolderName);
			break;
		case MAIN_RACE_ASSASSIN_M:
		case MAIN_RACE_ASSASSIN_W:
			__LoadGameAssassinEx(pRaceData, c_rstrFolderName);
			break;
		case MAIN_RACE_SURA_M:
		case MAIN_RACE_SURA_W:
			__LoadGameSuraEx(pRaceData, c_rstrFolderName);
			break;
		case MAIN_RACE_SHAMAN_M:
		case MAIN_RACE_SHAMAN_W:
			__LoadGameShamanEx(pRaceData, c_rstrFolderName);
			break;
		default:
			break;
	}
}

/**
* \brief
* \param pRaceData
* \param wMotionMode
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__RegisterSharedEmotionAnis(CRaceData* pRaceData, const uint16_t wMotionMode, const std::string& c_rstrFolderName) -> void
{
	static auto& race_manager = CRaceManager::Instance();
	race_manager.SetPathName(c_rstrFolderName.c_str());
	pRaceData->RegisterMotionMode(wMotionMode);

	for (const auto& it : m_vecEmotions)
		__RegisterCacheMotionData(pRaceData, wMotionMode, it.motion_id, it.file_name);
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadEmotions(CRaceData* pRaceData, const std::string& c_rstrFolderName) -> void
{
	static auto& race_manager = CRaceManager::Instance();
	race_manager.SetPathName(c_rstrFolderName.c_str());
	
	const auto& actionPath = c_rstrFolderName + "action/";
	const auto& weddingPath = c_rstrFolderName + "wedding/";

	__RegisterSharedEmotionAnis(pRaceData, CRaceMotionData::MODE_GENERAL, actionPath);
	__RegisterSharedEmotionAnis(pRaceData, CRaceMotionData::MODE_WEDDING_DRESS, actionPath);

	race_manager.SetPathName(weddingPath.c_str());
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_WEDDING_DRESS);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_WEDDING_DRESS, CRaceMotionData::NAME_WAIT, "wait.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_WEDDING_DRESS, CRaceMotionData::NAME_WALK, "walk.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_WEDDING_DRESS, CRaceMotionData::NAME_RUN, "walk.msa");
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadSkill(CRaceData * pRaceData, const std::string & c_rstrFolderName) -> void
{
	static auto& race_manager = CRaceManager::Instance();
	const auto& bJobIndex = static_cast<uint8_t>(RaceToJob(pRaceData->GetRaceIndex()));
	const auto bSkillEffectCountStart = bJobIndex == NRaceData::JOB_SHAMAN ? 1 : 0;

	race_manager.SetPathName(string_path({ c_rstrFolderName, "skill/" }).c_str());

	for (const auto & skillData : m_vecSkillFileName)
	{
		if (skillData.bJobIndex == bJobIndex)
		{
			auto iRow = 1;
			for (const auto & skillName : skillData.vecExtraFileName)
			{
				if (iRow == 7)
				{
					iRow += 9;
				}

				__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + iRow, string_path({ skillName, ".msa" }).c_str());

				++iRow;
			}

			for (auto bSkillEffectCount = bSkillEffectCountStart; bSkillEffectCount < CPythonSkill::SKILL_EFFECT_COUNT; ++bSkillEffectCount)
			{
				char szSuffixSkillName[8] = {};
				if (bSkillEffectCount != 0)
					snprintf(szSuffixSkillName, sizeof(szSuffixSkillName), "_%d", bSkillEffectCount + 1);

				auto iSecondRow = 1;
				for (const auto & skillName : skillData.vecFileName)
				{
					if (iSecondRow == 7)
					{
						iSecondRow += 9;
					}

					__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (bSkillEffectCount * CPythonSkill::SKILL_GRADEGAP) + iSecondRow, string_path({ skillName, szSuffixSkillName, ".msa" }).c_str());
					++iSecondRow;
				}
			}
			
			break;
		}
	}
}

/**
* \brief Load npclist file.
*/
auto CPythonPlayerSettingsModule::__LoadGameNPC() -> void
{
	CMappedFile file;
	const VOID * pvData;

#if defined(ENABLE_DOGMODE_RENEWAL)
	if (CPythonSystem::Instance().IsDogMode()) {
		static auto& pack_manager = CFileSystem::Instance();
		if (!pack_manager.Get(file, "data/monsters/npclist_dogmode.txt", &pvData, __FUNCTION__)) {
			TraceError("CPythonPlayerSettingsModule::LoadGameNPC(fileName=data/monsters/npclist_dogmode.txt) - Load Error");
			return;
		}
	} else {
		static auto& pack_manager = CFileSystem::Instance();
		if (!pack_manager.Get(file, "data/monsters/npclist.txt", &pvData, __FUNCTION__)) {
			TraceError("CPythonPlayerSettingsModule::LoadGameNPC(fileName=data/monsters/npclist.txt) - Load Error");
			return;
		}
	}
#else
	static auto& pack_manager = CFileSystem::Instance();
	if (!pack_manager.Get(file, "data/monsters/npclist.txt", &pvData, __FUNCTION__)) {
		TraceError("CPythonPlayerSettingsModule::LoadGameNPC(fileName=data/monsters/npclist.txt) - Load Error");
		return;
	}
#endif

	CMemoryTextFileLoader fileLoader;
	fileLoader.Bind(file.Size(), pvData);

	std::vector<std::vector<std::string>> vecContent;
	CTokenVector kTokenVector;
	for (auto i = 0; i < fileLoader.GetLineCount(); ++i)
	{
		if (!fileLoader.SplitLineByTab(i, &kTokenVector))
			continue;

		vecContent.emplace_back(kTokenVector);
	}

	LoadGameNPC(vecContent);
}

/**
 * \brief Load all of the functions.
 */
auto CPythonPlayerSettingsModule::Load() -> void
{
	PyObject_CallObject(PyObject_GetAttrString(PyImport_ImportModule("introloading"), "__main__"), nullptr);
	__LoadGameNPC();

	static auto& race_manager = CRaceManager::Instance();
	static auto& resource_manager = CResourceManager::instance();

	const auto start2 = std::chrono::system_clock::now();
	auto dwRaceIndex = 0;
	for (const auto& it : m_vecRaceData)
	{
		CRaceData* pRaceData = nullptr;
		race_manager.CreateRace(dwRaceIndex);
		if (race_manager.GetRaceDataPointer(dwRaceIndex, &pRaceData))
		{
			pRaceData->LoadRaceData(it.stFileName.c_str());

			__SetIntroMotions(pRaceData, it.stFolderName);
			__LoadEmotions(pRaceData, it.stFolderName);;
			__LoadGameRace(pRaceData, it.stFolderName);
			__LoadHorse(pRaceData, it.stFolderName);
			__LoadFishing(pRaceData, it.stFolderName);
			__LoadGuildSkill(pRaceData, it.stFolderName);
			__LoadSkill(pRaceData, it.stFolderName);


			pRaceData->LoadAllMotions();
			++dwRaceIndex;
		}
	}
	
	for (const auto& granny_file : m_vecGrannyFiles)
		resource_manager.LoadMotionFile(granny_file);
}

///////////////////////////////////////////////////
/// initPlayerSettingsModule
///////////////////////////////////////////////////

auto appRegisterSkill(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bRaceIndex = 0, bSkillGroupIndex = 0, bEmpireIndex = 0;
	if (!PyTuple_GetByte(poArgs, 0, &bRaceIndex) || !PyTuple_GetByte(poArgs, 1, &bSkillGroupIndex) || !PyTuple_GetByte(poArgs, 2, &bEmpireIndex))
		return Py_BuildException();

	RegisterSkill(bRaceIndex, bSkillGroupIndex, bEmpireIndex);
	return Py_BuildNone();
}

PyObject * appGetEmotionPath(PyObject* poSelf, PyObject* poArgs)
{
	const auto & poList = PyList_New(0);
	for (const auto & it : m_vecRaceData)
		PyList_Append(poList, PyString_FromString(it.stFolderName.c_str()));

	return poList;
}

void TraceErrorHandler(const char* c_szFormat)
{
	TraceError(c_szFormat);
}

void initPlayerSettingsModule()
{
	auto* pkPlayer = &CPythonPlayer::Instance();
	auto* pkItem = &CPythonItem::Instance();
	auto* pkFlying = &CFlyingManager::Instance();
	auto* pkNetStream = &CPythonNetworkStream::Instance();
	auto* pkResource = &CResourceManager::Instance();
	auto* pkRaceManager = &CRaceManager::Instance();

	PythonPlayerSettingsModuleLibRegisterFunctions(
		[pkItem](const DWORD eItemType, const std::string& c_rstFileName) { pkItem->SetUseSoundFileName(eItemType, c_rstFileName); },
		[pkItem](const DWORD eItemType, const std::string& c_rstFileName) { pkItem->SetDropSoundFileName(eItemType, c_rstFileName); },
		[pkFlying](const DWORD dwIndex, const BYTE byType, const char* c_szFileName) { pkFlying->RegisterIndexedFlyData(dwIndex, byType, c_szFileName); },
		[pkNetStream](const char* pcEmoticonString) { pkNetStream->RegisterEmoticonString(pcEmoticonString); }, 
		[pkPlayer](const DWORD dwSlotIndex, const DWORD dwSkillIndex) { pkPlayer->SetSkill(dwSlotIndex, dwSkillIndex); },
		[pkPlayer](const DWORD dwEID, const char * c_szEftFileName, bool isCache) { pkPlayer->RegisterEffect(dwEID, c_szEftFileName, isCache); },
		[pkResource](const std::string& stMotionFileName) { pkResource->LoadMotionFile(stMotionFileName); },
		[pkRaceManager](const DWORD dwRaceIndex, const char* c_szName) { pkRaceManager->RegisterRaceName(dwRaceIndex, c_szName); },
		[pkRaceManager](const char* c_szName, const char* c_szSrcName) { pkRaceManager->RegisterRaceSrcName(c_szName, c_szSrcName); },
		CInstanceBase::SetDustGap,
		CInstanceBase::SetHorseDustGap,
		CInstanceBase::RegisterEffect,
		TraceErrorHandler);
	
	PythonPlayerSettingsModuleLibRegisterVariables(m_vecPassiveGuildSkillIndex, m_vecActiveGuildSkillIndex,
		m_vecSkillIndex, m_vecHorse, m_vecSkillFileName, m_vecEffects,
		m_vecEmoticons, m_vecRaceData, m_vecUseSoundFileName,
		m_vecDropSoundFileName, m_vecFlyEffects, c_iSkillIndex_Language1);

	static struct PyMethodDef s_methods[] =
	{
		{ "RegisterSkill", appRegisterSkill, METH_VARARGS },
		{ nullptr, nullptr, 0 },
	};

	Py_InitModule("playerLoad", s_methods);
}
#endif
