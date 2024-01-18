import uiscriptlocale

BOARD_WIDTH=176
BOARD_HEIGHT=352

window = {
	"name":"InventoryWindow",
	"x":SCREEN_WIDTH-BOARD_WIDTH,
	"y":SCREEN_HEIGHT-BOARD_HEIGHT-37-25,
	"style":("movable", "float",),
	"width":BOARD_WIDTH,
	"height":BOARD_HEIGHT,
	"children":
	[
		{
			"name":"board",
			"type":"board",
			"style":("attach",),
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
							"text":uiscriptlocale.INVENTORY_TITLE,
							"text_horizontal_align":"center"
						},
					],
				},
				{
					"name":"Inventory_Tab_01",
					"type":"radio_button",
					"x":10,
					"y":33,
					"default_image":"d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image":"d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image":"d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text":uiscriptlocale.INVENTORY_PAGE_BUTTON_TOOLTIP_1,
					"children":
					[
						{
							"name":"Inventory_Tab_01_Print",
							"type":"text",
							"x":0,
							"y":0,
							"all_align":"center",
							"text":"I",
						},
					],
				},
				{
					"name":"Inventory_Tab_02",
					"type":"radio_button",
					"x":10+39,
					"y":33,
					"default_image":"d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image":"d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image":"d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text":uiscriptlocale.INVENTORY_PAGE_BUTTON_TOOLTIP_2,
					"children":
					[
						{
							"name":"Inventory_Tab_02_Print",
							"type":"text",
							"x":0,
							"y":0,
							"all_align":"center",
							"text":"II",
						},
					],
				},
				{
					"name":"Inventory_Tab_03",
					"type":"radio_button",
					"x":10+39+39,
					"y":33,
					"default_image":"d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image":"d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image":"d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text":uiscriptlocale.INVENTORY_PAGE_BUTTON_TOOLTIP_3,
					"children":
					[
						{
							"name":"Inventory_Tab_03_Print",
							"type":"text",
							"x":0,
							"y":0,
							"all_align":"center",
							"text":"III",
						},
					],
				},
				{
					"name":"Inventory_Tab_04",
					"type":"radio_button",
					"x":10+39+39+39,
					"y":33,
					"default_image":"d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image":"d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image":"d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text":uiscriptlocale.INVENTORY_PAGE_BUTTON_TOOLTIP_4,
					"children":
					(
						{
							"name":"Inventory_Tab_04_Print",
							"type":"text",
							"x":0,
							"y":0,
							"all_align":"center",
							"text":"IV",
						},
					),
				},
				{
					"name":"ItemSlot",
					"type":"grid_table",
					"x":8,
					"y":55,
					"start_index":0,
					"x_count":5,
					"y_count":9,
					"x_step":32,
					"y_step":32,
					"image":"d:/ymir work/ui/public/slot_base.sub"
				},
			],
		},
	],
}
