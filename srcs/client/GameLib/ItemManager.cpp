#include "StdAfx.h"
#include "../eterPack/EterPackManager.h"
#include "../eterLib/ResourceManager.h"

#include "ItemManager.h"

#ifdef USE_MULTIPLE_OPENING
int32_t CItemManager::GetItemType(const uint32_t vnum)
{
    const auto it = m_ItemMap.find(vnum);
    if (it == m_ItemMap.end())
    {
        for (auto const& i : m_vec_ItemRange)
        {
            const auto table = i->GetTable();
            if (!table)
            {
                continue;
            }

            if ((table->dwVnum < vnum) && vnum < (table->dwVnum + table->dwVnumRange))
            {
               return table->bType;
            }
        }
    }

    const auto table = it->second->GetTable();
    if (table)
    {
        return table->bType;
    }

    return -1;
}
#endif

static DWORD s_adwItemProtoKey[4] =
{
	173217,
	72619434,
	408587239,
	27973291
};

BOOL CItemManager::SelectItemData(DWORD dwIndex)
{
	TItemMap::iterator f = m_ItemMap.find(dwIndex);

#ifdef ENABLE_ITEM_EXTRA_PROTO
	m_pSelectedExtraProto = GetExtraProto(dwIndex);
#endif

	if (m_ItemMap.end() == f)
	{
		int n = m_vec_ItemRange.size();
		for (int i = 0; i < n; i++)
		{
			CItemData* p = m_vec_ItemRange[i];
			const CItemData::TItemTable* pTable = p->GetTable();
			if ((pTable->dwVnum < dwIndex) &&
				dwIndex < (pTable->dwVnum + pTable->dwVnumRange))
			{
				m_pSelectedItemData = p;
				return TRUE;
			}
		}
		Tracef(" CItemManager::SelectItemData - FIND ERROR [%d]\n", dwIndex);
		return FALSE;
	}

	m_pSelectedItemData = f->second;

	return TRUE;
}

CItemData* CItemManager::GetSelectedItemDataPointer()
{
	return m_pSelectedItemData;
}

#ifdef ENABLE_ITEM_EXTRA_PROTO
CItemData::TItemExtraProto* CItemManager::GetSelectedExtraProto()
{
	return m_pSelectedExtraProto;
}
#endif

#ifdef INGAME_WIKI
bool CItemManager::CanIncrRefineLevel() {
	if (!m_pSelectedItemData)
		return false;

	return m_pSelectedItemData->GetRefinedVnum() == 0 ? false : true;
}
#endif

BOOL CItemManager::GetItemDataPointer(DWORD dwItemID, CItemData** ppItemData)
{
	if (0 == dwItemID)
		return FALSE;

	TItemMap::iterator f = m_ItemMap.find(dwItemID);

	if (m_ItemMap.end() == f)
	{
		int n = m_vec_ItemRange.size();
		for (int i = 0; i < n; i++)
		{
			CItemData* p = m_vec_ItemRange[i];
			const CItemData::TItemTable* pTable = p->GetTable();
			if ((pTable->dwVnum < dwItemID) &&
				dwItemID < (pTable->dwVnum + pTable->dwVnumRange))
			{
				*ppItemData = p;
				return TRUE;
			}
		}
		Tracef(" CItemManager::GetItemDataPointer - FIND ERROR [%d]\n", dwItemID);
		return FALSE;
	}

	*ppItemData = f->second;

	return TRUE;
}

CItemData* CItemManager::MakeItemData(DWORD dwIndex)
{
	TItemMap::iterator f = m_ItemMap.find(dwIndex);

	if (m_ItemMap.end() == f)
	{
		CItemData* pItemData = CItemData::New();

		m_ItemMap.insert(TItemMap::value_type(dwIndex, pItemData));

		return pItemData;
	}

	return f->second;
}

////////////////////////////////////////////////////////////////////////////////////////
// Load Item Table

bool CItemManager::LoadItemList(const char* c_szFileName)
{
	CMappedFile File;
	LPCVOID pData;

	if (!CFileSystem::Instance().Get(File, c_szFileName, &pData, __FUNCTION__))
		return false;

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(File.Size(), pData);

	CTokenVector TokenVector;
	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &TokenVector, "\t"))
			continue;

		if (!(TokenVector.size() == 3 || TokenVector.size() == 4))
		{
			TraceError(" CItemManager::LoadItemList(%s) - StrangeLine in %d\n", c_szFileName, i);
			continue;
		}

		const std::string& c_rstrID = TokenVector[0];
		const std::string & c_rstrType = TokenVector[1];
		const std::string& c_rstrIcon = TokenVector[2];

		DWORD dwItemVNum = atoi(c_rstrID.c_str());

		CItemData* pItemData = MakeItemData(dwItemVNum);

		extern BOOL USE_VIETNAM_CONVERT_WEAPON_VNUM;
		if (USE_VIETNAM_CONVERT_WEAPON_VNUM)
		{
			extern DWORD Vietnam_ConvertWeaponVnum(DWORD vnum);
			DWORD dwMildItemVnum = Vietnam_ConvertWeaponVnum(dwItemVNum);
			if (dwMildItemVnum == dwItemVNum)
			{
				if (4 == TokenVector.size())
				{
					const std::string& c_rstrModelFileName = TokenVector[3];
					pItemData->SetDefaultItemData(c_rstrIcon.c_str(), c_rstrModelFileName.c_str());
				}
				else
				{
					pItemData->SetDefaultItemData(c_rstrIcon.c_str());
				}
			}
			else
			{
				DWORD dwMildBaseVnum = dwMildItemVnum / 10 * 10;
				char szMildIconPath[MAX_PATH];
				sprintf(szMildIconPath, "icon/item/%.5d.tga", dwMildBaseVnum);
				if (4 == TokenVector.size())
				{
					char szMildModelPath[MAX_PATH];
					sprintf(szMildModelPath, "d:/ymir work/item/weapon/%.5d.gr2", dwMildBaseVnum);
					pItemData->SetDefaultItemData(szMildIconPath, szMildModelPath);
				}
				else
				{
					pItemData->SetDefaultItemData(szMildIconPath);
				}
			}
		}
		else
		{
			if (4 == TokenVector.size())
			{
#if defined(ENABLE_AURA_SYSTEM)
				if (!strcmp(c_rstrType.c_str(), "AURA")) {
					const std::string& c_rstrAuraEffectFileName = TokenVector[3];
					pItemData->SetAuraEffectID(c_rstrAuraEffectFileName.c_str());
					pItemData->SetDefaultItemData(c_rstrIcon.c_str());
				} else {
					const std::string& c_rstrModelFileName = TokenVector[3];
					pItemData->SetDefaultItemData(c_rstrIcon.c_str(), c_rstrModelFileName.c_str());
				}
#else
				const std::string& c_rstrModelFileName = TokenVector[3];
				pItemData->SetDefaultItemData(c_rstrIcon.c_str(), c_rstrModelFileName.c_str());
#endif
			}
			else
			{
				pItemData->SetDefaultItemData(c_rstrIcon.c_str());
			}
		}
	}

	return true;
}

const std::string& __SnapString(const std::string& c_rstSrc, std::string& rstTemp)
{
	UINT uSrcLen = c_rstSrc.length();
	if (uSrcLen < 2)
		return c_rstSrc;

	if (c_rstSrc[0] != '"')
		return c_rstSrc;

	UINT uLeftCut = 1;

	UINT uRightCut = uSrcLen;
	if (c_rstSrc[uSrcLen - 1] == '"')
		uRightCut = uSrcLen - 1;

	rstTemp = c_rstSrc.substr(uLeftCut, uRightCut - uLeftCut);
	return rstTemp;
}
#ifdef __ENABLE_NEW_OFFLINESHOP__
void CItemManager::GetItemsNameMap(std::map<DWORD, std::string>& inMap)
{
	inMap.clear();

	for (auto& it : m_ItemMap)
		inMap.insert(std::make_pair(it.first, it.second->GetName()));
}
#endif

bool CItemManager::LoadItemDesc(const char* c_szFileName)
{
	const VOID* pvData;
	CMappedFile kFile;
	if (!CFileSystem::Instance().Get(kFile, c_szFileName, &pvData, __FUNCTION__))
	{
		Tracenf("CItemManager::LoadItemDesc(c_szFileName=%s) - Load Error", c_szFileName);
		return false;
	}

	CMemoryTextFileLoader kTextFileLoader;
	kTextFileLoader.Bind(kFile.Size(), pvData);

	std::string stTemp;

	CTokenVector kTokenVector;
	for (DWORD i = 0; i < kTextFileLoader.GetLineCount(); ++i)
	{
		if (!kTextFileLoader.SplitLineByTab(i, &kTokenVector))
			continue;

		while (kTokenVector.size() < ITEMDESC_COL_NUM)
			kTokenVector.push_back("");

		//assert(kTokenVector.size()==ITEMDESC_COL_NUM);

		DWORD dwVnum = atoi(kTokenVector[ITEMDESC_COL_VNUM].c_str());
		const std::string& c_rstDesc = kTokenVector[ITEMDESC_COL_DESC];
		const std::string& c_rstSumm = kTokenVector[ITEMDESC_COL_SUMM];
		TItemMap::iterator f = m_ItemMap.find(dwVnum);
		if (m_ItemMap.end() == f)
			continue;

		CItemData* pkItemDataFind = f->second;

		pkItemDataFind->SetDescription(__SnapString(c_rstDesc, stTemp));
		pkItemDataFind->SetSummary(__SnapString(c_rstSumm, stTemp));
	}
	return true;
}

DWORD GetHashCode(const char* pString)
{
	uint32_t i, len;
	uint32_t ch;
	uint32_t result;

	len = strlen(pString);
	result = 5381;
	for (i = 0; i < len; i++)
	{
		ch = (uint32_t)pString[i];
		result = ((result << 5) + result) + ch; // hash * 33 + ch
	}

	return result;
}

bool CItemManager::LoadItemTable(const char* c_szFileName)
{
	CMappedFile file;
	LPCVOID pvData;

	if (!CFileSystem::Instance().Get(file, c_szFileName, &pvData, __FUNCTION__))
		return false;

	DWORD dwFourCC, dwElements, dwDataSize;
	DWORD dwVersion = 0;
	DWORD dwStride = 0;

	file.Read(&dwFourCC, sizeof(DWORD));

	if (dwFourCC == MAKEFOURCC('M', 'I', 'P', 'X'))
	{
		file.Read(&dwVersion, sizeof(DWORD));
		file.Read(&dwStride, sizeof(DWORD));

		if (dwVersion != 1)
		{
			TraceError("CPythonItem::LoadItemTable: invalid item_proto[%s] VERSION[%d]", c_szFileName, dwVersion);
			return false;
		}

#ifdef ENABLE_PROTOSTRUCT_AUTODETECT
		if (!CItemData::TItemTableAll::IsValidStruct(dwStride))
#else
		if (dwStride != sizeof(CItemData::TItemTable))
#endif
		{
			TraceError("CPythonItem::LoadItemTable: invalid item_proto[%s] STRIDE[%d] != sizeof(SItemTable)",
				c_szFileName, dwStride, sizeof(CItemData::TItemTable));
			return false;
		}
	}
	else if (dwFourCC != MAKEFOURCC('M', 'I', 'P', 'T'))
	{
		TraceError("CPythonItem::LoadItemTable: invalid item proto type %s", c_szFileName);
		return false;
	}

	file.Read(&dwElements, sizeof(DWORD));
	file.Read(&dwDataSize, sizeof(DWORD));

	std::unique_ptr<BYTE[]> pbData(new BYTE[dwDataSize]());
	file.Read(pbData.get(), dwDataSize);

	/////

	CLZObject zObj;

	if (!CLZO::Instance().Decompress(zObj, pbData.get(), s_adwItemProtoKey))
	{
//		delete[] pbData;
		return false;
	}

	/////

	char szName[64 + 1];
	std::map<DWORD, DWORD> itemNameMap;

	for (DWORD i = 0; i < dwElements; ++i)
	{
#ifdef ENABLE_PROTOSTRUCT_AUTODETECT
		CItemData::TItemTable t = { 0 };
		CItemData::TItemTableAll::Process(zObj.GetBuffer(), dwStride, i, t);
#else
		CItemData::TItemTable& t = *((CItemData::TItemTable*)zObj.GetBuffer() + i);
#endif
		CItemData::TItemTable* table = &t;

		CItemData* pItemData;
		DWORD dwVnum = table->dwVnum;

		TItemMap::iterator f = m_ItemMap.find(dwVnum);
		if (m_ItemMap.end() == f)
		{
			_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", dwVnum);
#ifdef INGAME_WIKI
			pItemData = CItemData::New();
#endif
			if (CResourceManager::Instance().IsFileExist(szName, __FUNCTION__) == false)
			{
				std::map<DWORD, DWORD>::iterator itVnum = itemNameMap.find(GetHashCode(table->szName));

				if (itVnum != itemNameMap.end())
					_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", itVnum->second);
				else
					_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", dwVnum - dwVnum % 10);

				if (CResourceManager::Instance().IsFileExist(szName, __FUNCTION__) == false)
				{
#ifdef INGAME_WIKI
					pItemData->ValidateImage(false);
#endif
#ifdef _DEBUG
					TraceError("%16s(#%-5d) cannot find icon file. setting to default.", table->szName, dwVnum);
#endif
					const DWORD EmptyBowl = 27995;
					_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", EmptyBowl);
				}
			}

#if !defined(INGAME_WIKI)
			pItemData = CItemData::New();
#endif

			pItemData->SetDefaultItemData(szName);
			m_ItemMap.insert(TItemMap::value_type(dwVnum, pItemData));
#ifdef INGAME_WIKI
			pItemData->SetItemTableData(table);
			if (!CResourceManager::Instance().IsFileExist(pItemData->GetIconFileName().c_str(), __FUNCTION__))
				pItemData->ValidateImage(false);
#endif
		}
		else
		{
			pItemData = f->second;
#ifdef INGAME_WIKI
			pItemData->SetItemTableData(table);
#endif
		}
		if (itemNameMap.find(GetHashCode(table->szName)) == itemNameMap.end())
			itemNameMap.insert(std::map<DWORD, DWORD>::value_type(GetHashCode(table->szName), table->dwVnum));
		pItemData->SetItemTableData(table);
		if (0 != table->dwVnumRange)
		{
			m_vec_ItemRange.push_back(pItemData);
		}
	}

//	delete[] pbData;
	return true;
}

#if defined(ENABLE_SHINING_SYSTEM)
bool CItemManager::LoadShiningTable(const char* szShiningTable)
{
	CMappedFile File;
	LPCVOID pData;

	if (!CFileSystem::Instance().Get(File, szShiningTable, &pData, __FUNCTION__)) {
		return false;
	}

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(File.Size(), pData);

	CTokenVector TokenVector;
	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &TokenVector, "\t"))
			continue;

		if (TokenVector.size() > (1 + CItemData::ITEM_SHINING_MAX_COUNT))
		{
			TraceError("CItemManager::LoadShiningTable(%s) - LoadShiningTable in %d\n - RowSize: %d MaxRowSize: %d", szShiningTable, i, TokenVector.size(), CItemData::ITEM_SHINING_MAX_COUNT);
		}

		const std::string & c_rstrID = TokenVector[0];
		DWORD dwItemVNum = atoi(c_rstrID.c_str());

		CItemData * pItemData = MakeItemData(dwItemVNum);
		if (pItemData)
		{
			for (BYTE i = 0; i < CItemData::ITEM_SHINING_MAX_COUNT; i++)
			{
				if (i < (TokenVector.size()-1))
				{
					const std::string & c_rstrEffectPath = TokenVector[i + 1];
					pItemData->SetItemShiningTableData(i, c_rstrEffectPath.c_str());
				}
				else
				{
					pItemData->SetItemShiningTableData(i, "");
				}
			}
		}
	}

	return true;
}
#endif

#ifdef ENABLE_ITEM_EXTRA_PROTO
bool CItemManager::LoadItemExtraProto(std::string filename)
{
	CMappedFile file;
	LPCVOID pvData;

	if (!CFileSystem::Instance().Get(file, filename.c_str(), &pvData, __FUNCTION__))
		return false;

	CMemoryTextFileLoader kTextFileLoader;
	kTextFileLoader.Bind(file.Size(), pvData);

	std::string stTemp;
	CItemData::TItemExtraProto Proto;

	CTokenVector kTokenVector;
	for (DWORD i = 0; i < kTextFileLoader.GetLineCount(); ++i)
	{
		if (i == 0)
		{
			auto line = kTextFileLoader.GetLineString(0);
			if (line.find("vnum") != std::string::npos)
				continue;
		}

		if (!kTextFileLoader.SplitLineByTab(i, &kTokenVector))
			continue;

		if (kTokenVector.size() != CItemData::ITEM_EXTRA_PROTO_FIELD_COUNT)
		{
			TraceError("Invalid line (Item extra proto) %d ", i + 1);
			continue;
		}

		auto& vnum = kTokenVector[CItemData::EItemExtraProto::ITEM_VNUM];
		Proto.dwVnum = strtoul(vnum.c_str(), nullptr, 10);
#ifdef ENABLE_RARITY_SYSTEM
		auto& rarity = kTokenVector[CItemData::EItemExtraProto::ITEM_RARITY];
		Proto.iRarity = strtol(rarity.c_str(), nullptr, 10);
#endif

#ifdef ENABLE_NEW_EXTRA_BONUS
		int index = 0;
		for (int i = (int)CItemData::EItemExtraProto::ITEM_EXTRA_BONUS_START; i <= (int)CItemData::EItemExtraProto::ITEM_EXTRA_BONUS_END; i += 2, index++)
		{
			auto& type = kTokenVector[i];
			auto& value = kTokenVector[i + 1];
			Proto.ExtraBonus[index].bType = atoi(type.c_str());
			Proto.ExtraBonus[index].lValue = atoi(value.c_str());
		}
#endif
		m_map_extraProto.emplace(Proto.dwVnum, Proto);
	}

	return true;
}


CItemData::TItemExtraProto* CItemManager::GetExtraProto(DWORD vnum)
{
	auto Iter = m_map_extraProto.find(vnum);
	if (Iter != m_map_extraProto.end())
		return &(Iter->second);
	return nullptr;
}


#endif



void CItemManager::Destroy()
{
	TItemMap::iterator i;
	for (i = m_ItemMap.begin(); i != m_ItemMap.end(); ++i)
		CItemData::Delete(i->second);

	m_ItemMap.clear();
#ifdef INGAME_WIKI
	m_tempItemVec.clear();
#endif
}



#ifdef ENABLE_ACCE_SYSTEM
bool CItemManager::LoadItemScale(const char* path) {
	CMappedFile File;
	LPCVOID pData;
	if (!CFileSystem::Instance().Get(File, path, &pData, __FUNCTION__)) {
		return false;
	}

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(File.Size(), pData);

	CTokenVector kTokenVector;
	for (uint32_t i = 0; i < textFileLoader.GetLineCount(); ++i) {
		if (!textFileLoader.SplitLineByTab(i, &kTokenVector)) {
			continue;
		} else if (kTokenVector.size() < 5) {
			TraceError("LoadItemScale: invalid line %u.", i);
			continue;
		}

		uint8_t c = 0;
		const std::string& s_id = kTokenVector[c++];
		const std::string& s_job = kTokenVector[c++];
		const std::string& s_sex = kTokenVector[c++];
		const std::string& s_x = kTokenVector[c++];
		const std::string& s_y = kTokenVector[c++];
		const std::string& s_z = kTokenVector[c];

		uint32_t id = atoi(s_id.c_str());
		uint8_t job = 0;
		if (!strcmp(s_job.c_str(), "JOB_WARRIOR")) {
			job = NRaceData::JOB_WARRIOR;
		} else if (!strcmp(s_job.c_str(), "JOB_ASSASSIN")) {
			job = NRaceData::JOB_ASSASSIN;
		} else if (!strcmp(s_job.c_str(), "JOB_SURA")) {
			job = NRaceData::JOB_SURA;
		} else if (!strcmp(s_job.c_str(), "JOB_SHAMAN")) {
			job = NRaceData::JOB_SHAMAN;
		}

		for (uint8_t j = 0; j < CItemData::ITEM_ACCE_GRADE_MAX_NUM; j++) {
			CItemData* pItemData = MakeItemData(id + j);
			if (pItemData) {
				pItemData->SetItemScale(job, s_sex[0] == 'M' ? 1 : 0, atof(s_x.c_str()), atof(s_y.c_str()), atof(s_z.c_str()));
			}
		}
	}

	return true;
}
#endif

#ifdef INGAME_WIKI
bool CItemManager::IsUniqueRealItem(const uint32_t vnum) const {
	if (vnum >= 49309 && vnum <= 49312) {
		return true;
	} else if (vnum >= 49349 && vnum <= 49352) {
		return true;
	}

	return false;
}

bool CItemManager::CanLoadWikiItem(DWORD dwVnum)
{
	DWORD StartRefineVnum = GetWikiItemStartRefineVnum(dwVnum);

	if (StartRefineVnum != dwVnum)
		return false;

	if (IsUniqueRealItem(dwVnum) == false && StartRefineVnum % 10 != 0)
		return false;

	CItemData* tbl = nullptr;
	if (!GetItemDataPointer(StartRefineVnum, &tbl))
		return false;

	return true;
}

DWORD CItemManager::GetWikiItemStartRefineVnum(DWORD dwVnum)
{
	if (IsUniqueRealItem(dwVnum)) {
		return dwVnum;
	}

	auto baseItemName = GetWikiItemBaseRefineName(dwVnum);
	if (!baseItemName.size())
		return 0;

	DWORD manage_vnum = dwVnum;
	while (!(strcmp(baseItemName.c_str(), GetWikiItemBaseRefineName(manage_vnum).c_str())))
		--manage_vnum;

	return (manage_vnum + 1);
}

std::string CItemManager::GetWikiItemBaseRefineName(DWORD dwVnum)
{
	CItemData* tbl = nullptr;
	if (!GetItemDataPointer(dwVnum, &tbl))
		return "";

	auto* p = const_cast<char*>(strrchr(tbl->GetName(), '+'));
	if (!p)
		return "";

	std::string sFirstItemName(tbl->GetName(),
		(tbl->GetName() + (p - tbl->GetName())));

	return sFirstItemName;
}

bool CItemManager::IsFilteredAntiflag(CItemData* itemData, DWORD raceFilter)
{
	if (raceFilter != 0)
	{
		if (!itemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_SHAMAN) && raceFilter & CItemData::ITEM_ANTIFLAG_SHAMAN)
			return false;

		if (!itemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_SURA) && raceFilter & CItemData::ITEM_ANTIFLAG_SURA)
			return false;

		if (!itemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_ASSASSIN) && raceFilter & CItemData::ITEM_ANTIFLAG_ASSASSIN)
			return false;

		if (!itemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_WARRIOR) && raceFilter & CItemData::ITEM_ANTIFLAG_WARRIOR)
			return false;

#ifdef INGAME_WIKI_WOLFMAN
		if (!itemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_WOLFMAN) && raceFilter & CItemData::ITEM_ANTIFLAG_WOLFMAN)
			return false;
#endif
	}

	return true;
}

size_t CItemManager::WikiLoadClassItems(BYTE classType, DWORD raceFilter)
{
	m_tempItemVec.clear();

	for (TItemMap::iterator it = m_ItemMap.begin(); it != m_ItemMap.end(); ++it)
	{
		if (!it->second->IsValidImage() || it->first < 10 || it->second->IsBlacklisted())
			continue;

		bool _can_load = CanLoadWikiItem(it->first);

		switch (classType)
		{
		case 0: // weapon
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_WEAPON && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 1: // body
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_BODY && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 2:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_EAR && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 3:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_FOOTS && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 4:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_HEAD && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 5:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_NECK && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 6:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_SHIELD && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 7:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_WRIST && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 8: // chests
			if (it->second->GetType() == CItemData::ITEM_TYPE_GIFTBOX)
				m_tempItemVec.push_back(it->first);
			break;
		case 9: // belts
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_BELT)
				m_tempItemVec.push_back(it->first);
			break;
		case 10: // talisman
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_PENDANT && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		}
	}

	return m_tempItemVec.size();
}

std::tuple<const char*, int> CItemManager::SelectByNamePart(const char* namePart)
{
	char searchName[CItemData::ITEM_NAME_MAX_LEN + 1];
	memcpy(searchName, namePart, sizeof(searchName));
	for (size_t j = 0; j < sizeof(searchName); j++)
		searchName[j] = static_cast<char>(tolower(searchName[j]));
	std::string tempSearchName = searchName;

	for (TItemMap::iterator i = m_ItemMap.begin(); i != m_ItemMap.end(); i++)
	{
		const CItemData::TItemTable* tbl = i->second->GetTable();

		if (!i->second->IsBlacklisted())
		{
			DWORD StartRefineVnum = GetWikiItemStartRefineVnum(i->first);
			if (StartRefineVnum != 0)
			{
				CItemData* _sRb = nullptr;
				if (!GetItemDataPointer(StartRefineVnum, &_sRb))
					continue;

				if (_sRb->IsBlacklisted())
					continue;
			}
		}
		else
			continue;

		CItemData* itemData = nullptr;
		if (!GetItemDataPointer(i->first, &itemData))
			continue;

		std::string tempName = itemData->GetName();
		if (!tempName.size())
			continue;

		std::transform(tempName.begin(), tempName.end(), tempName.begin(), ::tolower);

		const size_t tempSearchNameLenght = tempSearchName.length();
		if (tempName.length() < tempSearchNameLenght)
			continue;

		if (!tempName.substr(0, tempSearchNameLenght).compare(tempSearchName))
			return std::make_tuple(itemData->GetName(), i->first);
	}

	return std::make_tuple("", -1);
}
#endif

#if defined(ENABLE_AURA_SYSTEM)
bool CItemManager::LoadAuraScale(const char* c_szFileName) {
	CMappedFile File;
	LPCVOID pData;

	if (!CFileSystem::Instance().Get(File, c_szFileName, &pData, __FUNCTION__)) {
		return false;
	}

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(File.Size(), pData);

	CTokenVector TokenVector;
	for (auto i = 0; i < textFileLoader.GetLineCount(); ++i) {
		if (!textFileLoader.SplitLine(i, &TokenVector, "\t")) {
			continue;
		}

		if (TokenVector.size() != AURA_SCALE_COL_NUM) {
			TraceError(" CItemManager::LoadAuraScale(%s) - Error on line %d\n", c_szFileName, i);
			continue;
		}

		const std::string& c_rstrID = TokenVector[AURA_SCALE_COL_VNUM];
		const std::string& c_rstrJob = TokenVector[AURA_SCALE_COL_JOB];
		const std::string& c_rstrSex = TokenVector[AURA_SCALE_COL_SEX];
		const std::string& c_rstrMeshScaleX = TokenVector[AURA_SCALE_COL_MESH_SCALE_X];
		const std::string& c_rstrMeshScaleY = TokenVector[AURA_SCALE_COL_MESH_SCALE_Y];
		const std::string& c_rstrMeshScaleZ = TokenVector[AURA_SCALE_COL_MESH_SCALE_Z];
		const std::string& c_rstrParticleScale = TokenVector[AURA_SCALE_COL_PARTICLE_SCALE];

		uint32_t dwItemVnum = atoi(c_rstrID.c_str());
		uint8_t byJob = 0;

		if (!strcmp(c_rstrJob.c_str(), "JOB_WARRIOR")) {
			byJob = NRaceData::JOB_WARRIOR;
		} else if (!strcmp(c_rstrJob.c_str(), "JOB_ASSASSIN")) {
			byJob = NRaceData::JOB_ASSASSIN;
		} else if (!strcmp(c_rstrJob.c_str(), "JOB_SURA")) {
			byJob = NRaceData::JOB_SURA;
		} else if (!strcmp(c_rstrJob.c_str(), "JOB_SHAMAN")) {
			byJob = NRaceData::JOB_SHAMAN;
		}

		uint8_t bySex = c_rstrSex[0] == 'M';

		float fMeshScaleX = atof(c_rstrMeshScaleX.c_str()) * 0.01f;
		float fMeshScaleY = atof(c_rstrMeshScaleY.c_str()) * 0.01f;
		float fMeshScaleZ = atof(c_rstrMeshScaleZ.c_str()) * 0.01f;
		float fParticleScale = atof(c_rstrParticleScale.c_str());

		for (uint8_t i = 0; i < CItemData::AURA_GRADE_MAX_NUM; ++i) {
			CItemData* pItemData = MakeItemData(dwItemVnum + i);
			if (pItemData) {
				pItemData->SetAuraScaleTableData(byJob, bySex, fMeshScaleX, fMeshScaleY, fMeshScaleZ, fParticleScale);
			}
		}
	}

	return true;
}
#endif

CItemManager::CItemManager() : m_pSelectedItemData(NULL)
{
}
CItemManager::~CItemManager()
{
	Destroy();
}
