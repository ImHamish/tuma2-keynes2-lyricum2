import uiScriptLocale
import item

window = {
	"name" : "sideBarBoard",

	"x" : -40,
	"y" : 3,

	"width" : 60,
	"height" : 185,


	"style" : ("movable", "float",),

	"children" :
	(
		{
			"name" : "board",
			"type" : "invisibleboard",
			"style" : ("attach",),

			"x" : 0,
			"y" : 24,

			"width" : 60,
			"height" : 185,

			"children" : 
			(
				## Search Shop
				{
					"name" : "SearchButton",
					"type" : "button",

					"x" : 11,
					"y" : 7,

					"default_image" : "d:/ymir work/ui/game/windows/btn_search_norm.png",
					"over_image" : "d:/ymir work/ui/game/windows/btn_search_hover.png",
					"down_image" : "d:/ymir work/ui/game/windows/btn_search_down.png",
				},
				## Shop Offline
				{
					"name" : "ShopButton",
					"type" : "button",

					"x" : 11,
					"y" : 11+35,

					"default_image" : "d:/ymir work/ui/game/windows/btn_shop_norm.png",
					"over_image" : "d:/ymir work/ui/game/windows/btn_shop_hover.png",
					"down_image" : "d:/ymir work/ui/game/windows/btn_shop_down.png",
				},
				## Special Storage
				{
					"name" : "StorageButton",
					"type" : "button",

					"x" : 11,
					"y" : 15 + 35*2,

					"default_image" : "d:/ymir work/ui/game/windows/btn_storage_norm.png",
					"over_image" : "d:/ymir work/ui/game/windows/btn_storage_hover.png",
					"down_image" : "d:/ymir work/ui/game/windows/btn_storage_down.png",
				},
				
				## Switchbot
				{
					"name" : "SwitchBotButton",
					"type" : "button",

					"x" : 11,
					"y" : 19 + 35*3,

					"default_image" : "d:/ymir work/ui/game/windows/btn_switchbot_norm.png",
					"over_image" : "d:/ymir work/ui/game/windows/btn_switchbot_hover.png",
					"down_image" : "d:/ymir work/ui/game/windows/btn_switchbot_down.png",
				},
			),
		},
	),
}