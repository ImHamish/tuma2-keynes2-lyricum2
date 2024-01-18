#include "stdafx.h"
#include "../common/tables.h"
#include "packet.h"
#include "item.h"
#include "char.h"
#include "item_manager.h"
#include "desc.h"
#include "char_manager.h"
#include "banword.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "config.h"
#include "event.h"
#include "locale_service.h"
#include <fstream>

#include "sectree_manager.h"
#include "sectree.h"
#include "config.h"
#ifdef ENABLE_NEW_OFFLINESHOP_LOGS
#include "log.h"
#endif
#include "questmanager.h"
#include "new_offlineshop.h"
#include "new_offlineshop_manager.h"
#if defined(ENABLE_OFFLINESHOP_REWORK)
#include "p2p.h"
#endif

//#include <offlineshop/Offlineshop.h>

#if defined(DISABLE_OFFLINESHOP_STRICTRULES)
#define OFFSHOP_MAP_INDEX 30
//static std::set<uint32_t> s_allowed_maps = {1, 21, 41};
static std::set<uint32_t> s_allowed_maps = { OFFSHOP_MAP_INDEX };
#endif

static bool IsValidShopName(const char* str)
{
	if (!str || !*str)
		return false;

	if (strlen(str) < 1)
		return false;

	for (const char* tmp = str; *tmp; ++tmp)
	{
		if (isdigit(*tmp) || isalpha(*tmp))
			continue;

		switch (*tmp)
		{
		case ' ':
		case '_':
		case '-':
		case '.':
		case '!':
		case '@':
		case '#':
		case '$':
		case '%':
		case '^':
		case '&':
		case '*':
		case '(':
		case ')':
		case ',':
			continue;
		}

		return false;
	}

	return true;
}

#define __ENABLE_OFFLINESHOP_GM_PROTECTION__
#ifdef __ENABLE_OFFLINESHOP_GM_PROTECTION__
#define MIN_USE_OFFLINESHOP_GMLEVEL GM_IMPLEMENTOR
#define _IS_VALID_GM_LEVEL(ch) (ch->GetGMLevel() == GM_PLAYER || ch->GetGMLevel() >= MIN_USE_OFFLINESHOP_GMLEVEL)
#else
#define _IS_VALID_GM_LEVEL(ch) (true)
#endif

#define PI 3.14159265
#define RADIANS_PER_DEGREE (PI/180.0)
#define TORAD(a)	((a)*RADIANS_PER_DEGREE)

struct MapInfo {
	int map_index = 0;
	int32_t x,y,radius;
};

struct Range { 
	size_t min_count,max_count; 
	int32_t min_radius,max_radius; 
};


inline std::vector<MapInfo> GetMapInfo()
{
	std::vector<MapInfo> Val;

#if defined(DISABLE_OFFLINESHOP_STRICTRULES)
	MapInfo shop;
	shop.map_index = OFFSHOP_MAP_INDEX;
	shop.radius = 5000;
	shop.x = 181400;
	shop.y = 1176800;
	Val.emplace_back(shop);
#else
	MapInfo red_village;
	red_village.map_index = 1;
	red_village.radius = 2765;
	red_village.x = 474000;
	red_village.y = 954700;
	Val.emplace_back(red_village);

	MapInfo yellow_village;
	yellow_village.map_index = 21;
	yellow_village.radius = 2765;
	yellow_village.x = 63200;
	yellow_village.y = 166400;
	Val.emplace_back(yellow_village);

	MapInfo blue_village;
	blue_village.map_index = 41;
	blue_village.radius = 2765;
	blue_village.x = 959700;
	blue_village.y = 268900;
	Val.emplace_back(blue_village);
#endif

	return Val;
}

inline std::vector<Range> GetRanges()
{
	std::vector<Range> Val;

	Range low_count;
	low_count.min_count = 0;
	low_count.max_count = 25;
	low_count.min_radius = 0;
	low_count.max_radius = 1400;
	Val.emplace_back(low_count);

	Range medium_low_count;
	medium_low_count.min_count = low_count.max_count;
	medium_low_count.max_count = 40;
	medium_low_count.min_radius = 0;
	medium_low_count.max_radius = 2100;
	Val.emplace_back(medium_low_count);

	Range medium_count;
	medium_count.min_count = medium_low_count.max_count;
	medium_count.max_count = 60;
	medium_count.min_radius = 0;
	medium_count.max_radius = 2100;
	Val.emplace_back(medium_count);

	Range medium_high_count;
	medium_high_count.min_count = medium_count.max_count;
	medium_high_count.max_count = 80;
	medium_high_count.min_radius = 0;
	medium_high_count.max_radius = 2500;
	Val.emplace_back(medium_high_count);

	Range high_count;
	high_count.min_count = medium_high_count.max_count;
	high_count.max_count = 100;
	high_count.min_radius = 0;
	high_count.max_radius = 3000;
	Val.emplace_back(high_count);

	return Val;
}

const std::vector<MapInfo> g_Maps = GetMapInfo();


uint32_t Offlineshop_GetMapCount(){
	return g_Maps.size();
}

void Offlineshop_GetMapIndex(size_t index, int* out_index)
{
	*out_index = g_Maps[index].map_index;
}

bool Offlineshop_CheckPositionDistance(long x1, long y1, long x2, long y2){
	const double xoff = x1 > x2? x1 - x2: x2-x1;
	const double yoff = y1 > y2? y1 - y2: y2-y1;

	#define PITAGORA(c1,c2) sqrt((c1*c1)+(c2*c2))
	const double distance = PITAGORA(xoff,yoff);

	return (distance > 350.0);
}


void Offlineshop_GetRangeByCount(size_t ent_count, int32_t radius, int32_t* out_min, int32_t* out_max)
{
	static const auto RangeVector = GetRanges();

	for(const auto& RangeElm : RangeVector)
	{
		if(ent_count > RangeElm.min_count && ent_count < RangeElm.max_count)
		{
			*out_min = RangeElm.min_radius;
			*out_max = RangeElm.max_radius;
			return;
		}
	}

	*out_min = 0;
	*out_max = radius;
}

void Offlineshop_GetNewPos(int index, size_t ent_count, long* out_x, long* out_y)
{
	auto& mapInfo = g_Maps[index];

	int32_t min_ =0, max_=0;
	Offlineshop_GetRangeByCount(ent_count, mapInfo.radius, &min_, &max_);

	long random_distance	= number(min_, max_);
	long random_degree		= number(0, 360);
	
	long centerx = mapInfo.x;
	long centery = mapInfo.y;

	if (random_degree >= 0.0 && random_degree < 90.0)
	{
		*out_x = centerx + (random_distance*cos(TORAD(random_degree)));
		*out_y = centery + (random_distance*sin(TORAD(random_degree)));
	}

	else if (random_degree >= 90.0 && random_degree < 180.0)
	{
		const float beta = 180.0-random_degree;
		*out_x = centerx - (random_distance*cos(TORAD(beta)));
		*out_y = centery + (random_distance*sin(TORAD(beta)));
	}

	else if (random_degree >= 180.0 && random_degree < 270.0)
	{
		const float beta = 270.0-random_degree;
		*out_x = centerx - (random_distance*cos(TORAD(beta)));
		*out_y = centery - (random_distance*sin(TORAD(beta)));
	}

	else
	{
		const float beta = 360.0-random_degree;
		*out_x = centerx + (random_distance*cos(TORAD(beta)));
		*out_y = centery - (random_distance*sin(TORAD(beta)));
	}
}

#ifdef __ENABLE_NEW_OFFLINESHOP__
bool MatchWearFlag(DWORD dwWearFilter, DWORD dwWearTable)
{
	if(dwWearFilter==0)
		return true;


	static const DWORD flags[] = {
		ITEM_ANTIFLAG_MALE,
		ITEM_ANTIFLAG_FEMALE,
		ITEM_ANTIFLAG_WARRIOR,
		ITEM_ANTIFLAG_ASSASSIN,
		ITEM_ANTIFLAG_SURA,
		ITEM_ANTIFLAG_SHAMAN,
	};


	const size_t counts = sizeof(flags)/sizeof(DWORD);

	for(size_t i=0; i < counts; i++)
		if(IS_SET(dwWearFilter, flags[i]) &&!IS_SET(dwWearTable, flags[i]))
			return false;
	return true;
}


bool MatchAttributes(const TPlayerItemAttribute* pAttributesFilter,const TPlayerItemAttribute* pAttributesItem)
{
	for (int i = 0; i < ITEM_ATTRIBUTE_NORM_NUM; i++)
	{
		if(pAttributesFilter[i].bType == 0)
			continue;

		bool bFound=false;

		BYTE type	= pAttributesFilter[i].bType;
		int  val	= pAttributesFilter[i].sValue;


		for (int i = 0; i < ITEM_ATTRIBUTE_NORM_NUM; i++)
		{
			if (pAttributesItem[i].bType == type)
			{
				bFound = pAttributesItem[i].sValue >= val;
				break;
			}
		}

		if(!bFound)
			return false;
	}

	return true;
}


std::string StringToLower(const char* name, size_t len)
{
	std::string res;
	res.resize(len);

	for(size_t i=0; i < len; i++)
		res[i] = tolower(*(name + i));
	return res;
}

//topatch

//updated 25-01-2020
bool IsGoodSalePrice(const offlineshop::TPriceInfo price) {
#if defined(ENABLE_NEW_GOLD_LIMIT)
	if (price.illYang >= GOLD_MAX_MAX || price.illYang < 0) {
		return false;
	}
#else
	if (price.illYang >= GOLD_MAX || price.illYang < 0) {
		return false;
	}
#endif

#ifdef __ENABLE_CHEQUE_SYSTEM__
	else if (price.iCheque >= CHEQUE_MAX || price.iCheque < 0){
		return false;
	}
#endif

	else {
		return true;
	}
}




bool MatchItemName(std::string stName, const char* table , const size_t tablelen)
{
	std::string stTable= StringToLower(table, tablelen);
	return stTable.find(stName) != std::string::npos;
}

bool CheckCharacterActions(LPCHARACTER ch)
{
	if(!ch)
	{
		return false;
	}

	if (ch->IsDead())
		return false;

	if(ch->GetExchange())
	{
		return false;
	}


	if(ch->GetSafebox())
	{
		return false;
	}

	if(ch->GetMyShop())
	{
		return false;
	}


	if (ch->IsCubeOpen())
	{
		return false;
	}

	if (ch->IsOpenSafebox())
		return false;

#ifdef ENABLE_ACCE_SYSTEM
	if (ch->IsAcceOpen())
	{
		return false;
	}

#endif

#ifdef __ATTR_TRANSFER_SYSTEM__
	if (ch->IsAttrTransferOpen())
	{
		return false;
	}
#endif

#if defined(ENABLE_AURA_SYSTEM)
	if (ch->IsAuraRefineWindowOpen())
	{
		return false;
	}
#endif

#if defined(ENABLE_CHANGELOOK)
	if (ch->isChangeLookOpened())
	{
		return false;
	}
#endif

#if defined(USE_ATTR_6TH_7TH)
	if (ch->IsOpenAttr67Add())
	{
		return false;
	}
#endif

	return true;
}

long long GetTotalAmountFromPrice(const offlineshop::TPriceInfo& price)
{
	long long total =0;
	total += price.illYang;
	return total;
}

bool CheckNewAuctionOfferPrice(const offlineshop::TPriceInfo& price, const offlineshop::TPriceInfo& best)
{
	long long totalValueIn =0, totalValueBest=0;


	totalValueIn	= GetTotalAmountFromPrice(price);
	totalValueBest	= GetTotalAmountFromPrice(best);

	if(totalValueBest< 1000)
		totalValueBest += 1000;

	else
	{
		const float percentage = (float)OFFLINESHOP_AUCTION_RAISE_PERCENTAGE / 100.0f;
		totalValueBest += (long long)(((long double)(totalValueBest)) * percentage);
	}
	
	
	return totalValueIn >= totalValueBest;
}









namespace offlineshop
{
	EVENTINFO(offlineshopempty_info)
	{
		int empty;

		offlineshopempty_info()
			: empty(0)
		{
		}
	};

	EVENTFUNC(func_offlineshop_update_duration)
	{
		offlineshop::GetManager().UpdateShopsDuration();
		offlineshop::GetManager().UpdateAuctionsDuration();
		offlineshop::GetManager().ClearSearchTimeMap();
		offlineshop::GetManager().ClearOfferTimeMap();
		return OFFLINESHOP_DURATION_UPDATE_TIME;
	}


	offlineshop::CShopManager& GetManager()
	{
		return offlineshop::CShopManager::instance();
	}


	offlineshop::CShop * CShopManager::PutsNewShop(TShopInfo * pInfo)
	{
		OFFSHOP_DEBUG("puts new shop %s ", pInfo->szName);

		auto it = m_mapShops.insert(std::make_pair(pInfo->dwOwnerID, offlineshop::CShop())).first;
		offlineshop::CShop& rShop = it->second;

		rShop.SetDuration(pInfo->dwDuration);
		rShop.SetOwnerPID(pInfo->dwOwnerID);
		rShop.SetName(pInfo->szName);
#ifdef ENABLE_NEW_SHOP_IN_CITIES
		CreateNewShopEntities(rShop);
#endif
		return &rShop;
	}




	void CShopManager::PutsAuction(const TAuctionInfo& auction)
	{
		CAuction& obj = m_mapAuctions[auction.dwOwnerID];
		obj.SetInfo(auction);
	}




	void CShopManager::PutsAuctionOffer(const TAuctionOfferInfo& offer)
	{
		auto it = m_mapAuctions.find(offer.dwOwnerID);
		if (it == m_mapAuctions.end())
			return;

		CAuction& obj = it->second;
		obj.AddOffer(offer);
	}






	offlineshop::CShop* CShopManager::GetShopByOwnerID(DWORD dwPID) 
	{
		auto it=m_mapShops.find(dwPID);
		if(it == m_mapShops.end())
			return nullptr;

		return &(it->second);
	}


	offlineshop::CAuction* CShopManager::GetAuctionByOwnerID(DWORD dwPID)
	{
		auto it=m_mapAuctions.find(dwPID);
		if(it == m_mapAuctions.end())
			return nullptr;

		return &(it->second);
	}


	void CShopManager::RemoveSafeboxFromCache(DWORD dwOwnerID)
	{
		auto it = m_mapSafeboxs.find(dwOwnerID);
		if(it==m_mapSafeboxs.end())
			return;

		m_mapSafeboxs.erase(it);
	}



	void CShopManager::RemoveGuestFromShops(LPCHARACTER ch)
	{
		if(ch->GetOfflineShopGuest())
			ch->GetOfflineShopGuest()->RemoveGuest(ch);

		ch->SetOfflineShopGuest(nullptr);

		if(ch->GetOfflineShop())
			ch->GetOfflineShop()->RemoveGuest(ch);

		ch->SetOfflineShop(nullptr);
	}


	CShopManager::CShopManager()
	{
		offlineshopempty_info* info = AllocEventInfo<offlineshopempty_info>();
		m_eventShopDuration = event_create(func_offlineshop_update_duration, info, OFFLINESHOP_DURATION_UPDATE_TIME);
		m_vecCities.resize(Offlineshop_GetMapCount());
	}



	CShopManager::~CShopManager()
	{
		Destroy();
	}



	void CShopManager::Destroy()
	{
		//deleting event
		if(m_eventShopDuration)
			event_cancel(&m_eventShopDuration);

		m_eventShopDuration = nullptr;
		
		//clearing containers
		m_mapOffer.clear();
		m_mapSafeboxs.clear();
		m_mapShops.clear();

		
#ifdef ENABLE_NEW_SHOP_IN_CITIES
		for (offlineshop::CShopManager::TCityShopInfo& city : m_vecCities)
		{
			for (auto it = city.entitiesByPID.begin(); it != city.entitiesByPID.end(); it++)
				M2_DELETE(it->second);

			city.entitiesByPID.clear();
			city.entitiesByVID.clear();
		}

		m_vecCities.clear();
#endif

	}



#ifdef ENABLE_NEW_SHOP_IN_CITIES

	bool IsEmptyString(const std::string& st){
		return st.find_first_not_of(" \t\r\n") == std::string::npos;
	}


	bool CShopManager::__CanUseCity(size_t index)
	{
		int map_index=0;
		Offlineshop_GetMapIndex(index, &map_index);
		return SECTREE_MANAGER::instance().GetMap(map_index) != nullptr;
	}


	bool CShopManager::__CheckEntitySpawnPos(const long x, const long y, const TCityShopInfo& city)
	{
		const SHOPENTITIES_MAP& entitiesMap = city.entitiesByPID;
		

		for (auto it = entitiesMap.begin(); it != entitiesMap.end(); it++)
		{
			const ShopEntity& entity = *(it->second);
			const PIXEL_POSITION pos = entity.GetXYZ();

			if(!Offlineshop_CheckPositionDistance(pos.x, pos.y, x, y))
				return false;
		}

		return true;
	}


	void CShopManager::__UpdateEntity(const offlineshop::CShop& rShop)
	{
		auto it = m_vecCities.begin();
		for (; it != m_vecCities.end(); it++)
		{
			auto itMap = it->entitiesByPID.find(rShop.GetOwnerPID());
			if(itMap == it->entitiesByPID.end())
				continue;

			ShopEntity& ent = *(itMap->second);
			ent.SetShopName(rShop.GetName());

			if (ent.GetSectree())
				ent.ViewReencode();

#ifdef ENABLE_OFFLINESHOP_DEBUG
			else
			{
				fprintf(stderr, "cant find sectree for entity : name %s , pid %u ",ent.GetShopName(), ent.GetShop()->GetOwnerPID());
			}
#endif
		}
	}



	void CShopManager::CreateNewShopEntities(offlineshop::CShop& rShop)
	{
#define PI 3.14159265
#define RADIANS_PER_DEGREE (PI/180.0)
#define TORAD(a)	((a)*RADIANS_PER_DEGREE)

		int index=0;
		auto it = m_vecCities.begin();
		for (; it != m_vecCities.end(); it++, index++)
		{
			TCityShopInfo& city = *it;

			long shop_pos_x = 0, shop_pos_y=0;
			int iCheckCount =0;

			int map_index =0;
			Offlineshop_GetMapIndex(index, &map_index);

			size_t ent_count = it->entitiesByPID.size();

			do {
				Offlineshop_GetNewPos(index, ent_count, &shop_pos_x, &shop_pos_y);

			} while(!__CheckEntitySpawnPos(shop_pos_x, shop_pos_y, city) &&  iCheckCount++ < 10);


			

			LPSECTREE sectree = SECTREE_MANAGER::Instance().Get(map_index, shop_pos_x, shop_pos_y);

			if (sectree)
			{
				ShopEntity* pEntity = new ShopEntity();

				pEntity->SetShopName(rShop.GetName());

				pEntity->SetShopType(0);//TODO: add differents shop skins 
				pEntity->SetMapIndex(map_index);
				pEntity->SetXYZ(shop_pos_x, shop_pos_y, 0);
				pEntity->SetShop(&rShop);

				sectree->InsertEntity(pEntity);
				pEntity->UpdateSectree();

				city.entitiesByPID.insert(std::make_pair(rShop.GetOwnerPID(),	pEntity));
				city.entitiesByVID.insert(std::make_pair(pEntity->GetVID(),		pEntity));
			}
		}

	}




	void CShopManager::DestroyNewShopEntities(const offlineshop::CShop& rShop)
	{
		if (g_bAuthServer)
		{
			return;
		}

		auto it = m_vecCities.begin();
		for (; it != m_vecCities.end(); it++)
		{
			TCityShopInfo& city = *it;

			auto iter = city.entitiesByPID.find(rShop.GetOwnerPID());

			if (iter == city.entitiesByPID.end())
			{
//				sys_err("CANNOT FOUND NEW SHOP ENTITY : %u ",rShop.GetOwnerPID());
				continue;
			}

			ShopEntity* entity = iter->second;
			DWORD dwVID = entity->GetVID();

			if (entity->GetSectree())
			{
				entity->ViewCleanup();
				entity->GetSectree()->RemoveEntity(entity);
			}

			entity->Destroy();


			M2_DELETE(entity);
			city.entitiesByPID.erase(iter);
			city.entitiesByVID.erase(city.entitiesByVID.find(dwVID));
		}
	}





	void CShopManager::EncodeInsertShopEntity(ShopEntity& shop, LPCHARACTER ch)
	{
		if (!ch->GetDesc())
			return;
		
		TPacketGCNewOfflineshop pack;
		pack.bHeader	= HEADER_GC_NEW_OFFLINESHOP;
		pack.bSubHeader	= SUBHEADER_GC_INSERT_SHOP_ENTITY;
		pack.wSize		= sizeof(pack)+ sizeof(TSubPacketGCInsertShopEntity);

		const PIXEL_POSITION pos = shop.GetXYZ();

		TSubPacketGCInsertShopEntity subpack;
		subpack.dwVID = shop.GetVID();
		subpack.iType = shop.GetShopType();
		
		subpack.x = pos.x;
		subpack.y = pos.y;
		subpack.z = pos.z;

		strncpy(subpack.szName, shop.GetShopName(), sizeof(subpack.szName));

		ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
		ch->GetDesc()->Packet(&subpack, sizeof(subpack));
	}




	void CShopManager::EncodeRemoveShopEntity(ShopEntity& shop, LPCHARACTER ch)
	{
		if (!ch->GetDesc())
			return;
		
		TPacketGCNewOfflineshop pack;
		pack.bHeader	= HEADER_GC_NEW_OFFLINESHOP;
		pack.bSubHeader	= SUBHEADER_GC_REMOVE_SHOP_ENTITY;
		pack.wSize		= sizeof(pack)+ sizeof(TSubPacketGCRemoveShopEntity);

		TSubPacketGCRemoveShopEntity subpack;
		subpack.dwVID = shop.GetVID();

		ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
		ch->GetDesc()->Packet(&subpack, sizeof(subpack));
	}


#endif




	CShopSafebox* CShopManager::GetShopSafeboxByOwnerID(DWORD dwPID)
	{
		SAFEBOXMAP::iterator it = m_mapSafeboxs.find(dwPID);
		if(it == m_mapSafeboxs.end())
			return nullptr;
		return &(it->second);
	}


	//offers
	bool CShopManager::PutsNewOffer(const TOfferInfo* pInfo)
	{
		OFFERSMAP::iterator it= m_mapOffer.find(pInfo->dwOffererID);

		if (it == m_mapOffer.end())
			it = m_mapOffer.insert(std::make_pair(pInfo->dwOffererID, std::vector<TOfferInfo>())).first;

		else
		{
			auto itVec = it->second.begin();
			for (; itVec != it->second.end(); itVec++)
			{
				if(itVec->dwOfferID == pInfo->dwOfferID)
					return false;
			}
		}
		

		it->second.push_back(*pInfo);
		return true;
	}


	//db packets exchanging
	void CShopManager::SendShopBuyDBPacket(DWORD dwBuyerID, DWORD dwOwnerID, DWORD dwItemID)
	{
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader	= SUBHEADER_GD_BUY_ITEM;

		TSubPacketGDBuyItem subpack;
		subpack.dwGuestID	= dwBuyerID;
		subpack.dwOwnerID	= dwOwnerID;
		subpack.dwItemID	= dwItemID;

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		OFFSHOP_DEBUG("sending for shop %u and item %u (buyer %u) ",dwOwnerID, dwItemID, dwBuyerID);
#ifdef ENABLE_NEW_OFFLINESHOP_LOGS
		LogManager::instance().OfflineshopLog(dwOwnerID, dwItemID, "%u is buying the item", dwBuyerID);
#endif
		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
	}


	bool CShopManager::RecvShopBuyDBPacket(DWORD dwBuyerID, DWORD dwOwnerID,DWORD dwItemID)
	{
		OFFSHOP_DEBUG("buyer %u , owner %u , itemid %u ",dwBuyerID, dwOwnerID, dwItemID);

		CShop* pkShop = GetShopByOwnerID(dwOwnerID);
		if(!pkShop)
			return false;


		CShopItem* pItem = nullptr;
		if(!pkShop->GetItem(dwItemID, &pItem))
			return false;

		OFFSHOP_DEBUG("checked %s" , "successful");

		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwBuyerID);

		if (ch)
		{
			OFFSHOP_DEBUG("buyer is online , name %s , item id %u ",ch->GetName(), dwItemID);

			LPITEM pkItem = pItem->CreateItem();
			if (!pkItem)
			{
				sys_err("cannot create item ( dwItemID %u , dwVnum %u, dwShopOwner %u, dwBuyer %u ) ",dwItemID, pItem->GetInfo()->dwVnum, dwOwnerID, dwBuyerID );
				return false;
			}

			TItemPos pos;
			if (!ch->CanTakeInventoryItem(pkItem, &pos)) 
			{
				M2_DESTROY_ITEM(pkItem);

				CShopSafebox* pSafebox = ch->GetShopSafebox()? ch->GetShopSafebox() : GetShopSafeboxByOwnerID(ch->GetPlayerID());
				if (!pSafebox)
					return false;
				
				/*
				if(!pSafebox->AddItem(pItem))
					return false;
				*/

				SendShopSafeboxAddItemDBPacket(ch->GetPlayerID(), *pItem);
				SendChatPacket(ch, CHAT_PACKET_RECV_ITEM_SAFEBOX);
			}

			else
			{
				pkItem->AddToCharacter(ch, pos);
				ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
			}

#if defined(ENABLE_OFFLINESHOP_REWORK)
			LPCHARACTER tch = CHARACTER_MANAGER::instance().FindByPID(dwOwnerID);
			if (tch)
			{
				pkShop->SendNotificationClientPacket(pItem->GetInfo()->dwVnum, pItem->GetPrice()->illYang, pItem->GetInfo()->dwCount);
			}
			else
			{
				CCI* pkCCI = P2P_MANAGER::instance().FindByPID(dwOwnerID);
				if (pkCCI)
				{
					TPacketGGOfflineShopNotification p2p;
					p2p.bHeader = HEADER_GG_OFFLINE_SHOP_NOTIFICATION;
					p2p.ownerId = dwOwnerID;
					p2p.vnum = pItem->GetInfo()->dwVnum;
					p2p.price = pItem->GetPrice()->illYang;
					p2p.count = pItem->GetInfo()->dwCount;
					P2P_MANAGER::instance().Send(&p2p, sizeof(TPacketGGOfflineShopNotification));
				}
			}
#endif

			DWORD dwItemID = pItem->GetID();
			pkShop->BuyItem(dwItemID);
		}

		else
		{
			OFFSHOP_DEBUG("buyer isn't online , item removed %u (shop %u)",dwItemID, pkShop->GetOwnerPID());

			DWORD dwItemID = pItem->GetID();
			pkShop->BuyItem(dwItemID);
		}


		return true;
	}



	void CShopManager::SendShopEditItemDBPacket(DWORD dwOwnerID, DWORD dwItemID, const TPriceInfo& rPrice)
	{
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader	= SUBHEADER_GD_EDIT_ITEM;

		TSubPacketGDEditItem subpack;
		subpack.dwOwnerID	= dwOwnerID;
		subpack.dwItemID	= dwItemID;
		CopyObject(subpack.priceInfo , rPrice);

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		OFFSHOP_DEBUG("item id %u, owner shop %u",dwItemID, dwOwnerID);
#ifdef ENABLE_NEW_OFFLINESHOP_LOGS
		LogManager::instance().OfflineshopLog(dwOwnerID, dwItemID, "change the price of item to %lld yang "
#ifdef __ENABLE_CHEQUE_SYSTEM__
			" and %d cheque "
#endif
			, rPrice.illYang
#ifdef __ENABLE_CHEQUE_SYSTEM__
			, pPrice.iCheque
#endif
		);
#endif
		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
	}

	//topatch
	bool CShopManager::RecvShopEditItemClientPacket(LPCHARACTER ch, DWORD dwItemID, const TPriceInfo& price)
	{
		if (!ch)
		{
			return false;
		}

		if (!_IS_VALID_GM_LEVEL(ch))
		{
			return false;
		}

#if defined(ENABLE_ANTI_FLOOD)
		if (thecore_pulse() < ch->GetAntiFloodPulse(FLOOD_OFFSHOP_EDITITEM) + PASSES_PER_SEC(1))
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1710, "");
#else
			ch->ChatPacket(CHAT_TYPE_INFO, "You can do this once every second.");
#endif
			return false;
		}
#endif

		CShop* pkShop = ch->GetOfflineShop();
		if (pkShop == nullptr)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You don't have a private shop.");
			return true;
		}

		quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
		if (pPC == nullptr)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return true;
		}

		if (pPC->IsRunning())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return true;
		}

		if (!IsGoodSalePrice(price))
			return false;

		CShopItem* pItem	= nullptr;
		if(!pkShop->GetItem(dwItemID, &pItem))
			return false;

		const auto editItemPricePulse = pkShop->GetEditItemPricePulse();
		if (editItemPricePulse > get_global_time())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot edit the item price yet.");
			return true;
		}

		pkShop->SetEditItemPricePulse(get_global_time() + OFFLINESHOP_SECONDS_DELAY_EDIT_ITEM_PRICE);

		TPriceInfo* pPrice = pItem->GetPrice();

		//updated 25 - 01 - 2020 
#ifndef __ENABLE_CHEQUE_SYSTEM__
		if(price.illYang == pPrice->illYang)
		{
			return true;
		}
#endif

#if defined(ENABLE_ANTI_FLOOD)
		ch->SetAntiFloodPulse(FLOOD_OFFSHOP_EDITITEM, thecore_pulse());
#endif
		ch->SetOfflineShopUseTime();

		SendShopEditItemDBPacket(pkShop->GetOwnerPID(), dwItemID, price);
		return true;
	}



	void CShopManager::SendShopRemoveItemDBPacket(DWORD dwOwnerID, DWORD dwItemID)
	{
#if defined(DISABLE_OFFLINESHOP_STRICTRULES)
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader	= SUBHEADER_GD_REMOVE_ITEM;

		TSubPacketGDRemoveItem subpack;
		subpack.dwOwnerID	= dwOwnerID;
		subpack.dwItemID	= dwItemID;

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		OFFSHOP_DEBUG("owner %u , item %u ",dwOwnerID, dwItemID);
#ifdef ENABLE_NEW_OFFLINESHOP_LOGS
		LogManager::instance().OfflineshopLog(dwOwnerID, dwItemID, "the item is removed");
#endif
		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
#endif
	}

	bool CShopManager::RecvShopRemoveItemDBPacket(DWORD dwOwnerID, DWORD dwItemID)
	{
		CShop* pkShop = GetShopByOwnerID(dwOwnerID);
		if(!pkShop)
			return false;

		//topatch 29-10
		CheckOfferOnItem(dwOwnerID, dwItemID);

		OFFSHOP_DEBUG("owner %u , item %u", dwOwnerID, dwItemID);
		return pkShop->RemoveItem(dwItemID);
	}




	void CShopManager::SendShopAddItemDBPacket(DWORD dwOwnerID, const TItemInfo& rItemInfo)
	{
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader	= SUBHEADER_GD_ADD_ITEM;

		TSubPacketGDAddItem subpack;
		subpack.dwOwnerID	= dwOwnerID;
		CopyObject(subpack.itemInfo, rItemInfo);
		
		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		OFFSHOP_DEBUG("owner %u , item vnum %u , item count %u ",dwOwnerID, rItemInfo.item.dwVnum , rItemInfo.item.dwCount);
		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
	}


	bool CShopManager::RecvShopAddItemDBPacket(DWORD dwOwnerID, const TItemInfo& rItemInfo)
	{
		CShop* pkShop = GetShopByOwnerID(dwOwnerID);
		if(!pkShop)
			return false;

		CShopItem newItem(rItemInfo.dwItemID);
		newItem.SetInfo(rItemInfo.item);
		newItem.SetPrice(rItemInfo.price);
		newItem.SetOwnerID(rItemInfo.dwOwnerID);

		OFFSHOP_DEBUG("owner %u , item id %u ",dwOwnerID, rItemInfo.dwItemID);
		return pkShop->AddItem(newItem);
	}



	//SHOPS 
	void CShopManager::SendShopForceCloseDBPacket(DWORD dwPID)
	{
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader	= SUBHEADER_GD_SHOP_FORCE_CLOSE;

		TSubPacketGDShopForceClose subpack;
		subpack.dwOwnerID = dwPID;

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		OFFSHOP_DEBUG("shop %u ",dwPID);
		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
	}


	bool CShopManager::RecvShopForceCloseDBPacket(DWORD dwPID)
	{
		CShop* pkShop = GetShopByOwnerID(dwPID);
		if(!pkShop)
			return false;

		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwPID);
		
		if (ch)
			ch->SetOfflineShop(nullptr);
		

		CShop::LISTGUEST* guests = pkShop->GetGuests();
		
		for (auto& pid : *guests)
		{

			LPCHARACTER guest = AS_LPGUEST(pid);
			if (!guest) {
				continue;
			}

			if (ch && ch == guest)
				SendShopOpenMyShopNoShopClientPacket(guest);

			else
				SendShopListClientPacket(guest);

			guest->SetOfflineShopGuest(NULL);
		}


		std::set<DWORD> setPids;

		//offers check
		CShop::VECSHOPOFFER& vec = *pkShop->GetOffers();
		for (offlineshop::TOfferInfo& offer : vec)
		{
			DWORD buyer = offer.dwOffererID;

			auto itOffer = m_mapOffer.find(buyer);
			if (itOffer != m_mapOffer.end())
			{
				CShop::VECSHOPOFFER& buyerOffers = itOffer->second;
				for (auto itBuyer = buyerOffers.begin(); itBuyer != buyerOffers.end(); itBuyer++)
				{
					if (itBuyer->dwOfferID == offer.dwOfferID)
					{
						buyerOffers.erase(itBuyer);
						setPids.insert(buyer);
						break;
					}
				}
			}
		}

		for (auto pid : setPids)
		{
			LPCHARACTER chBuyer = CHARACTER_MANAGER::instance().FindByPID(pid);
			if (chBuyer)
				RecvOfferListRequestPacket(chBuyer);
		}



#ifdef ENABLE_NEW_SHOP_IN_CITIES
		DestroyNewShopEntities(*pkShop);
#endif
		pkShop->Clear();

		m_mapShops.erase(m_mapShops.find(pkShop->GetOwnerPID()));
		return true;
	}


	void CShopManager::SendShopLockBuyItemDBPacket(DWORD dwBuyerID, DWORD dwOwnerID,DWORD dwItemID, long long TotalPriceSeen)
	{
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader = SUBHEADER_GD_BUY_LOCK_ITEM;

		TSubPacketGDLockBuyItem subpack;
		subpack.dwGuestID = dwBuyerID;
		subpack.dwOwnerID = dwOwnerID;
		subpack.dwItemID  = dwItemID;
		subpack.TotalPriceSeen = TotalPriceSeen;

		TEMP_BUFFER buff;
		buff.write(&pack,	 sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		OFFSHOP_DEBUG("shop %u , buyer %u , item %u (size %u) ",dwOwnerID, dwBuyerID, dwItemID, buff.size());
		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
	}

	bool CShopManager::RecvShopLockedBuyItemDBPacket(DWORD dwBuyerID, DWORD dwOwnerID,DWORD dwItemID)
	{
		CShop* pkShop	= GetShopByOwnerID(dwOwnerID);
		LPCHARACTER ch	= CHARACTER_MANAGER::instance().FindByPID(dwBuyerID);

		if(!ch || !pkShop)
			return false;

		OFFSHOP_DEBUG("found shop %u ",dwBuyerID);

		CShopItem* pkItem = nullptr;
		if(!pkShop->GetItem(dwItemID, &pkItem))
			return false;
		
		OFFSHOP_DEBUG("found item %u",dwItemID);

		if(!pkItem->CanBuy(ch))
			return false;

		OFFSHOP_DEBUG("can buy %u",dwItemID);

		TPriceInfo* pPrice = pkItem->GetPrice();
		if (pPrice->illYang <= 0) {
			sys_err("CRITICAL: %u is trying to buy the item (%u) for %lld Yang!", dwBuyerID, dwItemID, pPrice->illYang);
			return false;
		}

#if defined(ENABLE_NEW_GOLD_LIMIT)
		ch->ChangeGold(-pPrice->illYang);
#else
		ch->PointChange(POINT_GOLD, -pPrice->illYang);
#endif
#ifdef __ENABLE_CHEQUE_SYSTEM__
		ch->PointChange(POINT_CHEQUE, -pPrice->iCheque);
#endif
#ifdef ENABLE_NEW_OFFLINESHOP_LOGS
		LogManager::instance().OfflineshopLog(dwOwnerID, dwItemID, "%u start buying the item for %lld", dwBuyerID, pPrice->illYang);
#endif
		SendShopBuyDBPacket(dwBuyerID, dwOwnerID, dwItemID);
		return true;
	}

	void CShopManager::SendShopCannotBuyLockedItemDBPacket(DWORD dwOwnerID, DWORD dwItemID) //topatch
	{
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader = SUBHEADER_GD_CANNOT_BUY_LOCK_ITEM;

		TSubPacketGDCannotBuyLockItem subpack;
		subpack.dwOwnerID = dwOwnerID;
		subpack.dwItemID = dwItemID;

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		OFFSHOP_DEBUG("item id %u, owner shop %u", dwItemID, dwOwnerID);
		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
	}


	bool CShopManager::RecvShopExpiredDBPacket(DWORD dwPID) //topatch
	{
		CShop* pkShop = GetShopByOwnerID(dwPID);
		if (!pkShop)
			return false;

		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwPID);

		if (ch)
			ch->SetOfflineShop(nullptr);


		//*getting the guest list before to remove the shop
		//*that is necessary to send the shop list packets
		CShop::LISTGUEST guests = *pkShop->GetGuests();


		std::set<DWORD> setPids;

		//offers check
		CShop::VECSHOPOFFER& vec = *pkShop->GetOffers();
		for (offlineshop::TOfferInfo& offer : vec)
		{
			//for each offer removing from buyer
			DWORD buyer = offer.dwOffererID;

			//searching buyer into map
			auto itOffer = m_mapOffer.find(buyer);
			if (itOffer != m_mapOffer.end())
			{
				//searching offer id in vec
				CShop::VECSHOPOFFER& buyerOffers = itOffer->second;
				for (auto itBuyer = buyerOffers.begin(); itBuyer != buyerOffers.end(); itBuyer++)
				{
					if (itBuyer->dwOfferID == offer.dwOfferID)
					{
						buyerOffers.erase(itBuyer);
						setPids.insert(buyer);
						break;
					}
				}
			}
		}

		for (auto pid : setPids)
		{
			LPCHARACTER chBuyer = CHARACTER_MANAGER::instance().FindByPID(pid);
			if (chBuyer)
				RecvOfferListRequestPacket(chBuyer);
		}



#ifdef ENABLE_NEW_SHOP_IN_CITIES
		DestroyNewShopEntities(*pkShop);
#endif
		pkShop->Clear();
		m_mapShops.erase(m_mapShops.find(pkShop->GetOwnerPID()));


		for (auto& guest : guests)
		{
			LPCHARACTER chGuest = AS_LPGUEST(guest);
			if (!chGuest)
				continue;

			if (ch && ch == chGuest)
				SendShopOpenMyShopNoShopClientPacket(chGuest);

			else
				SendShopListClientPacket(chGuest);

			chGuest->SetOfflineShopGuest(nullptr);
		}

		return true;
	}

 

	void CShopManager::SendShopCreateNewDBPacket(const TShopInfo& shop, std::vector<TItemInfo>& vec
	)
	{
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader	= SUBHEADER_GD_SHOP_CREATE_NEW;

		TSubPacketGDShopCreateNew subpack;
		CopyObject(subpack.shop, shop);
		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		for (auto& i : vec)
			buff.write(&i, sizeof(TItemInfo));

		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
	}


	bool CShopManager::RecvShopCreateNewDBPacket(const TShopInfo& shop, std::vector<TItemInfo>& vec)
	{
		OFFSHOP_DEBUG("shop %s , shop id %u ", shop.szName, shop.dwOwnerID);

		if(m_mapShops.find(shop.dwOwnerID)!= m_mapShops.end())
			return false;


		CShop newShop;
		newShop.SetOwnerPID(shop.dwOwnerID);
		newShop.SetDuration(shop.dwDuration);
		newShop.SetName(shop.szName);

		std::vector<CShopItem> items;
		items.reserve(vec.size());

		for (const offlineshop::TItemInfo& rItem : vec)
		{
			CShopItem shopItem(rItem.dwItemID);

			shopItem.SetOwnerID(rItem.dwOwnerID);
			shopItem.SetPrice(rItem.price);
			shopItem.SetInfo(rItem.item);

			OFFSHOP_DEBUG("item id %u , item vnum %u , item count %u ",rItem.dwItemID, rItem.item.dwVnum , rItem.item.dwCount);

			items.emplace_back(std::move(shopItem));
		}

		newShop.SetItems(&items);

		OFFSHOP_DEBUG("shop %s , shop id %u inserted into map (items count %d)", shop.szName, shop.dwOwnerID, shop.dwCount);
		SHOPMAP::iterator it = m_mapShops.insert(std::make_pair(newShop.GetOwnerPID(), newShop)).first;

#ifdef ENABLE_NEW_SHOP_IN_CITIES
		CreateNewShopEntities(it->second);
#endif

		

		LPCHARACTER chOwner = it->second.FindOwnerCharacter();
		if (chOwner)
		{
			chOwner->SetOfflineShop(&(it->second));
			chOwner->SetOfflineShopGuest(&(it->second));

			it->second.AddGuest(chOwner);
			SendShopOpenMyShopClientPacket(chOwner);
		}

		return true;
	}



	void CShopManager::SendShopChangeNameDBPacket(DWORD dwOwnerID, const char* szName)
	{
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader	= SUBHEADER_GD_SHOP_CHANGE_NAME;

		TSubPacketGDShopChangeName subpack;
		subpack.dwOwnerID	= dwOwnerID;
		strncpy(subpack.szName, szName, sizeof(subpack.szName));

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		OFFSHOP_DEBUG("shop id %u , name [%s]",dwOwnerID, szName);
		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
	}






	bool CShopManager::RecvShopChangeNameDBPacket(DWORD dwOwnerID, const char* szName)
	{
		CShop* pkShop = GetShopByOwnerID(dwOwnerID);
		if(!pkShop)
			return false;

		pkShop->SetName(szName);
		pkShop->RefreshToOwner();

#ifdef ENABLE_NEW_SHOP_IN_CITIES
		__UpdateEntity(*pkShop);
#endif

		OFFSHOP_DEBUG("id %u , name %s ",dwOwnerID, szName);
		return true;
	}

#if defined(ENABLE_OFFLINESHOP_REWORK)
	bool CShopManager::RecvShopExtendTimeClientPacket(LPCHARACTER ch, DWORD dwTime)
	{
		if(!ch || !ch->GetOfflineShop())
			return false;
		
		if (ch->GetGold() < 1000000)
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1712, "");
#else
			ch->ChatPacket(CHAT_TYPE_INFO, "You don't have enough Yang.");
#endif
			return false;
		}
	
		// patch with warp check
		ch->SetOfflineShopUseTime();
	
		CShop* pkShop = GetShopByOwnerID(ch->GetPlayerID());
		if(!pkShop)
			return false;

		if (pkShop->GetDuration() > OFFLINESHOP_DURATION_MAX_MINUTES-1440)
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1713, "");
#else
			ch->ChatPacket(CHAT_TYPE_INFO, "Your offline shop can't be extended more than 7 days.");
#endif
			return false;
		}

		SendShopExtendTimeDBPacket(ch->GetPlayerID(), dwTime);
#if defined(ENABLE_NEW_GOLD_LIMIT)
		ch->ChangeGold(-1000000);
#else
		ch->PointChange(POINT_GOLD, -1000000);
#endif
		return true;
	}

	void CShopManager::SendShopExtendTimeDBPacket(DWORD dwOwnerID, DWORD dwTime)
	{
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader	= SUBHEADER_GD_SHOP_EXTEND_TIME;

		TSubPacketGDShopExtendTime subpack;
		subpack.dwOwnerID	= dwOwnerID;
		subpack.dwTime = dwTime;

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		OFFSHOP_DEBUG("shop id %u , time %d",dwOwnerID, dwTime);
		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
	}
	
	bool CShopManager::RecvShopExtendTimeDBPacket(DWORD dwOwnerID, DWORD dwTime)
	{
		CShop* pkShop = GetShopByOwnerID(dwOwnerID);
		if(!pkShop)
			return false;

		pkShop->SetDuration(pkShop->GetDuration() + dwTime);
		pkShop->RefreshToOwner();

#ifdef ENABLE_NEW_SHOP_IN_CITIES
		__UpdateEntity(*pkShop);
#endif

		OFFSHOP_DEBUG("id %u , time %d ",dwOwnerID, dwTime);
		return true;
	}
#endif

	//OFFER
	void CShopManager::SendShopOfferNewDBPacket(const TOfferInfo& offer)
	{
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader	= SUBHEADER_GD_OFFER_CREATE;

		TSubPacketGDOfferCreate subpack;
		subpack.dwOwnerID	= offer.dwOwnerID;
		subpack.dwItemID	= offer.dwItemID;
		CopyObject(subpack.offer, offer);

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		OFFSHOP_DEBUG("offerer %u , shop %u ",offer.dwOffererID , offer.dwOwnerID);
		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
	}





	bool CShopManager::RecvShopOfferNewDBPacket(const TOfferInfo& offer)
	{
		CShop* pkShop = GetShopByOwnerID(offer.dwOwnerID);
		if(!pkShop)
			return false;

		OFFSHOP_DEBUG("offerer %u , shop %u ", offer.dwOffererID , offer.dwOwnerID);
		if(!pkShop->AddOffer(&offer))
			return false;

		if(!PutsNewOffer(&offer))
			return false;

		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(offer.dwOffererID);
		if (ch != nullptr)
			SendChatPacket(ch, CHAT_PACKET_OFFER_CREATE);

		return true;
	}





	void CShopManager::SendShopOfferNotifiedDBPacket(DWORD dwOfferID, DWORD dwOwnerID)
	{
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader	= SUBHEADER_GD_OFFER_NOTIFIED;

		TSubPacketGDOfferNotified subpack;
		subpack.dwOfferID	= dwOfferID;
		subpack.dwOwnerID	= dwOwnerID;

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
	}





	bool CShopManager::RecvShopOfferNotifiedDBPacket(DWORD dwOfferID, DWORD dwOwnerID)
	{
		CShop* pkShop = GetShopByOwnerID(dwOwnerID);
		if(!pkShop)
			return false;

		DWORD dwBuyer=0;

		//edit offer in shop
		CShop::VECSHOPOFFER* vec = pkShop->GetOffers();
		for (DWORD i = 0; i < vec->size(); i++)
		{
			TOfferInfo& offer = vec->at(i);
			if (offer.dwOfferID == dwOfferID)
			{
				OFFSHOP_DEBUG("notified offer successful %u , %u ",dwOfferID, dwOwnerID);

				offer.bNoticed = true;
				dwBuyer = offer.dwOffererID;
				break;
			}
		}

		if(dwBuyer==0)
			return false;

		OFFSHOP_DEBUG("searching dwBuyer %u in map",dwBuyer);

		//edit offer in map
		auto it = m_mapOffer.find(dwBuyer);
		if(it==m_mapOffer.end())
			return false;

		
		OFFSHOP_DEBUG("found buyer successful");

		//searching offer in vector
		CShop::VECSHOPOFFER& vecBuyer = it->second;

		for (auto itVec = vecBuyer.begin(); itVec != vecBuyer.end(); itVec++)
		{
			if(itVec->dwOfferID!=dwOfferID)
				continue;

			OFFSHOP_DEBUG("found offer successful");
			itVec->bNoticed=true;
			break;
		}


		return true;
	}





	void CShopManager::SendShopOfferAcceptDBPacket(const TOfferInfo& offer)
	{
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader	= SUBHEADER_GD_OFFER_ACCEPT;

		TSubPacketGDOfferNotified subpack;
		subpack.dwOwnerID	= offer.dwOwnerID;
		subpack.dwOfferID	= offer.dwOfferID;

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
	}





	void CShopManager::SendShopOfferCancelDBPacket(const TOfferInfo& offer)
	{
		
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader	= SUBHEADER_GD_OFFER_CANCEL;

		TSubPacketGDOfferCancel subpack;
		subpack.dwOwnerID	= offer.dwOwnerID;
		subpack.dwOfferID	= offer.dwOfferID;

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
	}





	bool CShopManager::RecvShopOfferCancelDBPacket(DWORD dwOfferID, DWORD dwOwnerID, bool isRemovingItem) //offlineshop-updated 05/08/19
	{
		OFFSHOP_DEBUG("dwOfferID : %u , dwOwnerID %u ",dwOfferID, dwOwnerID);

		CShop* pkShop = GetShopByOwnerID(dwOwnerID);
		if(!pkShop)
			return false;

		CShop::VECSHOPOFFER& vecOffers		= *pkShop->GetOffers();
		CShop::VECSHOPOFFER::iterator it	= vecOffers.begin();

		TOfferInfo * pInfo=nullptr;

		for (; it != vecOffers.end(); it++)
		{
			if (it->dwOfferID == dwOfferID)
			{
				pInfo = &(*it);
				break;
			}
		}

		if(!pInfo)
			return false;

		OFFSHOP_DEBUG("found offer successful : %u ",dwOfferID);


		DWORD dwBuyerID = pInfo->dwOffererID;
		vecOffers.erase(it);


		auto iter = m_mapOffer.find(dwBuyerID);
		if (iter != m_mapOffer.end())
		{
			OFFSHOP_DEBUG("removing offer from offer vector by buyer %u ",dwBuyerID);

			std::vector<TOfferInfo>& vec = iter->second;
			auto iterVec= vec.begin();

			for (; iterVec != vec.end(); iterVec++)
			{
				if (iterVec->dwOfferID == dwOfferID)
				{
					vec.erase(iterVec);
					break;
				}
			}
		}


		//offlineshop-updated 05/08/19
		if(iter->second.empty())
			m_mapOffer.erase(iter);


		if (!isRemovingItem)
		{
			LPCHARACTER chOwner = CHARACTER_MANAGER::Instance().FindByPID(dwOwnerID);
			if(chOwner && chOwner->GetOfflineShopGuest() && chOwner->GetOfflineShopGuest()==chOwner->GetOfflineShop())
				SendShopOpenMyShopClientPacket(chOwner);
		}


		LPCHARACTER chBuyer = CHARACTER_MANAGER::Instance().FindByPID(dwBuyerID);
		if (chBuyer && chBuyer->IsLookingOfflineshopOfferList())
			RecvOfferListRequestPacket(chBuyer);
		

		//end

		return true;
	}






	bool CShopManager::RecvShopOfferAcceptDBPacket(DWORD dwOfferID, DWORD dwOwnerID)
	{
		CShop* pkShop = GetShopByOwnerID(dwOwnerID);
		if(!pkShop)
			return false;

		CShop::VECSHOPOFFER& vecOffers		= *pkShop->GetOffers();
		CShop::VECSHOPOFFER::iterator it	= vecOffers.begin();

		TOfferInfo * pInfo=nullptr;

		for (; it != vecOffers.end(); it++)
		{
			if (it->dwOfferID == dwOfferID)
			{
				pInfo = &(*it);
				break;
			}
		}

		if(!pInfo)
			return false;

		pkShop->AcceptOffer(pInfo);

		//checking about owner refreshing info
		LPCHARACTER chOwner = CHARACTER_MANAGER::instance().FindByPID(pkShop->GetOwnerPID());
		if(chOwner && chOwner->GetOfflineShop()==pkShop && chOwner->GetOfflineShopGuest()==pkShop)
			SendShopOpenMyShopClientPacket(chOwner);



		//removing offer from offer by buyer
		auto itMap = m_mapOffer.find(pInfo->dwOffererID);
		if (itMap != m_mapOffer.end())
		{
			std::vector<TOfferInfo>& vec = itMap->second;
			auto itVec = vec.begin();

			for (; itVec != vec.end(); itVec++)
			{
				if (itVec->dwOfferID == dwOfferID)
				{
					//checking if buyer was on offerlist
					LPCHARACTER chBuyer = CHARACTER_MANAGER::instance().FindByPID(itVec->dwOffererID);
					itVec->bAccepted = true;

					if(chBuyer && chBuyer->IsLookingOfflineshopOfferList())
						RecvOfferListRequestPacket(chBuyer);

					break;
				}
			}
		}


		return true;
	}





	bool CShopManager::RecvShopSafeboxAddItemDBPacket(DWORD dwOwnerID, DWORD dwItemID, const TItemInfoEx& item)
	{
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwOwnerID);
		CShopSafebox* pkSafebox = ch && ch->GetShopSafebox() ? ch->GetShopSafebox() : GetShopSafeboxByOwnerID(dwOwnerID);

		if(!pkSafebox)
			return false;

		CShopItem shopItem(dwItemID);
		shopItem.SetInfo(item);
		shopItem.SetOwnerID(dwOwnerID);
		
		pkSafebox->AddItem(&shopItem);
		if(ch && ch->GetShopSafebox())
			pkSafebox->RefreshToOwner(ch);

		OFFSHOP_DEBUG("safebox owner %u , item %u ",dwOwnerID, dwItemID);
		return true;
	}



	bool CShopManager::SendShopSafeboxAddItemDBPacket(DWORD dwOwnerID, const CShopItem& item) {
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader = SUBHEADER_GD_SAFEBOX_ADD_ITEM;


		TSubPacketGDSafeboxAddItem subpack;
		subpack.dwOwnerID = dwOwnerID;
		CopyObject(subpack.item , *item.GetInfo());


		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
		//* add return true
		return true;
	}



	bool CShopManager::RecvShopSafeboxAddValutesDBPacket(DWORD dwOwnerID, const TValutesInfo& valute)
	{
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwOwnerID);
		CShopSafebox* pkSafebox = ch && ch->GetShopSafebox() ? ch->GetShopSafebox() : GetShopSafeboxByOwnerID(dwOwnerID);

		if(!pkSafebox)
			return false;

		
		pkSafebox->AddValute(valute);
		if(ch && ch->GetShopSafebox())
		{
			pkSafebox->RefreshToOwner(ch);
//			LogManager::Instance().CharLog(ch, 0, "OFFLINESHOP_SAFEBOX_RECEIVE_MONEY", logBuf);
		}
		else
		{
//			LogManager::Instance().CharLog(dwOwnerID, 0, 0, 0, "OFFLINESHOP_SAFEBOX_RECEIVE_MONEY", logBuf, "", 0);
		}
	
		return true;
	}





	void CShopManager::SendShopSafeboxGetItemDBPacket(DWORD dwOwnerID, DWORD dwItemID)
	{
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader	= SUBHEADER_GD_SAFEBOX_GET_ITEM;

		TSubPacketGDSafeboxGetItem subpack;
		subpack.dwOwnerID	= dwOwnerID;
		subpack.dwItemID	= dwItemID;

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		OFFSHOP_DEBUG(" owner % u , item %u ",dwOwnerID , dwItemID);
		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
	}



	void CShopManager::SendShopSafeboxGetValutesDBPacket(DWORD dwOwnerID, const TValutesInfo& valutes)
	{
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader	= SUBHEADER_GD_SAFEBOX_GET_VALUTES;

		TSubPacketGDSafeboxGetValutes subpack;
		subpack.dwOwnerID	= dwOwnerID;
		CopyObject(subpack.valute , valutes);

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP, 0, buff.read_peek(), buff.size());
	}


	bool CShopManager::RecvShopSafeboxLoadDBPacket(DWORD dwOwnerID, const TValutesInfo& valute, const std::vector<DWORD>& ids, const std::vector<TItemInfoEx>& items)
	{
		RemoveSafeboxFromCache(dwOwnerID);

		CShopSafebox::VECITEM vec;
		vec.reserve(ids.size());

		for (uint32_t i = 0; i < ids.size(); i++)
		{
			CShopItem item(ids[i]);
			item.SetInfo(items[i]);
			item.SetOwnerID(dwOwnerID);
			vec.emplace_back(std::move(item));
		}


		CShopSafebox safebox;
		safebox.SetItems(&vec);
		safebox.SetValuteAmount(valute);

		m_mapSafeboxs.emplace(dwOwnerID, std::move(safebox));
		return true;
	}


	//patch 08-03-2020
	bool CShopManager::RecvShopSafeboxExpiredItemDBPacket(DWORD dwOwnerID, DWORD dwItemID) {
		CShopSafebox* safebox = GetShopSafeboxByOwnerID(dwOwnerID);
		if (!safebox)
			return false;

		if (!safebox->RemoveItem(dwItemID))
			return false;

		safebox->RefreshToOwner();
		return true;
	}




	//AUCTION
	void CShopManager::SendAuctionCreateDBPacket(const TAuctionInfo& auction)
	{
		OFFSHOP_DEBUG("auction %u, name %s, duration %u ",auction.dwOwnerID, auction.szOwnerName, auction.dwDuration);

		TPacketGDNewOfflineShop pack;
		pack.bSubHeader = SUBHEADER_GD_AUCTION_CREATE;

		TSubPacketGDAuctionCreate subpack;
		CopyObject(subpack.auction , auction);

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP , 0 , buff.read_peek() , buff.size());
	}



	void CShopManager::SendAuctionAddOfferDBPacket(const TAuctionOfferInfo& offer)
	{
		TPacketGDNewOfflineShop pack;
		pack.bSubHeader = SUBHEADER_GD_AUCTION_ADD_OFFER;

		TSubPacketGDAuctionAddOffer subpack;
		CopyObject(subpack.offer , offer);

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP , 0 , buff.read_peek() , buff.size());
	}




	bool CShopManager::RecvAuctionCreateDBPacket(const TAuctionInfo& auction)
	{
		OFFSHOP_DEBUG("auction %u, name %s, duration %u ",auction.dwOwnerID, auction.szOwnerName, auction.dwDuration);

		//check if exist
		if(m_mapAuctions.find(auction.dwOwnerID) != m_mapAuctions.end())
			return false;

		//set info
		CAuction& obj = m_mapAuctions[auction.dwOwnerID];
		obj.SetInfo(auction);

		//check about owner
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(auction.dwOwnerID);
		if (ch)
		{
			ch->SetAuction(&obj);
			SendAuctionOpenAuctionClientPacket(ch, obj.GetInfo(), std::vector<TAuctionOfferInfo>());
		}

		return true;
	}



	bool CShopManager::RecvAuctionAddOfferDBPacket(const TAuctionOfferInfo& offer)
	{
		OFFSHOP_DEBUG("offer : %u auction, %u buyer",offer.dwOwnerID, offer.dwBuyerID);

		//check if exists
		auto it = m_mapAuctions.find(offer.dwOwnerID);
		if(it==m_mapAuctions.end())
			return false;

		//adding offer
		CAuction& obj = it->second;
		obj.AddOffer(offer);

		if (obj.GetInfo().dwDuration == 0)
			obj.IncreaseDuration();
		return true;
	}


	//updated 30/09/19
	bool CShopManager::RecvAuctionExpiredDBPacket(DWORD dwID)
	{
		OFFSHOP_DEBUG("id : %u",dwID);

		//temp container to kick guest
		CShop::LISTGUEST tempGuestList;

		//removing auction from map
		auto it = m_mapAuctions.find(dwID);
		if (it != m_mapAuctions.end())
		{
			CAuction& auct = it->second;

			OFFSHOP_DEBUG("found auction %u (guest count %u) ",dwID, auct.GetGuests().size());

			//set to null the character::auctionguest pointer
			CShop::LISTGUEST& guestList = auct.GetGuests();
			for (auto& itGuest : guestList)
			{
				LPCHARACTER chGuest = AS_LPGUEST(itGuest);
				if (!chGuest)
					continue;

				chGuest->SetAuctionGuest(NULL);

				OFFSHOP_DEBUG("removing guest from auction %s ", chGuest->GetName());
				tempGuestList.emplace_back(itGuest);
			}
			
			m_mapAuctions.erase(it);
		}
		
		DWORD dwOwnerID = 0;
		LPCHARACTER owner = CHARACTER_MANAGER::instance().FindByPID(dwID);
		if(owner)
			owner->SetAuction(nullptr);

		for (auto & itGuests : tempGuestList)
		{
			LPCHARACTER chGuest = AS_LPGUEST(itGuests);
			if(chGuest == nullptr)
				continue;

			RecvAuctionListRequestClientPacket(chGuest);
		}

		return true;
	}

	//client packets exchanging
	bool CShopManager::RecvShopCreateNewClientPacket(LPCHARACTER ch, TShopInfo& rShopInfo, std::vector<TShopItemInfo> & vec)
	{
		if(!ch || ch->GetOfflineShop())
			return false;

		if (!_IS_VALID_GM_LEVEL(ch))
			return false;

#if defined(DISABLE_OFFLINESHOP_STRICTRULES)
		if (s_allowed_maps.find(ch->GetMapIndex()) == s_allowed_maps.end())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Nu poti sa creezi shop in aceasta mapa, poti doar in bazar.");
			return false;
		}
#endif

		if (!ch->CanHandleItem()|| !CheckCharacterActions(ch))
		{
			SendChatPacket(ch,CHAT_PACKET_CANNOT_DO_NOW);
			return false;
		}
		
		if (vec.empty()) // fix shop no items
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Nu poti crea un shop fara obiecte.");
			return false;
		}

		if (vec.size() > OFFLINESHOP_MAX_ITEM_NUM)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Magazinul tau a atins numarul maxim de obiecte.");
			return false;
		}

		quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
		if (pPC == nullptr)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return false;
		}

		if (pPC->IsRunning())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return false;
		}

		OFFSHOP_DEBUG("ch name %s , item count %u , duration %u ", ch->GetName(), rShopInfo.dwCount, rShopInfo.dwDuration);

		static char szNameChecked[OFFLINE_SHOP_NAME_MAX_LEN];

		//cheching about bandword
		strncpy(szNameChecked, rShopInfo.szName, sizeof(szNameChecked));
		if (CBanwordManager::instance().CheckString(szNameChecked, strlen(szNameChecked)))
			return false;

		if (!IsValidShopName(szNameChecked))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't give your shop an invalid name.");
			return false;
		}

		ch->SetOfflineShopUseTime();

		//making full name
		snprintf(rShopInfo.szName, sizeof(rShopInfo.szName), "%s@%s" , ch->GetName(), szNameChecked );
		
		std::vector<TItemInfo> vecItem;
		vecItem.reserve(vec.size());

		rShopInfo.dwOwnerID = ch->GetPlayerID();
		TItemInfo itemInfo;

		for (DWORD i = 0; i < vec.size(); i++)
		{
			TShopItemInfo& rShopItem = vec[i];
#if defined(ENABLE_NEW_GOLD_LIMIT)
			if (rShopItem.price.illYang >= GOLD_MAX_MAX || rShopItem.price.illYang < 0)
#else
			if (rShopItem.price.illYang >= GOLD_MAX || rShopItem.price.illYang < 0)
#endif
			{
				return false;
			}

#ifdef ENABLE_SWITCHBOT_WORLDARD
			if (rShopItem.pos.IsSwitchBotPosition())
			{
				return false;
			}
#endif
			if (rShopItem.pos.IsDragonSoulEquipPosition())
			{
				return false;
			}

			LPITEM item = ch->GetItem(rShopItem.pos);
			if(!item)
				return false;
			
			if(IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE))
				return false;
			
			if(IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MYSHOP))
				return false;
			
			if (item->isLocked() || item->IsEquipped() || item->IsExchanging())
			{
				SendChatPacket(ch,CHAT_PACKET_CANNOT_DO_NOW);
				return true;
			}
			
#ifdef ENABLE_SOULBIND_SYSTEM
			if (item->IsSealed()){
				SendChatPacket(ch, CHAT_PACKET_CANNOT_DO_NOW);
				return true;
			}
#endif

			//checking about double insert same item
			for (DWORD j = 0; j < vec.size(); j++)
			{
				if(i==j)
					continue;

				TShopItemInfo& rShopItemCheck = vec[j];
				if(rShopItemCheck.pos == rShopItem.pos)
					return false;
			}
			
			ZeroObject(itemInfo);
			
			itemInfo.dwOwnerID = ch->GetPlayerID();
			memcpy(itemInfo.item.aAttr ,	item->GetAttributes(),	sizeof(itemInfo.item.aAttr));
			memcpy(itemInfo.item.alSockets,	item->GetSockets(),		sizeof(itemInfo.item.alSockets));

			itemInfo.item.dwVnum	= item->GetVnum();
			itemInfo.item.dwCount	= item->GetCount();
			//patch 08-03-2020
			itemInfo.item.expiration = GetItemExpiration(item);

#if defined(ENABLE_CHANGELOOK)
			itemInfo.item.dwTransmutation = item->GetTransmutation();
#endif
#ifdef ATTR_LOCK
			itemInfo.item.iLockedAttr = item->GetLockedAttr();
#endif
#ifdef ENABLE_NEW_OFFLINESHOP_LOGS
			LogManager::instance().OfflineshopLog(ch->GetPlayerID(), 0, "trying to open shop , adding item vnum %u count %u original id %u ", itemInfo.item.dwVnum, itemInfo.item.dwCount, item->GetID());
#endif
			CopyObject(itemInfo.price, rShopItem.price);
			vecItem.push_back(itemInfo);
		}

		for (offlineshop::TShopItemInfo& rShopItem : vec)
		{
			LPITEM item = ch->GetItem(rShopItem.pos);
			if (item == nullptr)
			{
				sys_err("CRITICAL: %s no item at pos cell %u window %u", ch->GetName(), rShopItem.pos.cell, rShopItem.pos.window_type);
				return false;
			}
			M2_DESTROY_ITEM(item->RemoveFromCharacter());
		}

		OFFSHOP_DEBUG("ch name %s , checked successful , send to db ", ch->GetName());

		

		rShopInfo.dwDuration = MIN(rShopInfo.dwDuration , OFFLINESHOP_DURATION_MAX_MINUTES);
		SendShopCreateNewDBPacket(rShopInfo, vecItem);

		return true;
	}
	
	bool CShopManager::RecvShopChangeNameClientPacket(LPCHARACTER ch, const char* szName)
	{
		if(!ch || szName == nullptr)
			return false;

		const auto shop = ch->GetOfflineShop();
		if (!shop)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You don't have a private shop.");
			return false;
		}

		if (strncasecmp(szName, shop->GetName(), strlen(szName)) == 0)
			return true;
		const auto changeNamePulse = shop->GetChangeNamePulse();
		if (changeNamePulse > get_global_time())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot change the name of the shop yet.");
			return false;
		}

		static char szNameChecked[OFFLINE_SHOP_NAME_MAX_LEN];
		static char szFullName[OFFLINE_SHOP_NAME_MAX_LEN];

		//cheching about bandword
		strncpy(szNameChecked, szName, sizeof(szNameChecked));
		if (CBanwordManager::instance().CheckString(szNameChecked, strlen(szNameChecked)))
			return false;

		if (!IsValidShopName(szNameChecked))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't give your shop an invalid name.");
			return false;
		}

		//making full name
		snprintf(szFullName, sizeof(szFullName), "%s@%s" , ch->GetName(), szNameChecked );
#ifdef ENABLE_NEW_OFFLINESHOP_LOGS
		LogManager::instance().OfflineshopLog(ch->GetPlayerID(), 0, "changing shop name : %s -> %s ", ch->GetOfflineShop()->GetName(), szFullName);
#endif

		shop->SetChangeNamePulse(get_global_time() + OFFLINESHOP_SECONDS_DELAY_CHANGE_NAME);
		SendShopChangeNameDBPacket(ch->GetPlayerID(), szFullName);
		return true;
	}


	bool CShopManager::RecvShopForceCloseClientPacket(LPCHARACTER ch)
	{
		if(!ch || !ch->GetOfflineShop())
			return false;

#ifdef ENABLE_NEW_OFFLINESHOP_LOGS
		LogManager::instance().OfflineshopLog(ch->GetPlayerID(), 0, "player asked to close shop (remain items count %u) ", ch->GetOfflineShop()->GetItems()->size());
#endif

		ch->SetOfflineShopUseTime();
		SendShopForceCloseDBPacket(ch->GetPlayerID());
		return true;
	}


	bool CShopManager::RecvShopRequestListClientPacket(LPCHARACTER ch)
	{
		if(!ch || !ch->GetDesc())
			return false;

		SendShopListClientPacket(ch);
		return true;
	}


	bool CShopManager::RecvShopOpenClientPacket(LPCHARACTER ch, DWORD dwOwnerID)
	{
		if(!ch || !ch->GetDesc())
			return false;

		CShop* pkShop = GetShopByOwnerID(dwOwnerID);
		if(!pkShop)
			return false;

		ch->SetOfflineShopUseTime();

		if (ch->GetOfflineShopGuest())
			ch->GetOfflineShopGuest()->RemoveGuest(ch);

		//offlineshop-updated 04/08/19
		if(ch->GetPlayerID() == dwOwnerID)
			SendShopOpenMyShopClientPacket(ch);
		else
			SendShopOpenClientPacket(ch , pkShop);


		pkShop->AddGuest(ch);
		ch->SetOfflineShopGuest(pkShop);
		return true;
	}


	bool CShopManager::RecvShopOpenMyShopClientPacket(LPCHARACTER ch)
	{
		if(!ch || !ch->GetDesc())
			return false;

		ch->SetOfflineShopUseTime();

		if (!ch->GetOfflineShop())
		{
			SendShopOpenMyShopNoShopClientPacket(ch);
		}


		else
		{
			SendShopOpenMyShopClientPacket(ch);
			ch->GetOfflineShop()->AddGuest(ch);
			ch->SetOfflineShopGuest(ch->GetOfflineShop());
		}
		

		return true;
	}

	bool CShopManager::RecvShopBuyItemClientPacket(LPCHARACTER ch, DWORD dwOwnerID, DWORD dwItemID, bool isSearch, long long TotalPriceSeen)
	{
		OFFSHOP_DEBUG("owner %u , item id %u ", dwOwnerID, dwItemID);

		if(!ch)
			return false;

		if (!_IS_VALID_GM_LEVEL(ch))
			return false;

		if (!ch->CanHandleItem() || !CheckCharacterActions(ch))
		{
			SendChatPacket(ch,CHAT_PACKET_CANNOT_DO_NOW);
			return true;
		}

		if (ch->GetPlayerID() == dwOwnerID) {
			SendChatPacket(ch,CHAT_PACKET_CANNOT_DO_NOW);
			return true;
		}

		quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
		if (pPC == nullptr)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot buy the selected item.");
			return false;
		}

		if (pPC->IsRunning())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot buy the selected item.");
			return false;
		}

		CShop* pkShop=nullptr;
		if(!(pkShop=GetShopByOwnerID(dwOwnerID)))
			return false;

		CShopItem* pitem=nullptr;
		if(!pkShop->GetItem(dwItemID, &pitem))
			return false;

		if(!pitem->CanBuy(ch))
			return false;

		if (pitem->GetPrice()->GetTotalYangAmount() != TotalPriceSeen)
			return false;

		ch->SetOfflineShopUseTime();

		{ // Check only for empty space
			LPITEM pkItem = pitem->CreateItem();
			if (!pitem)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "You cannot buy the selected item.");
				return false;
			}

			TItemPos pos;
			if (!ch->CanTakeInventoryItem(pkItem, &pos))
			{
				M2_DESTROY_ITEM(pkItem);
				ch->ChatPacket(CHAT_TYPE_INFO, "You don't have enough space in your inventory.");
				return false;
			}
			M2_DESTROY_ITEM(pkItem);
		}
		OFFSHOP_DEBUG("sending packet to db (buyer %u , owner %u , item %u )",ch->GetPlayerID() , dwOwnerID, dwItemID);
		
		if(isSearch)
			SendShopBuyItemFromSearchClientPacket(ch, dwOwnerID, dwItemID);

		SendShopLockBuyItemDBPacket(ch->GetPlayerID(), dwOwnerID, dwItemID, TotalPriceSeen);
		return true;
	}




#ifdef ENABLE_NEW_SHOP_IN_CITIES
	bool CShopManager::RecvShopClickEntity(LPCHARACTER ch, DWORD dwShopEntityVID)
	{
		for (auto & vecCitie : m_vecCities)
		{
			auto iterMap = vecCitie.entitiesByVID.find(dwShopEntityVID);
			if(vecCitie.entitiesByVID.end() == iterMap)
				continue;
			

			DWORD dwPID = iterMap->second->GetShop()->GetOwnerPID();
			
			
			RecvShopOpenClientPacket(ch, dwPID);
			return true;
		}

		sys_err("cannot found clicked entity , %s vid %u ",ch->GetName(), dwShopEntityVID);
		return false;
	}
#endif




	void CShopManager::SendShopListClientPacket(LPCHARACTER ch)
	{
		if (!ch->GetDesc())
			return;

		TEMP_BUFFER buff;
		TPacketGCNewOfflineshop pack;
		pack.bHeader	= HEADER_GC_NEW_OFFLINESHOP;
		pack.bSubHeader	= SUBHEADER_GC_SHOP_LIST;
		pack.wSize		= sizeof(pack) + sizeof(TSubPacketGCShopList) + (m_mapShops.size() * sizeof(TShopInfo)) ;

		buff.write(&pack, sizeof(pack));

		TSubPacketGCShopList subPack;
		subPack.dwShopCount = m_mapShops.size();
		buff.write(&subPack, sizeof(subPack));


		TShopInfo shopInfo;

		auto it=m_mapShops.begin();
		for (; it != m_mapShops.end(); it++)
		{
			const CShop& rShop	= it->second;
			DWORD dwOwner		= it->first;

			ZeroObject(shopInfo);

			shopInfo.dwCount	= rShop.GetItems()->size();
			shopInfo.dwDuration	= rShop.GetDuration();
			shopInfo.dwOwnerID	= dwOwner;
			strncpy(shopInfo.szName, rShop.GetName(), sizeof(shopInfo.szName));

			buff.write(&shopInfo, sizeof(shopInfo));
		}
		
		ch->GetDesc()->Packet(buff.read_peek() , buff.size());
	}


	void CShopManager::SendShopOpenClientPacket(LPCHARACTER ch, CShop* pkShop)
	{
		if(!ch || !ch->GetDesc() || !pkShop)
			return;

		CShop::VECSHOPITEM* pVec = pkShop->GetItems();
		TEMP_BUFFER buff;
		TPacketGCNewOfflineshop pack;
		pack.bHeader	= HEADER_GC_NEW_OFFLINESHOP;
		pack.bSubHeader	= SUBHEADER_GC_SHOP_OPEN;
		pack.wSize		= sizeof(pack) + sizeof(TSubPacketGCShopOpen) + sizeof(TItemInfo)*pVec->size();

		buff.write(&pack, sizeof(pack));

		

		TSubPacketGCShopOpen subPack;
		subPack.shop.dwCount	= pVec->size();
		subPack.shop.dwDuration	= pkShop->GetDuration();
		subPack.shop.dwOwnerID	= pkShop->GetOwnerPID();
		strncpy(subPack.shop.szName, pkShop->GetName(), sizeof(subPack.shop.szName));

		buff.write(&subPack, sizeof(subPack));

		TItemInfo itemInfo;

		for (offlineshop::CShopItem & rItem : *pVec)
		{
			ZeroObject(itemInfo);

			itemInfo.dwItemID	= rItem.GetID();
			itemInfo.dwOwnerID	= pkShop->GetOwnerPID();
			CopyObject(itemInfo.item, *(rItem.GetInfo()));
			CopyObject(itemInfo.price,*(rItem.GetPrice()));

			buff.write(&itemInfo, sizeof(itemInfo));
		}
		
		ch->GetDesc()->Packet(buff.read_peek(), buff.size());
	}


	void CShopManager::SendShopOpenMyShopNoShopClientPacket(LPCHARACTER ch)
	{
		if (!ch->GetDesc())
			return;

		TPacketGCNewOfflineshop pack;
		pack.bHeader	= HEADER_GC_NEW_OFFLINESHOP;
		pack.bSubHeader	= SUBHEADER_GC_SHOP_OPEN_OWNER_NO_SHOP;
		pack.wSize		= sizeof(pack);


		ch->GetDesc()->Packet(&pack, sizeof(pack));
	}

	void CShopManager::SendShopBuyItemFromSearchClientPacket(LPCHARACTER ch, DWORD dwOwnerID, DWORD dwItemID)
	{
		if (!ch->GetDesc())
			return;
		
		TPacketGCNewOfflineshop pack;
		pack.bHeader	= HEADER_GC_NEW_OFFLINESHOP;
		pack.bSubHeader	= SUBHEADER_GC_SHOP_BUY_ITEM_FROM_SEARCH;
		pack.wSize		= sizeof(pack) + sizeof(TSubPacketGCShopBuyItemFromSearch);

		TSubPacketGCShopBuyItemFromSearch subpack;
		subpack.dwOwnerID = dwOwnerID;
		subpack.dwItemID  = dwItemID;

		TEMP_BUFFER buff;
		buff.write(&pack,		sizeof(pack));
		buff.write(&subpack,	sizeof(subpack));

		ch->GetDesc()->Packet(buff.read_peek(), buff.size());
	}


	void CShopManager::SendShopOpenMyShopClientPacket(LPCHARACTER ch)
	{
		if (!ch->GetDesc())
			return;

		if(!ch->GetOfflineShop())
			return;

		CShop* pkShop	= ch->GetOfflineShop();
		DWORD dwOwnerID	= ch->GetPlayerID();

		CShop::VECSHOPITEM*  pVec		= pkShop->GetItems();
		CShop::VECSHOPITEM*  pVecSold	= pkShop->GetItemsSold();
		CShop::VECSHOPOFFER* pVecOffer	= pkShop->GetOffers();

		TEMP_BUFFER buff;
		TPacketGCNewOfflineshop pack;
		pack.bHeader	= HEADER_GC_NEW_OFFLINESHOP;
		pack.bSubHeader	= SUBHEADER_GC_SHOP_OPEN_OWNER;
		pack.wSize		= sizeof(pack) + sizeof(TSubPacketGCShopOpenOwner) + sizeof(TItemInfo)*pVec->size() + sizeof(TItemInfo)* pVecSold->size() + sizeof(TOfferInfo)*pVecOffer->size();
		
		buff.write(&pack, sizeof(pack));

		

		TSubPacketGCShopOpenOwner subPack;
		subPack.shop.dwCount	= pVec->size();
		subPack.shop.dwDuration	= pkShop->GetDuration();
		subPack.shop.dwOwnerID	= dwOwnerID;
		subPack.dwSoldCount		= pVecSold->size();
		subPack.dwOfferCount	= pVecOffer->size();

		strncpy(subPack.shop.szName, pkShop->GetName(), sizeof(subPack.shop.szName));
		
		
		OFFSHOP_DEBUG("owner %u , item count %u , duration %u offer count %u ",subPack.shop.dwOwnerID, subPack.shop.dwCount , subPack.shop.dwDuration, subPack.dwOfferCount);
		
		
		buff.write(&subPack, sizeof(subPack));

		TItemInfo itemInfo;

		for (offlineshop::CShopItem & rItem : *pVec)
		{
			ZeroObject(itemInfo);

			itemInfo.dwItemID	= rItem.GetID();
			itemInfo.dwOwnerID	= dwOwnerID;
			CopyObject(itemInfo.item, *(rItem.GetInfo()));
			CopyObject(itemInfo.price,*(rItem.GetPrice()));

			OFFSHOP_DEBUG("item id %u , item vnum %u , item count %u ",itemInfo.dwItemID, itemInfo.item.dwVnum , itemInfo.item.dwCount);
			buff.write(&itemInfo, sizeof(itemInfo));
		}


		for (offlineshop::CShopItem& rItem : *pVecSold)
		{
			ZeroObject(itemInfo);

			itemInfo.dwItemID	= rItem.GetID();
			itemInfo.dwOwnerID	= dwOwnerID;
			CopyObject(itemInfo.item, *(rItem.GetInfo()));
			CopyObject(itemInfo.price,*(rItem.GetPrice()));

			OFFSHOP_DEBUG("item id %u , item vnum %u , item count %u ",itemInfo.dwItemID, itemInfo.item.dwVnum , itemInfo.item.dwCount);
			buff.write(&itemInfo, sizeof(itemInfo));
		}


		//writing offer vector (no need to convert to some other object)
		if(!pVecOffer->empty())
			buff.write(&pVecOffer->at(0), sizeof(TOfferInfo) * pVecOffer->size());

		ch->GetDesc()->Packet(buff.read_peek(), buff.size());


		for (offlineshop::TOfferInfo& offer : *pVecOffer)
		{
			if (!offer.bAccepted && !offer.bNoticed)
				SendShopOfferNotifiedDBPacket(offer.dwOfferID, offer.dwOwnerID);
		}
	}




	void CShopManager::SendShopForceClosedClientPacket(DWORD dwOwnerID)
	{
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwOwnerID);
		if(!ch || !ch->GetDesc())
			return;

		TPacketGCNewOfflineshop pack;
		pack.bHeader	= HEADER_GC_NEW_OFFLINESHOP;
		pack.bSubHeader	= SUBHEADER_GC_SHOP_OPEN_OWNER;

		pack.wSize = sizeof(pack);
		ch->GetDesc()->Packet(&pack , sizeof(pack));
	}




	//ITEMS
	bool CShopManager::RecvShopAddItemClientPacket(LPCHARACTER ch, const TItemPos& pos, const TPriceInfo& price)
	{
		if(!ch)
			return false;

		CShop* pkShop = ch->GetOfflineShop();
		if (!pkShop)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You don't have a private shop.");
			return true;
		}

		if (!_IS_VALID_GM_LEVEL(ch))
			return false;

#if defined(DISABLE_OFFLINESHOP_STRICTRULES)
		if (s_allowed_maps.find(ch->GetMapIndex()) == s_allowed_maps.end())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Nu poti sa adaugi iteme din aceasta mapa, poti doar in bazar.");
			return true;
		}
#endif

		if (!ch->CanHandleItem()|| !CheckCharacterActions(ch))
		{
			SendChatPacket(ch,CHAT_PACKET_CANNOT_DO_NOW);
			return true;
		}

		if (pkShop->GetItems()->size() > OFFLINESHOP_MAX_ITEM_NUM)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Your private shop reached the maximum number of items.");
			return true;
		}

		quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
		if (pPC == nullptr)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return true;
		}

		if (pPC->IsRunning())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return true;
		}

		LPITEM pkItem = ch->GetItem(pos);
		if(!pkItem)
			return false;

		if (pkItem->isLocked() || pkItem->IsEquipped() || pkItem->IsExchanging())
		{
			SendChatPacket(ch,CHAT_PACKET_CANNOT_DO_NOW);
			return true;
		}

#ifdef ENABLE_SWITCHBOT_WORLDARD
		if (pos.IsSwitchBotPosition())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot sell this item.");
			return false;
		}
#endif

		if (pos.IsDragonSoulEquipPosition())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot sell this item.");
			return false;
		}

#ifdef ENABLE_SOULBIND_SYSTEM
		if (pkItem->IsSealed()) {
			SendChatPacket(ch, CHAT_PACKET_CANNOT_DO_NOW);
			return true;
		}
#endif

		if(IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_GIVE))
			return false;
		if(IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_MYSHOP))
			return false;

		if (!IsGoodSalePrice(price))
			return false;

		if (pos.IsEquipPosition())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot sell this item.");
			return true;
		}

		if (pos.IsDragonSoulEquipPosition())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot sell this item.");
			return true;
		}

		ch->SetOfflineShopUseTime();

		TItemInfo itemInfo;
		ZeroObject(itemInfo);

		itemInfo.dwOwnerID		= ch->GetPlayerID();
		itemInfo.item.dwVnum	= pkItem->GetVnum();
		itemInfo.item.dwCount	= pkItem->GetCount();
		//patch 08-03-2020
		itemInfo.item.expiration = GetItemExpiration(pkItem);

		memcpy(itemInfo.item.aAttr,		pkItem->GetAttributes(),	sizeof(itemInfo.item.aAttr));
		memcpy(itemInfo.item.alSockets,	pkItem->GetSockets(),		sizeof(itemInfo.item.alSockets));

#if defined(ENABLE_CHANGELOOK)
		itemInfo.item.dwTransmutation = pkItem->GetTransmutation();
#endif
#ifdef ATTR_LOCK
		itemInfo.item.iLockedAttr = pkItem->GetLockedAttr();
#endif
		CopyObject(itemInfo.price, price);

#ifdef ENABLE_NEW_OFFLINESHOP_LOGS
		LogManager::instance().OfflineshopLog(ch->GetPlayerID(), 0, "adding new item to the shop vnum %u count %u (original item ID %u) ", itemInfo.item.dwVnum, itemInfo.item.dwCount, pkItem->GetID());
#endif

		M2_DESTROY_ITEM( pkItem->RemoveFromCharacter() );

		SendShopAddItemDBPacket(ch->GetPlayerID(), itemInfo);
		return true;
	}

	bool CShopManager::RecvShopRemoveItemClientPacket(LPCHARACTER ch, DWORD dwItemID)
	{
#if defined(DISABLE_OFFLINESHOP_STRICTRULES)
		if (!ch || !ch->GetOfflineShop())
		{
			return false;
		}

		if (!_IS_VALID_GM_LEVEL(ch))
		{
			return false;
		}

#if defined(ENABLE_ANTI_FLOOD)
		if (thecore_pulse() < ch->GetAntiFloodPulse(FLOOD_OFFSHOP_REMOVEITEM) + PASSES_PER_SEC(1))
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1710, "");
#else
			ch->ChatPacket(CHAT_TYPE_INFO, "You can do this once every second.");
#endif
			return false;
		}
#endif

		if (!ch->CanHandleItem())
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1714, "");
#else
			ch->ChatPacket(CHAT_TYPE_INFO, "Try again in few seconds.");
#endif
			return false;
		}

		if (!CheckCharacterActions(ch))
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1714, "");
#else
			ch->ChatPacket(CHAT_TYPE_INFO, "Try again in few seconds.");
#endif
			return false;
		}

		quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
		if (!pPC)
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1715, "");
#else
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
#endif
			return false;
		}

		if (pPC->IsRunning())
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1715, "");
#else
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
#endif
			return false;
		}

		CShop* pkShop = ch->GetOfflineShop();
		CShopItem* pItem = nullptr;

		OFFSHOP_DEBUG("owner %u , item id %u ",ch->GetPlayerID() , dwItemID);

		if (pkShop->GetItems()->size() == 1)
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1716, "");
#else
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't remove the last item from the shop.");
#endif
			return false;
		}

		if (!pkShop->GetItem(dwItemID, &pItem))
		{
			return false;
		}

#if defined(ENABLE_ANTI_FLOOD)
		ch->SetAntiFloodPulse(FLOOD_OFFSHOP_REMOVEITEM, thecore_pulse());
#endif
		ch->SetOfflineShopUseTime();
		SendShopRemoveItemDBPacket(pkShop->GetOwnerPID(), pItem->GetID());
		return true;
#else
		return false;
#endif
	}

	bool CShopManager::RecvShopEditItemDBPacket(DWORD dwOwnerID, DWORD dwItemID, const TPriceInfo& rPrice)
	{
		CShop* pkShop = GetShopByOwnerID(dwOwnerID);
		if (!pkShop)
			return false;

		CShopItem* pItem = nullptr;
		if (!pkShop->GetItem(dwItemID, &pItem))
			return false;

		OFFSHOP_DEBUG("owner id %u , item id %u ", dwOwnerID, dwItemID);

		CShopItem newItem(*pItem);
		newItem.SetPrice(rPrice);

		pkShop->ModifyItem(dwItemID, newItem);
		return true;
	}



	//FILTER
    bool CShopManager::RecvShopFilterRequestClientPacket(LPCHARACTER ch,
                                                            const TFilterInfo& filter)
    {
        if (!ch)
        {
            return false;
        }

        std::vector<TItemInfo> vec;

        if (!CheckSearchTime(ch->GetPlayerID()))
        {
            SendShopFilterResultClientPacket(ch, vec);
            SendChatPacket(ch,CHAT_PACKET_CANNOT_SEARCH_YET);
            return false;
        }

        ch->SetOfflineShopUseTime();

        std::string stName = StringToLower(filter.szName, strnlen(filter.szName, sizeof(filter.szName)));

        std::vector<TItemInfo> vecItems;

        bool bAll = filter.bType == 0 &&
                    filter.bSubType == 0 ? true : false;

        auto cit= m_mapShops.begin();
        for (; cit != m_mapShops.end(); cit++)
        {
            const CShop& rcShop = cit->second;
            if(rcShop.GetOwnerPID() == ch->GetPlayerID())
            {
                continue;
            }

#if defined(ENABLE_OFFLINESHOP_REWORK)
            std::string stNameFull = rcShop.GetName();
            std::string stNameOwner = "";

            if (stNameFull.size() != 0 &&
                stNameFull.find("@") != std::string::npos)
            {
                for (int i = 0; i < stNameFull.length(); ++i)
                {
                    if (stNameFull[i] == '@')
                    {
                        break;
                    }

                    stNameOwner += stNameFull[i];
                }
            }
            else
            {
                stNameOwner = stNameFull;
            }

            if (filter.bPlayerName == true &&
                strcasecmp(stNameOwner.c_str(), stName.c_str()) != 0)
            {
                continue;
            }
#endif

            CShop::VECSHOPITEM* pShopItems = rcShop.GetItems();

            auto cItemIter = pShopItems->begin();
            for (; cItemIter != pShopItems->end(); cItemIter++)
            {
                const CShopItem& rItem = *cItemIter;
                const TItemInfoEx& rItemInfo = *rItem.GetInfo();
                const TPriceInfo& rItemPrice = *rItem.GetPrice();

                TItemTable* pTable = ITEM_MANAGER::instance().GetTable(rItemInfo.dwVnum);
                if (!pTable)
                {
                    sys_err("CANNOT FIND ITEM TABLE [%d]");
                    continue;
                }

                if (filter.bType != ITEM_NONE &&
                    filter.bType != pTable->bType)
                {
                    continue;
                }

                if (filter.bType != ITEM_NONE &&
                    filter.bSubType != SUBTYPE_NOSET &&
                    filter.bSubType != pTable->bSubType)
                {
                    continue;
                }

#ifdef ENABLE_RARITY_SYSTEM
                if (filter.iRarity != -1)
                {
                    TItemExtraProto* ExtraProto = ITEM_MANAGER::instance().GetExtraProto(rItemInfo.dwVnum);
                    if (ExtraProto &&
                        ExtraProto->iRarity != filter.iRarity)
                    {
                        continue;
                    }

                    if (filter.iRarity != 0 && !ExtraProto)
                    {
                        continue;
                    }
                }
#endif

                int iLimitLevel = pTable->aLimits[0].bType == LIMIT_LEVEL ? 
                                pTable->aLimits[0].lValue : pTable->aLimits[1].bType == LIMIT_LEVEL ?
                                pTable->aLimits[1].lValue : 0;

                if ((filter.iLevelStart != 0 ||
                    filter.iLevelEnd != 0))
                {
                    if (iLimitLevel == 0)
                    {
                        continue;
                    }
    
                    if (iLimitLevel < filter.iLevelStart &&
                        filter.iLevelStart!=0)
                    {
                        continue;
                    }
    
                    if (iLimitLevel > filter.iLevelEnd &&
                        filter.iLevelEnd!=0)
                    {
                        continue;
                    }
                }

                if (filter.priceStart.illYang != 0)
                {
                    if (GetTotalAmountFromPrice(rItemPrice) < filter.priceStart.illYang)
                    {
                        continue;
                    }
                }

                if(filter.priceEnd.illYang != 0)
                    if(GetTotalAmountFromPrice(rItemPrice) > filter.priceEnd.illYang)
                        continue;

#if defined(ENABLE_OFFLINESHOP_REWORK)
                if (filter.bPlayerName == false)
                {
#endif
#ifdef ENABLE_MULTI_NAMES
                if (strnlen(filter.szName, sizeof(filter.szName)) != 0 &&
                    !MatchItemName(stName , pTable->szLocaleName[ch->GetDesc()->GetLanguage()] , strnlen(pTable->szLocaleName[ch->GetDesc()->GetLanguage()], ITEM_NAME_MAX_LEN)))
#else
                if (strnlen(filter.szName, sizeof(filter.szName)) != 0 &&
                    !MatchItemName(stName , pTable->szLocaleName , strnlen(pTable->szLocaleName, ITEM_NAME_MAX_LEN)))
#endif
                {
                    continue;
                }
#if defined(ENABLE_OFFLINESHOP_REWORK)
                }
#endif

                if (!MatchWearFlag(filter.dwWearFlag, pTable->dwAntiFlags))
                {
                    continue;
                }

                if (!MatchAttributes(filter.aAttr, rItemInfo.aAttr))
                {
                    continue;
                }

                TItemInfo itemInfo;
                CopyObject(itemInfo.item, rItemInfo);
                CopyObject(itemInfo.price,rItemPrice);

                itemInfo.dwItemID = rItem.GetID();
                itemInfo.dwOwnerID = rcShop.GetOwnerPID();
#if defined(ENABLE_OFFLINESHOP_REWORK)
                strlcpy(itemInfo.szOwnerName, rcShop.GetName(), sizeof(itemInfo.szOwnerName));
#endif

                vecItems.push_back(itemInfo);

                if (bAll == true)
                {
                    if (vecItems.size() >= OFFLINESHOP_MAX_SEARCH_RESULT)
                    {
                        break;
                    }
                }
            }
        }

        if (vecItems.size() > 0)
        {
            std::sort(vecItems.begin(), vecItems.end(),
                        [](TItemInfo a, TItemInfo b)
                        {
                            return (a.price.GetTotalYangAmount() / b.item.dwCount) < (b.price.GetTotalYangAmount() / b.item.dwCount);
                        }
            );
        }

        for (auto item : vecItems) 
        {
            vec.push_back(item);

            if (vec.size() >= OFFLINESHOP_MAX_SEARCH_RESULT)
            {
                break;
            }
        }

        vecItems.clear();

        SendShopFilterResultClientPacket(ch, vec);
        return true;
    }


	void CShopManager::SendShopFilterResultClientPacket(LPCHARACTER ch, const std::vector<TItemInfo>& items)
	{
		if(!ch || !ch->GetDesc())
			return;

		TEMP_BUFFER buff;

		TPacketGCNewOfflineshop pack;
		pack.bHeader	= HEADER_GC_NEW_OFFLINESHOP;
		pack.bSubHeader	= SUBHEADER_GC_SHOP_FILTER_RESULT;
		pack.wSize		= sizeof(pack) + sizeof(TSubPacketGCShopFilterResult) + sizeof(TItemInfo)*items.size();
		buff.write(&pack, sizeof(pack));

		TSubPacketGCShopFilterResult subpack;
		subpack.dwCount	= items.size();
		buff.write(&subpack, sizeof(subpack));

		for (DWORD i = 0; i < items.size(); i++)
		{
			const TItemInfo& rItemInfo= items[i];
			buff.write(&rItemInfo, sizeof(rItemInfo));
		}


		ch->GetDesc()->Packet(buff.read_peek(), buff.size());
	}




	//OFFERS
	bool CShopManager::RecvShopCreateOfferClientPacket(LPCHARACTER ch, TOfferInfo& offer)
	{
		return false;
	}


	bool CShopManager::RecvShopEditOfferClientPacket(LPCHARACTER ch, const TOfferInfo& offer)
	{
		if(!ch)
			return false;

		if (!_IS_VALID_GM_LEVEL(ch))
			return false;

		//next feature
		return true;
	}


	bool CShopManager::RecvShopAcceptOfferClientPacket(LPCHARACTER ch, DWORD dwOfferID)
	{
		if(!ch || !ch->GetOfflineShop())
			return false;

		if (!_IS_VALID_GM_LEVEL(ch))
			return false;

#ifdef DISABLE_OFFSHOP_OFFERS
		return true;
#endif

		quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
		if (pPC == nullptr)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return false;
		}

		if (pPC->IsRunning())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return false;
		}

		CShop* pkShop = ch->GetOfflineShop();
		if (pkShop == nullptr)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You don't have a private shop.");
			return false;
		}

		TOfferInfo* info = nullptr;
		CShop::VECSHOPOFFER& vec = *(pkShop->GetOffers());
		for (auto & i : vec)
		{
			if (dwOfferID == i.dwOfferID)
			{
				info = &i;
				break;
			}
		}

		if (!info)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "This offer does not exist.");
			return false;
		}

		if (info->bAccepted)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "This offer has already been accepted.");
			return false;
		}

		if (ch->GetPlayerID() != info->dwOwnerID)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot accept this offer.");
			return false;
		}

		CShopItem* item = nullptr;
		if (!pkShop->GetItem(info->dwItemID, &item))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "This item does not exist to accept an offer.");
			return false;
		}

		// patch with warp check
		ch->SetOfflineShopUseTime();

		info->bAccepted = true;
		SendShopOfferAcceptDBPacket(*info);
		return true;
	}



	bool CShopManager::RecvShopCancelOfferClientPacket(LPCHARACTER ch, DWORD dwOfferID, DWORD dwOwnerID)
	{
		if(!ch)
			return false;

		if (!_IS_VALID_GM_LEVEL(ch))
			return false;

#ifdef DISABLE_OFFSHOP_OFFERS
		return true;
#endif

		quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
		if (pPC == nullptr)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return false;
		}

		if (pPC->IsRunning())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return false;
		}

		CShop* pkShop	= GetShopByOwnerID(dwOwnerID);
		if(!pkShop)
			return false;

		TOfferInfo* info = nullptr;
		CShop::VECSHOPOFFER& vec = *(pkShop->GetOffers());
		for (auto & i : vec)
		{
			if (dwOfferID == i.dwOfferID)
			{
				info = &i;
				break;
			}
		}



		if(!info || info->bAccepted)
			return false;


		CShopItem* item = nullptr;
		if (!pkShop->GetItem(info->dwItemID, &item))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "This item does not exist to cancel an offer.");
			return false;
		}

		if (ch->GetPlayerID() != pkShop->GetOwnerPID() && ch->GetPlayerID() != info->dwOffererID)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot cancel this offer.");
			return false;
		}

		ch->SetOfflineShopUseTime();

		OFFSHOP_DEBUG("success %u offer , %u shop ", dwOfferID, dwOwnerID);
		SendShopOfferCancelDBPacket(*info);
		return true;
	}


	bool CShopManager::RecvOfferListRequestPacket(LPCHARACTER ch) //offlineshop-updated 03/08/19
	{
		if (!ch->GetDesc())
			return false;

		TPacketGCNewOfflineshop pack;
		pack.bHeader	= HEADER_GC_NEW_OFFLINESHOP;
		pack.bSubHeader	= SUBHEADER_GC_OFFER_LIST;
		pack.wSize		= sizeof(pack) + sizeof(TSubPacketGCShopOfferList);

		TSubPacketGCShopOfferList subpack;
		subpack.dwOfferCount = 0;

		TEMP_BUFFER buff;

		

		auto it = m_mapOffer.find(ch->GetPlayerID());
		if (it == m_mapOffer.end() || it->second.empty())
		{
			buff.write(&pack, sizeof(pack));
			buff.write(&subpack, sizeof(subpack));

			OFFSHOP_DEBUG("return because not found or empty vec : found > %s  (id %u) ",it!=m_mapOffer.end()?"TRUE":"FALSE" , ch->GetPlayerID());

			ch->GetDesc()->Packet(buff.read_peek() , buff.size());
			return true;
		}



		const std::vector<TOfferInfo>& vec = it->second;
		pack.wSize += sizeof(TOfferInfo)*vec.size();

		std::vector<TMyOfferExtraInfo> extrainfo;
		extrainfo.resize(vec.size());
		subpack.dwOfferCount = vec.size();

		OFFSHOP_DEBUG("found %u in map, size %u ",ch->GetPlayerID(), vec.size());

		for (DWORD i = 0; i < vec.size(); i++)
		{
			const TOfferInfo& offer = vec[i];
			CShop* pkShop = GetShopByOwnerID(offer.dwOwnerID);
			if (!pkShop)
			{
				sys_err("cannot find item's shop %u , offer id %u ",offer.dwOwnerID, offer.dwOfferID);
				return false;
			}

			CShopItem* pkItem=nullptr;
			if(!pkShop->GetItem(offer.dwItemID, &pkItem) && !pkShop->GetItemSold(offer.dwItemID, &pkItem))
			{
				sys_err("cannot find item info %u , offer id %u ",offer.dwItemID, offer.dwOfferID);
				return false;
			}

			TItemInfo& itemInfo = extrainfo[i].item;
			itemInfo.dwItemID	= offer.dwItemID;
			itemInfo.dwOwnerID	= offer.dwOwnerID;

			CopyObject(itemInfo.item, *pkItem->GetInfo());
			CopyObject(itemInfo.price, *pkItem->GetPrice());

			strncpy(extrainfo[i].szShopName , pkShop->GetName(), OFFLINE_SHOP_NAME_MAX_LEN);
		}

		pack.wSize += sizeof(TMyOfferExtraInfo)*extrainfo.size();

		buff.write(&pack,			sizeof(pack));
		buff.write(&subpack,		sizeof(subpack));
		buff.write(&vec[0],			sizeof(TOfferInfo)*vec.size());
		buff.write(&extrainfo[0] ,	sizeof(TMyOfferExtraInfo)*extrainfo.size());//offlineshop-updated 04/08/19

		//offlineshop-updated 05/08/19
		ch->SetLookingOfflineshopOfferList(true);
		ch->GetDesc()->Packet(buff.read_peek(), buff.size());
		return true;
	}




	//SAFEBOX
	bool CShopManager::RecvShopSafeboxOpenClientPacket(LPCHARACTER ch)
	{
		if(!ch || ch->GetShopSafebox())
			return false;

		CShopSafebox* pkSafebox = GetShopSafeboxByOwnerID(ch->GetPlayerID());
		if(!pkSafebox)
			return false;

		ch->SetOfflineShopUseTime();

		ch->SetShopSafebox(pkSafebox);
		pkSafebox->RefreshToOwner(ch);
		return true;
	}



	bool CShopManager::RecvShopSafeboxGetItemClientPacket(LPCHARACTER ch, DWORD dwItemID)
	{
		if (!ch || !ch->GetShopSafebox() || ch->IsDead())
		{
			return false;
		}

#if defined(ENABLE_ANTI_FLOOD)
		if (thecore_pulse() < ch->GetAntiFloodPulse(FLOOD_OFFSHOP_WITHDRAWITEM) + PASSES_PER_SEC(1))
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1710, "");
#else
			ch->ChatPacket(CHAT_TYPE_INFO, "You can do this once every second.");
#endif
				return false;
		}
#endif

#if defined(DISABLE_OFFLINESHOP_STRICTRULES)
		if (s_allowed_maps.find(ch->GetMapIndex()) == s_allowed_maps.end())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Nu poti sa retragi iteme din aceasta mapa, poti doar in bazar.");
			return false;
		}
#endif

		if (!ch->CanHandleItem())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot withdraw this item.");
			return false;
		}

		if (!CheckCharacterActions(ch))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot withdraw this item.");
			return false;
		}

		quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
		if (pPC == nullptr)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return false;
		}

		if (pPC->IsRunning())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return false;
		}

		CShopSafebox* pkSafebox = ch->GetShopSafebox();
		if (!pkSafebox)
		{
			return false;
		}

		CShopItem* pItem = nullptr;
		if (!pkSafebox->GetItem(dwItemID, &pItem))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot withdraw this item.");
			return false;
		}

		LPITEM pkItem = pItem->CreateItem();
		if (!pkItem)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot withdraw this item.");
			return false;
		}

		TItemPos itemPos;
		if (!ch->CanTakeInventoryItem(pkItem, &itemPos))
		{
			M2_DESTROY_ITEM(pkItem);
			return false;
		}

#if defined(ENABLE_ANTI_FLOOD)
		ch->SetAntiFloodPulse(FLOOD_OFFSHOP_WITHDRAWITEM, thecore_pulse());
#endif
		ch->SetOfflineShopUseTime();

		if (!pkSafebox->RemoveItem(dwItemID))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot withdraw this item.");
			M2_DESTROY_ITEM(pkItem);
			return true;
		}

		pkSafebox->RefreshToOwner();
		pkItem->AddToCharacter(ch, itemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);

		SendShopSafeboxGetItemDBPacket(ch->GetPlayerID(), dwItemID);
		return true;
	}


	bool CShopManager::RecvShopSafeboxGetValutesClientPacket(LPCHARACTER ch, const TValutesInfo& valutes)
	{
		if (!ch || !ch->GetShopSafebox() || ch->IsDead())
			return false;

#if defined(ENABLE_ANTI_FLOOD)
		if (thecore_pulse() < ch->GetAntiFloodPulse(FLOOD_OFFSHOP_WITHDRAWYANG) + PASSES_PER_SEC(1))
		{
#if defined(ENABLE_TEXTS_RENEWAL)
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1710, "");
#else
			ch->ChatPacket(CHAT_TYPE_INFO, "You can do this once every second.");
#endif
				return false;
		}
#endif

#if defined(DISABLE_OFFLINESHOP_STRICTRULES)
		if (s_allowed_maps.find(ch->GetMapIndex()) == s_allowed_maps.end())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Nu poti sa retragi yang din aceasta mapa, poti doar in bazar.");
			return false;
		}
#endif

		if (valutes.illYang < 0)
			return false;

#ifdef __ENABLE_CHEQUE_SYSTEM__
		if (valutes.iCheque < 0)
			return false;
#endif

#if defined(ENABLE_NEW_GOLD_LIMIT)
		if((long long) ch->GetGold() + valutes.illYang > (long long) GOLD_MAX_MAX)
			return false;
#else
		if((long long) ch->GetGold() + valutes.illYang > (long long) GOLD_MAX)
			return false;
#endif

#ifdef __ENABLE_CHEQUE_SYSTEM__
		if (ch->GetCheque() + valutes.iCheque >= CHEQUE_MAX)
			return false;
#endif

		quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
		if (pPC == nullptr)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return false;
		}

		if (pPC->IsRunning())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return false;
		}

		CShopSafebox* pkSafebox		= ch->GetShopSafebox();
		CShopSafebox::SValuteAmount peekAmount(valutes);

		if(!pkSafebox->RemoveValute(peekAmount))
			return false;

#if defined(ENABLE_ANTI_FLOOD)
		ch->SetAntiFloodPulse(FLOOD_OFFSHOP_WITHDRAWYANG, thecore_pulse());
#endif
		ch->SetOfflineShopUseTime();

#if defined(ENABLE_NEW_GOLD_LIMIT)
		ch->ChangeGold(valutes.illYang);
#else
		ch->PointChange(POINT_GOLD, valutes.illYang);
#endif
#ifdef __ENABLE_CHEQUE_SYSTEM__
		ch->PointChange(POINT_CHEQUE, valutes.iCheque);
#endif

		pkSafebox->RefreshToOwner();
		SendShopSafeboxGetValutesDBPacket(ch->GetPlayerID(), valutes);
		return true;
	}

	bool CShopManager::RecvShopSafeboxCloseClientPacket(LPCHARACTER ch)
	{
		if(!ch || !ch->GetShopSafebox() || ch->IsDead())
			return false;
		
		ch->SetShopSafebox(nullptr);
		return true;
	}




	void CShopManager::SendShopSafeboxRefresh(LPCHARACTER ch, const TValutesInfo& valute, const std::vector<CShopItem>& vec)
	{
		if (!ch)
			return;

		if (!ch->GetDesc())
			return;

		CShopSafebox* pkSafebox = ch->GetShopSafebox();
		if (pkSafebox == nullptr)
			return;

		// patch with warp check
		ch->SetOfflineShopUseTime();

		TPacketGCNewOfflineshop pack;
		pack.bHeader = HEADER_GC_NEW_OFFLINESHOP;
		pack.wSize = sizeof(pack) + sizeof(TSubPacketGCShopSafeboxRefresh) + ((sizeof(DWORD) + sizeof(TItemInfoEx)) * vec.size());
		pack.bSubHeader = SUBHEADER_GC_SHOP_SAFEBOX_REFRESH;

		TSubPacketGCShopSafeboxRefresh subpack;
		subpack.dwItemCount = vec.size();
		CopyObject(subpack.valute, valute);

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		TItemInfoEx item;
		DWORD dwItemID = 0;

		for (const offlineshop::CShopItem& shopitem : vec)
		{
			dwItemID = shopitem.GetID();
			CopyObject(item, *shopitem.GetInfo());

			buff.write(&dwItemID, sizeof(dwItemID));
			buff.write(&item, sizeof(item));
		}

		ch->GetDesc()->Packet(buff.read_peek(), buff.size());
	}



	bool CShopManager::RecvAuctionListRequestClientPacket(LPCHARACTER ch, bool owner)
	{
		if(!ch || !ch->GetDesc())
			return false;

		TAuctionListElement temp;
		std::vector<TAuctionListElement> vec;
		vec.reserve(m_mapAuctions.size());

		for (auto& mapAuction : m_mapAuctions)
		{
			const CAuction& rAuction = mapAuction.second;

			CopyObject(temp.actual_best, rAuction.GetBestOffer());
			CopyObject(temp.auction, rAuction.GetInfo());

			temp.dwOfferCount = rAuction.GetOffers().size();
			vec.push_back(temp);
		}

		SendAuctionListClientPacket(ch, vec);
		return true;
	}



	bool CShopManager::RecvAuctionOpenRequestClientPacket(LPCHARACTER ch, uint32_t dwOwnerID)
	{
		auto it = m_mapAuctions.find(dwOwnerID);
		if (it == m_mapAuctions.end())
		{
			return false;
		}
		ch->SetOfflineShopUseTime();

		it->second.AddGuest(ch);
		//SendAuctionOpenAuctionClientPacket(ch, it->second.GetInfo(), it->second.GetOffers());
		return true;
	}



	bool CShopManager::RecvMyAuctionOpenRequestClientPacket(LPCHARACTER ch)
	{
		OFFSHOP_DEBUG("pid %u , exist %s ",ch->GetPlayerID(), m_mapAuctions.find(ch->GetPlayerID()) != m_mapAuctions.end() ? "TRUE" : "FALSE" );

		ch->SetOfflineShopUseTime();

		if (!ch->GetAuction())
		{
			auto it = m_mapAuctions.find(ch->GetPlayerID());

			if (it == m_mapAuctions.end())
				SendAuctionOpenMyAuctionNoAuctionClientPacket(ch);
				
			else
			{
				it->second.AddGuest(ch);
				//SendAuctionOpenAuctionClientPacket(ch, it->second.GetInfo(), it->second.GetOffers());
			}
			
		}

		else
		{
			CAuction* pkAuction = ch->GetAuction();
			pkAuction->AddGuest(ch);
			//SendAuctionOpenAuctionClientPacket(ch, pkAuction->GetInfo(), pkAuction->GetOffers());
		}

		return true;
	}



	bool CShopManager::RecvAuctionCreateClientPacket(LPCHARACTER ch, DWORD dwDuration, const TPriceInfo& init_price, const TItemPos& pos)
	{
		//checking about duplicate item :D
		if(!ch || ch->GetAuction())
			return false;

		if (!_IS_VALID_GM_LEVEL(ch))
			return false;

		if (!ch->CanHandleItem() || !CheckCharacterActions(ch))
		{
			SendChatPacket(ch,CHAT_PACKET_CANNOT_DO_NOW);
			return false;
		}

		quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
		if (pPC == nullptr)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return false;
		}

		if (pPC->IsRunning())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return false;
		}

		if (pos.IsDragonSoulEquipPosition())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot create an auction now.");
			return false;
		}

		if (pos.IsEquipPosition())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot create an auction now.");
			return false;
		}

#ifdef ENABLE_SWITCHBOT_WORLDARD
		if (pos.IsSwitchBotPosition())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot create an auction now.");
			return false;
		}
#endif

		OFFSHOP_DEBUG("owner %u , duration %u ",ch->GetPlayerID(), dwDuration);

		//checking about hacking duration
		dwDuration = MIN(OFFLINESHOP_DURATION_MAX_MINUTES, dwDuration);


		//checking about duplicate item
		LPITEM item = ch->GetItem(pos);
		if(!item)
			return false;

		if(item->IsEquipped() || item->IsExchanging() || item->isLocked())
			return false;



		TItemTable* pItemTable= ITEM_MANAGER::instance().GetTable(item->GetVnum());
		if(!pItemTable)
			return false;

		if(IS_SET(pItemTable->dwAntiFlags, ITEM_ANTIFLAG_GIVE) || IS_SET(pItemTable->dwAntiFlags , ITEM_ANTIFLAG_MYSHOP))
			return false;

#ifdef ENABLE_SOULBIND_SYSTEM
		if (item->IsSealed()) {
			return false;
		}
#endif

		if (init_price.illYang <= 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "That is not the correct amount of Yang.");
			return false;
		}

#ifdef __ENABLE_CHEQUE_SYSTEM__
		if (init_price.iCheque < 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "That is not the correct amount of Won.");
			return false;
		}
#endif

		ch->SetOfflineShopUseTime();

		//making info
		TAuctionInfo auction;
		auction.dwDuration = dwDuration;
		auction.dwOwnerID  = ch->GetPlayerID();
		strncpy(auction.szOwnerName, ch->GetName(), sizeof(auction.szOwnerName));
		CopyObject(auction.init_price, init_price);


		TItemInfoEx& itemInfo = auction.item;
		itemInfo.dwCount	= item->GetCount();
		itemInfo.dwVnum		= item->GetVnum();
		
		//patch 08-03-2020
		itemInfo.expiration = GetItemExpiration(item);

		memcpy(itemInfo.aAttr ,		item->GetAttributes(),	sizeof(itemInfo.aAttr));
		memcpy(itemInfo.alSockets,	item->GetSockets(),		sizeof(itemInfo.alSockets));
		
#if defined(ENABLE_CHANGELOOK)
		itemInfo.dwTransmutation = item->GetTransmutation();
#endif
#ifdef ATTR_LOCK
		itemInfo.iLockedAttr = item->GetLockedAttr();
#endif

		//destroy/remove/send
		M2_DESTROY_ITEM(item->RemoveFromCharacter());
		SendAuctionCreateDBPacket(auction);
		return true;
	}



	bool CShopManager::RecvAuctionAddOfferClientPacket(LPCHARACTER ch, DWORD dwOwnerID, const TPriceInfo& price)
	{
#ifdef DISABLE_OFFSHOP_OFFERS
		return true;
#endif

		//checking about guesting
		if(!ch || !ch->GetDesc() || !ch->GetAuctionGuest() || ch->GetAuctionGuest()->GetInfo().dwOwnerID != dwOwnerID)
			return false;

		if (!_IS_VALID_GM_LEVEL(ch))
			return false;

		//check anti auto-offer
		if(ch->GetPlayerID() == dwOwnerID)
			return false;

		//check about enough money
		if((long long) ch->GetGold() < price.illYang)
			return false;

		if (price.illYang <= 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "That is not the correct amount of Yang you want to offer.");
			return true;
		}

		quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
		if (pPC == nullptr)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return true;
		}

		if (pPC->IsRunning())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can't use a private shop now.");
			return true;
		}

#ifdef __ENABLE_CHEQUE_SYSTEM__
		if ( ch->GetCheque() < price.iCheque)
			return false;

		if (price.iCheque < 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "That is not the correct amount of Won you want to offer.");
			return true;
		}
#endif

#ifdef ENABLE_SOULBIND_SYSTEM
		if (item->IsSealed()) {
			SendChatPacket(ch, CHAT_PACKET_CANNOT_DO_NOW);
			return true;
		}
#endif

		if (!CheckLastOfferTime(ch->GetPlayerID())) {
			SendChatPacket(ch, CHAT_PACKET_CANNOT_DO_NOW);
			return false;
		}

		//checking about raise from best buyer
		CAuction* pAuction = ch->GetAuctionGuest();
		if(pAuction->GetBestBuyer() == ch->GetPlayerID())
			return false;
		
		OFFSHOP_DEBUG("pAuction->GetBestBuyer() = %u ",pAuction->GetBestBuyer());

		if (pAuction->GetOffers().empty())
		{
			if (price < pAuction->GetInfo().init_price)
				return false;
		}

		else
		{
			//checking about min raise amount (+10% by default)
			const TPriceInfo& bestOffer = pAuction->GetBestOffer();
			if(!CheckNewAuctionOfferPrice(price,bestOffer))
				return false;
		}

		ch->SetOfflineShopUseTime();

#if defined(ENABLE_NEW_GOLD_LIMIT)
		ch->ChangeGold(-price.illYang);
#else
		ch->PointChange(POINT_GOLD, -price.illYang);
#endif
#ifdef __ENABLE_CHEQUE_SYSTEM__
		ch->PointChange(POINT_CHEQUE, -price.iCheque);
#endif

		TAuctionOfferInfo offer;
		offer.dwBuyerID	= ch->GetPlayerID();
		offer.dwOwnerID	= dwOwnerID;
		CopyObject(offer.price	, price);
#ifdef __ENABLE_CHEQUE_SYSTEM__
		// converting amount cheque in yang
		offer.price.illYang = offer.price.GetTotalYangAmount();
		offer.price.iCheque=0;
#endif

		strncpy(offer.szBuyerName, ch->GetName(), sizeof(offer.szBuyerName));


		SendAuctionAddOfferDBPacket(offer);
		ch->Save();
		return true;
	}



	bool CShopManager::RecvAuctionExitFromAuction(LPCHARACTER ch, DWORD dwOwnerID)
	{
		auto it = m_mapAuctions.find(ch->GetPlayerID());
		if(it == m_mapAuctions.end())
			return false;

		ch->SetOfflineShopUseTime();

		it->second.RemoveGuest(ch);
		return true;
	}




	void CShopManager::SendAuctionListClientPacket(LPCHARACTER ch, const std::vector<TAuctionListElement>& auctionVec)
	{
		if (!ch || !ch->GetDesc())
			return;

		TPacketGCNewOfflineshop pack;
		pack.bHeader = HEADER_GC_NEW_OFFLINESHOP;
		pack.bSubHeader = SUBHEADER_GC_AUCTION_LIST;
		pack.wSize = sizeof(pack) + sizeof(TSubPacketGCAuctionList) + (sizeof(TAuctionListElement) * auctionVec.size());
		TSubPacketGCAuctionList subpack;
		subpack.dwCount = auctionVec.size();
		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));
		if (!auctionVec.empty())
			buff.write(&auctionVec[0], sizeof(auctionVec[0]) * auctionVec.size());

		ch->GetDesc()->Packet(buff.read_peek(), buff.size());
	}




	void CShopManager::SendAuctionOpenAuctionClientPacket(LPCHARACTER ch, const TAuctionInfo& auction, const std::vector<TAuctionOfferInfo>& vec)
	{
		if (!ch->GetDesc())
			return;

		TPacketGCNewOfflineshop pack;
		pack.bHeader	= HEADER_GC_NEW_OFFLINESHOP;
		pack.bSubHeader	= ch->GetPlayerID()!=auction.dwOwnerID ? SUBHEADER_GC_OPEN_AUCTION: SUBHEADER_GC_OPEN_MY_AUCTION;
		pack.wSize		= sizeof(pack)+ sizeof(TSubPacketGCAuctionOpen) + (sizeof(TAuctionOfferInfo) * vec.size());

		TSubPacketGCAuctionOpen subpack;
		CopyObject(subpack.auction, auction);
		subpack.dwOfferCount = vec.size();


		TEMP_BUFFER buff;
		buff.write(&pack,		sizeof(pack));
		buff.write(&subpack,	sizeof(subpack));

		if(!vec.empty())
			buff.write(&vec[0], sizeof(vec[0]) * vec.size());

		ch->GetDesc()->Packet(buff.read_peek(), buff.size());
	}


	
	void CShopManager::SendAuctionOpenMyAuctionNoAuctionClientPacket(LPCHARACTER ch)
	{
		if(!ch || !ch->GetDesc())
			return;

		TPacketGCNewOfflineshop pack;
		pack.bHeader	= HEADER_GC_NEW_OFFLINESHOP;
		pack.bSubHeader	= SUBHEADER_GC_OPEN_MY_AUCTION_NO_AUCTION;
		pack.wSize		= sizeof(pack);

		ch->GetDesc()->Packet(&pack, sizeof(pack));
	}




	void CShopManager::RecvCloseBoardClientPacket(LPCHARACTER ch)
	{
		if(!ch || !ch->GetDesc())
			return;

		//auction
		if (ch->GetAuctionGuest())
		{
			ch->GetAuctionGuest()->RemoveGuest(ch);
			ch->SetAuctionGuest(NULL);
		}


		//safebox
		if(ch->GetShopSafebox())
			ch->SetShopSafebox(NULL);

		//shop
		if (ch->GetOfflineShopGuest())
		{
			ch->GetOfflineShopGuest()->RemoveGuest(ch);
			ch->SetOfflineShopGuest(NULL);
		}


		if(ch->GetOfflineShop())
			ch->GetOfflineShop()->RemoveGuest(ch);

		//offlineshop-updated 05/08/19
		ch->SetLookingOfflineshopOfferList(false);
	}

	void CShopManager::RecvCloseMyAuction(LPCHARACTER ch)
	{
		if(!ch || !ch->GetDesc())
			return;
		
		if (ch->GetAuction())
		{
			TPacketGDNewOfflineShop pack;
			pack.bSubHeader = SUBHEADER_GD_AUCTION_CLOSE;

			TSubPacketGDAuctionClose subpack;
			CopyObject(subpack.dwOwnerID , ch->GetPlayerID());
			
			TEMP_BUFFER buff;
			buff.write(&pack, sizeof(pack));
			buff.write(&subpack, sizeof(subpack));

			db_clientdesc->DBPacket(HEADER_GD_NEW_OFFLINESHOP , 0 , buff.read_peek() , buff.size());
		}
	}

	void CShopManager::UpdateShopsDuration()
	{
		auto it = m_mapShops.begin();
		for (; it != m_mapShops.end(); it++)
		{
			CShop& shop = it->second;

			if(shop.GetDuration() > 0)
				shop.DecreaseDuration();
		}
	}





	void CShopManager::UpdateAuctionsDuration()
	{
		auto it = m_mapAuctions.begin();
		for (; it != m_mapAuctions.end(); it++)
		{
			CAuction& auction = it->second;
			auction.DecreaseDuration();
		}
	}


	void CShopManager::ClearSearchTimeMap()
	{
		m_searchTimeMap.clear();

		// fix flooding offers
		m_offerCooldownMap.clear();
	}

	// fix flooding offers
	bool CShopManager::CheckOfferCooldown(DWORD dwPID) {
		DWORD now = get_dword_time();
		const DWORD cooldown_seconds = 15;

		auto it = m_offerCooldownMap.find(dwPID);
		if (it == m_offerCooldownMap.end()) {
			m_offerCooldownMap[dwPID] = now + cooldown_seconds *1000;
			return true;
		}

		if (it->second > now)
			return false;

		it->second = now + cooldown_seconds * 1000;
		return true;
	}


	bool CShopManager::CheckSearchTime(DWORD dwPID) 
	{
		auto it = m_searchTimeMap.find(dwPID);
		if (it == m_searchTimeMap.end())
		{
			m_searchTimeMap.insert(std::make_pair(dwPID, get_dword_time()));
			return true;
		}

		if(it->second + OFFLINESHOP_SECONDS_PER_SEARCH*1000 > get_dword_time())
		{
			return false;
		}

		it->second = get_dword_time();
		return true;
	}

	//*new check about auction offers
	bool CShopManager::CheckLastOfferTime(DWORD dwPID) 
	{
		auto it = m_offerTimeMap.find(dwPID);
		if (it == m_offerTimeMap.end())
		{
			m_offerTimeMap.insert(std::make_pair(dwPID, get_dword_time()));
			return true;
		}

		if (it->second + OFFLINESHOP_SECONDS_PER_OFFER * 1000 > get_dword_time())
			return false;

		it->second = get_dword_time();
		return true;
	}

	void CShopManager::ClearOfferTimeMap() 
	{
		m_offerTimeMap.clear();
	}

	//topatch 29-10
	void CShopManager::CheckOfferOnItem(DWORD dwOwnerID, DWORD dwItemID) 
	{
		//for (auto it; it != m_mapOffer.end();) {
		for (auto it = m_mapOffer.begin(); it != m_mapOffer.end();) {
			auto& vec = it->second;

			for (auto itVec = vec.begin(); itVec != vec.end();) {
				if (itVec->dwItemID == dwItemID)
					itVec = vec.erase(itVec);
				else
					itVec++;
			}

			if (vec.empty())
				it = m_mapOffer.erase(it);
			else
				it++;
		}
	}

}

#endif //__ENABLE_NEW_OFFLINESHOP__
