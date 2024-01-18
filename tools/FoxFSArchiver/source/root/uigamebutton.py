import app
import player
import net
import ui
import localeinfo
if app.ENABLE_PREMIUM_PLAYERS:
	import wndMgr
class GameButtonWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow("uiscript/gamewindow.py")

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self, filename):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, filename)
		except Exception, msg:
			import dbg
			dbg.TraceError("GameButtonWindow.LoadScript - %s" % (msg))
			app.Abort()
			return False

		try:
			self.gameButtonDict={
				"STATUS" : self.GetChild("StatusPlusButton"),
				"SKILL" : self.GetChild("SkillPlusButton"),
				"QUEST" : self.GetChild("QuestButton"),
				"BUILD" : self.GetChild("BuildGuildBuilding"),
				"EXIT_OBSERVER" : self.GetChild("ExitObserver"),
			}
			
			if app.ENABLE_CHOOSE_DOCTRINE_GUI:
				self.gameButtonDict.update({"DOCTRINE" : self.GetChild("DoctrineButton")})
			else:
				self.GetChild("DoctrineButton").Hide()
			
			self.gameButtonDict["EXIT_OBSERVER"].SetEvent(ui.__mem_func__(self.__OnClickExitObserver))
		except Exception, msg:
			import dbg
			dbg.TraceError("GameButtonWindow.LoadScript - %s" % (msg))
			app.Abort()
			return False

		self.__HideAllGameButton()
		self.SetObserverMode(player.IsObserverMode())
		return True

	def Destroy(self):
		for key in self.gameButtonDict:
			self.gameButtonDict[key].SetEvent(0)

		self.gameButtonDict={}

	def SetButtonEvent(self, name, event):
		try:
			self.gameButtonDict[name].SetEvent(event)
		except Exception, msg:
			print "GameButtonWindow.LoadScript - %s" % (msg)
			app.Abort()
			return

	def ShowBuildButton(self):
		self.gameButtonDict["BUILD"].Show()

	def HideBuildButton(self):
		self.gameButtonDict["BUILD"].Hide()

	def CheckGameButton(self):
		if not self.IsShow():
			return
		
		if app.ENABLE_CHOOSE_DOCTRINE_GUI:
			group = net.GetMainActorSkillGroup()
			if group == 1 or group == 2 or player.GetStatus(player.LEVEL) < 5:
				self.gameButtonDict["DOCTRINE"].Hide()
			else:
				self.gameButtonDict["DOCTRINE"].Show()
		
		statusPlusButton=self.gameButtonDict["STATUS"]
		skillPlusButton=self.gameButtonDict["SKILL"]
		
		if player.GetStatus(player.STAT) > 0:
			statusPlusButton.Show()
		else:
			statusPlusButton.Hide()

		if self.__IsSkillStat():
			if app.ENABLE_CHOOSE_DOCTRINE_GUI:
				if self.gameButtonDict["DOCTRINE"].IsShow():
					skillPlusButton.Hide()
				else:
					skillPlusButton.Show()
			else:
				skillPlusButton.Show()
		else:
			skillPlusButton.Hide()

		if app.ENABLE_PREMIUM_PLAYERS:
			premiumButton = ui.MakeButton(self, 5, 10, "", "d:/ymir work/ui/premium_prodomo/btn/", "btn_affect_shower.png", "btn_affect_shower.png", "btn_affect_shower.png")
			premiumButton.ShowToolTip = lambda slotIndex = 0 : self.OverInToolTip(slotIndex)
			premiumButton.HideToolTip = lambda slotIndex = 0 : self.OverOutToolTip(slotIndex)
			premiumButton.SetEvent(self.__OnClickPremiumButton)
			premiumButton.Show()
			self.premiumButton = premiumButton
			
			thinBoardPremium = ui.ThinBoard()
			thinBoardPremium.SetParent(self)
			thinBoardPremium.SetPosition(10, 50)
			thinBoardPremium.Hide()
			self.thinBoardPremium = thinBoardPremium
			
			self.thinBoardPremium.SetSize(125, 50)

	def __IsSkillStat(self):
		if player.GetStatus(player.SKILL_ACTIVE) > 0 and player.GetStatus(player.LEVEL) >= 5:
			return True

		return False

	def __OnClickExitObserver(self):
		net.SendChatPacket("/observer_exit")

	def __HideAllGameButton(self):
		for btn in self.gameButtonDict.values():
			btn.Hide()

	def SetObserverMode(self, isEnable):
		if isEnable:
			self.gameButtonDict["EXIT_OBSERVER"].Show()
		else:
			self.gameButtonDict["EXIT_OBSERVER"].Hide()

	if app.ENABLE_PREMIUM_PLAYERS:
		def OverInToolTip(self, slotIndex):
			textLine = ui.TextLine()
			textLine.SetParent(self.thinBoardPremium)
			textLine.SetPosition(10, 10)
			if player.IsPremium():
				textLine.SetText(localeinfo.PREMIUM_PLAYERS_STATUS_ACTIVE)
			else:
				textLine.SetText(localeinfo.PREMIUM_PLAYERS_STATUS_INACTIVE)
			textLine.Show()
			self.textLine = textLine
			
			textLine2 = ui.TextLine()
			textLine2.SetParent(self.thinBoardPremium)
			textLine2.SetPosition(62, 30)
			textLine2.SetText('|cFF0cff00Click')
			textLine2.SetHorizontalAlignCenter()
			textLine2.Show()
			self.textLine2 = textLine2
			
			self.thinBoardPremium.Show()

		def OverOutToolTip(self, slotIndex):
			self.thinBoardPremium.Hide()

		def __OnClickPremiumButton(self):
			player.SendPremiumPlayersOpenRequest()

		def __OnClickPremiumButton(self):
			player.SendPremiumPlayersOpenRequest()