quest horse_summon begin
	state start begin
		function get_horse_summon_prob_pct() 
			local prob = {10, 15, 20, 30, 40, 50, 60, 70, 80, 90, 100}
			local skill_level = pc.get_skill_level(131) + 1
			if skill_level > 11 then
				return 100
			else
				return prob[skill_level]
			end
		end

		when 50053.use with horse.get_grade() == 0 begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			say_title(string.format("%s:", item_name(50053)))
			say("")
			say(gameforge[pc.get_language()].horse_summon._1)
		end

		when 50053.use with horse.get_level() != 21 begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			say_title(string.format("%s:", item_name(50053)))
			say("")
			say(gameforge[pc.get_language()].horse_summon._2)
		end

		when 50053.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			local lang = pc.get_language()
			if pc.is_mount() == true then
				syschat(gameforge[lang].horse_summon._3)
				return
			end
			
			local horse_data = {100, 200, 300}
			local idx = item.get_vnum() - 50050
			if item.get_vnum() == 50149 then
				if pc.getsp() >= 500 then
					if number(1, 100) <= horse_summon.get_horse_summon_prob_pct() then
						syschat(gameforge[lang].horse_summon._4)
						horse.summon()
					else
						syschat(gameforge[lang].horse_summon._5)
					end
					
					pc.change_sp(-horse_data[idx])
				else
					syschat(gameforge[lang].horse_summon._6)
				end
			elseif item.get_vnum() != 50149 then
				if pc.getsp() >= horse_data[idx] then
					if number(1, 100) <= horse_summon.get_horse_summon_prob_pct() then
						syschat(gameforge[lang].horse_summon._4)
						horse.summon()
					else
						syschat(gameforge[lang].horse_summon._5)
					end
					
					pc.change_sp(-horse_data[idx])
				else
					syschat(string.format(gameforge[lang].horse_summon._7, horse_data[idx]))
				end
			end
		end
	end
end

