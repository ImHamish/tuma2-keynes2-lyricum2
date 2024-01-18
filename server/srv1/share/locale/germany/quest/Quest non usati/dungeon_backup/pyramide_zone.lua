quest pyramide_zone begin
	state start begin
		function clear(arg)
			clear_server_timer("pyramide_zone_prepare", arg)
			clear_server_timer("pyramide_zone_end", arg)
			clear_server_timer("pyramide_zone_complete", arg)
			clear_server_timer("pyramide_zone_check", arg)
			clear_server_timer("pyramide_zone_warp", arg)
			clear_server_timer("pyramide_zone_limit", arg)
			clear_server_timer("pyramide_zone_spawn", arg)
			if d.find(arg) then
				d.setf(arg, "was_completed", 1)
				d.kill_all(arg)
				d.clear_regen(arg)
				d.exit_all_lobby(arg, 2)
			end
		end

		when pyramide_zone_complete.server_timer begin
			pyramide_zone.clear(get_server_timer_arg())
		end

		when pyramide_zone_end.server_timer begin
			local arg = get_server_timer_arg()
			d.notice(arg, 1040, "", true)
			d.notice(arg, 1041, "", true)
			pyramide_zone.clear(arg)
		end

		when 8472.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3570000 and idx < 3580000 then
				if d.getf(idx, "floor") == 1 then
					clear_server_timer("pyramide_zone_limit", idx)
					d.setf(idx, "floor", 2)
					d.setf(idx, "count", 0)
					d.spawn_mob(idx, 9332, 306, 158, 45)
					d.set_regen_file(idx, "data/dungeon/pyramide/regen1.txt")
					d.notice(idx, 993, "", true)
					server_timer("pyramide_zone_warp", 3, idx)
				end
			end
		end

		when 8474.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3570000 and idx < 3580000 then
				if d.getf(idx, "floor") == 6 then
					local c = d.getf(idx, "count")
					if c == 2 then
						d.setf(idx, "count", 3)
						game.drop_item(30804)
					elseif c == 6 then
						local c = d.getf(idx, "killed") + 1
						if c == 6 then
							d.setf(idx, "count", 7)
							game.drop_item(30804)
						else
							d.setf(idx, "killed", c)
						end
					end
				end
			end
		end

		when pyramide_zone_spawn.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				local f = d.getf(arg, "floor")
				if f == 7 then
					clear_server_timer("pyramide_zone_spawn", arg)
					d.regen_file(arg, "data/dungeon/pyramide/regen3.txt")
				end
			else
				pyramide_zone.clear(arg)
			end
		end

		when 8475.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3570000 and idx < 3580000 then
				if d.getf(idx, "floor") == 7 then
					d.setf(idx, "canpick", 1)
					d.notice(idx, 1193, "", true)
				end
			end
		end

		when 4158.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3570000 and idx < 3580000 then
				if d.getf(idx, "floor") == 7 then
					d.setf(idx, "was_completed", 1)
					d.complete(4158, 3600, 357, "pyramide_zone")
					if party.is_party() then
						notice_all(1270, party.get_leader_name())
					else
						notice_all(1271, pc.get_name())
					end
					
					d.notice(idx, 1207, "", true)
					d.notice(idx, 1208, "", true)
					d.notice(idx, 1252, "", true)
					d.kill_all(idx)
					d.clear_regen(idx)
					d.spawn_mob(idx, 9337, 992, 1142, 270)
					local bonus = 10 + game.get_event_flag("dungeon_bonus")
					if math.random(1, 100) <= bonus then
						d.spawn_mob(idx, 5003, 992, 1142)
					end
					server_timer("pyramide_zone_complete", 180, idx)
				end
			end
		end

		when 9334.take with pc.in_dungeon() and item.get_vnum() == 30805 begin
			local idx = pc.get_map_index()
			if idx >= 3570000 and idx < 3580000 then
				if d.getf(idx, "floor") == 7 then
					item.remove()
					npc.kill()
					if d.getf(idx, "opened") == 1 then
						d.kill_unique(idx, "final_boss")
						d.spawn_mob(idx, 4158, 1012, 1144, 270)
					else
						d.setf(idx, "opened", 1)
						d.spawn_mob(idx, 8475, 948, 1142)
						d.notice(idx, 1194, "", true)
						d.notice(idx, 1195, "", true)
						d.notice(idx, 1197, "", true)
						d.notice(idx, 1196, "", true)
						server_timer("pyramide_zone_spawn", 299, idx)
					end
				end
			end
		end

		when 9335.chat."Pickup the key!" with pc.in_dungeon() begin
			setskin(NOWINDOW)
			local idx = pc.get_map_index()
			if idx >= 3570000 and idx < 3580000 then
				if d.getf(idx, "floor") == 7 then
					if d.getf(idx, "canpick") == 1 then
						d.setf(idx, "canpick", 0)
						npc.purge()
						pc.give_item2(30805, 1)
					else
						syschat(gameforge[pc.get_language()].PyramidDungeon_zone._10)
					end
				end
			end
		end

		when pyramide_zone_check.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				local f = d.getf(arg, "floor")
				if f == 7 then
					if d.count_monster(arg) == 0 then
						clear_server_timer("pyramide_zone_check", arg)
						d.clear_regen(arg)
						d.setf(arg, "canpick", 1)
						d.notice(arg, 1193, "", true)
					end
				end
			else
				pyramide_zone.clear(arg)
			end
		end

		when 4157.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3570000 and idx < 3580000 then
				if d.getf(idx, "floor") == 6 then
					if d.getf(idx, "count") == 8 then
						d.setf(idx, "floor", 7)
						d.setf(idx, "canpick", 0)
						d.setf(idx, "opened", 0)
						d.spawn_mob(idx, 9334, 985, 1162, 270)
						d.spawn_mob(idx, 9334, 985, 1123, 270)
						d.spawn_mob(idx, 9335, 978, 1162, 270)
						d.spawn_mob(idx, 9335, 978, 1123, 270)
						local vid = d.spawn_mob(idx, 9336, 1012, 1144, 270)
						d.set_unique(idx, "final_boss", vid)
						d.regen_file(idx, "data/dungeon/pyramide/regen3.txt")
						server_loop_timer("pyramide_zone_check", 2, idx)
						d.notice(idx, 1190, "", true)
						server_timer("pyramide_zone_warp", 3, idx)
					end
				end
			end
		end

		when 9333.take with pc.in_dungeon() and item.get_vnum() == 30804 begin
			local idx = pc.get_map_index()
			if idx >= 3570000 and idx < 3580000 then
				if d.getf(idx, "floor") == 6 then
					item.remove()
					local c = d.getf(idx, "count")
					if c == 1 then
						if npc.get_vid() == d.getf(idx, "uniue_vid_anu1") then
							d.setf(idx, "count", 2)
							npc.kill()
							d.kill_all_monsters(idx)
							d.clear_regen(idx)
							local pos = {
										[1] = {["x"] = 157, ["y"] = 1042},
										[2] = {["x"] = 195, ["y"] = 1015},
										[3] = {["x"] = 202, ["y"] = 1040},
										[4] = {["x"] = 151, ["y"] = 1019},
										[5] = {["x"] = 163, ["y"] = 1009},
										[6] = {["x"] = 176, ["y"] = 1053}
							}
							
							table_shuffle(pos)
							d.spawn_mob(idx, 8474, pos[1]["x"], pos[1]["y"])
							d.notice(idx, 1184, "", true)
						else
							d.setf(idx, "count", 0)
							d.notice(idx, 1182, "", true)
						end
					elseif c == 3 then
						if npc.get_vid() == d.getf(idx, "uniue_vid_anu2") then
							d.setf(idx, "count", 4)
							npc.kill()
							d.kill_all_monsters(idx)
							d.set_regen_file(idx, "data/dungeon/pyramide/regen2.txt")
							d.notice(idx, 1186, "", true)
						else
							local pos = {
										[1] = {["x"] = 157, ["y"] = 1042},
										[2] = {["x"] = 195, ["y"] = 1015},
										[3] = {["x"] = 202, ["y"] = 1040},
										[4] = {["x"] = 151, ["y"] = 1019},
										[5] = {["x"] = 163, ["y"] = 1009},
										[6] = {["x"] = 176, ["y"] = 1053}
							}
							
							table_shuffle(pos)
							d.spawn_mob(idx, 8474, pos[1]["x"], pos[1]["y"])
							d.notice(idx, 1182, "", true)
							d.setf(idx, "count", 2)
						end
					elseif c == 5 then
						if npc.get_vid() == d.getf(idx, "uniue_vid_anu3") then
							d.setf(idx, "count", 6)
							d.setf(idx, "killed", 0)
							npc.kill()
							d.kill_all_monsters(idx)
							d.clear_regen(idx)
							local pos = {
										[1] = {["x"] = 157, ["y"] = 1042},
										[2] = {["x"] = 195, ["y"] = 1015},
										[3] = {["x"] = 202, ["y"] = 1040},
										[4] = {["x"] = 151, ["y"] = 1019},
										[5] = {["x"] = 163, ["y"] = 1009},
										[6] = {["x"] = 176, ["y"] = 1053}
							}
							
							for index, position in ipairs(pos) do
								d.spawn_mob(idx, 8474, position["x"], position["y"])
							end
							
							d.notice(idx, 1184, "", true)
						else
							d.notice(idx, 1182, "", true)
							d.setf(idx, "count", 4)
						end
					elseif c == 7 then
						d.setf(idx, "count", 8)
						npc.kill()
						d.kill_all_monsters(idx)
						d.spawn_mob(idx, 4157, 177, 1031)
						d.notice(idx, 1188, "", true)
						d.notice(idx, 1189, "", true)
					end
				end
			end
		end

		when 4611.kill or 4612.kill or 4616.kill or 4617.kill or 4618.kill or 4619.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3570000 and idx < 3580000 then
				if d.getf(idx, "floor") == 6 then
					local c = d.getf(idx, "count")
					if c == 0 then
						if number(1, 100) > 97 then -- 3%
							d.setf(idx, "count", 1)
							game.drop_item(30804)
						end
					elseif c == 4 then
						if number(1, 100) > 97 then -- 3%
							d.setf(idx, "count", 5)
							game.drop_item(30804)
						end
					end
				end
			end
		end

		when 4155.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3570000 and idx < 3580000 then
				if d.getf(idx, "floor") == 5 then
					d.setf(idx, "floor", 6)
					clear_server_timer("pyramide_zone_limit", idx)
					local npcpos = {
								[1] = {["x"] = 166, ["y"] = 1044, ["dir"] = 135},
								[2] = {["x"] = 188, ["y"] = 1045, ["dir"] = 225},
								[3] = {["x"] = 188, ["y"] = 1016, ["dir"] = 315},
								[4] = {["x"] = 166, ["y"] = 1015, ["dir"] = 45}
					}
					
					table_shuffle(npcpos)
					for index, position in ipairs(npcpos) do
						local vid = d.spawn_mob(idx, 9333, position["x"], position["y"], position["dir"])
						d.setf(idx, string.format("uniue_vid_anu%d", index), vid)
					end
					
					d.set_regen_file(idx, "data/dungeon/pyramide/regen2.txt")
					d.notice(idx, 1175, "", true)
					server_timer("pyramide_zone_warp", 3, idx)
				end
			end
		end

		when 8473.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3570000 and idx < 3580000 then
				if d.getf(idx, "floor") == 4 then
					if d.getf(idx, "unique_vid") == npc.get_vid() then
						d.setf(idx, "floor", 5)
						d.kill_all_monsters(idx)
						d.clear_regen(idx)
						d.spawn_mob(idx, 4155, 577, 136)
						d.notice(idx, 1171, "", true)
						d.notice(idx, 1172, "", true)
					else
						d.spawn_mob(idx, 4154, pc.get_x() - 8960, pc.get_y() - 22528)
						d.notice(idx, 1173, "", true)
					end
				end
			end
		end

		when 4154.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3570000 and idx < 3580000 then
				if d.getf(idx, "floor") == 3 then
					d.setf(idx, "floor", 4)
					d.setf(idx, "count", 0)
					
					local stonepos = {
								[1] = {["x"] = 587, ["y"] = 108},
								[2] = {["x"] = 564, ["y"] = 108},
								[3] = {["x"] = 564, ["y"] = 128},
								[4] = {["x"] = 587, ["y"] = 128},
								[5] = {["x"] = 564, ["y"] = 148},
								[6] = {["x"] = 587, ["y"] = 148},
								[7] = {["x"] = 564, ["y"] = 168},
								[8] = {["x"] = 587, ["y"] = 168}
					}
					
					local random_num = number(1, table.getn(stonepos))
					for index, position in ipairs(stonepos) do
						local vid = d.spawn_mob(idx, 8473, position["x"], position["y"])
						if random_num == index then
							d.setf(idx, "unique_vid", vid)
						end
					end
					
					d.notice(idx, 1168, "", true)
					server_timer("pyramide_zone_warp", 3, idx)
				end
			end
		end

		when 9332.chat."Decipher!" with pc.in_dungeon() begin
			setskin(NOWINDOW)
			local idx = pc.get_map_index()
			if idx >= 3570000 and idx < 3580000 then
				local f = d.getf(idx, "floor")
				if f == 2 or f == 3 then
					if pc.count_item(30800) > 0 and pc.count_item(30801) > 0 and pc.count_item(30802) > 0 and pc.count_item(30803) > 0 then
						npc.kill()
						pc.remove_item(30800, pc.count_item(30800))
						pc.remove_item(30801, pc.count_item(30801))
						pc.remove_item(30802, pc.count_item(30802))
						pc.remove_item(30803, pc.count_item(30803))
						d.spawn_mob(idx, 4154, 306, 126)
						d.notice(idx, 1167, "", true)
					else
						syschat(gameforge[pc.get_language()].PyramidDungeon_zone._9)
					end
				end
			end
		end

		when 4601.kill or 4602.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3570000 and idx < 3580000 then
				if d.getf(idx, "floor") == 2 then
					if number(1, 100) > 96 then -- 4%
						local c = d.getf(idx, "count") + 1
						d.setf(idx, "count", c)
						if c == 1 then
							game.drop_item(30800)
						elseif c == 2 then
							game.drop_item(30801)
						elseif c == 3 then
							game.drop_item(30802)
						elseif c == 4 then
							d.setf(idx, "floor", 3)
							game.drop_item(30803)
							d.kill_all_monsters(idx)
							d.clear_regen(idx)
						end
					end
				end
			end
		end

		when pyramide_zone_warp.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				clear_server_timer("pyramide_zone_warp", arg)
				local f = d.getf(arg, "floor")
				if f == 2 then
					d.jump_all(arg, 9266, 22621)
					server_timer("pyramide_zone_limit", 1199, arg)
					d.notice(arg, 1164, "", true)
					d.notice(arg, 1165, "", true)
				elseif f == 4 then
					d.jump_all(arg, 9536, 22608)
					d.notice(arg, 1169, "", true)
					d.notice(arg, 1170, "", true)
				elseif f == 6 then
					d.jump_all(arg, 9137, 23558)
					d.notice(arg, 1181, "", true)
					d.notice(arg, 1182, "", true)
				elseif f == 7 then
					d.jump_all(arg, 9759, 23655)
					d.notice(arg, 1191, "", true)
					d.notice(arg, 1192, "", true)
				end
			else
				pyramide_zone.clear(arg)
			end
		end

		when pyramide_zone_limit.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.notice(arg, 1040, "", true)
				d.notice(arg, 1041, "", true)
			end
			
			pyramide_zone.clear(arg)
		end

		when pyramide_zone_prepare.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.spawn_mob(arg, 8472, 91, 130)
				server_timer("pyramide_zone_end", 1799, arg)
				server_timer("pyramide_zone_limit", 239, arg)
				d.notice(arg, 1128, string.format("%d", 30), true)
				d.notice(arg, 1160, "", true)
				d.notice(arg, 1161, "", true)
			else
				pyramide_zone.clear(arg)
			end
		end

		when logout begin
			local idx = pc.get_map_index()
			if idx >= 3570000 and idx < 3580000 then
				pc.setf("pyramide_zone", "disconnect", get_global_time() + 300)
			end
		end

		when login begin
			local idx = pc.get_map_index()
			if idx == 357 then
				pc.warp(536900, 1331400)
			elseif idx >= 3570000 and idx < 3580000 then
				pc.set_warp_location(215, 5369, 13314)
				pc.setf("pyramide_zone", "idx", idx)
				pc.setf("pyramide_zone", "ch", pc.get_channel_id())
				if d.getf(idx, "floor") == 0 then
					if not party.is_party() then
						d.setf(idx, "floor", 1)
						server_timer("pyramide_zone_prepare", 1, idx)
						d.setf(idx, "was_completed", 0)
					else
						if party.is_leader() then
							d.setf(idx, "floor", 1)
							server_timer("pyramide_zone_prepare", 1, idx)
							d.setf(idx, "was_completed", 0)
						end
					end
				end
			end
		end

		when 9331.chat."A new territory?!?" begin
			local lang = pc.get_language()
			
			say_title(string.format("%s:", mob_name(9331)))
			say("")
			say(string.format(gameforge[lang].common.dungeon_1, pc.get_name()))
			local mapIdx = pc.get_map_index()
			if mapIdx != 215 then
				return
			end
			
			say(gameforge[lang].common.dungeon_2)
			local agree = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			say_title(string.format("%s:", mob_name(9331)))
			say("")
			if agree == 2 then
				say(gameforge[lang].common.dungeon_3)
				return
			end
			
			local goahead = 1
			local rejoinTIME = pc.getf("pyramide_zone", "disconnect") - get_global_time()
			if rejoinTIME > 0 then
				local rejoinIDX = pc.getf("pyramide_zone", "idx")
				if rejoinIDX > 0 then
					local rejoinCH = pc.getf("pyramide_zone", "ch")
					if rejoinCH != 0 and rejoinCH != pc.get_channel_id() then
						say(string.format(gameforge[lang].common.dungeon_26, rejoinCH))
						return
					end
					
					if rejoinCH != 0 and d.find(rejoinIDX) then
						if d.getf(rejoinIDX, "was_completed") == 0 then
							say(gameforge[lang].common.dungeon_4)
							local agree2 = select(gameforge[lang].common.yes, gameforge[lang].common.no)
							if agree2 == 2 then
								say_title(string.format("%s:", mob_name(9331)))
								say("")
								say(gameforge[lang].common.dungeon_3)
								return
							end
							
							local f = d.getf(rejoinIDX, "floor")
							if f == 1 then
								goahead = 0
								pc.warp(905100, 2261700, rejoinIDX)
							elseif f == 2 or f == 3 then
								goahead = 0
								pc.warp(926600, 2262100, rejoinIDX)
							elseif f == 4 or f == 5 then
								goahead = 0
								pc.warp(953600, 2260800, rejoinIDX)
							elseif f == 6 then
								goahead = 0
								pc.warp(913700, 2355800, rejoinIDX)
							elseif f == 7 then
								goahead = 0
								pc.warp(975900, 2365500, rejoinIDX)
							end
						end
					end
				end
			end
			
			if goahead == 1 then
				pc.setf("pyramide_zone", "disconnect", 0)
				pc.setf("pyramide_zone", "idx", 0)
				pc.setf("pyramide_zone", "ch", 0)
				
				say(gameforge[lang].common.dungeon_5)
				say_reward(string.format(gameforge[lang].common.dungeon_6, 105))
				say_reward(string.format(gameforge[lang].common.dungeon_7, 120))
				say_reward(string.format("- %s: %d", item_name(30798), 1))
				if party.is_party() then
					say_reward(gameforge[lang].common.dungeon_8)
				end
				say(gameforge[lang].common.dungeon_9)
				local n = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				say_title(string.format("%s:", mob_name(9331)))
				say("")
				if n == 2 then
					say(gameforge[lang].common.dungeon_3)
					return
				end
				
				local flag = string.format("ww_357_%d", pc.get_channel_id())
				local ww_flag = game.get_event_flag(flag) - get_global_time()
				if ww_flag > 0 then
					say(gameforge[lang].common.dungeon_28)
					say(string.format(gameforge[lang].common.dungeon_29, ww_flag))
					return
				end
				
				myname = pc.get_name()
				result, cooldown, name = d.check_entrance(105, 120, 30798, 0, 1, "pyramide_zone.cooldown")
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
						say(string.format(gameforge[lang].common.dungeon_18, 1, item_name(30798)))
					else
						say(string.format(gameforge[lang].common.dungeon_17, name, 1, item_name(30798)))
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
					d.remove_item(30798, 1, 0, 0, 30800, 255, 30801, 255, 30802, 255, 30803, 255, 30804, 255, 30805, 255, 3600, "pyramide_zone")
					game.set_event_flag(string.format("ww_357_%d", pc.get_channel_id()), get_global_time() + 5)
					d.join_cords(357, 9051, 22617)
				end
			end
		end
	end
end

