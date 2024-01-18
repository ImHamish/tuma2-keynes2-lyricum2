import app
import player
import net

import ui
import uiscriptlocale
import wndMgr
import localeinfo
import constinfo

if app.ENABLE_NEW_FISH_EVENT:
	import uiminigamefishevent

if app.ENABLE_LETTERS_EVENT:
	import uiminigamelettersevent

if app.ENABLE_HALLOWEEN_EVENT_2022:
	import uiminigamehalloweenevent

if app.ENABLE_OKEY_CARD_GAME:
	import uiminigamecardsevent

class MiniGameDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.board = None
		self.closeButton = None
		self.buttonDict	= {}
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.Destroy()

	def Destroy(self):
		self.isLoaded = 0
		self.board = None
		self.closeButton = None
		self.buttonDict	= {}

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
		
		self.isLoaded = 1
		
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/minigamedialog.py")
		except:
			import exception
			exception.Abort("MiniGameDialog.LoadWindow.LoadObject")
		
		try:
			self.board = self.GetChild("board")
			self.closeButton = self.GetChild("close_button")
			self.closeButton.SetEvent(ui.__mem_func__(self.Close))
		except:
			import exception
			exception.Abort("MiniGameDialog.LoadWindow.BindObject")
		
		self.Hide()

	def AppendButton(self, name, func):
		if self.buttonDict.has_key(name):
			return
		
		button = ui.Button()
		button.SetParent(self.board)
		button.SetPosition(10, (10 * (len(self.buttonDict) + 1)) + len(self.buttonDict) * 25)
		button.SetUpVisual("d:/ymir work/ui/public/xlarge_button_01.sub")
		button.SetOverVisual("d:/ymir work/ui/public/xlarge_button_02.sub")
		button.SetDownVisual("d:/ymir work/ui/public/xlarge_button_03.sub")
		
		if name:
			button.SetText(name)
		
		if func: 
			button.SetEvent(ui.__mem_func__(func))
		
		button.Show()
		self.buttonDict[name] = button

	def DeleteButton(self, name):
		if not self.buttonDict.has_key(name):
			return
		
		self.buttonDict[name].Hide()
		del self.buttonDict[name]

	def DeleteAllButton(self):
		for button in self.buttonDict.values():
			button.Hide()
			del button
		
		self.buttonDict.clear()

	def RefreshDialog(self):
		self.board.SetSize(200, (25 * (len(self.buttonDict) + 1)) + (10 * ((len(self.buttonDict) + 1) + 1)))
		self.SetSize(200, (25 * (len(self.buttonDict) + 1)) + (10 * ((len(self.buttonDict) + 1) + 1)))
		self.closeButton.SetPosition( 10, (10 * (len(self.buttonDict) + 1)) + len(self.buttonDict) * 25 )

class MiniGameWindow(ui.ScriptWindow):
	def __init__(self):
		self.isLoaded = 0
		self.miniGameDialog = None
		
		if app.ENABLE_NEW_FISH_EVENT:
			self.fishGame = None
			self.tooltipItem = None
			self.wndInterface = None
			self.wndInventory = None
		
		if app.ENABLE_LETTERS_EVENT:
			self.lettersGame = None
		
		if app.ENABLE_HALLOWEEN_EVENT_2022:
			self.halloweenGame = None
		
		if app.ENABLE_OKEY_CARD_GAME:
			self.okeyGame = None
		
		self.eventStatus = {}
		
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	def Hide(self):
		if self.miniGameDialog:
			self.miniGameDialog.Hide()
		
		wndMgr.Hide(self.hWnd)

	def Destroy(self):
		self.isLoaded = 0
		
		if self.miniGameDialog:
			self.miniGameDialog.Destroy()
			self.miniGameDialog = None
		
		if app.ENABLE_NEW_FISH_EVENT:
			if self.fishGame:
				self.fishGame.Destroy()
				self.fishGame = None
			
			self.tooltipItem = None
			self.wndInterface = None
			self.wndInventory = None
		
		if app.ENABLE_LETTERS_EVENT:
			if self.lettersGame:
				self.lettersGame.Destroy()
				self.lettersGame = None
		
		if app.ENABLE_HALLOWEEN_EVENT_2022:
			if self.halloweenGame:
				self.halloweenGame.Destroy()
				self.halloweenGame = None
		
		if app.ENABLE_OKEY_CARD_GAME:
			if self.okeyGame:
				self.okeyGame.Destroy()
				self.okeyGame = None

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
		
		self.isLoaded = 1
		
		try:
			self.__LoadScript("uiscript/minigamewindow.py")
		except:
			import exception
			exception.Abort("MiniGameWindow.LoadWindow.LoadObject")
		
		try:
			self.eventBannerButton = ui.Button()
			self.eventBannerButton.SetParent(self.GetChild("mini_game_window"))
			self.eventBannerButton.SetPosition(0, 0)
			self.eventBannerButton.SetUpVisual("d:/ymir work/ui/minigame/banner.sub")
			self.eventBannerButton.SetOverVisual("d:/ymir work/ui/minigame/banner.sub")
			self.eventBannerButton.SetDownVisual("d:/ymir work/ui/minigame/banner.sub")
			self.eventBannerButton.SetEvent(ui.__mem_func__(self.__ClickIntegrationEventBannerButton))
			self.eventBannerButton.Hide()
		except:
			import exception
			exception.Abort("MiniGameWindow.LoadWindow.EventBannerButton.BindObject")
		
		try:
			self.miniGameDialog = MiniGameDialog()
			self.miniGameDialog.Hide()
		except:
			import exception
			exception.Abort("MiniGameWindow.LoadWindow.MiniGameDialog")
		
		self.Show()

	def __LoadScript(self, fileName):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)

	def __ClickIntegrationEventBannerButton(self):
		if not self.miniGameDialog:
			return
		
		if self.miniGameDialog.IsShow():
			self.miniGameDialog.Close()
		else:
			self.miniGameDialog.Show()

	def IntegrationMiniGame(self, event, status):
		if event != 0:
			self.eventStatus[event] = status
		
		if event == 0 and status == 0:
			if app.ENABLE_NEW_FISH_EVENT and self.fishGame:
				self.fishGame.Destroy()
				self.fishGame = None
			
			if app.ENABLE_LETTERS_EVENT and self.lettersGame:
				self.lettersGame.Destroy()
				self.lettersGame = None
			
			if app.ENABLE_HALLOWEEN_EVENT_2022 and self.halloweenGame:
				self.halloweenGame.Destroy()
				self.halloweenGame = None
			
			if app.ENABLE_OKEY_CARD_GAME and self.okeyGame:
				self.okeyGame.Destroy()
				self.okeyGame = None
			
			self.eventBannerButton.Hide()
		else:
			self.eventBannerButton.Show()
		
		if app.ENABLE_NEW_FISH_EVENT and event == player.JIGSAW_EVENT and status == 0:
			if self.fishGame:
				self.fishGame.Destroy()
				self.fishGame = None
		
		if app.ENABLE_LETTERS_EVENT and event == player.LETTERS_EVENT and status == 0:
			if self.lettersGame:
				self.lettersGame.Destroy()
				self.lettersGame = None
		
		if app.ENABLE_HALLOWEEN_EVENT_2022 and event == player.HALLOWEEN_EVENT and status == 0:
			if self.halloweenGame:
				self.halloweenGame.Destroy()
				self.halloweenGame = None
		
		if app.ENABLE_OKEY_CARD_GAME and event == player.OKEY_CARD_EVENT and status == 0:
			if self.okeyGame:
				self.okeyGame.Destroy()
				self.okeyGame = None
		
		if self.miniGameDialog:
			self.miniGameDialog.DeleteAllButton()
			
			if event == 0 and status == 0:
				self.miniGameDialog.Hide()
			else:
				for k, v in self.eventStatus.items():
					if app.ENABLE_NEW_FISH_EVENT and k == player.JIGSAW_EVENT and v != 0:
						if not self.fishGame:
							self.fishGame = uiminigamefishevent.MiniGameFish()
							if self.tooltipItem:
								self.fishGame.SetItemToolTip(self.tooltipItem)
							
							if self.wndInterface:
								self.fishGame.BindInterface(self.wndInterface)
							
							if self.wndInventory:
								self.fishGame.BindInventory(self.wndInventory)
						
						self.miniGameDialog.AppendButton(localeinfo.MINIGAME_FISH_EVENT, self.__ClickFishEventButton)
					elif app.ENABLE_LETTERS_EVENT and k == player.LETTERS_EVENT and v != 0:
						if not self.lettersGame:
							self.lettersGame = uiminigamelettersevent.MiniGameLetters()
							if self.tooltipItem:
								self.lettersGame.SetItemToolTip(self.tooltipItem)
						
						self.miniGameDialog.AppendButton(localeinfo.MINIGAME_LETTERS_EVENT, self.OnClickLettersButton)
					elif app.ENABLE_HALLOWEEN_EVENT_2022 and k == player.HALLOWEEN_EVENT and v != 0:
						if not self.halloweenGame:
							self.halloweenGame = uiminigamehalloweenevent.MiniGameHalloween()
						
						self.miniGameDialog.AppendButton(localeinfo.MINIGAME_HALLOWEEN_EVENT, self.OnClickHalloweenButton)
					elif app.ENABLE_OKEY_CARD_GAME and k == player.OKEY_CARD_EVENT and v != 0:
						if not self.okeyGame:
							self.okeyGame = uiminigamecardsevent.MiniGameOkeyCards()
						
						self.miniGameDialog.AppendButton(localeinfo.MINIGAME_OKEY, self.OnClickOkeyGameButton)
			
			self.miniGameDialog.RefreshDialog()

	def OnRefreshSlots(self):
		if app.ENABLE_LETTERS_EVENT and self.lettersGame:
			self.lettersGame.OnRefreshSlots()

	if app.ENABLE_HALLOWEEN_EVENT_2022:
		def OnClickHalloweenButton(self):
			if self.miniGameDialog:
				self.miniGameDialog.Close()
			
			if self.halloweenGame:
				self.halloweenGame.Open()

	if app.ENABLE_OKEY_CARD_GAME:
		def OnClickOkeyGameButton(self):
			if self.miniGameDialog:
				self.miniGameDialog.Close()
			
			if self.okeyGame:
				self.okeyGame.Open()

		def OpenCardsInfoWindow(self):
			if self.okeyGame and self.okeyGame.wndGamePage:
				self.okeyGame.wndGamePage.Open()

		def OpenCardsWindow(self, safemode):
			if self.okeyGame and self.okeyGame.wndGamePage:
				self.okeyGame.wndGamePage.Open(safemode)

		def UpdateCardsInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points):
			if self.okeyGame and self.okeyGame.wndGamePage:
				self.okeyGame.wndGamePage.UpdateCardsInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points)

		def UpdateCardsFieldInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
			if self.okeyGame and self.okeyGame.wndGamePage:
				self.okeyGame.wndGamePage.UpdateCardsFieldInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)

		def CardsPutReward(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
			if self.okeyGame and self.okeyGame.wndGamePage:
				self.okeyGame.wndGamePage.CardsPutReward(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)

	if app.ENABLE_LETTERS_EVENT:
		def OnClickLettersButton(self):
			if self.miniGameDialog:
				self.miniGameDialog.Close()
			
			if self.lettersGame:
				self.lettersGame.Open()

	if app.ENABLE_NEW_FISH_EVENT:
		def MiniGameFishUse(self, shape, useCount):
			if self.fishGame:
				self.fishGame.MiniGameFishUse(shape, useCount)

		def MiniGameFishAdd(self, pos, shape):
			if self.fishGame:
				self.fishGame.MiniGameFishAdd(pos, shape)
				
		def MiniGameFishReward(self, vnum):
			if self.fishGame:
				self.fishGame.MiniGameFishReward(vnum)

		def MiniGameFishCount(self, count):
			if self.fishGame:
				self.fishGame.MiniGameFishCount(count)

		def __ClickFishEventButton(self):
			if self.miniGameDialog:
				self.miniGameDialog.Close()
			
			if self.fishGame:
				self.fishGame.Open()

		def SetItemToolTip(self, tooltip):
			self.tooltipItem = tooltip

		def BindInterface(self, interface):
			self.wndInterface = interface

		def BindInventory(self, inventory):
			self.wndInventory = inventory

	def HideMiniGameDialog(self):
		if self.eventBannerButton:
			if self.eventBannerButton.IsShow():
				self.eventBannerButton.Hide()

		if self.miniGameDialog:
			if self.miniGameDialog.IsShow():
				self.miniGameDialog.Hide()

	def ShowMiniGameDialog(self):
		if self.eventBannerButton:
			self.eventBannerButton.Show()
