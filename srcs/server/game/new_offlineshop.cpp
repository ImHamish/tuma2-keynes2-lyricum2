#include "stdafx.h"
#include "../common/tables.h"
#include "packet.h"
#include "item.h"
#include "char.h"
#include "item_manager.h"
#include "desc.h"
#include "char_manager.h"

#include "new_offlineshop.h"
#include "new_offlineshop_manager.h"

#ifdef __ENABLE_NEW_OFFLINESHOP__


namespace offlineshop
{
	/*
		structure
		-	contain item informations (offlineshop::SShopItemInfo)
		-	contain price (prezzo)
		-	contain window type (shop/shop safebox)

		methods
		-	constructor using LPITEM, price, window (immediatly set)
		-	constructor default (all to zero, alloc id)
		-	copy constructor (using in vector)

		-	gettable to get prototype table directly (very useful in filtering)
		-	getprice to get the sell price
		-	getinfo to get the item informations

		-	setprice to set sell price
		-	setinfo using LPITEM to get immediatly information from item
		-	setwindow to set the current window (shop/ shop safebox)
		-	getwindow to get the current window ||
	*/


	CShopItem::CShopItem(DWORD dwID) : m_itemInfo()
	{
		m_dwOwnerID = 0;
		m_dwID = dwID;
		m_byWindow = 0;

		ZeroObject(m_itemInfo);
		ZeroObject(m_priceInfo);
	}


	CShopItem::CShopItem(const CShopItem& rCopy) : m_itemInfo(), m_dwOwnerID(0)
	{
		m_dwID = rCopy.GetID();
		m_byWindow = rCopy.GetWindow();

		CopyObject(m_itemInfo, *rCopy.GetInfo());
		CopyObject(m_priceInfo, *rCopy.GetPrice());
	}

	CShopItem::CShopItem(LPITEM pItem, const TPriceInfo& sPrice, BYTE bWindowType, DWORD dwID):  m_itemInfo(), m_byWindow(), m_dwID(0)
	{
		//setting item window
		SetWindow(bWindowType);


		//setting price info
		CopyObject(m_priceInfo, sPrice);

		//setting info
		if (pItem)
		{
			//basic info
			m_itemInfo.dwCount	= pItem->GetCount();
			m_dwOwnerID			= pItem->GetOwner() ? pItem->GetOwner()->GetPlayerID() : 0;
			m_itemInfo.dwVnum	= pItem->GetVnum();
			//patch 08-03-2020
			m_itemInfo.expiration = GetItemExpiration(pItem);

			//attributes
			const TPlayerItemAttribute* pAttributes = pItem->GetAttributes();
			memcpy(m_itemInfo.aAttr, pAttributes, sizeof(m_itemInfo.aAttr));

			//sockets
			const long* pSockets = pItem->GetSockets();
			memcpy(m_itemInfo.alSockets, pSockets, sizeof(m_itemInfo.alSockets));

#if defined(ENABLE_CHANGELOOK)
			m_itemInfo.dwTransmutation = pItem->GetTransmutation();
#endif
#ifdef ATTR_LOCK
			m_itemInfo.iLockedAttr = pItem->GetLockedAttr();
#endif
		}

		else
		{
			sys_err("offlineshop::CShopItem - constructor using item/price/window : item == nullptr! ");
		}

		if (dwID != 0)
			m_dwID = dwID;
	}


	CShopItem::~CShopItem()
	{
	}

	bool CShopItem::GetTable(TItemTable ** ppTable) const
	{
		if ((*ppTable = ITEM_MANAGER::instance().GetTable(m_itemInfo.dwVnum)))
			return true;
		return false;
	}

	TPriceInfo* CShopItem::GetPrice() const
	{
		return const_cast<TPriceInfo*>(&m_priceInfo);
	}

	LPITEM CShopItem::CreateItem() const
	{
		LPITEM item = ITEM_MANAGER::instance().CreateItem(m_itemInfo.dwVnum, m_itemInfo.dwCount);
		if (!item)
			return nullptr;


		item->SetAttributes(m_itemInfo.aAttr);
		item->SetSockets(m_itemInfo.alSockets);
#if defined(ENABLE_CHANGELOOK)
		item->SetTransmutation(m_itemInfo.dwTransmutation);
#endif
#ifdef ATTR_LOCK
		item->SetLockedAttr(m_itemInfo.iLockedAttr);
#endif


		return item;
	}

	TItemInfoEx* CShopItem::GetInfo() const
	{
		return const_cast<TItemInfoEx*>(&m_itemInfo);
	}

	void CShopItem::SetInfo(LPITEM pItem)
	{
		if (pItem)
		{
			//basic info
			m_itemInfo.dwCount	= pItem->GetCount();
			m_dwOwnerID			= pItem->GetOwner() ? pItem->GetOwner()->GetPlayerID() : 0;
			m_itemInfo.dwVnum	= pItem->GetVnum();

			//attributes
			const TPlayerItemAttribute* pAttributes = pItem->GetAttributes();
			memcpy(m_itemInfo.aAttr, pAttributes, sizeof(m_itemInfo.aAttr));

			//sockets
			const long* pSockets = pItem->GetSockets();
			memcpy(m_itemInfo.alSockets, pSockets, sizeof(m_itemInfo.alSockets));
#if defined(ENABLE_CHANGELOOK)
			m_itemInfo.dwTransmutation = pItem->GetTransmutation();
#endif
#ifdef ATTR_LOCK
			m_itemInfo.iLockedAttr = pItem->GetLockedAttr();
#endif
		}

		else
		{
			sys_err("offlineshop::CShopItem - SetInfo: item == nullptr! ");
		}
	}


	void CShopItem::SetInfo(const TItemInfoEx& info)
	{
		CopyObject(m_itemInfo, info);
	}


	void CShopItem::SetPrice(const TPriceInfo& sPrice)
	{
		CopyObject(m_priceInfo, sPrice);
	}

	void CShopItem::SetWindow(BYTE byWin)
	{
		m_byWindow = byWin;
	}

	BYTE CShopItem::GetWindow() const
	{
		return m_byWindow;
	}

	DWORD CShopItem::GetID() const
	{
		return m_dwID;
	}


	void CShopItem::SetOwnerID(DWORD dwOwnerID)
	{
		m_dwOwnerID = dwOwnerID;
	}



	bool CShopItem::CanBuy(LPCHARACTER ch) const
	{
		if(!ch)
			return false;

		if (m_priceInfo.illYang > static_cast<long long>(ch->GetGold()) || m_priceInfo.illYang <= 0)
			return false;
#ifdef __ENABLE_CHEQUE_SYSTEM__
		if (m_priceInfo.iCheque > ch->GetCheque() || m_priceInfo.iCheque <= 0)
		{
			return false;
		}
#endif
		return true;
	}


	void CShopItem::operator=(const CShopItem & rItem)
	{
		m_dwID = rItem.GetID();
		m_byWindow = rItem.GetWindow();

		CopyObject(m_itemInfo, *rItem.GetInfo());
		CopyObject(m_priceInfo, *rItem.GetPrice());
	}









	/*
	CShop

	structure
	-	item pointers vector
	-	owner's player id
	-	duration
	-	offers vector
	-	guests list
	-	shop virtual id

	metodi
	-	getitems to get a pointer to items vector
	-	getoffers to get a pointer to offers vector
	-	getguests to get a pointer to the guests list
	-	notify offers to notify to character new offer

	-	getduration to know duration of the shop
	-	setduration used to set the initi duration a the boot
	-	decreaseduration to get and decrease duration sametime

	-	setownerpid to set the owner's pid
	-	getownerpid to get the owner's pid

	-	addguest to add a new guest to the shop
	-	removeguest to remove a guest to the shop (close board or logout)

	-	setitems used to set initial item on boot
	-	modifyitem to modify an item and refresh to guest
	-	buyitem guest , used to buy an item when you are looking the shop
	-	buyitem character, used to buy item when you are looking a filtered search
	-	removeitem to remove an item and send refresh to uests
	-	additem to add an item to the shop and send refresh to guests
	-	clear to delete the item pointer in vector item and remove all element into containers
	-	getitem to find item by virtualid
	-	findowner to use find by pid (char manager) to search owner id

	-	addoffer to add an offer and send notification at the owner (if online, otherwise it will recv it when login)
	-	(private) refresh list item to a guest or all (args != null -> send to one, otherwise send to all)
	-	(private) notifyoffer to send notify packet

	*/



	CShop::CShop()
	{
		m_dwPID = 0;
		m_dwDuration = 0;
		m_stName.clear();
		m_changeNamePulse = 0;
		m_editItemPricePulse = 0;
	}


	CShop::CShop(const CShop& rCopy)
	{
		CopyContainer(m_listGuests, *rCopy.GetGuests());
		CopyContainer(m_vecItems,   *rCopy.GetItems());
		CopyContainer(m_vecOffers,  *rCopy.GetOffers());

		m_dwPID			= rCopy.GetOwnerPID();
		m_dwDuration	= rCopy.GetDuration();
		m_stName		= rCopy.GetName();
		m_changeNamePulse = 0;
		m_editItemPricePulse = 0;
	}


	CShop::~CShop()
	{
	}




	CShop::VECSHOPITEM * CShop::GetItems() const
	{
		return const_cast<CShop::VECSHOPITEM*>(&m_vecItems);
	}

	CShop::VECSHOPITEM* CShop::GetItemsSold() const
	{
		return const_cast<CShop::VECSHOPITEM*>(&m_vecItemSold);
	}


	CShop::VECSHOPOFFER * CShop::GetOffers() const
	{
		return const_cast<CShop::VECSHOPOFFER*>(&m_vecOffers);
	}

	CShop::LISTGUEST * CShop::GetGuests() const
	{
		return const_cast<CShop::LISTGUEST*>(&m_listGuests);
	}


	void CShop::SetDuration(DWORD dwDuration)
	{
		m_dwDuration=dwDuration;
	}

	DWORD CShop::DecreaseDuration()
	{
		return --m_dwDuration;
	}

	DWORD CShop::GetDuration() const
	{
		return m_dwDuration;
	}



	void CShop::SetOwnerPID(DWORD dwOwnerPID)
	{
		m_dwPID = dwOwnerPID;
	}



	DWORD CShop::GetOwnerPID() const
	{
		return m_dwPID;
	}



	bool CShop::AddGuest(LPCHARACTER ch)
	{
		for (const auto& guest : m_listGuests)
		{
			if (guest == AS_GUESTID(ch))
				return false;
		}

		m_listGuests.emplace_back(AS_GUESTID(ch));

		return true;
	}



	bool CShop::RemoveGuest(LPCHARACTER ch)
	{
		for (auto it = m_listGuests.begin(); it != m_listGuests.end(); ++it)
		{
			if (*it == AS_GUESTID(ch) )
			{
				m_listGuests.erase(it);
				return true;
			}
		}

		return false;
	}



	void CShop::SetItems(const VECSHOPITEM* pVec)
	{
		CopyContainer(m_vecItems, *pVec);
		for (auto& vecItem : m_vecItems)
			vecItem.SetWindow(NEW_OFFSHOP);

		if (!m_listGuests.empty())
			__RefreshItems();
	}


	bool CShop::AddItem(CShopItem & rItem)
	{
		rItem.SetWindow(NEW_OFFSHOP);
		m_vecItems.push_back(rItem);
		__RefreshItems();
		return true;
	}

	bool CShop::AddItemSold(CShopItem & rItem)
	{
		rItem.SetWindow(NEW_OFFSHOP);
		m_vecItemSold.push_back(rItem);
		__RefreshItems();
		return true;
	}



	bool CShop::RemoveItem(DWORD dwItemID)
	{
		for (auto it = m_vecItems.begin(); it != m_vecItems.end(); ++it)
		{
			if (dwItemID == it->GetID())
			{
				m_vecItems.erase(it);
				__RefreshItems();
				return true;
			}
		}
		return false;
	}



	bool CShop::ModifyItem(DWORD dwItemID, const CShopItem & rItem)
	{
		if (rItem.GetID() != dwItemID)
		{
			sys_err( "have you forgot to set item id ? %d - %d don't match ",rItem.GetID(), dwItemID);
			return false;
		}


		CShopItem* pItem = nullptr;
		if (!GetItem(dwItemID, &pItem))
			return false;

		*pItem = rItem;
		pItem->SetWindow(NEW_OFFSHOP);
		__RefreshItems();
		return true;
	}




	bool CShop::BuyItem(DWORD dwItem)
	{
		CShopItem* pItem = nullptr;
		if (!GetItem(dwItem, &pItem))
			return false;


		m_vecItemSold.emplace_back(*pItem);
		RemoveItem(dwItem);
		return true;
	}

#if defined(ENABLE_OFFLINESHOP_REWORK)
	void CShop::SendNotificationClientPacket(DWORD dwItemID, long long dwItemPrice, WORD dwItemCount)
	{
		LPCHARACTER pkOwner = FindOwnerCharacter();
		if (!pkOwner->GetDesc())
			return;
		
		TPacketGCNewOfflineshop pack;
		pack.bHeader	= HEADER_GC_NEW_OFFLINESHOP;
		pack.bSubHeader	= SUBHEADER_GC_NOTIFICATION;
		pack.wSize		= sizeof(pack)+ sizeof(TSubPacketGCNotification);

		TSubPacketGCNotification subpack;
		subpack.dwItemID = dwItemID;
		subpack.dwItemPrice = dwItemPrice;
		subpack.dwItemCount = dwItemCount;

		pkOwner->GetDesc()->BufferedPacket(&pack, sizeof(pack));
		pkOwner->GetDesc()->Packet(&subpack, sizeof(subpack));
	}
#endif

	bool CShop::GetItem(DWORD dwItem, CShopItem** ppItem)
	{
		for (auto& item : m_vecItems)
		{
			if (dwItem == item.GetID())
			{
				*ppItem = &item;
				return true;
			}
		}

		return false;
	}



	bool CShop::GetItemSold(DWORD dwItem, CShopItem** ppItem)
	{
		for (auto& it : m_vecItemSold)
		{
			if (dwItem == it.GetID())
			{
				*ppItem = &it;
				return true;
			}
		}

		return false;
	}


	bool CShop::AddOffer(const TOfferInfo* pOfferInfo)
	{
		CShopItem* pitem = nullptr;
		if (!GetItem(pOfferInfo->dwItemID, &pitem))
			return false;

		m_vecOffers.push_back(*pOfferInfo);

		if (!pOfferInfo->bNoticed && !pOfferInfo->bAccepted)
		{
			//offlineshop-updated 04/08/19 reminder
			LPCHARACTER pkOwner = FindOwnerCharacter();
			if (pkOwner)
			{
				if (pkOwner->GetOfflineShop() && pkOwner->GetOfflineShop() == pkOwner->GetOfflineShopGuest())
				{
					NotifyOffers(pkOwner);
					GetManager().SendShopOpenMyShopClientPacket(pkOwner);
				}
			}
		}

		return true;
	}



	bool CShop::AcceptOffer(const TOfferInfo* pOffer)
	{
		CShopItem* pkItem = nullptr;
		if(!GetItem(pOffer->dwItemID, &pkItem))
			return false;

		TPriceInfo* pPrice = pkItem->GetPrice();
		CopyObject(*pPrice, pOffer->price);
		BuyItem(pkItem->GetID());



		for (offlineshop::TOfferInfo& offer : m_vecOffers)
		{
			if (offer.dwOfferID == offer.dwOwnerID)
			{
				offer.bAccepted = true;
				return true;
			}
		}

		

		return false;
	}



	void CShop::__RefreshItems(LPCHARACTER ch)
	{
		if(m_listGuests.empty())
			return;

		if (!ch)
		{
			auto it = m_listGuests.begin();
			while (it != m_listGuests.end())
			{
				const LPCHARACTER chGuest = AS_LPGUEST(*it);
				if(!chGuest)
				{
					it = m_listGuests.erase(it);
					continue;
				}

				if(chGuest->GetPlayerID() == m_dwPID)
				{
					GetManager().SendShopOpenMyShopClientPacket(chGuest);
				}
				else
				{
					GetManager().SendShopOpenClientPacket(chGuest , this);
				}

				++it;
			}
		}
		else
		{
			if(ch->GetPlayerID() == m_dwPID)
			{
				GetManager().SendShopOpenMyShopClientPacket(ch);
			}
			else
			{
				GetManager().SendShopOpenClientPacket(ch , this);
			}
		}
	}



	void CShop::Clear()
	{
		m_vecItems.clear();
		m_vecOffers.clear();
		m_listGuests.clear();
	}


	LPCHARACTER CShop::FindOwnerCharacter() const
	{
		return CHARACTER_MANAGER::instance().FindByPID(GetOwnerPID());
	}


	void CShop::NotifyOffers(LPCHARACTER ch)
	{
		for (auto& vecOffer : m_vecOffers)
		{
			if (!vecOffer.bNoticed && !vecOffer.bAccepted)
			{
				vecOffer.bNoticed = true;
				GetManager().SendShopOfferNotifiedDBPacket(vecOffer.dwOfferID, vecOffer.dwOwnerID);
			}
		}
	}

	void  CShop::NotifyAcceptedOffers(LPCHARACTER ch) //UNUSED
	{
		for (auto& vecOffer : m_vecOffers)
		{
			if (!vecOffer.bNoticed && vecOffer.bAccepted)
			{
				vecOffer.bNoticed = true;
				GetManager().SendShopOfferNotifiedDBPacket(vecOffer.dwOfferID, vecOffer.dwOwnerID);
			}
		}
	}



	void CShop::__SendOfferNotify(LPCHARACTER ch, TOfferInfo* pOffer)
	{
		//TODO : add send packet
	}



	const char* CShop::GetName() const
	{
		return m_stName.c_str();
	}


	void CShop::SetName(const char* pcszName)
	{
		m_stName = pcszName;
	}

	void CShop::RefreshToOwner() const
	{
		LPCHARACTER ch = FindOwnerCharacter();
		if(!ch)
			return;

		GetManager().SendShopOpenMyShopClientPacket(ch);
	}




	//AUCTION
	CAuction::CAuction()
	{
		ZeroObject(m_info);
		ZeroObject(m_bestOffer);
		m_dwBestBuyer=0;
	}


	CAuction::~CAuction()
	{
	}


	void CAuction::SetInfo(const TAuctionInfo& auction)
	{
		CopyObject(m_info, auction);
	}



	void CAuction::SetOffers(const std::vector<TAuctionOfferInfo>& vec)
	{
		CopyContainer(m_offersVec, vec);
	}




	bool CAuction::AddOffer(const TAuctionOfferInfo& offer)
	{
		m_offersVec.push_back(offer);
		__SetBestOffer();
		__RefreshToGuests();
		return true;
	}



	bool CAuction::AddGuest(LPCHARACTER ch)
	{
		for (const auto& guest : m_guestsList)
		{
			if (AS_GUESTID(ch) == guest)
				return false;
		}

		m_guestsList.emplace_back(AS_GUESTID(ch));
		ch->SetAuctionGuest(this);
		GetManager().SendAuctionOpenAuctionClientPacket(ch, m_info, m_offersVec);
		return true;
	}




	bool CAuction::RemoveGuest(LPCHARACTER ch)
	{
		for (auto it = m_guestsList.begin(); it != m_guestsList.end(); ++it)
		{
			if (AS_GUESTID(ch) == *it)
			{
				m_guestsList.erase(it);
				ch->SetAuctionGuest(nullptr);
				return true;
			}
		}

		return false;
	}




	void CAuction::DecreaseDuration()
	{
		if(m_info.dwDuration!=0)
			m_info.dwDuration--;
	}

	void CAuction::IncreaseDuration()
	{
		m_info.dwDuration++;
	}

	const TAuctionInfo& CAuction::GetInfo() const
	{
		return m_info;
	}


	const CAuction::AUCTION_OFFERVEC& CAuction::GetOffers() const
	{
		return m_offersVec;
	}


	const TPriceInfo& CAuction::GetBestOffer() const
	{
		return m_bestOffer;
	}



	CShop::LISTGUEST& CAuction::GetGuests()
	{
		return m_guestsList;
	}



	uint32_t CAuction::GetBestBuyer() const
	{
		return m_dwBestBuyer;
	}



	void CAuction::__RefreshToGuests()
	{
		auto it = m_guestsList.begin();
		while (it != m_guestsList.end())
		{
			const LPCHARACTER chGuest = AS_LPGUEST(*it);
			if (!chGuest)
			{
				it = m_guestsList.erase(it);
			}
			else
			{
				GetManager().SendAuctionOpenAuctionClientPacket(chGuest, m_info, m_offersVec);
				++it;
			}
		}
	}




	bool CAuction::__SetBestOffer()
	{
		const TPriceInfo* pInfo=nullptr;
		for (auto& [price, dwOwnerID, dwBuyerID, szBuyerName] : m_offersVec)
		{
			if (!pInfo)
			{
				pInfo = &(price);
				m_dwBestBuyer = dwBuyerID;
				continue;
			}
			if (pInfo->illYang < price.illYang)
			{
				pInfo = &price;
				m_dwBestBuyer = dwBuyerID;
			}
		}

		if (pInfo)
			CopyObject(m_bestOffer, *pInfo);


		return pInfo != nullptr;
	}



#ifdef ENABLE_NEW_SHOP_IN_CITIES

	ShopEntity::ShopEntity() : m_szName()
	{
		m_iType = 0;
		m_dwVID	= AllocID();
		m_pkShop= nullptr;
		CEntity::Initialize(ENTITY_NEWSHOPS);
	}


	//NEW ENTITY
	void ShopEntity::EncodeInsertPacket(LPENTITY entity)
	{
		const LPCHARACTER ch = entity->GetType() == ENTITY_CHARACTER ? dynamic_cast<LPCHARACTER>(entity) : nullptr;
		if (!ch || !ch->IsPC())
			return;

		GetManager().EncodeInsertShopEntity(*this, ch);
	}


	void ShopEntity::EncodeRemovePacket(LPENTITY entity)
	{
		const LPCHARACTER ch = entity->GetType() == ENTITY_CHARACTER ? dynamic_cast<LPCHARACTER>(entity) : nullptr;
		if (!ch || !ch->IsPC())
			return;

		GetManager().EncodeRemoveShopEntity(*this, ch);
	}


	const char* ShopEntity::GetShopName() const
	{
		return m_szName;
	}



	void ShopEntity::SetShopName(const char* name)
	{
		strncpy(m_szName, name , sizeof(m_szName));
	}

	int ShopEntity::GetShopType() const
	{
		return m_iType;
	}


	void ShopEntity::SetShopType(int iType)
	{
		m_iType=iType;
	}


	void ShopEntity::SetShop(offlineshop::CShop* pOfflineShop)
	{
		m_pkShop=pOfflineShop;
	}

#endif
}



#endif //__ENABLE_NEW_OFFLINESHOP__






