quest bp_shop3 begin
	state start begin
		when 20302.click or
			 20322.click or
			 20342.click begin
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].common.choose)
			local s = select(gameforge[lang].bp_shop._1, gameforge[lang].bp_shop._3,gameforge[lang].common.close)
			if s == 1 then
			setskin(NOWINDOW)
			npc.open_shop(13)
			elseif s == 2 then
			setskin(NOWINDOW)
			npc.open_shop(34)
			end
		end
	end
end