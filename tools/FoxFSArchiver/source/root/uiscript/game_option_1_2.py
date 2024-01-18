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
			"name" : "show_name_window",
			"type" : "window",
			"x" : 0,
			"y" : 0,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "show_name_title_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.tga",
					"children":
					(
						{
							"name" : "title_show_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : uiscriptlocale.OPTION_ALWAYS_SHOW_NAME,
						},
					),
				},
				{
					"name" : "always_show_name_on_button",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"text" : uiscriptlocale.OPTION_ALWAYS_SHOW_NAME_ON,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "always_show_name_off_button",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33,
					"text" : uiscriptlocale.OPTION_ALWAYS_SHOW_NAME_OFF,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
			),
		},
		{
			"name" : "name_type_window",
			"type" : "window",
			"x" : 0,
			"y" : SMALL_OPTION_HEIGHT,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "name_type_title_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.tga",
					"children":
					(
						{
							"name" : "title_name_type",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : uiscriptlocale.OPTION_NAMES_TYPE_NAME,
						},
					),
				},
				{
					"name" : "name_type1_button",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"text" : uiscriptlocale.OPTION_NAMES_TYPE_NAME_0,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "name_type2_button",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33,
					"text" : uiscriptlocale.OPTION_NAMES_TYPE_NAME_1,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
			),
		},
		{
			"name" : "monster_info_window",
			"type" : "window",
			"x" : 0,
			"y" : SMALL_OPTION_HEIGHT * 2,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "monster_info_title_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.tga",
					"children":
					(
						{
							"name" : "title_monster_info",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : uiscriptlocale.OPTION_MOB_INFO,
						},
					),
				},
				{
					"name" : "show_mob_level_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"text" : uiscriptlocale.OPTION_MOB_INFO_LEVEL,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "show_mob_AI_flag_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33,
					"text" : uiscriptlocale.OPTION_MOB_INFO_AGGR,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
			),
		},

		{
			"name" : "effect_range_info_window",
			"type" : "window",
			"x" : 0,
			"y" : SMALL_OPTION_HEIGHT * 3,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "effect_range_info_title_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.tga",
					"children":
					(
						{
							"name" : "title_effect_range_info",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : uiscriptlocale.NEW_LOADING_EFFECT_DIST_TITLE,
						},
					),
				},
				{
					"name" : "effect_range_low",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"text" : uiscriptlocale.NEW_LOADING_DIST_OPTION3,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "effect_range_medium",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33,
					"text" : uiscriptlocale.NEW_LOADING_DIST_OPTION2,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "effect_range_high",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 33,
					"text" : uiscriptlocale.NEW_LOADING_DIST_OPTION1,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
			),
		},

		{
			"name" : "loading_dist_info_window",
			"type" : "window",
			"x" : 0,
			"y" : SMALL_OPTION_HEIGHT * 4,
			"width":304,
			"height":SLIDER_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "loading_dist_info_title_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.tga",
					"children":
					(
						{
							"name" : "title_loading_dist_info",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : uiscriptlocale.NEW_LOADING_DIST_TITLE,
						},
					),
				},

				{
					"name" : "loading_distance_max",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 33,
					"text" : uiscriptlocale.NEW_LOADING_DIST_OPTION1,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "loading_distance_mid",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33,
					"text" : uiscriptlocale.NEW_LOADING_DIST_OPTION2,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "loading_distance_min",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"text" : uiscriptlocale.NEW_LOADING_DIST_OPTION3,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
			),
		},

		{
			"name" : "dog_mode_window",
			"type" : "window",
			"x" : 0,
			"y" : SMALL_OPTION_HEIGHT * 5,
			"width":304,
			"height":SMALL_OPTION_HEIGHT * 2,
			"children":
			(
				{
					"name" : "dog_mode_title_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.tga",
					"children":
					(
						{
							"name" : "dog_mode_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : uiscriptlocale.DOGMODE,
						},
					),
				},
				{
					"name" : "dog_mode_0",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"text" : uiscriptlocale.AUTO_ON,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "dog_mode_1",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33,
					"text" : uiscriptlocale.AUTO_OFF,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "dogmode_apply",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33 * 2,
					"text" : uiscriptlocale.OPTION_TILING_APPLY,
					"default_image" : IMG_DIR + "category_0.tga",
					"over_image" : IMG_DIR + "category_1.tga",
					"down_image" : IMG_DIR + "category_2.tga",
				},
			),
		},
		{
			"name" : "hide_mode1_window",
			"type" : "window",
			"x" : 0,
			"y" : (SMALL_OPTION_HEIGHT * 7) - 30,
			"width":304,
			"height":SMALL_OPTION_HEIGHT * 3,
			"children":
			(
				{
					"name" : "hide_mode1_title_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.tga",
					"children":
					(
						{
							"name" : "title_hide_mode1",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : uiscriptlocale.HIDE_OPTION,
						},
					),
				},
				{
					"name" : "hidemode1_0",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"text" : uiscriptlocale.HIDE_OPTION0,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "hidemode1_1",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33,
					"text" : uiscriptlocale.HIDE_OPTION1,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "hidemode1_2",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 33,
					"text" : uiscriptlocale.HIDE_OPTION2,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "hidemode1_6",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : (33 * 2),
					"text" : uiscriptlocale.GRAPHICONOFF_EFFECT_LEVEL,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "hidemode1_5",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : (33 * 2),
					"text" : uiscriptlocale.HIDE_OPTION5,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "hidemode1_3",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 33,
					"text" : uiscriptlocale.HIDE_OPTION3,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_y" : 8,
					"default_image" : IMG_DIR + "radio_unselected.tga",
					"over_image" : IMG_DIR + "radio_unselected.tga",
					"down_image" : IMG_DIR + "radio_selected.tga",
				},
				{
					"name" : "hidemode1_4",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : (33 * 3),
					"text" : uiscriptlocale.HIDE_OPTION4,
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
