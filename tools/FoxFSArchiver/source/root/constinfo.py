import app
import chrmgr
import player
import net

import localeinfo

USE_AUTO_LAST_CHAT_SHOUT = False
AUTO_LAST_CHAT_SHOUT = None
LAST_CHAT_SHOUT = None

if app.USE_AUTO_HUNT and not app.USE_PICKUP_FILTER_ICO_AS_AFFECT:
	filterIcoX = 0
	filterIcoY = 0

if app.USE_WHEEL_OF_FORTUNE:
	dragon_coins=0

if app.ENABLE_OFFLINESHOP_REWORK:
	shopSearchTimeWait=5
	shopSearchWaitTime=0

inventoryPageIndex=0
isOpenedCostumeWindowWhenClosingEquipment=0

if app.USE_ENVIRONMENT_OPTIONS:
	ENVIRONMENT_NIGHT="d:/ymir work/environment/moonlight04.msenv"

if app.USE_AUTO_REFINE:
	AUTO_REFINE = False
	IS_AUTO_REFINE = False
	AUTO_REFINE_TYPE = 0
	AUTO_REFINE_DATA = {
		"ITEM":[-1, -1],
		"NPC":[0, -1, -1, 0]
	}

if app.ENABLE_LETTERS_EVENT:
	rewardLettersList = []

if app.ENABLE_MESSENGER_BLOCK:
	ME_KEY = 0

if app.LINK_IN_CHAT:
	def GetLink(text):
		start = text.find("http://")
		if start == -1:
			start = text.find("https://")
			if start == -1:
				return ""
		
		link = text[start:len(text)]
		splitted = link.split(" ")
		return splitted[0]

INTROLOADING = 1
INTROLOADING1 = 0

wndExpandedMenu = 1

if app.ENABLE_CAPITALE_SYSTEM:
	wndExpandedMoneyTaskbar = 0

if app.ENABLE_BIOLOGIST_UI:
	haveBiologist = 0
	remainBiologistTime = 0
	notifiedBiologist = []

dogmode = 0
restart = 0
equipview = 0
exp_status = 0

if app.ENABLE_DUNGEON_INFO_SYSTEM:
	dungeonInfo = []
	dungeonRanking = {
		"ranking_type" : 0,
		"ranking_list" : [],
		"my_ranking" : [],
	}
	dungeonData = {
		"quest_index" : 0,
		"quest_cmd" : "",
	}

if app.NEW_PET_SYSTEM:
	PETGUI = 0
	PETMINILEVEL = 0
	PETMINIEVO = 0

if app.ENABLE_DUNGEON_INFO_SYSTEM:
	def MapNameByIndex(idx):
		MAP_INDEX_DICT = {
			0 : localeinfo.MAP_NAME_0,
			27 : localeinfo.MAP_NAME_27,
			66 : localeinfo.MAP_NAME_66,
			208 : localeinfo.MAP_NAME_208,
			209 : localeinfo.MAP_NAME_209,
			210 : localeinfo.MAP_NAME_210,
			212 : localeinfo.MAP_NAME_212,
			216 : localeinfo.MAP_NAME_216,
			217 : localeinfo.MAP_NAME_217,
			218 : localeinfo.MAP_NAME_218,
			351 : localeinfo.MAP_NAME_351,
			352 : localeinfo.MAP_NAME_352,
			353 : localeinfo.MAP_NAME_353,
			355 : localeinfo.MAP_NAME_355,
			357 : localeinfo.MAP_NAME_357,
			26 : localeinfo.MAP_NAME_26,
			25 : localeinfo.MAP_NAME_25,
			136 : localeinfo.MAP_NAME_136,
			156 : localeinfo.MAP_NAME_156,
			157 : localeinfo.MAP_NAME_157
		}
		
		if app.ENABLE_HALLOWEEN_EVENT_2022:
			MAP_INDEX_DICT[231] = localeinfo.MAP_NAME_231
			MAP_INDEX_DICT[232] = localeinfo.MAP_NAME_232
			MAP_INDEX_DICT[233] = localeinfo.MAP_NAME_233
		
		if idx in MAP_INDEX_DICT:
			return MAP_INDEX_DICT[idx]
		else:
			return MAP_INDEX_DICT[0]

INPUT_IGNORE = 0

#RENDER_TARGET
DISABLE_MODEL_PREVIEW = 0
#RENDER_TARGET

ACCOUNT_NAME = "NoName"
WOLF_MAN = "DISABLED"
WOLF_WOMEN = "DISABLED"

# EXTRA BEGIN
# don't set a random channel when you open the client
ENABLE_RANDOM_CHANNEL_SEL = 0
# don't remove id&pass if the login attempt fails
ENABLE_CLEAN_DATA_IF_FAIL_LOGIN = 0
# ctrl+v will now work
ENABLE_PASTE_FEATURE = 0
# display all the bonuses added by a stone instead of the first one
ENABLE_FULLSTONE_DETAILS = 1
# enable successfulness % in the refine dialog
ENABLE_REFINE_PCT = 1
# extra ui features
EXTRA_UI_FEATURE = 1
#
NEW_678TH_SKILL_ENABLE = 1
# EXTRA END

# option
IN_GAME_WIKI_HELP_ENABLE = 1
CONSOLE_ENABLE = 0

ENERGY = 0
ENERGY_END_TIME = 0
ENERGY_TYPE = 0
ENABLE_ENERGY_SYSTEM = 1

PVPMODE_ENABLE = 1
PVPMODE_ACCELKEY_DELAY = 0.5
PVPMODE_PROTECTED_LEVEL = 30

FOG_LEVEL0 = 4800.0
FOG_LEVEL1 = 9600.0
FOG_LEVEL2 = 12800.0
FOG_LEVEL = FOG_LEVEL0
FOG_LEVEL_LIST=[FOG_LEVEL0, FOG_LEVEL1, FOG_LEVEL2]

if app.ENABLE_HIDE_COSTUME_SYSTEM:
	hiddenStatusDict = {
						app.HIDE_PART_BODY : 0,
						app.HIDE_PART_HAIR : 0,
	}
	
	if app.ENABLE_ACCE_SYSTEM:
		hiddenStatusDict[app.HIDE_PART_SASH] = 0
	
	if app.ENABLE_WEAPON_COSTUME_SYSTEM:
		hiddenStatusDict[app.HIDE_PART_WEAPON] = 0
	
	if app.ENABLE_AURA_SYSTEM:
		hiddenStatusDict[app.HIDE_PART_AURA] = 0
	
	if app.ENABLE_PETSKIN:
		hiddenStatusDict[app.HIDE_PETSKIN] = 0
	
	if app.ENABLE_MOUNTSKIN:
		hiddenStatusDict[app.HIDE_MOUNTSKIN] = 0
	
	if app.ENABLE_ACCE_SYSTEM:
		hiddenStatusDict[app.HIDE_STOLE_PART] = 0

CAMERA_MAX_DISTANCE_SHORT = 2500.0
CAMERA_MAX_DISTANCE_LONG = 3500.0
CAMERA_MAX_DISTANCE_LONG_LONG = 4500.0
CAMERA_MAX_DISTANCE_LIST=[CAMERA_MAX_DISTANCE_SHORT, CAMERA_MAX_DISTANCE_LONG, CAMERA_MAX_DISTANCE_LONG_LONG]
CAMERA_MAX_DISTANCE = CAMERA_MAX_DISTANCE_SHORT

CHRNAME_COLOR_INDEX = 0

if app.ENABLE_PVP_ADVANCED:
	DUEL_IS_SHOW_EQUIP=0
	DUEL_SAVE_VID=0

if app.NEW_PET_SYSTEM:
	# NEW_PET_SYSTEM
	FEEDWIND = 0
	SKILL_PET4 = 0
	SKILL_PET3 = 0
	SKILL_PET2 = 0
	SKILL_PET1 = 0
	LASTAFFECT_POINT = 0
	LASTAFFECT_VALUE = 0
	EVOLUTION = 0
	#END NEW_PET_SYSTEM

# constant
HIGH_PRICE = 500000
MIDDLE_PRICE = 50000
ERROR_METIN_STONE = 28960
SUB2_LOADING_ENABLE = 1
EXPANDED_COMBO_ENABLE = 1
#CONVERT_EMPIRE_LANGUAGE_ENABLE = 1
USE_ITEM_WEAPON_TABLE_ATTACK_BONUS = 0
ADD_DEF_BONUS_ENABLE = 1
LOGIN_COUNT_LIMIT_ENABLE = 0

USE_SKILL_EFFECT_UPGRADE_ENABLE = 1

VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD = 1
GUILD_MONEY_PER_GSP = 100
GUILD_WAR_TYPE_SELECT_ENABLE = 1
TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE = 0

HAIR_COLOR_ENABLE = 1
ARMOR_SPECULAR_ENABLE = 1
WEAPON_SPECULAR_ENABLE = 1
SEQUENCE_PACKET_ENABLE = 0
KEEP_ACCOUNT_CONNETION_ENABLE = 1
CONVERT_EMPIRE_LANGUAGE_ENABLE = 0
USE_ITEM_WEAPON_TABLE_ATTACK_BONUS = 0
ADD_DEF_BONUS_ENABLE = 0
LOGIN_COUNT_LIMIT_ENABLE = 0
PVPMODE_PROTECTED_LEVEL = 15
TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE = 10

isItemQuestionDialog = 0
##ENABLE_MULTI_LANGUAGE
def GET_ITEM_QUESTION_DIALOG_STATUS():
	global isItemQuestionDialog
	return isItemQuestionDialog

def SET_ITEM_QUESTION_DIALOG_STATUS(flag):
	global isItemQuestionDialog
	isItemQuestionDialog = flag



########################

def SET_DEFAULT_FOG_LEVEL():
	global FOG_LEVEL
	app.SetMinFog(FOG_LEVEL)

def SET_FOG_LEVEL_INDEX(index):
	global FOG_LEVEL
	global FOG_LEVEL_LIST
	try:
		FOG_LEVEL=FOG_LEVEL_LIST[index]
	except IndexError:
		FOG_LEVEL=FOG_LEVEL_LIST[0]
	app.SetMinFog(FOG_LEVEL)

def GET_FOG_LEVEL_INDEX():
	global FOG_LEVEL
	global FOG_LEVEL_LIST
	return FOG_LEVEL_LIST.index(FOG_LEVEL)

########################

def SET_DEFAULT_CAMERA_MAX_DISTANCE():
	global CAMERA_MAX_DISTANCE
	app.SetCameraMaxDistance(CAMERA_MAX_DISTANCE)

def SET_CAMERA_MAX_DISTANCE_INDEX(index):
	global CAMERA_MAX_DISTANCE
	global CAMERA_MAX_DISTANCE_LIST
	try:
		CAMERA_MAX_DISTANCE=CAMERA_MAX_DISTANCE_LIST[index]
	except:
		CAMERA_MAX_DISTANCE=CAMERA_MAX_DISTANCE_LIST[0]

	app.SetCameraMaxDistance(CAMERA_MAX_DISTANCE)

def GET_CAMERA_MAX_DISTANCE_INDEX():
	global CAMERA_MAX_DISTANCE
	global CAMERA_MAX_DISTANCE_LIST
	return CAMERA_MAX_DISTANCE_LIST.index(CAMERA_MAX_DISTANCE)

########################

def SET_DEFAULT_CHRNAME_COLOR():
	global CHRNAME_COLOR_INDEX
	chrmgr.SetEmpireNameMode(CHRNAME_COLOR_INDEX)

def SET_CHRNAME_COLOR_INDEX(index):
	global CHRNAME_COLOR_INDEX
	CHRNAME_COLOR_INDEX=index
	chrmgr.SetEmpireNameMode(index)

def GET_CHRNAME_COLOR_INDEX():
	global CHRNAME_COLOR_INDEX
	return CHRNAME_COLOR_INDEX

def SET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD(index):
	global VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD
	VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD = index

def GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD():
	global VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD
	return VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD

#def SET_DEFAULT_CONVERT_EMPIRE_LANGUAGE_ENABLE():
#	global CONVERT_EMPIRE_LANGUAGE_ENABLE
#	net.SetEmpireLanguageMode(CONVERT_EMPIRE_LANGUAGE_ENABLE)

def SET_DEFAULT_USE_ITEM_WEAPON_TABLE_ATTACK_BONUS():
	global USE_ITEM_WEAPON_TABLE_ATTACK_BONUS
	player.SetWeaponAttackBonusFlag(USE_ITEM_WEAPON_TABLE_ATTACK_BONUS)

def SET_DEFAULT_USE_SKILL_EFFECT_ENABLE():
	global USE_SKILL_EFFECT_UPGRADE_ENABLE
	app.SetSkillEffectUpgradeEnable(USE_SKILL_EFFECT_UPGRADE_ENABLE)

def SET_TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE():
	global TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE
	app.SetTwoHandedWeaponAttSpeedDecreaseValue(TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE)

########################
import item

ACCESSORY_MATERIAL_LIST = [50623, 50624, 50625, 50626, 50627, 50628, 50629, 50630, 50631, 50632, 50633, 50634, 50635, 50636, 50637, 50638, 50639]
JewelAccessoryInfos = [
		# jewel		wrist	neck	ear
		[ 50634,	14420,	16220,	17220 ],
		[ 50635,	14500,	16500,	17500 ],
		[ 50636,	14520,	16520,	17520 ],
		[ 50637,	14540,	16540,	17540 ],
		[ 50638,	14560,	16560,	17560 ],
		[ 50639,	14570,	16570,	17570 ],
		[ 50640,	14240,	16240,	17240 ],
	]
def GET_ACCESSORY_MATERIAL_VNUM(vnum, subType):
	if (vnum >= 14220 and vnum <= 14234) or\
		(vnum >= 16220 and vnum <= 16234) or\
		(vnum >= 17220 and vnum <= 17234):
		return 50634

	if (vnum >= 14580 and vnum <= 14589) or\
		(vnum >= 14240 and vnum <= 14245) or\
		(vnum >= 15010 and vnum <= 15013) or\
		(vnum >= 16580 and vnum <= 16593) or\
		(vnum >= 16240 and vnum <= 16245) or\
		(vnum >= 17240 and vnum <= 17245) or\
		(vnum >= 17570 and vnum <= 17583):
		return 50690

	ret = vnum
	item_base = (vnum / 10) * 10
	for info in JewelAccessoryInfos:
		if item.ARMOR_WRIST == subType:
			if info[1] == item_base:
				return info[0]
		elif item.ARMOR_NECK == subType:
			if info[2] == item_base:
				return info[0]
		elif item.ARMOR_EAR == subType:
			if info[3] == item_base:
				return info[0]

	if vnum >= 16210 and vnum <= 16219:
		return 50625

	if item.ARMOR_WRIST == subType:
		WRIST_ITEM_VNUM_BASE = 14000
		ret -= WRIST_ITEM_VNUM_BASE
	elif item.ARMOR_NECK == subType:
		NECK_ITEM_VNUM_BASE = 16000
		ret -= NECK_ITEM_VNUM_BASE
	elif item.ARMOR_EAR == subType:
		EAR_ITEM_VNUM_BASE = 17000
		ret -= EAR_ITEM_VNUM_BASE

	type = ret/20

	if type<0 or type>=len(ACCESSORY_MATERIAL_LIST):
		type = (ret-170) / 20
		if type<0 or type>=len(ACCESSORY_MATERIAL_LIST):
			return 0

	return ACCESSORY_MATERIAL_LIST[type]

def GET_BELT_MATERIAL_VNUM(vnum, subType = 0):
	return 18900

def IS_AUTO_POTION(itemVnum):
	return IS_AUTO_POTION_HP(itemVnum) or IS_AUTO_POTION_SP(itemVnum)

def IS_AUTO_POTION_HP(itemVnum):
	if itemVnum == 72725 or itemVnum == 72726:
		return 1
	elif itemVnum >= 61020 and itemVnum <= 61026:
		return 1		
	
	return 0

def IS_AUTO_POTION_SP(itemVnum):
	if itemVnum == 72729 or itemVnum == 72730:
		return 1
	
	return 0

def IS_INFINITE_AUTO_POTION(itemVnum):
	if itemVnum == 72726 or itemVnum == 72730:
		return 1
	
	return 0

if app.NEW_PET_SYSTEM:
	def IS_PET_SEAL(itemVnum):
		if itemVnum >= 55701 and itemVnum <= 55711:
			return 1
		
		return 0

def IS_PET_SEAL_OLD(itemVnum):
	if itemVnum == 38200 or itemVnum == 38201:
		return 1
	elif itemVnum == 84032 or itemVnum == 84052 or itemVnum == 84072 or itemVnum == 84092 or itemVnum == 84112:
		return 1
	elif itemVnum == 84013 or itemVnum == 84133 or itemVnum == 84132:
		return 1
	elif itemVnum >= 53006 and itemVnum <= 53305:
		return 1
	elif itemVnum == 48301 or itemVnum == 48311 or itemVnum == 48321:
		return 1
	elif itemVnum == 49010 or itemVnum == 49050:
		return 1
	elif itemVnum >= 60101 and itemVnum <= 60104:
		return 1
	
	return 0

if app.ENABLE_MULTI_LANGUAGE:
	availLangsDict = {\
		"en":1,
		"ro":2,
		"it":3,
		"de":5,
	}

	def TRANSFORM_LANG(lang):
		langDict = { 
			1 : "en", 
			2 : "ro",
			3 : "it", 
			4 : "tr",
			5 : "de",
			6 : "pl",
			7 : "pt",
			8 : "es",
			9 : "cz",
			10 : "hu",
		}
		for key, value in langDict.iteritems():
			if key == lang:
				return value
			elif value == lang:
				return key
				
		return "None"

def replace_line(file_name, line_num, text):
	lines = open(file_name, 'r').readlines()
	lines[line_num] = text
	out = open(file_name, 'w')
	out.writelines(lines)
	out.close()

if app.ENABLE_INFINITE_RAFINES:
	ACCESSORY_MATERIAL_LIST2 = [50673, 50674, 50675, 50676, 50677, 50678, 50679, 50680, 50681, 50682, 50683, 50684, 50685, 50686, 50687, 50688, 50689]
	JewelAccessoryInfos2 = [
			# jewel		wrist	neck	ear
			[ 50684,	14420,	16220,	17220 ],
			[ 50685,	14500,	16500,	17500 ],
			[ 50686,	14520,	16520,	17520 ],
			[ 50687,	14540,	16540,	17540 ],
			[ 50688,	14560,	16560,	17560 ],
			[ 50689,	14570,	16570,	17570 ],
			[ 50690,	14240,	16240,	17240 ],
		]
	def GET_ACCESSORY_MATERIAL_VNUM2(vnum, subType):
		if (vnum >= 14220 and vnum <= 14233) or\
			(vnum >= 16220 and vnum <= 16233) or\
			(vnum >= 17220 and vnum <= 17233):
			return 50684

		if (vnum >= 14580 and vnum <= 14589) or\
			(vnum >= 14240 and vnum <= 14245) or\
			(vnum >= 15010 and vnum <= 15013) or\
			(vnum >= 16580 and vnum <= 16593) or\
			(vnum >= 16240 and vnum <= 16245) or\
			(vnum >= 17240 and vnum <= 17245) or\
			(vnum >= 17570 and vnum <= 17583):
			return 50690

		ret = vnum
		item_base = (vnum / 10) * 10
		for info in JewelAccessoryInfos2:
			if item.ARMOR_WRIST == subType:
				if info[1] == item_base:
					return info[0]
			elif item.ARMOR_NECK == subType:
				if info[2] == item_base:
					return info[0]
			elif item.ARMOR_EAR == subType:
				if info[3] == item_base:
					return info[0]

		if vnum >= 16210 and vnum <= 16219:
			return 50675

		if item.ARMOR_WRIST == subType:
			WRIST_ITEM_VNUM_BASE = 14000
			ret -= WRIST_ITEM_VNUM_BASE
		elif item.ARMOR_NECK == subType:
			NECK_ITEM_VNUM_BASE = 16000
			ret -= NECK_ITEM_VNUM_BASE
		elif item.ARMOR_EAR == subType:
			EAR_ITEM_VNUM_BASE = 17000
			ret -= EAR_ITEM_VNUM_BASE

		type = ret/20

		if type<0 or type>=len(ACCESSORY_MATERIAL_LIST2):
			type = (ret-170) / 20
			if type<0 or type>=len(ACCESSORY_MATERIAL_LIST2):
				return 0

		return ACCESSORY_MATERIAL_LIST2[type]

def SetGameInst(inst):
	from _weakref import proxy
	
	global gameInst
	gameInst = proxy(inst)

def ClearGameInst():
	global gameInst
	gameInst = None

def GetGameInst():
	return gameInst

def SetInterfaceInst(inst):
	from _weakref import proxy
	
	global interfaceInst
	interfaceInst = proxy(inst)

def ClearInterfaceInst():
	global interfaceInst
	interfaceInst = None

def GetInterfaceInst():
	return interfaceInst
