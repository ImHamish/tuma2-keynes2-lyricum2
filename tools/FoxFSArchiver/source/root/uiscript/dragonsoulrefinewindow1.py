import uiscriptlocale

window = {
	"name" : "DragonSoulRefineWindow",

	## ¿ëÈ¥¼® Ã¢ ¹Ù·Î ¿ÞÂÊ
	"x" : SCREEN_WIDTH - 176 - 272 - 10 - 272,
	"y" : SCREEN_HEIGHT - 37 - 505,

	"style" : ("movable", "float",),

	"width" : 272,
	"height" : 195,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 272,
			"height" : 195,

			"children" :
			(
				## Base BackGroud Image
				{
					"name" : "DragonSoulRefineWindowBaseImage",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,

					"image" : "d:/ymir work/ui/dragonsoul/dragon_soul_refine_bg.tga",
				},

				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 5,
					"y" : 7,

					"width" : 265,
					"color" : "yellow",

					"children" :
					(
						{ 
							"name":"TitleName", 
							"type":"text", 
							"x":140, 
							"y":5, 
							"text":uiscriptlocale.DRAGONSOUL_REFINE_WINDOW_TITLE, 
							"text_horizontal_align":"center" 
						},
					),
				},
				
				## Refine Slot
				{
					"name" : "RefineSlot",
					"type" : "grid_table",

					#"image" : "d:/ymir work/ui/dragonsoul/cap.tga", 

					"x" : 20,
					"y" : 42,

					"start_index" : 0,
					"x_count" : 2,
					"y_count" : 1,
					"x_step" : 32+9,
					"y_step" : 32+9,
				},

				## Result Slot
				{
					"name" : "ResultSlot",
					"type" : "grid_table",

					"x" : 219,
					"y" : 42,

					"start_index" : 0,
					"x_count" : 2,
					"y_count" : 3,
					"x_step" : 32,
					"y_step" : 32,
				},
				
				## Grade Button
				{
					"name" : "GradeButton",
					"type" : "toggle_button",

					"x" : 39,
					"y" : 120,

					"default_image" : "d:/ymir work/ui/dragonsoul/button_def.png",
					"over_image" : "d:/ymir work/ui/dragonsoul/button_over.png",
					"down_image" : "d:/ymir work/ui/dragonsoul/button_down.png",

					"children" :
					(
						{
							"name" : "GradeSlotTitle",
							"type" : "text",
							"x" : 0,
							"y" : 0,
							"all_align" : "center",
							"text" : uiscriptlocale.GRADE_SELECT,
							"color" : 0xFFF1E6C0,
						},
					),
				},

				## Step Button
				{
					"name" : "StepButton",
					"type" : "toggle_button",

					"x" : 39,
					"y" : 89,

					"default_image" : "d:/ymir work/ui/dragonsoul/button_def.png",
					"over_image" : "d:/ymir work/ui/dragonsoul/button_over.png",
					"down_image" : "d:/ymir work/ui/dragonsoul/button_down.png",

					"children" :
					(
						{
							"name" : "StepSlotTitle",
							"type" : "text",
							"x" : 0,
							"y" : 0,
							"all_align" : "center",
							"text" : uiscriptlocale.STEP_SELECT,
							"color" : 0xFFF1E6C0,
						},
					),
				},

				## Refine Button
				{
					"name" : "StrengthButton",
					"type" : "toggle_button",

					"x" : 153,
					"y" : 89,

					"default_image" : "d:/ymir work/ui/dragonsoul/button_def.png",
					"over_image" : "d:/ymir work/ui/dragonsoul/button_over.png",
					"down_image" : "d:/ymir work/ui/dragonsoul/button_down.png",

					"children" :
					(
						{
							"name" : "RefineSlotTitle",
							"type" : "text",
							"x" : 0,
							"y" : 0,
							"all_align" : "center",
							"text" : uiscriptlocale.STRENGTH_SELECT,
							"color" : 0xFFF1E6C0,
						},
					),
				},

				## Money Print
				{
					"name":"Money_Slot",
					"type" : "text",

					"x":165,
					"y":144,

					#"horizontal_align" : "right",
					#"text_horizontal_align" : "right",

					"text" : "123456789",
				},

				## Do Refine Button
				{
					"name" : "DoRefineButton",
					"type" : "button",

					"x" : 87,
					"y" : 165,

					"default_image" : "d:/ymir work/ui/dragonsoul/button_def.png",
					"over_image" : "d:/ymir work/ui/dragonsoul/button_over.png",
					"down_image" : "d:/ymir work/ui/dragonsoul/button_down.png",

					"children" :
					(
						{ 
							"name" : "DoRefineButtonTitle", 
							"type" : "text", 
							"x" : 0, 
							"y" : 0, 
							"text" : uiscriptlocale.DO_REFINE, 
							"all_align" : "center",
						},
					),
				},
				## Do Refine All Button
				{
					"name" : "DoRefineAllButton",
					"type" : "button",
					"x" : 28,
					"y" : 165,
					
					"default_image" : "d:/ymir work/ui/dragonsoul/button_def.png",
					"over_image" : "d:/ymir work/ui/dragonsoul/button_over.png",
					"down_image" : "d:/ymir work/ui/dragonsoul/button_down.png",
					
					"children" :
					(
						{
							"name" : "DoRefineAllButtonTitle", 
							"type" : "text", 
							"x" : 0, 
							"y" : 0, 
							"text" : uiscriptlocale.DO_REFINE_ALL, 
							"all_align" : "center",
							"color" : 0xFFF1E6C0,
						},
					),
				},
			),
		},
	),
}

