#pragma once

#include "../common/stl.h"

class CMapLocation : public singleton<CMapLocation> {
	public:
		typedef struct SLocation {
			long addr;
			WORD port;
		} TLocation;

#ifdef ENABLE_GENERAL_CH
		typedef struct SLocationTable {
			BYTE channel;
			long index;
			TLocation location;
		} TLocationTable;
#endif

		bool	Get(
#ifdef ENABLE_GENERAL_CH
BYTE channel, 
#endif
		long x, long y, long & lIndex, long & lAddr, WORD & wPort);
		bool	Get(
#ifdef ENABLE_GENERAL_CH
BYTE channel, 
#endif
		int iIndex, long & lAddr, WORD & wPort);
		void	Insert(long lIndex, const char * c_pszHost, WORD wPort
#ifdef ENABLE_GENERAL_CH
, BYTE channel
#endif
		);
	protected:
#ifdef ENABLE_GENERAL_CH
		std::vector<TLocationTable> m_vector_address;
#else
		std::map<long, TLocation> m_map_address;
#endif
};
