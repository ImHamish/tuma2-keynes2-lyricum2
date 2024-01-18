quest farah_shop begin
	state start begin
		when 9527.click begin
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].common.choose)
			local s = select(gameforge[lang].farah_shop._1,gameforge[lang].farah_shop._2,gameforge[lang].farah_shop._3,gameforge[lang].christmas_shop._1,gameforge[lang].farah_shop._4,gameforge[lang].common.close)
			if s == 1 then
			setskin(NOWINDOW)
			npc.open_shop(64)
			elseif s == 2 then
			setskin(NOWINDOW)
			npc.open_shop(68)
			elseif s == 3 then
			setskin(NOWINDOW)
			npc.open_shop(67)
			elseif s == 4 then
			setskin(NOWINDOW)
			npc.open_shop(69)
			elseif s == 5 then
			setskin(NOWINDOW)
			npc.open_shop(70)
			end
		end
	end
end