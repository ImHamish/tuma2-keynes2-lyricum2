quest change_yang begin
	state start begin
		when 30094.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			pc.remove_item(30094, 1)
			pc.change_gold(200000 * number(1, 3))
		end

		when 30095.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			pc.remove_item(30095, 1)
			pc.change_gold(400000 * number(1, 3))
		end

		when 30096.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			pc.remove_item(30096, 1)
			pc.change_gold(700000 * number(1, 3))
		end
	end
end

