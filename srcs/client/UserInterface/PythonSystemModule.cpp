#include "StdAfx.h"
#include "PythonSystem.h"

PyObject * systemGetWidth(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetWidth());
}

PyObject * systemGetHeight(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetHeight());
}

PyObject * systemSetInterfaceHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BuildException();

	CPythonSystem::Instance().SetInterfaceHandler(poHandler);
	return Py_BuildNone();
}

PyObject * systemDestroyInterfaceHandler(PyObject* poSelf, PyObject* poArgs)
{
	CPythonSystem::Instance().DestroyInterfaceHandler();
	return Py_BuildNone();
}

PyObject * systemReserveResource(PyObject* poSelf, PyObject* poArgs)
{
	std::set<std::string> ResourceSet;
	CResourceManager::Instance().PushBackgroundLoadingSet(ResourceSet);
	return Py_BuildNone();
}

PyObject * systemisInterfaceConfig(PyObject* poSelf, PyObject* poArgs)
{
	int isInterfaceConfig = CPythonSystem::Instance().isInterfaceConfig();
	return Py_BuildValue("i", isInterfaceConfig);
}

PyObject * systemSaveWindowStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	int iVisible;
	if (!PyTuple_GetInteger(poArgs, 1, &iVisible))
		return Py_BuildException();

	int iMinimized;
	if (!PyTuple_GetInteger(poArgs, 2, &iMinimized))
		return Py_BuildException();

	int ix;
	if (!PyTuple_GetInteger(poArgs, 3, &ix))
		return Py_BuildException();

	int iy;
	if (!PyTuple_GetInteger(poArgs, 4, &iy))
		return Py_BuildException();

	int iHeight;
	if (!PyTuple_GetInteger(poArgs, 5, &iHeight))
		return Py_BuildException();

	CPythonSystem::Instance().SaveWindowStatus(iIndex, iVisible, iMinimized, ix, iy, iHeight);
	return Py_BuildNone();
}

PyObject * systemGetWindowStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const CPythonSystem::TWindowStatus & c_rWindowStatus = CPythonSystem::Instance().GetWindowStatusReference(iIndex);
	return Py_BuildValue("iiiii", c_rWindowStatus.isVisible,
								  c_rWindowStatus.isMinimized,
								  c_rWindowStatus.ixPosition,
								  c_rWindowStatus.iyPosition,
								  c_rWindowStatus.iHeight);
}

PyObject * systemGetConfig(PyObject * poSelf, PyObject * poArgs)
{
	CPythonSystem::TConfig *tmp = CPythonSystem::Instance().GetConfig();

	int iRes = CPythonSystem::Instance().GetResolutionIndex(tmp->width, tmp->height, tmp->bpp);
	int iFrequency = CPythonSystem::Instance().GetFrequencyIndex(iRes, tmp->frequency);

	return Py_BuildValue("iiiiiiii",  iRes,
									  iFrequency,
									  tmp->is_software_cursor,
									  tmp->is_object_culling,
									  tmp->music_volume,
									  tmp->voice_volume,
									  tmp->gamma,
									  tmp->iDistance);
}

PyObject * systemSetSaveID(PyObject * poSelf, PyObject * poArgs)
{
	int iValue;
	if (!PyTuple_GetInteger(poArgs, 0, &iValue))
		return Py_BuildException();

	char * szSaveID;
	if (!PyTuple_GetString(poArgs, 1, &szSaveID))
		return Py_BuildException();

	CPythonSystem::Instance().SetSaveID(iValue, szSaveID);
	return Py_BuildNone();
}

PyObject * systemisSaveID(PyObject * poSelf, PyObject * poArgs)
{
	int value = CPythonSystem::Instance().IsSaveID();
	return Py_BuildValue("i", value);
}

PyObject * systemGetSaveID(PyObject * poSelf, PyObject * poArgs)
{
	const char * c_szSaveID = CPythonSystem::Instance().GetSaveID();
	return Py_BuildValue("s", c_szSaveID);
}

PyObject * systemGetMusicVolume(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetMusicVolume());
}

PyObject * systemGetSoundVolume(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetSoundVolume());
}

PyObject * systemSetMusicVolume(PyObject * poSelf, PyObject * poArgs)
{
	float fVolume;
	if (!PyTuple_GetFloat(poArgs, 0, &fVolume))
		return Py_BuildException();

	CPythonSystem::Instance().SetMusicVolume(fVolume);
	return Py_BuildNone();
}

PyObject * systemSetSoundVolumef(PyObject * poSelf, PyObject * poArgs)
{
	float fVolume;
	if (!PyTuple_GetFloat(poArgs, 0, &fVolume))
		return Py_BuildException();

	CPythonSystem::Instance().SetSoundVolumef(fVolume);
	return Py_BuildNone();
}

PyObject * systemIsSoftwareCursor(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsSoftwareCursor());
}

PyObject * systemSetViewChatFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetViewChatFlag(iFlag);

	return Py_BuildNone();
}

PyObject * systemIsViewChat(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsViewChat());
}

PyObject * systemSetAlwaysShowNameFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetAlwaysShowNameFlag(iFlag);

	return Py_BuildNone();
}

PyObject * systemSetShowDamageFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowDamageFlag(iFlag);

	return Py_BuildNone();
}

PyObject * systemSetShowSalesTextFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowSalesTextFlag(iFlag);

	return Py_BuildNone();
}

#ifdef WJ_SHOW_MOB_INFO
#include "PythonCharacterManager.h"
#include "PythonNonPlayer.h"
#include "PythonSystem.h"
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
PyObject * systemIsShowMobAIFlag(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowMobAIFlag());
}
PyObject * systemSetShowMobAIFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowMobAIFlagFlag(iFlag);

	for (CPythonCharacterManager::CharacterIterator it=CPythonCharacterManager::Instance().CharacterInstanceBegin(), ti=CPythonCharacterManager::Instance().CharacterInstanceEnd();
			it!=ti;
			++it)
	{
		CInstanceBase * pkInst = *it;
		if (pkInst && pkInst->IsEnemy())
			pkInst->MobInfoAiFlagRefresh();
	}
	return Py_BuildNone();
}
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
PyObject * systemIsShowMobLevel(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowMobLevel());
}
PyObject * systemSetShowMobLevel(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowMobLevelFlag(iFlag);

	for (CPythonCharacterManager::CharacterIterator it=CPythonCharacterManager::Instance().CharacterInstanceBegin(), ti=CPythonCharacterManager::Instance().CharacterInstanceEnd();
			it!=ti;
			++it)
	{
		CInstanceBase * pkInst = *it;
		if (pkInst && pkInst->IsEnemy())
			pkInst->MobInfoLevelRefresh();
	}
	return Py_BuildNone();
}
#endif
#endif

PyObject * systemIsAlwaysShowName(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsAlwaysShowName());
}

PyObject * systemIsShowDamage(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowDamage());
}

PyObject * systemIsShowSalesText(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowSalesText());
}

PyObject * systemSetConfig(PyObject * poSelf, PyObject * poArgs)
{
	int res_index;
	int width;
	int height;
	int bpp;
	int frequency_index;
	int frequency;
	int software_cursor;
	int shadow;
	int object_culling;
	int music_volume;
	int voice_volume;
	int gamma;
	int distance;

	if (!PyTuple_GetInteger(poArgs, 0, &res_index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &frequency_index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 2, &software_cursor))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 3, &shadow))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 4, &object_culling))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 5, &music_volume))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 6, &voice_volume))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 7, &gamma))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 8, &distance))
		return Py_BuildException();

	if (!CPythonSystem::Instance().GetResolution(res_index, (DWORD *) &width, (DWORD *) &height, (DWORD *) &bpp))
		return Py_BuildNone();

	if (!CPythonSystem::Instance().GetFrequency(res_index,frequency_index, (DWORD *) &frequency))
		return Py_BuildNone();

	CPythonSystem::TConfig tmp;

	memcpy(&tmp, CPythonSystem::Instance().GetConfig(), sizeof(tmp));

	tmp.width				= width;
	tmp.height				= height;
	tmp.bpp					= bpp;
	tmp.frequency			= frequency;
	tmp.is_software_cursor	= software_cursor ? true : false;
	tmp.is_object_culling	= object_culling ? true : false;
	tmp.music_volume		= (char) music_volume;
	tmp.voice_volume		= (char) voice_volume;
	tmp.gamma				= gamma;
	tmp.iDistance			= distance;

	CPythonSystem::Instance().SetConfig(&tmp);
	return Py_BuildNone();
}

PyObject * systemApplyConfig(PyObject * poSelf, PyObject * poArgs)
{
	CPythonSystem::Instance().ApplyConfig();
	return Py_BuildNone();
}

PyObject * systemSaveConfig(PyObject * poSelf, PyObject * poArgs)
{
	int ret = CPythonSystem::Instance().SaveConfig();
	return Py_BuildValue("i", ret);
}

PyObject * systemGetResolutionCount(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetResolutionCount());
}

PyObject * systemGetFrequencyCount(PyObject * poSelf, PyObject * poArgs)
{
	int	index;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonSystem::Instance().GetFrequencyCount(index));
}

PyObject * systemGetResolution(PyObject * poSelf, PyObject * poArgs)
{
	int	index;
	DWORD width = 0, height = 0, bpp = 0;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	CPythonSystem::Instance().GetResolution(index, &width, &height, &bpp);
	return Py_BuildValue("iii", width, height, bpp);
}

PyObject * systemGetCurrentResolution(PyObject * poSelf, PyObject *poArgs)
{
	CPythonSystem::TConfig *tmp = CPythonSystem::Instance().GetConfig();
	return Py_BuildValue("iii", tmp->width, tmp->height, tmp->bpp);
}

PyObject * systemGetFrequency(PyObject * poSelf, PyObject * poArgs)
{
	int	index, frequency_index;
	DWORD frequency = 0;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &frequency_index))
		return Py_BuildException();

	CPythonSystem::Instance().GetFrequency(index, frequency_index, &frequency);
	return Py_BuildValue("i", frequency);
}

PyObject * systemGetShadowLevel(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetShadowLevel());
}

PyObject * systemSetShadowLevel(PyObject * poSelf, PyObject * poArgs)
{
	int level;

	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	if (level > 0)
		CPythonSystem::Instance().SetShadowLevel(level);

	return Py_BuildNone();
}


#ifdef ENABLE_MULTI_LANGUAGE
PyObject * systemSetChatFilterValue(PyObject * poSelf, PyObject * poArgs)
{
	char * szFilter;
	if (!PyTuple_GetString(poArgs, 0, &szFilter))
		return Py_BuildException();

	CPythonSystem::Instance().SetChatFilterValue(szFilter);
	return Py_BuildNone();
}

PyObject * systemGetChatFilterValue(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("s", CPythonSystem::Instance().GetChatFilterValue().c_str());
}

PyObject * systemIsAutoTranslateWhisper(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsAutoTranslateWhisper());
}

PyObject * systemSetAutoTranslateWhisper(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();
	
	CPythonSystem::Instance().SetAutoTranslateWhisper(iFlag);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_PERSPECTIVE_VIEW
PyObject * systemSetFieldPerspective(PyObject * poSelf, PyObject * poArgs)
{
	float fValue;
	if (!PyTuple_GetFloat(poArgs, 0, &fValue))
		return Py_BuildException();
	
	CPythonSystem::Instance().SetFieldPerspective(fValue);
	return Py_BuildNone();
}

PyObject * systemGetFieldPerspective(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetFieldPerspective());
}
#endif

#ifdef ENABLE_BIOLOGIST_UI
PyObject * systemSetBiologistAlert(PyObject * poSelf, PyObject * poArgs)
{
	bool value;
	if (!PyTuple_GetBoolean(poArgs, 0, &value))
		return Py_BuildException();
	
	CPythonSystem::Instance().SetBiologistAlert(value);
	return Py_BuildNone();
}

PyObject * systemGetBiologistAlert(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("b", CPythonSystem::Instance().GetBiologistAlert());
}
#endif

#ifdef ENABLE_BIOLOGIST_UI
PyObject * systemSetCameraType(PyObject * poSelf, PyObject * poArgs) {
	BYTE value;
	if (!PyTuple_GetInteger(poArgs, 0, &value))
		return Py_BuildException();

	CPythonSystem::Instance().SetCameraType(value);
	return Py_BuildNone();
}

PyObject * systemGetCameraType(PyObject * poSelf, PyObject * poArgs) {
	return Py_BuildValue("i", CPythonSystem::Instance().GetCameraType());
}

PyObject * systemGetCameraHeight(PyObject * poSelf, PyObject * poArgs) {
	return Py_BuildValue("f", CPythonSystem::Instance().GetCameraHeight());
}
#endif

#ifdef OUTLINE_NAMES_TEXTLINE
PyObject * systemSetNamesType(PyObject * poSelf, PyObject * poArgs) {
	int type;
	if (PyTuple_GetInteger(poArgs, 0, &type)) {
		CPythonSystem::Instance().SetNamesType(type == 0 ? false : true);
	}

	return Py_BuildNone();
}

PyObject * systemGetNamesType(PyObject * poSelf, PyObject * poArgs) {
	return Py_BuildValue("b", CPythonSystem::Instance().GetNamesType());
}
#endif

#ifdef ENABLE_NEW_CHAT
PyObject * systemGetChatFilter(PyObject * poSelf, PyObject * poArgs) {
	return Py_BuildValue("i", CPythonSystem::Instance().GetChatFilter());
}

PyObject * systemSetChatFilter(PyObject * poSelf, PyObject * poArgs) {
	int value;
	if (PyTuple_GetInteger(poArgs, 0, &value)) {
		CPythonSystem::Instance().SetChatFilter(value);
	}

	return Py_BuildNone();
}
#endif

PyObject * systemSetTimePm(PyObject * poSelf, PyObject * poArgs)
{
	bool value;
	if (!PyTuple_GetBoolean(poArgs, 0, &value))
		return Py_BuildException();
	
	CPythonSystem::Instance().SetTimePm(value);
	return Py_BuildNone();
}

PyObject * systemGetTimePm(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("b", CPythonSystem::Instance().GetTimePm());
}

PyObject * systemSetHideModeStatus(PyObject * poSelf, PyObject * poArgs) {
	int type;
	if (!PyTuple_GetInteger(poArgs, 0, &type))
		return Py_BuildException();

	int value;
	if (!PyTuple_GetInteger(poArgs, 1, &value))
		return Py_BuildException();

	CPythonSystem::Instance().SetHideModeStatus(type, value);
	return Py_BuildNone();
}

PyObject * systemGetHideModeStatus(PyObject * poSelf, PyObject * poArgs) {
	return Py_BuildValue("(bbbbbbb)", CPythonSystem::Instance().GetHideMode1Status(), CPythonSystem::Instance().GetHideMode2Status(), CPythonSystem::Instance().GetHideMode3Status(), CPythonSystem::Instance().GetHideMode4Status(), CPythonSystem::Instance().GetHideMode5Status(), CPythonSystem::Instance().GetHideMode6Status(), CPythonSystem::Instance().GetHideMode7Status());
}

#if defined(ENABLE_DOGMODE_RENEWAL)
PyObject * systemSetDogMode(PyObject * poSelf, PyObject * poArgs)
{
	int value;
	if (!PyTuple_GetInteger(poArgs, 0, &value))
		return Py_BuildException();

	CPythonSystem::Instance().SetDogMode(value);
	return Py_BuildNone();
}

PyObject * systemIsDogMode(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("b", CPythonSystem::Instance().IsDogMode());
}
#endif

#if defined(ENABLE_TRACK_WINDOW)
PyObject* systemSetDungeonTrack(PyObject* poSelf, PyObject* poArgs) {
	int iValue;

	if (!PyTuple_GetInteger(poArgs, 0, &iValue)) {
		return Py_BuildException();
	}

	CPythonSystem::Instance().SetDungeonTrack(iValue);
	return Py_BuildNone();
}

PyObject* systemGetDungeonTrack(PyObject* poSelf, PyObject* poArgs) {
	return Py_BuildValue("i", CPythonSystem::Instance().GetDungeonTrack());
}

PyObject* systemSetBossTrack(PyObject* poSelf, PyObject* poArgs) {
	int iValue;

	if (!PyTuple_GetInteger(poArgs, 0, &iValue)) {
		return Py_BuildException();
	}

	CPythonSystem::Instance().SetBossTrack(iValue);
	return Py_BuildNone();
}

PyObject* systemGetBossTrack(PyObject* poSelf, PyObject* poArgs) {
	return Py_BuildValue("i", CPythonSystem::Instance().GetBossTrack());
}
#endif

#if defined(USE_FOG_FIX)
PyObject* systemSetFogModeOption(PyObject* poSelf, PyObject* poArgs)
{
	int iOpt;
	if (!PyTuple_GetInteger(poArgs, 0, &iOpt))
	{
		return Py_BuildException();
	}

	CPythonSystem::Instance().SetFogModeOption(iOpt);
	return Py_BuildNone();
}

PyObject* systemGetFogModeOption(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetFogModeOption());
}
#endif

#if defined(USE_ENVIRONMENT_OPTIONS)
PyObject* systemSetNightModeOption(PyObject* poSelf, PyObject* poArgs)
{
	int iOpt;
	if (!PyTuple_GetInteger(poArgs, 0, &iOpt))
	{
		return Py_BuildException();
	}

	CPythonSystem::Instance().SetNightModeOption(iOpt);
	return Py_BuildNone();
}

PyObject* systemGetNightModeOption(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetNightModeOption());
}

PyObject* systemSetSnowModeOption(PyObject* poSelf, PyObject* poArgs)
{
	int iOpt;
	if (!PyTuple_GetInteger(poArgs, 0, &iOpt))
	{
		return Py_BuildException();
	}

	CPythonSystem::Instance().SetSnowModeOption(iOpt);
	return Py_BuildNone();
}

PyObject* systemGetSnowModeOption(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetSnowModeOption());
}

PyObject* systemSetSnowTextureModeOption(PyObject* poSelf, PyObject* poArgs)
{
	int iOpt;
	if (!PyTuple_GetInteger(poArgs, 0, &iOpt))
	{
		return Py_BuildException();
	}

	CPythonSystem::Instance().SetSnowTextureModeOption(iOpt);
	return Py_BuildNone();
}

PyObject* systemGetSnowTextureModeOption(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetSnowTextureModeOption());
}
#endif

void initsystemSetting()
{
	static PyMethodDef s_methods[] =
	{
#if defined(ENABLE_TRACK_WINDOW)
		{"SetDungeonTrack", systemSetDungeonTrack, METH_VARARGS},
		{"GetDungeonTrack", systemGetDungeonTrack, METH_VARARGS},
		{"SetBossTrack", systemSetBossTrack, METH_VARARGS},
		{"GetBossTrack", systemGetBossTrack, METH_VARARGS},
#endif
#if defined(ENABLE_DOGMODE_RENEWAL)
		{"SetDogMode", systemSetDogMode, METH_VARARGS},
		{"IsDogMode", systemIsDogMode, METH_VARARGS},
#endif
		{ "GetWidth",					systemGetWidth,					METH_VARARGS },
		{ "GetHeight",					systemGetHeight,				METH_VARARGS },

		{ "SetInterfaceHandler",		systemSetInterfaceHandler,		METH_VARARGS },
		{ "DestroyInterfaceHandler",	systemDestroyInterfaceHandler,	METH_VARARGS },
		{ "ReserveResource",			systemReserveResource,			METH_VARARGS },

		{ "isInterfaceConfig",			systemisInterfaceConfig,		METH_VARARGS },
		{ "SaveWindowStatus",			systemSaveWindowStatus,			METH_VARARGS },
		{ "GetWindowStatus",			systemGetWindowStatus,			METH_VARARGS },

		{ "GetResolutionCount",			systemGetResolutionCount,		METH_VARARGS },
		{ "GetFrequencyCount",			systemGetFrequencyCount,		METH_VARARGS },

		{ "GetCurrentResolution",		systemGetCurrentResolution,		METH_VARARGS },

		{ "GetResolution",				systemGetResolution,			METH_VARARGS },
		{ "GetFrequency",				systemGetFrequency,				METH_VARARGS },

		{ "ApplyConfig",				systemApplyConfig,				METH_VARARGS },
		{ "SetConfig",					systemSetConfig,				METH_VARARGS },
		{ "SaveConfig",					systemSaveConfig,				METH_VARARGS },
		{ "GetConfig",					systemGetConfig,				METH_VARARGS },

		{ "SetSaveID",					systemSetSaveID,				METH_VARARGS },
		{ "isSaveID",					systemisSaveID,					METH_VARARGS },
		{ "GetSaveID",					systemGetSaveID,				METH_VARARGS },

		{ "GetMusicVolume",				systemGetMusicVolume,			METH_VARARGS },
		{ "GetSoundVolume",				systemGetSoundVolume,			METH_VARARGS },

		{ "SetMusicVolume",				systemSetMusicVolume,			METH_VARARGS },
		{ "SetSoundVolumef",			systemSetSoundVolumef,			METH_VARARGS },
		{ "IsSoftwareCursor",			systemIsSoftwareCursor,			METH_VARARGS },

		{ "SetViewChatFlag",			systemSetViewChatFlag,			METH_VARARGS },
		{ "IsViewChat",					systemIsViewChat,				METH_VARARGS },

		{ "SetAlwaysShowNameFlag",		systemSetAlwaysShowNameFlag,	METH_VARARGS },
		{ "IsAlwaysShowName",			systemIsAlwaysShowName,			METH_VARARGS },

		{ "SetShowDamageFlag",			systemSetShowDamageFlag,		METH_VARARGS },
		{ "IsShowDamage",				systemIsShowDamage,				METH_VARARGS },

		{ "SetShowSalesTextFlag",		systemSetShowSalesTextFlag,		METH_VARARGS },
		{ "IsShowSalesText",			systemIsShowSalesText,			METH_VARARGS },

		{ "GetShadowLevel",				systemGetShadowLevel,			METH_VARARGS },
		{ "SetShadowLevel",				systemSetShadowLevel,			METH_VARARGS },

#ifdef WJ_SHOW_MOB_INFO
		{ "IsShowMobAIFlag",			systemIsShowMobAIFlag,			METH_VARARGS },
		{ "SetShowMobAIFlag",			systemSetShowMobAIFlag,			METH_VARARGS },

		{ "IsShowMobLevel",				systemIsShowMobLevel,			METH_VARARGS },
		{ "SetShowMobLevel",			systemSetShowMobLevel,			METH_VARARGS },
#endif

#ifdef ENABLE_MULTI_LANGUAGE
		{"SetChatFilterValue", systemSetChatFilterValue, METH_VARARGS},
		{"GetChatFilterValue", systemGetChatFilterValue, METH_VARARGS},
		{"IsAutoTranslateWhisper", systemIsAutoTranslateWhisper, METH_VARARGS},
		{"SetAutoTranslateWhisper", systemSetAutoTranslateWhisper, METH_VARARGS},
#endif
#ifdef ENABLE_PERSPECTIVE_VIEW
		{"SetFieldPerspective", systemSetFieldPerspective, METH_VARARGS},
		{"GetFieldPerspective", systemGetFieldPerspective, METH_VARARGS},
#endif
#ifdef ENABLE_BIOLOGIST_UI
		{"SetBiologistAlert", systemSetBiologistAlert, METH_VARARGS},
		{"GetBiologistAlert", systemGetBiologistAlert, METH_VARARGS},
#endif
#ifdef ENABLE_SAVECAMERA_PREFERENCES
		{"SetCameraType", systemSetCameraType, METH_VARARGS},
		{"GetCameraType", systemGetCameraType, METH_VARARGS},
		{"GetCameraHeight", systemGetCameraHeight, METH_VARARGS},
#endif
#ifdef OUTLINE_NAMES_TEXTLINE
		{"SetNamesType", systemSetNamesType, METH_VARARGS},
		{"GetNamesType", systemGetNamesType, METH_VARARGS},
#endif
#ifdef ENABLE_NEW_CHAT
		{"SetChatFilter", systemSetChatFilter, METH_VARARGS},
		{"GetChatFilter", systemGetChatFilter, METH_VARARGS},
#endif
		{"SetTimePm", systemSetTimePm, METH_VARARGS},
		{"GetTimePm", systemGetTimePm, METH_VARARGS},
		{"SetHideModeStatus", systemSetHideModeStatus, METH_VARARGS},
		{"GetHideModeStatus", systemGetHideModeStatus, METH_VARARGS},
#if defined(USE_FOG_FIX)
		{"SetFogMode", systemSetFogModeOption, METH_VARARGS},
		{"GetFogMode", systemGetFogModeOption, METH_VARARGS},
#endif
#if defined(USE_ENVIRONMENT_OPTIONS)
		{"SetNightModeOption", systemSetNightModeOption, METH_VARARGS},
		{"GetNightModeOption", systemGetNightModeOption, METH_VARARGS},
		{"SetSnowModeOption", systemSetSnowModeOption, METH_VARARGS},
		{"GetSnowModeOption", systemGetSnowModeOption, METH_VARARGS},
		{"SetSnowTextureModeOption", systemSetSnowTextureModeOption, METH_VARARGS},
		{"GetSnowTextureModeOption", systemGetSnowTextureModeOption, METH_VARARGS},
#endif
		{NULL, NULL, NULL}
	};

	PyObject * poModule = Py_InitModule("systemSetting", s_methods);

	PyModule_AddIntConstant(poModule, "WINDOW_STATUS",		CPythonSystem::WINDOW_STATUS);
	PyModule_AddIntConstant(poModule, "WINDOW_INVENTORY",	CPythonSystem::WINDOW_INVENTORY);
	PyModule_AddIntConstant(poModule, "WINDOW_ABILITY",		CPythonSystem::WINDOW_ABILITY);
	PyModule_AddIntConstant(poModule, "WINDOW_SOCIETY",		CPythonSystem::WINDOW_SOCIETY);
	PyModule_AddIntConstant(poModule, "WINDOW_JOURNAL",		CPythonSystem::WINDOW_JOURNAL);
	PyModule_AddIntConstant(poModule, "WINDOW_COMMAND",		CPythonSystem::WINDOW_COMMAND);

	PyModule_AddIntConstant(poModule, "WINDOW_QUICK",		CPythonSystem::WINDOW_QUICK);
	PyModule_AddIntConstant(poModule, "WINDOW_GAUGE",		CPythonSystem::WINDOW_GAUGE);
	PyModule_AddIntConstant(poModule, "WINDOW_MINIMAP",		CPythonSystem::WINDOW_MINIMAP);
	PyModule_AddIntConstant(poModule, "WINDOW_CHAT",		CPythonSystem::WINDOW_CHAT);
}
