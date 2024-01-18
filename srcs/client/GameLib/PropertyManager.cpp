#include "StdAfx.h"
#include "../EterPack/EterPackManager.h"
#include "../eterBase/tinyxml2.h"
#include "PropertyManager.h"
#include "Property.h"

CPropertyManager::CPropertyManager()
{
	m_bInitialized = false;
}

CPropertyManager::~CPropertyManager()
{
	Clear();
}

bool CPropertyManager::Initialize(const std::string& listFilename)
{
	if (m_bInitialized) {
		return true;
	}

	CMappedFile kPropertyXML;
	PBYTE pbPropertyXML;
	if (CFileSystem::Instance().Get(kPropertyXML, listFilename.c_str(), (LPCVOID*)&pbPropertyXML, __FUNCTION__))
	{
		char* pszXML = new char[kPropertyXML.Size() + 1];
		memcpy(pszXML, pbPropertyXML, kPropertyXML.Size());
		pszXML[kPropertyXML.Size()] = 0;
		std::stringstream kXML;
		kXML << pszXML;


		tinyxml2::XMLDocument doc;
		if (doc.Parse(pszXML) != tinyxml2::XML_SUCCESS) {
			TraceError("CPropertyManager::Initialize: Failed to load %s (%s)", listFilename.c_str(), doc.ErrorName());
			delete[] pszXML;
			return false;
		}

		tinyxml2::XMLElement *levelElement = doc.FirstChildElement("PropertyList");
		if (!levelElement)
			return true;

		for (tinyxml2::XMLElement* child = levelElement->FirstChildElement("Property"); child != NULL; child = child->NextSiblingElement())
		{
			CProperty* pProperty = new CProperty(child->Attribute("filename"));
			if (!pProperty->ReadFromXML(child->Attribute("crc")))
			{
				TraceError("CPropertyManager::Initialize: Cannot register '%s'!", child->Attribute("filename"));
				delete pProperty;
				continue;
			}

			DWORD dwCRC = pProperty->GetCRC();

			TPropertyCRCMap::iterator itor = m_PropertyByCRCMap.find(dwCRC);

			if (m_PropertyByCRCMap.end() != itor)
			{
#if defined(_DEBUG)
				Tracef("CPropertyManager::Initialize: Property already registered, replace %s to %s\n",
					itor->second->GetFileName(), child->Attribute("filename"));
#endif

				delete itor->second;
				itor->second = pProperty;
			}
			else
			{
				m_PropertyByCRCMap.insert(TPropertyCRCMap::value_type(dwCRC, pProperty));
			}


			const tinyxml2::XMLAttribute*	pAttrib = child->FirstAttribute();
			while (pAttrib)
			{
				CTokenVector kVec;
				kVec.push_back(pAttrib->Value());
				pProperty->PutVector(pAttrib->Name(), kVec);
				pAttrib = pAttrib->Next();
			}

		}

		delete[] pszXML;
	}
	else {
		return false;
	}

	m_bInitialized = true;
	return true;
}

bool CPropertyManager::Register(const char* c_pszFileName, CProperty** ppProperty)
{
	CMappedFile file;
	LPCVOID c_pvData;

	if (!CFileSystem::Instance().Get(file, c_pszFileName, &c_pvData, __FUNCTION__))
	{
		return false;
	}

	CProperty* pProperty = new CProperty(c_pszFileName);
	if (!pProperty->ReadFromMemory(c_pvData, file.Size(), c_pszFileName)) {
		delete pProperty;
		return false;
	}

	DWORD dwCRC = pProperty->GetCRC();

	TPropertyCRCMap::iterator itor = m_PropertyByCRCMap.find(dwCRC);

	if (m_PropertyByCRCMap.end() != itor)
	{
		//Tracef("Property already registered, replace %s to %s\n",
		//	itor->second->GetFileName(),
		//	c_pszFileName);
		delete itor->second;
		itor->second = pProperty;
	} else {
		m_PropertyByCRCMap.emplace(TPropertyCRCMap::value_type(dwCRC, pProperty));
	}

	if (ppProperty) {
		*ppProperty = pProperty;
	}

	return true;
}

bool CPropertyManager::Get(const char* c_pszFileName, CProperty** ppProperty)
{
	return Register(c_pszFileName, ppProperty);
}

bool CPropertyManager::Get(DWORD dwCRC, CProperty** ppProperty)
{
	TPropertyCRCMap::iterator itor = m_PropertyByCRCMap.find(dwCRC);

	if (m_PropertyByCRCMap.end() == itor)
		return false;

	*ppProperty = itor->second;
	return true;
}

void CPropertyManager::Clear()
{
	stl_wipe_second(m_PropertyByCRCMap);
}
