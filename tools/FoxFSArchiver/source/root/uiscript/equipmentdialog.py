import uiscriptlocale

BOARD_WIDTH=157+6
BOARD_HEIGHT=188+6

window = {
	"name":"EquipmentDialog",
#	"style":("movable", "float","rtl"),
	"x":0,
	"y":0,
	"width":BOARD_WIDTH,
	"height":BOARD_HEIGHT,
	"children":
	[
		{
			"name":"ui_thinboard",
			"type":"thinboard_circle",
			"x":0,
			"y":0,
			"width":BOARD_WIDTH,
			"height":BOARD_HEIGHT,
			"children":
			[
				{
					"name":"equipment_window",
					"type":"window",
					"x":1.5,
					"y":4,
					"width":BOARD_WIDTH-3,
					"height":BOARD_HEIGHT-3,
					"children":[
						{
							"name":"equipment_base_image",
							"type":"image",
#							"style":("attach",),
							"x":0,
							"y":0,
							"horizontal_align":"center",
							"image":"d:/ymir work/ui/equipment_bg_without_ring_view.tga",
							"children":[
								{
									"name":"primary_slot",
									"type":"slot",
									"x":3,
									"y":3,
									"width":150,
									"height":140,
									"slot":[
										{"index":EQUIPMENT_START_INDEX+0, "x":39, "y":37, "width":32, "height":64},
										{"index":EQUIPMENT_START_INDEX+1, "x":39, "y":2, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+2, "x":4, "y":99, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+3, "x":75, "y":67, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+4, "x":3, "y":3, "width":32, "height":96},
										{"index":EQUIPMENT_START_INDEX+5, "x":114, "y":67, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+6, "x":114, "y":35, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+9, "x":114, "y":2, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+10, "x":75, "y":35, "width":32, "height":32},
										{"index":EQUIPMENT_BELT, "x":39, "y":99, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+18, "x":75, "y":2, "width":32, "height":32},
										{"index":COSTUME_START_INDEX+2, "x":75, "y":99, "width":32, "height":32},
									],
								},
								{
									"name":"secondary_slot",
									"type":"slot",
									"x":3,
									"y":140,
									"width":150,
									"height":36,
									"slot": [
										{"index":EQUIPMENT_START_INDEX+7, "x":2, "y":7, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+8, "x":2+37, "y":7, "width":32, "height":32},
										{"index":COSTUME_EFFECT_BODY_SLOT, "x":2+37+37, "y":7, "width":32, "height":32},
										{"index":COSTUME_EFFECT_WEAPON_SLOT, "x":2+37+37+37, "y":7, "width":32, "height":32},
									],
								},
							],
						},
						{
							"name":"costume_button",
							"type":"button",
							"x":119,
							"y":102,
							"tooltip_text":uiscriptlocale.COSTUME_TITLE,
							"default_image":"d:/ymir work/ui/game/taskbar/costume_button_01.tga",
							"over_image":"d:/ymir work/ui/game/taskbar/costume_button_02.tga",
							"down_image":"d:/ymir work/ui/game/taskbar/costume_button_03.tga",
						},
					],
				},
			],
		},
	],
}
