#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "PythonItem.h"
#include "PythonShop.h"
#include "PythonExchange.h"
#include "PythonSafeBox.h"
#include "PythonCharacterManager.h"

#include "AbstractPlayer.h"
#if defined(ENABLE_GAYA_RENEWAL)
#include "PythonPlayer.h"
#endif
#include "../GameLib/ItemManager.h"

//////////////////////////////////////////////////////////////////////////
// SafeBox

bool CPythonNetworkStream::SendSafeBoxMoneyPacket(BYTE byState, DWORD dwMoney)
{
	assert(!"CPythonNetworkStream::SendSafeBoxMoneyPacket - Don't use this function");
	return false;

//	TPacketCGSafeboxMoney kSafeboxMoney;
//	kSafeboxMoney.bHeader = HEADER_CG_SAFEBOX_MONEY;
//	kSafeboxMoney.bState = byState;
//	kSafeboxMoney.dwMoney = dwMoney;
//	if (!Send(sizeof(kSafeboxMoney), &kSafeboxMoney))
//		return false;
//
//	return SendSequence();
}

#if defined(ITEM_CHECKINOUT_UPDATE)
bool CPythonNetworkStream::SendSafeBoxCheckinPacket(TItemPos InventoryPos, BYTE bySafeBoxPos, bool SelectPosAuto)
#else
bool CPythonNetworkStream::SendSafeBoxCheckinPacket(TItemPos InventoryPos, BYTE bySafeBoxPos)
#endif
{
	__PlayInventoryItemDropSound(InventoryPos);

	TPacketCGSafeboxCheckin kSafeboxCheckin;
	kSafeboxCheckin.bHeader = HEADER_CG_SAFEBOX_CHECKIN;
	kSafeboxCheckin.ItemPos = InventoryPos;
	kSafeboxCheckin.bSafePos = bySafeBoxPos;
#if defined(ITEM_CHECKINOUT_UPDATE)
	kSafeboxCheckin.SelectPosAuto = SelectPosAuto;
#endif
	if (!Send(sizeof(kSafeboxCheckin), &kSafeboxCheckin))
		return false;

	return SendSequence();
}

#if defined(ITEM_CHECKINOUT_UPDATE)
bool CPythonNetworkStream::SendSafeBoxCheckoutPacket(BYTE bySafeBoxPos, TItemPos InventoryPos, bool SelectPosAuto)
#else
bool CPythonNetworkStream::SendSafeBoxCheckoutPacket(BYTE bySafeBoxPos, TItemPos InventoryPos)
#endif
{
	__PlaySafeBoxItemDropSound(bySafeBoxPos);

	TPacketCGSafeboxCheckout kSafeboxCheckout;
	kSafeboxCheckout.bHeader = HEADER_CG_SAFEBOX_CHECKOUT;
	kSafeboxCheckout.bSafePos = bySafeBoxPos;
	kSafeboxCheckout.ItemPos = InventoryPos;
#if defined(ITEM_CHECKINOUT_UPDATE)
	kSafeboxCheckout.SelectPosAuto = SelectPosAuto;
#endif
	if (!Send(sizeof(kSafeboxCheckout), &kSafeboxCheckout))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendSafeBoxItemMovePacket(BYTE bySourcePos, BYTE byTargetPos, WORD byCount)
{
	__PlaySafeBoxItemDropSound(bySourcePos);

	TPacketCGItemMove kItemMove;
	kItemMove.header = HEADER_CG_SAFEBOX_ITEM_MOVE;
	kItemMove.pos = TItemPos(INVENTORY, bySourcePos);
	kItemMove.count = byCount;
	kItemMove.change_pos = TItemPos(INVENTORY, byTargetPos);
	if (!Send(sizeof(kItemMove), &kItemMove))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvSafeBoxSetPacket()
{
	TPacketGCItemSet2 kItemSet;
	if (!Recv(sizeof(kItemSet), &kItemSet))
		return false;

	TItemData kItemData;
	kItemData.vnum	= kItemSet.vnum;
	kItemData.count = kItemSet.count;
#if defined(ENABLE_CHANGELOOK)
	kItemData.transmutation = kItemSet.transmutation;
#endif
#ifdef ATTR_LOCK
	kItemData.lockedattr = kItemSet.lockedattr;
#endif
	kItemData.flags = kItemSet.flags;
	kItemData.anti_flags = kItemSet.anti_flags;

	for (int isocket=0; isocket<ITEM_SOCKET_SLOT_MAX_NUM; ++isocket) {
		kItemData.alSockets[isocket] = kItemSet.alSockets[isocket];
	}
	for (int iattr=0; iattr<ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++iattr) {
		kItemData.aAttr[iattr] = kItemSet.aAttr[iattr];
	}

	CPythonSafeBox::Instance().SetItemData(kItemSet.Cell.cell, kItemData);

	__RefreshSafeboxWindow();

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxDelPacket()
{
	TPacketGCItemDel kItemDel;
	if (!Recv(sizeof(kItemDel), &kItemDel))
		return false;

	CPythonSafeBox::Instance().DelItemData(kItemDel.pos);

	__RefreshSafeboxWindow();

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxWrongPasswordPacket()
{
	TPacketGCSafeboxWrongPassword kSafeboxWrongPassword;

	if (!Recv(sizeof(kSafeboxWrongPassword), &kSafeboxWrongPassword))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnSafeBoxError", Py_BuildValue("()"));

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxSizePacket()
{
	TPacketGCSafeboxSize kSafeBoxSize;
	if (!Recv(sizeof(kSafeBoxSize), &kSafeBoxSize))
		return false;

	CPythonSafeBox::Instance().OpenSafeBox(kSafeBoxSize.bSize);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenSafeboxWindow", Py_BuildValue("(i)", kSafeBoxSize.bSize));

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxMoneyChangePacket()
{
	TPacketGCSafeboxMoneyChange kMoneyChange;
	if (!Recv(sizeof(kMoneyChange), &kMoneyChange))
		return false;

	CPythonSafeBox::Instance().SetMoney(kMoneyChange.dwMoney);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSafeboxMoney", Py_BuildValue("()"));

	return true;
}

// SafeBox
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Mall
#if defined(ITEM_CHECKINOUT_UPDATE)
bool CPythonNetworkStream::SendMallCheckoutPacket(BYTE byMallPos, TItemPos InventoryPos, bool SelectPosAuto)
#else
bool CPythonNetworkStream::SendMallCheckoutPacket(BYTE byMallPos, TItemPos InventoryPos)
#endif
{
	__PlayMallItemDropSound(byMallPos);

	TPacketCGMallCheckout kMallCheckoutPacket;
	kMallCheckoutPacket.bHeader = HEADER_CG_MALL_CHECKOUT;
	kMallCheckoutPacket.bMallPos = byMallPos;
	kMallCheckoutPacket.ItemPos = InventoryPos;
#if defined(ITEM_CHECKINOUT_UPDATE)
	kMallCheckoutPacket.SelectPosAuto = SelectPosAuto;
#endif
	if (!Send(sizeof(kMallCheckoutPacket), &kMallCheckoutPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvMallOpenPacket()
{
	TPacketGCMallOpen kMallOpen;
	if (!Recv(sizeof(kMallOpen), &kMallOpen))
		return false;

	CPythonSafeBox::Instance().OpenMall(kMallOpen.bSize);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenMallWindow", Py_BuildValue("(i)", kMallOpen.bSize));

	return true;
}
bool CPythonNetworkStream::RecvMallItemSetPacket()
{
	TPacketGCItemSet2 kItemSet;
	if (!Recv(sizeof(kItemSet), &kItemSet))
		return false;

	TItemData kItemData;
	kItemData.vnum = kItemSet.vnum;
	kItemData.count = kItemSet.count;
#if defined(ENABLE_CHANGELOOK)
	kItemData.transmutation = kItemSet.transmutation;
#endif
#ifdef ATTR_LOCK
	kItemData.lockedattr = kItemSet.lockedattr;
#endif
	kItemData.flags = kItemSet.flags;
	kItemData.anti_flags = kItemSet.anti_flags;

	for (int isocket=0; isocket<ITEM_SOCKET_SLOT_MAX_NUM; ++isocket)
		kItemData.alSockets[isocket] = kItemSet.alSockets[isocket];
	for (int iattr=0; iattr<ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++iattr)
		kItemData.aAttr[iattr] = kItemSet.aAttr[iattr];

	CPythonSafeBox::Instance().SetMallItemData(kItemSet.Cell.cell, kItemData);

	__RefreshMallWindow();

	return true;
}
bool CPythonNetworkStream::RecvMallItemDelPacket()
{
	TPacketGCItemDel kItemDel;
	if (!Recv(sizeof(kItemDel), &kItemDel))
		return false;

	CPythonSafeBox::Instance().DelMallItemData(kItemDel.pos);

	__RefreshMallWindow();
	Tracef(" >> CPythonNetworkStream::RecvMallItemDelPacket\n");

	return true;
}
// Mall
//////////////////////////////////////////////////////////////////////////

// Item
// Recieve
bool CPythonNetworkStream::RecvItemSetPacket()
{
	TPacketGCItemSet packet_item_set;

	if (!Recv(sizeof(TPacketGCItemSet), &packet_item_set))
		return false;

	TItemData kItemData;
	kItemData.vnum	= packet_item_set.vnum;
	kItemData.count	= packet_item_set.count;
#if defined(ENABLE_CHANGELOOK)
	kItemData.transmutation = packet_item_set.transmutation;
#endif
#ifdef ATTR_LOCK
	kItemData.lockedattr = packet_item_set.lockedattr;
#endif
	kItemData.flags = 0;
	for (int i=0; i<ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		kItemData.alSockets[i]=packet_item_set.alSockets[i];
	for (int j=0; j<ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		kItemData.aAttr[j]=packet_item_set.aAttr[j];

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.SetItemData(packet_item_set.Cell, kItemData);
	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemSetPacket2()
{
	TPacketGCItemSet2 packet_item_set;

	if (!Recv(sizeof(TPacketGCItemSet2), &packet_item_set))
		return false;

	TItemData kItemData;
	kItemData.vnum	= packet_item_set.vnum;
	kItemData.count	= packet_item_set.count;
#if defined(ENABLE_CHANGELOOK)
	kItemData.transmutation = packet_item_set.transmutation;
#endif
#ifdef ATTR_LOCK
	kItemData.lockedattr = packet_item_set.lockedattr;
#endif
	kItemData.flags = packet_item_set.flags;
	kItemData.anti_flags = packet_item_set.anti_flags;

	for (int i=0; i<ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		kItemData.alSockets[i]=packet_item_set.alSockets[i];
	for (int j=0; j<ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		kItemData.aAttr[j]=packet_item_set.aAttr[j];

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.SetItemData(packet_item_set.Cell, kItemData);
	if (packet_item_set.highlight)
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Highlight_Item", Py_BuildValue("(ii)", packet_item_set.Cell.window_type, packet_item_set.Cell.cell));

	__RefreshInventoryWindow();
	return true;
}


bool CPythonNetworkStream::RecvItemUsePacket()
{
	TPacketGCItemUse packet_item_use;

	if (!Recv(sizeof(TPacketGCItemUse), &packet_item_use))
		return false;

	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemUpdatePacket()
{
	TPacketGCItemUpdate packet_item_update;

	if (!Recv(sizeof(TPacketGCItemUpdate), &packet_item_update))
		return false;

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.SetItemCount(packet_item_update.Cell, packet_item_update.count);
#if defined(ENABLE_CHANGELOOK)
	rkPlayer.SetItemTransmutation(packet_item_update.Cell, packet_item_update.transmutation);
#endif
#ifdef ATTR_LOCK
	rkPlayer.SetItemAttrLocked(packet_item_update.Cell, packet_item_update.lockedattr);
#endif 

	for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		rkPlayer.SetItemMetinSocket(packet_item_update.Cell, i, packet_item_update.alSockets[i]);
	for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		rkPlayer.SetItemAttribute(packet_item_update.Cell, j, packet_item_update.aAttr[j].bType, packet_item_update.aAttr[j].sValue);

	if (packet_item_update.Cell.window_type == INVENTORY) {
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RefreshInventoryCell", Py_BuildValue("(i)", packet_item_update.Cell.cell));
	} else {
		__RefreshInventoryWindow();
	}

	return true;
}

bool CPythonNetworkStream::RecvItemGroundAddPacket()
{
	TPacketGCItemGroundAdd packet_item_ground_add;

	if (!Recv(sizeof(TPacketGCItemGroundAdd), &packet_item_ground_add))
		return false;

	__GlobalPositionToLocalPosition(packet_item_ground_add.lX, packet_item_ground_add.lY);

	CPythonItem::Instance().CreateItem(packet_item_ground_add.dwVID,
									   packet_item_ground_add.dwVnum,
									   packet_item_ground_add.lX,
									   packet_item_ground_add.lY,
									   packet_item_ground_add.lZ);
	return true;
}


bool CPythonNetworkStream::RecvItemOwnership()
{
	TPacketGCItemOwnership p;

	if (!Recv(sizeof(TPacketGCItemOwnership), &p))
		return false;

	CPythonItem::Instance().SetOwnership(p.dwVID, p.szName);
	return true;
}

bool CPythonNetworkStream::RecvItemGroundDelPacket()
{
	TPacketGCItemGroundDel	packet_item_ground_del;

	if (!Recv(sizeof(TPacketGCItemGroundDel), &packet_item_ground_del))
		return false;

	CPythonItem::Instance().DeleteItem(packet_item_ground_del.vid);
	return true;
}

bool CPythonNetworkStream::RecvQuickSlotAddPacket()
{
	TPacketQuickSlotAdd p;
	if (!Recv(sizeof(TPacketQuickSlotAdd), &p))
	{
		return false;
	}

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.AddQuickSlot(p.pos, p.slot.type, p.slot.pos);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvQuickSlotDelPacket()
{
	TPacketQuickSlotDel p;
	if (!Recv(sizeof(TPacketQuickSlotDel), &p))
	{
		return false;
	}

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.DeleteQuickSlot(p.pos);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvQuickSlotMovePacket()
{
	TPacketQuickSlotSwap p;
	if (!Recv(sizeof(TPacketQuickSlotSwap), &p))
	{
		return false;
	}

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.MoveQuickSlot(p.pos, p.change_pos);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::SendShopEndPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop packet_shop;
	packet_shop.header = HEADER_CG_SHOP;
	packet_shop.subheader = SHOP_SUBHEADER_CG_END;

	if (!Send(sizeof(packet_shop), &packet_shop))
	{
		Tracef("SendShopEndPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendShopBuyPacket(BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_BUY;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendShopBuyPacket Error\n");
		return false;
	}

	BYTE bCount=1;
	if (!Send(sizeof(BYTE), &bCount))
	{
		Tracef("SendShopBuyPacket Error\n");
		return false;
	}

	if (!Send(sizeof(BYTE), &bPos))
	{
		Tracef("SendShopBuyPacket Error\n");
		return false;
	}

	return SendSequence();
}

#ifdef ENABLE_BUY_STACK_FROM_SHOP
bool CPythonNetworkStream::SendShopBuyMultiplePacket(uint8_t p, uint8_t c) {
	if (!__CanActMainInstance()) {
		return true;
	}

	if (p < 0 || c <= 0 || c > MULTIPLE_BUY_LIMIT) {
		return true;
	}

	TPacketCGShop pack;
	pack.header = HEADER_CG_SHOP;
	pack.subheader = SHOP_SUBHEADER_CG_BUY2;

	if (!Send(sizeof(TPacketCGShop), &pack)) {
		Tracef("SendShopBuyMultiplePacket Error\n");
		return false;
	}

	if (!Send(sizeof(uint8_t), &p)) {
		Tracef("SendShopBuyMultiplePacket Error\n");
		return false;
	}

	if (!Send(sizeof(uint8_t), &c)) {
		Tracef("SendShopBuyMultiplePacket Error\n");
		return false;
	}

	return SendSequence();
}
#endif

bool CPythonNetworkStream::SendShopSellPacket(BYTE bySlot)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_SELL;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendShopSellPacket Error\n");
		return false;
	}
	if (!Send(sizeof(BYTE), &bySlot))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}

	return SendSequence();
}

#ifdef ENABLE_EXTRA_INVENTORY
bool CPythonNetworkStream::SendShopSellPacketNew(TItemPos pos, WORD byCount)
#else
bool CPythonNetworkStream::SendShopSellPacketNew(BYTE bySlot, WORD byCount)
#endif
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGShop PacketShop;
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_SELL2;

	if (!Send(sizeof(TPacketCGShop), &PacketShop))
	{
		Tracef("SendShopSellPacket Error\n");
		return false;
	}
#ifdef ENABLE_EXTRA_INVENTORY
	if (!Send(sizeof(BYTE), &pos.window_type))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}
	if (!Send(sizeof(WORD), &pos.cell))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}
#else
	if (!Send(sizeof(BYTE), &bySlot))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}
#endif

	WORD wCount = byCount > 2000 ? 2000 : byCount;
	if (!Send(sizeof(WORD), &wCount))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}

#ifdef ENABLE_EXTRA_INVENTORY
	Tracef(" SendShopSellPacketNew(type=%d, bySlot=%d, byCount=%d)\n", pos.window_type, pos.cell, byCount);
#else
	Tracef(" SendShopSellPacketNew(bySlot=%d, byCount=%d)\n", bySlot, byCount);
#endif

	return SendSequence();
}

bool CPythonNetworkStream::CheckCannotWhileAttacking(TItemPos itemPos)
{
	if (!__IsPlayerAttacking())
	{
		return false;
	}

	const TItemData * pPlayerItem = CPythonPlayer::Instance().GetItemData(itemPos);
	if (pPlayerItem)
	{
		CItemData* pItemData = NULL;
		if (CItemManager::Instance().GetItemDataPointer(pPlayerItem->vnum, &pItemData))
		{
			bool bRet;

			uint8_t itemType = pItemData->GetType();
			switch (itemType)
			{
				case CItemData::ITEM_TYPE_ARMOR:
				case CItemData::ITEM_TYPE_WEAPON:
				case CItemData::ITEM_TYPE_BELT:
				{
					bRet = true;
					break;
				}
				case CItemData::ITEM_TYPE_COSTUME:
				{
#if defined(USE_MOUNT_COSTUME_SYSTEM)
					bRet = pItemData->GetSubType() == CItemData::COSTUME_MOUNT ? false : true;
#else
					bRet = true;
#endif
					break;
				}
				default:
				{
					bRet = false;
					break;
				}
			}

			if (bRet == true)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_WHILE_ATTACKING"));
				return true;
			}
		}
	}

	return false;
}

// Send
bool CPythonNetworkStream::SendItemUsePacket(TItemPos pos)
{
	if (!__CanActMainInstance())
		return true;

	if (__IsEquipItemInSlot(pos))
	{
		if (CPythonExchange::Instance().isTrading())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_EXCHANGE"));
			return true;
		}

		if (CPythonShop::Instance().IsOpen())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_SHOP"));
			return true;
		}

		if (__IsPlayerAttacking())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_WHILE_ATTACKING"));
			return true;
		}
	}
	else if (CheckCannotWhileAttacking(pos))
	{
		return true;
	}

	__PlayInventoryItemUseSound(pos);

	TPacketCGItemUse itemUsePacket;
	itemUsePacket.header = HEADER_CG_ITEM_USE;
	itemUsePacket.pos = pos;

	if (!Send(sizeof(TPacketCGItemUse), &itemUsePacket))
	{
		Tracen("SendItemUsePacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendItemUseToItemPacket(TItemPos source_pos, TItemPos target_pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemUseToItem itemUseToItemPacket;
	itemUseToItemPacket.header = HEADER_CG_ITEM_USE_TO_ITEM;
	itemUseToItemPacket.source_pos = source_pos;
	itemUseToItemPacket.target_pos = target_pos;

	if (!Send(sizeof(TPacketCGItemUseToItem), &itemUseToItemPacket))
	{
		Tracen("SendItemUseToItemPacket Error");
		return false;
	}

#ifdef _DEBUG
	Tracef(" << SendItemUseToItemPacket(src=%d, dst=%d)\n", source_pos, target_pos);
#endif

	return SendSequence();
}

bool CPythonNetworkStream::SendItemDropPacket(TItemPos pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemDrop itemDropPacket;
	itemDropPacket.header = HEADER_CG_ITEM_DROP;
	itemDropPacket.pos = pos;

	if (!Send(sizeof(TPacketCGItemDrop), &itemDropPacket))
	{
		Tracen("SendItemDropPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendItemDestroyPacket(TItemPos pos)
{
	if (!__CanActMainInstance())
		return true;
	
	TPacketCGItemDestroy itemDestroyPacket;
	itemDestroyPacket.header = HEADER_CG_ITEM_DESTROY;
	itemDestroyPacket.pos = pos;
	if (!Send(sizeof(itemDestroyPacket), &itemDestroyPacket))
	{
		Tracen("SendItemDestroyPacket Error");
		return false;
	}
	
	return SendSequence();
}

bool CPythonNetworkStream::SendItemDivisionPacket(TItemPos pos)
{
	if (!__CanActMainInstance())
		return true;
	
	TPacketCGItemDivision p;
	p.header = HEADER_CG_ITEM_DIVISION;
	p.pos = pos;
	if (!Send(sizeof(p), &p))
	{
		Tracen("SendItemDivisionPacket Error");
		return false;
	}
	
	return SendSequence();
}

bool CPythonNetworkStream::SendItemDropPacketNew(TItemPos pos, DWORD count)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemDrop2 itemDropPacket;
	itemDropPacket.header = HEADER_CG_ITEM_DROP2;
	itemDropPacket.pos = pos;
	itemDropPacket.count = count;

	if (!Send(sizeof(itemDropPacket), &itemDropPacket))
	{
		Tracen("SendItemDropPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::__IsEquipItemInSlot(TItemPos uSlotPos)
{
	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	return rkPlayer.IsEquipItemInSlot(uSlotPos);
}

void CPythonNetworkStream::__PlayInventoryItemUseSound(TItemPos uSlotPos)
{
	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	DWORD dwItemID=rkPlayer.GetItemIndex(uSlotPos);

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.PlayUseSound(dwItemID);
}

void CPythonNetworkStream::__PlayInventoryItemDropSound(TItemPos uSlotPos)
{
	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	DWORD dwItemID=rkPlayer.GetItemIndex(uSlotPos);

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

//void CPythonNetworkStream::__PlayShopItemDropSound(UINT uSlotPos)
//{
//	DWORD dwItemID;
//	CPythonShop& rkShop=CPythonShop::Instance();
//	if (!rkShop.GetSlotItemID(uSlotPos, &dwItemID))
//		return;
//
//	CPythonItem& rkItem=CPythonItem::Instance();
//	rkItem.PlayDropSound(dwItemID);
//}

void CPythonNetworkStream::__PlaySafeBoxItemDropSound(UINT uSlotPos)
{
	DWORD dwItemID;
	CPythonSafeBox& rkSafeBox=CPythonSafeBox::Instance();
	if (!rkSafeBox.GetSlotItemID(uSlotPos, &dwItemID))
		return;

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

void CPythonNetworkStream::__PlayMallItemDropSound(UINT uSlotPos)
{
	DWORD dwItemID;
	CPythonSafeBox& rkSafeBox=CPythonSafeBox::Instance();
	if (!rkSafeBox.GetSlotMallItemID(uSlotPos, &dwItemID))
		return;

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

bool CPythonNetworkStream::SendItemMovePacket(TItemPos pos, TItemPos change_pos, WORD num)
{
	if (!__CanActMainInstance())
		return true;

	if (__IsEquipItemInSlot(pos))
	{
		if (CPythonExchange::Instance().isTrading())
		{
			if (pos.IsEquipCell() || change_pos.IsEquipCell())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_EXCHANGE"));
				return true;
			}
		}

		if (CPythonShop::Instance().IsOpen())
		{
			if (pos.IsEquipCell() || change_pos.IsEquipCell())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_SHOP"));
				return true;
			}
		}

		if (__IsPlayerAttacking())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_WHILE_ATTACKING"));
			return true;
		}
	}
	else if (CheckCannotWhileAttacking(pos))
	{
		return true;
	}

	__PlayInventoryItemDropSound(pos);

	TPacketCGItemMove	itemMovePacket;
	itemMovePacket.header = HEADER_CG_ITEM_MOVE;
	itemMovePacket.pos = pos;
	itemMovePacket.change_pos = change_pos;
	itemMovePacket.count = num;

	if (!Send(sizeof(TPacketCGItemMove), &itemMovePacket))
	{
		Tracen("SendItemMovePacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendItemPickUpPacket(DWORD vid)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemPickUp	itemPickUpPacket;
	itemPickUpPacket.header = HEADER_CG_ITEM_PICKUP;
	itemPickUpPacket.vid = vid;

	if (!Send(sizeof(TPacketCGItemPickUp), &itemPickUpPacket))
	{
		Tracen("SendItemPickUpPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuickSlotAddPacket(uint8_t wpos, uint8_t type, uint16_t pos)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketQuickSlotAdd p;

	p.header = HEADER_CG_QUICKSLOT_ADD;
	p.pos = wpos;
	p.slot.type = type;
	p.slot.pos = pos;

	if (!Send(sizeof(TPacketQuickSlotAdd), &p))
	{
		Tracen("SendQuickSlotAddPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuickSlotDelPacket(uint8_t pos)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketQuickSlotDel p;
	p.header = HEADER_CG_QUICKSLOT_DEL;
	p.pos = pos;

	if (!Send(sizeof(TPacketQuickSlotDel), &p))
	{
		Tracen("SendQuickSlotDelPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuickSlotMovePacket(uint8_t pos, uint8_t change_pos)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketQuickSlotSwap p;
	p.header = HEADER_CG_QUICKSLOT_SWAP;
	p.pos = pos;
	p.change_pos = change_pos;

	if (!Send(sizeof(TPacketQuickSlotSwap), &p))
	{
		Tracen("SendQuickSlotSwapPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvSpecialEffect()
{
	TPacketGCSpecialEffect kSpecialEffect;
	if (!Recv(sizeof(kSpecialEffect), &kSpecialEffect))
		return false;

	DWORD effect = -1;
	bool bPlayPotionSound = false;	//포션을 먹을 경우는 포션 사운드를 출력하자.!!
	bool bAttachEffect = true;		//캐리터에 붙는 어태치 이펙트와 일반 이펙트 구분.!!
	switch (kSpecialEffect.type)
	{
		case SE_HPUP_RED:
			effect = CInstanceBase::EFFECT_HPUP_RED;
			bPlayPotionSound = true;
			break;
		case SE_SPUP_BLUE:
			effect = CInstanceBase::EFFECT_SPUP_BLUE;
			bPlayPotionSound = true;
			break;
		case SE_SPEEDUP_GREEN:
			effect = CInstanceBase::EFFECT_SPEEDUP_GREEN;
			bPlayPotionSound = true;
			break;
		case SE_DXUP_PURPLE:
			effect = CInstanceBase::EFFECT_DXUP_PURPLE;
			bPlayPotionSound = true;
			break;
		case SE_CRITICAL:
			effect = CInstanceBase::EFFECT_CRITICAL;
			break;
		case SE_PENETRATE:
			effect = CInstanceBase::EFFECT_PENETRATE;
			break;
		case SE_BLOCK:
			effect = CInstanceBase::EFFECT_BLOCK;
			break;
		case SE_DODGE:
			effect = CInstanceBase::EFFECT_DODGE;
			break;
		case SE_CHINA_FIREWORK:
			effect = CInstanceBase::EFFECT_FIRECRACKER;
			bAttachEffect = false;
			break;
		case SE_SPIN_TOP:
			effect = CInstanceBase::EFFECT_SPIN_TOP;
			bAttachEffect = false;
			break;
		case SE_SUCCESS :
			effect = CInstanceBase::EFFECT_SUCCESS ;
			bAttachEffect = false ;
			break ;
		case SE_FAIL :
			effect = CInstanceBase::EFFECT_FAIL ;
			break ;
		case SE_FR_SUCCESS:
			effect = CInstanceBase::EFFECT_FR_SUCCESS;
			bAttachEffect = false ;
			break;
		case SE_LEVELUP_ON_14_FOR_GERMANY:	//레벨업 14일때 ( 독일전용 )
			effect = CInstanceBase::EFFECT_LEVELUP_ON_14_FOR_GERMANY;
			bAttachEffect = false ;
			break;
		case SE_LEVELUP_UNDER_15_FOR_GERMANY: //레벨업 15일때 ( 독일전용 )
			effect = CInstanceBase::EFFECT_LEVELUP_UNDER_15_FOR_GERMANY;
			bAttachEffect = false ;
			break;
		case SE_PERCENT_DAMAGE1:
			effect = CInstanceBase::EFFECT_PERCENT_DAMAGE1;
			break;
		case SE_PERCENT_DAMAGE2:
			effect = CInstanceBase::EFFECT_PERCENT_DAMAGE2;
			break;
		case SE_PERCENT_DAMAGE3:
			effect = CInstanceBase::EFFECT_PERCENT_DAMAGE3;
			break;
		case SE_AUTO_HPUP:
			effect = CInstanceBase::EFFECT_AUTO_HPUP;
			break;
		case SE_AUTO_SPUP:
			effect = CInstanceBase::EFFECT_AUTO_SPUP;
			break;
		case SE_EQUIP_RAMADAN_RING:
			effect = CInstanceBase::EFFECT_RAMADAN_RING_EQUIP;
			break;
		case SE_EQUIP_HALLOWEEN_CANDY:
			effect = CInstanceBase::EFFECT_HALLOWEEN_CANDY_EQUIP;
			break;
		case SE_EQUIP_HAPPINESS_RING:
 			effect = CInstanceBase::EFFECT_HAPPINESS_RING_EQUIP;
			break;
		case SE_EQUIP_LOVE_PENDANT:
			effect = CInstanceBase::EFFECT_LOVE_PENDANT_EQUIP;
			break;
#ifdef __EFFETTO_MANTELLO__
		case SE_MANTELLO:
			effect = CInstanceBase::EFFECT_MANTELLO;
			break;
#endif

#ifdef ENABLE_ACCE_SYSTEM
		case SE_EFFECT_ACCE_SUCCEDED:
			effect = CInstanceBase::EFFECT_ACCE_SUCCEDED;
			break;

		case SE_EFFECT_ACCE_EQUIP:
			effect = CInstanceBase::EFFECT_ACCE_EQUIP;
			break;
#endif
#ifdef VERSION_162_ENABLED
		case SE_EFFECT_HEALER:
			effect = CInstanceBase::EFFECT_HEALER;
			break;
#endif
#if defined(USE_BATTLEPASS)
		case SE_EFFECT_BP_MISSION_COMPLETED:
		{
			effect = CInstanceBase::EFFECT_BP_MISSION_COMPLETED;
			break;
		}
		case SE_EFFECT_BP_COMPLETED:
		{
			effect = CInstanceBase::EFFECT_BP_COMPLETED;
			break;
		}
#endif
		default:
			TraceError("%d is not a special effect number. TPacketGCSpecialEffect",kSpecialEffect.type);
			break;
	}

	if (bPlayPotionSound)
	{
		IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
		if(rkPlayer.IsMainCharacterIndex(kSpecialEffect.vid))
		{
			CPythonItem& rkItem=CPythonItem::Instance();
			rkItem.PlayUsePotionSound();
		}
	}

	if (-1 != effect)
	{
		CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kSpecialEffect.vid);
		if (pInstance)
		{
			if(bAttachEffect)
				pInstance->AttachSpecialEffect(effect);
			else
				pInstance->CreateSpecialEffect(effect);
		}
	}

	return true;
}


bool CPythonNetworkStream::RecvSpecificEffect()
{
	TPacketGCSpecificEffect kSpecificEffect;
	if (!Recv(sizeof(kSpecificEffect), &kSpecificEffect))
		return false;

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kSpecificEffect.vid);
	//EFFECT_TEMP
	if (pInstance)
	{
		CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_TEMP, "", kSpecificEffect.effect_file, false);
		pInstance->AttachSpecialEffect(CInstanceBase::EFFECT_TEMP);
	}

	return true;
}

bool CPythonNetworkStream::RecvDragonSoulRefine()
{
	TPacketGCDragonSoulRefine kDragonSoul;

	if (!Recv(sizeof(kDragonSoul), &kDragonSoul))
		return false;


	switch (kDragonSoul.bSubType)
	{
	case DS_SUB_HEADER_OPEN:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_Open", Py_BuildValue("()"));
		break;
	case DS_SUB_HEADER_REFINE_FAIL:
	case DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE:
	case DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL:
	case DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY:
	case DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL:
	case DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_RefineFail", Py_BuildValue("(iii)",
			kDragonSoul.bSubType, kDragonSoul.Pos.window_type, kDragonSoul.Pos.cell));
		break;
	case DS_SUB_HEADER_REFINE_SUCCEED:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_RefineSucceed",
				Py_BuildValue("(ii)", kDragonSoul.Pos.window_type, kDragonSoul.Pos.cell));
		break;
	}

	return true;
}

#ifdef ENABLE_SWITCHBOT_WORLDARD
bool CPythonNetworkStream::RecvSwitchBot()
{
	TPacketGCSwitchBotReceive KSwitchBot;

	if (!Recv(sizeof(KSwitchBot), &KSwitchBot))
		return false;

	switch (KSwitchBot.bSubType)
	{
		case SWITCH_RECEIVE_OPEN:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SWITCHBOT_OPEN", Py_BuildValue("()"));
			break;

		case SWITCH_RECEIVE_CLEAR_INFO:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SWITCHBOT_CLEAR_INFO", Py_BuildValue("()"));
			break;

		case SWITCH_RECEIVE_INFO_BONUS_ITEM:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SWITCHBOT_INFO_BONUS", Py_BuildValue("(iiiiii)",KSwitchBot.id_bonus, KSwitchBot.value_bonus_0, KSwitchBot.value_bonus_1, KSwitchBot.value_bonus_2, KSwitchBot.value_bonus_3, KSwitchBot.value_bonus_4));
			break;

		case SWITCH_RECEIVE_INFO_EXTRA:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SWITCHBOT_INFO_EXTRA", Py_BuildValue("()"));
			break;
	}

	return true;

}
#endif

#if defined(ENABLE_GAYA_RENEWAL)
bool CPythonNetworkStream::RecvGemShopOpen()
{
	TPacketGCGemShopOpen kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
		return false;
	
	CPythonPlayer::Instance().ClearGemShopItemVector();
	for (int i = 0; i < 9; ++i)
	{
		TGemShopItem GemItem;
		GemItem.slotIndex = kPacket.shopItems[i].slotIndex;
		GemItem.status = kPacket.shopItems[i].status;
		
		GemItem.dwVnum = kPacket.shopItems[i].dwVnum;
		GemItem.bCount = kPacket.shopItems[i].bCount;
		GemItem.dwPrice = kPacket.shopItems[i].dwPrice;

		CPythonPlayer::Instance().SetGemShopItemData(kPacket.shopItems[i].slotIndex, GemItem);
	}
	
	CPythonPlayer::Instance().SetGemShopRefreshTime(kPacket.nextRefreshTime);
	
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenGemShop", Py_BuildValue("()"));

	return true;
}

bool CPythonNetworkStream::RecvGemShopRefresh()
{
	TPacketGCGemShopRefresh kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
		return false;
	
	CPythonPlayer::Instance().ClearGemShopItemVector();
	for (int i = 0; i < 9; ++i)
	{
		TGemShopItem GemItem;
		GemItem.slotIndex = kPacket.shopItems[i].slotIndex;
		GemItem.status = kPacket.shopItems[i].status;
		
		GemItem.dwVnum = kPacket.shopItems[i].dwVnum;
		GemItem.bCount = kPacket.shopItems[i].bCount;
		GemItem.dwPrice = kPacket.shopItems[i].dwPrice;

		CPythonPlayer::Instance().SetGemShopItemData(kPacket.shopItems[i].slotIndex, GemItem);
	}
	
	CPythonPlayer::Instance().SetGemShopRefreshTime(kPacket.nextRefreshTime);
	
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGemShop", Py_BuildValue("()"));

	return true;
}

bool CPythonNetworkStream::SendGemShopBuy(BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;
	
	TPacketCGGemShop PacketGemShop;
	PacketGemShop.header = HEADER_CG_GEM_SHOP;
	PacketGemShop.subheader = GEM_SHOP_SUBHEADER_CG_BUY;
	
	if (!Send(sizeof(TPacketCGShop), &PacketGemShop))
	{
		Tracef("SendGemShopBuyPacket Error\n");
		return false;
	}

	if (!Send(sizeof(BYTE), &bPos))
	{
		Tracef("SendGemShopBuyPacket Error\n");
		return false;
	}
	
	return SendSequence();
}

bool CPythonNetworkStream::SendGemShopAdd(BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;
	
	TPacketCGGemShop PacketGemShop;
	PacketGemShop.header = HEADER_CG_GEM_SHOP;
	PacketGemShop.subheader = GEM_SHOP_SUBHEADER_CG_ADD;
	
	if (!Send(sizeof(TPacketCGShop), &PacketGemShop))
	{
		Tracef("SendGemShopAddPacket Error\n");
		return false;
	}

	if (!Send(sizeof(BYTE), &bPos))
	{
		Tracef("SendGemShopAddPacket Error\n");
		return false;
	}
	
	return SendSequence();
}

bool CPythonNetworkStream::SendGemShopRefresh()
{
	if (!__CanActMainInstance())
		return true;
	
	TPacketCGGemShop PacketGemShop;
	PacketGemShop.header = HEADER_CG_GEM_SHOP;
	PacketGemShop.subheader = GEM_SHOP_SUBHEADER_CG_REFRESH;
	
	if (!Send(sizeof(TPacketCGShop), &PacketGemShop))
	{
		Tracef("SendGemShopAddPacket Error\n");
		return false;
	}
	
	return SendSequence();
}
#endif

#if defined(ENABLE_EXCHANGE_FRAGMENTS)
bool CPythonNetworkStream::SendExchangeFragmentsPacket(TItemPos itemPos)
{
	if (!__CanActMainInstance()) {
		return true;
	}

	TPacketExchangeFragments p;
	p.bHeader = HEADER_CG_EXCHANGE_FRAGMENTS;
	p.itemPos = itemPos;

	if (!Send(sizeof(TPacketExchangeFragments), &p))
	{
		Tracef("SendExchangeFragmentsPacket Error\n");
		return false;
	}

	Tracef("SendExchangeFragmentsPacket(type=%d, slot=%d)\n", itemPos.window_type, itemPos.cell);

	return SendSequence();
}
#endif

#if defined(ENABLE_NEW_FISH_EVENT)
bool CPythonNetworkStream::SendFishBoxUse(uint8_t window, WORD cell) {
	if (!__CanActMainInstance()) {
		return true;
	}

	TPacketCGFishEvent p;
	p.bHeader = HEADER_CG_FISH_EVENT_SEND;
	p.bSubheader = FISH_EVENT_SUBHEADER_BOX_USE;

	if (!Send(sizeof(TPacketCGFishEvent), &p)) {
		Tracef("SendFishEventPacket Error\n");
		return false;
	}

	if (!Send(sizeof(window), &window))
	{
		Tracef("SendFishBoxUse Error\n");
		return false;
	}

	if (!Send(sizeof(cell), &cell))
	{
		Tracef("SendFishBoxUse Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendFishShapeAdd(uint8_t pos) {
	if (!__CanActMainInstance()) {
		return true;
	}

	TPacketCGFishEvent p;
	p.bHeader = HEADER_CG_FISH_EVENT_SEND;
	p.bSubheader = FISH_EVENT_SUBHEADER_SHAPE_ADD;

	if (!Send(sizeof(TPacketCGFishEvent), &p)) {
		Tracef("SendFishEventPacket Error\n");
		return false;
	}

	if (!Send(sizeof(pos), &pos)) {
		Tracef("SendFishShapeAdd Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvFishEventInfo() {
	TPacketGCFishEventInfo p;

	if (!Recv(sizeof(p), &p)) {
		return false;
	}

	switch (p.bSubheader) {
		case FISH_EVENT_SUBHEADER_BOX_USE: {
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameFishUse", Py_BuildValue("(ii)", p.dwFirstArg, p.dwSecondArg));
			break;
		}
		case FISH_EVENT_SUBHEADER_SHAPE_ADD: {
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameFishAdd", Py_BuildValue("(ii)", p.dwFirstArg, p.dwSecondArg));
			break;
		}
		case FISH_EVENT_SUBHEADER_GC_REWARD: {
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameFishReward", Py_BuildValue("(i)", p.dwFirstArg));
			break;
		}
		case FISH_EVENT_SUBHEADER_GC_ENABLE: {
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameFishEvent", Py_BuildValue("(ii)", p.dwFirstArg, p.dwSecondArg));
			break;
		}
		default: {
			break;
		}
	}

	return true;
}
#endif

#if defined(ENABLE_MULTI_ITEM_USE)
bool CPythonNetworkStream::SendMultiItemUsePacket(uint16_t count, TItemPos pos)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	if (__IsEquipItemInSlot(pos))
	{
		if (CPythonExchange::Instance().isTrading())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_EXCHANGE"));
			return true;
		}

		if (CPythonShop::Instance().IsOpen())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_SHOP"));
			return true;
		}

		if (__IsPlayerAttacking())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_WHILE_ATTACKING"));
			return true;
		}
	}
	else if (CheckCannotWhileAttacking(pos))
	{
		return true;
	}

	__PlayInventoryItemUseSound(pos);

	TPacketCGMultiItemUse p;
	p.header = HEADER_CG_MULTI_ITEM_USE;
	p.count = count;
	p.pos = pos;

	if (!Send(sizeof(p), &p))
	{
		Tracen("SendMultiItemUsePacket error.");
		return false;
	}

	return SendSequence();
}
#endif

#ifdef USE_MULTIPLE_OPENING
bool CPythonNetworkStream::SendMultipleOpening(const TItemPos pos, const uint16_t count)
{
    if (count == 0)
    {
        return true;
    }

    if (!__CanActMainInstance())
    {
        return true;
    }

    if (__IsEquipItemInSlot(pos))
    {
        if (CPythonExchange::Instance().isTrading())
        {
            PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_EXCHANGE"));
            return true;
        }
    
        if (CPythonShop::Instance().IsOpen())
        {
            PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_SHOP"));
            return true;
        }
    }

    static uint32_t s_dwNextMultipleOpeningTime = 0;
    uint32_t dwCurTime = ELTimer_GetMSec();

    if (dwCurTime < s_dwNextMultipleOpeningTime)
    {
        PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(si)", "TRY_AGAIN_IN_MILLESECONDS", s_dwNextMultipleOpeningTime - dwCurTime));
        return true;
    }

    s_dwNextMultipleOpeningTime = dwCurTime + 500;

    __PlayInventoryItemUseSound(pos);

    TPacketCGMultipleOpening p;
    p.pos = pos;
    p.count = count;

    if (!Send(sizeof(p), &p))
    {
        Tracen("SendMultipleOpening Error");
        return false;
    }

    return SendSequence();
}
#endif
