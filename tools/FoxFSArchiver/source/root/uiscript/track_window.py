import localeinfo

WINDOW_WIDTH = 636
WINDOW_HEIGHT = 408#518

IMG_DIR = "d:/ymir work/ui/game/track/ymirdesign/"

window = {
	"name" : "Track_Window",
	"x" : 0,
	"y" : 0,
	"style" : ("movable", "float",),
	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,
	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("movable", "attach", "float",),
			"x" : 0,
			"y" : 0,
			"width":WINDOW_WIDTH,
			"height":WINDOW_HEIGHT,
			"children" :
			(
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 6,
					"y" : 6,
					"width" : WINDOW_WIDTH - 13,
					"color" : "yellow",
					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",
							"x" : (WINDOW_WIDTH - 15) / 2,
							"y" : 3,
							"text" : localeinfo.TRACK_WINDOW_TITLE,
							"text_horizontal_align" : "center"
						},
					),
				},
				{
					"name" : "bg",
					"type" : "image",
					"style" : ("movable","attach", "float",),
					"x" : 10,
					"y" : 36,
					"image" : IMG_DIR + "bg_new.tga",
				},
#				{
#					"name" : "close",
#					"type" : "button",
#					"x" : WINDOW_WIDTH-31,
#					"y" : 3,
#					"default_image" : IMG_DIR + "close_0.tga",
#					"over_image" : IMG_DIR + "close_1.tga",
#					"down_image" : IMG_DIR + "close_2.tga",
#				},
				{
					"name" : "worldboss_info",
					"type" : "button",
					"x" : WINDOW_WIDTH - 171,
					"y" : 55,
					"default_image" : IMG_DIR + "info_bg.tga",
					"over_image" : IMG_DIR + "info_bg.tga",
					"down_image" : IMG_DIR + "info_bg.tga",
					"text" : "",
				},
				{
					"name" : "open_shop",
					"type" : "button",
					"x" : WINDOW_WIDTH - 171,
					"y" : 100,
					"default_image" : IMG_DIR + "open_shop_0.tga",
					"over_image" : IMG_DIR + "open_shop_1.tga",
					"down_image" : IMG_DIR + "open_shop_2.tga",
					"text" : localeinfo.TRACK_OPEN_SHOP,
				},
				{
					"name" : "dungeon_info",
					"type" : "radio_button",
					"x" : WINDOW_WIDTH - 171,
					"y" : 148,
					"default_image" : IMG_DIR + "dungeon_0.tga",
					"over_image" : IMG_DIR + "dungeon_1.tga",
					"down_image" : IMG_DIR + "dungeon_2.tga",
					"children":
					(
						{
							"name" : "dungeon_info_text",
							"type" : "multi_text2",
							"x" : 93,
							"y" : 32,
							"text" : localeinfo.TRACK_DUNGEON_INFORMATION,
							"text_type" : "horizontal#center",
							"text_range" : 11,
						},
					),
				},
				{
					"name" : "dungeon_info_notice",
					"type" : "toggle_button",
					"x" : WINDOW_WIDTH - 171,
					"y" : WINDOW_HEIGHT - 72,
					"default_image" : IMG_DIR + "checkbox.tga",
					"over_image" : IMG_DIR + "checkbox.tga",
					"down_image" : IMG_DIR + "checked.tga",
					"children":
					(
						{
							"name" : "dungeon_info_notice_text",
							"type" : "multi_text2",
							"x" : 22,
							"y" : -5,
							"text" : localeinfo.TRACK_DUNGEON_NOTICE_INFORMATION,
							"text_type" : "horizontal#left",
							"text_range" : 11,
						},
					),
				},
				{
					"name" : "worldboss_info_notice",
					"type" : "toggle_button",
					"x" : WINDOW_WIDTH - 171,
					"y" : WINDOW_HEIGHT - 42,
					"default_image" : IMG_DIR + "checkbox.tga",
					"over_image" : IMG_DIR + "checkbox.tga",
					"down_image" : IMG_DIR + "checked.tga",
					"children":
					(
						{
							"name" : "dungeon_info_notice_text",
							"type" : "multi_text2",
							"x" : 22,
							"y" : -5,
							"text" : localeinfo.TRACK_BOSS_NOTICE_INFORMATION,
							"text_type" : "horizontal#left",
							"text_range" : 11,
						},
					),
				},
				{
					"name" : "listbox",
					"type" : "listbox_new",
					"x" : 18,
					"y" : 50,
					"width" : 405,
					"height" : 342,
				},
			),
		},
	),
}
