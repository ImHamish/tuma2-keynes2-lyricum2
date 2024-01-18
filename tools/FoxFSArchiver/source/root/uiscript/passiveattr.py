import uiscriptlocale

ROOT="d:/ymir work/ui/game/passive_attr/"
PUBLIC_PATH="d:/ymir work/ui/public/"

WINDOW_WIDTH=200
WINDOW_HEIGHT=215

BG_START_X=13
BG_START_Y=35

BG_WIDTH=172
BG_HEIGHT=172

window={
	"name":"PassiveAttrUI",
	"style":("movable", "float",),
	"x":(SCREEN_WIDTH-WINDOW_WIDTH)/2,
	"y":(SCREEN_HEIGHT-WINDOW_HEIGHT)/2,
	"width":WINDOW_WIDTH,
	"height":WINDOW_HEIGHT,
	"children":
	[
		{
			"name":"board",
			"type":"board",
			"style":("attach",),
			"x":0,
			"y":0,
			"width":WINDOW_WIDTH,
			"height":WINDOW_HEIGHT,
			"children":
			[
				{
					"name":"TitleBar",
					"type":"titlebar",
					"style":("attach",),
					"x":8,
					"y":7,
					"width":WINDOW_WIDTH-16,
					"color":"yellow",
					"children":
					[
						{
							"name":"TitleName",
							"type":"text",
							"x":WINDOW_WIDTH/2,
							"y":3,
							"text":uiscriptlocale.PASSIVE_ATTR_UI_TITLE,
							"text_horizontal_align":"center"
						},
					],
				},
				{
					"name":"bg",
					"type":"image",
					"x":BG_START_X,
					"y":BG_START_Y-2,
					"image":ROOT+"bg.sub",
					"children":
					[
						{
							"name":"passive_attr_sub_slot1",
							"type":"slot",
							"x":0,
							"y":0,
							"width":BG_WIDTH,
							"height":BG_HEIGHT,
							"slot":
							[
								{"index":EQUIPMENT_TALISMAN1_START, "x":83-BG_START_X, "y":48-BG_START_Y, "width":32, "height":32},
								{"index":EQUIPMENT_TALISMAN1_START+1, "x":83-BG_START_X, "y":162-BG_START_Y, "width":32, "height":32},
								{"index":EQUIPMENT_TALISMAN1_START+2, "x":26-BG_START_X, "y":105-BG_START_Y, "width":32, "height":32},
								{"index":EQUIPMENT_TALISMAN1_START+3, "x":140-BG_START_X, "y":105-BG_START_Y, "width":32, "height":32},
							],
						},
						{
							"name":"passive_attr_sub_slot2",
							"type":"slot",
							"x":0,
							"y":0,
							"width":BG_WIDTH,
							"height":BG_HEIGHT,
							"slot":
							[
								{"index":EQUIPMENT_TALISMAN2_START, "x":83-BG_START_X, "y":48-BG_START_Y, "width":32, "height":32},
								{"index":EQUIPMENT_TALISMAN2_START+1, "x":83-BG_START_X, "y":162-BG_START_Y, "width":32, "height":32},
								{"index":EQUIPMENT_TALISMAN2_START+2, "x":26-BG_START_X, "y":105-BG_START_Y, "width":32, "height":32},
								{"index":EQUIPMENT_TALISMAN2_START+3, "x":140-BG_START_X, "y":105-BG_START_Y, "width":32, "height":32},
							],
						},
						{
							"name":"pendant_bonus",
							"type":"expanded_image",
							"x":83-BG_START_X,
							"y":105-BG_START_Y,
							"image":"d:/ymir work/ui/pendant/bonus.tga",
						},
						{
							"name":"deck_button1",
							"type":"radio_button",
							"x":19-BG_START_X,
							"y":169-BG_START_Y,
							"default_image":ROOT+"up_button_default.sub",
							"over_image":ROOT+"up_button_over.sub",
							"down_image":ROOT+"up_button_down.sub",
						},
						{
							"name":"deck_button2",
							"type":"radio_button",
							"x":147-BG_START_X,
							"y":169-BG_START_Y,
							"default_image":ROOT+"down_button_default.sub",
							"over_image":ROOT+"down_button_over.sub",
							"down_image":ROOT+"down_button_down.sub",
						},
					],
				},
			],
		},
	],
}
