quest voucherarmadevided begin
	state start begin
		when 40006.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
		say_title("Devided Soul Weapon voucher(30z):")
		say("")
		say("Alege o arma:")
		say_reward("Acesta este un SKIN!")
		say("")
		local premiu = select ("Sabie", "2-Maini", "Pumnale", "Arc", "Clopot", "Evantai", "Decid mai tarziu!")
		if premiu == 1 then
			if pc.get_sex() == 0 then
				pc.give_item2(84294,1)
				pc.remove_item(40006, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84294,1)
				pc.remove_item(40006, 1)
			end
		elseif premiu == 2 then
			if pc.get_sex() == 0 then
				pc.give_item2(84295,1)
				pc.remove_item(40006, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84295,1)
				pc.remove_item(40006, 1)
			end
		elseif premiu == 3 then
			if pc.get_sex() == 0 then
				pc.give_item2(84296,1)
				pc.remove_item(40006, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84296,1)
				pc.remove_item(40006, 1)
			end
		elseif premiu == 4 then
			if pc.get_sex() == 0 then
				pc.give_item2(84297,1)
				pc.remove_item(40006, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84297,1)
				pc.remove_item(40006, 1)
			end
		elseif premiu == 5 then
			if pc.get_sex() == 0 then
				pc.give_item2(84298,1)
				pc.remove_item(40006, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84298,1)
				pc.remove_item(40006, 1)
			end
		elseif premiu == 6 then
			if pc.get_sex() == 0 then
				pc.give_item2(84299,1)
				pc.remove_item(40006, 1)
			elseif pc.get_sex() == 1 then
				pc.give_item2(84299,1)
				pc.remove_item(40006, 1)
			end
		end	
	end
end
end