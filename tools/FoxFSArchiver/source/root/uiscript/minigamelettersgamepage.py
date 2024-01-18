import uiscriptlocale

WINDOW_WIDTH = 262 + 20
WINDOW_HEIGHT = 46 + 27 + 118
WHITE_COLOR = 0xFFFFFFFF

window = {
	"name":"MiniGameLettersEventGamePage",
	"style":("movable", "float",),
	"x":SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y":SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	"width":WINDOW_WIDTH,
	"height":WINDOW_HEIGHT,
	"children":
	(
		{
			"name":"board",
			"type":"board",
			"style":("attach",),
			"x":0,
			"y":0,
			"width":WINDOW_WIDTH,
			"height":WINDOW_HEIGHT,
			"children":
			(
				{
					"name":"titlebar",
					"type":"titlebar",
					"style":("attach",),
					"x":0,
					"y":0,
					"width":WINDOW_WIDTH,
					"color":"yellow",
					"children":
					(
						{
							"name":"TitleName",
							"type":"text",
							"x":0,
							"y":0,
							"text":uiscriptlocale.MINI_GAME_LETTERS_TITLE,
							"all_align":"center"
						},
					),
				},
				{
					"name":"board_event",
					"type":"expanded_image",
					"style":("attach",),
					"x":10, 
					"y":27,
					"image":"d:/ymir work/ui/letters_event/bg.tga",
				},
				{
					"name":"material_slot",
					"type":"grid_table",
					"x":10 + 13, 
					"y":27 + 7,
					"start_index":0,
					"x_count":5,
					"y_count":1,
					"x_step":32,
					"y_step":32,
					"x_blank":19,
					"y_blank":5,
					"image":"d:/ymir work/ui/public/slot_base.sub"
				},
				{
					"name":"board_reward",
					"type":"border_a",
					"x":10, 
					"y":76,
					"width" : 72, 
					"height" : 104,
					"children":
					[
						{
							"name":"reward_slot",
							"type":"grid_table",
							"x":20, 
							"y":36,
							"start_index":0,
							"x_count":1,
							"y_count":1,
							"x_step":32,
							"y_step":32,
							"image":"d:/ymir work/ui/public/slot_base.sub"
						},
					],
				},
				{
					"name":"reward_button",
					"type":"button",
					"x":10 + 78 + 2,
					"y":WINDOW_HEIGHT-32,
					"text":uiscriptlocale.MINI_GAME_LETTERS_COLLECT,
					"default_image":"d:/ymir work/ui/public/large_button_01.sub",
					"over_image":"d:/ymir work/ui/public/large_button_02.sub",
					"down_image":"d:/ymir work/ui/public/large_button_03.sub",
				},
				{
					"name":"help_button",
					"type":"button",
					"x":WINDOW_WIDTH - 14 - 88,
					"y":WINDOW_HEIGHT-32,
					"text":uiscriptlocale.MINI_GAME_LETTERS_HELP_TEXT,
					"default_image":"d:/ymir work/ui/public/large_button_01.sub",
					"over_image":"d:/ymir work/ui/public/large_button_02.sub",
					"down_image":"d:/ymir work/ui/public/large_button_03.sub",
				},
			),
		},
	),
}
