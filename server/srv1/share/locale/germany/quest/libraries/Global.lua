NOWINDOW = 0;
NORMAL = 1;
CINEMATIC = 2;
SCROLL = 3;

CONFIRM_NO = 0;
CONFIRM_YES = 1;
CONFIRM_OK = 1;
CONFIRM_TIMEOUT = 2;

PART_MAIN = 0;
PART_HAIR = 3;

MASTER_SKILL_LEVEL = 20;
GRAND_MASTER_SKILL_LEVEL = 30;
PERFECT_MASTER_SKILL_LEVEL = 40;

MALE = 0;
FEMALE = 1;

NO_SKILL_GROUP = 0;

BODY = 1;
MENTAL = 2;

BLADE_FIGHT = 1;
ARCHERY = 2;

WEAPONARY = 1;
BLACK_MAGIC = 2;

DRAGON_FORCE = 1;
HEALING_FORCE = 2;

--** Praticamente l'output della funzione pc.get_job().
WARRIOR = 0;
ASSASSIN = 1;
NINJA = ASSASSIN;
SURA = 2;
SHAMAN = 3;

--** Praticamente l'output della funzione pc.get_race().
WARRIOR_M	= 0
NINJA_W		= 1
SURA_M		= 2
SHAMAN_W	= 3
WARRIOR_W	= 4
NINJA_M		= 5
SURA_W		= 6
SHAMAN_M	= 7

--[[
	Ritorna:
		La lista di tutte le skills esistenti,
		divisa per razza e dottrina.
]]
ACTIVE_SKILL_LIST = {
	--** Warrior
	[WARRIOR] = {
		-- Body
		[BODY] = {1, 2, 3, 4, 5, 6},
		-- Mental
		[MENTAL] = {16, 17, 18, 19, 20, 21}
	},

	--** Ninja
	[NINJA] = {
		-- Blade Fight
		[BLADE_FIGHT] = {31, 32, 33, 34, 35, 36},
		-- Archery
		[ARCHERY] = {46, 47, 48, 49, 50, 51}
	},

	--** Sura
	[SURA] = {
		-- Weaponary
		[WEAPONARY] = {61, 62, 63, 64, 65, 66},
		-- Black Magic
		[BLACK_MAGIC] = {76, 77, 78, 79, 80, 81}
	},

	--** Shaman
	[SHAMAN] = {
		-- Dragon Power
		[DRAGON_FORCE] = {91, 92, 93, 94, 95, 96},
		-- Healing Power
		[HEALING_FORCE] = {106, 107, 108, 109, 110, 111}
	}
};

--[[
	Ritorna:
		Il nome della razza in base al sesso.
]]
RACE_NAME_LIST = {
	[WARRIOR] = {
		[MALE] = "Warrior",
		[FEMALE] = "Warrior"
	},
	[NINJA] = {
		[MALE] = "Ninja",
		[FEMALE] = "Ninja"
	},
	[SURA] = {
		[MALE] = "Sura",
		[FEMALE] = "Sura"
	},
	[SHAMAN] = {
		[MALE] = "Shaman",
		[FEMALE] = "Shaman"
	},
};