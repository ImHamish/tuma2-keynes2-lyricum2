#include "StdAfx.h"

#if defined(__ENABLE_NEW_OFFLINESHOP__)
#include <fstream>

#include "../GameLib/ItemManager.h"

#include "Packet.h"
#include "PythonNetworkStream.h"
#include "PythonTextTail.h"
#include "PythonSystem.h"
#include "PythonOfflineshop.h"

#define ApplyPyMethod(str, ...)		if(m_poWindow)PyCallClassMemberFunc(m_poWindow, str, __VA_ARGS__); else TraceError("OFFLINESHOP:: CANNOT CALL CLASS MEMBER %s",str);
#define GetPyLongLong(tuple,p)		if(!PyTuple_GetLongLong(tuple,		 iArg++,  (p))){	TraceError("%s : cant get %s ",__FUNCTION__, #p);return Py_BadArgument();}
#define GetPyInteger(tuple,p)		if(!PyTuple_GetInteger(tuple,		 iArg++,  (p))){	TraceError("%s : cant get %s ",__FUNCTION__, #p);return Py_BadArgument();}
#define GetPyDWORD(tuple,p)			if(!PyTuple_GetUnsignedLong(tuple,   iArg++,  (p))){	TraceError("%s : cant get %s ",__FUNCTION__, #p);return Py_BadArgument();}
#define GetPyString(tuple,p)		if(!PyTuple_GetString(tuple,		 iArg++,  (p))){	TraceError("%s : cant get %s ",__FUNCTION__, #p);return Py_BadArgument();}
#define GetPyObject(tuple,p)		if(!PyTuple_GetObject(tuple,		 iArg++,  (p))){	TraceError("%s : cant get %s ",__FUNCTION__, #p);return Py_BadArgument();}

#define NOARGS Py_BuildValue("()")
#define PutsError(fmt, ...) TraceError("In function %s line %d : " ##fmt , __FUNCTION__,__LINE__, __VA_ARGS__)
#define UNUSED_VAR(var) var;

std::string GetFormatString(const char* fmt, ...)
{
	static char szBuffer[300];
	va_list args;
	va_start(args, fmt);
		vsnprintf(szBuffer, sizeof(szBuffer), fmt, args);
	va_end(args);

	return std::string(szBuffer);
}

void CheckOfflineshopFolder() {
	DWORD attr = 0;
	if ((attr = ::GetFileAttributesA("offlineshop")) == INVALID_FILE_ATTRIBUTES ||
		attr != FILE_ATTRIBUTE_DIRECTORY)
		::CreateDirectoryA("offlineshop", 0);
}



CPythonOfflineshop::CPythonOfflineshop()
{
	m_poWindow = nullptr;
#ifdef ENABLE_NEW_SHOP_IN_CITIES
	m_bIsShowName = false;
#endif

	CheckOfflineshopFolder();
}

//starts
void CPythonOfflineshop::SetWindowObjectPointer(PyObject* poWindow)
{
	m_poWindow = poWindow;
}


PyObject* CPythonOfflineshop::GetOfflineshopBoard()
{
	return m_poWindow;
}


void CPythonOfflineshop::BuyFromSearch(DWORD dwOwnerID, DWORD dwItemID)
{
	ApplyPyMethod("SearchFilter_BuyFromSearch", Py_BuildValue("(ii)", dwOwnerID, dwItemID));
}



void CPythonOfflineshop::ShopListAddItem(const offlineshop::TShopInfo& shop)
{
	ApplyPyMethod("ShopListAddItem" , Py_BuildValue("(iiis)", shop.dwOwnerID, shop.dwDuration , shop.dwCount , shop.szName));
}


void CPythonOfflineshop::ShopListShow()
{
	ApplyPyMethod("ShopListShow" , NOARGS);
}

void CPythonOfflineshop::ShopListClear()
{
	ApplyPyMethod("ShopListClear" , NOARGS);
}


void CPythonOfflineshop::OpenShop(const offlineshop::TShopInfo& shop, const std::vector<offlineshop::TItemInfo>& vec)
{
	ApplyPyMethod("OpenShop" , Py_BuildValue("(iiis)", shop.dwOwnerID, shop.dwDuration , shop.dwCount , shop.szName));

	for (DWORD i = 0; i < vec.size(); i++)
	{
		const offlineshop::TItemInfo& item = vec[i];

		ApplyPyMethod("OpenShopItem_Alloc" , NOARGS);

		{
			ApplyPyMethod("OpenShopItem_SetValue",	Py_BuildValue("(sii)", "id",	i,	item.dwItemID));
			ApplyPyMethod("OpenShopItem_SetValue",	Py_BuildValue("(sii)", "vnum",	i,	item.item.dwVnum));
			ApplyPyMethod("OpenShopItem_SetValue",	Py_BuildValue("(sii)", "count",	i,	item.item.dwCount));
#if defined(ENABLE_CHANGELOOK)
			ApplyPyMethod("OpenShopItem_SetValue",	Py_BuildValue("(sii)", "trans",	i,	item.item.dwTransmutation));
#endif
#ifdef ATTR_LOCK
			ApplyPyMethod("OpenShopItem_SetValue", Py_BuildValue("(sii)", "locked_attr", i, item.item.iLockedAttr));
#endif

			for(int j =0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; j++)
				ApplyPyMethod("OpenShopItem_SetValue",	Py_BuildValue("(siiii)",	"attr",		i,	j, item.item.aAttr[j].bType , item.item.aAttr[j].sValue));

			for(int j=0; j < ITEM_SOCKET_SLOT_MAX_NUM; j++)
				ApplyPyMethod("OpenShopItem_SetValue",	Py_BuildValue("(siii)",		"socket",	i,	j, item.item.alSockets[j] ));

			ApplyPyMethod("OpenShopItem_SetValue",	Py_BuildValue("(siK)", "price",	 i,	item.price.illYang));
#ifdef ENABLE_CHEQUE_SYSTEM
			ApplyPyMethod("OpenShopItem_SetValue",	Py_BuildValue("(sii)", "cheque", i,	item.price.iCheque));
#endif
		}
	}

	ApplyPyMethod("OpenShop_End", NOARGS);
	
}

void CPythonOfflineshop::OpenShopOwner(
		const offlineshop::TShopInfo& shop, 
		const std::vector<offlineshop::TItemInfo>& vec , 
		const std::vector<offlineshop::TItemInfo>& solds,
		const std::vector<offlineshop::TOfferInfo>& offers
)
{
	ApplyPyMethod("OpenShopOwner_Start" , Py_BuildValue("(iiis)", shop.dwOwnerID, shop.dwDuration , shop.dwCount , shop.szName));

	for (DWORD i = 0; i < vec.size(); i++)
	{
		const offlineshop::TItemInfo& item = vec[i];

		ApplyPyMethod("OpenShopOwnerItem_Alloc" , NOARGS);

		{
			ApplyPyMethod("OpenShopOwnerItem_SetValue",	Py_BuildValue("(sii)", "id",	i,	item.dwItemID));
			ApplyPyMethod("OpenShopOwnerItem_SetValue",	Py_BuildValue("(sii)", "vnum",	i,	item.item.dwVnum));
			ApplyPyMethod("OpenShopOwnerItem_SetValue",	Py_BuildValue("(sii)", "count",	i,	item.item.dwCount));
#if defined(ENABLE_CHANGELOOK)
			ApplyPyMethod("OpenShopOwnerItem_SetValue",	Py_BuildValue("(sii)", "trans",	i,	item.item.dwTransmutation));
#endif
#ifdef ATTR_LOCK
			ApplyPyMethod("OpenShopOwnerItem_SetValue", Py_BuildValue("(sii)", "locked_attr", i, item.item.iLockedAttr));
#endif
			for(int j =0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; j++)
				ApplyPyMethod("OpenShopOwnerItem_SetValue",	Py_BuildValue("(siiii)",	"attr",		i,	j, item.item.aAttr[j].bType , item.item.aAttr[j].sValue));

			for(int j=0; j < ITEM_SOCKET_SLOT_MAX_NUM; j++)
				ApplyPyMethod("OpenShopOwnerItem_SetValue",	Py_BuildValue("(siii)",		"socket",	i,	j, item.item.alSockets[j] ));

			ApplyPyMethod("OpenShopOwnerItem_SetValue",	Py_BuildValue("(siK)", "price",	 i,	item.price.illYang));
#ifdef ENABLE_CHEQUE_SYSTEM
			ApplyPyMethod("OpenShopOwnerItem_SetValue", Py_BuildValue("(sii)", "cheque", i, item.price.iCheque));
#endif
		}


		ApplyPyMethod("OpenShopOwnerItem_Show" ,	NOARGS);
	}



	for (DWORD i = 0; i < solds.size(); i++)
	{
		const offlineshop::TItemInfo& item = solds[i];

		ApplyPyMethod("OpenShopOwnerItemSold_Alloc" , NOARGS);

		{
			ApplyPyMethod("OpenShopOwnerItemSold_SetValue",	Py_BuildValue("(sii)", "id",	i,	item.dwItemID));
			ApplyPyMethod("OpenShopOwnerItemSold_SetValue",	Py_BuildValue("(sii)", "vnum",	i,	item.item.dwVnum));
			ApplyPyMethod("OpenShopOwnerItemSold_SetValue",	Py_BuildValue("(sii)", "count",	i,	item.item.dwCount));
#if defined(ENABLE_CHANGELOOK)
			ApplyPyMethod("OpenShopOwnerItemSold_SetValue",	Py_BuildValue("(sii)", "trans",	i,	item.item.dwTransmutation));
#endif
#ifdef ATTR_LOCK
			ApplyPyMethod("OpenShopOwnerItemSold_SetValue", Py_BuildValue("(sii)", "locked_attr", i, item.item.iLockedAttr));
#endif
			for(int j =0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; j++)
				ApplyPyMethod("OpenShopOwnerItemSold_SetValue",	Py_BuildValue("(siiii)",	"attr",		i,	j, item.item.aAttr[j].bType , item.item.aAttr[j].sValue));

			for(int j=0; j < ITEM_SOCKET_SLOT_MAX_NUM; j++)
				ApplyPyMethod("OpenShopOwnerItemSold_SetValue",	Py_BuildValue("(siii)",		"socket",	i,	j, item.item.alSockets[j] ));

			ApplyPyMethod("OpenShopOwnerItemSold_SetValue",	Py_BuildValue("(siK)", "price",	 i,	item.price.illYang));
#ifdef ENABLE_CHEQUE_SYSTEM
			ApplyPyMethod("OpenShopOwnerItemSold_SetValue", Py_BuildValue("(sii)", "cheque", i, item.price.iCheque));
#endif
		}

	}


	for (DWORD i = 0; i < offers.size(); i++)
	{
		const offlineshop::TOfferInfo& offer = offers[i];

		//offlineshop-updated 03/08/19
		ApplyPyMethod("OpenShopOwner_SetOffer", Py_BuildValue("iiiKis",  offer.dwItemID, offer.dwOffererID, offer.dwOfferID, offer.price.illYang, offer.bAccepted?1:0, offer.szBuyerName));
	}



	ApplyPyMethod("OpenShopOwner_End" ,	NOARGS);
}

void CPythonOfflineshop::OpenShopOwnerNoShop()
{
	ApplyPyMethod("OpenShopOwnerNoShop" ,	NOARGS);
}


void CPythonOfflineshop::ShopClose()
{
	ApplyPyMethod("ShopClose" ,	NOARGS);
}


void CPythonOfflineshop::ShopFilterResult(const std::vector<offlineshop::TItemInfo>& vec)
{
	ApplyPyMethod("ShopFilterResult" , Py_BuildValue("(i)", vec.size()));

	for (DWORD i = 0; i < vec.size(); i++)
	{
		const offlineshop::TItemInfo& item = vec[i];

		ApplyPyMethod("ShopFilterResultItem_Alloc" , NOARGS);

		{
			ApplyPyMethod("ShopFilterResultItem_SetValue",	Py_BuildValue("(sii)", "id",	i,	item.dwItemID));
			ApplyPyMethod("ShopFilterResultItem_SetValue",	Py_BuildValue("(sii)", "vnum",	i,	item.item.dwVnum));
			ApplyPyMethod("ShopFilterResultItem_SetValue",	Py_BuildValue("(sii)", "count",	i,	item.item.dwCount));
#if defined(ENABLE_CHANGELOOK)
			ApplyPyMethod("ShopFilterResultItem_SetValue",	Py_BuildValue("(sii)", "trans",	i,	item.item.dwTransmutation));
#endif
#ifdef ATTR_LOCK
			ApplyPyMethod("ShopFilterResultItem_SetValue", Py_BuildValue("(sii)", "locked_attr", i, item.item.iLockedAttr));
#endif
			ApplyPyMethod("ShopFilterResultItem_SetValue",	Py_BuildValue("(sii)", "owner",	i,	item.dwOwnerID));
#if defined(ENABLE_OFFLINESHOP_REWORK)
			ApplyPyMethod("ShopFilterResultItem_SetValue",	Py_BuildValue("(sis)", "owner_name",	i,	item.szOwnerName));
#endif

			for(int j =0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; j++)
				ApplyPyMethod("ShopFilterResultItem_SetValue",	Py_BuildValue("(siiii)",	"attr",		i,	j, item.item.aAttr[j].bType , item.item.aAttr[j].sValue));

			for(int j=0; j < ITEM_SOCKET_SLOT_MAX_NUM; j++)
				ApplyPyMethod("ShopFilterResultItem_SetValue",	Py_BuildValue("(siii)",		"socket",	i,	j, item.item.alSockets[j] ));

			ApplyPyMethod("ShopFilterResultItem_SetValue",	Py_BuildValue("(siK)", "price",	 i,	item.price.illYang));
#ifdef ENABLE_CHEQUE_SYSTEM
			ApplyPyMethod("ShopFilterResultItem_SetValue", Py_BuildValue("(sii)", "cheque", i, item.price.iCheque));
#endif
		}
	}

	ApplyPyMethod("ShopFilterResult_Show" ,	NOARGS);
}



void CPythonOfflineshop::OfferListReceived(const std::vector<offlineshop::TOfferInfo>& offers, const std::vector<offlineshop::TMyOfferExtraInfo>& extras) //offlineshop-updated 03/08/19
{
	ApplyPyMethod("OfferList_Clear", NOARGS);

	for (DWORD i=0; i < offers.size(); i++)
	{
		const offlineshop::TOfferInfo& offer = offers[i];
		ApplyPyMethod("OfferList_AddOffer" , Py_BuildValue("(siiiiKii)",
			extras[i].szShopName, offer.dwOfferID, offer.dwOffererID, offer.dwOwnerID, offer.dwItemID, offer.price.illYang, offer.bNoticed, offer.bAccepted
		));

		OFFSHOP_DEBUG("offerlist recv offer accept? %s ",offer.bAccepted?"TREU":"FALSE");

		const offlineshop::TItemInfo& item			= extras[i].item;
		const offlineshop::TItemInfoEx& itemInfo	= item.item;

		//ApplyPyMethod("OfferList_AllocItem" , NOARGS);

		ApplyPyMethod("OfferList_ItemSetValue" ,		Py_BuildValue("(si)" ,		"id",		item.dwItemID));
		ApplyPyMethod("OfferList_ItemSetValue" ,		Py_BuildValue("(si)" ,		"owner",	item.dwOwnerID));
		ApplyPyMethod("OfferList_ItemSetValue" ,		Py_BuildValue("(si)" ,		"vnum",		itemInfo.dwVnum));
		ApplyPyMethod("OfferList_ItemSetValue" ,		Py_BuildValue("(si)" ,		"count",	itemInfo.dwCount));
#if defined(ENABLE_CHANGELOOK)
		ApplyPyMethod("OfferList_ItemSetValue" ,		Py_BuildValue("(si)" ,		"trans",	itemInfo.dwTransmutation));
#endif
#ifdef ATTR_LOCK
		ApplyPyMethod("OfferList_ItemSetValue",			Py_BuildValue("(si)",		"locked_attr", itemInfo.iLockedAttr));
#endif
		ApplyPyMethod("OfferList_ItemSetValue" ,		Py_BuildValue("(sK)" ,		"price",	item.price.illYang));


		for(int j=0; j < ITEM_SOCKET_SLOT_MAX_NUM; j++)
			ApplyPyMethod("OfferList_ItemSetValue" ,		Py_BuildValue("(sii)" ,	"socket",	j,		itemInfo.alSockets[j]));

		for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; j++)
		{
			int iType  = itemInfo.aAttr[j].bType;
			int iValue = itemInfo.aAttr[j].sValue;

			ApplyPyMethod("OfferList_ItemSetValue" ,		Py_BuildValue("(sii)" ,	"attr_type",	j,		iType));
			ApplyPyMethod("OfferList_ItemSetValue" ,		Py_BuildValue("(sii)" ,	"attr_value",	j,		iValue));
		}

	}


	OFFSHOP_DEBUG("offer list end ");
	ApplyPyMethod("OfferList_End", NOARGS);
}




void CPythonOfflineshop::SafeboxRefresh(const offlineshop::TValutesInfo& valute, const std::vector<DWORD>& ids, const std::vector<offlineshop::TItemInfoEx>& items)
{
	ApplyPyMethod("ShopSafebox_Clear", NOARGS);
#ifdef ENABLE_CHEQUE_SYSTEM
	ApplyPyMethod("ShopSafebox_SetValutes", Py_BuildValue("(Ki)", valute.illYang, valute.iCheque));
#else
	ApplyPyMethod("ShopSafebox_SetValutes", Py_BuildValue("(K)", valute.illYang));
#endif

	for (DWORD i = 0; i < ids.size(); i++)
	{
		const offlineshop::TItemInfoEx& itemInfo = items[i];

		ApplyPyMethod("ShopSafebox_AllocItem", NOARGS);
		
		ApplyPyMethod("ShopSafebox_SetValue" ,		Py_BuildValue("(si)" ,		"id",		ids[i]));
		ApplyPyMethod("ShopSafebox_SetValue" ,		Py_BuildValue("(si)" ,		"vnum",		itemInfo.dwVnum));
		ApplyPyMethod("ShopSafebox_SetValue" ,		Py_BuildValue("(si)" ,		"count",	itemInfo.dwCount));
		
#if defined(ENABLE_CHANGELOOK)
		ApplyPyMethod("ShopSafebox_SetValue" ,		Py_BuildValue("(si)" ,		"trans",	itemInfo.dwTransmutation));
#endif
#ifdef ATTR_LOCK
		ApplyPyMethod("ShopSafebox_SetValue" ,		Py_BuildValue("(si)" ,		"locked_attr",	itemInfo.iLockedAttr));
#endif

		for(int j=0; j < ITEM_SOCKET_SLOT_MAX_NUM; j++)
			ApplyPyMethod("ShopSafebox_SetValue" ,		Py_BuildValue("(sii)" ,	"socket",	j,		itemInfo.alSockets[j]));

		for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; j++)
		{
			int iType  = itemInfo.aAttr[j].bType;
			int iValue = itemInfo.aAttr[j].sValue;

			ApplyPyMethod("ShopSafebox_SetValue" ,		Py_BuildValue("(sii)" ,	"attr_type",	j,		iType));
			ApplyPyMethod("ShopSafebox_SetValue" ,		Py_BuildValue("(sii)" ,	"attr_value",	j,		iValue));
		}
	}


	ApplyPyMethod("ShopSafebox_RefreshEnd", NOARGS);
}



void CPythonOfflineshop::AuctionList(const std::vector<offlineshop::TAuctionListElement>& auctions, bool owner)
{
	ApplyPyMethod("AuctionList_Clear", NOARGS);

	OFFSHOP_DEBUG("Recv auctions size %u ",auctions.size());

	for (DWORD i = 0; i < auctions.size(); i++)
	{
		const offlineshop::TAuctionListElement& element = auctions[i];
		const offlineshop::TAuctionInfo&		auction	= element.auction;
		const offlineshop::TItemInfoEx&			item	= auction.item;

		ApplyPyMethod("AuctionList_Alloc", NOARGS);

		ApplyPyMethod("AuctionList_SetInto",	Py_BuildValue("(isiKKi)",	auction.dwOwnerID, auction.szOwnerName, auction.dwDuration, 
			auction.init_price.illYang,  element.actual_best.illYang,	element.dwOfferCount
		));

			
#if defined(ENABLE_CHANGELOOK)
		ApplyPyMethod("AuctionList_SetItemInfo", Py_BuildValue("(iii)" , item.dwVnum, item.dwCount, item.dwTransmutation));
#else
		ApplyPyMethod("AuctionList_SetItemInfo", Py_BuildValue("(ii)" , item.dwVnum, item.dwCount));
#endif
#ifdef ATTR_LOCK
		ApplyPyMethod("AuctionList_SetItemLockedAttr", Py_BuildValue("(i)", item.iLockedAttr));
#endif
		
		
		for(int i=0; i < ITEM_SOCKET_SLOT_MAX_NUM; i++)
			ApplyPyMethod("AuctionList_SetItemSocket", Py_BuildValue("(ii)", i, item.alSockets[i]));

		for (int i = 0; i < ITEM_ATTRIBUTE_SLOT_MAX_NUM; i++)
		{
			int type   = item.aAttr[i].bType;
			int value  = item.aAttr[i].sValue;

			ApplyPyMethod("AuctionList_SetItemAttribute", Py_BuildValue("(sii)", "type",  i , type));
			ApplyPyMethod("AuctionList_SetItemAttribute", Py_BuildValue("(sii)", "value", i , value));
		}
	}
	
	if (owner) {
		ApplyPyMethod("AuctionList_End_Owner", NOARGS);
	}
	else {
		ApplyPyMethod("AuctionList_End", NOARGS);
	}
}



void CPythonOfflineshop::MyAuctionOpen(const offlineshop::TAuctionInfo& auction, const std::vector<offlineshop::TAuctionOfferInfo>& offers)
{
	ApplyPyMethod("MyAuction_Clear", NOARGS);

	const offlineshop::TItemInfoEx& item = auction.item;
	ApplyPyMethod("MyAuction_SetInto",	Py_BuildValue("(isiK)",	auction.dwOwnerID, auction.szOwnerName, auction.dwDuration, auction.init_price.illYang));

	
#if defined(ENABLE_CHANGELOOK)
	ApplyPyMethod("MyAuction_SetItemInfo", Py_BuildValue("(iii)" , item.dwVnum, item.dwCount, item.dwTransmutation));
#else
	ApplyPyMethod("MyAuction_SetItemInfo", Py_BuildValue("(ii)" , item.dwVnum, item.dwCount));
#endif

#ifdef ATTR_LOCK
	ApplyPyMethod("MyAuction_SetItemLockedAttr", Py_BuildValue("(i)", item.iLockedAttr));
#endif

	for(int i=0; i < ITEM_SOCKET_SLOT_MAX_NUM; i++)
		ApplyPyMethod("MyAuction_SetItemSocket", Py_BuildValue("(ii)", i, item.alSockets[i]));

	for (int i = 0; i < ITEM_ATTRIBUTE_SLOT_MAX_NUM; i++)
	{
		int type   = item.aAttr[i].bType;
		int value  = item.aAttr[i].sValue;

		ApplyPyMethod("MyAuction_SetItemAttribute", Py_BuildValue("(sii)", "type",  i , type));
		ApplyPyMethod("MyAuction_SetItemAttribute", Py_BuildValue("(sii)", "value", i , value));
	}


	for (DWORD i = 0; i < offers.size(); i++)
	{
		const offlineshop::TAuctionOfferInfo& offer = offers[i];
		ApplyPyMethod("MyAuction_AddOffer", Py_BuildValue("(isiK)" , offer.dwBuyerID, offer.szBuyerName, offer.dwOwnerID, offer.price.illYang));
	}

	ApplyPyMethod("MyAuction_End", NOARGS);
}



void CPythonOfflineshop::AuctionOpen(const offlineshop::TAuctionInfo& auction, const std::vector<offlineshop::TAuctionOfferInfo>& offers)
{
	ApplyPyMethod("OpenAuction_Clear", NOARGS);

	const offlineshop::TItemInfoEx& item = auction.item;
	ApplyPyMethod("OpenAuction_SetInto",	Py_BuildValue("(isiK)",	auction.dwOwnerID, auction.szOwnerName, auction.dwDuration, auction.init_price.illYang));


#if defined(ENABLE_CHANGELOOK)
	ApplyPyMethod("OpenAuction_SetItemInfo", Py_BuildValue("(iii)" , item.dwVnum, item.dwCount, item.dwTransmutation));
#else
	ApplyPyMethod("OpenAuction_SetItemInfo", Py_BuildValue("(ii)" , item.dwVnum, item.dwCount));
#endif

#ifdef ATTR_LOCK
	ApplyPyMethod("OpenAuction_SetItemLockedAttr", Py_BuildValue("(i)", item.iLockedAttr));
#endif

	for(int i=0; i < ITEM_SOCKET_SLOT_MAX_NUM; i++)
		ApplyPyMethod("OpenAuction_SetItemSocket", Py_BuildValue("(ii)", i, item.alSockets[i]));

	for (int i = 0; i < ITEM_ATTRIBUTE_SLOT_MAX_NUM; i++)
	{
		int type   = item.aAttr[i].bType;
		int value  = item.aAttr[i].sValue;

		ApplyPyMethod("OpenAuction_SetItemAttribute", Py_BuildValue("(sii)", "type",  i , type));
		ApplyPyMethod("OpenAuction_SetItemAttribute", Py_BuildValue("(sii)", "value", i , value));
	}


	for (DWORD i = 0; i < offers.size(); i++)
	{
		const offlineshop::TAuctionOfferInfo& offer = offers[i];
		ApplyPyMethod("OpenAuction_AddOffer", Py_BuildValue("(isiK)" , offer.dwBuyerID, offer.szBuyerName, offer.dwOwnerID, offer.price.illYang));

	}

	ApplyPyMethod("OpenAuction_End", NOARGS);
}



void CPythonOfflineshop::OpenMyAuctionNoAuction()
{
	ApplyPyMethod("MyAuction_NoAuction", NOARGS);
}

void CPythonOfflineshop::RefreshItemNameMap()
{
	ApplyPyMethod("ClearItemNames", NOARGS);

	std::map<DWORD, std::string> nameMap;
	CItemManager::instance().GetItemsNameMap(nameMap);

	for(auto& it : nameMap)
		ApplyPyMethod("AppendItemName", Py_BuildValue("(is)", it.first, it.second.c_str()));

}

void CPythonOfflineshop::EnableGuiRefreshSymbol()
{
	ApplyPyMethod("BINARY_EnableRefreshSymbol", NOARGS);
}



void CPythonOfflineshop::ShopBuilding_AddInventoryItem(int iSlot)
{
	ApplyPyMethod("ShopBuilding_AddInventoryItem" , Py_BuildValue("(i)", iSlot));
}


void CPythonOfflineshop::AuctionBuilding_AddInventoryItem(int iSlot)
{
	ApplyPyMethod("AuctionBuilding_AddInventoryItem" , Py_BuildValue("(i)", iSlot));
}




void CPythonOfflineshop::ShopBuilding_AddItem(int iWin, int iSlot)
{
	ApplyPyMethod("ShopBuilding_AddItem" , Py_BuildValue("(ii)",iWin, iSlot));
}


void CPythonOfflineshop::AuctionBuilding_AddItem(int iWin, int iSlot)
{
	ApplyPyMethod("AuctionBuilding_AddItem" , Py_BuildValue("(ii)", iWin, iSlot));
}

#if defined(ENABLE_OFFLINESHOP_REWORK)
void CPythonOfflineshop::SendNotification(DWORD dwItemID, long long dwItemPrice, WORD dwItemCount)
{
	ApplyPyMethod("SendNotification" , Py_BuildValue("(iLi)", dwItemID, dwItemPrice, dwItemCount));
}
#endif

#ifdef ENABLE_NEW_SHOP_IN_CITIES
void CPythonOfflineshop::InsertEntity(DWORD dwVID, int iType, const char* szName, long x, long y, long z)
{
	RemoveEntity(dwVID);
	offlineshop::ShopInstance& shop = *(new offlineshop::ShopInstance());
	shop.SetVID(dwVID);
	shop.SetShopType(iType);
	shop.SetSign(szName);

	CPythonBackground&rkBgMgr=CPythonBackground::Instance();
	rkBgMgr.GlobalPositionToLocalPosition(x, y);


	OFFSHOP_DEBUG(" x %ld , y %ld, z %ld ",x,y,z);
	z = CPythonBackground::Instance().GetHeight(x, y) + 0.0f;
	shop.Show(x,y,z);

	std::string shopname = szName;
	size_t pos =0;

	if((pos=shopname.find('@')) != std::string::npos && ++pos != shopname.length())
		shopname = shopname.substr(pos);

	if (!CPythonSystem::instance().GetHideMode4Status())
	{
		CPythonTextTail::instance().RegisterShopInstanceTextTail(dwVID, shopname.c_str(), shop.GetThingInstancePtr());
	}
	else
	{
		shop.Hide();
	}

	m_vecShopInstance.push_back(&shop);
}



void CPythonOfflineshop::RemoveEntity(DWORD dwVID)
{
	for (auto it = m_vecShopInstance.begin(); it != m_vecShopInstance.end(); it++)
	{
		offlineshop::ShopInstance& shop = *(*it);
		if (shop.GetVID() == dwVID)
		{
			shop.Clear();
			//deleting texttail
			CPythonTextTail::Instance().DeleteShopTextTail(dwVID);
			delete(*it);

			m_vecShopInstance.erase(it);
			return;
		}
	}
}



void CPythonOfflineshop::RenderEntities()
{
	//OFFSHOP_DEBUG("rendering shops");

	for (auto & iter : m_vecShopInstance)
	{
		iter->Render();
		iter->BlendRender();
	}
}

void CPythonOfflineshop::UpdateEntities()
{
	//OFFSHOP_DEBUG("rendering shops");

	for (auto & iter : m_vecShopInstance)
	{
		iter->Update();
	}

}

//void CPythonOfflineshop::DeformEntities()
//{
//	//OFFSHOP_DEBUG("deforming shops");
//
//	for (auto & iter : m_vecShopInstance)
//	{
//		iter->Deform();
//	}
//
//}

bool CPythonOfflineshop::GetShowNameFlag()
{
	return m_bIsShowName;
}


void CPythonOfflineshop::SetShowNameFlag(bool flag)
{
	m_bIsShowName = flag;
}


void CPythonOfflineshop::DeleteEntities()
{
	for (auto& iter : m_vecShopInstance)
	{
		//iter->Clear(); put comment here
		CPythonTextTail::instance().DeleteShopTextTail(iter->GetVID());
		iter->Clear(); //move it here
		delete(iter);
	}

	m_vecShopInstance.clear();
}

void CPythonOfflineshop::HideEntities()
{
	for (const auto& iterator : m_vecShopInstance)
	{
		if (iterator->IsShow())
		{
			iterator->Hide();
			CPythonTextTail::Instance().DeleteShopTextTail(iterator->GetVID());
		}
	}
}

void CPythonOfflineshop::ShowEntities()
{
	for (const auto& iterator : m_vecShopInstance)
	{
		if (!iterator->IsShow())
		{
			iterator->ShowShop();
			std::string shopname = iterator->GetSign();
			size_t pos = 0;
			if ((pos = shopname.find('@')) != std::string::npos && ++pos != shopname.length())
				shopname = shopname.substr(pos);

			CPythonTextTail::instance().RegisterShopInstanceTextTail(iterator->GetVID(), shopname.c_str(), iterator->GetThingInstancePtr());
		}
	}
}
#endif




/*
	* Initializing Module
	* Used to send packet via CPythonNetworkStream
	* Converting the pyArg in right argument
*/

PyObject* offlineshopSendShopCreate(PyObject* poSelf, PyObject* poArgs)
{
	//eg		: offlineshop.SendShopCreate(name, duration, items)
	//items		: a tuple which containing iteminfo
	//iteminfo	: a tuple which containing ordered data (windowtype, position, price)

	char * pszName	= nullptr;
	DWORD dwDur		= 0;
	int iArg		= 0;


	PyObject* poTupleItems	= nullptr;

	GetPyString(poArgs,		&pszName);
	GetPyDWORD(poArgs,		&dwDur);
	GetPyObject(poArgs,		&poTupleItems);
	
	offlineshop::TShopInfo shopInfo;
	shopInfo.dwCount	= PyTuple_Size(poTupleItems);
	if (!shopInfo.dwCount || !dwDur)
		return Py_BuildNone();
	shopInfo.dwDuration	= dwDur;
	shopInfo.dwOwnerID	= 0;

	strncpy(shopInfo.szName , pszName, sizeof(shopInfo.szName));

	std::vector<offlineshop::TShopItemInfo> vec;
	offlineshop::TShopItemInfo temp;

	vec.reserve(shopInfo.dwCount);
	for (int i = 0; i < shopInfo.dwCount; i++)
	{
		iArg=i;
		PyObject* poItemInfo = nullptr;
		GetPyObject(poTupleItems, &poItemInfo);

		int iWindow=0, iPos=0;
		long long illYang=0;
		iArg=0;
			
		GetPyInteger(poItemInfo,	&iWindow);
		GetPyInteger(poItemInfo,	&iPos);
		GetPyLongLong(poItemInfo,	&illYang);
#ifdef ENABLE_CHEQUE_SYSTEM
		GetPyInteger(poItemInfo,	&temp.price.iCheque);
#endif

		temp.pos			= TItemPos((BYTE) iWindow, iPos);
		temp.price.illYang	= illYang;

		vec.push_back(temp);
	}
	
	CPythonNetworkStream::instance().SendOfflineshopShopCreate(shopInfo, vec);
	return Py_BuildNone();
}


PyObject* offlineshopSendChangeName(PyObject* poSelf , PyObject* poArgs)
{
	//eg		: offlineshop.SendChangeName(name)
	char * pszName	= nullptr;
	int iArg		= 0;
	GetPyString(poArgs,		&pszName);

	CPythonNetworkStream::instance().SendOfflineshopChangeName(pszName);
	return Py_BuildNone();
}

#if defined(ENABLE_OFFLINESHOP_REWORK)
PyObject* offlineshopSendExtendTime(PyObject* poSelf , PyObject* poArgs)
{
	//eg		: offlineshop.SendExtendTime(time)
	DWORD dwTime 	= 0;
	int iArg		= 0;
	GetPyDWORD(poArgs,		&dwTime);

	CPythonNetworkStream::instance().SendOfflineshopExtendTime(dwTime);
	return Py_BuildNone();
}
#endif

PyObject* offlineshopSendForceCloseShop(PyObject* poSelf , PyObject* poArgs)
{
	//eg		: offlineshop.SendShopForceClose()

	CPythonNetworkStream::instance().SendOfflineshopForceCloseShop();
	return Py_BuildNone();
}


PyObject* offlineshopSendRequestShopList(PyObject* poSelf , PyObject* poArgs)
{
	//eg		: offlineshop.SendRequestShopList()

	CPythonNetworkStream::instance().SendOfflineshopRequestShopList();
	return Py_BuildNone();
}


PyObject* offlineshopSendOpenShop(PyObject* poSelf , PyObject* poArgs)
{
	//eg		: offlineshop.SendOpenShop(uid)
	int iArg	=0;
	DWORD dwID	=0;

	GetPyDWORD(poArgs, &dwID);

	CPythonNetworkStream::Instance().SendOfflineshopOpenShop(dwID);
	return Py_BuildNone();
}


PyObject* offlineshopSendOpenShopOwner(PyObject* poSelf , PyObject* poArgs)
{
	//eg		: offlineshop.SendOpenShop()
	CPythonNetworkStream::Instance().SendOfflineshopOpenShopOwner();
	return Py_BuildNone();
}


PyObject* offlineshopSendBuyItem(PyObject* poSelf , PyObject* poArgs)
{
	//eg		: offlineshop.SendBuyItem(owner,itemid)
	int iArg	=0;
	DWORD dwOwnerID	=0, dwItemID=0;
	long long TotalPriceSeen = 0;

	GetPyDWORD(poArgs , &dwOwnerID);
	GetPyDWORD(poArgs , &dwItemID);
	GetPyLongLong(poArgs, &TotalPriceSeen);

	CPythonNetworkStream::instance().SendOfflineshopBuyItem(dwOwnerID, dwItemID, false, TotalPriceSeen);
	return Py_BuildNone();
}


PyObject* offlineshopSendBuyItemFromSearch(PyObject* poSelf , PyObject* poArgs)
{
	//eg		: offlineshop.SendBuyItem(owner,itemid)
	int iArg	=0;
	DWORD dwOwnerID	=0, dwItemID=0;
	long long TotalPriceSeen = 0;

	GetPyDWORD(poArgs , &dwOwnerID);
	GetPyDWORD(poArgs , &dwItemID);
	GetPyLongLong(poArgs, &TotalPriceSeen);

	CPythonNetworkStream::instance().SendOfflineshopBuyItem(dwOwnerID, dwItemID, true, TotalPriceSeen);
	return Py_BuildNone();
}


PyObject* offlineshopSendAddItem(PyObject* poSelf , PyObject* poArgs)
{
	//eg		: offlineshop.SendAddItem(window_type,  slot,  price)

	int iArg =0, iWindow=0, iSlot=0;
	long long illYang=0;
#ifdef ENABLE_CHEQUE_SYSTEM
	int iCheque = 0;
#endif
	GetPyInteger(poArgs, &iWindow);
	GetPyInteger(poArgs, &iSlot);
	GetPyLongLong(poArgs, &illYang);
#ifdef  ENABLE_CHEQUE_SYSTEM
	GetPyInteger(poArgs, &iCheque);
#endif //  ENABLE_CHEQUE_SYSTEM
	offlineshop::TShopItemInfo info;
	TItemPos temp((BYTE)iWindow, iSlot);

	info.price.illYang = illYang;
#ifdef ENABLE_CHEQUE_SYSTEM
	info.price.iCheque = iCheque;
#endif
	offlineshop::CopyObject(info.pos , temp);

	CPythonNetworkStream::instance().SendOfflineshopAddItem(info);
	return Py_BuildNone();
}


PyObject* offlineshopSendRemoveItem(PyObject* poSelf , PyObject* poArgs)
{
	//eg			: offlineshop.SendRemoveItem(itemid)

	int iArg =0;
	DWORD dwItemID=0;
	GetPyDWORD(poArgs, &dwItemID);

	CPythonNetworkStream::instance().SendOfflineshopRemoveItem(dwItemID);
	return Py_BuildNone();
}


PyObject* offlineshopSendEditItem(PyObject* poSelf , PyObject* poArgs)
{
	//eg			: offlineshop.SendEditItem(itemid, price)

	int iArg =0;
	long long illYang=0;
	DWORD dwItemID=0;
#ifdef ENABLE_CHEQUE_SYSTEM
	int iCheque = 0;
#endif
	GetPyDWORD(poArgs, &dwItemID);
	GetPyLongLong(poArgs, &illYang);
#ifdef  ENABLE_CHEQUE_SYSTEM
	GetPyInteger(poArgs, &iCheque);
#endif //  ENABLE_CHEQUE_SYSTEM
	offlineshop::TPriceInfo price;
	price.illYang = illYang;
#ifdef ENABLE_CHEQUE_SYSTEM
	price.iCheque = iCheque;
#endif
	CPythonNetworkStream::instance().SendOfflineShopEditItem(dwItemID,price);
	return Py_BuildNone();
}


PyObject* offlineshopSendFilterRequest(PyObject* poSelf , PyObject* poArgs)
{
	//eg			: offlineshop.SendFilterRequest(itemtype, itemsubtype, itemname, prices, levels, wearflag, attrs)
	
	//prices		: a tuple which contain the start price valutes and the end price valutes
	//levels		: a tuple which contain the start level and end level 
	//attrs			: a tuple which contain ITEM_ATTRIBUTE_SLOT_NORM_NUM tuples (which they contain : (type, value) attribute info)


	offlineshop::TFilterInfo info;
	
	char* szName=nullptr;
	PyObject* poPriceTuple=nullptr, *poLevelTuple=nullptr, *poAttrsTuple=nullptr;

	int iArg=0;
	long long illYangStart=0, illYangEnd=0;

	GetPyInteger(poArgs, &info.bType);
	GetPyInteger(poArgs, &info.bSubType);

	GetPyString(poArgs,	 &szName);
	GetPyObject(poArgs,  &poPriceTuple);
	GetPyObject(poArgs,  &poLevelTuple);
	GetPyDWORD( poArgs,  &info.dwWearFlag);
	GetPyObject(poArgs,  &poAttrsTuple);

	if (PyTuple_Size(poArgs) > iArg) {
		GetPyInteger(poArgs, &info.iRarity);
	}

	//reset args index to use different tuple
	iArg=0;
	GetPyLongLong(poPriceTuple, &illYangStart);
	GetPyLongLong(poPriceTuple, &illYangEnd);

	strncpy(info.szName, szName, sizeof(info.szName));

	info.priceStart.illYang	= illYangStart;
	info.priceEnd.illYang	= illYangEnd;

	//reset args index to use different tuple
	iArg=0;
	GetPyInteger(poLevelTuple, &info.iLevelStart);
	GetPyInteger(poLevelTuple, &info.iLevelEnd);

#if defined(ENABLE_OFFLINESHOP_REWORK)
	int bPlayerName = 0;
	GetPyInteger(poLevelTuple, &bPlayerName);
	info.bPlayerName = bPlayerName;
#endif

	//reset args index to use different tuple
	iArg=0;
	PyObject* poTupleAttrArr[ITEM_ATTRIBUTE_SLOT_NORM_NUM];

	for (int i = 0; i < ITEM_ATTRIBUTE_SLOT_NORM_NUM; i++)
		GetPyObject(poAttrsTuple , &poTupleAttrArr[i]);

	for (int i = 0; i < ITEM_ATTRIBUTE_SLOT_NORM_NUM; i++)
	{
		iArg = 0;
		int iValue = 0;

		GetPyInteger(poTupleAttrArr[i], &info.aAttr[i].bType);
		GetPyInteger(poTupleAttrArr[i], &iValue);

		info.aAttr[i].sValue = (short)iValue;
	}
	
	CPythonNetworkStream::instance().SendOfflineshopFilterRequest(info);
	return Py_BuildNone();
}


PyObject* offlineshopSendOfferCreate(PyObject* poSelf , PyObject* poArgs)
{
	//eg			: offlineshop.SendOfferCreate(ownerid, itemid, price)

	offlineshop::TOfferInfo offer;
	int iArg=0;
	long long illYang=0;
#ifdef ENABLE_CHEQUE_SYSTEM
	int iCheque = 0;
#endif
	GetPyDWORD(poArgs, &offer.dwOwnerID);
	GetPyDWORD(poArgs, &offer.dwItemID);
	GetPyLongLong(poArgs, &illYang);
#ifdef ENABLE_CHEQUE_SYSTEM
	GetPyInteger(poArgs, &iCheque);
#endif
	offer.price.illYang = illYang;
#ifdef ENABLE_CHEQUE_SYSTEM
	offer.price.iCheque = iCheque;
#endif

	CPythonNetworkStream::instance().SendOfflineshopOfferCreate(offer);
	return Py_BuildNone();
}


PyObject* offlineshopSendOfferAccept(PyObject* poSelf , PyObject* poArgs)
{
	//eg			: offlineshop.SendOfferCreate(offerid)
	int iArg=0;
	DWORD dwOfferID=0;

	GetPyDWORD(poArgs, &dwOfferID);
	
	CPythonNetworkStream::instance().SendOfflineshopOfferAccept(dwOfferID);
	return Py_BuildNone();
}


PyObject* offlineshopSendOfferCancel(PyObject* poSelf , PyObject* poArgs)
{
	//eg			: offlineshop.SendOfferCreate(offerid, ownerid)
	int iArg=0;
	DWORD dwOfferID=0;
	DWORD dwOwnerID=0;

	GetPyDWORD(poArgs, &dwOfferID);
	GetPyDWORD(poArgs, &dwOwnerID);
	
	CPythonNetworkStream::instance().SendOfflineshopOfferCancel(dwOfferID, dwOwnerID);
	return Py_BuildNone();
}


PyObject* offlineshopSendOfferListRequest(PyObject* poSelf , PyObject* poArgs)
{
	CPythonNetworkStream::instance().SendOfflineshopOfferListRequest();
	return Py_BuildNone();
}


PyObject* offlineshopSendSafeboxOpen(PyObject* poSelf , PyObject* poArgs)
{
	CPythonNetworkStream::instance().SendOfflineshopSafeboxOpen();
	return Py_BuildNone();
}


PyObject* offlineshopSendSafeboxGetItem(PyObject* poSelf , PyObject* poArgs)
{
	//eg			: offlineshop.SendSafeboxGetItem(itemid)
	int iArg=0;
	DWORD dwItemID=0;

	GetPyDWORD(poArgs, &dwItemID);
	CPythonNetworkStream::instance().SendOfflineshopSafeboxGetItem(dwItemID);
	return Py_BuildNone();
}


PyObject* offlineshopSendSafeboxGetValutes(PyObject* poSelf , PyObject* poArgs)
{
	//eg			: offlineshop.SendSafeboxGetValutes(amount)
	int iArg=0;
	long long illYang=0;
#ifdef ENABLE_CHEQUE_SYSTEM
	int iCheque = 0;
#endif
	GetPyLongLong(poArgs, &illYang);
#ifdef ENABLE_CHEQUE_SYSTEM
	GetPyInteger(poArgs, &iCheque);
#endif
	offlineshop::TValutesInfo info;
	info.illYang = illYang;
#ifdef ENABLE_CHEQUE_SYSTEM
	info.iCheque = iCheque;
#endif

	CPythonNetworkStream::instance().SendOfflineshopSafeboxGetValutes(info);
	return Py_BuildNone();
}


PyObject* offlineshopSendSafeboxClose(PyObject* poSelf , PyObject* poArgs)
{
	CPythonNetworkStream::instance().SendOfflineshopSafeboxOpen();
	return Py_BuildNone();
}


//AUCTION
PyObject* offlineshopSendAuctionListRequest(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream::instance().SendOfflineshopAuctionListRequest();
	return Py_BuildNone();
}


PyObject* offlineshopSendAuctionOpenAuction(PyObject* poSelf, PyObject* poArgs)
{
	int iArg=0;
	
	DWORD dwOwnerID=0;
	GetPyDWORD(poArgs, &dwOwnerID);

	CPythonNetworkStream::instance().SendOfflineshopAuctionOpen(dwOwnerID);
	return Py_BuildNone();
}

PyObject* offlineshopSendAuctionAddOffer(PyObject* poSelf, PyObject* poArgs)
{
	int iArg=0;

	DWORD dwOwnerID=0;
	long long illYang=0;
#ifdef ENABLE_CHEQUE_SYSTEM
	int iCheque = 0;
#endif
	GetPyDWORD(poArgs, &dwOwnerID);
	GetPyLongLong(poArgs, &illYang);
#ifdef ENABLE_CHEQUE_SYSTEM
	GetPyInteger(poArgs, &iCheque);
#endif
	offlineshop::TPriceInfo price;
	price.illYang = illYang;
#ifdef ENABLE_CHEQUE_SYSTEM
	price.iCheque = iCheque;
#endif

	CPythonNetworkStream::instance().SendOfflineshopAuctionAddOffer(dwOwnerID, price);
	return Py_BuildNone();
}

PyObject* offlineshopSendAuctionExitFrom(PyObject* poSelf, PyObject* poArgs)
{
	int iArg=0;

	DWORD dwOwnerID=0;

	GetPyDWORD(poArgs, &dwOwnerID);
	CPythonNetworkStream::instance().SendOfflineshopAuctionExitFrom(dwOwnerID);
	return Py_BuildNone();
}

PyObject* offlineshopSendAuctionCreate(PyObject* poSelf, PyObject* poArgs)
{
	int iWindowType=0, iPos=0;
	long long illYang=0;
	DWORD dwDuration=0;

	int iArg=0;

	GetPyInteger(poArgs, &iWindowType);
	GetPyInteger(poArgs, &iPos);
	GetPyLongLong(poArgs, &illYang);
	GetPyDWORD(poArgs , &dwDuration);

	offlineshop::TPriceInfo price;
	price.illYang = illYang;


	CPythonNetworkStream::instance().SendOfflineshopAuctionCreate(TItemPos(iWindowType, iPos), price, dwDuration);
	return Py_BuildNone();
}

PyObject* offlineshopSendAuctionOpenMy(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream::instance().SendOfflineshopAuctionOpenMy();
	return Py_BuildNone();
}




PyObject* offlineshopSendCloseBoard(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream::instance().SendOfflineshopCloseBoard();
	return Py_BuildNone();
}

PyObject* offlineshopRefreshItemNameMap(PyObject* poSelf, PyObject* poArgs)
{
	CPythonOfflineshop::instance().RefreshItemNameMap();
	return Py_BuildNone();
}


PyObject* offlineshopSetOfflineshopBoard(PyObject* poSelf , PyObject* poArgs)
{
	PyObject* poWin=nullptr;
	int iArg=0;

	GetPyObject(poArgs, &poWin);


	CPythonOfflineshop::instance().SetWindowObjectPointer(poWin);
	return Py_BuildNone();
}



PyObject* offlineshopGetOfflineshopBoard(PyObject* poSelf , PyObject* poArgs)
{
	PyObject* poInterface = CPythonOfflineshop::instance().GetOfflineshopBoard();
	
	if(!poInterface)
		return Py_BuildNone();

	Py_IncRef(poInterface);
	return poInterface;
}


PyObject* offlineshopShopBuilding_AddInventoryItem(PyObject* poSelf, PyObject* poArgs)
{
	int iArg=0,  iSlotIndex=0;
	GetPyInteger(poArgs , &iSlotIndex);

	CPythonOfflineshop::instance().ShopBuilding_AddInventoryItem(iSlotIndex);
	return Py_BuildNone();
}



PyObject* offlineshopAuctionBuilding_AddInventoryItem(PyObject* poSelf, PyObject* poArgs)
{
	int iArg=0,  iSlotIndex=0;
	GetPyInteger(poArgs , &iSlotIndex);

	CPythonOfflineshop::instance().AuctionBuilding_AddInventoryItem(iSlotIndex);
	return Py_BuildNone();
}



PyObject* offlineshopShopBuilding_AddItem(PyObject* poSelf, PyObject* poArgs)
{
	int iArg=0,  iSlotIndex=0, iWin =0;

	GetPyInteger(poArgs , &iWin);
	GetPyInteger(poArgs , &iSlotIndex);

	CPythonOfflineshop::instance().ShopBuilding_AddItem(iWin, iSlotIndex);
	return Py_BuildNone();
}



PyObject* offlineshopAuctionBuilding_AddItem(PyObject* poSelf, PyObject* poArgs)
{
	int iArg=0,  iSlotIndex=0, iWin=0;

	GetPyInteger(poArgs , &iWin);
	GetPyInteger(poArgs , &iSlotIndex);

	CPythonOfflineshop::instance().AuctionBuilding_AddItem(iWin, iSlotIndex);
	return Py_BuildNone();
}



#ifdef ENABLE_NEW_SHOP_IN_CITIES
PyObject* offlineshopHideShopNames(PyObject* poSelf, PyObject* poArgs)
{
	//CPythonOfflineshop::instance().SetShowNameFlag(false);
	return Py_BuildNone();
}



PyObject* offlineshopShowShopNames(PyObject* poSelf, PyObject* poArgs)
{
	//CPythonOfflineshop::instance().SetShowNameFlag(true);
	return Py_BuildNone();
}

PyObject* offlineshopShowShopEntities(PyObject* poSelf, PyObject* poArgs)
{
	CPythonOfflineshop::instance().ShowEntities();
	return Py_BuildNone();
}

PyObject* offlineshopHideShopEntities(PyObject* poSelf, PyObject* poArgs)
{
	CPythonOfflineshop::instance().HideEntities();
	return Py_BuildNone();
}
#endif

PyObject* offlineshopCloseMyAuction(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream::instance().SendOfflineshopCloseMyAuction();
	return Py_BuildNone();
}

void initofflineshop()
{
	static PyMethodDef s_methods[] =
	{
		//packets exchanging

		{ "SendShopCreate",				offlineshopSendShopCreate,				METH_VARARGS },
		{ "SendChangeName",				offlineshopSendChangeName,				METH_VARARGS },
#if defined(ENABLE_OFFLINESHOP_REWORK)
		{ "SendExtendTime",				offlineshopSendExtendTime,				METH_VARARGS },
#endif
		{ "SendForceCloseShop",			offlineshopSendForceCloseShop,			METH_VARARGS },
		{ "SendRequestShopList",		offlineshopSendRequestShopList,			METH_VARARGS },
		{ "SendOpenShop",				offlineshopSendOpenShop,				METH_VARARGS },
		{ "SendOpenShopOwner",			offlineshopSendOpenShopOwner,			METH_VARARGS },
		{ "SendBuyItem",				offlineshopSendBuyItem,					METH_VARARGS },
		{ "SendBuyItemFromSearch",		offlineshopSendBuyItemFromSearch,		METH_VARARGS },
		{ "SendAddItem",				offlineshopSendAddItem,					METH_VARARGS },
		{ "SendRemoveItem",				offlineshopSendRemoveItem,				METH_VARARGS },
		{ "SendEditItem",				offlineshopSendEditItem,				METH_VARARGS },
		{ "SendFilterRequest",			offlineshopSendFilterRequest,			METH_VARARGS },
		{ "SendOfferCreate",			offlineshopSendOfferCreate,				METH_VARARGS },
		{ "SendOfferAccept",			offlineshopSendOfferAccept,				METH_VARARGS },
		{ "SendOfferCancel",			offlineshopSendOfferCancel,				METH_VARARGS },
		{ "SendOfferListRequest",		offlineshopSendOfferListRequest,		METH_VARARGS },
		{ "SendSafeboxOpen",			offlineshopSendSafeboxOpen,				METH_VARARGS },
		{ "SendSafeboxGetItem",			offlineshopSendSafeboxGetItem,			METH_VARARGS },
		{ "SendSafeboxGetValutes",		offlineshopSendSafeboxGetValutes,		METH_VARARGS },
		{ "SendSafeboxClose",			offlineshopSendSafeboxClose,			METH_VARARGS },
		
		//AUCTION
		{ "SendAuctionListRequest",			offlineshopSendAuctionListRequest,		METH_VARARGS },
		{ "SendAuctionOpenAuction",			offlineshopSendAuctionOpenAuction,		METH_VARARGS },
		{ "SendAuctionAddOffer",			offlineshopSendAuctionAddOffer,			METH_VARARGS },
		{ "SendAuctionExitFrom",			offlineshopSendAuctionExitFrom,			METH_VARARGS },
		{ "SendAuctionCreate",				offlineshopSendAuctionCreate,			METH_VARARGS },
		{ "SendAuctionOpenMy",				offlineshopSendAuctionOpenMy,			METH_VARARGS },


		{ "SendCloseBoard",					offlineshopSendCloseBoard,				METH_VARARGS },

		//interfaces methods
		{ "RefreshItemNameMap",			offlineshopRefreshItemNameMap,				METH_VARARGS },
	
		{ "SetOfflineshopBoard",		offlineshopSetOfflineshopBoard,				METH_VARARGS },
		{ "GetOfflineshopBoard",		offlineshopGetOfflineshopBoard,				METH_VARARGS },

		//create shop methods
		{ "ShopBuilding_AddInventoryItem",		offlineshopShopBuilding_AddInventoryItem,		METH_VARARGS },
		{ "AuctionBuilding_AddInventoryItem",	offlineshopAuctionBuilding_AddInventoryItem,	METH_VARARGS },
		
		//create shop methods
		{ "ShopBuilding_AddItem",				offlineshopShopBuilding_AddItem,		METH_VARARGS },
		{ "AuctionBuilding_AddItem",			offlineshopAuctionBuilding_AddItem,	METH_VARARGS },

#ifdef ENABLE_NEW_SHOP_IN_CITIES
		{ "HideShopNames",						offlineshopHideShopNames,						METH_VARARGS },
		{ "ShowShopNames",						offlineshopShowShopNames,						METH_VARARGS },

		{ "ShowShopEntities",					offlineshopShowShopEntities,					METH_VARARGS },
		{ "HideShopEntities",					offlineshopHideShopEntities,					METH_VARARGS },
#endif

		{"CloseMyAuction", offlineshopCloseMyAuction, METH_VARARGS},
		{ NULL,									NULL,									NULL		 },
	};

	PyObject * poModule = Py_InitModule("offlineshop", s_methods);

	PyModule_AddIntConstant(poModule, "FILTER_ATTRIBUTE_NUM",				ITEM_ATTRIBUTE_SLOT_NORM_NUM);
	PyModule_AddIntConstant(poModule, "OFFLINESHOP_MAX_DAYS",				offlineshop::OFFLINESHOP_DURATION_MAX_DAYS);
	PyModule_AddIntConstant(poModule, "OFFLINESHOP_MAX_HOURS",				offlineshop::OFFLINESHOP_DURATION_MAX_HOURS);
	PyModule_AddIntConstant(poModule, "OFFLINESHOP_MAX_MINUTES",			offlineshop::OFFLINESHOP_DURATION_MAX_MINUTES);
	PyModule_AddIntConstant(poModule, "OFFLINESHOP_MAX_ITEM_NUM", offlineshop::OFFLINESHOP_MAX_ITEM_NUM);
}


CPythonOfflineshop::~CPythonOfflineshop()
{
	for(auto& iter : m_vecShopInstance) {
		delete(iter);
	}

	m_vecShopInstance.clear();
}




#endif
