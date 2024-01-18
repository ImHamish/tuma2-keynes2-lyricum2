quest gaya_baule begin
	state start begin
		when 39084.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			pc.remove_item(39084, 1)
			pc.change_gaya(1)
		end

		when 39085.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			pc.remove_item(39085, 1)
			pc.change_gaya(3)
		end

		when 39086.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			pc.remove_item(39086, 1)
			pc.change_gaya(5)
		end
	end
end

