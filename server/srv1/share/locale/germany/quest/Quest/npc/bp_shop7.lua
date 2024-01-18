quest bp_shop7 begin
	state start begin
		when 20306.click or
			 20326.click or
			 20346.click begin
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].common.choose)
			local s = select(gameforge[lang].bp_shop._1, gameforge[lang].bp_shop._3,gameforge[lang].common.close)
			if s == 1 then
			setskin(NOWINDOW)
			npc.open_shop(17)
			elseif s == 2 then
			setskin(NOWINDOW)
			npc.open_shop(37)
			end
		end
	end
end