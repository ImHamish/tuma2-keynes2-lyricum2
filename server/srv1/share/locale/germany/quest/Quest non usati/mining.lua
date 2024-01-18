quest mining begin
	state start begin
		when 20015.take with item.vnum >= 29101 and item.vnum < 29110 and item.get_socket(0) == item.get_value(2) begin
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			local lang = pc.get_language()
			local vnum = item.get_vnum()
			if vnum >= 29101 and vnum < 29200 and item.get_socket(0) != item.get_value(2) then
				say(gameforge[pc.get_language()].mining._1)
			elseif vnum >= 29101 and vnum < 29110 and item.get_socket(0) == item.get_value(2) then
				say(gameforge[lang].mining._2)
				say(gameforge[lang].mining._3)
				say(string.format(gameforge[lang].mining._4, item.get_level() + 1))
				if item.get_value(3) == 100 then
					say(gameforge[lang].mining._5)
				else
					say(gameforge[lang].mining._6)
					say(string.format(gameforge[lang].mining._7, 100 - item.get_value(3)))
					say(gameforge[lang].mining._8)
				end
				
				local s = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				if s == 1 then
					local f = __refine_pick(item.get_cell())
					if f == 2 then
						say(gameforge[lang].mining._9)
					elseif f == 1 then
						say(gameforge[lang].mining._10)
					else
						say(gameforge[lang].mining._11)
					end 
				else
					say(gameforge[lang].mining._12)
				end
			end
		end
		
		when 20047.click or
			 20048.click or
			 20049.click or
			 20050.click or
			 20051.click or
			 20052.click or
			 20053.click or
			 20054.click or
			 20055.click or
			 20056.click or
			 20057.click or
			 20058.click or
			 20059.click or
			 30301.click or
			 30302.click or
			 30303.click or
			 30304.click or
			 30305.click begin
			if pc.is_mount() != true then
				pc.mining()
			end
		end
	end
end

