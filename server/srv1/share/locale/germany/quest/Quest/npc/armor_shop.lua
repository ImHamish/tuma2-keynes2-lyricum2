quest armor_shop begin
	state start begin
		when 9002.click begin
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].common.choose)
			local s = select(gameforge[lang].armor_shop._1, gameforge[lang].armor_shop._2, gameforge[lang].common.close)
			if s == 1 then
			setskin(NOWINDOW)
			npc.open_shop(4)
			elseif s == 2 then
			setskin(NOWINDOW)
			npc.open_shop(10)
			end
		end
	end
end