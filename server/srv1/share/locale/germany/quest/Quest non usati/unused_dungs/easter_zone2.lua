quest easter_zone2 begin
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
			clear_server_timer("easter_zone2_prepare", arg)
			clear_server_timer("easter_zone2_end", arg)
			clear_server_timer("easter_zone2_complete", arg)
			clear_server_timer("easter_zone_wave", arg)
			clear_server_timer("easter_zone_check", arg)
			clear_server_timer("easter_zone_check2", arg)
			clear_server_timer("easter_zone_warp", arg)
			if d.find(arg) then
				d.setf(arg, "was_completed", 1)
				d.kill_all(arg)
				d.clear_regen(arg)
				d.exit_all_lobby(arg, d.getf(arg, "out_x"), d.getf(arg, "out_y"))
			end
		end

		when easter_zone2_complete.server_timer begin
			easter_zone2.clear(get_server_timer_arg())
		end

		when easter_zone2_end.server_timer begin
			local arg = get_server_timer_arg()
			d.notice(arg, 1773, "", true)
			d.notice(arg, 1774, "", true)
			easter_zone2.clear(arg)
		end

		when easter_zone_warp.server_timer begin
			local arg = get_server_timer_arg()
			clear_server_timer("easter_zone_warp", arg)
			if d.find(arg) then
				local f = d.getf(arg, "floor")
				if f == 4 then
					d.jump_all(arg, 4403, 22685)
				end
			else
				easter_zone2.clear(arg)
			end
		end

		when 4103.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1570000 and idx < 1580000 then
				if d.getf(idx, "floor") == 23 then
					clear_server_timer("easter_zone2_end", idx)
					d.setf(idx, "was_completed", 1)
					d.complete(4103, 3600, 157, "easter_zone2")
					if party.is_party() then
						notice_all(1771, party.get_leader_name())
					else
						notice_all(1772, pc.get_name())
					end
					
					d.notice(idx, 1775, "", true)
					d.notice(idx, 1776, "", true)
					d.kill_all(idx)
					d.clear_regen(idx)
					
					server_timer("easter_zone2_complete", 30, idx)
				end
			end
		end

		when 8463.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1570000 and idx < 1580000 then
				if d.getf(idx, "floor") == 17 then
					local stonescount = 8
					local n = d.getf(idx, "stones_c") + 1
					d.setf(idx, "stones_c", n)
					
					if n < stonescount then
						d.notice(idx, 1777, string.format("%d", stonescount - n), true)
					else
						d.setf(idx, "floor", 18)
						game.drop_item(30793, 1)
						d.notice(idx, 1778, "", true)
						d.notice(idx, 1779, "", true)
					end
				end
			end
		end

		when 9315.take with item.get_vnum() == 30793 and pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1570000 and idx < 1580000 then
				pc.remove_item(30793, 1)
				npc.kill()
				
				local f = d.getf(idx, "floor")
				if f == 16 then
					d.setf(idx, "floor", 17)
					d.regen_file(idx, "data/dungeon/easter2020/regen_4c.txt")
					d.notice(idx, 1780, "", true)
				elseif f == 18 then
					d.setf(idx, "floor", 19)
					
					local bosspos = {
								[1] = {["x"] = 573, ["y"] = 523},
								[2] = {["x"] = 565, ["y"] = 576},
								[3] = {["x"] = 607, ["y"] = 586},
								[4] = {["x"] = 625, ["y"] = 535}
					}
					
					local random_num = number(1, table.getn(bosspos))
					for index, position in ipairs(bosspos) do
						local vid = d.spawn_mob(idx, 4102, position["x"], position["y"])
						if random_num == index then
							d.setf(idx, "unique_vid", vid)
						end
					end
					
					d.notice(idx, 1781, "", true)
					d.notice(idx, 1782, "", true)
				elseif f == 20 then
					d.setf(idx, "floor", 21)
					d.notice(idx, 1783, "", true)
					server_timer("easter_zone_wave", 3, idx)
				elseif f == 22 then
					d.setf(idx, "floor", 23)
					d.notice(idx, 1784, "", true)
					d.notice(idx, 1785, "", true)
					server_timer("easter_zone_wave", 10, idx)
				end
			end
		end

		when 9310.click with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1570000 and idx < 1580000 and d.getf(idx, "floor") == 6 then
				local lang = pc.get_language()
				
				if npc.get_vid() == d.getf(idx, "unique_vid") then
					d.setf(idx, "floor", 7)
					game.drop_item(30789, 1)
					npc.purge()
					syschat(string.format(gameforge[lang].easter_zone2._7, mob_name(9308)))
				else
					npc.purge()
					syschat(gameforge[lang].easter_zone2._6)
				end
			end
		end

		when 9311.take with item.get_vnum() == 30791 and pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1570000 and idx < 1580000 then
				if d.getf(idx, "floor") == 11 then
					d.spawn_mob(idx, 9312, 598, 469, 135)
					pc.remove_item(30791, 1)
					npc.kill()
					d.notice(idx, 1786, "", true)
					
					local basketspos = {
								[1] = {["x"] = 611, ["y"] = 382},
								[2] = {["x"] = 613, ["y"] = 408},
								[3] = {["x"] = 613, ["y"] = 428},
								[4] = {["x"] = 614, ["y"] = 458},
								[5] = {["x"] = 587, ["y"] = 462},
								[6] = {["x"] = 594, ["y"] = 445},
								[7] = {["x"] = 597, ["y"] = 426},
								[8] = {["x"] = 597, ["y"] = 426},
								[9] = {["x"] = 597, ["y"] = 400},
								[10] = {["x"] = 604, ["y"] = 418},
								[11] = {["x"] = 603, ["y"] = 440}
					}
					
					local random_num = number(1, table.getn(basketspos))
					for index, position in ipairs(basketspos) do
						local vid = d.spawn_mob(idx, 8462, position["x"], position["y"])
						if random_num == index then
							d.setf(idx, "unique_vid", vid)
						end
					end
				end
			end
		end

		when 9312.take with item.get_vnum() == 30791 and pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1570000 and idx < 1580000 then
				if d.getf(idx, "floor") == 12 then
					d.spawn_mob(idx, 9313, 598, 469, 135)
					pc.remove_item(30791, 1)
					npc.kill()
					d.notice(idx, 1786, "", true)
					
					local basketspos = {
								[1] = {["x"] = 611, ["y"] = 382},
								[2] = {["x"] = 613, ["y"] = 408},
								[3] = {["x"] = 613, ["y"] = 428},
								[4] = {["x"] = 614, ["y"] = 458},
								[5] = {["x"] = 587, ["y"] = 462},
								[6] = {["x"] = 594, ["y"] = 445},
								[7] = {["x"] = 597, ["y"] = 426},
								[8] = {["x"] = 597, ["y"] = 426},
								[9] = {["x"] = 597, ["y"] = 400},
								[10] = {["x"] = 604, ["y"] = 418},
								[11] = {["x"] = 603, ["y"] = 440}
					}
					
					local random_num = number(1, table.getn(basketspos))
					for index, position in ipairs(basketspos) do
						local vid = d.spawn_mob(idx, 8462, position["x"], position["y"])
						if random_num == index then
							d.setf(idx, "unique_vid", vid)
						end
					end
				end
			end
		end

		when 9308.chat."What next?!?" with pc.in_dungeon() and d.getf(pc.get_map_index(), "floor") == 14 begin
			local idx = pc.get_map_index()
			if idx >= 1570000 and idx < 1580000 then
				addimage(25, 10, "easter2020_bg3.tga")
				addimage(225, 150, "easter2020_rabbit.tga")
				
				local lang = pc.get_language()
				
				say("[ENTER][ENTER]")
				say_title(string.format("%s:[ENTER]", mob_name(9308)))
				if party.is_party() and not party.is_leader() then
					say(gameforge[lang].easter_zone2._13)
					return
				else
					if pc.count_item(30792) < 1 then
						say(gameforge[lang].easter_zone2._14)
						say_item(""..item_name(30792).."", 30792, "")
						say(string.format(gameforge[lang].easter_zone2._15, mob_name(9314)))
					else
						pc.remove_item(30792, 1)
						say(gameforge[lang].easter_zone2._16)
						say(gameforge[lang].easter_zone2._17)
						say(gameforge[lang].easter_zone2._18)
						say(gameforge[lang].easter_zone2._19)
						say(gameforge[lang].easter_zone2._20)
						say(gameforge[lang].easter_zone2._21)
						say(gameforge[lang].easter_zone2._22)
						wait()
						setskin(NOWINDOW)
						npc.purge()
						d.kill_all(idx)
						d.setf(idx, "floor", 15)
						d.regen_file(idx, "data/dungeon/easter2020/regen_4a.txt")
						d.set_regen_file(idx, "data/dungeon/easter2020/regen_4b.txt")
						d.notice(idx, 1789, "", true)
						d.notice(idx, 1790, "", true)
					end
				end
			else
				setskin(NOWINDOW)
			end
		end

		when 9313.take with item.get_vnum() == 30791 and pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1570000 and idx < 1580000 then
				if d.getf(idx, "floor") == 13 then
					d.spawn_mob(idx, 9314, 598, 469, 135)
					d.spawn_mob(idx, 9308, 599, 454, 180)
					pc.remove_item(30791, 1)
					npc.kill()
					d.notice(idx, 1787, "", true)
					d.notice(idx, 1788, "", true)
				end
			end
		end

		when 9314.click with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1570000 and idx < 1580000 then
				if d.getf(idx, "floor") == 13 then
					local lang = pc.get_language()
					
					if party.is_party() and not party.is_leader() then
						syschat(gameforge[lang].easter_zone2._12)
					else
						npc.purge()
						pc.give_item2(30792, 1)
						syschat(string.format(gameforge[lang].easter_zone2._11, mob_name(9308)))
						d.setf(idx, "floor", 14)
					end
				end
			end
		end

		when 8462.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1570000 and idx < 1580000 then
				if npc.get_vid() == d.getf(idx, "unique_vid") then
					if d.getf(idx, "floor") == 10 then
						d.setf(idx, "floor", 11)
					elseif d.getf(idx, "floor") == 11 then
						d.setf(idx, "floor", 12)
					elseif d.getf(idx, "floor") == 12 then
						d.setf(idx, "floor", 13)
					end
					
					d.kill_all_monsters(idx)
					game.drop_item(30791, 1)
				end
			end
		end

		when 9308.take with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1570000 and idx < 1580000 then
				local f = d.getf(idx, "floor")
				local v = item.get_vnum()
				if v == 30788 and f == 5 then
					pc.remove_item(30788, 1)
					
					d.setf(idx, "floor", 6)
					local carrotpos = {
								[1] = {["x"] = 584, ["y"] = 156},
								[2] = {["x"] = 536, ["y"] = 176},
								[3] = {["x"] = 567, ["y"] = 198},
								[4] = {["x"] = 563, ["y"] = 224},
								[5] = {["x"] = 521, ["y"] = 228},
								[6] = {["x"] = 523, ["y"] = 257},
								[7] = {["x"] = 541, ["y"] = 256},
								[8] = {["x"] = 564, ["y"] = 267},
								[9] = {["x"] = 566, ["y"] = 307},
								[10] = {["x"] = 587, ["y"] = 276},
								[11] = {["x"] = 623, ["y"] = 301},
								[12] = {["x"] = 619, ["y"] = 263},
								[13] = {["x"] = 654, ["y"] = 269},
								[14] = {["x"] = 637, ["y"] = 252},
								[15] = {["x"] = 622, ["y"] = 231},
								[16] = {["x"] = 656, ["y"] = 208},
								[17] = {["x"] = 609, ["y"] = 213},
								[18] = {["x"] = 608, ["y"] = 158}
					}
					
					local random_num = number(1, table.getn(carrotpos))
					for index, position in ipairs(carrotpos) do
						local vid = d.spawn_mob(idx, 9310, position["x"], position["y"])
						if random_num == index then
							d.setf(idx, "unique_vid", vid)
						end
					end
					
					local lang = pc.get_language()
					addimage(25, 10, "easter2020_bg2.tga")
					addimage(225, 150, "easter2020_rabbit.tga")
					say("[ENTER][ENTER]")
					say_title(string.format("%s:[ENTER]", mob_name(9308)))
					say(gameforge[lang].easter_zone2._1)
					say(gameforge[lang].easter_zone2._2)
					say(gameforge[lang].easter_zone2._3)
					wait()
					addimage(25, 10, "easter2020_bg2.tga");
					addimage(225, 150, "easter2020_rabbit.tga")
					say("[ENTER][ENTER]")
					say_title(string.format("%s:[ENTER]", mob_name(9308)))
					say(gameforge[lang].easter_zone2._4)
					say(gameforge[lang].easter_zone2._5)
					say_item(""..item_name(30789).."", 30789, "")
					wait()
					setskin(NOWINDOW)
					d.notice(idx, 1791, "", true)
				elseif v == 30789 and f == 7 then
					pc.remove_item(30789, 1)
					
					d.setf(idx, "floor", 8)
					local stonepos = {
								[1] = {["x"] = 575, ["y"] = 196},
								[2] = {["x"] = 538, ["y"] = 242},
								[3] = {["x"] = 553, ["y"] = 289},
								[4] = {["x"] = 598, ["y"] = 285},
								[5] = {["x"] = 639, ["y"] = 284},
								[6] = {["x"] = 639, ["y"] = 239},
								[7] = {["x"] = 632, ["y"] = 198}
					}
					
					local random_num = number(1, table.getn(stonepos))
					for index, position in ipairs(stonepos) do
						local vid = d.spawn_mob(idx, 8461, position["x"], position["y"])
						if random_num == index then
							d.setf(idx, "unique_vid", vid)
						end
					end
					
					local lang = pc.get_language()
					addimage(25, 10, "easter2020_bg2.tga")
					addimage(225, 150, "easter2020_rabbit.tga")
					say("[ENTER][ENTER]")
					say_title(string.format("%s:[ENTER]", mob_name(9308)))
					say(gameforge[lang].easter_zone2._8)
					say_item(""..item_name(30790).."", 30790, "")
					say(string.format(gameforge[lang].easter_zone2._9, mob_name(8461)))
					say(gameforge[lang].easter_zone2._10)
					wait()
					setskin(NOWINDOW)
					d.notice(idx, 1792, "", true)
				elseif v == 30790 and f == 9 then
					pc.remove_item(30790, 1)
					
					d.setf(idx, "floor", 10)
					npc.purge()
					d.kill_all(idx)
					d.spawn_mob(idx, 9309, 597, 476, 180)
					d.spawn_mob(idx, 9311, 598, 469, 135)
					local basketspos = {
								[1] = {["x"] = 611, ["y"] = 382},
								[2] = {["x"] = 613, ["y"] = 408},
								[3] = {["x"] = 613, ["y"] = 428},
								[4] = {["x"] = 614, ["y"] = 458},
								[5] = {["x"] = 587, ["y"] = 462},
								[6] = {["x"] = 594, ["y"] = 445},
								[7] = {["x"] = 597, ["y"] = 426},
								[8] = {["x"] = 597, ["y"] = 426},
								[9] = {["x"] = 597, ["y"] = 400},
								[10] = {["x"] = 604, ["y"] = 418},
								[11] = {["x"] = 603, ["y"] = 440}
					}
					
					local random_num = number(1, table.getn(basketspos))
					for index, position in ipairs(basketspos) do
						local vid = d.spawn_mob(idx, 8462, position["x"], position["y"])
						if random_num == index then
							d.setf(idx, "unique_vid", vid)
						end
					end
				end
			end
		end

		when 4096.kill or 4097.kill or 4098.kill or 4099.kill or 4100.kill or 4101.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1570000 and idx < 1580000 then
				local f = d.getf(idx, "floor")
				if f == 4 then
					local c = d.getf(idx, "monster_c") - 1
					d.setf(idx, "monster_c", c)
					
					if c <= 0 then
						d.setf(idx, "floor", 5)
						d.clear_regen(idx)
						d.setf(idx, "monster_c", 0)
						d.spawn_mob(idx, 9308, 608, 355, 180)
						game.drop_item(30788, 1)
						d.notice(idx, 1793, "", true)
						d.notice(idx, 1794, "", true)
					end
				elseif f == 15 then
					if number(1, 150) == 1 then
						d.setf(idx, "floor", 16)
						game.drop_item(30793, 1)
						d.clear_regen(idx)
						d.kill_all_monsters(idx)
						d.notice(idx, 1795, "", true)
						d.notice(idx, 1796, "", true)
					end
				elseif f == 21 then
					local c = d.getf(idx, "monster_c") - 1
					d.setf(idx, "monster_c", c)
					
					if c <= 0 then
						d.setf(idx, "floor", 22)
						d.clear_regen(idx)
						d.setf(idx, "monster_c", 0)
						game.drop_item(30793, 1)
						d.notice(idx, 1797, "", true)
						d.notice(idx, 1798, "", true)
					end
				end
			end
		end

		when 4102.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1570000 and idx < 1580000 then
				local f = d.getf(idx, "floor")
				if f == 3 then
					if d.getf(idx, "boss1_vid") == npc.get_vid() then
						d.setf(idx, "boss1_vid", 0)
						d.setf(idx, "floor", 4)
						clear_server_timer("easter_zone_check2", idx)
						d.clear_regen(idx)
						d.kill_all(idx)
						d.notice(idx, 1799, "", true)
						d.spawn_mob(idx, 9309, 609, 362, 180)
						d.regen_file(idx, "data/dungeon/easter2020/regen_2a.txt")
						d.setf(idx, "monster_c", d.count_monster(idx))
						server_timer("easter_zone_warp", 2, idx)
					end
				elseif f == 19 then
					if npc.get_vid() == d.getf(idx, "unique_vid") then
						d.setf(idx, "floor", 20)
						d.kill_all_monsters(idx)
						game.drop_item(30793, 1)
						d.notice(idx, 1801, "", true)
					else
						d.notice(idx, 1800, "", true)
					end
				end
			end
		end

		when easter_zone_check2.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				local f = d.getf(arg, "floor")
				if f == 3 then
					clear_server_timer("easter_zone_check2", arg)
					if d.getf(arg, "boss1_vid") != 0 then
						d.purge_vid(d.getf(arg, "boss1_vid"))
						local vid = d.spawn_mob(arg, 4102, 218, 374)
						d.setf(arg, "boss1_vid", vid)
						d.notice(arg, 1802, "", true)
					end
				end
			else
				easter_zone2.clear(arg)
			end
		end

		when easter_zone_check.server_timer begin
			local arg = get_server_timer_arg()
			if d.find(arg) then
				local f = d.getf(arg, "floor")
				if f == 2 then
					if d.count_monster(arg) == 0 then
						clear_server_timer("easter_zone_check", arg)
						d.setf(arg, "floor", 3)
						d.clear_regen(arg)
						local vid = d.spawn_mob(arg, 4102, 218, 374)
						d.setf(arg, "boss1_vid", vid)
						d.notice(arg, 1803, "", true)
						d.notice(arg, 1804, "", true)
						server_loop_timer("easter_zone_check2", 120, arg)
					end
				end
			else
				easter_zone2.clear(arg)
			end
		end

		when easter_zone_wave.server_timer begin
			local arg = get_server_timer_arg()
			clear_server_timer("easter_zone_wave", arg)
			if d.find(arg) then
				local f = d.getf(arg, "floor")
				if f == 21 then
					d.regen_file(arg, "data/dungeon/easter2020/regen_4b.txt")
					d.setf(arg, "monster_c", d.count_monster(arg))
					d.notice(arg, 1805, "", true)
				elseif f == 23 then
					d.spawn_mob(arg, 4103, 594, 547)
				else
					d.regen_file(arg, "data/dungeon/easter2020/regen_1a.txt")
					server_loop_timer("easter_zone_check", 2, arg)
				end
			else
				easter_zone2.clear(arg)
			end
		end

		when 8461.kill with pc.in_dungeon() begin
			local idx = pc.get_map_index()
			if idx >= 1570000 and idx < 1580000 then
				local f = d.getf(idx, "floor")
				if f == 1 then
					d.setf(idx, "floor", 2)
					server_timer("easter_zone_wave", 3, idx)
					d.notice(idx, 1806, "", true)
				elseif f == 8 then
					if npc.get_vid() == d.getf(idx, "unique_vid") then
						d.setf(idx, "floor", 9)
						game.drop_item(30790, 1)
						d.kill_all_monsters(idx)
						d.notice(idx, 1808, "", true)
					else
						d.notice(idx, 1807, "", true)
					end
				end
			end
		end

		when easter_zone2_prepare.server_timer begin
			local arg = get_server_timer_arg()
			clear_server_timer("easter_zone2_prepare", arg)
			if d.find(arg) then
				d.spawn_mob(arg, 8461, 201, 281)
				
				server_timer("easter_zone2_end", 2399, arg)
				d.notice(arg, 1809, "40", true)
				d.notice(arg, 1810, "", true)
			else
				easter_zone2.clear(arg)
			end
		end

		when logout begin
			local idx = pc.get_map_index()
			if idx >= 1570000 and idx < 1580000 then
				pc.setf("easter_zone2", "disconnect", get_global_time() + 300)
			end
		end

		when login begin
			local idx = pc.get_map_index()
			local empire = pc.get_empire()
			if idx == 157 then
				pc.warp(easter_zone2.get_out_xy(empire)[1] * 100, easter_zone2.get_out_xy(empire)[2] * 100)
			elseif idx >= 1570000 and idx < 1580000 then
				pc.set_warp_location(easter_zone2.get_out_xy(empire)[3], easter_zone2.get_out_xy(empire)[1], easter_zone2.get_out_xy(empire)[2])
				pc.setf("easter_zone2", "idx", idx)
				pc.setf("easter_zone2", "ch", pc.get_channel_id())
				if d.getf(idx, "floor") == 0 then
					if not party.is_party() then
						d.setf(idx, "floor", 1)
						d.setf(idx, "was_completed", 0)
						d.setf(idx, "out_x", easter_zone2.get_out_xy(empire)[1])
						d.setf(idx, "out_y", easter_zone2.get_out_xy(empire)[2])
						server_timer("easter_zone2_prepare", 1, idx)
					else
						if party.is_leader() then
							d.setf(idx, "floor", 1)
							d.setf(idx, "was_completed", 0)
							d.setf(idx, "out_x", easter_zone2.get_out_xy(empire)[1])
							d.setf(idx, "out_y", easter_zone2.get_out_xy(empire)[2])
							server_timer("easter_zone2_prepare", 1, idx)
						end
					end
				end
			end
		end

		when 9308.chat."A new territory?!?" begin
			local lang = pc.get_language()
			
			say_title(string.format("%s:", mob_name(9308)))
			say("")
			say(string.format(gameforge[lang].common.dungeon_1, pc.get_name()))
			local mapIdx = pc.get_map_index()
			if mapIdx != 3 and mapIdx != 23 and mapIdx != 43 then
				return
			end
			
			say(gameforge[lang].common.dungeon_2)
			local agree = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			say_title(string.format("%s:", mob_name(9308)))
			say("")
			if agree == 2 then
				say(gameforge[lang].common.dungeon_3)
				return
			end
			
			local goahead = 1
			local rejoinTIME = pc.getf("easter_zone2", "disconnect") - get_global_time()
			if rejoinTIME > 0 then
				local rejoinIDX = pc.getf("easter_zone2", "idx")
				if rejoinIDX > 0 then
					local rejoinCH = pc.getf("easter_zone2", "ch")
					if rejoinCH != 0 and rejoinCH != pc.get_channel_id() then
						say(string.format(gameforge[lang].common.dungeon_26, rejoinCH))
						return
					end
					
					if rejoinCH != 0 and d.find(rejoinIDX) then
						if d.getf(rejoinIDX, "was_completed") == 0 then
							say(gameforge[lang].common.dungeon_4)
							local agree2 = select(gameforge[lang].common.yes, gameforge[lang].common.no)
							if agree2 == 2 then
								say_title(string.format("%s:", mob_name(9308)))
								say("")
								say(gameforge[lang].common.dungeon_3)
								return
							end
							
							local f = d.getf(rejoinIDX, "floor")
							if f >= 1 and f <= 3 then
								goahead = 0
								pc.warp(403700, 2276600, rejoinIDX)
							elseif f >= 4 then
								goahead = 0
								pc.warp(440300, 2268500, rejoinIDX)
							end
						end
					end
				end
			end
			
			if goahead == 1 then
				pc.setf("easter_zone2", "disconnect", 0)
				pc.setf("easter_zone2", "idx", 0)
				pc.setf("easter_zone2", "ch", 0)
				
				say(gameforge[lang].common.dungeon_5)
				say_reward(string.format(gameforge[lang].common.dungeon_6, 45))
				say_reward(string.format(gameforge[lang].common.dungeon_7, 90))
				say_reward(string.format("- %s: %d", item_name(30787), 1))
				if party.is_party() then
					say_reward(gameforge[lang].common.dungeon_8)
				end
				say(gameforge[lang].common.dungeon_9)
				local n = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				say_title(string.format("%s:", mob_name(9308)))
				say("")
				if n == 2 then
					say(gameforge[lang].common.dungeon_3)
					return
				end
				
				local flag = string.format("ww_157_%d", pc.get_channel_id())
				local ww_flag = game.get_event_flag(flag) - get_global_time()
				if ww_flag > 0 then
					say(gameforge[lang].common.dungeon_28)
					say(string.format(gameforge[lang].common.dungeon_29, ww_flag))
					return
				end
				
				myname = pc.get_name()
				result, cooldown, name = d.check_entrance(45, 90, 30787, 0, 1, "easter_zone2.cooldown")
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
						say(string.format(gameforge[lang].common.dungeon_18, 1, item_name(30787)))
					else
						say(string.format(gameforge[lang].common.dungeon_17, name, 1, item_name(30787)))
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
					d.remove_item(30787, 1, 0, 0, 30788, 255, 30789, 255, 30790, 255, 30791, 255, 30792, 255, 30793, 255, 3600, "easter_zone2")
					game.set_event_flag(string.format("ww_157_%d", pc.get_channel_id()), get_global_time() + 5)
					d.join_cords(157, 4037, 22766)
				end
			end
		end
	end
end

