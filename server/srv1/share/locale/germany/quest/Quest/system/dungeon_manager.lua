quest dungeon_manager begin
	state start begin
		when login begin
			dungeonlib.update()
		end
		
		when button begin
			cmdchat("GetDungeonInfo INPUT#1")
			local cmd = split(input(cmdchat("GetDungeonInfo CMD#")), "#")
			cmdchat("GetDungeonInfo INPUT#0")
			if cmd[1] == "WARP" then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].dungeon_manager._1)
					return
				end
				
				local index = tonumber(cmd[2]) + 1
				if index < 0 or index > 16 then
					syschat("?!?")
					return
				end
				
				local dungeonTable = dungeonInfo.table
				if pc.get_level() < dungeonTable[index]["level_limit"][1] then
					syschat(string.format(gameforge[pc.get_language()].dungeon_manager._2, dungeonTable[index]["level_limit"][1]))
					return
				end
				
				local mapIdx = dungeonTable[index]["map_index"]
				if mapIdx == 156 or mapIdx == 157 then
					local info = {
						{3072+422, 8192+626},
						{1024+473, 2048+353},
						{8192+463, 2048+366}
					}

					pc.warp(info[pc.get_empire()][1] * 100, info[pc.get_empire()][2] * 100)
				else
					pc.warp(tonumber(cmd[3]) * 100, tonumber(cmd[4]) * 100)
				end
			elseif cmd[1] == "RANKING" then
				dungeonlib.update_ranking(tonumber(cmd[2]), tonumber(cmd[3]))
			end
		end
	end
end

