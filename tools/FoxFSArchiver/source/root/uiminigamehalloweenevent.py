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
		exception.Abort("MiniGameHalloween.LoadScript")

class HalloweenEventGameWaitingPage(ui.ScriptWindow):
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

	def __init__(self):
		import exception
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.SetWindowName("HalloweenEventGameWaitingPage")
		self.descIndex = 0
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
		
		self.isLoaded = 1
		
		try:
			LoadScript(self, "uiscript/minigamehalloweeneventwaitingpage.py")
		except:
			import exception
			exception.Abort("HalloweenEventGameWaitingPage.__LoadWindow.LoadObject")
		try:
			self.titleBar = self.GetChild("titlebar")
			self.gameStartButton = self.GetChild("game_start_button")
			self.descBoard = self.GetChild("desc_board")
			self.prevButton = self.GetChild("prev_button")
			self.nextButton = self.GetChild("next_button")
		except:
			import exception
			exception.Abort("HalloweenEventGameWaitingPage.__LoadWindow.BindObject")
		
		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.gameStartButton.SetEvent(ui.__mem_func__(self.__ClickStartButton))
		
		self.descriptionBox = self.DescriptionBox()
		self.descriptionBox.SetParent(self.descBoard)
		self.descriptionBox.Show()
		
		self.prevButton.SetEvent(ui.__mem_func__(self.PrevDescriptionPage))
		self.nextButton.SetEvent(ui.__mem_func__(self.NextDescriptionPage))

	def Show(self):
		event.ClearEventSet(self.descIndex)
		self.descIndex = event.RegisterEventSet(uiscriptlocale.HALLOWEEN_EVENT_DESC_FILE_NAME)
		event.SetRestrictedCount(self.descIndex, 130)
		event.SetVisibleLineCount(self.descIndex, 9)
		
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
		self.Close()

	def PrevDescriptionPage(self):
		if True == event.IsWait(self.descIndex):
			if event.GetVisibleStartLine(self.descIndex) - 9 >= 0:
				event.SetVisibleStartLine(self.descIndex, event.GetVisibleStartLine(self.descIndex) - 9)
				event.Skip(self.descIndex)
		else:
			event.Skip(self.descIndex)

	def NextDescriptionPage(self):
		if True == event.IsWait(self.descIndex):
			event.SetVisibleStartLine(self.descIndex, event.GetVisibleStartLine(self.descIndex) + 9)
			event.Skip(self.descIndex)
		else:
			event.Skip(self.descIndex)

	def OnUpdate(self):
		(xposEventSet, yposEventSet) = self.descBoard.GetGlobalPosition()
		event.UpdateEventSet(self.descIndex, xposEventSet + 7, -(yposEventSet + 7))
		self.descriptionBox.SetIndex(self.descIndex)

	def OnPressEscapeKey(self):
		self.Close()
		return True

class MiniGameHalloween(ui.Window):
	wndWaitingPage = None

	def __init__(self):
		ui.Window.__init__(self)
		self.SetWindowName("MiniGameHalloweenWindow")
		self.wndWaitingPage	= HalloweenEventGameWaitingPage()

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.isLoaded = 0
		
		if self.wndWaitingPage:
			self.wndWaitingPage.Destroy()
			self.wndWaitingPage = None

	def Open(self):
		if self.wndWaitingPage:
			if self.wndWaitingPage.IsShow():
				return
			
			self.wndWaitingPage.Show()

	def Close(self):
		if self.wndWaitingPage:
			if self.wndWaitingPage.IsShow():
				self.wndWaitingPage.Close()

