import uiscriptlocale

SMALL_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_01.sub"
LARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_03.sub"
XLARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_04.sub"

window = {
	"name" : "GuildWindow_GuildInfoPage",

	"x" : 8,
	"y" : 30,

	"width" : 360,
	"height" : 298,

	"children" :
	(

		## Guild Info Title
		{
			"name":"Guild_Info_Title_Bar", "type":"horizontalbar", "x":5, "y":10, "width":169,
			"children" :
			(
				{ "name":"Guild_Info_Point_Value", "type":"text", "x":8, "y":3, "text":uiscriptlocale.GUILD_INFO, },

				## GuildName
				{
					"name" : "GuildName", "type" : "text", "x" : 3, "y" : 31, "text" : uiscriptlocale.GUILD_INFO_NAME,
					"children" :
					(
						{
							"name" : "GuildNameSlot",
							"type" : "image",
							"x" : 70,
							"y" : -2,
							"image" : LARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "GuildNameValue", "type":"text", "text":uiscriptlocale.GUILD_INFO_NAME_VALUE, "x":0, "y":0, "all_align":"center"},
							),
						},
					),
				},

				## GuildMaster
				{
					"name" : "GuildMaster", "type" : "text", "x" : 3, "y" : 57, "text" : uiscriptlocale.GUILD_INFO_MASTER,
					"children" :
					(
						{
							"name" : "GuildMasterNameSlot",
							"type" : "image",
							"x" : 70,
							"y" : -2,
							"image" : LARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "GuildMasterNameValue", "type":"text", "text":uiscriptlocale.GUILD_INFO_MASTER_VALUE, "x":0, "y":0, "all_align":"center"},
							),
						},
					),
				},

				## GuildLevel
				{
					"name" : "GuildLevel", "type" : "text", "x" : 3, "y" : 93, "text" : uiscriptlocale.GUILD_INFO_LEVEL,
					"children" :
					(
						{
							"name" : "GuildLevelSlot",
							"type" : "slotbar",
							"x" : 70,
							"y" : -2,
							"width" : 45,
							"height" : 17,
							"children" :
							(
								{"name" : "GuildLevelValue", "type":"text", "text":"30", "x":0, "y":0, "all_align":"center"},
							),
						},
					),
				},

				## CurrentExperience
				{
					"name" : "CurrentExperience", "type" : "text", "x" : 3, "y" : 119, "text" : uiscriptlocale.GUILD_INFO_CUR_EXP,
					"children" :
					(
						{
							"name" : "CurrentExperienceSlot",
							"type" : "image",
							"x" : 70,
							"y" : -2,
							"image" : LARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "CurrentExperienceValue", "type":"text", "text":"10000000", "x":0, "y":0, "all_align":"center"},
							),
						},
					),
				},

				## LastExperience
				{
					"name" : "LastExperience", "type" : "text", "x" : 3, "y" : 145, "text" : uiscriptlocale.GUILD_INFO_REST_EXP,
					"children" :
					(
						{
							"name" : "LastExperienceSlot",
							"type" : "image",
							"x" : 70,
							"y" : -2,
							"image" : LARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "LastExperienceValue", "type":"text", "text":"123123123123", "x":0, "y":0, "all_align":"center"},
							),
						},
					),
				},

				## GuildMemberCount
				{
					"name" : "GuildMemberCount", "type" : "text", "x" : 3, "y" : 171, "text" : uiscriptlocale.GUILD_INFO_MEMBER_NUM,
					"children" :
					(
						{
							"name" : "GuildMemberCountSlot",
							"type" : "image",
							"x" : 70,
							"y" : -2,
							"image" : LARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "GuildMemberCountValue", "type":"text", "text":"30 / 32", "x":0, "y":0, "all_align":"center"},
							),
						},
					),
				},

				## GuildMemberLevelAverage
				{
					"name" : "GuildMemberLevelAverage", "type" : "text", "x" : 3, "y" : 197, "text" : uiscriptlocale.GUILD_INFO_MEMBER_AVG_LEVEL,
					"children" :
					(
						{
							"name" : "GuildMemberLevelAverageSlot",
							"type" : "image",
							"x" : 108,
							"y" : -2,
							"image" : SMALL_VALUE_FILE,
							"children" :
							(
								{"name" : "GuildMemberLevelAverageValue", "type":"text", "text":"53", "x":0, "y":0, "all_align":"center"},
							),
						},
					),
				},

				## GuildMoney
				{
					"name" : "GuildMoney", "type" : "text", "x" : 3, "y" : 233, "text" : uiscriptlocale.GUILD_MONEY,
					"children" :
					(
						{
							"name" : "GuildMoneySlot",
							"type" : "image",
							"x" : 70,
							"y" : -2,
							"image" : LARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "GuildMoneyValue", "type":"text", "text":"9999999", "x":0, "y":0, "all_align":"center"},
							),
						},
					),
				},
			),
		},

		## Button
		{
			"name" : "OfferButton",
			"type" : "button",
			"x" : 127,
			"y" : 100,
			"text" : uiscriptlocale.GUILD_INFO_OFFER_EXP,
			"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
			"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
			"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
		},
			{
				"name" : "DepositButton",
				"type" : "button",
				"x" : 78,
				"y" : 263,
				"text" : uiscriptlocale.GUILD_DEPOSIT,
				"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
				"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
				"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
			},
			{
				"name" : "WithdrawButton",
				"type" : "button",
				"x" : 126,
				"y" : 263,
				"text" : uiscriptlocale.GUILD_WITHDRAW,
				"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
				"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
				"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
			},

		###############################################################################################################

		## Guild Mark Title
		{
			"name":"Guild_Mark_Title_Bar", "type":"horizontalbar", "x":188, "y":10, "width":169,
			"children" :
			(

				{ "name":"Guild_Mark_Title", "type":"text", "x":8, "y":3, "text":uiscriptlocale.GUILD_MARK, },

				## LargeGuildMark
				{
					"name" : "LargeGuildMarkSlot",
					"type" : "slotbar",
					"x" : 5,
					"y" : 24,
					"width" : 48+1,
					"height" : 36+1,
					"children" :
					(
						{
							"name" : "LargeGuildMark",
							"type" : "mark",
							"x" : 1,
							"y" : 1,
						},
					),
				},

			),
		},

		## UploadButton
		{
			"name" : "UploadGuildMarkButton",
			"type" : "button",
			"x" : 260,
			"y" : 33,
			"text" : uiscriptlocale.GUILD_INFO_UPLOAD_MARK,
			"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
			"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
			"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
		},
		{
			"name" : "UploadGuildSymbolButton",
			"type" : "button",
			"x" : 260,
			"y" : 33 + 23,
			"text" : uiscriptlocale.GUILD_INFO_UPLOAD_SYMBOL,
			"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
			"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
			"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
		},

		## Guild Mark Title
		{
			"name":"Guild_Mark_Title_Bar", "type":"horizontalbar", "x":188, "y":85, "width":169,
			"children" :
			(

				{ "name":"Guild_Mark_Title", "type":"text", "x":8, "y":3, "text":uiscriptlocale.GUILD_INFO_ENEMY_GUILD, },

				{
					"name" : "EnemyGuildSlot1",
					"type" : "image",
					"x" : 4,
					"y" : 27 + 26*0,
					"image" : XLARGE_VALUE_FILE,
					"children" :
					(
						{"name" : "EnemyGuildName1", "type":"text", "text":uiscriptlocale.GUILD_INFO_ENEMY_GUILD_EMPTY, "x":0, "y":0, "all_align":"center"},
					),
				},
				{
					"name" : "EnemyGuildSlot2",
					"type" : "image",
					"x" : 4,
					"y" : 27 + 26*1,
					"image" : XLARGE_VALUE_FILE,
					"children" :
					(
						{"name" : "EnemyGuildName2", "type":"text", "text":uiscriptlocale.GUILD_INFO_ENEMY_GUILD_EMPTY, "x":0, "y":0, "all_align":"center"},
					),
				},
				{
					"name" : "EnemyGuildSlot3",
					"type" : "image",
					"x" : 4,
					"y" : 27 + 26*2,
					"image" : XLARGE_VALUE_FILE,
					"children" :
					(
						{"name" : "EnemyGuildName3", "type":"text", "text":uiscriptlocale.GUILD_INFO_ENEMY_GUILD_EMPTY, "x":0, "y":0, "all_align":"center"},
					),
				},
				{
					"name" : "EnemyGuildSlot4",
					"type" : "image",
					"x" : 4,
					"y" : 27 + 26*3,
					"image" : XLARGE_VALUE_FILE,
					"children" :
					(
						{"name" : "EnemyGuildName4", "type":"text", "text":uiscriptlocale.GUILD_INFO_ENEMY_GUILD_EMPTY, "x":0, "y":0, "all_align":"center"},
					),
				},
				{
					"name" : "EnemyGuildSlot5",
					"type" : "image",
					"x" : 4,
					"y" : 27 + 26*4,
					"image" : XLARGE_VALUE_FILE,
					"children" :
					(
						{"name" : "EnemyGuildName5", "type":"text", "text":uiscriptlocale.GUILD_INFO_ENEMY_GUILD_EMPTY, "x":0, "y":0, "all_align":"center"},
					),
				},
				{
					"name" : "EnemyGuildSlot6",
					"type" : "image",
					"x" : 4,
					"y" : 27 + 26*5,
					"image" : XLARGE_VALUE_FILE,
					"children" :
					(
						{"name" : "EnemyGuildName6", "type":"text", "text":uiscriptlocale.GUILD_INFO_ENEMY_GUILD_EMPTY, "x":0, "y":0, "all_align":"center"},
					),
				},

			),
		},

		## CancelButtons
		{
			"name" : "EnemyGuildCancel1",
			"type" : "button",
			"x" : 310,
			"y" : 111 + 26*0,
			"text" : uiscriptlocale.CANCEL,
			"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
			"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
			"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
		},
		{
			"name" : "EnemyGuildCancel2",
			"type" : "button",
			"x" : 310,
			"y" : 111 + 26*1,
			"text" : uiscriptlocale.CANCEL,
			"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
			"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
			"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
		},
		{
			"name" : "EnemyGuildCancel3",
			"type" : "button",
			"x" : 310,
			"y" : 111 + 26*2,
			"text" : uiscriptlocale.CANCEL,
			"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
			"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
			"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
		},
		{
			"name" : "EnemyGuildCancel4",
			"type" : "button",
			"x" : 310,
			"y" : 111 + 26*3,
			"text" : uiscriptlocale.CANCEL,
			"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
			"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
			"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
		},
		{
			"name" : "EnemyGuildCancel5",
			"type" : "button",
			"x" : 310,
			"y" : 111 + 26*4,
			"text" : uiscriptlocale.CANCEL,
			"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
			"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
			"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
		},
		{
			"name" : "EnemyGuildCancel6",
			"type" : "button",
			"x" : 310,
			"y" : 111 + 26*5,
			"text" : uiscriptlocale.CANCEL,
			"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
			"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
			"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
		},

		## DeclareWar
		{
			"name" : "DeclareWarButton",
			"type" : "button",
			"x" : 250 + 15,
			"y" : 264,
			"text" : uiscriptlocale.GUILD_INFO_DECALRE_WAR,
			"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
			"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
			"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
		},

	),
}
