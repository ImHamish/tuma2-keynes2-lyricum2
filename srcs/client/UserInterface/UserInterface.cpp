#include "StdAfx.h"
#include "PythonApplication.h"
#include "ProcessScanner.h"
#include "resource.h"

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include "../eterPack/EterPackManager.h"
#include "../eterLib/Util.h"
#include "../eterBase/CPostIt.h"

#include "PythonConfig.h"
#include "ProcessCRC.h"
#ifdef ENABLE_HWID
#include "CHwidManager.h"
#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#if defined(DISTRIBUTE)
#include <xorstr.hpp>
#endif

extern "C" {
extern int _fltused;
volatile int _AVOID_FLOATING_POINT_LIBRARY_BUG = _fltused;
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
FILE __iob_func[3] = { *stdin,*stdout,*stderr };
};

#pragma comment(linker, "/NODEFAULTLIB:libci.lib")

#pragma comment(lib, "version.lib")
#pragma comment(lib, "cryptopp-static.lib")
#pragma comment(lib, "libsodium.lib")
#pragma comment(lib, "imagehlp.lib")
#pragma comment(lib, "devil.lib")
#pragma comment(lib, "granny2.lib")
#pragma comment(lib, "mss32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "oldnames.lib")
#pragma comment(lib, "SpeedTreeRT.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dmoguids.lib")
#pragma comment(lib, "liblz4_static.lib")
#if defined(ENABLE_DISCORD_RPC)
#pragma comment(lib, "discord-rpc.lib")
#endif
#if defined(__ENABLE_NEW_OFFLINESHOP__)
#pragma comment(lib, "shlwapi.lib")
#endif
#if defined(ENABLE_CLIENT_OPTIMIZATION)
#pragma comment(lib, "resource.lib")
#endif

#ifdef TRACY_ENABLE
#pragma comment(lib, "TracyClient.lib")
#endif

//#pragma comment( lib, "wsock32.lib" )
#include <stdlib.h>

bool __IS_TEST_SERVER_MODE__=false;

// #define ENABLE_DAEMONPROTECTION

#ifdef ENABLE_DAEMONPROTECTION
#define DPDLL_FILENAME	"dpdll.dll"
#define DPDLL_CRC32		0x48104810
#define DPDLL_FILESIZE	1234567+4
#endif

extern bool SetDefaultCodePage(DWORD codePage);

////////////////////////////////////////////

int Setup(LPSTR lpCmdLine); // Internal function forward

std::vector<std::wstring> packFiles = {
	L"pack/effect",
	L"pack/etc",
	L"pack/guild",
	L"pack/icon",
	L"pack/item",
	L"pack/map",
	L"pack/monster",
	L"pack/monster2",
	L"pack/npc",
	L"pack/npc_pet",
	L"pack/npc_mount",
	L"pack/npc2",
	L"pack/pc",
	L"pack/pc2",
	L"pack/season",
	L"pack/sound",
	L"pack/terrain",
	L"pack/textureset",
	L"pack/tree",
	L"pack/zone1",
	L"pack/zone2",
	L"pack/zone3",
	L"pack/locale",
	L"pack/data",
	L"pack/patch1",
	L"pack/patch2",
	L"pack/patch1_d",
	L"pack/patch2_d",
	L"pack/root",
};

void PackUninitialize() {
	for (const auto& packname : packFiles) {
		CFileSystem::Instance().UnloadArchive(packname);
	}
}

bool PackInitialize(const char * c_pszFolder)
{
	NANOBEGIN

#if defined(ENABLE_NEW_ANTICHEAT_RULES)
	struct stat st;
	if (stat( "D:\\ymir work", &st) == 0 ) {
		MessageBox(NULL, "Please remove the folder D:\\Ymir Work!", "keynes2", MB_ICONSTOP);
		return false;
	}

	if (stat( "C:\\Memorybreak", &st) == 0 ) {
		MessageBox(NULL, "Please remove the folder C:\\Memorybreak!", "keynes2", MB_ICONSTOP);
		return false;
	}
#endif

	if (_access(c_pszFolder, 0) != 0)
		return false;

	CTextFileLoader::SetCacheMode();
#if defined(USE_RELATIVE_PATH)
	CFileSystem::Instance().SetRelativePathMode();
#endif

	CSoundData::SetPackMode();

	for (const auto& packname : packFiles) {
		CFileSystem::Instance().LoadArchive(packname);
	}

	NANOEND
	return true;
}

bool RunMainScript(CPythonLauncher& pyLauncher, const char* lpCmdLine)
{
	initpack();
	initdbg();
	initime();
	initgrp();
	initgrpImage();
	initgrpText();
	initwndMgr();
	/////////////////////////////////////////////
	initudp();
	initapp();
	initsystemSetting();
	initchr();
	initchrmgr();
	initPlayer();
	initItem();
	initNonPlayer();
	initTrade();
	initChat();
	initTextTail();
	initnet();
	initMiniMap();
	initProfiler();
	initEvent();
	initeffect();
	initfly();
	initsnd();
	initeventmgr();
	initshop();
	initskill();
#ifdef NEW_PET_SYSTEM
	initskillpet();
#endif
	initquest();
	initBackground();
	initMessenger();
#ifdef ENABLE_ACCE_SYSTEM
	initAcce();
#endif
#if defined(ENABLE_CHANGELOOK)
	initChangeLook();
#endif
#ifdef ENABLE_CONFIG_MODULE
	initcfg();
#endif

	initsafebox();
#if defined(ENABLE_CLIENT_OPTIMIZATION)
	initPlayerSettingsModule();
#endif
	initguild();
	initServerStateChecker(); 
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	intcuberenewal();
#endif
#ifdef INGAME_WIKI
	initWiki();
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
	initofflineshop();
#endif

	PyObject * builtins = PyImport_ImportModule("__builtin__");
#ifdef NDEBUG
	PyModule_AddIntConstant(builtins, "__DEBUG__", 1);
#else
	PyModule_AddIntConstant(builtins, "__DEBUG__", 0);
#endif
#if defined(DISTRIBUTE)
	PyModule_AddIntConstant(builtins, "__USE_DISTRIBUTE__", 1);
#else
	PyModule_AddIntConstant(builtins, "__USE_DISTRIBUTE__", 0);
#endif

	PyModule_AddStringConstant(builtins, "__COMMAND_LINE__", "");
	if (!pyLauncher.RunFile("system.py"))
	{
		TraceError("RunMain Error");
		std::system("PAUSE");
		return false;
	}

	return true;
}

bool Main(HINSTANCE hInstance, LPSTR lpCmdLine)
{
	DWORD dwRandSeed = time(NULL)+DWORD(GetCurrentProcess());
	srandom(dwRandSeed);
	srand(random());

	SetLogLevel(1);

#ifndef __VTUNE__
	ilInit();
#endif

	if (!Setup(lpCmdLine))
	{
		return false;
	}

#ifdef _DEBUG
	OpenConsoleWindow();
	OpenLogFile(true);
#else
	OpenLogFile(false);
#endif

	BuildProcessCRC();

	static CLZO lzo;
	static CFileSystem EterPackManager;
#ifdef ENABLE_HWID
	static CHwidManager HwidManager;
#endif

	if (!PackInitialize("pack"))
	{
#ifdef ENABLE_LITTLE_PACK
		TraceError("Pack initialization failed.");
#else
		LogBox("Pack Initialization failed. Check log.txt file..");
#endif
		return false;
	}

#ifdef ENABLE_CONFIG_MODULE
	static CPythonConfig m_pyConfig;
	m_pyConfig.Initialize("config.cfg");
#endif

	if(LocaleService_LoadGlobal(hInstance))
		SetDefaultCodePage(LocaleService_GetCodePage());

	CPythonApplication * app = new CPythonApplication;

	app->Initialize(hInstance);

	bool ret=false;
	{
		CPythonLauncher pyLauncher;

		if (pyLauncher.Create()) {
			ret=RunMainScript(pyLauncher, lpCmdLine);
		}

		app->Clear();

		timeEndPeriod(1);
		pyLauncher.Clear();
	}

	app->Destroy();
	delete app;

	return ret;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	LocaleService_LoadConfig("locale.cfg");
	SetDefaultCodePage(LocaleService_GetCodePage());

	Main(hInstance, lpCmdLine);

	PackUninitialize();

	::CoUninitialize();
}

static void GrannyError(granny_log_message_type Type,
	granny_log_message_origin Origin,
	char const* File,
	granny_int32x Line,
	char const* Error,
	void* UserData)
{
	TraceError("GRANNY: %s", Error);
}

int Setup(LPSTR lpCmdLine)
{
	/*
	 *	타이머 정밀도를 올린다.
	 */
	TIMECAPS tc;
	UINT wTimerRes;

	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
		return 0;

	wTimerRes = MINMAX(tc.wPeriodMin, 1, tc.wPeriodMax);
	timeBeginPeriod(wTimerRes);

	/*
	 *	그래니 에러 핸들링
	 */

/*	granny_log_callback Callback;
    Callback.Function = GrannyError;
    Callback.UserData = 0;
    GrannySetLogCallback(&Callback);
*/
	return 1;
}

