#pragma once

#include "RaceData.h"

#if defined(USE_RACE_HEIGHT)
typedef struct SRaceHeight
{
	float fHeight;
	float fRiding;
	float fScaleZ;
} TRaceHeight;
#endif

class CRaceManager : public CSingleton<CRaceManager>
{
	public:
		typedef std::map<DWORD, CRaceData *> TRaceDataMap;
		typedef TRaceDataMap::iterator TRaceDataIterator;

	public:
		CRaceManager();
		virtual ~CRaceManager();

		void Create();
		void Destroy();

		void RegisterRaceName(DWORD dwRaceIndex, const char * c_szName);
		void RegisterRaceSrcName(const char * c_szName, const char * c_szSrcName);
		void SetPathName(const char * c_szPathName);
		const char * GetFullPathFileName(const char* c_szFileName);

		// Handling
		void CreateRace(DWORD dwRaceIndex);
		void SelectRace(DWORD dwRaceIndex);
		CRaceData * GetSelectedRaceDataPointer();
		// Handling

#ifdef INGAME_WIKI
		BOOL GetRaceDataPointer(DWORD dwRaceIndex, CRaceData ** ppRaceData, bool printTrace = true);
#else
		BOOL GetRaceDataPointer(DWORD dwRaceIndex, CRaceData ** ppRaceData);
#endif

#if defined(USE_RACE_HEIGHT)
	public:
		void	SetRaceHeight(uint32_t, float, float, float);
		float	GetRaceHeight(uint32_t);
		float	GetRidingRaceHeight(uint32_t);
		float	GetRaceScaleZ(uint32_t);
		float GetRaceScale(uint32_t);

	protected:
		std::map<uint32_t, TRaceHeight> m_kMap_iRaceKey_fRaceAdditionalHeight;
#endif

	protected:
		CRaceData* __LoadRaceData(DWORD dwRaceIndex);
		bool __LoadRaceMotionList(CRaceData& rkRaceData, const char* pathName, const char* motionListFileName);

		void __Initialize();
		void __DestroyRaceDataMap();

	protected:
		TRaceDataMap					m_RaceDataMap;

		std::map<std::string, std::string> m_kMap_stRaceName_stSrcName;
		std::map<DWORD, std::string>	m_kMap_dwRaceKey_stRaceName;

	private:
		std::string						m_strPathName;
		CRaceData *						m_pSelectedRaceData;
};