quest reset_scroll2 begin
	state start begin
when 71003.use begin

say("Redistribuirea Competentelor:")
say("Cu acest item iti poti reseta competentele ") 
say("actuale, pentru a le creste nivelul la M1. ")

local result = ResetScroll.GetSkillList(pc.get_job(), pc.get_skill_group())

local vnum_list = result[1]
local name_list = result[2]

if table.getn(vnum_list) < 1 then
say("Momentan nu detii nici-o competenta pe")
say("care o poti reseta.")
say("")
return
end


local i = select_table(name_list)

if table.getn(name_list) == i then
return
end

local name = name_list[i]
local vnum = vnum_list[i]

say("Redistribuirea Competentelor:") 
say(string.format("Competenta actuala este %s.", name))
say("Inca doresti sa resetezi aceasta competenta ?") 
say("")

local c = select("Da, Vreau.", "M-am razgandit.")
if 2 == c then
return
end


--local val = pc.getqf("force_to_master_skill")
--if val > 0 then
--- l
-- say("Redistribuirea Competenþelor:")
-- say("Abilitatea a fost resetata,")
-- say("ai primit 17 puncte de abilitate.")
-- say("")
-- say_reward("Abilitatea respectiva va deveni Mare")
-- say_reward("Maestru la 17 puncte.")
-- say("")
-- return
--end

char_log(0, "RESET_ONE_SKILL", "USE_ITEM(71003)")
pc.remove_item(71003)

char_log(0, "RESET_ONE_SKILL", string.format("RESET_SKILL[%s]", name))
pc.clear_one_skill(vnum)

char_log(0, "RESET_ONE_SKILL", "APPLY_17MASTER_BONUS")
pc.setqf("force_to_master_skill", 1)

say("Redistribuirea Competentelor:")
say(string.format("%s fost resetata.", name))

--- l
say("Redistribuirea competentelor a avut succes.")
say("")
end
end
end