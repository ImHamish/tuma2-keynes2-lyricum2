#include "stdafx.h"
#include "start_position.h"

char g_nation_name[4][32] =
{
	"",
	"Shinsoo",
	"Chunjo",
	"Jinno",
};

#ifdef __ENABLE_CAPITALE_MAP__
long g_start_map[4] =
{
	0,	// reserved
	1,	// 신수국
	21,	// 천조국
	41	// 진노국
};

DWORD g_start_position[4][2] =
{
	{      0,      0 },	// reserved
	{ 469300, 964200 },	
	{  55700, 157900 },	
	{ 963000, 276800 }	
};

DWORD arena_return_position[4][2] =
{
	{      0,      0 },	// reserved
	{   347600, 882700  }, 
	{   138600, 236600  }, 
	{   857200, 251800  }  
};


DWORD g_create_position[4][2] =
{
	{      0,      0 },	// reserved
	{ 459800, 953900 },
	{ 52070, 166600 },
	{ 963100, 276800 },
};

DWORD g_create_position_canada[4][2] =
{
	{      0,      0 },	// reserved
	{ 457100, 946900 },
	{ 45700, 166500 },
	{ 966300, 288300 },
};
#else
long g_start_map[4] =
{
	0,	// reserved
	1,	// 신수국
	21,	// 천조국
	41	// 진노국
};

DWORD g_start_position[4][2] =
{
	{      0,      0 },	// reserved
	{ 311200, 336800 },	// 신수국 Rossi
	{ 516400, 336600 },	// 천조국 gialli
	{ 413700, 337000 }	// 진노국 Blu
};

DWORD arena_return_position[4][2] =
{
	{      0,      0 },	// reserved
	{ 311200, 336800 },	// 신수국 Rossi
	{ 516400, 336600 },	// 천조국 gialli
	{ 413700, 337000 }	// 진노국 Blu
};


DWORD g_create_position[4][2] =
{
	{      0,      0 },	// reserved
	{ 311200, 336800 },	// 신수국 Rossi
	{ 516400, 336600 },	// 천조국 gialli
	{ 413700, 337000 }	// 진노국 Blu
};

DWORD g_create_position_canada[4][2] =
{
	{      0,      0 },	// reserved
	{ 311200, 336800 },	// 신수국 Rossi
	{ 516400, 336600 },	// 천조국 gialli
	{ 413700, 337000 }	// 진노국 Blu
};
#endif

