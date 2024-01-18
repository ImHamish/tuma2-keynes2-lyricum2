import uiscriptlocale
import localeinfo


ROOT_PATH = "d:/ymir work/ui/public/"
ROOT_PATH_NEW = "grafica/select_interface/"
LOCALE_PATH = uiscriptlocale.SELECT_PATH


BOARD_X = SCREEN_WIDTH * (25) / 800
BOARD_Y = SCREEN_HEIGHT * (156) / 600


PLUS_BUTTON_WIDTH = 20
TEMPORARY_HEIGHT = 30
BOARD_ITEM_ADD_POSITION = -40

## Default Value ##
X_GAP = 11
Y_GAP = 12

NAME_X = 18
NAME_Y = 95
NAME_SCALE_X = 0.65
NAME_SCALE_Y = 0.65

FLAG_SCALE_X = 0.45
FLAG_SCALE_Y = 0.45

SHADOW_SCALE_X = 3.0 * SCREEN_WIDTH  / 800.0
SHADOW_SCALE_Y = 2.0 * SCREEN_HEIGHT / 600.0

STAT_GAUGE_X = X_GAP + 3
STAT_GAUGE_Y = 286
STAT_GAUGE_BAR_X = 40
STAT_GAUGE_BAR_WIDTH = 105
STAT_GAUGE_GAP = 18
STAT_GAUGE_TEXT_WIDTH = 21
STAT_GAUGE_TEXT_HEIGHT = 13

#THINBOARD_GOLD_WIDTH = 197
THINBOARD_GOLD_HEIGHT = 364

THINBOARD_CIRCLE_LEFT_WIDTH = 175

THINBOARD_CIRCLE_RIGHT_WIDTH = 180
THINBOARD_CIRCLE_RIGHT_HEIGHT = 270


DESC_FACE_X = 4
DESC_FACE_Y = -23

FACE_X = 7
FACE_Y = 4
SELECT_BTN_X = X_GAP + 4#12
SELECT_BTN_Y = 10
SELECT_BTN_GAP = 47#49

###################


window = {
	"name" : "New_SelectCharacterWindow",
	"x" : 0, "y" : 0, "width" : SCREEN_WIDTH, "height" : SCREEN_HEIGHT,
	
	"children"	:
	(
		## BackGround##
		{
			"name" : "BackGround", "type" : "expanded_image", "x" : 0, "y" : 0, "x_scale" : float(SCREEN_WIDTH) / 1920.0, "y_scale" : float(SCREEN_HEIGHT) / 1080.0,
			"image" : ROOT_PATH_NEW + "background.png",
		},

		## Name##

		{

			"name" : "name_warrior", "type" : "expanded_image", 
			"x" : BOARD_X + NAME_X, "y" : BOARD_Y - NAME_Y, "x_scale" : NAME_SCALE_X, "y_scale" : NAME_SCALE_Y,
			"image" : LOCALE_PATH + "name_warrior.sub",

		},


		{

			"name" : "name_assassin", "type" : "expanded_image",
			"x" : BOARD_X + NAME_X, "y" : BOARD_Y - NAME_Y, "x_scale" : NAME_SCALE_X, "y_scale" : NAME_SCALE_Y,
			"image" : LOCALE_PATH + "name_assassin.sub",

		},

   
		{
			"name" : "name_sura", "type" : "expanded_image",
			"x" : BOARD_X + NAME_X, "y" : BOARD_Y - NAME_Y, "x_scale" : NAME_SCALE_X, "y_scale" : NAME_SCALE_Y,


			"image" : LOCALE_PATH + "name_sura.sub",
		},
		{
			"name" : "name_shaman", "type" : "expanded_image",
			"x" : BOARD_X + NAME_X, "y" : BOARD_Y - NAME_Y, "x_scale" : NAME_SCALE_X, "y_scale" : NAME_SCALE_Y,


			"image" : LOCALE_PATH + "name_shaman.sub",
		},
		{
			"name" : "name_wolfman", "type" : "expanded_image",
			"x" : BOARD_X + NAME_X, "y" : BOARD_Y - NAME_Y, "x_scale" : NAME_SCALE_X, "y_scale" : NAME_SCALE_Y,



			"image" : LOCALE_PATH + "name_wolfman.sub",
		},
		
		## Shadow ##
		{
			"name" : "Shadow", "type" : "expanded_image", "x" : SCREEN_WIDTH/2 - (64)*SHADOW_SCALE_X, "y" : SCREEN_HEIGHT - SCREEN_HEIGHT*(70)/600.0 - (88)*SHADOW_SCALE_Y/2.0, 
			"x_scale" : SHADOW_SCALE_X, "y_scale" : SHADOW_SCALE_Y,



			"image" : LOCALE_PATH + "shadow/shadow.tga",#test.bmp",
		},
		


		##SelectCharList Board##

		{
		
			"name" : "character_board",
			"type" : "image",


			"x" : BOARD_X,
			"y" : BOARD_Y,
			

			"image" : ROOT_PATH_NEW + "select_character_window.png",
			

			"children" :
			(
				{ "name" : "NoneButton_0", "type" : "image", "x" : SELECT_BTN_X+5, "y" : SELECT_BTN_Y + SELECT_BTN_GAP+10  , "image" : ROOT_PATH_NEW + "character_background.png"},
				{ "name" : "NoneButton_1", "type" : "image", "x" : SELECT_BTN_X+5, "y" : SELECT_BTN_Y + SELECT_BTN_GAP*2+10  , "image" : ROOT_PATH_NEW + "character_background.png"},
				{ "name" : "NoneButton_2", "type" : "image", "x" : SELECT_BTN_X+5, "y" : SELECT_BTN_Y + SELECT_BTN_GAP*3+10, "image" : ROOT_PATH_NEW + "character_background.png"},
				{ "name" : "NoneButton_3", "type" : "image", "x" : SELECT_BTN_X+5, "y" : SELECT_BTN_Y + SELECT_BTN_GAP*4+10, "image" : ROOT_PATH_NEW + "character_background.png"},
				{ "name" : "NoneButton_4", "type" : "image", "x" : SELECT_BTN_X+5, "y" : SELECT_BTN_Y + SELECT_BTN_GAP*5+10, "image" : ROOT_PATH_NEW + "character_background.png"},
				
				{
					"name" : "CharacterSlot_0",	"type" : "radio_button", 
					"x" : SELECT_BTN_X+5, "y" : SELECT_BTN_Y + SELECT_BTN_GAP+10,
					
					"default_image" : ROOT_PATH_NEW + "character_background.png",
					"over_image"	: ROOT_PATH_NEW + "character_background_hover.png",
					"down_image"	: ROOT_PATH_NEW + "character_background_selected.png",
				},
				{
					"name" : "CharacterSlot_1", "type" : "radio_button",
					"x" : SELECT_BTN_X+5, "y" : SELECT_BTN_Y + SELECT_BTN_GAP*2+10,

					"default_image" : ROOT_PATH_NEW + "character_background.png",
					"over_image"	: ROOT_PATH_NEW + "character_background_hover.png",
					"down_image"	: ROOT_PATH_NEW + "character_background_selected.png",

				},
				{
					"name" : "CharacterSlot_2", "type" : "radio_button",
					"x" : SELECT_BTN_X+5, "y" : SELECT_BTN_Y + SELECT_BTN_GAP*3+10,

					"default_image" : ROOT_PATH_NEW + "character_background.png",
					"over_image"	: ROOT_PATH_NEW + "character_background_hover.png",
					"down_image"	: ROOT_PATH_NEW + "character_background_selected.png",

				},
				{
					"name" : "CharacterSlot_3", "type" : "radio_button",
					"x" : SELECT_BTN_X+5, "y" : SELECT_BTN_Y + SELECT_BTN_GAP*4+10,

					"default_image" : ROOT_PATH_NEW + "character_background.png",
					"over_image"	: ROOT_PATH_NEW + "character_background_hover.png",
					"down_image"	: ROOT_PATH_NEW + "character_background_selected.png",

				},
				
				
				{
					"name" : "CharacterSlot_4", "type" : "radio_button",
					"x" : SELECT_BTN_X+5, "y" : SELECT_BTN_Y + SELECT_BTN_GAP*5+10,

					"default_image" : ROOT_PATH_NEW + "character_background.png",
					"over_image"	: ROOT_PATH_NEW + "character_background_hover.png",
					"down_image"	: ROOT_PATH_NEW + "character_background_selected.png",
				 },
				 
				## Face Image ##
				{ "name" : "CharacterFace_0", "type" : "image", "x" : SELECT_BTN_X - FACE_X+6, "y" : SELECT_BTN_Y + SELECT_BTN_GAP+15 				   - FACE_Y, "image" : "D:/ymir work/ui/intro/public_intro/face/face_warrior_m_01.sub"},
				{ "name" : "CharacterFace_1", "type" : "image", "x" : SELECT_BTN_X - FACE_X+6, "y" : SELECT_BTN_Y + SELECT_BTN_GAP*2+15   - FACE_Y, "image" : "D:/ymir work/ui/intro/public_intro/face/face_warrior_m_01.sub"},
				{ "name" : "CharacterFace_2", "type" : "image", "x" : SELECT_BTN_X - FACE_X+6, "y" : SELECT_BTN_Y + SELECT_BTN_GAP*3+15 - FACE_Y, "image" : "D:/ymir work/ui/intro/public_intro/face/face_warrior_m_01.sub"},
				{ "name" : "CharacterFace_3", "type" : "image", "x" : SELECT_BTN_X - FACE_X+6, "y" : SELECT_BTN_Y + SELECT_BTN_GAP*4+15 - FACE_Y, "image" : "D:/ymir work/ui/intro/public_intro/face/face_warrior_m_01.sub"},
				{ "name" : "CharacterFace_4", "type" : "image", "x" : SELECT_BTN_X - FACE_X+6, "y" : SELECT_BTN_Y + SELECT_BTN_GAP*5+15 - FACE_Y, "image" : "D:/ymir work/ui/intro/public_intro/face/face_warrior_m_01.sub"},
				
				{
					"name" : "create_button", "type" : "button",
					"x" : SELECT_BTN_X, "y" : SELECT_BTN_Y + SELECT_BTN_GAP*5+10,

					"default_image" : ROOT_PATH_NEW + "create_new_character.png",
					"over_image" 	: ROOT_PATH_NEW + "create_new_character_hover.png",
					"down_image" 	: ROOT_PATH_NEW + "create_new_character_down.png",

				},
				
				{
					"name" : "delete_button", "type" : "button",
					"x" : SELECT_BTN_X-2, "y" : SELECT_BTN_Y + SELECT_BTN_GAP*5+10+66,

					"default_image" : ROOT_PATH_NEW + "delete_character.png",
					"over_image" 	: ROOT_PATH_NEW + "delete_character_down.png",
					"down_image" 	: ROOT_PATH_NEW + "delete_character_hover.png",

				},
				
				{
					"name" : "EmpireFlag", "type" : "expanded_image",
					"x" : SELECT_BTN_X-5, "y" : SELECT_BTN_Y + SELECT_BTN_GAP-48, "x_scale" : FLAG_SCALE_X, "y_scale" : FLAG_SCALE_Y,
					"image" : LOCALE_PATH + "empire/empireflag_a.sub"

				},
				{
					"name" : "EmpireName", "type" : "text",
					"x" :  SELECT_BTN_X+90, "y" : SELECT_BTN_Y + SELECT_BTN_GAP-38,
					"text" : uiscriptlocale.SELECT_EMPIRE_NAME,
					"fontsize" : "LARGE",
				},
				
			),
		},

		## Buttons - Start, Exit ##
		{
			"name" : "start_button", "type" : "button",
			"x" : SCREEN_WIDTH/2 - 117, "y" : SCREEN_HEIGHT - SCREEN_HEIGHT*(60)/600.0,

			"default_image" : ROOT_PATH_NEW + "play.png",
			"over_image"	: ROOT_PATH_NEW + "play_over.png",
			"down_image"	: ROOT_PATH_NEW + "play_down.png",
		},
		{
			"name" : "prev_button","type" : "button",
			"x" : SCREEN_WIDTH/2 - 290, "y" : SCREEN_HEIGHT - SCREEN_HEIGHT*(55)/600.0,

			"default_image" : ROOT_PATH_NEW + "return.png",
			"over_image"	: ROOT_PATH_NEW + "return_over.png",
			"down_image"	: ROOT_PATH_NEW + "return_down.png",
		},
		{
			"name" : "exit_button","type" : "button",
			"x" : SCREEN_WIDTH/2 + 140, "y" : SCREEN_HEIGHT - SCREEN_HEIGHT*(55)/600.0,

			"default_image" : ROOT_PATH_NEW + "exit.png",
			"over_image"	: ROOT_PATH_NEW + "exit_over.png",
			"down_image"	: ROOT_PATH_NEW + "exit_down.png",
		},
		{
			"name" : "character_discriptionboard",
			"type" : "image",

			"x" : SCREEN_WIDTH - BOARD_X - (180 + (X_GAP * 2)),
			"y" : SCREEN_HEIGHT * (256) / 600,
			

			"image" : ROOT_PATH_NEW + "character_stats.png",
			
			"children"	:
			(

				{
					"name" : "raceName_Btn",
					"type" : "image",

					"x" : 10,
					"y" : -17,
					# "image" : ROOT_PATH + "public_intro_btn/raceName_btn.sub",


					"children" :
					(
					
					
						# {
							# "name" : "DiscFace",
							# "type" : "image",

							# "x" : DESC_FACE_X,
							# "y" : DESC_FACE_Y,
							# "image" : "icon/face/warrior_m.tga"
						# },
						
						# { 
							# "name":"info_text_bg",
							# "type":"image",
							# "x":-13,
							# "y":30, 
							# "image" : "d:/ymir work/ui/minigame/catchking/challenge_text_bg.png",
							# "style" : ("not_pick",),
						# },
						
						# {
							# "name":"title_name_bg",
							# "type":"text",
							
							# "x":0,
							# "y":28,
							
							# "text": "Status", 
							# "all_align":"center"
						# },
				
						# {
							# "name" : "raceName_Text",
							# "type" : "text",
							# "x" : 100,
							# "y" : 5,
							# "r" : 0.7843,
							# "g" : 0.7843,
							# "b" : 0.7843,

							# "text" : localeInfo.JOB_WARRIOR, "text_horizontal_align" : "center"
						# }
					),
				},
				
				## stat Images
				{	"name":"hth_img",			"type" : "image",	"x" : STAT_GAUGE_X+4, "y" : 38, "image":"d:/ymir work/ui/game/windows/char_info_con.sub",			},
				# {	"name":"sungma_str_img",	"type" : "image",	"x" : STAT_GAUGE_X,	"y" : STAT_GAUGE_Y + STAT_GAUGE_GAP * 0, "image":"d:/ymir work/ui/game/windows/char_info_sungma_str.sub",	},
				{	"name":"int_img",			"type" : "image",	"x" : STAT_GAUGE_X+4,	"y" : 38 + 20, "image":"d:/ymir work/ui/game/windows/char_info_int.sub",			},
				# {	"name":"sungma_hp_img",		"type" : "image",	"x" : STAT_GAUGE_X,	"y" : STAT_GAUGE_Y + STAT_GAUGE_GAP * 1, "image":"d:/ymir work/ui/game/windows/char_info_sungma_hp.sub",	},
				{	"name":"str_img",			"type" : "image",	"x" : STAT_GAUGE_X+4, "y" : 38 + 20 + 20, "image":"d:/ymir work/ui/game/windows/char_info_str.sub",			},
				# {	"name":"sungma_move_img",	"type" : "image",	"x" : STAT_GAUGE_X, "y" : STAT_GAUGE_Y + STAT_GAUGE_GAP * 2, "image":"d:/ymir work/ui/game/windows/char_info_sungma_move.sub",	},
				{	"name":"dex_img",			"type" : "image",	"x" : STAT_GAUGE_X+4, "y" : 38 + 20 + 20 +20, "image":"d:/ymir work/ui/game/windows/char_info_dex.sub",			},
				# {	"name":"sungma_immune_img",	"type" : "image",	"x" : STAT_GAUGE_X, "y" : STAT_GAUGE_Y + STAT_GAUGE_GAP * 3, "image":"d:/ymir work/ui/game/windows/char_info_sungma_immune.sub",},

				
				{
					"name" : "hth",
					"type" : "text",
					"x" : STAT_GAUGE_X,
	 
					"y" : 43+10,

					# "text" : uiscriptlocale.CREATE_HP,

					"children" :
					(
						{
							"name" : "hth_gauge",
							"type" : "gauge",
							"x" : STAT_GAUGE_BAR_X,
	
							"y" : 4,
							"width" : STAT_GAUGE_BAR_WIDTH,
	  
							"color" : "red",
						},
						{
							"name" : "hth_slot",
							
							"x" : STAT_GAUGE_BAR_WIDTH + STAT_GAUGE_BAR_X + 7,
							"y" : -1,
							
							"width" : STAT_GAUGE_TEXT_WIDTH,
							"height" : STAT_GAUGE_TEXT_HEIGHT,
							
							
							"children" :
							(
								{
									"name" : "hth_value",
									"type" : "text",
									"x" : 0,
									"y" : 1,
									"all_align" : "center",
									"text" : "0",
								},
							),
						},

					),
				},
				{
					"name" : "int",
					"type" : "text",
					"x" : STAT_GAUGE_X,

					"y" : 43+18+10,

					# "text" : uiscriptlocale.CREATE_SP,

					"children" :
					(
						{
							"name" : "int_gauge",
							"type" : "gauge",
							"x" : STAT_GAUGE_BAR_X,
	
							"y" : 4,
							"width" : STAT_GAUGE_BAR_WIDTH,
	  
							"color" : "pink",
						},
						{
							"name" : "int_slot",
							

							"x" : STAT_GAUGE_BAR_WIDTH + STAT_GAUGE_BAR_X + 7,
							"y" : -1,
							
							"width" : STAT_GAUGE_TEXT_WIDTH,
							"height" : STAT_GAUGE_TEXT_HEIGHT,
							
							"children" :
							(
								{
									"name" : "int_value",
									"type" : "text",

									"x" : 0,
									"y" : 1,
									"all_align" : "center",
									"text" : "0",
								},
							),
						},
					),
				},
				{
					"name" : "str",
					"type" : "text",
					"x" : STAT_GAUGE_X,
	 
					"y" : 43+18+18+10,

					# "text" : uiscriptlocale.CREATE_ATT_GRADE,

					"children" :
					(
						{
							"name" : "str_gauge",
							"type" : "gauge",
							"x" : STAT_GAUGE_BAR_X,
	
							"y" : 4,
							"width" : STAT_GAUGE_BAR_WIDTH,
	  
							"color" : "purple",
						},
						{
							"name" : "str_slot",
							"x" : STAT_GAUGE_BAR_WIDTH + STAT_GAUGE_BAR_X + 7,

							"y" : -1,
							
							"width" : STAT_GAUGE_TEXT_WIDTH,
							"height" : STAT_GAUGE_TEXT_HEIGHT,
							"children" :
							(
								{
									"name" : "str_value",
									"type" : "text",

									"x" : 0,
									"y" : 1,
									"all_align" : "center",


									"text" : "0",
								},
							),
						},
					),
				},
				{
					"name" : "dex",
					"type" : "text",
					"x" : STAT_GAUGE_X,

					"y" : 43+18+18+18+10,

					# "text" : uiscriptlocale.CREATE_DEX_GRADE,

					"children" :
					(
						{
							"name" : "dex_gauge",
							"type" : "gauge",
							"x" : STAT_GAUGE_BAR_X,

							"y" : 4,
							"width" : STAT_GAUGE_BAR_WIDTH,

							"color" : "blue",
						},
						{
							"name" : "dex_slot",
							"x" : STAT_GAUGE_BAR_WIDTH + STAT_GAUGE_BAR_X + 7,

							"y" : -1,
							
							"width" : STAT_GAUGE_TEXT_WIDTH,
							"height" : STAT_GAUGE_TEXT_HEIGHT,
							"children" :
							(
								{
									"name" : "dex_value",
									"type" : "text",
									"x" : 0,
									"y" : 1,
									"all_align" : "center",
									"text" : "0",
								},
							),
						},
					),
				},
			), 
		},
	),	
}
