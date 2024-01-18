quest changelook_system begin
	state start begin
		when 60003.chat."Transmutation" begin
			say_title(string.format("%s:", mob_name(60003)))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].changelook_system._1)
			say(gameforge[lang].changelook_system._2)
			say("")
			say(gameforge[lang].changelook_system._3)
			say("")
			if select(gameforge[lang].common.yes, gameforge[lang].common.no) == 2 then
				return
			end
			
			pc.open_changelook(true)
			setskin(NOWINDOW)
		end
	end
end
