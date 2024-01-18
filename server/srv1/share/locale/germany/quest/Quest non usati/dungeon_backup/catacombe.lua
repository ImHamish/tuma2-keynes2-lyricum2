quest catacombe begin
	state start begin
		function clear(arg)
			clear_server_timer("catacombe_prepare", arg)
			clear_server_timer("catacombe_warp", arg)
			clear_server_timer("catacombe_checker", arg)
			clear_server_timer("catacombe_end", arg)
			clear_server_timer("catacombe_complete", arg)
			if d.find(arg) then
				d.setf(arg, "was_completed", 1)
				d.kill_all(arg)
				d.clear_regen(arg)
				d.exit_all_lobby(arg, 1)
			end
		end

		when catacombe_complete.server_timer begin
			catacombe.clear(get_server_timer_arg())
		end

		when 30102.take with pc.in_dungeon() and item.get_vnum() == 30312 begin
			local idx = pc.get_map_index()
			if idx >= 2160000 and idx < 2170000 then
				if d.getf(idx, "floor") == 2 then
					d.setf(idx, "floor", 3)
					npc.purge()
					pc.remove_item(30312, pc.count_item(30312))
					d.clear_regen(idx)
					d.kill_all(idx)
					d.spawn_mob(idx, 8038, 1270, 712)
					d.spawn_mob(idx, 8038, 1318, 737)
					d.spawn_mob(idx, 8038, 1317, 665)
					d.regen_file(idx, "data/dungeon/devilcatacomb/regen3.txt")
					d.notice(idx, 1003, "", true)
					server_timer("catacombe_warp", 2, idx)
				end
			end
		end

		when 2591.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 2160000 and idx < 2170000 then
				if d.getf(idx, "floor") == 2 then
					if d.getf(idx, "unique_vid") == npc.get_vid() then
						d.setf(idx, "dropped", 1)
						game.drop_item(30312, 1)
						d.notice(idx, 1000, "", true)
						d.notice(idx, 1001, "", true)
					else
						d.notice(idx, 1002, "", true)
					end
				end
			end
		end

		when 2597.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 2160000 and idx < 2170000 then
				if d.getf(idx, "floor") == 3 then
					d.setf(idx, "floor", 4)
					d.spawn_mob(idx, 2598, 74, 1110)
					d.set_regen_file(idx, "data/dungeon/devilcatacomb/regen4.txt")
					d.notice(idx, 1005, "", true)
					d.notice(idx, 1006, "", true)
					server_timer("catacombe_warp", 2, idx)
				end
			end
		end

		when catacombe_checker.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				local f = d.getf(arg, "floor")
				if f == 3 then
					if d.count_monster(arg) == 0 then
						clear_server_timer("catacombe_checker", arg)
						d.spawn_mob(arg, 2597, 1300, 710)
						d.notice(arg, 1004, "", true)
					end
				end
			else
				catacombe.clear(arg)
			end
		end

		when 2598.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 2160000 and idx < 2170000 then
				if d.getf(idx, "floor") == 4 then
					d.setf(idx, "was_completed", 1)
					d.complete(2598, 3600, 216, "catacombe")
					if party.is_party() then
						notice_all(933, party.get_leader_name())
					else
						notice_all(1123, pc.get_name())
					end
					
					d.notice(idx, 1008, "", true)
					d.notice(idx, 1009, "", true)
					d.kill_all(idx)
					d.clear_regen(idx)
					local bonus = 10 + game.get_event_flag("dungeon_bonus")
					if math.random(1, 100) <= bonus then
						d.spawn_mob(idx, 5002, 74, 1110)
					end
					server_timer("catacombe_complete", 30, idx)
				end
			end
		end

		when catacombe_warp.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				clear_server_timer("catacombe_warp", arg)
				local f = d.getf(arg, "floor")
				if f == 2 then
					d.jump_all(arg, 3917, 12932)
					d.notice(arg, 999, "", true)
				elseif f == 3 then
					d.jump_all(arg, 4434, 12698)
					d.notice(arg, 1089, "", true)
					server_loop_timer("catacombe_checker", 2, arg)
				elseif f == 4 then
					d.jump_all(arg, 3147, 13187)
					d.notice(arg, 1007, "", true)
				end
			else
				catacombe.clear(arg)
			end
		end

		when 8038.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 2160000 and idx < 2170000 then
				if d.getf(idx, "floor") == 1 then
					if d.getf(idx, "unique_vid") == npc.get_vid() then
						d.setf(idx, "floor", 2)
						d.clear_regen(idx)
						d.kill_all(idx)
						d.spawn_mob(idx, 30102, 848, 740)
						
						local stonepos = {
									[1] = {["x"] = 851, ["y"] = 597},
									[2] = {["x"] = 705, ["y"] = 818},
									[3] = {["x"] = 717, ["y"] = 655},
									[4] = {["x"] = 981, ["y"] = 663},
									[5] = {["x"] = 984, ["y"] = 827}
						}
						
						local random_num = number(1, table.getn(stonepos))
						for index, position in ipairs(stonepos) do
							local vid = d.spawn_mob(idx, 2591, position["x"], position["y"])
							if random_num == index then
								d.setf(idx, "unique_vid", vid)
							end
						end
						
						d.set_regen_file(idx, "data/dungeon/devilcatacomb/regen2.txt")
						d.notice(idx, 1015, "", true)
						server_timer("catacombe_warp", 2, idx)
					else
						d.notice(idx, 1016, "", true)
					end
				end
			end
		end

		when catacombe_end.server_timer begin
			local arg = get_server_timer_arg()
			d.notice(arg, 1040, "", true)
			d.notice(arg, 1041, "", true)
			catacombe.clear(arg)
		end

		when catacombe_prepare.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				local stonepos = {
							[1] = {["x"] = 1340, ["y"] = 348},
							[2] = {["x"] = 1243, ["y"] = 357},
							[3] = {["x"] = 1334, ["y"] = 151},
							[4] = {["x"] = 1252, ["y"] = 148},
							[5] = {["x"] = 1149, ["y"] = 148},
							[6] = {["x"] = 1139, ["y"] = 244},
							[7] = {["x"] = 1148, ["y"] = 355}
				}
				
				local random_num = number(1, table.getn(stonepos))
				for index, position in ipairs(stonepos) do
					local vid = d.spawn_mob(arg, 8038, position["x"], position["y"])
					if random_num == index then
						d.setf(arg, "unique_vid", vid)
					end
				end
				
				d.set_regen_file(arg, "data/dungeon/devilcatacomb/regen1.txt")
				server_timer("catacombe_end", 2399, arg)
				d.notice(arg, 1128, string.format("%d", 40), true)
				d.notice(arg, 995, "", true)
			else
				catacombe.clear(arg)
			end
		end

		when logout begin
			local idx = pc.get_map_index()
			if idx >= 2160000 and idx < 2170000 then
				pc.setf("catacombe", "disconnect", get_global_time() + 300)
			end
		end

		when login begin
			local idx = pc.get_map_index()
			if idx == 216 then
				pc.warp(535400, 1428400)
			elseif idx >= 2160000 and idx < 2170000 then
				pc.set_warp_location(219, 5354, 14284)
				pc.setf("catacombe", "idx", idx)
				pc.setf("catacombe", "ch", pc.get_channel_id())
				if d.getf(idx, "floor") == 0 then
					if not party.is_party() then
						d.setf(idx, "floor", 1)
						server_timer("catacombe_prepare", 1, idx)
						d.setf(idx, "was_completed", 0)
					else
						if party.is_leader() then
							d.setf(idx, "floor", 1)
							server_timer("catacombe_prepare", 1, idx)
							d.setf(idx, "was_completed", 0)
						end
					end
				end
			end
		end

		when 20367.chat."A new territory?!?" begin
			local lang = pc.get_language()
			
			say_title(string.format("%s:", mob_name(20367)))
			say("")
			say(string.format(gameforge[lang].common.dungeon_1, pc.get_name()))
			local mapIdx = pc.get_map_index()
			if mapIdx != 219 then
				return
			end
			
			say(gameforge[lang].common.dungeon_2)
			local agree = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			say_title(string.format("%s:", mob_name(20367)))
			say("")
			if agree == 2 then
				say(gameforge[lang].common.dungeon_3)
				return
			end
			
			local goahead = 1
			local rejoinTIME = pc.getf("catacombe", "disconnect") - get_global_time()
			if rejoinTIME > 0 then
				local rejoinIDX = pc.getf("catacombe", "idx")
				if rejoinIDX > 0 then
					local rejoinCH = pc.getf("catacombe", "ch")
					if rejoinCH != 0 and rejoinCH != pc.get_channel_id() then
						say(string.format(gameforge[lang].common.dungeon_26, rejoinCH))
						return
					end
					
					if rejoinCH != 0 and d.find(rejoinIDX) then
						if d.getf(rejoinIDX, "was_completed") == 0 then
							say(gameforge[lang].common.dungeon_4)
							local agree2 = select(gameforge[lang].common.yes, gameforge[lang].common.no)
							if agree2 == 2 then
								say_title(string.format("%s:", mob_name(20367)))
								say("")
								say(gameforge[lang].common.dungeon_3)
								return
							end
							
							local f = d.getf(rejoinIDX, "floor")
							if f == 1 then
								goahead = 0
								pc.warp(445000, 1228200, rejoinIDX)
							elseif f == 2 then
								goahead = 0
								pc.warp(391700, 1293200, rejoinIDX)
							elseif f == 3 then
								goahead = 0
								pc.warp(443400, 1269800, rejoinIDX)
							elseif f == 4 then
								goahead = 0
								pc.warp(314700, 1318700, rejoinIDX)
							end
						end
					end
				end
			end
			
			if goahead == 1 then
				pc.setf("catacombe", "disconnect", 0)
				pc.setf("catacombe", "idx", 0)
				pc.setf("catacombe", "ch", 0)
				
				say(gameforge[lang].common.dungeon_5)
				say_reward(string.format(gameforge[lang].common.dungeon_6, 75))
				say_reward(string.format(gameforge[lang].common.dungeon_7, 100))
				say_reward(string.format("- %s: %d", item_name(30320), 1))
				if party.is_party() then
					say_reward(gameforge[lang].common.dungeon_8)
				end
				say(gameforge[lang].common.dungeon_9)
				local n = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				say_title(string.format("%s:", mob_name(20367)))
				say("")
				if n == 2 then
					say(gameforge[lang].common.dungeon_3)
					return
				end
				
				local flag = string.format("ww_216_%d", pc.get_channel_id())
				local ww_flag = game.get_event_flag(flag) - get_global_time()
				if ww_flag > 0 then
					say(gameforge[lang].common.dungeon_28)
					say(string.format(gameforge[lang].common.dungeon_29, ww_flag))
					return
				end
				
				myname = pc.get_name()
				result, cooldown, name = d.check_entrance(75, 100, 30320, 30321, 1, "catacombe.cooldown")
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
						say(string.format(gameforge[lang].common.dungeon_18, 1, item_name(30320)))
					else
						say(string.format(gameforge[lang].common.dungeon_17, name, 1, item_name(30320)))
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
					d.remove_item(30320, 1, 30321, 1, 30312, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3600, "catacombe")
					game.set_event_flag(string.format("ww_216_%d", pc.get_channel_id()), get_global_time() + 5)
					d.join_cords(216, 4450, 12282)
				end
			end
		end
	end
end

