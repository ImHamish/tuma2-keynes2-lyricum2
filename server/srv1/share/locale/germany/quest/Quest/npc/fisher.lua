quest fisher begin
	state start begin
		when 9009.take begin
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			local vnum = item.get_vnum()
			local lang = pc.get_language()
			if vnum == 27390 or vnum == 27490 then
				say(gameforge[lang].fisher._1)
				say(gameforge[lang].fisher._2)
				return
			end
			
			if not (vnum >= 27200 and vnum <= 27380) and not (vnum >= 27400 and vnum <= 27480) then
				say(gameforge[lang].fisher._3)
				return
			end
			
			if item.get_socket(0) == item.get_value(2) then
				say(gameforge[lang].fisher._4)
				local s = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				say_title(string.format("%s:", mob_name(npc.get_race())))
				say("")
				if s == 1 then
					local f = __fish_real_refine_rod(item.get_cell())
					if f == 2 then
						say(gameforge[lang].fisher._5)
					elseif f == 1 then
						say(gameforge[lang].fisher._6)
					elseif f == 3 then
						say(gameforge[lang].fisher._7)
					elseif f == 4 then
						say(gameforge[lang].fisher._8)
					else
						say(gameforge[lang].common.unknownerr)
					end
				else
					say(gameforge[lang].fisher._9)
				end
			else
				say(gameforge[lang].fisher._10)
			end
		end
	end
end

