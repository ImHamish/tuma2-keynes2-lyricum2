#ifndef __INC_METIN_II_GAME_INPUT_PROCESSOR__
#define __INC_METIN_II_GAME_INPUT_PROCESSOR__

#include "packet_info.h"

enum
{
	INPROC_CLOSE,
	INPROC_HANDSHAKE,
	INPROC_LOGIN,
	INPROC_MAIN,
	INPROC_DEAD,
	INPROC_DB,
	INPROC_UDP,
	INPROC_P2P,
	INPROC_AUTH,
	INPROC_GUILD_MARK,
};

void LoginFailure(LPDESC d, const char * c_pszStatus);
extern void SendShout(const char*, uint8_t
#if defined(ENABLE_MESSENGER_BLOCK)
, const char*
#endif
);

class CInputProcessor
{
	public:
		CInputProcessor();
		virtual ~CInputProcessor() = default;

		virtual bool Process(LPDESC d, const void * c_pvOrig, int iBytes, int & r_iBytesProceed);
		virtual BYTE GetType() = 0;

		void BindPacketInfo(CPacketInfo * pPacketInfo);
		void Pong(LPDESC d);
		void Handshake(LPDESC d, const char * c_pData);
		void Version(LPCHARACTER ch, const char* c_pData);
		virtual int Chat(LPDESC d, const char* data, size_t uiBytes);
	protected:
		virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData) = 0;

		CPacketInfo * m_pPacketInfo;
		int	m_iBufferLeft;

		CPacketInfoCG 	m_packetInfoCG;
};

class CInputClose : public CInputProcessor
{
	public:
		BYTE	GetType() override { return INPROC_CLOSE; }

	protected:
		int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData) override { return m_iBufferLeft; }
};

class CInputHandshake : public CInputProcessor
{
	public:
		CInputHandshake();
		virtual ~CInputHandshake();

		BYTE	GetType() override { return INPROC_HANDSHAKE; }

	protected:
		int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData) override;

	protected:
		static void		GuildMarkLogin(LPDESC d, const char* c_pData);

		CPacketInfo *	m_pMainPacketInfo;
};

class CInputLogin : public CInputProcessor
{
	public:
		BYTE	GetType() override { return INPROC_LOGIN; }

	protected:
		int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData) override;

	protected:
		void		LoginByKey(LPDESC d, const char * data);

		void		CharacterSelect(LPDESC d, const char * data);
		void		CharacterCreate(LPDESC d, const char * data);
		void		CharacterDelete(LPDESC d, const char * data);

		void		Entergame(LPDESC d, const char * data);
		void		Empire(LPDESC d, const char * c_pData);
		void		ChangeCountryFlag(LPDESC d, const char* c_pData);
};

class CInputGuildMark : public CInputProcessor
{
public:
	BYTE GetType() override { return INPROC_GUILD_MARK; }
protected:
	int	Analyze(LPDESC d, BYTE bHeader, const char* c_pData) override;
private:
	static void GuildMarkCRCList(LPDESC d, const char* c_pData);
	static void GuildMarkIDXList(LPDESC d, const char* c_pData);
	static int GuildMarkUpload(LPDESC d, const char* c_pData, size_t uiBytes);
	static void GuildSymbolCRC(LPDESC d, const char* c_pData);
};

class CInputMain : public CInputProcessor
{
	public:
		BYTE	GetType() override { return INPROC_MAIN; }

	protected:
		int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData) override;

	protected:
		void		Attack(LPCHARACTER ch, const BYTE header, const char* data);
		int32_t Whisper(LPCHARACTER, const char*, size_t);
#ifdef __NEWPET_SYSTEM__
		void		BraveRequestPetName(LPCHARACTER ch, const char* c_pData);
#endif
		int			Chat(LPDESC d, const char * data, size_t uiBytes) override;
		void		ItemUse(LPCHARACTER ch, const char * data);
		void		ItemDrop(LPCHARACTER ch, const char * data);
		void		ItemDrop2(LPCHARACTER ch, const char * data);
		void		ItemDestroy(LPCHARACTER ch, const char * data);
#if defined(ENABLE_EXCHANGE_FRAGMENTS)
		void FragmentsExchange(LPCHARACTER, const char *);
#endif
		void		ItemDivision(LPCHARACTER ch, const char * data);
		void		ItemMove(LPCHARACTER ch, const char * data);
		void		ItemPickup(LPCHARACTER ch, const char * data);
		void		ItemToItem(LPCHARACTER ch, const char * pcData);
		void		QuickslotAdd(LPCHARACTER ch, const char * data);
		void		QuickslotDelete(LPCHARACTER ch, const char * data);
		void		QuickslotSwap(LPCHARACTER ch, const char * data);
		int			Shop(LPCHARACTER ch, const char * data, size_t uiBytes);
#if defined(ENABLE_GAYA_RENEWAL)
		int GemShop(LPCHARACTER ch, const char * data, size_t uiBytes);
#endif
		void		OnClick(LPCHARACTER ch, const char * data);
		void		Exchange(LPCHARACTER ch, const char * data);
		void		Position(LPCHARACTER ch, const char * data);
		void		Move(LPCHARACTER ch, const char * data);
		int			SyncPosition(LPCHARACTER ch, const char * data, size_t uiBytes);
		void		FlyTarget(LPCHARACTER ch, const char * pcData, BYTE bHeader);
		void		UseSkill(LPCHARACTER ch, const char * pcData);

		void		ScriptAnswer(LPCHARACTER ch, const void * pvData);
		void		ScriptButton(LPCHARACTER ch, const void * pvData);
		void		ScriptSelectItem(LPCHARACTER ch, const void * pvData);

		void		QuestInputString(LPCHARACTER ch, const void * pvData);
		void		QuestConfirm(LPCHARACTER ch, const void* pvData);
		void		Target(LPCHARACTER ch, const char * pcData);
		void		Warp(LPCHARACTER ch, const char * pcData);
		void		SafeboxCheckin(LPCHARACTER ch, const char * c_pData);
		void		SafeboxCheckout(LPCHARACTER ch, const char * c_pData, bool bMall);
		void		SafeboxItemMove(LPCHARACTER ch, const char * data);
		int			Messenger(LPCHARACTER ch, const char* c_pData, size_t uiBytes);

		void PartyInvite(LPCHARACTER, const char*);
		void 		PartyInviteAnswer(LPCHARACTER ch, const char * c_pData);
		void		PartyRemove(LPCHARACTER ch, const char * c_pData);
		void		PartySetState(LPCHARACTER ch, const char * c_pData);
		void		PartyUseSkill(LPCHARACTER ch, const char * c_pData);
		void		PartyParameter(LPCHARACTER ch, const char * c_pData);
#ifdef __INGAME_WIKI__
		void		RecvWikiPacket(LPCHARACTER ch, const char * c_pData);
#endif
		int			Guild(LPCHARACTER ch, const char * data, size_t uiBytes);
		void		AnswerMakeGuild(LPCHARACTER ch, const char* c_pData);

		void		Fishing(LPCHARACTER ch, const char* c_pData);
		void		ItemGive(LPCHARACTER ch, const char* c_pData);
		void		Hack(LPCHARACTER ch, const char * c_pData);
		int			MyShop(LPCHARACTER ch, const char * c_pData, size_t uiBytes);
#if defined(USE_BATTLEPASS)
		int ReciveExtBattlePassActions(LPCHARACTER ch, const char* data, size_t uiBytes);
		int ReciveExtBattlePassPremiumItem(LPCHARACTER ch, const char* data, size_t uiBytes);
#endif
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
		void 		CubeRenewalSend(LPCHARACTER ch, const char* data);
#endif
#ifdef __ENABLE_PREMIUM_PLAYERS__
		void 		RecvPremiumPlayersPacket(LPCHARACTER ch, const char* c_pData);
#endif


		void		Refine(LPCHARACTER ch, const char* c_pData);
#ifdef ENABLE_SWITCHBOT_WORLDARD
		void 		SwitchBotSend(LPCHARACTER ch, const char * data);
#endif
#ifdef ENABLE_ACCE_SYSTEM
		void Acce(LPCHARACTER, const char*);
#endif
#if defined(ENABLE_CHANGELOOK)
		void ChangeLook(LPCHARACTER, const char*);
#endif
		void		Roulette(LPCHARACTER ch, const char* c_pData);

#ifdef ENABLE_NEW_FISHING_SYSTEM
		void FishingNew(LPCHARACTER ch, const char* c_pData);
#endif
#ifdef USE_CAPTCHA_SYSTEM
        void    Captcha(LPCHARACTER, const char*);
#endif
#if defined(ENABLE_NEW_FISH_EVENT)
		int32_t FishEvent(LPCHARACTER, const char*, size_t);
#endif
#if defined(ENABLE_LETTERS_EVENT)
		void TakeLettersReward(LPCHARACTER);
#endif
#if defined(ENABLE_AURA_SYSTEM)
		int32_t Aura(LPCHARACTER ch, const char*, size_t);
#endif
#if defined(USE_ATTR_6TH_7TH)
	void Attr67Add(LPCHARACTER ch, const char* c_pData);
#endif
#ifdef USE_MULTIPLE_OPENING
    void MultipleOpening(LPCHARACTER ch, const char* c_pData);
#endif
	void RequestMapBossInfo(LPCHARACTER ch);
	void WhisperInfo(LPCHARACTER ch, const char* c_pData);
};

class CInputDead : public CInputMain
{
	public:
		BYTE	GetType() override { return INPROC_DEAD; }

	protected:
		int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData) override;
};

class CInputDB : public CInputProcessor
{
public:
	bool Process(LPDESC d, const void * c_pvOrig, int iBytes, int & r_iBytesProceed) override;
	BYTE GetType() override { return INPROC_DB; }

protected:
	int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData) override;

protected:
	void		MapLocations(const char * c_pData);
	void		LoginSuccess(DWORD dwHandle, const char *data);
	void		PlayerCreateFailure(LPDESC d, BYTE bType);	// 0 = 일반 실패 1 = 이미 있음
	void		PlayerDeleteSuccess(LPDESC d, const char * data);
	void		PlayerDeleteFail(LPDESC d);
	void		PlayerLoad(LPDESC d, const char* data);
	void		PlayerCreateSuccess(LPDESC d, const char * data);
	void		Boot(const char* data);
	void		QuestLoad(LPDESC d, const char * c_pData);
	void		SafeboxLoad(LPDESC d, const char * c_pData);
	void		SafeboxChangeSize(LPDESC d, const char * c_pData);
	void		SafeboxWrongPassword(LPDESC d);
	void		SafeboxChangePasswordAnswer(LPDESC d, const char* c_pData);
	void		MallLoad(LPDESC d, const char * c_pData);
	void		EmpireSelect(LPDESC d, const char * c_pData);
	void		P2P(const char * c_pData);
	void		ItemLoad(LPDESC d, const char * c_pData);
	void		AffectLoad(LPDESC d, const char * c_pData);
#if defined(USE_BATTLEPASS)
	void ExtBattlePassLoad(LPDESC d, const char* c_pData);
#endif

	void		GuildLoad(const char * c_pData);
	void		GuildSkillUpdate(const char* c_pData);
	void		GuildSkillRecharge();
	void		GuildExpUpdate(const char* c_pData);
	void		GuildAddMember(const char* c_pData);
	void		GuildRemoveMember(const char* c_pData);
	void		GuildChangeGrade(const char* c_pData);
	void		GuildChangeMemberData(const char* c_pData);
	void		GuildDisband(const char* c_pData);
	void		GuildLadder(const char* c_pData);
	void		GuildWar(const char* c_pData);
	void		GuildWarScore(const char* c_pData);

	void		GuildSkillUsableChange(const char* c_pData);
	void		GuildMoneyChange(const char* c_pData);
	void		GuildWithdrawMoney(const char* c_pData);
	void		GuildWarReserveAdd(TGuildWarReserve * p);
	void		GuildWarReserveUpdate(TGuildWarReserve * p);
	void		GuildWarReserveDelete(DWORD dwID);
	void		GuildWarBet(TPacketGDGuildWarBet * p);
	void		GuildChangeMaster(TPacketChangeGuildMaster* p);

	void		LoginAlready(LPDESC d, const char * c_pData);

	void		PartyCreate(const char* c_pData);
	void		PartyDelete(const char* c_pData);
	void		PartyAdd(const char* c_pData);
	void		PartyRemove(const char* c_pData);
	void		PartyStateChange(const char* c_pData);
	void		PartySetMemberLevel(const char* c_pData);

	void		Time(const char * c_pData);

	void		ReloadProto(const char * c_pData);
	void		ChangeName(LPDESC d, const char * data);
	void		ChangeCountryFlag(LPDESC d, const char* c_pData);
	void		AuthLogin(LPDESC d, const char * c_pData);
	void		ItemAward(const char * c_pData);

	void		ChangeEmpirePriv(const char* c_pData);
	void		ChangeGuildPriv(const char* c_pData);
	void		ChangeCharacterPriv(const char* c_pData);

	void		MoneyLog(const char* c_pData);

	void		SetEventFlag(const char* c_pData);

	void		CreateObject(const char * c_pData);
	void		DeleteObject(const char * c_pData);
	void		UpdateLand(const char * c_pData);

	void		Notice(const char * c_pData);
	void		MarriageAdd(TPacketMarriageAdd * p);
	void		MarriageUpdate(TPacketMarriageUpdate * p);
	void		MarriageRemove(TPacketMarriageRemove * p);

	void		WeddingRequest(TPacketWeddingRequest* p);
	void		WeddingReady(TPacketWeddingReady* p);
	void		WeddingStart(TPacketWeddingStart* p);
	void		WeddingEnd(TPacketWeddingEnd* p);

	// MYSHOP_PRICE_LIST
	/// 아이템 가격정보 리스트 요청에 대한 응답 패킷(HEADER_DG_MYSHOP_PRICELIST_RES) 처리함수
	/**
	* @param	d 아이템 가격정보 리스트를 요청한 플레이어의 descriptor
	* @param	p 패킷데이터의 포인터
	*/
	void		MyshopPricelistRes( LPDESC d, const TPacketMyshopPricelistHeader* p );
	// END_OF_MYSHOP_PRICE_LIST
	//
	//RELOAD_ADMIN
	void ReloadAdmin( const char * c_pData );
	//END_RELOAD_ADMIN
#ifdef ENABLE_EVENT_MANAGER
	void EventManager(const char* c_pData);
#endif
	void		DetailLog(const TPacketNeedLoginLogInfo* info);
	// 독일 선물 기능 테스트
	void		ItemAwardInformer(TPacketItemAwardInfromer* data);
#if defined(BL_OFFLINE_MESSAGE)
	void		ReadOfflineMessages(LPDESC desc, const char* pcData);
#endif
	void		RespondChannelStatus(LPDESC desc, const char* pcData);
#ifdef ENABLE_CHANNEL_SWITCH_SYSTEM
	void		ChangeChannel(LPDESC desc, const char* pcData);
#endif
#if defined(ENABLE_INGAME_ITEMSHOP)
	void ItemShop(LPDESC d, const char* c_pData);
#endif

	void RecvGuildSymbolAuthKey(const TPacketGuildSymbolAuthKey*);
	void RecvGuildSymbolAuth(LPDESC d, const char* c_pData);

	protected:
		DWORD		m_dwHandle = 0;
};

class CInputP2P : public CInputProcessor
{
	public:
		CInputP2P();
		BYTE	GetType() override { return INPROC_P2P; }

	protected:
		int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData) override;

	public:
		void		Setup(LPDESC d, const char * c_pData);
		void		Login(LPDESC d, const char * c_pData);
		void		Logout(LPDESC d, const char * c_pData);
		int			Relay(LPDESC d, const char * c_pData, size_t uiBytes);
#ifdef ENABLE_FULL_NOTICE
		int			Notice(LPDESC d, const char * c_pData, size_t uiBytes, bool bBigFont=false);
#else
		int			Notice(LPDESC d, const char * c_pData, size_t uiBytes);
#endif
		int32_t NoticeNew(LPDESC d, const char * c_pData, size_t uiBytes);

		int			Guild(LPDESC d, const char* c_pData, size_t uiBytes);
		void Shout(const char*);
		void		Disconnect(const char * c_pData);
		void		MessengerAdd(const char * c_pData);
		void		MessengerRemove(const char * c_pData);
#if defined(ENABLE_MESSENGER_BLOCK)
		void MessengerBlock(const char*);
#endif
		void		MessengerMobile(const char * c_pData);
		void		FindPosition(LPDESC d, const char* c_pData);
		void		WarpCharacter(const char* c_pData);
		void		GuildWarZoneMapIndex(const char* c_pData);
		void		Transfer(const char * c_pData);
		void		LoginPing(LPDESC d, const char * c_pData);
		void		BlockChat(const char * c_pData);
		void		IamAwake(LPDESC d, const char * c_pData);
#if defined(ENABLE_OFFLINESHOP_REWORK)
		void		OfflineShopNotification(LPDESC d, const char* c_pData);
#endif
#ifdef ENABLE_REWARD_SYSTEM
		void		RewardInfo(const char* c_pData);
#endif
#if defined(ENABLE_CLOSE_GAMECLIENT_CMD)
		void CloseClient(const char*);
#endif
#if defined(ENABLE_ULTIMATE_REGEN)
		void NewRegen(const char*);
#endif

	protected:
		CPacketInfoGG 	m_packetInfoGG;
};

class CInputAuth : public CInputProcessor
{
	public:
		CInputAuth();
		BYTE GetType() override { return INPROC_AUTH; }

	protected:
		int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData) override;

	public:
		void Login(LPDESC, const char*);
};

#endif /* __INC_METIN_II_GAME_INPUT_PROCESSOR__ */

