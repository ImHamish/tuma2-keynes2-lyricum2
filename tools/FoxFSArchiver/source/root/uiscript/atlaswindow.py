import uiScriptLocale

ROOT = "d:/ymir work/ui/minimap/"

window = {
	"name" : "AtlasWindow",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH - 136 - 256 - 10,
	"y" : 0,

	"width" : 256 + 15,
	"height" : 256 + 38,

	"children" :
	(
		## BOARD
		{
			"name" : "board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : 256 + 15,
			"height" : 256 + 38,

			"title" : uiScriptLocale.ZONE_MAP,
			"children":
			(
				{
					"name": "boss_refresh",
					"type": "button",

					"x": 20,
					"y": 10,

					"tooltip_text": uiScriptLocale.MARKLIST_REFRESH,

					"default_image": "d:/ymir work/ui/game/inventory/refresh_small_button_01.sub",
					"over_image": "d:/ymir work/ui/game/inventory/refresh_small_button_02.sub",
					"down_image": "d:/ymir work/ui/game/inventory/refresh_small_button_03.sub",
				},
			),
		},
	),
}
