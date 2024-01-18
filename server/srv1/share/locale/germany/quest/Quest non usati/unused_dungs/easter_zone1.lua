quest easter_zone1 begin
	state start begin
		function get_out_xy(empire)
			local info = {
				{3072+422, 8192+626, 3},
				{1024+473, 2048+353, 23},
				{8192+463, 2048+366, 43}
			}
			
			return info[empire]
		end

		function clear(arg)
			clear_server_timer("easter_zone1_prepare", arg)
			clear_server_timer("easter_zone1_end", arg)
			clear_server_timer("easter_zone1_complete", arg)
			if d.find(arg) then
				d.setf(arg, "was_completed", 1)
				d.kill_all(arg)
				d.clear_regen(arg)
				d.exit_all_lobby(arg, d.getf(arg, "out_x"), d.getf(arg, "out_y"))
			end
		end

		when easter_zone1_complete.server_timer begin
			easter_zone1.clear(get_server_timer_arg())
		end

		when easter_zone1_end.server_timer begin
			local arg = get_server_timer_arg()
			d.notice(arg, 1758, "", true)
			d.notice(arg, 1759, "", true)
			easter_zone1.clear(arg)
		end

		when 9446.chat."Take reward" with pc.in_dungeon() begin
			setskin(NOWINDOW)
			local idx = pc.get_map_index()
			if idx >= 1560000 and idx < 1570000 then
				if d.getf(idx, "step") == 13 and d.getf(idx, "was_completed") == 1 then
					local pid = pc.get_player_id()
					if d.getf(idx, string.format("%d_reward_state", pid)) != 1 then
						d.setf(idx, string.format("%d_reward_state", pid), 1)
						pc.give_item2(50161, 5)
						pc.give_item2(30898, 1)
					else
						local lang = pc.get_language()
						syschat(gameforge[lang].easter_zone1._1)
					end 
				end
			end
		end

		when 4525.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1560000 and idx < 1570000 then
				if d.getf(idx, "step") == 13 then
					clear_server_timer("easter_zone1_end", idx)
					d.setf(idx, "was_completed", 1)
					d.complete(4525, 9000, 156, "easter_zone1")
					if party.is_party() then
						notice_all(1756, party.get_leader_name())
					else
						notice_all(1757, pc.get_name())
					end
					
					d.notice(idx, 1760, "", true)
					d.notice(idx, 1761, "", true)
					d.kill_all(idx)
					d.clear_regen(idx)
					d.spawn_mob(idx, 9446, 229, 364, 180)
					
					server_timer("easter_zone1_complete", 30, idx)
				end
			end
		end

		when 9443.take with pc.in_dungeon() and item.get_vnum() == 30606 begin
			local idx = pc.get_map_index()
			if idx >= 1560000 and idx < 1570000 then
				local step = d.getf(idx, "step")
				
				pc.remove_item(30606, 1)
				
				if step == 6 then
					d.setf(idx, "step", 7)
					d.regen_file(idx, "data/dungeon/easter2022/regen_2f_mobs.txt")
					d.notice(idx, 1762, "", true)
				elseif step == 9 then
					d.setf(idx, "step", 10)
					d.regen_file(idx, "data/dungeon/easter2022/regen_2f_mobs.txt")
					d.notice(idx, 1762, "", true)
				elseif step == 12 then
					d.setf(idx, "step", 13)
					d.spawn_mob(idx, 4525, 229, 364)
					d.notice(idx, 1763, "", true)
				else
					return
				end
				
				local vid = npc.get_vid()
				if vid == d.getf(idx, "unique2_vid1") then
					d.spawn_mob(idx, 9444, 236, 411, 180)
					d.purge_vid(d.getf(idx, "unique2_vid1"))
				elseif vid == d.getf(idx, "unique2_vid2") then
					d.spawn_mob(idx, 9444, 276, 386, 225)
					d.purge_vid(d.getf(idx, "unique2_vid2"))
				elseif vid == d.getf(idx, "unique2_vid3") then
					d.spawn_mob(idx, 9444, 189, 381, 135)
					d.purge_vid(d.getf(idx, "unique2_vid3"))
				end
			end
		end

		when 4524.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1560000 and idx < 1570000 then
				local step = d.getf(idx, "step")
				if step == 5 then
					d.setf(idx, "step", 6)
					game.drop_item(30606, 1)
				elseif step == 8 then
					d.setf(idx, "step", 9)
					d.setf(idx, "monster_c", 0)
					game.drop_item(30606, 1)
				elseif step == 11 then
					d.setf(idx, "step", 12)
					game.drop_item(30606, 1)
				end
			end
		end

		when 9441.take with pc.in_dungeon() and item.get_vnum() == 30605 begin
			local idx = pc.get_map_index()
			if idx >= 1560000 and idx < 1570000 then
				if d.getf(idx, "step") == 4 then
					pc.remove_item(30605, 1)
					d.setf(idx, "step", 5)
					
					d.kill_all(idx)
					d.clear_regen(idx)
					
					d.setf(idx, "monster_c", 0)
					d.spawn_mob(idx, 4524, 229, 364, 180)
					
					local npcpos = {
								[1] = {["x"] = 236, ["y"] = 411, ["dir"] = 180},
								[2] = {["x"] = 276, ["y"] = 386, ["dir"] = 225},
								[3] = {["x"] = 189, ["y"] = 381, ["dir"] = 135}
					}
					
					for index, position in ipairs(npcpos) do
						local vid = d.spawn_mob(idx, 9443, position["x"], position["y"], position["dir"])
						d.setf(idx, string.format("unique2_vid%d", index), vid)
					end
					
					d.notice(idx, 1764, "", true)
					d.notice(idx, 1765, "", true)
				end
			end
		end

		when 8465.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1560000 and idx < 1570000 then
				if d.getf(idx, "step") == 3 then
					local c = d.getf(idx, "stones_c") + 1
					d.setf(idx, "stones_c", c)
					if c >= 4 then
						d.setf(idx, "step", 4)
						
						game.drop_item(30605, 1)
						
						d.notice(idx, 1767, "", true)
					else
						d.notice(idx, 1766, string.format("%d", 4 - c), true)
					end
				end
			end
		end

		when 30604.use with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1560000 and idx < 1570000 then
				if d.getf(idx, "step") == 2 then
					pc.remove_item(30604, 1)
					
					for i = 1, 4 do
						d.purge_vid(d.getf(idx, string.format("unique_vid%d", i)))
					end
					
					d.regen_file(idx, "data/dungeon/easter2022/regen_1f_stones.txt")
					
					d.setf(idx, "step", 3)
					d.notice(idx, 1768, "", true)
				end
			end
		end

		when 4518.kill or 4519.kill or 4520.kill or 4521.kill or 4522.kill or 4523.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1560000 and idx < 1570000 then
				local step = d.getf(idx, "step")
				if step == 1 then
					local c = d.getf(idx, "monster_c") + 1
					d.setf(idx, "monster_c", c)
					
					if c >= 125 then
						d.setf(idx, "step", 2)
						game.drop_item(30604, 1)
					end
				elseif step == 7 then
					local c = d.getf(idx, "monster_c") + 1
					d.setf(idx, "monster_c", c)
					
					if c >= 208 then
						d.setf(idx, "step", 8)
						d.spawn_mob(idx, 4524, 229, 364, 180)
					end
				elseif step == 10 then
					local c = d.getf(idx, "monster_c") + 1
					d.setf(idx, "monster_c", c)
					
					if c >= 208 then
						d.setf(idx, "step", 11)
						d.spawn_mob(idx, 4524, 229, 364, 180)
					end
				end
			end
		end

		when easter_zone1_prepare.server_timer begin
			local arg = get_server_timer_arg()
			clear_server_timer("easter_zone1_prepare", arg)
			if d.find(arg) then
				d.regen_file(arg, "data/dungeon/easter2022/regen_1f_mobs.txt")
				d.spawn_mob(arg, 9441, 235, 281, 180);
				
				local npcpos = {
							[1] = {["x"] = 243, ["y"] = 203},
							[2] = {["x"] = 243, ["y"] = 219},
							[3] = {["x"] = 240, ["y"] = 239},
							[4] = {["x"] = 237, ["y"] = 260}
				}
				
				for index, position in ipairs(npcpos) do
					local vid = d.spawn_mob(arg, 9442, position["x"], position["y"])
					d.setf(arg, string.format("unique_vid%d", index), vid)
				end
				
				d.setf(arg, "monster_c", 0)
				
				server_timer("easter_zone1_end", 2399, arg)
				d.notice(arg, 1769, "40", true)
				d.notice(arg, 1770, "", true)
			else
				easter_zone1.clear(arg)
			end
		end

		when logout begin
			local idx = pc.get_map_index()
			if idx >= 1560000 and idx < 1570000 then
				pc.setf("easter_zone1", "disconnect", get_global_time() + 300)
			end
		end

		when login begin
			local idx = pc.get_map_index()
			local empire = pc.get_empire()
			if idx == 156 then
				pc.warp(easter_zone1.get_out_xy(empire)[1] * 100, easter_zone1.get_out_xy(empire)[2] * 100)
			elseif idx >= 1560000 and idx < 1570000 then
				pc.set_warp_location(easter_zone1.get_out_xy(empire)[3], easter_zone1.get_out_xy(empire)[1], easter_zone1.get_out_xy(empire)[2])
				pc.setf("easter_zone1", "idx", idx)
				pc.setf("easter_zone1", "ch", pc.get_channel_id())
				if d.getf(idx, "floor") == 0 then
					if not party.is_party() then
						d.setf(idx, "floor", 1)
						d.setf(idx, "step", 1)
						server_timer("easter_zone1_prepare", 1, idx)
						d.setf(idx, "was_completed", 0)
						d.setf(idx, "out_x", easter_zone1.get_out_xy(empire)[1])
						d.setf(idx, "out_y", easter_zone1.get_out_xy(empire)[2])
					else
						if party.is_leader() then
							d.setf(idx, "floor", 1)
							d.setf(idx, "step", 1)
							server_timer("easter_zone1_prepare", 1, idx)
							d.setf(idx, "was_completed", 0)
							d.setf(idx, "out_x", easter_zone1.get_out_xy(empire)[1])
							d.setf(idx, "out_y", easter_zone1.get_out_xy(empire)[2])
						end
					end
				end
			end
		end

		when 9440.chat."A new territory?!?" begin
			local lang = pc.get_language()
			
			say_title(string.format("%s:", mob_name(9440)))
			say("")
			say(string.format(gameforge[lang].common.dungeon_1, pc.get_name()))
			local mapIdx = pc.get_map_index()
			if mapIdx != 3 and mapIdx != 23 and mapIdx != 43 then
				return
			end
			
			say(gameforge[lang].common.dungeon_2)
			local agree = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			say_title(string.format("%s:", mob_name(9440)))
			say("")
			if agree == 2 then
				say(gameforge[lang].common.dungeon_3)
				return
			end
			
			local goahead = 1
			local rejoinTIME = pc.getf("easter_zone1", "disconnect") - get_global_time()
			if rejoinTIME > 0 then
				local rejoinIDX = pc.getf("easter_zone1", "idx")
				if rejoinIDX > 0 then
					local rejoinCH = pc.getf("easter_zone1", "ch")
					if rejoinCH != 0 and rejoinCH != pc.get_channel_id() then
						say(string.format(gameforge[lang].common.dungeon_26, rejoinCH))
						return
					end
					
					if rejoinCH != 0 and d.find(rejoinIDX) then
						if d.getf(rejoinIDX, "was_completed") == 0 then
							say(gameforge[lang].common.dungeon_4)
							local agree2 = select(gameforge[lang].common.yes, gameforge[lang].common.no)
							if agree2 == 2 then
								say_title(string.format("%s:", mob_name(9440)))
								say("")
								say(gameforge[lang].common.dungeon_3)
								return
							end
							
							goahead = 0
							pc.warp(3608300, 2270300, rejoinIDX)
						end
					end
				end
			end
			
			if goahead == 1 then
				pc.setf("easter_zone1", "disconnect", 0)
				pc.setf("easter_zone1", "idx", 0)
				pc.setf("easter_zone1", "ch", 0)
				
				say(gameforge[lang].common.dungeon_5)
				say_reward(string.format(gameforge[lang].common.dungeon_6, 90))
				say_reward(string.format(gameforge[lang].common.dungeon_7, 135))
				say_reward(string.format("- %s: %d", item_name(30603), 1))
				if party.is_party() then
					say_reward(gameforge[lang].common.dungeon_8)
				end
				say(gameforge[lang].common.dungeon_9)
				local n = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				say_title(string.format("%s:", mob_name(9440)))
				say("")
				if n == 2 then
					say(gameforge[lang].common.dungeon_3)
					return
				end
				
				local flag = string.format("ww_156_%d", pc.get_channel_id())
				local ww_flag = game.get_event_flag(flag) - get_global_time()
				if ww_flag > 0 then
					say(gameforge[lang].common.dungeon_28)
					say(string.format(gameforge[lang].common.dungeon_29, ww_flag))
					return
				end
				
				myname = pc.get_name()
				result, cooldown, name = d.check_entrance(90, 135, 30603, 0, 1, "easter_zone1.cooldown")
				if result == 0 then
					say(gameforge[lang].common.unknownerr)
				elseif result == 2 then
					say(gameforge[lang].common.dungeon_10)
				elseif result == 3 then
					if myname == name then
						say(gameforge[lang].common.dungeon_20)
					else
						say(string.format(gameforge[lang].common.dungeon_19, name))
					end
				elseif result == 4 then
					if myname == name then
						say(gameforge[lang].common.dungeon_12)
					else
						say(string.format(gameforge[lang].common.dungeon_11, name))
					end
				elseif result == 5 then
					if myname == name then
						say(gameforge[lang].common.dungeon_14)
					else
						say(string.format(gameforge[lang].common.dungeon_13, name))
					end
				elseif result == 6 or result == 7 then
					if myname == name then
						say(string.format(gameforge[lang].common.dungeon_18, 1, item_name(30603)))
					else
						say(string.format(gameforge[lang].common.dungeon_17, name, 1, item_name(30603)))
					end
				elseif result == 8 then
					local h = math.floor(cooldown / 3600)
					local m = math.floor((cooldown - (3600 * h)) / 60)
					local hS = gameforge[lang].common.hours
					if h == 1 then
						hS = gameforge[lang].common.hour
					end
					local mS = gameforge[lang].common.minutes
					if m == 1 then
						mS = gameforge[lang].common.minute
					end
					
					if myname == name then
						say(string.format(gameforge[lang].common.dungeon_16, h, hS, m, mS))
					else
						say(string.format(gameforge[lang].common.dungeon_15, name, h, hS, m, mS))
					end
				elseif result == 1 then
					say(gameforge[lang].common.dungeon_21)
					wait()
					d.remove_item(30603, 1, 0, 0, 30604, 255, 30605, 255, 30606, 255, 0, 0, 0, 0, 0, 0, 9000, "easter_zone1")
					game.set_event_flag(string.format("ww_156_%d", pc.get_channel_id()), get_global_time() + 5)
					d.join_cords(156, 36083, 22703)
				end
			end
		end
	end
end

