--[[
	Inizializes: The 'Warp' class.
]]
Warp = {};

--[[
	Returns:
		The full warp config.

	Structure:
		[section_key] = {
			["section"] = "The section name", 
			["data"] = {
				{
					["name"] = "The name of the first map",
					["warp"] = {
						[1] = {["x"] = x_shinsoo, ["y"] = y_shinsoo},
						[2] = {["x"] = x_chunjo, ["y"] = y_chunjo},
						[3] = {["x"] = x_jinno, ["y"] = y_jinno}
					},
					["price"] = the warp cost,
					["level"] = the minimum level to enter the map,
					["flag"] = The flag which has to be enabled to enter the map
				},
				{
					["name"] = "The name of the second map",
					["warp"] = {
						[1] = {["x"] = x_shinsoo, ["y"] = y_shinsoo},
						[2] = {["x"] = x_chunjo, ["y"] = y_chunjo},
						[3] = {["x"] = x_jinno, ["y"] = y_jinno}
					},
					["price"] = the warp cost,
					["level"] = the minimum level to enter the map,
					["item_data"] = {["vnum"] = the item required, ["quantity"] = the quantity needed}, 
					["flag"] = The flag which has to be enabled to enter the map
				},
				..
			}
		},
]]
Warp.GetConfig = function()
	local config = {
		[1] = {
			["name"] = "Villaggi", 
			["data"] = {
				{["name"] = "Shinsoo: Yongan", ["warp"] = {[1] = {["x"] = 474300, ["y"] = 954800}, [2] = {["x"] = 474300, ["y"] = 954800}, [3] = {["x"] = 474300, ["y"] = 954800}}, ["price"] = 1000, ["level"] = 1, ["item_data"] = {["vnum"] = nil, ["quantity"] = nil}, ["flag"] = nil},
				{["name"] = "Shinsoo: Jayang", ["warp"] = {[1] = {["x"] = 353100, ["y"] = 882900}, [2] = {["x"] = 353100, ["y"] = 882900}, [3] = {["x"] = 353100, ["y"] = 882900}}, ["price"] = 1000, ["level"] = 1, ["item_data"] = {["vnum"] = nil, ["quantity"] = nil}, ["flag"] = nil},
				{["name"] = "Chunjo: Joan",    ["warp"] = {[1] = {["x"] = 63800,  ["y"] = 166400}, [2] = {["x"] = 63800,  ["y"] = 166400}, [3] = {["x"] = 63800,  ["y"] = 166400}}, ["price"] = 1000, ["level"] = 1, ["item_data"] = {["vnum"] = nil, ["quantity"] = nil}, ["flag"] = nil},
				{["name"] = "Chunjo: Bokjung", ["warp"] = {[1] = {["x"] = 145500, ["y"] = 240000}, [2] = {["x"] = 145500, ["y"] = 240000}, [3] = {["x"] = 145500, ["y"] = 240000}}, ["price"] = 1000, ["level"] = 1, ["item_data"] = {["vnum"] = nil, ["quantity"] = nil}, ["flag"] = nil},
				{["name"] = "Jinno: Pyungmoo", ["warp"] = {[1] = {["x"] = 959900, ["y"] = 269200}, [2] = {["x"] = 959900, ["y"] = 269200}, [3] = {["x"] = 959900, ["y"] = 269200}}, ["price"] = 1000, ["level"] = 1, ["item_data"] = {["vnum"] = nil, ["quantity"] = nil}, ["flag"] = nil},
				{["name"] = "Jinno: Bakra",    ["warp"] = {[1] = {["x"] = 863900, ["y"] = 246000}, [2] = {["x"] = 863900, ["y"] = 246000}, [3] = {["x"] = 863900, ["y"] = 246000}}, ["price"] = 1000, ["level"] = 1, ["item_data"] = {["vnum"] = nil, ["quantity"] = nil}, ["flag"] = nil},
			}
		},
		[2] = {
			["name"] = "Mappe Neutrali",
			["data"] = {
				{["name"] = "Valle di Seungryong",         ["warp"] = {[1] = {["x"] = 402100,  ["y"] = 673900},  [2] = {["x"] = 270400,  ["y"] = 739900},  [3] = {["x"] = 321300,  ["y"] = 808000}},  ["price"] = 3000, ["level"] = 25, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},  ["flag"] = nil},
				{["name"] = "Deserto Yongbi",              ["warp"] = {[1] = {["x"] = 217800,  ["y"] = 627200},  [2] = {["x"] = 221900,  ["y"] = 502700},  [3] = {["x"] = 344000,  ["y"] = 502500}},  ["price"] = 5000, ["level"] = 25, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},  ["flag"] = nil},
				{["name"] = "Sotterraneo dei Ragni 1",     ["warp"] = {[1] = {["x"] = 60000,   ["y"] = 496000},  [2] = {["x"] = 60000,   ["y"] = 496000},  [3] = {["x"] = 60000,   ["y"] = 496000}},  ["price"] = 5000, ["level"] = 25, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},  ["flag"] = nil},
				{["name"] = "Sotterraneo dei Ragni 2",     ["warp"] = {[1] = {["x"] = 91835,   ["y"] = 525504},  [2] = {["x"] = 91835,   ["y"] = 525504},  [3] = {["x"] = 91835,   ["y"] = 525504}},  ["price"] = 5000, ["level"] = 25, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},  ["flag"] = nil},
				{["name"] = "Sotterraneo dei Ragni 3",     ["warp"] = {[1] = {["x"] = 51200,   ["y"] = 563200},  [2] = {["x"] = 51200,   ["y"] = 563200},  [3] = {["x"] = 51200,   ["y"] = 563200}},  ["price"] = 5000, ["level"] = 75, ["item_data"] = {["vnum"] = 71095, ["quantity"] = 1},    ["flag"] = nil},
				{["name"] = "Monte Sohan",                 ["warp"] = {[1] = {["x"] = 434200,  ["y"] = 290600},  [2] = {["x"] = 375200,  ["y"] = 174900},  [3] = {["x"] = 491800,  ["y"] = 173600}},  ["price"] = 5000, ["level"] = 25, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},  ["flag"] = nil},
				{["name"] = "Tempio Hwang",                ["warp"] = {[1] = {["x"] = 553700,  ["y"] = 145000},  [2] = {["x"] = 553700,  ["y"] = 145000},  [3] = {["x"] = 553700,  ["y"] = 145000}},  ["price"] = 7000, ["level"] = 25, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},  ["flag"] = nil},
				{["name"] = "Doyyumhwaji",                 ["warp"] = {[1] = {["x"] = 599400,  ["y"] = 756300},  [2] = {["x"] = 597800,  ["y"] = 622200},  [3] = {["x"] = 730700,  ["y"] = 689800}},  ["price"] = 7000, ["level"] = 25, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},  ["flag"] = nil},
				{["name"] = "Foresta Fantasma",            ["warp"] = {[1] = {["x"] = 288700,  ["y"] = 5700},    [2] = {["x"] = 288700,  ["y"] = 5700},    [3] = {["x"] = 288700,  ["y"] = 5700}},    ["price"] = 8000, ["level"] = 60, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},  ["flag"] = nil},
				{["name"] = "Bosco Rosso",                 ["warp"] = {[1] = {["x"] = 1119600, ["y"] = 70000},   [2] = {["x"] = 1119600, ["y"] = 70000},   [3] = {["x"] = 1119600, ["y"] = 70000}},   ["price"] = 8000, ["level"] = 60, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},  ["flag"] = nil},
				{["name"] = "Grotta dell' Esilio 1",       ["warp"] = {[1] = {["x"] = 284300,  ["y"] = 810300},  [2] = {["x"] = 284300,  ["y"] = 810300},  [3] = {["x"] = 284300,  ["y"] = 810300}},  ["price"] = 8000, ["level"] = 75, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},  ["flag"] = nil},
				{["name"] = "Grotta dell' Esilio 2",       ["warp"] = {[1] = {["x"] = 241633,  ["y"] = 1275269}, [2] = {["x"] = 241633,  ["y"] = 1275269}, [3] = {["x"] = 241633,  ["y"] = 1275269}}, ["price"] = 8000, ["level"] = 75, ["item_data"] = {["vnum"] = 30190, ["quantity"] = 1},    ["flag"] = nil},
				{["name"] = "Capo Fuoco del Drago",        ["warp"] = {[1] = {["x"] = 1024000, ["y"] = 1664000}, [2] = {["x"] = 1024000, ["y"] = 1664000}, [3] = {["x"] = 1024000, ["y"] = 1664000}}, ["price"] = 8000, ["level"] = 95, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},  ["flag"] = nil},
				{["name"] = "Baia di Nefrite",             ["warp"] = {[1] = {["x"] = 1049600, ["y"] = 1510400}, [2] = {["x"] = 1049600, ["y"] = 1510400}, [3] = {["x"] = 1049600, ["y"] = 1510400}}, ["price"] = 8000, ["level"] = 95, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},  ["flag"] = nil},
				--{["name"] = "Falesia di Guatama",          ["warp"] = {[1] = {["x"] = 1177600, ["y"] = 1664000}, [2] = {["x"] = 1177600, ["y"] = 1664000}, [3] = {["x"] = 1177600, ["y"] = 1664000}}, ["price"] = 8000, ["level"] = 95, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},  ["flag"] = nil},
				--{["name"] = "Monti Tonanti",               ["warp"] = {[1] = {["x"] = 1126400, ["y"] = 1510400}, [2] = {["x"] = 1126400, ["y"] = 1510400}, [3] = {["x"] = 1126400, ["y"] = 1510400}}, ["price"] = 8000, ["level"] = 95, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},  ["flag"] = nil},
			}
		},
		[3] = {
			["name"] = "Terra delle Gilde",
			["data"] = {
				{["name"] = "Shinsoo: Jungrang", ["warp"] = {[1] = {["x"] = 135600, ["y"] = 4300},  [2] = {["x"] = 135600, ["y"] = 4300},  [3] = {["x"] = 135600, ["y"] = 4300}},  ["price"] = 1000, ["level"] = 1, ["item_data"] = {["vnum"] = nil, ["quantity"] = nil}, ["flag"] = nil},
				{["name"] = "Chunjo: Waryong",   ["warp"] = {[1] = {["x"] = 221900, ["y"] = 9300},  [2] = {["x"] = 221900, ["y"] = 9300},  [3] = {["x"] = 221900, ["y"] = 9300}},  ["price"] = 1000, ["level"] = 1, ["item_data"] = {["vnum"] = nil, ["quantity"] = nil}, ["flag"] = nil},
				{["name"] = "Jinno: Imha",       ["warp"] = {[1] = {["x"] = 271800, ["y"] = 13000}, [2] = {["x"] = 271800, ["y"] = 13000}, [3] = {["x"] = 271800, ["y"] = 13000}}, ["price"] = 1000, ["level"] = 1, ["item_data"] = {["vnum"] = nil, ["quantity"] = nil}, ["flag"] = nil},
			}
		},
		[4] = {
			["name"] = "Dungeon",
			["data"] = {
				{["name"] = "Torre dei Demoni",          ["warp"] = {[1] = {["x"] = 590600,  ["y"] = 111000},  [2] = {["x"] = 590600,  ["y"] = 111000},  [3] = {["x"] = 590600,  ["y"] = 111000}},  ["price"] = 3000, ["level"] = 40, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},   ["flag"] = nil},
				{["name"] = "Stanza di Cristallo",       ["warp"] = {[1] = {["x"] = 181191,  ["y"] = 1220716}, [2] = {["x"] = 181191,  ["y"] = 1220716}, [3] = {["x"] = 181191,  ["y"] = 1220716}}, ["price"] = 3000, ["level"] = 75, ["item_data"] = {["vnum"] = 30190, ["quantity"] = 1},     ["flag"] = nil},
				{["name"] = "Catacombe del Diavolo",     ["warp"] = {[1] = {["x"] = 591649,  ["y"] = 100205},  [2] = {["x"] = 591649,  ["y"] = 100205},  [3] = {["x"] = 591649,  ["y"] = 100205}},  ["price"] = 3000, ["level"] = 75, ["item_data"] = {["vnum"] = nil,   ["quantity"] = nil},   ["flag"] = nil},
				{["name"] = "Stanza degli Aracnidi",     ["warp"] = {[1] = {["x"] = 68900,   ["y"] = 610800},  [2] = {["x"] = 68900,   ["y"] = 610800},  [3] = {["x"] = 68900,   ["y"] = 610800}},  ["price"] = 5000, ["level"] = 75, ["item_data"] = {["vnum"] = 71095, ["quantity"] = 1},     ["flag"] = nil},
			}
		}
	};

	return config;
end -- function

--[[
	Returns:
		An array containing all the existent sections' names.
]]
Warp.GetSection = function()
	local section_names = {};
	local data = Warp.GetConfig();

	for _, section in ipairs(data) do
		table.insert(section_names, section["name"]);
	end -- for

	return section_names;
end -- function

--[[
	Returns:
		name, coordinates, price and minimum level of the selected map.

	Explanation:
		The selected map is given by the number inserted in the 'section' argument
		which gets the section_key from the Warp.GetConfig array. (see the Warp.GetConfigs' structure)
]]
Warp.GetBuildSection = function(section)
	local names, warp, price, level, item_data, flag = {}, {}, {}, {}, {}, {};
	local data = Warp.GetConfig()[section]["data"];

	for _, section in ipairs(data) do
		table.insert(names,     section["name"]);
		table.insert(warp,      section["warp"][pc.get_empire()]);
		table.insert(price,     section["price"]);
		table.insert(level,     section["level"]);
		table.insert(item_data, {["vnum"] = section["item_data"]["vnum"], ["quantity"] = section["item_data"]["quantity"]});
		table.insert(flag,      section["flag"]);
	end -- for

	return names, warp, price, level, item_data, flag;
end -- function