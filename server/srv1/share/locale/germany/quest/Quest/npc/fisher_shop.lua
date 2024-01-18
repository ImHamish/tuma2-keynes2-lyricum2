quest fisher_store begin
	state start begin
		when 9009.chat."Fishing Articles" begin
			npc.open_shop(2)
			setskin(NOWINDOW)
		end
	end
end