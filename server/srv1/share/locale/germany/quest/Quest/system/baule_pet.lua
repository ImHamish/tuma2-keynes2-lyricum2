quest baule_pet begin
	state start begin
		when 88967.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			say_title(string.format("%s:", item_name(88967)))
			local lang = pc.get_language()
			say(gameforge[lang].baule_pet._1)
			local s = select(item_name(55401), item_name(55402), item_name(55403), item_name(55404), item_name(55405), item_name(55406), item_name(55407), item_name(55408), item_name(55409), item_name(55410), item_name(55411), gameforge[pc.get_language()].common.close)
			if s == 12 then
				return
			end

			pc.remove_item(88967, 1)
			local r = {[0] = 55401, [1] = 55402, [2] = 55403, [3] = 55404, [4] = 55405, [5] = 55406, [6] = 55407, [7] = 55408, [8] = 55409, [9] = 55410, [10] = 55411}
			pc.give_item2(r[s-1], 1)
		end
	end
end

