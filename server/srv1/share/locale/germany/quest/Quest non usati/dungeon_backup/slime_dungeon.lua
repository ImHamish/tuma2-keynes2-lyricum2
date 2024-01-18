quest slime_dungeon begin
	state start begin
		function clear(arg)
			clear_server_timer("slime_prepare", arg)
			clear_server_timer("slime_checker", arg)
			clear_server_timer("slime_floor2", arg)
			clear_server_timer("slime_floor3", arg)
			clear_server_timer("slime_floor4", arg)
			clear_server_timer("slime_floor7", arg)
			clear_server_timer("slime_floor8", arg)
			clear_server_timer("slime_end", arg)
			clear_server_timer("slime_complete", arg)
			if d.find(arg) then
				d.setf(arg, "was_completed", 1)
				d.kill_all(arg)
				d.clear_regen(arg)
				d.exit_all_lobby(arg, 1)
			end
		end

		when slime_complete.server_timer begin
			slime_dungeon.clear(get_server_timer_arg())
		end

		when slime_floor4.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.regen_file(arg, "data/dungeon/slime_cave/regen_3.txt")
			else
				slime_dungeon.clear(arg)
			end
		end

		when slime_floor7.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.spawn_mob(arg, 767, 285, 260)
				d.regen_file(arg, "data/dungeon/slime_cave/regen_1.txt")
				server_loop_timer("slime_checker", 2, arg)
			else
				slime_dungeon.clear(arg)
			end
		end

		when 9237.take with pc.in_dungeon() and item.get_vnum() == 30624 begin
			local idx = pc.get_map_index()
			if idx >= 270000 and idx < 280000 then
				if d.getf(idx, "floor") == 6 then
					d.setf(idx, "floor", 7)
					pc.remove_item(30624, pc.count_item(30624))
					npc.kill()
					d.notice(idx, 1145, "", true)
					d.notice(idx, 1139, "", true)
					d.notice(idx, 1140, "", true)
					server_timer("slime_floor7", 2, idx)
				end
			end
		end

		when 9238.take with pc.in_dungeon() and item.get_vnum() == 30623 begin
			local idx = pc.get_map_index()
			if idx >= 270000 and idx < 280000 then
				if d.getf(idx, "floor") == 5 then
					d.setf(idx, "floor", 6)
					pc.remove_item(30623, pc.count_item(30623))
					pc.give_item2(30624, 1)
					npc.purge()
					d.notice(idx, 1144, "", true)
				end
			end
		end

		when 9237.chat."What?" with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 270000 and idx < 280000 then
				if d.getf(idx, "floor") == 3 then
					local lang = pc.get_language()
					
					say_title(string.format("%s:", mob_name(9237)))
					say("")
					say(gameforge[lang].slime_dungeon._1)
					say(gameforge[lang].slime_dungeon._2)
					say(gameforge[lang].slime_dungeon._3)
					say(gameforge[lang].slime_dungeon._4)
					d.setf(idx, "floor", 4)
					d.setf(idx, "count", 4)
					d.notice(idx, 1149, "", true)
					d.notice(idx, 1150, "", true)
					server_timer("slime_floor4", 2, idx)
				else
					setskin(NOWINDOW)
				end
			else
				setskin(NOWINDOW)
			end
		end

		when slime_floor3.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.regen_file(arg, "data/dungeon/slime_cave/regen_2.txt")
			else
				slime_dungeon.clear(arg)
			end
		end

		when 768.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 270000 and idx < 280000 then
				if d.getf(idx, "floor") == 8 then
					d.setf(idx, "was_completed", 1)
					d.complete(768, 2400, 27, "slime_dungeon")
					if party.is_party() then
						notice_all(928, party.get_leader_name())
					else
						notice_all(929, pc.get_name())
					end
					
					d.notice(idx, 1054, "", true)
					d.notice(idx, 1055, "", true)
					d.kill_all(idx)
					d.clear_regen(idx)
					local bonus = 10 + game.get_event_flag("dungeon_bonus")
					if math.random(1, 100) <= bonus then
						d.spawn_mob(idx, 5001, 285, 260)
					end
					server_timer("slime_complete", 30, idx)
				end
			end
		end

		when slime_floor8.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.spawn_mob(arg, 768, 285, 260)
			else
				slime_dungeon.clear(arg)
			end
		end

		when slime_floor2.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.spawn_mob(arg, 767, 285, 260)
				d.regen_file(arg, "data/dungeon/slime_cave/regen_1.txt")
				server_loop_timer("slime_checker", 2, arg)
			else
				slime_dungeon.clear(arg)
			end
		end

		when slime_checker.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				local f = d.getf(arg, "floor")
				if f == 2 then
					if d.count_monster(arg) == 0 then
						clear_server_timer("slime_checker", arg)
						d.setf(arg, "floor", 3)
						d.notice(arg, 1151, "", true)
						d.notice(arg, 1152, "", true)
						d.notice(arg, 1153, "", true)
						server_timer("slime_floor3", 2, arg)
					end
				elseif f == 7 then
					if d.count_monster(arg) == 0 then
						clear_server_timer("slime_checker", arg)
						d.setf(arg, "floor", 8)
						d.notice(arg, 1154, "", true)
						d.notice(arg, 1155, "", true)
						server_timer("slime_floor8", 2, arg)
					end
				end
			else
				slime_dungeon.clear(arg)
			end
		end

		when 8430.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 270000 and idx < 280000 then
				if d.getf(idx, "floor") == 1 then
					d.setf(idx, "floor", 2)
					d.notice(idx, 1138, "", true)
					d.notice(idx, 1139, "", true)
					d.notice(idx, 1140, "", true)
					server_timer("slime_floor2", 2, idx)
				elseif d.getf(idx, "floor") == 4 then
					local c = d.getf(idx, "count") - 1
					if c == 0 then
						d.setf(idx, "floor", 5)
						d.notice(idx, 1142, "", true)
						d.notice(idx, 1143, "", true)
						game.drop_item(30623, 1)
					else
						d.setf(idx, "count", c)
						d.notice(idx, 1141, c, true)
					end
				end
			end
		end

		when slime_end.server_timer begin
			local arg = get_server_timer_arg()
			d.notice(arg, 1040, "", true)
			d.notice(arg, 1041, "", true)
			slime_dungeon.clear(arg)
		end

		when slime_prepare.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.spawn_mob(arg, 8430, 245, 256)
				server_timer("slime_end", 1799, arg)
			else
				slime_dungeon.clear(arg)
			end
		end

		when logout begin
			local idx = pc.get_map_index()
			if idx >= 270000 and idx < 280000 then
				pc.setf("slime_dungeon", "disconnect", get_global_time() + 300)
			end
		end

		when login begin
			local idx = pc.get_map_index()
			if idx == 27 then
				pc.warp(535400, 1428400)
			elseif idx >= 270000 and idx < 280000 then
				pc.set_warp_location(219, 5354, 14284)
				pc.setf("slime_dungeon", "idx", idx)
				pc.setf("slime_dungeon", "ch", pc.get_channel_id())
				if d.getf(idx, "floor") == 0 then
					if not party.is_party() then
						d.setf(idx, "floor", 1)
						server_timer("slime_prepare", 1, idx)
						d.setf(idx, "was_completed", 0)
					else
						if party.is_leader() then
							d.setf(idx, "floor", 1)
							server_timer("slime_prepare", 1, idx)
							d.setf(idx, "was_completed", 0)
						end
					end
				end
			end
		end

		when 9236.chat."A new territory?!?" begin
			local lang = pc.get_language()
			
			say_title(string.format("%s:", mob_name(9236)))
			say("")
			say(string.format(gameforge[lang].common.dungeon_1, pc.get_name()))
			local mapIdx = pc.get_map_index()
			if mapIdx != 219 then
				return
			end
			
			say(gameforge[lang].common.dungeon_2)
			local agree = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			say_title(string.format("%s:", mob_name(9236)))
			say("")
			if agree == 2 then
				say(gameforge[lang].common.dungeon_3)
				return
			end
			
			local goahead = 1
			local rejoinTIME = pc.getf("slime_dungeon", "disconnect") - get_global_time()
			if rejoinTIME > 0 then
				local rejoinIDX = pc.getf("slime_dungeon", "idx")
				if rejoinIDX > 0 then
					local rejoinCH = pc.getf("slime_dungeon", "ch")
					if rejoinCH != 0 and rejoinCH != pc.get_channel_id() then
						say(string.format(gameforge[lang].common.dungeon_26, rejoinCH))
						return
					end
					
					if rejoinCH != 0 and d.find(rejoinIDX) then
						if d.getf(rejoinIDX, "was_completed") == 0 then
							say(gameforge[lang].common.dungeon_4)
							local agree2 = select(gameforge[lang].common.yes, gameforge[lang].common.no)
							if agree2 == 2 then
								say_title(string.format("%s:", mob_name(9236)))
								say("")
								say(gameforge[lang].common.dungeon_3)
								return
							end
							
							local f = d.getf(rejoinIDX, "floor")
							if f != 0 then
								goahead = 0
								pc.warp(942000, 127700, rejoinIDX)
							end
						end
					end
				end
			end
			
			if goahead == 1 then
				pc.setf("slime_dungeon", "disconnect", 0)
				pc.setf("slime_dungeon", "idx", 0)
				pc.setf("slime_dungeon", "ch", 0)
				
				say(gameforge[lang].common.dungeon_5)
				say_reward(string.format(gameforge[lang].common.dungeon_6, 45))
				say_reward(string.format(gameforge[lang].common.dungeon_7, 65))
				say_reward(string.format("- %s: %d", item_name(30625), 1))
				if party.is_party() then
					say_reward(gameforge[lang].common.dungeon_8)
				end
				say(gameforge[lang].common.dungeon_9)
				local n = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				say_title(string.format("%s:", mob_name(9236)))
				say("")
				if n == 2 then
					say(gameforge[lang].common.dungeon_3)
					return
				end
				
				local flag = string.format("ww_27_%d", pc.get_channel_id())
				local ww_flag = game.get_event_flag(flag) - get_global_time()
				if ww_flag > 0 then
					say(gameforge[lang].common.dungeon_28)
					say(string.format(gameforge[lang].common.dungeon_29, ww_flag))
					return
				end
				
				myname = pc.get_name()
				result, cooldown, name = d.check_entrance(45, 65, 30625, 0, 1, "slime_dungeon.cooldown")
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
						say(string.format(gameforge[lang].common.dungeon_18, 1, item_name(30625)))
					else
						say(string.format(gameforge[lang].common.dungeon_17, name, 1, item_name(30625)))
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
					d.remove_item(30625, 1, 0, 0, 30623, 255, 30624, 255, 0, 0, 0, 0, 0, 0, 0, 0, 2400, "slime_dungeon")
					game.set_event_flag(string.format("ww_27_%d", pc.get_channel_id()), get_global_time() + 5)
					d.join_cords(27, 9420, 1277)
				end
			end
		end
	end
end

