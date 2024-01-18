quest okeycards_event begin
	state start begin
		when 20417.chat."Top" begin
			say_title(string.format("%s:", mob_name(20417)))
			say("")
			local lang = pc.get_language()
			if game.get_okay_event_status() != 1 then
				say(gameforge[lang].common.event_status_off)
				return
			end
			
			say(pc.get_okay_global_rank(gameforge[lang].common.place, gameforge[lang].common.points))
		end
		
		--when 20417.chat."Top Round" begin
		--	say_title(string.format("%s:", mob_name(20417)))
		--	say("")
		--	local lang = pc.get_language()
		--	if game.get_okay_event_status() != 1 then
		--		say(gameforge[lang].common.event_status_off)
		--		return
		--	end
		--	
		--	say(pc.get_okay_rund_rank(gameforge[lang].common.place, gameforge[lang].common.points))
		--end
	end
end
