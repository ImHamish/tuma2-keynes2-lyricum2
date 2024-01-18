import uiscriptlocale

BOARD_WIDTH=176
BOARD_HEIGHT=290
if SCREEN_HEIGHT <= 600:
	POS_Y=0
else:
	POS_Y=SCREEN_HEIGHT-352-37-25-BOARD_HEIGHT+2

window = {
	"name":"EquipmentDialog",
	"style":("movable", "float","rtl"),
	"x":SCREEN_WIDTH-BOARD_WIDTH,
	"y":POS_Y,
	"width":BOARD_WIDTH,
	"height":BOARD_HEIGHT,
	"children":
	[
		{
			"name":"board",
			"type":"board",
			"x":0,
			"y":0,
			"width":BOARD_WIDTH,
			"height":BOARD_HEIGHT,
			"children":
			[
				{
					"name":"TitleBar",
					"type":"titlebar",
					"style":("attach",),
					"x":8,
					"y":7,
					"width":161,
					"color":"yellow",
					"children":
					[
						{
							"name":"TitleName",
							"type":"text",
							"x":77,
							"y":3,
							"text":uiscriptlocale.EQUIPMENT_WINDOW,
							"text_horizontal_align":"center"
						},
					],
				},
				{
					"name":"base_tab",
					"type":"image",
					"x":10,
					"y":33,
					"width":BOARD_WIDTH,
					"height":25,
					"image":"d:/ymir work/ui/equipment_bg_without_ring_tab00.tga",
				},
				{
					"name":"equipment_window",
					"type":"window",
					"x":0,
					"y":56,
					"width":BOARD_WIDTH,
					"height":BOARD_HEIGHT,
					"children":[
						{
							"name":"equipment_base_image",
							"type":"image",
							"style":("attach",),
							"x":0,
							"y":0,
							"horizontal_align":"center",
							"image":"d:/ymir work/ui/equipment_bg_without_ring.tga",
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
								{
									"name":"nohide_button_piece7",
									"type":"button",
									"x":75-8+32,
									"y":2+2,
									"default_image":"d:/ymir work/ui/pattern/visible_mark_01.tga",
									"over_image":"d:/ymir work/ui/pattern/visible_mark_02.tga",
									"down_image":"d:/ymir work/ui/pattern/visible_mark_01.tga",
								},
								{
									"name":"yeshide_button_piece7",
									"type":"button",
									"x":75-8+32,
									"y":2+2,
									"default_image":"d:/ymir work/ui/pattern/visible_mark_03.tga",
									"over_image":"d:/ymir work/ui/pattern/visible_mark_02.tga",
									"down_image":"d:/ymir work/ui/pattern/visible_mark_03.tga",
								},
							],
						},
						{
							"name":"dragon_soul_button",
							"type":"button",
							"x":15,
							"y":186,
							"tooltip_text":uiscriptlocale.TASKBAR_DRAGON_SOUL,
							"default_image":"icon/butoane/Dss_button.png",
							"over_image":"icon/butoane/Dss_button_over.png",
							"down_image":"icon/butoane/Dss_button_down.png",
						},
						{
							"name":"extra_inventory_button",
							"type":"button",
							"x":53,
							"y":186,
							"tooltip_text":uiscriptlocale.EXTRA_INVENTORY_TITLE,
							"default_image":"d:/ymir work/ui/game/taskbar/mall_button_01.tga",
							"over_image":"d:/ymir work/ui/game/taskbar/mall_button_02.tga",
							"down_image":"d:/ymir work/ui/game/taskbar/mall_button_03.tga",
						},
						{
							"name":"premium_private_shop_button",
							"type":"button",
							"x":91,
							"y":186,
							"tooltip_text":uiscriptlocale.OFFLINE_SHOP,
							"default_image":"icon/item/private_button_01.tga",
							"over_image":"icon/item/private_button_02.tga",
							"down_image":"icon/item/private_button_03.tga",
						},
						{
							"name":"costume_button",
							"type":"button",
							"x":130,
							"y":186,
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
