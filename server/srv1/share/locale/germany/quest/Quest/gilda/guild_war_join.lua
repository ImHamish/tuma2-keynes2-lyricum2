quest guild_war_join begin
	state start begin
		when letter begin
			if pc.get_war_map() == 0 and guild.get_any_war() != 0 then
				send_letter(gameforge[pc.get_language()].guild_war_join._1)
			end
		end

		when button or info begin
			local lang = pc.get_language()
			say_title(string.format("%s:", gameforge[lang].guild_war_join._1))
			say("")
			local e = guild.get_any_war()
			if e == 0 then
				say(gameforge[lang].guild_war_join._2)
			else
				say(gameforge[lang].guild_war_join._3)
				say(string.format(gameforge[lang].guild_war_join._4, guild.name(e)))
				local s = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				if s == 1 then
					guild.war_enter(e)
				else
					send_letter(gameforge[lang].guild_war_join._1)
				end
			end
		end
	end
end

