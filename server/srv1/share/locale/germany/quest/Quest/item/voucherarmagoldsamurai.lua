quest voucherarmagoldsamurai begin
	state start begin
		when 40018.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
		say_title("Voucher Arma Samurai de Aur:")
		say("")
		say("Alege o arma:")
		say_reward("Acesta este un SKIN si este PERMANENT!")
		say("")
		local premiu = select ("Sabie", "Sabie Sura", "2-Maini", "Pumnale", "Arc", "Clopot", "Evantai", "Decid mai tarziu!")
		if premiu == 1 then
			if pc.get_sex() == 0 then
				pc.give_item2(84324,1)
				pc.remove_item(40018, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84324,1)
				pc.remove_item(40018, 1)
			end
		elseif premiu == 2 then
			if pc.get_sex() == 0 then
				pc.give_item2(84328,1)
				pc.remove_item(40018, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84328,1)
				pc.remove_item(40018, 1)
			end
		elseif premiu == 3 then
			if pc.get_sex() == 0 then
				pc.give_item2(84325,1)
				pc.remove_item(40018, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84325,1)
				pc.remove_item(40018, 1)
			end
		elseif premiu == 4 then
			if pc.get_sex() == 0 then
				pc.give_item2(84326,1)
				pc.remove_item(40018, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84326,1)
				pc.remove_item(40018, 1)
			end
		elseif premiu == 5 then
			if pc.get_sex() == 0 then
				pc.give_item2(84327,1)
				pc.remove_item(40018, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84327,1)
				pc.remove_item(40018, 1)
			end
		elseif premiu == 6 then
			if pc.get_sex() == 0 then
				pc.give_item2(84329,1)
				pc.remove_item(40018, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84329,1)
				pc.remove_item(40018, 1)
			end
		elseif premiu == 7 then
			if pc.get_sex() == 0 then
				pc.give_item2(84330,1)
				pc.remove_item(40018, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84330,1)
				pc.remove_item(40018, 1)
			end
		end	
	end
end
end