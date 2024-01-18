import uiscriptlocale

ROOT = "d:/ymir work/ui/game/"
Y_ADD_POSITION = 0

window = {
	"name" : "GiftBox",
	"x" : 10,
	"y" : SCREEN_HEIGHT - 132,
	"width" : 50,
	"height" : 50,
	"children" :
	(
		{
			"name" : "GiftBox_Icon",
			"type" : "button",
			"x" : 0,
			"y" : 0,
			"default_image" : "D:/Ymir Work/UI/Pattern/GiftBox/present_for_fruends_icon.tga",
			"over_image" : "D:/Ymir Work/UI/Pattern/GiftBox/present_for_fruends_icon.tga",
			"down_image" : "D:/Ymir Work/UI/Pattern/GiftBox/present_for_fruends_icon.tga",
		},
		
		{
			"name" : "GiftBox_ToolTip",
			"x" : 0,
			"y" : 0,
			"width" : 50,
			"height" : 10,
			"type" : "window",
		},
	),
}
