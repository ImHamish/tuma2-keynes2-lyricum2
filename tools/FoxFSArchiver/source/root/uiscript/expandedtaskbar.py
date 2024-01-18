import uiscriptlocale

BOARD_WIDTH = 0
BOARD_HEIGHT = 0

LAST_I = 3
LAST_X_POS = 0

window = {
	"name":"ExpandTaskBar",
	"x" : SCREEN_WIDTH / 2 - 5,
	"y" : SCREEN_HEIGHT - 74,
	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,
	"children":
	[
		{
			"name":"ExpanedTaskBar_Board",
			"type":"window",
			"x":0,
			"y":0,
			"width":BOARD_WIDTH,
			"height":BOARD_HEIGHT,
			"children":
			[
		{
			"name":"battlepass_button",
			"type" : "button",
			"x" : 37,
			"y" : 0,
			"width" : 37,
			"height" : 37,
			"tooltip_text":uiscriptlocale.BATTLEPASS_TITLE,
			"default_image":"icon/item/bp_button_01.tga",
			"over_image":"icon/item/bp_button_02.tga",
			"down_image":"icon/item/bp_button_03.tga",
		},
		{
			"name":"reward_button",
			"type" : "button",
			"x" : 37 * 2,
			"y" : 0,
			"width" : 37,
			"height" : 37,
			"tooltip_text":uiscriptlocale.REWARD_SYSTEM_1,
			"default_image":"icon/item/party_button_01.tga",
			"over_image":"icon/item/party_button_02.tga",
			"down_image":"icon/item/party_button_03.tga",
		},
			],
		},
	],
}

if USE_AUTO_HUNT:
	window["width"] = 37 * LAST_I
	window["height"] = 37 * LAST_I
	window["children"][0]["width"] = window["children"][0]["width"] + 37

	if LAST_I == 1:
		window["children"][0]["height"] = window["children"][0]["height"] + 37

	window["children"][0]["children"] = window["children"][0]["children"] + [
		{
			"name" : "autohunt_button",
			"type" : "button",
			"x" : 37 * (LAST_I - 1),
			"y" : 0,
			"width" : 37,
			"height" : 37,
			"tooltip_text" : uiscriptlocale.KEYCHANGE_AUTO_WINDOW,
			"default_image" : "icon/item/taskbar_auto_button_01.tga",
			"over_image" : "icon/item/taskbar_auto_button_02.tga",
			"down_image" : "icon/item/taskbar_auto_button_03.tga",
		},
	]

	LAST_X_POS += 38
	LAST_I += 1
