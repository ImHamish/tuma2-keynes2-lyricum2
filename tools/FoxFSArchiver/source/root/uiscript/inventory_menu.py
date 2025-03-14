import uiscriptlocale

BUTTONS_COUNT = 5
BOARD_WIDTH = 24
BOARD_HEIGHT = (35 * BUTTONS_COUNT + 35 * 2)
IMG_PATH = "d:/ymir work/ui/game/new_inventory/"

window = {
	"name" : "InventoryMenuWindow",
	"x" : 0,
	"y" : 0,
	"style" : ("movable", "float",),
	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,
	"children" :
	(
		{
			"name" : "board",
			"type" : "bar",
			"x" : 0,
			"y" : 0,
			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,
			"color" : 0x00000000,
			"children" : 
			(
				{
					"name" : "button1",
					"type" : "button",
					"x" : -12,
					"y" : 7,
					"tooltip_text" : "SearchShop",
					"tooltip_x" : -54,
					"tooltip_y" : 7,
					"default_image" : "new/sidebar/search_normal.png",
					"over_image" : "new/sidebar/search_hover.png",
					"down_image" : "new/sidebar/search_down.png",
				},
				{
					"name" : "button2",
					"type" : "button",
					"x" : -12,
					"y" : 7+35,
					"tooltip_text" : uiscriptlocale.SWITCH_BOT,
					"tooltip_x" : -54,
					"tooltip_y" : 7,
					"default_image" : "new/sidebar/switch_normal.png",
					"over_image" : "new/sidebar/switch_hover.png",
					"down_image" : "new/sidebar/switch_down.png",
				},
				{
					"name" : "button3",
					"type" : "button",
					"x" : -12,
					"y" : 7+35*2,
					"tooltip_text" : uiscriptlocale.BIOLOG_TITLE,
					"tooltip_x" : -54,
					"tooltip_y" : 7,
					"default_image" : "new/sidebar/bio_normal.png",
					"over_image" : "new/sidebar/bio_hover.png",
					"down_image" : "new/sidebar/bio_down.png",
				},
				{
					"name" : "button4",
					"type" : "button",
					"x" : -12,
					"y" : 7+35*3,
					"tooltip_text" : uiscriptlocale.DUNGEON_TITLE,
					"tooltip_x" : -54,
					"tooltip_y" : 7,
					"default_image" : "new/sidebar/boss_normal.png",
					"over_image" : "new/sidebar/boss_hover.png",
					"down_image" : "new/sidebar/boss_down.png",
				},
				{
					"name" : "button5",
					"type" : "button",
					"x" : -12,
					"y" : 7+35*4,
					"tooltip_text" : "Event Calendar",
					"tooltip_x" : -54,
					"tooltip_y" : 7,
					"default_image" : "new/sidebar/calendar_normal.png",
					"over_image" : "new/sidebar/calendar_hover.png",
					"down_image" : "new/sidebar/calendar_down.png",
				},
				{
					"name" : "button6",
					"type" : "button",
					"x" : -12,
					"y" : 7+35*5,
					"tooltip_text" : "AntiXP Ring",
					"tooltip_x" : -54,
					"tooltip_y" : 7,
					"default_image" : "new/sidebar/antiexp_normal.png",
					"over_image" : "new/sidebar/antiexp_hover.png",
					"down_image" : "new/sidebar/antiexp_down.png",
				},
				{
					"name" : "button7",
					"type" : "button",
					"x" : -12,
					"y" : 7+35*6,
					"tooltip_text" : "Wiki",
					"tooltip_x" : -54,
					"tooltip_y" : 7,
					"default_image" : "new/sidebar/wiki_normal.png",
					"over_image" : "new/sidebar/wiki_hover.png",
					"down_image" : "new/sidebar/wiki_down.png",
				},
			),
		},
	),
}
