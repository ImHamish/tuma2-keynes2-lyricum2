import uiscriptlocale

ROOT = "d:/ymir work/ui/public/"
WINDOW_WIDTH = 330

window = {
	"name" : "WhisperDialog",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 0,

	"width" : WINDOW_WIDTH,
	"height" : 200,

	"children" :
	(
		{
			"name" : "board",
			"type" : "thinboard",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : WINDOW_WIDTH,
			"height" : 200,

			"children" :
			(
				## Title
				{
					"name" : "name_slot",
					"type" : "image",
					"style" : ("attach",),

					"x" : 10,
					"y" : 10,

					"image":"d:/ymir work/ui/public/Parameter_Slot_05.sub",

					"children" :
					(
						{
							"name" : "titlename",
							"type" : "text",

							"x" : 3,
							"y" : 3,

							"text" : uiscriptlocale.WHISPER_NAME,
						},
						{
							"name" : "titlename_edit",
							"type" : "editline",

							"x" : 3,
							"y" : 3,

							"width" : 120,
							"height" : 17,

							"input_limit" : PLAYER_NAME_MAX_LEN,

							"text" : uiscriptlocale.WHISPER_NAME,
						},
					),
				},

				## Button
				{
					"name" : "ignorebutton",
					"type" : "toggle_button",

					"x" : 145,
					"y" : 10,

					"text" : uiscriptlocale.WHISPER_BAN,

					"default_image" : "d:/ymir work/ui/public/small_thin_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/small_thin_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/small_thin_button_03.sub",
				},
				{
					"name" : "reportviolentwhisperbutton",
					"type" : "button",

					"x" : 145,
					"y" : 10,

					"text" : uiscriptlocale.WHISPER_REPORT,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
				{
					"name" : "acceptbutton",
					"type" : "button",

					"x" : 145,
					"y" : 10,

					"text" : uiscriptlocale.OK,

					"default_image" : "d:/ymir work/ui/public/small_thin_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/small_thin_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/small_thin_button_03.sub",
				},
				{
					"name" : "minimizebutton",
					"type" : "button",

					"x" : WINDOW_WIDTH - 41,
					"y" : 12,

					"tooltip_text" : uiscriptlocale.MINIMIZE,

					"default_image" : "d:/ymir work/ui/public/minimize_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/minimize_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/minimize_button_03.sub",
				},
				{
					"name" : "closebutton",
					"type" : "button",

					"x" : WINDOW_WIDTH - 24,
					"y" : 12,

					"tooltip_text" : uiscriptlocale.CLOSE,

					"default_image" : "d:/ymir work/ui/public/close_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/close_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/close_button_03.sub",
				},

				## ScrollBar
				{
					"name" : "scrollbar",
					"type" : "thin_scrollbar",

					"x" : WINDOW_WIDTH - 25,
					"y" : 35,

					"size" : WINDOW_WIDTH - 160,
				},

				## Edit Bar
				{
					"name" : "editbar",
					"type" : "bar",

					"x" : 10,
					"y" : 200 - 60,

					"width" : WINDOW_WIDTH - 18,
					"height" : 50,

					"color" : 0x77000000,

					"children" :
					(
						{
							"name" : "chatline",
							"type" : "editline",

							"x" : 5,
							"y" : 5,

							"width" : WINDOW_WIDTH - 70,
							"height" : 40,

							"with_codepage" : 1,
							"input_limit" : 40,
							"limit_width" : WINDOW_WIDTH - 90,
							"multi_line" : 1,
						},
						{
							"name" : "sendbutton",
							"type" : "button",

							"x" : WINDOW_WIDTH - 80,
							"y" : 10,

							"text" : uiscriptlocale.WHISPER_SEND,

							"default_image" : "d:/ymir work/ui/public/xlarge_thin_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/xlarge_thin_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/xlarge_thin_button_03.sub",
						},
					),
				},
			),
		},
	),
}
