quest halloween_zone2 begin
	state start begin
		function is_in(arg)
			return pc.in_dungeon() and arg >= 2320000 and arg < 2330000
		end

		function clear(arg)
			clear_server_timer("halloween_zone2_prepare", arg)
			clear_server_timer("halloween_zone2_end", arg)
			clear_server_timer("halloween_zone2_complete", arg)
			clear_server_timer("halloween_zone2_gravedestroy", arg)
			clear_server_timer("halloween_zone2_secondwavespawn", arg)
			clear_server_timer("halloween_zone2_jumper", arg)
			clear_server_timer("halloween_zone2_4thwave_2", arg)
			clear_server_timer("halloween_zone2_finalboss", arg)
			clear_server_timer("halloween_zone2_hpcheck", arg)
			clear_server_timer("halloween_zone2_finalboss2", arg)
			
			if d.find(arg) then
				d.setf(arg, "was_completed", 1)
				d.kill_all(arg)
				d.clear_regen(arg)
				d.exit_all_lobby(arg, 2)
			end
		end

		function clean(arg)
			if d.find(arg) then
				d.kill_all(arg)
				d.clear_regen(arg)
				d.kill_all(arg)
			end
		end

		when halloween_zone2_complete.server_timer begin
			halloween_zone2.clear(get_server_timer_arg())
		end

		when halloween_zone2_end.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.notice(arg, 1040, "", true)
				d.notice(arg, 1041, "", true)
			end
			
			halloween_zone2.clear(arg)
		end

		when 4568.kill with halloween_zone2.is_in(pc.get_map_index()) and not npc.is_pc() and d.getf(pc.get_map_index(), "floor") == 2 and d.getf(pc.get_map_index(), "cankillfinalboss") == 1 begin
			local arg = pc.get_map_index()
			
			d.setf(arg, "was_completed", 1)
			d.complete(4568, 10800, 232, "halloween_zone2")
			
			if party.is_party() then
				notice_all(1581, party.get_leader_name())
			else
				notice_all(1582, pc.get_name())
			end
			
			d.notice(arg, 1583, "", true)
			d.notice(arg, 1580, "", true)
			
			d.kill_all(arg)
			d.clear_regen(arg)
			
			server_timer("halloween_zone2_complete", 30, arg)
		end

		when halloween_zone2_finalboss2.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.spawn_mob(arg, 4568, 895, 970)
			else
				halloween_zone2.clear(arg)
			end
		end

		when halloween_zone2_hpcheck.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				if d.unique_get_hp_perc(arg, "finalboss") > 50 then
					d.purge_unique(arg, "finalboss")
					d.notice(arg, 1575, "", true)
					d.notice(arg, 1576, "", true)
					
					server_timer("halloween_zone2_finalboss2", 10, arg)
				end
			else
				halloween_zone2.clear(arg)
			end
		end

		when halloween_zone2_finalboss.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.setf(arg, "cankillfinalboss", 1)
				d.set_unique(arg, "finalboss", d.spawn_mob(arg, 4568, 895, 970))
				server_timer("halloween_zone2_hpcheck", 20, arg)
			else
				halloween_zone2.clear(arg)
			end
		end

		when 9420.take with item.get_vnum() == 30902 and halloween_zone2.is_in(pc.get_map_index()) and not npc.is_pc() and d.getf(pc.get_map_index(), "floor") == 2 and d.getf(pc.get_map_index(), "pillar") == 1 begin
			local arg = pc.get_map_index()
			local position = {
								{920, 947, 360},
								{957, 947, 360},
								{957, 992, 180},
								{920, 992, 180}
			}
			
			local status = d.getf(arg, "pillar_c")
			local n = d.getf(arg, "pillar_c") + 1
			
			item.remove()
			
			d.setf(arg, "pillar", 0)
			d.setf(arg, "pillar_c", n)
			
			local npcVid = npc.get_vid()
			
			if npcVid == d.get_unique_vid(arg, "pillar_1") then
				d.spawn_mob(arg, 9421, position[1][1], position[1][2], position[1][3])
				d.kill_unique(arg, "pillar_1")
			elseif npcVid == d.get_unique_vid(arg, "pillar_2") then
				d.spawn_mob(arg, 9421, position[2][1], position[2][2], position[2][3])
				d.kill_unique(arg, "pillar_2")
			elseif npcVid == d.get_unique_vid(arg, "pillar_3") then
				d.spawn_mob(arg, 9421, position[3][1], position[3][2], position[3][3])
				d.kill_unique(arg, "pillar_3")
			elseif npcVid == d.get_unique_vid(arg, "pillar_4") then
				d.spawn_mob(arg, 9421, position[4][1], position[4][2], position[4][3])
				d.kill_unique(arg, "pillar_4")
			end
			
			if status == 1 then
				d.setf(arg, "3w_monsters_k", 1)
				d.regen_file(arg, "data/dungeon/halloween2021_dungeon/2/regen_2f_mobs1.txt")
				d.notice(arg, 1572, "", true)
			elseif status == 2 then
				local n = number(1, 8)
				local pumpkin_stone_pos = {
											{962, 950},
											{947, 950},
											{930, 950},
											{903, 950},
											{962, 985},
											{947, 985},
											{930, 985},
											{903, 985}
				}
				
				table_shuffle(pumpkin_stone_pos)
				for i = 1, 8 do
					if i != n then
						d.set_unique(arg, "fake_"..i, d.spawn_mob(arg, 8723, pumpkin_stone_pos[i][1], pumpkin_stone_pos[i][2]))
					end
				end
				
				d.set_unique(arg, "real", d.spawn_mob(arg, 8723, pumpkin_stone_pos[n][1], pumpkin_stone_pos[n][2]))
				d.setf(arg, "destroyfpumpkin", 2)
				d.notice(arg, 1573, "", true)
			elseif status == 3 then
				d.setf(arg, "4w_monsters_k", 1)
				d.setf(arg, "4w_stage", 1)
				d.regen_file(arg, "data/dungeon/halloween2021_dungeon/2/regen_2f_mobs2.txt")
				d.notice(arg, 1572, "", true)
			elseif status == 4 then
				d.kill_all_monsters(arg)
				d.notice(arg, 1574, "", true)
				server_timer("halloween_zone2_finalboss", 10, arg)
			end
		end

		when halloween_zone2_4thwave_2.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.setf(arg, "4w_monsters_k", 1)
				d.regen_file(arg, "data/dungeon/halloween2021_dungeon/2/regen_2f_mobs2.txt")
			else
				halloween_zone2.clear(arg)
			end
		end

		when 4558.kill or 
			 4559.kill or 
			 4560.kill or 
			 4561.kill or 
			 4562.kill or 
			 4563.kill or 
			 4564.kill or 
			 4565.kill with halloween_zone2.is_in(pc.get_map_index()) and not npc.is_pc() and d.getf(pc.get_map_index(), "floor") == 2 and d.getf(pc.get_map_index(), "4w_monsters_k") == 1 begin
			local arg = pc.get_map_index()
			local n = d.getf(arg, "4w_monsters_c") + 1
			d.setf(arg, "4w_monsters_c", n)
			
			local x = pc.get_x()
			local y = pc.get_y()
			
			if x > (3584 + 851) and y > (512 + 921) and x < (3584 + 1023) and y < (512 + 1020) then
				if n >= 137 and d.getf(arg, "spawned2") != 1 then
					d.kill_all_monsters(arg)
					
					d.setf(arg, "4w_monsters_c", 0)
					d.setf(arg, "4w_monsters_k", 0)
					
					if d.getf(arg, "4w_stage") == 1 then
						d.setf(arg, "4w_stage", 2)
						
						d.notice(arg, 1571, "", true)
						
						server_timer("halloween_zone2_4thwave_2", 10, arg)
					else
						d.setf(arg, "spawned2", 1)
						d.setf(arg, "pillar", 1)
						
						game.drop_item(30902, 1)
						
						d.notice(arg, 1570, "", true)
					end
				end
			end
		end

		when 4558.kill or 
			 4559.kill or 
			 4560.kill or 
			 4561.kill or 
			 4562.kill or 
			 4563.kill or 
			 4564.kill or 
			 4565.kill with halloween_zone2.is_in(pc.get_map_index()) and not npc.is_pc() and d.getf(pc.get_map_index(), "floor") == 2 and d.getf(pc.get_map_index(), "3w_monsters_k") == 1 begin
			local arg = pc.get_map_index()
			local n = d.getf(arg, "3w_monsters_c") + 1
			d.setf(arg, "3w_monsters_c", n)
			
			local x = pc.get_x()
			local y = pc.get_y()
			
			if x > (3584 + 851) and y > (512 + 921) and x < (3584 + 1023) and y < (512 + 1020) then
				if n >= 127 and d.getf(arg, "spawned3") != 1 then
					d.setf(arg, "spawned3", 1)
					
					d.kill_all_monsters(arg)
					
					d.setf(arg, "3w_monsters_c", 0)
					d.setf(arg, "3w_monsters_k", 0)
					d.setf(arg, "pillar", 1)
					
					game.drop_item(30902, 1)
					
					d.notice(arg, 1569, "", true)
				end
			end
		end

		when 8723.kill with halloween_zone2.is_in(pc.get_map_index()) and not npc.is_pc() and d.getf(pc.get_map_index(), "floor") == 2 begin
			local arg = pc.get_map_index()
			
			if d.getf(arg, "destroyfpumpkin") == 1 then
				game.drop_item(30902, 1)
				d.notice(arg, 1567, "", true)
			elseif d.getf(arg, "destroyfpumpkin") == 2 then
				if npc.get_vid() == d.get_unique_vid(arg, "real") then
					--d.purge_area(2977900, 2344900, 2995100, 2354800)
					d.kill_all_monsters(arg)
					game.drop_item(30902, 1)
					d.setf(arg, "pillar", 1)
				else
					d.notice(arg, 1568, "", true)
				end
			end
		end

		when halloween_zone2_jumper.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.setf(arg, "floor", 2)
				d.setf(arg, "pillar", 1)
				d.setf(arg, "pillar_c", 1)
				d.setf(arg, "destroyfpumpkin", 1)
				
				local pillarpos = {
											{["x"] = 920, ["y"] = 947, ["dir"] = 360},
											{["x"] = 957, ["y"] = 947, ["dir"] = 360},
											{["x"] = 957, ["y"] = 992, ["dir"] = 180},
											{["x"] = 920, ["y"] = 992, ["dir"] = 180}
				}
				
				for index, position in ipairs(pillarpos) do
					d.set_unique(arg, "pillar_"..index, d.spawn_mob(arg, 9420, position["x"], position["y"], position["dir"]))
				end
				
				d.set_unique(arg, "pumpkinmasshp", d.spawn_mob(arg, 8723, 937, 969))
				d.unique_set_maxhp(arg, "pumpkinmasshp", 3000000)
				
				d.jump_all(arg, (3584 + 984), (512 + 969))
			else
				halloween_zone2.clear(arg)
			end
		end

		when 4567.kill with halloween_zone2.is_in(pc.get_map_index()) and not npc.is_pc() and d.getf(pc.get_map_index(), "floor") == 1 and d.getf(pc.get_map_index(), "cankillsecboss") == 1 begin
			local arg = pc.get_map_index()
			
			halloween_zone2.clean(arg)
			
			d.notice(arg, 1566, "10", true)
			
			server_timer("halloween_zone2_jumper", 10, arg)
		end

		when 4558.kill or 
			 4559.kill or 
			 4560.kill or 
			 4561.kill or 
			 4562.kill or 
			 4563.kill or 
			 4564.kill or 
			 4565.kill with halloween_zone2.is_in(pc.get_map_index()) and not npc.is_pc() and d.getf(pc.get_map_index(), "floor") == 1 and d.getf(pc.get_map_index(), "2w_monsters_k") == 1 begin
			local arg = pc.get_map_index()
			local n = d.getf(arg, "2w_monsters_c") + 1
			d.setf(arg, "2w_monsters_c", n)
			
			local x = pc.get_x()
			local y = pc.get_y()
			
			if x > (3584 + 253) and y > (512 + 230) and x < (3584 + 595) and y < (512 + 601) then
				if n >= 395 and d.getf(arg, "spawned1") != 1 then
					d.setf(arg, "spawned1", 1)
					
					d.kill_all(arg)
					
					d.setf(arg, "2w_monsters_c", 0)
					d.setf(arg, "2w_monsters_k", 0)
					d.setf(arg, "cankillsecboss", 1)
					
					d.spawn_mob(arg, 4567, 408, 559)
					
					d.notice(arg, 1564, "", true)
					d.notice(arg, 1565, "", true)
				end
			end
		end

		when halloween_zone2_secondwavespawn.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.setf(arg, "2w_monsters_k", 1)
				d.regen_file(arg, "data/dungeon/halloween2021_dungeon/2/regen_1f_mobs2.txt")
			else
				halloween_zone2.clear(arg)
			end
		end

		when 4566.kill with halloween_zone2.is_in(pc.get_map_index()) and not npc.is_pc() and d.getf(pc.get_map_index(), "floor") == 1 begin
			local arg = pc.get_map_index()
			local n = d.getf(arg, "firstbosscount") + 1
			
			d.setf(arg, "firstbosscount", n)
			
			if d.getf(arg, "firstbosscount") >= 4 then
				clear_server_timer("halloween_zone2_gravedestroy", arg)
				
				d.setf(arg, "destroygraves", 0)
				
				d.notice(arg, 1562, "", true)
				d.notice(arg, 1563, "", true)
				
				server_timer("halloween_zone2_secondwavespawn", 10, arg)
			end
		end

		when 8722.kill with halloween_zone2.is_in(pc.get_map_index()) and not npc.is_pc() and d.getf(pc.get_map_index(), "floor") == 1 and d.getf(pc.get_map_index(), "destroygraves") == 1 begin
			local x = pc.get_local_x()
			local y = pc.get_local_y()
			local arg = pc.get_map_index()
			
			local countGrave = d.getf(arg, "gravecount")
			local nextGrave = d.getf(arg, "gravecount") + 1
			
			d.setf(arg, "gravecount", nextGrave)
			
			if countGrave == 1 then
				d.set_unique(arg, "boss1_1", d.spawn_mob(arg, 4566, x, y))
				d.unique_set_maxhp(arg, "boss1_1", 1500000)
			elseif countGrave == 2 then
				d.set_unique(arg, "boss1_2", d.spawn_mob(arg, 4566, x, y))
				d.unique_set_maxhp(arg, "boss1_2", 1800000)
			elseif countGrave == 3 then
				d.set_unique(arg, "boss1_3", d.spawn_mob(arg, 4566, x, y))
				d.unique_set_maxhp(arg, "boss1_3", 2100000)
			elseif countGrave == 4 then
				d.set_unique(arg, "boss1_4", d.spawn_mob(arg, 4566, x, y))
				d.unique_set_maxhp(arg, "boss1_4", 2350000)
			end
		end

		when halloween_zone2_gravedestroy.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.notice(arg, 1560, "", true)
			end
			
			halloween_zone2.clear(arg)
		end

		when 30901.use with halloween_zone2.is_in(pc.get_map_index()) and d.getf(pc.get_map_index(), "floor") == 1 and d.getf(pc.get_map_index(), "1w_monsters_k") == 0 begin
			local arg = pc.get_map_index()
			if d.getf(arg, "activate_graves") == 1 then
				local timeLimit = 600
				
				item.remove()
				
				d.kill_all(arg)
				
				d.setf(arg, "activate_graves", 0)
				d.setf(arg, "destroygraves", 1)
				d.setf(arg, "gravecount", 1)
				
				d.regen_file(arg, "data/dungeon/halloween2021_dungeon/2/regen_1f_stones.txt")
				
				d.notice(arg, 1558, string.format("%d", math.floor(timeLimit / 60)), true)
				d.notice(arg, 1559, "", true)
				
				server_timer("halloween_zone2_gravedestroy", timeLimit, arg)
			else
				syschat(gameforge[pc.get_language()].halloween_event._11)
			end
		end

		when 4558.kill or 
			 4559.kill or 
			 4560.kill or 
			 4561.kill or 
			 4562.kill or 
			 4563.kill or 
			 4564.kill or 
			 4565.kill with halloween_zone2.is_in(pc.get_map_index()) and not npc.is_pc() and d.getf(pc.get_map_index(), "floor") == 1 and d.getf(pc.get_map_index(), "1w_monsters_k") == 1 begin
			local arg = pc.get_map_index()
			local n = d.getf(arg, "1w_monsters_c") + 1
			d.setf(arg, "1w_monsters_c", n)
			
			local x = pc.get_x()
			local y = pc.get_y()
			
			if x > (3584 + 253) and y > (512 + 230) and x < (3584 + 595) and y < (512 + 601) then
				if n >= 629 and d.getf(arg, "spawned4") != 1 then
					d.setf(arg, "spawned4", 1)
					
					game.drop_item(30901, 1)
					
					d.setf(arg, "1w_monsters_c", 0)
					d.setf(arg, "1w_monsters_k", 0)
					d.setf(arg, "activate_graves", 1)
				end
			end
		end

		when halloween_zone2_prepare.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				d.setf(arg, "1w_monsters_k", 1)
				
				d.regen_file(arg, "data/dungeon/halloween2021_dungeon/2/regen_1f_mobs.txt")
				d.regen_file(arg, "data/dungeon/halloween2021_dungeon/2/regen_1f_graves.txt")
				
				d.notice(arg, 1560, "30", true)
				d.notice(arg, 1556, "", true)
				
				server_timer("halloween_zone2_end", 1799, arg)
			else
				halloween_zone2.clear(arg)
			end
		end

		when logout begin
			local arg = pc.get_map_index()
			if arg >= 2320000 and arg < 2330000 then
				pc.setf("halloween_zone2", "disconnect", get_global_time() + 300)
			end
		end

		when login begin
			local arg = pc.get_map_index()
			if arg == 232 then
				pc.warp(536900, 1331400)
			elseif arg >= 2320000 and arg < 2330000 then
				if game.get_halloween_event_status() != 1 then
					pc.warp(536900, 1331400)
				end
				
				pc.set_warp_location(226, 5369, 13314)
				pc.setf("halloween_zone2", "idx", arg)
				pc.setf("halloween_zone2", "ch", pc.get_channel_id())
				
				if d.getf(arg, "floor") == 0 then
					if not party.is_party() then
						d.setf(arg, "floor", 1)
						server_timer("halloween_zone2_prepare", 1, arg)
						d.setf(arg, "was_completed", 0)
					else
						if party.is_leader() then
							d.setf(arg, "floor", 1)
							server_timer("halloween_zone2_prepare", 1, arg)
							d.setf(arg, "was_completed", 0)
						end
					end
				end
			end
		end

		when 9418.chat."GM: Reset Cooldown (medium)" with pc.is_gm() begin
			addimage(25, 10, "h2021_dun_bg1.tga")
			addimage(225, 170, "h2021_npc1.tga")
			say("")
			say("")
			say("")
			say_title(string.format("%s:", mob_name(9418)))
			say("")
			if pc.is_gm() then -- second check
				pc.setqf("cooldown", 0)
				say("The cooldown was removed.")
			else
				say("?!?")
			end
		end

		when 9418.chat."Cursed Cemetery (medium)" begin
			local lang = pc.get_language()
			
			addimage(25, 10, "h2021_dun_bg1.tga")
			addimage(225, 170, "h2021_npc1.tga")
			say("")
			say("")
			say("")
			say_title(string.format("%s:", mob_name(9418)))
			say("")
			if game.get_halloween_event_status() != 1 then
				say(gameforge[lang].halloween_event._10)
				say(gameforge[lang].halloween_event._11)
			else
				say(string.format(gameforge[lang].common.dungeon_1, pc.get_name()))
				local mapIdx = pc.get_map_index()
				if mapIdx != 226 then
					return
				end
				
				say(gameforge[lang].common.dungeon_2)
				local agree = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				addimage(25, 10, "h2021_dun_bg1.tga")
				addimage(225, 170, "h2021_npc1.tga")
				say("")
				say("")
				say("")
				say_title(string.format("%s:", mob_name(9418)))
				say("")
				if agree == 2 then
					say(gameforge[lang].common.dungeon_3)
					return
				end
				
				local goahead = 1
				local rejoinTIME = pc.getf("halloween_zone2", "disconnect") - get_global_time()
				if rejoinTIME > 0 then
					local rejoinIDX = pc.getf("halloween_zone2", "idx")
					if rejoinIDX > 0 then
						local rejoinCH = pc.getf("halloween_zone2", "ch")
						if rejoinCH != 0 and rejoinCH != pc.get_channel_id() then
							say(string.format(gameforge[lang].common.dungeon_26, rejoinCH))
							return
						end
						
						if rejoinCH != 0 and d.find(rejoinIDX) then
							if d.getf(rejoinIDX, "was_completed") == 0 then
								say(gameforge[lang].common.dungeon_4)
								local agree2 = select(gameforge[lang].common.yes, gameforge[lang].common.no)
								if agree2 == 2 then
									addimage(25, 10, "h2021_dun_bg1.tga")
									addimage(225, 170, "h2021_npc1.tga")
									say("")
									say("")
									say("")
									say_title(string.format("%s:", mob_name(9418)))
									say("")
									say(gameforge[lang].common.dungeon_3)
									return
								end
								
								local f = d.getf(rejoinIDX, "floor")
								if f == 1 then
									goahead = 0
									pc.warp((3584 + 402) * 100, (512 + 280) * 100, rejoinIDX)
								elseif f == 2 then
									goahead = 0
									pc.warp((3584 + 984) * 100, (512 + 969) * 100, rejoinIDX)
								end
							end
						end
					end
				end
				
				if goahead == 1 then
					pc.setf("halloween_zone2", "disconnect", 0)
					pc.setf("halloween_zone2", "idx", 0)
					pc.setf("halloween_zone2", "ch", 0)
					
					say(gameforge[lang].common.dungeon_5)
					say_reward(string.format("- %s: %d", item_name(30900), 1))
					if party.is_party() then
						say_reward(gameforge[lang].common.dungeon_8)
					end
					say("")
					say(gameforge[lang].common.dungeon_9)
					local n = select(gameforge[lang].common.yes, gameforge[lang].common.no)
					addimage(25, 10, "h2021_dun_bg1.tga")
					addimage(225, 170, "h2021_npc1.tga")
					say("")
					say("")
					say("")
					say_title(string.format("%s:", mob_name(9418)))
					say("")
					if n == 2 then
						say(gameforge[lang].common.dungeon_3)
						return
					end
					
					local flag = string.format("ww_232_%d", pc.get_channel_id())
					local ww_flag = game.get_event_flag(flag) - get_global_time()
					if ww_flag > 0 then
						say(gameforge[lang].common.dungeon_28)
						say(string.format(gameforge[lang].common.dungeon_29, ww_flag))
						return
					end
					
					myname = pc.get_name()
					result, cooldown, name = d.check_entrance(1, 127, 30900, 0, 1, "halloween_zone2.cooldown")
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
							say(string.format(gameforge[lang].common.dungeon_18, 1, item_name(30900)))
						else
							say(string.format(gameforge[lang].common.dungeon_17, name, 1, item_name(30900)))
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
						d.remove_item(30900, 1, 0, 0, 30901, 255, 30902, 255, 0, 0, 0, 0, 0, 0, 0, 0, 10800, "halloween_zone2")
						game.set_event_flag(string.format("ww_232_%d", pc.get_channel_id()), get_global_time() + 5)
						d.join_cords(232, (3584 + 402), (512 + 280))
					end
				end
			end
		end
	end
end
