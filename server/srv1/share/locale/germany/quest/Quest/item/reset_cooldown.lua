quest reset_cooldown begin
	state start begin
		when 30918.use or
			 30919.use or
			 30920.use or
			 30921.use or
			 30922.use or
			 30923.use or
			 30924.use or
			 30925.use or
			 30926.use or
			 30927.use begin
			local itemVnum = item.get_vnum()
			local lang = pc.get_language()
			
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[lang].common.cannot_use_dung)
				return
			end
			
			local questNames = {
									[30918] = "dungeonorchi",
									[30919] = "slime_dungeon",
									[30920] = "dungeon_spider",
									[30921] = "deviltower_zone",
									[30922] = "catacombe",
									[30923] = "dragonlair_zone",
									[30924] = "razador_zone",
									[30925] = "nemere_zone",
									[30926] = "temple_ochao",
									[30927] = "enchanted_forest"
			}
			
			local questName = questNames[itemVnum]
			if questName == "" then
				say_title(string.format("%s:", item_name(itemVnum)))
				say("")
				say(gameforge[lang].reset_cooldown._6)
				return
			end
			
			local dungNames = {
									[30918] = gameforge[lang].reset_cooldown._7,
									[30919] = gameforge[lang].reset_cooldown._8,
									[30920] = gameforge[lang].reset_cooldown._9,
									[30921] = gameforge[lang].reset_cooldown._10,
									[30922] = gameforge[lang].reset_cooldown._11,
									[30923] = gameforge[lang].reset_cooldown._12,
									[30924] = gameforge[lang].reset_cooldown._13,
									[30925] = gameforge[lang].reset_cooldown._14,
									[30926] = gameforge[lang].reset_cooldown._15,
									[30927] = gameforge[lang].reset_cooldown._16
			}
			
			say_title(string.format("%s:", item_name(itemVnum)))
			say("")
			say(gameforge[lang].reset_cooldown._1)
			say(gameforge[lang].reset_cooldown._2)
			say(string.format("- %s.", dungNames[itemVnum]))
			local cooldown = pc.getf(questName, "cooldown") - get_global_time()
			if cooldown < 0 then
				cooldown = 0
			end
			
			local h = math.floor(cooldown / 3600)
			local m = math.floor((cooldown - (3600 * h)) / 60)
			local hS = gameforge[lang].common.hours
			if h == 1 then
				hS = gameforge[lang].common.hour
			end
			local mS = gameforge[lang].common.minutes
			if m == 1 then
				mS = gameforge[lang].common.minute
			end
			
			say(string.format(gameforge[lang].reset_cooldown._3, h, hS, m, mS))
			say("")
			if cooldown > 0 then
				say(gameforge[lang].reset_cooldown._4)
				if select(gameforge[lang].common.yes, gameforge[lang].common.no) == 1 then
					say_title(string.format("%s:", item_name(itemVnum)))
					say("")
					if party.is_party() then
						say(gameforge[lang].reset_cooldown._17)
						return
					end
					
					local rejoinTIME = pc.getf(questName, "disconnect") - get_global_time()
					if rejoinTIME > 0 then
						local rejoinIDX = pc.getf(questName, "idx")
						if rejoinIDX > 0 then
							say(string.format(gameforge[lang].reset_cooldown._18, pc.getf(questName, "ch")))
							say(gameforge[lang].reset_cooldown._19)
							return
						end
					end
					
					pc.remove_item(itemVnum, 1)
					pc.setf(questName, "disconnect", 0)
					pc.setf(questName, "idx", 0)
					pc.setf(questName, "ch", 0)
					pc.setf(questName, "cooldown", 0)
					
					say(gameforge[lang].reset_cooldown._5)
				end
			end
		end
	end
end
