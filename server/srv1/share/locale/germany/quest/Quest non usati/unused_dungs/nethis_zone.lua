quest nethis_zone begin
	state start begin
		function get_key_itemvnum()
			return 70428
		end
		
		function get_cooldown_time()
			return 10800
		end
		
		function get_limit_time()
			return 1800
		end
		
		function get_main_coordinates()
			return {768, 16384}
		end
		
		function get_floor_coordinates(v)
			local xy = {
				{128, 67},
				{384, 67},
				{640, 67},
				--{384, 325},
				--{640, 325},
				{128, 690},
			}
			
			return xy[v]
		end
		
		function is_in_dungeon(idx)
			if idx >= 1360000 and idx < 1370000 then
				return true
			end
			
			return false
		end
		
		function clear(arg)
			clear_server_timer("nethis_zone_prepare", arg)
			clear_server_timer("nethis_zone_end", arg)
			clear_server_timer("nethis_zone_complete", arg)
			clear_server_timer("nethis_zone_limit", arg)
			clear_server_timer("nethis_zone_checker", arg)
			clear_server_timer("nethis_zone_warp", arg)
			
			if d.find(arg) then
				d.setf(arg, "was_completed", 1)
				d.exit_all_lobby(arg, 636, 14567)
				d.kill_all(arg)
				d.clear_regen(arg)
			end
		end
		
		when nethis_zone_complete.server_timer begin
			nethis_zone.clear(get_server_timer_arg())
		end
		
		when nethis_zone_end.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.notice(arg, 1730, "", false)
				d.notice(arg, 1731, "", false)
			end
			
			nethis_zone.clear(arg)
		end
		
		when 6756.kill with nethis_zone.is_in_dungeon(pc.get_map_index()) begin
			local arg = pc.get_map_index()
			if d.find(arg) then
				if d.getf(arg, "floor") == 4 then
					d.setf(arg, "was_completed", 1)
					d.complete(6756, nethis_zone.get_cooldown_time(), 136, "nethis_zone")
					
					if party.is_party() then
						notice_all(1746, party.get_leader_name())
					else
						notice_all(1747, pc.get_name())
					end
					
					d.notice(arg, 1748, "", true)
					d.notice(arg, 1749, "", true)
					
					clear_server_timer("nethis_zone_limit", arg)
					clear_server_timer("nethis_zone_end", arg)
					
					server_timer("nethis_zone_complete", 60, arg)
				end
			end
		end
		
		when nethis_zone_warp.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				local f = d.getf(arg, "floor")
				local s = d.getf(arg, "step")
				if s == 7 then
					d.setf(arg, "floor", 2)
					d.setf(arg, "step", 8)
					
					d.spawn_mob(arg, 4027, 384, 107, 180)
					
					local cC = nethis_zone.get_main_coordinates()
					local cF = nethis_zone.get_floor_coordinates(2)
					d.jump_all(arg, cC[1]+cF[1], cC[2]+cF[2])
					
					d.notice(arg, 1729, "", false)
				elseif f == 3 then
					d.spawn_mob(arg, 4027, 640, 107, 180)
					
					local cC = nethis_zone.get_main_coordinates()
					local cF = nethis_zone.get_floor_coordinates(3)
					d.jump_all(arg, cC[1]+cF[1], cC[2]+cF[2])
					
					d.notice(arg, 1729, "", false)
				elseif f == 4 then
					d.spawn_mob(arg, 6756, 127, 635, 1)
					
					local cC = nethis_zone.get_main_coordinates()
					local cF = nethis_zone.get_floor_coordinates(4)
					d.jump_all(arg, cC[1]+cF[1], cC[2]+cF[2])
					
					d.notice(arg, 1744, "5", false)
					server_timer("nethis_zone_limit", 300, arg)
				end
			else
				nethis_zone.clear(arg)
			end
		end
		
		when nethis_zone_limit.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.notice(arg, 1730, "", false)
				d.notice(arg, 1731, "", false)
			end
			
			nethis_zone.clear(arg)
		end
		
		when nethis_zone_checker.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				if d.count_monster(arg) == 0 then
					clear_server_timer("nethis_zone_limit", arg)
					clear_server_timer("nethis_zone_checker", arg)
					
					d.setf(arg, "floor", 3)
					d.notice(arg, 1736, "", false)
					
					server_timer("nethis_zone_warp", 3, arg)
				end
			else
				nethis_zone.clear(arg)
			end
		end
		
		when 8071.kill with nethis_zone.is_in_dungeon(pc.get_map_index()) begin
			local arg = pc.get_map_index()
			if d.find(arg) then
				if number(1, 2) == 1 then
					clear_server_timer("nethis_zone_limit", arg)
					
					d.setf(arg, "step", 10)
					d.setf(arg, "floor", 4)
					
					d.notice(arg, 1735, "", false)
					d.notice(arg, 1745, "", false)
					
					server_timer("nethis_zone_warp", 3, arg)
				else
					local s = d.getf(arg, "step")
					d.setf(arg, "step", s+1)
					if s == 9 then
						clear_server_timer("nethis_zone_limit", arg)
						
						d.setf(arg, "floor", 4)
						
						d.notice(arg, 1735, "", false)
						d.notice(arg, 1745, "", false)
						
						server_timer("nethis_zone_warp", 3, arg)
					else
						d.notice(arg, 1734, "", false)
					end
				end
			end
		end
		
		when 4027.click with nethis_zone.is_in_dungeon(pc.get_map_index()) begin
			local arg = pc.get_map_index()
			if d.find(arg) then
				npc.purge()
				local f = d.getf(arg, "floor")
				if f == 2 then
					local t = number(1, 6)
					local r = d.getf(arg, "try")
					d.notice(arg, 1739, string.format("%d", t), false)
					if t == r then
						d.notice(arg, 1741, "", false)
						
						clear_server_timer("nethis_zone_limit", arg)
						clear_server_timer("nethis_zone_checker", arg)
						
						d.setf(arg, "floor", 4)
						
						server_timer("nethis_zone_warp", 3, arg)
					else
						d.kill_all(arg)
						d.clear_regen(arg)
						
						d.regen_file(arg, "data/dungeon/nethis/floor2.txt")
						
						d.notice(arg, 1740, string.format("%d", r), false)
						d.notice(arg, 1742, "2", false)
						
						server_timer("nethis_zone_limit", 300, arg)
						server_loop_timer("nethis_zone_checker", 3, arg)
					end
				elseif f == 3 then
					local t = number(1, 6)
					local r = d.getf(arg, "try")
					d.notice(arg, 1739, string.format("%d", t), false)
					if t == r then
						d.notice(arg, 1741, "", false)
						
						clear_server_timer("nethis_zone_limit", arg)
						clear_server_timer("nethis_zone_checker", arg)
						
						d.setf(arg, "floor", 4)
						
						server_timer("nethis_zone_warp", 3, arg)
					else
						d.kill_all(arg)
						d.clear_regen(arg)
						
						d.notice(arg, 1740, string.format("%d", r), false)
						
						d.spawn_mob(arg, 8071, 674, 92, 1)
						d.spawn_mob(arg, 8071, 674, 163, 180)
						d.spawn_mob(arg, 8071, 605, 92, 1)
						d.spawn_mob(arg, 8071, 605, 163, 180)
						
						d.notice(arg, 1733, "", false)
						d.notice(arg, 1743, "2", false)
						
						server_timer("nethis_zone_limit", 300, arg)
					end
				end
			end
		end
		
		when 8068.kill with nethis_zone.is_in_dungeon(pc.get_map_index()) begin
			local arg = pc.get_map_index()
			if d.find(arg) then
				if number(1, 2) == 1 then
					d.setf(arg, "step", 7)
					
					d.notice(arg, 1735, "", false)
					d.notice(arg, 1736, "", false)
					
					server_timer("nethis_zone_warp", 3, arg)
				else
					local s = d.getf(arg, "step")
					d.setf(arg, "step", s+1)
					if s == 6 then
						d.notice(arg, 1735, "", false)
						d.notice(arg, 1736, "", false)
						
						server_timer("nethis_zone_warp", 3, arg)
					else
						d.notice(arg, 1734, "", false)
					end
				end
			end
		end
		
		when 4024.click with nethis_zone.is_in_dungeon(pc.get_map_index()) begin
			local arg = pc.get_map_index()
			if d.find(arg) then
				local s = d.getf(arg, "step")
				local t = d.getf(arg, "try")
				if s == 1 then
					d.setf(arg, "try", t + 1)
					d.setf(arg, "step", 2)
					npc.purge()
					
					d.notice(arg, 1732, "", false)
					d.spawn_mob(arg, 4024, 128, 87, 1)
				elseif s == 2 then
					if number(1, 3) == 1 or t == 6 then
						d.setf(arg, "step", 3)
						npc.purge()
						
						d.spawn_mob(arg, 8068, 163, 93, 1)
						d.spawn_mob(arg, 8068, 163, 162, 180)
						d.spawn_mob(arg, 8068, 92, 93, 1)
						d.spawn_mob(arg, 8068, 93, 162, 180)
						
						d.notice(arg, 1733, "", false)
						d.notice(arg, 1737, string.format("%d", t), false)
						d.notice(arg, 1738, "", false)
					else
						d.setf(arg, "step", 1)
						npc.purge()
						
						d.notice(arg, 1732, "", false)
						d.spawn_mob(arg, 4024, 128, 107, 180)
					end
				end
			end
		end
		
		when nethis_zone_prepare.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.setf(arg, "step", 1)
				d.setf(arg, "try", 1)
				
				d.notice(arg, 1728, string.format("%d", nethis_zone.get_limit_time()/60), false)
				d.notice(arg, 1729, "", false)
				
				server_timer("nethis_zone_end", nethis_zone.get_limit_time()-1, arg)
			else
				nethis_zone.clear(arg)
			end
		end

		when logout begin
			if nethis_zone.is_in_dungeon(pc.get_map_index()) then
				pc.setf("nethis_zone", "disconnect", get_global_time() + 300)
			end
		end

		when login begin
			local idx = pc.get_map_index()
			if idx == 136 then
				pc.warp(636*100, 14567*100)
			elseif nethis_zone.is_in_dungeon(idx) then
				pc.set_warp_location(135, 636, 14567)
				pc.setf("nethis_zone", "idx", idx)
				pc.setf("nethis_zone", "ch", pc.get_channel_id())
				if d.getf(idx, "floor") == 0 then
					if not party.is_party() then
						d.setf(idx, "floor", 1)
						d.spawn_mob(idx, 4024, 128, 107, 180)
						server_timer("nethis_zone_prepare", 1, idx)
						d.setf(idx, "was_completed", 0)
					else
						if party.is_leader() then
							d.setf(idx, "floor", 1)
							d.spawn_mob(idx, 4024, 128, 107, 180)
							server_timer("nethis_zone_prepare", 1, idx)
							d.setf(idx, "was_completed", 0)
						end
					end
				end
			end
		end

		when 20804.chat."Temple of the Serpent?!?" begin
			local lang = pc.get_language()
			
			say_title(string.format("%s:", mob_name(20804)))
			say("")
			say(string.format(gameforge[lang].common.dungeon_1, pc.get_name()))
			local mapIdx = pc.get_map_index()
			if mapIdx != 135 then
				return
			end
			
			say(gameforge[lang].common.dungeon_2)
			local agree = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			say_title(string.format("%s:", mob_name(20804)))
			say("")
			if agree == 2 then
				say(gameforge[lang].common.dungeon_3)
				return
			end
			
			local goahead = 1
			local rejoinTIME = pc.getf("nethis_zone", "disconnect") - get_global_time()
			if rejoinTIME > 0 then
				local rejoinIDX = pc.getf("nethis_zone", "idx")
				if rejoinIDX > 0 then
					local rejoinCH = pc.getf("nethis_zone", "ch")
					if rejoinCH != 0 and rejoinCH != pc.get_channel_id() then
						say(string.format(gameforge[lang].common.dungeon_26, rejoinCH))
						return
					end
					
					if rejoinCH != 0 and d.find(rejoinIDX) then
						if d.getf(rejoinIDX, "was_completed") == 0 then
							say(gameforge[lang].common.dungeon_4)
							local agree2 = select(gameforge[lang].common.yes, gameforge[lang].common.no)
							if agree2 == 2 then
								say_title(string.format("%s:", mob_name(20804)))
								say("")
								say(gameforge[lang].common.dungeon_3)
								return
							end
							
							local f = d.getf(rejoinIDX, "floor")
							if f != 0 then
								goahead = 0
								local cC = nethis_zone.get_main_coordinates()
								local cF = nethis_zone.get_floor_coordinates(f)
								pc.warp((cC[1]+cF[1])*100, (cC[2]+cF[2])*100, rejoinIDX)
							end
						end
					end
				end
			end
			
			if goahead == 1 then
				pc.setf("nethis_zone", "disconnect", 0)
				pc.setf("nethis_zone", "idx", 0)
				pc.setf("nethis_zone", "ch", 0)
				
				say(gameforge[lang].common.dungeon_5)
				say_reward(string.format(gameforge[lang].common.dungeon_6, 125))
				say_reward(string.format(gameforge[lang].common.dungeon_7, 135))
				say_reward(string.format("- %s: %d", item_name(nethis_zone.get_key_itemvnum()), 1))
				if party.is_party() then
					say_reward(gameforge[lang].common.dungeon_8)
				end
				say(gameforge[lang].common.dungeon_9)
				local n = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				say_title(string.format("%s:", mob_name(20804)))
				say("")
				if n == 2 then
					say(gameforge[lang].common.dungeon_3)
					return
				end
				
				local flag = string.format("ww_%d_%d", 136, pc.get_channel_id())
				local ww_flag = game.get_event_flag(flag) - get_global_time()
				if ww_flag > 0 then
					say(gameforge[lang].common.dungeon_28)
					say(string.format(gameforge[lang].common.dungeon_29, ww_flag))
					return
				end
				
				myname = pc.get_name()
				result, cooldown, name = d.check_entrance(125, 135, nethis_zone.get_key_itemvnum(), 0, 1, "nethis_zone.cooldown")
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
						say(string.format(gameforge[lang].common.dungeon_18, 1, item_name(nethis_zone.get_key_itemvnum())))
					else
						say(string.format(gameforge[lang].common.dungeon_17, name, 1, item_name(nethis_zone.get_key_itemvnum())))
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
					d.remove_item(nethis_zone.get_key_itemvnum(), 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, nethis_zone.get_cooldown_time(), "nethis_zone")
					game.set_event_flag(string.format("ww_%d_%d", 136, pc.get_channel_id()), get_global_time() + 5)
					local cC = nethis_zone.get_main_coordinates()
					local cF = nethis_zone.get_floor_coordinates(1)
					d.join_cords(136, cC[1]+cF[1], cC[2]+cF[2])
				end
			end
		end
	end
end
