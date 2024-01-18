#include "StdAfx.h"

#ifdef ENABLE_CONFIG_MODULE
#include "PythonConfig.h"
// #include "../../Extern/include/minini_12b/minIni.h"

CPythonConfig::CPythonConfig()
{
	m_pkIniFile = NULL;
}
CPythonConfig::~CPythonConfig()
{
	if (m_pkIniFile)
	{
		delete m_pkIniFile;
		m_pkIniFile = NULL;
	}
}

void CPythonConfig::Initialize(const std::string& szFileName)
{
	if (m_pkIniFile)
	{
		if (m_stFileName == szFileName)
			return;

		delete m_pkIniFile;
	}

	m_pkIniFile = new minIni(szFileName);
	m_stFileName = szFileName;

	Initialize_LoadDefaults();
}

void CPythonConfig::Initialize_LoadDefault(EClassTypes eType, const std::string& stKey, float fDefaultValue)
{
	Initialize_LoadDefault(eType, stKey, std::to_string(fDefaultValue));
}

void CPythonConfig::Initialize_LoadDefault(EClassTypes eType, const std::string& stKey, const std::string& stDefaultValue)
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to load default key %s with value %s", stKey.c_str(), stDefaultValue.c_str());
		return;
	}

	const char* c_pszNoKeyVal = "!---nokeygiven";
	if (c_pszNoKeyVal == GetString(eType, stKey, c_pszNoKeyVal))
	{
		Write(eType, stKey, stDefaultValue);
	}
}

void CPythonConfig::Initialize_LoadDefaults()
{
	int iCPUCoreCount = std::thread::hardware_concurrency();

	float fPerfTreeRange = 15000.0f;
	float fPerfGravelRange = 15000.0f;
	float fPrefEffectRange = 15000.0f;
	float fPrefShopRange = 15000.0f;
	if (iCPUCoreCount < 8)
	{
		fPerfTreeRange = 1500.0f;
		fPerfGravelRange = 1500.0f;
		fPrefEffectRange = 1500.0f;
		fPrefShopRange = 1500.0f;
	}

	Initialize_LoadDefault(CLASS_OPTION, "perf_tree_range", fPerfTreeRange);
	Initialize_LoadDefault(CLASS_OPTION, "perf_gravel_range", fPerfGravelRange);
	Initialize_LoadDefault(CLASS_OPTION, "perf_effect_range", fPrefEffectRange);
	Initialize_LoadDefault(CLASS_OPTION, "perf_shop_range", fPrefShopRange);
	Initialize_LoadDefault(CLASS_OPTION, "perf_loading_dist", 0.0f);
}

std::string CPythonConfig::GetClassNameByType(EClassTypes eType) const
{
	switch (eType)
	{
	case CLASS_GENERAL:
		return "GENERAL";
		break;

	case CLASS_CHAT:
		return "CHAT";
		break;

	case CLASS_OPTION:
		return "OPTION";
		break;

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	case CLASS_SKILL_COLOR:
		return "SKILL_COLOR";
		break;
#endif
	}

	TraceError("unkown class type %u", eType);
	return "UNKOWN";
}

void CPythonConfig::Write(EClassTypes eType, const std::string& stKey, const std::string& stValue)
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to write key %s with value %s", stKey.c_str(), stValue.c_str());
		return;
	}

	m_pkIniFile->put(GetClassNameByType(eType), stKey, stValue);
}
void CPythonConfig::Write(EClassTypes eType, const std::string& stKey, const char* szValue)
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to write key %s with value %s", stKey.c_str(), szValue);
		return;
	}

	m_pkIniFile->put(GetClassNameByType(eType), stKey, szValue);
}
void CPythonConfig::Write(EClassTypes eType, const std::string& stKey, int iValue)
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to write key %s with value %d", stKey.c_str(), iValue);
		return;
	}

	m_pkIniFile->put(GetClassNameByType(eType), stKey, iValue);
}
void CPythonConfig::Write(EClassTypes eType, const std::string& stKey, bool bValue)
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to write key %s with value %d", stKey.c_str(), bValue);
		return;
	}

	m_pkIniFile->put(GetClassNameByType(eType), stKey, bValue);
}

std::string CPythonConfig::GetString(EClassTypes eType, const std::string& stKey, const std::string& stDefaultValue) const
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to read key %s", stKey.c_str());
		return stDefaultValue;
	}

	return m_pkIniFile->gets(GetClassNameByType(eType), stKey, stDefaultValue);
}
int CPythonConfig::GetInteger(EClassTypes eType, const std::string& stKey, int iDefaultValue) const
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to read key %s", stKey.c_str());
		return iDefaultValue;
	}

	return m_pkIniFile->geti(GetClassNameByType(eType), stKey, iDefaultValue);
}

float CPythonConfig::GetFloat(EClassTypes eType, const std::string& stKey, float fDefaultValue) const
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to read key %s", stKey.c_str());
		return fDefaultValue;
	}

	return m_pkIniFile->getf(GetClassNameByType(eType), stKey, fDefaultValue);
}

bool CPythonConfig::GetBool(EClassTypes eType, const std::string& stKey, bool bDefaultValue) const
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to read key %s", stKey.c_str());
		return bDefaultValue;
	}

	return m_pkIniFile->getbool(GetClassNameByType(eType), stKey, bDefaultValue);
}

void CPythonConfig::RemoveSection(EClassTypes eType)
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to remove type %u", eType);
		return;
	}

	m_pkIniFile->del(GetClassNameByType(eType));
}
#endif
