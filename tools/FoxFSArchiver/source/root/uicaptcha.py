import app
import net
import ui
import uitooltip
import grp
import chat
import uicommon
import time
import localeinfo
import newconstinfo

class CaptchaDialog(ui.ScriptWindow):
	CAPTCHA_TIME = 0.0
	TOOLTIP_COLOR = grp.GenerateColor((255.00 / 255), (255.00 / 255), (77.00 / 255), 1.0)
	captchaImg = 0
	bHided = 0

	def __init__(self):
		ui.ScriptWindow.__init__(self, "TOP_MOST")
		
		self.interfaceInst = newconstinfo.GetInterfaceInstance()
		
		self.isLoaded = False
		self.captchaTime = None
		self.toolTip = None
		self.popUp = None

	def __del__(self):
		self.interfaceInst = None
		
		if self.captchaImg != 0:
			del self.captchaImg
			self.captchaImg = 0
		
		ui.ScriptWindow.__del__(self)
		self.isLoaded = False
		self.captchaTime = None
		self.toolTip = None
		self.popUp = None

	def LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/captchadialog.py")
		except:
			import exception
			exception.Abort("CaptchaDialog.LoadDialog.LoadScript")
		
		try:
			self.thinboard = self.GetChild("thin_board")
			self.board = self.GetChild("board")
			self.titleBar = self.GetChild("title_bar")
			self.titleName = self.GetChild("title_name")
			
			self.randomCaptchaInfo = self.GetChild("random_captcha_info")
			
			self.captchaTimeImage = self.GetChild("captcha_time_image")
			self.captchaTimeName = self.GetChild("captcha_time_name")
			self.captchaValue = self.GetChild("captcha_value")
			
			self.acceptButton = self.GetChild("accept_button")
			self.cancelButton = self.GetChild("cancel_button")
		except:
			import exception
			exception.Abort("CaptchaDialog.LoadDialog.BindObject")

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.__OnCancelButton))
		self.acceptButton.SetEvent(ui.__mem_func__(self.__OnAcceptButton))
		self.cancelButton.SetEvent(ui.__mem_func__(self.__OnCancelButton))
		
		self.toolTip = uitooltip.ToolTip()
		self.popUp = uicommon.PopupDialog(1)
		
		self.startTime = float(app.GetTime())
		if self.captchaTime is None:
			self.captchaTime = app.GetGlobalTimeStamp() + int(self.CAPTCHA_TIME)
		
		self.captchaValue.SetFocus()
		
		self.isLoaded = True

	def Destroy(self):
		self.ClearDictionary()
		self.Close()

	def Open(self, code, limit, filename):
		if code == 0 and limit == 0:
			if self.interfaceInst:
				self.interfaceInst.ShowDefaultWindows()
			
			self.captchaValue.KillFocus()
			self.Close()
		elif code == 0 and limit == 1:
			if self.popUp:
				self.popUp.Close()
			
			self.captchaValue.SetText("")
			self.popUp.Open()
			self.popUp.SetTop()
			self.popUp.SetText(localeinfo.CAPTCHA_WRONG)
			return 0
		else:
			self.CAPTCHA_TIME = limit
			
			if not self.isLoaded:
				self.LoadDialog()
			
			if self.captchaImg != 0:
				del self.captchaImg
				self.captchaImg = 0
			
			self.captchaImg = ui.ExpandedImageBox()
			self.captchaImg.SetParent(self.GetChild("captcha_bg_ui"))
			self.captchaImg.LoadImage("UserData/{}.bmp".format(filename))
			self.captchaImg.SetPosition(1, 1)
			self.captchaImg.Show()
			
			import os
			os.remove("UserData/{}.bmp".format(filename))
			
			self.bHided = 0
			
			self.SetCenterPosition()
			self.SetTop()
			self.Show()

	def Close(self):
		if self.captchaImg != 0:
			del self.captchaImg
			self.captchaImg = 0

		if self.toolTip:
			self.toolTip.HideToolTip()

		if self.popUp:
			self.popUp.Close()

		self.captchaTime = None
		self.isLoaded = False
		self.Hide()

	def CheckCaptcha(self):
		if self.toolTip:
			self.toolTip.HideToolTip()
		
		if self.popUp:
			self.popUp.Close()
		
		if len(self.captchaValue.GetText()) <= 0:
			self.popUp.Open()
			self.popUp.SetText(localeinfo.CAPTCHA_EMPTY)
			return 0
		
		return 1

	def __OnAcceptButton(self):
		if self.CheckCaptcha() == 1:
			net.ajf35iuhfs55539141(int(self.captchaValue.GetText()))
			self.captchaValue.SetText("")

	def __OnCancelButton(self):
		if len(self.captchaValue.GetText()) > 0:
			self.captchaValue.SetText("")
		else:
			self.CheckCaptcha()

	def OnUpdate(self):
		if self.IsShow():
			if self.captchaTime is None:
				self.captchaTime = app.GetGlobalTimeStamp() + int(self.CAPTCHA_TIME)
			
			leftTime = max(0, self.captchaTime - app.GetGlobalTimeStamp())
			leftMin, leftSec = divmod(leftTime, 30)
			
			if leftTime > 0:
				if leftSec <= 5:
					self.captchaTimeName.SetFontColor((255.00 / 255), (0.00 / 255), (0.00 / 255))
				
				self.captchaTimeName.SetText("%02d:%02d" % (leftMin, leftSec))
				
				if not self.bHided:
					if self.interfaceInst:
						self.interfaceInst.HideAllWindows()
					
					self.bHided = 1
			else:
				leftTime = None
				self.captchaTimeName.SetText("00:00")
			
			if self.toolTip:
				if self.randomCaptchaInfo.IsIn():
					self.toolTip.ClearToolTip()
					self.toolTip.AutoAppendTextLine(localeinfo.CAPTCHA_TOOLTIP % (leftSec), self.TOOLTIP_COLOR)
					self.toolTip.AlignHorizonalCenter()
					self.toolTip.ShowToolTip()
				else:
					self.toolTip.HideToolTip()

