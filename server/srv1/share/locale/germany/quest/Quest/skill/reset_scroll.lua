quest reset_scroll begin
	state start begin
		when 71054.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			say_title(string.format("%s:", item_name(item.get_vnum())))
			say("")
			local lang = pc.get_language()
			ResetScroll.ShowRequisites(item.get_vnum());
			if (not ResetScroll.CheckRequisites(item.get_vnum())) then
				return
			end

			if (select(gameforge[lang].reset_scroll._1, gameforge[lang].reset_scroll._2) == 1) then
				say_title(string.format("%s:", item_name(item.get_vnum())))
				say("")
				say(gameforge[lang].reset_scroll._7)
				local emp = select(
					gameforge[lang].reset_scroll._3,
					gameforge[lang].reset_scroll._4,
					gameforge[lang].reset_scroll._5,
					gameforge[lang].reset_scroll._6
				)
				
				if (emp == 4) then
					return
				end
				
				say_title(string.format("%s:", item_name(item.get_vnum())))
				say("")
				local ret = pc.change_empire(emp)
				if (ret == 1) then
					say_reward(string.format(gameforge[lang].reset_scroll._8, locale.empire_names[pc.get_empire()]))
					return
				elseif (ret == 2) then
					say_reward(gameforge[lang].reset_scroll._9)
					say_reward(gameforge[lang].reset_scroll._10)
					return
				elseif (ret == 3) then
					say_reward(gameforge[lang].reset_scroll._11)
					say_reward(gameforge[lang].reset_scroll._12)
					return
				end
				
				pc.remove_item(71054, 1);
				say(gameforge[lang].reset_scroll._13)
			end
		end

		when 71002.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			say_title(string.format("%s:", item_name(item.get_vnum())))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].reset_scroll._16)
			if (select(gameforge[lang].reset_scroll._1, gameforge[lang].reset_scroll._2) == 1) then
				pc.remove_item(71002, 1);
				pc.reset_point();
				say_title(string.format("%s:", item_name(item.get_vnum())))
				say("")
				say(gameforge[lang].reset_scroll._17)
			end
		end

		when 71048.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			say_title(string.format("%s:", item_name(item.get_vnum())))
			say("")
			local lang = pc.get_language()
			say(gameforge[lang].reset_scroll._48)
			if (select(gameforge[lang].reset_scroll._1, gameforge[lang].reset_scroll._2) == 1) then
				say_title(string.format("%s:", item_name(item.get_vnum())))
				say("")
				if pc.is_polymorphed() then
					say(gameforge[lang].reset_scroll._18)
					return
				end
				
				local change = pc.change_sex()
				if change == 3 then
					say(gameforge[lang].reset_scroll._49)
					return
				elseif change == 2 then
					say(gameforge[lang].reset_scroll._50)
					return
				elseif change == 0 then
					say(gameforge[lang].common.unknownerr)
					return
				end
				
				pc.remove_item(71048, 1)
				pc.polymorph(20032)
				say(gameforge[lang].reset_scroll._19)
			end
		end

		when 71099.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			say_title(string.format("%s:", item_name(item.get_vnum())))
			say("")
			ResetScroll.ShowRequisites(item.get_vnum());
			if (not ResetScroll.CheckRequisites(item.get_vnum())) then
				return;
			end
			
			local lang = pc.get_language()
			if (select(gameforge[lang].reset_scroll._1, gameforge[lang].reset_scroll._2) == 1) then
				say_title(string.format("%s:", item_name(item.get_vnum())))
				say("")
				say(gameforge[lang].reset_scroll._21)
				say_title(string.format("%s:[ENTER]", item_name(item.get_vnum())))
				local s = input();
				say_title(string.format("%s:", item_name(item.get_vnum())))
				say("")
				if (pc.get_name() == s) then 
					say(gameforge[lang].reset_scroll._22)
					return
				elseif (s == nil) then 
					say(gameforge[lang].reset_scroll._23)
					return
				end
				
				local ret = guild.change_master(s)
				if (ret == 0 or ret == 1 or ret == 4) then
					say_reward(string.format(gameforge[lang].reset_scroll._24, ret));
					return
				elseif (ret == 2) then 
					say_reward(string.format(gameforge[lang].reset_scroll._25, s))
					say_reward(gameforge[lang].reset_scroll._26)
					return;
				end
				
				pc.remove_item(71099, 1)
				say(gameforge[lang].reset_scroll._27)
				big_notice_all(911, string.format("%s#%s#%s", pc.get_name(), guild.get_name(guild.get_id0()), s))
			end
		end

		--** Ridistribuzione Status: VIT, INT, STR, DEX.
		when 71103.use or 71104.use or 71105.use or 71106.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			local item_vnum = item.get_vnum()
			local lang = pc.get_language()
			local status_name = {
									[71103] = gameforge[lang].reset_scroll._28,
									[71104] = gameforge[lang].reset_scroll._29,
									[71105] = gameforge[lang].reset_scroll._30,
									[71106] = gameforge[lang].reset_scroll._31,
			}
			
			say_title(string.format("%s:", item_name(item.get_vnum())))
			say("")
			if (select(gameforge[lang].reset_scroll._1, gameforge[lang].reset_scroll._2) == 1) then
				local ret = pc.reset_status(item_vnum - 71103);
				say_title(string.format("%s:", item_name(item.get_vnum())))
				say("")
				if (not ret) then
					say_reward(gameforge[lang].reset_scroll._32)
					return
				end
				
				pc.remove_item(item_vnum, 1);
				say(string.format(gameforge[lang].reset_scroll._33, status_name[item_vnum]))
				
			end
		end

		when 71100.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			say_title(string.format("%s:", item_name(item.get_vnum())))
			say("")
			local lang = pc.get_language()
			ResetScroll.ShowRequisites(item.get_vnum());
			if (not ResetScroll.CheckRequisites(item.get_vnum())) then
				return
			end
			
			if (select(gameforge[lang].reset_scroll._1, gameforge[lang].reset_scroll._2) == 1) then
				say_title(string.format("%s:", item_name(item.get_vnum())))
				say("")
				if not pc.can_warp() then
					say(gameforge[lang].reset_scroll._35)
				end
				
				pc.remove_item(71100, 1)
				local addBonus = false
				local skillgroup = pc.get_skill_group()
				if pc.get_job() == 0 and skillgroup == 2 then
					addBonus = true
				elseif pc.get_job() == 1 then
					addBonus = true
				elseif pc.get_job() == 2 and skillgroup == 2 then
					addBonus = true
				elseif pc.get_job() == 3 then
					addBonus = true
				end

				pc.clear_skill();
				pc.set_skill_group(0);
				
				--set_quest_state("new_skill", "start")
				
				say(gameforge[lang].reset_scroll._36)
				if horse.is_summon() then
					if horse.is_riding() then
						horse.unride()
					end
					
					horse.unsummon()
				end
				
				pc.set_skill_level(131, 0)
				horse.set_level(0)
				select(gameforge[lang].common.wait)
				pc.warp(pc.get_x() * 100, pc.get_y() * 100)
			end
		end

		when 71055.use begin
			if pc.get_map_index() >= 10000 then
				syschat(gameforge[pc.get_language()].common.cannot_use_dung)
				return
			end
			
			if not pc.can_warp() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			say_title(string.format("%s:", item_name(item.get_vnum())))
			say("")
			ResetScroll.ShowRequisites(item.get_vnum());
			if (not ResetScroll.CheckRequisites(item.get_vnum())) then
				return;
			end
			
			local lang = pc.get_language()
			say(gameforge[lang].reset_scroll._39)
			local new_name = input()
			say_title(string.format("%s:", item_name(item.get_vnum())))
			say("")
			local old_name = pc.get_name();
			if (new_name == old_name) then
				say_reward(gameforge[lang].reset_scroll._40)
				return
			end
			
			local ret = pc.change_name(new_name);
			if (ret == 0) then
				say_reward(gameforge[lang].reset_scroll._41)
			elseif (ret == 1) then
				say_reward(gameforge[lang].reset_scroll._42)
			elseif (ret == 2) then
				say_reward(gameforge[lang].reset_scroll._43)
			elseif (ret == 3) then
				say_reward(gameforge[lang].reset_scroll._44)
			elseif (ret == 4) then
				pc.remove_item(71055, 1);
				say_reward(gameforge[lang].reset_scroll._45)
				say_reward(gameforge[lang].reset_scroll._46)
                say("[DELAY value;1300]3[/DELAY] [DELAY value;1300]2[/DELAY] [DELAY value;1300]1[/DELAY]")
                command("logout")
			else
				say_reward(gameforge[lang].reset_scroll._47)
			end
		end
	end
end

