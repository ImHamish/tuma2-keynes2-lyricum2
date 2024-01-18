quest razador_zone begin
	state start begin
		function clear(arg)
			clear_server_timer("razador_zone_prepare", arg)
			clear_server_timer("razador_zone_warp", arg)
			clear_server_timer("razador_zone_check", arg)
			clear_server_timer("razador_zone_end", arg)
			clear_server_timer("razador_zone_complete", arg)
			if d.find(arg) then
				d.setf(arg, "was_completed", 1)
				d.kill_all(arg)
				d.clear_regen(arg)
				d.exit_all_lobby(arg, 1)
			end
		end

		when razador_zone_complete.server_timer begin
			razador_zone.clear(get_server_timer_arg())
		end

		when 6091.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3510000 and idx < 3520000 then
				if d.getf(idx, "floor") == 8 then
					d.setf(idx, "was_completed", 1)
					d.complete(6091, 3600, 351, "razador_zone")
					if party.is_party() then
						notice_all(1242, party.get_leader_name())
					else
						notice_all(1243, pc.get_name())
					end
					
					d.notice(idx, 1263, "", true)
					d.notice(idx, 1261, "", true)
					d.kill_all(idx)
					d.clear_regen(idx)
					local bonus = 10 + game.get_event_flag("dungeon_bonus")
					if math.random(1, 100) <= bonus then
						d.spawn_mob(idx, 5002, 686, 637)
					end
					server_timer("razador_zone_complete", 30, idx)
				end
			end
		end

		when razador_zone_end.server_timer begin
			local arg = get_server_timer_arg()
			d.notice(arg, 1040, "", true)
			d.notice(arg, 1041, "", true)
			razador_zone.clear(arg)
		end

		when razador_zone_warp.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				clear_server_timer("razador_zone_warp", arg)
				d.jump_all(arg, 8109, 6867)
			else
				razador_zone.clear(arg)
			end
		end

		when razador_zone_check.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				local f = d.getf(arg, "floor")
				if f == 1 then
					if d.count_monster(arg) == 0 then
						clear_server_timer("razador_zone_check", arg)
						d.setf(arg, "floor", 2)
						d.setf(arg, "step", 0)
						d.clear_regen(arg)
						d.notice(arg, 1090, "", true)
						d.notice(arg, 1082, "", true)
					end
				elseif f == 3 then
					if d.count_monster(arg) == 0 then
						clear_server_timer("razador_zone_check", arg)
						d.setf(arg, "floor", 4)
						d.setf(arg, "step", 0)
						d.clear_regen(arg)
						d.notice(arg, 1090, "", true)
						d.notice(arg, 1082, "", true)
					end
				end
			else
				razador_zone.clear(arg)
			end
		end

		when 6001.kill or 6002.kill or 6003.kill or 6004.kill or 6005.kill or 6006.kill or 6007.kill or 6008.kill or 6009.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3510000 and idx < 3520000 then
				local f = d.getf(idx, "floor")
				if f == 2 then
					if number(1, 100) > 99 then -- 1%
						d.setf(idx, "floor", 3)
						d.kill_all_monsters(idx)
						d.clear_regen(idx)
						game.drop_item(30329, 1)
					end
				elseif f == 5 then
					if number(1, 100) > 98 then -- 2%
						game.drop_item(30330, 1)
					end
				end
			end
		end

		when 6051.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3510000 and idx < 3520000 then
				if d.getf(idx, "floor") == 4 then
					d.setf(idx, "floor", 5)
					d.setf(idx, "step", 0)
					d.kill_all_monsters(idx)
					d.clear_regen(idx)
					d.notice(idx, 1084, "", true)
					d.notice(idx, 1082, "", true)
				end
			end
		end

		when 20386.take with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3510000 and idx < 3520000 then
				local f = d.getf(idx, "floor")
				if f == 3 and d.getf(idx, "step") == 1 then
					if item.get_vnum() == 30329 then
						npc.purge()
						item.remove()
						d.setf(idx, "step", 0)
						d.notice(idx, 1083, "", true)
						d.notice(idx, 1082, "", true)
					end
				elseif f == 5 then
					if item.get_vnum() == 30330 then
						npc.purge()
						item.remove()
						local c = d.getf(idx, "opened") + 1
						if c == 7 then
							d.setf(idx, "floor", 6)
							d.setf(idx, "step", 0)
							pc.remove_item(30330, pc.count_item(30330))
							
							d.kill_all_monsters(idx)
							d.clear_regen(idx)
							d.notice(idx, 1085, "", true)
							d.notice(idx, 1082, "", true)
						else
							d.setf(idx, "opened", c)
							d.notice(idx, 1265, "", true)
						end
					end
				end
			end
		end

		when 8057.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3510000 and idx < 3520000 then
				if d.getf(idx, "floor") == 6 then
					d.setf(idx, "floor", 7)
					d.setf(idx, "step", 0)
					d.kill_all_monsters(idx)
					d.clear_regen(idx)
					d.notice(idx, 1086, "", true)
					d.notice(idx, 1082, "", true)
				end
			end
		end

		when 20385.chat."Mission?" with pc.in_dungeon() begin
			setskin(NOWINDOW)
			local idx = pc.get_map_index()
			if idx >= 3510000 and idx < 3520000 then
				if party.is_party() and not party.is_leader() then
					syschat(gameforge[pc.get_language()].common.dungeon_24)
					return
				end
				
				local f = d.getf(idx, "floor")
				if f == 1 then
					if d.getf(idx, "step") == 0 then
						d.setf(idx, "step", 1)
						server_loop_timer("razador_zone_check", 2, idx)
						d.kill_unique(idx, "unique1_vid1")
						d.kill_unique(idx, "unique2_vid1")
						d.regen_file(idx, "data/dungeon/razador/regen1.txt")
						d.notice(idx, 1069, "", true)
						d.notice(idx, 1070, "", true)
					end
				elseif f == 2 then
					if d.getf(idx, "step") == 0 then
						d.setf(idx, "step", 1)
						d.spawn_mob(idx, 20386, 195, 352)
						d.kill_unique(idx, "unique1_vid2")
						d.kill_unique(idx, "unique2_vid2")
						d.set_regen_file(idx, "data/dungeon/razador/regen2.txt")
						d.notice(idx, 1071, "", true)
					end
				elseif f == 3 then
					if d.getf(idx, "step") == 0 then
						d.setf(idx, "step", 1)
						server_loop_timer("razador_zone_check", 2, idx)
						d.kill_unique(idx, "unique1_vid3")
						d.kill_unique(idx, "unique2_vid3")
						d.regen_file(idx, "data/dungeon/razador/regen3.txt")
						d.notice(idx, 1072, "", true)
						d.notice(idx, 1073, "", true)
					end
				elseif f == 4 then
					if d.getf(idx, "step") == 0 then
						d.setf(idx, "step", 1)
						d.spawn_mob(idx, 6051, 470, 175)
						d.kill_unique(idx, "unique1_vid4")
						d.kill_unique(idx, "unique2_vid4")
						d.set_regen_file(idx, "data/dungeon/razador/regen4.txt")
						d.notice(idx, 1074, "", true)
					end
				elseif f == 5 then
					if d.getf(idx, "step") == 0 then
						d.setf(idx, "step", 1)
						d.setf(idx, "opened", 0)
						d.spawn_mob(idx, 20386, 486, 345)
						d.spawn_mob(idx, 20386, 511, 336)
						d.spawn_mob(idx, 20386, 525, 349)
						d.spawn_mob(idx, 20386, 521, 365)
						d.spawn_mob(idx, 20386, 503, 372)
						d.spawn_mob(idx, 20386, 486, 365)
						d.spawn_mob(idx, 20386, 500, 354)
						d.kill_unique(idx, "unique1_vid5")
						d.kill_unique(idx, "unique2_vid5")
						d.set_regen_file(idx, "data/dungeon/razador/regen5.txt")
						d.notice(idx, 1075, "", true)
						d.notice(idx, 1076, "", true)
					end
				elseif f == 6 then
					if d.getf(idx, "step") == 0 then
						d.setf(idx, "step", 1)
						d.setf(idx, "opened", 0)
						d.spawn_mob(idx, 8057, 511, 480)
						d.kill_unique(idx, "unique1_vid6")
						d.kill_unique(idx, "unique2_vid6")
						d.set_regen_file(idx, "data/dungeon/razador/regen6.txt")
						d.notice(idx, 1079, "", true)
						d.notice(idx, 1080, "", true)
					end
				elseif f == 7 then
					if d.getf(idx, "step") == 0 then
						d.setf(idx, "floor", 8)
						d.setf(idx, "step", 1)
						d.setf(idx, "opened", 0)
						d.spawn_mob(idx, 6091, 686, 637)
						d.set_regen_file(idx, "data/dungeon/razador/regen7.txt")
						d.notice(idx, 1060, "", true)
						server_timer("razador_zone_warp", 2, idx)
					end
				end
			end
		end

		when razador_zone_prepare.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.spawn_mob(arg, 20385, 352, 362)
				local lock1 = {
							[1] = {["x"] = 320, ["y"] = 394, ["dir"] = 135},
							[2] = {["x"] = 293, ["y"] = 359, ["dir"] = 90},
							[3] = {["x"] = 333, ["y"] = 321, ["dir"] = 210},
							[4] = {["x"] = 378, ["y"] = 320, ["dir"] = 152},
							[5] = {["x"] = 400, ["y"] = 355, ["dir"] = 90},
							[6] = {["x"] = 394, ["y"] = 401, ["dir"] = 223}
				}
				
				for index1, position1 in ipairs(lock1) do
					d.set_unique(arg, string.format("unique1_vid%d", index1), d.spawn_mob(arg, 20387, position1["x"], position1["y"], position1["dir"]))
				end
				
				local lock2 = {
							[1] = {["x"] = 268, ["y"] = 447, ["dir"] = 135},
							[2] = {["x"] = 234, ["y"] = 359, ["dir"] = 90},
							[3] = {["x"] = 300, ["y"] = 264, ["dir"] = 210},
							[4] = {["x"] = 454, ["y"] = 217, ["dir"] = 135},
							[5] = {["x"] = 470, ["y"] = 355, ["dir"] = 90},
							[6] = {["x"] = 467, ["y"] = 469, ["dir"] = 239}
				}
				
				for index2, position2 in ipairs(lock2) do
					d.set_unique(arg, string.format("unique2_vid%d", index2), d.spawn_mob(arg, 20388, position2["x"], position2["y"], position2["dir"]))
				end
				
				server_timer("razador_zone_end", 2999, arg)
				d.notice(arg, 1128, "50", true)
				d.notice(arg, 1082, "", true)
			else
				razador_zone.clear(arg)
			end
		end

		when logout begin
			local idx = pc.get_map_index()
			if idx >= 3510000 and idx < 3520000 then
				pc.setf("razador_zone", "disconnect", get_global_time() + 300)
			end
		end

		when login begin
			local idx = pc.get_map_index()
			if idx == 351 then
				pc.warp(535400, 1428400)
			elseif idx >= 3510000 and idx < 3520000 then
				pc.set_warp_location(219, 5354, 14284)
				pc.setf("razador_zone", "idx", idx)
				pc.setf("razador_zone", "ch", pc.get_channel_id())
				if d.getf(idx, "floor") == 0 then
					if not party.is_party() then
						d.setf(idx, "floor", 1)
						d.setf(idx, "step", 0)
						server_timer("razador_zone_prepare", 1, idx)
						d.setf(idx, "was_completed", 0)
					else
						if party.is_leader() then
							d.setf(idx, "floor", 1)
							d.setf(idx, "step", 0)
							server_timer("razador_zone_prepare", 1, idx)
							d.setf(idx, "was_completed", 0)
						end
					end
				end
			end
		end

		when 20394.chat."A new territory?!?" begin
			local lang = pc.get_language()
			
			say_title(string.format("%s:", mob_name(20394)))
			say("")
			say(string.format(gameforge[lang].common.dungeon_1, pc.get_name()))
			local mapIdx = pc.get_map_index()
			if mapIdx != 219 then
				return
			end
			
			say(gameforge[lang].common.dungeon_2)
			local agree = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			say_title(string.format("%s:", mob_name(20394)))
			say("")
			if agree == 2 then
				say(gameforge[lang].common.dungeon_3)
				return
			end
			
			local goahead = 1
			local rejoinTIME = pc.getf("razador_zone", "disconnect") - get_global_time()
			if rejoinTIME > 0 then
				local rejoinIDX = pc.getf("razador_zone", "idx")
				if rejoinIDX > 0 then
					local rejoinCH = pc.getf("razador_zone", "ch")
					if rejoinCH != 0 and rejoinCH != pc.get_channel_id() then
						say(string.format(gameforge[lang].common.dungeon_26, rejoinCH))
						return
					end
					
					if rejoinCH != 0 and d.find(rejoinIDX) then
						if d.getf(rejoinIDX, "was_completed") == 0 then
							say(gameforge[lang].common.dungeon_4)
							local agree2 = select(gameforge[lang].common.yes, gameforge[lang].common.no)
							if agree2 == 2 then
								say_title(string.format("%s:", mob_name(20394)))
								say("")
								say(gameforge[lang].common.dungeon_3)
								return
							end
							
							local f = d.getf(rejoinIDX, "floor")
							if f == 8 then
								goahead = 0
								pc.warp(810900, 686700, rejoinIDX)
							elseif f >= 1 and f <= 7 then
								goahead = 0
								pc.warp(776600, 671900, rejoinIDX)
							end
						end
					end
				end
			end
			
			if goahead == 1 then
				pc.setf("razador_zone", "disconnect", 0)
				pc.setf("razador_zone", "idx", 0)
				pc.setf("razador_zone", "ch", 0)
				
				say(gameforge[lang].common.dungeon_5)
				say_reward(string.format(gameforge[lang].common.dungeon_6, 85))
				say_reward(string.format(gameforge[lang].common.dungeon_7, 105))
				say_reward(string.format("- %s: %d", item_name(71174), 1))
				if party.is_party() then
					say_reward(gameforge[lang].common.dungeon_8)
				end
				say(gameforge[lang].common.dungeon_9)
				local n = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				say_title(string.format("%s:", mob_name(20394)))
				say("")
				if n == 2 then
					say(gameforge[lang].common.dungeon_3)
					return
				end
				
				local flag = string.format("ww_351_%d", pc.get_channel_id())
				local ww_flag = game.get_event_flag(flag) - get_global_time()
				if ww_flag > 0 then
					say(gameforge[lang].common.dungeon_28)
					say(string.format(gameforge[lang].common.dungeon_29, ww_flag))
					return
				end
				
				myname = pc.get_name()
				result, cooldown, name = d.check_entrance(85, 105, 71174, 0, 1, "razador_zone.cooldown")
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
						say(string.format(gameforge[lang].common.dungeon_18, 1, item_name(71174)))
					else
						say(string.format(gameforge[lang].common.dungeon_17, name, 1, item_name(71174)))
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
					d.remove_item(71174, 1, 0, 0, 30329, 255, 30330, 255, 0, 0, 0, 0, 0, 0, 0, 0, 3600, "razador_zone")
					game.set_event_flag(string.format("ww_351_%d", pc.get_channel_id()), get_global_time() + 5)
					d.join_cords(351, 7766, 6719)
				end
			end
		end
	end
end

