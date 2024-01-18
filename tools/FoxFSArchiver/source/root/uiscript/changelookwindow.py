import uiscriptlocale

BUTTON_ROOT = "d:/ymir work/ui/public/"

window = {
	"name" : "ChangeLookWindow",
	"x" : 0,
	"y" : 0,
	"style" : ("movable", "float",),
	"width" : 235,
	"height" : 270,
	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),
			"x" : 0,
			"y" : 0,
			"width" : 235,
			"height" : 270,
			"children" :
			(
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 6,
					"y" : 6,
					"width" : 222,
					"color" : "yellow",
					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",
							"x" : 95,
							"y" : 3,
							"text" : "",
							"text_horizontal_align" : "center"
						},
					),
				},
				{
					"name" : "ChangeLook_SlotImg",
					"type" : "image",
					"x" : 24,
					"y" : 35,
					"image" : "d:/ymir work/ui/game/changelook/ingame_convert.tga",
					"children" :
					(
						{
							"name" : "ChangeLookSlot",
							"type" : "slot",
							"x" : 3,
							"y" : 3,
							"width" : 190,
							"height" : 200,
							"slot" : (
								{
									"index" : 0,
									"x" : 26,
									"y" : 20,
									"width" : 31,
									"height" : 96
								},
								{
									"index" : 1,
									"x" : 125,
									"y" : 20,
									"width" : 31,
									"height" : 96
								},
								{
									"index" : 2,
									"x" : 75,
									"y" : 126,
									"width" : 31,
									"height" : 31,
								},
							),
						},
						# {
							# "name" : "ChangeLookSlot_PassYangItem",
							# "type" : "slot",

							# "x" : 77.5,
							# "y" : 128,

							# "width" : 31,
							# "height" : 31,

							# "slot" : (
								# {
									# "index" : 0,
									# "x" : 1,
									# "y" : 1,
									# "width" : 31,
									# "height" : 31
								# }, # À¯·á¾ÆÀÌÅÛ
							# ),
						# },
					),
				},
				{
					"name" : "Cost",
					"type" : "text",
					"text" : "",
					"text_horizontal_align" : "center",
					"x" : 118,
					"y" : 215,
				},
				{
					"name" : "AcceptButton",
					"type" : "button",
					"x" : 55,
					"y" : 235,
					"default_image" : BUTTON_ROOT + "acceptbutton00.sub",
					"over_image" : BUTTON_ROOT + "acceptbutton01.sub",
					"down_image" : BUTTON_ROOT + "acceptbutton02.sub",
				},
				{
					"name" : "CancelButton",
					"type" : "button",
					"x" : 129,
					"y" : 235,
					"default_image" : BUTTON_ROOT + "canclebutton00.sub",
					"over_image" : BUTTON_ROOT + "cancleButton01.sub",
					"down_image" : BUTTON_ROOT + "cancleButton02.sub",
				},
				{
					"name" : "ChangeLookToolTIpButton",
					"type" : "button",
					"x" : 186,
					"y" : 8,
					"default_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
					"over_image" : "d:/ymir work/ui/pattern/q_mark_02.tga",
					"down_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
				},
			),
		},
	),
}
