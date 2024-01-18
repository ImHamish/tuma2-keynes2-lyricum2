#include "StdAfx.h"
#include "../eterLib/ResourceManager.h"

#include "ItemData.h"
#if defined(ENABLE_AURA_SYSTEM)
#include "../EffectLib/EffectManager.h"
#endif

CDynamicPool<CItemData>		CItemData::ms_kPool;

extern DWORD GetDefaultCodePage();

CItemData* CItemData::New()
{
	return ms_kPool.Alloc();
}

void CItemData::Delete(CItemData* pkItemData)
{
	pkItemData->Clear();
	ms_kPool.Free(pkItemData);
}

void CItemData::DestroySystem()
{
	ms_kPool.Destroy();
}

CGraphicThing * CItemData::GetModelThing()
{
	return m_pModelThing;
}

CGraphicThing * CItemData::GetSubModelThing()
{
	if (m_pSubModelThing)
		return m_pSubModelThing;
	else
		return m_pModelThing;
}

CGraphicThing * CItemData::GetDropModelThing()
{
	return m_pDropModelThing;
}

CGraphicSubImage * CItemData::GetIconImage()
{
	if(m_pIconImage == NULL && m_strIconFileName.empty() == false)
		__SetIconImage(m_strIconFileName.c_str());
	return m_pIconImage;
}

DWORD CItemData::GetLODModelThingCount()
{
	return m_pLODModelThingVector.size();
}

BOOL CItemData::GetLODModelThingPointer(DWORD dwIndex, CGraphicThing ** ppModelThing)
{
	if (dwIndex >= m_pLODModelThingVector.size())
		return FALSE;

	*ppModelThing = m_pLODModelThingVector[dwIndex];

	return TRUE;
}

DWORD CItemData::GetAttachingDataCount()
{
	return m_AttachingDataVector.size();
}

BOOL CItemData::GetCollisionDataPointer(DWORD dwIndex, const NRaceData::TAttachingData ** c_ppAttachingData)
{
	if (dwIndex >= GetAttachingDataCount())
		return FALSE;

	if (NRaceData::ATTACHING_DATA_TYPE_COLLISION_DATA != m_AttachingDataVector[dwIndex].dwType)
		return FALSE;

	*c_ppAttachingData = &m_AttachingDataVector[dwIndex];
	return TRUE;
}

BOOL CItemData::GetAttachingDataPointer(DWORD dwIndex, const NRaceData::TAttachingData ** c_ppAttachingData)
{
	if (dwIndex >= GetAttachingDataCount())
		return FALSE;

	*c_ppAttachingData = &m_AttachingDataVector[dwIndex];
	return TRUE;
}

void CItemData::SetSummary(const std::string& c_rstSumm)
{
	m_strSummary=c_rstSumm;
}

void CItemData::SetDescription(const std::string& c_rstDesc)
{
	m_strDescription=c_rstDesc;
}
/*
BOOL CItemData::LoadItemData(const char * c_szFileName)
{
	CTextFileLoader TextFileLoader;

	if (!TextFileLoader.Load(c_szFileName))
	{
		//Lognf(1, "CItemData::LoadItemData(c_szFileName=%s) - FAILED", c_szFileName);
		return FALSE;
	}

	TextFileLoader.SetTop();

	TextFileLoader.GetTokenString("modelfilename", &m_strModelFileName);
	TextFileLoader.GetTokenString("submodelfilename", &m_strSubModelFileName);
	TextFileLoader.GetTokenString("dropmodelfilename", &m_strDropModelFileName);
	TextFileLoader.GetTokenString("iconimagefilename", &m_strIconFileName);

	char szDescriptionKey[32+1];
	_snprintf(szDescriptionKey, 32, "%ddescription", GetDefaultCodePage());
	if (!TextFileLoader.GetTokenString(szDescriptionKey, &m_strDescription))
	{
		TextFileLoader.GetTokenString("description", &m_strDescription);
	}

	// LOD Model File Name List
	CTokenVector * pLODModelList;
	if (TextFileLoader.GetTokenVector("lodmodellist", &pLODModelList))
	{
		m_strLODModelFileNameVector.clear();
		m_strLODModelFileNameVector.resize(pLODModelList->size());

		for (DWORD i = 0; i < pLODModelList->size(); ++i)
		{
			m_strLODModelFileNameVector[i] = pLODModelList->at(0);
		}
	}

	// Attaching Data
	// Item 에 Attaching Data 일단 없음.
//	if (TextFileLoader.SetChildNode("attachingdata"))
//	{
//		if (!NRaceData::LoadAttachingData(TextFileLoader, &m_AttachingDataVector))
//			return FALSE;
//
//		TextFileLoader.SetParentNode();
//	}

	__LoadFiles();

	return TRUE;
}
*/
void CItemData::SetDefaultItemData(const char * c_szIconFileName, const char * c_szModelFileName)
{
	if(c_szModelFileName)
	{
		m_strModelFileName = c_szModelFileName;
		m_strDropModelFileName = c_szModelFileName;
	}
	else
	{
		m_strModelFileName = "";
		m_strDropModelFileName = "d:/ymir work/item/etc/item_bag.gr2";
	}
	m_strIconFileName = c_szIconFileName;

	m_strSubModelFileName = "";
	m_strDescription = "";
	m_strSummary = "";
	memset(m_ItemTable.alSockets, 0, sizeof(m_ItemTable.alSockets));

	__LoadFiles();
}

void CItemData::__LoadFiles()
{
	// Model File Name
	if (!m_strModelFileName.empty())
		m_pModelThing = (CGraphicThing *)CResourceManager::Instance().GetResourcePointer(m_strModelFileName.c_str());

	if (!m_strSubModelFileName.empty())
		m_pSubModelThing = (CGraphicThing *)CResourceManager::Instance().GetResourcePointer(m_strSubModelFileName.c_str());

	if (!m_strDropModelFileName.empty())
		m_pDropModelThing = (CGraphicThing *)CResourceManager::Instance().GetResourcePointer(m_strDropModelFileName.c_str());


	if (!m_strLODModelFileNameVector.empty())
	{
		m_pLODModelThingVector.clear();
		m_pLODModelThingVector.resize(m_strLODModelFileNameVector.size());

		for (DWORD i = 0; i < m_strLODModelFileNameVector.size(); ++i)
		{
			const std::string & c_rstrLODModelFileName = m_strLODModelFileNameVector[i];
			m_pLODModelThingVector[i] = (CGraphicThing *)CResourceManager::Instance().GetResourcePointer(c_rstrLODModelFileName.c_str());
		}
	}
}

#define ENABLE_LOAD_ALTER_ITEMICON
void CItemData::__SetIconImage(const char * c_szFileName)
{
	if (!CResourceManager::Instance().IsFileExist(c_szFileName, __FUNCTION__))
	{
		TraceError("%s not found. CItemData::__SetIconImage",c_szFileName);
		m_pIconImage = NULL;
#ifdef ENABLE_LOAD_ALTER_ITEMICON
		static const char* c_szAlterIconImage = "icon/item/27995.tga";
		if (CResourceManager::Instance().IsFileExist(c_szAlterIconImage, __FUNCTION__))
			m_pIconImage = (CGraphicSubImage *)CResourceManager::Instance().GetResourcePointer(c_szAlterIconImage);
#endif
	}
	else if (m_pIconImage == NULL)
		m_pIconImage = (CGraphicSubImage *)CResourceManager::Instance().GetResourcePointer(c_szFileName);
}

void CItemData::SetItemTableData(TItemTable * pItemTable)
{
	memcpy(&m_ItemTable, pItemTable, sizeof(TItemTable));
}

#if defined(ENABLE_ACCE_SYSTEM)
void CItemData::SetItemScale(uint8_t job, uint8_t sex, float x, float y, float z) {
	m_itemScale[job][sex].x = x * 0.01f;
	m_itemScale[job][sex].y = y * 0.01f;
	m_itemScale[job][sex].z = z * 0.01f;
}

bool CItemData::GetItemScale(uint8_t job, uint8_t sex, D3DXVECTOR3& t) {
	if (job >= NRaceData::JOB_MAX_NUM || sex >= NRaceData::SEX_MAX_NUM) {
		return false;
	}

	t = m_itemScale[job][sex];
	return true;
}
#endif

const CItemData::TItemTable* CItemData::GetTable() const
{
	return &m_ItemTable;
}

DWORD CItemData::GetIndex() const
{
	return m_ItemTable.dwVnum;
}

const char* CItemData::GetName() const
{
	return m_ItemTable.szLocaleName;
}

const char * CItemData::GetDescription() const
{
	return m_strDescription.c_str();
}

const char * CItemData::GetSummary() const
{
	return m_strSummary.c_str();
}


BYTE CItemData::GetType() const
{
	return m_ItemTable.bType;
}

BYTE CItemData::GetSubType() const
{
	return m_ItemTable.bSubType;
}

#define DEF_STR(x) #x

const char* CItemData::GetUseTypeString() const
{
	if (GetType() != CItemData::ITEM_TYPE_USE)
		return "NOT_USE_TYPE";

	switch (GetSubType())
	{
		case USE_TUNING:
			return DEF_STR(USE_TUNING);
		case USE_DETACHMENT:
			return DEF_STR(USE_DETACHMENT);
		case USE_CLEAN_SOCKET:
			return DEF_STR(USE_CLEAN_SOCKET);
		case USE_CHANGE_ATTRIBUTE:
			return DEF_STR(USE_CHANGE_ATTRIBUTE);
#if defined(USE_ATTR_6TH_7TH)
		case USE_CHANGE_ATTRIBUTE2:
			return DEF_STR(USE_CHANGE_ATTRIBUTE2);
#endif
		case USE_ADD_ATTRIBUTE:
			return DEF_STR(USE_ADD_ATTRIBUTE);
		case USE_ADD_ATTRIBUTE2:
			return DEF_STR(USE_ADD_ATTRIBUTE2);
		case USE_ADD_ACCESSORY_SOCKET:
			return DEF_STR(USE_ADD_ACCESSORY_SOCKET);
		case USE_PUT_INTO_ACCESSORY_SOCKET:
			return DEF_STR(USE_PUT_INTO_ACCESSORY_SOCKET);
		case USE_PUT_INTO_BELT_SOCKET:
			return DEF_STR(USE_PUT_INTO_BELT_SOCKET);
		case USE_PUT_INTO_RING_SOCKET:
			return DEF_STR(USE_PUT_INTO_RING_SOCKET);
#ifdef ENABLE_USE_COSTUME_ATTR
		case USE_CHANGE_COSTUME_ATTR:
			return DEF_STR(USE_CHANGE_COSTUME_ATTR);
		case USE_RESET_COSTUME_ATTR:
			return DEF_STR(USE_RESET_COSTUME_ATTR);
#endif
#ifdef ATTR_LOCK
		case USE_ADD_ATTRIBUTE_LOCK:
			return DEF_STR(USE_ADD_ATTRIBUTE);
		case USE_CHANGE_ATTRIBUTE_LOCK:
			return DEF_STR(USE_CHANGE_ATTRIBUTE);
		case USE_DELETE_ATTRIBUTE_LOCK:
			return DEF_STR(USE_CHANGE_ATTRIBUTE);
#endif
#ifdef ENABLE_ATTR_COSTUMES
		case USE_CHANGE_ATTR_COSTUME:
			return DEF_STR(USE_CHANGE_ATTR_COSTUME);
		case USE_ADD_ATTR_COSTUME1:
			return DEF_STR(USE_ADD_ATTR_COSTUME1);
		case USE_ADD_ATTR_COSTUME2:
			return DEF_STR(USE_ADD_ATTR_COSTUME2);
		case USE_REMOVE_ATTR_COSTUME:
			return DEF_STR(USE_REMOVE_ATTR_COSTUME);
#endif
#ifdef ENABLE_NEW_PET_EDITS
		case USE_PET_REVIVE:
			return DEF_STR(USE_PET_REVIVE);
		case USE_PET_ENCHANT:
			return DEF_STR(USE_PET_ENCHANT);
#endif
#ifdef ENABLE_REMOTE_ATTR_SASH_REMOVE
		case USE_ATTR_SASH_REMOVE:
			return DEF_STR(USE_ATTR_SASH_REMOVE);
#endif
#ifdef ENABLE_STOLE_COSTUME
		case USE_ENCHANT_STOLE:
			return DEF_STR(USE_ENCHANT_STOLE);
#endif
#if defined(USE_NEW_ENCHANT_ATTR)
		case USE_SPECIAL_ENCHANT:
			return DEF_STR(USE_SPECIAL_ENCHANT);
#endif
#if defined(USE_AURA_ATTR_REMOVER)
		case USE_AURA_ATTRREMOVER:
			return DEF_STR(USE_AURA_ATTRREMOVER);
#endif

		case USE_REMOVE_ACCESSORY_SOCKETS:
			return DEF_STR(USE_REMOVE_ACCESSORY_SOCKETS);
	}
	return "USE_UNKNOWN_TYPE";
}


DWORD CItemData::GetWeaponType() const
{
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (GetType()==CItemData::ITEM_TYPE_COSTUME && GetSubType()==CItemData::COSTUME_WEAPON)
		return GetValue(3);
#endif
	return m_ItemTable.bSubType;
}

BYTE CItemData::GetSize() const
{
	return m_ItemTable.bSize;
}

BOOL CItemData::IsAntiFlag(DWORD dwFlag) const
{
	return (dwFlag & m_ItemTable.dwAntiFlags) != 0;
}

BOOL CItemData::IsFlag(DWORD dwFlag) const
{
	return (dwFlag & m_ItemTable.dwFlags) != 0;
}

BOOL CItemData::IsWearableFlag(DWORD dwFlag) const
{
	return (dwFlag & m_ItemTable.dwWearFlags) != 0;
}

BOOL CItemData::HasNextGrade() const
{
	return 0 != m_ItemTable.dwRefinedVnum;
}

DWORD CItemData::GetWearFlags() const
{
	return m_ItemTable.dwWearFlags;
}

DWORD CItemData::GetIBuyItemPrice() const
{
	return m_ItemTable.dwIBuyItemPrice;
}

DWORD CItemData::GetISellItemPrice() const
{
	return m_ItemTable.dwISellItemPrice;
}


BOOL CItemData::GetLimit(BYTE byIndex, TItemLimit * pItemLimit) const
{
	if (byIndex >= ITEM_LIMIT_MAX_NUM)
	{
		assert(byIndex < ITEM_LIMIT_MAX_NUM);
		return FALSE;
	}

	*pItemLimit = m_ItemTable.aLimits[byIndex];

	return TRUE;
}

BOOL CItemData::GetApply(BYTE byIndex, TItemApply * pItemApply) const
{
	if (byIndex >= ITEM_APPLY_MAX_NUM)
	{
		assert(byIndex < ITEM_APPLY_MAX_NUM);
		return FALSE;
	}

	*pItemApply = m_ItemTable.aApplies[byIndex];
	return TRUE;
}

long CItemData::GetValue(BYTE byIndex) const
{
	if (byIndex >= ITEM_VALUES_MAX_NUM)
	{
		assert(byIndex < ITEM_VALUES_MAX_NUM);
		return 0;
	}

	return m_ItemTable.alValues[byIndex];
}

long CItemData::SetSocket(BYTE byIndex,DWORD value)
{
	if (byIndex >= ITEM_SOCKET_MAX_NUM)
	{
		assert(byIndex < ITEM_SOCKET_MAX_NUM);
		return -1;
	}

	return m_ItemTable.alSockets[byIndex] = value;
}

long CItemData::GetSocket(BYTE byIndex) const
{
	if (byIndex >= ITEM_SOCKET_MAX_NUM)
	{
		assert(byIndex < ITEM_SOCKET_MAX_NUM);
		return -1;
	}

	return m_ItemTable.alSockets[byIndex];
}

//서버와 동일 서버 함수 변경시 같이 변경!!(이후에 합친다)
//SocketCount = 1 이면 초급무기
//SocketCount = 2 이면 중급무기
//SocketCount = 3 이면 고급무기
int CItemData::GetSocketCount() const
{
	return m_ItemTable.bGainSocketPct;
}

DWORD CItemData::GetIconNumber() const
{
	return m_ItemTable.dwVnum;
//!@#
//	return m_ItemTable.dwIconNumber;
}

UINT CItemData::GetSpecularPoweru() const
{
	return m_ItemTable.bSpecular;
}

float CItemData::GetSpecularPowerf() const
{
	UINT uSpecularPower=GetSpecularPoweru();

	return float(uSpecularPower) / 100.0f;
}

//refine 값은 아이템번호 끝자리와 일치한다-_-(테이블이용으로 바꿀 예정)
UINT CItemData::GetRefine() const
{
	return GetIndex()%10;
}

BOOL CItemData::IsEquipment() const
{
	switch (GetType())
	{
		case ITEM_TYPE_WEAPON:
		case ITEM_TYPE_ARMOR:
			return TRUE;
			break;
	}

	return FALSE;
}

#if defined(ENABLE_SHINING_SYSTEM)
void CItemData::SetItemShiningTableData(BYTE bIndex, const char* szEffectname)
{
	sprintf(m_ItemShiningTable.szShinings[bIndex], szEffectname);
}
#endif

#if defined(ENABLE_AURA_SYSTEM)
void CItemData::SetAuraScaleTableData(uint8_t byJob, uint8_t bySex, float fMeshScaleX, float fMeshScaleY, float fMeshScaleZ, float fParticleScale) {
	m_AuraScaleTable.v3MeshScale[bySex][byJob].x = fMeshScaleX;
	m_AuraScaleTable.v3MeshScale[bySex][byJob].y = fMeshScaleY;
	m_AuraScaleTable.v3MeshScale[bySex][byJob].z = fMeshScaleZ;
	m_AuraScaleTable.fParticleScale[bySex][byJob] = fParticleScale;
}

D3DXVECTOR3& CItemData::GetAuraMeshScaleVector(uint8_t byJob, uint8_t bySex) {
	return m_AuraScaleTable.v3MeshScale[bySex][byJob];
}

float CItemData::GetAuraParticleScale(uint8_t byJob, uint8_t bySex) {
	return m_AuraScaleTable.fParticleScale[bySex][byJob];
}

void CItemData::SetAuraEffectID(const char* szAuraEffectPath) {
	if (szAuraEffectPath) {
		CEffectManager::Instance().RegisterEffect2(szAuraEffectPath, &m_dwAuraEffectID);
	}
}
#endif

#if defined(USE_ATTR_6TH_7TH)
uint32_t CItemData::GetAttr67MaterialVnum() const
{
	TItemLimit ItemLimit;
	if (!GetLimit(0, &ItemLimit) && ItemLimit.bType != LIMIT_LEVEL)
	{
		return 0;
	}

	if (ItemLimit.lValue <= 29)
	{
		return 69070;
	}
	else if (ItemLimit.lValue == 30)
	{
		return 69078;
	}
	else if (ItemLimit.lValue >= 31 && ItemLimit.lValue <= 39)
	{
		return 69071;
	}
	else if (ItemLimit.lValue >= 40 && ItemLimit.lValue <= 49)
	{
		return 69072;
	}
	else if (ItemLimit.lValue >= 50 && ItemLimit.lValue <= 59)
	{
		return 69073;
	}
	else if (ItemLimit.lValue >= 60 && ItemLimit.lValue <= 74)
	{
		return 69074;
	}
	else if (ItemLimit.lValue == 75)
	{
		return 69079;
	}
	else if (ItemLimit.lValue >= 76 && ItemLimit.lValue <= 89)
	{
		return 69075;
	}
	else if (ItemLimit.lValue >= 90 && ItemLimit.lValue <= 104)
	{
		return 69076;
	}
	else if (ItemLimit.lValue == 105)
	{
		return 69080;
	}
	else if (ItemLimit.lValue >= 106 && ItemLimit.lValue <= 120)
	{
		return 69077;
	}
	else if (ItemLimit.lValue >= 120 && ItemLimit.lValue <= 150)
	{
		return 69081;
	}

#if defined(ENABLE_PENDANT)
	if (GetType() == ITEM_TYPE_ARMOR && GetSubType() == ARMOR_PENDANT)
	{
		return 69082;
	}
#endif

	return 0;
}
#endif

#ifdef USE_PICKUP_FILTER
int32_t CItemData::GetLevelLimit() const
{
    for (uint8_t i = 0; i < ITEM_LIMIT_MAX_NUM; i++)
    {
        if (m_ItemTable.aLimits[i].bType == LIMIT_LEVEL)
        {
            return m_ItemTable.aLimits[i].lValue;
        }
    }

    return 0;
}

int32_t CItemData::GetRefineLevel() const
{
    const char* name = m_ItemTable.szName;
    char* p = const_cast<char*>(strrchr(name, '+'));
    if (!p)
    {
        return 0;
    }

    int32_t rtn = 0;
    str_to_number(rtn, p + 1);

    const char* locale_name = m_ItemTable.szLocaleName;
    p = const_cast<char*>(strrchr(locale_name, '+'));
    if (p)
    {
        int32_t locale_rtn = 0;
        str_to_number(locale_rtn, p + 1);
        if (locale_rtn != rtn)
        {
            TraceError("refine_level_based_on_NAME(%d) is not equal to refine_level_based_on_LOCALE_NAME(%d).", rtn, locale_rtn);
        }
    }

    return rtn;
}

bool CItemData::FilterCheckRefine(uint8_t bValue1, uint8_t bValue2)
{
    if (bValue1 == 0
        && bValue2 == 200)
    {
        return true;
    }

    int32_t iValue = GetRefineLevel();
    if (iValue < bValue1
        || iValue > bValue2)
    {
        return false;
    }

    return true;
}

bool CItemData::FilterCheckLevel(uint8_t bValue1, uint8_t bValue2)
{
    if (bValue1 == 0
        && bValue2 == 120)
    {
        return true;
    }

    int32_t iValue = GetLevelLimit();
    if (iValue < bValue1
        || iValue > bValue2)
    {
        return false;
    }

    return true;
}
#endif

void CItemData::Clear()
{
    m_strSummary = "";
    m_strModelFileName = "";
    m_strSubModelFileName = "";
    m_strDropModelFileName = "";
    m_strIconFileName = "";
    m_strLODModelFileNameVector.clear();
    m_pModelThing = NULL;
    m_pSubModelThing = NULL;
    m_pDropModelThing = NULL;
    m_pIconImage = NULL;
    m_pLODModelThingVector.clear();

    memset(&m_ItemTable, 0, sizeof(m_ItemTable));

#ifdef INGAME_WIKI
    m_isValidImage = true;
    m_wikiInfo.isSet = false;
    m_wikiInfo.hasData = false;
    m_isBlacklisted = false;
#endif

#ifdef ENABLE_AURA_SYSTEM
    memset(&m_AuraScaleTable, 0, sizeof(m_AuraScaleTable));
    m_dwAuraEffectID = 0;
#endif

#ifdef ENABLE_ACCE_SYSTEM
    memset(&m_itemScale, 0, sizeof(m_itemScale));
#endif

#ifdef ENABLE_SHINING_SYSTEM
    memset(&m_ItemShiningTable, 0, sizeof(m_ItemShiningTable));
#endif
}

CItemData::CItemData()
{
    Clear();
}

CItemData::~CItemData()
{
}
