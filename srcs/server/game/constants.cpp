#include "stdafx.h"
#include "char.h"
#include "../common/CommonDefines.h"
TJobInitialPoints JobInitialPoints[JOB_MAX_NUM] =
/*
   {
   int st, ht, dx, iq;
   int max_hp, max_sp;
   int hp_per_ht, sp_per_iq;
   int hp_per_lv_begin, hp_per_lv_end;
   int sp_per_lv_begin, sp_per_lv_end;
   int max_stamina;
   int stamina_per_con;
   int stamina_per_lv_begin, stamina_per_lv_end;
   }
 */
{
	// str con dex int 초기HP 초기SP  CON/HP INT/SP  HP랜덤/lv   MP랜덤/lv  초기stam  stam/con stam/lv
	{   6,  4,  3,  3,  600,   200,     40,    20,    36, 44,     18, 22,     800,      5,      1, 3  }, // JOB_WARRIOR  16
	{   4,  3,  6,  3,  650,   200,     40,    20,    36, 44,     18, 22,     800,      5,      1, 3  }, // JOB_ASSASSIN 16
	{   5,  3,  3,  5,  650,   200,     40,    20,    36, 44,     18, 22,     800,      5,      1, 3  }, // JOB_SURA	 16
	{   3,  4,  3,  6,  700,   200,     40,    20,    36, 44,     18, 22,     800,      5,      1, 3  },  // JOB_SHAMANa  16
};

const TMobRankStat MobRankStats[MOB_RANK_MAX_NUM] =
/*
   {
   int         iGoldPercent;
   }
 */
{
	{  20,  }, // MOB_RANK_PAWN,
	{  20,  }, // MOB_RANK_S_PAWN,
	{  25,  }, // MOB_RANK_KNIGHT,
	{  30,  }, // MOB_RANK_S_KNIGHT,
	{  50,  }, // MOB_RANK_BOSS,
	{ 100,  }  // MOB_RANK_KING,
};

TBattleTypeStat BattleTypeStats[BATTLE_TYPE_MAX_NUM] =
/*
   {
   int         AttGradeBias;
   int         DefGradeBias;
   int         MagicAttGradeBias;
   int         MagicDefGradeBias;
   }
 */
{
	{	  0,	  0,	  0,	-10	}, // BATTLE_TYPE_MELEE,
	{	 10,	-20,	-10,	-15	}, // BATTLE_TYPE_RANGE,
	{	 -5,	 -5,	 10,	 10	}, // BATTLE_TYPE_MAGIC,
	{	  0,	  0,	  0,	  0	}, // BATTLE_TYPE_SPECIAL,
	{	 10,	-10,	  0,	-15	}, // BATTLE_TYPE_POWER,
	{	-10,	 10,	-10,	  0	}, // BATTLE_TYPE_TANKER,
	{	 20,	-20,	  0,	-10	}, // BATTLE_TYPE_SUPER_POWER,
	{	-20,	 20,	-10,	  0	}, // BATTLE_TYPE_SUPER_TANKER,
};

const DWORD * exp_table = NULL;

#ifdef __NEWPET_SYSTEM__
DWORD * exppet_table = NULL;
//const DWORD pets = 4;
const DWORD Pet_Table[11][2] =
{
	{55701, 34041},
	{55702, 34045},
	{55703, 34049},
	{55704, 34053},
	{55705, 34036},
	{55706, 34064},
	{55707, 34073},
	{55708, 34075},
	{55709, 34080},
	{55710, 34082},
	{55711, 34095}
};

const DWORD Pet_Skill_Table[12][23] =
{
	{1, 78, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
	{2, 80, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
	{3, 79, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
	{4, 81, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
	{5, 120, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
	{6, 116, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
	{7, 117, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
	{8, 66, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40},
	{9, 68, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40},
	{10, 64, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
	{11, 27, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
	{12, 39, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}
};

const DWORD Pet_SKill_TablePerc[4][20] =
{
	{ 2, 5, 8, 11, 13, 16, 19, 22, 24, 27, 30, 33, 35, 38, 41, 44, 46, 49, 52, 55 },
	{ 2, 5, 7, 10, 12, 15, 17, 20, 23, 25, 27, 30, 32, 35, 37, 40, 42, 45, 47, 50 },
	{ 2, 5, 8, 11, 13, 16, 19, 22, 25, 27, 30, 33, 36, 38, 41, 44, 47, 50, 52, 55 },
	{ 2, 5, 7, 10, 12, 15, 17, 20, 23, 25, 27, 30, 32, 35, 37, 40, 42, 45, 47, 50 }
};

DWORD exppet_table_common[121];
#endif

const DWORD exp_table_common[PLAYER_MAX_LEVEL_CONST + 1] =
{
	0,	//	0
	300,
	800,
	1500,
	2500,
	4300,
	7200,
	11000,
	17000,
	24000,
	33000,	//	10
	43000,
	58000,
	76000,
	100000,
	130000,
	169000,
	219000,
	283000,
	365000,
	472000,	//	20
	610000,
	705000,
	813000,
	937000,
	1077000,
	1237000,
	1418000,
	1624000,
	1857000,
	2122000,	//	30
	2421000,
	2761000,
	3145000,
	3580000,
	4073000,
	4632000,
	5194000,
	5717000,
	6264000,
	6837000,	//	40
	7600000,
	8274000,
	8990000,
	9753000,
	10560000,
	11410000,
	12320000,
	13270000,
	14280000,
	15340000,	//	50
	16870000,
	18960000,
	19980000,
	21420000,
	22930000,
	24530000,
	26200000,
	27960000,
	29800000,
	32780000,	//	60
	36060000,
	39670000,
	43640000,
	48000000,
	52800000,
	58080000,
	63890000,
	70280000,
	77310000,
	85040000,	//	70
	93540000,
	102900000,
	113200000,
	124500000,
	137000000,
	150700000,
	165700000,
	236990000,
	260650000,
	286780000,	//	80
	315380000,
	346970000,
	381680000,
	419770000,
	461760000,
	508040000,
	558740000,
	614640000,
	676130000,
	743730000,	//	90
	1041222000,
	1145344200,
	1259878620,
	1385866482,
	1524453130,
	1676898443,
	1844588288,
	2029047116,
	2050000000,	//	99레벨 일 때 필요경험치 (100레벨이 되기 위한)
	2150000000u,	//	100
	2210000000u,
	2250000000u,
	2280000000u,
	2310000000u,
	2330000000u,	//	105
	2350000000u,
	2370000000u,
	2390000000u,
	2400000000u,
	2410000000u,	//	110
	2420000000u,
	2430000000u,
	2440000000u,
	2450000000u,
	2460000000u,	//	115
	2470000000u,
	2480000000u,
	2490000000u,
	2490000000u,
	2500000000u,	//	120
	// extra
	2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u, // 130
	2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u, // 140
	2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u, // 150
	2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u, // 160
	2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u, // 170
	2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u, // 180
	2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u, // 190
	2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u, // 200
	2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u, // 210
	2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u, // 220
	2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u, // 230
	2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u, // 240
	2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u,2500000000u, // 250
};

const int * aiPercentByDeltaLev = NULL;
const int * aiPercentByDeltaLevForBoss = NULL;

// 적과 나와의 레벨차이에 의한 계산에 사용되는 테이블
// MIN(MAX_EXP_DELTA_OF_LEV - 1, (적렙 + 15) - 내렙))
const int aiPercentByDeltaLevForBoss_euckr[MAX_EXP_DELTA_OF_LEV] =
{
	1,      // -15  0
	3,          // -14  1
	5,          // -13  2
	7,          // -12  3
	15,         // -11  4
	30,         // -10  5
	60,         // -9   6
	90,         // -8   7
	91,         // -7   8
	92,         // -6   9
	93,         // -5   10
	94,         // -4   11
	95,         // -3   12
	97,         // -2   13
	99,         // -1   14
	100,        // 0    15
	105,        // 1    16
	110,        // 2    17
	115,        // 3    18
	120,        // 4    19
	125,        // 5    20
	130,        // 6    21
	135,        // 7    22
	140,        // 8    23
	145,        // 9    24
	150,        // 10   25
	155,        // 11   26
	160,        // 12   27
	165,        // 13   28
	170,        // 14   29
	180         // 15   30
};



const int aiPercentByDeltaLev_euckr[MAX_EXP_DELTA_OF_LEV] =
{
	1,  //  -15 0
	5,  //  -14 1
	10, //  -13 2
	20, //  -12 3
	30, //  -11 4
	50, //  -10 5
	70, //  -9  6
	80, //  -8  7
	85, //  -7  8
	90, //  -6  9
	92, //  -5  10
	94, //  -4  11
	96, //  -3  12
	98, //  -2  13
	100,    //  -1  14
	100,    //  0   15
	105,    //  1   16
	110,    //  2   17
	115,    //  3   18
	120,    //  4   19
	125,    //  5   20
	130,    //  6   21
	135,    //  7   22
	140,    //  8   23
	145,    //  9   24
	150,    //  10  25
	155,    //  11  26
	160,    //  12  27
	165,    //  13  28
	170,    //  14  29
	180,    //  15  30
};

const DWORD party_exp_distribute_table[PLAYER_EXP_TABLE_MAX + 1] =
{
	0,
	10,		10,		10,		10,		15,		15,		20,		25,		30,		40,		// 1 - 10
	50,		60,		80,		100,	120,	140,	160,	184,	210,	240,	// 11 - 20
	270,	300,	330,	360,	390,	420,	450,	480,	510,	550,	// 21 - 30
	600,	640,	700,	760,	820,	880,	940,	1000,	1100,	1180,	// 31 - 40
	1260,	1320,	1380,	1440,	1500,	1560,	1620,	1680,	1740,	1800,	// 41 - 50
	1860,	1920,	2000,	2100,	2200,	2300,	2450,	2600,	2750,	2900,	// 51 - 60
	3050,	3200,	3350,	3500,	3650,	3800,	3950,	4100,	4250,	4400,	// 61 - 70
	4600,	4800,	5000,	5200,	5400,	5600,	5800,	6000,	6200,	6400,	// 71 - 80
	6600,	6900,	7100,	7300,	7600,	7800,	8000,	8300,	8500,	8800,	// 81 - 90
	9000,	9000,	9000,	9000,	9000,	9000,	9000,	9000,	9000,	9000,	// 91 - 100
	10000,	10000,	10000,	10000,	10000,	10000,	10000,	10000,	10000,	10000,	// 101 - 110
	12000,	12000,	12000,	12000,	12000,	12000,	12000,	12000,	12000,	12000,	// 111 - 120
	// 14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	// 130
	// 14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	// 140
	// 14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	// 150
	// 14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	// 160
	// 14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	// 170
	// 14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	// 180
	// 14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	// 190
	// 14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	// 200
	// 14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	// 210
	// 14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	// 220
	// 14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	// 230
	// 14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	// 240
	// 14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	14000,	// 250
};

Coord aArroundCoords[ARROUND_COORD_MAX_NUM] =
{
	{	     0,	      0	    },
	{        0,      50     },
	{       35,     35      },
	{       50,     -0      },
	{       35,     -35     },
	{       0,      -50     },
	{       -35,    -35     },
	{       -50,    0       },
	{       -35,    35      },
	{       0,      100     },
	{       71,     71      },
	{       100,    -0      },
	{       71,     -71     },
	{       0,      -100    },
	{       -71,    -71     },
	{       -100,   0       },
	{       -71,    71      },
	{       0,      150     },
	{       106,    106     },
	{       150,    -0      },
	{       106,    -106    },
	{       0,      -150    },
	{       -106,   -106    },
	{       -150,   0       },
	{       -106,   106     },
	{       0,      200     },
	{       141,    141     },
	{       200,    -0      },
	{       141,    -141    },
	{       0,      -200    },
	{       -141,   -141    },
	{       -200,   0       },
	{       -141,   141     },
	{       0,      250     },
	{       177,    177     },
	{       250,    -0      },
	{       177,    -177    },
	{       0,      -250    },
	{       -177,   -177    },
	{       -250,   0       },
	{       -177,   177     },
	{       0,      300     },
	{       212,    212     },
	{       300,    -0      },
	{       212,    -212    },
	{       0,      -300    },
	{       -212,   -212    },
	{       -300,   0       },
	{       -212,   212     },
	{       0,      350     },
	{       247,    247     },
	{       350,    -0      },
	{       247,    -247    },
	{       0,      -350    },
	{       -247,   -247    },
	{       -350,   0       },
	{       -247,   247     },
	{       0,      400     },
	{       283,    283     },
	{       400,    -0      },
	{       283,    -283    },
	{       0,      -400    },
	{       -283,   -283    },
	{       -400,   0       },
	{       -283,   283     },
	{       0,      450     },
	{       318,    318     },
	{       450,    -0      },
	{       318,    -318    },
	{       0,      -450    },
	{       -318,   -318    },
	{       -450,   0       },
	{       -318,   318     },
	{       0,      500     },
	{       354,    354     },
	{       500,    -0      },
	{       354,    -354    },
	{       0,      -500    },
	{       -354,   -354    },
	{       -500,   0       },
	{       -354,   354     },
	{       0,      550     },
	{       389,    389     },
	{       550,    -0      },
	{       389,    -389    },
	{       0,      -550    },
	{       -389,   -389    },
	{       -550,   0       },
	{       -389,   389     },
	{       0,      600     },
	{       424,    424     },
	{       600,    -0      },
	{       424,    -424    },
	{       0,      -600    },
	{       -424,   -424    },
	{       -600,   0       },
	{       -424,   424     },
	{       0,      650     },
	{       460,    460     },
	{       650,    -0      },
	{       460,    -460    },
	{       0,      -650    },
	{       -460,   -460    },
	{       -650,   0       },
	{       -460,   460     },
	{       0,      700     },
	{       495,    495     },
	{       700,    -0      },
	{       495,    -495    },
	{       0,      -700    },
	{       -495,   -495    },
	{       -700,   0       },
	{       -495,   495     },
	{       0,      750     },
	{       530,    530     },
	{       750,    -0      },
	{       530,    -530    },
	{       0,      -750    },
	{       -530,   -530    },
	{       -750,   0       },
	{       -530,   530     },
	{       0,      800     },
	{       566,    566     },
	{       800,    -0      },
	{       566,    -566    },
	{       0,      -800    },
	{       -566,   -566    },
	{       -800,   0       },
	{       -566,   566     },
	{       0,      850     },
	{       601,    601     },
	{       850,    -0      },
	{       601,    -601    },
	{       0,      -850    },
	{       -601,   -601    },
	{       -850,   0       },
	{       -601,   601     },
	{       0,      900     },
	{       636,    636     },
	{       900,    -0      },
	{       636,    -636    },
	{       0,      -900    },
	{       -636,   -636    },
	{       -900,   0       },
	{       -636,   636     },
	{       0,      950     },
	{       672,    672     },
	{       950,    -0      },
	{       672,    -672    },
	{       0,      -950    },
	{       -672,   -672    },
	{       -950,   0       },
	{       -672,   672     },
	{       0,      1000    },
	{       707,    707     },
	{       1000,   -0      },
	{       707,    -707    },
	{       0,      -1000   },
	{       -707,   -707    },
	{       -1000,  0       },
	{       -707,   707     },
};

const DWORD guild_exp_table[GUILD_MAX_LEVEL+1] =
{
	0,
	15000UL,
	45000UL,
	90000UL,
	160000UL,
	235000UL,
	325000UL,
	430000UL,
	550000UL,
	685000UL,
	835000UL,
	1000000UL,
	1500000UL,
	2100000UL,
	2800000UL,
	3600000UL,
	4500000UL,
	6500000UL,
	8000000UL,
	10000000UL,
	42000000UL
};

// INTERNATIONAL_VERSION 길드경험치
const DWORD guild_exp_table2[GUILD_MAX_LEVEL+1] =
{
	0,
	6000UL,
	18000UL,
	36000UL,
	64000UL,
	94000UL,
	130000UL,
	172000UL,
	220000UL,
	274000UL,
	334000UL,
	400000UL,
	600000UL,
	840000UL,
	1120000UL,
	1440000UL,
	1800000UL,
	2600000UL,
	3200000UL,
	4000000UL,
	16800000UL
};
// END_OF_INTERNATIONAL_VERSION 길드경험치

#if defined(ENABLE_NEW_FISH_EVENT)
const uint8_t fishEventPosition1[7][7] = {
	{FISH_EVENT_SHAPE_1, 0, 6, 12, 0, 0, 0},
	{FISH_EVENT_SHAPE_2, 0, 0, 0, 0, 0, 0},
	{FISH_EVENT_SHAPE_3, 0, 6, 7, 0, 0, 0},
	{FISH_EVENT_SHAPE_4, 0, 1, 7, 0, 0, 0},
	{FISH_EVENT_SHAPE_5, 0, 1, 6, 7, 0, 0},
	{FISH_EVENT_SHAPE_6, 0, 1, 7, 8, 0, 0},
	{FISH_EVENT_SHAPE_7, 0, 1, 2, 6, 7, 8},
};

const uint8_t fishEventPosition2[7][8] = {
	{FISH_EVENT_SHAPE_1, 12, 0, 6, 12, 0, 0, 0},
	{FISH_EVENT_SHAPE_2, 24, 0, 0, 0, 0, 0, 0},
	{FISH_EVENT_SHAPE_3, 17, 0, 6, 7, 0, 0, 0},
	{FISH_EVENT_SHAPE_4, 17, 0, 1, 7, 0, 0, 0},
	{FISH_EVENT_SHAPE_5, 17, 0, 1, 6, 7, 0, 0},
	{FISH_EVENT_SHAPE_6, 16, 0, 1, 7, 8, 0, 0},
	{FISH_EVENT_SHAPE_7, 16, 0, 1, 2, 6, 7, 8},
};
#endif

#if defined(ENABLE_LETTERS_EVENT)
const uint32_t lettersReward[2] = {50011, 10};
#endif

const int aiMobEnchantApplyIdx[MOB_ENCHANTS_MAX_NUM] =
{
	APPLY_CURSE_PCT,
	APPLY_SLOW_PCT,
	APPLY_POISON_PCT,
	APPLY_STUN_PCT,
	APPLY_CRITICAL_PCT,
	APPLY_PENETRATE_PCT,
};

const int aiMobResistsApplyIdx[MOB_RESISTS_MAX_NUM] =
{
	APPLY_RESIST_SWORD,
	APPLY_RESIST_TWOHAND,
	APPLY_RESIST_DAGGER,
	APPLY_RESIST_BELL,
	APPLY_RESIST_FAN,
	APPLY_RESIST_BOW,
	APPLY_RESIST_FIRE,
	APPLY_RESIST_ELEC,
	APPLY_RESIST_MAGIC,
	APPLY_RESIST_WIND,
	APPLY_POISON_REDUCE,
};

TItemAttrMap g_map_itemAttr;
TItemAttrMap g_map_itemRare;

const TApplyInfo aApplyInfo[MAX_APPLY_NUM] =
/*
{
   DWORD dwPointType;
}
 */
{
	// Point Type
	{ POINT_NONE,			},   // APPLY_NONE,		0
	{ POINT_MAX_HP,		        },   // APPLY_MAX_HP,		1
	{ POINT_MAX_SP,		        },   // APPLY_MAX_SP,		2
	{ POINT_HT,			        },   // APPLY_CON,		3
	{ POINT_IQ,			        },   // APPLY_INT,		4
	{ POINT_ST,			        },   // APPLY_STR,		5
	{ POINT_DX,			        },   // APPLY_DEX,		6
	{ POINT_ATT_SPEED,		        },   // APPLY_ATT_SPEED,	7
	{ POINT_MOV_SPEED,		        },   // APPLY_MOV_SPEED,	8
	{ POINT_CASTING_SPEED,	        },   // APPLY_CAST_SPEED,	9
	{ POINT_HP_REGEN,			},   // APPLY_HP_REGEN,		10
	{ POINT_SP_REGEN,			},   // APPLY_SP_REGEN,		11
	{ POINT_POISON_PCT,		        },   // APPLY_POISON_PCT,	12
	{ POINT_STUN_PCT,		        },   // APPLY_STUN_PCT,		13
	{ POINT_SLOW_PCT,		        },   // APPLY_SLOW_PCT,		14
	{ POINT_CRITICAL_PCT,		},   // APPLY_CRITICAL_PCT,	15
	{ POINT_PENETRATE_PCT,	        },   // APPLY_PENETRATE_PCT,	16
	{ POINT_ATTBONUS_HUMAN,	        },   // APPLY_ATTBONUS_HUMAN,	17
	{ POINT_ATTBONUS_ANIMAL,	        },   // APPLY_ATTBONUS_ANIMAL,	18
	{ POINT_ATTBONUS_ORC,		},   // APPLY_ATTBONUS_ORC,	19
	{ POINT_ATTBONUS_MILGYO,	        },   // APPLY_ATTBONUS_MILGYO,	20
	{ POINT_ATTBONUS_UNDEAD,	        },   // APPLY_ATTBONUS_UNDEAD,	21
	{ POINT_ATTBONUS_DEVIL,	        },   // APPLY_ATTBONUS_DEVIL,	22
	{ POINT_STEAL_HP,		        },   // APPLY_STEAL_HP,		23
	{ POINT_STEAL_SP,		        },   // APPLY_STEAL_SP,		24
	{ POINT_MANA_BURN_PCT,	        },   // APPLY_MANA_BURN_PCT,	25
	{ POINT_DAMAGE_SP_RECOVER,	        },   // APPLY_DAMAGE_SP_RECOVER,26
	{ POINT_BLOCK,		        },   // APPLY_BLOCK,		27
	{ POINT_DODGE,		        },   // APPLY_DODGE,		28
	{ POINT_RESIST_SWORD,		},   // APPLY_RESIST_SWORD,	29
	{ POINT_RESIST_TWOHAND,	        },   // APPLY_RESIST_TWOHAND,	30
	{ POINT_RESIST_DAGGER,	        },   // APPLY_RESIST_DAGGER,	31
	{ POINT_RESIST_BELL,		},   // APPLY_RESIST_BELL,	32
	{ POINT_RESIST_FAN,		        },   // APPLY_RESIST_FAN,	33
	{ POINT_RESIST_BOW,		        },   // APPLY_RESIST_BOW,	34
	{ POINT_RESIST_FIRE,		},   // APPLY_RESIST_FIRE,	35
	{ POINT_RESIST_ELEC,		},   // APPLY_RESIST_ELEC,	36
	{ POINT_RESIST_MAGIC,		},   // APPLY_RESIST_MAGIC,	37
	{ POINT_RESIST_WIND,		},   // APPLY_RESIST_WIND,	38
	{ POINT_REFLECT_MELEE,	        },   // APPLY_REFLECT_MELEE,	39
	{ POINT_REFLECT_CURSE,	        },   // APPLY_REFLECT_CURSE,	40
	{ POINT_POISON_REDUCE,	        },   // APPLY_POISON_REDUCE,	41
	{ POINT_KILL_SP_RECOVER,	        },   // APPLY_KILL_SP_RECOVER,	42
	{ POINT_EXP_DOUBLE_BONUS,	        },   // APPLY_EXP_DOUBLE_BONUS,	43
	{ POINT_GOLD_DOUBLE_BONUS,	        },   // APPLY_GOLD_DOUBLE_BONUS,44
	{ POINT_ITEM_DROP_BONUS,	        },   // APPLY_ITEM_DROP_BONUS,	45
	{ POINT_POTION_BONUS,		},   // APPLY_POTION_BONUS,	46
	{ POINT_KILL_HP_RECOVERY,	        },   // APPLY_KILL_HP_RECOVER,	47
	{ POINT_IMMUNE_STUN,		},   // APPLY_IMMUNE_STUN,	48
	{ POINT_IMMUNE_SLOW,		},   // APPLY_IMMUNE_SLOW,	49
	{ POINT_IMMUNE_FALL,		},   // APPLY_IMMUNE_FALL,	50
	{ POINT_NONE,			},   // APPLY_SKILL,		51
	{ POINT_BOW_DISTANCE,		},   // APPLY_BOW_DISTANCE,	52
	{ POINT_ATT_GRADE_BONUS,	        },   // APPLY_ATT_GRADE,	53
	{ POINT_DEF_GRADE_BONUS,	        },   // APPLY_DEF_GRADE,	54
	{ POINT_MAGIC_ATT_GRADE_BONUS,      },   // APPLY_MAGIC_ATT_GRADE,	55
	{ POINT_MAGIC_DEF_GRADE_BONUS,      },   // APPLY_MAGIC_DEF_GRADE,	56
	{ POINT_CURSE_PCT,			},   // APPLY_CURSE_PCT,	57
	{ POINT_MAX_STAMINA			},   // APPLY_MAX_STAMINA	58
	{ POINT_ATTBONUS_WARRIOR		},   // APPLY_ATTBONUS_WARRIOR  59
	{ POINT_ATTBONUS_ASSASSIN		},   // APPLY_ATTBONUS_ASSASSIN 60
	{ POINT_ATTBONUS_SURA		},   // APPLY_ATTBONUS_SURA     61
	{ POINT_ATTBONUS_SHAMAN		},   // APPLY_ATTBONUS_SHAMAN   62
	{ POINT_ATTBONUS_MONSTER		},   //	APPLY_ATTBONUS_MONSTER  63
	{ POINT_ATT_BONUS		},   // 64 // APPLY_MALL_ATTBONUS
	{ POINT_MALL_DEFBONUS		},   // 65
	{ POINT_MALL_EXPBONUS		},   // 66 APPLY_MALL_EXPBONUS
	{ POINT_MALL_ITEMBONUS		},   // 67
	{ POINT_MALL_GOLDBONUS		},   // 68
	{ POINT_MAX_HP_PCT			},		// 69
	{ POINT_MAX_SP_PCT			},		// 70
	{ POINT_SKILL_DAMAGE_BONUS		},	// 71
	{ POINT_NORMAL_HIT_DAMAGE_BONUS	},	// 72

	// DEFEND_BONUS_ATTRIBUTES
	{ POINT_SKILL_DEFEND_BONUS		},	// 73
	{ POINT_NORMAL_HIT_DEFEND_BONUS	},	// 74
	// END_OF_DEFEND_BONUS_ATTRIBUTES

	{ POINT_PC_UNUSED01	},		// 75
	{ POINT_PC_UNUSED02	},		// 76

	{ POINT_NONE,		},				// 77 사용시 HP 소모 APPLY_EXTRACT_HP_PCT

	{ POINT_RESIST_WARRIOR,		},		// 78 무사에게 저항 APPLY_RESIST_WARRIOR
	{ POINT_RESIST_ASSASSIN,	},		// 79 자객에게 저항 APPLY_RESIST_ASSASSIN
	{ POINT_RESIST_SURA,		},		// 80 수라에게 저항 APPLY_RESIST_SURA
	{ POINT_RESIST_SHAMAN,		},		// 81 무당에게 저항 APPLY_RESIST_SHAMAN
	{ POINT_ENERGY				},		// 82 기력
	{ POINT_DEF_GRADE			},		// 83 방어력. DEF_GRADE_BONUS는 클라에서 두배로 보여지는 의도된 버그(...)가 있다.
	{ POINT_COSTUME_ATTR_BONUS	},		// 84 코스튬에 붙은 속성에 대해서만 보너스를 주는 기력
	{ POINT_MAGIC_ATT_BONUS_PER },			// 85 마법 공격력 +x%
	{ POINT_MELEE_MAGIC_ATT_BONUS_PER		},			// 86 APPLY_MELEE_MAGIC_ATTBONUS_PER
	{ POINT_RESIST_ICE,			},   // APPLY_RESIST_ICE,	87
	{ POINT_RESIST_EARTH,		},   // APPLY_RESIST_EARTH,	88
	{ POINT_RESIST_DARK,		},   // APPLY_RESIST_DARK,	89
	{ POINT_RESIST_CRITICAL,		},   // APPLY_ANTI_CRITICAL_PCT,	90
	{ POINT_RESIST_PENETRATE,		},   // APPLY_ANTI_PENETRATE_PCT,	91
	{POINT_NONE,},						// 92
	{POINT_NONE,},						// 93
	{POINT_NONE,},						// 94
	{POINT_NONE,},						// 95
	{POINT_NONE,},						// 96
#ifdef ELEMENT_NEW_BONUSES
#ifdef ENABLE_NEW_BONUS_TALISMAN
	{POINT_ATTBONUS_IRR_SPADA,},		// 97
	{POINT_ATTBONUS_IRR_SPADONE,},		// 98
	{POINT_ATTBONUS_IRR_PUGNALE,},		// 99
	{POINT_ATTBONUS_IRR_FRECCIA,},		// 100
	{POINT_ATTBONUS_IRR_VENTAGLIO,},	// 101
	{POINT_ATTBONUS_IRR_CAMPANA,},		// 102
	{POINT_RESIST_MEZZIUOMINI,},		// 103
	{POINT_DEF_TALISMAN,},				// 104
	{POINT_ATTBONUS_INSECT,},			// 105
	{POINT_ATTBONUS_DESERT,},			// 106
	{POINT_ATTBONUS_FORT_ZODIAC,},		// 107
#else
	{POINT_NONE,},						// 97
	{POINT_NONE,},						// 98
	{POINT_NONE,},						// 99
	{POINT_NONE,},						// 100
	{POINT_NONE,},						// 101
	{POINT_NONE,},						// 102
	{POINT_NONE,},						// 103
	{POINT_NONE,},						// 104
	{POINT_NONE,},						// 105
	{POINT_NONE,},						// 106
	{POINT_NONE,},						// 107
#endif
	{POINT_ATTBONUS_ELEC,},				// 108
	{POINT_ATTBONUS_FIRE,},				// 109
	{POINT_ATTBONUS_ICE,},				// 110
	{POINT_ATTBONUS_WIND,},				// 111
	{POINT_ATTBONUS_EARTH,},			// 112
	{POINT_ATTBONUS_DARK,},				// 113
#else
	{POINT_NONE,},						// 108
	{POINT_NONE,},						// 109
	{POINT_NONE,},						// 110
	{POINT_NONE,},						// 111
	{POINT_NONE,},						// 112
	{POINT_NONE,},						// 113
#endif
#ifdef ENABLE_ACCE_SYSTEM
	{POINT_ACCEDRAIN_RATE,},			// 114
#else
	{POINT_NONE,},						// 114
#endif
#ifdef ENABLE_APPLY_MAGICRES_REDUCTION
	{POINT_RESIST_MAGIC_REDUCTION,},	// 115
#else
	{POINT_NONE,},						// 115
#endif
#ifdef ENABLE_STRONG_METIN
	{POINT_ATTBONUS_METIN,},			// 116
#else
	{POINT_NONE,},						// 116
#endif
#ifdef ENABLE_STRONG_BOSS
	{POINT_ATTBONUS_BOSS,},				// 117
#else
	{POINT_NONE,},						// 117
#endif
#ifdef ENABLE_RESIST_MONSTER
	{POINT_RESIST_MONSTER,},			// 118
#else
	{POINT_NONE,},						// 118
#endif
#ifdef ENABLE_MEDI_PVM
	{POINT_ATTBONUS_MEDI_PVM,},			// 119
#else
	{POINT_NONE,},						// 119
#endif
	{POINT_PVM_CRITICAL_PCT,},			// 120
#ifdef ENABLE_DS_RUNE
	{POINT_RUNE_MONSTERS,},				// 121
#else
	{POINT_NONE,},						// 121
#endif
#ifdef ENABLE_NEW_COMMON_BONUSES
	{POINT_DOUBLE_DROP_ITEM,},			// 122
	{POINT_IRR_WEAPON_DEFENSE,},		// 123
#else
	{POINT_NONE,},						// 122
	{POINT_NONE,},						// 123
#endif
	{POINT_NONE,},						// 124
	{POINT_FISHING_RARE,},				// 125
#ifdef ENABLE_NEW_USE_POTION
	{POINT_PARTY_DROPEXP,},				// 126
#else
	{POINT_NONE,},						// 126
#endif
#if defined(USE_MOUNT_COSTUME_SYSTEM)
	{POINT_MOUNT,},						// 127
#else
	{POINT_NONE,},						// 127
#endif
	{POINT_ATT_RACES,},					// 128
	{POINT_RES_RACES,},					// 129
#if defined(ENABLE_PETSKIN)
	{POINT_PETSKIN,},					// 130
#else
	{POINT_NONE,},						// 130
#endif
#if defined(ENABLE_MOUNTSKIN)
	{POINT_MOUNTSKIN,},					// 131
#else
	{POINT_NONE,},						// 131
#endif
#if defined(ENABLE_HALLOWEEN_EVENT_2022) && !defined(USE_NO_HALLOWEEN_EVENT_2022_BONUSES)
    {POINT_FEAR,},                      // 132
    {POINT_RESISTANCE_FEAR,},           // 133
#else
    {POINT_NONE,},                      // 132
    {POINT_NONE,},                      // 133
#endif
#if defined(USE_ANTI_EXP)
	{POINT_ANTI_EXP,},					// 134
#else
	{POINT_NONE,},						// 134
#endif
};

const int aiItemMagicAttributePercentHigh[ITEM_ATTRIBUTE_MAX_LEVEL] =
{
	//25, 25, 40, 8, 2,
	30, 40, 20, 8, 2
};

const int aiItemMagicAttributePercentLow[ITEM_ATTRIBUTE_MAX_LEVEL] =
{
	//45, 25, 20, 10, 0,
	50, 40, 10, 0, 0
};

// ADD_ITEM_ATTRIBUTE
const int aiItemAttributeAddPercent[ITEM_ATTRIBUTE_MAX_NUM] =
{
	100, 80, 60, 50, 30, 0, 0,
};
// END_OF_ADD_ITEM_ATTRIBUTE

const int aiExpLossPercents[PLAYER_EXP_TABLE_MAX + 1] =
{
	0,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 4, // 1 - 10
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, // 11 - 20
	4, 4, 4, 4, 4, 4, 4, 3, 3, 3, // 21 - 30
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // 31 - 40
	3, 3, 3, 3, 2, 2, 2, 2, 2, 2, // 41 - 50
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // 51 - 60
	2, 2, 1, 1, 1, 1, 1, 1, 1, 1, // 61 - 70
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 71 - 80
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 81 - 90
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 91 - 100
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 101 - 110
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 111 - 120
	// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 130
	// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 140
	// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 150
	// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 160
	// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 170
	// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 180
	// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 190
	// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 200
	// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 210
	// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 220
	// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 230
	// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 240
	// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 250
};

const int aiSkillBookCountForLevelUp[10] =
{
	3, 3, 3, 3, 3, 4, 4, 5, 5, 6
};

// ADD_GRANDMASTER_SKILL
const int aiGrandMasterSkillBookCountForLevelUp[10] =
{
	3, 3, 5, 5, 7, 7, 10, 10, 10, 20,
};

const int aiGrandMasterSkillBookMinCount[10] =
{
//	1, 1, 3, 5, 10, 15, 20, 30, 40, 50,
//	3, 3, 5, 5, 10, 10, 15, 15, 20, 30
	1, 1, 1, 2,  2,  3,  3,  4,  5,  6
};

const int aiGrandMasterSkillBookMaxCount[10] =
{
//	6, 15, 30, 45, 60, 80, 100, 120, 160, 200,
//	6, 10, 15, 20, 30, 40, 50, 60, 70, 80
	5,  7,  9, 11, 13, 15, 20, 25, 30, 35
};
// END_OF_ADD_GRANDMASTER_SKILL

const int CHN_aiPartyBonusExpPercentByMemberCount[9] =
{
	0, 0, 12, 18, 26, 40, 53, 70, 100
};


// UPGRADE_PARTY_BONUS
const int KOR_aiPartyBonusExpPercentByMemberCount[9] =
{
	0,
	0,
	30, // 66% * 2 - 100
	60, // 53% * 3 - 100
	75, // 44% * 4 - 100
	90, // 38% * 5 - 100
	105, // 34% * 6 - 100
	110, // 30% * 7 - 100
	140, // 30% * 8 - 100
};

const int KOR_aiUniqueItemPartyBonusExpPercentByMemberCount[9] =
{
	0,
	0,
	15*2,
	14*3,
	13*4,
	12*5,
	11*6,
	10*7,
	10*8,
};
// END_OF_UPGRADE_PARTY_BONUS

const int * aiChainLightningCountBySkillLevel = NULL;

const int aiChainLightningCountBySkillLevel_euckr[SKILL_MAX_LEVEL+1] =
{
	0,	// 0
	2,	// 1
	2,	// 2
	2,	// 3
	2,	// 4
	2,	// 5
	2,	// 6
	2,	// 7
	2,	// 8
	3,	// 9
	3,	// 10
	3,	// 11
	3,	// 12
	3,	// 13
	3,	// 14
	3,	// 15
	3,	// 16
	3,	// 17
	3,	// 18
	4,	// 19
	4,	// 20
	4,	// 21
	4,	// 22
	4,	// 23
	5,	// 24
	5,	// 25
	5,	// 26
	5,	// 27
	5,	// 28
	5,	// 29
	5,	// 30
	5,	// 31
	5,	// 32
	5,	// 33
	5,	// 34
	5,	// 35
	5,	// 36
	5,	// 37
	5,	// 38
	5,	// 39
	5,	// 40
};

const SStoneDropInfo aStoneDrop[STONE_INFO_MAX_NUM] =
{
	// mob	pct	{+0	+1	+2	+3	+4}
	{8001,	60,	{35,35,	30,	0,	0}	},
	{8002,	60,	{35,35,	30,	0,	0}	},
	{8003,	60,	{34,34,	32,	0,	0}	},
	{8004,	60,	{34,34,	32,	0,	0}	},
	{8005,	60,	{22,28,	30,	20,	0}	},
	{8006,	60,	{22,28,	30,	20,	0}	},
	{8007,	60,	{22,28,	30,	20,	0}	},
	{8008,	60,	{12,24,	32,	26,	6}	},
	{8009,	60,	{12,24,	32,	26,	6}	},
	{8010,	60,	{12,24,	32,	26,	6}	},
	{8011,	60,	{14,20,	28,	28,	10}	},
	{8013,	60,	{14,20,	28,	28,	10}	},
	{8014,	60,	{14,20,	28,	28,	10}	},
	{8024,	60,	{14,20,	26,	28,	12}	},
	{8025,	60,	{14,20,	26,	28,	12}	},
	{8026,	60,	{14,20,	26,	28,	12}	},
	{8027,	60,	{14,20,	26,	28,	12}	},
	{8051,	60,	{5,	27,	40,	22,	6}	},
	{8052,	60,	{5,	27,	40,	22,	6}	},
	{8053,	60,	{5,	27,	40,	22,	6}	},
	{8054,	60,	{5,	27,	40,	22,	6}	},
	{8055,	60,	{5,	27,	40,	22,	6}	},
	{8056,	60,	{5,	27,	40,	22,	6}	},
	{8059,	60,	{5,	27,	40,	22,	6}	},
	{8061,	60,	{5,	27,	40,	22,	6}	},
};

const SStofaDropInfo aStofaDrop[STONE_STOFA_INFO_MAX_NUM] =
{
/*  metinVnum		chance80019	chance80022	chance80023	chance80024 chance80025	chance80026	chance80027 */
	{8024,			{55,			55,			55,			55,			55,			55,			55}},
	{8025,			{55,			55,			55,			55,			55,			55,			55}},
	{8026,			{55,			55,			55,			55,			55,			55,			55}},
	{8027,			{55,			55,			55,			55,			55,			55,			55}},
	{8051,			{55,			55,			55,			55,			55,			55,			55}},
	{8052,			{55,			55,			55,			55,			55,			55,			55}},
	{8053,			{55,			55,			55,			55,			55,			55,			55}},
	{8054,			{55,			55,			55,			55,			55,			55,			55}},
	{8055,			{55,			55,			55,			55,			55,			55,			55}},
	{8056,			{55,			55,			55,			55,			55,			55,			55}},
	{8059,			{55,			55,			55,			55,			55,			55,			55}},
	{8061,			{55,			55,			55,			55,			55,			55,			55}},
};

const char * c_apszEmpireNames[EMPIRE_MAX_NUM] =
{
	"Shinsoo, Chunjo, Jinno",
	"Shinsoo",
	"Chunjo",
	"Jinno"
};

const char * c_apszPrivNames[MAX_PRIV_NUM] =
{
	"",
	"item",
	"yang",
	"x10 yang",
	"exp"
};

std::string m_closeText[LANGUAGE_MAX_NUM] =
{
	"Close",
	"Close",
	"Inchide",
	"Chiudi",
	"Kapat",
	"Schließen",
	"Blisko",
	"Perto",
	"Cerca",
	"Zavit",
	"Bezaras",
};

std::string m_horseText[LANGUAGE_MAX_NUM] =
{
	" Horse ",
	" Horse ",
	" Cal ",
	" Cavallo ",
	" At ",
	" Pferd ",
	" Ko? ",
	" Egua ",
	" Caballo ",
	" Ko ",
	" Lo ",
};

const int aiPolymorphPowerByLevel[SKILL_MAX_LEVEL + 1] =
{
	10,   // 1
	11,   // 2
	11,   // 3
	12,   // 4
	13,   // 5
	13,   // 6
	14,   // 7
	15,   // 8
	16,   // 9
	17,   // 10
	18,   // 11
	19,   // 12
	20,   // 13
	22,   // 14
	23,   // 15
	24,   // 16
	26,   // 17
	27,   // 18
	29,   // 19
	31,   // 20
	33,   // 21
	35,   // 22
	37,   // 23
	39,   // 24
	41,   // 25
	44,   // 26
	46,   // 27
	49,   // 28
	52,   // 29
	55,   // 30
	59,   // 31
	62,   // 32
	66,   // 33
	70,   // 34
	74,   // 35
	79,   // 36
	84,   // 37
	89,   // 38
	94,   // 39
	100,  // 40
};

TGuildWarInfo KOR_aGuildWarInfo[GUILD_WAR_TYPE_MAX_NUM] =
/*
   {
   long lMapIndex;
   int iWarPrice;
   int iWinnerPotionRewardPctToWinner;
   int iLoserPotionRewardPctToWinner;
   int iInitialScore;
   int iEndScore;
   };
 */
{
	{ 0,        0,      0,      0,      0,      0       },
	{ 110,      0,      100,    50,     0,      100     },
	{ 111,      0,      100,    50,     0,      10      },
};

//
// 악세서리 소켓용 수치들
//

// 다이아몬드로 소켓을 추가할 때 확률
const int aiAccessorySocketAddPct[ITEM_ACCESSORY_SOCKET_MAX_NUM] =
{
	50, 50, 50
};

// 악세서리 수치 값의 몇%만큼의 성능을 추가하는지
const int aiAccessorySocketEffectivePct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1] =
{
	0, 10, 20, 40
};

// 소켓 지속시간 24, 12, 6
const int aiAccessorySocketDegradeTime[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1] =
{
	0, 3600 * 24, 3600 * 12, 3600 * 6
};

// 소켓 장착 성공률
const int aiAccessorySocketPutPct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1] =
{
	70, 70, 70, 0
};
// END_OF_ACCESSORY_REFINE

#include "../common/length.h"
// from import_item_proto.c
typedef struct SValueName
{
	const char *	c_pszName;
	long		lValue;
} TValueName;

TValueName c_aApplyTypeNames[] =
{
	{"STR", APPLY_STR},
	{"DEX", APPLY_DEX},
	{"CON", APPLY_CON},
	{"INT", APPLY_INT},
	{"MAX_HP", APPLY_MAX_HP},
	{"MAX_SP", APPLY_MAX_SP},
	{"MAX_STAMINA", APPLY_MAX_STAMINA},
	{"POISON_REDUCE", APPLY_POISON_REDUCE},
	{"EXP_DOUBLE_BONUS", APPLY_EXP_DOUBLE_BONUS},
	{"GOLD_DOUBLE_BONUS", APPLY_GOLD_DOUBLE_BONUS},
	{"ITEM_DROP_BONUS", APPLY_ITEM_DROP_BONUS},
	{"HP_REGEN", APPLY_HP_REGEN},
	{"SP_REGEN", APPLY_SP_REGEN},
	{"ATTACK_SPEED", APPLY_ATT_SPEED},
	{"MOVE_SPEED", APPLY_MOV_SPEED},
	{"CAST_SPEED", APPLY_CAST_SPEED},
	{"ATT_BONUS", APPLY_ATT_GRADE_BONUS},
	{"DEF_BONUS", APPLY_DEF_GRADE_BONUS},
	{"MAGIC_ATT_GRADE", APPLY_MAGIC_ATT_GRADE},
	{"MAGIC_DEF_GRADE", APPLY_MAGIC_DEF_GRADE},
	{"SKILL", APPLY_SKILL},
	{"ATTBONUS_ANIMAL", APPLY_ATTBONUS_ANIMAL},
	{"ATTBONUS_UNDEAD", APPLY_ATTBONUS_UNDEAD},
	{"ATTBONUS_DEVIL", APPLY_ATTBONUS_DEVIL},
	{"ATTBONUS_HUMAN", APPLY_ATTBONUS_HUMAN},
	{"ADD_BOW_DISTANCE", APPLY_BOW_DISTANCE},
	{"DODGE", APPLY_DODGE},
	{"BLOCK", APPLY_BLOCK},
	{"RESIST_SWORD", APPLY_RESIST_SWORD},
	{"RESIST_TWOHAND", APPLY_RESIST_TWOHAND},
	{"RESIST_DAGGER", APPLY_RESIST_DAGGER},
	{"RESIST_BELL", APPLY_RESIST_BELL},
	{"RESIST_FAN", APPLY_RESIST_FAN},
	{"RESIST_BOW", APPLY_RESIST_BOW},
	{"RESIST_FIRE", APPLY_RESIST_FIRE},
	{"RESIST_ELEC", APPLY_RESIST_ELEC},
	{"RESIST_MAGIC", APPLY_RESIST_MAGIC},
	{"RESIST_WIND", APPLY_RESIST_WIND},
	{"REFLECT_MELEE", APPLY_REFLECT_MELEE},
	{"REFLECT_CURSE", APPLY_REFLECT_CURSE},
	{"RESIST_ICE", APPLY_RESIST_ICE},
	{"RESIST_EARTH", APPLY_RESIST_EARTH},
	{"RESIST_DARK", APPLY_RESIST_DARK},
	{"RESIST_CRITICAL", APPLY_ANTI_CRITICAL_PCT},
	{"RESIST_PENETRATE", APPLY_ANTI_PENETRATE_PCT},
	{"POISON", APPLY_POISON_PCT},
	{"SLOW", APPLY_SLOW_PCT},
	{"STUN", APPLY_STUN_PCT},
	{"STEAL_HP", APPLY_STEAL_HP},
	{"STEAL_SP", APPLY_STEAL_SP},
	{"MANA_BURN_PCT", APPLY_MANA_BURN_PCT},
	{"CRITICAL", APPLY_CRITICAL_PCT},
	{"PENETRATE", APPLY_PENETRATE_PCT},
	{"KILL_SP_RECOVER", APPLY_KILL_SP_RECOVER},
	{"KILL_HP_RECOVER", APPLY_KILL_HP_RECOVER},
	{"PENETRATE_PCT", APPLY_PENETRATE_PCT},
	{"CRITICAL_PCT", APPLY_CRITICAL_PCT},
	{"POISON_PCT", APPLY_POISON_PCT},
	{"STUN_PCT", APPLY_STUN_PCT},
	{"ATT_BONUS_TO_WARRIOR", APPLY_ATTBONUS_WARRIOR},
	{"ATT_BONUS_TO_ASSASSIN", APPLY_ATTBONUS_ASSASSIN},
	{"ATT_BONUS_TO_SURA", APPLY_ATTBONUS_SURA},
	{"ATT_BONUS_TO_SHAMAN", APPLY_ATTBONUS_SHAMAN},
	{"ATT_BONUS_TO_MONSTER", APPLY_ATTBONUS_MONSTER},
	{"ATT_BONUS_TO_MOB", APPLY_ATTBONUS_MONSTER},
	{"MALL_ATTBONUS", APPLY_MALL_ATTBONUS},
	{"MALL_EXPBONUS", APPLY_MALL_EXPBONUS},
	{"MALL_DEFBONUS", APPLY_MALL_DEFBONUS},
	{"MALL_ITEMBONUS", APPLY_MALL_ITEMBONUS},
	{"MALL_GOLDBONUS", APPLY_MALL_GOLDBONUS},
	{"MAX_HP_PCT", APPLY_MAX_HP_PCT},
	{"MAX_SP_PCT", APPLY_MAX_SP_PCT},
	{"SKILL_DAMAGE_BONUS", APPLY_SKILL_DAMAGE_BONUS},
	{"NORMAL_HIT_DAMAGE_BONUS", APPLY_NORMAL_HIT_DAMAGE_BONUS},
	{"SKILL_DEFEND_BONUS", APPLY_SKILL_DEFEND_BONUS},
	{"NORMAL_HIT_DEFEND_BONUS",APPLY_NORMAL_HIT_DEFEND_BONUS},
	{"RESIST_WARRIOR", APPLY_RESIST_WARRIOR},
	{"RESIST_ASSASSIN", APPLY_RESIST_ASSASSIN},
	{"RESIST_SURA", APPLY_RESIST_SURA},
	{"RESIST_SHAMAN", APPLY_RESIST_SHAMAN},
	{"ENERGY", APPLY_ENERGY},
	{"COSTUME_ATTR_BONUS", APPLY_COSTUME_ATTR_BONUS},
	{"MAGIC_ATTBONUS_PER", APPLY_MAGIC_ATTBONUS_PER},
	{"MELEE_MAGIC_ATTBONUS_PER", APPLY_MELEE_MAGIC_ATTBONUS_PER},
#ifdef ENABLE_ACCE_SYSTEM
	{ "ACCEDRAIN_RATE", APPLY_ACCEDRAIN_RATE},
#endif
#ifdef ELEMENT_NEW_BONUSES
	{"ATTBONUS_ELEC", APPLY_ATTBONUS_ELEC},
	{"ATTBONUS_FIRE", APPLY_ATTBONUS_FIRE},
	{"ATTBONUS_ICE", APPLY_ATTBONUS_ICE},
	{"ATTBONUS_WIND", APPLY_ATTBONUS_WIND},
	{"ATTBONUS_EARTH", APPLY_ATTBONUS_EARTH},
	{"ATTBONUS_DARK", APPLY_ATTBONUS_DARK},
#endif
#ifdef ENABLE_NEW_BONUS_TALISMAN
	{"ATTBONUS_IRR_SPADA", APPLY_ATTBONUS_IRR_SPADA},
	{"ATTBONUS_IRR_SPADONE", APPLY_ATTBONUS_IRR_SPADONE},
	{"ATTBONUS_IRR_PUGNALE", APPLY_ATTBONUS_IRR_PUGNALE},
	{"ATTBONUS_IRR_FRECCIA", APPLY_ATTBONUS_IRR_FRECCIA},
	{"ATTBONUS_IRR_VENTAGLIO", APPLY_ATTBONUS_IRR_VENTAGLIO},
	{"ATTBONUS_IRR_CAMPANA", APPLY_ATTBONUS_IRR_CAMPANA},
	{"RESIST_MEZZIUOMINI", APPLY_RESIST_MEZZIUOMINI},
	{"DEF_TALISMAN", APPLY_DEF_TALISMAN},
	{"ATTBONUS_INSECT", APPLY_ATTBONUS_INSECT},
	{"ATTBONUS_DESERT", APPLY_ATTBONUS_DESERT},
	{"ATTBONUS_FORT_ZODIAC", APPLY_ATTBONUS_FORT_ZODIAC},
#endif
#ifdef ENABLE_STRONG_METIN
	{ "ATTBONUS_METIN", APPLY_ATTBONUS_METIN},
#endif
#ifdef ENABLE_STRONG_BOSS
	{ "ATTBONUS_BOSS", APPLY_ATTBONUS_BOSS},
#endif
#ifdef ENABLE_RESIST_MONSTER
	{ "RESIST_MONSTER", APPLY_RESIST_MONSTER},
#endif
#ifdef ENABLE_MEDI_PVM
	{ "DANNIMEDI_PVM", APPLY_ATTBONUS_MEDI_PVM},
#endif
	{"PVM_CRITICAL_PCT", APPLY_PVM_CRITICAL_PCT},
#ifdef ENABLE_DS_RUNE
	{"ATTBONUS_RUNE", APPLY_RUNE_MONSTERS},
#endif
#ifdef ENABLE_NEW_COMMON_BONUSES
	{"IRR_WEAPON_DEFENSE", APPLY_IRR_WEAPON_DEFENSE},
#endif
	{"EXP_DOUBLE_BONUS", APPLY_EXP_DOUBLE_BONUS},
	{"GOLD_DOUBLE_BONUS", APPLY_GOLD_DOUBLE_BONUS},
	{"APPLY_ITEM_DROP_BONUS", APPLY_ITEM_DROP_BONUS},
	{"ATT_RACES", APPLY_ATT_RACES},
	{"RES_RACES", APPLY_RES_RACES},
#if defined(ENABLE_HALLOWEEN_EVENT_2022) && !defined(USE_NO_HALLOWEEN_EVENT_2022_BONUSES)
    {"FEAR",                    APPLY_FEAR},
    {"RESISTANCE_FEAR",         APPLY_RESISTANCE_FEAR},
#else
    {"FEAR",                    APPLY_UNUSED132},
    {"RESISTANCE_FEAR",         APPLY_UNUSED133},
#endif
#if defined(USE_ANTI_EXP)
	{"ANTI_EXP", APPLY_ANTI_EXP},
#endif
	{"INFINITE_AFFECT_DURATION", 0x1FFFFFFF},
	{NULL, 0}
};

long FN_get_apply_type(const char *apply_type_string)
{
	TValueName	*value_name;
	for (value_name = c_aApplyTypeNames; value_name->c_pszName; ++value_name)
	{
		if (0==strcasecmp(value_name->c_pszName, apply_type_string))
			return value_name->lValue;
	}
	return 0;
}

#if defined(ENABLE_AURA_SYSTEM)
#if defined(ENABLE_NEW_GOLD_LIMIT)
const uint64_t s_aiAuraRefineInfo[AURA_GRADE_MAX_NUM][AURA_REFINE_INFO_MAX] = 
#else
const uint32_t s_aiAuraRefineInfo[AURA_GRADE_MAX_NUM][AURA_REFINE_INFO_MAX] = 
#endif
{
//  [ Grade | LevelMin | LevelMax | NeedEXP | EvolMaterial | EvolMaterialCount | EvolCost | EvolPCT ]
	{1,       1,         49,        250,      59996,         20,                 50000000000, 100},
	{2,       50,        99,        500,      59996,         40,                 100000000000, 100},
	{3,       100,       149,       1000,     59996,         60,                 200000000000, 100},
	{4,       150,       199,       2000,     59996,         80,                 300000000000, 100},
	{5,       200,       249,       4000,     59996,         100,                 400000000000, 100},
	{6,       250,       250,       0,        0,             0,                  0,            0},
	{0,       0,         0,         0,        0,             0,                  0,            0}
};

#if defined(ENABLE_NEW_GOLD_LIMIT)
const uint64_t* GetAuraRefineInfo(uint8_t bLevel)
#else
const uint32_t* GetAuraRefineInfo(uint8_t bLevel)
#endif
{
	if (bLevel > 250) {
		return NULL;
	}

	for (auto i = 0; i < AURA_GRADE_MAX_NUM; ++i) {
		if (bLevel >= s_aiAuraRefineInfo[i][AURA_REFINE_INFO_LEVEL_MIN] && bLevel <= s_aiAuraRefineInfo[i][AURA_REFINE_INFO_LEVEL_MAX]) {
			return s_aiAuraRefineInfo[i];
		}
	}

	return NULL;
}

#if defined(ENABLE_NEW_GOLD_LIMIT)
const uint64_t GetAuraRefineInfo(uint8_t bGrade, uint8_t bInfo)
#else
const uint32_t GetAuraRefineInfo(uint8_t bGrade, uint8_t bInfo)
#endif
{
	if (bGrade >= AURA_GRADE_MAX_NUM || bInfo >= AURA_REFINE_INFO_MAX) {
		return 0;
	}

	return s_aiAuraRefineInfo[bGrade - 1][bInfo];
}
#endif
