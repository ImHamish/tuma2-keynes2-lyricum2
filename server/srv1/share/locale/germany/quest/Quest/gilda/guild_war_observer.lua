quest guild_war_observer begin
	state start begin
		when 11001.chat."Lista Razboaie" or 11003.chat."Lista Razboaie" or 11005.chat."Lista Razboaie" begin
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			local lang = pc.get_language()
			local g = guild.get_warp_war_list()
			local gname_table = {}
			table.foreachi(g,
			function(n, p) 
			gname_table[n] = string.format(gameforge[lang].guild_war_observer._1, guild.get_name(p[1]), guild.get_name(p[2]))
			end)
			
			if table.getn(g) == 0 then
				say(gameforge[lang].guild_war_observer._2)
			else
				gname_table[table.getn(g)+1] = gameforge[lang].common.confirm
				say(gameforge[lang].guild_war_observer._3)
				local s = select_table(gname_table)
				if s != table.getn(gname_table) then
					pc.warp_to_guild_war_observer_position(g[s][1], g[s][2])
				end
			end
		end
	end
end

