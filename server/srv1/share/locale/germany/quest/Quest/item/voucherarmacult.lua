quest voucherarmadevided begin
	state start begin
		when 40005.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
		say_title("Dark Cult Weapon voucher(30d):")
		say("")
		say("Alege o arma:")
		say_reward("Acesta este un SKIN!")
		say("")
		local premiu = select ("Sabie", "2-Maini", "Pumnale", "Arc", "Clopot", "Evantai", "Decid mai tarziu!")
		if premiu == 1 then
			if pc.get_sex() == 0 then
				pc.give_item2(48914,1)
				pc.remove_item(40005, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(48914,1)
				pc.remove_item(40005, 1)
			end
		elseif premiu == 2 then
			if pc.get_sex() == 0 then
				pc.give_item2(48915,1)
				pc.remove_item(40005, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(48915,1)
				pc.remove_item(40005, 1)
			end
		elseif premiu == 3 then
			if pc.get_sex() == 0 then
				pc.give_item2(48916,1)
				pc.remove_item(40005, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(48916,1)
				pc.remove_item(40005, 1)
			end
		elseif premiu == 4 then
			if pc.get_sex() == 0 then
				pc.give_item2(48917,1)
				pc.remove_item(40005, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(48917,1)
				pc.remove_item(40005, 1)
			end
		elseif premiu == 5 then
			if pc.get_sex() == 0 then
				pc.give_item2(48919,1)
				pc.remove_item(40005, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(48919,1)
				pc.remove_item(40005, 1)
			end
		elseif premiu == 6 then
			if pc.get_sex() == 0 then
				pc.give_item2(48920,1)
				pc.remove_item(40005, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(48920,1)
				pc.remove_item(40005, 1)
			end
		end	
	end
end
end