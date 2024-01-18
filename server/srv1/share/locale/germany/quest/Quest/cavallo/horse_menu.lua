quest horse_menu begin
	state start begin
		when 20030.click or 20101.click or 20102.click or 20103.click or 20104.click or 20105.click or 20106.click or 20107.click or 20108.click or 20109.click begin
			if horse.is_mine() then
				local lang = pc.get_language()
				say(gameforge[lang].horse_menu._1)
				
				local s = 0
				if horse.is_dead() then
					s = select(gameforge[lang].horse_menu._2, gameforge[lang].horse_menu._4, gameforge[lang].horse_menu._5, gameforge[lang].horse_menu._6)
				else
					s = select(gameforge[lang].horse_menu._3, gameforge[lang].horse_menu._4, gameforge[lang].horse_menu._5, gameforge[lang].horse_menu._6)
				end
				
				if s == 1 then
					if horse.is_dead() then
						horse.revive()
						setskin(NOWINDOW)
					else
						local food = horse.get_grade() + 50054 - 1
						if pc.countitem(food) > 0 then
							pc.removeitem(food, 1)
							horse.feed()
							setskin(NOWINDOW)
						else
							say(gameforge[lang].horse_menu._7)
							say(string.format(gameforge[lang].horse_menu._8, item_name(food)))
						end
					end
				elseif s == 2 then
					horse.ride()
					setskin(NOWINDOW)
				elseif s == 3 then
					horse.unsummon()
					setskin(NOWINDOW)
				else
					setskin(NOWINDOW)
				end
			end
		end
	end
end

