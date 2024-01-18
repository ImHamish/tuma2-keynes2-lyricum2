quest arboreal_craft begin
	state start begin
		when 20413.chat."Open the Refinament Window" with pc.get_level() >= 90 begin
			command("cube open")
			setskin(NOWINDOW)
		end
	end
end

