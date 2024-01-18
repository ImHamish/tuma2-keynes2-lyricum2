#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "desc.h"
#include "sectree_manager.h"
#include "packet.h"
#include "protocol.h"
#include "log.h"
#include "skill.h"
#include "unique_item.h"
#include "profiler.h"
#include "marriage.h"
#include "item_addon.h"
#include "locale_service.h"
#include "item.h"
#include "item_manager.h"
#include "affect.h"
#include "DragonSoul.h"
#include "buff_on_attributes.h"
#include "belt_inventory_helper.h"
#include "../common/VnumHelper.h"
#include "../common/CommonDefines.h"

CItem::CItem(DWORD dwVnum)
	: m_pProto(nullptr), m_dwVnum(dwVnum), m_bWindow(0), m_dwID(0), m_bEquipped(false), m_dwVID(0), m_wCell(0), m_dwCount(0), m_lFlag(0), m_dwLastOwnerPID(0),
	m_bExchanging(false), m_pkDestroyEvent(NULL), m_pkExpireEvent(NULL)
#if defined(ENABLE_CHANGELOOK)
	, m_dwTransmutation(0)
#endif
#ifdef ENABLE_SOUL_SYSTEM
	, m_pkSoulItemEvent(NULL)
#endif
	, m_pkUniqueExpireEvent(NULL), m_pkTimerBasedOnWearExpireEvent(NULL), m_pkRealTimeExpireEvent(NULL),
   	m_pkAccessorySocketExpireEvent(NULL), m_pkOwnershipEvent(NULL), m_dwOwnershipPID(0), m_bSkipSave(false), m_isLocked(false),
	m_dwMaskVnum(0), m_dwSIGVnum (0)
{
	memset( &m_alSockets, 0, sizeof(m_alSockets) );
	memset( &m_aAttr, 0, sizeof(m_aAttr) );
#ifdef USE_MULTIPLE_OPENING
    m_bUpdateStatus = false;
#endif
}

CItem::~CItem()
{
	Destroy();
}

void CItem::Initialize()
{
	CEntity::Initialize(ENTITY_ITEM);

	m_bWindow = RESERVED_WINDOW;
	m_pOwner = NULL;
	m_dwID = 0;
	m_bEquipped = false;
	m_dwVID = m_wCell = m_dwCount = m_lFlag = 0;
	m_pProto = NULL;
	m_bExchanging = false;
#if defined(ENABLE_CHANGELOOK)
	m_dwTransmutation = 0;
#endif
#ifdef ENABLE_SOUL_SYSTEM
	m_pkSoulItemEvent = NULL;
#endif
	m_pkUniqueExpireEvent = NULL;
	memset(&m_alSockets, 0, sizeof(m_alSockets));
	memset(&m_aAttr, 0, sizeof(m_aAttr));
#ifdef ATTR_LOCK
	m_sLockedAttr = -1;
#endif

	m_pkDestroyEvent = NULL;
	m_pkOwnershipEvent = NULL;
	m_dwOwnershipPID = 0;

	m_pkTimerBasedOnWearExpireEvent = NULL;
	m_pkRealTimeExpireEvent = NULL;

	m_pkAccessorySocketExpireEvent = NULL;

	m_bSkipSave = false;
	m_dwLastOwnerPID = 0;
#ifdef USE_MULTIPLE_OPENING
    m_bUpdateStatus = false;
#endif
}

void CItem::Destroy()
{
	event_cancel(&m_pkDestroyEvent);
	event_cancel(&m_pkOwnershipEvent);
	event_cancel(&m_pkUniqueExpireEvent);

#ifdef ENABLE_SOUL_SYSTEM
	event_cancel(&m_pkSoulItemEvent);
#endif

	event_cancel(&m_pkTimerBasedOnWearExpireEvent);
	event_cancel(&m_pkRealTimeExpireEvent);
	event_cancel(&m_pkAccessorySocketExpireEvent);

	CEntity::Destroy();

	if (GetSectree())
		GetSectree()->RemoveEntity(this);
}

EVENTFUNC(item_destroy_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "item_destroy_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;

	if (pkItem->GetOwner())
		sys_err("item_destroy_event: Owner exist. (item %s owner %s)", pkItem->GetName(), pkItem->GetOwner()->GetName());

	pkItem->SetDestroyEvent(NULL);
	M2_DESTROY_ITEM(pkItem);
	return 0;
}

void CItem::SetDestroyEvent(LPEVENT pkEvent)
{
	m_pkDestroyEvent = pkEvent;
}

void CItem::StartDestroyEvent(int iSec)
{
	if (m_pkDestroyEvent)
		return;

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetDestroyEvent(event_create(item_destroy_event, info, PASSES_PER_SEC(iSec)));
}

void CItem::EncodeInsertPacket(LPENTITY ent)
{
	LPDESC d;

	if (!(d = ent->GetDesc()))
		return;

	const PIXEL_POSITION & c_pos = GetXYZ();

	struct packet_item_ground_add pack;

	pack.bHeader	= HEADER_GC_ITEM_GROUND_ADD;
	pack.x		= c_pos.x;
	pack.y		= c_pos.y;
	pack.z		= c_pos.z;
	pack.dwVnum		= GetVnum();
	pack.dwVID		= m_dwVID;
	//pack.count	= m_dwCount;

	d->Packet(&pack, sizeof(pack));

	if (m_pkOwnershipEvent != NULL)
	{
		item_event_info * info = dynamic_cast<item_event_info *>(m_pkOwnershipEvent->info);

		if ( info == NULL )
		{
			sys_err( "CItem::EncodeInsertPacket> <Factor> Null pointer" );
			return;
		}

		TPacketGCItemOwnership p;

		p.bHeader = HEADER_GC_ITEM_OWNERSHIP;
		p.dwVID = m_dwVID;
		strlcpy(p.szName, info->szOwnerName, sizeof(p.szName));

		d->Packet(&p, sizeof(TPacketGCItemOwnership));
	}
}

void CItem::EncodeRemovePacket(LPENTITY ent)
{
	LPDESC d;

	if (!(d = ent->GetDesc()))
		return;

	struct packet_item_ground_del pack;

	pack.bHeader	= HEADER_GC_ITEM_GROUND_DEL;
	pack.dwVID		= m_dwVID;

	d->Packet(&pack, sizeof(pack));
	sys_log(2, "Item::EncodeRemovePacket %s to %s", GetName(), ((LPCHARACTER) ent)->GetName());
}

void CItem::SetProto(const TItemTable * table)
{
	assert(table != NULL);
	m_pProto = table;
	SetFlag(m_pProto->dwFlags);
}

#ifdef ENABLE_ITEM_EXTRA_PROTO
void CItem::SetExtraProto(TItemExtraProto* Proto) 
{
	m_ExtraProto = Proto;
}

TItemExtraProto* CItem::GetExtraProto() 
{
	return m_ExtraProto;
}
#endif

void CItem::UsePacketEncode(LPCHARACTER ch, LPCHARACTER victim, struct packet_item_use *packet)
{
	if (!GetVnum())
		return;

	packet->header 	= HEADER_GC_ITEM_USE;
	packet->ch_vid 	= ch->GetVID();
	packet->victim_vid 	= victim->GetVID();
	packet->Cell = TItemPos(GetWindow(), m_wCell);
	packet->vnum	= GetVnum();
}

void CItem::RemoveFlag(long bit)
{
	REMOVE_BIT(m_lFlag, bit);
}

void CItem::AddFlag(long bit)
{
	SET_BIT(m_lFlag, bit);
}

void CItem::UpdatePacket()
{
#ifdef USE_MULTIPLE_OPENING
    if (GetUpdateStatus())
    {
        return;
    }
#endif

	if (!m_pOwner || !m_pOwner->GetDesc())
		return;

	TPacketGCItemUpdate pack;

	pack.header = HEADER_GC_ITEM_UPDATE;
	pack.Cell = TItemPos(GetWindow(), m_wCell);
	pack.count = m_dwCount;
#if defined(ENABLE_CHANGELOOK)
	pack.transmutation = m_dwTransmutation;
#endif
#ifdef ATTR_LOCK
	pack.lockedattr = m_sLockedAttr;
#endif

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		pack.alSockets[i] = m_alSockets[i];

	memcpy(pack.aAttr, GetAttributes(), sizeof(pack.aAttr));

	sys_log(2, "UpdatePacket %s -> %s", GetName(), m_pOwner->GetName());
	m_pOwner->GetDesc()->Packet(&pack, sizeof(pack));
}

WORD CItem::GetCount()
{
	if (GetType() == ITEM_ELK) return MIN(m_dwCount, INT_MAX);
	else
	{
		return MIN(m_dwCount, g_bItemCountLimit);
	}
}

#ifdef ATTR_LOCK

bool CItem::CheckHumanApply()
{
	bool bHaveHuman = false;
	TItemTable * p = ITEM_MANAGER::instance().GetTable(GetVnum());
	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
		if (p->aApplies[i].bType == APPLY_ATTBONUS_HUMAN)
			bHaveHuman = true;
			
	return bHaveHuman;
}

void CItem::AddLockedAttr()
{
	int iCount = GetAttributeCount();
	int iRand = rand() % (iCount);
	bool bCheckHuman = CheckHumanApply();

	if (bCheckHuman)
	{
		while(GetAttributeType(iRand) == APPLY_ATTBONUS_HUMAN || GetAttributeType(iRand) == APPLY_SKILL_DAMAGE_BONUS || GetAttributeType(iRand) == APPLY_NORMAL_HIT_DAMAGE_BONUS)
			iRand = rand() % (iCount);
	}
	else
	{
		while(GetAttributeType(iRand) == APPLY_SKILL_DAMAGE_BONUS || GetAttributeType(iRand) == APPLY_NORMAL_HIT_DAMAGE_BONUS)
			iRand = rand() % (iCount);
	}
	
	SetLockedAttr((short)iRand);
}
void CItem::ChangeLockedAttr()
{
	int iCount = GetAttributeCount();
	int iRand = rand() % (iCount);
	bool bCheckHuman = CheckHumanApply();

	if (bCheckHuman)
	{
		while (iRand == (int)GetLockedAttr() || GetAttributeType(iRand) == APPLY_ATTBONUS_HUMAN || GetAttributeType(iRand) == APPLY_SKILL_DAMAGE_BONUS || GetAttributeType(iRand) == APPLY_NORMAL_HIT_DAMAGE_BONUS)
			iRand = rand() % (iCount);
	}
	else
	{
		while(iRand == (int)GetLockedAttr() || GetAttributeType(iRand) == APPLY_SKILL_DAMAGE_BONUS || GetAttributeType(iRand) == APPLY_NORMAL_HIT_DAMAGE_BONUS)
			iRand = rand() % (iCount);
	}
	
	SetLockedAttr((short)iRand);
}
void CItem::RemoveLockedAttr()
{
	SetLockedAttr(-1);	
}
void CItem::SetLockedAttr(short sIndex)
{
	m_sLockedAttr = sIndex;
	UpdatePacket();
	Save();
}

#endif

bool CItem::SetCount(WORD count)
{
	if (GetType() == ITEM_ELK)
	{
		m_dwCount = MIN(count, INT_MAX);
	}
	else
	{
		m_dwCount = MIN(count, g_bItemCountLimit);
	}

	if (count == 0 && m_pOwner)
	{
		if (GetSubType() == USE_ABILITY_UP || GetSubType() == USE_POTION || GetVnum() == 70020)
		{
			LPCHARACTER pOwner = GetOwner();
			WORD wCell = GetCell();

			RemoveFromCharacter();

			if (!IsDragonSoul()
			)
			{
				LPITEM pItem = pOwner->FindSpecifyItem(GetVnum());

				if (NULL != pItem)
				{
#ifdef ENABLE_EXTRA_INVENTORY
					if (IsExtraItem()) {
						pOwner->ChainQuickslotItem(pItem, QUICKSLOT_TYPE_ITEM_EXTRA, wCell);
					} else {
						pOwner->ChainQuickslotItem(pItem, QUICKSLOT_TYPE_ITEM, wCell);
					}
#else
					pOwner->ChainQuickslotItem(pItem, QUICKSLOT_TYPE_ITEM, wCell);
#endif
				}
				else
				{
#ifdef ENABLE_EXTRA_INVENTORY
					if (IsExtraItem()) {
						pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM_EXTRA, wCell, 65535);
					} else {
						pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, wCell, 65535);
					}
#else
					pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, wCell, 65535);
#endif
				}
			}

			M2_DESTROY_ITEM(this);
		}
		else
		{
			if (!IsDragonSoul())
			{
#ifdef ENABLE_EXTRA_INVENTORY
				if (IsExtraItem()) {
					m_pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM_EXTRA, m_wCell, 65535);
				} else {
					m_pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, m_wCell, 65535);
				}
#else
				m_pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, m_wCell, 65535);
#endif
			}

			M2_DESTROY_ITEM(RemoveFromCharacter());
		}

		return false;
	}

	UpdatePacket();

	Save();
	return true;
}

#if defined(ENABLE_CHANGELOOK)
void CItem::SetTransmutation(uint32_t vnum) {
	m_dwTransmutation = vnum;
	UpdatePacket();
	Save();
}
#endif

LPITEM CItem::RemoveFromCharacter()
{
	if (!m_pOwner) {
		sys_err("Item::RemoveFromCharacter owner null (vnum: %u)", GetVnum());
		return (this);
	}

	LPCHARACTER pOwner = m_pOwner;

	if (m_bEquipped)	// 장착되었는가?
	{
		Unequip();
		//pOwner->UpdatePacket();
		SetWindow(RESERVED_WINDOW);
		Save();
		return (this);
	}
	else
	{
		if (GetWindow() != SAFEBOX && GetWindow() != MALL
#if defined(USE_ATTR_6TH_7TH)
			&& GetWindow() != ATTR67_ADD
#endif
		)
		{
			if (IsDragonSoul())
			{
				if (m_wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
					sys_err("CItem::RemoveFromCharacter: pos >= DRAGON_SOUL_INVENTORY_MAX_NUM");
				else
					pOwner->SetItem(TItemPos(m_bWindow, m_wCell), NULL);
			}
#ifdef ENABLE_EXTRA_INVENTORY
			else if (IsExtraItem())
			{
				if (m_wCell >= EXTRA_INVENTORY_MAX_NUM)
					sys_err("CItem::RemoveFromCharacter: pos >= EXTRA_INVENTORY_MAX_NUM");
				else
					pOwner->SetItem(TItemPos(m_bWindow, m_wCell), NULL);
			}
#endif
#ifdef ENABLE_SWITCHBOT_WORLDARD
			else if (GetWindow() == SWITCHBOT)
			{
				if (m_wCell >= SWITCHBOT_SLOT_MAX)
					sys_err("CItem::RemoveFromCharacter: pos >= SWITCHBOT_SLOT_MAX");
				else
					pOwner->SetItem(TItemPos(m_bWindow, m_wCell), NULL);
			}
#endif
			else
			{
				TItemPos cell(INVENTORY, m_wCell);

				if (false == cell.IsDefaultInventoryPosition() && false == cell.IsBeltInventoryPosition()) {
					sys_err("CItem::RemoveFromCharacter: Invalid Item Position (vnum: %u, cell: %d)", GetVnum(), m_wCell);
				} else {
					pOwner->SetItem(cell, NULL);
				}
			}
		}

#if defined(USE_ATTR_6TH_7TH)
		if (GetWindow() == ATTR67_ADD)
		{
			pOwner->SetItem(TItemPos(ATTR67_ADD, 0), NULL);
		}
#endif

		m_pOwner = NULL;
		m_wCell = 0;

		SetWindow(RESERVED_WINDOW);
		Save();
		return (this);
	}
}

#ifdef __HIGHLIGHT_SYSTEM__
bool CItem::AddToCharacter(LPCHARACTER ch, TItemPos Cell, bool isHighLight)
#else
bool CItem::AddToCharacter(LPCHARACTER ch, TItemPos Cell)
#endif
{
	assert(GetSectree() == NULL);
	assert(m_pOwner == NULL);
	WORD pos = Cell.cell;
	BYTE window_type = Cell.window_type;

	if (INVENTORY == window_type)
	{
		if (m_wCell >= INVENTORY_MAX_NUM && BELT_INVENTORY_SLOT_START > m_wCell)
		{
			sys_err("CItem::AddToCharacter: cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
			return false;
		}
	}

	else if (DRAGON_SOUL_INVENTORY == window_type)
	{
		if (m_wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
		{
			sys_err("CItem::AddToCharacter: cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
			return false;
		}
	}
#ifdef ENABLE_EXTRA_INVENTORY
	else if (window_type == EXTRA_INVENTORY)
	{
		if (m_wCell >= EXTRA_INVENTORY_MAX_NUM)
		{
			sys_err("CItem::AddToCharacter: EXTRA cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
			return false;
		}
	}
#endif
#ifdef ENABLE_SWITCHBOT_WORLDARD
	else if (SWITCHBOT == window_type)
	{
		if (m_wCell >= SWITCHBOT_SLOT_MAX){
			sys_err("CItem::AddToCharacter: cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
			return false;
		}
	}
#endif
	if (ch->GetDesc())
		m_dwLastOwnerPID = ch->GetPlayerID();


#if defined(ENABLE_ACCE_SYSTEM)
	if (GetType() == ITEM_COSTUME &&
		GetSubType() == COSTUME_ACCE &&
		GetSocket(ACCE_ABSORPTION_SOCKET) == 0
	) {
		long lVal = GetValue(ACCE_GRADE_VALUE_FIELD);
		switch (lVal)
		{
			case 2:
			{
				lVal = ACCE_GRADE_2_ABS;
				break;
			}
			case 3:
			{
				lVal = ACCE_GRADE_3_ABS;
				break;
			}
			case 4:
			{
				lVal = number(ACCE_GRADE_4_ABS_MIN, ACCE_GRADE_4_ABS_MAX_COMB);
				break;
			}
			default:
			{
				lVal = ACCE_GRADE_1_ABS;
				break;
			}
		}

		SetSocket(ACCE_ABSORPTION_SOCKET, lVal);
	}
#endif

	event_cancel(&m_pkDestroyEvent);

#ifdef __HIGHLIGHT_SYSTEM__
	ch->SetItem(TItemPos(window_type, pos), this, isHighLight);
#else
	ch->SetItem(TItemPos(window_type, pos), this);
#endif
	m_pOwner = ch;

	Save();
	return true;
}

bool CItem::CanAddToCharacter(LPCHARACTER ch, const TItemPos& Cell)
{
	WORD pos = Cell.cell;
	BYTE window_type = Cell.window_type;

#ifdef ENABLE_EXTRA_INVENTORY
	if (IsExtraItem())
	{
		if (window_type != EXTRA_INVENTORY)
			return false;

		BYTE category = GetExtraCategory();
		if (pos < category * EXTRA_INVENTORY_CATEGORY_MAX_NUM || pos >= (category + 1) * EXTRA_INVENTORY_CATEGORY_MAX_NUM)
			return false;
	}
#endif

	if (INVENTORY == window_type)
	{
		if (m_wCell >= INVENTORY_MAX_NUM && BELT_INVENTORY_SLOT_START > m_wCell)
			return false;

		if (ch->GetItem(Cell) != nullptr)
			return false;

		return true;
	}

	if (DRAGON_SOUL_INVENTORY == window_type)
	{
		if (m_wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
			return false;

		if (ch->GetItem(Cell) != nullptr)
			return false;

		return true;
	}

#ifdef ENABLE_EXTRA_INVENTORY
	if (window_type == EXTRA_INVENTORY)
	{
		if (m_wCell >= EXTRA_INVENTORY_MAX_NUM)
			return false;

		if (ch->GetItem(Cell) != nullptr)
			return false;

		return true;
	}
#endif

#ifdef ENABLE_SWITCHBOT_WORLDARD
	if (SWITCHBOT == window_type)
	{
		if (m_wCell >= SWITCHBOT_SLOT_MAX) 
			return false;

		if (ch->GetItem(Cell) != nullptr)
			return false;

		return true;
	}
#endif

#if defined(USE_ATTR_6TH_7TH)
	if (ATTR67_ADD == window_type)
	{
		if (m_wCell >= ATTR67_ADD_SLOT_MAX)
			return false;

		if (ch->GetItem(Cell) != nullptr)
			return false;

		return true;
	}
#endif

	return false;
}

LPITEM CItem::RemoveFromGround()
{
	if (GetSectree())
	{
		SetOwnership(NULL);

		GetSectree()->RemoveEntity(this);

		ViewCleanup();

		Save();
	}

	return (this);
}

bool CItem::AddToGround(long lMapIndex, const PIXEL_POSITION & pos, bool skipOwnerCheck)
{
	if (0 == lMapIndex)
	{
		sys_err("wrong map index argument: %d", lMapIndex);
		return false;
	}

	if (GetSectree())
	{
		sys_err("sectree already assigned");
		return false;
	}

	if (!skipOwnerCheck && m_pOwner)
	{
		sys_err("owner pointer not null");
		return false;
	}

	LPSECTREE tree = SECTREE_MANAGER::instance().Get(lMapIndex, pos.x, pos.y);

	if (!tree)
	{
		sys_err("cannot find sectree by %dx%d", pos.x, pos.y);
		return false;
	}

	//tree->Touch();

	SetWindow(GROUND);
	SetXYZ(pos.x, pos.y, pos.z);
	tree->InsertEntity(this);
	UpdateSectree();
	Save();
	return true;
}

#ifdef USE_PICKUP_INCREASED_RANGE
bool CItem::DistanceValid(LPCHARACTER ch, bool bPickup/* = false*/)
#else
bool CItem::DistanceValid(LPCHARACTER ch)
#endif
{
    if (!GetSectree())
    {
        return false;
    }

#ifdef USE_PICKUP_INCREASED_RANGE
    int32_t iLimit = bPickup == true ? 900 : 600;
#else
    int32_t iLimit = 600;
#endif

    int32_t iDist = DISTANCE_APPROX(GetX() - ch->GetX(), GetY() - ch->GetY());
    if (iDist > iLimit)
    {
        return false;
    }

    return true;
}

bool CItem::CanUsedBy(LPCHARACTER ch)
{
	// Anti flag check
	switch (ch->GetJob())
	{
		case JOB_WARRIOR:
			if (GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
				return false;
			break;

		case JOB_ASSASSIN:
			if (GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
				return false;
			break;

		case JOB_SHAMAN:
			if (GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
				return false;
			break;

		case JOB_SURA:
			if (GetAntiFlag() & ITEM_ANTIFLAG_SURA)
				return false;
			break;
	}

	return true;
}

int CItem::FindEquipCell(LPCHARACTER ch, int iCandidateCell)
{
	uint8_t itemType = GetType();

	if ((0 == GetWearFlag() || itemType == ITEM_TOTEM) &&
		itemType != ITEM_COSTUME &&
		itemType != ITEM_DS &&
		itemType != ITEM_SPECIAL_DS &&
		itemType != ITEM_RING &&
		itemType != ITEM_BELT) {
		return -1;
	}

	if (itemType == ITEM_DS ||
		itemType == ITEM_SPECIAL_DS) {
		if (iCandidateCell < 0) {
 			return WEAR_MAX_NUM + GetSubType();
		} else {
			for (uint16_t i = 0; i < DRAGON_SOUL_DECK_MAX_NUM; i++) {
				if (WEAR_MAX_NUM + i * DS_SLOT_MAX + GetSubType() == iCandidateCell) {
					return iCandidateCell;
				}
			}

			return -1;
		}
	} else if (GetType() == ITEM_COSTUME) {
		uint8_t itemSubType = GetSubType();

		if (itemSubType == COSTUME_BODY) {
			return WEAR_COSTUME_BODY;
		} else if (itemSubType == COSTUME_HAIR) {
			return WEAR_COSTUME_HAIR;
		}
#if defined(USE_MOUNT_COSTUME_SYSTEM)
		else if (itemSubType == COSTUME_MOUNT)
		{
			return WEAR_COSTUME_MOUNT;
		}
#endif
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
		else if (itemSubType == COSTUME_WEAPON) {
			return WEAR_COSTUME_WEAPON;
		}
#endif
#ifdef ENABLE_ACCE_SYSTEM
		else if (itemSubType == COSTUME_ACCE) {
			return WEAR_COSTUME_ACCE_SLOT;
		}
#endif
#ifdef ENABLE_STOLE_COSTUME
		else if (itemSubType == COSTUME_STOLE) {
			return WEAR_COSTUME_ACCE;
		}
#endif
#if defined(ENABLE_PETSKIN)
		else if (itemSubType == COSTUME_PETSKIN) {
			return WEAR_PETSKIN;
		}
#endif
#if defined(ENABLE_MOUNTSKIN)
		else if (GetSubType() == COSTUME_MOUNTSKIN) {
			return WEAR_MOUNTSKIN;
		}
#endif
#ifdef ENABLE_COSTUME_EFFECT
		else if (itemSubType == COSTUME_EFFECT_BODY) {
			return WEAR_COSTUME_EFFECT_BODY;
		} else if (itemSubType == COSTUME_EFFECT_WEAPON) {
			return WEAR_COSTUME_EFFECT_WEAPON;
		}
#endif
#if defined(ENABLE_AURA_SYSTEM)
		else if (itemSubType == COSTUME_AURA) {
			return WEAR_COSTUME_AURA;
		}
#endif
	}
#if !defined(USE_MOUNT_COSTUME_SYSTEM) && !defined(ENABLE_ACCE_SYSTEM)
	else if (itemType == ITEM_RING)
	{
		if (ch->GetWear(WEAR_RING1))
			return WEAR_RING2;
		else
			return WEAR_RING1;
	}
#endif
	else if (itemType == ITEM_BELT) {
		return WEAR_BELT;
	}

	uint32_t itemWearFlag = GetWearFlag();

	if (itemWearFlag & WEARABLE_BODY) {
		return WEAR_BODY;
	} else if (itemWearFlag & WEARABLE_HEAD) {
		return WEAR_HEAD;
	} else if (itemWearFlag & WEARABLE_FOOTS) {
		return WEAR_FOOTS;
	} else if (itemWearFlag & WEARABLE_WRIST) {
		return WEAR_WRIST;
	} else if (itemWearFlag & WEARABLE_WEAPON) {
		return WEAR_WEAPON;
	} else if (itemWearFlag & WEARABLE_SHIELD) {
		return WEAR_SHIELD;
	} else if (itemWearFlag & WEARABLE_NECK) {
		return WEAR_NECK;
	} else if (itemWearFlag & WEARABLE_EAR) {
		return WEAR_EAR;
	} else if (itemWearFlag & WEARABLE_ARROW) {
		return WEAR_ARROW;
	} else if (itemWearFlag & WEARABLE_UNIQUE) {
#ifdef ENABLE_NEW_UNIQUE_WEAR_LIMITED
		uint8_t itemSubType = GetSubType();

		if (itemSubType == UNIQUE_PVM) {
			return WEAR_UNIQUE1;
		} else if (itemSubType == UNIQUE_PVP) {
			return WEAR_UNIQUE2;
		}

		return -1;
#else
		if (ch->GetWear(WEAR_UNIQUE1)) {
			return WEAR_UNIQUE2;
		} else {
			return WEAR_UNIQUE1;
		}
#endif
	} else if (itemWearFlag & WEARABLE_ABILITY) {
		if (!ch->GetWear(WEAR_ABILITY1)) {
			return WEAR_ABILITY1;
		} else if (!ch->GetWear(WEAR_ABILITY2)) {
			return WEAR_ABILITY2;
		} else if (!ch->GetWear(WEAR_ABILITY3)) {
			return WEAR_ABILITY3;
		} else if (!ch->GetWear(WEAR_ABILITY4)) {
			return WEAR_ABILITY4;
		} else if (!ch->GetWear(WEAR_ABILITY5)) {
			return WEAR_ABILITY5;
		} else if (!ch->GetWear(WEAR_ABILITY6)) {
			return WEAR_ABILITY6;
		} else if (!ch->GetWear(WEAR_ABILITY7)) {
			return WEAR_ABILITY7;
		}
#if !defined(ENABLE_STOLE_REAL)
		else if (!ch->GetWear(WEAR_ABILITY8)) {
			return WEAR_ABILITY8;
		}
#endif
		else {
			return -1;
		}
	}

	return -1;
}

void CItem::ModifyPoints(bool bAdd)
{
	if (!m_pOwner) {
		return;
	}

	int accessoryGrade;

	// 무기와 갑옷만 소켓을 적용시킨다.
	if (false == IsAccessoryForSocket())
	{
		if (m_pProto->bType == ITEM_WEAPON || m_pProto->bType == ITEM_ARMOR)
		{
			// 소켓이 속성강화에 사용되는 경우 적용하지 않는다 (ARMOR_WRIST ARMOR_NECK ARMOR_EAR)
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			{
				DWORD dwVnum;

				if ((dwVnum = GetSocket(i)) <= 2)
					continue;

				TItemTable * p = ITEM_MANAGER::instance().GetTable(dwVnum);

				if (!p)
				{
					sys_err("cannot find table by vnum %u", dwVnum);
					continue;
				}

				if (ITEM_METIN == p->bType)
				{
					for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
					{
						if (p->aApplies[i].bType == APPLY_NONE)
							continue;
						
						if (p->aApplies[i].bType == APPLY_SKILL)
							m_pOwner->ApplyPoint(p->aApplies[i].bType, bAdd ? p->aApplies[i].lValue : p->aApplies[i].lValue ^ 0x00800000);
						else
							m_pOwner->ApplyPoint(p->aApplies[i].bType, bAdd ? p->aApplies[i].lValue : -p->aApplies[i].lValue);
					}
				}
			}
		}

		accessoryGrade = 0;
	}
	else
	{
		accessoryGrade = MIN(GetAccessorySocketGrade(), ITEM_ACCESSORY_SOCKET_MAX_NUM);
	}

	for (int32_t i = 0; i < ITEM_APPLY_MAX_NUM; ++i) {
		if (m_pProto->aApplies[i].bType == APPLY_NONE) {
			continue;
		}

#if defined(ENABLE_ACCE_SYSTEM)
		if (m_pProto->bType == ITEM_COSTUME && m_pProto->bSubType == COSTUME_ACCE) {
			continue;
		}
#endif

#if defined(ENABLE_AURA_SYSTEM)
		if (m_pProto->bType == ITEM_COSTUME && m_pProto->bSubType == COSTUME_AURA) {
			continue;
		}
#endif

		long value = m_pProto->aApplies[i].lValue;

		if (m_pProto->aApplies[i].bType == APPLY_SKILL) {
			m_pOwner->ApplyPoint(m_pProto->aApplies[i].bType, bAdd ? value : value ^ 0x00800000);
		} else {
			if (0 != accessoryGrade) {
				value += MAX(accessoryGrade, value * aiAccessorySocketEffectivePct[accessoryGrade] / 100);
			}

			m_pOwner->ApplyPoint(m_pProto->aApplies[i].bType, bAdd ? value : -value);
		}
	}

#if defined(ENABLE_ACCE_SYSTEM)
	long lAcceDrainPer = 0;
	if (m_pProto->bType == ITEM_COSTUME && m_pProto->bSubType == COSTUME_ACCE) {
		lAcceDrainPer = GetSocket(ACCE_ABSORPTION_SOCKET);
		const long c_lDrainSocket = GetSocket(ACCE_ABSORBED_SOCKET);

		TItemTable* p = NULL;
		if (c_lDrainSocket != 0) {
			p = ITEM_MANAGER::instance().GetTable(c_lDrainSocket);
		}

		if (p != NULL) {
			if (p->bType == ITEM_WEAPON) {
				switch (p->bSubType) {
					case WEAPON_SWORD:
					case WEAPON_DAGGER:
					case WEAPON_BOW:
					case WEAPON_TWO_HANDED:
					case WEAPON_BELL:
					case WEAPON_FAN:
						{
							long lAttGrade = 0;
							if (p->alValues[3] > 0 || p->alValues[4] > 0) {
								lAttGrade = p->alValues[3] > p->alValues[4] ? p->alValues[3] + p->alValues[5] : p->alValues[4] + p->alValues[5];
							}

							float fValue = (lAttGrade / 100.0f) * lAcceDrainPer;
							int32_t iValue = static_cast<int32_t>(roundf(fValue));
							if (iValue > 0) {
								m_pOwner->ApplyPoint(APPLY_ATT_GRADE_BONUS, bAdd ? iValue : -iValue);
							}

							long lAttMagicGrade = 0;
							if (p->alValues[1] > 0 || p->alValues[2] > 0) {
								lAttMagicGrade = p->alValues[1] > p->alValues[2] ? p->alValues[1] + p->alValues[5] : p->alValues[2] + p->alValues[5];
							}

							fValue = (lAttMagicGrade / 100.0f) * lAcceDrainPer;
							iValue = static_cast<int32_t>(roundf(fValue));
							if (iValue > 0) {
								m_pOwner->ApplyPoint(APPLY_MAGIC_ATT_GRADE, bAdd ? iValue : -iValue);
							}

							break;
						}
					default: {
						break;
					}
				}
			}

			for (int32_t i = 0; i < ITEM_APPLY_MAX_NUM; ++i) {
				if (p->aApplies[i].lValue <= 0 || 
					p->aApplies[i].bType == APPLY_NONE || 
					p->aApplies[i].bType == APPLY_MOUNT
#if defined(ENABLE_PETSKIN)
 || p->aApplies[i].bType == APPLY_PETSKIN
#endif
#if defined(ENABLE_MOUNTSKIN)
 || p->aApplies[i].bType == APPLY_MOUNTSKIN
#endif
				) {
					continue;
				}

				float fValue = (p->aApplies[i].lValue / 100.0f) * lAcceDrainPer;
				int32_t iValue = static_cast<int32_t>(roundf(fValue));
				if (iValue <= 0) {
					continue;
				}

				if (p->aApplies[i].bType == APPLY_SKILL) {
					m_pOwner->ApplyPoint(p->aApplies[i].bType, bAdd ? iValue : iValue ^ 0x00800000);
				} else {
					m_pOwner->ApplyPoint(p->aApplies[i].bType, bAdd ? iValue : -iValue);
				}
			}
		}
	}
#endif

#if defined(ENABLE_AURA_SYSTEM)
	float fAuraDrainPer = 0.0f;
	if (m_pProto->bType == ITEM_COSTUME && m_pProto->bSubType == COSTUME_AURA) {
		const long c_lLevelSocket = GetSocket(ITEM_SOCKET_AURA_CURRENT_LEVEL);
		const long c_lDrainSocket = GetSocket(ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM);

		BYTE bCurLevel = (c_lLevelSocket / 100000) - 1000;

		fAuraDrainPer = 1.0f * bCurLevel / 10.0f;

		TItemTable* p = NULL;
		if (c_lDrainSocket != 0) {
			p = ITEM_MANAGER::instance().GetTable(c_lDrainSocket);
		}

		if (p != NULL && (ITEM_ARMOR == p->bType && (ARMOR_SHIELD == p->bSubType || 
													ARMOR_WRIST == p->bSubType || 
													ARMOR_NECK == p->bSubType || 
													ARMOR_EAR == p->bSubType))
		) {
			for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i) {
				if (p->aApplies[i].lValue <= 0 || 
					p->aApplies[i].bType == APPLY_NONE || 
					p->aApplies[i].bType == APPLY_MOUNT
#if defined(ENABLE_PETSKIN)
 || p->aApplies[i].bType == APPLY_PETSKIN
#endif
#if defined(ENABLE_MOUNTSKIN)
 || p->aApplies[i].bType == APPLY_MOUNTSKIN
#endif
				) {
					continue;
				}

				float fValue = (p->aApplies[i].lValue / 100.0f) * fAuraDrainPer;
				int32_t iValue = static_cast<int32_t>(roundf(fValue));
				if (iValue <= 0) {
					continue;
				}

				if (p->aApplies[i].bType == APPLY_SKILL) {
					m_pOwner->ApplyPoint(p->aApplies[i].bType, bAdd ? iValue : iValue ^ 0x00800000);
				} else {
					m_pOwner->ApplyPoint(p->aApplies[i].bType, bAdd ? iValue : -iValue);
				}
			}
		}
	}
#endif

#if defined(ENABLE_ITEM_EXTRA_PROTO) && defined(ENABLE_NEW_EXTRA_BONUS)
	if (HasExtraProto()) {
		for (auto i = 0; i < NEW_EXTRA_BONUS_COUNT; i++) {
			auto type = m_ExtraProto->ExtraBonus[i].bType;
			if (type != APPLY_NONE) {
				auto value = m_ExtraProto->ExtraBonus[i].lValue;
				m_pOwner->ApplyPoint(m_ExtraProto->ExtraBonus[i].bType, bAdd ? value : -value);
			}
		}
	}
#endif

	if (true == CItemVnumHelper::IsRamadanMoonRing(GetVnum()) || 
		true == CItemVnumHelper::IsHalloweenCandy(GetVnum()) || 
		true == CItemVnumHelper::IsHappinessRing(GetVnum()) || 
		true == CItemVnumHelper::IsLovePendant(GetVnum())
	) {
	}
	else {
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			if (GetAttributeType(i))
			{
#ifdef ATTR_LOCK
				if (GetLockedAttr() == i) {
					continue;
				}
#endif

				const TPlayerItemAttribute& ia = GetAttribute(i);
				long sValue = ia.sValue;

#if defined(ENABLE_ACCE_SYSTEM)
				if (m_pProto->bType == ITEM_COSTUME && m_pProto->bSubType == COSTUME_ACCE) {
					if (ia.sValue <= 0) {
						continue;
					}

					float fValue = (sValue / 100.0f) * lAcceDrainPer;
					int iValue = static_cast<int>(roundf(fValue));
					if (iValue <= 0) {
						continue;
					}

					if (ia.bType == APPLY_SKILL) {
						m_pOwner->ApplyPoint(ia.bType, bAdd ? iValue : iValue ^ 0x00800000);
					} else {
						m_pOwner->ApplyPoint(ia.bType, bAdd ? iValue : -iValue);
					}

					continue;
				}
#endif

#if defined(ENABLE_AURA_SYSTEM)
				if (m_pProto->bType == ITEM_COSTUME && m_pProto->bSubType == COSTUME_AURA) {
					if (ia.sValue <= 0) {
						continue;
					}

					float fValue = (sValue / 100.0f) * fAuraDrainPer;
					int32_t iValue = static_cast<int32_t>(roundf(fValue));
					if (iValue <= 0) {
						continue;
					}

					if (ia.bType == APPLY_SKILL) {
						m_pOwner->ApplyPoint(ia.bType, bAdd ? iValue : iValue ^ 0x00800000);
					} else {
						m_pOwner->ApplyPoint(ia.bType, bAdd ? iValue : -iValue);
					}

					continue;
				}
#endif

				if (ia.bType == APPLY_SKILL)
					m_pOwner->ApplyPoint(ia.bType, bAdd ? sValue : sValue ^ 0x00800000);
				else
					m_pOwner->ApplyPoint(ia.bType, bAdd ? sValue : -sValue);
			}
		}
	}

	switch (m_pProto->bType)
	{
		case ITEM_PICK:
		case ITEM_ROD:
			{
				if (bAdd)
				{
					if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON) {
						m_pOwner->SetPart(PART_WEAPON, GetVnum());
					}
				} else {
					if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON) {
						m_pOwner->SetPart(PART_WEAPON, 0);
					}
				}
			}
			break;
		case ITEM_WEAPON:
			{
#ifdef ENABLE_COSTUME_EFFECT
				if (IsMainWeapon()) {
					CItem* item = m_pOwner->GetWear(WEAR_COSTUME_EFFECT_WEAPON);
					if (item) {
						DWORD toSetValueEffect;
						switch (this->GetSubType()) {
							case WEAPON_SWORD:
								toSetValueEffect = item->GetValue(0);
								break;
							case WEAPON_DAGGER:
								toSetValueEffect = item->GetValue(2);
								break;
							case WEAPON_BOW:
								toSetValueEffect = item->GetValue(3);
								break;
							case WEAPON_TWO_HANDED:
								toSetValueEffect = item->GetValue(1);
								break;
							case WEAPON_BELL:
								toSetValueEffect = item->GetValue(4);
								break;
							case WEAPON_FAN:
								toSetValueEffect = item->GetValue(5);
								break;
							default:
								toSetValueEffect = 0;
								break;
						}
						
						if (toSetValueEffect > 0) {
							DWORD dwWeaponVnum = GetVnum();
							if (((dwWeaponVnum >= 1180) && (dwWeaponVnum <= 1189)) || 
								((dwWeaponVnum >= 1090) && (dwWeaponVnum <= 1099)) || 
								(dwWeaponVnum == 1199) || 
								(dwWeaponVnum == 1209) || 
								(dwWeaponVnum == 1219) || 
								(dwWeaponVnum == 1229) || 
								(dwWeaponVnum == 40099) || 
								((dwWeaponVnum >= 7190) && (dwWeaponVnum <= 7199))
							)
								toSetValueEffect += 500;
						}
						
						if (!bAdd)
							toSetValueEffect = 0;
						
						m_pOwner->SetPart((BYTE)PART_EFFECT_WEAPON, toSetValueEffect);
					}
				}
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
				if (0 != m_pOwner->GetWear(WEAR_COSTUME_WEAPON))
					break;
#endif

				if (bAdd)
				{
					if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON) {
#if defined(ENABLE_CHANGELOOK)
						m_pOwner->SetPart(PART_WEAPON, GetTransmutation() != 0 ? GetTransmutation() : GetVnum());
#else
						m_pOwner->SetPart(PART_WEAPON, GetVnum());
#endif
					}
				} else {
					if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON) {
						m_pOwner->SetPart(PART_WEAPON, 0);
					}
				}
			}
			break;
		case ITEM_ARMOR:
			{
#ifdef ENABLE_COSTUME_EFFECT
				if (GetSubType() == ARMOR_BODY) {
					CItem* item = m_pOwner->GetWear(WEAR_COSTUME_EFFECT_BODY);
					if (item) {
						DWORD toSetValueEffect;
						toSetValueEffect = item->GetValue(0);
						if ((!bAdd) && (!m_pOwner->GetWear(WEAR_COSTUME_BODY)))
							toSetValueEffect = 0;
						
						m_pOwner->SetPart((BYTE)PART_EFFECT_BODY, toSetValueEffect);
					}
				}
#endif

				if (0 != m_pOwner->GetWear(WEAR_COSTUME_BODY)) {
					break;
				}

				if (GetSubType() == ARMOR_BODY) {
					if (bAdd) {
						if (GetProto()->bSubType == ARMOR_BODY) {
#if defined(ENABLE_CHANGELOOK)
							m_pOwner->SetPart(PART_MAIN, GetTransmutation() != 0 ? GetTransmutation() : GetVnum());
#else
							m_pOwner->SetPart(PART_MAIN, GetVnum());
#endif
						}
					} else {
						if (GetProto()->bSubType == ARMOR_BODY) {
							m_pOwner->SetPart(PART_MAIN, m_pOwner->GetOriginalPart(PART_MAIN));
						}
					}
				}
			}
			break;
		case ITEM_COSTUME:
			{
				DWORD toSetValue = GetVnum();
				EParts toSetPart = PART_MAX_NUM;

				if (GetSubType() == COSTUME_BODY)
				{
#ifdef ENABLE_COSTUME_EFFECT
					CItem* item = m_pOwner->GetWear(WEAR_COSTUME_EFFECT_BODY);
					if (item) {
						DWORD toSetValueEffect = item->GetValue(0);
						if (!bAdd && !m_pOwner->GetWear(WEAR_BODY)) {
							toSetValueEffect = 0;
						}

						m_pOwner->SetPart((BYTE)PART_EFFECT_BODY, toSetValueEffect);
					}
#endif

					toSetPart = PART_MAIN;

					if (false == bAdd)
					{
						const CItem* pArmor = m_pOwner->GetWear(WEAR_BODY);
						toSetValue = (NULL != pArmor) ? pArmor->GetVnum() : m_pOwner->GetOriginalPart(PART_MAIN);
#if defined(ENABLE_CHANGELOOK)
						if (pArmor) {
							toSetValue = pArmor->GetTransmutation() != 0 ? pArmor->GetTransmutation() : pArmor->GetVnum();
						}
#endif
					}
#if defined(ENABLE_CHANGELOOK)
					else {
						toSetValue = GetTransmutation() != 0 ? GetTransmutation() : GetVnum();
					}
#endif
				}
				else if (GetSubType() == COSTUME_HAIR)
				{
					toSetPart = PART_HAIR;

#if defined(ENABLE_CHANGELOOK)
					uint32_t transmutation = GetTransmutation();
					if (transmutation != 0)
					{
						TItemTable* pItemTable = ITEM_MANAGER::instance().GetTable(transmutation);
						toSetValue = pItemTable != NULL ? pItemTable->alValues[3] : GetValue(3);
					} else {
						toSetValue = (true == bAdd) ? this->GetValue(3) : 0;
					}
#else
					toSetValue = (true == bAdd) ? this->GetValue(3) : 0;
#endif
				}
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
				else if (GetSubType() == COSTUME_WEAPON)
				{
					toSetPart = PART_WEAPON;

					if (false == bAdd) {
						const CItem* pWeapon = m_pOwner->GetWear(WEAR_WEAPON);
						if (pWeapon != NULL) {
							toSetValue = (NULL != pWeapon) ? pWeapon->GetVnum() : m_pOwner->GetOriginalPart(PART_WEAPON);
#if defined(ENABLE_CHANGELOOK)
							if (pWeapon->GetTransmutation() != 0) {
								toSetValue = pWeapon->GetTransmutation();
							}
#endif
						} else {
							toSetValue = 0;
						}
					}
#if defined(ENABLE_CHANGELOOK)
					else {
						toSetValue = GetTransmutation() != 0 ? GetTransmutation() : GetVnum();
					}
#endif
				}
#endif
#ifdef ENABLE_ACCE_SYSTEM
				else if (GetSubType() == COSTUME_ACCE)
				{
					toSetValue -= 85000;
					if (GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_EFFECT_FROM_ABS)
						toSetValue += 1000;
					
					toSetValue = (bAdd == true) ? toSetValue : 0;
					
#ifdef ENABLE_STOLE_COSTUME
					const CItem* pAcce = m_pOwner->GetWear(WEAR_COSTUME_ACCE);
					if (pAcce) {
						toSetValue = pAcce->GetVnum();
						toSetValue -= 85000;
						toSetValue += 1000;
					}
#endif
					
					toSetPart = PART_ACCE;
				}
#endif
#ifdef ENABLE_STOLE_COSTUME
				else if (GetSubType() == COSTUME_STOLE)
				{
					toSetValue -= 85000;
					if (!bAdd) {
						CItem* pAcce = m_pOwner->GetWear(WEAR_COSTUME_ACCE_SLOT);
						if (!pAcce)
							toSetValue = 0;
						else {
							toSetValue = pAcce->GetVnum();
							toSetValue -= 85000;
							if (pAcce->GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_EFFECT_FROM_ABS)
								toSetValue += 1000;
						}
					}
					else
						toSetValue += 1000;
					
					toSetPart = PART_ACCE;
				}
#endif
#ifdef ENABLE_COSTUME_EFFECT
				else if (GetSubType() == COSTUME_EFFECT_BODY)
				{
					if (bAdd) {
						CItem* item = m_pOwner->GetWear(WEAR_BODY);
						toSetValue = item != NULL ? this->GetValue(0) : 0;
						if (toSetValue == 0) {
							item = m_pOwner->GetWear(WEAR_COSTUME_BODY);
							toSetValue = item != NULL ? this->GetValue(0) : 0;
						}
					}
					else
						toSetValue = 0;
					
					toSetPart = PART_EFFECT_BODY;
				}
				else if (GetSubType() == COSTUME_EFFECT_WEAPON)
				{
					if (bAdd) {
						CItem* item = m_pOwner->GetWear(WEAR_WEAPON);
						if (item) {
							switch (item->GetSubType()) {
								case WEAPON_SWORD:
									toSetValue = this->GetValue(0);
									break;
								case WEAPON_DAGGER:
									toSetValue = this->GetValue(2);
									break;
								case WEAPON_BOW:
									toSetValue = this->GetValue(3);
									break;
								case WEAPON_TWO_HANDED:
									toSetValue = this->GetValue(1);
									break;
								case WEAPON_BELL:
									toSetValue = this->GetValue(4);
									break;
								case WEAPON_FAN:
									toSetValue = this->GetValue(5);
									break;
								default:
									toSetValue = 0;
									break;
							}
							
							if (toSetValue > 0) {
								DWORD dwWeaponVnum = item->GetVnum();
								if (((dwWeaponVnum >= 1180) && (dwWeaponVnum <= 1189)) || 
									((dwWeaponVnum >= 1090) && (dwWeaponVnum <= 1099)) || 
									(dwWeaponVnum == 1199) || 
									(dwWeaponVnum == 1209) || 
									(dwWeaponVnum == 1219) || 
									(dwWeaponVnum == 1229) || 
									(dwWeaponVnum == 40099) || 
									((dwWeaponVnum >= 7190) && (dwWeaponVnum <= 7199))
								)
									toSetValue += 500;
							}
						}
						else
							toSetValue = 0;
					}
					else
						toSetValue = 0;
					
					toSetPart = PART_EFFECT_WEAPON;
				}
#endif
#if defined(ENABLE_AURA_SYSTEM)
				else if (GetSubType() == COSTUME_AURA) {
					if (m_wCell == INVENTORY_MAX_NUM + WEAR_COSTUME_AURA) {
						m_pOwner->SetPart(PART_AURA, bAdd ? GetVnum() : 0);
					}
				}
#endif

				if (PART_MAX_NUM != toSetPart)
				{
					m_pOwner->SetPart((BYTE)toSetPart, toSetValue);
					m_pOwner->UpdatePacket();
				}
			}
			break;
		case ITEM_UNIQUE:
			{
				if (0 != GetSIGVnum())
				{
					const CSpecialItemGroup* pItemGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(GetSIGVnum());
					if (NULL == pItemGroup)
						break;
					DWORD dwAttrVnum = pItemGroup->GetAttrVnum(GetVnum());
					const CSpecialAttrGroup* pAttrGroup = ITEM_MANAGER::instance().GetSpecialAttrGroup(dwAttrVnum);
					if (NULL == pAttrGroup)
						break;
					for (auto it = pAttrGroup->m_vecAttrs.begin(); it != pAttrGroup->m_vecAttrs.end(); it++)
					{
						m_pOwner->ApplyPoint(it->apply_type, bAdd ? it->apply_value : -it->apply_value);
					}
				}
			}
			break;
	}
}

bool CItem::IsEquipable() const
{
	switch (this->GetType())
	{
	case ITEM_COSTUME:
	case ITEM_ARMOR:
	case ITEM_WEAPON:
	case ITEM_ROD:
	case ITEM_PICK:
	case ITEM_UNIQUE:
	case ITEM_DS:
	case ITEM_SPECIAL_DS:
	case ITEM_RING:
	case ITEM_BELT:
		return true;
	}

	return false;
}

#define ENABLE_IMMUNE_FIX
// return false on error state
bool CItem::EquipTo(LPCHARACTER ch, BYTE bWearCell)
{
	if (!ch)
	{
		sys_err("EquipTo: nil character");
		return false;
	}

	// 용혼석 슬롯 index는 WEAR_MAX_NUM 보다 큼.
	if (IsDragonSoul())
	{
		if (bWearCell < WEAR_MAX_NUM || bWearCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
		{
			sys_err("EquipTo: invalid dragon soul cell (this: #%d %s wearflag: %d cell: %d)", GetOriginalVnum(), GetName(), GetSubType(), bWearCell - WEAR_MAX_NUM);
			return false;
		}
	}
	else
	{
		if (bWearCell >= WEAR_MAX_NUM)
		{
			sys_err("EquipTo: invalid wear cell (this: #%d %s wearflag: %d cell: %d)", GetOriginalVnum(), GetName(), GetWearFlag(), bWearCell);
			return false;
		}
	}

	if (ch->GetWear(bWearCell))
	{
		sys_err("EquipTo: item already exist (this: #%d %s cell: %d %s)", GetOriginalVnum(), GetName(), bWearCell, ch->GetWear(bWearCell)->GetName());
		return false;
	}

	if (GetOwner())
		RemoveFromCharacter();

	ch->SetWear(bWearCell, this); // 여기서 패킷 나감

	m_pOwner = ch;
	m_bEquipped = true;
	m_wCell	= INVENTORY_MAX_NUM + bWearCell;

#ifndef ENABLE_IMMUNE_FIX
	DWORD dwImmuneFlag = 0;

	for (int i = 0; i < WEAR_MAX_NUM; ++i)
	{
		if (m_pOwner->GetWear(i))
		{
			SET_BIT(dwImmuneFlag, m_pOwner->GetWear(i)->m_pProto->dwImmuneFlag);
		}
	}

	m_pOwner->SetImmuneFlag(dwImmuneFlag);
#endif

	uint8_t itemType = GetType();

	if (IsDragonSoul())
	{
		DSManager::instance().ActivateDragonSoul(this);
	}
	else
	{
		ModifyPoints(true);

		StartUniqueExpireEvent();
		if (-1 != GetProto()->cLimitTimerBasedOnWearIndex)
			StartTimerBasedOnWearExpireEvent();

		// ACCESSORY_REFINE
		StartAccessorySocketExpireEvent();
		// END_OF_ACCESSORY_REFINE
	}

	ch->BuffOnAttr_AddBuffsFromItem(this);

	m_pOwner->ComputeBattlePoints();

	m_pOwner->UpdatePacket();

#if defined(ENABLE_PETSKIN)
	if (itemType == ITEM_COSTUME && GetSubType() == COSTUME_PETSKIN) {
		m_pOwner->UpdatePetSkin();
	}
#endif

#if defined(ENABLE_NEW_ANTICHEAT_RULES)
	if (itemType == ITEM_WEAPON) {
		m_pOwner->SetDaggerEquipped(GetSubType() == WEAPON_DAGGER ? true : false);
	}
#endif

	Save();
	return (true);
}

bool CItem::Unequip()
{
	if (!m_pOwner || GetCell() < INVENTORY_MAX_NUM)
	{
		// ITEM_OWNER_INVALID_PTR_BUG
		sys_err("%s %u m_pOwner %p, GetCell %d",
				GetName(), GetID(), get_pointer(m_pOwner), GetCell());
		// END_OF_ITEM_OWNER_INVALID_PTR_BUG
		return false;
	}

	if (this != m_pOwner->GetWear(GetCell() - INVENTORY_MAX_NUM))
	{
		sys_err("m_pOwner->GetWear() != this");
		return false;
	}

#if defined(USE_MOUNT_COSTUME_SYSTEM) && defined(ENABLE_ANTI_FLOOD)
	if ((GetCell() - INVENTORY_MAX_NUM) == WEAR_COSTUME_MOUNT)
	{
		m_pOwner->SetAntiFloodPulse(FLOOD_MOUNT_ITEM, thecore_pulse());
	}
#endif

	if (IsRideItem())
	{
		ClearMountAttributeAndAffect();
	}

	if (IsDragonSoul())
	{
		DSManager::instance().DeactivateDragonSoul(this);
	}
	else
	{
		ModifyPoints(false);
	}

	StopUniqueExpireEvent();

	if (-1 != GetProto()->cLimitTimerBasedOnWearIndex)
		StopTimerBasedOnWearExpireEvent();

	// ACCESSORY_REFINE
	StopAccessorySocketExpireEvent();
	// END_OF_ACCESSORY_REFINE

	uint8_t itemType = GetType();

	m_pOwner->BuffOnAttr_RemoveBuffsFromItem(this);

	m_pOwner->SetWear(GetCell() - INVENTORY_MAX_NUM, NULL);

#ifndef ENABLE_IMMUNE_FIX
	DWORD dwImmuneFlag = 0;

	for (int i = 0; i < WEAR_MAX_NUM; ++i)
	{
		if (m_pOwner->GetWear(i))
		{
			SET_BIT(dwImmuneFlag, m_pOwner->GetWear(i)->m_pProto->dwImmuneFlag);
		}
	}

	m_pOwner->SetImmuneFlag(dwImmuneFlag);
#endif

	m_pOwner->ComputeBattlePoints();

	m_pOwner->UpdatePacket();

#if defined(ENABLE_PETSKIN)
	if (itemType == ITEM_COSTUME && GetSubType() == COSTUME_PETSKIN) {
		m_pOwner->UpdatePetSkin();
	}
#endif

#if defined(ENABLE_NEW_ANTICHEAT_RULES)
	if (itemType == ITEM_WEAPON) {
		m_pOwner->SetDaggerEquipped(false);
	}
#endif

	m_pOwner = NULL;
	m_wCell = 0;
	m_bEquipped	= false;

	return true;
}

long CItem::GetValue(DWORD idx)
{
	assert(idx < ITEM_VALUES_MAX_NUM);
	return GetProto()->alValues[idx];
}

void CItem::SetExchanging(bool bOn)
{
	m_bExchanging = bOn;
}

void CItem::Save()
{
	if (m_bSkipSave)
		return;

	ITEM_MANAGER::instance().DelayedSave(this);
}

bool CItem::CreateSocket(BYTE bSlot, BYTE bGold)
{
	assert(bSlot < ITEM_SOCKET_MAX_NUM);

	if (m_alSockets[bSlot] != 0)
	{
		sys_err("Item::CreateSocket : socket already exist %s %d", GetName(), bSlot);
		return false;
	}

	if (bGold)
		m_alSockets[bSlot] = 2;
	else
		m_alSockets[bSlot] = 1;

	UpdatePacket();

	Save();
	return true;
}

void CItem::SetSockets(const long * c_al)
{
	memcpy(m_alSockets, c_al, sizeof(m_alSockets));
	Save();
}

void CItem::SetSocket(int i, long v, bool bLog)
{
	assert(i < ITEM_SOCKET_MAX_NUM);
	m_alSockets[i] = v;
	UpdatePacket();
	Save();
	if (bLog)
	{
		if (g_iDbLogLevel>=LOG_LEVEL_MAX)
			LogManager::instance().ItemLog(i, v, 0, GetID(), "SET_SOCKET", "", "", GetOriginalVnum());
	}
}

#if defined(ENABLE_NEW_GOLD_LIMIT)
uint64_t CItem::GetGold()
#else
uint32_t CItem::GetGold()
#endif
{
	if (IS_SET(GetFlag(), ITEM_FLAG_COUNT_PER_1GOLD))
	{
		if (GetProto()->dwGold == 0)
			return GetCount();
		else
			return GetCount() / GetProto()->dwGold;
	}
	else
		return GetProto()->dwGold;
}

#if defined(ENABLE_NEW_GOLD_LIMIT)
uint64_t CItem::GetShopBuyPrice()
#else
uint32_t CItem::GetShopBuyPrice()
#endif
{
	return GetProto()->dwShopBuyPrice;
}

bool CItem::IsOwnershipEvent(LPCHARACTER ch)
{
	if (!m_pkOwnershipEvent)
	{
		return false;
	}

	return m_dwOwnershipPID == ch->GetPlayerID() ? true : false;
}

bool CItem::IsOwnership(LPCHARACTER ch)
{
	if (!m_pkOwnershipEvent)
		return true;

	return m_dwOwnershipPID == ch->GetPlayerID() ? true : false;
}

EVENTFUNC(ownership_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "ownership_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;

	pkItem->SetOwnershipEvent(NULL);

	TPacketGCItemOwnership p;

	p.bHeader	= HEADER_GC_ITEM_OWNERSHIP;
	p.dwVID	= pkItem->GetVID();
	p.szName[0]	= '\0';

	pkItem->PacketAround(&p, sizeof(p));
	return 0;
}

void CItem::SetOwnershipEvent(LPEVENT pkEvent)
{
	m_pkOwnershipEvent = pkEvent;
}

void CItem::SetOwnership(LPCHARACTER ch, int iSec)
{
	if (!ch)
	{
		if (m_pkOwnershipEvent)
		{
			event_cancel(&m_pkOwnershipEvent);
			m_dwOwnershipPID = 0;

			TPacketGCItemOwnership p;

			p.bHeader	= HEADER_GC_ITEM_OWNERSHIP;
			p.dwVID	= m_dwVID;
			p.szName[0]	= '\0';

			PacketAround(&p, sizeof(p));
		}
		return;
	}

	if (m_pkOwnershipEvent)
		return;

	if (iSec <= 10)
		iSec = 30;

	m_dwOwnershipPID = ch->GetPlayerID();

	item_event_info* info = AllocEventInfo<item_event_info>();
	strlcpy(info->szOwnerName, ch->GetName(), sizeof(info->szOwnerName));
	info->item = this;

	SetOwnershipEvent(event_create(ownership_event, info, PASSES_PER_SEC(iSec)));

	TPacketGCItemOwnership p;

	p.bHeader = HEADER_GC_ITEM_OWNERSHIP;
	p.dwVID = m_dwVID;
	strlcpy(p.szName, ch->GetName(), sizeof(p.szName));

	PacketAround(&p, sizeof(p));
}

int CItem::GetSocketCount()
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
	{
		if (GetSocket(i) == 0)
			return i;
	}
	return ITEM_SOCKET_MAX_NUM;
}

bool CItem::AddSocket()
{
	int count = GetSocketCount();
	if (count == ITEM_SOCKET_MAX_NUM)
		return false;
	m_alSockets[count] = 1;
	return true;
}

void CItem::AlterToSocketItem(int iSocketCount)
{
	if (iSocketCount >= ITEM_SOCKET_MAX_NUM)
	{
		sys_log(0, "Invalid Socket Count %d, set to maximum", ITEM_SOCKET_MAX_NUM);
		iSocketCount = ITEM_SOCKET_MAX_NUM;
	}

	for (int i = 0; i < iSocketCount; ++i)
		SetSocket(i, 1);
}

void CItem::AlterToMagicItem()
{
	if (GetAttributeSetIndex() < 0)
	{
		return;
	}

	int iSecondPct;
	int iThirdPct;

	switch (GetType())
	{
		case ITEM_WEAPON:
			{
				iSecondPct = 20;
				iThirdPct = 5;
			}
			break;

		case ITEM_ARMOR:
			{
				if (GetSubType() == ARMOR_BODY)
				{
					iSecondPct = 10;
					iThirdPct = 2;
				}
				else
				{
					iSecondPct = 10;
					iThirdPct = 1;
				}
			}
			break;
#ifdef ENABLE_ATTR_COSTUMES
		case ITEM_COSTUME:
			{
				uint8_t subtype = GetSubType();
				iSecondPct = subtype == COSTUME_BODY || subtype == COSTUME_HAIR
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
				 || subtype == COSTUME_WEAPON
#endif
				 ? 100 : 0;
				iThirdPct = subtype == COSTUME_BODY || subtype == COSTUME_HAIR
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
				 || subtype == COSTUME_WEAPON
#endif
				 ? 100 : 0;
			}
			break;
#endif
		default:
			{
				iSecondPct = 0;
				iThirdPct = 0;
			}
			break;
	}

	if (iSecondPct == 0 && iThirdPct == 0)
	{
		return;
	}

	PutAttribute(aiItemMagicAttributePercentHigh);
	if (number(1, 100) <= iSecondPct)
	{
		PutAttribute(aiItemMagicAttributePercentLow);
	}

	if (number(1, 100) <= iThirdPct)
	{
		PutAttribute(aiItemMagicAttributePercentLow);
	}
}

DWORD CItem::GetRefineFromVnum()
{
	return ITEM_MANAGER::instance().GetRefineFromVnum(GetVnum());
}

int CItem::GetRefineLevel()
{
	const char* name = GetBaseName();
	char* p = const_cast<char*>(strrchr(name, '+'));

	if (!p)
		return 0;

	int	rtn = 0;
	str_to_number(rtn, p+1);

	const char* locale_name = GetName();
	p = const_cast<char*>(strrchr(locale_name, '+'));

	if (p)
	{
		int	locale_rtn = 0;
		str_to_number(locale_rtn, p+1);
		if (locale_rtn != rtn)
		{
			sys_err("refine_level_based_on_NAME(%d) is not equal to refine_level_based_on_LOCALE_NAME(%d).", rtn, locale_rtn);
		}
	}

	return rtn;
}

bool CItem::IsPolymorphItem()
{
	return GetType() == ITEM_POLYMORPH;
}

EVENTFUNC(unique_expire_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "unique_expire_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;
	if (pkItem == nullptr)
		return 0;

	if (pkItem->GetValue(2) == 0)
	{
		if (pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) <= 1)
		{
			sys_log(0, "UNIQUE_ITEM: expire %s %u", pkItem->GetName(), pkItem->GetID());
			pkItem->SetUniqueExpireEvent(NULL);
			ITEM_MANAGER::instance().RemoveItem(pkItem, "UNIQUE_EXPIRE");
			return 0;
		}
		else
		{
			pkItem->SetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME, pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) - 1);
			return PASSES_PER_SEC(60);
		}
	}
	else
	{
		time_t cur = get_global_time();

		if (pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) <= cur)
		{
			pkItem->SetUniqueExpireEvent(NULL);
			ITEM_MANAGER::instance().RemoveItem(pkItem, "UNIQUE_EXPIRE");
			return 0;
		}
		else
		{
			// 게임 내에 시간제 아이템들이 빠릿빠릿하게 사라지지 않는 버그가 있어
			// 수정
			// by rtsummit
			if (pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) - cur < 600)
				return PASSES_PER_SEC(pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) - cur);
			else
				return PASSES_PER_SEC(600);
		}
	}
}

// 시간 후불제
// timer를 시작할 때에 시간 차감하는 것이 아니라,
// timer가 발화할 때에 timer가 동작한 시간 만큼 시간 차감을 한다.
EVENTFUNC(timer_based_on_wear_expire_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "expire_event <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;
	if (pkItem == nullptr)
		return 0;

	int remain_time = pkItem->GetSocket(ITEM_SOCKET_REMAIN_SEC) - processing_time/passes_per_sec;
	if (remain_time <= 0)
	{
		sys_log(0, "ITEM EXPIRED : expired %s %u", pkItem->GetName(), pkItem->GetID());
		pkItem->SetTimerBasedOnWearExpireEvent(NULL);
		pkItem->SetSocket(ITEM_SOCKET_REMAIN_SEC, 0);

		// 일단 timer based on wear 용혼석은 시간 다 되었다고 없애지 않는다.
		if (pkItem->IsDragonSoul())
		{
			DSManager::instance().DeactivateDragonSoul(pkItem);
		}
		else
		{
			ITEM_MANAGER::instance().RemoveItem(pkItem, "TIMER_BASED_ON_WEAR_EXPIRE");
		}
		return 0;
	}
	
	pkItem->SetSocket(ITEM_SOCKET_REMAIN_SEC, remain_time);
	return PASSES_PER_SEC (MIN (60, remain_time));
}

void CItem::SetUniqueExpireEvent(LPEVENT pkEvent)
{
	m_pkUniqueExpireEvent = pkEvent;
}

void CItem::SetTimerBasedOnWearExpireEvent(LPEVENT pkEvent)
{
	m_pkTimerBasedOnWearExpireEvent = pkEvent;
}

EVENTFUNC(real_time_expire_event)
{
	const item_vid_event_info* info = reinterpret_cast<const item_vid_event_info*>(event->info);

	if (NULL == info)
		return 0;

	const LPITEM item = ITEM_MANAGER::instance().FindByVID( info->item_vid );

	if (NULL == item)
		return 0;
	
#ifdef ENABLE_NEW_USE_POTION
	if (info->newpotion) {
		long remainSec = item->GetSocket(0);
		if (remainSec <= 0) {
			if (item->GetSocket(1) == 1) {
				LPCHARACTER pkOwner = item->GetOwner();
				if (pkOwner) {
					if (pkOwner->FindAffect(item->GetValue(0))) {
						pkOwner->RemoveAffect(item->GetValue(0));
					}

					pkOwner->ChatPacketNew(CHAT_TYPE_INFO, 27, "%s", item->GetName());
				}
			}

			ITEM_MANAGER::instance().RemoveItem(item, "REAL_TIME_EXPIRE");
			return 0;
		}

		if (item->GetSocket(1) != 1) {
			return PASSES_PER_SEC(60);
		}
		else {
			long nextSec = (remainSec - 60) > 0 ? (remainSec - 60) : 0;
			item->SetSocket(0, nextSec);
			if (nextSec <= 0) {
				LPCHARACTER pkOwner = item->GetOwner();
				if (pkOwner) {
					if (pkOwner->FindAffect(item->GetValue(0))) {
						pkOwner->RemoveAffect(item->GetValue(0));
					}

					pkOwner->ChatPacketNew(CHAT_TYPE_INFO, 27, "%s", item->GetName());
				}

				ITEM_MANAGER::instance().RemoveItem(item, "REAL_TIME_EXPIRE");
				return 0;
			} else {
				return PASSES_PER_SEC(nextSec > 60 ? 60 : nextSec);
			}
		}
	}
#endif

	const time_t current = get_global_time();
	if (current > item->GetSocket(0))
	{
/* to review
#if defined(ENABLE_CHANGELOOK)
		if (item->isLocked()) {
			const LPCHARACTER owner = item->GetOwner();
			if (owner) {
				owner->ClearChangeLookWindow();
			}
		}
#endif
*/

		item->CleanRealTimeExpireEvent();

		ITEM_MANAGER::instance().RemoveItem(item, "REAL_TIME_EXPIRE");
		return 0;
	}

	return PASSES_PER_SEC(1);
}

void CItem::StartRealTimeExpireEvent()
{
	if (m_pkRealTimeExpireEvent)
		return;
	
	for (int i=0 ; i < ITEM_LIMIT_MAX_NUM ; i++)
	{
		if (LIMIT_REAL_TIME == GetProto()->aLimits[i].bType || LIMIT_REAL_TIME_START_FIRST_USE == GetProto()->aLimits[i].bType)
		{
			item_vid_event_info* info = AllocEventInfo<item_vid_event_info>();
			info->item_vid = GetVID();
#ifdef ENABLE_NEW_USE_POTION
			if ((GetType() == ITEM_USE) && (GetSubType() == USE_NEW_POTIION)) {
				long remainSec = GetSocket(0);
				if (remainSec <= 0) {
					if (GetSocket(1) == 1) {
						LPCHARACTER pkOwner = GetOwner();
						if (pkOwner) {
							if (pkOwner->FindAffect(GetValue(0))) {
								pkOwner->RemoveAffect(GetValue(0));
							}

							pkOwner->ChatPacketNew(CHAT_TYPE_INFO, 27, "%s", GetName());
						}
					}

					ITEM_MANAGER::instance().RemoveItem(this, "REAL_TIME_EXPIRE");
					return;
				}

				info->newpotion = true;
				m_pkRealTimeExpireEvent = event_create(real_time_expire_event, info, PASSES_PER_SEC(remainSec > 60 ? 60 : remainSec));
			}
			else {
				info->newpotion = false;
				m_pkRealTimeExpireEvent = event_create( real_time_expire_event, info, PASSES_PER_SEC(1));
			}
#else
			m_pkRealTimeExpireEvent = event_create( real_time_expire_event, info, PASSES_PER_SEC(1));
#endif
			
			sys_log(0, "REAL_TIME_EXPIRE: StartRealTimeExpireEvent");
			return;
		}
	}
}

void CItem::StopRealTimeExpireEvent()
{
	if (!m_pkRealTimeExpireEvent)
	{
		return;
	}

	event_cancel(&m_pkRealTimeExpireEvent);
	m_pkRealTimeExpireEvent = nullptr;
}

void CItem::CleanRealTimeExpireEvent()
{
	if (m_pkRealTimeExpireEvent)
	{
		m_pkRealTimeExpireEvent = nullptr;
	}
}

bool CItem::IsRealTimeItem()
{
	if(!GetProto())
		return false;
	
	for (int i=0 ; i < ITEM_LIMIT_MAX_NUM ; i++)
	{
		if (LIMIT_REAL_TIME == GetProto()->aLimits[i].bType)
			return true;
	}
	return false;
}

bool CItem::IsRealTimeFirstUseItem()
{
	if(GetProto()) {
		for (int i=0 ; i < ITEM_LIMIT_MAX_NUM ; i++) {
			if (LIMIT_REAL_TIME_START_FIRST_USE == GetProto()->aLimits[i].bType)
				return true;
		}
	}

	return false;
}

bool CItem::IsUnlimitedTimeUnique()
{
	if(GetProto()) {
		for (int i=0 ; i < ITEM_LIMIT_MAX_NUM ; i++) {
			if (LIMIT_UNIQUE_UNLIMITED == GetProto()->aLimits[i].bType)
				return true;
		}
	}

	return false;
}

void CItem::StartUniqueExpireEvent()
{
	if (GetType() != ITEM_UNIQUE)
		return;

	if (m_pkUniqueExpireEvent)
		return;

	//기간제 아이템일 경우 시간제 아이템은 동작하지 않는다
	if (IsRealTimeItem() || IsRealTimeFirstUseItem() || IsUnlimitedTimeUnique())
		return;

	// HARD CODING
	if (GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		m_pOwner->ShowAlignment(false);

	int iSec = GetSocket(ITEM_SOCKET_UNIQUE_SAVE_TIME);

	if (iSec == 0)
		iSec = 60;
	else
		iSec = MIN(iSec, 60);

	SetSocket(ITEM_SOCKET_UNIQUE_SAVE_TIME, 0);

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetUniqueExpireEvent(event_create(unique_expire_event, info, PASSES_PER_SEC(iSec)));
}

// 시간 후불제
// timer_based_on_wear_expire_event 설명 참조
void CItem::StartTimerBasedOnWearExpireEvent()
{
	if (m_pkTimerBasedOnWearExpireEvent)
		return;

	//기간제 아이템일 경우 시간제 아이템은 동작하지 않는다
	if (IsRealTimeItem())
		return;

	if (-1 == GetProto()->cLimitTimerBasedOnWearIndex)
		return;

	int iSec = GetSocket(0);

	// 남은 시간을 분단위로 끊기 위해...
	if (0 != iSec)
	{
		iSec %= 60;
		if (0 == iSec)
			iSec = 60;
	}

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetTimerBasedOnWearExpireEvent(event_create(timer_based_on_wear_expire_event, info, PASSES_PER_SEC(iSec)));
}

void CItem::StopUniqueExpireEvent()
{
	if (!m_pkUniqueExpireEvent)
		return;

	if (GetValue(2) != 0) // 게임시간제 이외의 아이템은 UniqueExpireEvent를 중단할 수 없다.
		return;

	// HARD CODING
	if (GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		m_pOwner->ShowAlignment(true);

	SetSocket(ITEM_SOCKET_UNIQUE_SAVE_TIME, event_time(m_pkUniqueExpireEvent) / passes_per_sec);
	event_cancel(&m_pkUniqueExpireEvent);

	ITEM_MANAGER::instance().SaveSingleItem(this);
}

void CItem::StopTimerBasedOnWearExpireEvent()
{
	if (!m_pkTimerBasedOnWearExpireEvent)
		return;

	int remain_time = GetSocket(ITEM_SOCKET_REMAIN_SEC) - event_processing_time(m_pkTimerBasedOnWearExpireEvent) / passes_per_sec;

	SetSocket(ITEM_SOCKET_REMAIN_SEC, remain_time);
	event_cancel(&m_pkTimerBasedOnWearExpireEvent);

	ITEM_MANAGER::instance().SaveSingleItem(this);
}

#ifdef ENABLE_SWITCHBOT_WORLDARD
void CItem::ApplyAddon(int iAddonType, bool switchbot)
#else
void CItem::ApplyAddon(int iAddonType)
#endif
{
#ifdef ENABLE_SWITCHBOT_WORLDARD
	CItemAddonManager::instance().ApplyAddonTo(iAddonType, this, switchbot);
#else
	CItemAddonManager::instance().ApplyAddonTo(iAddonType, this);
#endif
}

int CItem::GetSpecialGroup() const
{
	return ITEM_MANAGER::instance().GetSpecialGroupFromItem(GetVnum());
}

//
// 악세서리 소켓 처리.
//
bool CItem::IsAccessoryForSocket()
{
	return (m_pProto->bType == ITEM_ARMOR && (m_pProto->bSubType == ARMOR_WRIST || m_pProto->bSubType == ARMOR_NECK || m_pProto->bSubType == ARMOR_EAR)) ||
		(m_pProto->bType == ITEM_BELT);				// 2013년 2월 새로 추가된 '벨트' 아이템의 경우 기획팀에서 악세서리 소켓 시스템을 그대로 이용하자고 함.
}

void CItem::SetAccessorySocketGrade(int iGrade
#ifdef ENABLE_INFINITE_RAFINES
, bool infinite
#endif
)
{
	SetSocket(0, MINMAX(0, iGrade, GetAccessorySocketMaxGrade()));

	int iDownTime = 
#ifdef ENABLE_INFINITE_RAFINES
	infinite == true ? 86410 : aiAccessorySocketDegradeTime[GetAccessorySocketGrade()];
#else
	aiAccessorySocketDegradeTime[GetAccessorySocketGrade()]
#endif
	;

	//if (test_server)
	//	iDownTime /= 60;

	SetAccessorySocketDownGradeTime(iDownTime);
}

void CItem::SetAccessorySocketMaxGrade(int iMaxGrade)
{
	SetSocket(1, MINMAX(0, iMaxGrade, ITEM_ACCESSORY_SOCKET_MAX_NUM));
}

void CItem::SetAccessorySocketDownGradeTime(DWORD time)
{
	SetSocket(2, time);
}

EVENTFUNC(accessory_socket_expire_event)
{
	item_vid_event_info* info = dynamic_cast<item_vid_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "accessory_socket_expire_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM item = ITEM_MANAGER::instance().FindByVID(info->item_vid);
	if (item->GetAccessorySocketDownGradeTime() <= 1)
	{
degrade:
		item->SetAccessorySocketExpireEvent(NULL);
		item->AccessorySocketDegrade();
		return 0;
	}
	else
	{
		int iTime = item->GetAccessorySocketDownGradeTime() - 60;

		if (iTime <= 1)
			goto degrade;

		item->SetAccessorySocketDownGradeTime(iTime);

		if (iTime > 60)
			return PASSES_PER_SEC(60);
		else
			return PASSES_PER_SEC(iTime);
	}
}

void CItem::StartAccessorySocketExpireEvent()
{
	if (!IsAccessoryForSocket())
		return;

	if (m_pkAccessorySocketExpireEvent)
		return;

	if (GetAccessorySocketMaxGrade() == 0)
		return;

	if (GetAccessorySocketGrade() == 0)
		return;

	int iSec = GetAccessorySocketDownGradeTime();
#ifdef ENABLE_INFINITE_RAFINES
	if (iSec > 86400) {
		return;
	}
#endif
	SetAccessorySocketExpireEvent(NULL);

	if (iSec <= 1)
		iSec = 5;
	else
		iSec = MIN(iSec, 60);

	item_vid_event_info* info = AllocEventInfo<item_vid_event_info>();
	info->item_vid = GetVID();

	SetAccessorySocketExpireEvent(event_create(accessory_socket_expire_event, info, PASSES_PER_SEC(iSec)));
}

void CItem::StopAccessorySocketExpireEvent()
{
	if (!m_pkAccessorySocketExpireEvent)
		return;

	if (!IsAccessoryForSocket())
		return;

	int new_time = GetAccessorySocketDownGradeTime() - (60 - event_time(m_pkAccessorySocketExpireEvent) / passes_per_sec);

	event_cancel(&m_pkAccessorySocketExpireEvent);

	if (new_time <= 1)
	{
		AccessorySocketDegrade();
	}
	else
	{
		SetAccessorySocketDownGradeTime(new_time);
	}
}

bool CItem::IsRideItem()
{
#if defined(USE_MOUNT_COSTUME_SYSTEM)
	if (ITEM_COSTUME == GetType() && COSTUME_MOUNT == GetSubType()) {
		return true;
	}
#else
	if (ITEM_UNIQUE == GetType() && UNIQUE_SPECIAL_RIDE == GetSubType()) {
		return true;
	}

	if (ITEM_UNIQUE == GetType() && UNIQUE_SPECIAL_MOUNT_RIDE == GetSubType()) {
		return true;
	}
#endif

	return false;
}

#if defined(ENABLE_MOUNTSKIN)
bool CItem::IsRideSkin()
{
	if (GetType() == ITEM_COSTUME
		&& GetSubType() == COSTUME_MOUNTSKIN)
	{
		return true;
	}

	return false;
}
#endif

#if defined(USE_MOUNT_COSTUME_SYSTEM)
uint32_t CItem::GetMountVnum()
{
	if (!IsRideItem() 
#if defined(USE_MOUNT_COSTUME_SYSTEM)
		 && !IsRideSkin()
#endif
	)
	{
		return 0;
	}
//	const uint32_t vnum = FindApplyValue(APPLY_MOUNT);

// transmutation

	return FindApplyValue(APPLY_MOUNT);
}
#endif

bool CItem::IsRamadanRing()
{
	if (GetVnum() == UNIQUE_ITEM_RAMADAN_RING)
		return true;
	return false;
}

void CItem::ClearMountAttributeAndAffect()
{
	LPCHARACTER ch = GetOwner();
	if (!ch)
	{
		return;
	}

	ch->RemoveAffect(AFFECT_MOUNT);
	ch->RemoveAffect(AFFECT_MOUNT_BONUS);

	ch->MountVnum(0);

	ch->PointChange(POINT_ST, 0);
	ch->PointChange(POINT_DX, 0);
	ch->PointChange(POINT_HT, 0);
	ch->PointChange(POINT_IQ, 0);
}

void CItem::SetAccessorySocketExpireEvent(LPEVENT pkEvent)
{
	m_pkAccessorySocketExpireEvent = pkEvent;
}

void CItem::AccessorySocketDegrade()
{
	if (GetAccessorySocketGrade() > 0)
	{
		LPCHARACTER ch = GetOwner();
		if (ch) {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 117, "%s", GetName());
		}

		ModifyPoints(false);
		SetAccessorySocketGrade(GetAccessorySocketGrade()-1);
		ModifyPoints(true);

		int iDownTime = aiAccessorySocketDegradeTime[GetAccessorySocketGrade()];

		if (test_server)
			iDownTime /= 60;

		SetAccessorySocketDownGradeTime(iDownTime);

		if (iDownTime)
			StartAccessorySocketExpireEvent();
	}
}

// ring에 item을 박을 수 있는지 여부를 체크해서 리턴
static const bool CanPutIntoRing(LPITEM ring, LPITEM item)
{
	//const DWORD vnum = item->GetVnum();
	return false;
}

bool CItem::CanPutInto(LPITEM item)
{
	if (item->GetType() == ITEM_BELT) {
		if (GetSubType() == USE_PUT_INTO_BELT_SOCKET && GetValue(0) != 1) {
			return true;
		}
		else {
			return false;
		}
	}
	else if(item->GetType() == ITEM_RING)
		return CanPutIntoRing(item, this);

	else if (item->GetType() != ITEM_ARMOR)
		return false;

	DWORD vnum = item->GetVnum();

	if (GetVnum() == 50634) {
		return (vnum >= 14220 && vnum <= 14234) || (vnum >= 16220 && vnum <= 16234) || (vnum >= 17220 && vnum <= 17234) ? true : false;
	}

	if (GetVnum() == 50640) {
		return (vnum >= 14580 && vnum <= 14589) || (vnum >= 15010 && vnum <= 15013) || (vnum >= 16580 && vnum <= 16593) || (vnum >= 17570 && vnum <= 17583) ? true : false;
	}

	struct JewelAccessoryInfo
	{
		DWORD jewel;
		DWORD wrist;
		DWORD neck;
		DWORD ear;
	};
	const static JewelAccessoryInfo infos[] = {
		{ 50634, 14420, 16220, 17220 },
		{ 50635, 14500, 16500, 17500 },
		{ 50636, 14520, 16520, 17520 },
		{ 50637, 14540, 16540, 17540 },
		{ 50638, 14560, 16560, 17560 },
		{ 50639, 14570, 16570, 17570 },
	};

	DWORD item_type = (item->GetVnum() / 10) * 10;
	for (size_t i = 0; i < sizeof(infos) / sizeof(infos[0]); i++)
	{
		const JewelAccessoryInfo& info = infos[i];
		switch(item->GetSubType())
		{
		case ARMOR_WRIST:
			if (info.wrist == item_type)
			{
				if (info.jewel == GetVnum())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			break;
		case ARMOR_NECK:
			if (info.neck == item_type)
			{
				if (info.jewel == GetVnum())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			break;
		case ARMOR_EAR:
			if (info.ear == item_type)
			{
				if (info.jewel == GetVnum())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			break;
		}
	}
	if (item->GetSubType() == ARMOR_WRIST)
		vnum -= 14000;
	else if (item->GetSubType() == ARMOR_NECK)
		vnum -= 16000;
	else if (item->GetSubType() == ARMOR_EAR)
		vnum -= 17000;
	else
		return false;

	DWORD type = vnum / 20;

	if (type < 0 || type > 11)
	{
		type = (vnum - 170) / 20;

		if (50623 + type != GetVnum())
			return false;
		else
			return true;
	}
	else if (item->GetVnum() >= 16210 && item->GetVnum() <= 16219)
	{
		if (50625 != GetVnum())
			return false;
		else
			return true;
	}
	else if (item->GetVnum() >= 16230 && item->GetVnum() <= 16239)
	{
		if (50626 != GetVnum())
			return false;
		else
			return true;
	}

	return 50623 + type == GetVnum();
}

#ifdef ENABLE_INFINITE_RAFINES
bool CItem::CanPutInto2(LPITEM item)
{
	if (item->GetType() == ITEM_BELT) {
		if (GetSubType() == USE_PUT_INTO_BELT_SOCKET && GetValue(0) == 1) {
			return true;
		}
		else {
			return false;
		}
	}

	else if(item->GetType() == ITEM_RING)
		return CanPutIntoRing(item, this);

	else if (item->GetType() != ITEM_ARMOR)
		return false;

	DWORD vnum = item->GetVnum();

	if (GetVnum() == 50684) {
		return (vnum >= 14220 && vnum <= 14234) || (vnum >= 16220 && vnum <= 16234) || (vnum >= 17220 && vnum <= 17234) ? true : false;
	}

	if (GetVnum() == 50690) {
		return (vnum >= 14580 && vnum <= 14589) || (vnum >= 15010 && vnum <= 15013) || (vnum >= 16580 && vnum <= 16593) || (vnum >= 17570 && vnum <= 17583) || (vnum >= 16240 && vnum <= 16245) || (vnum >= 17240 && vnum <= 17245) || (vnum >= 14240 && vnum <= 14245) ? true : false;
	}

	struct JewelAccessoryInfo
	{
		DWORD jewel;
		DWORD wrist;
		DWORD neck;
		DWORD ear;
	};
	const static JewelAccessoryInfo infos[] = {
		{ 50684, 14420, 16220, 17220 },
		{ 50685, 14500, 16500, 17500 },
		{ 50686, 14520, 16520, 17520 },
		{ 50687, 14540, 16540, 17540 },
		{ 50688, 14560, 16560, 17560 },
		{ 50689, 14570, 16570, 17570 },
		{ 50690, 14240, 16240, 17240 },
	};

	DWORD item_type = (item->GetVnum() / 10) * 10;
	for (size_t i = 0; i < sizeof(infos) / sizeof(infos[0]); i++)
	{
		const JewelAccessoryInfo& info = infos[i];
		switch(item->GetSubType())
		{
		case ARMOR_WRIST:
			if (info.wrist == item_type)
			{
				if (info.jewel == GetVnum())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			break;
		case ARMOR_NECK:
			if (info.neck == item_type)
			{
				if (info.jewel == GetVnum())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			break;
		case ARMOR_EAR:
			if (info.ear == item_type)
			{
				if (info.jewel == GetVnum())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			break;
		}
	}
	if (item->GetSubType() == ARMOR_WRIST)
		vnum -= 14000;
	else if (item->GetSubType() == ARMOR_NECK)
		vnum -= 16000;
	else if (item->GetSubType() == ARMOR_EAR)
		vnum -= 17000;
	else
		return false;

	DWORD type = vnum / 20;

	if (type < 0 || type > 11)
	{
		type = (vnum - 170) / 20;

		if (50673 + type != GetVnum())
			return false;
		else
			return true;
	}
	else if (item->GetVnum() >= 16210 && item->GetVnum() <= 16219)
	{
		if (50675 != GetVnum())
			return false;
		else
			return true;
	}
	else if (item->GetVnum() >= 16230 && item->GetVnum() <= 16239)
	{
		if (50676 != GetVnum())
			return false;
		else
			return true;
	}

	return 50673 + type == GetVnum();
}
#endif

bool CItem::CheckItemUseLevel(int nLevel)
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		if (this->m_pProto->aLimits[i].bType == LIMIT_LEVEL)
		{
			if (this->m_pProto->aLimits[i].lValue > nLevel) return false;
			else return true;
		}
	}
	return true;
}

long CItem::FindApplyValue(BYTE bApplyType)
{
	if (m_pProto == NULL)
		return 0;

	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		if (m_pProto->aApplies[i].bType == bApplyType)
			return m_pProto->aApplies[i].lValue;
	}

	return 0;
}

void CItem::CopySocketTo(LPITEM pItem)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		pItem->m_alSockets[i] = m_alSockets[i];
	}
}

int CItem::GetAccessorySocketGrade()
{
   	return MINMAX(0, GetSocket(0), GetAccessorySocketMaxGrade());
}

int CItem::GetAccessorySocketMaxGrade()
{
   	return MINMAX(0, GetSocket(1), ITEM_ACCESSORY_SOCKET_MAX_NUM);
}

int CItem::GetAccessorySocketDownGradeTime()
{
#ifdef ENABLE_INFINITE_RAFINES
	return GetSocket(2);
#else
	return MINMAX(0, GetSocket(2), aiAccessorySocketDegradeTime[GetAccessorySocketGrade()]);
#endif
}

void CItem::AttrLog()
{
	const char * pszIP = NULL;

	if (GetOwner() && GetOwner()->GetDesc())
		pszIP = GetOwner()->GetDesc()->GetHostName();

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (m_alSockets[i])
		{
			if (g_iDbLogLevel>=LOG_LEVEL_MAX)
				LogManager::instance().ItemLog(i, m_alSockets[i], 0, GetID(), "INFO_SOCKET", "", pszIP ? pszIP : "", GetOriginalVnum());
		}
	}

	for (int i = 0; i<ITEM_ATTRIBUTE_MAX_NUM; ++i)
	{
		int	type	= m_aAttr[i].bType;
		int value	= m_aAttr[i].sValue;

		if (type)
		{
			if (g_iDbLogLevel>=LOG_LEVEL_MAX)
				LogManager::instance().ItemLog(i, type, value, GetID(), "INFO_ATTR", "", pszIP ? pszIP : "", GetOriginalVnum());
		}
	}
}

int CItem::GetLevelLimit()
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		if (this->m_pProto->aLimits[i].bType == LIMIT_LEVEL)
		{
			return this->m_pProto->aLimits[i].lValue;
		}
	}
	return 0;
}

bool CItem::OnAfterCreatedItem()
{
	// 아이템을 한 번이라도 사용했다면, 그 이후엔 사용 중이지 않아도 시간이 차감되는 방식
	if (-1 != this->GetProto()->cLimitRealTimeFirstUseIndex)
	{
		// Socket1에 아이템의 사용 횟수가 기록되어 있으니, 한 번이라도 사용한 아이템은 타이머를 시작한다.
		if (0 != GetSocket(1))
		{
			StartRealTimeExpireEvent();
		}
	}

#ifdef ENABLE_SOUL_SYSTEM
	if(GetType() == ITEM_SOUL)
	{
		StartSoulItemEvent();
	}
#endif

	return true;
}

bool CItem::IsDragonSoul()
{
	return GetType() == ITEM_DS;
}

int CItem::GiveMoreTime_Per(float fPercent)
{
	if (IsDragonSoul())
	{
		DWORD duration = DSManager::instance().GetDuration(this);
		DWORD remain_sec = GetSocket(ITEM_SOCKET_REMAIN_SEC);
		DWORD given_time = fPercent * duration / 100u;
		if (remain_sec == duration)
			return false;
		if ((given_time + remain_sec) >= duration)
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, duration);
			return duration - remain_sec;
		}
		else
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, given_time + remain_sec);
			return given_time;
		}
	}
	// 우선 용혼석에 관해서만 하도록 한다.
	else
		return 0;
}

int CItem::GiveMoreTime_Fix(DWORD dwTime)
{
	if (IsDragonSoul())
	{
		DWORD duration = DSManager::instance().GetDuration(this);
		DWORD remain_sec = GetSocket(ITEM_SOCKET_REMAIN_SEC);
		if (remain_sec == duration)
			return false;
		if ((dwTime + remain_sec) >= duration)
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, duration);
			return duration - remain_sec;
		}
		else
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, dwTime + remain_sec);
			return dwTime;
		}
	}
	// 우선 용혼석에 관해서만 하도록 한다.
	else
		return 0;
}


int	CItem::GetDuration()
{
	if(!GetProto())
		return -1;

	for (int i=0 ; i < ITEM_LIMIT_MAX_NUM ; i++)
	{
		if (LIMIT_REAL_TIME == GetProto()->aLimits[i].bType)
			return GetProto()->aLimits[i].lValue;
	}

	if (GetProto()->cLimitTimerBasedOnWearIndex >= 0)
	{
		BYTE cLTBOWI = GetProto()->cLimitTimerBasedOnWearIndex;
		return GetProto()->aLimits[cLTBOWI].lValue;
	}

	return -1;
}

bool CItem::IsSameSpecialGroup(const LPITEM item) const
{
	// 서로 VNUM이 같다면 같은 그룹인 것으로 간주
	if (this->GetVnum() == item->GetVnum())
		return true;

	if (GetSpecialGroup() && (item->GetSpecialGroup() == GetSpecialGroup()))
		return true;

	return false;
}


#ifdef ENABLE_EXTRA_INVENTORY
bool CItem::IsExtraItem()
{
	uint8_t ret = 0;

	switch (GetVnum()) {
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
		case 30906:
		case 71051:
		case 71052: {
			ret = 1;
			break;
		}
#endif
		case 70612:
		case 70613:
		case 70614:
		case 88968:
		case 30002:
		case 30003:
		case 30004:
		case 30005:
		case 30006:
		case 30015:
		case 30047:
		case 30050:
		case 30165:
		case 30166:
		case 30167:
		case 30168:
		case 30251:
		case 30252: {
			ret = 1;
			break;
		}
		case 30277:
		case 30279:
		case 30284:
		case 86053:
		case 86054:
		case 86055:
		case 70102:
		case 39008:
		case 71001:
		case 72310:
		case 39030:
		case 71094:
#ifdef __NEWPET_SYSTEM__
		case 86077:
		case 86076:
		case 55010:
		case 55011:
		case 55012:
		case 55013:
		case 55014:
		case 55015:
		case 55016:
		case 55017:
		case 55018:
		case 55019:
		case 55020:
		case 55021:
#endif
		case 50513:
		case 50525:
		case 50526:
		case 50527: {
			ret = 2;
			break;
		}
		default:
			break;
	}

	if (ret != 0) {
		return ret == 2 ? true : false;
	}

	switch (GetType()) {
		case ITEM_MATERIAL:
		case ITEM_METIN:
		case ITEM_SKILLBOOK:
		case ITEM_SKILLFORGET:
		case ITEM_GIFTBOX:
		case ITEM_GACHA:
		case ITEM_TREASURE_BOX:
		case ITEM_TREASURE_KEY:
		{
			if (GetType() == ITEM_GIFTBOX && GetValue(3) == SUMMON_MOB_VALUE3_VNUM) {
				return false;
			}

			return true;
		}
		case ITEM_USE:
		{
			BYTE subtype = GetSubType();
			return (subtype == USE_CHANGE_ATTRIBUTE ||
			 subtype == USE_ADD_ATTRIBUTE ||
			 subtype == USE_ADD_ATTRIBUTE2 ||
			 subtype == USE_CHANGE_ATTRIBUTE2 ||
			 subtype == USE_CHANGE_COSTUME_ATTR ||
			 subtype == USE_RESET_COSTUME_ATTR ||
			 subtype == USE_CHANGE_ATTRIBUTE_PLUS ||
#ifdef ATTR_LOCK
			 subtype == USE_ADD_ATTRIBUTE_LOCK ||
			 subtype == USE_CHANGE_ATTRIBUTE_LOCK ||
			 subtype == USE_DELETE_ATTRIBUTE_LOCK ||
#endif
#ifdef ENABLE_ATTR_COSTUMES
			 subtype == USE_CHANGE_ATTR_COSTUME ||
			 subtype == USE_ADD_ATTR_COSTUME1 ||
			 subtype == USE_ADD_ATTR_COSTUME2 ||
			 subtype == USE_REMOVE_ATTR_COSTUME ||
#endif
#ifdef ENABLE_DS_ENCHANT
			 subtype == USE_DS_ENCHANT ||
#endif
#ifdef ENABLE_DS_ENCHANT
			 subtype == USE_ENCHANT_STOLE
#endif
			);
		}
		default:
		{
			break;
		}
	}

	return false;
}


BYTE CItem::GetExtraCategory()
{
	uint8_t ret = 100;

	switch (GetVnum()) {
		case 30277:
		case 30279:
		case 30284:
		case 86053:
		case 86054:
		case 86055: {
			ret = 1;
			break;
		}
		case 70102:
		case 39008:
		case 71001:
		case 72310:
		case 39030:
		case 71094:
#ifdef __NEWPET_SYSTEM__
		case 86077:
		case 86076:
		case 55010:
		case 55011:
		case 55012:
		case 55013:
		case 55014:
		case 55015:
		case 55016:
		case 55017:
		case 55018:
		case 55019:
		case 55020:
		case 55021:
#endif
		case 50513:
		case 50525:
		case 50526:
		case 50527: {
			ret = 0;
			break;
		}
		default: {
			break;
		}
	}

	if (ret != 100) {
		return ret;
	}

	switch (GetType())
	{
		case ITEM_SKILLBOOK:
		case ITEM_SKILLFORGET:
		{
			return 0;
		}
		case ITEM_MATERIAL:
		{
			return 1;
		}
		case ITEM_METIN:
		{
			return 2;
		}
		case ITEM_GIFTBOX:
		case ITEM_GACHA:
		case ITEM_TREASURE_BOX:
		case ITEM_TREASURE_KEY:
		{
			return 3;
		}
		case ITEM_USE:
		{
			BYTE subtype = GetSubType();
			if (subtype == USE_CHANGE_ATTRIBUTE ||
			 subtype == USE_ADD_ATTRIBUTE ||
			 subtype == USE_ADD_ATTRIBUTE2 ||
			 subtype == USE_CHANGE_ATTRIBUTE2 ||
			 subtype == USE_CHANGE_COSTUME_ATTR ||
			 subtype == USE_RESET_COSTUME_ATTR ||
			 subtype == USE_CHANGE_ATTRIBUTE_PLUS ||
#ifdef ATTR_LOCK
			 subtype == USE_ADD_ATTRIBUTE_LOCK ||
			 subtype == USE_CHANGE_ATTRIBUTE_LOCK ||
			 subtype == USE_DELETE_ATTRIBUTE_LOCK ||
#endif
#ifdef ENABLE_ATTR_COSTUMES
			 subtype == USE_CHANGE_ATTR_COSTUME ||
			 subtype == USE_ADD_ATTR_COSTUME1 ||
			 subtype == USE_ADD_ATTR_COSTUME2 ||
			 subtype == USE_REMOVE_ATTR_COSTUME ||
#endif
#ifdef ENABLE_DS_ENCHANT
			 subtype == USE_DS_ENCHANT ||
#endif
#ifdef ENABLE_DS_ENCHANT
			 subtype == USE_ENCHANT_STOLE
#endif
			) {
				return 4;
			} else {
				break;
			}
		}
		default:
		{
			break;
		}
	}

	return 0;
}
#endif

#ifdef ENABLE_SOUL_SYSTEM
EVENTFUNC(soul_item_event)
{
	const item_vid_event_info * pInfo = reinterpret_cast<item_vid_event_info*>(event->info);
	if (!pInfo)
		return 0;
	
	const LPITEM pItem = ITEM_MANAGER::instance().FindByVID(pInfo->item_vid);
	if (!pItem)
		return 0;
	
	int iCurrentMinutes = (pItem->GetSocket(2) / 10000);
	int iCurrentStrike = (pItem->GetSocket(2) % 10000);
	int iNextMinutes = iCurrentMinutes + 1;
	
	if(iNextMinutes >= pItem->GetLimitValue(1))
	{
		if (pItem->GetValue(0) != 1)
		{
			pItem->SetSocket(2, (pItem->GetLimitValue(1) * 10000 + iCurrentStrike)); // just in case
			pItem->SetSoulItemEvent(NULL);
			return 0;
		}
	}
	
	pItem->SetSocket(2, (iNextMinutes * 10000 + iCurrentStrike));
	
	if(test_server)
		return PASSES_PER_SEC(5);
	
	return PASSES_PER_SEC(60);
}

void CItem::SetSoulItemEvent(LPEVENT pkEvent)
{
	m_pkSoulItemEvent = pkEvent;
}

void CItem::StartSoulItemEvent()
{
	if (GetType() != ITEM_SOUL)
		return;

	if (m_pkSoulItemEvent)
		return;

	int iMinutes = (GetSocket(2) / 10000);
	if(iMinutes >= GetLimitValue(1))
		return;

	item_vid_event_info * pInfo = AllocEventInfo<item_vid_event_info>();
	pInfo->item_vid = GetVID();
	SetSoulItemEvent(event_create(soul_item_event, pInfo, PASSES_PER_SEC( test_server ? 5 : 60 )));
}
#endif

#ifdef ENABLE_MULTI_NAMES
const char * CItem::GetName(BYTE Lang) {
	if (Lang == 0) {
		if (m_pOwner) {
			if (m_pOwner->GetDesc()) {
				return m_pProto ? m_pProto->szLocaleName[m_pOwner->GetDesc()->GetLanguage()] : NULL;
			} else {
				return m_pProto ? m_pProto->szLocaleName[1] : NULL;
			}
		} else {
			return m_pProto ? m_pProto->szLocaleName[1] : NULL;
		}
	} else {
		return m_pProto ? m_pProto->szLocaleName[Lang] : NULL;
	}
}
#endif

#if defined(USE_ATTR_6TH_7TH)
uint32_t CItem::GetAttr67MaterialVnum()
{

	return CItemVnumHelper::GetAttr67MaterialVnumByLevel(GetLevelLimit());
}
#endif

#ifdef USE_PICKUP_FILTER
bool CItem::FilterCheckRefine(uint8_t bValue1, uint8_t bValue2)
{
    if (bValue1 == 0 && bValue2 == 200)
    {
        return true;
    }

    int32_t iValue = GetRefineLevel();
    if (iValue < bValue1 || iValue > bValue2)
    {
        return false;
    }

    return true;
}

bool CItem::FilterCheckLevel(uint8_t bValue1, uint8_t bValue2)
{
    if (bValue1 == 0 && bValue2 == 120)
    {
        return true;
    }

    int32_t iValue = GetLevelLimit();
    if (iValue < bValue1 || iValue > bValue2)
    {
        return false;
    }

    return true;
}
#endif
