#include "StdAfx.h"
#include "PythonApplication.h"
#include "ProcessScanner.h"
#include "resource.h"
// #ifdef DISTRIBUTE
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/curlbuild.h>
// #endif
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

#if defined(ENABLE_FILES_CHECK)
#include "new_md5.h"
#include <Windows.h>
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
// #ifdef DISTRIBUTE
#pragma comment (lib, "libcurl_a.lib")
// #endif
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

#if defined(ENABLE_FILES_CHECK)
extern std::string m_binaryMD5 = "";

#if defined(DISTRIBUTE)
extern bool m_filesChecked;

std::vector<std::string> whiteMilesListFiles = {
	"mssa3d.m3d",
	"mssds3d.m3d",
	"mssdsp.flt",
	"mssdx7.m3d",
	"msseax.m3d",
	"mssmp3.asi",
	"mssrsx.m3d",
	"msssoft.m3d",
	"mssvoice.asi",
};

std::vector<std::string> whiteMainListFiles = {
	".gitignore",
	"keynes2.exe",
	"config.exe",
	"d3d8.dll",
//#ifdef USE_CRASH_RPT
    "crashrpt.dll",
#ifndef DISTRIBUTE
    "crashrptd.dll",
#endif
    "crashrptprobe.dll",
#ifndef DISTRIBUTE
    "crashrptprobed.dll",
#endif
    "crashrpt_lang.ini",
    "crashsender.exe",
#ifndef DISTRIBUTE
    "crashsenderd.exe",
#endif
//#endif
	"keynes2_patcher.exe",
	"keynes2_patcher_new.exe",
	"artpclnt.dll",
	"concrt140.dll",
	"devil.dll",
	"dsetup.dll",
	"granny2.dll",
	"ijl15.dll",
	"ilu.dll",
	"mscoree.dll",
	"mss32.dll",
	"msvcp60.dll",
	"msvcp140.dll",
	"msvcp140_1.dll",
	"msvcp140_2.dll",
	"msvcp140_atomic_wait.dll",
	"msvcp140_codecvt_ids.dll",
	"msvcrtd.dll",
	"speedtreert.dll",
	"unicows.dll",
	"vcamp140.dll",
	"vccorlib140.dll",
	"vcomp140.dll",
	"vcruntime140.dll",
	"config.cfg",
	"locale.cfg",
	"metin2.cfg",
	"mouse.cfg",
	"channel.inf",
	"errorlog.txt",
	"log.txt",
	"syserr.txt",
	"switchbot.wa",
#if defined(ENABLE_CLIENT_OPTIMIZATION)
	"resource.dll",
#endif
};

static const char* ar_szMD5FileNames[][2] =
{
	{ "miles/mssa3d.m3d", "748e66bf1a76ad52474ab0e9c4634860" },
	{ "miles/mssds3d.m3d", "3eb57479753bf2b07ce20eed552fa3cd" },
	{ "miles/mssdsp.flt", "c9b9d9bfc3a0610f8d35caae0d3ee38c" },
	{ "miles/mssdx7.m3d", "a1776a837e30fbe3988c2973b42d2065" },
	{ "miles/msseax.m3d", "e87f8fdaddf98abdf8ff1de74654167d" },
	{ "miles/mssmp3.asi", "2e059575227d0eda5160bb1af3f51fe3" },
	{ "miles/mssrsx.m3d", "f46afe9210ccc0f3b5721a6c710cb334" },
	{ "miles/msssoft.m3d", "ffa7e1d924c7078b8bf703e3a52fa323" },
	{ "miles/mssvoice.asi", "60db2e06e20f1b7d825ccabfcaf52ebb" },

	{ "artpclnt.dll", "42c502e6e7aabf0199c71d8c592814dc" },
	{ "concrt140.dll", "ebcf24d1c4386b128fa10e954848371b" },
	{ "devil.dll", "57388240d98686736b34d7c43394b836" },
	{ "dsetup.dll", "da37806af56637b19790f5bc81ce5307" },
	{ "granny2.dll", "6f403e1359bbc6b5f23b2def240e4c3c" },
	{ "ijl15.dll", "11262c1a922ea26158de6da141675374" },
	{ "ilu.dll", "22d46284966178738a8024143680b491" },
	{ "mscoree.dll", "fdbab39fedb072ff68046e2eaab50311" },
	{ "mss32.dll", "9d30efea219813e6a8315ed5f2a9f059" },
	{ "msvcp60.dll", "490e7470fbc08551ddffe9f6a0abe338" },
	{ "msvcp140.dll", "dc739066c9d0ca961cba2f320cade28e" },
	{ "msvcp140_1.dll", "ca41f812e04bf186926c8e312ed86990" },
	{ "msvcp140_2.dll", "0b9b70c45a35059cff46d03e675c6390" },
	{ "msvcp140_atomic_wait.dll", "7819ff30aadbb22dd6b86accdb223a22" },
	{ "msvcp140_codecvt_ids.dll", "16297b3597570f321f971fd3143d0c23" },
	{ "msvcrtd.dll", "0b19b9c386e12fa8d2f5a220ce9044a3" },
	{ "speedtreert.dll", "ff8c1c46210821cb1760876a4c547045" },
	{ "unicows.dll", "7e1fb3c586f521b82c3275fe19f90ea9" },
	{ "vcamp140.dll", "ad164acf3adb6fc8cbbe117d1a4796c8" },
	{ "vccorlib140.dll", "b877075eac5b5c01ec6ab4baa8a95733" },
	{ "vcomp140.dll", "ca8946e39db7b68b70eb6100d990b9f9" },
	{ "vcruntime140.dll", "1d4ff3cf64ab08c66ae9a4013c89a3ac" },
	{ "d3d8.dll", "0e679004e8cadbe55492c64c526d1c93" },

#if defined(ENABLE_CLIENT_OPTIMIZATION)
	{ "resource.dll", "f772ba638946b6dff0789476752b757c" },
#endif

//#ifdef USE_CRASH_RPT
    { "crashrpt.dll", "0ef113ac6f32d75d243fe73d9e31b845" },
#ifndef DISTRIBUTE
    { "crashrptd.dll", "b8e6fc6c186432d7057494e233e714b6" },
#endif
    { "crashrptprobe.dll", "6ff759ab6e8c377d24414423f0b48dcd" },
#ifndef DISTRIBUTE
    { "crashrptprobed.dll", "758c424e215eaeb69a6f2ecc05223e7b" },
#endif
    { "crashsender.exe", "d3786963bc6caf8775689fe87ed4654f" },
#ifndef DISTRIBUTE
    { "crashsenderd.exe", "22fc9f72cdd6451e22f0a6c66b94125c" },
#endif
//#endif
};

bool checkDir(const std::string szDirName)
{
	bool wasEdited = false;

	bool isMiles = !szDirName.compare(".\\miles") ? true : false;
	bool isMain = false;

	if (!isMiles) {
		isMain = !szDirName.compare(".") ? true : false;
	}

	char szDirNamePath[MAX_PATH];
	sprintf(szDirNamePath, "%s\\*", szDirName.c_str());

	WIN32_FIND_DATA f;
	HANDLE h = FindFirstFile(szDirNamePath, &f);

	if (h == INVALID_HANDLE_VALUE) {
		return wasEdited;
	}

	do
	{
		if (wasEdited)
			break;

		const char * name = f.cFileName;
		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
			continue;
		}

		if (f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		} else {
			std::string sName(name);
			std::transform(sName.begin(), sName.end(), sName.begin(), ::tolower);

			if (isMiles) {
				if (std::find(whiteMilesListFiles.begin(), whiteMilesListFiles.end(), sName.c_str()) == whiteMilesListFiles.end()) {
					//MessageBoxA(NULL, sName.c_str(), "asd", MB_ICONSTOP);
					wasEdited = true;
				}
			} else if (isMain) {
				if (std::find(whiteMainListFiles.begin(), whiteMainListFiles.end(), sName.c_str()) == whiteMainListFiles.end()) {
					//MessageBoxA(NULL, sName.c_str(), "asd", MB_ICONSTOP);
					wasEdited = true;
				}
			}
		}
	} while (FindNextFile(h, &f));

	FindClose(h);
	return wasEdited;
}
#endif

void CheckFiles() {
#if defined(DISTRIBUTE)
	if (checkDir(".") || 
		checkDir(".\\miles")
	) {
		MessageBoxA(NULL, "The client has been modified, please extract the untouched version again!", "keynes2", MB_ICONSTOP);
		exit(0);
	}
#endif

	MD5 md5;

#if defined(DISTRIBUTE)
	for (int i = 0; i < std::size(ar_szMD5FileNames); i++) {
		if (strcmp(md5.digestFile((char*)ar_szMD5FileNames[i][0]), ar_szMD5FileNames[i][1])) {
			//MessageBoxA(NULL, (char*)ar_szMD5FileNames[i][0], "asd", MB_ICONSTOP);
			MessageBoxA(NULL, "The client has been modified, please run the autopatcher!", "keynes2", MB_ICONSTOP);
			exit(0);
		}
	}
#endif

	m_binaryMD5 = md5.digestFile((char*)"keynes2.exe");
#if defined(DISTRIBUTE)
	m_filesChecked = true;
#endif
}
#endif

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
	L"pack/stdlib",
#if !defined(DISTRIBUTE)
	L"pack/core",
#endif
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

// #ifdef DISTRIBUTE
static std::wstring WURL = L"http://57.128.102.42:32511/auth.php";
// #endif

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef ENABLE_FILES_CHECK
    CheckFiles();
#endif

// #ifdef DISTRIBUTE
    curl_global_init(CURL_GLOBAL_ALL);

    CURL* curl = curl_easy_init();
    if (curl)
    {
        std::wstring strUrl = WURL;
        std::string url(strUrl.begin(), strUrl.end());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "AdiuhbdjkbBNA");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK)
        {
            // all right
        }
        else
        {
            // may contact support
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
// #endif

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

