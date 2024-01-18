import localeinfo
import uiscriptlocale

PATH = 'd:/ymir work/ui/premium_prodomo/'

WINDOW_WIDTH = 250+167
WINDOW_HEIGHT = 112

window = {
	'name': 'PremiumPlayersWindow', 
	'style': ('float', 'movable'),
	'x': 60, 
	'y': 100, 
	'width': WINDOW_WIDTH, 
	'height': WINDOW_HEIGHT,
	'children': (
		{
		'name': 'board',
		'type': 'board_with_titlebar', 
		'style': ('attach',), 
		'x': 0, 
		'y': 0, 
		'width': WINDOW_WIDTH, 
		'height': WINDOW_HEIGHT, 
		'title': 'Premium',
			'children':  (
				{
				'name': 'ThinBoard', 
				'type': 'thinboard', 
				'x': 10, 
				'y': 32, 
				'width': 229, 
				'height': 70,
					'children': (
					
						{
						'name': 'StatusBox', 
						'type': 'image', 
						'x': 3, 
						'y': 3, 
						'image': PATH + 'box_status.png',
							'children': (
								{
								'name': 'StatusValue', 
								'type': 'text', 
								'x': 60, 
								'y': 3, 'text': uiscriptlocale.PREMIUM_PLAYERS_STATUS_INACTIVE, 
								'text_horizontal_align': 'center', 'color': 0xFFff9090
								},
								{
								'name': 'StarCheck', 
								'type': 'image',
								'x': 112, 
								'y': -5, 'image': PATH + 'star_inactive.png'
								},
								{
								'name': 'ActivateButton', 
								'type': 'button', 
								'x': 145, 
								'y': 2, 
								'default_image': PATH + 'btn/activate_01.png', 
								'over_image': PATH + 'btn/activate_02.png', 
								'down_image': PATH + 'btn/activate_01.png', 
								'text': "Activate"
								},
								{
								'name': 'ListButton', 
								'type': 'button', 
								'x': 145, 
								'y': 2, 
								'default_image': PATH + 'btn/activate_01.png', 
								'over_image': PATH + 'btn/activate_02.png', 
								'down_image': PATH + 'btn/activate_01.png', 
								'text': 'List'},
							),
						},
						
						{
						'name': 'FacilityBox', 
						'type': 'image', 
						'x': 3, 
						'y': 24, 
						'image': PATH + 'box_empty.png',
							'children': (
								{
								'name': 'FacilityTitle', 
								'type': 'text', 
								'x': 112, 
								'y': 3, 'text': uiscriptlocale.PREMIUM_PLAYERS_FACILITY_TITLE, 
								'text_horizontal_align': 'center', 'color': 0xFFff9090
								},
								{
								'name': 'DropDownButton', 
								'type': 'button', 
								'x': 205, 
								'y': 4, 'default_image': PATH + 'btn/drop_down_01.png', 
								'over_image': PATH + 'btn/drop_down_02.png', 
								'down_image': PATH + 'btn/drop_down_03.png'
								},
								{
								'name': 'DropUpButton', 
								'type': 'button', 
								'x': 205, 
								'y': 4, 
								'default_image': PATH + 'btn/drop_up_01.png', 
								'over_image': PATH + 'btn/drop_up_02.png', 
								'down_image': PATH + 'btn/drop_up_03.png'
								},
							),
						},
						
						{
						'name': 'FacilityContent', 
						'type': 'image', 
						'x': 2, 
						'y': 44, 
						'image': PATH + 'facility_content.png',
							'children': (
								{
								'name': 'AffectValue0', 
								'type': 'text', 
								'x': 112, 
								'y': 3, 
								'text': uiscriptlocale.PREMIUM_PLAYERS_ACTIVATE_BONUS1, 
								'text_horizontal_align': 'center', 
								'color': 0xFFadad97
								},
								{
								'name': 'AffectValue0', 
								'type': 'text', 
								'x': 112, 
								'y': 2+17, 
								'text': uiscriptlocale.PREMIUM_PLAYERS_ACTIVATE_BONUS2, 
								'text_horizontal_align': 'center', 
								'color': 0xFFadad97
								},
								{
								'name': 'AffectValue0', 
								'type': 'text', 
								'x': 112, 
								'y': 45, 
								'text': uiscriptlocale.PREMIUM_PLAYERS_ACTIVATE_BONUS3,
								'text_horizontal_align': 'center', 
								'color': 0xFFadad97
								},
								{
								'name': 'AffectValue0', 
								'type': 'text', 
								'x': 112, 
								'y': 45+15, 
								'text': uiscriptlocale.PREMIUM_PLAYERS_ACTIVATE_BONUS4, 
								'text_horizontal_align': 'center', 
								'color': 0xFFadad97
								},
								{
								'name': 'AffectValue0', 
								'type': 'text', 
								'x': 112, 
								'y': 45+15*2, 
								'text': uiscriptlocale.PREMIUM_PLAYERS_ACTIVATE_BONUS5, 
								'text_horizontal_align': 'center', 
								'color': 0xFFadad97
								},
							),
						},
						{
						'name': 'TimeBox', 
						'type': 'image', 
						'x': 3, 
						'y': 45, 
						'image': PATH + 'box_empty.png',
							'children': (
								{
								'name': 'TimeValue', 
								'type': 'text', 
								'x': 112, 
								'y': 3, 
								'text': uiscriptlocale.PREMIUM_PLAYERS_TIME_VALUE_INACTIVE, 
								'text_horizontal_align': 'center', 
								'color': 0xFFff9090
								},
								{
								'name': 'TimeGauge', 
								'type': 'image', 
								'x': 4, 
								'y': 18, 
								'image': PATH + 'gauge_time.png',
									'children': (
										{
										'name': 'TimeGaugeFull', 
										'type': 'expanded_image', 
										'x': 2, 
										'y': 2, 
										'image': PATH + 'gauge_time_full.png'
										},
									),
								},
							),
						},
					),
				},
			),
		},
		
		{
		'name': 'ActivateBoard', 
		'type': 'board', 
		'x': 245, 
		'y': 0, 
		'width': 167, 
		'height': 119,
			'children': (
				{
				'name': 'AttachBoard', 
				'type': 
				'image', 
				'x': 8, 
				'y': 8, 
				'image': PATH + 'activate_board.png',
					'children': (
						{
						'name': 'ActivateTitle', 
						'type': 'text', 
						'x': 75, 
						'y': 10, 
						'text': "Required items", 
						'text_horizontal_align': 'center', 
						'color': 0xFFadad97
						},
						
						{
						'name': 'ItemSlot', 
						'type': 'slot', 
						'x': 16, 
						'y': 36, 
						'width': 125, 
						'height': 32, 
							'slot': (
								{
								'index': 0, 
								'x': 0, 
								'y': 0, 
								'width': 32,
								'height': 32
								},
								{
								'index': 1, 
								'x': 44, 
								'y': 0, 
								'width': 32, 
								'height': 32
								},
								{
								'index': 2, 
								'x': 88, 
								'y': 0, 
								'width': 32, 
								'height': 32
								},
							),
						},
						
						{
						'name': 'ConfirmButton', 
						'type': 'button', 
						'x': 4, 
						'y': 77, 
						'default_image': PATH + 'btn/confirm_01.png', 
						'over_image': PATH + 'btn/confirm_02.png', 
						'down_image': PATH + 'btn/confirm_03.png', 
						'text': "Confirm"
						},
					),
				},
			),
		},
	),
}




















