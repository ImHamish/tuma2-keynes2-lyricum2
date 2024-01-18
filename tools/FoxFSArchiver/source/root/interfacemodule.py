import app
import player
import net
import uimarketsystem
import constinfo
import systemSetting
import wndMgr
import chat
import grp
import uitaskbar
import uicharacter
import uiinventory
import uidragonsoul
import uichat
import uimessenger
import guild

if app.ENABLE_EVENT_MANAGER:
	import uieventcalendar

if app.ENABLE_REWARD_SYSTEM:
	import uireward

import ui
import uiwhisper
import uishop
import uiexchange
import uisystem
import uirestart
import uitooltip
import uiminimap
import uiparty
import uisafebox
import uiguild
import uiquest
import uiprivateshopbuilder
import uicommon
import uirefine
import uiequipmentdialog
import uigamebutton
import uitip
import uicube
import miniMap
# ACCESSORY_REFINE_ADD_METIN_STONE
import uiselectitem
# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE
import uiscriptlocale
import event
import whispermanager
import localeinfo

if app.ENABLE_PREMIUM_PLAYERS:
	import uipremiumplayers

if app.ENABLE_SWITCHBOT_WORLDARD:
	import uiswtichbot

if app.__ENABLE_NEW_OFFLINESHOP__:
	import offlineshop
	if app.ENABLE_OFFLINESHOP_REWORK:
		import uiofflineshop_ae
	else:
		import uiofflineshop

if app.ENABLE_MELEY_LAIR_DUNGEON:
	import uidragonlairranking

if app.ENABLE_ACCE_SYSTEM:
	import uiacce

if app.ENABLE_EXTRA_INVENTORY:
	import uiextrainventory

if app.ENABLE_ATTR_TRANSFER_SYSTEM:
	import uiattrtransfer

if app.ENABLE_DUNGEON_INFO_SYSTEM:
	import uidungeoninfo

if app.ENABLE_RANKING:
	import uiranking

if app.ENABLE_BIOLOGIST_UI:
	import uibiolog

if app.ENABLE_NEW_FISHING_SYSTEM:
	import uifishing

if app.ENABLE_CHOOSE_DOCTRINE_GUI:
	import uidoctrinechoose

if app.LINK_IN_CHAT:
	import os

if app.USE_BATTLEPASS:
	import uibattlepassextended

if app.ENABLE_INGAME_CHCHANGE:
	import uichannel

if app.ENABLE_INGAME_ITEMSHOP:
	import uiitemshop

if app.ENABLE_GAYA_RENEWAL:
	import uigemshop
	import uiselectitemex

if app.USE_CAPTCHA_SYSTEM:
	import uicaptcha

if app.ENABLE_MINI_GAME:
	import uiminigame

if app.ENABLE_AURA_SYSTEM:
	import uiaura

if app.ENABLE_CHANGELOOK:
	import uichangelook

if app.ENABLE_TRACK_WINDOW:
	import uitrack

if app.USE_WHEEL_OF_FORTUNE:
	import uiwheeloffortune

if app.USE_ATTR_6TH_7TH:
	import uiattr67add

if app.USE_AUTO_HUNT:
	import uiauto

if app.USE_PICKUP_FILTER:
	import uifilter

IsQBHide = 0
class Interface(object):
	CHARACTER_STATUS_TAB = 1
	CHARACTER_SKILL_TAB = 2

	def __init__(self):
		systemSetting.SetInterfaceHandler(self)
		self.windowOpenPosition = 0
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.onTopWindow = player.ON_TOP_WND_NONE
		
		self.dlgWhisperWithoutTarget = None
		self.inputDialog = None
		self.tipBoard = None
		self.bigBoard = None
		if app.USE_WHEEL_OF_FORTUNE:
			self.wndWheelofFortune=0
		
		if app.ENABLE_INGAME_ITEMSHOP:
			self.wndItemShop = None
		
		if app.ENABLE_EVENT_MANAGER:
			self.wndEventManager = None
			self.wndEventIcon=0
		
		if app.ENABLE_REWARD_SYSTEM:
			self.wndReward = None
		
		if app.ENABLE_INGAME_CHCHANGE:
			self.wndChangeCh = None

		# ITEM_MALL
		self.mallPageDlg = None
		# END_OF_ITEM_MALL
		
		if app.ENABLE_CUBE_RENEWAL_WORLDARD:
			self.wndCubeRenewal = None
		self.wndTaskBar = None
		self.wndCharacter = None
		self.wndInventory = None
		self.wndExpandedTaskBar = None
		if app.ENABLE_CAPITALE_SYSTEM:
			self.wndExpandedMoneyTaskbar = None
		self.wndDragonSoul = None
		self.wndDragonSoulRefine = None
		self.wndChat = None
		self.wndMessenger = None
		self.wndMiniMap = None
		self.wndGuild = None
		self.wndGuildBuilding = None
		if app.ENABLE_OFFLINESHOP_REWORK:
			self.wndShopOffline = None
		
		if app.USE_CAPTCHA_SYSTEM:
			self.wndCaptcha = None
		
		if app.ENABLE_GAYA_RENEWAL:
			self.dlgGemShop = None

		if app.LINK_IN_CHAT:
			self.openLinkQuestionDlg = None
		
		if app.ENABLE_OFFLINESHOP_REWORK:
			self.wndOfflineshop = None
		
		self.wndCalendarWindow = None
		if app.ENABLE_NEW_FISHING_SYSTEM:
			self.wndFishingWindow = None
		if app.ENABLE_EXTRA_INVENTORY:
			self.wndExtraInventory = None
		
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			self.wndDungeonInfo = None
		
		if app.ENABLE_RANKING:
			self.wndRanking = None
		
		if app.ENABLE_WHISPER_ADMIN_SYSTEM:
			self.wndWhisperManager = None
		
		if app.ENABLE_CHANGELOOK:
			self.wndChangeLook = None
		
		if app.USE_BATTLEPASS:
			self.wndBattlePassExtended = None
			self.isFirstOpeningExtBattlePass = False

		if app.USE_PICKUP_FILTER:
			self.wndFilterPopup = None

			if not app.USE_PICKUP_FILTER_ICO_AS_AFFECT:
				self.wndFilterIco = None

			self.wndFilter = None

		if app.USE_AUTO_HUNT:
			self.wndAutoWindow = None

		event.SetInterfaceWindow(self)
		
		if app.USE_CAPTCHA_SYSTEM:
			self.uiAffectShower = None
			self.uitargetBoard = None
			self.IsHideUIMode = False
		
		self.listGMName = {}
		
		if app.ENABLE_MULTI_LANGUAGE:
			self.listLangName = {}

		self.wndQuestWindow = {}
		self.wndQuestWindowNewKey = 0
		self.privateShopAdvertisementBoardDict = {}
		self.guildScoreBoardDict = {}
		self.equipmentDialogDict={}

		self.wndChatLog = None
		if app.ENABLE_BIOLOGIST_UI:
			self.wndBiologist = None
		
		if app.ENABLE_CHOOSE_DOCTRINE_GUI:
			self.wndDoctrine = None
		
		if app.ENABLE_INGAME_CHCHANGE:
			if self.wndChangeCh:
				del self.wndChangeCh
				self.wndChangeCh = None
			
			self.wndChangeCh = uichannel.ChangeChannel()
		
		if app.ENABLE_MINI_GAME:
			self.wndMiniGame = None
			self.miniGameDict = {}
		
		event.SetInterfaceWindow(self)

	def __del__(self):
		systemSetting.DestroyInterfaceHandler()
		event.SetInterfaceWindow(None)

	def __MakeUICurtain(self):
		wndUICurtain = ui.Bar("TOP_MOST")
		wndUICurtain.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		wndUICurtain.SetColor(0x77000000)
		wndUICurtain.Hide()
		self.wndUICurtain = wndUICurtain

	def __MakeMessengerWindow(self):
		self.wndMessenger = uimessenger.MessengerWindow()

		from _weakref import proxy
		self.wndMessenger.SetWhisperButtonEvent(lambda n,i=proxy(self):i.OpenWhisperDialog(n))
		self.wndMessenger.SetGuildButtonEvent(ui.__mem_func__(self.ToggleGuildWindow))

	if app.ENABLE_WHISPER_MESSENGER_BUTTONS:
		def RefreshWhisperMessengerButtons(self):
			if self.whisperDialogDict:
				for dlg in self.whisperDialogDict.itervalues():
					dlg.RefreshMessengerButtons()

	def __MakeGuildWindow(self):
		self.wndGuild = uiguild.GuildWindow()

	def __MakeChatWindow(self):
		wndChat = uichat.ChatWindow(self)
		wndChat.SetSize(wndChat.CHAT_WINDOW_WIDTH, 0)
		wndChat.SetPosition(wndMgr.GetScreenWidth()/2 - wndChat.CHAT_WINDOW_WIDTH/2, wndMgr.GetScreenHeight() - wndChat.EDIT_LINE_HEIGHT - 37)
		wndChat.SetHeight(200)
		wndChat.Refresh()
		wndChat.Show()

		self.wndChat = wndChat
		self.wndChat.SetSendWhisperEvent(ui.__mem_func__(self.OpenWhisperDialogWithoutTarget))
		self.wndChat.SetOpenChatLogEvent(ui.__mem_func__(self.ToggleChatLogWindow))

	if app.ENABLE_SWITCHBOT_WORLDARD:
		def OnPressSwitchbotButton(self):
			if app.ENABLE_RENEWAL_OX:
				import background
				if background.GetCurrentMapName() == "metin2_map_oxevent":
					import chr
					if not chr.IsGameMaster(player.GetMainCharacterIndex()):
						import chat
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CANNOT_USE_SWITCHBOT)
						return
			
			net.SBOpen()

	def __MakeTaskBar(self):
		wndTaskBar = uitaskbar.TaskBar()
		wndTaskBar.LoadWindow()
		self.wndTaskBar = wndTaskBar
		
		self.wndTaskBar.SetToggleButtonEvent(uitaskbar.TaskBar.BUTTON_CHARACTER, ui.__mem_func__(self.ToggleCharacterWindowStatusPage))
		# self.wndTaskBar.SetToggleButtonEvent(uitaskbar.TaskBar.EVENT_BUTTON, ui.__mem_func__(self.ToggleEventButton))
		self.wndTaskBar.SetToggleButtonEvent(uitaskbar.TaskBar.BUTTON_INVENTORY, ui.__mem_func__(self.ToggleInventoryWindow))
		self.wndTaskBar.SetToggleButtonEvent(uitaskbar.TaskBar.BUTTON_MESSENGER, ui.__mem_func__(self.ToggleMessenger))
		self.wndTaskBar.SetToggleButtonEvent(uitaskbar.TaskBar.BUTTON_SYSTEM, ui.__mem_func__(self.ToggleSystemDialog))
		if app.ENABLE_CAPITALE_SYSTEM:
			self.wndTaskBar.SetToggleButtonEvent(uitaskbar.TaskBar.BUTTON_EXPAND_MONEY, ui.__mem_func__(self.ToggleExpandedMoneyButton))
		
		if uitaskbar.TaskBar.IS_EXPANDED:
			self.wndTaskBar.SetToggleButtonEvent(uitaskbar.TaskBar.BUTTON_EXPAND, ui.__mem_func__(self.ToggleExpandedButton))
			self.wndExpandedTaskBar = uitaskbar.ExpandedTaskBar()
			self.wndExpandedTaskBar.LoadWindow()
			
			if app.ENABLE_REWARD_SYSTEM:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uitaskbar.ExpandedTaskBar.BUTTON_REWARD, ui.__mem_func__(self.OpenRewardWindow))

			if app.USE_BATTLEPASS:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uitaskbar.ExpandedTaskBar.BUTTON_BATTLEPASS, ui.__mem_func__(self.ToggleBattlePassExtended))


			#if app.ENABLE_DUNGEON_INFO_SYSTEM:
			#	self.wndExpandedTaskBar.SetToggleButtonEvent(uitaskbar.ExpandedTaskBar.BUTTON_DUNGEONINFO, ui.__mem_func__(self.ToggleDungeonInfoWindow))
			#
			#if app.ENABLE_BIOLOGIST_UI:
			#	self.wndExpandedTaskBar.SetToggleButtonEvent(uitaskbar.ExpandedTaskBar.BUTTON_BIOLOGIST, ui.__mem_func__(self.ClickBiologistButton))
			#
			#if app.USE_BATTLEPASS:
			#	self.wndExpandedTaskBar.SetToggleButtonEvent(uitaskbar.ExpandedTaskBar.BUTTON_BATTLEPASS, ui.__mem_func__(self.ToggleBattlePassExtended))
			#
			#if app.ENABLE_SWITCHBOT_WORLDARD:
			#	self.wndExpandedTaskBar.SetToggleButtonEvent(uitaskbar.ExpandedTaskBar.BUTTON_SWITCHBOT, ui.__mem_func__(self.OnPressSwitchbotButton))
			#
			#if app.ENABLE_TRACK_WINDOW:
			#	self.wndExpandedTaskBar.SetToggleButtonEvent(uitaskbar.ExpandedTaskBar.BUTTON_WORLDBOSS, ui.__mem_func__(self.OpenTrackWindow))

			if app.USE_AUTO_HUNT:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uitaskbar.ExpandedTaskBar.BUTTON_HUNTING, ui.__mem_func__(self.ToggleAutoWindow))

			if constinfo.wndExpandedMenu == 1:
				self.ToggleExpandedButton()
		else:
			self.wndTaskBar.SetToggleButtonEvent(uitaskbar.TaskBar.BUTTON_CHAT, ui.__mem_func__(self.ToggleChat))

		self.wndEnergyBar = None
		if app.ENABLE_ENERGY_SYSTEM:
			wndEnergyBar = uitaskbar.EnergyBar()
			wndEnergyBar.LoadWindow()
			self.wndEnergyBar = wndEnergyBar

	def __MakeParty(self):
		wndParty = uiparty.PartyWindow()
		wndParty.Hide()
		self.wndParty = wndParty

	if app.ENABLE_CHOOSE_DOCTRINE_GUI:
		def OnRecvDoctrine(self):
			if self.wndDoctrine and not self.wndDoctrine.IsShow():
				self.wndDoctrine.Open()

		def __OnClickDoctrineButton(self):
			if self.wndDoctrine:
				if self.wndDoctrine.IsShow():
					self.wndDoctrine.Close()
				else:
					self.wndDoctrine.Open()

	def __MakeGameButtonWindow(self):
		wndGameButton = uigamebutton.GameButtonWindow()
		wndGameButton.SetTop()
		wndGameButton.Show()
		wndGameButton.SetButtonEvent("STATUS", ui.__mem_func__(self.__OnClickStatusPlusButton))
		wndGameButton.SetButtonEvent("SKILL", ui.__mem_func__(self.__OnClickSkillPlusButton))
		wndGameButton.SetButtonEvent("QUEST", ui.__mem_func__(self.__OnClickQuestButton))
		wndGameButton.SetButtonEvent("BUILD", ui.__mem_func__(self.__OnClickBuildButton))
		if app.ENABLE_CHOOSE_DOCTRINE_GUI:
			wndGameButton.SetButtonEvent("DOCTRINE", ui.__mem_func__(self.__OnClickDoctrineButton))

		self.wndGameButton = wndGameButton

	def __IsChatOpen(self):
		return True

	def __MakeWindows(self):
		wndCharacter = uicharacter.CharacterWindow()
		wndInventory = uiinventory.InventoryWindow()
		self.wndInventory = wndInventory
		
		
		if app.ENABLE_CAPITALE_SYSTEM:
			self.wndExpandedMoneyTaskbar = uiinventory.ExpandedMoneyTaskBar()
			self.wndExpandedMoneyTaskbar.LoadWindow()
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			wndDragonSoul = uidragonsoul.DragonSoulWindow()
			wndDragonSoulRefine = uidragonsoul.DragonSoulRefineWindow()
		else:
			wndDragonSoul = None
			wndDragonSoulRefine = None
		
		if not app.ENABLE_OFFLINESHOP_REWORK:
			self.wndOfflineshop = uiofflineshop.NewOfflineShopBoard()
		
		self.wndCalendarWindow = uieventcalendar.EventCalendarWindow()
		wndMiniMap = uiminimap.MiniMap()
		
		wndSafebox = uisafebox.SafeboxWindow()
		
		# ITEM_MALL
		wndMall = uisafebox.MallWindow()
		self.wndMall = wndMall
		# END_OF_ITEM_MALL

		wndChatLog = uichat.ChatLogWindow(self)
		self.wndCharacter = wndCharacter
		self.wndDragonSoul = wndDragonSoul
		self.wndDragonSoulRefine = wndDragonSoulRefine
		self.wndMiniMap = wndMiniMap
		self.wndSafebox = wndSafebox
		self.wndChatLog = wndChatLog
		
		if app.USE_ATTR_6TH_7TH:
			wndAttr67Add = uiattr67add.Attr67AddWindow()
			self.wndAttr67Add = wndAttr67Add
		
		if app.ENABLE_OFFLINESHOP_REWORK:
			self.wndShopOffline = uiofflineshop_ae.OfflineShopWindow()
			self.wndShopOffline.Hide()
		
		if app.USE_CAPTCHA_SYSTEM:
			self.wndCaptcha = uicaptcha.CaptchaDialog()
		
		if app.ENABLE_GAYA_RENEWAL:
			self.dlgGemShop = uigemshop.GemShopWindow()
		
		if app.USE_BATTLEPASS:
			self.wndBattlePassExtended = uibattlepassextended.BattlePassWindow()
		
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.SetDragonSoulRefineWindow(self.wndDragonSoulRefine)
			self.wndDragonSoulRefine.SetInventoryWindows(self.wndInventory, self.wndDragonSoul)
			self.wndInventory.SetDragonSoulRefineWindow(self.wndDragonSoulRefine)
		if app.ITEM_CHECKINOUT_UPDATE:
			self.wndInventory.SetSafeboxWindow(self.wndSafebox)
		
		if app.ENABLE_NEW_FISHING_SYSTEM:
			self.wndFishingWindow = uifishing.FishingWindow()
		else:
			self.wndFishingWindow = None
		
		if app.ENABLE_RANKING:
			self.wndRanking = uiranking.RankingWindow()
		
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			self.wndDungeonInfo = uidungeoninfo.DungeonInfo()
		
		if app.ENABLE_WHISPER_ADMIN_SYSTEM:
			self.wndWhisperManager = whispermanager.wnd()
		if app.ENABLE_BIOLOGIST_UI:
			self.wndBiologist = uibiolog.BiologWindow()
			self.wndBiologistChange = uibiolog.BiologWindow_Change()
		
		if app.ENABLE_CHOOSE_DOCTRINE_GUI:
			self.wndDoctrine = uidoctrinechoose.DoctrineWindow()
		
		if app.ENABLE_AURA_SYSTEM:
			self.wndAura = uiaura.AuraWindow()
		
		if app.ENABLE_TRACK_WINDOW:
			self.wndTrackWindow = uitrack.TrackWindow()

	def __MakeDialogs(self):
		self.dlgExchange = uiexchange.ExchangeDialog()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.dlgExchange.BindInterface(self)
			self.dlgExchange.SetInven(self.wndInventory)
			self.wndInventory.BindWindow(self.dlgExchange)
		
		self.dlgExchange.LoadDialog()
		self.dlgExchange.SetCenterPosition()
		self.dlgExchange.Hide()

		self.dlgShop = uishop.ShopDialog()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.dlgShop.BindInterface(self)
		
		self.dlgShop.LoadDialog()
		self.dlgShop.Hide()

		self.dlgRestart = uirestart.RestartDialog()
		self.dlgRestart.LoadDialog()
		self.dlgRestart.Hide()

		self.dlgSystem = uisystem.SystemDialog()
		self.dlgSystem.LoadDialog()

		self.dlgSystem.Hide()

		self.dlgPassword = uisafebox.PasswordDialog()
		self.dlgPassword.Hide()

		self.hyperlinkItemTooltip = uitooltip.HyperlinkItemToolTip()
		self.hyperlinkItemTooltip.Hide()
		
		self.tooltipItem = uitooltip.ItemToolTip()
		self.tooltipItem.Hide()
		
		self.tooltipSkill = uitooltip.SkillToolTip()
		self.tooltipSkill.Hide()
		
		self.privateShopBuilder = uiprivateshopbuilder.PrivateShopBuilder()
		self.privateShopBuilder.Hide()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.privateShopBuilder.BindInterface(self)
			self.privateShopBuilder.SetInven(self.wndInventory)
			self.wndInventory.BindWindow(self.privateShopBuilder)
		
		self.dlgRefineNew = uirefine.RefineDialogNew()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.dlgRefineNew.SetInven(self.wndInventory)
			self.wndInventory.BindWindow(self.dlgRefineNew)
		
		self.dlgRefineNew.Hide()

	def __MakeTipBoard(self):
		self.tipBoard = uitip.TipBoard()
		self.tipBoard.Hide()

		self.bigBoard = uitip.BigBoard()
		self.bigBoard.Hide()

	if app.ENABLE_ATTR_TRANSFER_SYSTEM:
		def __MakeAttrTransferWindow(self):
			self.wndAttrTransfer = uiattrtransfer.TransferWindow()
			self.wndAttrTransfer.BindInventory(self.wndInventory)
			self.wndAttrTransfer.LoadWindow()
			self.wndAttrTransfer.Hide()

	if app.ENABLE_MELEY_LAIR_DUNGEON:
		def __MakeMeleyRanking(self):
			self.wndMeleyRanking = uidragonlairranking.Window()
			self.wndMeleyRanking.LoadWindow()
			self.wndMeleyRanking.Hide()

	def __MakeCubeWindow(self):
		self.wndCube = uicube.CubeWindow()
		self.wndCube.LoadWindow()
		self.wndCube.Hide()

	if app.ENABLE_ACCE_SYSTEM:
		def __MakeAcceWindow(self):
			self.wndAcceCombine = uiacce.CombineWindow()
			self.wndAcceCombine.LoadWindow()
			self.wndAcceCombine.Hide()
			
			self.wndAcceAbsorption = uiacce.AbsorbWindow()
			self.wndAcceAbsorption.LoadWindow()
			self.wndAcceAbsorption.Hide()
			
			if self.wndInventory:
				self.wndInventory.SetAcceWindow(self.wndAcceCombine, self.wndAcceAbsorption)
	
	def __MakeCubeResultWindow(self):
		self.wndCubeResult = uicube.CubeResultWindow()
		self.wndCubeResult.LoadWindow()
		self.wndCubeResult.Hide()

	# ACCESSORY_REFINE_ADD_METIN_STONE
	def __MakeItemSelectWindow(self):
		self.wndItemSelect = uiselectitem.SelectItemWindow()
		self.wndItemSelect.Hide()
		if app.ENABLE_GAYA_RENEWAL:
			self.wndItemSelectEx = uiselectitemex.SelectItemWindow()
			self.wndItemSelectEx.Hide()
	# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

	if app.ENABLE_EXTRA_INVENTORY:
		def __MakeExtraInventoryWindow(self, parent):
			self.wndExtraInventory = uiextrainventory.ExtraInventoryWindow()
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.wndExtraInventory.BindInterfaceClass(self)
				self.wndExtraInventory.BindWindow(self.dlgExchange)
				if self.privateShopBuilder:
					self.privateShopBuilder.SetExtraInven(self.wndExtraInventory)
					self.wndExtraInventory.BindWindow(self.privateShopBuilder)
					
				self.dlgExchange.BindInterface(self)
				self.dlgExchange.SetExtraInven(self.wndExtraInventory)
			
			self.wndExtraInventory.LoadWindow(parent)

	if app.ENABLE_CUBE_RENEWAL_WORLDARD:
		def __MakeCubeRenewal(self):
			import uicuberenewal
			self.wndCubeRenewal = uicuberenewal.CubeRenewalWindows()
			self.wndCubeRenewal.Hide()

	if app.ENABLE_SWITCHBOT_WORLDARD:
		def __MakeSwitchBot(self):
			self.wndSwitchBot = uiswtichbot.UiSwitchBot()
			self.wndSwitchBot.Hide()

	if app.ENABLE_PREMIUM_PLAYERS:
		def __MakePremiumPlayersWindow(self):
			self.wndPremiumPlayers = uipremiumplayers.PremiumPlayersWindow()
			self.wndPremiumPlayers.LoadWindow()
			self.wndPremiumPlayers.BindInterfaceClass(self)
			self.wndPremiumPlayers.Hide()
			
			self.wndPremiumPlayersRanking = uipremiumplayers.Ranking()
			self.wndPremiumPlayersRanking.LoadWindow()
			self.wndPremiumPlayersRanking.BindInterfaceClass(self.wndPremiumPlayers)
			self.wndPremiumPlayersRanking.Hide()

	def MakeInterface(self):
		self.__MakeMessengerWindow()
		self.__MakeGuildWindow()
		self.__MakeChatWindow()
		self.__MakeParty()
		self.__MakeWindows()
		self.__MakeDialogs()
		
		self.__MakeUICurtain()
		self.__MakeTaskBar()
		self.__MakeGameButtonWindow()
		self.__MakeTipBoard()
		if app.ENABLE_ATTR_TRANSFER_SYSTEM:
			self.__MakeAttrTransferWindow()
		
		if app.ENABLE_CUBE_RENEWAL_WORLDARD:
			self.__MakeCubeRenewal()
		if app.ENABLE_MELEY_LAIR_DUNGEON:
			self.__MakeMeleyRanking()
		self.__MakeCubeWindow()
		self.__MakeCubeResultWindow()
		if app.ENABLE_PREMIUM_PLAYERS:
			self.__MakePremiumPlayersWindow()

		if app.ENABLE_ACCE_SYSTEM:
			self.__MakeAcceWindow()
		
		if app.ENABLE_CHANGELOOK:
			self.__MakeChangeLookWindow()
		
		if app.ENABLE_SWITCHBOT_WORLDARD:
			self.__MakeSwitchBot()
		# ACCESSORY_REFINE_ADD_METIN_STONE
		self.__MakeItemSelectWindow()
		# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE
		
		if app.ENABLE_OFFLINESHOP_REWORK:
			self.wndShopOffline.SetItemToolTip(self.tooltipItem)
			self.wndShopOffline.BindInterface(self)
		
		self.questButtonList = []
		self.whisperButtonList = []
		self.whisperDialogDict = {}
		self.privateShopAdvertisementBoardDict = {}

		self.wndInventory.SetItemToolTip(self.tooltipItem)
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.SetItemToolTip(self.tooltipItem)
			self.wndDragonSoulRefine.SetItemToolTip(self.tooltipItem)
		if app.ENABLE_EXTRA_INVENTORY:
			self.__MakeExtraInventoryWindow(self.wndDragonSoulRefine)
		self.wndSafebox.SetItemToolTip(self.tooltipItem)
		if app.ENABLE_ATTR_TRANSFER_SYSTEM:
			self.wndAttrTransfer.SetItemToolTip(self.tooltipItem)
		
		self.wndCube.SetItemToolTip(self.tooltipItem)
		self.wndCubeResult.SetItemToolTip(self.tooltipItem)
		self.wndSafebox.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_ACCE_SYSTEM:
			self.wndAcceCombine.SetItemToolTip(self.tooltipItem)
			self.wndAcceAbsorption.SetItemToolTip(self.tooltipItem)
		
		if app.ENABLE_CHANGELOOK:
			self.wndChangeLook.SetItemToolTip(self.tooltipItem)
		
		# ITEM_MALL
		self.wndMall.SetItemToolTip(self.tooltipItem)
		# END_OF_ITEM_MALL

		self.wndCharacter.SetSkillToolTip(self.tooltipSkill)
		self.wndTaskBar.SetItemToolTip(self.tooltipItem)
		self.wndTaskBar.SetSkillToolTip(self.tooltipSkill)
		self.wndGuild.SetSkillToolTip(self.tooltipSkill)

		# ACCESSORY_REFINE_ADD_METIN_STONE
		self.wndItemSelect.SetItemToolTip(self.tooltipItem)
		# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

		self.dlgShop.SetItemToolTip(self.tooltipItem)
		self.dlgExchange.SetItemToolTip(self.tooltipItem)
		self.privateShopBuilder.SetItemToolTip(self.tooltipItem)
		if app.ENABLE_GAYA_RENEWAL:
			self.wndItemSelectEx.SetItemToolTip(self.tooltipItem)
			self.dlgGemShop.SetItemToolTip(self.tooltipItem)
		
		if app.ENABLE_EXTRA_INVENTORY:
			self.wndExtraInventory.SetItemToolTip(self.tooltipItem)
		
		if app.ENABLE_MINI_GAME:
			self.IntegrationEventBanner()
		
		self.__InitWhisper()
		
		if app.ENABLE_BIOLOGIST_UI:
			self.wndBiologist.SetItemToolTip(self.tooltipItem)
			self.wndBiologistChange.SetItemToolTip(self.tooltipItem)
		if app.ENABLE_CHOOSE_DOCTRINE_GUI:
			self.wndDoctrine.SetSkillToolTip(self.tooltipSkill)
		
		if app.ENABLE_AURA_SYSTEM:
			self.wndAura.SetItemToolTip(self.tooltipItem)

	def MakeHyperlinkTooltip(self, hyperlink):
		tokens = hyperlink.split(":")
		if tokens and len(tokens):
			type = tokens[0]
			
			if "item" == type:
				self.hyperlinkItemTooltip.SetHyperlinkItem(tokens)
			elif "msg" == type:# and str(tokens[1]) != player.GetMainCharacterName():
				self.OpenWhisperDialog(str(tokens[1]))
			elif app.LINK_IN_CHAT and type == "web" and (tokens[1].startswith("httpXxX") or tokens[1].startswith("httpsXxX")):
				link = "start " + tokens[1].replace("XxX", "://").replace("&","^&")
				os.system(link)
			elif app.LINK_IN_CHAT and "sysweb" == type:
				os.system("start " + tokens[1].replace("XxX", "://").replace("&","^&"))

	## Make Windows & Dialogs
	################################

	def Close(self):
		if app.USE_PICKUP_FILTER:
			if self.wndFilterPopup:
				self.wndFilterPopup.Close()
				self.wndFilterPopup.Destroy()
				del self.wndFilterPopup
				self.wndFilterPopup = None

				if not app.USE_PICKUP_FILTER_ICO_AS_AFFECT and self.wndFilterIco:
					self.wndFilterIco.Hide()
					self.wndFilterIco.Destroy()
					del self.wndFilterIco
					self.wndFilterIco = None

			if self.wndFilter:
				self.wndFilter.Hide()
				self.wndFilter.Destroy()
				del self.wndFilter
				self.wndFilter = None

		if app.USE_AUTO_HUNT and self.wndAutoWindow:
			self.wndAutoWindow.Hide()
			self.wndAutoWindow.Destroy()
			del self.wndAutoWindow
			self.wndAutoWindow = None

		if app.USE_WHEEL_OF_FORTUNE:
			if self.wndWheelofFortune:
				self.wndWheelofFortune.Hide()
				self.wndWheelofFortune.Destroy()
				del self.wndWheelofFortune
				self.wndWheelofFortune=0
		
		if self.dlgWhisperWithoutTarget:
			self.dlgWhisperWithoutTarget.Destroy()
			del self.dlgWhisperWithoutTarget

		if uiquest.QuestDialog.__dict__.has_key("QuestCurtain"):
			uiquest.QuestDialog.QuestCurtain.Close()

		if self.wndQuestWindow:
			for key, eachQuestWindow in self.wndQuestWindow.items():
				eachQuestWindow.nextCurtainMode = -1
				eachQuestWindow.CloseSelf()
				eachQuestWindow = None
		self.wndQuestWindow = {}

		if app.ENABLE_EVENT_MANAGER:
			if self.wndEventManager:
				self.wndEventManager.Hide()
				self.wndEventManager.Destroy()
				del self.wndEventManager
				self.wndEventManager = None
			
			if self.wndEventIcon:
				self.wndEventIcon.Destroy()
				del self.wndEventIcon
				self.wndEventIcon = None
		
		if app.ENABLE_REWARD_SYSTEM:
			if self.wndReward:
				self.wndReward.Hide()
				self.wndReward.Destroy()
				self.wndReward = None
		
		if self.wndTaskBar:
			self.wndTaskBar.Destroy()

		if app.ENABLE_CAPITALE_SYSTEM:
			if self.wndExpandedMoneyTaskbar:
				self.wndExpandedMoneyTaskbar.Destroy()


		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Destroy()

		if self.wndEnergyBar:
			self.wndEnergyBar.Destroy()

		if self.wndCharacter:
			self.wndCharacter.Close()

		if self.wndCharacter:
			self.wndCharacter.Destroy()
		
		if self.wndInventory:
			self.wndInventory.Close()
			self.wndInventory.Destroy()

		if self.wndDragonSoul:
			self.wndDragonSoul.Destroy()

		if self.wndDragonSoulRefine:
			self.wndDragonSoulRefine.Destroy()

		if self.dlgExchange:
			self.dlgExchange.Destroy()

		if self.dlgShop:
			self.dlgShop.Destroy()

		if self.dlgRestart:
			self.dlgRestart.Destroy()

		if self.dlgSystem:
			self.dlgSystem.Destroy()

		if self.dlgPassword:
			self.dlgPassword.Destroy()

		if self.wndMiniMap:
			self.wndMiniMap.Destroy()

		if self.wndSafebox:
			self.wndSafebox.Destroy()
		
		if self.wndMall:
			self.wndMall.Destroy()

		if self.wndParty:
			self.wndParty.Destroy()

		if app.ENABLE_ATTR_TRANSFER_SYSTEM:
			if self.wndAttrTransfer:
				self.wndAttrTransfer.Destroy()
		
		if app.USE_ATTR_6TH_7TH:
			if self.wndAttr67Add:
				self.wndAttr67Add.Destroy()
		
		if app.ENABLE_MELEY_LAIR_DUNGEON:
			if self.wndMeleyRanking:
				self.wndMeleyRanking.Destroy()

		if self.wndCube:
			self.wndCube.Destroy()
		
		if app.ENABLE_ACCE_SYSTEM and  self.wndAcceCombine:
			self.wndAcceCombine.Destroy()
			
		if app.ENABLE_ACCE_SYSTEM and self.wndAcceAbsorption:
			self.wndAcceAbsorption.Destroy()
		
		if app.ENABLE_CHANGELOOK and self.wndChangeLook:
			del self.wndChangeLook
			self.wndChangeLook = None
		
		if self.wndCubeResult:
			self.wndCubeResult.Destroy()

		if self.wndMessenger:
			self.wndMessenger.Destroy()

		if self.wndGuild:
			self.wndGuild.Destroy()
		
		if self.privateShopBuilder:
			self.privateShopBuilder.Destroy()

		if self.dlgRefineNew:
			self.dlgRefineNew.Destroy()

		if self.wndGuildBuilding:
			self.wndGuildBuilding.Destroy()

		if self.wndGameButton:
			self.wndGameButton.Destroy()
		
		if not app.ENABLE_OFFLINESHOP_REWORK:
			if self.wndOfflineshop:
				self.wndOfflineshop.Destroy()
		
		if self.wndCalendarWindow:
			self.wndCalendarWindow.Destroy()
		if app.ENABLE_PREMIUM_PLAYERS:
			if self.wndPremiumPlayers:
				self.wndPremiumPlayers.Destroy()
			if self.wndPremiumPlayersRanking:
				self.wndPremiumPlayersRanking.Destroy()
		# ITEM_MALL
		if self.mallPageDlg:
			self.mallPageDlg.Destroy()
		# END_OF_ITEM_MALL
		
		if app.ENABLE_GAYA_RENEWAL:
			if self.dlgGemShop:
				self.dlgGemShop.Destroy()
			
			if self.wndItemSelectEx:
				self.wndItemSelectEx.Destroy()
		
		if app.USE_BATTLEPASS:
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.Destroy()
			
			del self.wndBattlePassExtended
			self.wndBattlePassExtended = None
		
		# ACCESSORY_REFINE_ADD_METIN_STONE
		if self.wndItemSelect:
			self.wndItemSelect.Destroy()
		# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE
		
		if app.ENABLE_OFFLINESHOP_REWORK:
			if self.wndShopOffline:
				self.wndShopOffline.Destroy()
				self.wndShopOffline.Hide()
				del self.wndShopOffline
		
		if app.ENABLE_NEW_FISHING_SYSTEM:
			if self.wndFishingWindow:
				self.wndFishingWindow.Destroy()
				del self.wndFishingWindow
		
		if app.ENABLE_EXTRA_INVENTORY:
			if self.wndExtraInventory:
				self.wndExtraInventory.Destroy()

		if app.ENABLE_CUBE_RENEWAL_WORLDARD:
			if self.wndCubeRenewal:
				self.wndCubeRenewal.Destroy()
				self.wndCubeRenewal.Close()
		
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			if self.wndDungeonInfo:
				self.wndDungeonInfo.Destroy()
		
		if app.ENABLE_INGAME_ITEMSHOP:
			if self.wndItemShop:
				self.wndItemShop.Hide()
				self.wndItemShop.Destroy()
				self.wndItemShop = None
		
		if app.ENABLE_AURA_SYSTEM:
			if self.wndAura:
				self.wndAura.Destroy()
				del self.wndAura
				self.wndAura = None
		
		if app.ENABLE_TRACK_WINDOW:
			if self.wndTrackWindow:
				self.wndTrackWindow.Destroy()
				del self.wndTrackWindow
				self.wndTrackWindow = None
		
		if app.ENABLE_SWITCHBOT_WORLDARD:
			if self.wndSwitchBot:
				self.wndSwitchBot.Close()
				self.wndSwitchBot.Destroy()
				del self.wndSwitchBot

		if app.ENABLE_INGAME_CHCHANGE:
			if self.wndChangeCh:
				del self.wndChangeCh
				self.wndChangeCh = None
		
		if app.USE_CAPTCHA_SYSTEM:
			if self.wndCaptcha:
				self.wndCaptcha.Destroy()
		
		self.wndChatLog.Destroy()
		
		for btn in self.questButtonList:
			btn.SetEvent(0)
		for btn in self.whisperButtonList:
			btn.SetEvent(0)
		for dlg in self.whisperDialogDict.itervalues():
			dlg.Destroy()
		for brd in self.guildScoreBoardDict.itervalues():
			brd.Destroy()
		
		for k,v in self.equipmentDialogDict.items():
			if v==0:
				continue
			
			if app.ENABLE_PVP_ADVANCED:
				v.Destroy()
			
			del v
			
			self.equipmentDialogDict[k]=0
		
		# ITEM_MALL
		del self.mallPageDlg
		# END_OF_ITEM_MALL

		del self.wndGuild
		del self.wndMessenger
		del self.wndUICurtain
		if self.wndChat:
			self.wndChat.Destroy()
			del self.wndChat
		
		del self.wndTaskBar
		if app.ENABLE_CUBE_RENEWAL_WORLDARD:
			del self.wndCubeRenewal
		if app.ENABLE_CAPITALE_SYSTEM:
			if self.wndExpandedMoneyTaskbar:
				del self.wndExpandedMoneyTaskbar
		if self.wndExpandedTaskBar:
			del self.wndExpandedTaskBar
		del self.wndEnergyBar
		del self.wndCharacter
		del self.wndInventory
		if self.wndDragonSoul:
			del self.wndDragonSoul
		if self.wndDragonSoulRefine:
			del self.wndDragonSoulRefine
		del self.dlgExchange
		del self.dlgShop
		del self.dlgRestart
		del self.dlgSystem
		del self.dlgPassword
		del self.hyperlinkItemTooltip
		
		if self.tooltipItem:
			del self.tooltipItem
			self.tooltipItem = None
		
		if self.tooltipSkill:
			del self.tooltipSkill
			self.tooltipSkill = None
		
		del self.wndMiniMap
		del self.wndSafebox
		del self.wndMall
		del self.wndParty
		if app.ENABLE_ATTR_TRANSFER_SYSTEM:
			del self.wndAttrTransfer
		
		if app.ENABLE_MELEY_LAIR_DUNGEON:
			del self.wndMeleyRanking
		del self.wndCube
		del self.wndCubeResult
		del self.privateShopBuilder
		del self.inputDialog
		del self.wndChatLog
		del self.dlgRefineNew
		del self.wndGuildBuilding
		del self.wndGameButton
		del self.tipBoard
		del self.bigBoard
		del self.wndItemSelect
		
		if app.ENABLE_PREMIUM_PLAYERS:
			del self.wndPremiumPlayers
			del self.wndPremiumPlayersRanking
		
		if app.ENABLE_GAYA_RENEWAL:
			if self.wndItemSelectEx:
				del self.wndItemSelectEx
			
			if self.dlgGemShop:
				del self.dlgGemShop
		
		if app.ENABLE_MINI_GAME:
			if self.wndMiniGame:
				self.wndMiniGame.Destroy()
				del self.wndMiniGame
		
		if app.ENABLE_RANKING:
			if self.wndRanking:
				self.wndRanking.Destroy()
				del self.wndRanking
		
		if app.ENABLE_EXTRA_INVENTORY:
			del self.wndExtraInventory
		
		if app.ENABLE_ACCE_SYSTEM:
			del self.wndAcceCombine
			del self.wndAcceAbsorption
		
		if app.ENABLE_CHANGELOOK:
			del self.wndChangeLook
		
		if not app.ENABLE_OFFLINESHOP_REWORK:
			del self.wndOfflineshop
		
		del self.wndCalendarWindow
		
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			if self.wndDungeonInfo:
				del self.wndDungeonInfo
		
		if app.USE_CAPTCHA_SYSTEM:
			if self.wndCaptcha:
				del self.wndCaptcha
		
		if app.USE_ATTR_6TH_7TH:
			if self.wndAttr67Add:
				del self.wndAttr67Add
		
		self.questButtonList = []
		self.whisperButtonList = []
		self.whisperDialogDict = {}
		self.privateShopAdvertisementBoardDict = {}
		self.guildScoreBoardDict = {}
		self.equipmentDialogDict={}
		
		uichat.DestroyChatInputSetWindow()
		
		if app.ENABLE_WHISPER_ADMIN_SYSTEM:
			if self.wndWhisperManager:
				self.wndWhisperManager.Destroy()
			del self.wndWhisperManager
		
		if app.ENABLE_BIOLOGIST_UI:
			if self.wndBiologist:
				self.wndBiologist.Destroy()
			del self.wndBiologist
			if self.wndBiologistChange:
				self.wndBiologistChange.Destroy()
			del self.wndBiologistChange
		
		if app.ENABLE_CHOOSE_DOCTRINE_GUI:
			if self.wndDoctrine:
				self.wndDoctrine.Close()
				del self.wndDoctrine

	def CloseRankWindow(self):
			self.wndRank.Close()
				
	# def ToggleOpenRankWindow(self):
		# if self.wndRank:
			# self.wndRank.Open()
		# elif self.wndRank:
			# self.wndRank.Destroy()
			# del self.wndRank

	## Skill
	def OnUseSkill(self, slotIndex, coolTime):
		self.wndCharacter.OnUseSkill(slotIndex, coolTime)
		self.wndTaskBar.OnUseSkill(slotIndex, coolTime)
		self.wndGuild.OnUseSkill(slotIndex, coolTime)

		if app.USE_AUTO_HUNT and self.wndAutoWindow:
			self.wndAutoWindow.OnUseSkill()

	def OnActivateSkill(self, slotIndex):
		self.wndCharacter.OnActivateSkill(slotIndex)
		self.wndTaskBar.OnActivateSkill(slotIndex)

		if app.USE_AUTO_HUNT and self.wndAutoWindow:
			self.wndAutoWindow.OnActivateSkill()

	def OnDeactivateSkill(self, slotIndex):
		self.wndCharacter.OnDeactivateSkill(slotIndex)
		self.wndTaskBar.OnDeactivateSkill(slotIndex)

		if app.USE_AUTO_HUNT and self.wndAutoWindow:
			self.wndAutoWindow.OnDeactivateSkill(slotIndex)

	def OnChangeCurrentSkill(self, skillSlotNumber):
		self.wndTaskBar.OnChangeCurrentSkill(skillSlotNumber)

	def SelectMouseButtonEvent(self, dir, event):
		self.wndTaskBar.SelectMouseButtonEvent(dir, event)

	## Refresh
	def RefreshAlignment(self):
		self.wndCharacter.RefreshAlignment()

	def RefreshStatus(self):
		self.wndTaskBar.RefreshStatus()
		self.wndCharacter.RefreshStatus()
		
		if self.wndEnergyBar:
			self.wndEnergyBar.RefreshStatus()
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.RefreshStatus()
			
		if app.ENABLE_CAPITALE_SYSTEM:
			if self.wndExpandedMoneyTaskbar:
				self.wndExpandedMoneyTaskbar.RefreshStatus()
		
		if app.ENABLE_TRACK_WINDOW and self.wndTrackWindow:
			self.wndTrackWindow.RefreshPoints()

	def RefreshStamina(self):
		self.wndTaskBar.RefreshStamina()

	def RefreshSkill(self):
		self.wndCharacter.RefreshSkill()
		self.wndTaskBar.RefreshSkill()
		if app.ENABLE_CHOOSE_DOCTRINE_GUI:
			self.wndDoctrine.RefreshSkill()

	def RefreshInventory(self):
		
		self.wndTaskBar.RefreshQuickSlot()
		self.wndInventory.RefreshItemSlot()
		if app.ENABLE_SWITCHBOT_WORLDARD:
			if self.wndSwitchBot:
				self.wndSwitchBot.RefreshItemSlot()
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.RefreshItemSlot()
		if app.ENABLE_EXTRA_INVENTORY:
			self.wndExtraInventory.RefreshItemSlot()
		
		if app.ENABLE_LETTERS_EVENT and self.wndMiniGame:
			self.wndMiniGame.OnRefreshSlots()
		
		if app.ENABLE_AURA_SYSTEM:
			if player.IsAuraRefineWindowOpen():
				if self.wndAura and self.wndAura.IsShow():
					self.wndAura.RefreshAuraWindow()

	def RefreshInventoryCell(self, slotIndex):
		if self.wndInventory:
			self.wndInventory.RefreshInventoryCell(slotIndex)
		
		if app.ENABLE_LETTERS_EVENT and self.wndMiniGame:
			self.wndMiniGame.OnRefreshSlots()

	def RefreshCharacter(self):
		self.wndCharacter.RefreshCharacter()
		self.wndTaskBar.RefreshQuickSlot()

	def RefreshQuest(self):
		self.wndCharacter.RefreshQuest()

	def RefreshSafebox(self):
		self.wndSafebox.RefreshSafebox()

	# ITEM_MALL
	def RefreshMall(self):
		self.wndMall.RefreshMall()

	def OpenItemMall(self):
		if not self.mallPageDlg:
			self.mallPageDlg = uishop.MallPageDialog()

		self.mallPageDlg.Open()
	# END_OF_ITEM_MALL

	def RefreshMessenger(self):
		self.wndMessenger.RefreshMessenger()

	def RefreshGuildInfoPage(self):
		self.wndGuild.RefreshGuildInfoPage()

	def RefreshGuildBoardPage(self):
		self.wndGuild.RefreshGuildBoardPage()

	def RefreshGuildMemberPage(self):
		self.wndGuild.RefreshGuildMemberPage()

	def RefreshGuildMemberPageGradeComboBox(self):
		self.wndGuild.RefreshGuildMemberPageGradeComboBox()

	def RefreshGuildSkillPage(self):
		self.wndGuild.RefreshGuildSkillPage()

	def RefreshGuildGradePage(self):
		self.wndGuild.RefreshGuildGradePage()

	def DeleteGuild(self):
		self.wndMessenger.ClearGuildMember()
		self.wndGuild.DeleteGuild()

	def RefreshMobile(self):
		self.dlgSystem.RefreshMobile()

	def OnMobileAuthority(self):
		self.dlgSystem.OnMobileAuthority()

	def OnBlockMode(self, mode):
		self.dlgSystem.OnBlockMode(mode)

	# Shop
	def OpenShopDialog(self, vid):
		self.wndInventory.Open()
		#self.wndInventory.SetTop()
		self.dlgShop.Open(vid)
		self.dlgShop.SetTop()

	def CloseShopDialog(self):
		self.dlgShop.Close()

	def RefreshShopDialog(self):
		self.dlgShop.Refresh()

	## Quest
	def OpenCharacterWindowQuestPage(self):
		self.wndCharacter.Show()
		self.wndCharacter.SetState("QUEST")

	def OpenQuestWindow(self, skin, idx):
		if constinfo.INPUT_IGNORE == 1:
			return
		
		wnds = ()

		q = uiquest.QuestDialog(skin, idx)
		q.SetWindowName("QuestWindow" + str(idx))
		q.Show()
		if skin:
			q.Lock()
			wnds = self.__HideWindows()

			# UNKNOWN_UPDATE
			q.AddOnDoneEvent(lambda tmp_self, args=wnds: self.__ShowWindows(args))
			# END_OF_UNKNOWN_UPDATE

		if skin:
			q.AddOnCloseEvent(q.Unlock)
		q.AddOnCloseEvent(lambda key = self.wndQuestWindowNewKey:ui.__mem_func__(self.RemoveQuestDialog)(key))
		self.wndQuestWindow[self.wndQuestWindowNewKey] = q

		self.wndQuestWindowNewKey = self.wndQuestWindowNewKey + 1

		# END_OF_UNKNOWN_UPDATE

	def RemoveQuestDialog(self, key):
		del self.wndQuestWindow[key]

	## Exchange
	def StartExchange(self):
		self.dlgExchange.OpenDialog()
		self.dlgExchange.Refresh()

	def EndExchange(self):
		self.dlgExchange.CloseDialog()

	if app.WJ_ENABLE_TRADABLE_ICON:
		def CantTradableItemExchange(self, dstSlotIndex, srcSlotIndex):
			self.dlgExchange.CantTradableItem(dstSlotIndex, srcSlotIndex)

		def CantTradableExtraItemExchange(self, dstSlotIndex, srcSlotIndex):
			self.dlgExchange.CantTradableExtraItem(dstSlotIndex, srcSlotIndex)

	def RefreshExchange(self):
		self.dlgExchange.Refresh()

	## Party
	def AddPartyMember(self, pid, name):
		self.wndParty.AddPartyMember(pid, name)

		self.__ArrangeQuestButton()

	def UpdatePartyMemberInfo(self, pid):
		self.wndParty.UpdatePartyMemberInfo(pid)

	def RemovePartyMember(self, pid):
		self.wndParty.RemovePartyMember(pid)

		self.__ArrangeQuestButton()

	def LinkPartyMember(self, pid, vid):
		self.wndParty.LinkPartyMember(pid, vid)

	def UnlinkPartyMember(self, pid):
		self.wndParty.UnlinkPartyMember(pid)

	def UnlinkAllPartyMember(self):
		self.wndParty.UnlinkAllPartyMember()

	def ExitParty(self):
		self.wndParty.ExitParty()

		self.__ArrangeQuestButton()

	def PartyHealReady(self):
		self.wndParty.PartyHealReady()

	def ChangePartyParameter(self, distributionMode):
		self.wndParty.ChangePartyParameter(distributionMode)

	## Safebox
	def AskSafeboxPassword(self):
		if self.wndSafebox.IsShow():
			return

		# SAFEBOX_PASSWORD
		self.dlgPassword.SetTitle(localeinfo.PASSWORD_TITLE)
		self.dlgPassword.SetSendMessage("/safebox_password ")
		# END_OF_SAFEBOX_PASSWORD

		self.dlgPassword.ShowDialog()
		self.dlgPassword.SetTop()

	def OpenSafeboxWindow(self, size):
		self.dlgPassword.CloseDialog()
		self.wndSafebox.ShowWindow(size)

	def RefreshSafeboxMoney(self):
		self.wndSafebox.RefreshSafeboxMoney()

	def CommandCloseSafebox(self):
		self.wndSafebox.CommandCloseSafebox()

	# ITEM_MALL
	def AskMallPassword(self):
		if self.wndMall.IsShow():
			return
		self.dlgPassword.SetTitle(localeinfo.MALL_PASSWORD_TITLE)
		self.dlgPassword.SetSendMessage("/mall_password ")
		self.dlgPassword.ShowDialog()
		self.dlgPassword.SetTop()

	def OpenMallWindow(self, size):
		self.dlgPassword.CloseDialog()
		self.wndMall.ShowWindow(size)

	def CommandCloseMall(self):
		self.wndMall.CommandCloseMall()
	# END_OF_ITEM_MALL

	## Guild
	def OnStartGuildWar(self, guildSelf, guildOpp):
		self.wndGuild.OnStartGuildWar(guildSelf, guildOpp)

		guildWarScoreBoard = uiguild.GuildWarScoreBoard()
		guildWarScoreBoard.Open(guildSelf, guildOpp)
		guildWarScoreBoard.Show()
		self.guildScoreBoardDict[uiguild.GetGVGKey(guildSelf, guildOpp)] = guildWarScoreBoard

	def OnEndGuildWar(self, guildSelf, guildOpp):
		self.wndGuild.OnEndGuildWar(guildSelf, guildOpp)

		key = uiguild.GetGVGKey(guildSelf, guildOpp)

		if not self.guildScoreBoardDict.has_key(key):
			return

		self.guildScoreBoardDict[key].Destroy()
		del self.guildScoreBoardDict[key]

	# GUILDWAR_MEMBER_COUNT
	def UpdateMemberCount(self, gulidID1, memberCount1, guildID2, memberCount2):
		key = uiguild.GetGVGKey(gulidID1, guildID2)

		if not self.guildScoreBoardDict.has_key(key):
			return

		self.guildScoreBoardDict[key].UpdateMemberCount(gulidID1, memberCount1, guildID2, memberCount2)
	# END_OF_GUILDWAR_MEMBER_COUNT

	def OnRecvGuildWarPoint(self, gainGuildID, opponentGuildID, point):
		key = uiguild.GetGVGKey(gainGuildID, opponentGuildID)
		if not self.guildScoreBoardDict.has_key(key):
			return

		guildBoard = self.guildScoreBoardDict[key]
		guildBoard.SetScore(gainGuildID, opponentGuildID, point)

	## PK Mode
	def OnChangePKMode(self):
		self.wndCharacter.RefreshAlignment()
		self.dlgSystem.OnChangePKMode()

	## Refine
	def OpenRefineDialog(self, targetItemPos, nextGradeItemVnum, cost, prob, type):
		if self.dlgRefineNew:
			self.dlgRefineNew.Open(targetItemPos, nextGradeItemVnum, cost, prob, type)

	def CloseRefineDialog(self):
		if self.dlgRefineNew and\
			self.dlgRefineNew.IsShow:
			self.dlgRefineNew.Close()

	if app.USE_AUTO_REFINE:
		def RefineFail(self, isFail):
			if self.dlgRefineNew:
				self.dlgRefineNew.CheckRefine(isFail)

	def AppendMaterialToRefineDialog(self, vnum, count):
		if self.dlgRefineNew:
			self.dlgRefineNew.AppendMaterial(vnum, count)

	## Show & Hide
	def ShowDefaultWindows(self):
		self.wndTaskBar.Show()
		self.wndMiniMap.Show()
		self.wndMiniMap.ShowMiniMap()
		if self.wndEnergyBar:
			self.wndEnergyBar.Show()
		
		if app.ENABLE_MINI_GAME:
			if self.wndMiniGame:
				self.wndMiniGame.ShowMiniGameDialog()
		
		if app.ENABLE_CAPITALE_SYSTEM:
			if self.wndExpandedMoneyTaskbar and constinfo.wndExpandedMoneyTaskbar == 0:
				self.wndExpandedMoneyTaskbar.Show()
				self.wndExpandedMoneyTaskbar.SetTop()
		
		if app.USE_CAPTCHA_SYSTEM:
			if self.wndChat:
				self.wndChat.Show()
			
			if self.wndGameButton:
				self.wndGameButton.Show()
			
			if self.uiAffectShower:
				self.uiAffectShower.Show()
			
			self.IsHideUIMode = False

	if app.USE_CAPTCHA_SYSTEM:
		def GetHideUIMode(self):
			return self.IsHideUIMode

	def ShowAllWindows(self):
		self.wndTaskBar.Show()
		self.wndCharacter.Show()
		self.wndInventory.Open()
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.Show()
			self.wndDragonSoulRefine.Show()
		self.wndChat.Show()
		self.wndMiniMap.Show()
		if app.ENABLE_RANKING:
			if self.wndRanking:
				self.wndRanking.Open()
		
		if self.wndEnergyBar:
			self.wndEnergyBar.Show()
		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Show()
			self.wndExpandedTaskBar.SetTop()
		
		if app.ENABLE_CAPITALE_SYSTEM:
			if self.wndExpandedMoneyTaskbar:
				self.wndExpandedMoneyTaskbar.Show()
				self.wndExpandedMoneyTaskbar.SetTop()
		
		if app.ENABLE_WHISPER_ADMIN_SYSTEM:
			self.wndWhisperManager.Show()
		if app.ENABLE_NEW_FISHING_SYSTEM:
			if self.wndFishingWindow:
				self.wndFishingWindow.Open()

		if app.ENABLE_INGAME_CHCHANGE:
			if self.wndChangeCh:
				self.wndChangeCh.OnOpen()
		
		if app.USE_BATTLEPASS and self.wndBattlePassExtended:
			self.wndBattlePassExtended.Show()

		if app.USE_PICKUP_FILTER:
			if not app.USE_PICKUP_FILTER_ICO_AS_AFFECT and self.wndFilterIco:
				self.wndFilterIco.Show()

			if self.wndFilter:
				self.wndFilter.Show()

	def HideAllWindows(self):
		if app.USE_CAPTCHA_SYSTEM:
			if self.IsHideUIMode == True:
				return
		
		if self.wndTaskBar:
			self.wndTaskBar.Hide()
		
		if self.wndEnergyBar:
			self.wndEnergyBar.Hide()
		
		if self.wndCharacter:
			self.wndCharacter.Hide()
		
		if self.wndInventory:
			if app.ENABLE_ATTR_COSTUMES:
				self.wndInventory.OnCloseRemoveAttrCostume()
			
			self.wndInventory.Close(1)
		
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.Hide()
			self.wndDragonSoulRefine.Hide()
		
		if self.wndChat:
			if app.USE_CAPTCHA_SYSTEM:
				self.wndChat.hide_btnChatSizing()
			
			self.wndChat.Hide()
		
		if self.wndMiniMap:
			self.wndMiniMap.Hide()
		
		if self.wndMessenger:
			self.wndMessenger.Hide()
		
		if self.wndGuild:
			self.wndGuild.Hide()
		
		if app.ENABLE_CAPITALE_SYSTEM:
			if self.wndExpandedMoneyTaskbar:
				self.wndExpandedMoneyTaskbar.Hide()
		
		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Hide()
			
		if app.ENABLE_PREMIUM_PLAYERS:
			if self.wndPremiumPlayers:
				self.wndPremiumPlayers.Hide()
			if self.wndPremiumPlayersRanking:
				self.wndPremiumPlayersRanking.Hide()
		
		if app.ENABLE_MINI_GAME:
			if self.wndMiniGame:
				self.wndMiniGame.HideMiniGameDialog()
		
		if app.ENABLE_RANKING:
			if self.wndRanking:
				self.wndRanking.Close()
		
		if not app.ENABLE_OFFLINESHOP_REWORK:
			if self.wndOfflineshop:
				self.wndOfflineshop.Hide()
		
		if self.wndCalendarWindow:
			self.wndCalendarWindow.Hide()
		
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			if self.wndDungeonInfo:
				self.wndDungeonInfo.Hide()
		
		if app.ENABLE_WHISPER_ADMIN_SYSTEM:
			self.wndWhisperManager.Hide()
		
		if app.ENABLE_BIOLOGIST_UI:
			self.wndBiologist.Hide()
			self.wndBiologistChange.Hide()
		
		if app.ENABLE_NEW_FISHING_SYSTEM:
			if self.wndFishingWindow:
				self.wndFishingWindow.Close()
		
		if app.ENABLE_INGAME_CHCHANGE:
			if self.wndChangeCh:
				self.wndChangeCh.OnClose()
		
		if app.ENABLE_AURA_SYSTEM:
			if self.wndAura:
				self.wndAura.Hide()
		
		if app.ENABLE_TRACK_WINDOW and self.wndTrackWindow:
			self.wndTrackWindow.Hide()

		if app.USE_AUTO_HUNT and self.wndAutoWindow:
			self.wndAutoWindow.Hide()

		if app.USE_BATTLEPASS and self.wndBattlePassExtended:
			self.wndBattlePassExtended.Hide()

		if app.USE_PICKUP_FILTER:
			if self.wndFilterPopup:
				self.wndFilterPopup.Close()

			if not app.USE_PICKUP_FILTER_ICO_AS_AFFECT and self.wndFilterIco:
				self.wndFilterIco.Hide()

			if self.wndFilter:
				self.wndFilter.Hide()

		if app.USE_CAPTCHA_SYSTEM:
			if self.uiAffectShower:
				self.uiAffectShower.Hide()
			
			if self.uiTargetBoard:
				self.uiTargetBoard.Hide()
			
			self.IsHideUIMode = True

	def ShowMouseImage(self):
		self.wndTaskBar.ShowMouseImage()

	def HideMouseImage(self):
		self.wndTaskBar.HideMouseImage()

	def ToggleChat(self):
		if True == self.wndChat.IsEditMode():
			self.wndChat.CloseChat()
		else:
			self.wndChat.OpenChat()

	def IsOpenChat(self):
		return self.wndChat.IsEditMode()

	def SetChatFocus(self):
		self.wndChat.SetChatFocus()

	def OpenRestartDialog(self):
		self.dlgRestart.OpenDialog()
		self.dlgRestart.SetTop()

	def CloseRestartDialog(self):
		self.dlgRestart.Close()

		if app.USE_AUTO_HUNT and self.wndAutoWindow and self.wndAutoWindow.IsShow():
			self.wndAutoWindow.Hide()

	def ToggleSystemDialog(self):
		if False == self.dlgSystem.IsShow():
			self.dlgSystem.OpenDialog()
			self.dlgSystem.SetTop()
		else:
			self.dlgSystem.Close()

	def OpenSystemDialog(self):
		self.dlgSystem.OpenDialog()
		self.dlgSystem.SetTop()

	def ToggleMessenger(self):
		if self.wndMessenger.IsShow():
			self.wndMessenger.Hide()
		else:
			self.wndMessenger.SetTop()
			self.wndMessenger.Show()

	def ToggleMiniMap(self):
		if app.IsPressed(app.DIK_LSHIFT) or app.IsPressed(app.DIK_RSHIFT):
			if False == self.wndMiniMap.isShowMiniMap():
				self.wndMiniMap.ShowMiniMap()
				self.wndMiniMap.SetTop()
			else:
				self.wndMiniMap.HideMiniMap()

		else:
			self.wndMiniMap.ToggleAtlasWindow()

	def PressMKey(self):
		if app.IsPressed(app.DIK_LALT) or app.IsPressed(app.DIK_RALT):
			self.ToggleMessenger()

		else:
			self.ToggleMiniMap()

	def MiniMapScaleUp(self):
		self.wndMiniMap.ScaleUp()

	def MiniMapScaleDown(self):
		self.wndMiniMap.ScaleDown()

	def ToggleCharacterWindow(self, state):
		if False == player.IsObserverMode():
			if False == self.wndCharacter.IsShow():
				self.OpenCharacterWindowWithState(state)
			else:
				if state == self.wndCharacter.GetState():
					self.wndCharacter.OverOutItem()
					self.wndCharacter.Hide()
				else:
					self.wndCharacter.SetState(state)

	def OpenCharacterWindowWithState(self, state):
		if False == player.IsObserverMode():
			self.wndCharacter.SetState(state)
			self.wndCharacter.Show()
			self.wndCharacter.SetTop()

	def ToggleCharacterWindowStatusPage(self):
		self.ToggleCharacterWindow("STATUS")
		
	# def ToggleEventButton(self):
			# self.MakeEventCalendar()
			# if self.wndEventManager.IsShow():
				# self.wndEventManager.Close()
			# else:
				# self.wndEventManager.Open()

	def ToggleInventoryWindow(self):
		if False == player.IsObserverMode():
			if False == self.wndInventory.IsShow():
				self.wndInventory.Open()
				#self.wndInventory.SetTop()
			else:
				self.wndInventory.OverOutItem()
				self.wndInventory.Close()

	if app.ENABLE_CAPITALE_SYSTEM:
		def ToggleExpandedMoneyButton(self):
			if False == player.IsObserverMode():
				if False == self.wndExpandedMoneyTaskbar.IsShow():
					constinfo.wndExpandedMoneyTaskbar = 0
					self.wndExpandedMoneyTaskbar.Show()
					self.wndExpandedMoneyTaskbar.SetTop()
				else:
					constinfo.wndExpandedMoneyTaskbar = 1
					self.wndExpandedMoneyTaskbar.Close()

	def ToggleExpandedButton(self):
		if False == player.IsObserverMode():
			if False == self.wndExpandedTaskBar.IsShow():
				constinfo.wndExpandedMenu = 1
				self.wndExpandedTaskBar.Show()
				self.wndExpandedTaskBar.SetTop()
			else:
				constinfo.wndExpandedMenu = 0
				self.wndExpandedTaskBar.Close()

	if app.ENABLE_DS_SET:
		def SetDSSet(self, idx):
			if self.wndDragonSoul:
				self.wndDragonSoul.SetDSSet(idx)

	def DragonSoulActivate(self, deck):
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.ActivateDragonSoulByExtern(deck)

	def DragonSoulDeactivate(self):
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.DeactivateDragonSoul()

	def Highligt_Item(self, inven_type, inven_pos):
		if not app.ENABLE_HIGHLIGHT_SYSTEM:
			if player.DRAGON_SOUL_INVENTORY == inven_type:
				if app.ENABLE_DRAGON_SOUL_SYSTEM:
					self.wndDragonSoul.HighlightSlot(inven_pos)
		else:
			if inven_type == player.INVENTORY:
				self.wndInventory.HighlightSlot(inven_pos)
			elif inven_type == player.DRAGON_SOUL_INVENTORY:
				if app.ENABLE_DRAGON_SOUL_SYSTEM:
					self.wndDragonSoul.HighlightSlot(inven_pos)
			elif app.ENABLE_EXTRA_INVENTORY and inven_type == player.EXTRA_INVENTORY and self.wndExtraInventory:
				self.wndExtraInventory.HighlightSlot(inven_pos)

	def ToggleDragonSoulWindow(self):
		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if False == self.wndDragonSoul.IsShow():
					self.wndDragonSoul.Show()
				else:
					self.wndDragonSoul.Close()

	def ToggleDragonSoulWindowWithNoInfo(self):
		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if False == self.wndDragonSoul.IsShow():
					self.wndDragonSoul.Show()
				else:
					self.wndDragonSoul.Close()

	if app.ENABLE_DUNGEON_INFO_SYSTEM:
		def ToggleDungeonInfoWindow(self):
			if False == player.IsObserverMode():
				if False == self.wndDungeonInfo.IsShow():
					self.wndDungeonInfo.Open()
				else:
					self.wndDungeonInfo.Close()

	def FailDragonSoulRefine(self, reason, inven_type, inven_pos):
		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if True == self.wndDragonSoulRefine.IsShow():
					self.wndDragonSoulRefine.RefineFail(reason, inven_type, inven_pos)

	def SucceedDragonSoulRefine(self, inven_type, inven_pos):
		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if True == self.wndDragonSoulRefine.IsShow():
					self.wndDragonSoulRefine.RefineSucceed(inven_type, inven_pos)

	def OpenDragonSoulRefineWindow(self):
		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if False == self.wndDragonSoulRefine.IsShow():
					self.wndDragonSoulRefine.Show()
					if None != self.wndDragonSoul:
						if False == self.wndDragonSoul.IsShow():
							self.wndDragonSoul.Show()

	def CloseDragonSoulRefineWindow(self):
	

		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if True == self.wndDragonSoulRefine.IsShow():
					self.wndDragonSoulRefine.Close()

	def ToggleGuildWindow(self):
		if not self.wndGuild.IsShow():
			if self.wndGuild.CanOpen():
				self.wndGuild.Open()
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.GUILD_YOU_DO_NOT_JOIN)
		else:
			self.wndGuild.OverOutItem()
			self.wndGuild.Hide()

	def ToggleChatLogWindow(self):
		if self.wndChatLog.IsShow():
			self.wndChatLog.Hide()
		else:
			self.wndChatLog.Show()

	def OnPressBtnOfflineshop(self):
		if app.ENABLE_RENEWAL_OX:
			import background
			if background.GetCurrentMapName() == "metin2_map_oxevent":
				import chr
				
				if not chr.IsGameMaster(player.GetMainCharacterIndex()):
					import chat
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CANNOT_USE_OFFLINESHOP)
					return
		
		if app.ENABLE_OFFLINESHOP_REWORK:
			if not self.wndShopOffline.IsShow() or\
				self.wndShopOffline.wBoard[3].IsShow():
				self.wndShopOffline.Open()
			else:
				self.wndShopOffline.Close()
		else:
			if self.wndOfflineshop.IsShow():
				self.wndOfflineshop.Close()
			else:
				self.wndOfflineshop.Open()

	def ToggleCalendarWindow(self):
		if self.wndCalendarWindow.IsShow():
			self.wndCalendarWindow.Close()
		else:
			self.wndCalendarWindow.Open()

	def CheckGameButton(self):
		if self.wndGameButton:
			self.wndGameButton.CheckGameButton()

	def __OnClickStatusPlusButton(self):
		self.ToggleCharacterWindow("STATUS")

	def __OnClickSkillPlusButton(self):
		self.ToggleCharacterWindow("SKILL")

	def __OnClickQuestButton(self):
		self.ToggleCharacterWindow("QUEST")

	def __OnClickBuildButton(self):
		self.BUILD_OpenWindow()

	if app.ENABLE_ATTR_TRANSFER_SYSTEM:
		def OpenAttrTransferWindow(self):
			self.wndAttrTransfer.Open()
			if self.wndInventory.IsShow() == False:
				self.wndInventory.Open()

		def CloseAttrTransferWindow(self):
			self.wndAttrTransfer.Close()

		def AttrTransferSuccess(self):
			self.wndAttrTransfer.Success()

	if app.ENABLE_MELEY_LAIR_DUNGEON:
		def OpenMeleyRanking(self):
			self.wndMeleyRanking.Open()

		def RankMeleyRanking(self, line, name, members, time):
			self.wndMeleyRanking.AddRank(line, name, members, time)

	# show GIFT
	def ShowGift(self):
		self.wndTaskBar.ShowGift()

	if app.ENABLE_CUBE_RENEWAL_WORLDARD:
		def BINARY_CUBE_RENEWAL_OPEN(self):
			self.wndCubeRenewal.Show()
			self.wndCubeRenewal.SetCenterPosition()

	if app.ENABLE_SWITCHBOT_WORLDARD:
		def BINARY_SWITCHBOT_OPEN(self):
			self.wndSwitchBot.Show()

		def BINARY_SWITCHBOT_CLEAR_INFO(self):
			self.wndSwitchBot.func_clear_bonus_item()

		def BINARY_SWITCHBOT_INFO_BONUS(self,id_bonus,bonus_value_0,bonus_value_1,bonus_value_2,bonus_value_3,bonus_value_4):
			self.wndSwitchBot.func_set_bonus_items(id_bonus,bonus_value_0,bonus_value_1,bonus_value_2,bonus_value_3,bonus_value_4)

		def BINARY_SWITCHBOT_INFO_EXTRA(self):
			self.wndSwitchBot.func_add_mh()

	def OpenCubeWindow(self):
		self.wndCube.Open()

		if False == self.wndInventory.IsShow():
			self.wndInventory.Open()

	if app.ENABLE_ACCE_SYSTEM:
		# def ActAcce(self, iAct, bWindow):
			# if iAct == 1:
				# if bWindow == True:
					# if not self.wndAcceCombine.IsOpened():
						# self.wndAcceCombine.Open()
					
					# if not self.wndInventory.IsShow():
						# self.wndInventory.Open()
				# else:
					# if not self.wndAcceAbsorption.IsOpened():
						# self.wndAcceAbsorption.Open()
					
					# if not self.wndInventory.IsShow():
						# self.wndInventory.Open()
				
				# self.wndInventory.RefreshBagSlotWindow()
			# elif iAct == 2:
				# if bWindow == True:
					# if self.wndAcceCombine.IsOpened():
						# self.wndAcceCombine.Close()
				# else:
					# if self.wndAcceAbsorption.IsOpened():
						# self.wndAcceAbsorption.Close()
				
				# self.wndInventory.RefreshBagSlotWindow()
			# elif iAct == 3 or iAct == 4:
				# if bWindow == True:
					# if self.wndAcceCombine.IsOpened():
						# self.wndAcceCombine.Refresh(iAct)
				# else:
					# if self.wndAcceAbsorption.IsOpened():
						# self.wndAcceAbsorption.Refresh(iAct)
				
				# self.wndInventory.RefreshBagSlotWindow()
		
		#@ikd 
		def ActAcce(self, iAct, bWindow):
			board = (self.wndAcceAbsorption,self.wndAcceCombine)[int(bWindow)]
			if iAct == 1:
				self.ActAcceOpen(board)
				
			elif iAct == 2:
				self.ActAcceClose(board)
			
			
			elif iAct == 3 or iAct == 4:
				self.ActAcceRefresh(board, iAct)
	
		
		def ActAcceOpen(self,board):
			if not board.IsOpened():
				board.Open()
			if not self.wndInventory.IsShow():
				self.wndInventory.Open()
			self.wndInventory.RefreshBagSlotWindow()
		
		
		def ActAcceClose(self,board):
			if board.IsOpened():
				board.Close()
			self.wndInventory.RefreshBagSlotWindow()
		
		def ActAcceRefresh(self,board,iAct):
			if board.IsOpened():
				board.Refresh(iAct)
			self.wndInventory.RefreshBagSlotWindow()


	def UpdateCubeInfo(self, gold, itemVnum, count):
		self.wndCube.UpdateInfo(gold, itemVnum, count)

	def CloseCubeWindow(self):
		self.wndCube.Close()

	def FailedCubeWork(self):
		self.wndCube.Refresh()

	def SucceedCubeWork(self, itemVnum, count):
		self.wndCube.Clear()

		if 0:
			self.wndCubeResult.SetPosition(*self.wndCube.GetGlobalPosition())
			self.wndCubeResult.SetCubeResultItem(itemVnum, count)
			self.wndCubeResult.Open()
			self.wndCubeResult.SetTop()

	def __HideWindows(self):
		hideWindows = self.wndTaskBar,\
						self.wndCharacter,\
						self.wndInventory,\
						self.wndMiniMap,\
						self.wndGuild,\
						self.wndMessenger,\
						self.wndChat,\
						self.wndParty,\
						self.wndGameButton,

		if self.wndInventory.wndEquipment:
			hideWindows += self.wndInventory.wndEquipment,

			if app.USE_AUTO_AGGREGATE:
				wndBraveCape = self.wndInventory.wndEquipment.wndBraveCape
				if wndBraveCape:
					self.wndInventory.wndEquipment.isOpenedBraveCapeWindowWhenClosingInventory = wndBraveCape.IsOpeningInventory()
					wndBraveCape.Close()

		if self.wndInventory.wndCostume:
			hideWindows += self.wndInventory.wndCostume,
		
		if self.wndEnergyBar:
			hideWindows += self.wndEnergyBar,
		
		if app.ENABLE_MINI_GAME:
			if self.wndMiniGame:
				hideWindows += self.wndMiniGame,
		
		if app.ENABLE_CAPITALE_SYSTEM:
			if self.wndExpandedMoneyTaskbar:
				hideWindows += self.wndExpandedMoneyTaskbar,
		
		if self.wndExpandedTaskBar:
			hideWindows += self.wndExpandedTaskBar,
		
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			hideWindows += self.wndDragonSoul,\
						self.wndDragonSoulRefine,
		
		if not app.ENABLE_OFFLINESHOP_REWORK:
			if self.wndOfflineshop:
				hideWindows += self.wndOfflineshop,
		
		if self.wndCalendarWindow:
			hideWindows += self.wndCalendarWindow,

		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			if self.wndDungeonInfo:
				hideWindows += self.wndDungeonInfo,

		if app.ENABLE_PREMIUM_PLAYERS:
			if self.wndPremiumPlayers:
				hideWindows += self.wndPremiumPlayers,
			if self.wndPremiumPlayersRanking:
				hideWindows += self.wndPremiumPlayersRanking,

		if app.ENABLE_RANKING:
			if self.wndRanking:
				hideWindows += self.wndRanking,
		
		if app.USE_BATTLEPASS and self.wndBattlePassExtended:
			hideWindows += self.wndBattlePassExtended,
		
		if app.USE_ATTR_6TH_7TH and self.wndAttr67Add:
			hideWindows += self.wndAttr67Add,

		if app.USE_AUTO_HUNT and self.wndAutoWindow:
			hideWindows += self.wndAutoWindow,

		if app.USE_PICKUP_FILTER:
			if self.wndFilterPopup:
				self.wndFilterPopup.Close()

			if not app.USE_PICKUP_FILTER_ICO_AS_AFFECT and self.wndFilterIco:
				hideWindows += self.wndFilterIco,

			if self.wndFilter:
				hideWindows += self.wndFilter,

		hideWindows = filter(lambda x:x.IsShow(), hideWindows)
		map(lambda x:x.Hide(), hideWindows)
		import sys
		
		self.HideAllQuestButton()
		self.HideAllWhisperButton()
		
		if self.wndChat.IsEditMode():
			self.wndChat.CloseChat()

		return hideWindows

	def __ShowWindows(self, wnds):
		import sys
		map(lambda x:x.Show(), wnds)
		global IsQBHide
		if not IsQBHide:
			self.ShowAllQuestButton()
		else:
			self.HideAllQuestButton()

		self.ShowAllWhisperButton()

		if app.USE_AUTO_AGGREGATE and self.wndInventory.wndEquipment and self.wndInventory.wndEquipment.IsShow():
			wndBraveCape = self.wndInventory.wndEquipment.wndBraveCape
			if wndBraveCape:
				wndBraveCape.Show(self.wndInventory.wndEquipment.isOpenedBraveCapeWindowWhenClosingInventory)

	def BINARY_OpenAtlasWindow(self):
		if self.wndMiniMap:
			self.wndMiniMap.ShowAtlas()

	def BINARY_SetObserverMode(self, flag):
		self.wndGameButton.SetObserverMode(flag)

	# ACCESSORY_REFINE_ADD_METIN_STONE
	def BINARY_OpenSelectItemWindow(self):
		self.wndItemSelect.Open()
	# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

	#####################################################################################
	### Private Shop ###

	def OpenPrivateShopInputNameDialog(self):
		#if player.IsInSafeArea():
		#	chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CANNOT_OPEN_PRIVATE_SHOP_IN_SAFE_AREA)
		#	return

		inputDialog = uicommon.InputDialog()
		inputDialog.SetTitle(localeinfo.PRIVATE_SHOP_INPUT_NAME_DIALOG_TITLE)
		inputDialog.SetMaxLength(32)
		inputDialog.SetAcceptEvent(ui.__mem_func__(self.OpenPrivateShopBuilder))
		inputDialog.SetCancelEvent(ui.__mem_func__(self.ClosePrivateShopInputNameDialog))
		inputDialog.Open()
		self.inputDialog = inputDialog

	def ClosePrivateShopInputNameDialog(self):
		self.inputDialog = None
		return True

	def OpenPrivateShopBuilder(self):

		if not self.inputDialog:
			return True

		if not len(self.inputDialog.GetText()):
			return True

		self.privateShopBuilder.Open(self.inputDialog.GetText())
		self.ClosePrivateShopInputNameDialog()
		return True

	def AppearPrivateShop(self, vid, text):
		board = uiprivateshopbuilder.PrivateShopAdvertisementBoard()
		board.Open(vid, text)

		self.privateShopAdvertisementBoardDict[vid] = board

	def DisappearPrivateShop(self, vid):

		if not self.privateShopAdvertisementBoardDict.has_key(vid):
			return

		del self.privateShopAdvertisementBoardDict[vid]
		uiprivateshopbuilder.DeleteADBoard(vid)

	#####################################################################################
	### Equipment ###

	def OpenEquipmentDialog(self, vid):
		self.CloseEquipmentDialog(vid)
		
		dlg = uiequipmentdialog.EquipmentDialog()
		dlg.SetItemToolTip(self.tooltipItem)
		dlg.SetCloseEvent(ui.__mem_func__(self.CloseEquipmentDialog))
		dlg.Open(vid)
		
		self.equipmentDialogDict[vid]=dlg
		
		return self.equipmentDialogDict[vid]

	def SetEquipmentDialogItem(self, vid, slotIndex, vnum, count):
		if not vid in self.equipmentDialogDict:
			return
		
		self.equipmentDialogDict[vid].SetEquipmentDialogItem(slotIndex, vnum, count)

	def SetEquipmentDialogSocket(self, vid, slotIndex, socketIndex, value):
		if not vid in self.equipmentDialogDict:
			return
		
		self.equipmentDialogDict[vid].SetEquipmentDialogSocket(slotIndex, socketIndex, value)

	def SetEquipmentDialogAttr(self, vid, slotIndex, attrIndex, type, value):
		if not vid in self.equipmentDialogDict:
			return
		
		self.equipmentDialogDict[vid].SetEquipmentDialogAttr(slotIndex, attrIndex, type, value)

	def CloseEquipmentDialog(self, vid):
		if not vid in self.equipmentDialogDict:
			return
		
		if self.equipmentDialogDict.has_key(vid):
			v=self.equipmentDialogDict[vid]
			if v!=0:
				if app.ENABLE_PVP_ADVANCED:
					v.Destroy()
				
				del v
			
			self.equipmentDialogDict[vid]=0

	#####################################################################################

	#####################################################################################
	### Quest ###
	def BINARY_ClearQuest(self, index):
		btn = self.__FindQuestButton(index)
		if 0 != btn:
			self.__DestroyQuestButton(btn)

	def RecvQuest(self, index, name):
		# QUEST_LETTER_IMAGE
		self.BINARY_RecvQuest(index, name, "file", localeinfo.GetLetterImageName())
		# END_OF_QUEST_LETTER_IMAGE

	def BINARY_RecvQuest(self, index, name, iconType, iconName):

		btn = self.__FindQuestButton(index)
		if 0 != btn:
			self.__DestroyQuestButton(btn)

		btn = uiwhisper.WhisperButton()

		# QUEST_LETTER_IMAGE
		import item
		if "item"==iconType:
			item.SelectItem(int(iconName))
			buttonImageFileName=item.GetIconImageFileName()
		else:
			buttonImageFileName=iconName

		if iconName and (iconType not in ("item", "file")): # type "ex" implied
			btn.SetUpVisual("d:/ymir work/ui/game/quest/questicon/%s" % (iconName.replace("open", "close")))
			btn.SetOverVisual("d:/ymir work/ui/game/quest/questicon/%s" % (iconName))
			btn.SetDownVisual("d:/ymir work/ui/game/quest/questicon/%s" % (iconName))
		else:
			btn.SetUpVisual(localeinfo.GetLetterCloseImageName())
			btn.SetOverVisual(localeinfo.GetLetterOpenImageName())
			btn.SetDownVisual(localeinfo.GetLetterOpenImageName())


		btn.SetToolTipText(name, -20, 35)
		btn.ToolTipText.SetHorizontalAlignLeft()

		listOfTypes = iconType.split(",")
		if "blink" in listOfTypes:
			btn.Flash()

		listOfColors = {
			"golden":	0xFFffa200,
			"green":	0xFF00e600,
			"blue":		0xFF0099ff,
			"purple":	0xFFcc33ff,

			"fucsia":	0xFFcc0099,
			"aqua":		0xFF00ffff,
		}
		for k,v in listOfColors.iteritems():
			if k in listOfTypes:
				btn.ToolTipText.SetPackedFontColor(v)
		

			btn.SetToolTipText(name, -20, 35)

			btn.ToolTipText.SetHorizontalAlignLeft()
		
		btn.SetEvent(ui.__mem_func__(self.__StartQuest), btn)
		btn.Show()
		
		btn.index = index
		btn.name = name
		
		self.questButtonList.insert(0, btn)
		self.__ArrangeQuestButton()

	def __ArrangeQuestButton(self):

		screenWidth = wndMgr.GetScreenWidth()
		screenHeight = wndMgr.GetScreenHeight()

		if self.wndParty.IsShow():
			xPos = 100 + 30
		else:
			xPos = 20


		yPos = 170 * screenHeight / 600
		yCount = (screenHeight - 330) / 63

		count = 0
		for btn in self.questButtonList:
			btn.SetPosition(xPos + (int(count/yCount) * 100), yPos + (count%yCount * 63))
			count += 1
			global IsQBHide
			if IsQBHide:
				btn.Hide()
			else:
				btn.Show()

	def __StartQuest(self, btn):
		event.QuestButtonClick(btn.index)
		self.__DestroyQuestButton(btn)

	def __FindQuestButton(self, index):
		for btn in self.questButtonList:
			if btn.index == index:
				return btn

		return 0

	def __DestroyQuestButton(self, btn):
		btn.SetEvent(0)
		self.questButtonList.remove(btn)
		self.__ArrangeQuestButton()

	def HideAllQuestButton(self):
		for btn in self.questButtonList:
			btn.Hide()

	def ShowAllQuestButton(self):
		for btn in self.questButtonList:
			btn.Show()

	#####################################################################################
	### Whisper ###

	def __InitWhisper(self):
		chat.InitWhisper(self)

	def OpenWhisperDialogWithoutTarget(self):
		if not self.dlgWhisperWithoutTarget:
			dlgWhisper = uiwhisper.WhisperDialog(self.MinimizeWhisperDialog, self.CloseWhisperDialog)
			dlgWhisper.BindInterface(self)
			dlgWhisper.LoadDialog()
			dlgWhisper.OpenWithoutTarget(self.RegisterTemporaryWhisperDialog)
			dlgWhisper.SetPosition(self.windowOpenPosition*30,self.windowOpenPosition*30)
			dlgWhisper.Show()
			self.dlgWhisperWithoutTarget = dlgWhisper

			self.windowOpenPosition = (self.windowOpenPosition+1) % 5

		else:
			self.dlgWhisperWithoutTarget.SetTop()
			self.dlgWhisperWithoutTarget.OpenWithoutTarget(self.RegisterTemporaryWhisperDialog)

	def RegisterTemporaryWhisperDialog(self, name):
		if not self.dlgWhisperWithoutTarget:
			return

		btn = self.__FindWhisperButton(name)
		if 0 != btn:
			self.__DestroyWhisperButton(btn)

		elif self.whisperDialogDict.has_key(name):
			oldDialog = self.whisperDialogDict[name]
			oldDialog.Destroy()
			del self.whisperDialogDict[name]

		self.whisperDialogDict[name] = self.dlgWhisperWithoutTarget
		self.dlgWhisperWithoutTarget.OpenWithTarget(name)
		self.dlgWhisperWithoutTarget = None
		self.__CheckGameMaster(name)

	def OpenWhisperDialog(self, name):
		if not self.whisperDialogDict.has_key(name):
			dlg = self.__MakeWhisperDialog(name)
			dlg.OpenWithTarget(name)
			dlg.chatLine.SetFocus()
			dlg.Show()
			
			self.__CheckGameMaster(name)

			btn = self.__FindWhisperButton(name)
			if 0 != btn:
				self.__DestroyWhisperButton(btn)

	def RecvWhisper(self, name):
		if not self.whisperDialogDict.has_key(name):
			btn = self.__FindWhisperButton(name)
			if 0 == btn:
				btn = self.__MakeWhisperButton(name)
				btn.Flash()
				app.FlashApplication()
				chat.AppendChat(chat.CHAT_TYPE_NOTICE, localeinfo.RECEIVE_MESSAGE % (name))
			else:
				btn.Flash()
				app.FlashApplication()
		elif self.IsGameMasterName(name):
			dlg = self.whisperDialogDict[name]
			dlg.SetGameMasterLook()

	def MakeWhisperButton(self, name):
		self.__MakeWhisperButton(name)

	def ShowWhisperDialog(self, btn):
		try:
			self.__MakeWhisperDialog(btn.name)
			dlgWhisper = self.whisperDialogDict[btn.name]
			dlgWhisper.OpenWithTarget(btn.name)
			dlgWhisper.Show()
			self.__CheckGameMaster(btn.name)
		except:
			import dbg
			dbg.TraceError("interface.ShowWhisperDialog - Failed to find key")

		self.__DestroyWhisperButton(btn)

	def MinimizeWhisperDialog(self, name):

		if 0 != name:
			self.__MakeWhisperButton(name)

		self.CloseWhisperDialog(name)

	def CloseWhisperDialog(self, name):

		if 0 == name:

			if self.dlgWhisperWithoutTarget:
				self.dlgWhisperWithoutTarget.Destroy()
				self.dlgWhisperWithoutTarget = None

			return

		try:
			dlgWhisper = self.whisperDialogDict[name]
			dlgWhisper.Destroy()
			del self.whisperDialogDict[name]
		except:
			import dbg
			dbg.TraceError("interface.CloseWhisperDialog - Failed to find key")

	def __ArrangeWhisperButton(self):

		screenWidth = wndMgr.GetScreenWidth()
		screenHeight = wndMgr.GetScreenHeight()

		xPos = screenWidth - 70
		yPos = (170 * screenHeight / 600) + 16
		yCount = (screenHeight - 330) / 63
		#yCount = (screenHeight - 285) / 63

		count = 0
		for button in self.whisperButtonList:

			button.SetPosition(xPos + (int(count/yCount) * -50), yPos + (count%yCount * 63))
			count += 1

	def __FindWhisperButton(self, name):
		for button in self.whisperButtonList:
			if button.name == name:
				return button

		return 0

	def __MakeWhisperDialog(self, name):
		dlgWhisper = uiwhisper.WhisperDialog(self.MinimizeWhisperDialog, self.CloseWhisperDialog)
		dlgWhisper.BindInterface(self)
		dlgWhisper.LoadDialog()
		dlgWhisper.SetPosition(self.windowOpenPosition*30,self.windowOpenPosition*30)
		self.whisperDialogDict[name] = dlgWhisper

		self.windowOpenPosition = (self.windowOpenPosition+1) % 5

		return dlgWhisper

	def __MakeWhisperButton(self, name):
		whisperButton = uiwhisper.WhisperButton()
		whisperButton.SetUpVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
		whisperButton.SetOverVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
		whisperButton.SetDownVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
		if self.IsGameMasterName(name):
			whisperButton.SetToolTipTextWithColor(name, 0xffffa200)
		else:
			whisperButton.SetToolTipText(name)
		whisperButton.ToolTipText.SetHorizontalAlignCenter()
		whisperButton.SetEvent(ui.__mem_func__(self.ShowWhisperDialog), whisperButton)
		whisperButton.Show()
		whisperButton.name = name

		self.whisperButtonList.insert(0, whisperButton)
		self.__ArrangeWhisperButton()

		return whisperButton

	def __DestroyWhisperButton(self, button):
		button.SetEvent(0)
		self.whisperButtonList.remove(button)
		self.__ArrangeWhisperButton()

	def HideAllWhisperButton(self):
		for btn in self.whisperButtonList:
			btn.Hide()

	def ShowAllWhisperButton(self):
		for btn in self.whisperButtonList:
			btn.Show()

	def __CheckGameMaster(self, name):
		if not self.listGMName.has_key(name):
			return
		if self.whisperDialogDict.has_key(name):
			dlg = self.whisperDialogDict[name]
			dlg.SetGameMasterLook()

	def RegisterGameMasterName(self, name):
		if self.listGMName.has_key(name):
			return
		self.listGMName[name] = "GM"

	def IsGameMasterName(self, name):
		if self.listGMName.has_key(name):
			return True
		else:
			return False

	def BUILD_OpenWindow(self):
		self.wndGuildBuilding = uiguild.BuildGuildBuildingWindow()
		self.wndGuildBuilding.Open()
		self.wndGuildBuilding.wnds = self.__HideWindows()
		self.wndGuildBuilding.SetCloseEvent(ui.__mem_func__(self.BUILD_CloseWindow))

	def BUILD_CloseWindow(self):
		self.__ShowWindows(self.wndGuildBuilding.wnds)
		self.wndGuildBuilding = None
		if app.USE_ATTR_6TH_7TH:
			self.wndAttr67Add = None

	def BUILD_OnUpdate(self):
		if not self.wndGuildBuilding:
			return

		if self.wndGuildBuilding.IsPositioningMode():
			import background
			x, y, z = background.GetPickingPoint()
			self.wndGuildBuilding.SetBuildingPosition(x, y, z)

	def BUILD_OnMouseLeftButtonDown(self):
		if not self.wndGuildBuilding:
			return

		# GUILD_BUILDING
		if self.wndGuildBuilding.IsPositioningMode():
			self.wndGuildBuilding.SettleCurrentPosition()
			return True
		elif self.wndGuildBuilding.IsPreviewMode():
			pass
		else:
			return True
		# END_OF_GUILD_BUILDING
		return False

	def BUILD_OnMouseLeftButtonUp(self):
		if not self.wndGuildBuilding:
			return

		if not self.wndGuildBuilding.IsPreviewMode():
			return True

		return False

	def BULID_EnterGuildArea(self, areaID):
		# GUILD_BUILDING
		mainCharacterName = player.GetMainCharacterName()
		masterName = guild.GetGuildMasterName()

		if mainCharacterName != masterName:
			return

		if areaID != player.GetGuildID():
			return
		# END_OF_GUILD_BUILDING

		self.wndGameButton.ShowBuildButton()

	def BULID_ExitGuildArea(self, areaID):
		self.wndGameButton.HideBuildButton()

	def IsEditLineFocus(self):
		if self.ChatWindow.chatLine.IsFocus():
			return 1

		if self.ChatWindow.chatToLine.IsFocus():
			return 1

		return 0

	if app.ENABLE_DEFENSE_WAVE:
		def BINARY_Update_Mast_HP(self, hp):
			self.wndMiniMap.SetMastHP(hp)

		def BINARY_Update_Mast_Timer(self, text):
			self.wndMiniMap.setMastTimer(text)

		def BINARY_Update_Mast_Window(self, i):
			self.wndMiniMap.SetMastWindow(i)

	def GetInventoryPageIndex(self):
		if self.wndInventory:
			return self.wndInventory.GetInventoryPageIndex()
		else:
			return -1

	if app.WJ_ENABLE_TRADABLE_ICON:
		def SetOnTopWindow(self, onTopWnd):
			self.onTopWindow = onTopWnd

		def GetOnTopWindow(self):
			return self.onTopWindow

		def RefreshMarkInventoryBag(self):
			self.wndInventory.RefreshMarkSlots()
			self.wndExtraInventory.RefreshMarkSlots()

	def EmptyFunction(self):
		pass

	if app.ENABLE_EXTRA_INVENTORY:
		def ToggleExtraInventoryWindow(self):
			if not player.IsObserverMode():
				if not self.wndExtraInventory.IsShow():
					self.wndExtraInventory.Open()
					self.wndExtraInventory.SetTop()
				else:
					self.wndExtraInventory.Close()

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def RefreshVisibleCostume(self):
			if not self.wndInventory:
				return
			
			if app.ENABLE_ACCE_SYSTEM and\
				self.wndInventory.wndEquipment:
				self.wndInventory.wndEquipment.RefreshVisibleCostume()
			
			if self.wndInventory and self.wndInventory.wndCostume:
				self.wndInventory.wndCostume.RefreshVisibleCostume()

		def RefreshVisibleCostumeOnce(self, part, stat):
			if not self.wndInventory:
				return
			
			if app.ENABLE_ACCE_SYSTEM and\
				self.wndInventory.wndEquipment:
				self.wndInventory.wndEquipment.RefreshVisibleCostumeOnce(part, stat)
			
			if self.wndInventory and self.wndInventory.wndCostume:
				self.wndInventory.wndCostume.RefreshVisibleCostumeOnce(part, stat)

	if app.ENABLE_RANKING:
		def OpenRanking(self):
			if self.wndRanking:
				self.wndRanking.Open()

		def RankingClearData(self):
			if self.wndRanking:
				self.wndRanking.ClearData()

		def RankingAddRank(self, position, level, points, name, realPosition):
			if self.wndRanking:
				self.wndRanking.AddRank(position, name, points, level, realPosition)

		def RankingRefresh(self):
			if self.wndRanking:
				self.wndRanking.RefreshList()
				self.wndRanking.OnScroll()

		def RefreshExpBtn(self):
			if self.wndTaskBar:
				self.wndTaskBar.RefreshExpBtn()

	if app.ENABLE_PREMIUM_PLAYERS:
		def OpenPremiumPlayers(self):
			if False == self.wndPremiumPlayers.IsShow():
				self.wndPremiumPlayers.Open()
			else:
				self.wndPremiumPlayers.Close()

		def OpenPremiumPlayersRanking(self):
			if False == self.wndPremiumPlayersRanking.IsShow():
				self.wndPremiumPlayersRanking.Open()
			else:
				self.wndPremiumPlayersRanking.Close()
		
		def ListPremiumPlayers(self, byPos, szName):
			self.wndPremiumPlayersRanking.AddRank(byPos, szName)

	if app.ENABLE_WHISPER_ADMIN_SYSTEM:
		def OpenWhisperSystem(self):
			if not self.wndWhisperManager:
				return
			
			if self.wndWhisperManager.IsShow():
				self.wndWhisperManager.Hide()
			else:
				self.wndWhisperManager.Show()

	# if app.ENABLE_PVP_ADVANCED:
		# def OnRefreshBtnPvpMinimap(self):
			# if self.wndMiniMap:
				# self.wndMiniMap.OnRefreshBtnPvp()

	if app.ENABLE_SORT_INVEN:
		def Sort_InventoryDone(self):
			if self.wndInventory:
				self.wndInventory.Sort_InventoryDone()

		def Sort_ExtraInventoryDone(self):
			if self.wndExtraInventory:
				self.wndExtraInventory.Sort_ExtraInventoryDone()

	if app.ENABLE_NEW_CHAT:
		def ChatLogStat(self):
			stat = False
			if self.wndChatLog:
				stat = self.wndChatLog.IsShow()
			return stat

		def ChatChatStat(self):
			stat = False
			if self.wndChat:
				stat = self.wndChat.IsShow()
			return stat

	if app.ENABLE_BIOLOGIST_UI:
		def ClickBiologistButton(self):
			if self.wndBiologist != None:
				self.wndBiologist.RequestOpen()

		def OpenBiologist(self, args):
			if self.wndBiologist != None:
				self.wndBiologist.Open(args)

		def DeliveredBiologist(self, args):
			if self.wndBiologist != None:
				self.wndBiologist.Delivered(args)

		def RewardBiologist(self, args):
			if self.wndBiologist != None:
				self.wndBiologist.Reward(args)

		def CloseBiologist(self):
			if self.wndBiologist != None:
				self.wndBiologist.Close()

		def NextBiologist(self, args):
			if self.wndBiologist != None:
				self.wndBiologist.Next(args)

		def OpenBiologistChange(self):
			if self.wndBiologistChange != None:
				self.wndBiologistChange.Open()

		def AppendBiologistChange(self, args):
			if self.wndBiologistChange != None:
				self.wndBiologistChange.Append(args)

		def ClearBiologistChange(self):
			if self.wndBiologistChange != None:
				self.wndBiologistChange.Clear()

		def CloseBiologistChange(self):
			if self.wndBiologistChange != None:
				self.wndBiologistChange.Close()

	if app.ENABLE_NEW_FISHING_SYSTEM:
		def OnFishingStart(self, have, need):
			if self.wndFishingWindow and not self.wndFishingWindow.IsShow():
				self.wndFishingWindow.OnOpen(have, need)

		def OnFishingStop(self):
			if self.wndFishingWindow and self.wndFishingWindow.IsShow():
				self.wndFishingWindow.OnClose()

		def OnFishingCatch(self, have):
			if self.wndFishingWindow and self.wndFishingWindow.IsShow():
				self.wndFishingWindow.OnCatch(have)

		def OnFishingCatchFailed(self):
			if self.wndFishingWindow and self.wndFishingWindow.IsShow():
				self.wndFishingWindow.OnCatchFailed()

	def ToggleWikiNew(self):
		if app.ENABLE_RENEWAL_OX:
			import background
			if background.GetCurrentMapName() == "metin2_map_oxevent":
				import chr
				
				if not chr.IsGameMaster(player.GetMainCharacterIndex()):
					import chat
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CANNOT_USE_OFFLINESHOP)
					return
		
		net.ToggleWikiWindow()

	if app.ENABLE_EVENT_MANAGER:
		def MakeEventIcon(self):
			if not self.wndEventIcon:
				self.wndEventIcon = uieventcalendar.MovableImage()
				self.wndEventIcon.Show()

		def MakeEventCalendar(self):
			if self.wndEventManager == None:
				self.wndEventManager = uieventcalendar.EventCalendarWindow()
		def OpenEventCalendar(self):
			self.MakeEventCalendar()
			if self.wndEventManager.IsShow():
				self.wndEventManager.Close()
			else:
				self.wndEventManager.Open()
		def RefreshEventStatus(self, eventID, eventStatus, eventendTime, eventEndTimeText):
			if eventendTime != 0:
				eventendTime += app.GetGlobalTimeStamp()
			uieventcalendar.SetEventStatus(eventID, eventStatus, eventendTime, eventEndTimeText)
			self.RefreshEventManager()
		def ClearEventManager(self):
			uieventcalendar.server_event_data={}
		def RefreshEventManager(self):
			if self.wndEventManager:
				self.wndEventManager.Refresh()
			
			if self.wndEventIcon:
				self.wndEventIcon.Refresh()
		def AppendEvent(self, dayIndex, eventID, eventIndex, startTime, endTime, empireFlag, channelFlag, value0, value1, value2, value3, startRealTime, endRealTime, isAlreadyStart):
			self.MakeEventCalendar()
			self.MakeEventIcon()
			if startRealTime != 0:
				startRealTime += app.GetGlobalTimeStamp()
			if endRealTime != 0:
				endRealTime += app.GetGlobalTimeStamp()
			uieventcalendar.SetServerData(dayIndex, eventID, eventIndex, startTime, endTime, empireFlag, channelFlag, value0, value1, value2, value3, startRealTime, endRealTime, isAlreadyStart)

	if app.ENABLE_REWARD_SYSTEM:
		def MakeRewardWindow(self):
			if self.wndReward == None:
				self.wndReward = uireward.RewardWindow()
		def SetRewardPlayers(self, data):
			self.MakeRewardWindow()
			self.wndReward.SetRewardPlayers(data)
		def OpenRewardWindow(self):
			self.MakeRewardWindow()
			if self.wndReward.IsShow():
				self.wndReward.Close()
			else:
				self.wndReward.Open()
	
	def OpenSwitchWindow(self):
		if self.wndSwitchBot.IsShow():
			self.wndSwitchBot.Close()
		else:
			self.wndSwitchBot.Show()

	if app.USE_BATTLEPASS:
		def ReciveOpenExtBattlePass(self):
			if False == self.isFirstOpeningExtBattlePass:
				self.isFirstOpeningExtBattlePass = True
				self.wndBattlePassExtended.SetPage("PREMIUM")
			if False == self.wndBattlePassExtended.IsShow():
				self.ToggleBattlePassExtended()
			else:
				self.wndBattlePassExtended.SetPage(self.wndBattlePassExtended.GetPage())

		def ToggleBattlePassExtended(self):
			if False == self.isFirstOpeningExtBattlePass:
				net.SendExtBattlePassAction(1)
			if False == self.wndBattlePassExtended.IsShow():
				self.wndBattlePassExtended.Show()
				self.wndBattlePassExtended.SetTop()
			else:
				self.wndBattlePassExtended.Close()
		
		def AddExtendedBattleGeneralInfo(self, BattlePassName, BattlePassID, battlePassStartTime, battlePassEndTime):
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.RecvGeneralInfo(2, BattlePassName, BattlePassID, battlePassStartTime, battlePassEndTime)

		def AddExtendedBattlePassMission(self, battlepassID, missionIndex, missionType, missionInfo1, missionInfo2, missionInfo3):
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.AddMission(2, battlepassID, missionIndex, missionType, missionInfo1, missionInfo2, missionInfo3)

		def UpdateExtendedBattlePassMission(self, missionIndex, missionType, newProgress):
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.UpdateMission(2, missionIndex, missionType, newProgress)

		def AddExtendedBattlePassMissionReward(self, battlepassID, missionIndex, missionType, itemVnum, itemCount):
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.AddMissionReward(2, battlepassID, missionIndex, missionType, itemVnum, itemCount)

		def ClearExtendedBattlePassReward(self, battlepassID):
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.ClearReward(battlepassID)

		def AddExtendedBattlePassReward(self, battlepassID, itemVnum, itemCount):
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.AddReward(2, battlepassID, itemVnum, itemCount)

		def AddExtBattlePassRanklistEntry(self, playername, battlepassID, startTime, endTime):
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.AddRankingEntry(playername, 2, battlepassID, startTime, endTime)

	if app.ENABLE_INGAME_ITEMSHOP:
		def MakeItemShopWindow(self):
			if not self.wndItemShop:
				self.wndItemShop = uiitemshop.ItemShopWindow()

		def OpenItemShopWindow(self):
			self.MakeItemShopWindow()
			if self.wndItemShop.IsShow():
				self.wndItemShop.Close()
			else:
				self.wndItemShop.Open()

		def OpenItemShopMainWindow(self):
			self.MakeItemShopWindow()
			self.wndItemShop.Open()
			self.wndItemShop.LoadFirstOpening()

		def ItemShopHideLoading(self):
			self.MakeItemShopWindow()
			self.wndItemShop.Open()
			self.wndItemShop.CloseLoading()

		def ItemShopPurchasesWindow(self):
			self.MakeItemShopWindow()
			self.wndItemShop.Open()
			self.wndItemShop.OpenPurchasesWindow()

		def ItemShopSetDragonCoin(self,dragonCoin):
			self.MakeItemShopWindow()
			self.wndItemShop.SetDragonCoin(dragonCoin)

	if app.ENABLE_GAYA_RENEWAL:
		def OpenGemShop(self):
			self.dlgGemShop.Open()

		def CloseGemShop(self):
			self.dlgGemShop.Close()

		def RefreshGemShop(self):
			self.dlgGemShop.RefreshGemShop()

		def BINARY_OpenSelectItemExWindow(self):
			self.wndItemSelectEx.Open()

	if app.USE_CAPTCHA_SYSTEM:
		def RecvCaptcha(self, code, limit, filename):
			if self.wndCaptcha:
				self.wndCaptcha.Open(code, limit, filename)

		def SetAffectShower(self, uiAffectShower):
			self.uiAffectShower = uiAffectShower

		def SettargetBoard(self, uiTargetBoard):
			self.uiTargetBoard = uiTargetBoard

	if app.ENABLE_MINI_GAME:
		if app.ENABLE_HALLOWEEN_EVENT_2022:
			def SetHalloweenEventStatus(self, isEnable):
				self.miniGameDict[player.HALLOWEEN_EVENT] = isEnable

		if app.ENABLE_LETTERS_EVENT:
			def SetLettersEventStatus(self, isEnable):
				self.miniGameDict[player.LETTERS_EVENT] = isEnable

		if app.ENABLE_OKEY_CARD_GAME:
			def SetOkeyCardsEventStatus(self, isEnable):
				self.miniGameDict[player.OKEY_CARD_EVENT] = isEnable

			def OpenCardsInfoWindow(self):
				if self.wndMiniGame:
					self.wndMiniGame.OpenCardsInfoWindow()

			def OpenCardsWindow(self, safemode):
				if self.wndMiniGame:
					self.wndMiniGame.OpenCardsWindow(safemode)

			def UpdateCardsInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points):
				if self.wndMiniGame:
					self.wndMiniGame.UpdateCardsInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points)

			def UpdateCardsFieldInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
				if self.wndMiniGame:
					self.wndMiniGame.UpdateCardsFieldInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)

			def CardsPutReward(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
				if self.wndMiniGame:
					self.wndMiniGame.CardsPutReward(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)

			def CardsShowIcon(self):
				return
				#if self.wndMiniGame:
				#	self.wndCardsIcon.Show()

		if app.ENABLE_NEW_FISH_EVENT:
			def MiniGameFishUse(self, shape, useCount):
				if self.wndMiniGame:
					self.wndMiniGame.MiniGameFishUse(shape, useCount)

			def MiniGameFishAdd(self, pos, shape):
				if self.wndMiniGame:
					self.wndMiniGame.MiniGameFishAdd(pos, shape)

			def MiniGameFishReward(self, vnum):
				if self.wndMiniGame:
					self.wndMiniGame.MiniGameFishReward(vnum)

			def MiniGameFishCount(self, count):
				if self.wndMiniGame:
					self.wndMiniGame.MiniGameFishCount(count)

			def SetFishEventStatus(self, isEnable):
				self.miniGameDict[player.JIGSAW_EVENT] = isEnable

		def IntegrationEventBanner(self):
			isEmpty = 1
			
			for isEnable in self.miniGameDict.values():
				if isEnable != 0:
					isEmpty = 0
					break
			
			if isEmpty == 0:
				if not self.wndMiniGame:
					self.wndMiniGame = uiminigame.MiniGameWindow()
					if app.ENABLE_NEW_FISH_EVENT:
						if self.tooltipItem:
							if self.wndMiniGame:
								self.wndMiniGame.SetItemToolTip(self.tooltipItem)
						
						if app.WJ_ENABLE_TRADABLE_ICON:
							if self.wndMiniGame:
								self.wndMiniGame.BindInterface(self)
							
							if self.wndInventory:
								if self.wndMiniGame:
									self.wndMiniGame.BindInventory(self.wndInventory)
				
				for event, status in self.miniGameDict.items():
					self.wndMiniGame.IntegrationMiniGame(event, status)
			else:
				if self.wndMiniGame:
					self.wndMiniGame.IntegrationMiniGame(0, 0)

	if app.ENABLE_AURA_SYSTEM:
		def AuraWindowOpen(self, type):
			if self.wndAura.IsShow():
				return
			
			if self.inputDialog or self.privateShopBuilder.IsShow():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.ACCE_NOT_OPEN_REFINE)
				return
			
			if self.dlgRefineNew and self.dlgRefineNew.IsShow:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.ACCE_NOT_OPEN_REFINE)
				return
			
			self.wndAura.Open(type)
			if not self.wndInventory.IsShow():
				self.wndInventory.Open()

		def AuraWindowClose(self):
			if not self.wndAura.IsShow():
				return
			
			self.wndAura.CloseWindow()

	if app.ENABLE_CHANGELOOK:
		def __MakeChangeLookWindow(self):
			if not self.wndChangeLook:
				self.wndChangeLook = uichangelook.ChangeLookWindow()

		def ActChangeLook(self, iAct):
			if not self.wndChangeLook or not self.wndInventory:
				return
			
			if iAct == 1:
				if not self.wndChangeLook.IsShow():
					self.wndChangeLook.Open()
				
				if not self.wndInventory.IsShow():
					self.wndInventory.Open()
				
				self.wndInventory.RefreshBagSlotWindow()
			elif iAct == 2:
				if self.wndChangeLook.IsShow():
					self.wndChangeLook.Close()
				
				self.wndInventory.RefreshBagSlotWindow()
			elif iAct == 3 or iAct == 4:
				if self.wndChangeLook.IsShow():
					self.wndChangeLook.Refresh()
				
				self.wndInventory.RefreshBagSlotWindow()

	if app.ENABLE_TRACK_WINDOW:
		def TrackWindowCheckPacket(self):
			if self.wndTrackWindow:
				self.wndTrackWindow.CheckPacket()

		def MakeTrackWindow(self):
			if not self.wndTrackWindow:
				self.wndTrackWindow = uitrack.TrackWindow()

		def OpenTrackWindow(self):
			if not self.wndTrackWindow:
				return
			
			if self.wndTrackWindow.IsShow():
				self.wndTrackWindow.Close()
			else:
				self.wndTrackWindow.Open()

		def TrackDungeonInfo(self, cmdData):
			if not self.wndTrackWindow:
				return
			
			self.wndTrackWindow.TrackDungeonInfo(cmdData)

		def TrackBossInfo(self, bossID, bossLeftTime, bossChannel):
			if not self.wndTrackWindow:
				return
			
			self.wndTrackWindow.TrackBossInfo(int(bossID), int(bossLeftTime), int(bossChannel))

	if app.USE_WHEEL_OF_FORTUNE:
		def MakeWheelofFortune(self):
			if self.wndWheelofFortune==0:
				self.wndWheelofFortune=uiwheeloffortune.WheelofFortune()

		def OpenWheelofFortune(self):
			self.MakeWheelofFortune()
			if self.wndWheelofFortune.IsShow():
				self.wndWheelofFortune.Close()
			else:
				self.wndWheelofFortune.Open()

		def SetItemData(self, cmd):
			self.MakeWheelofFortune()
			self.wndWheelofFortune.SetItemData(str(cmd))

		def OnSetWhell(self, giftIndex):
			self.MakeWheelofFortune()
			self.wndWheelofFortune.OnSetWhell(int(giftIndex))

		def GetGiftData(self, itemVnum, itemCount):
			self.MakeWheelofFortune()
			self.wndWheelofFortune.GetGiftData(int(itemVnum), int(itemCount))

		def WheelSetDragonCoins(self, coins):
			self.MakeWheelofFortune()
			self.wndWheelofFortune.SetDragonCoins(coins)

	if app.USE_ATTR_6TH_7TH:
		def OpenAttr67AddDlg(self):
			if self.wndAttr67Add.IsShow():
				self.wndAttr67Add.SetTop()
				return
			
			self.wndAttr67Add.Show()
			self.wndAttr67Add.SetTop()

	if app.USE_AUTO_HUNT:
		def OnGameOver(self):
			if self.wndAutoWindow:
				self.wndAutoWindow.OnGameOver()

		def ToggleAutoWindow(self):
			if self.wndAutoWindow == None:
				self.wndAutoWindow = uiauto.AutoWindow()

			if False == player.IsObserverMode():
				if not self.wndAutoWindow.IsShow():
					self.wndAutoWindow.Show()
				else:
					self.wndAutoWindow.Hide()

		def SetAutoCooltime(self, slotindex, cooltime):
			if self.wndAutoWindow:
				self.wndAutoWindow.SetAutoCooltime(slotindex, cooltime)

		def GetAutoStartonoff(self):
			if self.wndAutoWindow:
				return self.wndAutoWindow.GetAutoStartonoff()

			return False

		def RefreshAutoSkillSlot(self):
			if self.wndAutoWindow:
				self.wndAutoWindow.RefreshAutoSkillSlot()

		def RefreshAutoPotionSlot(self):
			if self.wndAutoWindow:
				self.wndAutoWindow.RefreshAutoPotionSlot()

		def AutoHuntCheckOut(self, iPos):
			if self.wndAutoWindow:
				self.wndAutoWindow.Checkout(iPos)

	if app.USE_PICKUP_FILTER:
		def SendFilterSettings(self, bFlag = False):
			if app.USE_AUTO_HUNT:
				if not app.USE_PICKUP_FILTER_ICO_AS_AFFECT and not self.wndFilterIco:
					tmp = uifilter.FilterIco(self)
					self.wndFilterIco = tmp

				if player.GetPremiumHunt() < 1:
					return

			if self.wndFilter == None:
				self.wndFilter = uifilter.FilterWindow()

			self.wndFilter.SendFilterSettings(bFlag)

		def OpenFilterWindow(self):
			if app.USE_AUTO_HUNT and player.GetPremiumHunt() < 1:
				if not self.wndFilterPopup:
					tmp = uicommon.ConnectingDialog()
					tmp.SetText(localeinfo.LOOTING_SYSTEM_NEED_PREMIUM)
					tmp.AutoResize()
					self.wndFilterPopup = tmp
				else:
					if self.wndFilterPopup.IsShow():
						return

				self.wndFilterPopup.Open(3)
				return

			if self.wndFilter == None:
				self.wndFilter = uifilter.FilterWindow()

			if self.wndFilter.IsShow():
				if app.USE_AUTO_HUNT:
					self.wndFilter.OnPressEscapeKey()
				else:
					self.wndFilter.Hide()
			else:
				self.wndFilter.Show()

		if app.USE_AUTO_HUNT or app.ENABLE_PREMIUM_PLAYERS:
			def ClearFilterSettings(self):
				player.PickupFilterClear()

				if self.wndFilter and self.wndFilter.IsShow():
					self.wndFilter.Hide()

	if app.USE_AUTO_AGGREGATE:
		def SetBraveCapeStatus(self, status):
			if self.wndInventory:
				self.wndInventory.SetBraveCapeStatus(status)

	def OnRecvWhisperInfo(self, name, country_flag, empire):
		if name in self.whisperDialogDict:
			self.whisperDialogDict[name].OnRecvWhisperInfo(country_flag, empire)

	def OnAutoSendShout(self):
		if not self.wndChat:
			return

		self.wndChat.OnAutoSendShout()