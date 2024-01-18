quest baule_lega begin
	state start begin
		when 50136.use begin
			if pc.get_level() >= 1 then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(50136, 1)
				local lang = pc.get_language()
				chat(string.format(gameforge[lang].baule_start._1, item_name(50136)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(53295, 1)
				pc.give_item2(58008, 1)
				pc.give_item2(85160, 1)
				pc.give_item2(30907, 1)
				pc.give_item2(50512, 1)
				pc.give_item2(80005, 1)
				pc.give_item2(40006, 1)
				pc.give_item2(71153, 1)
				pc.give_item2(71153, 1)
				pc.give_item2(71153, 1)
				pc.give_item2(50259, 50)
				if pc.get_sex() == 0 then
					pc.give_item2(84290, 1)
					pc.give_item2(84292, 1)
				elseif pc.get_sex() == 1 then
					pc.give_item2(84291, 1)
					pc.give_item2(84293, 1)
				end
			end
		end
		
		when 50134.use begin
			if pc.get_level() >= 1 then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(50134, 1)
				local lang = pc.get_language()
				chat(string.format(gameforge[lang].baule_start._1, item_name(50134)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(85048, 1)
				pc.give_item2(40005, 1)
				pc.give_item2(53279, 1)
				pc.give_item2(58002, 1)
				pc.give_item2(71153, 1)
				pc.give_item2(50512, 1)
				if pc.get_sex() == 0 then
					pc.give_item2(41883, 1)
					pc.give_item2(45623, 1)
				elseif pc.get_sex() == 1 then
					pc.give_item2(41885, 1)
					pc.give_item2(45625, 1)
				end
			end
		end
		
		when 50137.use begin
			if pc.get_level() >= 1 then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(50137, 1)
				local lang = pc.get_language()
				chat(string.format(gameforge[lang].baule_start._1, item_name(50137)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(50512, 1)
				pc.give_item2(40907, 1)
				pc.give_item2(53298, 1)
				pc.give_item2(58009, 1)
				pc.give_item2(85083, 1)
				pc.give_item2(40003, 1)
				pc.give_item2(80007, 2)
				pc.give_item2(40007, 1)
				if pc.get_sex() == 0 then
					pc.give_item2(40193, 1)
					pc.give_item2(45242, 1)
				elseif pc.get_sex() == 1 then
					pc.give_item2(40192, 1)
					pc.give_item2(45243, 1)
				end
			end
		end
		
		when 50135.use begin
			if pc.get_level() >= 1 then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(50135, 1)
				local lang = pc.get_language()
				chat(string.format(gameforge[lang].baule_start._1, item_name(50135)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(53299, 1)
				pc.give_item2(40908, 1)
				pc.give_item2(85170, 1)
				pc.give_item2(40016, 1)
				if pc.get_sex() == 0 then
					pc.give_item2(84300, 1)
					pc.give_item2(84302, 1)
				elseif pc.get_sex() == 1 then
					pc.give_item2(84301, 1)
					pc.give_item2(84303, 1)
				end
			end
		end
		
		when 50133.use begin
			if pc.get_level() >= 1 then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(50133, 1)
				local lang = pc.get_language()
				chat(string.format(gameforge[lang].baule_start._1, item_name(50133)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(53299, 1)
				pc.give_item2(40908, 1)
				pc.give_item2(85170, 1)
				pc.give_item2(40018, 1)
				if pc.get_sex() == 0 then
					pc.give_item2(84320, 1)
					pc.give_item2(84322, 1)
				elseif pc.get_sex() == 1 then
					pc.give_item2(84321, 1)
					pc.give_item2(84323, 1)
				end
			end
		end
		
		when 50144.use begin
			if pc.get_level() >= 1 then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(50144, 1)
				local lang = pc.get_language()
				chat(string.format(gameforge[lang].baule_start._1, item_name(50144)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(50512, 1)
				pc.give_item2(40907, 1)
				pc.give_item2(53300, 1)
				pc.give_item2(58010, 1)
				pc.give_item2(85171, 1)
				pc.give_item2(40003, 1)
				pc.give_item2(80007, 2)
				pc.give_item2(40019, 1)
				if pc.get_sex() == 0 then
					pc.give_item2(84350, 1)
					pc.give_item2(84352, 1)
				elseif pc.get_sex() == 1 then
					pc.give_item2(84351, 1)
					pc.give_item2(84353, 1)
				end
			end
		end
		
		when 50145.use begin
			if pc.get_level() >= 1 then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(50145, 1)
				local lang = pc.get_language()
				chat(string.format(gameforge[lang].baule_start._1, item_name(50145)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(85179, 1)
				pc.give_item2(53301, 1)
				pc.give_item2(58011, 1)
				pc.give_item2(30907, 1)
				pc.give_item2(50512, 1)
				pc.give_item2(80005, 1)
				pc.give_item2(40020, 1)
				pc.give_item2(71153, 1)
				pc.give_item2(71153, 1)
				pc.give_item2(71153, 1)
				pc.give_item2(50259, 50)
				if pc.get_sex() == 0 then
					pc.give_item2(84370, 1)
					pc.give_item2(84372, 1)
				elseif pc.get_sex() == 1 then
					pc.give_item2(84371, 1)
					pc.give_item2(84373, 1)
				end
			end
		end
		
	end
end

