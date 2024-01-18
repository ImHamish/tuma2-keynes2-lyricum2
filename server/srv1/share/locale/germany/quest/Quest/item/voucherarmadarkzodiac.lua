quest voucherarmazodiac begin
	state start begin
		when 40007.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
		say_title("Dark Zodiac Weapon voucher(P):")
		say("")
		say("Alege o arma:")
		say_reward("Acesta este un SKIN si este PERMANENT!")
		say("")
		local premiu = select ("Sabie", "2-Maini", "Pumnale", "Arc", "Clopot", "Evantai", "Decid mai tarziu!")
		if premiu == 1 then
			if pc.get_sex() == 0 then
				pc.give_item2(40100,1)
				pc.remove_item(40007, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(40100,1)
				pc.remove_item(40007, 1)
			end
		elseif premiu == 2 then
			if pc.get_sex() == 0 then
				pc.give_item2(40097,1)
				pc.remove_item(40007, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(40097,1)
				pc.remove_item(40007, 1)
			end
		elseif premiu == 3 then
			if pc.get_sex() == 0 then
				pc.give_item2(40099,1)
				pc.remove_item(40007, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(40099,1)
				pc.remove_item(40007, 1)
			end
		elseif premiu == 4 then
			if pc.get_sex() == 0 then
				pc.give_item2(40098,1)
				pc.remove_item(40007, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(40098,1)
				pc.remove_item(40007, 1)
			end
		elseif premiu == 5 then
			if pc.get_sex() == 0 then
				pc.give_item2(40096,1)
				pc.remove_item(40007, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(40096,1)
				pc.remove_item(40007, 1)
			end
		elseif premiu == 6 then
			if pc.get_sex() == 0 then
				pc.give_item2(40095,1)
				pc.remove_item(40007, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(40095,1)
				pc.remove_item(40007, 1)
			end
		end	
	end
end
end