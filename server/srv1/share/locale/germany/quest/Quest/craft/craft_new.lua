quest craft_new begin
	state start begin
		when 20016.chat."Craft" begin
			command("cube open")
			setskin(NOWINDOW)
		end
	end
end

