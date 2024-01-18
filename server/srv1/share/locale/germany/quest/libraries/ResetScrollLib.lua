--[[
	Inizializza: la classe 'ResetScroll'.
]]
ResetScroll = {};

--[[
	Ritorna:
		La lista completa di vnum e nomi delle skill
		del personaggio di razza inserita nell'argomento 'job'
		e di dottrina inserita nell'argomento 'group'.
]]
ResetScroll.GetSkillList = function(job, group)
	local skill_vnum_list = {};
	local skill_name_list = {};

	if (pc.get_skill_group() ~= 0) then
		local skill_list = ACTIVE_SKILL_LIST[job][group]

		table.foreachi(skill_list,
			function(i, t)
				if (pc.get_skill_level(t) > 0) then
					table.insert(skill_vnum_list, t);
					table.insert(skill_name_list, SKILL_NAME_TABLE[t]);
				end -- if
			end -- function
		);
	end -- if

	return {skill_vnum_list, skill_name_list};
end -- function

--[[
	Ritorna:
		La lista completa di requisiti necessari personaggio
		l'utilizzo di un determinato item esistente all'interno
		dell'array 'requisites'.

	Struttura:
		[item_vnum] = {
			{"requisito_stringa1", lua_check1 (inverso a cio' che dice requisito_stringa1)},
			{"requisito_stringa2", lua_check2 (inverso a cio' che dice requisito_stringa2)}
		},
]]
ResetScroll.GetRequisites = function(vnum)
	local vnum_list = ResetScroll.GetSkillList(pc.get_job(), pc.get_skill_group());
	local name_list = ResetScroll.GetSkillList(pc.get_job(), pc.get_skill_group());

	local requisites = {
						[71054] = {
							{gameforge[pc.get_language()].reset_scroll._14, pc.is_polymorphed()},
							{gameforge[pc.get_language()].reset_scroll._15, pc.has_guild()}
						},
						[71002] = {},
						[71055] = {
							{gameforge[pc.get_language()].reset_scroll._37, party.is_party()},
							{gameforge[pc.get_language()].reset_scroll._38, pc.has_guild()},
						},
						[71099] = {
							{gameforge[pc.get_language()].reset_scroll._20, not pc.is_guild_master()}
						},
						[71100] = {
							{gameforge[pc.get_language()].reset_scroll._34, pc.get_skill_group() == 0}
						},
	};

	return requisites[vnum];
end -- function

--[[
	Mostra:
		La lista di requisiti necessari all'utilizzo dell'item
		di vnum esistente all'interno dell'array della funzione
		ResetScroll.GetRequisites, ritornando una stringa di default
		se il suddetto array relativo all'item usato e' vuoto.
]]
ResetScroll.ShowRequisites = function(vnum)
	local requisites = ResetScroll.GetRequisites(vnum);
	local lang = pc.get_language()
	say_reward(gameforge[lang].reset_scroll_new._1)
	if (table.getn(requisites) > 0) then
		for i = 1, table.getn(requisites) do
			if (i == table.getn(requisites)) then
				say_reward(string.format("%d. %s[ENTER]", i, requisites[i][1]))
			else
				say_reward(string.format("%d. %s,", i, requisites[i][1]))
			end -- if/else
		end -- for
	else
		say_reward(gameforge[lang].reset_scroll_new._2)
	end -- if
end -- function

--[[
	Ritorna:
		true se tutti i requisiti sono soffisfatti.
	Altrimenti:
		ritorna false ed outputta una lista di messaggi che informano l'utente
		di cosa c'e che non va.
]]
ResetScroll.CheckRequisites = function(vnum)
	local requisites = ResetScroll.GetRequisites(vnum);
	local lang = pc.get_language()
	if (table.getn(requisites) > 0) then
		for i = 1, table.getn(requisites) do
			if (requisites[i][2]) then
				say_reward(string.format("%s;", requisites[i][1]));
				return false;
			end -- if
		end -- for

		say(gameforge[lang].reset_scroll_new._3)
		return true;
	end -- if
	
	say(gameforge[lang].reset_scroll_new._4)
	return true;
end -- function
