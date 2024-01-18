#ifndef __INC_METIN_II_GAME_EXCHANGE_H__
#define __INC_METIN_II_GAME_EXCHANGE_H__

class CGrid;

enum EExchangeValues
{
#ifdef __NEW_EXCHANGE_WINDOW__
	EXCHANGE_ITEM_MAX_NUM = 24,
#else
	EXCHANGE_ITEM_MAX_NUM = 12,
#endif
	EXCHANGE_MAX_DISTANCE = 1000
};

class CExchange
{
	public:
		CExchange(LPCHARACTER pOwner);
		~CExchange();

		bool		Accept(bool bIsAccept = true);
		void		Cancel();
#if defined(ENABLE_NEW_GOLD_LIMIT)
		bool AddGold(uint64_t);
#else
		bool AddGold(uint32_t);
#endif
#if defined(ITEM_CHECKINOUT_UPDATE)
		int			GetEmptyExchange(BYTE size);
		bool		AddItem(TItemPos item_pos, BYTE display_pos, bool SelectPosAuto);
#else
		bool		AddItem(TItemPos item_pos, BYTE display_pos);
#endif
		bool		RemoveItem(BYTE pos);

		LPCHARACTER	GetOwner()	{ return m_pOwner;	}
		CExchange *	GetCompany()	{ return m_pCompany;	}

		bool		GetAcceptStatus() { return m_bAccept; }

		void		SetCompany(CExchange * pExchange)	{ m_pCompany = pExchange; }

	private:
		bool		Done();
		bool		Check(int * piItemCount);
		bool		CheckSpace();
		bool IsEmpty() const;

	private:
		CExchange *	m_pCompany;	// 상대방의 CExchange 포인터

		LPCHARACTER	m_pOwner;

		TItemPos		m_aItemPos[EXCHANGE_ITEM_MAX_NUM];
		LPITEM		m_apItems[EXCHANGE_ITEM_MAX_NUM];
		BYTE		m_abItemDisplayPos[EXCHANGE_ITEM_MAX_NUM];

		bool 		m_bAccept;
#if defined(ENABLE_NEW_GOLD_LIMIT)
		uint64_t m_lGold;
#else
		uint32_t m_lGold;
#endif
		CGrid* m_pGrid;
};
#endif
