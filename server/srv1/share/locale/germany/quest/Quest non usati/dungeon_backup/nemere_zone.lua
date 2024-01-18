quest nemere_zone begin
	state start begin
		function clear(arg)
			clear_server_timer("nemere_zone_prepare", arg)
			clear_server_timer("nemere_zone_warp", arg)
			clear_server_timer("nemere_zone_check", arg)
			clear_server_timer("nemere_zone_end", arg)
			clear_server_timer("nemere_zone_complete", arg)
			if d.find(arg) then
				d.setf(arg, "was_completed", 1)
				d.kill_all(arg)
				d.clear_regen(arg)
				d.exit_all_lobby(arg, 1)
			end
		end

		when nemere_zone_complete.server_timer begin
			nemere_zone.clear(get_server_timer_arg())
		end

		when 6191.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3520000 and idx < 3530000 then
				if d.getf(idx, "floor") == 9 then
					d.setf(idx, "was_completed", 1)
					d.complete(6191, 3600, 352, "nemere_zone")
					if party.is_party() then
						notice_all(919, party.get_leader_name())
					else
						notice_all(920, pc.get_name())
					end
					
					d.notice(idx, 1258, "", true)
					d.notice(idx, 1261, "", true)
					d.kill_all(idx)
					d.clear_regen(idx)
					local bonus = 10 + game.get_event_flag("dungeon_bonus")
					if math.random(1, 100) <= bonus then
						d.spawn_mob(idx, 5002, 927, 335)
					end
					server_timer("nemere_zone_complete", 30, idx)
				end
			end
		end

		when nemere_zone_end.server_timer begin
			local arg = get_server_timer_arg()
			d.notice(arg, 1040, "", true)
			d.notice(arg, 1041, "", true)
			nemere_zone.clear(arg)
		end

		when 20398.take with pc.in_dungeon() and item.get_vnum() == 30332 begin
			local idx = pc.get_map_index()
			if idx >= 3520000 and idx < 3530000 then
				if d.getf(idx, "floor") == 3 then
					npc.purge()
					item.remove()
					local c = d.getf(idx, "opened") + 1
					if c == 5 then
						d.setf(idx, "floor", 4)
						pc.remove_item(30332, pc.count_item(30332))
						
						d.kill_all(idx)
						d.clear_regen(idx)
						d.regen_file(idx, "data/dungeon/nemere/regen3.txt")
						d.notice(idx, 1106, "", true)
						d.notice(idx, 1097, "", true)
						server_timer("nemere_zone_warp", 2, idx)
					else
						d.setf(idx, "opened", c)
						d.notice(idx, 1104, "", true)
					end
				end
			end
		end

		when 6104.kill or 6105.kill or 6106.kill or 6107.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3520000 and idx < 3530000 then
				if d.getf(idx, "floor") == 3 then
					if number(1, 100) > 98 then -- 2%
						game.drop_item(30332, 1)
					end
				end
			end
		end

		when 6151.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3520000 and idx < 3530000 then
				if d.getf(idx, "floor") == 6 then
					if d.getf(idx, "unique_vid") == npc.get_vid() then
						d.setf(idx, "floor", 7)
						d.clear_regen(idx)
						d.kill_all(idx)
						d.spawn_mob(idx, 20399, 850, 655)
						d.regen_file(idx, "data/dungeon/nemere/regen5.txt")
						d.notice(idx, 1112, "", true)
						d.notice(idx, 1097, "", true)
						server_timer("nemere_zone_warp", 2, idx)
					else
						d.notice(idx, 1259, "", true)
					end
				end
			end
		end

		when 8058.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3520000 and idx < 3530000 then
				if d.getf(idx, "floor") == 5 then
					d.setf(idx, "floor", 6)
					d.set_regen_file(idx, "data/dungeon/nemere/regen4.txt")
					
					local bosspos = {
								[1] = {["x"] = 746, ["y"] = 490},
								[2] = {["x"] = 765, ["y"] = 478},
								[3] = {["x"] = 763, ["y"] = 449},
								[4] = {["x"] = 737, ["y"] = 446},
								[5] = {["x"] = 725, ["y"] = 465}
					}
					
					local random_num = number(1, table.getn(bosspos))
					for index, position in ipairs(bosspos) do
						local vid = d.spawn_mob(idx, 6151, position["x"], position["y"])
						if random_num == index then
							d.setf(idx, "unique_vid", vid)
						end
					end
					
					d.notice(idx, 1108, "", true)
					d.notice(idx, 1097, "", true)
					server_timer("nemere_zone_warp", 2, idx)
				end
			end
		end

		when nemere_zone_check.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				local f = d.getf(arg, "floor")
				if f == 4 then
					if d.count_monster(arg) == 0 then
						clear_server_timer("nemere_zone_check", arg)
						d.setf(arg, "floor", 5)
						d.clear_regen(arg)
						d.spawn_mob(arg, 8058, 570, 649)
						d.notice(arg, 1090, "", true)
						d.notice(arg, 1107, "", true)
						server_timer("nemere_zone_warp", 2, arg)
					end
				end
			else
				nemere_zone.clear(arg)
			end
		end

		when 20397.chat."Hey!" with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3520000 and idx < 3530000 then
				if d.getf(idx, "floor") == 8 then
					local lang = pc.get_language()
					say_title(string.format("%s:", mob_name(20397)))
					say("")
					if party.is_party() and not party.is_leader() then
						say(gameforge[lang].common.dungeon_24)
						return
					end
					
					say(gameforge[lang].common.dungeon_23)
					local agree = select(gameforge[lang].common.yes, gameforge[lang].common.no)
					if agree == 2 then
						return
					end
					
					d.setf(idx, "floor", 9)
					d.spawn_mob(idx, 6191, 927, 335)
					d.regen_file(idx, "data/dungeon/nemere/regen6.txt")
					d.notice(idx, 1097, "", true)
					server_timer("nemere_zone_warp", 2, idx)
				else
					setskin(NOWINDOW)
				end
			end
		end

		when 20399.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3520000 and idx < 3530000 then
				if d.getf(idx, "floor") == 7 then
					d.setf(idx, "floor", 8)
					d.kill_all(idx)
					d.clear_regen(idx)
					d.spawn_mob(idx, 20397, 850, 635)
					d.notice(idx, 1116, "", true)
					if party.is_party() then
						d.notice(idx, 1117, "", true)
					else
						d.notice(idx, 1118, "", true)
					end
				end
			end
		end

		when nemere_zone_warp.server_timer begin
			local arg = get_server_timer_arg()
			clear_server_timer("nemere_zone_warp", arg)
			if d.find(arg) then
				local f = d.getf(arg, "floor")
				if f == 3 then
					d.jump_all(arg, 5540, 2070)
					d.notice(arg, 1098, "", true)
					d.notice(arg, 1099, "5", true)
				elseif f == 4 then
					d.jump_all(arg, 5691, 2230)
					server_loop_timer("nemere_zone_check", 2, arg)
					d.notice(arg, 1089, "", true)
				elseif f == 6 then
					d.jump_all(arg, 5866, 2068)
					d.notice(arg, 1109, "", true)
				elseif f == 7 then
					d.jump_all(arg, 5969, 2225)
					d.notice(arg, 1115, "", true)
				elseif f == 8 then
					d.jump_all(arg, 5969, 2225)
					d.notice(arg, 1115, "", true)
				elseif f == 9 then
					d.jump_all(arg, 6047, 1926)
					d.notice(arg, 1119, "", true)
				end
			else
				nemere_zone.clear(arg)
			end
		end

		when 30331.use with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3520000 and idx < 3530000 then
				if d.getf(idx, "floor") == 2 then
					d.setf(idx, "floor", 3)
					d.setf(idx, "opened", 0)
					pc.remove_item(30331, pc.count_item(30331))
					d.set_regen_file(idx, "data/dungeon/nemere/regen2.txt")
					d.spawn_mob(idx, 20398, 419, 493)
					d.spawn_mob(idx, 20398, 449, 475)
					d.spawn_mob(idx, 20398, 438, 433)
					d.spawn_mob(idx, 20398, 405, 433)
					d.spawn_mob(idx, 20398, 388, 470)
					d.notice(idx, 1094, "", true)
					d.notice(idx, 1097, "", true)
					server_timer("nemere_zone_warp", 2, idx)
				end
			end
		end

		when 6101.kill or 6102.kill or 6103.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 3520000 and idx < 3530000 then
				if d.getf(idx, "floor") == 1 then
					if number(1, 100) > 99 then -- 1%
						d.setf(idx, "floor", 2)
						d.kill_all(idx)
						d.clear_regen(idx)
						game.drop_item(30331, 1)
					end
				end
			end
		end

		when nemere_zone_prepare.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.set_regen_file(arg, "data/dungeon/nemere/regen1.txt")
				server_timer("nemere_zone_end", 2999, arg)
				d.notice(arg, 1128, "50", true)
				d.notice(arg, 1092, "", true)
			else
				nemere_zone.clear(arg)
			end
		end

		when logout begin
			local idx = pc.get_map_index()
			if idx >= 3520000 and idx < 3530000 then
				pc.setf("nemere_zone", "disconnect", get_global_time() + 300)
			end
		end

		when login begin
			local idx = pc.get_map_index()
			if idx == 352 then
				pc.warp(535400, 1428400)
			elseif idx >= 3520000 and idx < 3530000 then
				pc.set_warp_location(219, 5354, 14284)
				pc.setf("nemere_zone", "idx", idx)
				pc.setf("nemere_zone", "ch", pc.get_channel_id())
				if d.getf(idx, "floor") == 0 then
					if not party.is_party() then
						d.setf(idx, "floor", 1)
						server_timer("nemere_zone_prepare", 1, idx)
						d.setf(idx, "was_completed", 0)
					else
						if party.is_leader() then
							d.setf(idx, "floor", 1)
							server_timer("nemere_zone_prepare", 1, idx)
							d.setf(idx, "was_completed", 0)
						end
					end
				end
			end
		end

		when 20395.chat."A new territory?!?" begin
			local lang = pc.get_language()
			
			say_title(string.format("%s:", mob_name(20395)))
			say("")
			say(string.format(gameforge[lang].common.dungeon_1, pc.get_name()))
			local mapIdx = pc.get_map_index()
			if mapIdx != 219 then
				return
			end
			
			say(gameforge[lang].common.dungeon_2)
			local agree = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			say_title(string.format("%s:", mob_name(20395)))
			say("")
			if agree == 2 then
				say(gameforge[lang].common.dungeon_3)
				return
			end
			
			local goahead = 1
			local rejoinTIME = pc.getf("nemere_zone", "disconnect") - get_global_time()
			if rejoinTIME > 0 then
				local rejoinIDX = pc.getf("nemere_zone", "idx")
				if rejoinIDX > 0 then
					local rejoinCH = pc.getf("nemere_zone", "ch")
					if rejoinCH != 0 and rejoinCH != pc.get_channel_id() then
						say(string.format(gameforge[lang].common.dungeon_26, rejoinCH))
						return
					end
					
					if rejoinCH != 0 and d.find(rejoinIDX) then
						if d.getf(rejoinIDX, "was_completed") == 0 then
							say(gameforge[lang].common.dungeon_4)
							local agree2 = select(gameforge[lang].common.yes, gameforge[lang].common.no)
							if agree2 == 2 then
								say_title(string.format("%s:", mob_name(20395)))
								say("")
								say(gameforge[lang].common.dungeon_3)
								return
							end
							
							local f = d.getf(rejoinIDX, "floor")
							if f == 1 or f == 2 then
								goahead = 0
								pc.warp(588100, 180400, rejoinIDX)
							elseif f == 3 then
								goahead = 0
								pc.warp(554000, 207000, rejoinIDX)
							elseif f == 4 or f == 5 then
								goahead = 0
								pc.warp(569100, 223000, rejoinIDX)
							elseif f == 6 then
								goahead = 0
								pc.warp(586600, 206800, rejoinIDX)
							elseif f == 7 or f == 8 then
								goahead = 0
								pc.warp(596900, 222500, rejoinIDX)
							elseif f == 9 then
								goahead = 0
								pc.warp(604700, 192600, rejoinIDX)
							end
						end
					end
				end
			end
			
			if goahead == 1 then
				pc.setf("nemere_zone", "disconnect", 0)
				pc.setf("nemere_zone", "idx", 0)
				pc.setf("nemere_zone", "ch", 0)
				
				say(gameforge[lang].common.dungeon_5)
				say_reward(string.format(gameforge[lang].common.dungeon_6, 85))
				say_reward(string.format(gameforge[lang].common.dungeon_7, 105))
				say_reward(string.format("- %s: %d", item_name(71175), 1))
				if party.is_party() then
					say_reward(gameforge[lang].common.dungeon_8)
				end
				say(gameforge[lang].common.dungeon_9)
				local n = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				say_title(string.format("%s:", mob_name(20395)))
				say("")
				if n == 2 then
					say(gameforge[lang].common.dungeon_3)
					return
				end
				
				local flag = string.format("ww_352_%d", pc.get_channel_id())
				local ww_flag = game.get_event_flag(flag) - get_global_time()
				if ww_flag > 0 then
					say(gameforge[lang].common.dungeon_28)
					say(string.format(gameforge[lang].common.dungeon_29, ww_flag))
					return
				end
				
				myname = pc.get_name()
				result, cooldown, name = d.check_entrance(85, 105, 71175, 0, 1, "nemere_zone.cooldown")
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
						say(string.format(gameforge[lang].common.dungeon_18, 1, item_name(71175)))
					else
						say(string.format(gameforge[lang].common.dungeon_17, name, 1, item_name(71175)))
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
					d.remove_item(71175, 1, 0, 0, 30331, 255, 30332, 255, 0, 0, 0, 0, 0, 0, 0, 0, 3600, "nemere_zone")
					game.set_event_flag(string.format("ww_352_%d", pc.get_channel_id()), get_global_time() + 5)
					d.join_cords(352, 5881, 1804)
				end
			end
		end
	end
end

