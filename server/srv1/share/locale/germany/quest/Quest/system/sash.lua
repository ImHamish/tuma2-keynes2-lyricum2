quest sash begin
	state start begin
		when 60003.chat."Transfer costume bonuses" begin
			say_title(string.format("%s:", mob_name(60003)))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].attr_transfer._1)
			say(gameforge[lang].attr_transfer._2)
			say("")
			say(gameforge[lang].attr_transfer._3)
			say(gameforge[lang].attr_transfer._4)
			say("")
			say(gameforge[lang].attr_transfer._5)
			say("")
			say(gameforge[lang].attr_transfer._6)
			say("")
			local confirm = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			if confirm == 2 then
				return
			end
			
			command("attrtransfer open")
			setskin(NOWINDOW)
		end

		when 60003.chat."Shoulder Sash" begin
			say_title(string.format("%s:", mob_name(60003)))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].sash._4)
			say("")
			local menu = select(gameforge[lang].sash._5,
								gameforge[lang].sash._6,
								gameforge[lang].sash._7,
								gameforge[lang].common.close)
			if menu == 1 then
				setskin(NOWINDOW)
				npc.open_shop(58)
			elseif menu == 2 then
				say_title(string.format("%s:", mob_name(60003)))
				say("")
				say(gameforge[lang].sash._1)
				say("")
				if select(gameforge[lang].common.yes, gameforge[lang].common.no) == 2 then
					return
				end
				
				pc.open_sash(true)
				setskin(NOWINDOW)
			elseif menu == 3 then
				say_title(string.format("%s:", mob_name(60003)))
				say("")
				say(gameforge[lang].sash._2)
				say(gameforge[lang].sash._3)
				say("")
				if select(gameforge[lang].common.yes, gameforge[lang].common.no) == 2 then
					return
				end
				
				pc.open_sash(false)
				setskin(NOWINDOW)
			end
		end
	end
end
