#pragma once

#include "../eterLib/FuncObject.h"
#include "../eterlib/NetStream.h"
#include "../eterLib/NetPacketHeaderMap.h"
#include "InsultChecker.h"

#include "packet.h"

class CInstanceBase;
class CNetworkActorManager;
struct SNetworkActorData;
struct SNetworkUpdateActorData;

class CPythonNetworkStream : public CNetworkStream, public CSingleton<CPythonNetworkStream>
{
	public:
		enum
		{
			SERVER_COMMAND_LOG_OUT = 0,
			SERVER_COMMAND_RETURN_TO_SELECT_CHARACTER = 1,
			SERVER_COMMAND_QUIT = 2,

			MAX_ACCOUNT_PLAYER
		};

		enum
		{
			ERROR_NONE,
			ERROR_UNKNOWN,
			ERROR_CONNECT_MARK_SERVER,
			ERROR_LOAD_MARK,
			ERROR_MARK_WIDTH,
			ERROR_MARK_HEIGHT,

			// MARK_BUG_FIX
			ERROR_MARK_UPLOAD_NEED_RECONNECT,
			ERROR_MARK_CHECK_NEED_RECONNECT,
			// END_OF_MARK_BUG_FIX
		};

		enum
		{
			ACCOUNT_CHARACTER_SLOT_ID,
			ACCOUNT_CHARACTER_SLOT_NAME,
			ACCOUNT_CHARACTER_SLOT_RACE,
			ACCOUNT_CHARACTER_SLOT_LEVEL,
			ACCOUNT_CHARACTER_SLOT_STR,
			ACCOUNT_CHARACTER_SLOT_DEX,
			ACCOUNT_CHARACTER_SLOT_HTH,
			ACCOUNT_CHARACTER_SLOT_INT,
			ACCOUNT_CHARACTER_SLOT_PLAYTIME,
			ACCOUNT_CHARACTER_SLOT_FORM,
			ACCOUNT_CHARACTER_SLOT_ADDR,
			ACCOUNT_CHARACTER_SLOT_PORT,
			ACCOUNT_CHARACTER_SLOT_GUILD_ID,
			ACCOUNT_CHARACTER_SLOT_GUILD_NAME,
			ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG,
			ACCOUNT_CHARACTER_SLOT_HAIR,
#ifdef ENABLE_ACCE_SYSTEM
			ACCOUNT_CHARACTER_SLOT_ACCE,
#endif
#if defined(ENABLE_AURA_SYSTEM)
			ACCOUNT_CHARACTER_SLOT_AURA,
#endif
			ACCOUNT_CHARACTER_SLOT_COUNTRY_FLAG,
		};

		enum
		{
			PHASE_WINDOW_LOGO,
			PHASE_WINDOW_LOGIN,
			PHASE_WINDOW_SELECT,
			PHASE_WINDOW_CREATE,
			PHASE_WINDOW_LOAD,
			PHASE_WINDOW_GAME,
			PHASE_WINDOW_EMPIRE,
			PHASE_WINDOW_NUM,
		};

	public:
		CPythonNetworkStream();
		virtual ~CPythonNetworkStream();

		bool IsLoadingPhase() { return (m_strPhase == "Loading"); }

		bool SendSpecial(int nLen, void * pvBuf);

		void StartGame();
		void Warp(LONG lGlobalX, LONG lGlobalY);

		void SetWaitFlag();
		void NotifyHack(const char* c_szMsg);
		void SendEmoticon(UINT eEmoticon);

		void ExitApplication();
		void ExitGame();
		void LogOutGame();
		void AbsoluteExitGame();
		void AbsoluteExitApplication();

		void EnableChatInsultFilter(bool isEnable);
		bool IsChatInsultIn(const char* c_szMsg);
		bool IsInsultIn(const char* c_szMsg);

		DWORD GetGuildID();
		void SetMarkImageFileName(const char* c_szImageFileName) { m_strMarkImageFileName = c_szImageFileName; }
		std::string GetMarkImageFileName() const { return m_strMarkImageFileName; }

		UINT UploadMark(DWORD dwPlayerID, const char* c_szImageFileName);
		UINT UploadSymbol(const char* c_szImageFileName);

		bool LoadInsultList(const char* c_szInsultListFileName);
		bool LoadConvertTable(DWORD dwEmpireID, const char* c_szFileName);

		UINT		GetAccountCharacterSlotDatau(UINT iSlot, UINT eType);
		const char* GetAccountCharacterSlotDataz(UINT iSlot, UINT eType);

		// SUPPORT_BGM
		const char*		GetFieldMusicFileName();
		float			GetFieldMusicVolume();
		// END_OF_SUPPORT_BGM

		bool IsSelectedEmpire();

		void ToggleGameDebugInfo();

		void SetMarkServer(const char* c_szAddr, UINT uPort);
		void ConnectLoginServer(const char* c_szAddr, UINT uPort);
		void ConnectGameServer(UINT iChrSlot);

		void SetLoginInfo(const char* c_szID, const char* c_szPassword);
		void SetLoginKey(DWORD dwLoginKey);
		void ClearLoginInfo( void );

		void SetHandler(PyObject* poHandler);
		void SetPhaseWindow(UINT ePhaseWnd, PyObject* poPhaseWnd);
		void ClearPhaseWindow(UINT ePhaseWnd, PyObject* poPhaseWnd);
		void SetServerCommandParserWindow(PyObject* poPhaseWnd);
#ifdef ENABLE_OPENSHOP_PACKET
		bool SendOpenShopPacket(int32_t shopid);
#endif
		bool SendSyncPositionElementPacket(DWORD dwVictimVID, DWORD dwVictimX, DWORD dwVictimY);

		bool SendAttackPacket(
#ifdef ENABLE_NEW_ATTACK_METHOD
uint32_t type, 
#endif
UINT uMotAttack, DWORD dwVIDVictim
#ifdef ENABLE_NEW_ATTACK_METHOD
, uint32_t crc32
#endif
		);
		bool SendCharacterStatePacket(const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg);
		bool SendUseSkillPacket(DWORD dwSkillIndex, DWORD dwTargetVID=0);

		bool SendTargetPacket(DWORD dwVID);

		// OLDCODE:
		bool SendCharacterStartWalkingPacket(float fRotation, long lx, long ly);
		bool SendCharacterEndWalkingPacket(float fRotation, long lx, long ly);
		bool SendCharacterCheckWalkingPacket(float fRotation, long lx, long ly);

		bool SendCharacterPositionPacket(BYTE iPosition);

		bool CheckCannotWhileAttacking(TItemPos);

		bool SendItemUsePacket(TItemPos pos);
#if defined(ENABLE_MULTI_ITEM_USE)
		bool SendMultiItemUsePacket(uint16_t, TItemPos);
#endif
		bool SendItemUseToItemPacket(TItemPos source_pos, TItemPos target_pos);
		bool SendItemDropPacket(TItemPos pos);
		bool SendItemDestroyPacket(TItemPos pos);
		bool SendItemDivisionPacket(TItemPos pos);

		bool SendItemDropPacketNew(TItemPos pos, DWORD count);
		bool SendItemMovePacket(TItemPos pos, TItemPos change_pos, WORD num);
		bool SendItemPickUpPacket(DWORD vid);
#ifdef INGAME_WIKI
		bool SendWikiRequestInfo(uint64_t retID, DWORD vnum, bool isMob);
#endif
#ifdef INGAME_WIKI
		void ToggleWikiWindow();
#endif
		bool SendQuickSlotAddPacket(uint8_t, uint8_t, uint16_t);
		bool SendQuickSlotDelPacket(uint8_t);
		bool SendQuickSlotMovePacket(uint8_t, uint8_t);

		// PointReset 개 임시
		bool SendPointResetPacket();

		// Shop
		bool SendShopEndPacket();
		bool SendShopBuyPacket(BYTE bPos);
#ifdef ENABLE_BUY_STACK_FROM_SHOP
		bool SendShopBuyMultiplePacket(uint8_t p, uint8_t c);
#endif
		bool SendShopSellPacket(BYTE bySlot);
#ifdef ENABLE_EXTRA_INVENTORY
		bool SendShopSellPacketNew(TItemPos pos, WORD byCount);
#else
		bool SendShopSellPacketNew(BYTE bySlot, WORD byCount);
#endif
#if defined(ENABLE_NEW_FISH_EVENT)
		bool SendFishBoxUse(uint8_t, WORD);
		bool SendFishShapeAdd(uint8_t);
		bool RecvFishEventInfo();
#endif
#if defined(ENABLE_EXCHANGE_FRAGMENTS)
		bool SendExchangeFragmentsPacket(TItemPos);
#endif
#if defined(ENABLE_GAYA_RENEWAL)
		bool SendGemShopBuy(BYTE);
		bool SendGemShopAdd(BYTE);
		bool SendGemShopRefresh();
#endif

		// Exchange
		bool SendExchangeStartPacket(DWORD vid);
#if defined(ITEM_CHECKINOUT_UPDATE)
		bool SendExchangeItemAddPacket(TItemPos ItemPos, BYTE byDisplayPos, bool SelectPosAuto);
#else
		bool SendExchangeItemAddPacket(TItemPos ItemPos, BYTE byDisplayPos);
#endif
#if defined(ENABLE_NEW_GOLD_LIMIT)
		bool SendExchangeElkAddPacket(uint64_t);
#else
		bool SendExchangeElkAddPacket(uint32_t);
#endif
#if defined(ENABLE_EVENT_MANAGER)
		bool RecvEventManager();
#endif
#ifdef USE_CAPTCHA_SYSTEM
        bool    RecvCaptcha();
        bool    SendCaptcha(uint16_t code);
#endif
#if defined(ENABLE_CLOSE_GAMECLIENT_CMD)
		void RecvCloseClient();
#endif
#if defined(ENABLE_NEW_ANTICHEAT_RULES)
		void RecvAntiCheat();
#endif

		bool SendExchangeItemDelPacket(BYTE pos);
		bool SendExchangeAcceptPacket();
		bool SendExchangeExitPacket();

		// Quest
		bool SendScriptAnswerPacket(int iAnswer);
		bool SendScriptButtonPacket(unsigned int iIndex);
		bool SendAnswerMakeGuildPacket(const char * c_szName);
		bool SendQuestInputStringPacket(const char * c_szString);
		bool SendQuestConfirmPacket(BYTE byAnswer, DWORD dwPID);

		// Event
		bool SendOnClickPacket(DWORD vid);

		// Fly
		bool SendFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition& kPPosTarget);
		bool SendAddFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition& kPPosTarget);
		bool SendShootPacket(UINT uSkill);

		// Command
		bool ClientCommand(const char * c_szCommand);
		void ServerCommand(char * c_szCommand);

		// Emoticon
		void RegisterEmoticonString(const char * pcEmoticonString);

		// Party
		bool SendPartyInvitePacket(DWORD dwVID);
		bool SendPartyInviteAnswerPacket(DWORD dwLeaderVID, BYTE byAccept);
		bool SendPartyRemovePacket(DWORD dwPID);
		bool SendPartySetStatePacket(DWORD dwVID, BYTE byState, BYTE byFlag);
		bool SendPartyUseSkillPacket(BYTE bySkillIndex, DWORD dwVID);
		bool SendPartyParameterPacket(BYTE byDistributeMode);

		// SafeBox
		bool SendSafeBoxMoneyPacket(BYTE byState, DWORD dwMoney);
#if defined(ITEM_CHECKINOUT_UPDATE)
		bool SendSafeBoxCheckinPacket(TItemPos InventoryPos, BYTE bySafeBoxPos, bool SelectPosAuto);
#else
		bool SendSafeBoxCheckinPacket(TItemPos InventoryPos, BYTE bySafeBoxPos);
#endif
#if defined(ITEM_CHECKINOUT_UPDATE)
		bool SendSafeBoxCheckoutPacket(BYTE bySafeBoxPos, TItemPos InventoryPos, bool SelectPosAuto);
#else
		bool SendSafeBoxCheckoutPacket(BYTE bySafeBoxPos, TItemPos InventoryPos);
#endif
		bool SendSafeBoxItemMovePacket(BYTE bySourcePos, BYTE byTargetPos, WORD byCount);
		// Mall
#if defined(ITEM_CHECKINOUT_UPDATE)
		bool SendMallCheckoutPacket(BYTE byMallPos, TItemPos InventoryPos, bool SelectPosAuto);
#else
		bool SendMallCheckoutPacket(BYTE byMallPos, TItemPos InventoryPos);
#endif

		// Guild
		bool SendGuildAddMemberPacket(DWORD dwVID);
		bool SendGuildRemoveMemberPacket(DWORD dwPID);
		bool SendGuildChangeGradeNamePacket(BYTE byGradeNumber, const char * c_szName);
		bool SendGuildChangeGradeAuthorityPacket(BYTE byGradeNumber, BYTE byAuthority);
		bool SendGuildOfferPacket(DWORD dwExperience);
		bool SendGuildPostCommentPacket(const char * c_szMessage);
		bool SendGuildDeleteCommentPacket(DWORD dwIndex);
		bool SendGuildRefreshCommentsPacket(DWORD dwHighestIndex);
		bool SendGuildChangeMemberGradePacket(DWORD dwPID, BYTE byGrade);
		bool SendGuildUseSkillPacket(DWORD dwSkillID, DWORD dwTargetVID);
		bool SendGuildChangeMemberGeneralPacket(DWORD dwPID, BYTE byFlag);
		bool SendGuildInvitePacket(DWORD dwVID);
		bool SendGuildInviteAnswerPacket(DWORD dwGuildID, BYTE byAnswer);
		bool SendGuildChargeGSPPacket(DWORD dwMoney);
		bool SendGuildDepositMoneyPacket(DWORD dwMoney);
		bool SendGuildWithdrawMoneyPacket(DWORD dwMoney);
		bool SendGuildChangeSymbol();
#ifdef NEW_PET_SYSTEM
		bool PetSetNamePacket(const char * petname);
#endif
		// Mall
		bool RecvMallOpenPacket();
		bool RecvMallItemSetPacket();
		bool RecvMallItemDelPacket();
		bool RecvBossPositionPacket();
		bool RecvWhisperInfo();
#ifdef __ENABLE_NEW_OFFLINESHOP__
		bool RecvOfflineshopPacket();


		bool RecvOfflineshopShopList();
		bool RecvOfflineshopShopOpen();
		bool RecvOfflineshopShopOpenOwner();
		bool RecvOfflineshopShopOpenOwnerNoShop();
		bool RecvOfflineshopShopClose();
		bool RecvOfflineshopShopFilterResult();
		bool RecvOfflineshopOfferList();
		bool RecvOfflineshopShopSafeboxRefresh();
		bool RecvOfflineshopShopBuyItemFromSearch();

		bool RecvOfflineshopAuctionList();
		bool RecvOfflineshopOpenMyAuction();
		bool RecvOfflineshopOpenMyAuctionNoAuction();
		bool RecvOfflineshopOpenAuction();
#ifdef ENABLE_NEW_SHOP_IN_CITIES
		bool RecvOfflineshopInsertEntity();
		bool RecvOfflineshopRemoveEntity();

		void SendOfflineshopOnClickShopEntity(DWORD dwPickedShopVID);
#endif


		void SendOfflineshopShopCreate(const offlineshop::TShopInfo& shopInfo, const std::vector<offlineshop::TShopItemInfo>& items);
		void SendOfflineshopChangeName(const char* szName);
		void SendOfflineshopForceCloseShop();

#if defined(ENABLE_OFFLINESHOP_REWORK)
		bool RecvOfflineshopNotification();
		void SendOfflineshopExtendTime(DWORD dwTime);
#endif

		void SendOfflineshopRequestShopList();

		void SendOfflineshopOpenShop(DWORD dwOwnerID);
		void SendOfflineshopOpenShopOwner();

		void SendOfflineshopBuyItem(DWORD dwOwnerID, DWORD dwItemID, bool isSearch, long long TotalPriceSeen);
		
		void SendOfflineshopAddItem(offlineshop::TShopItemInfo& itemInfo);
		void SendOfflineshopRemoveItem(DWORD dwItemID);
		void SendOfflineShopEditItem(DWORD dwItemID, const offlineshop::TPriceInfo& price);

		void SendOfflineshopFilterRequest(const offlineshop::TFilterInfo& filter);
		
		void SendOfflineshopOfferCreate(const offlineshop::TOfferInfo& offer);
		void SendOfflineshopOfferAccept(DWORD dwOfferID);
		void SendOfflineshopOfferCancel(DWORD dwOfferID, DWORD dwOwnerID);
		void SendOfflineshopOfferListRequest();

		void SendOfflineshopSafeboxOpen();
		void SendOfflineshopSafeboxGetItem(DWORD dwItemID);
		void SendOfflineshopSafeboxGetValutes(const offlineshop::TValutesInfo& valutes);
		void SendOfflineshopSafeboxClose();

		//AUCTION
		void SendOfflineshopAuctionListRequest();
		void SendOfflineshopAuctionOpen(DWORD dwOwnerID);
		void SendOfflineshopAuctionAddOffer(DWORD dwOwnerID, const offlineshop::TPriceInfo& price);
		void SendOfflineshopAuctionExitFrom(DWORD dwOwnerID);
		void SendOfflineshopAuctionCreate(const TItemPos& pos, const offlineshop::TPriceInfo& price, DWORD dwDuration);
		void SendOfflineshopAuctionOpenMy();
		void SendOfflineshopCloseMyAuction();
		
		void SendOfflineshopCloseBoard();
#endif

		// Lover
		bool RecvLoverInfoPacket();
		bool RecvLovePointUpdatePacket();

		// Dig
		bool RecvDigMotionPacket();

		// Fishing
		bool SendFishingPacket(int iRotation);
#if defined(ENABLE_INGAME_ITEMSHOP)
		bool RecvItemShop();
#endif
#ifdef ENABLE_NEW_FISHING_SYSTEM
		bool SendFishingPacketNew(int r, int i);
		bool RecvFishingNew();
#endif
		bool SendGiveItemPacket(DWORD dwTargetVID, TItemPos ItemPos, int iItemCount);

		// Private Shop
		bool SendBuildPrivateShopPacket(const char * c_szName, const std::vector<TShopItemTable> & c_rSellingItemStock);

		// Refine
#ifdef ENABLE_FEATURES_REFINE_SYSTEM
		bool SendRefinePacket(BYTE byPos, BYTE byType, BYTE bLow, BYTE bMedium, BYTE bExtra, BYTE bTotal);
#else
		bool SendRefinePacket(BYTE byPos, BYTE byType);
#endif
		bool SendSelectItemPacket(DWORD dwItemPos);


		// Client Version
		bool SendClientVersionPacket();

		// CRC Report
		bool __SendCRCReportPacket();

		// 용홍석 강화
		bool SendDragonSoulRefinePacket(BYTE bRefineType, TItemPos* pos);
#ifdef ENABLE_DS_REFINE_ALL
		bool SendDragonSoulRefineAllPacket(uint8_t subheader, uint8_t type, uint8_t grade);
#endif

		// Handshake
		bool RecvHandshakePacket();
		bool RecvHandshakeOKPacket();
#ifdef ENABLE_SWITCHBOT_WORLDARD
		bool SendSwitchBotOpen();
		bool SendSwitchBotClose();
		bool SendSwitchBotChange(PyObject* attr,int vnum_change);
		bool RecvSwitchBot();
#endif
#ifdef _IMPROVED_PACKET_ENCRYPTION_
		bool RecvKeyAgreementPacket();
		bool RecvKeyAgreementCompletedPacket();

#endif

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
		bool CubeRenewalMakeItem(int index_item, int count_item, int index_item_improve);
		bool CubeRenewalClose();
		bool RecvCubeRenewalPacket();
#endif

		// ETC
		DWORD GetMainActorVID();
		DWORD GetMainActorRace();
		DWORD GetMainActorEmpire();
		DWORD GetMainActorSkillGroup();
		void SetEmpireID(DWORD dwEmpireID);
		DWORD GetEmpireID();
		void __TEST_SetSkillGroupFake(int iIndex);
#ifdef ENABLE_ACCE_SYSTEM
		bool	SendAcceClosePacket();
		bool	SendAcceAddPacket(TItemPos tPos, BYTE bPos);
		bool	SendAcceRemovePacket(BYTE bPos);
		bool	SendAcceRefinePacket();
#endif
#if defined(ENABLE_CHANGELOOK)
		bool SendChangeLookClosePacket();
		bool SendChangeLookAddPacket(TItemPos, uint8_t);
		bool SendChangeLookRemovePacket(uint8_t);
		bool SendChangeLookRefinePacket();
#endif
#if defined(USE_ATTR_6TH_7TH)
		bool SendAttr67AddPacket(const TAttr67AddData* pkAttr67AddData, bool bAddPacket);
		bool SendAttr67ClosePacket();
#endif
		bool SendReqMapBossInfo();
	//////////////////////////////////////////////////////////////////////////
	// Phase 관련
	//////////////////////////////////////////////////////////////////////////
	public:
		void SetOffLinePhase();
		void SetHandShakePhase();
		void SetLoginPhase();
		void SetSelectPhase();
		void SetLoadingPhase();
		void SetGamePhase();
		void ClosePhase();

		// Login Phase
		bool SendLoginPacket(const char * c_szName, const char * c_szPassword);
		bool SendLoginPacketNew(const char * c_szName, const char * c_szPassword);
		bool SendDirectEnterPacket(const char * c_szName, const char * c_szPassword, UINT uChrSlot);

		bool SendEnterGame();

		// Select Phase
		bool SendSelectEmpirePacket(DWORD dwEmpireID);
		bool SendSelectCharacterPacket(BYTE account_Index);
		bool SendChangeNamePacket(BYTE index, const char *name);
		bool SendCreateCharacterPacket(BYTE index, const char *name, BYTE job, BYTE shape, BYTE byStat1, BYTE byStat2, BYTE byStat3, BYTE byStat4, const char* c_szCountryFlag);
		bool SendDestroyCharacterPacket(BYTE index, const char * szPrivateCode);
		bool SendChangeCountryFlagPacket(BYTE index, const char* c_szCountryFlag);

		// Main Game Phase
		bool SendWhisperInfo(const char* name);
		bool SendC2CPacket(DWORD dwSize, void * pData);
		bool SendChatPacket(const char * c_szChat, BYTE byType = CHAT_TYPE_TALKING);
		bool SendWhisperPacket(const char * name, const char * c_szChat);
		bool SendMobileMessagePacket(const char * name, const char * c_szChat);
		bool SendMessengerAddByVIDPacket(DWORD vid);
		bool SendMessengerAddByNamePacket(const char * c_szName);
		bool SendMessengerRemovePacket(const char * c_szKey, const char * c_szName);
#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
		bool SendWhisperAdminPacket(const char* c_szText, const char* c_szLang, int color);
#endif
#if defined(ENABLE_MESSENGER_BLOCK)
		bool SendMessengerAddBlockByVIDPacket(uint32_t);
		bool SendMessengerAddBlockByNamePacket(const char*);
		bool SendMessengerRemoveBlockPacket(const char*, const char*);
#endif
#if defined(ENABLE_LETTERS_EVENT)
		bool SendTakeLettersEventReward();
#endif

	protected:
#if defined(ENABLE_TEXTS_RENEWAL)
		typedef struct SRecvChat
		{
			uint8_t type;
			uint32_t idx;
			uint32_t vid;
			char szBuf[1024 + 1];
		} TRecvChat;

		std::queue<TRecvChat> m_ChatQueue;
		bool ProcessChat();
#endif

		bool OnProcess();
		void OffLinePhase();
		void HandShakePhase();
		void LoginPhase();
		void SelectPhase();
		void LoadingPhase();
		void GamePhase();

		bool __IsNotPing();

		void __DownloadMark();
		void __DownloadSymbol(const std::vector<DWORD> & c_rkVec_dwGuildID);

		void __PlayInventoryItemUseSound(TItemPos uSlotPos);
		void __PlayInventoryItemDropSound(TItemPos uSlotPos);
		//void __PlayShopItemDropSound(UINT uSlotPos);
		void __PlaySafeBoxItemDropSound(UINT uSlotPos);
		void __PlayMallItemDropSound(UINT uSlotPos);

		bool __CanActMainInstance();

		enum REFRESH_WINDOW_TYPE
		{
			RefreshStatus = (1 << 0),
			RefreshAlignmentWindow = (1 << 1),
			RefreshCharacterWindow = (1 << 2),
			RefreshEquipmentWindow = (1 << 3),
			RefreshInventoryWindow = (1 << 4),
			RefreshExchangeWindow = (1 << 5),
			RefreshSkillWindow = (1 << 6),
			RefreshSafeboxWindow  = (1 << 7),
			RefreshMessengerWindow = (1 << 8),
			RefreshGuildWindowInfoPage = (1 << 9),
			RefreshGuildWindowBoardPage = (1 << 10),
			RefreshGuildWindowMemberPage = (1 << 11),
			RefreshGuildWindowMemberPageGradeComboBox = (1 << 12),
			RefreshGuildWindowSkillPage = (1 << 13),
			RefreshGuildWindowGradePage = (1 << 14),
			RefreshTargetBoard = (1 << 15),
			RefreshMallWindow = (1 << 16),
		};

		void __RefreshStatus();
		void __RefreshAlignmentWindow();
		void __RefreshCharacterWindow();
		void __RefreshEquipmentWindow();
		void __RefreshInventoryWindow();
		void __RefreshExchangeWindow();
		void __RefreshSkillWindow();
		void __RefreshSafeboxWindow();
		void __RefreshMessengerWindow();
		void __RefreshGuildWindowInfoPage();
		void __RefreshGuildWindowBoardPage();
		void __RefreshGuildWindowMemberPage();
		void __RefreshGuildWindowMemberPageGradeComboBox();
		void __RefreshGuildWindowSkillPage();
		void __RefreshGuildWindowGradePage();
		void __RefreshTargetBoardByVID(DWORD dwVID);
		void __RefreshTargetBoardByName(const char * c_szName);
		void __RefreshTargetBoard();
		void __RefreshMallWindow();
		bool __SendHack(const char* c_szMsg);

	protected:
		bool RecvObserverAddPacket();
		bool RecvObserverRemovePacket();
		bool RecvObserverMovePacket();

		// Common
		bool RecvErrorPacket(int header);
		bool RecvPingPacket();
		bool RecvDefaultPacket(int header);
		bool RecvPhasePacket();

		// Login Phase
		bool __RecvLoginSuccessPacket3();
		bool __RecvLoginSuccessPacket4();
		bool __RecvLoginFailurePacket();
		bool __RecvEmpirePacket();
		bool __RecvLoginKeyPacket();

		// Select Phase
		bool __RecvPlayerCreateSuccessPacket();
		bool __RecvPlayerCreateFailurePacket();
		bool __RecvPlayerDestroySuccessPacket();
		bool __RecvPlayerDestroyFailurePacket();
		bool __RecvPreserveItemPacket();
		bool __RecvPlayerPoints();
		bool __RecvChangeName();
		bool __RecvChangeCountryFlag();

		// Loading Phase
		bool RecvMainCharacter();
		bool RecvMainCharacter2_EMPIRE();
		bool RecvMainCharacter3_BGM();
		bool RecvMainCharacter4_BGM_VOL();

		void __SetFieldMusicFileName(const char* musicName);
		void __SetFieldMusicFileInfo(const char* musicName, float vol);
		// END_OF_SUPPORT_BGM

		// Main Game Phase
		bool RecvWarpPacket();
		bool RecvPVPPacket();
		bool RecvDuelStartPacket();
        bool RecvGlobalTimePacket();
		bool RecvCharacterAppendPacket();
		bool RecvCharacterAdditionalInfo();
		bool RecvCharacterAppendPacketNew();
		bool RecvCharacterUpdatePacket();
		bool RecvCharacterUpdatePacketNew();
		bool RecvCharacterDeletePacket();
		bool RecvChatPacket();
		bool RecvOwnerShipPacket();
		bool RecvSyncPositionPacket();
		bool RecvWhisperPacket();
		bool RecvPointChange();					// Alarm to python
		bool RecvChangeSpeedPacket();

		bool RecvStunPacket();
		bool RecvDeadPacket();
		bool RecvCharacterMovePacket();
#if defined(ENABLE_NEW_GOLD_LIMIT)
		bool __RecvPlayerGold();
		bool RecvGoldChange();
#endif

		bool RecvItemSetPacket();					// Alarm to python
		bool RecvItemSetPacket2();					// Alarm to python
		bool RecvItemUsePacket();					// Alarm to python
		bool RecvItemUpdatePacket();				// Alarm to python
		bool RecvItemGroundAddPacket();
		bool RecvItemGroundDelPacket();
		bool RecvItemOwnership();

		bool RecvQuickSlotAddPacket();
		bool RecvQuickSlotDelPacket();
		bool RecvQuickSlotMovePacket();

		bool RecvCharacterPositionPacket();
		bool RecvMotionPacket();

		bool RecvShopPacket();
		bool RecvShopSignPacket();
		bool RecvExchangePacket();

		// Quest
		bool RecvScriptPacket();
		bool RecvQuestInfoPacket();
		bool RecvQuestConfirmPacket();
		bool RecvRequestMakeGuild();

		// Skill
		bool RecvSkillLevel();
		bool RecvSkillLevelNew();
		bool RecvSkillCoolTimeEnd();

		// Target
		bool RecvTargetPacket();
		bool RecvViewEquipPacket();
		bool RecvDamageInfoPacket();

		// Mount
		bool RecvMountPacket();

		// Fly
		bool RecvCreateFlyPacket();
		bool RecvFlyTargetingPacket();
		bool RecvAddFlyTargetingPacket();

		// Messenger
		bool RecvMessenger();

		// Guild
		bool RecvGuild();
#ifdef ENABLE_MULTI_LANGUAGE
		bool RecvRequestChangeLanguage();
#endif

		// Party
		bool RecvPartyInvite();
		bool RecvPartyAdd();
		bool RecvPartyUpdate();
		bool RecvPartyRemove();
		bool RecvPartyLink();
		bool RecvPartyUnlink();
		bool RecvPartyParameter();

		// SafeBox
		bool RecvSafeBoxSetPacket();
		bool RecvSafeBoxDelPacket();
		bool RecvSafeBoxWrongPasswordPacket();
		bool RecvSafeBoxSizePacket();
		bool RecvSafeBoxMoneyChangePacket();
#if defined(ENABLE_GAYA_RENEWAL)
		bool RecvGemShopOpen();
		bool RecvGemShopRefresh();
#endif

		// Fishing
		bool RecvFishing();

		// Dungeon
		bool RecvDungeon();

		// Time
		bool RecvTimePacket();

		// WalkMode
		bool RecvWalkModePacket();

		// ChangeSkillGroup
		bool RecvChangeSkillGroupPacket();

		// Refine
		bool RecvRefineInformationPacket();

		// Use Potion
		bool RecvSpecialEffect();

		// 서버에서 지정한 이팩트 발동 패킷.
		bool RecvSpecificEffect();

		// 용혼석 관련
		bool RecvDragonSoulRefine();
#ifdef INGAME_WIKI
		bool RecvWikiPacket();
#endif
		// MiniMap Info
		bool RecvNPCList();
#ifdef ENABLE_ATLAS_BOSS
		bool RecvBossList();
#endif
		bool RecvLandPacket();
		bool RecvTargetCreatePacket();
		bool RecvTargetCreatePacketNew();
		bool RecvTargetUpdatePacket();
		bool RecvTargetDeletePacket();

		// Affect
		bool RecvAffectAddPacket();
		bool RecvAffectRemovePacket();

		// Channel
		bool RecvChannelPacket();

		// Acce
#ifdef ENABLE_ACCE_SYSTEM
		bool	RecvAccePacket(bool bReturn = false);
#endif
#if defined(ENABLE_CHANGELOOK)
		bool RecvChangeLookPacket();
#endif
#ifdef ENABLE_RANKING
		bool	RecvRankingTable();
#endif
		// @fixme007
		bool RecvUnk213();

	protected:
		// 이모티콘
		bool ParseEmoticon(const char*, uint32_t*);

		// 파이썬으로 보내는 콜들
		void OnConnectFailure();
		void OnScriptEventStart(int iSkin, int iIndex);

		void OnRemoteDisconnect();
		void OnDisconnect();

		void SetGameOnline();
		void SetGameOffline();
		BOOL IsGameOnline();

#if defined(USE_BATTLEPASS)
	public:
		bool SendExtBattlePassAction(BYTE bAction);
		bool SendExtBattlePassItem(int slotindex);

	protected:
		bool RecvExtBattlePassOpenPacket();
		bool RecvExtBattlePassGeneralInfoPacket();
		bool RecvExtBattlePassMissionInfoPacket();
		bool RecvExtBattlePassMissionUpdatePacket();
		bool RecvExtBattlePassRankingPacket();
#endif
#ifdef ENABLE_PREMIUM_PLAYERS
	public:
		bool SendPremiumPlayersOpenRequest();
		bool SendPremiumPlayersListRequest();
		bool SendPremiumPlayersActivateRequest();

		bool RecvPremiumPlayersPacket();
#endif
	protected:
		bool CheckPacket(TPacketHeader * pRetHeader);

		void __InitializeGamePhase();
		void __InitializeMarkAuth();
		void __GlobalPositionToLocalPosition(LONG& rGlobalX, LONG& rGlobalY);
		void __LocalPositionToGlobalPosition(LONG& rLocalX, LONG& rLocalY);

		bool __IsPlayerAttacking();
		bool __IsEquipItemInSlot(TItemPos Cell);
		void __LeaveOfflinePhase() {}
		void __LeaveHandshakePhase() {}
		void __LeaveLoginPhase() {}
		void __LeaveSelectPhase() {}
		void __LeaveLoadingPhase() {}
		void __LeaveGamePhase();

		void __ClearNetworkActorManager();

		void __ClearSelectCharacterData();

		// DELETEME
		//void __SendWarpPacket();

		void __ConvertEmpireText(DWORD dwEmpireID, char* szText);

		void __RecvCharacterAppendPacket(SNetworkActorData * pkNetActorData);
		void __RecvCharacterUpdatePacket(SNetworkUpdateActorData * pkNetUpdateActorData);

		void __FilterInsult(char* szLine, UINT uLineLen);

		void __SetGuildID(DWORD id);

	protected:
		TPacketGCHandshake m_HandshakeData;
		DWORD m_dwChangingPhaseTime;
		DWORD m_dwBindupRetryCount;
		DWORD m_dwMainActorVID;
		DWORD m_dwMainActorRace;
		DWORD m_dwMainActorEmpire;
		DWORD m_dwMainActorSkillGroup;
		BOOL m_isGameOnline;
		BOOL m_isStartGame;

		DWORD m_dwGuildID;
		DWORD m_dwEmpireID;

		struct SServerTimeSync
		{
			DWORD m_dwChangeServerTime;
			DWORD m_dwChangeClientTime;
		} m_kServerTimeSync;

		void __ServerTimeSync_Initialize();
		//DWORD m_dwBaseServerTime;
		//DWORD m_dwBaseClientTime;

		DWORD m_dwLastGamePingTime;

		std::string	m_stID;
		std::string	m_stPassword;
		std::string	m_strLastCommand;
		std::string	m_strPhase;
		DWORD m_dwLoginKey;
		BOOL m_isWaitLoginKey;

		std::string m_stMarkIP;
		std::string m_strMarkImageFileName;

		CFuncObject<CPythonNetworkStream>	m_phaseProcessFunc;
		CFuncObject<CPythonNetworkStream>	m_phaseLeaveFunc;

		PyObject*							m_poHandler;
		PyObject*							m_apoPhaseWnd[PHASE_WINDOW_NUM];
		PyObject*							m_poSerCommandParserWnd;
#ifdef ENABLE_NEW_FISHING_SYSTEM
		bool m_phaseWindowGame;
#endif
		TSimplePlayerInformation			m_akSimplePlayerInfo[PLAYER_PER_ACCOUNT4];
		DWORD								m_adwGuildID[PLAYER_PER_ACCOUNT4];
		std::string							m_astrGuildName[PLAYER_PER_ACCOUNT4];
		bool m_bSimplePlayerInfo;

		CRef<CNetworkActorManager>			m_rokNetActorMgr;

		bool m_isRefreshStatus;
		bool m_isRefreshCharacterWnd;
		bool m_isRefreshEquipmentWnd;
		bool m_isRefreshInventoryWnd;
		bool m_isRefreshExchangeWnd;
		bool m_isRefreshSkillWnd;
		bool m_isRefreshSafeboxWnd;
		bool m_isRefreshMallWnd;
		bool m_isRefreshMessengerWnd;
		bool m_isRefreshGuildWndInfoPage;
		bool m_isRefreshGuildWndBoardPage;
		bool m_isRefreshGuildWndMemberPage;
		bool m_isRefreshGuildWndMemberPageGradeComboBox;
		bool m_isRefreshGuildWndSkillPage;
		bool m_isRefreshGuildWndGradePage;

		// Emoticon
		std::vector<std::string> m_EmoticonStringVector;

		struct STextConvertTable
		{
			char acUpper[26];
			char acLower[26];
			BYTE aacHan[5000][2];
		} m_aTextConvTable[3];



		struct SMarkAuth
		{
			CNetworkAddress m_kNetAddr;
			DWORD m_dwHandle;
			DWORD m_dwRandomKey;
		} m_kMarkAuth;



		DWORD m_dwSelectedCharacterIndex;

		CInsultChecker m_kInsultChecker;

		bool m_isEnableChatInsultFilter;
		bool m_bComboSkillFlag;

		std::deque<std::string> m_kQue_stHack;

	private:
		struct SDirectEnterMode
		{
			bool m_isSet;
			DWORD m_dwChrSlotIndex;
		} m_kDirectEnterMode;

		void __DirectEnterMode_Initialize();
		void __DirectEnterMode_Set(UINT uChrSlotIndex);
		bool __DirectEnterMode_IsSet();

	public:
		bool DirectEnterMode_IsSet() { return __DirectEnterMode_IsSet(); }

	public:
		DWORD EXPORT_GetBettingGuildWarValue(const char* c_szValueName);

	private:
		struct SBettingGuildWar
		{
			DWORD m_dwBettingMoney;
			DWORD m_dwObserverCount;
		} m_kBettingGuildWar;

		CInstanceBase * m_pInstTarget;

		void __BettingGuildWar_Initialize();
		void __BettingGuildWar_SetObserverCount(UINT uObserverCount);
		void __BettingGuildWar_SetBettingMoney(UINT uBettingMoney);

#if defined(ENABLE_TEXTS_RENEWAL)
	protected:
		bool RecvChatPacketNew();
#endif
#ifdef ENABLE_MULTI_NAMES
	public:
		bool IsTransName(DWORD race);

	private:
		std::vector<DWORD> m_autotrans;
#endif

#if defined(ENABLE_INGAME_CHCHANGE)
	public:
		void SetChannelNumber(BYTE ch) { m_Channel = ch; };
		BYTE GetChannelNumber() const { return m_MapIndex; };
		void SetMapIndex(DWORD idx) { m_MapIndex = idx; };
		DWORD GetMapIndex() const { return m_Channel; };
		bool SendChangeChannel(BYTE);

	protected:
		BYTE m_Channel;
		DWORD m_MapIndex;
#endif

#if defined(ENABLE_AURA_SYSTEM)
	protected:
		bool RecvAuraPacket();

	public:
		bool SendAuraRefineCheckIn(TItemPos, TItemPos, uint8_t);
		bool SendAuraRefineCheckOut(TItemPos, uint8_t);
		bool SendAuraRefineAccept(uint8_t);
		bool SendAuraRefineCancel();
#endif
#ifdef USE_MULTIPLE_OPENING
    public:
        bool SendMultipleOpening(const TItemPos pos, const uint16_t count);
#endif
};
