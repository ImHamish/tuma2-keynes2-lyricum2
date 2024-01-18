import uiscriptlocale

ROOT_OLD = "d:/ymir work/ui/minimap/"
ROOT_NEW = "d:/ymir work/ui/game/new_minimap/"
ROOT = "d:/ymir work/ui/minimap/"
ROOT2 = "d:/ymir work/ui/"
B_WIDTH = 154
B_HEIGHT = 154

window = {
	"name":"MiniMap_Window",
	"x":SCREEN_WIDTH-B_WIDTH+10,
	"y":0,
	"width":B_WIDTH,
	"height":B_HEIGHT,
	"children" :
	[
		{
			"name":"OpenWindow",
			"type":"window",
			"x":0,
			"y":0,
			"width":B_WIDTH,
			"height":B_HEIGHT,
			"children" :
			[
				{
					"name" : "OpenWindowBGI",
					"type" : "image",
					"x" : 6,
					"y" : 4,
					"image" : "new/minimap/bg.tga",
				},
				{
					"name" : "MiniMapWindow",
					"type" : "window",
					"x" : 14 + 4,
					"y" : 17 + 2,
					"width" : 117,
					"height" : 115,
				},
				{
					"name" : "RankingButton",
					"type" : "button",
					"x" : 16,
					"y" : 106,
					# "default_image" : ROOT_NEW + "button_rank_normal.tga",
					# "over_image" : ROOT_NEW + "button_rank_over.tga",
					# "down_image" : ROOT_NEW + "button_rank_down.tga",
				},
				{
					"name" : "BlockEquipButton",
					"type" : "button",
					"x" : 6,
					"y" : 76,
					# "default_image" : ROOT_NEW + "button_block_normal.tga",
					# "over_image" : ROOT_NEW + "button_block_over.tga",
					# "down_image" : ROOT_NEW + "button_block_down.tga",
				},
				{
					"name":"WikiButton",
					"type":"button",
					"x":65,
					"y":132,
					# "default_image":ROOT2+"wiki_btn_01.png",
					# "over_image":ROOT2+"wiki_btn_02.png",
					# "down_image":ROOT2+"wiki_btn_03.png",
				},
				{
					"name" : "ScaleUpButton",
					"type" : "button",
					"x" : 125,
					"y" : 107,
					"default_image" : "new/minimap/button_plus_normal.tga",
					"over_image" : "new/minimap/button_plus_over.tga",
					"down_image" : "new/minimap/button_plus_down.tga",
				},
				{
					"name" : "ScaleDownButton",
					"type" : "button",
					"x" : 111,
					"y" : 121,
					"default_image" : "new/minimap/button_minus_normal.tga",
					"over_image" : "new/minimap/button_minus_over.tga",
					"down_image" : "new/minimap/button_minus_down.tga",
				},
				{
					"name" : "AtlasDownButton",
					"type" : "button",
					"x" : 91,
					"y" : 133,
					"default_image" : "new/minimap/button_atlas_normal.tga",
					"over_image" : "new/minimap/button_atlas_over.tga",
					"down_image" : "new/minimap/button_atlas_down.tga",
				},
				{
					"name" : "MiniMapHideButton",
					"type" : "button",
					"x" : 112,
					"y" : 11,
					"default_image" : "new/minimap/button_close_normal.tga",
					"over_image" : "new/minimap/button_close_over.tga",
					"down_image" : "new/minimap/button_close_down.tga",
				},
				{
					"name":"textInfoBar",
					"type":"bar",
					"x":10,
					"y":163+30,
					"color":0x00000000,
					"width":132,
					"height":28,
					"children":
					(
						{
							"name":"textInfoValue2",
							"type":"text",
							"x":0,
							"y":3,
							"text":"",
							"outline":1,
							"horizontal_align":"center",
							"text_horizontal_align":"center"
						},
					),
				},
				{
					"name":"textInfoBar2",
					"type":"bar",
					"x":8,
					"y":163+3,
					"color":0x00000000,
					"width":132,
					"height":28,
					"children":
					(
						{
							"name":"ServerInfo",
							"type":"text",
							"x":0,
							"y":1,
							"text":"",
							"outline":1,
							"horizontal_align":"center",
							"text_horizontal_align":"center"
						},
						{
							"name":"MiniMapClock",
							"type":"text",
							"x":0,
							"y":12,
							"text":"",
							"outline":1,
							"horizontal_align":"center",
							"text_horizontal_align":"center"
						},
						{
							"name":"TimeInfo",
							"type":"text",
							"x":0,
							"y":12,
							"text":"",
							"outline":1,
							"horizontal_align":"center",
							"text_horizontal_align":"center"
						},
						{
							"name":"RenderInfo",
							"type":"text",
							"text_horizontal_align":"center",	
							"outline":1,
							"x":-25,
							"y":-140,
							"text":"",
						},
					),
				},
				{
					"name":"ObserverCount",
					"type":"text",
					"text_horizontal_align":"center",
					"outline":1,
					"x":82,
					"y":213,
					"text":"",
				},
			],
		},
		{
			"name":"CloseWindow",
			"type":"window",
			"x":10,
			"y":0,
			"width":132,
			"height":48,
			"children" :
			[
				{
					"name":"MiniMapShowButton",
					"type":"button",
					"x":100,
					"y":4,
					"default_image":ROOT_OLD+"minimap_open_default.sub",
					"over_image":ROOT_OLD+"minimap_open_default.sub",
					"down_image":ROOT_OLD+"minimap_open_default.sub",
				},
			],
		},
		{
			"name":"MastWindow",
			"type":"thinboard",
			"x":-18,
			"y":215,
			"width":165,
			"height":66,
			"children" :
			[
				{
					"name":"MastText",
					"type":"text",
					"text_horizontal_align":"center",
					"x":85,
					"y":8,
				},
				{
					"name":"MastHp",
					"type":"gauge",
					"x":42,
					"y":23,
					"width":85,
					"color":"red",
				},
				{
					"name":"ActualMastText",
					"type":"text",
					"text_horizontal_align":"center",
					"x":84,
					"y":31,
				},
				{
					"name":"MastTimerText",
					"type":"text",
					"text_horizontal_align":"center",
					"x":85,
					"y":42,
					"text":"-:-",
				},
			],
		},
	],
}
