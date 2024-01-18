#(C) 2019 Owsap Productions

import uiscriptlocale

ROOT = 'd:/ymir work/ui/game/dungeoninfo_2022/'

MAINBOARD_WIDTH = 613
MAINBOARD_HEIGHT = 428

LEFTBOARD_WIDTH = 315
LEFTBOARD_HEIGHT = 381
LEFTBOARD_X = 13
LEFTBOARD_Y = 35

SCROLLBAR_X = LEFTBOARD_WIDTH + 18
SCROLLBAR_Y = 32
SCROLLBAR_SIZE = LEFTBOARD_HEIGHT + 9

RIGHTBOARD_WIDTH = 246
RIGHTBOARD_HEIGHT = 381
RIGHTBOARD_X = 324 + 1
RIGHTBOARD_Y = 35

window = {
	"name" : "DungeonInfoWindow",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 0,

	"width" : MAINBOARD_WIDTH,
	"height" : MAINBOARD_HEIGHT,

	"children" :
	(
		#######################
		##   DUNGEON_BOARD   ##
		{
			"name" : "DungeonBoard",
			"type" : "board",
			"style" : ("attach", "ltr"),

			"x" : 0,
			"y" : 0,

			"width" : MAINBOARD_WIDTH,
			"height" : MAINBOARD_HEIGHT,

			"children" :
			(
				################################
				##   DUNGEON_BOARD_TITLEBAR   ##
				{
					"name" : "DungeonBoardTitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 6,
					"y" : 7,

					"width" : MAINBOARD_WIDTH - 13,

					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",

							"x" : 0,
							"y" : -2,

							"text": uiscriptlocale.DUNGEON_INFO,
							"all_align":"center"
						},
					),
				},
				##   DUNGEON_BOARD_TITLEBAR   ##
				################################

				##############################
				##   DUNGEON_BUTTON_BOARD   ##
				{
					"name" : "DungeonButtonThinBoard",
					"type" : "thinboard",

					"x" : LEFTBOARD_X - 4,
					"y" : LEFTBOARD_Y - 4,

					"width" : LEFTBOARD_WIDTH + 8,
					"height" : LEFTBOARD_HEIGHT + 8,
				},
				{
					"name" : "DungeonButtonBoard",
					"type" : "thinboard_circle",

					"x" : LEFTBOARD_X,
					"y" : LEFTBOARD_Y,

					"width" : LEFTBOARD_WIDTH,
					"height" : LEFTBOARD_HEIGHT,

					"children" : 
					(
						## logininfo.py
					),
				},
				##   DUNGEON_BUTTON_BOARD   ##
				##############################

				########################################
				##   DUNGEON_BUTTON_BOARD_SCROLLBAR   ##
				{
					"name" : "ScrollBar",
					"type" : "scrollbar",

					"x" : LEFTBOARD_WIDTH + 18 + 1,
					"y" : LEFTBOARD_Y - 4,

					"size" : LEFTBOARD_HEIGHT + 9,
				},
				##   DUNGEON_BUTTON_BOARD_SCROLLBAR   ##
				########################################

				############################
				##   DUNGEON_INFO_BOARD   ##
				############################
				############################
				{
					"name" : "DungeonInfoWindow",
					"type" : "window",

					"x" : LEFTBOARD_WIDTH + 18 + 17,
					"y" : RIGHTBOARD_Y,
					"width" : RIGHTBOARD_WIDTH,
					"height" : RIGHTBOARD_HEIGHT,
				},
				{
					"name" : "DungeonInfoThinBoard",
					"type" : "thinboard",

					"x" : LEFTBOARD_WIDTH + 18 + 17,
					"y" : RIGHTBOARD_Y - 4,

					"width" : RIGHTBOARD_WIDTH + 8,
					"height" : RIGHTBOARD_HEIGHT + 8,
					"children" :
					(
						{
							"name" : "DungeonInfoThinBoardCircleýp",
							"type" : "thinboard_circle",

							"x" : 4,
							"y" : 4,

							"width" : RIGHTBOARD_WIDTH,
							"height" : RIGHTBOARD_HEIGHT,
						},
					),
				},
				{
					"name" : "DungeonInfoBoard",
					"type" : "window",

					"x" : LEFTBOARD_WIDTH + 18 + 17 - 4,
					"y" : RIGHTBOARD_Y,

					"width" : RIGHTBOARD_WIDTH,
					"height" : RIGHTBOARD_HEIGHT,

#					"image" : "d:/ymir work/ui/game/dungeon_info/dungeon_bg.tga",

					"children" : 
					(
						## Title Bar
						{
							"name" : "DungeonInfoTitleBar",
							"type" : "image",

							"x" : 336 - RIGHTBOARD_X - 1,
							"y" : 74 - RIGHTBOARD_Y - 33,

							"image" : ROOT + "backgrounds/title.tga",

							"children" :
							(
								{
									"name" : "DungeonInfoName",
									"type" : "text",

									"x" : 0,
									"y" : 0,

									"text" : uiscriptlocale.DUNGEON_INFO_DUNGEON,
									"color" : 0xFFFEE3AE,
									"fontname" : "Tahoma:17",
									"all_align" : "center"
								},
							),
						},

						## Dungeon Type
						{
							"name" : "DungeonInfoBoard",
							"type" : "image",

							"x" : 336 - RIGHTBOARD_X,
							"y" : 74 - RIGHTBOARD_Y,

							"image" : ROOT+"backgrounds/66.tga",

						},
						{
							"name" : "DungeonInfoType",
							"type" : "text",

							"x" : 15,
							"y" : 43 * 1,

							"text" : uiscriptlocale.DUNGEON_INFO_TYPE,
						},

						## Dungeon Organization
						# {
							# "name" : "DungeonInfoOrganization",
							# "type" : "text",

							# "x" : 15,
							# "y" : 40 + 20 * 1,

							# "text" : uiscriptlocale.DUNGEON_INFO_ORGANIZATION,
						# },

						## Dungeon Level Limit
						{
							"name" : "DungeonInfoLevelLimit",
							"type" : "text",

							"x" : 15,
							"y" : 38 + 20 * 2,

							"text" : uiscriptlocale.DUNGEON_INFO_LEVEL_LIMIT,
						},

						## Dungeon Party Members
						{
							"name" : "DungeonInfoPartyMembers",
							"type" : "text",

							"x" : 15,
							"y" : 38 + 20 * 3,

							"text" : uiscriptlocale.DUNGEON_INFO_PARTY_MEMBERS,
						},

						## Dungeon Cooldown
						{
							"name" : "DungeonInfoCooldown",
							"type" : "text",

							"x" : 15,
							"y" : 36 + 20 * 4,

							"text" : uiscriptlocale.DUNGEON_INFO_COOLDOWN,
						},

						## Dungeon Duration
						{
							"name" : "DungeonInfoDuration",
							"type" : "text",

							"x" : 15,
							"y" : 32 + 20 * 5,

							"text" : uiscriptlocale.DUNGEON_INFO_DURATION,
						},

						## Dungeon Entrance
						# {
							# "name" : "DungeonInfoEntrance",
							# "type" : "text",

							# "x" : 105,
							# "y" : 32 + 20 * 5,

							# "text" : uiscriptlocale.DUNGEON_INFO_ENTRANCE,
						# },

						## Dungeon Strength Bonus
						{
							"name" : "DungeonInfoStrengthBonus",
							"type" : "text",

							"x" : 15,
							"y" : 40 + 20 * 1,
							"text" : uiscriptlocale.DUNGEON_INFO_STRENGTH,
						},

						## Dungeon Resistance Bonus
						# {
							# "name" : "DungeonInfoResistanceBonus",
							# "type" : "text",

							# "x" : 15+90,
							# "y" : 40 + 20 * 1,
							# "text" : uiscriptlocale.DUNGEON_INFO_RESISTANCE,
						# },

						## Dungeon Personal Stats
						{
							"name" : "DungeonInfoPersonalStats",
							"type" : "text",

							"x" : 15,
							"y" : -25,

							"text" : uiscriptlocale.DUNGEON_INFO_PERSONAL_STATS,
							"color" : 0xFFFEE3AE,
							"all_align" : "center",
						},

						## Personal Stats - Total Finished
						{
							"name" : "DungeonInfoTotalFinished",
							"type" : "text",

							"x" : 15,
							"y" : 26 + 20 * 8,

							"text" : uiscriptlocale.DUNGEON_INFO_TOTAL_FINISHED,
						},

						## Personal Stats - Fastest Time
						{
							"name" : "DungeonInfoFastestTime",
							"type" : "text",

							"x" : 15,
							"y" : 26 + 20 * 9,

							"text" : uiscriptlocale.DUNGEON_INFO_FASTEST_TIME,
						},

						## Personal Stats - Highest DMG
						{
							"name" : "DungeonInfoHighestDamage",
							"type" : "text",

							"x" : 15,
							"y" : 26 + 20 * 10,

							"text" : uiscriptlocale.DUNGEON_INFO_HIGHEST_DAMAGE,
						},

						## Special Item Slot BG
						{
							"name" : "DungeonInfoItem",
							"type" : "expanded_image",
							"style" : ("attach",),

							"x" : RIGHTBOARD_WIDTH - 50,
							"y" : RIGHTBOARD_HEIGHT - 50,

							"image" : "d:/ymir work/ui/minigame/fish_event/fish_special_slot.sub",

							"children" :
							(
								## Special Item Slot
								{
									"name" : "DungeonInfoItemSlot",
									"type" : "slot",

									"x" : 7,
									"y" : 7,

									"width" : 32,
									"height" : 32,

									"image" : "d:/ymir work/ui/pet/skill_button/skill_enable_button.sub",

									"slot" : (
										{ "index" : 0, "x" : 0, "y" : 0, "width" : 32, "height" : 32 },
									),
								},
							),
						},

						## Rank buttons
						{
							"name" : "DungeonRank1Button",
							"type" : "button",

							"x" : 20,
							"y" : 26 + 20 * 12,

							"tooltip_text" : uiscriptlocale.DUNGEON_RANKING_TYPE1,
							"tooltip_x" : 0,
							"tooltip_y" : -13,

							"default_image" : "d:/ymir work/ui/game/dungeon_info/button/rank_button00.sub",
							"over_image" : "d:/ymir work/ui/game/dungeon_info/button/rank_button01.sub",
							"down_image" : "d:/ymir work/ui/game/dungeon_info/button/rank_button02.sub",
						},
						{
							"name" : "DungeonRank2Button",
							"type" : "button",

							"x" : 20+80,
							"y" : 26 + 20 * 12,

							"tooltip_text" : uiscriptlocale.DUNGEON_RANKING_TYPE2,
							"tooltip_x" : 0,
							"tooltip_y" : -13,

							"default_image" : "d:/ymir work/ui/game/dungeon_info/button/rank_button10.sub",
							"over_image" : "d:/ymir work/ui/game/dungeon_info/button/rank_button11.sub",
							"down_image" : "d:/ymir work/ui/game/dungeon_info/button/rank_button12.sub",
						},
						{
							"name" : "DungeonRank3Button",
							"type" : "button",

							"x" : 20+80+80,
							"y" : 26 + 20 * 12,

							"tooltip_text" : uiscriptlocale.DUNGEON_RANKING_TYPE3,
							"tooltip_x" : 0,
							"tooltip_y" : -13,

							"default_image" : "d:/ymir work/ui/game/dungeon_info/button/rank_button20.sub",
							"over_image" : "d:/ymir work/ui/game/dungeon_info/button/rank_button21.sub",
							"down_image" : "d:/ymir work/ui/game/dungeon_info/button/rank_button22.sub",
						},
						{
							"name" : "DungeonInfoShopTicket",
							"type" : "button",

							"x" : 35,
							"y" : 340,

							"text" : uiscriptlocale.DRAGONSOUL_SHOP,
							"default_image" : ROOT + "buttons/button_1.png",
							"over_image" :    ROOT + "buttons/button_2.png",
							"down_image" :    ROOT + "buttons/button_2.png",
						},

						# Teleport & Close buttons
						{
							"name" : "DungeonInfoTeleportButton",
							"type" : "button",

							"x" : 35,
							"y" : 310,

							"text" : uiscriptlocale.DUNGEON_INFO_TELEPORT,
							"default_image" : ROOT + "buttons/button_1.png",
							"over_image" :    ROOT + "buttons/button_2.png",
							"down_image" :    ROOT + "buttons/button_2.png",
						},
						# {
							# "name" : "CloseDungeonBoard",
							# "type" : "button",

							# "x" : RIGHTBOARD_WIDTH /2 - 85,
							# "y" : RIGHTBOARD_HEIGHT - 25,

							# "text" : uiscriptlocale.DUNGEON_INFO_CLOSE,
							# "default_image" : "d:/ymir work/ui/game/dungeon_info/button/normal_button_default.sub",
							# "over_image" : "d:/ymir work/ui/game/dungeon_info/button/normal_button_over.sub",
							# "down_image" : "d:/ymir work/ui/game/dungeon_info/button/normal_button_down.sub",
						# },
					),
				},
				##   DUNGEON_INFO_BOARD   ##
				############################
			),
		},
		##   DUNGEON_BOARD   ##
		#######################
	),
}