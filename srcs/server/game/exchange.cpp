#include "stdafx.h"
#include "../libgame/grid.h"
#include "utils.h"
#include "desc.h"
#include "desc_client.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "locale_service.h"
#include "../common/length.h"
#include "exchange.h"
#include "DragonSoul.h"
#include "questmanager.h"
#if defined(ENABLE_MESSENGER_BLOCK)
#include "messenger_manager.h"
#endif

#if defined(ENABLE_NEW_GOLD_LIMIT)
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, uint64_t arg1, TItemPos arg2, DWORD arg3, void * pvData = NULL);
#else
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, uint32_t, TItemPos arg2, DWORD arg3, void * pvData = NULL);
#endif

#if defined(ENABLE_NEW_GOLD_LIMIT)
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, uint64_t arg1, TItemPos arg2, DWORD arg3, void * pvData)
#else
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, uint32_t arg1, TItemPos arg2, DWORD arg3, void * pvData)
#endif
{
	const LPDESC d = ch ? ch->GetDesc() : nullptr;
	if (!d)
	{
		return;
	}

	struct packet_exchange pack_exchg;

	pack_exchg.header = HEADER_GC_EXCHANGE;
	pack_exchg.sub_header = sub_header;
	pack_exchg.is_me = is_me;
	pack_exchg.arg1 = arg1;
	pack_exchg.arg2 = arg2;
	pack_exchg.arg3 = arg3;

	if (sub_header == EXCHANGE_SUBHEADER_GC_ITEM_ADD && pvData)
	{
#ifdef WJ_ENABLE_TRADABLE_ICON
		pack_exchg.arg4 = TItemPos(((LPITEM) pvData)->GetWindow(), ((LPITEM) pvData)->GetCell());
#endif
		memcpy(&pack_exchg.alSockets, ((LPITEM) pvData)->GetSockets(), sizeof(pack_exchg.alSockets));
		memcpy(&pack_exchg.aAttr, ((LPITEM) pvData)->GetAttributes(), sizeof(pack_exchg.aAttr));
#if defined(ENABLE_CHANGELOOK)
		pack_exchg.dwTransmutation = ((LPITEM)pvData)->GetTransmutation();
#endif
#ifdef ATTR_LOCK
		pack_exchg.lockedattr = ((LPITEM) pvData)->GetLockedAttr();
#endif
	}
	else
	{
#ifdef WJ_ENABLE_TRADABLE_ICON
		pack_exchg.arg4 = TItemPos(RESERVED_WINDOW, 0);
#endif
		memset(&pack_exchg.alSockets, 0, sizeof(pack_exchg.alSockets));
		memset(&pack_exchg.aAttr, 0, sizeof(pack_exchg.aAttr));
#if defined(ENABLE_CHANGELOOK)
		pack_exchg.dwTransmutation = 0;
#endif
#ifdef ATTR_LOCK
		pack_exchg.lockedattr = -1;
#endif
	}

	d->Packet(&pack_exchg, sizeof(pack_exchg));
}

bool CHARACTER::ExchangeStart(LPCHARACTER victim)
{
	if (!victim || this == victim) {
		return false;
	}

	if (IsObserverMode())
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 256, "");
#endif
		return false;
	}

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true) {
		return false;
	}

	if (quest::CQuestManager::instance().GetPCForce(victim->GetPlayerID())->IsRunning() == true) {
		return false;
	}

	if (victim->IsNPC() || !victim->IsPC())
	{
		return false;
	}

	if ( IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen()
#ifdef ENABLE_ACCE_SYSTEM
 || IsAcceOpen()
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
 || GetOfflineShopGuest() || GetAuctionGuest()
#endif
#if defined(ENABLE_AURA_SYSTEM)
 || IsAuraRefineWindowOpen()
#endif
#ifdef __ATTR_TRANSFER_SYSTEM__
 || IsAttrTransferOpen()
#endif
#if defined(ENABLE_CHANGELOOK)
 || isChangeLookOpened()
#endif
#if defined(USE_ATTR_6TH_7TH)
 || IsOpenAttr67Add()
#endif
	) {
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 292, "");
#endif
		return false;
	}

	if ( victim->IsOpenSafebox() || victim->GetShopOwner() || victim->GetMyShop() || victim->IsCubeOpen()
#ifdef ENABLE_ACCE_SYSTEM
 || victim->IsAcceOpen()
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
 || victim->GetOfflineShopGuest() || victim->GetAuctionGuest()
#endif
#if defined(ENABLE_AURA_SYSTEM)
 || victim->IsAuraRefineWindowOpen()
#endif
#ifdef __ATTR_TRANSFER_SYSTEM__
 || victim->IsAttrTransferOpen()
#endif
#if defined(ENABLE_CHANGELOOK)
 || victim->isChangeLookOpened()
#endif
#if defined(USE_ATTR_6TH_7TH)
 || victim->IsOpenAttr67Add()
#endif
	) {
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 293, "%s", victim->GetName());
#endif
		return false;
	}

	int iDist = DISTANCE_APPROX(GetX() - victim->GetX(), GetY() - victim->GetY());
	if (iDist >= EXCHANGE_MAX_DISTANCE)
		return false;

	if (GetExchange())
		return false;

	if (victim->GetExchange())
	{
		exchange_packet(this, EXCHANGE_SUBHEADER_GC_ALREADY, 0, 0, NPOS, 0);
		return false;
	}

	if (victim->IsBlockMode(BLOCK_EXCHANGE))
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 368, "%s", victim->GetName());
#endif
		return false;
	}

#if defined(ENABLE_MESSENGER_BLOCK)
	if (MessengerManager::instance().CheckMessengerList(GetName(), victim->GetName(), SYST_BLOCK)) {
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 1429, "%s", victim->GetName());
#else
		ChatPacket(CHAT_TYPE_INFO, "You can't perform this action because %s blocked you.", victim->GetName());
#endif
		return false;
	}
#endif

#ifdef ENABLE_PVP_ADVANCED
	if ((GetDuel("BlockExchange")))
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 516, "");
#endif
		return false;
	}

	if ((victim->GetDuel("BlockExchange")))
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ChatPacketNew(CHAT_TYPE_INFO, 517, "%s", victim->GetName());
#endif
		return false;
	}
#endif

	SetExchange(M2_NEW CExchange(this));
	victim->SetExchange(M2_NEW CExchange(victim));

	victim->GetExchange()->SetCompany(GetExchange());
	GetExchange()->SetCompany(victim->GetExchange());

	SetExchangeTime();
	victim->SetExchangeTime();

	exchange_packet(victim, EXCHANGE_SUBHEADER_GC_START, 0, GetVID(), NPOS, 0);
	exchange_packet(this, EXCHANGE_SUBHEADER_GC_START, 0, victim->GetVID(), NPOS, 0);

	return true;
}

CExchange::CExchange(LPCHARACTER pOwner)
{
	m_pCompany = NULL;

	m_bAccept = false;

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		m_apItems[i] = NULL;
		m_aItemPos[i] = NPOS;
		m_abItemDisplayPos[i] = 0;
	}

	m_lGold = 0;

	m_pOwner = pOwner;
	pOwner->SetExchange(this);

#ifdef __NEW_EXCHANGE_WINDOW__
	m_pGrid = M2_NEW CGrid(6, 4);
#else
	m_pGrid = M2_NEW CGrid(4, 3);
#endif
}

CExchange::~CExchange()
{
	M2_DELETE(m_pGrid);
}

#if defined(ITEM_CHECKINOUT_UPDATE)
int CExchange::GetEmptyExchange(BYTE size)
{
	for (unsigned int i = 0; m_pGrid && i < m_pGrid->GetSize(); i++)
		if (m_pGrid->IsEmpty(i, 1, size))
			return i;

	return -1;
}
bool CExchange::AddItem(TItemPos item_pos, BYTE display_pos, bool SelectPosAuto)
#else
bool CExchange::AddItem(TItemPos item_pos, BYTE display_pos)
#endif
{
	assert(m_pOwner != NULL && GetCompany());

	if (!item_pos.IsValidItemPosition())
	{
		return false;
	}

	if (item_pos.IsEquipPosition())
	{
		return false;
	}

#if defined(ENABLE_SWITCHBOT_WORLDARD)
	if (item_pos.IsSwitchBotPosition())
	{
		return false;
	}
#endif

	if (item_pos.IsDragonSoulEquipPosition())
	{
		return false;
	}

	LPITEM item;

	if (!(item = m_pOwner->GetItem(item_pos)))
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE))
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		m_pOwner->ChatPacketNew(CHAT_TYPE_INFO, 402, "%s", item->GetName());
#endif
		return false;
	}

	if (item->isLocked())
	{
		return false;
	}

	if (item->IsExchanging())
	{
		return false;
	}

	if (item->IsEquipped())
	{
		return false;
	}

	if (quest::CQuestManager::instance().GetPCForce(m_pOwner->GetPlayerID())->IsRunning() == true)
	{
		return false;
	}

	const LPCHARACTER victim = GetCompany()->GetOwner();
	if (quest::CQuestManager::instance().GetPCForce(victim->GetPlayerID())->IsRunning() == true)
	{
		return false;
	}

#if defined(ITEM_CHECKINOUT_UPDATE)
	if (SelectPosAuto)
	{
		int AutoPos = GetEmptyExchange(item->GetSize());
		if (AutoPos == -1)
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "<EXCHANGE> You don't have enough space.");
			return false;
		}

		display_pos = AutoPos;
	}
#endif

	if (!m_pGrid->IsEmpty(display_pos, 1, item->GetSize()))
	{
		sys_log(0, "EXCHANGE not empty item_pos %d %d %d", display_pos, 1, item->GetSize());
		return false;
	}

	Accept(false);
	GetCompany()->Accept(false);

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_apItems[i])
			continue;

		m_apItems[i] = item;
		m_aItemPos[i] = item_pos;
		m_abItemDisplayPos[i] = display_pos;
		m_pGrid->Put(display_pos, 1, item->GetSize());

		item->SetExchanging(true);

		exchange_packet(m_pOwner,
				EXCHANGE_SUBHEADER_GC_ITEM_ADD,
				true,
				item->GetVnum(),
				TItemPos(RESERVED_WINDOW, display_pos),
				item->GetCount(),
				item);

		exchange_packet(GetCompany()->GetOwner(),
				EXCHANGE_SUBHEADER_GC_ITEM_ADD,
				false,
				item->GetVnum(),
				TItemPos(RESERVED_WINDOW, display_pos),
				item->GetCount(),
				item);

		sys_log(0, "EXCHANGE AddItem success %s pos(%d, %d) %d", item->GetName(), item_pos.window_type, item_pos.cell, display_pos);

		return true;
	}

	return false;
}

bool CExchange::RemoveItem(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return false;

	if (!m_apItems[pos])
		return false;

	TItemPos PosOfInventory = m_aItemPos[pos];
	m_apItems[pos]->SetExchanging(false);

	m_pGrid->Get(m_abItemDisplayPos[pos], 1, m_apItems[pos]->GetSize());

	exchange_packet(GetOwner(),	EXCHANGE_SUBHEADER_GC_ITEM_DEL, true, pos, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_ITEM_DEL, false, pos, PosOfInventory, 0);

	Accept(false);
	GetCompany()->Accept(false);

	m_apItems[pos] = NULL;
	m_aItemPos[pos] = NPOS;
	m_abItemDisplayPos[pos] = 0;
	return true;
}

#if defined(ENABLE_NEW_GOLD_LIMIT)
bool CExchange::AddGold(uint64_t gold)
#else
bool CExchange::AddGold(uint32_t gold)
#endif
{
	if (gold <= 0)
	{
		return false;
	}

	if (GetOwner()->GetGold() < gold)
	{
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_LESS_GOLD, 0, 0, NPOS, 0);
		return false;
	}

	if (m_lGold > 0)
		return false;

	Accept(false);
	GetCompany()->Accept(false);

	m_lGold = gold;

	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_GOLD_ADD, true, m_lGold, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_GOLD_ADD, false, m_lGold, NPOS, 0);
	return true;
}

bool CExchange::Check(int * piItemCount)
{
	if (GetOwner()->GetGold() < m_lGold)
		return false;

	int item_count = 0;

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!m_apItems[i])
			continue;

		if (!m_aItemPos[i].IsValidItemPosition())
			return false;

		if (m_apItems[i] != GetOwner()->GetItem(m_aItemPos[i]))
			return false;

		++item_count;
	}

	*piItemCount = item_count;
	return true;
}

bool CExchange::CheckSpace()
{
	const LPCHARACTER victim = GetCompany()->GetOwner();
	if (victim == nullptr) {
		return false;
	}

	static std::vector<CGrid*> vecInventoryGrid;

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
	uint8_t bPage = 4;
#else
	uint8_t bPage = 2;
#endif

	for (uint8_t i = 0; i < bPage; ++i) {
		vecInventoryGrid.emplace_back(M2_NEW CGrid(5, 9));
	}

	for (auto& grid : vecInventoryGrid) {
		grid->Clear();
	}

	uint8_t bPageStat = 0;
	int32_t j = 0, n = 45;

	LPITEM item;
	for (; j < n; ++j) {
		item = victim->GetInventoryItem(j);
		if (!item) {
			continue;
		}

		vecInventoryGrid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetInventoryItem(n + j);
		if (!item) {
			continue;
		}

		vecInventoryGrid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecInventoryGrid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecInventoryGrid.at(bPageStat)->Put(j, 1, item->GetSize());
	}
#endif

#if defined(ENABLE_EXTRA_INVENTORY)
	static std::vector<CGrid*> vecExtraInventory1Grid;

	for (uint8_t i = 0; i < bPage; ++i) {
		vecExtraInventory1Grid.emplace_back(M2_NEW CGrid(5, 9));
	}

	for (auto& grid : vecExtraInventory1Grid) {
		grid->Clear();
	}

	bPageStat = 0;
	j = 0;
	n = EXTRA_INVENTORY_PAGE_SIZE;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem(j);
		if (!item) {
			continue;
		}

		vecExtraInventory1Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem(n + j);
		if (!item) {
			continue;
		}

		vecExtraInventory1Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory1Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory1Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}
#endif

	j = 0;
	++bPageStat;

	static std::vector<CGrid*> vecExtraInventory2Grid;

	for (uint8_t i = 0; i < bPage; ++i) {
		vecExtraInventory2Grid.emplace_back(M2_NEW CGrid(5, 9));
	}

	for (auto& grid : vecExtraInventory2Grid) {
		grid->Clear();
	}

	bPageStat = 0;
	j = 0;
	n = EXTRA_INVENTORY_PAGE_SIZE;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory2Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory2Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory2Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory2Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}
#endif

	j = 0;
	++bPageStat;

	static std::vector<CGrid*> vecExtraInventory3Grid;

	for (uint8_t i = 0; i < bPage; ++i) {
		vecExtraInventory3Grid.emplace_back(M2_NEW CGrid(5, 9));
	}

	for (auto& grid : vecExtraInventory3Grid) {
		grid->Clear();
	}

	bPageStat = 0;
	j = 0;
	n = EXTRA_INVENTORY_PAGE_SIZE;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory3Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory3Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory3Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory3Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}
#endif

	j = 0;
	++bPageStat;

	static std::vector<CGrid*> vecExtraInventory4Grid;

	for (uint8_t i = 0; i < bPage; ++i) {
		vecExtraInventory4Grid.emplace_back(M2_NEW CGrid(5, 9));
	}

	for (auto& grid : vecExtraInventory4Grid) {
		grid->Clear();
	}

	bPageStat = 0;
	j = 0;
	n = EXTRA_INVENTORY_PAGE_SIZE;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory4Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory4Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory4Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory4Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}
#endif

	j = 0;
	++bPageStat;

	static std::vector<CGrid*> vecExtraInventory5Grid;

	for (uint8_t i = 0; i < bPage; ++i) {
		vecExtraInventory5Grid.emplace_back(M2_NEW CGrid(5, 9));
	}

	for (auto& grid : vecExtraInventory5Grid) {
		grid->Clear();
	}

	bPageStat = 0;
	j = 0;
	n = EXTRA_INVENTORY_PAGE_SIZE;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory5Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory5Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory5Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}

	j = 0;
	++bPageStat;

	for (; j < n; ++j) {
		item = victim->GetExtraInventoryItem((n * bPageStat) + j);
		if (!item) {
			continue;
		}

		vecExtraInventory5Grid.at(bPageStat)->Put(j, 1, item->GetSize());
	}
#endif
#endif

    bool ret = true;

	static std::vector<WORD> s_vDSGrid(DRAGON_SOUL_INVENTORY_MAX_NUM);  
	bool bDSInitialized = false;

	for (int32_t i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i) {
		if (!(item = m_apItems[i]))
			continue;

		if (item->IsDragonSoul()) {
			if (!bDSInitialized) {
				bDSInitialized = true;
				victim->CopyDragonSoulItemGrid(s_vDSGrid);
			}

			bool bExistEmptySpace = false;
			WORD wBasePos = DSManager::instance().GetBasePosition(item);
			if (wBasePos >= DRAGON_SOUL_INVENTORY_MAX_NUM) {
                ret = false;
                break;
			}

			for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; i++) {
				WORD wPos = wBasePos + i;
				if (0 == s_vDSGrid[wPos])
				{
					bool bEmpty = true;
					for (int j = 1; j < item->GetSize(); j++) {
						if (s_vDSGrid[wPos + j * DRAGON_SOUL_BOX_COLUMN_NUM]) {
							bEmpty = false;
							break;
						}
					}
					
					if (bEmpty) {
						for (int j = 0; j < item->GetSize(); j++) {
							s_vDSGrid[wPos + j * DRAGON_SOUL_BOX_COLUMN_NUM] =  wPos + 1;
						}
						
						bExistEmptySpace = true;
						break;
					}
				}
				
				if (bExistEmptySpace) {
					break;
				}
			}
			if (!bExistEmptySpace) {
                ret = false;
                break;
			}
		}
#if defined(ENABLE_EXTRA_INVENTORY)
		else if (item->IsExtraItem())
		{
			uint8_t category = item->GetExtraCategory();
			int32_t iPos = -1;
			uint8_t checkPage = 0;

			if (category == 0) {
				iPos = vecExtraInventory1Grid.at(checkPage)->FindBlank(1, item->GetSize());
				if (iPos >= 0) {
					vecExtraInventory1Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
				} else {
					++checkPage;

					iPos = vecExtraInventory1Grid.at(checkPage)->FindBlank(1, item->GetSize());
					if (iPos >= 0) {
						vecExtraInventory1Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
					} else
#if !defined(ENABLE_EXTEND_INVEN_SYSTEM)
					{
                        ret = false;
                        break;
					}
#else
					{
						++checkPage;

						iPos = vecExtraInventory1Grid.at(checkPage)->FindBlank(1, item->GetSize());
						if (iPos >= 0) {
							vecExtraInventory1Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
						} else {
							++checkPage;

							iPos = vecExtraInventory1Grid.at(checkPage)->FindBlank(1, item->GetSize());
							if (iPos >= 0) {
								vecExtraInventory1Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
							} else {
                                ret = false;
                                break;
							}
						}
#endif
					}
				}
			}
			else if (category == 1) {
				iPos = vecExtraInventory2Grid.at(checkPage)->FindBlank(1, item->GetSize());
				if (iPos >= 0) {
					vecExtraInventory2Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
				} else {
					++checkPage;

					iPos = vecExtraInventory2Grid.at(checkPage)->FindBlank(1, item->GetSize());
					if (iPos >= 0) {
						vecExtraInventory2Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
					} else
#if !defined(ENABLE_EXTEND_INVEN_SYSTEM)
					{
                        ret = false;
                        break;
					}
#else
					{
						++checkPage;

						iPos = vecExtraInventory2Grid.at(checkPage)->FindBlank(1, item->GetSize());
						if (iPos >= 0) {
							vecExtraInventory2Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
						} else {
							++checkPage;

							iPos = vecExtraInventory2Grid.at(checkPage)->FindBlank(1, item->GetSize());
							if (iPos >= 0) {
								vecExtraInventory2Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
							} else {
                                ret = false;
                                break;
							}
						}
#endif
					}
				}
			}
			else if (category == 2) {
				iPos = vecExtraInventory3Grid.at(checkPage)->FindBlank(1, item->GetSize());
				if (iPos >= 0) {
					vecExtraInventory3Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
				} else {
					++checkPage;

					iPos = vecExtraInventory3Grid.at(checkPage)->FindBlank(1, item->GetSize());
					if (iPos >= 0) {
						vecExtraInventory3Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
					} else
#if !defined(ENABLE_EXTEND_INVEN_SYSTEM)
					{
                        ret = false;
                        break;
					}
#else
					{
						++checkPage;

						iPos = vecExtraInventory3Grid.at(checkPage)->FindBlank(1, item->GetSize());
						if (iPos >= 0) {
							vecExtraInventory3Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
						} else {
							++checkPage;

							iPos = vecExtraInventory3Grid.at(checkPage)->FindBlank(1, item->GetSize());
							if (iPos >= 0) {
								vecExtraInventory3Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
							} else {
                                ret = false;
                                break;
							}
						}
#endif
					}
				}
			}
			else if (category == 3) {
				iPos = vecExtraInventory4Grid.at(checkPage)->FindBlank(1, item->GetSize());
				if (iPos >= 0) {
					vecExtraInventory4Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
				} else {
					++checkPage;

					iPos = vecExtraInventory4Grid.at(checkPage)->FindBlank(1, item->GetSize());
					if (iPos >= 0) {
						vecExtraInventory4Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
					} else
#if !defined(ENABLE_EXTEND_INVEN_SYSTEM)
					{
                        ret = false;
                        break;
					}
#else
					{
						++checkPage;

						iPos = vecExtraInventory4Grid.at(checkPage)->FindBlank(1, item->GetSize());
						if (iPos >= 0) {
							vecExtraInventory4Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
						} else {
							++checkPage;

							iPos = vecExtraInventory4Grid.at(checkPage)->FindBlank(1, item->GetSize());
							if (iPos >= 0) {
								vecExtraInventory4Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
							} else {
                                ret = false;
                                break;
							}
						}
#endif
					}
				}
			}
			else if (category == 4) {
				iPos = vecExtraInventory5Grid.at(checkPage)->FindBlank(1, item->GetSize());
				if (iPos >= 0) {
					vecExtraInventory5Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
				} else {
					++checkPage;

					iPos = vecExtraInventory5Grid.at(checkPage)->FindBlank(1, item->GetSize());
					if (iPos >= 0) {
						vecExtraInventory5Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
					} else
#if !defined(ENABLE_EXTEND_INVEN_SYSTEM)
					{
                        ret = false;
                        break;
					}
#else
					{
						++checkPage;

						iPos = vecExtraInventory5Grid.at(checkPage)->FindBlank(1, item->GetSize());
						if (iPos >= 0) {
							vecExtraInventory5Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
						} else {
							++checkPage;

							iPos = vecExtraInventory5Grid.at(checkPage)->FindBlank(1, item->GetSize());
							if (iPos >= 0) {
								vecExtraInventory5Grid.at(checkPage)->Put(iPos, 1, item->GetSize());
							} else {
                                ret = false;
                                break;
							}
						}
#endif
					}
				}
			}
		}
#endif
		else {
			int32_t iPos = -1;
			uint8_t checkPage = 0;

			if (iPos >= 0) {
				vecInventoryGrid.at(checkPage)->Put(iPos, 1, item->GetSize());
			} else {
				++checkPage;

				iPos = vecInventoryGrid.at(checkPage)->FindBlank(1, item->GetSize());
				if (iPos >= 0) {
					vecInventoryGrid.at(checkPage)->Put(iPos, 1, item->GetSize());
				} else
#if !defined(ENABLE_EXTEND_INVEN_SYSTEM)
				{
                    ret = false;
                    break;
				}
#else
				{
					++checkPage;

					iPos = vecInventoryGrid.at(checkPage)->FindBlank(1, item->GetSize());
					if (iPos >= 0) {
						vecInventoryGrid.at(checkPage)->Put(iPos, 1, item->GetSize());
					} else {
						++checkPage;

						iPos = vecInventoryGrid.at(checkPage)->FindBlank(1, item->GetSize());
						if (iPos >= 0) {
							vecInventoryGrid.at(checkPage)->Put(iPos, 1, item->GetSize());
						} else {
                            ret = false;
                            break;
						}
					}
#endif
				}
			}
		}
	}

	for (auto it = vecInventoryGrid.begin(); it != vecInventoryGrid.end(); ++it) {
		M2_DELETE(*it);
	}

	vecInventoryGrid.clear();

#if defined(ENABLE_EXTRA_INVENTORY)
	for (auto it = vecExtraInventory1Grid.begin(); it != vecExtraInventory1Grid.end(); ++it) {
		M2_DELETE(*it);
	}

	vecExtraInventory1Grid.clear();

	for (auto it = vecExtraInventory2Grid.begin(); it != vecExtraInventory2Grid.end(); ++it) {
		M2_DELETE(*it);
	}

	vecExtraInventory2Grid.clear();

	for (auto it = vecExtraInventory3Grid.begin(); it != vecExtraInventory3Grid.end(); ++it) {
		M2_DELETE(*it);
	}

	vecExtraInventory3Grid.clear();

	for (auto it = vecExtraInventory4Grid.begin(); it != vecExtraInventory4Grid.end(); ++it) {
		M2_DELETE(*it);
	}

	vecExtraInventory4Grid.clear();

	for (auto it = vecExtraInventory5Grid.begin(); it != vecExtraInventory5Grid.end(); ++it) {
		M2_DELETE(*it);
	}

	vecExtraInventory5Grid.clear();
#endif

	return ret;
}

bool CExchange::IsEmpty() const
{
	if (*m_apItems || m_lGold)
		return false;
	return true;
}

bool CExchange::Done()
{
	int empty_pos, i;
	LPITEM item;
	const LPCHARACTER victim = GetCompany()->GetOwner();

	for (i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		item = m_apItems[i];
		if (!item)
		{
			continue;
		}

		if (item->IsDragonSoul())
			empty_pos = victim->GetEmptyDragonSoulInventory(item);
#ifdef ENABLE_EXTRA_INVENTORY
		else if (item->IsExtraItem())
			empty_pos = victim->GetEmptyExtraInventory(item);
#endif
		else
			empty_pos = victim->GetEmptyInventory(item->GetSize());

		if (empty_pos < 0)
		{
			sys_err("Exchange::Done : Cannot find blank position in inventory %s <-> %s item %s",
					m_pOwner->GetName(), victim->GetName(), item->GetName());
			continue;
		}

		assert(empty_pos >= 0);

#ifdef ENABLE_EXTRA_INVENTORY
		if (item->IsExtraItem()) {
			m_pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM_EXTRA, item->GetCell(), 65535);
		} else {
			m_pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, item->GetCell(), 65535);
		}
#else
		m_pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, item->GetCell(), 65535);
#endif

		item->RemoveFromCharacter();
		if (item->IsDragonSoul())
		{
			item->AddToCharacter(victim, TItemPos(DRAGON_SOUL_INVENTORY, empty_pos));
		}
#ifdef ENABLE_EXTRA_INVENTORY
		else if (item->IsExtraItem())
		{
			item->AddToCharacter(victim, TItemPos(EXTRA_INVENTORY, empty_pos));
		}
#endif
		else
		{
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		}

		ITEM_MANAGER::instance().FlushDelayedSave(item);

		item->SetExchanging(false);

		{
			char exchange_buf[51];

			snprintf(exchange_buf, sizeof(exchange_buf), "%s %u %u", item->GetName(), GetOwner()->GetPlayerID(), item->GetCount());
			LogManager::instance().ItemLog(victim, item, "EXCHANGE_TAKE", exchange_buf);

			snprintf(exchange_buf, sizeof(exchange_buf), "%s %u %u", item->GetName(), victim->GetPlayerID(), item->GetCount());
			LogManager::instance().ItemLog(GetOwner(), item, "EXCHANGE_GIVE", exchange_buf);

			if (item->GetVnum() >= 80003 && item->GetVnum() <= 80007)
			{
				LogManager::instance().GoldBarLog(victim->GetPlayerID(), item->GetID(), EXCHANGE_TAKE, "");
				LogManager::instance().GoldBarLog(GetOwner()->GetPlayerID(), item->GetID(), EXCHANGE_GIVE, "");
			}
		}

		m_apItems[i] = NULL;
	}

	if (m_lGold)
	{
#if defined(ENABLE_NEW_GOLD_LIMIT)
		GetOwner()->ChangeGold(-m_lGold);
		victim->ChangeGold(m_lGold);
#else
		GetOwner()->PointChange(POINT_GOLD, -m_lGold, true);
		victim->PointChange(POINT_GOLD, m_lGold, true);
#endif

		if (m_lGold > 100000000)
		{
			char exchange_buf[51];
			snprintf(exchange_buf, sizeof(exchange_buf), "%u %s", GetOwner()->GetPlayerID(), GetOwner()->GetName());
			LogManager::instance().CharLog(victim, m_lGold, "EXCHANGE_GOLD_TAKE", exchange_buf);

			snprintf(exchange_buf, sizeof(exchange_buf), "%u %s", victim->GetPlayerID(), victim->GetName());
			LogManager::instance().CharLog(GetOwner(), m_lGold, "EXCHANGE_GOLD_GIVE", exchange_buf);
		}
	}

	m_pGrid->Clear();
	return true;
}

bool CExchange::Accept(bool bAccept)
{
	if (m_bAccept == bAccept)
	{
		return true;
	}

	m_bAccept = bAccept;

	if (m_bAccept && GetCompany()->m_bAccept)
	{
		int	iItemCount;

		const LPCHARACTER victim = GetCompany()->GetOwner();

		GetOwner()->SetExchangeTime();
		victim->SetExchangeTime();

		if (quest::CQuestManager::instance().GetPCForce(GetOwner()->GetPlayerID())->IsRunning() == true)
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			GetOwner()->ChatPacketNew(CHAT_TYPE_INFO, 631, "");
			victim->ChatPacketNew(CHAT_TYPE_INFO, 631, "");
#endif
			goto EXCHANGE_END;
		}
		else if (quest::CQuestManager::instance().GetPCForce(victim->GetPlayerID())->IsRunning() == true)
		{
			victim->ChatPacket(CHAT_TYPE_INFO, "You cannot trade if you're using quests");
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "You cannot trade if the other part using quests");
			goto EXCHANGE_END;
		}

		if (!Check(&iItemCount))
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			GetOwner()->ChatPacketNew(CHAT_TYPE_INFO, 232, "");
#endif
			victim->ChatPacketNew(CHAT_TYPE_INFO, 274, "%s", GetOwner()->GetName());
			goto EXCHANGE_END;
		}

		if (!CheckSpace())
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			GetOwner()->ChatPacketNew(CHAT_TYPE_INFO, 365, "%s", victim->GetName());
			victim->ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
			goto EXCHANGE_END;
		}

		if (!GetCompany()->Check(&iItemCount))
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			victim->ChatPacketNew(CHAT_TYPE_INFO, 232, "");
			GetOwner()->ChatPacketNew(CHAT_TYPE_INFO, 274, "%s", victim->GetName());
#endif
			goto EXCHANGE_END;
		}

		if (!GetCompany()->CheckSpace())
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			victim->ChatPacketNew(CHAT_TYPE_INFO, 365, "%s", GetOwner()->GetName());
			GetOwner()->ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
			goto EXCHANGE_END;
		}

		if (GetCompany()->IsEmpty() && IsEmpty())
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "The trade with %s has been failed."), victim->GetName();
			victim->ChatPacket(CHAT_TYPE_INFO, "The trade with %s has been failed."), GetOwner()->GetName();
			goto EXCHANGE_END;
		}

		if (db_clientdesc->GetSocket() == INVALID_SOCKET)
		{
			sys_err("Cannot use exchange feature while DB cache connection is dead.");
#if defined(ENABLE_TEXTS_RENEWAL)
			victim->ChatPacketNew(CHAT_TYPE_INFO, 759, "");
			GetOwner()->ChatPacketNew(CHAT_TYPE_INFO, 759, "");
#endif
			goto EXCHANGE_END;
		}

		if (Done())
		{
			if (m_lGold)
				GetOwner()->Save();

			if (GetCompany()->Done())
			{
				if (GetCompany()->m_lGold)
					victim->Save();

#if defined(ENABLE_TEXTS_RENEWAL)
				GetOwner()->ChatPacketNew(CHAT_TYPE_INFO, 105, "%s", victim->GetName());
				victim->ChatPacketNew(CHAT_TYPE_INFO, 105, "%s", GetOwner()->GetName());
#endif
			}
		}

EXCHANGE_END:
		Cancel();
		return false;
	}
	else
	{
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_ACCEPT, true, m_bAccept, NPOS, 0);
		exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_ACCEPT, false, m_bAccept, NPOS, 0);
		return true;
	}
}

void CExchange::Cancel()
{
	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_END, 0, 0, NPOS, 0);
	GetOwner()->SetExchange(NULL);

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_apItems[i])
		{
			m_apItems[i]->SetExchanging(false);
		}
	}

	if (GetCompany())
	{
		GetCompany()->SetCompany(NULL);
		GetCompany()->Cancel();
	}

	M2_DELETE(this);
}

