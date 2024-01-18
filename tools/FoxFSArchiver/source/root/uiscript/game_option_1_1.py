import uiscriptlocale

IMG_DIR = "new/gameoption/"

TITLE_IMAGE_TEXT_X = 5
TITLE_IMAGE_TEXT_Y = 4

OPTION_START_X = 17
SLIDER_POSITION_X = 50

SLIDER_START_Y = 40
BUTTON_START_Y = 33
BUTTON_NEXT_Y = 20

RADIO_BUTTON_RANGE_X = 85
TOGGLE_BUTTON_RANGE_X = 85

RADIO_BUTTON_TEXT_X = 25
TOGGLE_BUTTON_TEXT_X = 20

SMALL_OPTION_HEIGHT = 65
NORMAL_OPTION_HEIGHT = 80
SLIDER_OPTION_HEIGHT = 65


window = {
	"name" : "GameOptionDialog",
	# Dont touch these lines!
	"style" : (),
	"x" : 171,
	"y" : 3,
	"width" : 300,
	"height" : 324,
	# Dont touch these lines!
	"children" :
	(
		{
			"name" : "time_mode_window",
			"type" : "window",
			"x" : 0,
			"y" : 0,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "time_mode_title_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.tga",
					"children":
					(
						{
							"name" : "title_time_mode",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : uiscriptlocale.TIME_TYPE,
						},
					),
				},
				{
					"name" : "time_pm_on",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"text" : uiscriptlocale.YES,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "time_pm_off",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33,
					"text" : uiscriptlocale.NO,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
			),
		},
	),
}