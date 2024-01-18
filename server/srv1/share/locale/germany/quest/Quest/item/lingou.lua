quest lingou begin
	state start begin
		when 80005.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			say_title(string.format("%s:", item_name(80005)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].lingou._1, "100kk"))
			say(gameforge[lang].lingou._2)
			if select(gameforge[lang].common.yes, gameforge[lang].common.no) == 1 then
				say_title(string.format("%s:", item_name(80005)))
				say("")
				if not pc.can_warp() then
					say(gameforge[lang].lingou._3)
					return
				end
				
				local myGold = pc.get_gold() + 100000000
				if myGold >= 9000000000000 then
					say(gameforge[lang].lingou._4)
					say(gameforge[lang].lingou._5)
					return
				end
				
				if pc.count_item(80005) >= 1 then
					pc.change_money(100000000)
					pc.remove_item(80005, 1)
					
					say(string.format(gameforge[lang].lingou._6, "100kk"))
					return
				end
				
				say("?!?")
			end
		end

		when 80006.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			say_title(string.format("%s:", item_name(80006)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].lingou._1, "500kk"))
			say(gameforge[lang].lingou._2)
			if select(gameforge[lang].common.yes, gameforge[lang].common.no) == 1 then
				say_title(string.format("%s:", item_name(80006)))
				say("")
				if not pc.can_warp() then
					say(gameforge[lang].lingou._3)
					return
				end
				
				local myGold = pc.get_gold() + 500000000
				if myGold >= 9000000000000 then
					say(gameforge[lang].lingou._4)
					say(gameforge[lang].lingou._5)
					return
				end
				
				if pc.count_item(80006) >= 1 then
					pc.change_money(500000000)
					pc.remove_item(80006, 1)
					
					say(string.format(gameforge[lang].lingou._6, "500kk"))
					return
				end
				
				say("?!?")
			end
		end

		when 80007.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			say_title(string.format("%s:", item_name(80007)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].lingou._1, "1kkk"))
			say(gameforge[lang].lingou._2)
			if select(gameforge[lang].common.yes, gameforge[lang].common.no) == 1 then
				say_title(string.format("%s:", item_name(80007)))
				say("")
				if not pc.can_warp() then
					say(gameforge[lang].lingou._3)
					return
				end
				
				local myGold = pc.get_gold() + 1000000000
				if myGold >= 9000000000000 then
					say(gameforge[lang].lingou._4)
					say(gameforge[lang].lingou._5)
					return
				end
				
				if pc.count_item(80007) >= 1 then
					pc.change_money(1000000000)
					pc.remove_item(80007, 1)
					
					say(string.format(gameforge[lang].lingou._6, "1kkk"))
					return
				end
				
				say("?!?")
			end
		end

		when 80008.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			say_title(string.format("%s:", item_name(80008)))
			say("")
			local lang = pc.get_language()
			say(string.format(gameforge[lang].lingou._1, "10kkk"))
			say(gameforge[lang].lingou._2)
			if select(gameforge[lang].common.yes, gameforge[lang].common.no) == 1 then
				say_title(string.format("%s:", item_name(80008)))
				say("")
				if not pc.can_warp() then
					say(gameforge[lang].lingou._3)
					return
				end
				
				local myGold = pc.get_gold() + 10000000000
				if myGold >= 9000000000000 then
					say(gameforge[lang].lingou._4)
					say(gameforge[lang].lingou._5)
					return
				end
				
				if pc.count_item(80008) >= 1 then
					pc.change_money(10000000000)
					pc.remove_item(80008, 1)
					
					say(string.format(gameforge[lang].lingou._6, "10kkk"))
					return
				end
				
				say("?!?")
			end
		end
	end
end
