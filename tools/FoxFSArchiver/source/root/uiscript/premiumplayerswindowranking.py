import localeinfo, uiscriptlocale

PATH = 'd:/ymir work/ui/premium_prodomo/'

WINDOW_WIDTH = 205
WINDOW_HEIGHT = 305

window = {
    'name': 'PremiumPlayersWindowRanking', 'style': ('float', 'movable'), 'x': 310, 'y': 100, 'width': WINDOW_WIDTH, 'height': WINDOW_HEIGHT,
    'children': (
        {'name': 'board', 'type': 'board_with_titlebar', 'style': ('attach',), 'x': 0, 'y': 0, 'width': WINDOW_WIDTH, 'height': WINDOW_HEIGHT, 'title': 'Premium Players',
            'children': (
				{'name': 'RankingTitle', 'type': 'image', 'x': 11, 'y': 33, 'image': PATH + 'ranking_list_header.png',
					'children': (
						{'name': 'RankingTitlePos', 'type': 'text', 'x': 8, 'y': 3, 'text': 'Poziþia'},
						{'name': 'RankingTitleName', 'type': 'text', 'x': 100, 'y': 3, 'text': 'Nume'},
					),
				},
				{'name': 'RankingBoard', 'type': 'thinboard', 'x': 8, 'y': 55, 'width': 188, 'height': 240,
					'children': (
						{'name': 'ScrollBar', 'type': 'scrollbar', 'x': 182, 'y': 1, 'size': 238},
					),
				},
            ),
        },
    ),
}