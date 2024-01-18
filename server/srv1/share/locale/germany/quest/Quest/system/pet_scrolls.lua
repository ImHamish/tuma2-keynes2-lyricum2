quest pet_scrolls begin
	state start begin
		when 86076.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			say_title(string.format("%s:", item_name(86076)))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].pet_scrolls._1)
			local s = select(gameforge[lang].pet_scrolls._2, gameforge[lang].pet_scrolls._3, gameforge[lang].pet_scrolls._4, gameforge[lang].pet_scrolls._5, gameforge[lang].pet_scrolls._6, gameforge[lang].pet_scrolls._7, gameforge[lang].pet_scrolls._8, gameforge[lang].pet_scrolls._9, gameforge[lang].pet_scrolls._10, gameforge[lang].pet_scrolls._11, gameforge[lang].pet_scrolls._12, gameforge[lang].pet_scrolls._13, gameforge[lang].common.close)
			if s != 13 then
				say_title(string.format("%s:", item_name(86076)))
				say("")
				if not pc.can_warp() then
					say(gameforge[lang].reset_scroll._35)
					return
				end
				
				say(gameforge[lang].pet_scrolls._14)
				local j = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				if j == 1 then
					local ret = newpet.reset_skill(s)
					if ret == 1 then
						say_title(string.format("%s:", item_name(86076)))
						say("")
						say(gameforge[lang].pet_scrolls._15)
						pc.remove_item(86076, 1)
					elseif ret == 2 then
						say_title(string.format("%s:", item_name(86076)))
						say("")
						say(gameforge[lang].pet_scrolls._16)
					elseif ret == 3 then
						say_title(string.format("%s:", item_name(86076)))
						say("")
						say(gameforge[lang].pet_scrolls._17)
					end
				end
			end
		end

		when 86077.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			say_title(string.format("%s:", item_name(86077)))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].pet_scrolls._18)
			local s = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			if s == 1 then
				if not pc.can_warp() then
					say_title(string.format("%s:", item_name(86077)))
					say("")
					say(gameforge[lang].reset_scroll._35)
					return
				end
				
				local ret = newpet.reset_skills()
				if ret == 1 then
					say_title(string.format("%s:", item_name(86077)))
					say("")
					say(gameforge[lang].pet_scrolls._19)
					pc.remove_item(86077, 1)
				elseif ret == 2 then
					say_title(string.format("%s:", item_name(86077)))
					say("")
					say(gameforge[lang].pet_scrolls._16)
				elseif ret == 3 then
					say_title(string.format("%s:", item_name(86077)))
					say("")
					say(gameforge[lang].pet_scrolls._20)
				end
			end
		end
	end
end

