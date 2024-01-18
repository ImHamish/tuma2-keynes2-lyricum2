quest aura_system begin
	state start begin
		when 60003.chat."Aura: Refinement" begin
			say_title(string.format("%s:", mob_name(60003)))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].aura_system._4)
			say("")
			local menu = select(gameforge[lang].aura_system._5,
								gameforge[lang].aura_system._6,
								gameforge[lang].aura_system._7,
								gameforge[lang].aura_system._8,
								gameforge[lang].common.close)
			if menu == 1 then
				say_title(string.format("%s:", mob_name(60003)))
				say("")
				say(gameforge[lang].aura_system._2)
				say("")
				if select(gameforge[lang].common.yes, gameforge[lang].common.no) == 2 then
					return
				end
				
				game.open_aura_growth_window()
				setskin(NOWINDOW)
			elseif menu == 2 then
				say_title(string.format("%s:", mob_name(60003)))
				say("")
				say(gameforge[lang].aura_system._3)
				say("")
				if select(gameforge[lang].common.yes, gameforge[lang].common.no) == 2 then
					return
				end
				
				game.open_aura_evolve_window()
				setskin(NOWINDOW)
			elseif menu == 3 then
				say_title(string.format("%s:", mob_name(60003)))
				say("")
				say(gameforge[lang].aura_system._1)
				say("")
				if select(gameforge[lang].common.yes, gameforge[lang].common.no) == 2 then
					return
				end
				
				game.open_aura_absorb_window()
				setskin(NOWINDOW)
			elseif menu == 4 then
			setskin(NOWINDOW)
			npc.open_shop(60)
			end
		end
	end
end
