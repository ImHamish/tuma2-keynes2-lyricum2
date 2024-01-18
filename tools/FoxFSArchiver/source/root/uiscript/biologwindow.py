import uiscriptlocale

BOARD_WIDTH = 330
BOARD_HEIGHT = 305

window = {
	"name" : "BiologWindow",
	"x" : 0,
	"y" : 0,
	"style" : ("movable", "float",),
	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,
	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),
			"x" : 0,
			"y" : 0,
			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,
			"children" :
			(
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 6,
					"y" : 6,
					"width" : BOARD_WIDTH - 10,
					"color" : "yellow",
					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",
							"x" : 0,
							"y" : 3,
							"text" : uiscriptlocale.BIOLOG_WND_TITLE,
							"horizontal_align" : "center",
							"text_horizontal_align" : "center"
						},
					),
				},

				{
					"name" : "SeparateBaseImage",
					"type" : "image",
					# "style" : ("attach",),

					"x" : 10,
					"y" : 30,

					"image" : "d:/ymir work/ui/biolog_header.tga",

					"children" :
					(
						{
							"name" : "missionNameText",
							"type" : "text",
							"x" : 0,
							"y" : 10,
							"fontname" : "Arial:16b",
							"text" : "",
							"horizontal_align" : "center",
							"text_horizontal_align" : "center"
						},
						{
							"name" : "requiredItem", 
							"type" : "grid_table", 
							"x" : 26, 
							"y" : 55, 
							"start_index" : 0, 
							"x_count" : 1, 
							"y_count" : 1, 
							"x_step" : 32, 
							"y_step" : 32, 
						},
						{
							"name" : "countDelivered",
							"type" : "text",
							"x" : -70,
							"y" : 55,

							"text" : "",
							"horizontal_align" : "center",
							"text_horizontal_align":"center"
						},
						{
							"name" : "Shop_Button",
							"type" : "button",
							"x" : 85+5,
							"y" : 85,
							"horizontal_align" : "center",
							"default_image" : "d:/ymir work/ui/shop_icon_01.tga",
							"over_image" : "d:/ymir work/ui/shop_icon_02.tga",
							"down_image" : "d:/ymir work/ui/shop_icon_03.tga",
						},
						{
							"name" : "Deliver_Button",
							"type" : "button",
							"x" : 3,
							"y" : 52,
							"horizontal_align" : "center",
							"text" : uiscriptlocale.BIOLOGO_CONSEGNA_ITEM,
							"default_image" : "d:/ymir work/ui/Delivery_01.tga",
							"over_image" : "d:/ymir work/ui/Delivery_02.tga",
							"down_image" : "d:/ymir work/ui/Delivery_03.tga",
						},
						{
							"name" : "itemIcon1",
							"type" : "image",
							"x" : 34+100,
							"y" :80,
							"image" : "d:/ymir work/ui/game/biologist/40144.tga",
						},
						{
							"name" : "itemIcon2",
							"type" : "image",
							"x" : 85+105,
							"y" : 77,
							"image" : "d:/ymir work/ui/game/biologist/40143.tga",
						},


						{
							"name" : "clock",
							"type" : "image",
							"x" : 160+100,
							"y" : 5+100+25,
							"image" : "d:/ymir work/ui/clock.tga",
						},

						{
							"name" : "TimeText",
							"type" : "text",
							"x" : 120,
							"y" : 5+100+25,
							"text" : "",
							"text_horizontal_align" : "center"
						},

						{
							"name" : "rewardText",
							"type" : "text",
							"x" : 0,
							"y" : 5+100+25+20+5+2,
							"text" : uiscriptlocale.BIOLOG_WND_REWARD,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center"
						},
						{
							"name" : "rewardTypeText",
							"type" : "text",
							"x" : 0,
							"y" : 5+100+25+20+18,
							"text" : "",

							"horizontal_align" : "center",
							"text_horizontal_align" : "center"
						},

						{
							"name" : "rewardSlot",
							"type" : "slotbar",
							"x" : 9,
							"y" : 236 + 16 -67,
							"width" : 293,
							"height" : 69,
						},
						{
							"name" : "RewardButton",
							"type" : "button",
							"x" : 215,
							"y" : 52,
							"text" : uiscriptlocale.BIOLOG_WND_REWARD_BUTTON,
							"default_image" : "d:/ymir work/ui/Prize_01.tga",
							"over_image" : "d:/ymir work/ui/Prize_02.tga",
							"down_image" : "d:/ymir work/ui/Prize_03.tga",
						},
					),
				},
			),
		},
	),
}
