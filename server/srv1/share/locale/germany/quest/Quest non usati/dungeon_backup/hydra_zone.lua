quest hydra_zone begin
	state start begin
		function clear(arg)
			clear_server_timer("hydra_zone_prepare", arg)
			clear_server_timer("hydra_zone_end", arg)
			clear_server_timer("hydra_zone_warp", arg)
			clear_server_timer("hydra_zone_complete", arg)
			clear_server_timer("hydra_zone_check", arg)
			if d.find(arg) then
				d.setf(arg, "was_completed", 1)
				d.kill_all(arg)
				d.clear_regen(arg)
				d.exit_all_lobby(arg, 2)
			end
		end

		function spawndoors(idx, i)
			if d.find(idx) then
				if i == 1 then
					local door3vid = d.spawn_mob(idx, 3970, 402, 438, 360)
					local door4vid = d.spawn_mob(idx, 3970, 366, 438, 360)
					d.setf(idx, "door3", door3vid)
					d.setf(idx, "door4", door4vid)
				else
					local door1vid = d.spawn_mob(idx, 3970, 401, 370, 360)
					local door2vid = d.spawn_mob(idx, 3970, 368, 370, 360)
					d.setf(idx, "door1", door1vid)
					d.setf(idx, "door2", door2vid)
				end
			end
		end

		function purgedoors(idx, i)
			if d.find(idx) then
				if i == 1 then
					d.purge_vid(d.getf(idx, "door3"))
					d.purge_vid(d.getf(idx, "door4"))
				else
					d.purge_vid(d.getf(idx, "door1"))
					d.purge_vid(d.getf(idx, "door2"))
				end
			end
		end

		when hydra_zone_complete.server_timer begin
			hydra_zone.clear(get_server_timer_arg())
		end

		when hydra_zone_end.server_timer begin
			local arg = get_server_timer_arg()
			d.notice(arg, 1040, "", true)
			d.notice(arg, 1041, "", true)
			hydra_zone.clear(arg)
		end

		when 3960.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3530000 and idx < 3540000 then
				if d.getf(idx, "step") == 6 then
					d.setf(idx, "step", 7)
					d.clear_regen(idx)
					d.kill_all_monstershydra(idx)
					d.notice(idx, 1235, "", true)
					server_timer("hydra_zone_warp", 3, idx)
				end
			end
		end

		when 20437.click with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3530000 and idx < 3540000 then
				npc.lock()
				if d.getf(idx, "step") == 11 then
					party.give_gold(10000000)
					
					local lang = pc.get_language()
					say_title(string.format("%s:", mob_name(20437)))
					say("")
					say(gameforge[lang].defance_wave._3)
					npc.purge()
					d.restore_mast_partial_hp(idx)
					local c = d.getf(idx, "repaired") + 1
					if c == 5 then
						d.setf(idx, "step", 12)
						d.notice(idx, 1235, "", true)
						server_timer("hydra_zone_warp", 3, idx)
					else
						d.setf(idx, "repaired", c)
					end
				else
					npc.unlock()
					setskin(NOWINDOW)
				end
			end
		end

		when 3961.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3530000 and idx < 3540000 then
				if d.getf(idx, "step") == 10 then
					d.setf(idx, "step", 11)
					d.clear_regen(idx)
					d.kill_all_monstershydra(idx)
					d.setf(idx, "repaired", 0)
					local treepos = {
								[1] = {["x"] = 401, ["y"] = 388},
								[2] = {["x"] = 401, ["y"] = 416},
								[3] = {["x"] = 386, ["y"] = 416},
								[4] = {["x"] = 367, ["y"] = 416},
								[5] = {["x"] = 367, ["y"] = 388}
					}
					
					for index, position in ipairs(treepos) do
						local vid = d.spawn_mob(idx, 20437, position["x"], position["y"])
					end
					
					d.cmdchat(idx, "BINARY_Update_Mast_Timer 5")
				end
			end
		end

		when 3956.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3530000 and idx < 3540000 then
				if d.getf(idx, "step") == 5 then
					d.setf(idx, "step", 6)
					d.purge_vid(d.getf(idx, "boss_vid1"))
					d.spawn_mob(idx, 3960, 384, 374, 360)
					d.cmdchat(idx, "BINARY_Update_Mast_Timer 0")
				end
			end
		end

		when hydra_zone_warp.server_timer begin
			local arg = get_server_timer_arg()
			clear_server_timer("hydra_zone_warp", arg)
			if d.find(arg) then
				local s = d.getf(arg, "step")
				if s == 2 then
					d.setf(arg, "step", 3)
					d.setf(arg, "second", 5)
					d.jump_all(arg, 1665, 5221)
					server_loop_timer("hydra_zone_check", 1, arg)
				elseif s == 7 then
					d.setf(arg, "step", 8)
					d.setf(arg, "second", 5)
					hydra_zone.spawndoors(arg, 0)
					d.jump_all(arg, 1665, 5221)
					server_loop_timer("hydra_zone_check", 1, arg)
				elseif s == 12 then
					d.setf(arg, "step", 13)
					d.setf(arg, "second", 5)
					hydra_zone.spawndoors(arg, 0)
					d.jump_all(arg, 1665, 5221)
					server_loop_timer("hydra_zone_check", 1, arg)
				end
			else
				hydra_zone.clear(arg)
			end
		end

		when 3962.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3530000 and idx < 3540000 then
				if d.getf(idx, "step") == 16 then
					clear_server_timer("hydra_zone_end", idx)
					d.setf(idx, "was_completed", 1)
					d.complete(3962, 5400, 353, "hydra_zone")
					if party.is_party() then
						notice_all(1231, party.get_leader_name())
					else
						notice_all(1122, pc.get_name())
					end
					
					d.notice(idx, 1217, "", true)
					d.notice(idx, 1226, "", true)
					d.kill_all(idx)
					d.clear_regen(idx)
					d.spawn_mob(idx, 3965, 385, 417, 360)
					local bonus = 10 + game.get_event_flag("dungeon_bonus")
					if math.random(1, 100) <= bonus then
						d.spawn_mob(idx, 5003, 385, 417)
					end
					server_timer("hydra_zone_complete", 30, idx)
				end
			end
		end

		when 20432.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3530000 and idx < 3540000 then
				if d.getf(idx, "step") == 15 then
					local c = d.getf(idx, "killed") + 1
					if c == 4 then
						local success = d.set_vid_invincible(d.getf(idx, "boss"), false)
						if not success then
							hydra_zone.clear(idx)
						else
							d.setf(idx, "step", 16)
							d.cmdchat(idx, "BINARY_Update_Mast_Timer 0")
						end
					else
						d.setf(idx, "killed", c)
					end
				end
			end
		end

		when hydra_zone_check.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				local s = d.getf(arg, "step")
				if s == 1 then
					local sec = d.getf(arg, "second")
					if sec == 0 then
						clear_server_timer("hydra_zone_check", arg)
						d.setf(arg, "step", 2)
						d.setf(arg, "second", 149)
						d.notice(arg, 1238, "", true)
						hydra_zone.purgedoors(arg, 0)
						d.cmdchat(arg, "BINARY_Update_Mast_Timer 2|2|30")
						server_loop_timer("hydra_zone_check", 1, arg)
					else
						d.setf(arg, "second", sec - 1)
						local minutes = math.floor(tonumber(sec) / 60)
						local seconds = math.floor(tonumber(sec) - (tonumber(minutes) * 60))
						d.cmdchat(arg, string.format("BINARY_Update_Mast_Timer 4|%d|%d", minutes, seconds))
					end
				elseif s == 2 then
					local sec = d.getf(arg, "second")
					if sec == 0 then
						clear_server_timer("hydra_zone_check", arg)
						d.clear_regen(arg)
						hydra_zone.spawndoors(arg, 0)
						d.kill_all_monstershydra(arg)
						d.notice(arg, 1235, "", true)
						d.cmdchat(arg, "BINARY_Update_Mast_Timer 2|0|0")
						server_timer("hydra_zone_warp", 3, arg)
					else
						d.setf(arg, "second", sec - 1)
						if sec == 148 then
							d.set_regen_file(arg, "data/dungeon/hydra/regen1.txt")
						end
						
						local minutes = math.floor(tonumber(sec) / 60)
						local seconds = math.floor(tonumber(sec) - (tonumber(minutes) * 60))
						d.cmdchat(arg, string.format("BINARY_Update_Mast_Timer 2|%d|%d", minutes, seconds))
					end
				elseif s == 3 then
					local sec = d.getf(arg, "second")
					if sec == 0 then
						clear_server_timer("hydra_zone_check", arg)
						d.setf(arg, "step", 4)
						d.notice(arg, 1236, "", true)
						hydra_zone.purgedoors(arg, 0)
						d.cmdchat(arg, "BINARY_Update_Mast_Timer 4|0|0")
						server_loop_timer("hydra_zone_check", 1, arg)
					else
						d.setf(arg, "second", sec - 1)
						local minutes = math.floor(tonumber(sec) / 60)
						local seconds = math.floor(tonumber(sec) - (tonumber(minutes) * 60))
						d.cmdchat(arg, string.format("BINARY_Update_Mast_Timer 4|%d|%d", minutes, seconds))
					end
				elseif s == 4 then
					clear_server_timer("hydra_zone_check", arg)
					d.setf(arg, "step", 5)
					d.spawn_mob(arg, 3956, 385, 411, 180)
					d.set_regen_file(arg, "data/dungeon/hydra/regen1.txt")
					d.cmdchat(arg, "BINARY_Update_Mast_Timer 7")
				elseif s == 8 then
					local sec = d.getf(arg, "second")
					if sec == 0 then
						clear_server_timer("hydra_zone_check", arg)
						d.setf(arg, "step", 9)
						d.setf(arg, "second", 89)
						d.notice(arg, 1234, "", true)
						hydra_zone.purgedoors(arg, 0)
						d.cmdchat(arg, "BINARY_Update_Mast_Timer 2|1|30")
						server_loop_timer("hydra_zone_check", 1, arg)
					else
						d.setf(arg, "second", sec - 1)
						local minutes = math.floor(tonumber(sec) / 60)
						local seconds = math.floor(tonumber(sec) - (tonumber(minutes) * 60))
						d.cmdchat(arg, string.format("BINARY_Update_Mast_Timer 4|%d|%d", minutes, seconds))
					end
				elseif s == 9 then
					local sec = d.getf(arg, "second")
					if sec == 0 then
						clear_server_timer("hydra_zone_check", arg)
						d.setf(arg, "step", 10)
						d.purge_vid(d.getf(arg, "boss_vid2"))
						d.spawn_mob(arg, 3961, 384, 374, 360)
						d.cmdchat(arg, "BINARY_Update_Mast_Timer 0")
					else
						d.setf(arg, "second", sec - 1)
						if sec == 88 then
							d.set_regen_file(arg, "data/dungeon/hydra/regen2.txt")
						end
						
						local minutes = math.floor(tonumber(sec) / 60)
						local seconds = math.floor(tonumber(sec) - (tonumber(minutes) * 60))
						d.cmdchat(arg, string.format("BINARY_Update_Mast_Timer 2|%d|%d", minutes, seconds))
					end
				elseif s == 13 then
					local sec = d.getf(arg, "second")
					if sec == 0 then
						clear_server_timer("hydra_zone_check", arg)
						d.setf(arg, "step", 14)
						d.setf(arg, "second", 89)
						d.notice(arg, 1232, "", true)
						hydra_zone.purgedoors(arg, 0)
						d.cmdchat(arg, "BINARY_Update_Mast_Timer 4|0|0")
						server_loop_timer("hydra_zone_check", 1, arg)
					else
						d.setf(arg, "second", sec - 1)
						local minutes = math.floor(tonumber(sec) / 60)
						local seconds = math.floor(tonumber(sec) - (tonumber(minutes) * 60))
						d.cmdchat(arg, string.format("BINARY_Update_Mast_Timer 4|%d|%d", minutes, seconds))
					end
				elseif s == 14 then
					local sec = d.getf(arg, "second")
					if sec == 0 then
						clear_server_timer("hydra_zone_check", arg)
						d.setf(arg, "step", 15)
						d.purge_vid(d.getf(arg, "boss_vid3"))
						hydra_zone.purgedoors(arg, 1)
						local vid = d.spawn_mob(arg, 3962, 384, 374, 360)
						d.setf(arg, "boss", vid)
						local success = d.set_vid_invincible(vid, true)
						if not success then
							hydra_zone.clear(arg)
						else
							d.setf(arg, "killed", 0)
							local stonepos = {
										[1] = {["x"] = 400, ["y"] = 387},
										[2] = {["x"] = 400, ["y"] = 416},
										[3] = {["x"] = 369, ["y"] = 416},
										[4] = {["x"] = 369, ["y"] = 387}
							}
							
							for index, position in ipairs(stonepos) do
								local vid = d.spawn_mob(arg, 20432, position["x"], position["y"])
							end
							
							d.cmdchat(arg, "BINARY_Update_Mast_Timer 3")
						end
					else
						d.setf(arg, "second", sec - 1)
						if sec == 88 then
							d.set_regen_file(arg, "data/dungeon/hydra/regen3.txt")
						end
						
						local minutes = math.floor(tonumber(sec) / 60)
						local seconds = math.floor(tonumber(sec) - (tonumber(minutes) * 60))
						d.cmdchat(arg, string.format("BINARY_Update_Mast_Timer 2|%d|%d", minutes, seconds))
					end
				end
			else
				hydra_zone.clear(arg)
			end
		end

		when hydra_zone_prepare.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.setf(arg, "step", 1)
				d.setf(arg, "second", 5)
				hydra_zone.spawndoors(arg, 0)
				hydra_zone.spawndoors(arg, 1)
				d.spawn_mob(arg, 20434, 385, 401, 180)
				d.spawn_mob(arg, 20436, 385, 367, 180)
				local bosspos = {
							[1] = {["vnum"] = 3963, ["x"] = 378, ["y"] = 443, ["dir"] = 180},
							[2] = {["vnum"] = 3963, ["x"] = 385, ["y"] = 439, ["dir"] = 180},
							[3] = {["vnum"] = 3964, ["x"] = 392, ["y"] = 443, ["dir"] = 180}
				}
				
				local random_num = number(1, table.getn(bosspos))
				for index, position in ipairs(bosspos) do
					local vid = d.spawn_mob(arg, position["vnum"], position["x"], position["y"], position["dir"])
					d.setf(arg, string.format("boss_vid%d", index), vid)
				end
				
				server_timer("hydra_zone_end", 1799, arg)
				d.notice(arg, 1128, "30", true)
				server_loop_timer("hydra_zone_check", 1, arg)
			else
				hydra_zone.clear(arg)
			end
		end

		when logout begin
			local idx = pc.get_map_index()
			if idx >= 3530000 and idx < 3540000 then
				pc.setf("hydra_zone", "disconnect", get_global_time() + 300)
			end
		end

		when login begin
			local idx = pc.get_map_index()
			if idx == 353 then
				pc.warp(536900, 1331400)
			elseif idx >= 3530000 and idx < 3540000 then
				pc.set_warp_location(215, 5369, 13314)
				pc.setf("hydra_zone", "idx", idx)
				pc.setf("hydra_zone", "ch", pc.get_channel_id())
				if d.getf(idx, "floor") == 0 then
					if not party.is_party() then
						d.setf(idx, "floor", 1)
						server_timer("hydra_zone_prepare", 1, idx)
						d.setf(idx, "was_completed", 0)
					else
						if party.is_leader() then
							d.setf(idx, "floor", 1)
							server_timer("hydra_zone_prepare", 1, idx)
							d.setf(idx, "was_completed", 0)
						end
					end
				end
				
				cmdchat("BINARY_Update_Mast_Window 1")
			else
				cmdchat("BINARY_Update_Mast_Window 0")
			end
		end

		when 20013.chat."A new territory?!?" begin
			local lang = pc.get_language()
			
			say_title(string.format("%s:", mob_name(20013)))
			say("")
			say(string.format(gameforge[lang].common.dungeon_1, pc.get_name()))
			local mapIdx = pc.get_map_index()
			if mapIdx != 215 then
				return
			end
			
			say(gameforge[lang].common.dungeon_2)
			local agree = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			say_title(string.format("%s:", mob_name(20013)))
			say("")
			if agree == 2 then
				say(gameforge[lang].common.dungeon_3)
				return
			end
			
			local goahead = 1
			local rejoinTIME = pc.getf("hydra_zone", "disconnect") - get_global_time()
			if rejoinTIME > 0 then
				local rejoinIDX = pc.getf("hydra_zone", "idx")
				if rejoinIDX > 0 then
					local rejoinCH = pc.getf("hydra_zone", "ch")
					if rejoinCH != 0 and rejoinCH != pc.get_channel_id() then
						say(string.format(gameforge[lang].common.dungeon_26, rejoinCH))
						return
					end
					
					if rejoinCH != 0 and d.find(rejoinIDX) then
						if d.getf(rejoinIDX, "was_completed") == 0 then
							say(gameforge[lang].common.dungeon_4)
							local agree2 = select(gameforge[lang].common.yes, gameforge[lang].common.no)
							if agree2 == 2 then
								say_title(string.format("%s:", mob_name(20013)))
								say("")
								say(gameforge[lang].common.dungeon_3)
								return
							end
							
							local f = d.getf(rejoinIDX, "floor")
							if f != 0 then
								goahead = 0
								pc.warp(166500, 522100, rejoinIDX)
							end
						end
					end
				end
			end
			
			if goahead == 1 then
				pc.setf("hydra_zone", "disconnect", 0)
				pc.setf("hydra_zone", "idx", 0)
				pc.setf("hydra_zone", "ch", 0)
				
				say(gameforge[lang].common.dungeon_5)
				say_reward(string.format(gameforge[lang].common.dungeon_6, 105))
				say_reward(string.format(gameforge[lang].common.dungeon_7, 120))
				say_reward(string.format("- %s: %d", item_name(10951), 1))
				if party.is_party() then
					say_reward(gameforge[lang].common.dungeon_8)
				end
				say(gameforge[lang].common.dungeon_9)
				local n = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				say_title(string.format("%s:", mob_name(20013)))
				say("")
				if n == 2 then
					say(gameforge[lang].common.dungeon_3)
					return
				end
				
				local flag = string.format("ww_353_%d", pc.get_channel_id())
				local ww_flag = game.get_event_flag(flag) - get_global_time()
				if ww_flag > 0 then
					say(gameforge[lang].common.dungeon_28)
					say(string.format(gameforge[lang].common.dungeon_29, ww_flag))
					return
				end
				
				myname = pc.get_name()
				result, cooldown, name = d.check_entrance(105, 120, 10951, 0, 1, "hydra_zone.cooldown")
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
						say(string.format(gameforge[lang].common.dungeon_18, 1, item_name(10951)))
					else
						say(string.format(gameforge[lang].common.dungeon_17, name, 1, item_name(10951)))
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
					d.remove_item(10951, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5400, "hydra_zone")
					game.set_event_flag(string.format("ww_353_%d", pc.get_channel_id()), get_global_time() + 5)
					d.join_cords(353, 1665, 5221)
				end
			end
		end
	end
end

