import os

import app
import net
import wndMgr
import snd
import systemSetting
import ime
import grp

import ui
import uicommon
import uiscriptlocale
import musicinfo
import localeinfo
import serverinfo
import constinfo

class LoginWindow(ui.ScriptWindow):
	channelsNum = 4
	accountsNum = 6
	KEYS_ALLOWED = [app.DIK_F1, app.DIK_F2, app.DIK_F3, app.DIK_F4, app.DIK_F5, app.DIK_F6]

	def __init__(self, stream):
		ui.ScriptWindow.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
		net.SetAccountConnectorHandler(self)
		
		self.stream = stream
		
		self.idEditLine = None
		self.pwdEditLine = None
		self.questionDialog = None
		self.changeQuestionDialog = None
		
		self.loginBtn = None
		
		self.usernameBtn = None
		self.passwordBtn = None
		
		self.extraButtons = []
		self.extraButtonsUrls = []
		
		self.pickedChannel = 1
		self.channelButtons = []
		self.selectedChText = None
		
		self.accountManager = [\
								[None, None, None],
								[None, None, None],
								[None, None, None],
								[None, None, None],
								[None, None, None],
								[None, None, None],
		]
		
		self.accountManagerData = [\
									["", "", 0],
									["", "", 0],
									["", "", 0],
									["", "", 0],
									["", "", 0],
									["", "", 0],
		]
		
		if app.ENABLE_MULTI_LANGUAGE:
			self.langLeft = []
			self.langRight = []
			self.langCenter = []
			self.selectedLanguage = 0
			self.confirmButton = None
			self.leftButton = None
			self.rightButton = None

	def __del__(self):
		net.ClearPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
		net.SetAccountConnectorHandler(0)
		ui.ScriptWindow.__del__(self)

	def Open(self):
		if not os.path.exists("UserData"):
			os.makedirs("UserData")
		
		if not os.path.exists(os.getcwd() + os.sep + "UserData\chatting"):
			os.mkdir(os.getcwd() + os.sep + "UserData\chatting")
		
		self.loginFailureMsgDict = {
			"ALREADY":localeinfo.LOGIN_FAILURE_ALREAY,
			"NOID":localeinfo.LOGIN_FAILURE_NOT_EXIST,
			"WRONGPWD":localeinfo.LOGIN_FAILURE_NOT_EXIST,
			"FULL":localeinfo.LOGIN_FAILURE_TOO_MANY_USER,
			"SHUTDOWN":localeinfo.LOGIN_FAILURE_SHUTDOWN,
			"REPAIR":localeinfo.LOGIN_FAILURE_REPAIR_ID,
			"BLOCK":localeinfo.LOGIN_FAILURE_BLOCK_ID,
			"BESAMEKEY":localeinfo.LOGIN_FAILURE_BE_SAME_KEY,
			"NOTAVAIL":localeinfo.LOGIN_FAILURE_NOT_AVAIL,
			"NOBILL":localeinfo.LOGIN_FAILURE_NOBILL,
			"BLKLOGIN":localeinfo.LOGIN_FAILURE_BLOCK_LOGIN,
			"WEBBLK":localeinfo.LOGIN_FAILURE_WEB_BLOCK,
			"BAN1":localeinfo.LOGIN_FAILURE_BLOCK_ID,
			"BAN2":localeinfo.LOGIN_FAILURE_BLOCK_ID,
			"UNKNOWN":localeinfo.LOGIN_FAILURE_UPDATE,
		}
		
		self.loginFailureFuncDict = {
			"WRONGPWD":self.__DisconnectAndInputPassword,
			"QUIT":app.Exit,
		}
		
		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		self.SetWindowName("LoginWindowUI")
		
		try:
			ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/loginwindow.py")
			
			idEditLine = self.GetChild("UsernameEditLineUI")
			idEditLine.SetReturnEvent(self.__SetFocusPassEditLine)
			idEditLine.SetTabEvent(self.__SetFocusPassEditLine)
			idEditLine.SetEscapeEvent(ui.__mem_func__(self.OnPressEscapeKey))
			self.idEditLine = idEditLine
			
			pwdEditLine = self.GetChild("PasswordEditLineUI")
			pwdEditLine.SetReturnEvent(ui.__mem_func__(self.__OnClickLoginButton))
			pwdEditLine.SetTabEvent(self.__SetFocusIdEditLine)
			pwdEditLine.SetEscapeEvent(ui.__mem_func__(self.OnPressEscapeKey))
			self.pwdEditLine = pwdEditLine
			
			usernameBtn = self.GetChild("UsernameWelcomeUI")
			usernameBtn.SetEvent(ui.__mem_func__(self.__PressWelcome), 0)
			self.usernameBtn = usernameBtn
			
			passwordBtn = self.GetChild("PasswordWelcomeUI")
			passwordBtn.SetEvent(ui.__mem_func__(self.__PressWelcome), 1)
			self.passwordBtn = passwordBtn
			
			loginBtn = self.GetChild("LoginButtonUI")
			loginBtn.SetEvent(ui.__mem_func__(self.__OnClickLoginButton))
			self.loginBtn = loginBtn
			
			self.extraButtons = [
				self.GetChild("WebsiteButtonUI"),
				self.GetChild("DiscordButtonUI"),
				self.GetChild("RegisterTextUI"),
				self.GetChild("ForgotTextUI"),
				self.GetChild("ExitTextUI"),
			]
			
			self.extraButtonUrls = [
				"https://keynes2.online/",
				"https://discord.gg/keynes2",
				"https://keynes2.online/register",
				"https://keynes2.online/forgot",
				"#",
			]
			
			for i, btn in enumerate(self.extraButtons):
				btn.SetOverEvent(ui.__mem_func__(self.OverInExtraButton), i)
				btn.SetOverOutEvent(ui.__mem_func__(self.OverOutExtraButton), i)

				if i == 4:
					btn.SetEvent(ui.__mem_func__(self.__ExitGame))
				else:
					btn.SetEvent(ui.__mem_func__(self.ClickExtraButton), i)

			self.channelButtons = []
			for i in range(1, self.channelsNum + 1):
				btn = self.GetChild("Ch%dButtonUI" % i)
				btn.SetEvent(ui.__mem_func__(self.__OnClickChannel), i)
				btn.SetText(uiscriptlocale.LOGINWINDOW_CHANNEL % i)
				
				self.channelButtons.append(btn)
			
			self.selectedChText = self.GetChild("ChannelTextUI")
			
			for i in range(0, self.accountsNum):
				self.accountManager[i][0] = self.GetChild("Account%dTextUI" % i)
				self.accountManager[i][1] = self.GetChild("Account%dSaveUI" % i)
				self.accountManager[i][2] = self.GetChild("Account%dDeleteUI" % i)

				self.accountManager[i][0].SetText(uiscriptlocale.LOGINWINDOW_SAVE_EMPTY % (i + 1, i + 1))
				self.accountManager[i][1].SetEvent(ui.__mem_func__(self.__OnClickAccountAdd), i)
				self.accountManager[i][2].SetEvent(ui.__mem_func__(self.__OnClickAccountErase), i)
				self.accountManager[i][2].Hide()
			
			if app.ENABLE_MULTI_LANGUAGE:
				self.langLeft = {\
					#"hu" : self.GetChild("hu_left_language_ui"),
					#"pl" : self.GetChild("pl_left_language_ui"),
					# "de" : self.GetChild("de_left_language_ui"),
					"ro" : self.GetChild("ro_left_language_ui"),
					# "tr" : self.GetChild("tr_left_language_ui"),
					"en" : self.GetChild("en_left_language_ui"),
					# "pt" : self.GetChild("pt_left_language_ui"),
					# "it" : self.GetChild("it_left_language_ui"),
				}
				
				self.langRight = {\
					#"hu" : self.GetChild("hu_right_language_ui"),
					#"pl" : self.GetChild("pl_right_language_ui"),
					# "de" : self.GetChild("de_right_language_ui"),
					"ro" : self.GetChild("ro_right_language_ui"),
					# "tr" : self.GetChild("tr_right_language_ui"),
					"en" : self.GetChild("en_right_language_ui"),
					# "pt" : self.GetChild("pt_right_language_ui"),
					# "it" : self.GetChild("it_right_language_ui"),
				}
				
				self.langCenter = {\
					#"hu" : self.GetChild("hu_center_language_ui"),
					#"pl" : self.GetChild("pl_center_language_ui"),
					# "de" : self.GetChild("de_center_language_ui"),
					"ro" : self.GetChild("ro_center_language_ui"),
					# "tr" : self.GetChild("tr_center_language_ui"),
					"en" : self.GetChild("en_center_language_ui"),
					# "pt" : self.GetChild("pt_center_language_ui"),
					# "it" : self.GetChild("it_center_language_ui"),
				}
				
				self.selectedLanguage = 0
				
				confirmButton = self.GetChild("ConfirmButtonUI")
				confirmButton.SetEvent(ui.__mem_func__(self.__OnClickConfirmButton))
				self.confirmButton = confirmButton
				
				leftButton = self.GetChild("ArrowLeftButtonUI")
				leftButton.SetEvent(ui.__mem_func__(self.__OnClickLeftButton))
				self.leftButton = leftButton
				
				rightButton = self.GetChild("ArrowRightButtonUI")
				rightButton.SetEvent(ui.__mem_func__(self.__OnClickRightButton))
				self.rightButton = rightButton
			else:
				self.GetChild("menu_center_bottom2_ui").Hide()
		except:
			import exception
			exception.Abort("LoginWindow.Open")
		
		self.LoadChannelInfo()
		
		if os.path.isfile("UserData/account_data.cfg"):
			self.LoadAccountData()
		
		if app.ENABLE_MULTI_LANGUAGE and\
			len(self.langCenter) > 0:
			self.__OnLoadLanguage(1, 0)
		
		snd.SetSoundVolume(systemSetting.GetSoundVolume())
		if musicinfo.loginMusic != "":
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("bgm/" + musicinfo.loginMusic)
		
		ime.AddExceptKey(91)
		ime.AddExceptKey(93)
		
		self.Show()
		
		connectingIP = self.stream.GetConnectAddr()
		if connectingIP:
			self.__OpenLoginBoard()
		
		app.ShowCursor()

	def Close(self):
		if self.questionDialog:
			self.questionDialog.Close()
			del self.questionDialog
			self.questionDialog = None
		
		if self.changeQuestionDialog:
			self.changeQuestionDialog.Close()
			del self.changeQuestionDialog
			self.changeQuestionDialog = None
		
		self.stream.popupWindow.Close()
		
		self.loginFailureMsgDict = None
		self.loginFailureFuncDict = None
		
		if self.idEditLine != None:
			self.idEditLine.SetTabEvent(0)
			self.idEditLine.SetReturnEvent(0)
			self.idEditLine.SetEscapeEvent(0)
			self.idEditLine = None
		
		if self.idEditLine != None:
			self.pwdEditLine.SetReturnEvent(0)
			self.pwdEditLine.SetTabEvent(0)
			self.pwdEditLine.SetEscapeEvent(0)
			self.pwdEditLine = None
		
		self.loginBtn = None
		
		self.usernameBtn = None
		self.passwordBtn = None
		
		self.extraButtons = []
		self.extraButtonsUrls = []
		
		self.channelButtons = []
		self.selectedChText = None
		
		self.accountManager = [\
								[None, None, None],
								[None, None, None],
								[None, None, None],
								[None, None, None],
								[None, None, None],
								[None, None, None],
		]
		
		self.accountManagerData = [\
									["", "", 0],
									["", "", 0],
									["", "", 0],
									["", "", 0],
									["", "", 0],
									["", "", 0],
		]
		
		self.langLeft = []
		self.langRight = []
		self.langCenter = []
		self.selectedLanguage = 0
		self.confirmButton = None
		self.leftButton = None
		self.rightButton = None
		
		self.KillFocus()
		self.Hide()
		
		ime.ClearExceptKey()
		app.HideCursor()
		
		if musicinfo.loginMusic != "" and\
			musicinfo.selectMusic != "":
			snd.FadeOutMusic("bgm/" + musicinfo.loginMusic)

	def EmptyFunc(self):
		pass

	def __ExitGame(self):
		app.Exit()

	def ClickExtraButton(self, index):
		import os
		os.system("start " + self.extraButtonUrls[index])

	def OverInExtraButton(self, index):
		if index != 2 and\
			index != 3 and\
			index != 4:
			return
		
		self.extraButtons[index].SetTextColor(0xffa40b0b if index == 4 else 0xff1169dd)

	def OverOutExtraButton(self, index):
		if index != 2 and\
			index != 3 and\
			index != 4:
			return

		try:
			self.extraButtons[index].SetTextColor(ui.DEFAULT_TEXT_COLOR)
		except:
			pass

	def SaveChannelInfo(self):
		try:
			file = open("channel.inf", "w")
			file.write("%d %d %d" % (\
									self.__GetServerID(),\
									self.pickedChannel,\
									self.__GetRegionID()))
		except Exception as e:
			import dbg
			dbg.TraceError(str(e))

	def LoadChannelInfo(self):
		try:
			file = open("channel.inf")
			lines = file.readlines()
			if len(lines) > 0:
				tokens = lines[0].split()
				channel = int(tokens[1])
			
			file.close()
			
			self.__OnClickChannel(channel)
		except:
			self.__OnClickChannel(1)

	def SetChannelDownIndex(self, idx):
		self.selectedChText.SetText(uiscriptlocale.LOGINWINDOW_SELECTED_CH % idx)
		self.pickedChannel = idx
		self.SaveChannelInfo()
		self.__OnClickSelectServerButton()

	def __OnClickChannel(self, idx):
		self.SetChannelDownIndex(idx)
		
		j = 1
		for btn in self.channelButtons:
			btn.SetUpVisual("new/login/ch_button_normal.sub")
			btn.SetOverVisual("new/login/ch_button_over.sub")
			btn.SetDownVisual("new/login/ch_button_over.sub")
			j += 1
		
		activeBtn = self.channelButtons[idx - 1]
		if activeBtn is not None:
			activeBtn.SetUpVisual("new/login/ch_button_down.sub")
			activeBtn.SetOverVisual("new/login/ch_button_down.sub")
			activeBtn.SetDownVisual("new/login/ch_button_down.sub")

	def SaveAccountData(self):
		with open("UserData/account_data.cfg", "wb") as content_file:
			data = ""
			for idx, account in enumerate(self.accountManagerData):
				if idx != 9:
					if account[2] != 0:
						data += account[0] + ":#:" + account[1] + ":#:" + str(account[2]) + ";"
					else:
						data += account[0] + ":#:" + account[1] + ";"
				else:
					if account[2] != 0:
						data += account[0] + ":#:" + account[1] + ":#:" + str(account[2])
					else:
						data += account[0] + ":#:" + account[1]
			
			encData = app.EncryptByComputer(data)
			content_file.write(encData)

	def LoadAccountData(self):
		with open("UserData/account_data.cfg", "rb") as content_file:
			encContent = content_file.read()
			try:
				content = app.DecryptByComputer(encContent)
			except WindowsError:
				content = ""
			
			if ";" in content:
				accounts = content.split(";")
				for idx, account in enumerate(accounts):
					if ":#:" in account:
						data = account.split(":#:")
						self.accountManagerData[idx][0] = data[0]
						self.accountManagerData[idx][1] = data[1]
						if len(data) > 2:
							self.accountManagerData[idx][2] = long(data[2])
			
			for idx, account in enumerate(self.accountManagerData):
				if account[0] != "":
					self.accountManager[idx][0].SetText("%d. %s (F%d)" % (idx + 1, account[0].upper(), idx + 1))
					self.accountManager[idx][2].Show()
				else:
					self.accountManager[idx][0].SetText(uiscriptlocale.LOGINWINDOW_SAVE_EMPTY % (idx + 1, idx + 1))
					self.accountManager[idx][2].Hide()

	def __OnClickAccountErase(self, idx):
		self.accountManagerData[idx][0] = ""
		self.accountManagerData[idx][1] = ""
		
		self.accountManager[idx][0].SetText(uiscriptlocale.LOGINWINDOW_SAVE_EMPTY % (idx + 1, idx + 1))
		self.accountManager[idx][2].Hide()
		
		self.SaveAccountData()

	def __OnClickAccountAdd(self, idx):
		if self.accountManagerData[idx][0] == "":
			id = self.idEditLine.GetText()
			pwd = self.pwdEditLine.GetText()
			if id == "" or\
				pwd == "":
				return
			
			self.accountManagerData[idx][0] = id
			self.accountManagerData[idx][1] = pwd
			
			self.SaveAccountData()
			
			self.accountManager[idx][0].SetText("%d. %s (F%d)" % (idx + 1, id.upper(), idx + 1))
			self.accountManager[idx][2].Show()
		else:
			self.selectedAccountIndex = idx
			
			if self.usernameBtn.IsShow():
				self.usernameBtn.Hide()
			
			if self.passwordBtn.IsShow():
				self.passwordBtn.Hide()
			
			self.idEditLine.SetText(self.accountManagerData[idx][0])
			self.pwdEditLine.SetText(self.accountManagerData[idx][1])
			
			self.__SetFocusPassEditLine()
			
			self.__OnClickLoginButton()

	if app.ENABLE_MULTI_LANGUAGE:
		def __OnLoadLanguage(self, bLoad, bRight):
			for k, v in self.langLeft.iteritems():
				v.Hide()
			
			for k, v in self.langRight.iteritems():
				v.Hide()
			
			for k, v in self.langCenter.iteritems():
				v.Hide()
			
			langOrderDict = {
				#0 : "hu",
				#1 : "pl",
				# 0 : "de",
				0 : "ro",
				1 : "en",
				# 3 : "it",
				# 4 : "tr",
				# 5 : "pt",
			}
			
			if bLoad == 1:
				localeName = app.GetLocaleName()
			else:
				if bRight == 1:
					try:
						localeName = langOrderDict[self.selectedLanguage + 1]
					except:
						localeName = langOrderDict[0]
				else:
					try:
						localeName = langOrderDict[self.selectedLanguage - 1]
					except:
						localeName = langOrderDict[1] #7
			
			n = 0
			
			for k, v in langOrderDict.iteritems():
				if v == localeName:
					n = k
					break
			
			try:
				leftN = langOrderDict[n - 1]
			except:
				leftN = langOrderDict[1] #7
			
			try:
				rightN = langOrderDict[n + 1]
			except:
				rightN = langOrderDict[0]
			
			self.langLeft[leftN].Show()
			self.langRight[rightN].Show()
			self.langCenter[localeName].Show()
			
			self.selectedLanguage = n

		def __GetCodePage(self, lang):
			codePageDict = {
				"cz" : 1250,
				"de" : 1252,
				"en" : 1252,
				"es" : 1252,
				"fr" : 1252,
				"it" : 1252,
				"pl" : 1250,
				"pt" : 1252,
				"ro" : 1250,
				"tr" : 1254,
			}
			
			try:
				return codePageDict[lang]
			except:
				return 1252

		def __CloseQuestionDialog(self):
			if not self.questionDialog:
				return
			
			self.questionDialog.Close()
			del self.questionDialog
			self.questionDialog = None

		def __OnQuestionDialogCancel(self):
			self.__OnLoadLanguage(1, 0)
			self.__CloseQuestionDialog()

		def __OnQuestionDialogAccept(self):
			self.__CloseQuestionDialog()
			
			langOrderDict = {
				#0 : "hu",
				#1 : "pl",
				# 0 : "de",
				0 : "ro",
				1 : "en",
				# 3 : "it",
				# 4 : "tr",
				# 5 : "pt",
			}
			
			lang = langOrderDict[self.selectedLanguage]
			
			if app.GetLocaleName() != lang:
				with open("locale.cfg", "w+") as localeConfig:
					localeConfig.write("10022 %d %s" % (self.__GetCodePage(lang), lang))
				
				localeConfig.close()
				
				app.ShellExecute()
				self.__ExitGame()

		def __OnClickConfirmButton(self):
			self.__CloseQuestionDialog()
			
			langOrderDict = {
				#0 : "hu",
				#1 : "pl",
				# 0 : "de",
				0 : "ro",
				1 : "en",
				# 3 : "it",
				# 4 : "tr",
				# 5 : "pt",
			}
			
			lang = langOrderDict[self.selectedLanguage]
			
			if app.GetLocaleName() == lang:
				self.PopupNotifyMessage(uiscriptlocale.LOGINWINDOW_SAME_LANGUAGE)
				return
			
			self.stream.popupWindow.Close()
			
			self.questionDialog = uicommon.QuestionDialog()
			self.questionDialog.SetText(uiscriptlocale.LOGINWINDOW_CHANGE_LANGUAGE)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__OnQuestionDialogAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__CloseQuestionDialog))
			self.questionDialog.Open()

		def __OnClickLeftButton(self):
			self.__CloseQuestionDialog()
			self.__OnLoadLanguage(0, 0)

		def __OnClickRightButton(self):
			self.__CloseQuestionDialog()
			self.__OnLoadLanguage(0, 1)

	def PopupDisplayMessage(self, msg):
		if app.ENABLE_MULTI_LANGUAGE:
			self.OnCloseQuestionDialog()
		
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, 0, localeinfo.UI_CANCEL)

	def PopupNotifyMessage(self, msg, func = 0):
		if not func:
			func = self.EmptyFunc
		
		if app.ENABLE_MULTI_LANGUAGE:
			self.OnCloseQuestionDialog()
		
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, func, localeinfo.UI_OK)

	def __OpenLoginBoard(self):
		if self.idEditLine != None:
			self.idEditLine.SetText("")
			self.idEditLine.KillFocus()
		
		if self.pwdEditLine != None:
			self.pwdEditLine.SetText("")
			self.idEditLine.KillFocus()

	def __SetFocusIdEditLine(self):
		if self.idEditLine != None:
			if self.usernameBtn.IsShow():
				self.usernameBtn.Hide()
			
			self.idEditLine.KillFocus()
			self.idEditLine.SetFocus()

	def __SetFocusPassEditLine(self):
		if self.pwdEditLine != None:
			if self.passwordBtn.IsShow():
				self.passwordBtn.Hide()
			
			self.pwdEditLine.SetFocus()

	def __SetFocusPassEditLine2(self):
		if self.pwdEditLine != None:
			if self.passwordBtn.IsShow():
				self.passwordBtn.Hide()
			
			self.pwdEditLine.KillFocus()
			self.pwdEditLine.SetFocus()

	def __PressWelcome(self, isPassword):
		if isPassword == 1:
			self.passwordBtn.Hide()
			self.__SetFocusPassEditLine()
		else:
			self.usernameBtn.Hide()
			self.__SetFocusIdEditLine()

	def __Connect(self, id, pwd):
		if constinfo.SEQUENCE_PACKET_ENABLE:
			net.SetPacketSequenceMode()
		
		if app.ENABLE_MULTI_LANGUAGE:
			self.OnCloseQuestionDialog()
		
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(localeinfo.LOGIN_CONNETING, self.__SetFocusPassEditLine2, localeinfo.UI_CANCEL)
		
		constinfo.ACCOUNT_NAME = str(id)
		try:
			self.stream.SetLoginInfo(id, pwd)
		except:
			self.stream.SetLoginInfo(id, pwd, 1234)
		
		self.stream.Connect()

	def __OnClickLoginButton(self):
		id = self.idEditLine.GetText()
		pwd = self.pwdEditLine.GetText()
		
		if len(self.idEditLine.GetText()) == 0:
			self.PopupNotifyMessage(localeinfo.LOGIN_INPUT_ID, self.__SetFocusIdEditLine)
			return
		
		if len(self.pwdEditLine.GetText()) == 0:
			self.PopupNotifyMessage(localeinfo.LOGIN_INPUT_PASSWORD, self.__SetFocusPassEditLine2)
			return
		
		self.__Connect(id, pwd)

	def __SetServerInfo(self, name):
		net.SetServerInfo(name.strip())

	def __GetRegionID(self):
		return 0

	def __GetServerID(self):
		return 1

	def __OnClickSelectServerButton(self):
		regionID = self.__GetRegionID()
		serverID = self.__GetServerID()
		
		if not serverinfo.REGION_DICT.has_key(regionID):
			self.PopupNotifyMessage(localeinfo.CHANNEL_SELECT_REGION)
			return
		elif not serverinfo.REGION_DICT[regionID].has_key(serverID):
			self.PopupNotifyMessage(localeinfo.CHANNEL_SELECT_SERVER)
			return
		
		try:
			channelDict = serverinfo.REGION_DICT[regionID][serverID]["channel"]
		except Exception as e:
			import dbg
			dbg.TraceError(str(e))
			return
		
		try:
			state = channelDict[self.pickedChannel]["state"]
		except KeyError:
			self.PopupNotifyMessage(localeinfo.CHANNEL_SELECT_CHANNEL)
			return
		
		self.SaveChannelInfo()
		
		try:
			serverName = serverinfo.REGION_DICT[regionID][serverID]["name"]
			channelName = serverinfo.REGION_DICT[regionID][serverID]["channel"][self.pickedChannel]["name"]
			addrKey = serverinfo.REGION_DICT[regionID][serverID]["channel"][self.pickedChannel]["key"]
		except:
			serverName = localeinfo.CHANNEL_EMPTY_SERVER
			channelName = localeinfo.CHANNEL_NORMAL % self.pickedChannel
		
		self.__SetServerInfo("%s, %s " % (serverName, channelName))
		
		try:
			ip = serverinfo.REGION_DICT[regionID][serverID]["channel"][self.pickedChannel]["ip"]
			tcp_port = serverinfo.REGION_DICT[regionID][serverID]["channel"][self.pickedChannel]["tcp_port"]
		except Exception as e:
			import dbg
			dbg.TraceError(str(e))
			return
		
		try:
			account_ip = serverinfo.REGION_AUTH_SERVER_DICT[regionID][self.pickedChannel]["ip"]
			account_port = serverinfo.REGION_AUTH_SERVER_DICT[regionID][self.pickedChannel]["port"]
		except:
			account_ip = 0
			account_port = 0

		try:
			markKey = regionID * 1000 + serverID * 10
			markAddrValue = serverinfo.MARKADDR_DICT[markKey]
			
			net.SetMarkServer(markAddrValue["ip"], markAddrValue["tcp_port"][1])
			app.SetGuildMarkPath(markAddrValue["mark"])
			app.SetGuildSymbolPath(markAddrValue["symbol_path"])
		except Exception as e:
			import dbg
			dbg.TraceError(str(e))
			return
		
		self.stream.SetConnectInfo(ip, tcp_port, account_ip, account_port)

	def __DisconnectAndInputPassword(self):
		self.__SetFocusPassEditLine2()
		net.Disconnect()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return
		
		self.questionDialog.Close()
		del self.questionDialog
		self.questionDialog = None

	def RestartByPatch(self):
		self.OnCloseQuestionDialog()
		app.ShellExecute("autopatcher.exe")
		app.Exit()

	def NotifyChannelState(self, addrKey, state):
		#StateChecker unused
		#try:
		#	stateName = serverinfo.STATE_DICT[state]
		#except:
		#	stateName = serverinfo.STATE_NONE
		#
		#regionID = int(addrKey / 1000)
		#serverID = int(addrKey / 10) % 100
		#channelID = addrKey % 10
		pass

	def OnConnectFailure(self):
		snd.PlaySound("sound/ui/loginfail.wav")
		self.PopupNotifyMessage(localeinfo.LOGIN_CONNECT_FAILURE, self.__SetFocusPassEditLine2)

	def OnEndCountDown(self):
		self.OnConnectFailure()

	def OnHandShake(self):
		snd.PlaySound("sound/ui/loginok.wav")
		self.PopupDisplayMessage(localeinfo.LOGIN_CONNECT_SUCCESS)

	def OnLoginStart(self):
		self.PopupDisplayMessage(localeinfo.LOGIN_PROCESSING)

	def OnLoginFailure(self, error):
		try:
			loginFailureMsg = self.loginFailureMsgDict[error]
		except KeyError:
			loginFailureMsg = localeinfo.LOGIN_FAILURE_UNKNOWN  + error
		
		if self.loginFailureFuncDict != None:
			loginFailureFunc = self.loginFailureFuncDict.get(error, self.__SetFocusPassEditLine2)
			self.PopupNotifyMessage(loginFailureMsg, loginFailureFunc)
			snd.PlaySound("sound/ui/loginfail.wav")

	def OnUpdate(self):
		if constinfo.restart == 1:
			app.ShellExecute()
			app.Exit()

	def OnKeyDown(self, key):
		if key in self.KEYS_ALLOWED:
			self.__OnClickAccountAdd(key - app.DIK_F1)
		
		return True

	def OnPressEscapeKey(self):
		self.stream.popupWindow.Close()
		self.stream.SetPhaseWindow(0)
		return True

	def OnPressExitKey(self):
		self.stream.popupWindow.Close()
		self.stream.SetPhaseWindow(0)
		return True
