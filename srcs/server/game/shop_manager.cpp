#include "stdafx.h"
#include "../libgame/grid.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "shop.h"
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "questmanager.h"
#include "mob_manager.h"
#include "locale_service.h"
#include "desc_client.h"
#include "shop_manager.h"
#include "group_text_parse_tree.h"
#include "shopEx.h"
#include "shop_manager.h"
#include <cctype>

CShopManager::CShopManager()
{
}

CShopManager::~CShopManager()
{
	Destroy();
}

#if defined(ENABLE_GAYA_RENEWAL)
bool CShopManager::InitializeGemShop(TGemShopTable * table, int size)
{
	std::map<int, TGemShopTable *> map_shop;
	
	if (m_pGemShopTable)
	{
		delete [] (m_pGemShopTable);
		m_pGemShopTable = NULL;
	}
	
	TGemShopTable * shop_table = m_pGemShopTable;
	
	for (int i = 0; i < size; ++i, ++table)
	{
		if (map_shop.end() == map_shop.find(i))
		{
			TItemTable * pProto = ITEM_MANAGER::instance().GetTable(table->dwVnum);
			
			if (!pProto)
			{
				sys_err("No item by vnum : %d", table->dwVnum);
				continue;
			}
			
			shop_table = new TGemShopTable;
			memset(shop_table, 0, sizeof(TGemShopTable));

			shop_table->dwVnum = table->dwVnum;
			shop_table->bCount = table->bCount;
			shop_table->dwPrice = table->dwPrice;
			shop_table->dwRow = table->dwRow;
			
			map_shop[i] = shop_table;
		}
		else
		{
			shop_table = map_shop[i];
		}
	}
	
	m_pGemShopTable = new TGemShopTable[map_shop.size()];
	m_iGemShopTableSize = map_shop.size();

	auto it = map_shop.begin();

	int i = 0;

	while (it != map_shop.end())
	{
		memcpy((m_pGemShopTable + i), (it++)->second, sizeof(TGemShopTable));
		++i;
	}

	return true;
}
DWORD CShopManager::GemShopGetRandomId(DWORD dwRow)
{
	std::vector<DWORD> dwItemId;
	
	for (DWORD id = 0; id < m_iGemShopTableSize; id++)
	{
		if (m_pGemShopTable[id].dwRow == dwRow)
			dwItemId.push_back(id);
	}
	
	DWORD randomNumber = number(0, dwItemId.size() - 1);
	
	return dwItemId[randomNumber];
}
DWORD CShopManager::GemShopGetVnumById(DWORD id)
{
    if (id < m_iGemShopTableSize)
    {
        return m_pGemShopTable[id].dwVnum;
    }

    return 0;
}

BYTE CShopManager::GemShopGetCountById(DWORD id)
{
    if (id < m_iGemShopTableSize)
    {
        return m_pGemShopTable[id].bCount;
    }

    return 0;
}

DWORD CShopManager::GemShopGetPriceById(DWORD id)
{
    if (id < m_iGemShopTableSize)
    {
        return m_pGemShopTable[id].dwPrice;
    }

    return 0;
}
#endif

bool CShopManager::Initialize(TShopTable * table, int size)
{
	if (!m_map_pkShop.empty())
		return false;

	int i;

	for (i = 0; i < size; ++i, ++table)
	{
		LPSHOP shop = M2_NEW CShop;

		if (!shop->Create(table->dwVnum, table->dwNPCVnum, table->items))
		{
			M2_DELETE(shop);
			continue;
		}

		m_map_pkShop.insert(TShopMap::value_type(table->dwVnum, shop));
		m_map_pkShopByNPCVnum.insert(TShopMap::value_type(table->dwNPCVnum, shop));
	}
	char szShopTableExFileName[256];

	snprintf(szShopTableExFileName, sizeof(szShopTableExFileName),
		"%s/shop_table_ex.txt", LocaleService_GetBasePath().c_str());

	return ReadShopTableEx(szShopTableExFileName);
}

void CShopManager::Destroy()
{
	TShopMap::iterator it = m_map_pkShop.begin();

	while (it != m_map_pkShop.end())
	{
		M2_DELETE(it->second);
		++it;
	}

	m_map_pkShop.clear();
}

LPSHOP CShopManager::Get(DWORD dwVnum)
{
	TShopMap::const_iterator it = m_map_pkShop.find(dwVnum);

	if (it == m_map_pkShop.end())
		return NULL;

	return (it->second);
}

LPSHOP CShopManager::GetByNPCVnum(DWORD dwVnum)
{
	TShopMap::const_iterator it = m_map_pkShopByNPCVnum.find(dwVnum);

	if (it == m_map_pkShopByNPCVnum.end())
		return NULL;

	return (it->second);
}

/*
 * 인터페이스 함수들
 */

// 상점 거래를 시작
bool CShopManager::StartShopping(LPCHARACTER pkChr, LPCHARACTER pkChrShopKeeper, int iShopVnum)
{
	if (pkChr->GetShopOwner() == pkChrShopKeeper)
		return false;
	// this method is only for NPC
	
	if (pkChrShopKeeper->IsPC())
		return false;

	//PREVENT_TRADE_WINDOW
	if (pkChr->IsOpenSafebox() || pkChr->GetExchange() || pkChr->GetMyShop() || pkChr->IsCubeOpen()
#ifdef ENABLE_ACCE_SYSTEM
 || pkChr->IsAcceOpen()
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
 || pkChr->GetOfflineShopGuest() || pkChr->GetAuctionGuest()
#endif
#if defined(ENABLE_AURA_SYSTEM)
 || pkChr->IsAuraRefineWindowOpen()
#endif
#ifdef __ATTR_TRANSFER_SYSTEM__
 || pkChr->IsAttrTransferOpen()
#endif
#if defined(ENABLE_CHANGELOOK)
 || pkChr->isChangeLookOpened()
#endif
#if defined(USE_ATTR_6TH_7TH)
 || pkChr->IsOpenAttr67Add()
#endif
	)
	{
		pkChr->ChatPacketNew(CHAT_TYPE_INFO, 294, "");
		return false;
	}
	//END_PREVENT_TRADE_WINDOW

	long distance = DISTANCE_APPROX(pkChr->GetX() - pkChrShopKeeper->GetX(), pkChr->GetY() - pkChrShopKeeper->GetY());

	if (distance >= SHOP_MAX_DISTANCE)
	{
		sys_log(1, "SHOP: TOO_FAR: %s distance %d", pkChr->GetName(), distance);
		return false;
	}

	LPSHOP pkShop;

	if (iShopVnum)
		pkShop = Get(iShopVnum);
	else
		pkShop = GetByNPCVnum(pkChrShopKeeper->GetRaceNum());

	if (!pkShop)
	{
		sys_log(1, "SHOP: NO SHOP");
		return false;
	}

	bool bOtherEmpire = false;

	if (pkChr->GetEmpire() != pkChrShopKeeper->GetEmpire())
		bOtherEmpire = true;

	pkShop->AddGuest(pkChr, pkChrShopKeeper->GetVID(), bOtherEmpire);
	pkChr->SetShopOwner(pkChrShopKeeper);
	sys_log(0, "SHOP: START: %s", pkChr->GetName());
	return true;
}

LPSHOP CShopManager::FindPCShop(DWORD dwVID)
{
	TShopMap::iterator it = m_map_pkShopByPC.find(dwVID);

	if (it == m_map_pkShopByPC.end())
		return NULL;

	return it->second;
}

LPSHOP CShopManager::CreatePCShop(LPCHARACTER ch, TShopItemTable * pTable, BYTE bItemCount)
{
	if (bItemCount == 0)
	{
		return nullptr;
	}

	if (FindPCShop(ch->GetVID()))
		return NULL;

	LPSHOP pkShop = M2_NEW CShop;
	pkShop->SetPCShop(ch);
	pkShop->SetShopItems(pTable, bItemCount);

	m_map_pkShopByPC.insert(TShopMap::value_type(ch->GetVID(), pkShop));
	return pkShop;
}

void CShopManager::DestroyPCShop(LPCHARACTER ch)
{
	LPSHOP pkShop = FindPCShop(ch->GetVID());

	if (!pkShop)
		return;

	//PREVENT_ITEM_COPY;
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

	m_map_pkShopByPC.erase(ch->GetVID());
	M2_DELETE(pkShop);
}

// 상점 거래를 종료
void CShopManager::StopShopping(LPCHARACTER ch)
{
	LPSHOP shop;

	if (!(shop = ch->GetShop()))
		return;

	//PREVENT_ITEM_COPY;
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

	shop->RemoveGuest(ch);
	sys_log(0, "SHOP: END: %s", ch->GetName());
}

// 아이템 구입
void CShopManager::Buy(LPCHARACTER ch, BYTE pos)
{
	if (0 != g_BuySellTimeLimitValue)
	{
		if (get_dword_time() < ch->GetLastBuySellTime()+g_BuySellTimeLimitValue)
		{
			ch->ChatPacketNew(CHAT_TYPE_INFO, 510, "");
			return;
		}
	}

	ch->SetLastBuySellTime(get_dword_time());

	if (!ch->GetShop())
		return;

	if (ch->GetShopOwner())
	{
		if (DISTANCE_APPROX(ch->GetX() - ch->GetShopOwner()->GetX(), ch->GetY() - ch->GetShopOwner()->GetY()) > 2000)
		{
			ch->ChatPacketNew(CHAT_TYPE_INFO, 381, "");
			return;
		}
	}

	CShop* pkShop = ch->GetShop();
	//PREVENT_ITEM_COPY
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

	int ret = pkShop->Buy(ch, pos);

	if (SHOP_SUBHEADER_GC_OK != ret) // 문제가 있었으면 보낸다.
	{
		TPacketGCShop pack;

		pack.header	= HEADER_GC_SHOP;
		pack.subheader	= ret;
		pack.size	= sizeof(TPacketGCShop);

		ch->GetDesc()->Packet(&pack, sizeof(pack));
	}
}

#ifdef ENABLE_BUY_STACK_FROM_SHOP
void CShopManager::MultipleBuy(LPCHARACTER ch, uint8_t p, uint8_t c) {
	if (!ch->GetShop()) {
		return;
	}

	if (ch->GetShopOwner()) {
		if (DISTANCE_APPROX(ch->GetX() - ch->GetShopOwner()->GetX(), ch->GetY() - ch->GetShopOwner()->GetY()) > 2000) {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 381, "");
			return;
		}
	}

	CShop* pkShop = ch->GetShop();
	//PREVENT_ITEM_COPY
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

	int ret = pkShop->MultipleBuy(ch, p, c);
	if (SHOP_SUBHEADER_GC_OK != ret) {
		TPacketGCShop pack;
		pack.header = HEADER_GC_SHOP;
		pack.subheader = ret;
		pack.size = sizeof(TPacketGCShop);

		ch->GetDesc()->Packet(&pack, sizeof(pack));
	}
}
#endif

#ifdef ENABLE_EXTRA_INVENTORY
void CShopManager::Sell(LPCHARACTER ch, TItemPos Cell, WORD bCount)
#else
void CShopManager::Sell(LPCHARACTER ch, BYTE bCell, WORD bCount)
#endif
{
#if defined(ENABLE_ANTI_FLOOD)
	if (ch) {
		if (thecore_pulse() > ch->GetAntiFloodPulse(FLOOD_SELL_SHOP) + PASSES_PER_SEC(1)) {
			ch->SetAntiFloodCount(FLOOD_SELL_SHOP, 0);
			ch->SetAntiFloodPulse(FLOOD_SELL_SHOP, thecore_pulse());
		}

		if (ch->IncreaseAntiFloodCount(FLOOD_SELL_SHOP) >= 12) {
			LPDESC d = ch->GetDesc();
			if (d) {
				d->DelayedDisconnect(5);
				return;
			}
		}
	}
#endif

	if (0 != g_BuySellTimeLimitValue)
	{
		if (get_dword_time() < ch->GetLastBuySellTime()+g_BuySellTimeLimitValue)
		{
			ch->ChatPacketNew(CHAT_TYPE_INFO, 510, "");
			return;
		}
	}

	ch->SetLastBuySellTime(get_dword_time());

	if (!ch->GetShop())
		return;

	if (!ch->GetShopOwner())
		return;

	if (!ch->CanHandleItem())
		return;

	if (ch->GetShop()->IsPCShop())
		return;

#ifdef __ENABLE_NEW_OFFLINESHOP__
	if (ch->GetOfflineShopGuest() || ch->GetAuctionGuest())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot sell the item if you have opened a private shop.");
		return;
	}
#endif

	/*
	if (DISTANCE_APPROX(ch->GetX()-ch->GetShopOwner()->GetX(), ch->GetY()-ch->GetShopOwner()->GetY())>2000)
	{
		return;
	}
	*/

#ifdef ENABLE_EXTRA_INVENTORY
	LPITEM item = ch->GetItem(Cell);
#else
	LPITEM item = ch->GetInventoryItem(bCell);
#endif

	if (!item)
		return;

	if (item->IsEquipped() == true)
	{
		ch->ChatPacketNew(CHAT_TYPE_INFO, 541, "");
		return;
	}

	if (true == item->isLocked())
	{
		return;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SELL))
		return;

	if (bCount == 0 || bCount > item->GetCount())
		bCount = item->GetCount();

#if defined(ENABLE_NEW_GOLD_LIMIT)
	uint64_t dwPrice = item->GetShopBuyPrice();
#else
	uint32_t dwPrice = item->GetShopBuyPrice();
#endif

	if (IS_SET(item->GetFlag(), ITEM_FLAG_COUNT_PER_1GOLD))
	{
		if (dwPrice == 0)
			dwPrice = bCount;
		else
			dwPrice = bCount / dwPrice;
	}
	else
		dwPrice *= bCount;

	dwPrice /= 5;

	//세금 계산
	DWORD dwTax = 0;
	int iVal = 3;

	{
		dwTax = dwPrice * iVal/100;
		dwPrice -= dwTax;
	}

	if (test_server)
		sys_log(0, "Sell Item price id %d %s itemid %d", ch->GetPlayerID(), ch->GetName(), item->GetID());

	const uint64_t nTotalMoney = static_cast<uint64_t>(ch->GetGold()) + static_cast<uint64_t>(dwPrice);
#if defined(ENABLE_NEW_GOLD_LIMIT)
	if (GOLD_MAX_MAX <= nTotalMoney)
	{
		sys_err("[OVERFLOW_GOLD] id %u name %s gold %llu", ch->GetPlayerID(), ch->GetName(), ch->GetGold());
		ch->ChatPacketNew(CHAT_TYPE_INFO, 226, "%llu", GOLD_MAX_MAX);
		return;
	}
#else
	if (GOLD_MAX <= nTotalMoney)
	{
		sys_err("[OVERFLOW_GOLD] id %u name %s gold %lu", ch->GetPlayerID(), ch->GetName(), ch->GetGold());
		ch->ChatPacketNew(CHAT_TYPE_INFO, 226, "%lu", GOLD_MAX);
		return;
	}
#endif

#if defined(USE_BATTLEPASS)
	ch->UpdateExtBattlePassMissionProgress(BP_ITEM_SELL, bCount, item->GetVnum());
#endif

	DBManager::instance().SendMoneyLog(MONEY_LOG_SHOP, item->GetVnum(), dwPrice);

	if (bCount == item->GetCount())
		ITEM_MANAGER::instance().RemoveItem(item, "SELL");
	else
		item->SetCount(item->GetCount() - bCount);

#if defined(ENABLE_NEW_GOLD_LIMIT)
	ch->ChangeGold(dwPrice);
#else
	ch->PointChange(POINT_GOLD, dwPrice, false);
#endif
}

bool CompareShopItemName(const SShopItemTable& lhs, const SShopItemTable& rhs)
{
	TItemTable* lItem = ITEM_MANAGER::instance().GetTable(lhs.vnum);
	TItemTable* rItem = ITEM_MANAGER::instance().GetTable(rhs.vnum);
	if (lItem && rItem)
#ifdef ENABLE_MULTI_NAMES
		return strcmp(lItem->szLocaleName[DEFAULT_LANGUAGE], rItem->szLocaleName[DEFAULT_LANGUAGE]) < 0;
#else
		return strcmp(lItem->szLocaleName, rItem->szLocaleName) < 0;
#endif
	else
		return true;
}

bool ConvertToShopItemTable(IN CGroupNode* pNode, OUT TShopTableEx& shopTable)
{
	if (!pNode->GetValue("vnum", 0, shopTable.dwVnum))
	{
		sys_err("Group %s does not have vnum.", pNode->GetNodeName().c_str());
		return false;
	}

	if (!pNode->GetValue("name", 0, shopTable.name))
	{
		sys_err("Group %s does not have name.", pNode->GetNodeName().c_str());
		return false;
	}

	if (shopTable.name.length() >= SHOP_TAB_NAME_MAX)
	{
		sys_err("Shop name length must be less than %d. Error in Group %s, name %s", SHOP_TAB_NAME_MAX, pNode->GetNodeName().c_str(), shopTable.name.c_str());
		return false;
	}

	std::string stCoinType;
	if (!pNode->GetValue("cointype", 0, stCoinType))
	{
		stCoinType = "Gold";
	}

	if (IsEqualStr(stCoinType, "Gold"))
	{
		shopTable.coinType = SHOP_COIN_TYPE_GOLD;
	}
	else if (IsEqualStr(stCoinType, "SecondaryCoin"))
	{
		shopTable.coinType = SHOP_COIN_TYPE_SECONDARY_COIN;
	}
	else
	{
		sys_err("Group %s has undefine cointype(%s).", pNode->GetNodeName().c_str(), stCoinType.c_str());
		return false;
	}

	CGroupNode* pItemGroup = pNode->GetChildNode("items");
	if (!pItemGroup)
	{
		sys_err("Group %s does not have 'group items'.", pNode->GetNodeName().c_str());
		return false;
	}

	int itemGroupSize = pItemGroup->GetRowCount();
	std::vector <TShopItemTable> shopItems(itemGroupSize);
	if (itemGroupSize >= SHOP_HOST_ITEM_MAX_NUM)
	{
		sys_err("count(%d) of rows of group items of group %s must be smaller than %d", itemGroupSize, pNode->GetNodeName().c_str(), SHOP_HOST_ITEM_MAX_NUM);
		return false;
	}

	for (int i = 0; i < itemGroupSize; i++)
	{
		if (!pItemGroup->GetValue(i, "vnum", shopItems[i].vnum))
		{
			sys_err("row(%d) of group items of group %s does not have vnum column", i, pNode->GetNodeName().c_str());
			return false;
		}

		if (!pItemGroup->GetValue(i, "count", shopItems[i].count))
		{
			sys_err("row(%d) of group items of group %s does not have count column", i, pNode->GetNodeName().c_str());
			return false;
		}
		if (!pItemGroup->GetValue(i, "price", shopItems[i].price))
		{
			sys_err("row(%d) of group items of group %s does not have price column", i, pNode->GetNodeName().c_str());
			return false;
		}
	}
	std::string stSort;
	if (!pNode->GetValue("sort", 0, stSort))
	{
		stSort = "None";
	}

	if (IsEqualStr(stSort, "Asc"))
	{
		std::sort(shopItems.begin(), shopItems.end(), CompareShopItemName);
	}
	else if(IsEqualStr(stSort, "Desc"))
	{
		std::sort(shopItems.rbegin(), shopItems.rend(), CompareShopItemName);
	}

	CGrid grid = CGrid(5, 9);
	int iPos;

	memset(&shopTable.items[0], 0, sizeof(shopTable.items));

	for (size_t i = 0; i < shopItems.size(); i++)
	{
		TItemTable * item_table = ITEM_MANAGER::instance().GetTable(shopItems[i].vnum);
		if (!item_table)
		{
			sys_err("vnum(%d) of group items of group %s does not exist", shopItems[i].vnum, pNode->GetNodeName().c_str());
			return false;
		}

		iPos = grid.FindBlank(1, item_table->bSize);

		grid.Put(iPos, 1, item_table->bSize);
		shopTable.items[iPos] = shopItems[i];
	}

	shopTable.byItemCount = shopItems.size();
	return true;
}

bool CShopManager::ReadShopTableEx(const char* stFileName)
{
	// file 유무 체크.
	// 없는 경우는 에러로 처리하지 않는다.
	FILE* fp = fopen(stFileName, "rb");
	if (NULL == fp)
		return true;
	fclose(fp);

	CGroupTextParseTreeLoader loader;
	if (!loader.Load(stFileName))
	{
		sys_err("%s Load fail.", stFileName);
		return false;
	}

	CGroupNode* pShopNPCGroup = loader.GetGroup("shopnpc");
	if (NULL == pShopNPCGroup)
	{
		sys_err("Group ShopNPC is not exist.");
		return false;
	}

	typedef std::multimap <DWORD, TShopTableEx> TMapNPCshop;
	TMapNPCshop map_npcShop;
	for (int i = 0; i < pShopNPCGroup->GetRowCount(); i++)
	{
		DWORD npcVnum;
		std::string shopName;
		if (!pShopNPCGroup->GetValue(i, "npc", npcVnum) || !pShopNPCGroup->GetValue(i, "group", shopName))
		{
			sys_err("Invalid row(%d). Group ShopNPC rows must have 'npc', 'group' columns", i);
			return false;
		}
		std::transform(shopName.begin(), shopName.end(), shopName.begin(), (int(*)(int))std::tolower);
		CGroupNode* pShopGroup = loader.GetGroup(shopName.c_str());
		if (!pShopGroup)
		{
			sys_err("Group %s is not exist.", shopName.c_str());
			return false;
		}
		TShopTableEx table;
		if (!ConvertToShopItemTable(pShopGroup, table))
		{
			sys_err("Cannot read Group %s.", shopName.c_str());
			return false;
		}
		if (m_map_pkShopByNPCVnum.find(npcVnum) != m_map_pkShopByNPCVnum.end())
		{
			sys_err("%d cannot have both original shop and extended shop", npcVnum);
			return false;
		}

		map_npcShop.insert(TMapNPCshop::value_type(npcVnum, table));
	}

	for (TMapNPCshop::iterator it = map_npcShop.begin(); it != map_npcShop.end(); ++it)
	{
		DWORD npcVnum = it->first;
		TShopTableEx& table = it->second;
		if (m_map_pkShop.find(table.dwVnum) != m_map_pkShop.end())
		{
			sys_err("Shop vnum(%d) already exists", table.dwVnum);
			return false;
		}
		TShopMap::iterator shop_it = m_map_pkShopByNPCVnum.find(npcVnum);

		LPSHOPEX pkShopEx = NULL;
		if (m_map_pkShopByNPCVnum.end() == shop_it)
		{
			pkShopEx = M2_NEW CShopEx;
			pkShopEx->Create(0, npcVnum);
			m_map_pkShopByNPCVnum.insert(TShopMap::value_type(npcVnum, pkShopEx));
		}
		else
		{
			pkShopEx = dynamic_cast <CShopEx*> (shop_it->second);
			if (NULL == pkShopEx)
			{
				sys_err("WTF!!! It can't be happend. NPC(%d) Shop is not extended version.", shop_it->first);
				return false;
			}
		}

		if (pkShopEx->GetTabCount() >= SHOP_TAB_COUNT_MAX)
		{
			sys_err("ShopEx cannot have tab more than %d", SHOP_TAB_COUNT_MAX);
			return false;
		}

		if (pkShopEx->GetVnum() != 0 && m_map_pkShop.find(pkShopEx->GetVnum()) != m_map_pkShop.end())
		{
			sys_err("Shop vnum(%d) already exist.", pkShopEx->GetVnum());
			return false;
		}
		m_map_pkShop.insert(TShopMap::value_type (pkShopEx->GetVnum(), pkShopEx));
		pkShopEx->AddShopTable(table);
	}

	return true;
}
