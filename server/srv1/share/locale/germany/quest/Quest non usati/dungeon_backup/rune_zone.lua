quest rune_zone begin
	state start begin
		function clear(arg)
			clear_server_timer("rune_zone_prepare", arg)
			clear_server_timer("rune_zone_end", arg)
			clear_server_timer("rune_step_limit", arg)
			clear_server_timer("rune_zone_check", arg)
			clear_server_timer("rune_zone_warp", arg)
			clear_server_timer("rune_zone_complete", arg)
			if d.find(arg) then
				d.setf(arg, "was_completed", 1)
				d.kill_all(arg)
				d.clear_regen(arg)
				d.exit_all_lobby(arg, 2)
			end
		end

		function create_randomfloor(arg)
			if d.find(arg) then
				clear_server_timer("rune_step_limit", arg)
				d.kill_all_monsters(arg)
				d.clear_regen(arg)
				local f = d.getf(arg, "floor")
				if f == 1 then
					d.setf(arg, "floor", 2)
					local t = number(1, 3)
					d.setf(arg, "type", t)
					if t == 1 then
						d.set_regen_file(arg, "data/dungeon/rune/regen2_type1.txt")
					elseif t == 2 then
						clear_server_timer("rune_zone_check", arg)
						d.setf(arg, "step", 0)
						local vid = d.spawn_mob(arg, 3997, 146, 377)
						d.setf(arg, "boss", vid)
						server_loop_timer("rune_zone_check", 2, arg)
					elseif t == 3 then
						clear_server_timer("rune_zone_check", arg)
						d.setf(arg, "step", 0)
						d.regen_file(arg, "data/dungeon/rune/regen2_type3a.txt")
						d.regen_file(arg, "data/dungeon/rune/regen2_type3b.txt")
						server_loop_timer("rune_zone_check", 2, arg)
					end
					
					server_timer("rune_step_limit", 1199, arg)
					server_timer("rune_zone_warp", 3, arg)
					d.notice(arg, 979, "", true)
					d.notice(arg, 980, "", true)
				elseif f == 2 then
					d.setf(arg, "floor", 3)
					local t = number(1, 3)
					d.setf(arg, "type", t)
					if t == 1 then
						d.set_regen_file(arg, "data/dungeon/rune/regen3_type1.txt")
					elseif t == 2 then
						clear_server_timer("rune_zone_check", arg)
						d.setf(arg, "step", 0)
						local vid = d.spawn_mob(arg, 3998, 592, 377)
						d.setf(arg, "boss", vid)
						server_loop_timer("rune_zone_check", 2, arg)
					elseif t == 3 then
						clear_server_timer("rune_zone_check", arg)
						d.setf(arg, "step", 0)
						d.regen_file(arg, "data/dungeon/rune/regen3_type3a.txt")
						d.regen_file(arg, "data/dungeon/rune/regen3_type3b.txt")
						server_loop_timer("rune_zone_check", 2, arg)
					end
					
					server_timer("rune_step_limit", 1199, arg)
					server_timer("rune_zone_warp", 3, arg)
					d.notice(arg, 952, "", true)
				elseif f == 3 then
					clear_server_timer("rune_zone_check", arg)
					d.setf(arg, "floor", 4)
					local t = number(1, 3)
					d.setf(arg, "type", t)
					if t == 1 then
						d.set_regen_file(arg, "data/dungeon/rune/regen4_type1.txt")
					elseif t == 2 then
						d.setf(arg, "step", 0)
						local vid = d.spawn_mob(arg, 3996, 414, 121)
						d.setf(arg, "boss", vid)
						server_loop_timer("rune_zone_check", 2, arg)
					elseif t == 3 then
						clear_server_timer("rune_zone_check", arg)
						d.setf(arg, "step", 0)
						d.regen_file(arg, "data/dungeon/rune/regen4_type3a.txt")
						d.regen_file(arg, "data/dungeon/rune/regen4_type3b.txt")
						server_loop_timer("rune_zone_check", 2, arg)
					end
					
					server_timer("rune_step_limit", 1199, arg)
					server_timer("rune_zone_warp", 3, arg)
					d.notice(arg, 953, "", true)
				elseif f == 4 then
					clear_server_timer("rune_zone_check", arg)
					d.setf(arg, "floor", 5)
					d.setf(arg, "type", 4)
					local vid = d.spawn_mob(arg, 4012, 794, 107)
					d.setf(arg, "boss", vid)
					local success = d.set_vid_invincible(vid, true)
					if not success then
						d.notice(arg, 982, "", true)
						rune_zone.clear(arg)
					else
						d.regen_file(arg, "data/dungeon/rune/regen5.txt")
						server_loop_timer("rune_zone_check", 2, arg)
						server_timer("rune_step_limit", 1799, arg)
						server_timer("rune_zone_warp", 3, arg)
					end
				end
			else
				rune_zone.clear(arg)
			end
		end

		when rune_zone_complete.server_timer begin
			rune_zone.clear(get_server_timer_arg())
		end

		when rune_zone_end.server_timer begin
			local arg = get_server_timer_arg()
			d.notice(arg, 1040, "", true)
			d.notice(arg, 1041, "", true)
			rune_zone.clear(arg)
		end

		when rune_step_limit.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.notice(arg, 1040, "", true)
				d.notice(arg, 1041, "", true)
			end
			
			rune_zone.clear(arg)
		end

		when rune_zone_warp.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				clear_server_timer("rune_zone_warp", arg)
				local f = d.getf(arg, "floor")
				local notice = false
				if f == 2 then
					d.jump_all(arg, 6273, 14468)
					notice = true
				elseif f == 3 then
					d.jump_all(arg, 6732, 14440)
					notice = true
				elseif f == 4 then
					d.jump_all(arg, 6554, 14212)
					notice = true
				elseif f == 5 then
					d.jump_all(arg, 6955, 14213)
					d.notice(arg, 961, "30", true)
					d.notice(arg, 962, "", true)
				end
				
				if notice then
					local t = d.getf(arg, "type")
					if t == 1 then
						d.notice(arg, 957, "20", true)
						d.notice(arg, 958, "", true)
					elseif t == 2 then
						d.notice(arg, 959, "20", true)
					elseif t == 3 then
						d.notice(arg, 959, "20", true)
						d.notice(arg, 960, "", true)
					end
				end
			else
				rune_zone.clear(arg)
			end
		end

		when 89103.use with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 2180000 and idx < 2190000 then
				if d.getf(idx, "type") == 1 then
					pc.remove_item(89103, 1)
					rune_zone.create_randomfloor(idx)
				end
			end
		end

		when 89102.use with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 2180000 and idx < 2190000 then
				local f = d.getf(idx, "floor")
				if f == 2 or f == 3 or f == 4 then
					if d.getf(idx, "type") == 1 then
						if pc.count_item(89102) < 10 then
							syschat(gameforge[lang].rune_dungeon._27)
							return
						end
						
						if party.is_party() and not party.is_leader() then
							syschat(gameforge[lang].rune_dungeon._28)
							return
						end
						
						pc.remove_item(89102, 10)
						pc.give_item2(89103, 1)
					end
				end
			end
		end

		when 4003.kill or 4004.kill or 4005.kill or 4006.kill or 4007.kill or 4008.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 2180000 and idx < 2190000 then
				if d.getf(idx, "type") == 1 then
					if number(1, 100) > 95 then -- 5%
						if not party.is_party() then
							if pc.count_item(89102) < 10 and pc.count_item(89103) < 1 then
								pc.give_item2(89102, 1)
							end
						else
							if party.is_leader() then
								if pc.count_item(89102) < 10 and pc.count_item(89103) < 1 then
									pc.give_item2(89102, 1)
								end
							end
						end
					end
				end
			end
		end

		when 8202.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 2180000 and idx < 2190000 then
				if d.getf(idx, "floor") == 1 then
					local c = d.getf(idx, "count", 0) + 1
					if c == 6 then
						rune_zone.create_randomfloor(idx)
					elseif c < 6 then
						local vid = npc.get_vid()
						for i=1,6 do
							if d.getf(idx, string.format("unique_vid%d", i)) == vid then
								d.setf(idx, string.format("done_vid%d", i), 1)
							end
						end
						
						for i=1,6 do
							if d.getf(idx, string.format("done_vid%d", i)) == 0 then
								local success = d.set_vid_invincible(d.getf(idx, string.format("unique_vid%d", i)), false)
								if not success then
									d.notice(idx, 977, "", true)
									rune_zone.clear(idx)
									break
								else
									d.setf(idx, string.format("done_vid%d", i), 1)
									break
								end
							end
						end
						
						d.notice(idx, 978, string.format("%d", 6 - c), true)
					end
					d.setf(idx, "count", c)
				end
			end
		end

		when 3997.kill or 3998.kill or 3996.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 2180000 and idx < 2190000 then
				rune_zone.create_randomfloor(idx)
			end
		end

		when 4013.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 2180000 and idx < 2190000 then
				if d.getf(idx, "type") == 7 then
					d.setf(idx, "step", 0)
					d.setf(idx, "type", 8)
					local vid = d.spawn_mob(idx, 4011, 794, 107)
					d.setf(idx, "boss", vid)
					server_loop_timer("rune_zone_check", 2, idx)
					d.notice(idx, 965, "", true)
					d.notice(idx, 966, "", true)
				end
			end
		end

		when 20507.take with pc.in_dungeon() and item.get_vnum() == 89103 begin
			local idx = pc.get_map_index()
			if idx >= 2180000 and idx < 2190000 then
				if d.getf(idx, "type") == 6 then
					npc.purge()
					item.remove()
					local c = d.getf(idx, "opened") + 1
					if c == 5 then
						d.setf(idx, "type", 7)
						pc.remove_item(89103, pc.count_item(89103))
						d.clear_regen(idx)
						local success = d.set_vid_invincible(d.getf(idx, "boss"), false)
						if success then
							d.notice(idx, 967, "", true)
						else
							d.notice(idx, 983, "", true)
							rune_zone.clear(idx)
						end
					else
						d.setf(idx, "opened", c)
						d.notice(idx, 968, string.format("%d", 5 - c), true)
					end
				end
			end
		end

		when 4009.kill or 4010.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 2180000 and idx < 2190000 then
				if d.getf(idx, "type") == 6 then
					if number(1, 100) > 97 then -- 3%
						game.drop_item(89103, 1)
					end
				end
			end
		end

		when 4012.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 2180000 and idx < 2190000 then
				if d.getf(idx, "type") == 5 then
					d.setf(idx, "type", 6)
					local vid = d.spawn_mob(idx, 4013, 794, 107)
					d.setf(idx, "boss", vid)
					local success = d.set_vid_invincible(vid, true)
					if not success then
						d.notice(idx, 983, "", true)
						rune_zone.clear(idx)
					else
						d.setf(idx, "opened", 0)
						d.regen_file(idx, "data/dungeon/rune/regen6.txt")
						d.set_regen_file(idx, "data/dungeon/rune/regen7.txt")
						d.notice(idx, 969, "", true)
						d.notice(idx, 970, "", true)
						d.notice(idx, 971, "", true)
					end
				end
			end
		end

		when 4011.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 2180000 and idx < 2190000 then
				if d.getf(idx, "type") == 8 then
					clear_server_timer("rune_step_limit", idx)
					d.setf(idx, "was_completed", 1)
					d.complete(4011, 7200, 218, "rune_zone")
					if party.is_party() then
						notice_all(1283, party.get_leader_name())
					else
						notice_all(1239, pc.get_name())
					end
					
					d.notice(idx, 963, "", true)
					d.notice(idx, 964, "", true)
					d.kill_all(idx)
					d.clear_regen(idx)
					local bonus = 10 + game.get_event_flag("dungeon_bonus")
					if math.random(1, 100) <= bonus then
						d.spawn_mob(idx, 5002, 794, 107)
					end
					server_timer("rune_zone_complete", 30, idx)
				end
			end
		end

		when rune_zone_check.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				local f = d.getf(arg, "floor")
				local t = d.getf(arg, "type")
				if f == 1 then
					if d.count_monster(arg) == 6 then
						clear_server_timer("rune_zone_check", arg)
						local stone = number(1, 6)
						local success = d.set_vid_invincible(d.getf(arg, string.format("unique_vid%d", stone)), false)
						if success then
							d.notice(arg, 975, "", true)
							d.notice(arg, 976, "", true)
						else
							d.notice(arg, 977, "", true)
							rune_zone.clear(arg)
						end
					end
				elseif t == 2 then
					local s = d.getf(arg, "step")
					if (s == 1 or s == 3) and d.count_monster(arg) == 1 then
						local success = d.set_vid_invincible(d.getf(arg, "boss"), false)
						if success then
							if s == 1 then
								d.setf(arg, "step", 2)
							elseif s == 3 then
								d.setf(arg, "step", 4)
							end
							
							d.notice(arg, 938, "", true)
						else
							d.notice(arg, 981, "", true)
							rune_zone.clear(arg)
						end
					end
				elseif t == 3 then
					if d.count_monster(arg) == 0 then
						clear_server_timer("rune_zone_check", arg)
						if f == 2 then
							d.spawn_mob(arg, 3997, 146, 377)
						elseif f == 3 then
							d.spawn_mob(arg, 3998, 592, 377)
						elseif f == 4 then
							d.spawn_mob(arg, 3996, 414, 121)
						end
						
						d.notice(arg, 939, "", true)
						d.notice(arg, 940, "", true)
					end
				elseif t == 4 then
					if d.count_monster(arg) == 1 then
						clear_server_timer("rune_zone_check", arg)
						d.setf(arg, "type", 5)
						local success = d.set_vid_invincible(d.getf(arg, "boss"), false)
						if success then
							d.notice(arg, 938, "", true)
						else
							d.notice(arg, 983, "", true)
							rune_zone.clear(arg)
						end
					end
				elseif t == 8 then
					local s = d.getf(arg, "step")
					if (s == 1 or s == 3) and d.count_monster(arg) == 1 then
						local success = d.set_vid_invincible(d.getf(arg, "boss"), false)
						if success then
							if s == 1 then
								d.setf(arg, "step", 2)
							elseif s == 3 then
								d.setf(arg, "step", 4)
							end
							
							d.notice(arg, 938, "", true)
						else
							d.notice(arg, 983, "", true)
							rune_zone.clear(arg)
						end
					end
				end
			else
				rune_zone.clear(arg)
			end
		end

		when rune_zone_prepare.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.setf(arg, "type", 0)
				d.regen_file(arg, "data/dungeon/rune/regen1.txt")
				
				local stones = {
							[1] = {["x"] = 145, ["y"] = 106},
							[2] = {["x"] = 126, ["y"] = 127},
							[3] = {["x"] = 132, ["y"] = 156},
							[4] = {["x"] = 170, ["y"] = 155},
							[5] = {["x"] = 163, ["y"] = 115},
							[6] = {["x"] = 161, ["y"] = 133}
				}
				
				table_shuffle(stones)
				local success = true
				for index, position in ipairs(stones) do
					local vid = d.spawn_mob(arg, 8202, position["x"], position["y"])
					success = d.set_vid_invincible(vid, true)
					if not success then
						break
					else
						d.setf(arg, string.format("unique_vid%d", index), vid)
						d.setf(arg, string.format("done_vid%d", index), 0)
					end
				end
				
				if not success then
					d.notice(arg, 948, "", true)
					rune_zone.clear(arg)
				else
					d.setf(arg, "count", 0)
					server_timer("rune_step_limit", 899, arg)
					server_timer("rune_zone_end", 3899, arg)
					server_loop_timer("rune_zone_check", 2, arg)
					d.notice(arg, 949, "15", true)
					d.notice(arg, 950, "", true)
					d.notice(arg, 951, "", true)
				end
			else
				rune_zone.clear(arg)
			end
		end

		when logout begin
			local idx = pc.get_map_index()
			if idx >= 2180000 and idx < 2190000 then
				pc.setf("rune_zone", "disconnect", get_global_time() + 300)
			end
		end

		when login begin
			local idx = pc.get_map_index()
			if idx == 218 then
				pc.warp(536900, 1331400)
			elseif idx >= 2180000 and idx < 2190000 then
				pc.set_warp_location(215, 5369, 13314)
				pc.setf("rune_zone", "idx", idx)
				pc.setf("rune_zone", "ch", pc.get_channel_id())
				if d.getf(idx, "floor") == 0 then
					if not party.is_party() then
						d.setf(idx, "floor", 1)
						server_timer("rune_zone_prepare", 1, idx)
						d.setf(idx, "was_completed", 0)
					else
						if party.is_leader() then
							d.setf(idx, "floor", 1)
							server_timer("rune_zone_prepare", 1, idx)
							d.setf(idx, "was_completed", 0)
						end
					end
				end
			end
		end

		when 20506.chat."A new territory?!?" begin
			local lang = pc.get_language()
			
			say_title(string.format("%s:", mob_name(20506)))
			say("")
			say(string.format(gameforge[lang].common.dungeon_1, pc.get_name()))
			local mapIdx = pc.get_map_index()
			if mapIdx != 215 then
				return
			end
			
			say(gameforge[lang].common.dungeon_2)
			local agree = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			say_title(string.format("%s:", mob_name(20506)))
			say("")
			if agree == 2 then
				say(gameforge[lang].common.dungeon_3)
				return
			end
			
			local goahead = 1
			local rejoinTIME = pc.getf("rune_zone", "disconnect") - get_global_time()
			if rejoinTIME > 0 then
				local rejoinIDX = pc.getf("rune_zone", "idx")
				if rejoinIDX > 0 then
					local rejoinCH = pc.getf("rune_zone", "ch")
					if rejoinCH != 0 and rejoinCH != pc.get_channel_id() then
						say(string.format(gameforge[lang].common.dungeon_26, rejoinCH))
						return
					end
					
					if rejoinCH != 0 and d.find(rejoinIDX) then
						if d.getf(rejoinIDX, "was_completed") == 0 then
							say(gameforge[lang].common.dungeon_4)
							local agree2 = select(gameforge[lang].common.yes, gameforge[lang].common.no)
							if agree2 == 2 then
								say_title(string.format("%s:", mob_name(20506)))
								say("")
								say(gameforge[lang].common.dungeon_3)
								return
							end
							
							local f = d.getf(rejoinIDX, "floor")
							if f == 1 then
								goahead = 0
								pc.warp(624500, 1415200, rejoinIDX)
							elseif f == 2 then
								goahead = 0
								pc.warp(627300, 1446800, rejoinIDX)
							elseif f == 3 then
								goahead = 0
								pc.warp(673200, 1444000, rejoinIDX)
							elseif f == 4 then
								goahead = 0
								pc.warp(655400, 1421200, rejoinIDX)
							elseif f == 5 then
								goahead = 0
								pc.warp(695500, 1421300, rejoinIDX)
							end
						end
					end
				end
			end
			
			if goahead == 1 then
				pc.setf("rune_zone", "disconnect", 0)
				pc.setf("rune_zone", "idx", 0)
				pc.setf("rune_zone", "ch", 0)
				
				say(gameforge[lang].common.dungeon_5)
				say_reward(string.format(gameforge[lang].common.dungeon_6, 70))
				say_reward(string.format(gameforge[lang].common.dungeon_7, 120))
				say_reward(string.format("- %s: %d", item_name(89101), 1))
				if party.is_party() then
					say_reward(gameforge[lang].common.dungeon_8)
				end
				say(gameforge[lang].common.dungeon_9)
				local n = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				say_title(string.format("%s:", mob_name(20506)))
				say("")
				if n == 2 then
					say(gameforge[lang].common.dungeon_3)
					return
				end
				
				local flag = string.format("ww_218_%d", pc.get_channel_id())
				local ww_flag = game.get_event_flag(flag) - get_global_time()
				if ww_flag > 0 then
					say(gameforge[lang].common.dungeon_28)
					say(string.format(gameforge[lang].common.dungeon_29, ww_flag))
					return
				end
				
				myname = pc.get_name()
				result, cooldown, name = d.check_entrance(70, 120, 89101, 89107, 1, "rune_zone.cooldown")
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
						say(string.format(gameforge[lang].common.dungeon_18, 1, item_name(89101)))
					else
						say(string.format(gameforge[lang].common.dungeon_17, name, 1, item_name(89101)))
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
					d.remove_item(89101, 1, 89107, 1, 89102, 255, 89103, 255, 0, 0, 0, 0, 0, 0, 0, 0, 7200, "rune_zone")
					game.set_event_flag(string.format("ww_218_%d", pc.get_channel_id()), get_global_time() + 5)
					d.join_cords(218, 6245, 14152)
				end
			end
		end

		when 89100.use begin
			say_title(string.format("%s:", item_name(89100)))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].rune_dungeon._1)
			local cooldown = pc.getf("rune_zone", "cooldown") - get_global_time()
			if cooldown < 0 then
				cooldown = 0
			end
			
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
			
			say(string.format(gameforge[lang].rune_dungeon._2, h, hS, m, mS))
			if cooldown > 0 then
				say(gameforge[lang].rune_dungeon._3)
				local s = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				if s == 2 then
					return
				end
				
				pc.setf("rune_zone", "disconnect", 0)
				pc.setf("rune_zone", "idx", 0)
				pc.setf("rune_zone", "ch", 0)
				pc.setf("rune_zone", "cooldown", 0)
				pc.remove_item(89100, 1)
				
				say_title(string.format("%s:", item_name(89100)))
				say("")
				say(gameforge[lang].rune_dungeon._4)
			end
		end
	end
end

