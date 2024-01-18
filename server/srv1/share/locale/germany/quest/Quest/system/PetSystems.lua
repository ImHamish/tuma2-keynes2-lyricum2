quest PetSystems begin
	state start begin
		function get_pet_info(itemVnum)
			pet_info_map = {
				[55701] = {34041, "NoName", 0},
				[55702] = {34045, "NoName", 0},
				[55703] = {34049, "NoName", 0},
				[55704] = {34053, "NoName", 0},
				[55705] = {34036, "NoName", 0},
				[55706] = {34064, "NoName", 0},
				[55707] = {34073, "NoName", 0},
				[55708] = {34075, "NoName", 0},
				[55709] = {34080, "NoName", 0},
				[55710] = {34082, "NoName", 0},
				[55711] = {34095, "NoName", 0},
			}
			
			itemVnum = tonumber(itemVnum)
			return pet_info_map[itemVnum]
		end

		when 55701.use or
			55702.use or
			55703.use or
			55704.use or
			55705.use or
			55706.use or
			55707.use or
			55708.use or
			55709.use or
			55710.use or
			55711.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			local pet_info = PetSystems.get_pet_info(item.vnum)
			if null != pet_info then
				local mobVnum = pet_info[1]
				local petName = pet_info[2]
				if true == newpet.is_summon(mobVnum) then
					newpet.unsummon(mobVnum)
				else
					if newpet.count_summoned() < 1 then
						newpet.summon(mobVnum, petName, false)
					else
						syschat(gameforge[pc.get_language()].PetSystems._1)
					end
				end
			end
		end

		when 55008.use begin
			say_title(string.format("%s:", item_name(55008)))
			say("")
			local lang = pc.get_language()
			if newpet.count_summoned() < 1 then
				say(gameforge[lang].PetSystems._2)
				say(gameforge[lang].PetSystems._3)
			else
				say(gameforge[lang].PetSystems._4)
				local name = input()
				say_title(string.format("%s", item_name(55008)))
				say("")
				if name == "" then
					say(gameforge[lang].PetSystems._5)
					return
				elseif string.len(name) > 30 then
					say(gameforge[lang].PetSystems._6)
					return
				end
				
				local r = newpet.change_name(name)
				if r == 1 then
					pc.remove_item(55008, 1)
					say(gameforge[lang].PetSystems._7)
				elseif r == 0 then
					say(gameforge[lang].PetSystems._8)
				elseif r == 2 then
					say(gameforge[lang].PetSystems._9)
				elseif r == 3 then
					say(gameforge[lang].PetSystems._10)
					say(gameforge[lang].PetSystems._11)
				end
			end
		end
	end
end

