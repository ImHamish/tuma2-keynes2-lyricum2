quest regenbogen begin
	state start begin
		when 50512.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			say_title(string.format("%s", item_name(50512)))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].regenbogen._1)
			say(gameforge[lang].regenbogen._2)
			say(gameforge[lang].regenbogen._3)
			if select(gameforge[lang].common.yes, gameforge[lang].common.no) == 1 then
				say_title(string.format("%s", item_name(50512)))
				say("")
				local skill_group = pc.get_skill_group()
				if skill_group == 0 then
					say(gameforge[lang].regenbogen._4)
				elseif skill_group == 1 or skill_group == 2 then
					if not pc.can_warp() then
						say(gameforge[lang].regenbogen._5)
					end
					
					table.foreachi(special.active_skill_list[pc.get_job() + 1][skill_group],
									function(r, skill) pc.set_skill_level(skill, 40)
					end)
					pc.remove_item(50512,1)
					pc.set_skill_level(128, 40)
					pc.set_skill_level(127, 40)
					pc.set_skill_level(126, 40)
					pc.set_skill_level(124, 40)
					pc.set_skill_level(122, 2)
					pc.set_skill_level(121, 40)
					say(gameforge[lang].regenbogen._5)
					wait()
					pc.warp(pc.get_x() * 100, pc.get_y() * 100)
				end
			end
		end
	end
end

