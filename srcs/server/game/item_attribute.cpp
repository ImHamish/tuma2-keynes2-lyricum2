#include "stdafx.h"
#include "constants.h"
#include "log.h"
#include "item.h"
#include "char.h"
#include "desc.h"
#include "item_manager.h"
#include "config.h"

int CItem::GetAttributeSetIndex()
{
	if (GetType() == ITEM_WEAPON) {
		if (GetSubType() == WEAPON_ARROW) {
			return -1;
		}

		return ATTRIBUTE_SET_WEAPON;
	}

	if (GetType() == ITEM_ARMOR)
	{
		switch (GetSubType())
		{
			case ARMOR_BODY:
				return ATTRIBUTE_SET_BODY;

			case ARMOR_WRIST:
				return ATTRIBUTE_SET_WRIST;

			case ARMOR_FOOTS:
				return ATTRIBUTE_SET_FOOTS;

			case ARMOR_NECK:
				return ATTRIBUTE_SET_NECK;

			case ARMOR_HEAD:
				return ATTRIBUTE_SET_HEAD;

			case ARMOR_SHIELD:
				return ATTRIBUTE_SET_SHIELD;

			case ARMOR_EAR:
				return ATTRIBUTE_SET_EAR;
		
#if defined(ENABLE_PENDANT) && defined(ENABLE_NEW_BONUS_TALISMAN)
			case ARMOR_PENDANT:
				return ATTRIBUTE_SET_PENDANT;
#endif
		}
	}
#ifdef ENABLE_ATTR_COSTUMES
	else if (GetType() == ITEM_COSTUME) {
		switch (GetSubType())
		{
			case COSTUME_BODY:
				return ATTRIBUTE_SET_COSTUME_BODY;
			case COSTUME_HAIR:
				return ATTRIBUTE_SET_COSTUME_HAIR;
			case COSTUME_WEAPON:
				return ATTRIBUTE_SET_COSTUME_WEAPON;
#ifdef ENABLE_STOLE_COSTUME
			case COSTUME_STOLE:
				return ATTRIBUTE_SET_COSTUME_STOLE;
#endif
			default:
				break;
		}
	}
#endif

	return -1;
}

#ifdef ENABLE_SWITCHBOT_WORLDARD

int CItem::ChangeAttributeSwtichBot(int count,const TPlayerItemAttribute* aAttr, bool* check_add)
{
	static const int tmpChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		0, 10, 40, 35, 15,
	};

	int max_count = 0;

	for (int a = 0; a < count; ++a)
	{
		int iAttributeCount = GetAttributeCount();
		ClearAttribute();
		if (iAttributeCount == 0)
			return 0;

		TItemTable const * pProto = GetProto();

		if (pProto && pProto->sAddonType)
		{
			ApplyAddon(pProto->sAddonType,true);
		}


		for (int b = GetAttributeCount(); b < iAttributeCount; ++b)
		{
			int iAttrLevelPercent = number(1, 100);
			int c;

			for (c = 0; c < ITEM_ATTRIBUTE_MAX_LEVEL; ++c)
			{
				if (iAttrLevelPercent <= tmpChangeProb[c])
					break;

				iAttrLevelPercent -= tmpChangeProb[c];
			}

			int iAttributeSet = GetAttributeSetIndex();

			if (iAttributeSet < 0)
				return 0;

			BYTE bLevel = c + 1;
			if (bLevel > ITEM_ATTRIBUTE_MAX_LEVEL)
				return 0;

			std::vector<int> avail;

			int total = 0;

			for (int d = 0; d < MAX_APPLY_NUM; ++d)
			{
				const TItemAttrTable & r = g_map_itemAttr[d];

				if (r.bMaxLevelBySet[iAttributeSet] && !HasAttr(d))
				{
					avail.push_back(d);
					total += r.dwProb;
				}
			}

			unsigned int prob = number(1, total);
			int attr_idx = APPLY_NONE;

			for (DWORD e = 0; e < avail.size(); ++e)
			{
				const TItemAttrTable & r = g_map_itemAttr[avail[e]];

				if (prob <= r.dwProb)
				{
					attr_idx = avail[e];
					break;
				}

				prob -= r.dwProb;
			}

			if (!attr_idx)
			{
				sys_err("Cannot put item attribute %d %d", iAttributeSet, bLevel);
				return 0;
			}

			const TItemAttrTable & r = g_map_itemAttr[attr_idx];

			if (bLevel > r.bMaxLevelBySet[iAttributeSet])
				bLevel = r.bMaxLevelBySet[iAttributeSet];

			if (HasAttr(attr_idx))
				return 0;

			if (bLevel <= 0)
				return 0;

			int i = GetAttributeCount();
			if (i == MAX_NORM_ATTR_NUM)
			{
//				sys_err("item attribute overflow!");
				sys_err("%u cu %u", GetOwner() ? GetOwner()->GetPlayerID() : 0, GetID());
			}
			else
			{
				const TItemAttrTable & r = g_map_itemAttr[attr_idx];
				long lVal = r.lValues[MIN(4, bLevel - 1)];
#ifdef ENABLE_ATTR_COSTUMES
				if (GetType() == ITEM_COSTUME)
					lVal = r.lValues[MIN(9, bLevel + 5 - 1)];
#endif
				if (lVal)
					SetAttributeSwtichBot(i, attr_idx, lVal);
			}
		}

		max_count++;

		if (CheckBonusSwitchBot(aAttr))
		{
			*check_add = true;
			return max_count;
		}
	}
	*check_add = false;
	return max_count;
}

int CItem::GetCountSelect(const TPlayerItemAttribute* aAttr)
{
	int count = 0;
	for (int i = 0; i < MAX_NORM_ATTR_NUM; ++i)
	{
		if(aAttr[i].bType != 0)
		{
			count ++;
		}
	}

	return count;
}

bool CItem::CheckBonusSwitchBot(const TPlayerItemAttribute* aAttr)
{	

	int iAttributeCount = GetAttributeCount();

	int max_count_select = GetCountSelect(aAttr);

	int count_total = 0;

	for (int i = 0; i < iAttributeCount; ++i)
	{
		for (int a = 0; a < MAX_NORM_ATTR_NUM; ++a)
		{
			if(aAttr[a].bType != 0)
			{
				if(aAttr[a].bType == GetAttributeType(i) && GetAttributeValue(i) >= aAttr[a].sValue)
				{
					count_total ++;
				}
			}
		}
		
	}

	if (count_total == max_count_select){
		return true;
	}

	return false;

}

void CItem::SetAttributeSwtichBot(int i, BYTE bType, short sValue)
{
	assert(i < MAX_NORM_ATTR_NUM);

	m_aAttr[i].bType = bType;
	m_aAttr[i].sValue = sValue;
}


void CItem::AddAttributeSwitchBot(BYTE bApply, short sValue)
{

	if (HasAttr(bApply))
		return;

	int i = GetAttributeCount();

	if (i >= MAX_NORM_ATTR_NUM)
		sys_err("item attribute overflow!");
	else
	{
		if (sValue)
			SetAttributeSwtichBot(i, bApply, sValue);
	}
}

#endif

bool CItem::HasAttr(BYTE bApply)
{
	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
		if (m_pProto->aApplies[i].bType == bApply)
			return true;

	for (int i = 0; i < MAX_NORM_ATTR_NUM; ++i)
		if (GetAttributeType(i) == bApply)
			return true;

#ifdef ENABLE_ITEM_EXTRA_PROTO
	if (HasExtraProto())
	{
#ifdef ENABLE_NEW_EXTRA_BONUS
		for (int i = 0; i < NEW_EXTRA_BONUS_COUNT; i++){
			if (m_ExtraProto->ExtraBonus[i].bType == bApply)
				return true;
		}
#endif
	}
#endif

	return false;
}

bool CItem::HasRareAttr(BYTE bApply)
{
	for (int i = 0; i < MAX_RARE_ATTR_NUM; ++i)
		if (GetAttributeType(i + 5) == bApply)
			return true;

	return false;
}

void CItem::AddAttribute(BYTE bApply, short sValue)
{
	if (HasAttr(bApply))
		return;

	int i = GetAttributeCount();

	if (i >= MAX_NORM_ATTR_NUM)
		sys_err("item attribute overflow!");
	else
	{
		if (sValue)
			SetAttribute(i, bApply, sValue);
	}
}

void CItem::AddAttr(BYTE bApply, BYTE bLevel)
{
	if (HasAttr(bApply))
		return;

	if (bLevel <= 0)
		return;

	int i = GetAttributeCount();

	if (i == MAX_NORM_ATTR_NUM)
		sys_err("item attribute overflow!");
	else
	{
		const TItemAttrTable & r = g_map_itemAttr[bApply];
		long lVal = r.lValues[MIN(4, bLevel - 1)];
#ifdef ENABLE_ATTR_COSTUMES
		if (GetType() == ITEM_COSTUME)
			lVal = r.lValues[MIN(9, bLevel + 5 - 1)];
#endif
		
		if (lVal)
			SetAttribute(i, bApply, lVal);
	}
}

void CItem::PutAttributeWithLevel(BYTE bLevel)
{
	int iAttributeSet = GetAttributeSetIndex();
	if (iAttributeSet < 0)
		return;

	if (bLevel > ITEM_ATTRIBUTE_MAX_LEVEL)
		return;

	std::vector<int> avail;

	int total = 0;

	// 붙일 수 있는 속성 배열을 구축
	for (int i = 0; i < MAX_APPLY_NUM; ++i)
	{
		const TItemAttrTable & r = g_map_itemAttr[i];

		if (r.bMaxLevelBySet[iAttributeSet] && !HasAttr(i))
		{
			avail.push_back(i);
			total += r.dwProb;
		}
	}

	if (avail.empty())
	{
		return;
	}

	// 구축된 배열로 확률 계산을 통해 붙일 속성 선정
	unsigned int prob = number(1, total);
	int attr_idx = APPLY_NONE;

	for (DWORD i = 0; i < avail.size(); ++i)
	{
		const TItemAttrTable & r = g_map_itemAttr[avail[i]];

		if (prob <= r.dwProb)
		{
			attr_idx = avail[i];
			break;
		}

		prob -= r.dwProb;
	}

	if (!attr_idx)
	{
		sys_err("Cannot put item attribute %d %d", iAttributeSet, bLevel);
		return;
	}

	const TItemAttrTable & r = g_map_itemAttr[attr_idx];

	// 종류별 속성 레벨 최대값 제한
	if (bLevel > r.bMaxLevelBySet[iAttributeSet])
		bLevel = r.bMaxLevelBySet[iAttributeSet];

	AddAttr(attr_idx, bLevel);
}

void CItem::PutAttribute(const int * aiAttrPercentTable)
{
	int iAttrLevelPercent = number(1, 100);
	int i;

	for (i = 0; i < ITEM_ATTRIBUTE_MAX_LEVEL; ++i)
	{
		if (iAttrLevelPercent <= aiAttrPercentTable[i])
			break;

		iAttrLevelPercent -= aiAttrPercentTable[i];
	}

	PutAttributeWithLevel(i + 1);
}

void CItem::ChangeAttribute(const int* aiChangeProb)
{
	int iAttributeCount = GetAttributeCount();

	ClearAttribute();

	if (iAttributeCount == 0)
		return;

	TItemTable const * pProto = GetProto();

	if (pProto && pProto->sAddonType)
	{
		ApplyAddon(pProto->sAddonType);
	}

	static const int tmpChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		0, 10, 40, 35, 15,
	};

	for (int i = GetAttributeCount(); i < iAttributeCount; ++i)
	{
#ifdef ATTR_LOCK		
		if (GetLockedAttr() == i)
		{
			continue;
		}
#endif
		if (aiChangeProb == NULL)
		{
			PutAttribute(tmpChangeProb);
		}
		else
		{
			PutAttribute(aiChangeProb);
		}
	}
}

void CItem::AddAttribute()
{
	static const int aiItemAddAttributePercent[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		40, 50, 10, 0, 0
	};

	if (GetAttributeCount() < MAX_NORM_ATTR_NUM)
		PutAttribute(aiItemAddAttributePercent);
}

void CItem::ClearAttribute()
{
	for (int i = 0; i < MAX_NORM_ATTR_NUM; ++i)
	{
#ifdef ATTR_LOCK		
		if (GetLockedAttr() == i)
		{
			continue;
		}
#endif
		m_aAttr[i].bType = 0;
		m_aAttr[i].sValue = 0;
	}
}

int CItem::GetAttributeCount()
{
	int i;

	for (i = 0; i < MAX_NORM_ATTR_NUM; ++i)
	{
		if (GetAttributeType(i) == 0)
			break;
	}

	return i;
}

int CItem::FindAttribute(BYTE bType)
{
	for (int i = 0; i < MAX_NORM_ATTR_NUM; ++i)
	{
		if (GetAttributeType(i) == bType)
			return i;
	}

	return -1;
}

bool CItem::RemoveAttributeAt(int index)
{
	if (GetAttributeCount() <= index)
		return false;

	for (int i = index; i < MAX_NORM_ATTR_NUM - 1; ++i)
	{
		SetAttribute(i, GetAttributeType(i + 1), GetAttributeValue(i + 1));
	}

	SetAttribute(MAX_NORM_ATTR_NUM - 1, APPLY_NONE, 0);
	return true;
}

bool CItem::RemoveAttributeType(BYTE bType)
{
	int index = FindAttribute(bType);
	return index != -1 && RemoveAttributeType(index);
}

void CItem::SetAttributes(const TPlayerItemAttribute* c_pAttribute)
{
	memcpy(m_aAttr, c_pAttribute, sizeof(m_aAttr));
	Save();
}

void CItem::SetAttribute(int i, BYTE bType, short sValue)
{
	assert(i < MAX_NORM_ATTR_NUM);

	m_aAttr[i].bType = bType;
	m_aAttr[i].sValue = sValue;
	UpdatePacket();
	Save();

	if (bType)
	{
		const char * pszIP = NULL;

		if (GetOwner() && GetOwner()->GetDesc())
			pszIP = GetOwner()->GetDesc()->GetHostName();

		LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(i, bType, sValue, GetID(), "SET_ATTR", "", pszIP ? pszIP : "", GetOriginalVnum()));
	}
}

void CItem::SetForceAttribute(int i, BYTE bType, short sValue)
{
	assert(i < ITEM_ATTRIBUTE_MAX_NUM);

	m_aAttr[i].bType = bType;
	m_aAttr[i].sValue = sValue;
	UpdatePacket();
	Save();

	if (bType)
	{
		const char * pszIP = NULL;

		if (GetOwner() && GetOwner()->GetDesc())
			pszIP = GetOwner()->GetDesc()->GetHostName();

		LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(i, bType, sValue, GetID(), "SET_FORCE_ATTR", "", pszIP ? pszIP : "", GetOriginalVnum()));
	}
}


void CItem::CopyAttributeTo(LPITEM pItem)
{
	pItem->SetAttributes(m_aAttr);
}

int CItem::GetRareAttrCount()
{
	int ret = 0;

	for (DWORD dwIdx = ITEM_ATTRIBUTE_RARE_START; dwIdx < ITEM_ATTRIBUTE_RARE_END; dwIdx++)
	{
		if (m_aAttr[dwIdx].bType != 0)
			ret++;
	}

	return ret;
}

bool CItem::ChangeRareAttribute()
{
	if (GetRareAttrCount() == 0)
		return false;

	int cnt = GetRareAttrCount();

	for (int i = 0; i < cnt; ++i)
	{
		m_aAttr[i + ITEM_ATTRIBUTE_RARE_START].bType = 0;
		m_aAttr[i + ITEM_ATTRIBUTE_RARE_START].sValue = 0;
	}

	if (GetOwner() && GetOwner()->GetDesc())
		LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(GetOwner(), this, "SET_RARE_CHANGE", ""))
	else
		LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(0, 0, 0, GetID(), "SET_RARE_CHANGE", "", "", GetOriginalVnum()))

	for (int i = 0; i < cnt; ++i)
	{
		AddRareAttribute();
	}

	return true;
}

bool CItem::AddRareAttribute()
{
	int count = GetRareAttrCount();

	if (count >= ITEM_ATTRIBUTE_RARE_NUM)
		return false;

	int pos = count + ITEM_ATTRIBUTE_RARE_START;
	TPlayerItemAttribute & attr = m_aAttr[pos];

	int nAttrSet = GetAttributeSetIndex();
	std::vector<int> avail;

	for (int i = 0; i < MAX_APPLY_NUM; ++i)
	{
		const TItemAttrTable & r = g_map_itemRare[i];

		if (r.dwApplyIndex != 0 && r.bMaxLevelBySet[nAttrSet] > 0 && HasRareAttr(i) != true)
		{
			avail.push_back(i);
		}
	}

	if (avail.empty())
	{
		sys_err("Couldn't add a rare bonus - item_attr_rare has incorrect values!");
		return false;
	}

	const TItemAttrTable& r = g_map_itemRare[avail[number(0, avail.size() - 1)]];
	int nAttrLevel = 5;

	if (nAttrLevel > r.bMaxLevelBySet[nAttrSet])
		nAttrLevel = r.bMaxLevelBySet[nAttrSet];

	attr.bType = r.dwApplyIndex;
	attr.sValue = r.lValues[nAttrLevel - 1];

	UpdatePacket();

	Save();

	const char * pszIP = NULL;

	if (GetOwner() && GetOwner()->GetDesc())
		pszIP = GetOwner()->GetDesc()->GetHostName();

	LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().ItemLog(pos, attr.bType, attr.sValue, GetID(), "SET_RARE", "", pszIP ? pszIP : "", GetOriginalVnum()));
	return true;
}

void CItem::AddRareAttribute2(const int * aiAttrPercentTable)
{
	static const int aiItemAddAttributePercent[ITEM_ATTRIBUTE_MAX_LEVEL] =
	{
		40, 50, 10, 0, 0
	};
	if (aiAttrPercentTable == NULL)
		aiAttrPercentTable = aiItemAddAttributePercent;

	if (GetRareAttrCount() < MAX_RARE_ATTR_NUM)
		PutRareAttribute(aiAttrPercentTable);
}

void CItem::PutRareAttribute(const int * aiAttrPercentTable)
{
	int iAttrLevelPercent = number(1, 100);
	int i;

	for (i = 0; i < ITEM_ATTRIBUTE_MAX_LEVEL; ++i)
	{
		if (iAttrLevelPercent <= aiAttrPercentTable[i])
			break;

		iAttrLevelPercent -= aiAttrPercentTable[i];
	}

	PutRareAttributeWithLevel(i + 1);
}

void CItem::PutRareAttributeWithLevel(BYTE bLevel)
{
	int iAttributeSet = GetAttributeSetIndex();
	if (iAttributeSet < 0)
		return;

	if (bLevel > ITEM_ATTRIBUTE_MAX_LEVEL)
		return;

	std::vector<int> avail;

	int total = 0;

	// 붙일 수 있는 속성 배열을 구축
	for (int i = 0; i < MAX_APPLY_NUM; ++i)
	{
		const TItemAttrTable & r = g_map_itemRare[i];

		if (r.bMaxLevelBySet[iAttributeSet] && !HasRareAttr(i))
		{
			avail.push_back(i);
			total += r.dwProb;
		}
	}

	if (avail.empty())
	{
		return;
	}

	// 구축된 배열로 확률 계산을 통해 붙일 속성 선정
	unsigned int prob = number(1, total);
	int attr_idx = APPLY_NONE;

	for (DWORD i = 0; i < avail.size(); ++i)
	{
		const TItemAttrTable & r = g_map_itemRare[avail[i]];

		if (prob <= r.dwProb)
		{
			attr_idx = avail[i];
			break;
		}

		prob -= r.dwProb;
	}

	if (!attr_idx)
	{
		sys_err("Cannot put item rare attribute %d %d", iAttributeSet, bLevel);
		return;
	}

	const TItemAttrTable & r = g_map_itemRare[attr_idx];

	// 종류별 속성 레벨 최대값 제한
	if (bLevel > r.bMaxLevelBySet[iAttributeSet])
		bLevel = r.bMaxLevelBySet[iAttributeSet];

	AddRareAttr(attr_idx, bLevel);
}

void CItem::AddRareAttr(BYTE bApply, BYTE bLevel)
{
	if (HasRareAttr(bApply))
		return;

	if (bLevel <= 0)
		return;

	int i = ITEM_ATTRIBUTE_RARE_START + GetRareAttrCount();

	if (i == ITEM_ATTRIBUTE_RARE_END)
		sys_err("item rare attribute overflow!");
	else
	{
		const TItemAttrTable & r = g_map_itemRare[bApply];
		long lVal = r.lValues[MIN(4, bLevel - 1)];

		if (lVal)
			SetForceAttribute(i, bApply, lVal);
	}
}

