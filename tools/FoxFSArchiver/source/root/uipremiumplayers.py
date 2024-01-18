import ui, chat, uiscriptlocale, uitooltip, player, app, uicommon, localeinfo, constinfo

PATH = 'd:/ymir work/ui/premium_prodomo/'

def IsPremium():
	if player.IsPremium():
		return True
	else:
		return False

def TimeFormat(seconds):
	m, s = divmod(seconds, 60)
	h, m = divmod(m, 60)
	d, h = divmod(h, 24)
	
	return "{}d {}h {}m {}s".format(d, h, m, s)

reqDict = [92003]

class PremiumPlayersWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.dialog_question = uicommon.QuestionDialog()
		self.tooltip_item = uitooltip.ItemToolTip()
		self.tooltip_item.Hide()
		self.interface = None
		# self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def Destroy(self):
		self.ClearDictionary()
		
	def LoadWindow(self):
		try:
			ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/premiumplayerswindow.py")
		except:
			import exception
			exception.Abort("PremiumPlayersWindow.LoadWindow.LoadScript")

		try:
			self.board = self.GetChild('board')
			self.board.SetCloseEvent(self.Close)
			self.thinBoard = self.GetChild('ThinBoard')
			self.statusValue = self.GetChild('StatusValue')
			self.starCheck = self.GetChild('StarCheck')
			self.timeBox = self.GetChild('TimeBox')
			
			self.activateButton = self.GetChild('ActivateButton')
			self.activateButton.SetEvent(ui.__mem_func__(self.__OnClickActivateButton))
			self.listButton = self.GetChild('ListButton')
			self.listButton.SetEvent(ui.__mem_func__(self.__OnClickListButton))
			self.confirmButton = self.GetChild('ConfirmButton')
			self.confirmButton.SetEvent(ui.__mem_func__(self.__OnClickConfirmButton))
			
			
			self.dropDownButton = self.GetChild('DropDownButton')
			self.dropDownButton.SAFE_SetEvent(self.__OnClickDropButton, True)
			self.dropUpButton = self.GetChild('DropUpButton')
			self.dropUpButton.SAFE_SetEvent(self.__OnClickDropButton, False)
			self.dropUpButton.Hide()
			
			self.timeValue = self.GetChild('TimeValue')
			self.timeGauge = self.GetChild('TimeGauge')
			self.timeGaugeFull = self.GetChild('TimeGaugeFull')
			self.timeGauge.Hide()
			
			self.facilityContent = self.GetChild('FacilityContent')
			self.facilityContent.Hide()
			
			self.activateBoard = self.GetChild('ActivateBoard')
			self.activateBoard.Hide()
			
			self.wndItem = self.GetChild('ItemSlot')
			self.wndItem.SetOverInItemEvent(ui.__mem_func__(self.__OnOverInItem))
			self.wndItem.SetOverOutItemEvent(ui.__mem_func__(self.__OnOverOutItem))
		
			for index in xrange(len(reqDict)):
				self.wndItem.SetItemSlot(index, reqDict[index], 1)

		except:
			import exception
			exception.Abort("PremiumPlayersWindow.LoadDialog.BindObject")
	
	def __OnClickListButton(self):
		constinfo.PREMIUM_PLAYERS_RANKING_LIST = []
		player.SendPremiumPlayersListRequest()

	def __OnOverInItem(self, slot_index):
		self.tooltip_item.ClearToolTip()
		self.tooltip_item.AddItemData(reqDict[slot_index], 0, 0, 0, player.INVENTORY)

	def __OnOverOutItem(self):
		self.tooltip_item.Hide()
	
	def __OnClickActivateButton(self):
		if self.activateBoard.IsShow():
			self.activateBoard.Hide()
		else:
			self.activateBoard.Show()
	
	def __OnClickConfirmButton(self):
		self.dialog_question.SetWidth(320)
		self.dialog_question.SetText(uiscriptlocale.PREMIUM_PLAYERS_ACTIVATE_BOARD_CONFIRM_NOTICE)
		self.dialog_question.SetAcceptEvent(lambda arg=True: self.__RequestConfirmButton(arg))
		self.dialog_question.SetCancelEvent(lambda arg=False: self.__RequestConfirmButton(arg))
		self.dialog_question.Open()	

	def __RequestConfirmButton(self, isConfirm):
		if isConfirm:
			player.SendPremiumPlayersActivateRequest()
			
			if self.dialog_question:
				self.dialog_question.Hide()
			if self.activateBoard:
				self.activateBoard.Hide()
		else:
			if self.dialog_question:
				self.dialog_question.Hide()
	
	def __GetPageSize(self):
		WINDOW_WIDTH = 250
		WINDOW_HEIGHT = 112
		
		THINBOARD_WIDTH = 229
		THINBOARD_HEIGHT = 70
		
		if IsPremium():
			WINDOW_HEIGHT = WINDOW_HEIGHT + 12
			THINBOARD_HEIGHT = THINBOARD_HEIGHT + 12
		if self.dropUpButton.IsShow():
			WINDOW_HEIGHT = WINDOW_HEIGHT + 105
			THINBOARD_HEIGHT = THINBOARD_HEIGHT + 105
		if self.activateBoard.IsShow():
			WINDOW_WIDTH = WINDOW_WIDTH + 160
		
		return WINDOW_WIDTH, WINDOW_HEIGHT, THINBOARD_WIDTH, THINBOARD_HEIGHT
		
	def __OnClickDropButton(self, isDropDown):
		if isDropDown:
			self.facilityContent.Show()
			self.dropDownButton.Hide()
			self.dropUpButton.Show()
		else:
			self.facilityContent.Hide()
			self.dropDownButton.Show()
			self.dropUpButton.Hide()
	
	def __ArrangeElementsAndSetValues(self):
		if IsPremium():
			self.statusValue.SetText(uiscriptlocale.PREMIUM_PLAYERS_STATUS_ACTIVE)
			self.starCheck.LoadImage(PATH + 'star_active.png')
			self.timeBox.LoadImage(PATH + 'box_empty_large.png')
			self.activateButton.Hide()
			self.listButton.Show()
			self.timeGauge.Show()	
		else:
			self.statusValue.SetText(uiscriptlocale.PREMIUM_PLAYERS_STATUS_INACTIVE)
			self.starCheck.LoadImage(PATH + 'star_inactive.png')
			self.timeBox.LoadImage(PATH + 'box_empty.png')
			self.activateButton.Show()
			self.listButton.Hide()
			self.timeGauge.Hide()

		self.listButton.Hide()

		if self.facilityContent.IsShow():
			self.timeBox.SetPosition(3, 150)
		else:
			self.timeBox.SetPosition(3, 45)
		
		WINDOW_WIDTH, WINDOW_HEIGHT, THINBOARD_WIDTH, THINBOARD_HEIGHT = self.__GetPageSize()
		self.SetSize(WINDOW_WIDTH, WINDOW_HEIGHT)
		self.board.SetSize(250, WINDOW_HEIGHT)
		self.thinBoard.SetSize(THINBOARD_WIDTH, THINBOARD_HEIGHT)
	
	def __GetTime(self):
		remainTime = player.GetPremiumTime() - app.GetGlobalTimeStamp()
		if remainTime <= 0:
			return 0
		
		return remainTime
	
	def BindInterfaceClass(self, interface):
		self.interface = interface
	
	def OnUpdate(self):
		self.__ArrangeElementsAndSetValues()
		if IsPremium():
			self.timeValue.SetText(uiscriptlocale.PREMIUM_PLAYERS_TIME_VALUE_ACTIVE % TimeFormat(self.__GetTime()))
			self.timeGaugeFull.SetPercentage(self.__GetTime(), 2592000)
		else:
			self.timeValue.SetText(uiscriptlocale.PREMIUM_PLAYERS_TIME_VALUE_INACTIVE)
		
	def Open(self):
		self.Show()
	
	def Close(self):
		self.Hide()
		
	def OnPressEscapeKey(self):
		self.Close()
		return True
		
class Ranking(ui.ScriptWindow):
	MAX_LINE_COUNT = 10
	
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.wndPremiumPlayers = None
		self.ranking_list_button = {}
		self.ranking_list_pos = {}
		self.ranking_list_name = {}

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/PremiumPlayersWindowRanking.py")
			
		except:
			import exception
			exception.Abort("Ranking.LoadDialog.LoadScript")
		
		try:
			self.board = self.GetChild('board')
			self.rankingBoard = self.GetChild('RankingBoard')
			self.scrollBar = self.GetChild('ScrollBar')
		except:
			import exception
			exception.Abort("Ranking.LoadDialog.BindObject")
		
		self.board.SetCloseEvent(self.Close)
		
	def __LoadRankingListSlot(self):
		for index in xrange(min(self.MAX_LINE_COUNT, len(constinfo.PREMIUM_PLAYERS_RANKING_LIST))):
			self.__AppendRankingListSlot(index, self.rankingBoard, 5, 6 + (23*index))
		
		if len(constinfo.PREMIUM_PLAYERS_RANKING_LIST) <= self.MAX_LINE_COUNT:
			self.scrollBar.Hide()
		else:
			self.scrollBar.SetMiddleBarSize(float(self.MAX_LINE_COUNT) / float(len(constinfo.PREMIUM_PLAYERS_RANKING_LIST)))
			self.scrollBar.Show()
		
		self.scrollBar.SetScrollEvent(self.__OnScroll)
		self.scrollBar.SetScrollStep(0.10)
		
		if len(constinfo.PREMIUM_PLAYERS_RANKING_LIST) > self.MAX_LINE_COUNT:
			self.SetSize(205 + 5, self.GetHeight())
			self.board.SetSize(205 + 5, self.board.GetHeight())
			self.rankingBoard.SetSize(188 + 5, self.rankingBoard.GetHeight())
	
	def __OnScroll(self):
		slot_count = len(self.ranking_list_button)
		pos = int(self.scrollBar.GetPos() * (len(constinfo.PREMIUM_PLAYERS_RANKING_LIST) - slot_count))
		
		self.ranking_list_button = {}
		self.ranking_list_pos = {}
		self.ranking_list_name = {}
		
		for index in xrange(min(self.MAX_LINE_COUNT, slot_count)):
			realPos = index + pos			
			self.__AppendRankingListSlot(realPos, self.rankingBoard, 5, 6 + (23*index))
	
	def __AppendRankingListSlot(self, index, parent, xPos, yPos):
		self.ranking_list_button[index] = ui.MakeButton(parent, xPos, yPos, "", "d:/ymir work/ui/premium_prodomo/btn/", "ranking_list_01.png", "ranking_list_02.png", "ranking_list_02.png")
		self.ranking_list_pos[index] = ui.MakeTextLineNewPP(self.ranking_list_button[index], 20, 3, constinfo.PREMIUM_PLAYERS_RANKING_LIST[index]['byPos'], 0xFF31A513)
		self.ranking_list_name[index] = ui.MakeTextLineNewPP(self.ranking_list_button[index], 112, 3, constinfo.PREMIUM_PLAYERS_RANKING_LIST[index]['szName'], 0xFF31DEA3)	
	def BindInterfaceClass(self, wndPremiumPlayers):
		self.wndPremiumPlayers = wndPremiumPlayers
	
	def AddRank(self, byPos, szName):
		constinfo.PREMIUM_PLAYERS_RANKING_LIST.append({'byPos': str(byPos), 'szName': str(szName)})
		
	def OnRunMouseWheel(self, nLen):
		if not self.scrollBar.IsShow():
			return
			
		if nLen > 0 and self.scrollBar:
			self.scrollBar.OnUp()
		else:
			self.scrollBar.OnDown()	
	
	def Open(self):
		xPos, yPos = self.wndPremiumPlayers.GetGlobalPosition()
		self.SetPosition(xPos + self.wndPremiumPlayers.GetWidth(), yPos)
		self.Show()
		
		self.__LoadRankingListSlot()

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

