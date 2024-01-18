quest PetSystemIncubator begin
	state start begin
		when 55401.use or 55402.use or 55403.use or 55404.use or 55405.use or 55406.use or 55407.use or 55408.use or 55409.use or 55410.use or 55411.use begin
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			newpet.EggRequest(item.get_vnum())
			cmdchat(string.format("OpenPetIncubator %d ", (item.get_vnum()-55401)))
		end
	end
end

