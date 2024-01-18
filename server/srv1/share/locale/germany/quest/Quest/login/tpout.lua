quest mapblock begin
	state start begin
		when login begin
			if pc.is_gm() then
				syschat(string.format("Map index: %u", pc.get_map_index()))
			else
				local data = {
								[210] = 100,
								[73] = 75,
								[72] = 75,
								[304] = 90,
								[303] = 90,
								[302] = 90,
								[301] = 90,
								[74] = 105,
								[221] = 50
				}
				
				if data[pc.get_map_index()] ~= nil and pc.get_level() < data[pc.get_map_index()] then
					warp_to_village()
				end
			end
		end
	end
end

