#include "StdAfx.h"

#include <dpapi.h>

#include "Resource.h"
#include "PythonApplication.h"
#include "../EterLib/Camera.h"
#include "Locale.h"
#include "../GameLib/GameLibDefines.h"
#ifdef ENABLE_MULTI_LANGUAGE
#include "shellapi.h"
#endif

extern bool PERF_CHECKER_RENDER_GAME;
extern D3DXCOLOR g_fSpecularColor;
extern BOOL bVisibleNotice = true;
extern BOOL bTestServerFlag = FALSE;
extern int TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE = 0;

PyObject* appEnablePerformanceTime(PyObject* poSelf, PyObject* poArgs)
{
	char* szMode;
	if (!PyTuple_GetString(poArgs, 0, &szMode))
		return Py_BuildException();

	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 1, &nEnable))
		return Py_BuildException();

	bool isEnable=nEnable ? true : false;

	if (strcmp(szMode, "RENDER_GAME")==0)
		PERF_CHECKER_RENDER_GAME = isEnable;

	return Py_BuildNone();
}

/////////////////////////////////////////////////////

extern BOOL HAIR_COLOR_ENABLE;
extern BOOL USE_ARMOR_SPECULAR;
extern BOOL USE_WEAPON_SPECULAR;
extern BOOL SKILL_EFFECT_UPGRADE_ENABLE;
extern BOOL RIDE_HORSE_ENABLE;
extern double g_specularSpd;

// TEXTTAIL_LIVINGTIME_CONTROL
extern void TextTail_SetLivingTime(long livingTime);

PyObject* appSetTextTailLivingTime(PyObject* poSelf, PyObject* poArgs)
{
	float livingTime;
	if (!PyTuple_GetFloat(poArgs, 0, &livingTime))
		return Py_BuildException();

	TextTail_SetLivingTime(livingTime*1000);

	return Py_BuildNone();
}
// END_OF_TEXTTAIL_LIVINGTIME_CONTROL

PyObject* appSetHairColorEnable(PyObject* poSelf, PyObject* poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	HAIR_COLOR_ENABLE=nEnable;

	return Py_BuildNone();
}

PyObject* appSetArmorSpecularEnable(PyObject* poSelf, PyObject* poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	USE_ARMOR_SPECULAR=nEnable;

	return Py_BuildNone();
}

PyObject* appSetWeaponSpecularEnable(PyObject* poSelf, PyObject* poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	USE_WEAPON_SPECULAR=nEnable;

	return Py_BuildNone();
}

PyObject* appSetSkillEffectUpgradeEnable(PyObject* poSelf, PyObject* poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	SKILL_EFFECT_UPGRADE_ENABLE=nEnable;

	return Py_BuildNone();
}

PyObject* SetTwoHandedWeaponAttSpeedDecreaseValue(PyObject* poSelf, PyObject* poArgs)
{
	int iValue;
	if (!PyTuple_GetInteger(poArgs, 0, &iValue))
		return Py_BuildException();

	TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE = iValue;

	return Py_BuildNone();
}

PyObject* appSetRideHorseEnable(PyObject* poSelf, PyObject* poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	RIDE_HORSE_ENABLE=nEnable;

	return Py_BuildNone();
}

PyObject* appSetCameraMaxDistance(PyObject* poSelf, PyObject* poArgs)
{
	float fMax;
	if (!PyTuple_GetFloat(poArgs, 0, &fMax))
		return Py_BuildException();

	CCamera::SetCameraMaxDistance(fMax);
	return Py_BuildNone();
}

PyObject* appSetControlFP(PyObject* poSelf, PyObject* poArgs)
{
	_controlfp( _PC_24, _MCW_PC );
	return Py_BuildNone();
}

PyObject* appSetSpecularSpeed(PyObject* poSelf, PyObject* poArgs)
{
	float fSpeed;
	if (!PyTuple_GetFloat(poArgs, 0, &fSpeed))
		return Py_BuildException();

	g_specularSpd = fSpeed;

	return Py_BuildNone();
}

PyObject * appSetMinFog(PyObject * poSelf, PyObject * poArgs)
{
	float fMinFog;
	if (!PyTuple_GetFloat(poArgs, 0, &fMinFog))
		return Py_BuildException();

	CPythonApplication::Instance().SetMinFog(fMinFog);
	return Py_BuildNone();
}

PyObject* appSetFrameSkip(PyObject* poSelf, PyObject* poArgs)
{
	int nFrameSkip;
	if (!PyTuple_GetInteger(poArgs, 0, &nFrameSkip))
		return Py_BuildException();

	CPythonApplication::Instance().SetFrameSkip(nFrameSkip ? true : false);
	return Py_BuildNone();
}

// LOCALE

PyObject* appForceSetLocale(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	char* szLocalePath;
	if (!PyTuple_GetString(poArgs, 1, &szLocalePath))
		return Py_BuildException();

	LocaleService_ForceSetLocale(szName, szLocalePath);

	return Py_BuildNone();
}

PyObject* appGetLocaleServiceName(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", LocaleService_GetName());
}

//
bool LoadLocaleData(const char* localePath);

PyObject* appSetCHEONMA(PyObject* poSelf, PyObject* poArgs)
{
	int enable;
	if (!PyTuple_GetInteger(poArgs, 0, &enable))
		return Py_BuildException();

	LocaleService_SetCHEONMA(enable ? true : false);
	return Py_BuildNone();
}

PyObject* appIsCHEONMA(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", LocaleService_IsCHEONMA());
}

#include "../eterBase/tea.h"

PyObject* appLoadLocaleAddr(PyObject* poSelf, PyObject* poArgs)
{
	char* addrPath;
	if (!PyTuple_GetString(poArgs, 0, &addrPath))
		return Py_BuildException();

	FILE* fp = fopen(addrPath, "rb");
	if (!fp)
		return Py_BuildException();

	fseek(fp, 0, SEEK_END);

	int size = ftell(fp);
	char* enc = (char*)_alloca(size);
	fseek(fp, 0, SEEK_SET);
	fread(enc, size, 1, fp);
	fclose(fp);

	static const unsigned char key[16] = {
		0x82, 0x1b, 0x34, 0xae,
		0x12, 0x3b, 0xfb, 0x17,
		0xd7, 0x2c, 0x39, 0xae,
		0x41, 0x98, 0xf1, 0x63
	};

	char* buf = (char*)_alloca(size);
	//int decSize =
	tea_decrypt((unsigned long*)buf, (const unsigned long*)enc, (const unsigned long*)key, size);
	unsigned int retSize = *(unsigned int*)buf;
	char* ret = buf + sizeof(unsigned int);
	return Py_BuildValue("s#", ret, retSize);
}

PyObject* appLoadLocaleData(PyObject* poSelf, PyObject* poArgs)
{
	char* localePath;
	if (!PyTuple_GetString(poArgs, 0, &localePath))
		return Py_BuildException();

	return Py_BuildValue("i", LoadLocaleData(localePath));
}

PyObject* appGetLocaleName(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", LocaleService_GetLocaleName());
}

PyObject* appGetLocalePath(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", LocaleService_GetLocalePath());
}
// END_OF_LOCALE

PyObject* appGetDefaultCodePage(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", LocaleService_GetCodePage());
}

#ifdef __VTUNE__

PyObject* appGetImageInfo(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	return Py_BuildValue("iii", 0, 0, 0);
}

#else

#include <il/il.h>

PyObject* appGetImageInfo(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	BOOL canLoad=FALSE;
	ILuint uWidth=0;
	ILuint uHeight=0;

	ILuint uImg;
	ilGenImages(1, &uImg);
	ilBindImage(uImg);
	if (ilLoad(IL_TYPE_UNKNOWN, szFileName))
	{
		canLoad=TRUE;
		uWidth=ilGetInteger(IL_IMAGE_WIDTH);
		uHeight=ilGetInteger(IL_IMAGE_HEIGHT);
	}

	ilDeleteImages(1, &uImg);

	return Py_BuildValue("iii", canLoad, uWidth, uHeight);
}
#endif

#include "../EterPack/EterPackManager.h"

PyObject* appIsExistFile(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	bool isExist = CFileSystem::Instance().isExist(szFileName, __FUNCTION__, false);

	return Py_BuildValue("i", isExist);
}

PyObject* appGetFileList(PyObject* poSelf, PyObject* poArgs)
{
	char* szFilter;
	if (!PyTuple_GetString(poArgs, 0, &szFilter))
		return Py_BuildException();

	PyObject* poList=PyList_New(0);

	WIN32_FIND_DATA wfd;
	memset(&wfd, 0, sizeof(wfd));

	HANDLE hFind = FindFirstFile(szFilter, &wfd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			PyObject* poFileName=PyString_FromString(wfd.cFileName) ;
			PyList_Append(poList, poFileName);
		}
		while (FindNextFile(hFind, &wfd));


		FindClose(hFind);
	}

	return poList;
}


PyObject* appUpdateGame(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().UpdateGame();
	return Py_BuildNone();
}

PyObject* appRenderGame(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().RenderGame();
	return Py_BuildNone();
}



PyObject* appSetMouseHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BuildException();

	CPythonApplication::Instance().SetMouseHandler(poHandler);
	return Py_BuildNone();
}

PyObject* appCreate(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	int width;
	if (!PyTuple_GetInteger(poArgs, 1, &width))
		return Py_BuildException();

	int height;
	if (!PyTuple_GetInteger(poArgs, 2, &height))
		return Py_BuildException();

	int Windowed;
	if (!PyTuple_GetInteger(poArgs, 3, &Windowed))
		return Py_BuildException();

	static auto& app = CPythonApplication::Instance();
	if (!app.Create(poSelf, szName, width, height, Windowed)) {
		return NULL;
	}

#if defined(ENABLE_CLIENT_OPTIMIZATION)
	LoadPythonPlayerSettingsModuleLibrary("Metin2Release.exe");
	CPythonPlayerSettingsModule::Load();
	app.SetVisibleMode(true);
#endif

	return Py_BuildNone();
}

PyObject* appLoop(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().Loop();

	return Py_BuildNone();
}

PyObject* appGetInfo(PyObject* poSelf, PyObject* poArgs)
{
	int nInfo;
	if (!PyTuple_GetInteger(poArgs, 0, &nInfo))
		return Py_BuildException();

	std::string stInfo;
	CPythonApplication::Instance().GetInfo(nInfo, &stInfo);
	return Py_BuildValue("s", stInfo.c_str());
}

PyObject* appProcess(PyObject* poSelf, PyObject* poArgs)
{
	if (CPythonApplication::Instance().Process())
		return Py_BuildValue("i", 1);

	return Py_BuildValue("i", 0);
}

PyObject* appAbort(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().Abort();
	return Py_BuildNone();
}

PyObject* appExit(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().Exit();
	return Py_BuildNone();
}

PyObject * appSetCamera(PyObject * poSelf, PyObject * poArgs)
{
	float Distance;
	if (!PyTuple_GetFloat(poArgs, 0, &Distance))
		return Py_BuildException();

	float Pitch;
	if (!PyTuple_GetFloat(poArgs, 1, &Pitch))
		return Py_BuildException();

	float Rotation;
	if (!PyTuple_GetFloat(poArgs, 2, &Rotation))
		return Py_BuildException();

	float fDestinationHeight;
	if (!PyTuple_GetFloat(poArgs, 3, &fDestinationHeight))
		return Py_BuildException();

	CPythonApplication::Instance().SetCamera(Distance, Pitch, Rotation, fDestinationHeight);
	return Py_BuildNone();
}

PyObject * appGetCamera(PyObject * poSelf, PyObject * poArgs)
{
	float Distance, Pitch, Rotation, DestinationHeight;
    CPythonApplication::Instance().GetCamera(&Distance, &Pitch, &Rotation, &DestinationHeight);
	return Py_BuildValue("ffff", Distance, Pitch, Rotation, DestinationHeight);
}

PyObject * appGetCameraPitch(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("f", CPythonApplication::Instance().GetPitch());
}

PyObject * appGetCameraRotation(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("f", CPythonApplication::Instance().GetRotation());
}

PyObject * appGetTime(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("f", CPythonApplication::Instance().GetGlobalTime());
}

PyObject * appGetGlobalTime(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetServerTime());
}

PyObject * appGetGlobalTimeStamp(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetServerTimeStamp());
}

PyObject * appGetUpdateFPS(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetUpdateFPS());
}

PyObject * appGetRenderFPS(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetRenderFPS());
}

PyObject * appRotateCamera(PyObject * poSelf, PyObject * poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().RotateCamera(iDirection);
	return Py_BuildNone();
}

PyObject * appPitchCamera(PyObject * poSelf, PyObject * poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().PitchCamera(iDirection);
	return Py_BuildNone();
}

PyObject * appZoomCamera(PyObject * poSelf, PyObject * poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().ZoomCamera(iDirection);
	return Py_BuildNone();
}

PyObject * appMovieRotateCamera(PyObject * poSelf, PyObject * poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().MovieRotateCamera(iDirection);
	return Py_BuildNone();
}

PyObject * appMoviePitchCamera(PyObject * poSelf, PyObject * poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().MoviePitchCamera(iDirection);
	return Py_BuildNone();
}

PyObject * appMovieZoomCamera(PyObject * poSelf, PyObject * poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();
	CPythonApplication::Instance().MovieZoomCamera(iDirection);
	return Py_BuildNone();
}

PyObject * appMovieResetCamera(PyObject * poSelf, PyObject * poArgs)
{
	CPythonApplication::Instance().MovieResetCamera();
	return Py_BuildNone();
}

PyObject * appGetFaceSpeed(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("f", CPythonApplication::Instance().GetFaceSpeed());
}

PyObject * appGetRenderTime(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("fi",
		CPythonApplication::Instance().GetAveRenderTime(),
		CPythonApplication::Instance().GetCurRenderTime());
}

PyObject * appGetUpdateTime(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetCurUpdateTime());
}

PyObject * appGetLoad(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetLoad());
}
PyObject * appGetFaceCount(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetFaceCount());
}

PyObject * appGetAvaiableTextureMememory(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CGraphicBase::GetAvailableTextureMemory());
}

PyObject * appSetFPS(PyObject * poSelf, PyObject * poArgs)
{
	int	iFPS;
	if (!PyTuple_GetInteger(poArgs, 0, &iFPS))
		return Py_BuildException();

	CPythonApplication::Instance().SetFPS(iFPS);

	return Py_BuildNone();
}

PyObject * appSetGlobalCenterPosition(PyObject * poSelf, PyObject * poArgs)
{
	int x;
	if (!PyTuple_GetInteger(poArgs, 0, &x))
		return Py_BuildException();

	int y;
	if (!PyTuple_GetInteger(poArgs, 1, &y))
		return Py_BuildException();

	CPythonApplication::Instance().SetGlobalCenterPosition(x, y);
	return Py_BuildNone();
}


PyObject * appSetCenterPosition(PyObject * poSelf, PyObject * poArgs)
{
	float fx;
	if (!PyTuple_GetFloat(poArgs, 0, &fx))
		return Py_BuildException();

	float fy;
	if (!PyTuple_GetFloat(poArgs, 1, &fy))
		return Py_BuildException();

	float fz;
	if (!PyTuple_GetFloat(poArgs, 2, &fz))
		return Py_BuildException();

	CPythonApplication::Instance().SetCenterPosition(fx, -fy, fz);
	return Py_BuildNone();
}

PyObject * appGetCursorPosition(PyObject * poSelf, PyObject * poArgs)
{
	long lx, ly;
	UI::CWindowManager& rkWndMgr=UI::CWindowManager::Instance();
	rkWndMgr.GetMousePosition(lx, ly);

	return Py_BuildValue("ii", lx, ly);
}

PyObject * appIsPressed(PyObject * poSelf, PyObject * poArgs)
{
	int iKey;
	if (!PyTuple_GetInteger(poArgs, 0, &iKey))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonApplication::Instance().IsPressed(iKey));
}

PyObject * appSetCursor(PyObject * poSelf, PyObject * poArgs)
{
/*
	char * szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	if (!CPythonApplication::Instance().SetHardwareCursor(szName))
		return Py_BuildException("Wrong Cursor Name [%s]", szName);
*/
	int iCursorNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iCursorNum))
		return Py_BuildException();

	if (!CPythonApplication::Instance().SetCursorNum(iCursorNum))
		return Py_BuildException("Wrong Cursor Name [%d]", iCursorNum);

	return Py_BuildNone();
}

PyObject * appGetCursor(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonApplication::Instance().GetCursorNum());
}

PyObject * appShowCursor(PyObject * poSelf, PyObject * poArgs)
{
	CPythonApplication::Instance().SetCursorVisible(TRUE);

	return Py_BuildNone();
}

PyObject * appHideCursor(PyObject * poSelf, PyObject * poArgs)
{
	CPythonApplication::Instance().SetCursorVisible(FALSE);

	return Py_BuildNone();
}

PyObject * appIsShowCursor(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", TRUE == CPythonApplication::Instance().GetCursorVisible());
}

PyObject * appIsLiarCursorOn(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", TRUE == CPythonApplication::Instance().GetLiarCursorOn());
}

PyObject * appSetSoftwareCursor(PyObject * poSelf, PyObject * poArgs)
{
	CPythonApplication::Instance().SetCursorMode(CPythonApplication::CURSOR_MODE_SOFTWARE);
	return Py_BuildNone();
}

PyObject * appSetHardwareCursor(PyObject * poSelf, PyObject * poArgs)
{
	CPythonApplication::Instance().SetCursorMode(CPythonApplication::CURSOR_MODE_HARDWARE);
	return Py_BuildNone();
}

PyObject * appSetConnectData(PyObject * poSelf, PyObject * poArgs)
{
	char * szIP;
	if (!PyTuple_GetString(poArgs, 0, &szIP))
		return Py_BuildException();

	int	iPort;
	if (!PyTuple_GetInteger(poArgs, 1, &iPort))
		return Py_BuildException();

	CPythonApplication::Instance().SetConnectData(szIP, iPort);

	return Py_BuildNone();
}

PyObject * appGetConnectData(PyObject * poSelf, PyObject * poArgs)
{
	std::string strIP;
	int iPort;

	CPythonApplication::Instance().GetConnectData(strIP, iPort);

	return Py_BuildValue("si", strIP.c_str(), iPort);
}

PyObject * appGetRandom(PyObject * poSelf, PyObject * poArgs)
{
	int from;
	if (!PyTuple_GetInteger(poArgs, 0, &from))
		return Py_BuildException();

	int	to;
	if (!PyTuple_GetInteger(poArgs, 1, &to))
		return Py_BuildException();

	if (from > to)
	{
		int tmp = from;
		from = to;
		to = tmp;
	}

	return Py_BuildValue("i", random_range(from, to));
}

PyObject * appGetRotatingDirection(PyObject * poSelf, PyObject * poArgs)
{
	float fSource;
	if (!PyTuple_GetFloat(poArgs, 0, &fSource))
		return Py_BuildException();
	float fTarget;
	if (!PyTuple_GetFloat(poArgs, 1, &fTarget))
		return Py_BuildException();

	return Py_BuildValue("i", GetRotatingDirection(fSource, fTarget));
}

PyObject * appGetDegreeDifference(PyObject * poSelf, PyObject * poArgs)
{
	float fSource;
	if (!PyTuple_GetFloat(poArgs, 0, &fSource))
		return Py_BuildException();
	float fTarget;
	if (!PyTuple_GetFloat(poArgs, 1, &fTarget))
		return Py_BuildException();

	return Py_BuildValue("f", GetDegreeDifference(fSource, fTarget));
}

PyObject * appSleep(PyObject * poSelf, PyObject * poArgs)
{
	int	iTime;
	if (!PyTuple_GetInteger(poArgs, 0, &iTime))
		return Py_BuildException();

	Sleep(iTime);

	return Py_BuildNone();
}

PyObject * appSetDefaultFontName(PyObject * poSelf, PyObject * poArgs)
{
	char * szFontName;
	if (!PyTuple_GetString(poArgs, 0, &szFontName))
		return Py_BuildException();

	// DEFAULT_FONT
	DefaultFont_SetName(szFontName);
	// END_OF_DEFAULT_FONT

	return Py_BuildNone();
}

PyObject * appSetGuildSymbolPath(PyObject * poSelf, PyObject * poArgs)
{
	char * szPathName;
	if (!PyTuple_GetString(poArgs, 0, &szPathName))
		return Py_BuildException();

	SetGuildSymbolPath(szPathName);

	return Py_BuildNone();
}

PyObject * appEnableSpecialCameraMode(PyObject * poSelf, PyObject * poArgs)
{
	CPythonApplication::Instance().EnableSpecialCameraMode();
	return Py_BuildNone();
}

PyObject * appSetCameraSpeed(PyObject * poSelf, PyObject * poArgs)
{
	int iPercentage;
	if (!PyTuple_GetInteger(poArgs, 0, &iPercentage))
		return Py_BuildException();

	CPythonApplication::Instance().SetCameraSpeed(iPercentage);

	CCamera * pCamera = CCameraManager::Instance().GetCurrentCamera();
	if (pCamera)
		pCamera->SetResistance(float(iPercentage) / 100.0f);
	return Py_BuildNone();
}

PyObject * appIsFileExist(PyObject * poSelf, PyObject * poArgs)
{
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	return Py_BuildValue("i", -1 != _access(szFileName, 0));
}

PyObject * appSetCameraSetting(PyObject * poSelf, PyObject * poArgs)
{
	int ix;
	if (!PyTuple_GetInteger(poArgs, 0, &ix))
		return Py_BuildException();
	int iy;
	if (!PyTuple_GetInteger(poArgs, 1, &iy))
		return Py_BuildException();
	int iz;
	if (!PyTuple_GetInteger(poArgs, 2, &iz))
		return Py_BuildException();

	int iZoom;
	if (!PyTuple_GetInteger(poArgs, 3, &iZoom))
		return Py_BuildException();
	int iRotation;
	if (!PyTuple_GetInteger(poArgs, 4, &iRotation))
		return Py_BuildException();
	int iPitch;
	if (!PyTuple_GetInteger(poArgs, 5, &iPitch))
		return Py_BuildException();

	CPythonApplication::SCameraSetting CameraSetting;
	ZeroMemory(&CameraSetting, sizeof(CameraSetting));
	CameraSetting.v3CenterPosition.x = float(ix);
	CameraSetting.v3CenterPosition.y = float(iy);
	CameraSetting.v3CenterPosition.z = float(iz);
	CameraSetting.fZoom = float(iZoom);
	CameraSetting.fRotation = float(iRotation);
	CameraSetting.fPitch = float(iPitch);
	CPythonApplication::Instance().SetEventCamera(CameraSetting);
	return Py_BuildNone();
}

PyObject * appSaveCameraSetting(PyObject * poSelf, PyObject * poArgs)
{
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CPythonApplication::Instance().SaveCameraSetting(szFileName);
	return Py_BuildNone();
}

PyObject * appLoadCameraSetting(PyObject * poSelf, PyObject * poArgs)
{
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	bool bResult = CPythonApplication::Instance().LoadCameraSetting(szFileName);
	return Py_BuildValue("i", bResult);
}

PyObject * appSetDefaultCamera(PyObject * poSelf, PyObject * poArgs)
{
	CPythonApplication::Instance().SetDefaultCamera();
	return Py_BuildNone();
}

PyObject * appSetSightRange(PyObject * poSelf, PyObject * poArgs)
{
	int iRange;
	if (!PyTuple_GetInteger(poArgs, 0, &iRange))
		return Py_BuildException();

	CPythonApplication::Instance().SetForceSightRange(iRange);
	return Py_BuildNone();
}

extern int g_iAccumulationTime;

PyObject * apptestGetAccumulationTime(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", g_iAccumulationTime);
}

PyObject * apptestResetAccumulationTime(PyObject * poSelf, PyObject * poArgs)
{
	g_iAccumulationTime = 0;
	return Py_BuildNone();
}

PyObject * apptestSetSpecularColor(PyObject * poSelf, PyObject * poArgs)
{
	float fr;
	if (!PyTuple_GetFloat(poArgs, 0, &fr))
		return Py_BuildException();
	float fg;
	if (!PyTuple_GetFloat(poArgs, 1, &fg))
		return Py_BuildException();
	float fb;
	if (!PyTuple_GetFloat(poArgs, 2, &fb))
		return Py_BuildException();
	g_fSpecularColor = D3DXCOLOR(fr, fg, fb, 1.0f);
	return Py_BuildNone();
}

PyObject * appSetVisibleNotice(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();
	bVisibleNotice = iFlag;
	return Py_BuildNone();
}

PyObject * appIsVisibleNotice(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", bVisibleNotice);
}

PyObject * appEnableTestServerFlag(PyObject * poSelf, PyObject * poArgs)
{
	bTestServerFlag = TRUE;
	return Py_BuildNone();
}

PyObject * appIsEnableTestServerFlag(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", bTestServerFlag);
}

class CTextLineLoader
{
	public:
		CTextLineLoader(const char * c_szFileName)
		{
			const VOID* pvData;
			CMappedFile kFile;
			if (!CFileSystem::Instance().Get(kFile, c_szFileName, &pvData, __FUNCTION__))
				return;

			m_kTextFileLoader.Bind(kFile.Size(), pvData);
		}

		DWORD GetLineCount()
		{
			return m_kTextFileLoader.GetLineCount();
		}

		const char * GetLine(DWORD dwIndex)
		{
			if (dwIndex >= GetLineCount())
				return "";

			return m_kTextFileLoader.GetLineString(dwIndex).c_str();
		}

	protected:
		CMemoryTextFileLoader m_kTextFileLoader;
};

PyObject * appOpenTextFile(PyObject * poSelf, PyObject * poArgs)
{
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CTextLineLoader * pTextLineLoader = new CTextLineLoader(szFileName);

	return Py_BuildValue("i", (int)pTextLineLoader);
}

PyObject * appCloseTextFile(PyObject * poSelf, PyObject * poArgs)
{
	int iHandle;
	if (!PyTuple_GetInteger(poArgs, 0, &iHandle))
		return Py_BuildException();

	CTextLineLoader * pTextFileLoader = (CTextLineLoader *)iHandle;
	delete pTextFileLoader;

	return Py_BuildNone();
}

PyObject * appGetTextFileLineCount(PyObject * poSelf, PyObject * poArgs)
{
	int iHandle;
	if (!PyTuple_GetInteger(poArgs, 0, &iHandle))
		return Py_BuildException();

	CTextLineLoader * pTextFileLoader = (CTextLineLoader *)iHandle;
	return Py_BuildValue("i", pTextFileLoader->GetLineCount());
}

PyObject * appGetTextFileLine(PyObject * poSelf, PyObject * poArgs)
{
	int iHandle;
	if (!PyTuple_GetInteger(poArgs, 0, &iHandle))
		return Py_BuildException();
	int iLineIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iLineIndex))
		return Py_BuildException();

	CTextLineLoader * pTextFileLoader = (CTextLineLoader *)iHandle;
	return Py_BuildValue("s", pTextFileLoader->GetLine(iLineIndex));
}

PyObject * appSetGuildMarkPath(PyObject * poSelf, PyObject * poArgs)
{
	char * path;
	if (!PyTuple_GetString(poArgs, 0, &path))
		return Py_BuildException();

    char newPath[256];
    char * ext = strstr(path, ".tga");

    if (ext)
    {
		int extPos = ext - path;
        strncpy(newPath, path, extPos);
        newPath[extPos] = '\0';
    }
    else
        strncpy(newPath, path, sizeof(newPath)-1);

	CGuildMarkManager::Instance().SetMarkPathPrefix(newPath);
	return Py_BuildNone();
}

PyObject* appIsDevStage(PyObject* poSelf, PyObject* poArgs)
{
	int nIsDevelopmentStage = 0;
#if defined(LOCALE_SERVICE_STAGE_DEVELOPMENT)
	nIsDevelopmentStage = 1;
#endif
	return Py_BuildValue("i", nIsDevelopmentStage);
}

PyObject* appIsTestStage(PyObject* poSelf, PyObject* poArgs)
{
	int nIsTestStage = 0;
#if defined(LOCALE_SERVICE_STAGE_TEST)
	nIsTestStage = 1;
#endif
	return Py_BuildValue("i", nIsTestStage);
}

PyObject* appIsLiveStage(PyObject* poSelf, PyObject* poArgs)
{
	int nIsLiveStage = 0;
#if !defined(LOCALE_SERVICE_STAGE_TEST) && !defined(LOCALE_SERVICE_STAGE_DEVELOPMENT)
	nIsLiveStage = 1;
#endif
	return Py_BuildValue("i", nIsLiveStage);
}

PyObject* appLogoOpen(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	int nIsSuccess = 1; //CPythonApplication::Instance().OnLogoOpen(szName);
	CMovieMan::Instance().PlayLogo(szName);

	return Py_BuildValue("i", nIsSuccess);
}

PyObject* appLogoUpdate(PyObject* poSelf, PyObject* poArgs)
{
	int nIsRun = 0; //CPythonApplication::Instance().OnLogoUpdate();
	return Py_BuildValue("i", nIsRun);
}

PyObject* appLogoRender(PyObject* poSelf, PyObject* poArgs)
{
	//CPythonApplication::Instance().OnLogoRender();
	return Py_BuildNone();
}

PyObject* appLogoClose(PyObject* poSelf, PyObject* poArgs)
{
	//CPythonApplication::Instance().OnLogoClose();
	return Py_BuildNone();
}

PyObject* appFlashApplication(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().FlashApplication();
	return Py_BuildNone();
}

#if defined(ENABLE_MULTI_LANGUAGE)
PyObject* appShellExecute(PyObject* poSelf, PyObject* poArgs)
{
	TCHAR buf[FILENAME_MAX];
	GetModuleFileName(nullptr, buf, FILENAME_MAX);

	SHELLEXECUTEINFO sei = {0};
	sei.cbSize = sizeof(sei);
	sei.nShow = SW_SHOWNORMAL;
	sei.lpFile = TEXT(buf);
	sei.fMask = SEE_MASK_CLASSNAME;
	sei.lpVerb = TEXT("open");
	sei.lpClass = TEXT("exefile");
	ShellExecuteEx(&sei);

	CPythonApplication::Instance().Exit();
	return Py_BuildNone();
}
#endif

PyObject *appEncryptLocal(PyObject* /*poSelf*/,
                            PyObject* poArgs)
{
    char *data;
    Py_ssize_t size;
    if (!PyTuple_GetStringAndSize(poArgs, 0, &data, &size))
    {
        return Py_BadArgument();
    }

    if (data)
    {
        DATA_BLOB input, output;
        input.pbData = (uint8_t *)data;
        input.cbData = size;

        PyObject *resultStr = nullptr;

        if (CryptProtectData(&input, L"METIN2_LOCAL", nullptr, nullptr, nullptr,
                             CRYPTPROTECT_LOCAL_MACHINE | CRYPTPROTECT_UI_FORBIDDEN, &output))
        {
            resultStr = PyString_FromStringAndSize(reinterpret_cast<char *>(output.pbData), output.cbData);

            LocalFree(output.pbData);
        }
        else
        {
            PyErr_SetFromWindowsErr(GetLastError());
        }

        return resultStr;
    }

    Py_RETURN_NONE;
}

PyObject *appDecryptLocal(PyObject* /*poSelf*/,
                            PyObject* poArgs)
{
    char *data;
    Py_ssize_t size;
    if (!PyTuple_GetStringAndSize(poArgs, 0, &data, &size))
    {
        return Py_BadArgument();
    }

    if (data)
    {
        DATA_BLOB input, output;
        input.pbData = (uint8_t *)data;
        input.cbData = size;

        PyObject *resultStr = nullptr;
        if (CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &output))
        {
            resultStr = PyString_FromStringAndSize(reinterpret_cast<char *>(output.pbData), output.cbData);

            LocalFree(output.pbData);
        }
        else
        {
            PyErr_SetFromWindowsErr(GetLastError());
        }

        return resultStr;
    }

    Py_RETURN_NONE;
}

void initapp()
{
	static PyMethodDef s_methods[] =
	{
		{ "IsDevStage",					appIsDevStage,					METH_VARARGS },
		{ "IsTestStage",				appIsTestStage,					METH_VARARGS },
		{ "IsLiveStage",				appIsLiveStage,					METH_VARARGS },
		// TEXTTAIL_LIVINGTIME_CONTROL
		{ "SetTextTailLivingTime",		appSetTextTailLivingTime,		METH_VARARGS },
		// END_OF_TEXTTAIL_LIVINGTIME_CONTROL

		{ "EnablePerformanceTime",		appEnablePerformanceTime,		METH_VARARGS },
		{ "SetHairColorEnable",			appSetHairColorEnable,			METH_VARARGS },

		{ "SetArmorSpecularEnable",		appSetArmorSpecularEnable,		METH_VARARGS },
		{ "SetWeaponSpecularEnable",	appSetWeaponSpecularEnable,		METH_VARARGS },
		{ "SetSkillEffectUpgradeEnable",appSetSkillEffectUpgradeEnable,	METH_VARARGS },
		{ "SetTwoHandedWeaponAttSpeedDecreaseValue", SetTwoHandedWeaponAttSpeedDecreaseValue, METH_VARARGS },
		{ "SetRideHorseEnable",			appSetRideHorseEnable,			METH_VARARGS },

		{ "SetCameraMaxDistance",		appSetCameraMaxDistance,		METH_VARARGS },
		{ "SetMinFog",					appSetMinFog,					METH_VARARGS },
		{ "SetFrameSkip",				appSetFrameSkip,				METH_VARARGS },
		{ "GetImageInfo",				appGetImageInfo,				METH_VARARGS },
		{ "GetInfo",					appGetInfo,						METH_VARARGS },
		{ "UpdateGame",					appUpdateGame,					METH_VARARGS },
		{ "RenderGame",					appRenderGame,					METH_VARARGS },
		{ "Loop",						appLoop,						METH_VARARGS },
		{ "Create",						appCreate,						METH_VARARGS },
		{ "Process",					appProcess,						METH_VARARGS },
		{ "Exit",						appExit,						METH_VARARGS },
		{ "Abort",						appAbort,						METH_VARARGS },
		{ "SetMouseHandler",			appSetMouseHandler,				METH_VARARGS },
		{ "IsExistFile",				appIsExistFile,					METH_VARARGS },
		{ "GetFileList",				appGetFileList,					METH_VARARGS },

		{ "SetCamera",					appSetCamera,					METH_VARARGS },
		{ "GetCamera",					appGetCamera,					METH_VARARGS },
		{ "GetCameraPitch",				appGetCameraPitch,				METH_VARARGS },
		{ "GetCameraRotation",			appGetCameraRotation,			METH_VARARGS },
		{ "GetTime",					appGetTime,						METH_VARARGS },
		{ "GetGlobalTime",				appGetGlobalTime,				METH_VARARGS },
		{ "GetGlobalTimeStamp",			appGetGlobalTimeStamp,			METH_VARARGS },
		{ "GetUpdateFPS",				appGetUpdateFPS,				METH_VARARGS },
		{ "GetRenderFPS",				appGetRenderFPS,				METH_VARARGS },
		{ "RotateCamera",				appRotateCamera,				METH_VARARGS },
		{ "PitchCamera",				appPitchCamera,					METH_VARARGS },
		{ "ZoomCamera",					appZoomCamera,					METH_VARARGS },
		{ "MovieRotateCamera",			appMovieRotateCamera,			METH_VARARGS },
		{ "MoviePitchCamera",			appMoviePitchCamera,			METH_VARARGS },
		{ "MovieZoomCamera",			appMovieZoomCamera,				METH_VARARGS },
		{ "MovieResetCamera",			appMovieResetCamera,			METH_VARARGS },

		{ "GetAvailableTextureMemory",	appGetAvaiableTextureMememory,	METH_VARARGS },
		{ "GetRenderTime",				appGetRenderTime,				METH_VARARGS },
		{ "GetUpdateTime",				appGetUpdateTime,				METH_VARARGS },
		{ "GetLoad",					appGetLoad,						METH_VARARGS },
		{ "GetFaceSpeed",				appGetFaceSpeed,				METH_VARARGS },
		{ "GetFaceCount",				appGetFaceCount,				METH_VARARGS },
		{ "SetFPS",						appSetFPS,						METH_VARARGS },
		{ "SetGlobalCenterPosition",	appSetGlobalCenterPosition,		METH_VARARGS },
		{ "SetCenterPosition",			appSetCenterPosition,			METH_VARARGS },
		{ "GetCursorPosition",			appGetCursorPosition,			METH_VARARGS },

		{ "GetRandom",					appGetRandom,					METH_VARARGS },
		{ "IsPressed",					appIsPressed,					METH_VARARGS },
		{ "SetCursor",					appSetCursor,					METH_VARARGS },
		{ "GetCursor",					appGetCursor,					METH_VARARGS },
		{ "ShowCursor",					appShowCursor,					METH_VARARGS },
		{ "HideCursor",					appHideCursor,					METH_VARARGS },
		{ "IsShowCursor",				appIsShowCursor,				METH_VARARGS },
		{ "IsLiarCursorOn",				appIsLiarCursorOn,				METH_VARARGS },
		{ "SetSoftwareCursor",			appSetSoftwareCursor,			METH_VARARGS },
		{ "SetHardwareCursor",			appSetHardwareCursor,			METH_VARARGS },

		{ "SetConnectData",				appSetConnectData,				METH_VARARGS },
		{ "GetConnectData",				appGetConnectData,				METH_VARARGS },

		{ "GetRotatingDirection",		appGetRotatingDirection,		METH_VARARGS },
		{ "GetDegreeDifference",		appGetDegreeDifference,			METH_VARARGS },
		{ "Sleep",						appSleep,						METH_VARARGS },
		{ "SetDefaultFontName",			appSetDefaultFontName,			METH_VARARGS },
		{ "SetGuildSymbolPath",			appSetGuildSymbolPath,			METH_VARARGS },

		{ "EnableSpecialCameraMode",	appEnableSpecialCameraMode,		METH_VARARGS },
		{ "SetCameraSpeed",				appSetCameraSpeed,				METH_VARARGS },

		{ "SaveCameraSetting",			appSaveCameraSetting,			METH_VARARGS },
		{ "LoadCameraSetting",			appLoadCameraSetting,			METH_VARARGS },
		{ "SetDefaultCamera",			appSetDefaultCamera,			METH_VARARGS },
		{ "SetCameraSetting",			appSetCameraSetting,			METH_VARARGS },

		{ "SetSightRange",				appSetSightRange,				METH_VARARGS },

		{ "IsFileExist",				appIsFileExist,					METH_VARARGS },
		{ "OpenTextFile",				appOpenTextFile,				METH_VARARGS },
		{ "CloseTextFile",				appCloseTextFile,				METH_VARARGS },
		{ "GetTextFileLineCount",		appGetTextFileLineCount,		METH_VARARGS },
		{ "GetTextFileLine",			appGetTextFileLine,				METH_VARARGS },

		// LOCALE
		{ "GetLocaleServiceName",		appGetLocaleServiceName,		METH_VARARGS },
		{ "GetLocaleName",				appGetLocaleName,				METH_VARARGS },
		{ "GetLocalePath",				appGetLocalePath,				METH_VARARGS },
		{ "ForceSetLocale",				appForceSetLocale,				METH_VARARGS },
		// END_OF_LOCALE

		// CHEONMA
		{ "LoadLocaleAddr",				appLoadLocaleAddr,				METH_VARARGS },
		{ "LoadLocaleData",				appLoadLocaleData,				METH_VARARGS },
		{ "SetCHEONMA",					appSetCHEONMA,					METH_VARARGS },
		{ "IsCHEONMA",					appIsCHEONMA,					METH_VARARGS },
		// END_OF_CHEONMA

		{ "GetDefaultCodePage",			appGetDefaultCodePage,			METH_VARARGS },
		{ "SetControlFP",				appSetControlFP,				METH_VARARGS },
		{ "SetSpecularSpeed",			appSetSpecularSpeed,			METH_VARARGS },

		{ "testGetAccumulationTime",	apptestGetAccumulationTime,		METH_VARARGS },
		{ "testResetAccumulationTime",	apptestResetAccumulationTime,	METH_VARARGS },
		{ "testSetSpecularColor",		apptestSetSpecularColor,		METH_VARARGS },

		{ "SetVisibleNotice",			appSetVisibleNotice,			METH_VARARGS },
		{ "IsVisibleNotice",			appIsVisibleNotice,				METH_VARARGS },
		{ "EnableTestServerFlag",		appEnableTestServerFlag,		METH_VARARGS },
		{ "IsEnableTestServerFlag",		appIsEnableTestServerFlag,		METH_VARARGS },

		{ "SetGuildMarkPath",			appSetGuildMarkPath,			METH_VARARGS },

		{ "OnLogoUpdate",				appLogoUpdate,					METH_VARARGS },
		{ "OnLogoRender",				appLogoRender,					METH_VARARGS },
		{ "OnLogoOpen",					appLogoOpen,					METH_VARARGS },
		{ "OnLogoClose",				appLogoClose,					METH_VARARGS },
		{ "FlashApplication",			appFlashApplication,			METH_VARARGS },
#if defined(ENABLE_MULTI_LANGUAGE)
		{"ShellExecute", appShellExecute, METH_VARARGS},
#endif
        {
            "EncryptByComputer",
            appEncryptLocal,
            METH_VARARGS
        },
        {
            "DecryptByComputer",
            appDecryptLocal,
            METH_VARARGS
        },
        {
            NULL,
            NULL
        },
	};

	PyObject * poModule = Py_InitModule("app", s_methods);
#ifdef ENABLE_NEW_GAMEOPTION
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_GAMEOPTION", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_GAMEOPTION", 0);
#endif
	PyModule_AddIntConstant(poModule, "INFO_ITEM",		CPythonApplication::INFO_ITEM);
	PyModule_AddIntConstant(poModule, "INFO_ACTOR",		CPythonApplication::INFO_ACTOR);
	PyModule_AddIntConstant(poModule, "INFO_EFFECT",	CPythonApplication::INFO_EFFECT);
	PyModule_AddIntConstant(poModule, "INFO_TEXTTAIL",	CPythonApplication::INFO_TEXTTAIL);

	PyModule_AddIntConstant(poModule, "DEGREE_DIRECTION_SAME",		DEGREE_DIRECTION_SAME);
	PyModule_AddIntConstant(poModule, "DEGREE_DIRECTION_RIGHT",		DEGREE_DIRECTION_RIGHT);
	PyModule_AddIntConstant(poModule, "DEGREE_DIRECTION_LEFT",		DEGREE_DIRECTION_LEFT);

	PyModule_AddIntConstant(poModule, "VK_LEFT",	     VK_LEFT);
	PyModule_AddIntConstant(poModule, "VK_RIGHT",	     VK_RIGHT);
	PyModule_AddIntConstant(poModule, "VK_UP",		     VK_UP);
	PyModule_AddIntConstant(poModule, "VK_DOWN",	     VK_DOWN);
	PyModule_AddIntConstant(poModule, "VK_HOME",	     VK_HOME);
	PyModule_AddIntConstant(poModule, "VK_END",          VK_END);
	PyModule_AddIntConstant(poModule, "VK_DELETE",	     VK_DELETE);

	PyModule_AddIntConstant(poModule, "DIK_ESCAPE",      DIK_ESCAPE);
	PyModule_AddIntConstant(poModule, "DIK_ESC",         DIK_ESCAPE);	// 편의를 위해
	PyModule_AddIntConstant(poModule, "DIK_1",           DIK_1);
	PyModule_AddIntConstant(poModule, "DIK_2",           DIK_2);
	PyModule_AddIntConstant(poModule, "DIK_3",           DIK_3);
	PyModule_AddIntConstant(poModule, "DIK_4",           DIK_4);
	PyModule_AddIntConstant(poModule, "DIK_5",           DIK_5);
	PyModule_AddIntConstant(poModule, "DIK_6",           DIK_6);
	PyModule_AddIntConstant(poModule, "DIK_7",           DIK_7);
	PyModule_AddIntConstant(poModule, "DIK_8",           DIK_8);
	PyModule_AddIntConstant(poModule, "DIK_9",           DIK_9);
	PyModule_AddIntConstant(poModule, "DIK_0",           DIK_0);
	PyModule_AddIntConstant(poModule, "DIK_MINUS",       DIK_MINUS);        /* - on main keyboard */
	PyModule_AddIntConstant(poModule, "DIK_EQUALS",      DIK_EQUALS);
	PyModule_AddIntConstant(poModule, "DIK_BACK",        DIK_BACK);           /* backspace */
	PyModule_AddIntConstant(poModule, "DIK_TAB",         DIK_TAB);
	PyModule_AddIntConstant(poModule, "DIK_Q",           DIK_Q);
	PyModule_AddIntConstant(poModule, "DIK_W",           DIK_W);
	PyModule_AddIntConstant(poModule, "DIK_E",           DIK_E);
	PyModule_AddIntConstant(poModule, "DIK_R",           DIK_R);
	PyModule_AddIntConstant(poModule, "DIK_T",           DIK_T);
	PyModule_AddIntConstant(poModule, "DIK_Y",           DIK_Y);
	PyModule_AddIntConstant(poModule, "DIK_U",           DIK_U);
	PyModule_AddIntConstant(poModule, "DIK_I",           DIK_I);
	PyModule_AddIntConstant(poModule, "DIK_O",           DIK_O);
	PyModule_AddIntConstant(poModule, "DIK_P",           DIK_P);
	PyModule_AddIntConstant(poModule, "DIK_LBRACKET",    DIK_LBRACKET);
	PyModule_AddIntConstant(poModule, "DIK_RBRACKET",    DIK_RBRACKET);
	PyModule_AddIntConstant(poModule, "DIK_RETURN",      DIK_RETURN);         /* Enter on main keyboard */
	PyModule_AddIntConstant(poModule, "DIK_LCONTROL",    DIK_LCONTROL);
	PyModule_AddIntConstant(poModule, "DIK_A",           DIK_A);
	PyModule_AddIntConstant(poModule, "DIK_S",           DIK_S);
	PyModule_AddIntConstant(poModule, "DIK_D",           DIK_D);
	PyModule_AddIntConstant(poModule, "DIK_F",           DIK_F);
	PyModule_AddIntConstant(poModule, "DIK_G",           DIK_G);
	PyModule_AddIntConstant(poModule, "DIK_H",           DIK_H);
	PyModule_AddIntConstant(poModule, "DIK_J",           DIK_J);
	PyModule_AddIntConstant(poModule, "DIK_K",           DIK_K);
	PyModule_AddIntConstant(poModule, "DIK_L",           DIK_L);
	PyModule_AddIntConstant(poModule, "DIK_SEMICOLON",   DIK_SEMICOLON);
	PyModule_AddIntConstant(poModule, "DIK_APOSTROPHE",  DIK_APOSTROPHE);
	PyModule_AddIntConstant(poModule, "DIK_GRAVE",       DIK_GRAVE);          /* accent grave */
	PyModule_AddIntConstant(poModule, "DIK_LSHIFT",      DIK_LSHIFT);
	PyModule_AddIntConstant(poModule, "DIK_BACKSLASH",   DIK_BACKSLASH);
	PyModule_AddIntConstant(poModule, "DIK_Z",           DIK_Z);
	PyModule_AddIntConstant(poModule, "DIK_X",           DIK_X);
	PyModule_AddIntConstant(poModule, "DIK_C",           DIK_C);
	PyModule_AddIntConstant(poModule, "DIK_V",           DIK_V);
	PyModule_AddIntConstant(poModule, "DIK_B",           DIK_B);
	PyModule_AddIntConstant(poModule, "DIK_N",           DIK_N);
	PyModule_AddIntConstant(poModule, "DIK_M",           DIK_M);
	PyModule_AddIntConstant(poModule, "DIK_COMMA",       DIK_COMMA);
	PyModule_AddIntConstant(poModule, "DIK_PERIOD",      DIK_PERIOD);         /* . on main keyboard */
	PyModule_AddIntConstant(poModule, "DIK_SLASH",       DIK_SLASH);          /* / on main keyboard */
	PyModule_AddIntConstant(poModule, "DIK_RSHIFT",      DIK_RSHIFT);
	PyModule_AddIntConstant(poModule, "DIK_MULTIPLY",    DIK_MULTIPLY);       /* * on numeric keypad */
	PyModule_AddIntConstant(poModule, "DIK_LALT",        DIK_LMENU);          /* left Alt */
	PyModule_AddIntConstant(poModule, "DIK_SPACE",       DIK_SPACE);
	PyModule_AddIntConstant(poModule, "DIK_CAPITAL",     DIK_CAPITAL);
	PyModule_AddIntConstant(poModule, "DIK_F1",          DIK_F1);
	PyModule_AddIntConstant(poModule, "DIK_F2",          DIK_F2);
	PyModule_AddIntConstant(poModule, "DIK_F3",          DIK_F3);
	PyModule_AddIntConstant(poModule, "DIK_F4",          DIK_F4);
	PyModule_AddIntConstant(poModule, "DIK_F5",          DIK_F5);
	PyModule_AddIntConstant(poModule, "DIK_F6",          DIK_F6);
	PyModule_AddIntConstant(poModule, "DIK_F7",          DIK_F7);
	PyModule_AddIntConstant(poModule, "DIK_F8",          DIK_F8);
	PyModule_AddIntConstant(poModule, "DIK_F9",          DIK_F9);
	PyModule_AddIntConstant(poModule, "DIK_F10",         DIK_F10);
	PyModule_AddIntConstant(poModule, "DIK_NUMLOCK",     DIK_NUMLOCK);
	PyModule_AddIntConstant(poModule, "DIK_SCROLL",      DIK_SCROLL);         /* Scroll Lock */
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD7",     DIK_NUMPAD7);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD8",     DIK_NUMPAD8);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD9",     DIK_NUMPAD9);
	PyModule_AddIntConstant(poModule, "DIK_SUBTRACT",    DIK_SUBTRACT);       /* - on numeric keypad */
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD4",     DIK_NUMPAD4);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD5",     DIK_NUMPAD5);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD6",     DIK_NUMPAD6);
	PyModule_AddIntConstant(poModule, "DIK_ADD",         DIK_ADD);            /* + on numeric keypad */
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD1",     DIK_NUMPAD1);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD2",     DIK_NUMPAD2);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD3",     DIK_NUMPAD3);
	PyModule_AddIntConstant(poModule, "DIK_NUMPAD0",     DIK_NUMPAD0);
	PyModule_AddIntConstant(poModule, "DIK_DECIMAL",     DIK_DECIMAL);        /* . on numeric keypad */
	PyModule_AddIntConstant(poModule, "DIK_F11",         DIK_F11);
	PyModule_AddIntConstant(poModule, "DIK_F12",         DIK_F12);
	PyModule_AddIntConstant(poModule, "DIK_NEXTTRACK",   DIK_NEXTTRACK);      /* Next Track */
	PyModule_AddIntConstant(poModule, "DIK_NUMPADENTER", DIK_NUMPADENTER);    /* Enter on numeric keypad */
	PyModule_AddIntConstant(poModule, "DIK_RCONTROL",    DIK_RCONTROL);
	PyModule_AddIntConstant(poModule, "DIK_MUTE",        DIK_MUTE);           /* Mute */
	PyModule_AddIntConstant(poModule, "DIK_CALCULATOR",  DIK_CALCULATOR);     /* Calculator */
	PyModule_AddIntConstant(poModule, "DIK_PLAYPAUSE",   DIK_PLAYPAUSE);      /* Play / Pause */
	PyModule_AddIntConstant(poModule, "DIK_MEDIASTOP",   DIK_MEDIASTOP);      /* Media Stop */
	PyModule_AddIntConstant(poModule, "DIK_VOLUMEDOWN",  DIK_VOLUMEDOWN);     /* Volume - */
	PyModule_AddIntConstant(poModule, "DIK_VOLUMEUP",    DIK_VOLUMEUP);       /* Volume + */
	PyModule_AddIntConstant(poModule, "DIK_WEBHOME",     DIK_WEBHOME);        /* Web home */
	PyModule_AddIntConstant(poModule, "DIK_NUMPADCOMMA", DIK_NUMPADCOMMA);    /* , on numeric keypad (NEC PC98) */
	PyModule_AddIntConstant(poModule, "DIK_DIVIDE",      DIK_DIVIDE);         /* / on numeric keypad */
	PyModule_AddIntConstant(poModule, "DIK_SYSRQ",       DIK_SYSRQ);
	PyModule_AddIntConstant(poModule, "DIK_RALT",        DIK_RMENU);          /* right Alt */
	PyModule_AddIntConstant(poModule, "DIK_PAUSE",       DIK_PAUSE);          /* Pause */
	PyModule_AddIntConstant(poModule, "DIK_HOME",        DIK_HOME);           /* Home on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_UP",          DIK_UP);             /* UpArrow on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_PGUP",        DIK_PRIOR);          /* PgUp on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_LEFT",        DIK_LEFT);           /* LeftArrow on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_RIGHT",       DIK_RIGHT);          /* RightArrow on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_END",         DIK_END);            /* End on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_DOWN",        DIK_DOWN);           /* DownArrow on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_PGDN",        DIK_NEXT);           /* PgDn on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_INSERT",      DIK_INSERT);         /* Insert on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_DELETE",      DIK_DELETE);         /* Delete on arrow keypad */
	PyModule_AddIntConstant(poModule, "DIK_LWIN",        DIK_LWIN);           /* Left Windows key */
	PyModule_AddIntConstant(poModule, "DIK_RWIN",        DIK_RWIN);           /* Right Windows key */
	PyModule_AddIntConstant(poModule, "DIK_APPS",        DIK_APPS);           /* AppMenu key */

	// Cursor
	PyModule_AddIntConstant(poModule, "NORMAL",			CPythonApplication::CURSOR_SHAPE_NORMAL);
	PyModule_AddIntConstant(poModule, "ATTACK",			CPythonApplication::CURSOR_SHAPE_ATTACK);
	PyModule_AddIntConstant(poModule, "TARGET",			CPythonApplication::CURSOR_SHAPE_TARGET);
	PyModule_AddIntConstant(poModule, "TALK",			CPythonApplication::CURSOR_SHAPE_TALK);
	PyModule_AddIntConstant(poModule, "CANT_GO",		CPythonApplication::CURSOR_SHAPE_CANT_GO);
	PyModule_AddIntConstant(poModule, "PICK",			CPythonApplication::CURSOR_SHAPE_PICK);

	PyModule_AddIntConstant(poModule, "DOOR",			CPythonApplication::CURSOR_SHAPE_DOOR);
	PyModule_AddIntConstant(poModule, "CHAIR",			CPythonApplication::CURSOR_SHAPE_CHAIR);
	PyModule_AddIntConstant(poModule, "MAGIC",			CPythonApplication::CURSOR_SHAPE_MAGIC);
	PyModule_AddIntConstant(poModule, "BUY",			CPythonApplication::CURSOR_SHAPE_BUY);
	PyModule_AddIntConstant(poModule, "SELL",			CPythonApplication::CURSOR_SHAPE_SELL);

	PyModule_AddIntConstant(poModule, "CAMERA_ROTATE",	CPythonApplication::CURSOR_SHAPE_CAMERA_ROTATE);
	PyModule_AddIntConstant(poModule, "HSIZE",			CPythonApplication::CURSOR_SHAPE_HSIZE);
	PyModule_AddIntConstant(poModule, "VSIZE",			CPythonApplication::CURSOR_SHAPE_VSIZE);
	PyModule_AddIntConstant(poModule, "HVSIZE",			CPythonApplication::CURSOR_SHAPE_HVSIZE);

	PyModule_AddIntConstant(poModule, "CAMERA_TO_POSITIVE",		CPythonApplication::CAMERA_TO_POSITIVE);
	PyModule_AddIntConstant(poModule, "CAMERA_TO_NEGATIVE",		CPythonApplication::CAMERA_TO_NEGITIVE);
	PyModule_AddIntConstant(poModule, "CAMERA_STOP",			CPythonApplication::CAMERA_STOP);
#if defined(ENABLE_NEW_FISH_EVENT)
	PyModule_AddIntConstant(poModule, "FISH_EVENT_SHAPE_1", FISH_EVENT_SHAPE_1);
	PyModule_AddIntConstant(poModule, "FISH_EVENT_SHAPE_2", FISH_EVENT_SHAPE_2);
	PyModule_AddIntConstant(poModule, "FISH_EVENT_SHAPE_3", FISH_EVENT_SHAPE_3);
	PyModule_AddIntConstant(poModule, "FISH_EVENT_SHAPE_4", FISH_EVENT_SHAPE_4);
	PyModule_AddIntConstant(poModule, "FISH_EVENT_SHAPE_5", FISH_EVENT_SHAPE_5);
	PyModule_AddIntConstant(poModule, "FISH_EVENT_SHAPE_6", FISH_EVENT_SHAPE_6);
	PyModule_AddIntConstant(poModule, "FISH_EVENT_SHAPE_7", FISH_EVENT_SHAPE_7);
#endif
#ifdef __EFFETTO_MANTELLO__
	PyModule_AddIntConstant(poModule, "EFFECT_MANTELLO", 1);
#else
	PyModule_AddIntConstant(poModule, "EFFECT_MANTELLO", 0);
#endif

#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_WHISPER_ADMIN_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_WHISPER_ADMIN_SYSTEM",	0);
#endif

#ifdef NEW_PET_SYSTEM
	PyModule_AddIntConstant(poModule, "NEW_PET_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "NEW_PET_SYSTEM",	0);
#endif

#ifdef ENABLE_PREMIUM_PLAYERS
	PyModule_AddIntConstant(poModule, "ENABLE_PREMIUM_PLAYERS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_PREMIUM_PLAYERS", 0);
#endif
#ifdef ENABLE_CONFIG_MODULE
	PyModule_AddIntConstant(poModule, "ENABLE_CONFIG_MODULE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CONFIG_MODULE", 0);
#endif

#ifdef ENABLE_SOUL_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_SOUL_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SOUL_SYSTEM",	0);
#endif

#ifdef ENABLE_BUY_WITH_ITEM
    PyModule_AddIntConstant(poModule, "ENABLE_BUY_WITH_ITEM", 1);
#else
    PyModule_AddIntConstant(poModule, "ENABLE_BUY_WITH_ITEM", 0);
#endif

#ifdef ATTR_LOCK
	PyModule_AddIntConstant(poModule, "ATTR_LOCK", 1);
#else
	PyModule_AddIntConstant(poModule, "ATTR_LOCK", 0);
#endif
#ifdef ENABLE_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_COSTUME_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_COSTUME_SYSTEM",	0);
#endif

#if defined(ITEM_CHECKINOUT_UPDATE)
	PyModule_AddIntConstant(poModule, "ITEM_CHECKINOUT_UPDATE", true);
#else
	PyModule_AddIntConstant(poModule, "ITEM_CHECKINOUT_UPDATE", false);
#endif

#ifdef ENABLE_SWITCHBOT_WORLDARD
	PyModule_AddIntConstant(poModule, "ENABLE_SWITCHBOT_WORLDARD", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SWITCHBOT_WORLDARD", 0);
#endif

#ifdef ENABLE_HIGHLIGHT_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_HIGHLIGHT_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_HIGHLIGHT_SYSTEM", 0);
#endif

#ifdef ENABLE_PVP_ADVANCED
	PyModule_AddIntConstant(poModule, "ENABLE_PVP_ADVANCED",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_PVP_ADVANCED",	0);
#endif
#ifdef EQUIP_ENABLE_VIEW_SASH
	PyModule_AddIntConstant(poModule, "EQUIP_ENABLE_VIEW_SASH",	1);
#else
	PyModule_AddIntConstant(poModule, "EQUIP_ENABLE_VIEW_SASH",	0);
#endif
#ifdef ENABLE_NEW_EXCHANGE_WINDOW
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_EXCHANGE_WINDOW", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_EXCHANGE_WINDOW", 0);
#endif
#ifdef ENABLE_ENERGY_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_ENERGY_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ENERGY_SYSTEM",	0);
#endif

#ifdef ENABLE_DRAGON_SOUL_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_DRAGON_SOUL_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DRAGON_SOUL_SYSTEM",	0);
#endif

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_EQUIPMENT_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_EQUIPMENT_SYSTEM",	0);
#endif
#ifdef ENABLE_SOULBIND_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_SOULBIND_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SOULBIND_SYSTEM",	0);
#endif

#ifdef ENABLE_PLAYER_PER_ACCOUNT5
	PyModule_AddIntConstant(poModule, "ENABLE_PLAYER_PER_ACCOUNT5",	1);
	PyModule_AddIntConstant(poModule, "PLAYER_PER_ACCOUNT",	PLAYER_PER_ACCOUNT5);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_PLAYER_PER_ACCOUNT5",	0);
	PyModule_AddIntConstant(poModule, "PLAYER_PER_ACCOUNT",	PLAYER_PER_ACCOUNT4);
#endif
#ifdef ENABLE_QUIVER_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_QUIVER_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_QUIVER_SYSTEM",	0);
#endif

#ifdef ENABLE_LEVEL_IN_TRADE
	PyModule_AddIntConstant(poModule, "ENABLE_LEVEL_IN_TRADE",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_LEVEL_IN_TRADE",	0);
#endif

#ifdef ENABLE_678TH_SKILL
	PyModule_AddIntConstant(poModule, "ENABLE_678TH_SKILL",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_678TH_SKILL",	0);
#endif

#ifdef LOGIN_COUNT_DOWN_UI_MODIFY
	PyModule_AddIntConstant(poModule, "LOGIN_COUNT_DOWN_UI_MODIFY",	1);
#else
	PyModule_AddIntConstant(poModule, "LOGIN_COUNT_DOWN_UI_MODIFY",	0);
#endif

#ifdef ENABLE_ACCE_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_ACCE_SYSTEM",	1);
	PyModule_AddIntConstant(poModule, "ENABLE_ACCE_COSTUME_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ACCE_SYSTEM",	0);
	PyModule_AddIntConstant(poModule, "ENABLE_ACCE_COSTUME_SYSTEM",	0);
#endif

#if defined(USE_ENVIRONMENT_OPTIONS)
	PyModule_AddIntConstant(poModule, "USE_ENVIRONMENT_OPTIONS",	1);
#else
	PyModule_AddIntConstant(poModule, "USE_ENVIRONMENT_OPTIONS",	0);
#endif

#ifdef WJ_SHOW_MOB_INFO
	PyModule_AddIntConstant(poModule, "WJ_SHOW_MOB_INFO",	1);
#else
	PyModule_AddIntConstant(poModule, "WJ_SHOW_MOB_INFO",	0);
#endif

#ifdef NEW_SELECT_CHARACTER
	PyModule_AddIntConstant(poModule, "NEW_SELECT_CHARACTER",	1);
#else
	PyModule_AddIntConstant(poModule, "NEW_SELECT_CHARACTER",	0);
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_GROWTH_PET_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GROWTH_PET_SYSTEM",	0);
#endif

#ifdef ENABLE_AUTO_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_AUTO_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_AUTO_SYSTEM",	0);
#endif

#ifdef ENABLE_MONSTER_CARD
	PyModule_AddIntConstant(poModule, "ENABLE_MONSTER_CARD",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MONSTER_CARD",	0);
#endif

#ifdef ENABLE_HELP_RENEWAL
	PyModule_AddIntConstant(poModule, "ENABLE_HELP_RENEWAL",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_HELP_RENEWAL",	0);
#endif
#ifdef ENABLE_DICE_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_DICE_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DICE_SYSTEM",	0);
#endif

#ifdef ENABLE_CHEQUE_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_CHEQUE_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CHEQUE_SYSTEM",	0);
#endif

#ifdef ENABLE_EXTEND_INVEN_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_EXTEND_INVEN_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_EXTEND_INVEN_SYSTEM",	0);
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_WEAPON_COSTUME_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_WEAPON_COSTUME_SYSTEM",	0);
#endif
#ifdef ENABLE_SLOT_WINDOW_EX
	PyModule_AddIntConstant(poModule, "ENABLE_SLOT_WINDOW_EX",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SLOT_WINDOW_EX",	0);
#endif
#ifdef ENABLE_USE_COSTUME_ATTR
	PyModule_AddIntConstant(poModule, "ENABLE_USE_COSTUME_ATTR",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_USE_COSTUME_ATTR",	0);
#endif
#ifdef ENABLE_DEFENSE_WAVE
	PyModule_AddIntConstant(poModule, "ENABLE_DEFENSE_WAVE",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DEFENSE_WAVE",	0);
#endif

#ifdef VERSION_162_ENABLED
	PyModule_AddIntConstant(poModule, "VERSION_162_ENABLED", 1);
#else
	PyModule_AddIntConstant(poModule, "VERSION_162_ENABLED", 0);
#endif

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	#ifdef ENABLE_CUBE_RENEWAL_GEM_WORLDARD
		PyModule_AddIntConstant(poModule, "ENABLE_CUBE_RENEWAL_GEM_WORLDARD",	1);
	#else
		PyModule_AddIntConstant(poModule, "ENABLE_CUBE_RENEWAL_GEM_WORLDARD",	0);
	#endif

	#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
		PyModule_AddIntConstant(poModule, "ENABLE_CUBE_RENEWAL_COPY_WORLDARD",	1);
	#else
		PyModule_AddIntConstant(poModule, "ENABLE_CUBE_RENEWAL_COPY_WORLDARD",	1);
	#endif
		
	PyModule_AddIntConstant(poModule, "ENABLE_CUBE_RENEWAL_WORLDARD",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CUBE_RENEWAL_GEM_WORLDARD", 0);
	PyModule_AddIntConstant(poModule, "ENABLE_CUBE_RENEWAL_WORLDARD", 0);
#endif

#ifdef ENABLE_MELEY_LAIR_DUNGEON
	PyModule_AddIntConstant(poModule, "ENABLE_MELEY_LAIR_DUNGEON", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MELEY_LAIR_DUNGEON", 0);
#endif
#ifdef ENABLE_CAPITALE_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_CAPITALE_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CAPITALE_SYSTEM", 0);
#endif

#ifdef __ENABLE_RANGE_ALCHEMY__
	PyModule_AddIntConstant(poModule, "__ENABLE_RANGE_ALCHEMY__", 1);
#else
	PyModule_AddIntConstant(poModule, "__ENABLE_RANGE_ALCHEMY__", 0);
#endif

#ifdef __ENABLE_REFINE_ALCHEMY__
	PyModule_AddIntConstant(poModule, "__ENABLE_REFINE_ALCHEMY__", 1);
#else
	PyModule_AddIntConstant(poModule, "__ENABLE_REFINE_ALCHEMY__", 0);
#endif

#ifdef ENABLE_STRONG_METIN
	PyModule_AddIntConstant(poModule, "ENABLE_STRONG_METIN", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_STRONG_METIN", 0);
#endif

#ifdef ENABLE_STRONG_BOSS
	PyModule_AddIntConstant(poModule, "ENABLE_STRONG_BOSS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_STRONG_BOSS", 0);
#endif

#ifdef ENABLE_MEDI_PVM
	PyModule_AddIntConstant(poModule, "ENABLE_MEDI_PVM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MEDI_PVM", 0);
#endif

#ifdef ENABLE_RESIST_MONSTER
	PyModule_AddIntConstant(poModule, "ENABLE_RESIST_MONSTER", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RESIST_MONSTER", 0);
#endif
#ifdef ENABLE_VIEW_ELEMENT
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_ELEMENT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_ELEMENT", 0);
#endif
#ifdef ENABLE_PENDANT
	PyModule_AddIntConstant(poModule, "ENABLE_PENDANT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_PENDANT", 0);
#endif

#ifdef ENABLE_NEW_BONUS_TALISMAN
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_BONUS_TALISMAN", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_BONUS_TALISMAN", 0);
#endif

#ifdef NEW_BONUS_LIKAN
	PyModule_AddIntConstant(poModule, "NEW_BONUS_LIKAN", 1);
#else
	PyModule_AddIntConstant(poModule, "NEW_BONUS_LIKAN", 0);
#endif
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_TARGET_PLAYER_HP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_TARGET_PLAYER_HP", 0);
#endif
#ifdef ENABLE_VIEW_TARGET_DECIMAL_HP
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_TARGET_DECIMAL_HP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_VIEW_TARGET_DECIMAL_HP", 0);
#endif
// #ifdef __ENABLE_INGAME_CHCHANGE__
	// PyModule_AddIntConstant(poModule, "ENABLE_INGAME_CHCHANGE", 1);
// #else
	// PyModule_AddIntConstant(poModule, "ENABLE_INGAME_CHCHANGE", 0);
// #endif

#ifdef ENABLE_DATETIME_UNDER_MINIMAP
	PyModule_AddIntConstant(poModule, "ENABLE_DATETIME_UNDER_MINIMAP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DATETIME_UNDER_MINIMAP", 0);
#endif
#ifdef ENABLE_FEATURES_REFINE_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_FEATURES_REFINE_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_FEATURES_REFINE_SYSTEM", 0);
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
	PyModule_AddIntConstant(poModule, "__ENABLE_NEW_OFFLINESHOP__", 1);
#if defined(DISABLE_OFFSHOP_OFFERS)
	PyModule_AddIntConstant(poModule, "DISABLE_OFFSHOP_OFFERS", 1);
#else
	PyModule_AddIntConstant(poModule, "DISABLE_OFFSHOP_OFFERS", 0);
#endif
#else
	PyModule_AddIntConstant(poModule, "__ENABLE_NEW_OFFLINESHOP__", 0);
#endif


#ifdef ENABLE_EXTRA_INVENTORY
	PyModule_AddIntConstant(poModule, "ENABLE_EXTRA_INVENTORY", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_EXTRA_INVENTORY", 0);
#endif


#ifdef ENABLE_SORT_INVEN
	PyModule_AddIntConstant(poModule, "ENABLE_SORT_INVEN",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SORT_INVEN",	0);
#endif
#ifdef ENABLE_ITEM_EXTRA_PROTO
	PyModule_AddIntConstant(poModule, "ENABLE_ITEM_EXTRA_PROTO", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ITEM_EXTRA_PROTO", 0);
#endif


#ifdef ENABLE_RARITY_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_RARITY_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RARITY_SYSTEM", 0);
#endif

#ifdef ENABLE_NEW_EXTRA_BONUS
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_EXTRA_BONUS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_EXTRA_BONUS", 0);
#endif
#ifdef ENABLE_RANKING
	PyModule_AddIntConstant(poModule, "ENABLE_RANKING", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RANKING", 0);
#endif
#ifdef ENABLE_ATTR_COSTUMES
	PyModule_AddIntConstant(poModule, "ENABLE_ATTR_COSTUMES", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ATTR_COSTUMES", 0);
#endif
#ifdef ENABLE_ATTR_TRANSFER_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_ATTR_TRANSFER_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ATTR_TRANSFER_SYSTEM", 0);
#endif
#ifdef ENABLE_DUNGEON_INFO_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_DUNGEON_INFO_SYSTEM",	1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DUNGEON_INFO_SYSTEM",	0);
#endif
#ifdef ENABLE_DS_SET
	PyModule_AddIntConstant(poModule, "ENABLE_DS_SET", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DS_SET", 0);
#endif
#ifdef ENABLE_DS_ENCHANT
	PyModule_AddIntConstant(poModule, "ENABLE_DS_ENCHANT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DS_ENCHANT", 0);
#endif
#ifdef ENABLE_NEW_PET_EDITS
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_PET_EDITS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_PET_EDITS", 0);
#endif
#ifdef ENABLE_REMOTE_ATTR_SASH_REMOVE
	PyModule_AddIntConstant(poModule, "ENABLE_REMOTE_ATTR_SASH_REMOVE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_REMOTE_ATTR_SASH_REMOVE", 0);
#endif
#ifdef __ENABLE_NEW_EFFECT_STOLE__
	PyModule_AddIntConstant(poModule, "__ENABLE_NEW_EFFECT_STOLE__", 1);
#else
	PyModule_AddIntConstant(poModule, "__ENABLE_NEW_EFFECT_STOLE__", 0);
#endif
#ifdef ENABLE_ATLAS_BOSS
	PyModule_AddIntConstant(poModule, "ENABLE_ATLAS_BOSS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ATLAS_BOSS", 0);
#endif
#ifdef ENABLE_STOLE_REAL
	PyModule_AddIntConstant(poModule, "ENABLE_STOLE_REAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_STOLE_REAL", 0);
#endif
#ifdef ENABLE_STOLE_COSTUME
	PyModule_AddIntConstant(poModule, "ENABLE_STOLE_COSTUME", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_STOLE_COSTUME", 0);
#endif
#ifdef ENABLE_COSTUME_EFFECT
	PyModule_AddIntConstant(poModule, "ENABLE_COSTUME_EFFECT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_COSTUME_EFFECT", 0);
#endif
#ifdef ENABLE_MULTI_LANGUAGE
	PyModule_AddIntConstant(poModule, "ENABLE_MULTI_LANGUAGE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MULTI_LANGUAGE", 0);
#endif
#ifdef ENABLE_MULTI_LANGUAGE
	PyModule_AddIntConstant(poModule, "LANGUAGE_EN", CPythonApplication::LANGUAGE_EN);
	PyModule_AddIntConstant(poModule, "LANGUAGE_RO", CPythonApplication::LANGUAGE_RO);
	PyModule_AddIntConstant(poModule, "LANGUAGE_IT", CPythonApplication::LANGUAGE_IT);
	PyModule_AddIntConstant(poModule, "LANGUAGE_TR", CPythonApplication::LANGUAGE_TR);
	PyModule_AddIntConstant(poModule, "LANGUAGE_DE", CPythonApplication::LANGUAGE_DE);
	PyModule_AddIntConstant(poModule, "LANGUAGE_PL", CPythonApplication::LANGUAGE_PL);
	PyModule_AddIntConstant(poModule, "LANGUAGE_PT", CPythonApplication::LANGUAGE_PT);
	PyModule_AddIntConstant(poModule, "LANGUAGE_ES", CPythonApplication::LANGUAGE_ES);
	PyModule_AddIntConstant(poModule, "LANGUAGE_CZ", CPythonApplication::LANGUAGE_CZ);
	PyModule_AddIntConstant(poModule, "LANGUAGE_HU", CPythonApplication::LANGUAGE_HU);
	PyModule_AddIntConstant(poModule, "LANGUAGE_MAX_NUM", CPythonApplication::LANGUAGE_MAX_NUM);
#endif
#ifdef ENABLE_BLOCK_MULTIFARM
	PyModule_AddIntConstant(poModule, "ENABLE_BLOCK_MULTIFARM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_BLOCK_MULTIFARM", 0);
#endif
#ifdef ENABLE_NEW_USE_POTION
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_USE_POTION", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_USE_POTION", 0);
#endif
#ifdef ENABLE_NEW_RESTART
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_RESTART", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_RESTART", 0);
#endif
#ifdef ENABLE_NEW_COMMON_BONUSES
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_COMMON_BONUSES", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_COMMON_BONUSES", 0);
#endif
#ifdef ENABLE_PERSPECTIVE_VIEW
	PyModule_AddIntConstant(poModule, "ENABLE_PERSPECTIVE_VIEW", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_PERSPECTIVE_VIEW", 0);
#endif
#ifdef ENABLE_UI_EXTRA
	PyModule_AddIntConstant(poModule, "ENABLE_UI_EXTRA", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_UI_EXTRA", 0);
#endif
#ifdef ENABLE_NEW_CHAT
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_CHAT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_CHAT", 0);
#endif
#ifdef ENABLE_DS_GRADE_MYTH
	PyModule_AddIntConstant(poModule, "ENABLE_DS_GRADE_MYTH", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DS_GRADE_MYTH", 0);
#endif
#ifdef ENABLE_INFINITE_RAFINES
	PyModule_AddIntConstant(poModule, "ENABLE_INFINITE_RAFINES", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_INFINITE_RAFINES", 0);
#endif
#ifdef ENABLE_BIOLOGIST_UI
	PyModule_AddIntConstant(poModule, "ENABLE_BIOLOGIST_UI", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_BIOLOGIST_UI", 0);
#endif
#ifdef ENABLE_DS_POTION_DIFFRENT
	PyModule_AddIntConstant(poModule, "ENABLE_DS_POTION_DIFFRENT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DS_POTION_DIFFRENT", 0);
#endif
#ifdef ENABLE_NEW_FISHING_SYSTEM
	PyModule_AddIntConstant(poModule, "FISH_CATCH", CPythonApplication::CURSOR_SHAPE_FISH_CATCH);
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_FISHING_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_FISHING_SYSTEM", 0);
#endif
#ifdef INGAME_WIKI
	PyModule_AddIntConstant(poModule, "INGAME_WIKI", 1);
#else
	PyModule_AddIntConstant(poModule, "INGAME_WIKI", 0);
#endif
#ifdef WJ_ENABLE_TRADABLE_ICON
	PyModule_AddIntConstant(poModule, "WJ_ENABLE_TRADABLE_ICON", 1);
#else
	PyModule_AddIntConstant(poModule, "WJ_ENABLE_TRADABLE_ICON", 0);
#endif
#ifdef USE_NEW_GYEONGGONG_SKILL
	PyModule_AddIntConstant(poModule, "USE_NEW_GYEONGGONG_SKILL", 1);
#else
	PyModule_AddIntConstant(poModule, "USE_NEW_GYEONGGONG_SKILL", 0);
#endif
#ifdef ENABLE_SAVECAMERA_PREFERENCES
	PyModule_AddIntConstant(poModule, "ENABLE_SAVECAMERA_PREFERENCES", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SAVECAMERA_PREFERENCES", 0);
#endif
#ifdef ENABLE_GENDER_ALIGNMENT
	PyModule_AddIntConstant(poModule, "ENABLE_GENDER_ALIGNMENT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GENDER_ALIGNMENT", 0);
#endif
#ifdef OUTLINE_NAMES_TEXTLINE
	PyModule_AddIntConstant(poModule, "OUTLINE_NAMES_TEXTLINE", 1);
#else
	PyModule_AddIntConstant(poModule, "OUTLINE_NAMES_TEXTLINE", 0);
#endif
#ifdef ENABLE_CHOOSE_DOCTRINE_GUI
	PyModule_AddIntConstant(poModule, "ENABLE_CHOOSE_DOCTRINE_GUI", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CHOOSE_DOCTRINE_GUI", 0);
#endif
#ifdef ENABLE_DS_REFINE_ALL
	PyModule_AddIntConstant(poModule, "ENABLE_DS_REFINE_ALL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DS_REFINE_ALL", 0);
#endif
#ifdef ENABLE_BUY_STACK_FROM_SHOP
	PyModule_AddIntConstant(poModule, "ENABLE_BUY_STACK_FROM_SHOP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_BUY_STACK_FROM_SHOP", 0);
#endif
	PyModule_AddIntConstant(poModule, "MULTIPLE_BUY_LIMIT", MULTIPLE_BUY_LIMIT);
	PyModule_AddIntConstant(poModule, "SPECIAL_ACTION_START_INDEX", SPECIAL_ACTION_START_INDEX);
#ifdef LINK_IN_CHAT
	PyModule_AddIntConstant(poModule, "LINK_IN_CHAT", 1);
#else
	PyModule_AddIntConstant(poModule, "LINK_IN_CHAT", 0);
#endif
#ifdef ENABLE_REWARD_SYSTEM
	PyModule_AddIntConstant(poModule, "ENABLE_REWARD_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_REWARD_SYSTEM", 0);
#endif
#if defined(ENABLE_EVENT_MANAGER)
	PyModule_AddIntConstant(poModule, "ENABLE_EVENT_MANAGER", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_EVENT_MANAGER", 0);
#endif
#if defined(ENABLE_INGAME_CHCHANGE)
	PyModule_AddIntConstant(poModule, "ENABLE_INGAME_CHCHANGE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_INGAME_CHCHANGE", 0);
#endif
#if defined(ENABLE_DYNAMIC_SHOP_SLOTSIZE)
	PyModule_AddIntConstant(poModule, "ENABLE_DYNAMIC_SHOP_SLOTSIZE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DYNAMIC_SHOP_SLOTSIZE", 0);
#endif
#ifdef __CHANNEL_CHANGE_SYSTEM__
	PyModule_AddIntConstant(poModule, "CHANNEL_CHANGE_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "CHANNEL_CHANGE_SYSTEM", 0);
#endif
#if defined(ENABLE_CLIENT_OPTIMIZATION)
	PyModule_AddIntConstant(poModule, "ENABLE_CLIENT_OPTIMIZATION", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CLIENT_OPTIMIZATION", 0);
#endif
#if defined(ENABLE_SLOT_COVER_TRAFFIC_LIGHTS)
	PyModule_AddIntConstant(poModule, "ENABLE_SLOT_COVER_TRAFFIC_LIGHTS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_SLOT_COVER_TRAFFIC_LIGHTS", 0);
#endif
#if defined(USE_MOUNT_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "USE_MOUNT_COSTUME_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "USE_MOUNT_COSTUME_SYSTEM", 0);
#endif
#if defined(ENABLE_INGAME_ITEMSHOP)
	PyModule_AddIntConstant(poModule, "ENABLE_INGAME_ITEMSHOP", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_INGAME_ITEMSHOP", 0);
#endif
#if defined(ENABLE_GAYA_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_GAYA_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_GAYA_RENEWAL", 0);
#endif
#if defined(ENABLE_EXCHANGE_FRAGMENTS)
	PyModule_AddIntConstant(poModule, "ENABLE_EXCHANGE_FRAGMENTS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_EXCHANGE_FRAGMENTS", 0);
#endif
#if defined(USE_CAPTCHA_SYSTEM)
    PyModule_AddIntConstant(poModule, "USE_CAPTCHA_SYSTEM", 1);
#else
    PyModule_AddIntConstant(poModule, "USE_CAPTCHA_SYSTEM", 0);
#endif
#if defined(ENABLE_MINI_GAME)
	PyModule_AddIntConstant(poModule, "ENABLE_MINI_GAME", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MINI_GAME", 0);
#endif
#if defined(ENABLE_NEW_FISH_EVENT)
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_FISH_EVENT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_NEW_FISH_EVENT", 0);
#endif
#if defined(ENABLE_MESSENGER_BLOCK)
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_BLOCK", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_BLOCK", 0);
#endif
#if defined(ENABLE_WHISPER_MESSENGER_BUTTONS)
	PyModule_AddIntConstant(poModule, "ENABLE_WHISPER_MESSENGER_BUTTONS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_WHISPER_MESSENGER_BUTTONS", 0);
#endif
#if defined(ENABLE_LAST_HEALTH_ON_TARGET)
	PyModule_AddIntConstant(poModule, "ENABLE_LAST_HEALTH_ON_TARGET", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_LAST_HEALTH_ON_TARGET", 0);
#endif
#if defined(ENABLE_LETTERS_EVENT)
	PyModule_AddIntConstant(poModule, "ENABLE_LETTERS_EVENT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_LETTERS_EVENT", 0);
#endif
#if defined(ENABLE_RENEWAL_OX)
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_OX", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_RENEWAL_OX", 0);
#endif
#if defined(ENABLE_AURA_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_AURA_SYSTEM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_AURA_SYSTEM", 0);
#endif
#if defined(ENABLE_APPLY_MAGICRES_REDUCTION)
	PyModule_AddIntConstant(poModule, "ENABLE_APPLY_MAGICRES_REDUCTION", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_APPLY_MAGICRES_REDUCTION", 0);
#endif
#if defined(ENABLE_HIDE_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "ENABLE_HIDE_COSTUME_SYSTEM", 1);
	PyModule_AddIntConstant(poModule, "HIDE_PART_BODY", HIDE_PART_BODY);
	PyModule_AddIntConstant(poModule, "HIDE_PART_HAIR", HIDE_PART_HAIR);
#if defined(ENABLE_ACCE_SYSTEM)
	PyModule_AddIntConstant(poModule, "HIDE_PART_SASH", HIDE_PART_SASH);
#endif
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
	PyModule_AddIntConstant(poModule, "HIDE_PART_WEAPON", HIDE_PART_WEAPON);
#endif
#if defined(ENABLE_AURA_SYSTEM)
	PyModule_AddIntConstant(poModule, "HIDE_PART_AURA", HIDE_PART_AURA);
#endif
#if defined(ENABLE_PETSKIN)
	PyModule_AddIntConstant(poModule, "HIDE_PETSKIN", HIDE_PETSKIN);
#endif
#if defined(ENABLE_MOUNTSKIN)
	PyModule_AddIntConstant(poModule, "HIDE_MOUNTSKIN", HIDE_MOUNTSKIN);
#endif
#if defined(ENABLE_ACCE_SYSTEM)
	PyModule_AddIntConstant(poModule, "HIDE_PART_SASH", HIDE_PART_SASH);
#endif
	PyModule_AddIntConstant(poModule, "HIDE_STOLE_PART", HIDE_STOLE_PART);
	PyModule_AddIntConstant(poModule, "HIDE_PART_MAX", HIDE_PART_MAX);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_HIDE_COSTUME_SYSTEM", 0);
#endif
#if defined(ENABLE_PETSKIN)
	PyModule_AddIntConstant(poModule, "ENABLE_PETSKIN", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_PETSKIN", 0);
#endif
#if defined(ENABLE_MOUNTSKIN)
	PyModule_AddIntConstant(poModule, "ENABLE_MOUNTSKIN", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MOUNTSKIN", 0);
#endif
#if defined(ENABLE_CHANGELOOK)
	PyModule_AddIntConstant(poModule, "ENABLE_CHANGELOOK", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_CHANGELOOK", 0);
#endif
#if defined(ENABLE_DOGMODE_RENEWAL)
	PyModule_AddIntConstant(poModule, "ENABLE_DOGMODE_RENEWAL", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_DOGMODE_RENEWAL", 0);
#endif
#if defined(ENABLE_VOTE4BONUS)
	PyModule_AddIntConstant(poModule, "ENABLE_VOTE4BONUS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_VOTE4BONUS", 0);
#endif
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
	PyModule_AddIntConstant(poModule, "ENABLE_HALLOWEEN_EVENT_2022", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_HALLOWEEN_EVENT_2022", 0);
#endif
#if defined(ENABLE_ULTIMATE_REGEN)
	PyModule_AddIntConstant(poModule, "ENABLE_ULTIMATE_REGEN", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_ULTIMATE_REGEN", 0);
#endif
#if defined(ENABLE_TRACK_WINDOW)
	PyModule_AddIntConstant(poModule, "ENABLE_TRACK_WINDOW", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_TRACK_WINDOW", 0);
#endif
#if defined(ENABLE_WORLDBOSS)
	PyModule_AddIntConstant(poModule, "ENABLE_WORLDBOSS", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_WORLDBOSS", 0);
#endif
#if defined(ENABLE_MESSENGER_TEAM)
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_TEAM", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_TEAM", 0);
#endif
#if defined(ENABLE_MESSENGER_HELPER)
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_HELPER", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MESSENGER_HELPER", 0);
#endif
#if defined(ENABLE_OKEY_CARD_GAME)
	PyModule_AddIntConstant(poModule, "ENABLE_OKEY_CARD_GAME", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_OKEY_CARD_GAME", 0);
#endif
#if defined(ENABLE_MULTI_ITEM_USE)
	PyModule_AddIntConstant(poModule, "ENABLE_MULTI_ITEM_USE", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MULTI_ITEM_USE", 0);
#endif
#if defined(USE_BATTLEPASS)
	PyModule_AddIntConstant(poModule, "USE_BATTLEPASS", 1);
#else
	PyModule_AddIntConstant(poModule, "USE_BATTLEPASS", 0);
#endif
#if defined(USE_AUTO_REFINE)
	PyModule_AddIntConstant(poModule, "USE_AUTO_REFINE", 1);
#else
	PyModule_AddIntConstant(poModule, "USE_AUTO_REFINE", 0);
#endif
#if defined(USE_RACE_HEIGHT)
	PyModule_AddIntConstant(poModule, "USE_RACE_HEIGHT", 1);
#else
	PyModule_AddIntConstant(poModule, "USE_RACE_HEIGHT", 0);
#endif
#if defined(USE_FOG_FIX)
	PyModule_AddIntConstant(poModule, "USE_FOG_FIX", 1);
#else
	PyModule_AddIntConstant(poModule, "USE_FOG_FIX", 0);
#endif
#if defined(USE_ENVIRONMENT_OPTIONS)
	PyModule_AddIntConstant(poModule, "USE_ENVIRONMENT_OPTIONS", 1);
#else
	PyModule_AddIntConstant(poModule, "USE_ENVIRONMENT_OPTIONS", 0);
#endif
#if defined(USE_ANTI_EXP)
	PyModule_AddIntConstant(poModule, "USE_ANTI_EXP", 1);
#else
	PyModule_AddIntConstant(poModule, "USE_ANTI_EXP", 0);
#endif
#if defined(ENABLE_OFFLINESHOP_REWORK)
	PyModule_AddIntConstant(poModule, "ENABLE_OFFLINESHOP_REWORK", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_OFFLINESHOP_REWORK", 0);
#endif
#if defined(DISABLE_OFFLINESHOP_CHANGENAME)
	PyModule_AddIntConstant(poModule, "DISABLE_OFFLINESHOP_CHANGENAME", 1);
#else
	PyModule_AddIntConstant(poModule, "DISABLE_OFFLINESHOP_CHANGENAME", 0);
#endif
#if defined(USE_SOCKET_MAX_NUM)
	PyModule_AddIntConstant(poModule, "USE_SOCKET_MAX_NUM", USE_SOCKET_MAX_NUM);
#else
	PyModule_AddIntConstant(poModule, "USE_SOCKET_MAX_NUM", 0);
#endif
#if defined(USE_WIKI_ADDONS)
	PyModule_AddIntConstant(poModule, "USE_WIKI_ADDONS", 1);
#else
	PyModule_AddIntConstant(poModule, "USE_WIKI_ADDONS", 0);
#endif
#if defined(USE_WHEEL_OF_FORTUNE)
	PyModule_AddIntConstant(poModule, "USE_WHEEL_OF_FORTUNE", 1);
#else
	PyModule_AddIntConstant(poModule, "USE_WHEEL_OF_FORTUNE", 0);
#endif
#if defined(USE_NEW_ENCHANT_ATTR)
	PyModule_AddIntConstant(poModule, "USE_NEW_ENCHANT_ATTR", 1);
#else
	PyModule_AddIntConstant(poModule, "USE_NEW_ENCHANT_ATTR", 0);
#endif
#if defined(USE_AURA_ATTR_REMOVER)
	PyModule_AddIntConstant(poModule, "USE_AURA_ATTR_REMOVER", 1);
#else
	PyModule_AddIntConstant(poModule, "USE_AURA_ATTR_REMOVER", 0);
#endif
#if defined(USE_ATTR_6TH_7TH)
	PyModule_AddIntConstant(poModule, "USE_ATTR_6TH_7TH", 1);
#else
	PyModule_AddIntConstant(poModule, "USE_ATTR_6TH_7TH", 0);
#endif
#if defined(IS_KEYNES2)
	PyModule_AddIntConstant(poModule, "IS_KEYNES2", 1);
#else
	PyModule_AddIntConstant(poModule, "IS_KEYNES2", 0);
#endif

#ifdef USE_AUTO_HUNT
    PyModule_AddIntConstant(poModule, "USE_AUTO_HUNT", 1);
#else
    PyModule_AddIntConstant(poModule, "USE_AUTO_HUNT", 0);
#endif

#ifdef USE_NO_HALLOWEEN_EVENT_2022_BONUSES
    PyModule_AddIntConstant(poModule, "USE_NO_HALLOWEEN_EVENT_2022_BONUSES", 1);
#else
    PyModule_AddIntConstant(poModule, "USE_NO_HALLOWEEN_EVENT_2022_BONUSES", 0);
#endif

#ifdef USE_PICKUP_FILTER
    PyModule_AddIntConstant(poModule, "USE_PICKUP_FILTER", 1);
#else
    PyModule_AddIntConstant(poModule, "USE_PICKUP_FILTER", 0);
#endif

#ifdef USE_PICKUP_FILTER_ICO_AS_AFFECT
    PyModule_AddIntConstant(poModule, "USE_PICKUP_FILTER_ICO_AS_AFFECT", 1);
#else
    PyModule_AddIntConstant(poModule, "USE_PICKUP_FILTER_ICO_AS_AFFECT", 0);
#endif

#ifdef USE_AUTO_HUNT_NO_POTIONS
    PyModule_AddIntConstant(poModule, "USE_AUTO_HUNT_NO_POTIONS", 1);
#else
    PyModule_AddIntConstant(poModule, "USE_AUTO_HUNT_NO_POTIONS", 0);
#endif

#ifdef USE_AUTO_AGGREGATE
    PyModule_AddIntConstant(poModule, "USE_AUTO_AGGREGATE", 1);
#else
    PyModule_AddIntConstant(poModule, "USE_AUTO_AGGREGATE", 0);
#endif

#ifdef ENABLE_AFFECT_ALIGNMENT
	PyModule_AddIntConstant(poModule, "ENABLE_AFFECT_ALIGNMENT", 1);
#else
	PyModule_AddIntConstant(poModule, "ENABLE_AFFECT_ALIGNMENT", 0);
#endif

#ifdef USE_MULTIPLE_OPENING
    PyModule_AddIntConstant(poModule, "USE_MULTIPLE_OPENING", 1);
#else
    PyModule_AddIntConstant(poModule, "USE_MULTIPLE_OPENING", 0);
#endif
}
