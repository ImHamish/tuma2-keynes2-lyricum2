quest crea_gilda begin
	state start begin
		when 11000.chat."Paraseste Breasla" or 11002.chat."Paraseste Breasla" or 11004.chat."Paraseste Breasla" with pc.hasguild() and not pc.isguildmaster() begin
			local lang = pc.get_language()
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			say(gameforge[lang].guild_leave._1)
			say(gameforge[lang].guild_leave._2)
			say(gameforge[lang].guild_leave._3)
			local s = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			if s == 2 then
				return
			end
			
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			say(gameforge[lang].guild_leave._4)
			pc.remove_from_guild()
			pc.setqf("new_withdraw_time",get_global_time())
		end
	
		when 11000.chat."Disband the guild" or 11002.chat."Disband the guild" or 11004.chat."Disband the guild" with pc.hasguild() and pc.isguildmaster() begin
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].guild_manage._3)
			local s = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			if s == 1 then
				say(gameforge[lang].guild_manage._4)
				pc.destroy_guild()
				pc.setqf("new_disband_time",get_global_time())
				pc.setqf("new_withdraw_time",get_global_time())
			end
		end

		when 11000.chat."Creeaza o breasla" or 11002.chat."Creeaza o breasla" or 11004.chat."Creeaza o breasla" with not pc.hasguild() and not pc.isguildmaster() begin
			say_title(string.format("%s:", mob_name(npc.get_race())))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].crea_gilda._1)
			say_reward(string.format(gameforge[lang].crea_gilda._2, 5000000))
			say_reward(gameforge[lang].crea_gilda._3)
			local s = select(gameforge[lang].common.yes, gameforge[lang].common.no)
			if s == 1 then
				say_title(string.format("%s:", mob_name(npc.get_race())))
				say("")
				say(gameforge[lang].crea_gilda._4)
				local guild_name = string.gsub(input(), "[^A-Za-z0-9]", "")
				local guild_len_name = string.len(guild_name)
				say_title(string.format("%s:", mob_name(npc.get_race())))
				say("")
				if not ((2 < guild_len_name) and (guild_len_name < 12)) then
					say(gameforge[lang].crea_gilda._5)
					return
				end
				
				say(string.format(gameforge[lang].crea_gilda._6, guild_name))
				local s2 = select(gameforge[lang].common.yes, gameforge[lang].common.no)
				if s2 == 1 then
					say_title(string.format("%s:", mob_name(npc.get_race())))
					say("")
					if pc.get_gold() < 5000000 then
						say(gameforge[lang].crea_gilda._7)
						return
					end
					
					if pc.get_level() < 40 then
						say(gameforge[lang].crea_gilda._8)
						return
					end
					
					if pc.hasguild() or pc.isguildmaster() then
						say(gameforge[lang].crea_gilda._9)
						return
					end
					
					local info = pc.make_guild0(guild_name)
					if info == -2 then
						say(gameforge[lang].crea_gilda._10)
					elseif info == -1 then
						say(gameforge[lang].crea_gilda._11)
					elseif info == 0 then
						say(gameforge[lang].crea_gilda._12)
					elseif info == 1 then
						pc.change_gold(-5000000)
						say(gameforge[lang].crea_gilda._13)
					elseif info == 2 then
						say(gameforge[lang].crea_gilda._9)
					elseif info == 3 then
						say(gameforge[lang].crea_gilda._14)
					end
				end
			end
		end
	end
end

