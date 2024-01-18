import app
import chr
import chrmgr
import player

JOB_WARRIOR, JOB_ASSASSIN, JOB_SURA, JOB_SHAMAN = range(4)

RACE_WARRIOR_M, RACE_ASSASSIN_W, RACE_SURA_M, RACE_SHAMAN_W, RACE_WARRIOR_W, RACE_ASSASSIN_M, RACE_SURA_W, RACE_SHAMAN_M = range(8)

FACE_IMAGE_DICT = {
	RACE_WARRIOR_M : "d:/ymir work/ui/game/windows/face_warrior.sub",
	RACE_ASSASSIN_W : "d:/ymir work/ui/game/windows/face_assassin.sub",
	RACE_SURA_M : "d:/ymir work/ui/game/windows/face_sura.sub",
	RACE_SHAMAN_W : "d:/ymir work/ui/game/windows/face_shaman.sub",
}

PASSIVE_GUILD_SKILL_INDEX_LIST = ( 151, )
ACTIVE_GUILD_SKILL_INDEX_LIST = ( 152, 153, 154, 155, 156, 157, )

SKILL_INDEX_DICT = []

def DefineSkillIndexDict():
	global SKILL_INDEX_DICT
	
	SKILL_INDEX_DICT = {
		JOB_WARRIOR : {
			1 : (1, 2, 3, 4, 5, 6, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0,),
			2 : (16, 17, 18, 19, 20, 21, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0,),
			"SUPPORT" : (122, 123, 121, 124, 125, 0, 0, 0, 130, 131, 0, 0,),
		},
		JOB_ASSASSIN : {
			1 : (31, 32, 33, 34, 35, 36, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 140,),
			2 : (46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 140,),
			"SUPPORT" : (122, 123, 121, 124, 125, 0, 0, 0, 130, 131, 0, 0,),
		},
		JOB_SURA : {
			1 : (61, 62, 63, 64, 65, 66, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0,),
			2 : (76, 77, 78, 79, 80, 81, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0,),
			"SUPPORT" : (122, 123, 121, 124, 125, 0, 0, 0, 130, 131, 0, 0,),
		},
		JOB_SHAMAN : {
			1 : (91, 92, 93, 94, 95, 96, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0,),
			2 : (106, 107, 108, 109, 110, 111, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0,),
			"SUPPORT" : (122, 123, 121, 124, 125, 0, 0, 0, 130, 131, 0, 0,),
		},
	}

def RegisterSkill(race, group, empire = 0):
	DefineSkillIndexDict()
	
	job = chr.RaceToJob(race)
	
	if SKILL_INDEX_DICT.has_key(job):
		if SKILL_INDEX_DICT[job].has_key(group):
			activeSkillList = SKILL_INDEX_DICT[job][group]
			for i in xrange(len(activeSkillList)):
				skillIndex = activeSkillList[i]

				player.SetSkill(i+1, skillIndex)

				supportSkillList = SKILL_INDEX_DICT[job]["SUPPORT"]
			
			for i in xrange(len(supportSkillList)):
				player.SetSkill(i+100+1, supportSkillList[i])
	
	if 0 != empire:
		languageSkillList = []
		for i in xrange(3):
			if (i+1) != empire:
				languageSkillList.append(player.SKILL_INDEX_LANGUAGE1+i)
		
		for i in xrange(len(languageSkillList)):
			player.SetSkill(107 + i, languageSkillList[i])
	
	for i in xrange(len(PASSIVE_GUILD_SKILL_INDEX_LIST)):
		player.SetSkill(200 + i, PASSIVE_GUILD_SKILL_INDEX_LIST[i])
	
	for i in xrange(len(ACTIVE_GUILD_SKILL_INDEX_LIST)):
		player.SetSkill(210 + i, ACTIVE_GUILD_SKILL_INDEX_LIST[i])

def LoadGuildBuildingList(filename):
	import uiguild
	uiguild.BUILDING_DATA_LIST = []
	
	handle = app.OpenTextFile(filename)
	count = app.GetTextFileLineCount(handle)
	for i in xrange(count):
		line = app.GetTextFileLine(handle, i)
		tokens = line.split("\t")
		TOKEN_VNUM = 0
		TOKEN_TYPE = 1
		TOKEN_NAME = 2
		TOKEN_LOCAL_NAME = 3
		NO_USE_TOKEN_SIZE_1 = 4
		NO_USE_TOKEN_SIZE_2 = 5
		NO_USE_TOKEN_SIZE_3 = 6
		NO_USE_TOKEN_SIZE_4 = 7
		TOKEN_X_ROT_LIMIT = 8
		TOKEN_Y_ROT_LIMIT = 9
		TOKEN_Z_ROT_LIMIT = 10
		TOKEN_PRICE = 11
		TOKEN_MATERIAL = 12
		TOKEN_NPC = 13
		TOKEN_GROUP = 14
		TOKEN_DEPEND_GROUP = 15
		TOKEN_ENABLE_FLAG = 16
		LIMIT_TOKEN_COUNT = 17
		if not tokens[TOKEN_VNUM].isdigit():
			continue
		
		if len(tokens) < LIMIT_TOKEN_COUNT:
			import dbg
			dbg.TraceError("Strange token count [%d/%d] [%s]" % (len(tokens), LIMIT_TOKEN_COUNT, line))
			continue
		
		ENABLE_FLAG_TYPE_NOT_USE = False
		ENABLE_FLAG_TYPE_USE = True
		ENABLE_FLAG_TYPE_USE_BUT_HIDE = 2
		
		if ENABLE_FLAG_TYPE_NOT_USE == int(tokens[TOKEN_ENABLE_FLAG]):
			continue
		
		vnum = int(tokens[TOKEN_VNUM])
		type = tokens[TOKEN_TYPE]
		name = tokens[TOKEN_NAME]
		localName = tokens[TOKEN_LOCAL_NAME]
		xRotLimit = int(tokens[TOKEN_X_ROT_LIMIT])
		yRotLimit = int(tokens[TOKEN_Y_ROT_LIMIT])
		zRotLimit = int(tokens[TOKEN_Z_ROT_LIMIT])
		price = tokens[TOKEN_PRICE]
		material = tokens[TOKEN_MATERIAL]
		
		folderName = ""
		if "HEADQUARTER" == type:
			folderName = "headquarter"
		elif "FACILITY" == type:
			folderName = "facility"
		elif "OBJECT" == type:
			folderName = "object"
		elif "WALL" == type:
			folderName = "fence"
		
		materialList = ["0", "0", "0"]
		if material:
			if material[0] == "\"":
				material = material[1:]
			
			if material[-1] == "\"":
				material = material[:-1]
			
			for one in material.split("/"):
				data = one.split(",")
				if 2 != len(data):
					continue
					
				itemID = int(data[0])
				count = data[1]
				
				if itemID == uiguild.MATERIAL_STONE_ID:
					materialList[uiguild.MATERIAL_STONE_INDEX] = count
				elif itemID == uiguild.MATERIAL_LOG_ID:
					materialList[uiguild.MATERIAL_LOG_INDEX] = count
				elif itemID == uiguild.MATERIAL_PLYWOOD_ID:
					materialList[uiguild.MATERIAL_PLYWOOD_INDEX] = count
		
		chrmgr.RegisterRaceSrcName(name, folderName)
		chrmgr.RegisterRaceName(vnum, name)
		appendingData = { "VNUM":vnum,
						  "TYPE":type,
						  "NAME":name,
						  "LOCAL_NAME":localName,
						  "X_ROT_LIMIT":xRotLimit,
						  "Y_ROT_LIMIT":yRotLimit,
						  "Z_ROT_LIMIT":zRotLimit,
						  "PRICE":price,
						  "MATERIAL":materialList,
						  "SHOW" : True }
		
		if ENABLE_FLAG_TYPE_USE_BUT_HIDE == int(tokens[TOKEN_ENABLE_FLAG]):
			appendingData["SHOW"] = False
		
		uiguild.BUILDING_DATA_LIST.append(appendingData)
	
	app.CloseTextFile(handle)

if app.USE_RACE_HEIGHT:
	def LoadRaceHeight():
		try:
			lines = open("data/monsters/race_height.txt", "r").readlines()
		except IOError:
			return
		
		for line in lines:
			tokens = line[:-1].split("\t")
			if len(tokens) == 0 or not tokens[0]:
				continue
			
			vnum = int(tokens[0])
			height = float(tokens[1])
			riding = float(tokens[2])
			scale = float(tokens[3])
			
			chrmgr.SetRaceHeight(vnum, height, riding, scale)
