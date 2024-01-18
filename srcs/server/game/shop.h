#ifndef __INC_METIN_II_GAME_SHOP_H__
#define __INC_METIN_II_GAME_SHOP_H__

enum
{
	SHOP_MAX_DISTANCE = 1000
};

class CGrid;

/* ---------------------------------------------------------------------------------- */
class CShop
{
	public:
		typedef struct shop_item
		{
			DWORD			vnum;
#if defined(ENABLE_NEW_GOLD_LIMIT)
			uint64_t price;
#else
			uint32_t price;
#endif
			WORD count;
			LPITEM			pkItem;
			int				itemid;
#ifdef ENABLE_BUY_WITH_ITEM
			TShopItemPrice	itemprice[MAX_SHOP_PRICES];
#endif
			shop_item()
			{
				vnum = 0;
				price = 0;
				count = 0;
				itemid = 0;
#ifdef ENABLE_BUY_WITH_ITEM
				memset(itemprice, 0, sizeof(itemprice));
#endif
				pkItem = NULL;
			}
		} SHOP_ITEM;

		CShop();
		virtual ~CShop(); // @fixme139 (+virtual)

		bool	Create(DWORD dwVnum, DWORD dwNPCVnum, TShopItemTable * pItemTable);
		void	SetShopItems(TShopItemTable * pItemTable, BYTE bItemCount);

		virtual void	SetPCShop(LPCHARACTER ch);
		virtual bool	IsPCShop()	{ return m_pkPC ? true : false; }

		// 게스트 추가/삭제
		virtual bool	AddGuest(LPCHARACTER ch,DWORD owner_vid, bool bOtherEmpire);
		void	RemoveGuest(LPCHARACTER ch);

		// 물건 구입
#if defined(ENABLE_NEW_GOLD_LIMIT)
		virtual uint64_t Buy(LPCHARACTER ch, BYTE pos
#ifdef ENABLE_BUY_STACK_FROM_SHOP
, bool multiple = false
#endif
#if defined(ENABLE_ANTI_FLOOD)
, bool pass = false
#endif
);
#else
		virtual uint32_t Buy(LPCHARACTER ch, BYTE pos
#ifdef ENABLE_BUY_STACK_FROM_SHOP
, bool multiple = false
#endif
#if defined(ENABLE_ANTI_FLOOD)
, bool pass = false
#endif
);
#endif
#ifdef ENABLE_BUY_STACK_FROM_SHOP
		virtual uint8_t MultipleBuy(LPCHARACTER ch, uint8_t p, uint8_t c);
#endif
		// 게스트에게 패킷을 보냄
		void	BroadcastUpdateItem(BYTE pos);

		// 판매중인 아이템의 갯수를 알려준다.
		int		GetNumberByVnum(DWORD dwVnum);

		// 아이템이 상점에 등록되어 있는지 알려준다.
		virtual bool	IsSellingItem(DWORD itemID);

		DWORD	GetVnum() { return m_dwVnum; }
		DWORD	GetNPCVnum() { return m_dwNPCVnum; }

	protected:
		void	Broadcast(const void * data, int bytes);

	protected:
		DWORD				m_dwVnum;
		DWORD				m_dwNPCVnum;

		CGrid *				m_pGrid;

		typedef std::unordered_map<LPCHARACTER, bool> GuestMapType;
		GuestMapType m_map_guest;
		std::vector<SHOP_ITEM>		m_itemVector;	// 이 상점에서 취급하는 물건들

		LPCHARACTER			m_pkPC;
};

#endif
