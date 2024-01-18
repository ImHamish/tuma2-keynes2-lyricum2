import uiscriptlocale
import localeinfo

LOCALE_PATH = uiscriptlocale.WINDOWS_PATH

MAINBOARD_WIDTH = 200
MAINBOARD_HEIGHT = 405

LABEL_START_X = 0-10
LABEL_START_Y = 39
LABEL_WIDTH = 50
LABEL_HEIGHT = 17
LABEL_GAP = LABEL_HEIGHT+7
LABEL_NAME_POS_X = 0-10
TITLE_BAR_POS_X = 10
TITLE_BAR_WIDTH = 165

window = {
	"name" : "CharacterDetailsWindow",
	"style" : ("float", "movable", ),
	
	"x" : 274, #24+253-3,
	"y" : (SCREEN_HEIGHT - 398) / 2,

	"width" : MAINBOARD_WIDTH,
	"height" : MAINBOARD_HEIGHT,
	
	"children" :
	(
		## MainBoard
		{
			"name" : "MainBoard",
			"type" : "board",
			"style" : ("attach","ltr"),
			
			## CharacterWindow.py 영향 받음
			"x" : 0,
			"y" : 0,

			"width" : MAINBOARD_WIDTH,
			"height" : MAINBOARD_HEIGHT,
			
			"children" :
			(
				## 타이틀바
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 6,
					"y" : 7,

					"width" : MAINBOARD_WIDTH - 13,
					
					"children" :
					(
						{ "name" : "TitleName", "type" : "text", "x" : 0, "y" : 0, "text": localeinfo.DETAILS_TITLE, "all_align":"center" },
					),
				},
							
				## 스크롤 바
				{
					"name" : "ScrollBar",
					"type" : "scrollbar",

					"x" : 24,
					"y" : 31,
					"size" : MAINBOARD_HEIGHT - 40,
					"horizontal_align" : "right",
				},
				
				## 호리즌 바
				{ 
					"name" : "horizontalbar0", "type":"horizontalbar", "x":TITLE_BAR_POS_X, "y":LABEL_START_Y+LABEL_GAP*0, "width":TITLE_BAR_WIDTH, 
					"children" : ( { "name" : "horizontalbarName0", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", "fontsize" : "LARGE", "color" : 0xFFFF0303 }, ),
				},
				{ 
					"name" : "horizontalbar1", "type":"horizontalbar", "x":TITLE_BAR_POS_X, "y":LABEL_START_Y+LABEL_GAP*1, "width":TITLE_BAR_WIDTH,
					"children" : ( { "name" : "horizontalbarName1", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", "fontsize" : "LARGE", "color" : 0xFFFF0303 }, ),
				},
				{ 
					"name" : "horizontalbar2", "type":"horizontalbar", "x":TITLE_BAR_POS_X, "y":LABEL_START_Y+LABEL_GAP*2, "width":TITLE_BAR_WIDTH,
					"children" : ( { "name" : "horizontalbarName2", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", "fontsize" : "LARGE", "color" : 0xFFFF0303 }, ),
				},
				{ 
					"name" : "horizontalbar3", "type":"horizontalbar", "x":TITLE_BAR_POS_X, "y":LABEL_START_Y+LABEL_GAP*3, "width":TITLE_BAR_WIDTH,
					"children" : ( { "name" : "horizontalbarName3", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", "fontsize" : "LARGE", "color" : 0xFFFF0303 }, ),
				},
				{ 
					"name" : "horizontalbar4", "type":"horizontalbar", "x":TITLE_BAR_POS_X, "y":LABEL_START_Y+LABEL_GAP*4, "width":TITLE_BAR_WIDTH,
					"children" : ( { "name" : "horizontalbarName4", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", "fontsize" : "LARGE", "color" : 0xFFFF0303 }, ),
				},
				{ 
					"name" : "horizontalbar5", "type":"horizontalbar", "x":TITLE_BAR_POS_X, "y":LABEL_START_Y+LABEL_GAP*5, "width":TITLE_BAR_WIDTH,
					"children" : ( { "name" : "horizontalbarName5", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", "fontsize" : "LARGE", "color" : 0xFFFF0303 }, ),
				},
				{ 
					"name" : "horizontalbar6", "type":"horizontalbar", "x":TITLE_BAR_POS_X, "y":LABEL_START_Y+LABEL_GAP*6, "width":TITLE_BAR_WIDTH,
					"children" : ( { "name" : "horizontalbarName6", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", "fontsize" : "LARGE", "color" : 0xFFFF0303 }, ),
				},
				{ 
					"name" : "horizontalbar7", "type":"horizontalbar", "x":TITLE_BAR_POS_X, "y":LABEL_START_Y+LABEL_GAP*7, "width":TITLE_BAR_WIDTH,
					"children" : ( { "name" : "horizontalbarName7", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", "fontsize" : "LARGE", "color" : 0xFFFF0303 }, ),
				},
				{ 
					"name" : "horizontalbar8", "type":"horizontalbar", "x":TITLE_BAR_POS_X, "y":LABEL_START_Y+LABEL_GAP*8, "width":TITLE_BAR_WIDTH,
					"children" : ( { "name" : "horizontalbarName8", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", "fontsize" : "LARGE", "color" : 0xFFFF0303 }, ),
				},
				{ 
					"name" : "horizontalbar9", "type":"horizontalbar", "x":TITLE_BAR_POS_X, "y":LABEL_START_Y+LABEL_GAP*9, "width":TITLE_BAR_WIDTH,
					"children" : ( { "name" : "horizontalbarName9", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", "fontsize" : "LARGE", "color" : 0xFFFF0303 }, ),
				},
				{ 
					"name" : "horizontalbar10", "type":"horizontalbar", "x":TITLE_BAR_POS_X, "y":LABEL_START_Y+LABEL_GAP*10, "width":TITLE_BAR_WIDTH,
					"children" : ( { "name" : "horizontalbarName10", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", "fontsize" : "LARGE", "color" : 0xFFFF0303 }, ),
				},
				{ 
					"name" : "horizontalbar11", "type":"horizontalbar", "x":TITLE_BAR_POS_X, "y":LABEL_START_Y+LABEL_GAP*11, "width":TITLE_BAR_WIDTH,
					"children" : ( { "name" : "horizontalbarName11", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", "fontsize" : "LARGE", "color" : 0xFFFF0303 }, ),
				},
				{ 
					"name" : "horizontalbar12", "type":"horizontalbar", "x":TITLE_BAR_POS_X, "y":LABEL_START_Y+LABEL_GAP*12, "width":TITLE_BAR_WIDTH,
					"children" : ( { "name" : "horizontalbarName12", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", "fontsize" : "LARGE", "color" : 0xFFFF0303 }, ),
				}, ## 호리즌 바 끝
				
				## 라벨 ##
				{
					"name" : "label0", "type" : "thinboard_circle", "x" : LABEL_START_X, "y" : LABEL_START_Y+LABEL_GAP*0-5, "width" : LABEL_WIDTH, "height" : LABEL_HEIGHT,
					"horizontal_align" : "center",
					"children" : ( 
						{ "name" : "labelvalue0", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", },
					),
				},
				{
					"name" : "label1", "type" : "thinboard_circle", "x" : LABEL_START_X, "y" : LABEL_START_Y+LABEL_GAP*1-5, "width" : LABEL_WIDTH, "height" : LABEL_HEIGHT,
					"horizontal_align" : "center",
					"children" : ( 
						{ "name" : "labelvalue1", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", },
					),
				},
				{
					"name" : "label2", "type" : "thinboard_circle", "x" : LABEL_START_X, "y" : LABEL_START_Y+LABEL_GAP*2-5, "width" : LABEL_WIDTH, "height" : LABEL_HEIGHT,
					"horizontal_align" : "center",
					"children" : ( 
						{ "name" : "labelvalue2", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", },
					),
				},
				{
					"name" : "label3", "type" : "thinboard_circle", "x" : LABEL_START_X, "y" : LABEL_START_Y+LABEL_GAP*3-5, "width" : LABEL_WIDTH, "height" : LABEL_HEIGHT,
					"horizontal_align" : "center",
					"children" : ( 
						{ "name" : "labelvalue3", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", },
					),
				},
				{
					"name" : "label4", "type" : "thinboard_circle", "x" : LABEL_START_X, "y" : LABEL_START_Y+LABEL_GAP*4-5, "width" : LABEL_WIDTH, "height" : LABEL_HEIGHT,
					"horizontal_align" : "center",
					"children" : ( 
						{ "name" : "labelvalue4", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", },
					),
				},
				{
					"name" : "label5", "type" : "thinboard_circle", "x" : LABEL_START_X, "y" : LABEL_START_Y+LABEL_GAP*5-5, "width" : LABEL_WIDTH, "height" : LABEL_HEIGHT,
					"horizontal_align" : "center",
					"children" : ( 
						{ "name" : "labelvalue5", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", },
					),
				},
				{
					"name" : "label6", "type" : "thinboard_circle", "x" : LABEL_START_X, "y" : LABEL_START_Y+LABEL_GAP*6-5, "width" : LABEL_WIDTH, "height" : LABEL_HEIGHT,
					"horizontal_align" : "center",
					"children" : ( 
						{ "name" : "labelvalue6", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", },
					),
				},
				{
					"name" : "label7", "type" : "thinboard_circle", "x" : LABEL_START_X, "y" : LABEL_START_Y+LABEL_GAP*7-5, "width" : LABEL_WIDTH, "height" : LABEL_HEIGHT,
					"horizontal_align" : "center",
					"children" : ( 
						{ "name" : "labelvalue7", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", },
					),
				},
				{
					"name" : "label8", "type" : "thinboard_circle", "x" : LABEL_START_X, "y" : LABEL_START_Y+LABEL_GAP*8-5, "width" : LABEL_WIDTH, "height" : LABEL_HEIGHT,
					"horizontal_align" : "center",
					"children" : ( 
						{ "name" : "labelvalue8", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", },
					),
				},
				{
					"name" : "label9", "type" : "thinboard_circle", "x" : LABEL_START_X, "y" : LABEL_START_Y+LABEL_GAP*9-5, "width" : LABEL_WIDTH, "height" : LABEL_HEIGHT,
					"horizontal_align" : "center",
					"children" : ( 
						{ "name" : "labelvalue9", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", },
					),
				},
				{
					"name" : "label10", "type" : "thinboard_circle", "x" : LABEL_START_X, "y" : LABEL_START_Y+LABEL_GAP*10-5, "width" : LABEL_WIDTH, "height" : LABEL_HEIGHT,
					"horizontal_align" : "center",
					"children" : ( 
						{ "name" : "labelvalue10", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", },
					),
				},
				{
					"name" : "label11", "type" : "thinboard_circle", "x" : LABEL_START_X, "y" : LABEL_START_Y+LABEL_GAP*11-5, "width" : LABEL_WIDTH, "height" : LABEL_HEIGHT,
					"horizontal_align" : "center",
					"children" : ( 
						{ "name" : "labelvalue11", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", },
					),
				},
				{
					"name" : "label12", "type" : "thinboard_circle", "x" : LABEL_START_X, "y" : LABEL_START_Y+LABEL_GAP*12-5, "width" : LABEL_WIDTH, "height" : LABEL_HEIGHT,
					"horizontal_align" : "center",
					"children" : ( 
						{ "name" : "labelvalue12", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : "center", },
					),
				}, ## 라벨 끝
				
				## 버튼
				{ 
							"name" : "labelname0", "type" : "button",
							"x" : LABEL_NAME_POS_X, "y" : LABEL_START_Y+LABEL_GAP*0, 
							
							"text" : "", 
							"horizontal_align" : "center",
							
							"default_image" : LOCALE_PATH + "details.sub",
							"over_image" : LOCALE_PATH + "details.sub",
							"down_image" : LOCALE_PATH + "details.sub",
				},
				{ 
							"name" : "labelname1", "type" : "button",
							"x" : LABEL_NAME_POS_X, "y" : LABEL_START_Y+LABEL_GAP*1, 
							
							"text" : "", 
							"horizontal_align" : "center",
							
							"default_image" : LOCALE_PATH + "details.sub",
							"over_image" : LOCALE_PATH + "details.sub",
							"down_image" : LOCALE_PATH + "details.sub",
				},
				{ 
							"name" : "labelname2", "type" : "button",
							"x" : LABEL_NAME_POS_X, "y" : LABEL_START_Y+LABEL_GAP*2, 
							
							"text" : "", 
							"horizontal_align" : "center",
							
							"default_image" : LOCALE_PATH + "details.sub",
							"over_image" : LOCALE_PATH + "details.sub",
							"down_image" : LOCALE_PATH + "details.sub",
				},
				{ 
							"name" : "labelname3", "type" : "button",
							"x" : LABEL_NAME_POS_X, "y" : LABEL_START_Y+LABEL_GAP*3, 
							
							"text" : "", 
							"horizontal_align" : "center",
							
							"default_image" : LOCALE_PATH + "details.sub",
							"over_image" : LOCALE_PATH + "details.sub",
							"down_image" : LOCALE_PATH + "details.sub",
				},	
				{ 
							"name" : "labelname4", "type" : "button",
							"x" : LABEL_NAME_POS_X, "y" : LABEL_START_Y+LABEL_GAP*4, 
							
							"text" : "", 
							"horizontal_align" : "center",
							
							"default_image" : LOCALE_PATH + "details.sub",
							"over_image" : LOCALE_PATH + "details.sub",
							"down_image" : LOCALE_PATH + "details.sub",
				},	
				{ 
							"name" : "labelname5", "type" : "button",
							"x" : LABEL_NAME_POS_X, "y" : LABEL_START_Y+LABEL_GAP*5, 
							
							"text" : "", 
							"horizontal_align" : "center",
							
							
							"default_image" : LOCALE_PATH + "details.sub",
							"over_image" : LOCALE_PATH + "details.sub",
							"down_image" : LOCALE_PATH + "details.sub",
				},	
				{ 
							"name" : "labelname6", "type" : "button",
							"x" : LABEL_NAME_POS_X, "y" : LABEL_START_Y+LABEL_GAP*6, 
							
							"text" : "", 
							"horizontal_align" : "center",
							
							"default_image" : LOCALE_PATH + "details.sub",
							"over_image" : LOCALE_PATH + "details.sub",
							"down_image" : LOCALE_PATH + "details.sub",
				},	
				{ 
							"name" : "labelname7", "type" : "button",
							"x" : LABEL_NAME_POS_X, "y" : LABEL_START_Y+LABEL_GAP*7, 
							
							"text" : "", 
							"horizontal_align" : "center",
							
							"default_image" : LOCALE_PATH + "details.sub",
							"over_image" : LOCALE_PATH + "details.sub",
							"down_image" : LOCALE_PATH + "details.sub",
				},	
				{ 
							"name" : "labelname8", "type" : "button",
							"x" : LABEL_NAME_POS_X, "y" : LABEL_START_Y+LABEL_GAP*8, 
							
							"text" : "", 
							"horizontal_align" : "center",
							
							"default_image" : LOCALE_PATH + "details.sub",
							"over_image" : LOCALE_PATH + "details.sub",
							"down_image" : LOCALE_PATH + "details.sub",
				},	
				{ 
							"name" : "labelname9", "type" : "button",
							"x" : LABEL_NAME_POS_X, "y" : LABEL_START_Y+LABEL_GAP*9, 
							
							"text" : "", 
							"horizontal_align" : "center",
							
							"default_image" : LOCALE_PATH + "details.sub",
							"over_image" : LOCALE_PATH + "details.sub",
							"down_image" : LOCALE_PATH + "details.sub",
				},	
				{ 
							"name" : "labelname10", "type" : "button",
							"x" : LABEL_NAME_POS_X, "y" : LABEL_START_Y+LABEL_GAP*10, 
							
							"text" : "", 
							"horizontal_align" : "center",
							
							"default_image" : LOCALE_PATH + "details.sub",
							"over_image" : LOCALE_PATH + "details.sub",
							"down_image" : LOCALE_PATH + "details.sub",
				},	
				{ 
							"name" : "labelname11", "type" : "button",
							"x" : LABEL_NAME_POS_X, "y" : LABEL_START_Y+LABEL_GAP*11, 
							
							"text" : "", 
							"horizontal_align" : "center",
							
							"default_image" : LOCALE_PATH + "details.sub",
							"over_image" : LOCALE_PATH + "details.sub",
							"down_image" : LOCALE_PATH + "details.sub",
				},	
				{ 
							"name" : "labelname12", "type" : "button",
							"x" : LABEL_NAME_POS_X, "y" : LABEL_START_Y+LABEL_GAP*12, 
							
							"text" : "", 
							"horizontal_align" : "center",
							
							"default_image" : LOCALE_PATH + "details.sub",
							"over_image" : LOCALE_PATH + "details.sub",
							"down_image" : LOCALE_PATH + "details.sub",
				}, ## 버튼 끝
				
			),
		}, ## MainBoard End
	),
}