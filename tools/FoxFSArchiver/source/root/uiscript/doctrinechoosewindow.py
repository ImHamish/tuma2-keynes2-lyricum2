import uiscriptlocale

BOARD_WIDTH=299
BOARD_HEIGHT=214

window={
	"name":"SelectDialog",
	"style":("movable", "float",),
	"x":0,
	"y":0,
	"width":BOARD_WIDTH,
	"height":BOARD_HEIGHT,
	"children" :
	[
		{
			"name":"Board",
			"type":"board",
			"style":("attach",),
			"x":0,
			"y":0,
			"width":BOARD_WIDTH,
			"height":BOARD_HEIGHT,
			"children" :
			[
				{
					"name":"TitleBar",
					"type":"titlebar",
					"style":("attach",),
					"x":8,
					"y":10,
					"width":BOARD_WIDTH-16,
					"children" :
					[
						{
							"name":"TitleName",
							"type":"text",
							"text":uiscriptlocale.SKILL_SELECT_1,
							"horizontal_align":"center",
							"text_horizontal_align":"center",
							"x":0,
							"y":4,
						},
					],
				},
				{
					"name":"board_1",
					"type":"thinboard_circle",
					"style":("attach",),
					"x":10,
					"y":40,
					"width":BOARD_WIDTH-21,
					"height":44,
					"children" :
					[
						{
							"name":"doctrine1_bg",
							"type":"grid_table",
							"x":6,
							"y":4,
							"start_index":0,
							"x_count":6,
							"y_count":1,
							"x_step":36,
							"y_step":36,
							"x_blank":10,
							"y_blank":1,
							"image":"d:/ymir work/ui/skillslot.tga",
						},
						{
							"name":"doctrine1_slot",
							"type":"grid_table",
							"x":8,
							"y":6,
							"start_index":0,
							"x_count":6,
							"y_count":1,
							"x_step":36,
							"y_step":36,
							"x_blank":10,
							"y_blank":1,
						},
					],
				},
				{
					"name":"doctrine1_button",
					"type":"button",
					"x":(BOARD_WIDTH-180)/2,
					"y":88,
					"default_image":"d:/ymir work/ui/public/xlarge_button_01.sub",
					"over_image":"d:/ymir work/ui/public/xlarge_button_02.sub",
					"down_image":"d:/ymir work/ui/public/xlarge_button_03.sub",
				},
				{
					"name":"board_2",
					"type":"thinboard_circle",
					"style":("attach",),
					"x":10,
					"y":40+90,
					"width":BOARD_WIDTH-21,
					"height":44,
					"children" :
					[
						{
							"name":"doctrine2_bg",
							"type":"grid_table",
							"x":6,
							"y":4,
							"start_index":0,
							"x_count":6,
							"y_count":1,
							"x_step":36,
							"y_step":36,
							"x_blank":10,
							"y_blank":1,
							"image":"d:/ymir work/ui/skillslot.tga",
						},
						{
							"name":"doctrine2_slot",
							"type":"grid_table",
							"x":8,
							"y":6,
							"start_index":0,
							"x_count":6,
							"y_count":1,
							"x_step":36,
							"y_step":36,
							"x_blank":10,
							"y_blank":1,
						},
					],
				},
				{
					"name":"doctrine2_button",
					"type":"button",
					"x":(BOARD_WIDTH-180)/2,
					"y":88+90,
					"default_image":"d:/ymir work/ui/public/xlarge_button_01.sub",
					"over_image":"d:/ymir work/ui/public/xlarge_button_02.sub",
					"down_image":"d:/ymir work/ui/public/xlarge_button_03.sub",
				},
			],
		},
	],
}
