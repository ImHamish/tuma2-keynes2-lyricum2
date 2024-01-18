import app
import net

import dbg
import ui

if app.ENABLE_NEW_RESTART:
	import time, uiscriptlocale
	COOLDOWN = 5.0

if app.USE_AUTO_HUNT:
	import player
	import newconstinfo

###################################################################################################
## Restart
class RestartDialog(ui.ScriptWindow):
	if app.USE_AUTO_HUNT:
		autoInst = None
		restart = -1

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		if app.ENABLE_NEW_RESTART:
			self.arg1 = 0
			self.act = 0
			self.cooldownTime = 0.0
			self.cooldownCount = 0.0

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			filePath = "uiscript/restartdialog.py"
			if app.ENABLE_NEW_RESTART:
				filePath = "uiscript/restartdialog_new.py"
			
			pyScrLoader.LoadScriptFile(self, filePath)
		except Exception, msg:
			import sys
			(type, msg, tb)=sys.exc_info()
			dbg.TraceError("RestartDialog.LoadDialog - %s:%s" % (type, msg))
			app.Abort()
			return 0

		try:
			self.restartHereButton=self.GetChild("restart_here_button")
			self.restartTownButton=self.GetChild("restart_town_button")
		except:
			import sys
			(type, msg, tb)=sys.exc_info()
			dbg.TraceError("RestartDialog.LoadDialog - %s:%s" % (type, msg))
			app.Abort()
			return 0
		
		if app.ENABLE_NEW_RESTART:
			self.restartHereButton.SetToggleDownEvent(lambda arg1 = 1, arg2 = 1: self.RestartNew(arg1, arg2))
			self.restartHereButton.SetToggleUpEvent(lambda arg1 = 1, arg2 = 0: self.RestartNew(arg1, arg2))
			self.restartTownButton.SetToggleDownEvent(lambda arg1 = 2, arg2 = 1: self.RestartNew(arg1, arg2))
			self.restartTownButton.SetToggleUpEvent(lambda arg1 = 2, arg2 = 0: self.RestartNew(arg1, arg2))
		else:
			self.restartHereButton.SetEvent(ui.__mem_func__(self.RestartHere))
			self.restartTownButton.SetEvent(ui.__mem_func__(self.RestartTown))
		
		return 1

	def Destroy(self):
		if app.USE_AUTO_HUNT:
			self.autoInst = None

		self.restartHereButton=0
		self.restartTownButton=0
		self.ClearDictionary()

	def OpenDialog(self):
		if app.USE_AUTO_HUNT:
			self.restart = -1

		self.Show()

		if app.ENABLE_NEW_RESTART:
			self.restartHereButton.SetText(uiscriptlocale.RESTART_HERE_NEW % (str(COOLDOWN)))
			self.restartHereButton.SetUp()
			self.restartTownButton.SetText(uiscriptlocale.RESTART_TOWN_NEW % (str(COOLDOWN)))
			self.restartTownButton.SetUp()

	def Close(self):
		if app.USE_AUTO_HUNT:
			self.restart = -1

		self.Hide()
		return True

	if app.ENABLE_NEW_RESTART:
		def RestartNew(self, arg1, arg2):
			self.arg1 = arg1
			self.act = arg2
			if arg1 == 1:
				self.restartTownButton.SetUp()
				self.restartTownButton.SetText(uiscriptlocale.RESTART_TOWN_NEW % (str(COOLDOWN)))
			else:
				self.restartHereButton.SetUp()
				self.restartHereButton.SetText(uiscriptlocale.RESTART_HERE_NEW % (str(COOLDOWN)))
			
			if arg2 == 1:
				self.cooldownTime = float(app.GetTime())
				self.cooldownCount = COOLDOWN
			else:
				self.restartHereButton.SetText(uiscriptlocale.RESTART_HERE_NEW % (str(COOLDOWN)))
				self.restartTownButton.SetText(uiscriptlocale.RESTART_TOWN_NEW % (str(COOLDOWN)))

	if app.ENABLE_NEW_RESTART or app.USE_AUTO_HUNT:
		def OnUpdate(self):
			if self.act == 1:
				if self.cooldownCount <= 0.1:
					self.act = 0
					self.cooldownTime = 0
					self.cooldownCount = 0.0
					
					if self.arg1 == 1:
						self.restartHereButton.SetText(uiscriptlocale.RESTART_HERE_NEW % (str(self.cooldownCount)[:3]))
						self.RestartHere()
					else:
						self.restartTownButton.SetText(uiscriptlocale.RESTART_TOWN_NEW % (str(self.cooldownCount)[:3]))
						self.RestartTown()
				else:
					if float(app.GetTime()) > self.cooldownTime:
						self.cooldownCount -= 0.1
						
						if self.arg1 == 1:
							self.restartHereButton.SetText(uiscriptlocale.RESTART_HERE_NEW % (str(self.cooldownCount)[:3]))
						else:
							self.restartTownButton.SetText(uiscriptlocale.RESTART_TOWN_NEW % (str(self.cooldownCount)[:3]))
						
						self.cooldownTime = float(app.GetTime()) + 0.1
			else:
				if self.restart == -2:
					return
				elif self.restart == -1:
					if player.GetPremiumHunt() > 0:
						if not self.autoInst:
							interfaceInst = newconstinfo.GetInterfaceInstance()
							self.autoInst = interfaceInst.wndAutoWindow if interfaceInst.wndAutoWindow else None

						if not self.autoInst:
							self.restart = -2
							return

						import chrmgr
						if self.autoInst.bOnOff == True and chrmgr.GetAutoSystemByType(chrmgr.AUTO_ONOFF_RESTART):
							self.restart = int(app.GetTime()) + 11
							return
						else:
							self.restart = -2
					else:
						self.restart = -2
				elif int(app.GetTime()) >= self.restart:
					self.RestartHere()
					self.restart = int(app.GetTime()) + 1
					return

	def RestartHere(self):
		net.SendChatPacket("/restart_here")

	def RestartTown(self):
		net.SendChatPacket("/restart_town")

	def OnPressExitKey(self):
		return True

	def OnPressEscapeKey(self):
		return True
