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
	"style" : (),
	"x" : 171,
	"y" : 3,
	"width" : 300,
	"height" : 324,
	"children" :
	(
		{
			"name" : "camera_mode_window",
			"type" : "window",
			"x" : 0,
			"y" : 0,
			"width" : 304,
			"height" : SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "camera_mode_title_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.tga",
					"children":
					(
						{
							"name" : "title_camera_mode",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : uiscriptlocale.GAME_OPTIONS_TYPE,
						},
					),
				},
				{
					"name" : "camera_level0",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"text" : uiscriptlocale.OPTION_CAMERA_DISTANCE_SHORT,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "camera_level1",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33,
					"text" : uiscriptlocale.OPTION_CAMERA_DISTANCE_LONG,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "camera_level2",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 33,
					"text" : uiscriptlocale.OPTION_CAMERA_DISTANCE_LONG_LONG,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
			),
		},
		{
			"name" : "camera_perspective_window",
			"type" : "window",
			"x" : 0,
			"y" : SMALL_OPTION_HEIGHT,
			"width":304,
			"height":SLIDER_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "sound_title_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.tga",
					"children":
					(
						{
							"name" : "title_sound",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : uiscriptlocale.CAMERA_PERSPECTIVE,
						},
					),
				},
				{
					"name" : "camera_perspective_controller",
					"type" : "sliderbar",
					"x" : OPTION_START_X+SLIDER_POSITION_X,
					"y" : SLIDER_START_Y,
				},
			),
		},
	),
}
