import app
import chr
import player
import net

import ui
import chat
import ime
import time
import os
import wndMgr
import localeinfo
if app.ENABLE_WHISPER_MESSENGER_BUTTONS:
	import messenger

class WhisperButton(ui.Button):
	def __init__(self):
		ui.Button.__init__(self, "TOP_MOST")

	def __del__(self):
		ui.Button.__del__(self)

	def SetToolTipText(self, text, x=0, y = 32):
		ui.Button.SetToolTipText(self, text, x, y)
		self.ToolTipText.Show()

	def SetToolTipTextWithColor(self, text, color, x=0, y = 32):
		ui.Button.SetToolTipText(self, text, x, y)
		self.ToolTipText.SetPackedFontColor(color)
		self.ToolTipText.Show()

	def ShowToolTip(self):
		if 0 != self.ToolTipText:
			self.ToolTipText.Show()

	def HideToolTip(self):
		if 0 != self.ToolTipText:
			self.ToolTipText.Show()

class WhisperDialog(ui.ScriptWindow):
	EMPIRE_FLAG_NAME = {
		net.EMPIRE_A : "shinsoo",
		net.EMPIRE_B : "chunjo",
		net.EMPIRE_C : "jinno"
	}

	WINDOW_WIDTH = 330

	class TextRenderer(ui.Window):
		def SetTargetName(self, targetName):
			self.targetName = targetName

		def OnRender(self):
			(x, y) = self.GetGlobalPosition()
			chat.RenderWhisper(self.targetName, x, y)

	class ResizeButton(ui.DragButton):

		def __init__(self):
			ui.DragButton.__init__(self)

		def __del__(self):
			ui.DragButton.__del__(self)

		def OnMouseOverIn(self):
			app.SetCursor(app.HVSIZE)

		def OnMouseOverOut(self):
			app.SetCursor(app.NORMAL)

	if app.ENABLE_WHISPER_MESSENGER_BUTTONS:
		WHISPER_MESSENGER_BUTTON_X = 166 + 24
		WHISPER_MESSENGER_BUTTON_Y = 10
		WHISPER_MESSENGER_BUTTON_X_GAP = 25

	def __init__(self, eventMinimize, eventClose):
		print "NEW WHISPER DIALOG  ----------------------------------------------------------------------------"
		ui.ScriptWindow.__init__(self)
		self.targetName = ""
		self.eventMinimize = eventMinimize
		self.eventClose = eventClose
		self.eventAcceptTarget = None
		self.scrollBar = None

	def __del__(self):
		print "---------------------------------------------------------------------------- DELETE WHISPER DIALOG"
		ui.ScriptWindow.__del__(self)

	def LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/whisperdialog.py")
		except:
			import exception
			exception.Abort("WhisperDialog.LoadDialog.LoadScript")
		
		try:
			GetObject=self.GetChild
			self.titleName = GetObject("titlename")
			self.titleNameEdit = GetObject("titlename_edit")
			self.closeButton = GetObject("closebutton")
			self.scrollBar = GetObject("scrollbar")
			self.chatLine = GetObject("chatline")
			self.minimizeButton = GetObject("minimizebutton")
			self.ignoreButton = GetObject("ignorebutton")
			self.reportViolentWhisperButton = GetObject("reportviolentwhisperbutton")
			self.acceptButton = GetObject("acceptbutton")
			self.sendButton = GetObject("sendbutton")
			self.board = GetObject("board")
			self.editBar = GetObject("editbar")
		except:
			import exception
			exception.Abort("DialogWindow.LoadDialog.BindObject")

		self.titleName.SetText("")
		self.titleNameEdit.SetText("")
		self.minimizeButton.SetEvent(ui.__mem_func__(self.Minimize))
		self.closeButton.SetEvent(ui.__mem_func__(self.Close))
		self.scrollBar.SetPos(1.0)
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		self.chatLine.SetReturnEvent(ui.__mem_func__(self.SendWhisper))
		self.chatLine.SetEscapeEvent(ui.__mem_func__(self.Minimize))
		self.chatLine.SetMultiLine()
		self.sendButton.SetEvent(ui.__mem_func__(self.SendWhisper))
		self.titleNameEdit.SetReturnEvent(ui.__mem_func__(self.AcceptTarget))
		self.titleNameEdit.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.ignoreButton.SetToggleDownEvent(ui.__mem_func__(self.IgnoreTarget))
		self.ignoreButton.SetToggleUpEvent(ui.__mem_func__(self.IgnoreTarget))
		self.reportViolentWhisperButton.SetEvent(ui.__mem_func__(self.ReportViolentWhisper))
		self.acceptButton.SetEvent(ui.__mem_func__(self.AcceptTarget))

		self.textRenderer = self.TextRenderer()
		self.textRenderer.SetParent(self)
		self.textRenderer.SetPosition(20, 28)
		self.textRenderer.SetTargetName("")
		self.textRenderer.Show()

		self.resizeButton = self.ResizeButton()
		self.resizeButton.SetParent(self)
		self.resizeButton.SetSize(20, 20)
		self.resizeButton.SetPosition(self.WINDOW_WIDTH, 180)
		self.resizeButton.SetMoveEvent(ui.__mem_func__(self.ResizeWhisperDialog))
		self.resizeButton.Show()

		self.countryFlag = ui.ExpandedImageBox()
		self.countryFlag.SetParent(self.board)
		self.countryFlag.SetPosition(146, 12)
		self.countryFlag.Show()

		self.empireFlag = ui.ExpandedImageBox()
		self.empireFlag.SetParent(self.board)
		self.empireFlag.SetPosition(168, 11)
		self.empireFlag.Show()

		if app.ENABLE_WHISPER_MESSENGER_BUTTONS:
			self.messengerAddFriendButton = ui.Button()
			self.messengerAddFriendButton.SetParent(self.board)
			self.messengerAddFriendButton.SetUpVisual("d:/ymir work/ui/game/windows/messenger_add_friend_01.sub")
			self.messengerAddFriendButton.SetOverVisual("d:/ymir work/ui/game/windows/messenger_add_friend_02.sub")
			self.messengerAddFriendButton.SetDownVisual("d:/ymir work/ui/game/windows/messenger_add_friend_03.sub")
			self.messengerAddFriendButton.SetDisableVisual("d:/ymir work/ui/game/windows/messenger_add_friend_04.sub")
			if app.ENABLE_MESSENGER_BLOCK:
				self.messengerAddFriendButton.SetPosition(self.WHISPER_MESSENGER_BUTTON_X + self.WHISPER_MESSENGER_BUTTON_X_GAP, self.WHISPER_MESSENGER_BUTTON_Y)
			else:
				self.messengerAddFriendButton.SetPosition(self.WHISPER_MESSENGER_BUTTON_X, self.WHISPER_MESSENGER_BUTTON_Y)
			
			self.messengerAddFriendButton.SetEvent(ui.__mem_func__(self.OnMessengerAddFriendButton))
			self.messengerAddFriendButton.Show()
			
			if app.ENABLE_MESSENGER_BLOCK:
				self.messengerBlockButton = ui.Button()
				self.messengerBlockButton.SetParent(self.board)
				self.messengerBlockButton.SetUpVisual("d:/ymir work/ui/game/windows/messenger_block_01.sub")
				self.messengerBlockButton.SetOverVisual("d:/ymir work/ui/game/windows/messenger_block_02.sub")
				self.messengerBlockButton.SetDownVisual("d:/ymir work/ui/game/windows/messenger_block_03.sub")
				self.messengerBlockButton.SetDisableVisual("d:/ymir work/ui/game/windows/messenger_block_04.sub")
				self.messengerBlockButton.SetPosition(self.WHISPER_MESSENGER_BUTTON_X, self.WHISPER_MESSENGER_BUTTON_Y)
				self.messengerBlockButton.SetEvent(ui.__mem_func__(self.OnMessengerBlockButton))
				self.messengerBlockButton.Show()
				
				self.messengerUnblockButton = ui.Button()
				self.messengerUnblockButton.SetParent(self.board)
				self.messengerUnblockButton.SetUpVisual("d:/ymir work/ui/game/windows/messenger_delete_01.sub")
				self.messengerUnblockButton.SetOverVisual("d:/ymir work/ui/game/windows/messenger_delete_02.sub")
				self.messengerUnblockButton.SetDownVisual("d:/ymir work/ui/game/windows/messenger_delete_03.sub")
				self.messengerUnblockButton.SetDisableVisual("d:/ymir work/ui/game/windows/messenger_delete_04.sub")
				self.messengerUnblockButton.SetPosition(self.WHISPER_MESSENGER_BUTTON_X, self.WHISPER_MESSENGER_BUTTON_Y)
				self.messengerUnblockButton.SetEvent(ui.__mem_func__(self.OnMessengerUnblockButton))
				self.messengerUnblockButton.Hide()
		
		self.ResizeWhisperDialog()

	def Destroy(self):

		self.eventMinimize = None
		self.eventClose = None
		self.eventAcceptTarget = None

		self.ClearDictionary()
		self.scrollBar.Destroy()
		self.titleName = None
		self.titleNameEdit = None
		self.closeButton = None
		self.scrollBar = None
		self.chatLine = None
		self.sendButton = None
		self.ignoreButton = None
		self.reportViolentWhisperButton = None
		self.acceptButton = None
		self.minimizeButton = None
		self.textRenderer = None
		self.board = None
		self.editBar = None
		self.resizeButton = None
		self.countryFlag = None
		self.empireFlag = None
		if app.ENABLE_WHISPER_MESSENGER_BUTTONS:
			self.messengerAddFriendButton = None
			if app.ENABLE_MESSENGER_BLOCK:
				self.messengerBlockButton = None
				self.messengerUnblockButton = None

	def SetWhisperDialogSize(self, width, height):
		try:

			max = int((width-90)/6) * 3 - 6

			self.board.SetSize(width, height)
			self.scrollBar.SetPosition(width-25, 35)
			self.scrollBar.SetScrollBarSize(height-100)
			self.scrollBar.SetPos(1.0)
			self.editBar.SetSize(width-18, 50)
			self.chatLine.SetSize(width-90, 40)
			self.chatLine.SetLimitWidth(width-90)
			self.SetSize(width, height)

			if 0 != self.targetName:
				chat.SetWhisperBoxSize(self.targetName, width - 50, height - 90)

			if localeinfo.IsARABIC():
				self.textRenderer.SetPosition(width-20, 28)
				self.scrollBar.SetPosition(width-25+self.scrollBar.GetWidth(), 35)
				self.editBar.SetPosition(10 + self.editBar.GetWidth(), height-60)
				self.sendButton.SetPosition(width - 80 + self.sendButton.GetWidth(), 10)
				self.minimizeButton.SetPosition(width-42 + self.minimizeButton.GetWidth(), 12)
				self.closeButton.SetPosition(width-24+self.closeButton.GetWidth(), 12)
				self.chatLine.SetPosition(5 + self.chatLine.GetWidth(), 5)
				self.board.SetPosition(self.board.GetWidth(), 0)
			else:
				self.textRenderer.SetPosition(20, 28)
				self.scrollBar.SetPosition(width-25, 35)
				self.editBar.SetPosition(10, height-60)
				self.sendButton.SetPosition(width-80, 10)
				self.minimizeButton.SetPosition(width-42, 12)
				self.closeButton.SetPosition(width-24, 12)

			self.SetChatLineMax(max)

		except:
			import exception
			exception.Abort("WhisperDialog.SetWhisperDialogSize.BindObject")

	def SetChatLineMax(self, max):
		self.chatLine.SetMax(max)

		from grpText import GetSplitingTextLine

		text = self.chatLine.GetText()
		if text:
			self.chatLine.SetText(GetSplitingTextLine(text, max, 0))

	def ResizeWhisperDialog(self):
		(xPos, yPos) = self.resizeButton.GetLocalPosition()
		if xPos < self.WINDOW_WIDTH:
			self.resizeButton.SetPosition(self.WINDOW_WIDTH, yPos)
			return
		
		if yPos < 150:
			self.resizeButton.SetPosition(xPos, 150)
			return
		
		self.SetWhisperDialogSize(xPos + 20, yPos + 20)

	def OpenWithTarget(self, targetName):
		chat.CreateWhisper(targetName)
		chat.SetWhisperBoxSize(targetName, self.GetWidth() - 60, self.GetHeight() - 90)
		self.chatLine.SetFocus()
		self.titleName.SetText(targetName)
		self.targetName = targetName
		self.textRenderer.SetTargetName(targetName)
		self.titleNameEdit.Hide()
		self.ignoreButton.Hide()
		if app.IsDevStage():
			self.reportViolentWhisperButton.Show()
		else:
			self.reportViolentWhisperButton.Hide()
		self.acceptButton.Hide()
		self.minimizeButton.Show()
		self.countryFlag.Hide()
		self.empireFlag.Hide()
		if app.ENABLE_WHISPER_MESSENGER_BUTTONS:
			self.RefreshMessengerButtons()
		net.SendReqWhisperInfo(targetName)

	def OpenWithoutTarget(self, event):
		self.eventAcceptTarget = event
		self.titleName.SetText("")
		self.titleNameEdit.SetText("")
		self.titleNameEdit.SetFocus()
		self.targetName = 0
		self.titleNameEdit.Show()
		self.ignoreButton.Hide()
		self.reportViolentWhisperButton.Hide()
		self.acceptButton.Show()
		self.minimizeButton.Hide()
		self.countryFlag.Hide()
		self.empireFlag.Hide()
		if app.ENABLE_WHISPER_MESSENGER_BUTTONS:
			self.messengerAddFriendButton.Hide()
			if app.ENABLE_MESSENGER_BLOCK:
				self.messengerBlockButton.Hide()
				self.messengerUnblockButton.Hide()

	def SetGameMasterLook(self):
		self.reportViolentWhisperButton.Hide()

	def Minimize(self):
		self.titleNameEdit.KillFocus()
		self.chatLine.KillFocus()
		self.Hide()

		if None != self.eventMinimize:
			self.eventMinimize(self.targetName)

	def Close(self):
		chat.ClearWhisper(self.targetName)
		self.titleNameEdit.KillFocus()
		self.chatLine.KillFocus()
		self.Hide()

		if None != self.eventClose:
			self.eventClose(self.targetName)

	def ReportViolentWhisper(self):
		net.SendChatPacket("/reportviolentwhisper " + self.targetName)

	def IgnoreTarget(self):
		net.SendChatPacket("/ignore " + self.targetName)

	def AcceptTarget(self):
		name = self.titleNameEdit.GetText()
		if len(name) <= 0:
			self.Close()
			return

		if None != self.eventAcceptTarget:
			self.titleNameEdit.KillFocus()
			self.eventAcceptTarget(name)

			if app.ENABLE_WHISPER_MESSENGER_BUTTONS:
				if name != player.GetName():
					self.messengerAddFriendButton.Show()
					if app.ENABLE_MESSENGER_BLOCK:
						if messenger.IsBlockByName(self.targetName):
							self.messengerUnblockButton.Show()
						else:
							self.messengerBlockButton.Show()

	def OnScroll(self):
		chat.SetWhisperPosition(self.targetName, self.scrollBar.GetPos())

	if app.LINK_IN_CHAT:
		def GetLink(self, text):
			link = ""
			start = text.find("http://")
			if start == -1:
				start = text.find("https://")
			
			if start == -1:
				return ""
			
			return text[start:len(text)].split(" ")[0]

	def SendWhisper(self):
		import time
		text = self.chatLine.GetText()
		textLength = len(text)
		if textLength > 0:
			if app.LINK_IN_CHAT:
				import constinfo
				link = constinfo.GetLink(text)
				if link != "":
					text = text.replace(link, "|cFF00C0FC|h|Hsysweb:" + link.replace("://", "XxX") + "|h" + link + "|h|r")
					
			if net.IsInsultIn(text):
				chat.AppendChat(chat.CHAT_TYPE_INFO, locale.CHAT_INSULT_STRING)
				return
			VectorsOra = time.strftime("%H:%M:%S")
			net.SendWhisperPacket(self.targetName, "[Ora " + VectorsOra + " Lv " + str(player.GetStatus(player.LEVEL)) + "]:" + text)
			self.chatLine.SetText("")
			chat.AppendWhisper(chat.WHISPER_TYPE_CHAT, self.targetName, player.GetName() + "[Ora " + VectorsOra + " Lv " + str(player.GetStatus(player.LEVEL)) + "]:" + text)

	def OnTop(self):
		self.chatLine.SetFocus()

	def BindInterface(self, interface):
		self.interface = interface

	def OnMouseLeftButtonDown(self):
		hyperlink = ui.GetHyperlink()
		if hyperlink:
			if app.IsPressed(app.DIK_LALT):
				link = chat.GetLinkFromHyperlink(hyperlink)
				ime.PasteString(link)
			else:
				self.interface.MakeHyperlinkTooltip(hyperlink)

	if app.ENABLE_WHISPER_MESSENGER_BUTTONS:
		def OnMessengerAddFriendButton(self):
			if self.targetName:
				net.SendMessengerAddByNamePacket(self.targetName)

		if app.ENABLE_MESSENGER_BLOCK:
			def OnMessengerBlockButton(self):
				if self.targetName:
					net.SendMessengerAddBlockByNamePacket(self.targetName)

			def OnMessengerUnblockButton(self):
				if messenger.IsBlockByName(self.targetName):
					messenger.RemoveBlock(self.targetName)
					net.SendMessengerRemoveBlockPacket(self.targetName, self.targetName)

		def RefreshMessengerButtons(self):
			if self.targetName:
				if messenger.IsFriendByName(self.targetName):
					self.messengerAddFriendButton.Disable()
				else:
					self.messengerAddFriendButton.Enable()
				
				if app.ENABLE_MESSENGER_BLOCK:
					if messenger.IsBlockByName(self.targetName):
						self.messengerBlockButton.Hide()
						self.messengerUnblockButton.Show()
					else:
						self.messengerBlockButton.Show()
						self.messengerUnblockButton.Hide()

	def OnRunMouseWheel(self, nLen):
		if self.scrollBar != None and  self.scrollBar.IsShow():
			x, y = self.GetGlobalPosition()
			(xMouse, yMouse) = wndMgr.GetMousePosition()
			w = self.board.GetWidth()
			h = self.board.GetHeight()
			difX = xMouse - x
			difY = yMouse - y
			if difX <= w and difY <= h:
				if nLen > 0:
					self.scrollBar.OnUp()
					return True
				else:
					self.scrollBar.OnDown()
					return True

	def OnRecvWhisperInfo(self, country_flag, empire):
		if country_flag and self.countryFlag:
			try:
				filename = "d:/ymir work/ui/country_flags/chat/{}.png".format(country_flag)
				if filename != self.countryFlag.GetImagePath():
					self.countryFlag.LoadImage(filename)
					self.countryFlag.SetScale(1.1, 1.1)
				self.countryFlag.Show()
			except:
				self.countryFlag.Hide()

		if self.empireFlag and empire in self.EMPIRE_FLAG_NAME:
			try:
				filename = "d:/ymir work/ui/game/flag/{}.tga".format(self.EMPIRE_FLAG_NAME[empire])
				if filename != self.empireFlag.GetImagePath():
					self.empireFlag.LoadImage(filename)
					self.empireFlag.SetScale(1.2, 1.2)
				self.empireFlag.Show()
			except:
				self.empireFlag.Hide()

if "__main__" == __name__:
	import uiTest

	class TestApp(uiTest.App):
		def OnInit(self):
			wnd = WhisperDialog(self.OnMax, self.OnMin)
			wnd.LoadDialog()
			wnd.OpenWithoutTarget(self.OnNew)
			wnd.SetPosition(0, 0)
			wnd.Show()

			self.wnd = wnd

		def OnMax(self):
			pass

		def OnMin(self):
			pass

		def OnNew(self):
			pass

	TestApp().MainLoop()
