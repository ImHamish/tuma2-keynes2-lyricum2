--[[
 Dungeon Information System Library, Version 4.4
 Copyright 2019 Owsap Productions
]]

--[[ TODO:
 Change index with map_index for better organization and consultation
 Improve query updates
 Use client-side map names by map index
 Add SQL queries to questlua_dungeon
]]

dungeonlib = {}
dungeonInfo = {}
dungeonInfo.table = {
	{ -- dungeonorchi
		["status"] = 0,									-- Dungeon status [ 0 (Open), 1 (Running), 2 (Closed) ]
		["type"] = 1,									-- Dungeon type [ 0 (Unknown), 1 (Private), 2 (Global) ]
		["organization"] = 0,							-- Dungeon organization [ 0 (None), 1 (Party), 2 (Guild) ]
		["level_limit"] = {30, 60},						-- Dungeon level limit [ min_level, max_level ] ({0, 0} = {1, 120} : no limit)
		["party_member_limit"] = {0, 0},				-- Dungeon party members [ min_members, max_members ] ({0, 0} = {1, 8} : no limit)
		["map_index"] = 355,							-- Dungeon map index * important
		["map_coords"] = {2560 + 764, 6656 + 723},		-- Dungeon map coordinates [ x, y ]
		["cooldown"] = 1800,							-- [ 0 (None) ] | Ex: 60 * 60 * 3 = 3 hours
		["duration"] = 1800,							-- [ 0 (None) ] | Ex: 60 * 60 * 3 = 3 hours
		["entrance_map_index"] = 64,					-- Entrance map index
		["strength_bonus"] = 19,						-- Strength bonus id against dungeon monsters //forte vs orchi
		["resistance_bonus"] = 88,						-- Resistance bonus id against dungeon monsters //res vs elemento terra
		["required_item"] = {89106, 1},					-- Required dungeon item [ vnum, count]
	},
	-- { -- slime_dungeon
		-- ["status"] = 0,
		-- ["type"] = 1,
		-- ["organization"] = 0,
		-- ["level_limit"] = {40, 65},
		-- ["party_member_limit"] = {0, 0},
		-- ["map_index"] = 27,
		-- ["map_coords"] = {2560 + 283, 6656 + 1441},
		-- ["cooldown"] = 2100,
		-- ["duration"] = 1800,
		-- ["entrance_map_index"] = 64,
		-- ["strength_bonus"] = 18,
		-- ["resistance_bonus"] = 38,
		-- ["required_item"] = {30625, 1},
	-- },
	{ -- dungeon_spider
		["status"] = 0,
		["type"] = 1,
		["organization"] = 0,
		["level_limit"] = {50, 75},
		["party_member_limit"] = {0, 0},
		["map_index"] = 217,
		["map_coords"] = {512 + 177, 5632 + 478},
		["cooldown"] = 2400,
		["duration"] = 1800,
		["entrance_map_index"] = 217,
		["strength_bonus"] = 18,
		["resistance_bonus"] = 35,
		["required_item"] = {30325, 1},
	},
	{ -- deviltower_zone
		["status"] = 0,
		["type"] = 1,
		["organization"] = 0,
		["level_limit"] = { 40, 110 },
		["party_member_limit"] = { 0, 0 },
		["map_index"] = 66,
		["map_coords"] = {5376 + 527, 512 + 597},
		["cooldown"] = 2700,
		["duration"] = 3000,
		["entrance_map_index"] = 65, 
		["strength_bonus"] = 21,
		["resistance_bonus"] = 89,
		["required_item"] = {0, 0},
	},
	-- { -- rune_zone
		-- ["status"] = 2,
		-- ["type"] = 1,
		-- ["organization"] = 0,
		-- ["level_limit"] = {70, 120},
		-- ["party_member_limit"] = {0, 0},
		-- ["map_index"] = 218,
		-- ["map_coords"] = {5334, 13341},
		-- ["cooldown"] = 3600,
		-- ["duration"] = 3900,
		-- ["entrance_map_index"] = 215,
		-- ["strength_bonus"] = 121,
		-- ["resistance_bonus"] = 124,
		-- ["required_item"] = {89101, 1},
	-- },
	{ -- catacombe
		["status"] = 0,
		["type"] = 1,
		["organization"] = 0,
		["level_limit"] = {75, 105},
		["party_member_limit"] = {0, 0},
		["map_index"] = 216,
		["map_coords"] = {5376 + 537, 512 + 484},
		["cooldown"] = 3000,
		["duration"] = 2400,
		["entrance_map_index"] = 65,
		["strength_bonus"] = 21,
		["resistance_bonus"] = 89,
		["required_item"] = {30320, 1},
	},
	{ -- dragonlair_zone
		["status"] = 0,
		["type"] = 1,
		["organization"] = 0,
		["level_limit"] = {80, 105},
		["party_member_limit"] = {0, 0},
		["map_index"] = 208,
		["map_coords"] = {1536 + 285, 12032 + 173},
		["cooldown"] = 3600,
		["duration"] = 1800,
		["entrance_map_index"] = 73,
		["strength_bonus"] = 109,
		["resistance_bonus"] = 36,
		["required_item"] = {30179, 1},
	},
	{ -- meleylair
		["status"] = 0,
		["type"] = 2,
		["organization"] = 2,
		["level_limit"] = {100, 120},
		["party_member_limit"] = {4, 8},
		["map_index"] = 212,
		["map_coords"] = {5888 + 90, 6144 + 843},
		["cooldown"] = 9000,
		["duration"] = 3600,
		["entrance_map_index"] = 62,
		["strength_bonus"] = 108,
		["resistance_bonus"] = 35,
		["required_item"] = {0, 0},
	},
	{ -- razador_zone
		["status"] = 0,
		["type"] = 1,
		["organization"] = 0,
		["level_limit"] = {90, 110},
		["party_member_limit"] = {0, 0},
		["map_index"] = 351,
		["map_coords"] = {5888 + 251, 6144 + 928},
		["cooldown"] = 4500,
		["duration"] = 3000,
		["entrance_map_index"] = 62,
		["strength_bonus"] = 108,
		["resistance_bonus"] = 35,
		["required_item"] = {71174, 1},
	},
	{ -- nemere_zone
		["status"] = 0,
		["type"] = 1,
		["organization"] = 0,
		["level_limit"] = {95, 110},
		["party_member_limit"] = {0, 0},
		["map_index"] = 352,
		["map_coords"] = {3584 + 738, 1536 + 111},
		["cooldown"] = 5400,
		["duration"] = 3000,
		["entrance_map_index"] = 61,
		["strength_bonus"] = 110,
		["resistance_bonus"] = 87,
		["required_item"] = {71175, 1},
	},
	{ -- temple_ochao
		["status"] = 0,
		["type"] = 1,
		["organization"] = 0,
		["level_limit"] = {105, 110},
		["party_member_limit"] = {0, 0},
		["map_index"] = 209,
		["map_coords"] = {11776 + 1003, 16640 + 706},
		["cooldown"] = 7200,
		["duration"] = 1800,
		["entrance_map_index"] = 302,
		["strength_bonus"] = 22,
		["resistance_bonus"] = 38,
		["required_item"] = {76025, 1},
	},
	{ -- enchanted_forest
		["status"] = 0,
		["type"] = 1,
		["organization"] = 0,
		["level_limit"] = {100, 110},
		["party_member_limit"] = {0, 0},
		["map_index"] = 210,
		["map_coords"] = {7680 + 608, 14080 + 105},
		["cooldown"] = 7200,
		["duration"] = 1800,
		["entrance_map_index"] = 210,
		["strength_bonus"] = 19,
		["resistance_bonus"] = 88,
		["required_item"] = {30613, 1},
	},
	{ -- hydra_zone
		["status"] = 0,
		["type"] = 1,
		["organization"] = 0,
		["level_limit"] = {120, 120},
		["party_member_limit"] = {0, 0},
		["map_index"] = 353,
		["map_coords"] = {11049, 17889},
		["cooldown"] = 12600,
		["duration"] = 1800,
		["entrance_map_index"] = 301,
		["strength_bonus"] = 110,
		["resistance_bonus"] = 87,
		["required_item"] = {10951, 1},
	},
	{ -- pyramide_zone
		["status"] = 0,
		["type"] = 1,
		["organization"] = 0,
		["level_limit"] = {120, 120},
		["party_member_limit"] = {0, 0},
		["map_index"] = 357,
		["map_coords"] = {9216 + 915, 13056 + 582},
		["cooldown"] = 12600,
		["duration"] = 1800,
		["entrance_map_index"] = 74,
		["strength_bonus"] = 112,
		["resistance_bonus"] = 88,
		["required_item"] = {30798, 1},
	},
	-- { -- nethis_zone
		-- ["status"] = 0,
		-- ["type"] = 1,
		-- ["organization"] = 0,
		-- ["level_limit"] = {125, 135},
		-- ["party_member_limit"] = {0, 0},
		-- ["map_index"] = 136,
		-- ["map_coords"] = {636, 14567},
		-- ["cooldown"] = 10800,
		-- ["duration"] = 1800,
		-- ["entrance_map_index"] = 135,
		-- ["strength_bonus"] = 63,
		-- ["resistance_bonus"] = 118,
		-- ["required_item"] = {70428, 1},
	-- },
	-- { -- christmas_zone1
		-- ["status"] = 0,
		-- ["type"] = 1,
		-- ["organization"] = 0,
		-- ["level_limit"] = {40, 75},
		-- ["party_member_limit"] = {0, 0},
		-- ["map_index"] = 26,
		-- ["map_coords"] = {3584 + 777, 1536 + 598},
		-- ["cooldown"] = 4200,
		-- ["duration"] = 2700,
		-- ["entrance_map_index"] = 61,
		-- ["strength_bonus"] = 110,
		-- ["resistance_bonus"] = 87,
		-- ["required_item"] = {78207, 1},
	-- },
	-- { -- christmas_zone2
		-- ["status"] = 0,
		-- ["type"] = 1,
		-- ["organization"] = 0,
		-- ["level_limit"] = {80, 120},
		-- ["party_member_limit"] = {0, 0},
		-- ["map_index"] = 25,
		-- ["map_coords"] = {3584 + 777, 1536 + 598},
		-- ["cooldown"] = 5400,
		-- ["duration"] = 2700,
		-- ["entrance_map_index"] = 61,
		-- ["strength_bonus"] = 110,
		-- ["resistance_bonus"] = 87,
		-- ["required_item"] = {78208, 1},
	-- },
	-- { -- plechito_halloween2021_dungeon1
		-- ["status"] = 0,
		-- ["type"] = 1,
		-- ["organization"] = 0,
		-- ["level_limit"] = {50, 90},
		-- ["party_member_limit"] = {0, 0},
		-- ["map_index"] = 231,
		-- ["map_coords"] = {12399, 23202}, 
		-- ["cooldown"] = 7200,
		-- ["duration"] = 1800,
		-- ["entrance_map_index"] = 226,
		-- ["strength_bonus"] = 63,
		-- ["resistance_bonus"] = 118,
		-- ["required_item"] = {30883, 1},
	-- },
	-- { -- plechito_halloween2021_dungeon2
		-- ["status"] = 0,
		-- ["type"] = 1,
		-- ["organization"] = 0,
		-- ["level_limit"] = {0, 0},
		-- ["party_member_limit"] = {0, 0},
		-- ["map_index"] = 232,
		-- ["map_coords"] = {5364, 13283},
		-- ["cooldown"] = 10800,
		-- ["duration"] = 1800,
		-- ["entrance_map_index"] = 226,
		-- ["strength_bonus"] = 63,
		-- ["resistance_bonus"] = 118,
		-- ["required_item"] = {30900, 1},
	-- },
	-- { -- plechito_halloween2021_dungeon3
		-- ["status"] = 0,
		-- ["type"] = 1,
		-- ["organization"] = 0,
		-- ["level_limit"] = {90, 105},
		-- ["party_member_limit"] = {0, 0},
		-- ["map_index"] = 233,
		-- ["map_coords"] = {12399, 23202},
		-- ["cooldown"] = 14400,
		-- ["duration"] = 1800,
		-- ["entrance_map_index"] = 226,
		-- ["strength_bonus"] = 63,
		-- ["resistance_bonus"] = 118,
		-- ["required_item"] = {30903, 1},
	-- },
	-- { -- easter_zone1
		-- ["status"] = 0,
		-- ["type"] = 1,
		-- ["organization"] = 0,
		-- ["level_limit"] = {90, 135},
		-- ["party_member_limit"] = {0, 0},
		-- ["map_index"] = 156,
		-- ["map_coords"] = {0, 0},
		-- ["cooldown"] = 9000,
		-- ["duration"] = 2400,
		-- ["entrance_map_index"] = 3,
		-- ["strength_bonus"] = 18,
		-- ["resistance_bonus"] = 88,
		-- ["required_item"] = {30603, 1},
	-- },
	-- { -- easter_zone2
		-- ["status"] = 0,
		-- ["type"] = 1,
		-- ["organization"] = 0,
		-- ["level_limit"] = {45, 90},
		-- ["party_member_limit"] = {0, 0},
		-- ["map_index"] = 157,
		-- ["map_coords"] = {0, 0},
		-- ["cooldown"] = 3600,
		-- ["duration"] = 2400,
		-- ["entrance_map_index"] = 3,
		-- ["strength_bonus"] = 18,
		-- ["resistance_bonus"] = 38,
		-- ["required_item"] = {30787, 1},
	-- },
}

function dungeonlib.update_ranking(dungeonID, rankingType)
	cmdchat(string.format("CleanDungeonRanking"))
	d.get_rank(dungeonID, rankingType)
	cmdchat(string.format("OpenDungeonRanking"))
end

function dungeonlib.get_wait_time(wait_time)
	if wait_time > 0 then
		return wait_time
	end

	return 0
end

function dungeonlib.update()
	local dungeonTable = dungeonInfo.table
	count = 0
	for _ in pairs(dungeonTable) do count = count + 1 break end
	if count == 0 then
		return
	end

	cmdchat(string.format("DungeonInfo %d", q.getcurrentquestindex()))
	cmdchat(string.format("CleanDungeonInfo"))

	for index in ipairs(dungeonTable) do
		-- Get indexed data from dungeon table
		dungeonStatus = dungeonTable[index]["status"] -- todo: (game.get_event_flag("dungeon") ~? 0:open | >0:closed/running )
		dungeonType = dungeonTable[index]["type"]
		dungeonOrganization = dungeonTable[index]["organization"]
		dungeonLevelMinLimit = dungeonTable[index]["level_limit"][1]
		dungeonLevelMaxLimit = dungeonTable[index]["level_limit"][2]
		dungeonPartyMemberMinLimit = dungeonTable[index]["party_member_limit"][1]
		dungeonPartyMemberMaxLimit = dungeonTable[index]["party_member_limit"][2]
		dungeonMapIndex = dungeonTable[index]["map_index"]
		dungeonMapCoordX = dungeonTable[index]["map_coords"][1]
		dungeonMapCoordY = dungeonTable[index]["map_coords"][2]
		dungeonCooldown = dungeonTable[index]["cooldown"]
		dungeonDuration = dungeonTable[index]["duration"]
		dungeonEntranceMapIndex = dungeonTable[index]["entrance_map_index"]
		dungeonStrengthBonus = dungeonTable[index]["strength_bonus"]
		dungeonResistanceBonus = dungeonTable[index]["resistance_bonus"]
		dungeonItemVnum = dungeonTable[index]["required_item"][1]
		dungeonItemCount = dungeonTable[index]["required_item"][2]

		-- Get dungeon rank from indexed dungeon
		if dungeonMapIndex > 0 then
			dungeonFinished = d.get_my_rank(dungeonMapIndex, 1) -- dungeonlib.get_rank(index, pc.get_name(), 1)
			dungeonFastestTime = d.get_my_rank(dungeonMapIndex, 2) -- dungeonlib.get_rank(index, pc.get_name(), 2)
			dungeonHighestDamage = d.get_my_rank(dungeonMapIndex, 3) -- dungeonlib.get_rank(index, pc.get_name(), 3)
			
			if dungeonMapIndex == 355 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("dungeonorchi", "cooldown"))
			elseif dungeonMapIndex == 27 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("slime_dungeon", "cooldown"))
			elseif dungeonMapIndex == 217 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("dungeon_spider", "cooldown"))
			elseif dungeonMapIndex == 66 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("deviltower_zone", "cooldown"))
			-- elseif dungeonMapIndex == 218 then
				-- dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("rune_zone", "cooldown"))
			elseif dungeonMapIndex == 216 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("catacombe", "cooldown"))
			elseif dungeonMapIndex == 208 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("dragonlair_zone", "cooldown"))
			elseif dungeonMapIndex == 212 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("meleylair_zone", "cooldown"))
			elseif dungeonMapIndex == 351 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("razador_zone", "cooldown"))
			elseif dungeonMapIndex == 352 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("nemere_zone", "cooldown"))
			elseif dungeonMapIndex == 209 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("temple_ochao", "cooldown"))
			elseif dungeonMapIndex == 210 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("enchanted_forest", "cooldown"))
			elseif dungeonMapIndex == 353 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("hydra_zone", "cooldown"))
			elseif dungeonMapIndex == 357 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("pyramide_zone", "cooldown"))
			--elseif dungeonMapIndex == 231 then
			--	dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("halloween_zone1", "cooldown"))
			--elseif dungeonMapIndex == 232 then
			--	dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("halloween_zone2", "cooldown"))
			--elseif dungeonMapIndex == 233 then
			--	dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("halloween_zone3", "cooldown"))
			elseif dungeonMapIndex == 136 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("nethis_zone", "cooldown"))
			elseif dungeonMapIndex == 156 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("easter_zone1", "cooldown"))
			elseif dungeonMapIndex == 157 then
				dungeonWaitTime = dungeonlib.get_wait_time(pc.getf("easter_zone2", "cooldown"))
			else
				dungeonWaitTime = 0
			end
		else
			dungeonFinished = 0
			dungeonFastestTime = 0
			dungeonHighestDamage = 0
		end

		if dungeonLevelMinLimit <= 0 then dungeonLevelMinLimit = 1 end
		if dungeonLevelMaxLimit <= 0 then dungeonLevelMaxLimit = 127 end
		if dungeonPartyMemberMinLimit <= 0 then dungeonPartyMemberMinLimit = 1 end
		if dungeonPartyMemberMaxLimit <= 0 then dungeonPartyMemberMaxLimit = 8 end

		cmdchat(string.format("UpdateDungeonInfo %d %u %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
			dungeonStatus,
			dungeonWaitTime,
			dungeonType,
			dungeonOrganization,
			dungeonLevelMinLimit,
			dungeonLevelMaxLimit,
			dungeonPartyMemberMinLimit,
			dungeonPartyMemberMaxLimit,
			dungeonMapIndex,
			dungeonMapCoordX,
			dungeonMapCoordY,
			dungeonCooldown,
			dungeonDuration,
			dungeonEntranceMapIndex,
			dungeonStrengthBonus,
			dungeonResistanceBonus,
			dungeonItemVnum,
			dungeonItemCount,
			dungeonFinished,
			dungeonFastestTime,
			dungeonHighestDamage
		))
	end
end

