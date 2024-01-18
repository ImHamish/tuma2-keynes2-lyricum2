quest pet_system begin
	state start begin
		function get_pet_info(itemVnum)
			if pet_system.pet_info_map==nil then
				pet_system.pet_info_map = {
					[53010] = {34008},
					[53011] = {34007},
					[53012] = {34005},
					[53013] = {34006},
					[53017] = {34016},
					[53025] = {34024},
					[53026] = {34008},
					[53027] = {34008},
					[53028] = {34008},
					[53029] = {34094},
					[53030] = {34094},
					[53031] = {34094},
					[53248] = {34084},
					[53249] = {34084},
					[53250] = {34084},
					[53251] = {34085},
					[53252] = {34085},
					[53253] = {34085},
					[53256] = {34118},
					[53263] = {34093},
					[53264] = {34094},
					[53282] = {34114},
					[53283] = {34115},
					[53284] = {9483},
					[53285] = {9493},
					[53286] = {9493},
					[53287] = {9493},
					[48301] = {34100},
					[48311] = {34100},
					[48321] = {34100},
					[49010] = {34116},
					[49050] = {34117},
					[60101] = {34120},
					[60102] = {34120},
					[60103] = {34119},
					[60104] = {34119},
					[53271] = {9434},
					[53272] = {9435},
					[53273] = {9434},
					[53274] = {9435},
					[53275] = {9436},
					[53276] = {9437},
					[53277] = {59000},
					[53278] = {59001},
					[53294] = {59001},
					[53295] = {8482},
					[53296] = {8483},
					[53297] = {8484},
					[53298] = {8487},
					[53299] = {8488},
					[53300] = {8490},
					[53301] = {8492},
					[53279] = {65414},
					[53280] = {65414},
					[53288] = {9495},
					[53289] = {9496},
					[53290] = {8478},
					[53291] = {8478},
					[53292] = {8479},
					[53293] = {8479},
					[84013] = {35002},
					[84032] = {35003},
					[84052] = {35004},
					[84072] = {35005},
					[84092] = {35006},
					[84112] = {35007},
					[84133] = {35009},
					[84132] = {35008},
				}
			end
			
			return pet_system.pet_info_map[itemVnum]
		end

		function get_spawn_effect_file(idx)
			if pet_system.effect_table==nil then
				pet_system.effect_table = {
					[0] = nil,
					--[1] = "d:\\\\ymir work\\\\effect\\\\etc\\\\appear_die\\\\npc2_appear.mse",
				}
			end
			
			return pet_system.effect_table[idx]
		end

		when 53010.use or
			 53011.use or
			 53012.use or
			 53013.use or
			 53017.use or
			 53025.use or
			 53026.use or
			 53027.use or
			 53028.use or
			 53029.use or
			 53030.use or
			 53031.use or
			 53248.use or
			 53249.use or
			 53250.use or
			 53251.use or
			 53252.use or
			 53253.use or
			 53256.use or
			 53263.use or
			 53264.use or
			 53282.use or
			 53283.use or
			 53284.use or
			 53285.use or
			 53286.use or
			 53287.use or
			 48301.use or
			 48311.use or
			 48321.use or
			 49010.use or
			 49050.use or
			 60101.use or
			 60102.use or
			 60103.use or
			 60104.use or
			 53271.use or
			 53272.use or
			 53273.use or
			 53274.use or
			 53275.use or
			 53276.use or
			 53277.use or
			 53278.use or
			 53294.use or
			 53295.use or
			 53296.use or
			 53297.use or
			 53298.use or
			 53299.use or
			 53300.use or
			 53301.use or
			 53279.use or
			 53280.use or
			 53288.use or
			 53289.use or
			 53290.use or
			 53291.use or
			 53292.use or
			 53293.use or
			 84013.use or
			 84032.use or
			 84052.use or
			 84072.use or
			 84092.use or
			 84112.use or
			 84132.use or
			 84133.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			local pet_info = pet_system.get_pet_info(item.vnum)
			if null ~= pet_info then
				local mobVnum = pet_info[1]
				local spawn_effect_file_name = pet_system.get_spawn_effect_file(pet_info[3])
				if true == pet.is_summon(mobVnum) then
					if spawn_effect_file_name ~= nil then
						pet.spawn_effect(mobVnum, spawn_effect_file_name)
					end
					
					pet.unsummon(mobVnum)
				else
					if pet.count_summoned() < 1 then
						pet.summon(mobVnum, false)
					else
						syschat(gameforge[pc.get_language()].pet_system._1)
					end
					
					if spawn_effect_file_name ~= nil then
						pet.spawn_effect(mobVnum, spawn_effect_file_name)
					end
				end
			end
		end
	end
end

