quest halloween_event begin
	state start begin
		when 9418.chat."Happy Halloween!" begin
			local lang = pc.get_language()
			
			say_size(350,350)
			addimage(25, 10, "h2021_dun_bg1.tga")
			addimage(225, 225, "h2021_npc1.tga")
			say("")
			say("")
			say_title(string.format("%s:", mob_name(9418)))
			say("")
			say(gameforge[lang].halloween_event._7)
			say(gameforge[lang].halloween_event._8)
			
			if game.get_halloween_event_status() != 1 then
				return
			end
			
			say("")
			say(gameforge[lang].halloween_event._4)
			local menu = select(gameforge[lang].halloween_event._9, gameforge[lang].common.close)
			if menu == 1 then
				npc.open_shop(62)
				setskin(NOWINDOW)
			end
		end
		
		when 20416.chat."Happy Halloween!" begin
			local lang = pc.get_language()
			
			say_title(string.format("%s:", mob_name(20416)))
			say("")
			say(string.format(gameforge[lang].halloween_event._1, pc.get_name()))
			say(gameforge[lang].halloween_event._2)
			say(gameforge[lang].halloween_event._3)
			
			if game.get_halloween_event_status() != 1 then
				return
			end
			
			say("")
			say(gameforge[lang].halloween_event._4)
			local menu = select(gameforge[lang].halloween_event._5, gameforge[lang].halloween_event._6, gameforge[lang].common.close)
			if menu == 1 then
				npc.open_shop(61)
				setskin(NOWINDOW)
			elseif menu == 2 then
				command("cube open")
				setskin(NOWINDOW)
			end
		end
		
		when 20701.chat."Happy Halloween!" or 
			 20702.chat."Happy Halloween!" or 
			 20703.chat."Happy Halloween!" or 
			 20704.chat."Happy Halloween!" or 
			 20705.chat."Happy Halloween!" or 
			 20706.chat."Happy Halloween!" or 
			 20707.chat."Happy Halloween!" or 
			 20708.chat."Happy Halloween!" begin
			local lang = pc.get_language()
			
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			say(gameforge[lang].halloween_event._13)
			say(gameforge[lang].halloween_event._14)
			say(gameforge[lang].halloween_event._15)
			
			if game.get_halloween_event_status() != 1 then
				return
			end
			
			say("")
			say(gameforge[lang].halloween_event._4)
			local menu = select(gameforge[lang].halloween_event._6, gameforge[lang].common.close)
			if menu == 1 then
				command("cube open")
				setskin(NOWINDOW)
			end
		end
		
		when 20711.chat."Happy Halloween!" or 
			 20712.chat."Happy Halloween!" or 
			 20713.chat."Happy Halloween!" or 
			 20714.chat."Happy Halloween!" or 
			 20715.chat."Happy Halloween!" or 
			 20716.chat."Happy Halloween!" or 
			 20717.chat."Happy Halloween!" or 
			 20718.chat."Happy Halloween!" begin
			if game.get_halloween_event_status() != 1 then
				say_title(string.format("%s:", mob_name(npc.get_race())))
				say("")
				say("?!?")
				return
			end
			
			local lang = pc.get_language()
			
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			if get_today_count("halloween_event", "daily") > 0 then
				say(gameforge[lang].halloween_event._22)
				return
			end
			
			say(string.format(gameforge[lang].halloween_event._16, pc.get_name()))
			say(gameforge[lang].halloween_event._17)
			say(gameforge[lang].halloween_event._18)
			if pc.get_reborn() < 3 then
				say("")
				say_reward(gameforge[lang].halloween_event._19)
			else
				local reward = number(50, 200)
				inc_today_count("halloween_event", "daily")
				pc.give_skull(reward)
				
				say("")
				say_reward(string.format(gameforge[lang].halloween_event._20, reward))
				say_reward(gameforge[lang].halloween_event._21)
			end
		end
	end
end
