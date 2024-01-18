quest bp_shop6 begin
	state start begin
		when 20305.click or
			 20325.click or
			 20345.click begin
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].common.choose)
			local s = select(gameforge[lang].bp_shop._1, gameforge[lang].bp_shop._3,gameforge[lang].common.close)
			if s == 1 then
			setskin(NOWINDOW)
			npc.open_shop(16)
			elseif s == 2 then
			setskin(NOWINDOW)
			npc.open_shop(35)
			end
		end
	end
end