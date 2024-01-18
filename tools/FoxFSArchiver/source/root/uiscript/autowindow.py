import uiscriptlocale

AUTO_UI_BOARD_WIDTH = 254
AUTO_UI_BOARD_HIGHT = 520

ACTIVE_SKILL_SETTING_BAR_X = 14
ACTIVE_SKILL_SETTING_BAR_Y = 34
ACTIVE_SKILL_SETTING_BAR_WIDTH = 223

CLEAR_BUTTON_X = 207
CLEAR_BUTTON_Y = 1

ACTIVE_SKILL_SETTING_TEXT_X = 9
ACTIVE_SKILL_SETTING_TEXT_Y = 2

ACTIVE_SKILL_SETTING_SLOT_X = 15
ACTIVE_SKILL_SETTING_SLOT_Y = 57

ACTIVE_SKILL_SETTING_SLOT_WIDTH = 221
ACTIVE_SKILL_SETTING_SLOT_HEIGHT = 241

SLOT_INDEX_INTERVAL_X = 38
SLOT_INDEX_INTERVAL_Y = 54
SLOT_INTERVAL_Y = 29

SLOT_INDEX_WIDTH = 32
SLOT_INDEX_HEIGHT = 32

SLOT_COOL_TIME_BASE_X = 15
SLOT_COOL_TIME_INTERVAL_X = 38

SLOT_COOL_TIME_BASE_Y = 89
SLOT_COOL_TIME_INTERVAL_Y = 54
SLOT_COOL_TIME_LINE_INTERVAL_Y = 83

SLOT_EDIT_LINE_X = 3
SLOT_EDIT_LINE_Y = 3
SLOT_EDIT_LINE_INPUT_LIMIT = 4
SLOT_EDIT_LINE_ONLY_NUMBER = 1
SLOT_EDIT_LINE_WIDTH = 90
SLOT_EDIT_LINE_HEIGHT = 20

POTION_SETTING_BAR_X = 14
POTION_SETTING_BAR_Y = 171
POTION_SETTING_BAR_WIDTH = 223

POTION_SETTING_TEXT_X = 9
POTION_SETTING_TEXT_Y = 2

SETTING_BAR_X = 14
SETTING_BAR_Y = 308
SETTING_BAR_WIDTH = 223

SETTING_AUTO_SETTING_TEXT_X = 9
SETTING_AUTO_SETTING_TEXT_Y = 2

SETTING_AUTO_BAR_X = 3

SETTING_AUTO_ATTACK_BAR_Y = 20
SETTING_AUTO_RANGE_BAR_Y = 47
SETTING_AUTO_METINSTONES_BAR_Y = 74
SETTING_AUTO_POTION_BAR_Y = 101
SETTING_AUTO_SKILL_BAR_Y = 128
SETTING_AUTO_RESTART_BAR_Y = 155

AUTO_BUTTON_X = 194
AUTO_BUTTON_BASE_Y = 327
AUTO_BUTTON_ADD_Y = 27

AUTO_SAVE_BUTTON_X = 10
AUTO_SAVE_BUTTON_Y = 459 + 30

AUTO_START_BUTTON_X = 88
AUTO_START_BUTTON_Y = 459 + 30

AUTO_END_BUTTON_X = 166
AUTO_END_BUTTON_Y = 489

AUTO_TOOLTIP_BUTTON_X = 155
AUTO_TOOLTIP_BUTTON_Y = 9

window = {
	"name" : "AutoWindow",
	"x" : 0,
	"y" : 0,
	"style" : ("movable", "float",),
	"width" : AUTO_UI_BOARD_WIDTH,
	"height" : AUTO_UI_BOARD_HIGHT,
	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"x" : 0,
			"y" : 0,
			"width" : AUTO_UI_BOARD_WIDTH,
			"height" : AUTO_UI_BOARD_HIGHT,
			"title" : uiscriptlocale.AUTO_TITLE,
			"children" :
			(
				{
					"name" : "HorizontalBar1",
					"type" : "horizontalbar",
					"x" : ACTIVE_SKILL_SETTING_BAR_X,
					"y" : ACTIVE_SKILL_SETTING_BAR_Y,
					"width" : ACTIVE_SKILL_SETTING_BAR_WIDTH,
					"children" :
					(
						{
							"name" : "BaseInfo",
							"type" : "text",
							"x" : ACTIVE_SKILL_SETTING_TEXT_X,
							"y" : ACTIVE_SKILL_SETTING_TEXT_Y,
							"text" : uiscriptlocale.AUTO_SKILL_SETTING,
						},
						{
							"name" : "AutoSkillClearButton",
							"type" : "button",
							"x" : CLEAR_BUTTON_X,
							"y" : CLEAR_BUTTON_Y,
							"default_image" : "d:/ymir work/ui/game/windows/auto_system_clear_button_01.sub",
							"over_image" : "d:/ymir work/ui/game/windows/auto_system_clear_button_02.sub",
							"down_image" : "d:/ymir work/ui/game/windows/auto_system_clear_button_03.sub",
						},
					),
				},
				{
					"name" : "Auto_Active_Skill_Slot_Table",
					"type" : "slot",
					"x" : ACTIVE_SKILL_SETTING_SLOT_X,
					"y" : ACTIVE_SKILL_SETTING_SLOT_Y,
					"width" : ACTIVE_SKILL_SETTING_SLOT_WIDTH,
					"height" : ACTIVE_SKILL_SETTING_SLOT_HEIGHT,
					"image" : "d:/ymir work/ui/public/Slot_Base.sub",
					"slot": (
						{ "index" : 0, "x" : SLOT_INDEX_INTERVAL_X * 0, "y" : 0, "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 1, "x" : SLOT_INDEX_INTERVAL_X * 1, "y" : 0, "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 2, "x" : SLOT_INDEX_INTERVAL_X * 2, "y" : 0, "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 3, "x" : SLOT_INDEX_INTERVAL_X * 3, "y" : 0, "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 4, "x" : SLOT_INDEX_INTERVAL_X * 4, "y" : 0, "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 5, "x" : SLOT_INDEX_INTERVAL_X * 5, "y" : 0, "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 6, "x" : SLOT_INDEX_INTERVAL_X * 0, "y" : SLOT_INDEX_INTERVAL_Y, "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 7, "x" : SLOT_INDEX_INTERVAL_X * 1, "y" : SLOT_INDEX_INTERVAL_Y, "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 8, "x" : SLOT_INDEX_INTERVAL_X * 2, "y" : SLOT_INDEX_INTERVAL_Y, "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 9, "x" : SLOT_INDEX_INTERVAL_X * 3, "y" : SLOT_INDEX_INTERVAL_Y, "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 10, "x" : SLOT_INDEX_INTERVAL_X * 4, "y" : SLOT_INDEX_INTERVAL_Y, "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 11, "x" : SLOT_INDEX_INTERVAL_X * 5, "y" : SLOT_INDEX_INTERVAL_Y, "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },

						{ "index" : 12, "x" : SLOT_INDEX_INTERVAL_X * 0, "y": SLOT_INTERVAL_Y + (SLOT_INDEX_INTERVAL_Y * 2), "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 13, "x" : SLOT_INDEX_INTERVAL_X * 1, "y": SLOT_INTERVAL_Y + (SLOT_INDEX_INTERVAL_Y * 2), "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 14, "x" : SLOT_INDEX_INTERVAL_X * 2, "y": SLOT_INTERVAL_Y + (SLOT_INDEX_INTERVAL_Y * 2), "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 15, "x" : SLOT_INDEX_INTERVAL_X * 3, "y": SLOT_INTERVAL_Y + (SLOT_INDEX_INTERVAL_Y * 2), "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 16, "x" : SLOT_INDEX_INTERVAL_X * 4, "y": SLOT_INTERVAL_Y + (SLOT_INDEX_INTERVAL_Y * 2), "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 17, "x" : SLOT_INDEX_INTERVAL_X * 5, "y": SLOT_INTERVAL_Y + (SLOT_INDEX_INTERVAL_Y * 2), "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 18, "x" : SLOT_INDEX_INTERVAL_X * 0, "y": SLOT_INTERVAL_Y + (SLOT_INDEX_INTERVAL_Y * 3), "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 19, "x" : SLOT_INDEX_INTERVAL_X * 1, "y": SLOT_INTERVAL_Y + (SLOT_INDEX_INTERVAL_Y * 3), "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 20, "x" : SLOT_INDEX_INTERVAL_X * 2, "y": SLOT_INTERVAL_Y + (SLOT_INDEX_INTERVAL_Y * 3), "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 21, "x" : SLOT_INDEX_INTERVAL_X * 3, "y": SLOT_INTERVAL_Y + (SLOT_INDEX_INTERVAL_Y * 3), "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 22, "x" : SLOT_INDEX_INTERVAL_X * 4, "y": SLOT_INTERVAL_Y + (SLOT_INDEX_INTERVAL_Y * 3), "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
						{ "index" : 23, "x" : SLOT_INDEX_INTERVAL_X * 5, "y": SLOT_INTERVAL_Y + (SLOT_INDEX_INTERVAL_Y * 3), "width" : SLOT_INDEX_WIDTH, "height" : SLOT_INDEX_HEIGHT },
					),
				},
				{
					"name" : "cool_time_Image0",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X,
					"y" : SLOT_COOL_TIME_BASE_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children" :
					(
						{
							"name" : "editline0",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image1",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + SLOT_COOL_TIME_INTERVAL_X,
					"y" : SLOT_COOL_TIME_BASE_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children" :
					(
						{
							"name" : "editline1",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image2",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X * 2),
					"y" : SLOT_COOL_TIME_BASE_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children" :
					(
						{
							"name" : "editline2",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image3",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X * 3),
					"y" : SLOT_COOL_TIME_BASE_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline3",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image4",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X * 4),
					"y" : SLOT_COOL_TIME_BASE_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline4",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image5",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X * 5),
					"y" : SLOT_COOL_TIME_BASE_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline5",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image6",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X,
					"y" : SLOT_COOL_TIME_BASE_Y + SLOT_COOL_TIME_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline6",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image7",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + SLOT_COOL_TIME_INTERVAL_X,
					"y" : SLOT_COOL_TIME_BASE_Y + SLOT_COOL_TIME_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline7",
							"type" : "editline",
							"x": SLOT_EDIT_LINE_X,
							"y": SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image8",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X * 2),
					"y" : SLOT_COOL_TIME_BASE_Y + SLOT_COOL_TIME_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline8",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image9",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X * 3),
					"y" : SLOT_COOL_TIME_BASE_Y + SLOT_COOL_TIME_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline9",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image10",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X * 4),
					"y" : SLOT_COOL_TIME_BASE_Y + SLOT_COOL_TIME_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline10",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image11",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X * 5),
					"y" : SLOT_COOL_TIME_BASE_Y + SLOT_COOL_TIME_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline11",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image12",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X,
					"y" : SLOT_COOL_TIME_BASE_Y + SLOT_COOL_TIME_INTERVAL_Y + SLOT_COOL_TIME_LINE_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline12",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image13",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X+SLOT_COOL_TIME_INTERVAL_X,
					"y" : SLOT_COOL_TIME_BASE_Y + SLOT_COOL_TIME_INTERVAL_Y + SLOT_COOL_TIME_LINE_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline13",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image14",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X * 2),
					"y" : SLOT_COOL_TIME_BASE_Y + SLOT_COOL_TIME_INTERVAL_Y + SLOT_COOL_TIME_LINE_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline14",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image15",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X * 3),
					"y" : SLOT_COOL_TIME_BASE_Y + SLOT_COOL_TIME_INTERVAL_Y + SLOT_COOL_TIME_LINE_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline15",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image16",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X * 4),
					"y" : SLOT_COOL_TIME_BASE_Y + SLOT_COOL_TIME_INTERVAL_Y + SLOT_COOL_TIME_LINE_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline16",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image17",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X * 5),
					"y" : SLOT_COOL_TIME_BASE_Y + SLOT_COOL_TIME_INTERVAL_Y + SLOT_COOL_TIME_LINE_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline17",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image18",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X,
					"y" : SLOT_COOL_TIME_BASE_Y + (SLOT_COOL_TIME_INTERVAL_Y * 2) + SLOT_COOL_TIME_LINE_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline18",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image19",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + SLOT_COOL_TIME_INTERVAL_X,
					"y" : SLOT_COOL_TIME_BASE_Y + (SLOT_COOL_TIME_INTERVAL_Y * 2) + SLOT_COOL_TIME_LINE_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline19",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image20",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X * 2),
					"y" : SLOT_COOL_TIME_BASE_Y + (SLOT_COOL_TIME_INTERVAL_Y * 2) + SLOT_COOL_TIME_LINE_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline20",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image21",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X * 3),
					"y" : SLOT_COOL_TIME_BASE_Y + (SLOT_COOL_TIME_INTERVAL_Y * 2) + SLOT_COOL_TIME_LINE_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline21",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image22",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X*4),
					"y" : SLOT_COOL_TIME_BASE_Y+(SLOT_COOL_TIME_INTERVAL_Y*2) + SLOT_COOL_TIME_LINE_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline22",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "cool_time_Image23",
					"type" : "image",
					"x" : SLOT_COOL_TIME_BASE_X + (SLOT_COOL_TIME_INTERVAL_X * 5),
					"y" : SLOT_COOL_TIME_BASE_Y + (SLOT_COOL_TIME_INTERVAL_Y * 2) + SLOT_COOL_TIME_LINE_INTERVAL_Y,
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{
							"name" : "editline23",
							"type" : "editline",
							"x" : SLOT_EDIT_LINE_X,
							"y" : SLOT_EDIT_LINE_Y,
							"input_limit" : SLOT_EDIT_LINE_INPUT_LIMIT,
							"only_number" : SLOT_EDIT_LINE_ONLY_NUMBER,
							"width" : SLOT_EDIT_LINE_WIDTH,
							"height" : SLOT_EDIT_LINE_HEIGHT,
						},
					),
				},
				{
					"name" : "HorizontalBar2",
					"type" : "horizontalbar",
					"x" : POTION_SETTING_BAR_X,
					"y" : POTION_SETTING_BAR_Y,
					"width" : POTION_SETTING_BAR_WIDTH,
					"children" :
					(
						{
							"name" : "BaseInfo",
							"type" : "text",
							"x" : POTION_SETTING_TEXT_X,
							"y" : POTION_SETTING_TEXT_Y,
							"text" : uiscriptlocale.AUTO_POTION_SETTING,
						},
						{
							"name" : "AutoPotionClearButton",
							"type" : "button",
							"x" : CLEAR_BUTTON_X,
							"y" : CLEAR_BUTTON_Y,
							"default_image" : "d:/ymir work/ui/game/windows/auto_system_clear_button_01.sub",
							"over_image" : "d:/ymir work/ui/game/windows/auto_system_clear_button_02.sub",
							"down_image" : "d:/ymir work/ui/game/windows/auto_system_clear_button_03.sub",
						},
					),
				},
				{
					"name" : "HorizontalBar1",
					"type" : "horizontalbar",
					"x" : SETTING_BAR_X,
					"y" : SETTING_BAR_Y,
					"width" : SETTING_BAR_WIDTH,
					"children":
					(
						{
							"name" : "BaseInfo",
							"type" : "text",
							"x" : SETTING_AUTO_SETTING_TEXT_X,
							"y" : SETTING_AUTO_SETTING_TEXT_Y,
							"text" : uiscriptlocale.AUTO_SETTING,
						},
						{
							"name" : "attack_text_bar",
							"type" : "image",
							"x" : SETTING_AUTO_BAR_X,
							"y" : SETTING_AUTO_ATTACK_BAR_Y,
							"image" : "d:/ymir work/ui/game/windows/auto_system_text_bar_01.sub",
							"children":
							(
								{
									"name" : "BaseInfo",
									"type" : "text",
									"x":2,
									"y":0,
									"all_align" : "center",
									"text" : uiscriptlocale.AUTO_AUTO_ATTACK,
								},
							),
						},
						{
							"name" : "range_text_bar",
							"type" : "image",
							"x" : SETTING_AUTO_BAR_X,
							"y" : SETTING_AUTO_RANGE_BAR_Y,
							"image" : "d:/ymir work/ui/game/windows/auto_system_text_bar_01.sub",
							"children":
							(
								{
									"name" : "BaseInfo",
									"type" : "text",
									"x":2,
									"y":0,
									"all_align" : "center",
									"text" : uiscriptlocale.AUTO_AUTO_RANGE,
								},
							),
						},
						{
							"name" : "metinstone_text_bar",
							"type" : "image",
							"x" : SETTING_AUTO_BAR_X,
							"y" : SETTING_AUTO_METINSTONES_BAR_Y,
							"image" : "d:/ymir work/ui/game/windows/auto_system_text_bar_01.sub",
							"children":
							(
								{
									"name" : "BaseInfo",
									"type" : "text",
									"x":2,
									"y":0,
									"all_align" : "center",
									"text" : uiscriptlocale.AUTO_AUTO_METIN_STONES,
								},
							),
						},
						{
							"name" : "potion_text_bar",
							"type" : "image",
							"x" : SETTING_AUTO_BAR_X,
							"y" : SETTING_AUTO_POTION_BAR_Y,
							"image" : "d:/ymir work/ui/game/windows/auto_system_text_bar_02.sub",
							"children":
							(
								{
									"name" : "BaseInfo",
									"type" : "text",
									"x":2,
									"y":0,
									"all_align" : "center",
									"text" : uiscriptlocale.AUTO_AUTO_POTION,
								},
							),
						},
						{
							"name" : "skill_text_bar",
							"type" : "image",
							"x" : SETTING_AUTO_BAR_X,
							"y" : SETTING_AUTO_SKILL_BAR_Y,
							"image" : "d:/ymir work/ui/game/windows/auto_system_text_bar_02.sub",
							"children":
							(
								{
									"name" : "BaseInfo",
									"type" : "text",
									"x" : 2,
									"y" : 0,
									"all_align" : "center",
									"text" : uiscriptlocale.AUTO_AUTO_SKILL,
								},
							),
						},
						{
							"name" : "restart_text_bar",
							"type" : "image",
							"x" : SETTING_AUTO_BAR_X,
							"y" : SETTING_AUTO_RESTART_BAR_Y,
							"image" : "d:/ymir work/ui/game/windows/auto_system_text_bar_02.sub",
							"children":
							(
								{
									"name" : "BaseInfo",
									"type" : "text",
									"x" : 2,
									"y" : 0,
									"all_align" : "center",
									"text" : uiscriptlocale.AUTO_AUTO_RESTART,
								},
							),
						},
					),
				},
				{
					"name" : "AutoAttackButton",
					"type" : "toggle_button",
					"x" : AUTO_BUTTON_X,
					"y" : AUTO_BUTTON_BASE_Y,
					"text" : uiscriptlocale.AUTO_OFF,
					"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
				},
				{
					"name" : "AutoRangeButton",
					"type" : "toggle_button",
					"x" : AUTO_BUTTON_X,
					"y" : AUTO_BUTTON_BASE_Y + AUTO_BUTTON_ADD_Y,
					"text" : uiscriptlocale.AUTO_OFF,
					"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
				},
				{
					"name" : "AutoMetinStonesButton",
					"type" : "toggle_button",
					"x" : AUTO_BUTTON_X,
					"y" : AUTO_BUTTON_BASE_Y + (AUTO_BUTTON_ADD_Y * 2),
					"text" : uiscriptlocale.AUTO_OFF,
					"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
				},
				{
					"name" : "AutoPotionButton",
					"type" : "toggle_button",
					"x" : AUTO_BUTTON_X,
					"y" : AUTO_BUTTON_BASE_Y + (AUTO_BUTTON_ADD_Y * 3),
					"text" : uiscriptlocale.AUTO_OFF,
					"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
				},
				{
					"name" : "AutoSkillButton",
					"type" : "toggle_button",
					"x" : AUTO_BUTTON_X,
					"y" : AUTO_BUTTON_BASE_Y + (AUTO_BUTTON_ADD_Y * 4),
					"text" : uiscriptlocale.AUTO_OFF,
					"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
				},
				{
					"name" : "AutoRestartHereButton",
					"type" : "toggle_button",
					"x" : AUTO_BUTTON_X,
					"y" : AUTO_BUTTON_BASE_Y + (AUTO_BUTTON_ADD_Y * 5),
					"text" : uiscriptlocale.AUTO_OFF,
					"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
				},
				{
					"name" : "AutoSaveOnButton",
					"type" : "button",
					"x" : AUTO_SAVE_BUTTON_X,
					"y" : AUTO_SAVE_BUTTON_Y,
					"text" : uiscriptlocale.AUTO_SAVE,
					"default_image" : "d:/ymir work/ui/game/windows/auto_system_bottom_button_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/auto_system_bottom_button_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/auto_system_bottom_button_03.sub",
				},
				{
					"name" : "AutoStartOnButton",
					"type" : "button",
					"x" : AUTO_START_BUTTON_X,
					"y" : AUTO_START_BUTTON_Y,
					"text" : uiscriptlocale.AUTO_START,
					"default_image" : "d:/ymir work/ui/game/windows/auto_system_bottom_button_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/auto_system_bottom_button_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/auto_system_bottom_button_03.sub",
				},
				{
					"name" : "AutoStartOffButton",
					"type" : "button",
					"x" : AUTO_END_BUTTON_X,
					"y" : AUTO_END_BUTTON_Y,
					"text" : uiscriptlocale.AUTO_END,
					"default_image" : "d:/ymir work/ui/game/windows/auto_system_bottom_button_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/auto_system_bottom_button_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/auto_system_bottom_button_03.sub",
				},
				{
					"name" : "AutoToolTipButton",
					"type" : "button",
					"x" : AUTO_TOOLTIP_BUTTON_X,
					"y" : AUTO_TOOLTIP_BUTTON_Y,
					"default_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
					"over_image" : "d:/ymir work/ui/pattern/q_mark_02.tga",
					"down_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
				},
				{
					"name" : "AutoAllClearButton",
					"type" : "button",
					"x" : SETTING_BAR_X + CLEAR_BUTTON_X,
					"y" : SETTING_BAR_Y + CLEAR_BUTTON_Y,
					"default_image" : "d:/ymir work/ui/game/windows/auto_system_clear_button_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/auto_system_clear_button_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/auto_system_clear_button_03.sub",
				},
			),
		},
	),
}
