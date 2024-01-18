WINDOW_WIDTH = 410
WINDOW_HEIGHT = 440

RENDER_TARGET_INDEX = 1

window = {
	"name" : "RenderTargetWindow",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,

	"children":
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 7,

					"width" : WINDOW_WIDTH - 15,
					"color" : "yellow",

					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",

							"x" : (WINDOW_WIDTH - 15) / 2,
							"y" : 3,

							"text" : "Preview",
							"text_horizontal_align":"center"
						},
					),
				},

				# RenderView
				{
					"name" : "RenderTarget",
					"type" : "render_target",
					"style" : ("attach",),

					"x" : 10,
					"y" : 30,

					"width" : 390,
					"height" : 400,

					"index" : RENDER_TARGET_INDEX,
					"children" :
					(
						{
							"name" : "mvUpCmrBtn",
							"type" : "button",

							"x" : 132,
							"y" : 380,

							"default_image" : "d:/ymir work/ui/game/monster_card/button/up_camera/up_camera_button_default.sub",
							"over_image" : "d:/ymir work/ui/game/monster_card/button/up_camera/up_camera_button_over.sub",
							"down_image" : "d:/ymir work/ui/game/monster_card/button/up_camera/up_camera_button_down.sub",
						},

						{
							"name" : "mvDownCmrBtn",
							"type" : "button",

							"x" : 153,
							"y" : 380,

							"default_image" : "d:/ymir work/ui/game/monster_card/button/down_camera/down_camera_button_default.sub",
							"over_image" : "d:/ymir work/ui/game/monster_card/button/down_camera/down_camera_button_over.sub",
							"down_image" : "d:/ymir work/ui/game/monster_card/button/down_camera/down_camera_button_down.sub",
						},
						{
							"name" : "rotLeftBtn",
							"type" : "button",

							"x" : 174,
							"y" : 380,

							"default_image" : "d:/ymir work/ui/game/monster_card/button/left_rotation/left_rotation_button_default.sub",
							"over_image" : "d:/ymir work/ui/game/monster_card/button/left_rotation/left_rotation_button_over.sub",
							"down_image" : "d:/ymir work/ui/game/monster_card/button/left_rotation/left_rotation_button_down.sub",
						},

						{
							"name" : "rotRightBtn",
							"type" : "button",

							"x" : 195,
							"y" : 380,

							"default_image" : "d:/ymir work/ui/game/monster_card/button/right_rotation/right_rotation_button_default.sub",
							"over_image" : "d:/ymir work/ui/game/monster_card/button/right_rotation/right_rotation_button_over.sub",
							"down_image" : "d:/ymir work/ui/game/monster_card/button/right_rotation/right_rotation_button_down.sub",
						},

						{
							"name" : "mvResetBtn",
							"type" : "button",

							"x" : 216,
							"y" : 380,

							"default_image" : "d:/ymir work/ui/game/monster_card/button/mv_reset/mv_reset_button_default.sub",
							"over_image" : "d:/ymir work/ui/game/monster_card/button/mv_reset/mv_reset_button_over.sub",
							"down_image" : "d:/ymir work/ui/game/monster_card/button/mv_reset/mv_reset_button_down.sub",
						},

						{
							"name" : "zumInBtn",
							"type" : "button",

							"x" : 237,
							"y" : 380,

							"default_image" : "d:/ymir work/ui/game/monster_card/button/zoomin/zoomin_rotation_button_default.sub",
							"over_image" : "d:/ymir work/ui/game/monster_card/button/zoomin/zoomin_rotation_button_over.sub",
							"down_image" : "d:/ymir work/ui/game/monster_card/button/zoomin/zoomin_rotation_button_down.sub",
						},

						{
							"name" : "zumOutBtn",
							"type" : "button",

							"x" : 258,
							"y" : 380,

							"default_image" : "d:/ymir work/ui/game/monster_card/button/zoomout/zoomin_rotation_button_default.sub",
							"over_image" : "d:/ymir work/ui/game/monster_card/button/zoomout/zoomin_rotation_button_over.sub",
							"down_image" : "d:/ymir work/ui/game/monster_card/button/zoomout/zoomin_rotation_button_down.sub",
						},
					),
				},
			),
		},
	),
}