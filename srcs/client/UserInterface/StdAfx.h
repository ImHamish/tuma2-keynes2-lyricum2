#pragma once

#include "Locale_inc.h"
#include "../../config/CommonDefines.h"
#include "../CommonDefines.h"

#include <m2_services.hpp>
#include <m2_utils.hpp>
#include <m2_length.hpp>
#include <m2_itemlength.hpp>
#include <m2_constants.hpp>
#include <m2_tables.hpp>
#include <m2_vnumhelper.hpp>

#pragma warning(disable:4702)
#pragma warning(disable:4100)
#pragma warning(disable:4201)
#pragma warning(disable:4511)
#pragma warning(disable:4663)
#pragma warning(disable:4018)
#pragma warning(disable:4245)

#if _MSC_VER >= 1400
//if don't use below, time_t is 64bit
#if defined(_WIN32) && !defined(_WIN64)
#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif
#endif
#endif

#include <iterator>
#include "../eterLib/StdAfx.h"
#include "../eterPythonLib/StdAfx.h"
#include "../gameLib/StdAfx.h"
#include "../scriptLib/StdAfx.h"
#include "../milesLib/StdAfx.h"
#include "../EffectLib/StdAfx.h"
#include "../PRTerrainLib/StdAfx.h"
#include "../SpeedTreeLib/StdAfx.h"

#ifndef __D3DRM_H__
#define __D3DRM_H__
#endif

#include <dshow.h>
#include <qedit.h>
#include "Locale.h"
#include "GameType.h"

extern DWORD __DEFAULT_CODE_PAGE__;

enum
{
	POINT_MAX_NUM = 255,
	CHARACTER_NAME_MAX_LEN = 24,
#if defined(LOCALE_SERVICE_JAPAN)
	PLAYER_NAME_MAX_LEN = 16,
#else
	PLAYER_NAME_MAX_LEN = 12,
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
	OFFLINE_SHOP_NAME_MAX_LEN = 40 + CHARACTER_NAME_MAX_LEN +1,
	OFFLINE_SHOP_ITEM_MAX_LEN = 48,
#endif
};

void initudp();
void initapp();
void initime();
void initsystemSetting();
void initchr();
void initchrmgr();
void initChat();
void initTextTail();
void initime();
void initItem();
void initNonPlayer();
void initnet();
void initPlayer();
void initSectionDisplayer();
void initServerStateChecker();
void initRenderTarget();
void initTrade();
void initMiniMap();
void initProfiler();
void initEvent();
void initeffect();
void initsnd();
void initeventmgr();
void initBackground();
void initwndMgr();
void initshop();
void initpack();
void initskill();
#ifdef NEW_PET_SYSTEM
	void initskillpet();
#endif
void initfly();
void initquest();

#ifdef ENABLE_CONFIG_MODULE
void initcfg();
#endif

void initsafebox();
#if defined(ENABLE_CLIENT_OPTIMIZATION)
void initPlayerSettingsModule();
#endif
void initguild();
void initMessenger();
#ifdef __ENABLE_NEW_OFFLINESHOP__
void initofflineshop();
#endif
#ifdef ENABLE_ACCE_SYSTEM
void initAcce();
#endif
#if defined(ENABLE_CHANGELOOK)
void initChangeLook();
#endif
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
void intcuberenewal();
#endif
#ifdef INGAME_WIKI
void initWiki();
#endif
