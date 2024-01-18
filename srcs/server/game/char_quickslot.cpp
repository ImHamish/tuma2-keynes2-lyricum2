#include "stdafx.h"
#include "constants.h"
#include "char.h"
#include "desc.h"
#include "desc_manager.h"
#include "packet.h"
#include "item.h"

/////////////////////////////////////////////////////////////////////////////
// QUICKSLOT HANDLING
/////////////////////////////////////////////////////////////////////////////
void CHARACTER::SyncQuickslot(uint8_t bType, uint16_t bOldPos, uint16_t bNewPos)
{
	if (bOldPos == bNewPos)
	{
		return;
	}

	for (uint8_t i = 0; i < QUICKSLOT_MAX_NUM; ++i)
	{
		if (m_quickslot[i].type == bType && m_quickslot[i].pos == bOldPos)
		{
			if (bNewPos == 65535)
			{
				DelQuickslot(i);
			}
			else
			{
				TQuickSlot slot;
				slot.type = bType;
				slot.pos = bNewPos;

				SetQuickslot(i, slot);
			}
		}
	}
}

bool CHARACTER::GetQuickslot(uint8_t pos, TQuickSlot** ppSlot)
{
	if (pos >= QUICKSLOT_MAX_NUM)
	{
		return false;
	}

	*ppSlot = &m_quickslot[pos];
	return true;
}

bool CHARACTER::SetQuickslot(uint8_t pos, TQuickSlot& rSlot, bool bPacket)
{
	if (bPacket == true && rSlot.type == 12) {
		rSlot.type = QUICKSLOT_TYPE_ITEM_EXTRA;
	}

	TPacketQuickSlotAdd p;

	if (pos >= QUICKSLOT_MAX_NUM)
	{
		return false;
	}

	if (rSlot.type >= QUICKSLOT_TYPE_MAX_NUM)
	{
		return false;
	}

	for (uint8_t i = 0; i < QUICKSLOT_MAX_NUM; ++i)
	{
		if (rSlot.type == 0)
		{
			continue;
		}
		else if (m_quickslot[i].type == rSlot.type && m_quickslot[i].pos == rSlot.pos)
		{
			DelQuickslot(i);
		}
	}

#if defined(ENABLE_EXTRA_INVENTORY)
	uint8_t windowType = rSlot.type == QUICKSLOT_TYPE_ITEM_EXTRA ? EXTRA_INVENTORY : INVENTORY;
#else
	uint8_t windowType = INVENTORY;
#endif

	TItemPos srcCell(windowType, rSlot.pos);

	switch (rSlot.type)
	{
#if defined(ENABLE_EXTRA_INVENTORY)
		case QUICKSLOT_TYPE_ITEM_EXTRA:
			{
				if (rSlot.pos >= EXTRA_INVENTORY_MAX_NUM) {
					return false;
				}

				break;
			}
#endif

		case QUICKSLOT_TYPE_ITEM:
			{
				if (false == srcCell.IsDefaultInventoryPosition() && false == srcCell.IsBeltInventoryPosition()) {
					return false;
				}
			}
			break;
		case QUICKSLOT_TYPE_SKILL:
			if ((int) rSlot.pos >= SKILL_MAX_NUM)
				return false;

			break;

		case QUICKSLOT_TYPE_COMMAND:
			break;

		default:
			return false;
	}

	m_quickslot[pos] = rSlot;

	const LPDESC d = GetDesc();

	if (d)
	{
		p.header = HEADER_GC_QUICKSLOT_ADD;
		p.pos = pos;
		p.slot = m_quickslot[pos];

		d->Packet(&p, sizeof(p));
	}

	return true;
}

bool CHARACTER::DelQuickslot(BYTE pos)
{
	if (pos >= QUICKSLOT_MAX_NUM)
		return false;

	memset(&m_quickslot[pos], 0, sizeof(TQuickSlot));

	const LPDESC d = GetDesc();

	if (d)
	{
		TPacketQuickSlotDel p;
		p.header = HEADER_GC_QUICKSLOT_DEL;
		p.pos = pos;

		d->Packet(&p, sizeof(TPacketQuickSlotDel));
	}

	return true;
}

bool CHARACTER::SwapQuickslot(BYTE a, BYTE b)
{
	TQuickSlot quickslot;

	if (a >= QUICKSLOT_MAX_NUM || b >= QUICKSLOT_MAX_NUM)
		return false;

	quickslot = m_quickslot[a];

	m_quickslot[a] = m_quickslot[b];
	m_quickslot[b] = quickslot;

	const LPDESC d = GetDesc();
	if (d) {
		TPacketQuickSlotSwap p;
		p.header = HEADER_GC_QUICKSLOT_SWAP;
		p.pos = a;
		p.change_pos = b;

		d->Packet(&p, sizeof(TPacketQuickSlotSwap));
	}

	return true;
}

void CHARACTER::ChainQuickslotItem(LPITEM pItem, BYTE bType, BYTE bOldPos)
{
	if (pItem->IsDragonSoul())
		return;
	for ( int i=0; i < QUICKSLOT_MAX_NUM; ++i )
	{
		if ( m_quickslot[i].type == bType && m_quickslot[i].pos == bOldPos )
		{
			TQuickSlot slot;
			slot.type = bType;
			slot.pos = pItem->GetCell();

			SetQuickslot(i, slot);

			break;
		}
	}
}

