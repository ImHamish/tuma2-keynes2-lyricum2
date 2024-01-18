#ifndef __INC_METIN_II_GAME_ITEM_H__
#define __INC_METIN_II_GAME_ITEM_H__

#include "entity.h"

class CItem : public CEntity
{
	protected:
		// override methods from ENTITY class
		virtual void	EncodeInsertPacket(LPENTITY entity);
		virtual void	EncodeRemovePacket(LPENTITY entity);

	public:
		CItem(DWORD dwVnum);
		virtual ~CItem();

		int			GetLevelLimit();

		bool		CheckItemUseLevel(int nLevel);

		long		FindApplyValue(BYTE bApplyType);

		bool		IsStackable()		{ return (GetFlag() & ITEM_FLAG_STACKABLE)?true:false; }

		void		Initialize();
		void		Destroy();

		void		Save();

		void		SetWindow(BYTE b)	{ m_bWindow = b; }
		BYTE		GetWindow()		{ return m_bWindow; }

		void		SetID(DWORD id)		{ m_dwID = id;	}
		DWORD		GetID()			{ return m_dwID; }

		void			SetProto(const TItemTable * table);
		TItemTable const *	GetProto()	{ return m_pProto; }

#ifdef ENABLE_ITEM_EXTRA_PROTO
		void		SetExtraProto(TItemExtraProto* Proto);
		TItemExtraProto* GetExtraProto();
		bool		HasExtraProto() { return m_ExtraProto != nullptr; }
#endif

#ifdef ATTR_LOCK
		short	GetLockedAttr() const	{return m_sLockedAttr;}
		void	SetLockedAttr(short sIndex);
		void	AddLockedAttr();
		void	ChangeLockedAttr();
		void	RemoveLockedAttr();
		bool	CheckHumanApply();
#endif
#if defined(ENABLE_NEW_GOLD_LIMIT)
		uint64_t GetGold();
		uint64_t GetShopBuyPrice();
#else
		uint32_t GetGold();
		uint32_t GetShopBuyPrice();
#endif
#ifdef ENABLE_MULTI_NAMES
		const char *	GetName(BYTE Lang=0);
#else
		const char *	GetName()		{ return m_pProto ? m_pProto->szLocaleName : NULL; }
#endif

		const char *	GetBaseName()		{ return m_pProto ? m_pProto->szName : NULL; }
		BYTE		GetSize()		{ return m_pProto ? m_pProto->bSize : 0;	}

		void		SetFlag(long flag)	{ m_lFlag = flag;	}
		long		GetFlag()		{ return m_lFlag;	}

		void		AddFlag(long bit);
		void		RemoveFlag(long bit);

		DWORD		GetWearFlag()		{ return m_pProto ? m_pProto->dwWearFlags : 0; }
		DWORD		GetAntiFlag()		{ return m_pProto ? m_pProto->dwAntiFlags : 0; }
		DWORD		GetImmuneFlag()		{ return m_pProto ? m_pProto->dwImmuneFlag : 0; }

		void		SetVID(DWORD vid)	{ m_dwVID = vid;	}
		DWORD		GetVID()		{ return m_dwVID;	}

		bool		SetCount(WORD count);
		WORD		GetCount();

		// GetVnum과 GetOriginalVnum에 대한 comment
		// GetVnum은 Masking 된 Vnum이다. 이를 사용함으로써, 아이템의 실제 Vnum은 10이지만, Vnum이 20인 것처럼 동작할 수 있는 것이다.
		// Masking 값은 ori_to_new.txt에서 정의된 값이다.
		// GetOriginalVnum은 아이템 고유의 Vnum으로, 로그 남길 때, 클라이언트에 아이템 정보 보낼 때, 저장할 때는 이 Vnum을 사용하여야 한다.
		//
		DWORD		GetVnum() const		{ return m_dwMaskVnum ? m_dwMaskVnum : m_dwVnum;	}
		DWORD		GetOriginalVnum() const		{ return m_dwVnum;	}
		BYTE		GetType() const		{ return m_pProto ? m_pProto->bType : 0;	}
		BYTE		GetSubType() const	{ return m_pProto ? m_pProto->bSubType : 0;	}
		BYTE		GetLimitType(DWORD idx) const { return m_pProto ? m_pProto->aLimits[idx].bType : 0;	}
		long		GetLimitValue(DWORD idx) const { return m_pProto ? m_pProto->aLimits[idx].lValue : 0;	}
#if defined(ENABLE_CHANGELOOK)
		uint32_t GetTransmutation() const { return m_dwTransmutation; }
		void SetTransmutation(uint32_t vnum);
		bool IsChangeLookReverseScroll() const { return GetVnum() == 72325; }
		bool IsMainWeapon() const {
			return GetType() == ITEM_WEAPON && (GetSubType() == WEAPON_SWORD
												 || GetSubType() == WEAPON_DAGGER
												 || GetSubType() == WEAPON_BOW
												 || GetSubType() == WEAPON_TWO_HANDED
												 || GetSubType() == WEAPON_BELL
												 || GetSubType() == WEAPON_FAN
				);
		}
		bool IsArmorBody() { return GetType() == ITEM_ARMOR && GetSubType() == ARMOR_BODY; }
		bool IsCostumeBody() const { return GetType() == ITEM_COSTUME && GetSubType() == COSTUME_BODY; }
		bool IsCostumeHair() const { return GetType() == ITEM_COSTUME && GetSubType() == COSTUME_HAIR; }
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
		bool IsCostumeWeapon() { return GetType() == ITEM_COSTUME && GetSubType() == COSTUME_WEAPON; }
#endif
#endif
#ifdef ENABLE_NEW_USE_POTION
		BYTE	GetApplyType(DWORD idx) const { return m_pProto ? m_pProto->aApplies[idx].bType : 0;}
		long	GetApplyValue(DWORD idx) const { return m_pProto ? m_pProto->aApplies[idx].lValue : 0;}
#endif
		long		GetValue(DWORD idx);

		void		SetCell(LPCHARACTER ch, WORD pos)	{ m_pOwner = ch, m_wCell = pos;	}
		WORD		GetCell()				{ return m_wCell;	}

		LPITEM		RemoveFromCharacter();
#ifdef __HIGHLIGHT_SYSTEM__
		bool	AddToCharacter(LPCHARACTER ch, TItemPos Cell, bool isHighLight = true);
#else
		bool	AddToCharacter(LPCHARACTER ch, TItemPos Cell);
#endif
		bool	CanAddToCharacter(LPCHARACTER ch, const TItemPos& Cell);
		LPCHARACTER	GetOwner()		{ return m_pOwner; }

		LPITEM		RemoveFromGround();
		bool		AddToGround(long lMapIndex, const PIXEL_POSITION & pos, bool skipOwnerCheck = false);

		int			FindEquipCell(LPCHARACTER ch, int bCandidateCell = -1);
		bool		IsEquipped() const		{ return m_bEquipped;	}
		bool		EquipTo(LPCHARACTER ch, BYTE bWearCell);
		bool		IsEquipable() const;

		bool		CanUsedBy(LPCHARACTER ch);

#ifdef USE_PICKUP_INCREASED_RANGE
        bool DistanceValid(LPCHARACTER ch, bool bPickup = false);
#else
        bool DistanceValid(LPCHARACTER ch);
#endif

		void		UpdatePacket();
		void		UsePacketEncode(LPCHARACTER ch, LPCHARACTER victim, struct packet_item_use * packet);

		void		SetExchanging(bool isOn = true);
		bool		IsExchanging()		{ return m_bExchanging;	}

		bool		IsTwohanded();

		bool		IsPolymorphItem();

		void ModifyPoints(bool bAdd);

		bool		CreateSocket(BYTE bSlot, BYTE bGold);
		const long *	GetSockets()		{ return &m_alSockets[0];	}
		long		GetSocket(int i)	{ return m_alSockets[i];	}

		void		SetSockets(const long * al);
		void		SetSocket(int i, long v, bool bLog = true);

		int		GetSocketCount();
		bool		AddSocket();

		const TPlayerItemAttribute* GetAttributes()		{ return m_aAttr;	}
		const TPlayerItemAttribute& GetAttribute(int i)	{ return m_aAttr[i];	}

		BYTE		GetAttributeType(int i)	{ return m_aAttr[i].bType;	}
		short		GetAttributeValue(int i){ return m_aAttr[i].sValue;	}

		void		SetAttributes(const TPlayerItemAttribute* c_pAttribute);

		int		FindAttribute(BYTE bType);
		bool		RemoveAttributeAt(int index);
		bool		RemoveAttributeType(BYTE bType);

		bool		HasAttr(BYTE bApply);
		bool		HasRareAttr(BYTE bApply);

		void		SetDestroyEvent(LPEVENT pkEvent);
		void		StartDestroyEvent(int iSec=300);

		DWORD		GetRefinedVnum()	{ return m_pProto ? m_pProto->dwRefinedVnum : 0; }
		DWORD		GetRefineFromVnum();
		int		GetRefineLevel();

		void		SetSkipSave(bool b)	{ m_bSkipSave = b; }
		bool		GetSkipSave()		{ return m_bSkipSave; }

		bool IsOwnershipEvent(LPCHARACTER ch);
		bool		IsOwnership(LPCHARACTER ch);
		void		SetOwnership(LPCHARACTER ch, int iSec = 10);
		void		SetOwnershipEvent(LPEVENT pkEvent);

		DWORD		GetLastOwnerPID()	{ return m_dwLastOwnerPID; }

		int		GetAttributeSetIndex(); // 속성 붙는것을 지정한 배열의 어느 인덱스를 사용하는지 돌려준다.
		void		AlterToMagicItem();
		void		AlterToSocketItem(int iSocketCount);

		WORD		GetRefineSet()		{ return m_pProto ? m_pProto->wRefineSet : 0;	}

		void		StartUniqueExpireEvent();
		void		SetUniqueExpireEvent(LPEVENT pkEvent);

		void		StartTimerBasedOnWearExpireEvent();
		void		SetTimerBasedOnWearExpireEvent(LPEVENT pkEvent);

		void StartRealTimeExpireEvent();
		void StopRealTimeExpireEvent();
		void CleanRealTimeExpireEvent();
		bool		IsRealTimeItem();
		bool		IsRealTimeFirstUseItem();
		bool		IsUnlimitedTimeUnique();

		void		StopUniqueExpireEvent();
		void		StopTimerBasedOnWearExpireEvent();
		void		StopAccessorySocketExpireEvent();
		bool		IsAccessorySocketExpireEvent() const { return m_pkAccessorySocketExpireEvent != nullptr; }
		//			일단 REAL_TIME과 TIMER_BASED_ON_WEAR 아이템에 대해서만 제대로 동작함.
		int			GetDuration();

		int		GetAttributeCount();
		void		ClearAttribute();
		void		ChangeAttribute(const int* aiChangeProb=NULL);
		void		AddAttribute();
		void		AddAttribute(BYTE bType, short sValue);

#ifdef ENABLE_SWITCHBOT_WORLDARD
		void 		ApplyAddon(int iAddonType, bool switchbot = false);
#else
		void 		ApplyAddon(int iAddonType);
#endif

		int		GetSpecialGroup() const;
		bool	IsSameSpecialGroup(const LPITEM item) const;

		// ACCESSORY_REFINE
		// 액세서리에 광산을 통해 소켓을 추가
		bool		IsAccessoryForSocket();

		int		GetAccessorySocketGrade();
		int		GetAccessorySocketMaxGrade();
		int		GetAccessorySocketDownGradeTime();

		void		SetAccessorySocketGrade(int iGrade
#ifdef ENABLE_INFINITE_RAFINES
		, bool infinite = false
#endif
		);
		void		SetAccessorySocketMaxGrade(int iMaxGrade);
		void		SetAccessorySocketDownGradeTime(DWORD time);

		void		AccessorySocketDegrade();

		// 악세사리 를 아이템에 밖았을때 타이머 돌아가는것( 구리, 등 )
		void		StartAccessorySocketExpireEvent();
		void		SetAccessorySocketExpireEvent(LPEVENT pkEvent);

		bool		CanPutInto(LPITEM item);
#ifdef ENABLE_INFINITE_RAFINES
		bool		CanPutInto2(LPITEM item);
#endif
		// END_OF_ACCESSORY_REFINE

		void		CopyAttributeTo(LPITEM pItem);
		void		CopySocketTo(LPITEM pItem);

		int			GetRareAttrCount();
		bool		AddRareAttribute();
		bool		ChangeRareAttribute();

		void		AttrLog();

		void		Lock(bool f) { m_isLocked = f; }
		bool		isLocked() const { return m_isLocked; }

	private :
		void		SetAttribute(int i, BYTE bType, short sValue);
	public:
		void		SetForceAttribute(int i, BYTE bType, short sValue);

	protected:
		bool		EquipEx(bool is_equip);
		bool		Unequip();

		void		AddAttr(BYTE bApply, BYTE bLevel);
		void		PutAttribute(const int * aiAttrPercentTable);
		void		PutAttributeWithLevel(BYTE bLevel);

	public:
		void		AddRareAttribute2(const int * aiAttrPercentTable = NULL);
	protected:
		void		AddRareAttr(BYTE bApply, BYTE bLevel);
		void		PutRareAttribute(const int * aiAttrPercentTable);
		void		PutRareAttributeWithLevel(BYTE bLevel);

	protected:
		friend class CInputDB;
		friend class CHARACTER;
		bool		OnAfterCreatedItem();

	public:
		bool IsRideItem();
#if defined(ENABLE_MOUNTSKIN)
		bool IsRideSkin();
#endif
#if defined(USE_MOUNT_COSTUME_SYSTEM)
		uint32_t GetMountVnum();
#endif
		bool		IsRamadanRing();
		void ClearMountAttributeAndAffect();

		// 독일에서 기존 캐시 아이템과 같지만, 교환 가능한 캐시 아이템을 만든다고 하여,
		// 오리지널 아이템에, 교환 금지 플래그만 삭제한 새로운 아이템들을 새로운 아이템 대역에 할당하였다.
		// 문제는 새로운 아이템도 오리지널 아이템과 같은 효과를 내야하는데,
		// 서버건, 클라건, vnum 기반으로 되어있어
		// 새로운 vnum을 죄다 서버에 새로 다 박아야하는 안타까운 상황에 맞닿았다.
		// 그래서 새 vnum의 아이템이면, 서버에서 돌아갈 때는 오리지널 아이템 vnum으로 바꿔서 돌고 하고,
		// 저장할 때에 본래 vnum으로 바꿔주도록 한다.

		// Mask vnum은 어떤 이유(ex. 위의 상황)로 인해 vnum이 바뀌어 돌아가는 아이템을 위해 있다.
		void		SetMaskVnum(DWORD vnum)	{	m_dwMaskVnum = vnum; }
		DWORD		GetMaskVnum()			{	return m_dwMaskVnum; }
		bool		IsMaskedItem()	{	return m_dwMaskVnum != 0;	}


		// 용혼석
		bool		IsDragonSoul();
		int		GiveMoreTime_Per(float fPercent);
		int		GiveMoreTime_Fix(DWORD dwTime);
#ifdef ENABLE_SWITCHBOT_WORLDARD
	public:
		int		ChangeAttributeSwtichBot(int count,const TPlayerItemAttribute* aAttr,bool*check_add);
		void	SetAttributeSwtichBot(int i, BYTE bType, short sValue);
		int		GetCountSelect(const TPlayerItemAttribute* aAttr);
		bool 	CheckBonusSwitchBot(const TPlayerItemAttribute* aAttr);
		void 	AddAttributeSwitchBot(BYTE bApply, short sValue);

#endif
#ifdef ENABLE_SOUL_SYSTEM
	public:
		void		StartSoulItemEvent();
		void		SetSoulItemEvent(LPEVENT pkEvent);
#endif

	private:
		TItemTable const * m_pProto;		// 프로토 타잎

		DWORD		m_dwVnum;
		LPCHARACTER	m_pOwner;

		BYTE		m_bWindow;		// 현재 아이템이 위치한 윈도우
		DWORD		m_dwID;			// 고유번호
		bool		m_bEquipped;	// 장착 되었는가?
		DWORD		m_dwVID;		// VID
		WORD		m_wCell;		// 위치
		DWORD		m_dwCount;		// 개수
#if defined(ENABLE_CHANGELOOK)
		uint32_t m_dwTransmutation;
#endif
#ifdef ATTR_LOCK
		DWORD		m_sLockedAttr;
#endif
#ifdef ENABLE_ITEM_EXTRA_PROTO
		TItemExtraProto* m_ExtraProto;
#endif
		long		m_lFlag;		// 추가 flag
		DWORD		m_dwLastOwnerPID;	// 마지막 가지고 있었던 사람의 PID

		bool		m_bExchanging;	///< 현재 교환중 상태

		long		m_alSockets[ITEM_SOCKET_MAX_NUM];	// 아이템 소캣
		TPlayerItemAttribute	m_aAttr[ITEM_ATTRIBUTE_MAX_NUM];

		LPEVENT		m_pkDestroyEvent;
		LPEVENT		m_pkExpireEvent;
#ifdef ENABLE_SOUL_SYSTEM
		LPEVENT		m_pkSoulItemEvent;
#endif
		LPEVENT		m_pkUniqueExpireEvent;
		LPEVENT		m_pkTimerBasedOnWearExpireEvent;
		LPEVENT		m_pkRealTimeExpireEvent;
		LPEVENT		m_pkAccessorySocketExpireEvent;
		LPEVENT		m_pkOwnershipEvent;
		DWORD		m_dwOwnershipPID;

		bool		m_bSkipSave;

		bool		m_isLocked;

		DWORD		m_dwMaskVnum;
		DWORD		m_dwSIGVnum;
	public:
		void SetSIGVnum(DWORD dwSIG)
		{
			m_dwSIGVnum = dwSIG;
		}
		DWORD	GetSIGVnum() const
		{
			return m_dwSIGVnum;
		}
#ifdef ENABLE_EXTRA_INVENTORY
		bool	IsExtraItem();
		BYTE	GetExtraCategory();
#endif

#if defined(USE_ATTR_6TH_7TH)
	public:
		uint32_t GetAttr67MaterialVnum();
#endif

#ifdef USE_PICKUP_FILTER
    public:
        bool FilterCheckRefine(uint8_t bRefine1, uint8_t bRefine2);
        bool FilterCheckLevel(uint8_t bLevel1, uint8_t bLevel2);
#endif

#ifdef USE_MULTIPLE_OPENING
    private:
        bool m_bUpdateStatus;

    public:
        bool GetUpdateStatus() const
        {
            return m_bUpdateStatus;
        }

        void SetUpdateStatus(const bool v)
        {
            m_bUpdateStatus = v;
        }
#endif
};

EVENTINFO(item_event_info)
{
	LPITEM item;
	char szOwnerName[CHARACTER_NAME_MAX_LEN];

	item_event_info()
	: item( 0 )
	{
		::memset( szOwnerName, 0, CHARACTER_NAME_MAX_LEN );
	}
};

EVENTINFO(item_vid_event_info)
{
	DWORD item_vid;
#ifdef ENABLE_NEW_USE_POTION
	bool newpotion;
#endif

	item_vid_event_info()
	: item_vid( 0 )
#ifdef ENABLE_NEW_USE_POTION
	, newpotion(false)
#endif
	{
	}
};

#endif
