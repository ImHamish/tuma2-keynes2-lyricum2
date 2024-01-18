quest baule_start begin
	state start begin
		when 50188.use begin
			local lang = pc.get_language()
			if pc.get_level() >= 10 then
				if not pc.can_warp() then
					syschat(gameforge[lang].reset_scroll._35)
					return
				end
				
				pc.remove_item(50188, 1)
				chat(string.format(gameforge[lang].baule_start._1, item_name(50188)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(50189, 1)
				pc.give_item2(71085, 1)
				pc.give_item2(88922, 1)
				pc.give_item2(88923, 1)
				pc.give_item2(88924, 1)
				pc.give_item2(88925, 1)
				pc.give_item2(88926, 1)
				pc.give_item2(88927, 1)
				pc.give_item2(88929, 1)
				pc.give_item2(88918, 1)
				pc.give_item2(50134, 1)
			else
				if pc.get_map_index() >= 10000 then
					syschat(gameforge[pc.get_language()].common.cannot_use_dung)
					return
				end
				
				say_title(string.format("%s:", item_name(50188)))
				say("")
				say(string.format(gameforge[lang].baule_start._3, 10))
			end
		end

		when 50189.use begin
			local lang = pc.get_language()
			if pc.get_level() >= 20 then
				if not pc.can_warp() then
					syschat(gameforge[lang].reset_scroll._35)
					return
				end
				
				pc.remove_item(50189,1)
				chat(string.format(gameforge[lang].baule_start._1, item_name(50189)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(50190, 1)
				pc.give_item2(88959, 50)
				pc.give_item2(72001, 1)
			else
				if pc.get_map_index() >= 10000 then
					syschat(gameforge[pc.get_language()].common.cannot_use_dung)
					return
				end
				
				say_title(string.format("%s:", item_name(50189)))
				say("")
				say(string.format(gameforge[lang].baule_start._3, 20))
			end
		end

		when 50190.use begin
			local lang = pc.get_language()
			if pc.get_level() >= 30 then
				if not pc.can_warp() then
					syschat(gameforge[lang].reset_scroll._35)
					return
				end
				
				pc.remove_item(50190, 1)
				chat(string.format(gameforge[lang].baule_start._1, item_name(50190)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(50191, 1)
				pc.give_item2(72001, 1)
				pc.give_item2(72006, 1)
				pc.give_item2(88959, 100)
			else
				if pc.get_map_index() >= 10000 then
					syschat(gameforge[pc.get_language()].common.cannot_use_dung)
					return
				end
				
				say_title(string.format("%s:", item_name(50190)))
				say("")
				say(string.format(gameforge[lang].baule_start._3, 30))
			end
		end

		when 50191.use begin
			local lang = pc.get_language()
			if pc.get_level() >= 40 then
				if not pc.can_warp() then
					syschat(gameforge[lang].reset_scroll._35)
					return
				end
				
				pc.remove_item(50191, 1)
				chat(string.format(gameforge[lang].baule_start._1, item_name(50191)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(50192, 1)
				pc.give_item2(90027, 250)
				pc.give_item2(89106, 2)
			else
				if pc.get_map_index() >= 10000 then
					syschat(gameforge[pc.get_language()].common.cannot_use_dung)
					return
				end
				
				say_title(string.format("%s:", item_name(50191)))
				say("")
				say(string.format(gameforge[lang].baule_start._3, 40))
			end
		end

		when 50192.use begin
			local lang = pc.get_language()
			if pc.get_level() >= 50 then
				if not pc.can_warp() then
					syschat(gameforge[lang].reset_scroll._35)
					return
				end
				
				pc.remove_item(50192, 1)
				chat(string.format(gameforge[lang].baule_start._1, item_name(50192)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(50193, 1)
				pc.give_item2(88916, 1)
				pc.give_item2(88917, 1)
				pc.give_item2(88919, 1)
				pc.give_item2(88920, 1)
				pc.give_item2(88921, 1)
				pc.give_item2(30325, 1)
			else
				if pc.get_map_index() >= 10000 then
					syschat(gameforge[pc.get_language()].common.cannot_use_dung)
					return
				end
				
				say_title(string.format("%s:", item_name(50192)))
				say("")
				say(string.format(gameforge[lang].baule_start._3, 50))
			end
		end

		when 50193.use begin
			local lang = pc.get_language()
			if pc.get_level() >= 60 then
				if not pc.can_warp() then
					syschat(gameforge[lang].reset_scroll._35)
					return
				end
				
				pc.remove_item(50193, 1)
				chat(string.format(gameforge[lang].baule_start._1, item_name(50193)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(50194, 1)
				pc.give_item2(72006, 1)
				pc.give_item2(71153, 1)
			else
				if pc.get_map_index() >= 10000 then
					syschat(gameforge[pc.get_language()].common.cannot_use_dung)
					return
				end
				
				say_title(string.format("%s:", item_name(50193)))
				say("")
				say(string.format(gameforge[lang].baule_start._3, 60))
			end
		end
		
		when 50194.use begin
			local lang = pc.get_language()
			if pc.get_level() >= 70 then
				if not pc.can_warp() then
					syschat(gameforge[lang].reset_scroll._35)
					return
				end
				
				pc.remove_item(50194, 1)
				chat(string.format(gameforge[lang].baule_start._1, item_name(50194)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(50195, 1)
				pc.give_item2(30320, 2)
				pc.give_item2(71153, 1)
				pc.give_item2(90027, 250)
			else
				if pc.get_map_index() >= 10000 then
					syschat(gameforge[pc.get_language()].common.cannot_use_dung)
					return
				end
				
				say_title(string.format("%s:", item_name(50194)))
				say("")
				say(string.format(gameforge[lang].baule_start._3, 70))
			end
		end
		
		when 50195.use begin
			local lang = pc.get_language()
			if pc.get_level() >= 80 then
				if not pc.can_warp() then
					syschat(gameforge[lang].reset_scroll._35)
					return
				end
				
				pc.remove_item(50195, 1)
				chat(string.format(gameforge[lang].baule_start._1, item_name(50195)))
				chat(gameforge[lang].baule_start._2)
				pc.give_item2(30179, 2)
				pc.give_item2(72006, 1)
				pc.give_item2(88959, 350)
			else
				if pc.get_map_index() >= 10000 then
					syschat(gameforge[pc.get_language()].common.cannot_use_dung)
					return
				end
				
				say_title(string.format("%s:", item_name(50195)))
				say("")
				say(string.format(gameforge[lang].baule_start._3, 80))
			end
		end
		
	end
end

