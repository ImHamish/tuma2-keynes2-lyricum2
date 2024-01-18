quest voucherarmacraciun begin
	state start begin
		when 40020.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
		say_title("Voucher Arma Craciun:")
		say("")
		say("Alege o arma:")
		say_reward("Acesta este un SKIN!")
		say("")
		local premiu = select ("Sabie", "2-Maini","Sabie Sura", "Pumnale", "Arc", "Clopot", "Evantai", "Decid mai tarziu!")
		if premiu == 1 then
			if pc.get_sex() == 0 then
				pc.give_item2(84374,1)
				pc.remove_item(40020, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84374,1)
				pc.remove_item(40020, 1)
			end
		elseif premiu == 2 then
			if pc.get_sex() == 0 then
				pc.give_item2(84375,1)
				pc.remove_item(40020, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84375,1)
				pc.remove_item(40020, 1)
			end
		elseif premiu == 3 then
			if pc.get_sex() == 0 then
				pc.give_item2(84376,1)
				pc.remove_item(40020, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84376,1)
				pc.remove_item(40020, 1)
			end
		elseif premiu == 4 then
			if pc.get_sex() == 0 then
				pc.give_item2(84377,1)
				pc.remove_item(40020, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84377,1)
				pc.remove_item(40020, 1)
			end
		elseif premiu == 5 then
			if pc.get_sex() == 0 then
				pc.give_item2(84378,1)
				pc.remove_item(40020, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84378,1)
				pc.remove_item(40020, 1)
			end
		elseif premiu == 6 then
			if pc.get_sex() == 0 then
				pc.give_item2(84379,1)
				pc.remove_item(40020, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84379,1)
				pc.remove_item(40020, 1)
			end
		elseif premiu == 7 then
			if pc.get_sex() == 0 then
				pc.give_item2(84380,1)
				pc.remove_item(40020, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84380,1)
				pc.remove_item(40020, 1)
			end
		end	
	end
end
end