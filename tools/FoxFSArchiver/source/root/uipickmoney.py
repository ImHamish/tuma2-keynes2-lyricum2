import app
import wndMgr
import ui
import ime
import localeinfo

class PickMoneyDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.division = 0
		self.unitValue = 1
		self.maxValue = 0
		self.eventAccept = 0

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/pickmoneydialog.py")
		except:
			import exception
			exception.Abort("MoneyDialog.LoadDialog.LoadScript")
		
		try:
			self.board = self.GetChild("board")
			self.maxValueTextLine = self.GetChild("max_value")
			self.pickValueEditLine = self.GetChild("money_value")
			self.acceptButton = self.GetChild("accept_button")
			self.cancelButton = self.GetChild("cancel_button")
			self.divisionText = self.GetChild("divisionText")
			self.divisionBtn = self.GetChild("divisionBtn")
		except:
			import exception
			exception.Abort("MoneyDialog.LoadDialog.BindObject")
		
		self.pickValueEditLine.SetReturnEvent(ui.__mem_func__(self.OnAccept))
		self.pickValueEditLine.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.acceptButton.SetEvent(ui.__mem_func__(self.OnAccept))
		self.cancelButton.SetEvent(ui.__mem_func__(self.Close))
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.divisionBtn.SetToggleDownEvent(lambda arg = 1: self.SetDivisionBtn(arg))
		self.divisionBtn.SetToggleUpEvent(lambda arg = 0: self.SetDivisionBtn(arg))

	def Destroy(self):
		self.ClearDictionary()
		self.eventAccept = 0
		self.maxValue = 0
		self.pickValueEditLine = 0
		self.acceptButton = 0
		self.cancelButton = 0
		self.board = None

	def SetDivisionBtn(self, arg):
		self.division = arg

	def SetTitleName(self, text):
		self.board.SetTitleName(text)

	def SetAcceptEvent(self, event):
		self.eventAccept = event

	def SetMax(self, max):
		self.pickValueEditLine.SetMax(max)

	def Open(self, maxValue, unitValue=1, isDivion = False, isExchange = False):
		if localeinfo.IsYMIR() or localeinfo.IsCHEONMA() or localeinfo.IsHONGKONG():
			unitValue = ""
		
		if isExchange:
			self.SetSize(200, 90)
			if self.board:
				self.board.SetSize(200, 90)
			if self.acceptButton:
				self.acceptButton.SetPosition(19 + 15, 58)
			if self.cancelButton:
				self.cancelButton.SetPosition(90 + 15, 58)
			self.divisionText.Hide()
			self.divisionBtn.Hide()
		elif isDivion:
			y = 26
			self.SetSize(170, 90 + y)
			if self.board:
				self.board.SetSize(170, 90 + y)
			if self.acceptButton:
				self.acceptButton.SetPosition(19, 58 + y)
			if self.cancelButton:
				self.cancelButton.SetPosition(90, 58 + y)
			self.divisionText.Show()
			self.divisionBtn.Show()
			self.divisionBtn.SetUp()
			self.division = 0
		else:
			self.SetSize(170, 90)
			if self.board:
				self.board.SetSize(170, 90)
			if self.acceptButton:
				self.acceptButton.SetPosition(19, 58)
			if self.cancelButton:
				self.cancelButton.SetPosition(90, 58)
			self.divisionText.Hide()
			self.divisionBtn.Hide()
		
		width = self.GetWidth()
		(mouseX, mouseY) = wndMgr.GetMousePosition()
		if mouseX + width/2 > wndMgr.GetScreenWidth():
			xPos = wndMgr.GetScreenWidth() - width
		elif mouseX - width/2 < 0:
			xPos = 0
		else:
			xPos = mouseX - width/2
		
		self.SetPosition(xPos, mouseY - self.GetHeight() - 20)
		if localeinfo.IsARABIC():
			self.maxValueTextLine.SetText("/" + str(maxValue))
		else:
			self.maxValueTextLine.SetText(" / " + str(maxValue))
		
		self.pickValueEditLine.SetText(str(unitValue))
		self.pickValueEditLine.SetFocus()
		
		ime.SetCursorPosition(1)
		
		self.unitValue = unitValue
		self.maxValue = maxValue
		self.Show()
		self.SetTop()

	def Close(self):
		self.pickValueEditLine.KillFocus()
		self.Hide()

	def ConvertMoneyText(self, text, powers = dict(k = 10**3, m = 10**6, b = 10**9)):
		"""
		'1k' = 1.000
		'100kk' = 100.000.000
		'100m' = 100.000.000
		'1b' = 1.000.000.000
		'1kmb' = 1.000 (can't use multiple suffixes types)
		"""
		
		import re
		match = re.search(r'(\d+)({:s}+)?'.format('+|'.join(powers.keys())), text, re.I)
		if match:
			moneyValue, suffixName = match.groups()
			moneyValue = int(moneyValue)
			if not suffixName:
				return moneyValue
			
			return moneyValue * (powers[suffixName[0]] ** len(suffixName))
		
		return 0

	def OnAccept(self):
		text = self.pickValueEditLine.GetText()
		
		#if app.ENABLE_USE_KMB_MONEY_FORMAT and text:
		moneyValue = min(self.ConvertMoneyText(text), self.maxValue)
		if not moneyValue:
			return
		
		text = str(moneyValue)
		
		if len(text) > 0 and text.isdigit():
			#money = int(text)
			money = long(text)
			money = min(money, self.maxValue)
			if money > 0:
				if self.eventAccept:
					self.eventAccept(money)
		
		self.Close()

