#pragma once
#include "Property.h"
#include <set>
#include <map>

class CPropertyManager : public CSingleton<CPropertyManager>
{
	typedef std::map<DWORD, CProperty*> TPropertyCRCMap;
	typedef std::set<DWORD> TCRCSet;

	public:
		CPropertyManager();
		virtual ~CPropertyManager();

		void Clear();

		bool Initialize(const std::string& listFileName);
		bool Register(const char* c_pszFileName, CProperty** ppProperty = NULL);

		bool Get(DWORD dwCRC, CProperty** ppProperty);
		bool Get(const char* c_pszFileName, CProperty** ppProperty);

	protected:
		bool m_bInitialized;
		TPropertyCRCMap m_PropertyByCRCMap;
		TCRCSet m_ReservedCRCSet;
};
