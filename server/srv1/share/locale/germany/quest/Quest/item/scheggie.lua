quest scheggie begin
	state start begin
		when 86053.use begin
			local c = 10
			local m = pc.count_item(86053)
			if m >= c then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(86053, c)
				pc.give_item2(86056, 1)
			else
				syschat(string.format(gameforge[pc.get_language()].cristalli._1, c-m, item_name(86053), item_name(86056)))
			end
		end

		when 86054.use begin
			local c = 10
			local m = pc.count_item(86054)
			if m >= c then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(86054, c)
				pc.give_item2(86057, 1)
			else
				syschat(string.format(gameforge[pc.get_language()].cristalli._1, c-m, item_name(86054), item_name(86057)))
			end
		end

		when 86055.use begin
			local c = 10
			local m = pc.count_item(86055)
			if m >= c then
				if not pc.can_warp() then
					syschat(gameforge[pc.get_language()].reset_scroll._35)
					return
				end
				
				pc.remove_item(86055, c)
				pc.give_item2(86058, 1)
			else
				syschat(string.format(gameforge[pc.get_language()].cristalli._1, c-m, item_name(86055), item_name(86058)))
			end
		end
	end
end

