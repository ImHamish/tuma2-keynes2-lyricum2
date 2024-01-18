import uiscriptlocale

BOARD_WIDTH=113+22
BOARD_HEIGHT=212+44

window = {
	"name":"CostumeWindow",
	"x":SCREEN_WIDTH-176-BOARD_WIDTH+2,
	"y":SCREEN_HEIGHT-352-37-25-290+2,
	#"style":("movable", "float",),
	"width":BOARD_WIDTH,
	"height":BOARD_HEIGHT,
	"children":
	[
		{
			"name":"board",
			"type":"board",
			#"style":("attach",),
			"x":0,
			"y":0,
			"width":BOARD_WIDTH,
			"height":BOARD_HEIGHT,
			"children":
			[
				{
					"name":"TitleBar",
					"type":"titlebar",
					#"style":("attach",),
					"x":8,
					"y":7,
					"width":120,
					"color":"yellow",
					"children":
					[
						{
							"name":"TitleName",
							"type":"text",
							"x":60,
							"y":3,
							"text":uiscriptlocale.COSTUME_WINDOW_TITLE,
							"text_horizontal_align":"center"
						},
					],
				},
				{
					"name":"costume_base",
					"type":"image",
					"x":11,
					"y":33,
					"image":"d:/ymir work/ui/new_costume_bg.jpg",
					"children":
					[
						{
							"name":"costume_slot",
							"type":"slot",
							"x":3,
							"y":3,
							"width":110,
							"height":209,
							"slot":[
								{"index":COSTUME_START_INDEX, "x":62, "y":45, "width":32, "height":64},
								{"index":COSTUME_START_INDEX+1, "x":62, "y":9, "width":32, "height":32},
								{"index":COSTUME_START_INDEX+3, "x":62, "y":126, "width":32, "height":32},
								{"index":COSTUME_SLOT_WEAPON, "x":13, "y":13, "width":32, "height":96},
								{"index":COSTUME_PETSKIN_SLOT, "x":13, "y":126, "width":32, "height":32},
								{"index":COSTUME_MOUNTSKIN_SLOT, "x":13, "y":162, "width":32, "height":32},
								{"index":COSTUME_SLOT_AURA, "x":62, "y":162, "width":32, "height":32},
							],
						},
					],
				},
			],
		},
	],
}
