quest gaya_shop begin
	state start begin
		when 20503.click begin
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].common.choose)
			local s = select(gameforge[lang].gaya_shop._1,gameforge[lang].gaya_shop._2, gameforge[lang].common.close)
			if s == 1 then
			setskin(NOWINDOW)
			npc.open_shop(55)
			elseif s == 2 then
			command("cube open")
			setskin(NOWINDOW)
			end
		end
	end
end