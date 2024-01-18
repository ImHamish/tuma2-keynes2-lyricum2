import app
import player
import net

import event

import ui
import uiscriptlocale

def LoadScript(self, fileName):
	try:
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)
	except:
		import exception
		exception.Abort("MiniGameFish.LoadScript")

class LettersEventGameWaitingPage(ui.ScriptWindow):
	class DescriptionBox(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)
			self.descIndex = 0

		def __del__(self):
			ui.Window.__del__(self)

		def SetIndex(self, index):
			self.descIndex = index

		def OnRender(self):
			event.RenderEventSet(self.descIndex)

	def __init__(self, wndMiniGameLetters):
		import exception
		if not wndMiniGameLetters:
			exception.Abort("wndMiniGameLetters parameter must be set to MiniGameLetters")
			return
		
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.SetWindowName("LettersEventGameWaitingPage")
		self.wndMiniGameLetters = wndMiniGameLetters
		self.descIndex = 0
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
		
		self.isLoaded = 1
		
		try:
			LoadScript(self, "uiscript/minigameletterseventwaitingpage.py")
		except:
			import exception
			exception.Abort("LettersEventGameWaitingPage.__LoadWindow.LoadObject")
		try:
			self.titleBar = self.GetChild("titlebar")
			self.gameStartButton = self.GetChild("game_start_button")
			self.descBoard = self.GetChild("desc_board")
#			self.prevButton = self.GetChild("prev_button")
#			self.nextButton = self.GetChild("next_button")
		except:
			import exception
			exception.Abort("LettersEventGameWaitingPage.__LoadWindow.BindObject")
		
		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.gameStartButton.SetEvent(ui.__mem_func__(self.__ClickStartButton))
		
		self.descriptionBox = self.DescriptionBox()
		self.descriptionBox.SetParent(self.descBoard)
		self.descriptionBox.Show()
		
#		self.prevButton.SetEvent(ui.__mem_func__(self.PrevDescriptionPage))
#		self.nextButton.SetEvent(ui.__mem_func__(self.NextDescriptionPage))

	def Show(self):
		event.ClearEventSet(self.descIndex)
		self.descIndex = event.RegisterEventSet(uiscriptlocale.LETTERS_EVENT_DESC_FILE_NAME)
		event.SetRestrictedCount(self.descIndex, 65)
		event.SetVisibleLineCount(self.descIndex, 8)
		
		if self.descriptionBox:
			self.descriptionBox.Show()
		
		ui.ScriptWindow.Show(self)

	def Close(self):
		event.ClearEventSet(self.descIndex)
		self.descIndex = 0
		
		if self.descriptionBox:
			self.descriptionBox.Hide()
		
		ui.ScriptWindow.Hide(self)

	def Destroy(self):
		self.isLoaded = 0
		self.ClearDictionary()
		
		self.descIndex = None
		self.titleBar = None
		self.descriptionBox = None

	def __ClickStartButton(self):
		if self.wndMiniGameLetters:
			if self.wndMiniGameLetters.wndGamePage:
				self.wndMiniGameLetters.wndGamePage.Show()
				self.Close()

#	def PrevDescriptionPage(self):
#		if True == event.IsWait(self.descIndex):
#			if event.GetVisibleStartLine(self.descIndex) - 8 >= 0:
#				event.SetVisibleStartLine(self.descIndex, event.GetVisibleStartLine(self.descIndex) - 8)
#				event.Skip(self.descIndex)
#		else:
#			event.Skip(self.descIndex)
#
#	def NextDescriptionPage(self):
#		if True == event.IsWait(self.descIndex):
#			event.SetVisibleStartLine(self.descIndex, event.GetVisibleStartLine(self.descIndex) + 8)
#			event.Skip(self.descIndex)
#		else:
#			event.Skip(self.descIndex)

	def OnUpdate(self):
		(xposEventSet, yposEventSet) = self.descBoard.GetGlobalPosition()
		event.UpdateEventSet(self.descIndex, xposEventSet + 7, -(yposEventSet + 7))
		self.descriptionBox.SetIndex(self.descIndex)

	def OnPressEscapeKey(self):
		self.Close()
		return True

class LettersEventGamePage(ui.ScriptWindow):
	LETTERS_DICT = {
						0 : 79000,
						1 : 79001,
						2 : 79002,
						3 : 79003,
						4 : 79004
	}

	def __init__(self, wndMiniGameLetters):
		import exception
		if not wndMiniGameLetters:
			exception.Abort("wndMiniGameLetters parameter must be set to MiniGameLetters")
			return
		
		ui.ScriptWindow.__init__(self)
		self.wndRewardSlot = None
		self.wndMaterialSlot = None
		self.tooltipItem = None
		self.isLoaded = 0
		self.SetWindowName("LettersEventGamePageWindow")
		self.wndMiniGameLetters = wndMiniGameLetters
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def SetItemToolTip(self, tooltip):
		self.tooltipItem = tooltip

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
		
		self.isLoaded = 1
		
		try:
			LoadScript(self, "uiscript/minigamelettersgamepage.py")
		except:
			import exception
			exception.Abort("LettersEventGamePage.__LoadWindow.LoadObject")
		
		try:
			self.GetChild("titlebar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.GetChild("help_button").SetEvent(ui.__mem_func__(self.__ClickHelpButton))
			self.GetChild("reward_button").SetEvent(ui.__mem_func__(self.__ClickRewardButton))
			
			wndRewardSlot = self.GetChild("reward_slot")
			wndMaterialSlot = self.GetChild("material_slot")
		except:
			import exception
			exception.Abort("LettersEventGamePage.__LoadWindow.BindObject")
		
		wndRewardSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem2))
		wndRewardSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.wndRewardSlot = wndRewardSlot
		
		wndMaterialSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndMaterialSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.wndMaterialSlot = wndMaterialSlot

	def OnRefreshSlots(self):
		for i in xrange(1):
			self.wndRewardSlot.ClearSlot(i)
			self.wndRewardSlot.SetCantMouseEventSlot(i)
		
		try:
			import constinfo
			rewardVnum = constinfo.rewardLettersList[0]
			if rewardVnum:
				self.wndRewardSlot.SetItemSlot(0, rewardVnum, constinfo.rewardLettersList[1])
				self.wndRewardSlot.SetCanMouseEventSlot(0)
		except:
			pass
		
		for i in xrange(5):
			if i in self.LETTERS_DICT.keys():
				itemVnum = self.LETTERS_DICT[i]
				itemCount = player.GetItemCountByVnum(itemVnum)
				if itemCount == 0:
					if itemCount >= 1:
						itemCount = 0
					
					self.wndMaterialSlot.SetItemSlot(i, itemVnum, itemCount)
					self.wndMaterialSlot.SetCantMouseEventSlot(i)
					continue
				
				if itemCount >= 1:
					itemCount = 0
				
				self.wndMaterialSlot.SetCanMouseEventSlot(i)
				self.wndMaterialSlot.SetItemSlot(i, itemVnum, itemCount)
			else:
				self.wndMaterialSlot.ClearSlot(i)

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Show(self):
		self.OnRefreshSlots()
		ui.ScriptWindow.Show(self)

	def Close(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()
		
		ui.ScriptWindow.Hide(self)

	def Destroy(self):
		self.isLoaded = 0
		self.wndMiniGameLetters = None
		self.wndRewardSlot = None
		self.wndMaterialSlot = None
		self.tooltipItem = None
		self.ClearDictionary()

	def __ClickRewardButton(self):
		net.ajf32iuhfs55539141()

	def __ClickHelpButton(self):
		if self.wndMiniGameLetters and self.wndMiniGameLetters.wndWaitingPage:
			self.wndMiniGameLetters.wndWaitingPage.Show()
			self.Close()

	def OverInItem(self, slotIndex):
		if self.tooltipItem and slotIndex in self.LETTERS_DICT.keys():
			self.tooltipItem.SetItemToolTip(self.LETTERS_DICT[slotIndex])

	def OverInItem2(self, slotIndex):
		if self.tooltipItem:
			if slotIndex == 0 or slotIndex == 2:
				import constinfo
				rewardVnum = constinfo.rewardLettersList[0]
				if not rewardVnum:
					return
				
				self.tooltipItem.SetItemToolTip(rewardVnum)
			elif slotIndex == 4:
				import constinfo
				rewardVnum = constinfo.rewardLettersList[1]
				if not rewardVnum:
					return
				
				self.tooltipItem.SetItemToolTip(rewardVnum)

	def	OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

class MiniGameLetters(ui.Window):
	wndWaitingPage = None
	wndGamePage = None

	def __init__(self):
		ui.Window.__init__(self)
		self.SetWindowName("MiniGameLettersWindow")
		self.wndWaitingPage	= LettersEventGameWaitingPage(self)
		self.wndGamePage = LettersEventGamePage(self)

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.isLoaded = 0
		
		if self.wndWaitingPage:
			self.wndWaitingPage.Destroy()
			self.wndWaitingPage = None
		
		if self.wndGamePage:
			self.wndGamePage.Destroy()
			self.wndGamePage = None

	def SetItemToolTip(self, itemToolTip):
		if self.wndGamePage:
			self.wndGamePage.SetItemToolTip(itemToolTip)

	def OnRefreshSlots(self):
		if self.wndGamePage:
			self.wndGamePage.OnRefreshSlots()

	def Open(self):
		if self.wndGamePage:
			if self.wndGamePage.IsShow():
				return
		
		if self.wndWaitingPage:
			self.wndWaitingPage.Show()

	def Close(self):
		if self.wndWaitingPage:
			if self.wndWaitingPage.IsShow():
				self.wndWaitingPage.Close()
		
		if self.wndGamePage:
			if self.wndGamePage.IsShow():
				self.wndGamePage.Close()
