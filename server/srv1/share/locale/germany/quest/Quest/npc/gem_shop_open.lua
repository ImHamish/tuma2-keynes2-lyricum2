quest gem_shop_open begin
	state start begin
		when 20504.click begin
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].common.choose)
			local s = select(gameforge[lang].gaya_shop._1,gameforge[lang].gaya_shop._2, gameforge[lang].gaya_shop._3, gameforge[lang].common.close)
			if s == 1 then
			setskin(NOWINDOW)
			npc.open_shop(55)
			elseif s == 2 then
			command("cube open")
			setskin(NOWINDOW)
			elseif s == 3 then
			pc.open_gem_shop()
			end
		end
	end
end