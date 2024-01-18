quest biologist_change begin
	state start begin
		when 164401.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			pc.open_biologist_change()
		end
	end
end

