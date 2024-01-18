quest cristalli begin
	state start begin
		when 30277.use begin
			local c = 100
			local m = pc.count_item(30277)
			if m >= c then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(30277, c)
				pc.give_item2(30278, 1)
			else
				syschat(string.format(gameforge[pc.get_language()].cristalli._1, c-m, item_name(30277), item_name(30278)))
			end
		end

		when 30279.use begin
			local c = 100
			local m = pc.count_item(30279)
			if m >= c then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(30279, c)
				pc.give_item2(30280, 1)
			else
				syschat(string.format(gameforge[pc.get_language()].cristalli._1, c-m, item_name(30279), item_name(30280)))
			end
		end
	end
end

