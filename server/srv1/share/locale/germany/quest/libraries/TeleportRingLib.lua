--[[
-- Init - The Teleport Ring Class
]]

TeleportRing = {};

TeleportRing.ReturnData = function()
	if (TeleportRing.data == nil) then
		TeleportRing.data = {
			{
				["TeleportMapName"] = "Tinutul de Gheata(event/lv90+)",
				["TeleportHasOtherIndexs"] = false,
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {90, 120}, -- nivel minim / nivel maxim (delimiteaza nivelele la care te poti teleporta in mapa respectiva)
				["TeleportItemCheck"] = false,
				["x"] = 2252800,
				["y"] = 3046400
			},
			{
				["TeleportMapName"] = "ARENA PvP(Lv.5+)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Luptator", ["x"] = 2476300, ["y"] = 3611300},
					{["OtherIndexName"] = "Spectator", ["x"] = 2473900, ["y"] = 3611200}},
				["TeleportLevelCheck"] = false,
				["TeleportItemCheck"] = false,
			},
			{
				["TeleportMapName"] = "Map1(Lv.1+)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Jinno", ["x"] = 963100, ["y"] = 276000},
					{["OtherIndexName"] = "Chunjo", ["x"] = 63200, ["y"] = 166700},
					{["OtherIndexName"] = "Shinsoo", ["x"] = 473900, ["y"] = 954600}},
				["TeleportLevelCheck"] = false,
				["TeleportItemCheck"] = false
			},
			{
				["TeleportMapName"] = "Map2(Lv.10+)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Jinno", ["x"] = 863800, ["y"] = 246000},
					{["OtherIndexName"] = "Chunjo", ["x"] = 145700, ["y"] = 239800},
					{["OtherIndexName"] = "Shinsoo", ["x"] = 352300, ["y"] = 882700}},
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {10, 110}, -- nivel minim / nivel maxim (delimiteaza nivelele la care te poti teleporta in mapa respectiva)
				["TeleportItemCheck"] = false
			},
			{
				["TeleportMapName"] = "Bazar(Lv.1+)",
				["TeleportHasOtherIndexs"] = false,
				["TeleportLevelCheck"] = false,
				["TeleportLevel"] = {40, 105}, -- nivel minim / nivel maxim (delimiteaza nivelele la care te poti teleporta in mapa respectiva)
				["TeleportItemCheck"] = false,
				["x"] = 181400,
				["y"] = 1169300
			},
			{
				["TeleportMapName"] = "Valea Orcilor(Lv.30+)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Mijloc", ["x"] = 332700, ["y"] = 746300},
					{["OtherIndexName"] = "Zona Metine 1", ["x"] = 280400, ["y"] = 795200},
					{["OtherIndexName"] = "Zona Metine 2", ["x"] = 344000, ["y"] = 703600},
					{["OtherIndexName"] = "Zona Metine 3", ["x"] = 294900, ["y"] = 715700},
					{["OtherIndexName"] = "Zona Metine 4", ["x"] = 298700, ["y"] = 773400}},
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {30, 110}, -- nivel minim / nivel maxim (delimiteaza nivelele la care te poti teleporta in mapa respectiva)
				["TeleportItemCheck"] = false
			},			
			{
				["TeleportMapName"] = "Desert(Lv.30+)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Mijloc", ["x"] = 296300, ["y"] = 547500},
					{["OtherIndexName"] = "Zona Metine 1", ["x"] = 242700, ["y"] = 516800},
					{["OtherIndexName"] = "Zona Metine 2", ["x"] = 340900, ["y"] = 537000},
					{["OtherIndexName"] = "Zona Metine 3", ["x"] = 337500, ["y"] = 595900}},
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {30, 110}, -- nivel minim / nivel maxim (delimiteaza nivelele la care te poti teleporta in mapa respectiva)
				["TeleportItemCheck"] = false
			},
			{
				["TeleportMapName"] = "Muntele Sohan(Lv.35+)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Mijloc", ["x"] = 436100, ["y"] = 215100},
					{["OtherIndexName"] = "Zona Metine 1", ["x"] = 379100, ["y"] = 258000},
					{["OtherIndexName"] = "Zona Metine 2", ["x"] = 481300, ["y"] = 229500},
					{["OtherIndexName"] = "Zona Metine 3", ["x"] = 492100, ["y"] = 257400}},
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {35, 110}, -- nivel minim / nivel maxim (delimiteaza nivelele la care te poti teleporta in mapa respectiva)
				["TeleportItemCheck"] = false
			},
			{
				["TeleportMapName"] = "Temnita Paianjenilor(Lv.35-80)", -- Teleport Map Name
				["TeleportHasOtherIndexs"] = true, -- true = citeste "TeleportTheOtherIndexes" de mai jos, adica cand dai click iti arata si alte optiuni de teleportare / false = opusul.
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Temnita Paianjenilor 1", ["x"] = 51200, ["y"] = 486400},
					{["OtherIndexName"] = "Temnita Paianjenilor 2", ["x"] = 665600, ["y"] = 435200},					-- Optiunile de teleportare ale Mapei de mai sus (Pot fi adaugate oricate)
					{["OtherIndexName"] = "Temnita Paianjenilor 3", ["x"] = 97600, ["y"] = 571050}},
				["TeleportLevelCheck"] = true, -- true = iti citeste limitele de nivel adaugate mai jos / false = opusul.
				["TeleportLevel"] = {35, 80}, -- nivel minim / nivel maxim (delimiteaza nivelele la care te poti teleporta in mapa respectiva)
				["TeleportItemCheck"] = false
			},
			{
				["TeleportMapName"] = "Tara de Foc(Lv.40+)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Mijloc", ["x"] = 599300, ["y"] = 707200},
					{["OtherIndexName"] = "Zona Metine 1", ["x"] = 663900, ["y"] = 631600},
					{["OtherIndexName"] = "Zona Metine 2", ["x"] = 650300, ["y"] = 752600},
					{["OtherIndexName"] = "Zona Metine 3", ["x"] = 683300, ["y"] = 694800}},
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {40, 110}, -- nivel minim / nivel maxim (delimiteaza nivelele la care te poti teleporta in mapa respectiva)
				["TeleportItemCheck"] = false
			},
			{
				["TeleportMapName"] = "Taramul Gigantilor(Lv.40+)",
				["TeleportHasOtherIndexs"] = false,
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {40, 110}, -- nivel minim / nivel maxim (delimiteaza nivelele la care te poti teleporta in mapa respectiva)
				["TeleportItemCheck"] = false,
				["x"] = 828100,
				["y"] = 763400
			},
			{
				["TeleportMapName"] = "Padurea Fantomelor(Lv.50+)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Start", ["x"] = 290400, ["y"] = 5500},
					{["OtherIndexName"] = "Mijloc", ["x"] = 302500, ["y"] = 25900},
					{["OtherIndexName"] = "Sfarsit", ["x"] = 289000, ["y"] = 41900}},
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {50, 110}, -- nivel minim / nivel maxim (delimiteaza nivelele la care te poti teleporta in mapa respectiva)
				["TeleportItemCheck"] = false
			},
			{
				["TeleportMapName"] = "Padurea Rosie(Lv.50+)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Start", ["x"] = 1119900, ["y"] = 69200},
					{["OtherIndexName"] = "Sfarsit", ["x"] = 1120200, ["y"] = 7100},
					{["OtherIndexName"] = "Zona Metine 1", ["x"] = 1060000, ["y"] = 63400},
					{["OtherIndexName"] = "Zona Metine 2", ["x"] = 1061800, ["y"] = 24600},
					{["OtherIndexName"] = "Zona Metine 3", ["x"] = 1099400, ["y"] = 23200}},
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {50, 110}, -- nivel minim / nivel maxim (delimiteaza nivelele la care te poti teleporta in mapa respectiva)
				["TeleportItemCheck"] = false
			},
			{
				["TeleportMapName"] = "Grota Exilului(Lv.75+)",
				["TeleportHasOtherIndexs"] = false,
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {75, 110},
				["TeleportItemCheck"] = false,
				["TeleportItems"] = {30190, 1},
				["x"] = 145300,
				["y"] = 1323600
			},
			{
				["TeleportMapName"] = "Grota Exilului 2(Lv.75+)",
				["TeleportHasOtherIndexs"] = false,
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {75, 110},
				["TeleportItemCheck"] = false,
				["TeleportItems"] = {30190, 1},
				["x"] = 153600,
				["y"] = 1203200
			},
			{
				["TeleportMapName"] = "Peninsula(Lv.90+)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Start", ["x"] = 1085300, ["y"] = 1783300},
					{["OtherIndexName"] = "Zona Metine 1", ["x"] = 1161300, ["y"] = 1790200},
					{["OtherIndexName"] = "Zona Metine 2", ["x"] = 1156700, ["y"] = 1688300},
					{["OtherIndexName"] = "Zona Metine 3", ["x"] = 1047900, ["y"] = 1739800},
					{["OtherIndexName"] = "Zona Metine 4", ["x"] = 1050900, ["y"] = 1692000}},
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {90, 110},
				["TeleportItemCheck"] = false
			},
			{
				["TeleportMapName"] = "Nephrite(Lv.90+)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Start", ["x"] = 1087400, ["y"] = 1646000},
					{["OtherIndexName"] = "Zona Metine 1", ["x"] = 1057000, ["y"] = 1525200},
					{["OtherIndexName"] = "Zona Metine 2", ["x"] = 1089700, ["y"] = 1615500},
					{["OtherIndexName"] = "Zona Metine 3", ["x"] = 1070800, ["y"] = 1569600},
					{["OtherIndexName"] = "Zona Metine 4", ["x"] = 1062200, ["y"] = 1634600}},
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {90, 110},
				["TeleportItemCheck"] = false
			},
			{
				["TeleportMapName"] = "Thunder(Lv.90+)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Start", ["x"] = 1134700, ["y"] = 1653900},
					{["OtherIndexName"] = "Zona Metine 1", ["x"] = 1143200, ["y"] = 1535900},
					{["OtherIndexName"] = "Zona Metine 2", ["x"] = 1146400, ["y"] = 1597500},
					{["OtherIndexName"] = "Zona Metine 3", ["x"] = 1210800, ["y"] = 1639200},
					{["OtherIndexName"] = "Zona Metine 4", ["x"] = 1213700, ["y"] = 1523900}},
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {90, 110},
				["TeleportItemCheck"] = false
			},
			{
				["TeleportMapName"] = "Gautama(Lv.90+)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Start", ["x"] = 1225700, ["y"] = 1682000},
					{["OtherIndexName"] = "Zona Metine 1", ["x"] = 1194200, ["y"] = 1681000},
					{["OtherIndexName"] = "Zona Metine 2", ["x"] = 1215800, ["y"] = 1742600},
					{["OtherIndexName"] = "Zona Metine 3", ["x"] = 1277600, ["y"] = 1745500},
					{["OtherIndexName"] = "Zona Metine 4", ["x"] = 1320600, ["y"] = 1691600}},
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {90, 110},
				["TeleportItemCheck"] = false
			},			
			{
				["TeleportMapName"] = "Enchanted Forest(Lv.100+)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Start", ["x"] = 813300, ["y"] = 1503200},
					{["OtherIndexName"] = "Zona Metine 1", ["x"] = 785500, ["y"] = 1471700},
					{["OtherIndexName"] = "Zona Metine 2", ["x"] = 824800, ["y"] = 1464300},
					{["OtherIndexName"] = "Zona Metine 3", ["x"] = 805100, ["y"] = 1420400}},
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {100, 110},
				["TeleportItemCheck"] = false,
				["TeleportItems"] = {30190, 1}
			},
			{
				["TeleportMapName"] = "Taramul Lavei(Lv.110)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] = {
					{["OtherIndexName"] = "Zona Metine 1", ["x"] = 2653200, ["y"] = 2271700},
					{["OtherIndexName"] = "Zona Metine 2", ["x"] = 2677800, ["y"] = 2330600},
					{["OtherIndexName"] = "Zona Metine 3", ["x"] = 2617400, ["y"] = 2344500},
					{["OtherIndexName"] = "Zona Metine 4", ["x"] = 2670300, ["y"] = 2365300},
					{["OtherIndexName"] = "Zona Metine 5", ["x"] = 2671000, ["y"] = 2386900},
					{["OtherIndexName"] = "Zona Metine 6", ["x"] = 2717000, ["y"] = 2375300}},
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {110, 110},
				["TeleportItemCheck"] = false,
				["TeleportItems"] = {30190, 1}
			},
			{
				["TeleportMapName"] = "Nisipuri(Lv.110+)",
				["TeleportHasOtherIndexs"] = true,
				["TeleportTheOtherIndexes"] ={
					{["OtherIndexName"] = "Start", ["x"] = 933900, ["y"] = 1446700}},
				["TeleportLevelCheck"] = true,
				["TeleportLevel"] = {110, 120},
				["TeleportItemCheck"] = false,
			},
		};
	end -- if
	
	return TeleportRing.data;
end -- func

TeleportRing.TeleportIsLevelCheck = function(index)
	local data = TeleportRing.ReturnData();
	
	return data[index]["TeleportLevelCheck"];
end -- func

TeleportRing.TeleportIsItemsCheck = function(index)
	local data = TeleportRing.ReturnData();
	
	return data[index]["TeleportItemCheck"];
end -- func

TeleportRing.TeleportMapHasOtherIndex = function(index)
	local data = TeleportRing.ReturnData();
	
	return data[index]["TeleportHasOtherIndexs"];
end -- func

TeleportRing.InsertWindow = function()
	local data = TeleportRing.ReturnData(); local window = {};
	
	for index in data do table.insert(window, data[index]["TeleportMapName"]); end -- for
	table.insert(window, "Renunta");
	
	return window;
end -- func

TeleportRing.DisplayOtherIndexWindow = function(WinIndex)
	local data = TeleportRing.ReturnData(); local OtherWindow = {};
	
	for index in data[WinIndex]["TeleportTheOtherIndexes"] do table.insert(OtherWindow, data[WinIndex]["TeleportTheOtherIndexes"][index]["OtherIndexName"]); end -- for
	table.insert(OtherWindow, "Renunta");
	
	return OtherWindow;
end -- func

TeleportRing.isBetweenLevel = function(index)
	local data = TeleportRing.ReturnData(); local pLevel = pc.get_level();
	
	return pLevel >= data[index]["TeleportLevel"][1] and pLevel <= data[index]["TeleportLevel"][2];
end -- func

TeleportRing.hasItemNeeded = function(index)
	local data = TeleportRing.ReturnData();
	
	return pc.count_item(data[index]["TeleportItems"][1]) >= data[index]["TeleportItems"][2];
end -- func

-- The main data window of the ring

TeleportRing.MainWindow = function(iVnum)
	local data = TeleportRing.ReturnData(); local InsertedWindowTable = TeleportRing.InsertWindow();
	
	say_title(string.format("%s:", item_name(iVnum)))
	local FirstWindow = select_table(InsertedWindowTable);
	if (FirstWindow == table.getn(InsertedWindowTable)) then
		return false;
	else
		say_title(string.format("%s:", item_name(iVnum)))
		if TeleportRing.TeleportIsLevelCheck(FirstWindow) then
			if (not TeleportRing.isBetweenLevel(FirstWindow)) then
				say(string.format("Your level must be between %d and %d to[ENTER]be able to teleport in this map.", data[FirstWindow]["TeleportLevel"][1], data[FirstWindow]["TeleportLevel"][2]))
				return false;
			end -- if
		end -- if
		
		if TeleportRing.TeleportIsItemsCheck(FirstWindow) then
			if (not TeleportRing.hasItemNeeded(FirstWindow)) then
				say(string.format("Trebuie sa deti x%d - %s pentru[ENTER]a putea intra in aceasta mapa.", data[FirstWindow]["TeleportItems"][2], item_name(data[FirstWindow]["TeleportItems"][1])))
				return false;
			end -- if
		end -- if
		
		if TeleportRing.TeleportMapHasOtherIndex(FirstWindow) then
			local OtherInsertedWindowTable = TeleportRing.DisplayOtherIndexWindow(FirstWindow);
			
			local otherWind = select_table(OtherInsertedWindowTable);
			if (otherWind == table.getn(OtherInsertedWindowTable)) then
				return false;
			else
				if (not pc.can_warp()) then say("Wait 10 seconds in order to teleport!.") return false; end -- if
				
				if data[FirstWindow]["TeleportItemCheck"] then
					pc.remove_item(data[FirstWindow]["TeleportItems"][1], data[FirstWindow]["TeleportItems"][2]);
				end -- if
				return pc.warp(data[FirstWindow]["TeleportTheOtherIndexes"][otherWind]["x"], data[FirstWindow]["TeleportTheOtherIndexes"][otherWind]["y"]);
			end -- if
		end -- if
		
		if (not pc.can_warp()) then say("Wait 10 seconds in order to teleport!.") return false; end -- if
		if data[FirstWindow]["TeleportItemCheck"] then
			pc.remove_item(data[FirstWindow]["TeleportItems"][1], data[FirstWindow]["TeleportItems"][2]);
		end -- if
		return pc.warp(data[FirstWindow]["x"], data[FirstWindow]["y"]);
	end -- if
end -- func