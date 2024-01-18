#pragma once

#if defined(ENABLE_CLIENT_OPTIMIZATION)
#include "InstanceBase.h"
#include "PythonItem.h"
#include <cstdint>
#include <initializer_list>
#include <array>
#include <PythonPlayerSettingsModuleLib.h>

static const auto EMOTICON_PATH = "d:/ymir work/effect/etc/emoticon/";

enum
{
	HORSE_SKILL_WILDATTACK = CRaceMotionData::NAME_SKILL + 121,
	HORSE_SKILL_CHARGE = CRaceMotionData::NAME_SKILL + 122,
	HORSE_SKILL_SPLASH = CRaceMotionData::NAME_SKILL + 123,

	GUILD_SKILL_DRAGONBLOOD = CRaceMotionData::NAME_SKILL + 101,
	GUILD_SKILL_DRAGONBLESS = CRaceMotionData::NAME_SKILL + 102,
	GUILD_SKILL_BLESSARMOR = CRaceMotionData::NAME_SKILL + 103,
	GUILD_SKILL_SPPEDUP = CRaceMotionData::NAME_SKILL + 104,
	GUILD_SKILL_DRAGONWRATH = CRaceMotionData::NAME_SKILL + 105,
	GUILD_SKILL_MAGICUP = CRaceMotionData::NAME_SKILL + 106,

	COMBO_TYPE_1 = 0,
	COMBO_TYPE_2 = 1,
	COMBO_TYPE_3 = 2,

	COMBO_INDEX_1 = 0,
	COMBO_INDEX_2 = 1,
	COMBO_INDEX_3 = 2,
	COMBO_INDEX_4 = 3,
	COMBO_INDEX_5 = 4,
	COMBO_INDEX_6 = 5,
};

//! @brief skill index of passive guild skill
static const std::vector<uint8_t> m_vecPassiveGuildSkillIndex = { 151, };
//! @brief skill index of active guild skill
static const std::vector<uint8_t> m_vecActiveGuildSkillIndex = { 152, 153, 154, 155, 156, 157, };

//! @brief Skill index vector with for all characters
static const std::vector<TSkillIndex> m_vecSkillIndex =
{
//	{
//		NRaceData::JOB_WARRIOR,
//		{
//#if defined(ENABLE_NEW_PASSIVE_SKILLS)
//			{ 1, { 1, 2, 3, 4, 5, 6, 236, 0, 0, 0, 137, 0, 138, 0, 139, 0, } },
//			{ 2, { 16, 17, 18, 19, 20, 21, 240, 0, 0, 0, 137, 0, 138, 0, 139, 0, } },
//#else
//			{ 1, { 1, 2, 3, 4, 5, 6, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, } },
//			{ 2, { 16, 17, 18, 19, 20, 21, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, } },
//#endif
//		},
//		//{ 0, 0, 121, 134, 133, 129, 130, 131, 123, 124, 122, 132, 0, 0, 0, 0, 0, 0 }
//		{ 122, 123, 121, 130, 131, 143, 144, 145, 146, 0, 0, 0 }
//	},
//	{
//		NRaceData::JOB_ASSASSIN,
//		{
//#if defined(ENABLE_NEW_PASSIVE_SKILLS)
//			{ 1, { 31, 32, 33, 34, 35, 36, 237, 0, 0, 0, 137, 0, 138, 0, 139, 0, 140 } },
//			{ 2, { 46, 47, 48, 49, 50, 51, 241, 0, 0, 0, 137, 0, 138, 0, 139, 0, 140 } },
//#else
//			{ 1, { 31, 32, 33, 34, 35, 36, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 140 } },
//			{ 2, { 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 140 } },
//#endif
//		},
//		//{ 0, 0, 121, 134, 133, 129, 130, 131, 123, 124, 122, 132, 0, 0, 0, 0, 0, 0 }
//		{ 122, 123, 121, 130, 131, 143, 144, 145, 146, 0, 0, 0 }
//	},
//	{
//		NRaceData::JOB_SURA,
//		{
//#if defined(ENABLE_NEW_PASSIVE_SKILLS)
//			{ 1, { 61, 62, 63, 64, 65, 66, 238, 0, 179, 0, 137, 0, 138, 0, 139, 0 } },
//			{ 2, { 76, 77, 78, 79, 80, 81, 242, 0, 180, 0, 137, 0, 138, 0, 139, 0 } },
//#else
//			{ 1, { 61, 62, 63, 64, 65, 66, 0, 0, 179, 0, 137, 0, 138, 0, 139, 0 } },
//			{ 2, { 76, 77, 78, 79, 80, 81, 0, 0, 180, 0, 137, 0, 138, 0, 139, 0 } },
//#endif
//		},
//		//{ 0, 0, 121, 134, 133, 129, 130, 131, 123, 124, 122, 132, 0, 0, 0, 0, 0, 0 }
//		{ 122, 123, 121, 130, 131, 143, 144, 145, 146, 0, 0, 0 }
//	},
//	{
//		NRaceData::JOB_SHAMAN,
//		{
//#if defined(ENABLE_NEW_PASSIVE_SKILLS)
//			{ 1, { 91, 92, 93, 94, 95, 96, 239, 0, 0, 0, 137, 0, 138, 0, 139, 0 } },
//			{ 2, { 106, 107, 108, 109, 110, 111, 243, 0, 0, 0, 137, 0, 138, 0, 139, 0 } },
//#else
//			{ 1, { 91, 92, 93, 94, 95, 96, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0 } },
//			{ 2, { 106, 107, 108, 109, 110, 111, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0 } },
//#endif
//		},
//		//{ 0, 0, 121, 134, 133, 129, 130, 131, 123, 124, 122, 132, 0, 0, 0, 0, 0, 0 }
//		{ 122, 123, 121, 130, 131, 143, 144, 145, 146, 0, 0, 0 }
//	},
};

/**
* \brief Concatenate unlimited string arguments into one string.
*/
static auto string_path(const std::initializer_list<std::string> & args) -> std::string
{
	std::string stPathName;
	for (const auto & it : args)
		stPathName.append(it);
	return stPathName;
}

/**
* \brief
*/
static const std::vector<THorse> m_vecHorse =
{
	{ NRaceData::JOB_WARRIOR, { CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::MODE_HORSE_TWOHAND_SWORD } },
	{ NRaceData::JOB_ASSASSIN, { CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::MODE_HORSE_DUALHAND_SWORD } },
	{ NRaceData::JOB_SURA, { CRaceMotionData::MODE_HORSE_ONEHAND_SWORD } },
	{ NRaceData::JOB_SHAMAN, { CRaceMotionData::MODE_HORSE_FAN, CRaceMotionData::MODE_HORSE_BELL } },
};

/**
* \brief
*/
static const std::vector<TSkillData> m_vecSkillFileName =
{
	{
		NRaceData::JOB_WARRIOR,
		{
			"samyeon", "palbang", "jeongwi", "geomgyeong", "tanhwan", "gihyeol"
			, "gigongcham", "gyeoksan", "daejin", "cheongeun", "geompung", "noegeom"
		},
	},
	{
		NRaceData::JOB_ASSASSIN,
		{
			"amseup", "gungsin", "charyun", "eunhyeong", "sangong", "seomjeon"
			, "yeonsa", "gwangyeok", "hwajo", "gyeonggong", "dokgigung", "seomgwang"
		},
	},
	{
		NRaceData::JOB_SHAMAN,
		{
			"bipabu", "yongpa", "paeryong", "hosin", "boho", "gicheon"
			, "noejeon", "byeorak", "pokroe", "jeongeop", "kwaesok", "jeungryeok"
		},
		{
			"bipabu", "yongpa", "paeryong", "hosin_target", "boho_target", "gicheon_target"
			, "noejeon", "byeorak", "pokroe", "jeongeop_target", "kwaesok_target", "jeungryeok_target"
		},
	},
	{
		NRaceData::JOB_SURA,
		{
			"swaeryeong", "yonggwon", "gwigeom", "gongpo", "jumagap", "pabeop"
			, "maryeong", "hwayeom", "muyeong", "heuksin", "tusok", "mahwan"
		},
	},
};

/**
* \brief Cached effects
*/
static const std::vector<TCacheEffect> m_vecEffects =
{
	{ CInstanceBase::EFFECT_DUST, "", "d:/ymir work/effect/etc/dust/dust.mse", true },
	{ CInstanceBase::EFFECT_HORSE_DUST, "", "d:/ymir work/effect/etc/dust/running_dust.mse", true },
	{ CInstanceBase::EFFECT_HIT, "", "d:/ymir work/effect/hit/blow_1/blow_1_low.mse", true },
	{ CInstanceBase::EFFECT_HPUP_RED, "", "d:/ymir work/effect/etc/recuperation/drugup_red.mse", true },
	{ CInstanceBase::EFFECT_SPUP_BLUE, "", "d:/ymir work/effect/etc/recuperation/drugup_blue.mse", true },
	{ CInstanceBase::EFFECT_SPEEDUP_GREEN, "", "d:/ymir work/effect/etc/recuperation/drugup_green.mse", true },
	{ CInstanceBase::EFFECT_DXUP_PURPLE, "", "d:/ymir work/effect/etc/recuperation/drugup_purple.mse", true },
	{ CInstanceBase::EFFECT_AUTO_HPUP, "", "d:/ymir work/effect/etc/recuperation/autodrugup_red.mse", true },
	{ CInstanceBase::EFFECT_AUTO_SPUP, "", "d:/ymir work/effect/etc/recuperation/autodrugup_blue.mse", true },
	{ CInstanceBase::EFFECT_RAMADAN_RING_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item1.mse", true },
	{ CInstanceBase::EFFECT_HALLOWEEN_CANDY_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item2.mse", true },
	{ CInstanceBase::EFFECT_HAPPINESS_RING_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item3.mse", true },
	{ CInstanceBase::EFFECT_LOVE_PENDANT_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item4.mse", true },
#if defined(ENABLE_ACCE_SYSTEM)
	{ CInstanceBase::EFFECT_ACCE_SUCCEDED, "", "d:/ymir work/effect/etc/buff/buff_item6.mse", true },
	{ CInstanceBase::EFFECT_ACCE_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item7.mse", true },
#endif
#if defined(VERSION_162_ENABLED)
	{ CInstanceBase::EFFECT_HEALER, "", "d:/ymir work/effect/monster2/healer/healer_effect.mse", true },
#endif
#if defined(__EFFETTO_MANTELLO__)
	{ CInstanceBase::EFFECT_MANTELLO, "", "d:/ymir work/effect/etc/buff/buff_item9_cape.mse", true },
#endif
	{ CInstanceBase::EFFECT_PENETRATE, "Bip01", "d:/ymir work/effect/hit/gwantong.mse", true },
	{ CInstanceBase::EFFECT_FIRECRACKER, "", "d:/ymir work/effect/etc/firecracker/newyear_firecracker.mse", true },
	{ CInstanceBase::EFFECT_SPIN_TOP, "", "d:/ymir work/effect/etc/firecracker/paing_i.mse", true },
	{ CInstanceBase::EFFECT_SELECT, "", "d:/ymir work/effect/etc/click/click_select.mse", true },
	{ CInstanceBase::EFFECT_TARGET, "", "d:/ymir work/effect/etc/click/click_glow_select.mse", true },
	{ CInstanceBase::EFFECT_STUN, "Bip01 Head", "d:/ymir work/effect/etc/stun/stun.mse", true },
	{ CInstanceBase::EFFECT_CRITICAL, "Bip01 R Hand", "d:/ymir work/effect/hit/critical.mse", true },
	{ CInstanceBase::EFFECT_DAMAGE_TARGET, "", "d:/ymir work/effect/affect/damagevalue/target.mse", true },
	{ CInstanceBase::EFFECT_DAMAGE_NOT_TARGET, "", "d:/ymir work/effect/affect/damagevalue/nontarget.mse", true },
	{ CInstanceBase::EFFECT_DAMAGE_SELFDAMAGE, "", "d:/ymir work/effect/affect/damagevalue/damage.mse", true },
	{ CInstanceBase::EFFECT_DAMAGE_SELFDAMAGE2, "", "d:/ymir work/effect/affect/damagevalue/damage_1.mse", true },
	{ CInstanceBase::EFFECT_DAMAGE_POISON, "", "d:/ymir work/effect/affect/damagevalue/poison.mse", true },
#if defined(USE_DMG_RENEWAL)
	{ CInstanceBase::EFFECT_DAMAGE_FIRE, "", "d:/ymir work/effect/affect/damagevalue/burn.mse", true },
#endif
	{ CInstanceBase::EFFECT_DAMAGE_MISS, "", "d:/ymir work/effect/affect/damagevalue/miss.mse", true },
	{ CInstanceBase::EFFECT_DAMAGE_TARGETMISS, "", "d:/ymir work/effect/affect/damagevalue/target_miss.mse", true },
	{ CInstanceBase::EFFECT_LEVELUP_ON_14_FOR_GERMANY, "", "season1/effect/paymessage_warning.mse", true },
	{ CInstanceBase::EFFECT_LEVELUP_UNDER_15_FOR_GERMANY, "", "season1/effect/paymessage_decide.mse", true },
	{ CInstanceBase::EFFECT_PERCENT_DAMAGE1, "", "d:/ymir work/effect/hit/percent_damage1.mse", true },
	{ CInstanceBase::EFFECT_PERCENT_DAMAGE2, "", "d:/ymir work/effect/hit/percent_damage2.mse", true },
	{ CInstanceBase::EFFECT_PERCENT_DAMAGE3, "", "d:/ymir work/effect/hit/percent_damage3.mse", true },

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// No cache
	///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(USE_NEW_GYEONGGONG_SKILL)
	{ CInstanceBase::EFFECT_GYEONGGONG_BOOM, "Bip01", "d:/ymir work/effect/hit/gyeonggong_boom.mse", false },
#endif
#if defined(USE_BATTLEPASS)
	{CInstanceBase::EFFECT_BP_MISSION_COMPLETED, "", "d:/ymir work/effect/battlepass/premium_mission_complete.mse", true},
	{CInstanceBase::EFFECT_BP_COMPLETED, "", "d:/ymir work/effect/battlepass/premium_battlepass_complete.mse", true},
#endif
	{ CInstanceBase::EFFECT_SPAWN_APPEAR, "Bip01", "d:/ymir work/effect/etc/appear_die/monster_appear.mse", false },
	{ CInstanceBase::EFFECT_SPAWN_DISAPPEAR, "Bip01", "d:/ymir work/effect/etc/appear_die/monster_die.mse", false },
	{ CInstanceBase::EFFECT_FLAME_ATTACK, "equip_right_hand", "d:/ymir work/effect/hit/blow_flame/flame_3_weapon.mse", false },
	{ CInstanceBase::EFFECT_FLAME_HIT, "", "d:/ymir work/effect/hit/blow_flame/flame_3_blow.mse", false },
	{ CInstanceBase::EFFECT_FLAME_ATTACH, "", "d:/ymir work/effect/hit/blow_flame/flame_3_body.mse", false },
	{ CInstanceBase::EFFECT_ELECTRIC_ATTACK, "equip_right", "d:/ymir work/effect/hit/blow_electric/light_1_weapon.mse", false },
	{ CInstanceBase::EFFECT_ELECTRIC_HIT, "", "d:/ymir work/effect/hit/blow_electric/light_1_blow.mse", false },
	{ CInstanceBase::EFFECT_ELECTRIC_ATTACH, "", "d:/ymir work/effect/hit/blow_electric/light_1_body.mse", false },
	{ CInstanceBase::EFFECT_LEVELUP, "", "d:/ymir work/effect/etc/levelup_1/level_up.mse", false },
	{ CInstanceBase::EFFECT_SKILLUP, "", "d:/ymir work/effect/etc/skillup/skillup_1.mse", false },
	{ CInstanceBase::EFFECT_EMPIRE + 1, "Bip01", "d:/ymir work/effect/etc/empire/empire_A.mse", false },
	{ CInstanceBase::EFFECT_EMPIRE + 2, "Bip01", "d:/ymir work/effect/etc/empire/empire_B.mse", false },
	{ CInstanceBase::EFFECT_EMPIRE + 3, "Bip01", "d:/ymir work/effect/etc/empire/empire_C.mse", false },
	{ CInstanceBase::EFFECT_WEAPON + 1, "equip_right_hand", "d:/ymir work/pc/warrior/effect/geom_sword_loop.mse", false },
	{ CInstanceBase::EFFECT_WEAPON + 2, "equip_right_hand", "d:/ymir work/pc/warrior/effect/geom_spear_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 0, "Bip01", "locale/it/effect/gm.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_POISON, "Bip01", "d:/ymir work/effect/hit/blow_poison/poison_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_SLOW, "", "d:/ymir work/effect/affect/slow.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_STUN, "Bip01 Head", "d:/ymir work/effect/etc/stun/stun_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_DUNGEON_READY, "", "d:/ymir work/effect/etc/ready/ready.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_CHEONGEUN, "", "d:/ymir work/pc/warrior/effect/gyeokgongjang_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_GYEONGGONG, "", "d:/ymir work/pc/assassin/effect/gyeonggong_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_GWIGEOM, "Bip01 R Finger2", "d:/ymir work/pc/sura/effect/gwigeom_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_GONGPO, "", "d:/ymir work/pc/sura/effect/fear_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_JUMAGAP, "", "d:/ymir work/pc/sura/effect/jumagap_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_HOSIN, "", "d:/ymir work/pc/shaman/effect/3hosin_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_BOHO, "", "d:/ymir work/pc/shaman/effect/boho_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_KWAESOK, "", "d:/ymir work/pc/shaman/effect/10kwaesok_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_HEUKSIN, "", "d:/ymir work/pc/sura/effect/heuksin_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_MUYEONG, "", "d:/ymir work/pc/sura/effect/muyeong_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_FIRE, "Bip01", "d:/ymir work/effect/hit/hwayeom_loop_1.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_GICHEON, "Bip01 R Hand", "d:/ymir work/pc/shaman/effect/6gicheon_hand.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_JEUNGRYEOK, "Bip01 L Hand", "d:/ymir work/pc/shaman/effect/jeungryeok_hand.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_PABEOP, "Bip01 Head", "d:/ymir work/pc/sura/effect/pabeop_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_FALLEN_CHEONGEUN, "", "d:/ymir work/pc/warrior/effect/gyeokgongjang_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_WAR_FLAG1, "", "d:/ymir work/effect/etc/guild_war_flag/flag_red.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_WAR_FLAG2, "", "d:/ymir work/effect/etc/guild_war_flag/flag_blue.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_WAR_FLAG3, "", "d:/ymir work/effect/etc/guild_war_flag/flag_yellow.mse", false },
#if defined(ENABLE_MELEY_LAIR_DUNGEON)
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_STATUE1, "", "d:/ymir work/effect/monster2/redd_moojuk.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_STATUE2, "", "d:/ymir work/effect/monster2/redd_moojuk.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_STATUE3, "", "d:/ymir work/effect/monster2/redd_moojuk_blue.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_STATUE4, "", "d:/ymir work/effect/monster2/redd_moojuk_green.mse", false },
#endif
	{ CInstanceBase::EFFECT_REFINED + 1, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_7.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 2, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_8.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 3, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_9.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 4, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_7_b.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 5, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_8_b.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 6, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_9_b.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 7, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_7_f.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 8, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_8_f.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 9, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_9_f.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 10, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_7_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 11, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_8_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 12, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_9_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 13, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_7_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 14, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_8_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 15, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_9_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 16, "Bip01", "D:/ymir work/pc/common/effect/armor/armor_7.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 17, "Bip01", "D:/ymir work/pc/common/effect/armor/armor_8.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 18, "Bip01", "D:/ymir work/pc/common/effect/armor/armor_9.mse", false },
#if defined(ENABLE_COSTUME_EFFECT)
	{ CInstanceBase::EFFECT_REFINED + 76, "Bip01", "d:/ymir work/effect/shining/nero/ridack_armor_black.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 77, "PART_WEAPON", "d:/ymir work/effect/shining/nero/ridack_sword_black.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 78, "PART_WEAPON", "d:/ymir work/effect/shining/nero/ridack_spear_black.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 79, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining/nero/ridack_knife_black.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 80, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining/nero/ridack_bow_black.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 81, "PART_WEAPON", "d:/ymir work/effect/shining/nero/ridack_bell_black.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 82, "PART_WEAPON", "d:/ymir work/effect/shining/nero/ridack_fan_black.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 83, "PART_WEAPON", "d:/ymir work/effect/shining/nero/ridack_knife_black.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 131, "Bip01", "d:/ymir work/effect/shining/viola/ridack_armor_fushiia.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 132, "PART_WEAPON", "d:/ymir work/effect/shining/viola/ridack_sword_fushiia.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 133, "PART_WEAPON", "d:/ymir work/effect/shining/viola/ridack_spear_fushiia.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 134, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining/viola/ridack_knife_fushiia.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 135, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining/viola/ridack_bow_fushiia.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 136, "PART_WEAPON", "d:/ymir work/effect/shining/viola/ridack_bell_fushiia.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 137, "PART_WEAPON", "d:/ymir work/effect/shining/viola/ridack_fan_fushiia.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 138, "PART_WEAPON", "d:/ymir work/effect/shining/viola/ridack_knife_fushiia.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 139, "Bip01", "d:/ymir work/effect/shining/orange/ridack_armor_orange.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 140, "PART_WEAPON", "d:/ymir work/effect/shining/orange/ridack_sword_orange.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 141, "PART_WEAPON", "d:/ymir work/effect/shining/orange/ridack_spear_orange.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 142, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining/orange/ridack_knife_orange.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 143, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining/orange/ridack_bow_orange.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 144, "PART_WEAPON", "d:/ymir work/effect/shining/orange/ridack_bell_orange.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 145, "PART_WEAPON", "d:/ymir work/effect/shining/orange/ridack_fan_orange.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 146, "PART_WEAPON", "d:/ymir work/effect/shining/orange/ridack_knife_orange.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 147, "Bip01", "d:/ymir work/effect/shining/verdino/ridack_armor_jade.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 148, "PART_WEAPON", "d:/ymir work/effect/shining/verdino/ridack_sword_jade.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 149, "PART_WEAPON", "d:/ymir work/effect/shining/verdino/ridack_spear_jade.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 150, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining/verdino/ridack_knife_jade.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 151, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining/verdino/ridack_bow_jade.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 152, "PART_WEAPON", "d:/ymir work/effect/shining/verdino/ridack_bell_jade.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 153, "PART_WEAPON", "d:/ymir work/effect/shining/verdino/ridack_fan_jade.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 154, "PART_WEAPON", "d:/ymir work/effect/shining/verdino/ridack_knife_jade.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 155, "Bip01", "d:/ymir work/effect/shining1/viola/ridack_armor_purple2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 156, "PART_WEAPON", "d:/ymir work/effect/shining1/viola/ridack_sword_purple2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 157, "PART_WEAPON", "d:/ymir work/effect/shining1/viola/ridack_spear_purple2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 158, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining1/viola/ridack_knife_purple2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 159, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining1/viola/ridack_bow_purple2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 160, "PART_WEAPON", "d:/ymir work/effect/shining1/viola/ridack_bell_purple2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 161, "PART_WEAPON", "d:/ymir work/effect/shining1/viola/ridack_fan_purple2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 162, "PART_WEAPON", "d:/ymir work/effect/shining1/viola/ridack_knife_purple2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 163, "Bip01", "d:/ymir work/effect/shining1/blu/ridack_armor_blue2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 164, "PART_WEAPON", "d:/ymir work/effect/shining1/blu/ridack_sword_blue2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 165, "PART_WEAPON", "d:/ymir work/effect/shining1/blu/ridack_spear_blue2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 166, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining1/blu/ridack_knife_blue2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 167, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining1/blu/ridack_bow_blue2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 168, "PART_WEAPON", "d:/ymir work/effect/shining1/blu/ridack_bell_blue2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 169, "PART_WEAPON", "d:/ymir work/effect/shining1/blu/ridack_fan_blue2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 170, "PART_WEAPON", "d:/ymir work/effect/shining1/blu/ridack_knife_blue2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 171, "Bip01", "d:/ymir work/effect/shining1/rosa/ridack_armor_pink2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 172, "PART_WEAPON", "d:/ymir work/effect/shining1/rosa/ridack_sword_pink2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 173, "PART_WEAPON", "d:/ymir work/effect/shining1/rosa/ridack_spear_pink2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 174, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining1/rosa/ridack_knife_pink2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 175, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining1/rosa/ridack_bow_pink2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 176, "PART_WEAPON", "d:/ymir work/effect/shining1/rosa/ridack_bell_pink2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 177, "PART_WEAPON", "d:/ymir work/effect/shining1/rosa/ridack_fan_pink2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 178, "PART_WEAPON", "d:/ymir work/effect/shining1/rosa/ridack_knife_pink2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 179, "Bip01", "d:/ymir work/effect/shining1/bianco/ridack_armor_white2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 180, "PART_WEAPON", "d:/ymir work/effect/shining1/bianco/ridack_sword_white2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 181, "PART_WEAPON", "d:/ymir work/effect/shining1/bianco/ridack_spear_white2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 182, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining1/bianco/ridack_knife_white2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 183, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining1/bianco/ridack_bow_white2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 184, "PART_WEAPON", "d:/ymir work/effect/shining1/bianco/ridack_bell_white2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 185, "PART_WEAPON", "d:/ymir work/effect/shining1/bianco/ridack_fan_white2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 186, "PART_WEAPON", "d:/ymir work/effect/shining1/bianco/ridack_knife_white2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 187, "Bip01", "d:/ymir work/effect/shining2/verde/ridack_armor_green.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 188, "PART_WEAPON", "d:/ymir work/effect/shining2/verde/ridack_sword_green.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 189, "PART_WEAPON", "d:/ymir work/effect/shining2/verde/ridack_spear_green.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 190, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining2/verde/ridack_knife_green.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 191, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining2/verde/ridack_bow_green.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 192, "PART_WEAPON", "d:/ymir work/effect/shining2/verde/ridack_bell_green.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 193, "PART_WEAPON", "d:/ymir work/effect/shining2/verde/ridack_fan_green.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 194, "PART_WEAPON", "d:/ymir work/effect/shining2/verde/ridack_knife_green.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 195, "Bip01", "d:/ymir work/effect/shining2/rosso/ridack_armor_red.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 196, "PART_WEAPON", "d:/ymir work/effect/shining2/rosso/ridack_sword_red.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 197, "PART_WEAPON", "d:/ymir work/effect/shining2/rosso/ridack_spear_red.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 198, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining2/rosso/ridack_knife_red.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 199, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining2/rosso/ridack_bow_red.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 200, "PART_WEAPON", "d:/ymir work/effect/shining2/rosso/ridack_bell_red.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 201, "PART_WEAPON", "d:/ymir work/effect/shining2/rosso/ridack_fan_red.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 202, "PART_WEAPON", "d:/ymir work/effect/shining2/rosso/ridack_knife_red.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 203, "Bip01", "d:/ymir work/effect/shining2/blu/ridack_armor_blue.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 204, "PART_WEAPON", "d:/ymir work/effect/shining2/blu/ridack_sword_blue.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 205, "PART_WEAPON", "d:/ymir work/effect/shining2/blu/ridack_spear_blue.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 206, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining2/blu/ridack_knife_blue.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 207, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining2/blu/ridack_bow_blue.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 208, "PART_WEAPON", "d:/ymir work/effect/shining2/blu/ridack_bell_blue.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 209, "PART_WEAPON", "d:/ymir work/effect/shining2/blu/ridack_fan_blue.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 210, "PART_WEAPON", "d:/ymir work/effect/shining2/blu/ridack_knife_blue.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 211, "Bip01", "d:/ymir work/effect/shining2/giallo/ridack_armor_yellow.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 212, "PART_WEAPON", "d:/ymir work/effect/shining2/giallo/ridack_sword_yellow.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 213, "PART_WEAPON", "d:/ymir work/effect/shining2/giallo/ridack_spear_yellow.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 214, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining2/giallo/ridack_knife_yellow.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 215, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining2/giallo/ridack_bow_yellow.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 216, "PART_WEAPON", "d:/ymir work/effect/shining2/giallo/ridack_bell_yellow.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 217, "PART_WEAPON", "d:/ymir work/effect/shining2/giallo/ridack_fan_yellow.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 218, "PART_WEAPON", "d:/ymir work/effect/shining2/giallo/ridack_knife_yellow.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 794, "Bip01", "d:/ymir work/effect/shining13/ridack_armor_d.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 795, "PART_WEAPON", "d:/ymir work/effect/shining13/ridack_weapon_d.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 796, "PART_WEAPON", "d:/ymir work/effect/shining13/ridack_weapon_d_2h.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 797, "PART_WEAPON", "d:/ymir work/effect/shining13/ridack_weapon_d_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 798, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining13/ridack_weapon_d_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 799, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining13/ridack_weapon_d_b.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 800, "PART_WEAPON", "d:/ymir work/effect/shining13/ridack_weapon_d_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 801, "PART_WEAPON", "d:/ymir work/effect/shining13/ridack_weapon_d_f.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 802, "PART_WEAPON_LEFT", "d:/ymir work/effect/shining13/ridack_weapon_d_f.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 823, "Bip01", "d:/ymir work/effect/new_shinings/ridack_effect_blue/ridack_armorblue.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 824, "PART_WEAPON", "d:/ymir work/effect/new_shinings/ridack_effect_blue/weaponblue.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 825, "PART_WEAPON", "d:/ymir work/effect/new_shinings/ridack_effect_blue/weaponblue_2h.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 826, "PART_WEAPON", "d:/ymir work/effect/new_shinings/ridack_effect_blue/weaponblue_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 827, "PART_WEAPON_LEFT", "d:/ymir work/effect/new_shinings/ridack_effect_blue/weaponblue_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 828, "PART_WEAPON_LEFT", "d:/ymir work/effect/new_shinings/ridack_effect_blue/weaponblue_b.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 829, "PART_WEAPON", "d:/ymir work/effect/new_shinings/ridack_effect_blue/weaponblue_c.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 830, "PART_WEAPON", "d:/ymir work/effect/new_shinings/ridack_effect_blue/weaponblue_f.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 831, "PART_WEAPON_LEFT", "d:/ymir work/effect/new_shinings/ridack_effect_blue/weaponblue_f.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 832, "Bip01", "d:/ymir work/effect/new_shinings/ridack_effect_green/ridack_armorgreen.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 833, "PART_WEAPON", "d:/ymir work/effect/new_shinings/ridack_effect_green/weapongreen.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 834, "PART_WEAPON", "d:/ymir work/effect/new_shinings/ridack_effect_green/weapongreen_2h.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 835, "PART_WEAPON", "d:/ymir work/effect/new_shinings/ridack_effect_green/weapongreen_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 836, "PART_WEAPON_LEFT", "d:/ymir work/effect/new_shinings/ridack_effect_green/weapongreen_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 837, "PART_WEAPON_LEFT", "d:/ymir work/effect/new_shinings/ridack_effect_green/weapongreen_b.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 838, "PART_WEAPON", "d:/ymir work/effect/new_shinings/ridack_effect_green/weapongreen_c.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 839, "PART_WEAPON", "d:/ymir work/effect/new_shinings/ridack_effect_green/weapongreen_f.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 840, "PART_WEAPON_LEFT", "d:/ymir work/effect/new_shinings/ridack_effect_green/weapongreen_f.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 841, "Bip01", "d:/ymir work/effect/new_shinings/ridack_effect_red/ridack_armorred.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 842, "PART_WEAPON", "d:/ymir work/effect/new_shinings/ridack_effect_red/weaponred.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 843, "PART_WEAPON", "d:/ymir work/effect/new_shinings/ridack_effect_red/weaponred_2h.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 844, "PART_WEAPON", "d:/ymir work/effect/new_shinings/ridack_effect_red/weaponred_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 845, "PART_WEAPON_LEFT", "d:/ymir work/effect/new_shinings/ridack_effect_red/weaponred_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 846, "PART_WEAPON_LEFT", "d:/ymir work/effect/new_shinings/ridack_effect_red/weaponred_b.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 847, "PART_WEAPON", "d:/ymir work/effect/new_shinings/ridack_effect_red/weaponred_c.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 848, "PART_WEAPON", "d:/ymir work/effect/new_shinings/ridack_effect_red/weaponred_f.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 849, "PART_WEAPON_LEFT", "d:/ymir work/effect/new_shinings/ridack_effect_red/weaponred_f.mse", false },
#endif
#if defined(ENABLE_SOUL_SYSTEM)
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_SOUL_RED, "", "d:/ymir work/effect/etc/soul/soul_red_001.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_SOUL_BLUE, "", "d:/ymir work/effect/etc/soul/soul_blue_001.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_SOUL_MIX, "", "d:/ymir work/effect/etc/soul/soul_mix_001.mse", false },
#endif
};

static const std::vector<TEmotion> m_vecEmotions = {
	{ CRaceMotionData::NAME_CLAP, "clap.msa" },
	{ CRaceMotionData::NAME_CHEERS_1, "cheers_1.msa" },
	{ CRaceMotionData::NAME_CHEERS_2, "cheers_2.msa" },
	{ CRaceMotionData::NAME_DANCE_1, "dance_1.msa" },
	{ CRaceMotionData::NAME_DANCE_2, "dance_2.msa" },
	{ CRaceMotionData::NAME_DANCE_3, "dance_3.msa" },
	{ CRaceMotionData::NAME_DANCE_4, "dance_4.msa" },
	{ CRaceMotionData::NAME_DANCE_5, "dance_5.msa" },
	{ CRaceMotionData::NAME_DANCE_6, "dance_6.msa" },
	{ CRaceMotionData::NAME_CONGRATULATION, "congratulation.msa" },
	{ CRaceMotionData::NAME_FORGIVE, "forgive.msa" },
	{ CRaceMotionData::NAME_ANGRY, "angry.msa" },
	{ CRaceMotionData::NAME_ATTRACTIVE, "attractive.msa" },
	{ CRaceMotionData::NAME_SAD, "sad.msa" },
	{ CRaceMotionData::NAME_SHY, "shy.msa" },
	{ CRaceMotionData::NAME_CHEERUP, "cheerup.msa" },
	{ CRaceMotionData::NAME_BANTER, "banter.msa" },
	{ CRaceMotionData::NAME_JOY, "joy.msa" },
	{ CRaceMotionData::NAME_FRENCH_KISS_WITH_WARRIOR, "french_kiss_with_warrior.msa" },
	{ CRaceMotionData::NAME_FRENCH_KISS_WITH_ASSASSIN, "french_kiss_with_assassin.msa" },
	{ CRaceMotionData::NAME_FRENCH_KISS_WITH_SURA, "french_kiss_with_sura.msa" },
	{ CRaceMotionData::NAME_FRENCH_KISS_WITH_SHAMAN, "french_kiss_with_shaman.msa" },
	{ CRaceMotionData::NAME_KISS_WITH_WARRIOR, "kiss_with_warrior.msa" },
	{ CRaceMotionData::NAME_KISS_WITH_ASSASSIN, "kiss_with_assassin.msa" },
	{ CRaceMotionData::NAME_KISS_WITH_SURA, "kiss_with_sura.msa" },
	{ CRaceMotionData::NAME_KISS_WITH_SHAMAN, "kiss_with_shaman.msa" },
	{ CRaceMotionData::NAME_SLAP_HIT_WITH_WARRIOR, "slap_hit.msa" },
	{ CRaceMotionData::NAME_SLAP_HIT_WITH_ASSASSIN, "slap_hit.msa" },
	{ CRaceMotionData::NAME_SLAP_HIT_WITH_SURA, "slap_hit.msa" },
	{ CRaceMotionData::NAME_SLAP_HIT_WITH_SHAMAN, "slap_hit.msa" },
	{ CRaceMotionData::NAME_SLAP_HURT_WITH_WARRIOR, "slap_hurt.msa" },
	{ CRaceMotionData::NAME_SLAP_HURT_WITH_ASSASSIN, "slap_hurt.msa" },
	{ CRaceMotionData::NAME_SLAP_HURT_WITH_SURA, "slap_hurt.msa" },
	{ CRaceMotionData::NAME_SLAP_HURT_WITH_SHAMAN, "slap_hurt.msa" },
	{ CRaceMotionData::NAME_SELFIE, "selfie.msa" },
	{ CRaceMotionData::NAME_DANCE_7, "dance_7.msa" },
	{ CRaceMotionData::NAME_DOZE, "doze.msa" },
	{ CRaceMotionData::NAME_EXERCISE, "exercise.msa" },
	{ CRaceMotionData::NAME_PUSHUP, "pushup.msa" },
};

/**
* \brief Emoticons vector.
*/
static const std::vector<TEmoticon> m_vecEmoticons =
{
	{ CInstanceBase::EFFECT_EMOTICON + 0, "", string_path({ EMOTICON_PATH, "sweat.mse" }), "(È²´ç)" },
	{ CInstanceBase::EFFECT_EMOTICON + 1, "", string_path({ EMOTICON_PATH, "money.mse" }), "(µ·)" },
	{ CInstanceBase::EFFECT_EMOTICON + 2, "", string_path({ EMOTICON_PATH, "happy.mse" }), "(±â»Ý)" },
	{ CInstanceBase::EFFECT_EMOTICON + 3, "", string_path({ EMOTICON_PATH, "love_s.mse" }), "(ÁÁ¾Æ)" },
	{ CInstanceBase::EFFECT_EMOTICON + 4, "", string_path({ EMOTICON_PATH, "love_l.mse" }), "(»ç¶û)" },
	{ CInstanceBase::EFFECT_EMOTICON + 5, "", string_path({ EMOTICON_PATH, "angry.mse" }), "(ºÐ³ë)" },
	{ CInstanceBase::EFFECT_EMOTICON + 6, "", string_path({ EMOTICON_PATH, "aha.mse" }), "(¾ÆÇÏ)" },
	{ CInstanceBase::EFFECT_EMOTICON + 7, "", string_path({ EMOTICON_PATH, "gloom.mse" }), "(¿ì¿ï)" },
	{ CInstanceBase::EFFECT_EMOTICON + 8, "", string_path({ EMOTICON_PATH, "sorry.mse" }), "(ÁË¼Û)" },
	{ CInstanceBase::EFFECT_EMOTICON + 9, "", string_path({ EMOTICON_PATH, "!_mix_back.mse" }), "(!)" },
	{ CInstanceBase::EFFECT_EMOTICON + 10, "", string_path({ EMOTICON_PATH, "question.mse" }), "(?)" },
	{ CInstanceBase::EFFECT_EMOTICON + 11, "", string_path({ EMOTICON_PATH, "fish.mse" }), "(fish)" },
	{ CInstanceBase::EFFECT_EMOTICON + 12, "", string_path({ EMOTICON_PATH, "siren.mse" }), "(emoji1)" },
	{ CInstanceBase::EFFECT_EMOTICON + 13, "", string_path({ EMOTICON_PATH, "call.mse" }), "(emoji2)" },
	{ CInstanceBase::EFFECT_EMOTICON + 14, "", string_path({ EMOTICON_PATH, "charging.mse" }), "(emoji3)" },
	{ CInstanceBase::EFFECT_EMOTICON + 15, "", string_path({ EMOTICON_PATH, "celebration.mse" }), "(emoji4)" },
	{ CInstanceBase::EFFECT_EMOTICON + 16, "", string_path({ EMOTICON_PATH, "weather1.mse" }), "(emoji5)" },
	{ CInstanceBase::EFFECT_EMOTICON + 17, "", string_path({ EMOTICON_PATH, "alcohol.mse" }), "(emoji6)" },
	{ CInstanceBase::EFFECT_EMOTICON + 18, "", string_path({ EMOTICON_PATH, "hungry.mse" }), "(emoji7)" },
	{ CInstanceBase::EFFECT_EMOTICON + 19, "", string_path({ EMOTICON_PATH, "siren.mse" }), "(emoji8)" },
};

/**
* \brief Vector of race
*/
static const std::vector<TRaceInfo> m_vecRaceData =
{
	{ "data/shapes/warrior_m.msm", "d:/ymir work/pc/warrior/" },
	{ "data/shapes/assassin_w.msm", "d:/ymir work/pc/assassin/" },
	{ "data/shapes/sura_m.msm", "d:/ymir work/pc/sura/" },
	{ "data/shapes/shaman_w.msm", "d:/ymir work/pc/shaman/" },

	{ "data/shapes/warrior_w.msm", "d:/ymir work/pc2/warrior/" },
	{ "data/shapes/assassin_m.msm", "d:/ymir work/pc2/assassin/" },
	{ "data/shapes/sura_w.msm", "d:/ymir work/pc2/sura/" },
	{ "data/shapes/shaman_m.msm", "d:/ymir work/pc2/shaman/" },
};

/**
* \brief
*/
static const std::vector<TSoundFileName> m_vecUseSoundFileName =
{
	{ CPythonItem::USESOUND_DEFAULT, "sound/ui/drop.wav" },
	{ CPythonItem::USESOUND_ACCESSORY, "sound/ui/equip_ring_amulet.wav" },
	{ CPythonItem::USESOUND_ARMOR, "sound/ui/equip_metal_armor.wav" },
	{ CPythonItem::USESOUND_BOW, "sound/ui/equip_bow.wav" },
	{ CPythonItem::USESOUND_WEAPON, "sound/ui/equip_metal_weapon.wav" },
	{ CPythonItem::USESOUND_POTION, "sound/ui/eat_potion.wav" },
	{ CPythonItem::USESOUND_PORTAL, "sound/ui/potal_scroll.wav" },
};

/**
* \brief
*/
static const std::vector<TSoundFileName> m_vecDropSoundFileName =
{
	{ CPythonItem::DROPSOUND_DEFAULT, "sound/ui/drop.wav" },
	{ CPythonItem::DROPSOUND_ACCESSORY, "sound/ui/equip_ring_amulet.wav" },
	{ CPythonItem::DROPSOUND_ARMOR, "sound/ui/equip_metal_armor.wav" },
	{ CPythonItem::DROPSOUND_BOW, "sound/ui/equip_bow.wav" },
	{ CPythonItem::DROPSOUND_WEAPON, "sound/ui/equip_metal_weapon.wav" },
};

/**
* \brief
*/
static const std::vector<TFlyEffects> m_vecFlyEffects =
{
	{ CFlyingManager::FLY_EXP, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_yellow_small2.msf" },
	{ CFlyingManager::FLY_HP_MEDIUM, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_red_small.msf" },
	{ CFlyingManager::FLY_HP_BIG, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_red_big.msf" },
	{ CFlyingManager::FLY_SP_SMALL, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_blue_warrior_small.msf" },
	{ CFlyingManager::FLY_SP_MEDIUM, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_blue_small.msf" },
	{ CFlyingManager::FLY_SP_BIG, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_blue_big.msf" },
	{ CFlyingManager::FLY_FIREWORK1, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_1.msf" },
	{ CFlyingManager::FLY_FIREWORK2, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_2.msf" },
	{ CFlyingManager::FLY_FIREWORK3, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_3.msf" },
	{ CFlyingManager::FLY_FIREWORK4, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_4.msf" },
	{ CFlyingManager::FLY_FIREWORK5, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_5.msf" },
	{ CFlyingManager::FLY_FIREWORK6, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_6.msf" },
	{ CFlyingManager::FLY_FIREWORK_XMAS, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_xmas.msf" },
	{ CFlyingManager::FLY_CHAIN_LIGHTNING, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/pc/shaman/effect/pokroe.msf" },
	{ CFlyingManager::FLY_HP_SMALL, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_red_smallest.msf" },
	{ CFlyingManager::FLY_SKILL_MUYEONG, CFlyingManager::INDEX_FLY_TYPE_AUTO_FIRE, "d:/ymir work/pc/sura/effect/muyeong_fly.msf" },
};

/**
 * \brief 
 */
class CPythonPlayerSettingsModule : public CSingleton<CPythonPlayerSettingsModule>
{
public:
	CPythonPlayerSettingsModule();
	virtual ~CPythonPlayerSettingsModule();

	static auto Load() -> void;

protected:
	static auto __RegisterCacheMotionData(CRaceData* pRaceData, const uint16_t wMotionMode, const uint16_t wMotionIndex, const char* c_szFileName, const uint8_t byPercentage = 100) -> void;
	static auto __SetIntroMotions(CRaceData* pRaceData, const std::string& c_rstrFolderName) -> void;
	static auto __SetGeneralMotions(CRaceData* pRaceData, const uint16_t wMotionMode, const std::string& c_rstrFolderName) -> void;
	static auto __LoadFishing(CRaceData* pRaceData, const std::string& c_rstrFolderName) -> void;
	static auto __LoadCombo(CRaceData* pRaceData, const uint16_t wMotionMode) -> void;
	static auto __LoadWeaponMotion(CRaceData* pRaceData, WORD wMotionMode, const std::string & c_rstrFolderName) -> void;
	static auto __LoadHorse(CRaceData * pRaceData, const std::string & c_rstrFolderName) -> void;
	static auto __LoadSkill(CRaceData* pRaceData, const std::string& c_rstrFolderName) -> void;
	static auto __LoadGuildSkill(CRaceData* pRaceData, const std::string& c_rstrFolderName) -> void;
	static auto __RegisterSharedEmotionAnis(CRaceData* pRaceData, uint16_t wMotionMode,
	                                        const std::string& c_rstrFolderName) -> void;
	static auto __LoadEmotions(CRaceData* pRaceData, const std::string& c_rstrFolderName) -> void;
	static auto __LoadGameWarriorEx(CRaceData* pRaceData, const std::string& c_rstrFolderName) -> void;
	static auto __LoadGameAssassinEx(CRaceData* pRaceData, const std::string& c_rstrFolderName) -> void;
	static auto __LoadGameSuraEx(CRaceData* pRaceData, const std::string& c_rstrFolderName) -> void;
	static auto __LoadGameShamanEx(CRaceData* pRaceData, const std::string& c_rstrFolderName) -> void;
	static auto __LoadGameRace(CRaceData * pRaceData, const std::string & c_rstrFolderName) -> void;
	static auto __LoadGameNPC() -> void;

};
#endif
