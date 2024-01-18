quest baule_lega begin
	state start begin
		when 10960.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			local drop = {
								{70401, 5},
								{70401, 10},
								{84999, 3},
								{84999, 6},
								{86062, 4},
								{86062, 8},
								{86066, 3},
								{39015, 3},
								{50066, 1},
								{86060, 100},
								{86060, 150},
								{86051, 3},
								{86051, 6},
								{39076, 1},
								{39077, 1},
								{39078, 1}
			}
			pc.remove_item(10960, 1)
			local c = number(1, 17)
			local lang = pc.get_language()
			chat(string.format(gameforge[lang].baule_lega._1, item_name(10960)))
			chat(gameforge[lang].baule_lega._2)
			pc.give_item2(drop[c][1], drop[c][2])
			pc.give_gold(180000000)
		end

		when 10961.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			local drop = {
								{71084, 100},
								{71084, 200},
								{70401, 2},
								{70401, 4},
								{86062, 1},
								{86062, 2},
								{164407, 1},
								{70613, 1},
								{50926, 4},
								{39070, 1},
								{39066, 1},
								{39067, 1}
			}
			pc.remove_item(10961, 1)
			local c = number(1, 12)
			local lang = pc.get_language()
			chat(string.format(gameforge[lang].baule_lega._1, item_name(10961)))
			chat(gameforge[lang].baule_lega._2)
			pc.give_item2(drop[c][1], drop[c][2])
			pc.give_gold(100000000)
		end

		when 10962.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			local drop = {
								{71084, 25},
								{71084, 50},
								{71084, 75},
								{25040, 2},
								{25040, 4},
								{39068, 1},
								{39065, 1},
								{39070, 1}
			}
			pc.remove_item(10962, 1)
			local c = number(1, 8)
			local lang = pc.get_language()
			chat(string.format(gameforge[lang].baule_lega._1, item_name(10962)))
			chat(gameforge[lang].baule_lega._2)
			pc.give_item2(drop[c][1], drop[c][2])
			pc.give_gold(50000000)
		end

		when 10963.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			local drop = {
								{70401, 4},
								{70401, 8},
								{84999, 2},
								{84999, 4},
								{86062, 3},
								{86062, 6},
								{86066, 2},
								{39015, 2},
								{50066, 1},
								{86060, 50},
								{86060, 100},
								{86051, 2},
								{86051, 4},
								{39079, 1},
								{39080, 1},
								{39081, 1}
			}
			pc.remove_item(10963, 1)
			local c = number(1, 17)
			local lang = pc.get_language()
			chat(string.format(gameforge[lang].baule_lega._1, item_name(10963)))
			chat(gameforge[lang].baule_lega._2)
			pc.give_item2(drop[c][1], drop[c][2])
			pc.give_gold(100000000)
		end

		when 10964.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			local drop = {
								{71084, 80},
								{71084, 160},
								{70401, 1},
								{70401, 2},
								{86062, 1},
								{164407, 1},
								{70613, 1},
								{50926, 2},
								{25040, 4},
								{39072, 1},
								{39073, 1},
								{39074, 1}
			}
			pc.remove_item(10964, 1)
			local c = number(1, 12)
			local lang = pc.get_language()
			chat(string.format(gameforge[lang].baule_lega._1, item_name(10964)))
			chat(gameforge[lang].baule_lega._2)
			pc.give_item2(drop[c][1], drop[c][2])
			pc.give_gold(70000000)
		end

		when 10965.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			local drop = {
								{71084, 15},
								{71084, 30},
								{71084, 45},
								{25040, 1},
								{25040, 2},
								{39069, 1},
								{39068, 1},
								{39065, 1}
			}
			pc.remove_item(10965, 1)
			local c = number(1, 8)
			local lang = pc.get_language()
			chat(string.format(gameforge[lang].baule_lega._1, item_name(10965)))
			chat(gameforge[lang].baule_lega._2)
			pc.give_item2(drop[c][1], drop[c][2])
			pc.give_gold(30000000)
		end

		when 10966.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			local drop = {
								{70401, 3},
								{70401, 6},
								{84999, 1},
								{84999, 2},
								{86062, 2},
								{86062, 4},
								{86066, 1},
								{39015, 1},
								{50066, 1},
								{86060, 40},
								{86060, 80},
								{86051, 1},
								{86051, 2},
								{39067, 1},
								{39082, 1},
								{39083, 1}
			}
			pc.remove_item(10966, 1)
			local c = number(1, 17)
			local lang = pc.get_language()
			chat(string.format(gameforge[lang].baule_lega._1, item_name(10966)))
			chat(gameforge[lang].baule_lega._2)
			pc.give_item2(drop[c][1], drop[c][2])
			pc.give_gold(70000000)
		end

		when 10967.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			local drop = {
								{71084, 70},
								{71084, 140},
								{70401, 1},
								{86062, 1},
								{50926, 1},
								{25040, 1},
								{25040, 2},
								{39065, 1},
								{39070, 1},
								{39075, 1}
			}
			pc.remove_item(10967, 1)
			local c = number(1, 10)
			local lang = pc.get_language()
			chat(string.format(gameforge[lang].baule_lega._1, item_name(10967)))
			chat(gameforge[lang].baule_lega._2)
			pc.give_item2(drop[c][1], drop[c][2])
			pc.give_gold(40000000)
		end

		when 10968.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			local drop = {
								{71084, 10},
								{71084, 15},
								{71084, 20},
								{25040, 1},
								{25040, 2},
								{39071, 1},
								{39069, 1},
								{39068, 1}
			}
			pc.remove_item(10968, 1)
			local c = number(1, 8)
			local lang = pc.get_language()
			chat(string.format(gameforge[lang].baule_lega._1, item_name(10968)))
			chat(gameforge[lang].baule_lega._2)
			pc.give_item2(drop[c][1], drop[c][2])
			pc.give_gold(10000000)
		end
	end
end

