import app
import player
import net

import ui
import uiscriptlocale
import wndMgr
import miniMap
import colorinfo
import background
import time
import serverinfo as serverinfo
import uicommon
#from _weakref import proxy
import localeinfo
import grp
import systemSetting
import uitooltip
import nonplayer

class MapTextToolTip(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)

		textLine = ui.TextLine()
		textLine.SetParent(self)
		if not app.ENABLE_ATLAS_BOSS:
			textLine.SetHorizontalAlignCenter()
		
		textLine.SetOutline()
		if not app.ENABLE_ATLAS_BOSS:
			textLine.SetHorizontalAlignRight()
		else:
			textLine.SetHorizontalAlignLeft()
		
		textLine.Show()
		self.textLine = textLine
		if app.ENABLE_ATLAS_BOSS:
			textLine2 = ui.TextLine()
			textLine2.SetParent(self)
			textLine2.SetOutline()
			textLine2.SetHorizontalAlignLeft()
			textLine2.Show()
			self.textLine2 = textLine2

	def __del__(self):
		ui.Window.__del__(self)

	def SetText(self, text):
		self.textLine.SetText(text)

	if app.ENABLE_ATLAS_BOSS:
		def SetText2(self, text):
			self.textLine2.SetText(text)

		def ShowText2(self):
			self.textLine2.Show()

		def HideText2(self):
			self.textLine2.Hide()

	def SetTooltipPosition(self, PosX, PosY):
		if app.ENABLE_ATLAS_BOSS:
			PosY -= 24
		
		if localeinfo.IsARABIC():
			w, h = self.textLine.GetTextSize()
			self.textLine.SetPosition(PosX - w - 5, PosY)
			if app.ENABLE_ATLAS_BOSS:
				self.textLine2.SetPosition(PosX - w - 5, PosY + 10)
		else:
			self.textLine.SetPosition(PosX - 5, PosY)
			if app.ENABLE_ATLAS_BOSS:
				self.textLine2.SetPosition(PosX - 5, PosY + 10)

	def SetTextColor(self, TextColor):
		self.textLine.SetPackedFontColor(TextColor)
		if app.ENABLE_ATLAS_BOSS:
			self.textLine2.SetPackedFontColor(TextColor)

	def GetTextSize(self):
		return self.textLine.GetTextSize()

class AtlasWindow(ui.ScriptWindow):
	BOSS_COLOR = grp.GenerateColor(1.0, 1.0, 1.0, 1.0)
	
	class AtlasRenderer(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)
			self.AddFlag("not_pick")

		def OnUpdate(self):
			miniMap.UpdateAtlas()

		def OnRender(self):
			(x, y) = self.GetGlobalPosition()
			fx = float(x)
			fy = float(y)
			miniMap.RenderAtlas(fx, fy)

		def HideAtlas(self):
			miniMap.HideAtlas()

		def ShowAtlas(self):
			miniMap.ShowAtlas()

	def __init__(self):
		self.tooltipInfo = MapTextToolTip()
		self.tooltipInfo.Hide()
		self.infoGuildMark = ui.MarkBox()
		self.infoGuildMark.Hide()
		self.AtlasMainWindow = None
		self.mapName = ""
		self.board = 0
		self.boss_refresh = None
		self.questionDialog = None

		self.toolTipText = uitooltip.ToolTip(100)
		self.toolTipText.HideToolTip()

		ui.ScriptWindow.__init__(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/atlaswindow.py")
		except:
			import exception
			exception.Abort("AtlasWindow.LoadWindow.LoadScript")

		try:
			self.board = self.GetChild("board")
			self.boss_refresh = self.GetChild("boss_refresh")
		except:
			import exception
			exception.Abort("AtlasWindow.LoadWindow.BindObject")

		self.AtlasMainWindow = self.AtlasRenderer()
		self.board.SetCloseEvent(self.Hide)
		self.boss_refresh.SetEvent(ui.__mem_func__(self.RequestBossInfo))
		self.AtlasMainWindow.SetParent(self.board)
		self.AtlasMainWindow.SetPosition(7, 30)
		self.tooltipInfo.SetParent(self.board)
		self.infoGuildMark.SetParent(self.board)
		self.SetPosition(wndMgr.GetScreenWidth() - 136 - 256 - 10, 0)
		# self.board.SetOnMouseLeftButtonUpEvent(ui.__mem_func__(self.EventMouseLeftButtonUp))
		# if self.questionDialog != None:
			# del self.questionDialog
		
		# self.questionDialog = uicommon.QuestionDialog2()
		# self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCancelQuestion))
		# self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnAcceptQuestion))
		# self.questionDialog.iPosX = 0
		# self.questionDialog.iPosY = 0
		# self.questionDialog.Hide()
		self.Hide()

		miniMap.RegisterAtlasWindow(self)

	def Destroy(self):
		miniMap.UnregisterAtlasWindow()
		self.ClearDictionary()
		self.AtlasMainWindow = None
		self.tooltipAtlasClose = 0
		self.tooltipInfo = None
		self.infoGuildMark = None
		self.board = None
		if self.questionDialog != None:
			del self.questionDialog
		
		self.questionDialog = None
		self.boss_refresh = None

		if self.toolTipText:
			self.toolTipText.HideToolTip()
			del self.toolTipText
			self.toolTipText = None

	# def OnCancelQuestion(self):
		# if self.questionDialog == None:
			# return
		# elif not self.questionDialog.IsShow():
			# return
		
		# self.questionDialog.iPosX = 0
		# self.questionDialog.iPosY = 0
		# self.questionDialog.Close()

	# def OnAcceptQuestion(self):
		# if self.questionDialog == None:
			# return
		
		# net.SendChatPacket("/gotoxy %d %d" % (self.questionDialog.iPosX, self.questionDialog.iPosY))
		# self.OnCancelQuestion()

	# def OnMoveWindow(self, x, y):
		# self.OnCancelQuestion()

	# def EventMouseLeftButtonUp(self):
		# (mouseX, mouseY) = wndMgr.GetMousePosition()
		# if app.ENABLE_ATLAS_BOSS:
			# (bFind, sName, iPosX, iPosY, dwTextColor, dwGuildID, time) = miniMap.GetAtlasInfo(mouseX, mouseY)
		# else:
			# (bFind, sName, iPosX, iPosY, dwTextColor, dwGuildID) = miniMap.GetAtlasInfo(mouseX, mouseY)
		
		# if self.questionDialog.IsShow():
			# self.questionDialog.SetTop()
		
		# if False == bFind:
			# return 1
		
		# self.questionDialog.SetText1(localeinfo.ATLASINFO_QUESTIONDIALOG1 % (sName))
		# self.questionDialog.SetText2(localeinfo.ATLASINFO_QUESTIONDIALOG2)
		# self.questionDialog.iPosX = iPosX
		# self.questionDialog.iPosY = iPosY
		# self.questionDialog.SetWidth(170 + len(sName * 5))
		# self.questionDialog.Open()
		# return 1

	def RequestBossInfo(self):
		import net
		net.SendReqMapBossInfo()

	def OnUpdate(self):

		if not self.tooltipInfo:
			return

		if not self.infoGuildMark:
			return

		self.infoGuildMark.Hide()
		self.tooltipInfo.Hide()
		self.toolTipText.ClearToolTip()
		self.toolTipText.HideToolTip()

		if False == self.board.IsIn():
			return

		(mouseX, mouseY) = wndMgr.GetMousePosition()
		if app.ENABLE_ATLAS_BOSS:
			(bFind, sName, iPosX, iPosY, dwTextColor, dwGuildID, time) = miniMap.GetAtlasInfo(mouseX, mouseY)
		else:
			(bFind, sName, iPosX, iPosY, dwTextColor, dwGuildID) = miniMap.GetAtlasInfo(mouseX, mouseY)

		if False == bFind:
			dwTextColor = 0xffffd74c

		if "empty_guild_area" == sName:
			sName = localeinfo.GUILD_EMPTY_AREA

		if localeinfo.IsARABIC() and sName[-1].isalnum():
			self.tooltipInfo.SetText("(%s)%d, %d" % (sName, iPosX, iPosY))
			if app.ENABLE_ATLAS_BOSS:
				self.tooltipInfo.SetText2(localeinfo.MINIMAP_BOSS_RESPAWN_TIME % (time / 60))
		else:
			self.tooltipInfo.SetText("%s(%d, %d)" % (sName, iPosX, iPosY))
			if app.ENABLE_ATLAS_BOSS:
				self.tooltipInfo.SetText2(localeinfo.MINIMAP_BOSS_RESPAWN_TIME % (time / 60))

		(x, y) = self.GetGlobalPosition()

		if self.tooltipInfo:
			boss_info = miniMap.GetBossAtlasInfo(mouseX, mouseY)
			if boss_info is not None:
				vnum, respawn_time, real_time, is_alive = boss_info
				self.toolTipText.AutoAppendTextLine(nonplayer.GetMonsterName(vnum), self.toolTipText.TITLE_COLOR)

				if is_alive:
					self.toolTipText.AutoAppendTextLine("Respawn: {}".format(localeinfo.seconds_to_dhms(respawn_time)), self.toolTipText.NORMAL_COLOR)
				else:
					if real_time > app.GetGlobalTimeStamp():
						self.toolTipText.AutoAppendTextLine("{} {}".format(localeinfo.BOSS_INFO_COOLDOWN, localeinfo.seconds_to_dhms(real_time - app.GetGlobalTimeStamp())), self.toolTipText.NEGATIVE_COLOR)
					else:
						self.toolTipText.AutoAppendTextLine("Respawn: {}".format(localeinfo.seconds_to_dhms(respawn_time)), self.toolTipText.NORMAL_COLOR)

				self.toolTipText.ResizeToolTip()
				self.toolTipText.ShowToolTip()
				self.toolTipText.SetTop()
				return
			else:
				if bFind:
					self.tooltipInfo.SetTooltipPosition(mouseX - x, mouseY - y)
					self.tooltipInfo.SetTextColor(dwTextColor)
					self.tooltipInfo.Show()
					self.tooltipInfo.SetTop()

		if 0 != dwGuildID and bFind:
			textWidth, textHeight = self.tooltipInfo.GetTextSize()
			self.infoGuildMark.SetIndex(dwGuildID)
			self.infoGuildMark.SetPosition(mouseX - x - textWidth - 18 - 5, mouseY - y)
			self.infoGuildMark.Show()

	def Hide(self):
		if self.AtlasMainWindow:
			self.AtlasMainWindow.HideAtlas()
			self.AtlasMainWindow.Hide()

		if self.toolTipText:
			self.toolTipText.ClearToolTip()
			self.toolTipText.HideToolTip()

		# self.OnCancelQuestion()
		ui.ScriptWindow.Hide(self)

	def Show(self):
		if self.AtlasMainWindow:
			(bGet, iSizeX, iSizeY) = miniMap.GetAtlasSize()
			if bGet:
				self.SetSize(iSizeX + 15, iSizeY + 38)

				if localeinfo.IsARABIC():
					self.board.SetPosition(iSizeX+15, 0)

				self.board.SetSize(iSizeX + 15, iSizeY + 38)
				#self.AtlasMainWindow.SetSize(iSizeX, iSizeY)
				self.AtlasMainWindow.ShowAtlas()
				self.AtlasMainWindow.Show()
				self.RequestBossInfo()
		ui.ScriptWindow.Show(self)
		self.SetCenterPosition()

	def SetCenterPositionAdjust(self, x, y):
		self.SetPosition((wndMgr.GetScreenWidth() - self.GetWidth()) / 2 + x, (wndMgr.GetScreenHeight() - self.GetHeight()) / 2 + y)

	def OnPressEscapeKey(self):
		self.Hide()
		return True

def __RegisterMiniMapColor(type, rgb):
	miniMap.RegisterColor(type, rgb[0], rgb[1], rgb[2])

class MiniMap(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		
		miniMap.Create()
		miniMap.SetScale(2.0)
		
		self.AtlasWindow = AtlasWindow()
		self.AtlasWindow.LoadWindow()
		self.AtlasWindow.Hide()
		
		if app.INGAME_WIKI:
			self.btnWiki = None
		
		self.btnBlockEquip = None
		self.btnRanking = None
		
		self.mapName = ""
		self.isLoaded = 0
		self.canSeeInfo = True
		
		self.imprisonmentDuration = 0
		self.imprisonmentEndTime = 0
		self.imprisonmentEndTimeText = ""
		
		self.ShowToolTip = False

	def __del__(self):
		self.ShowToolTip = None
		self.toolTip = None
		miniMap.Destroy()
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.renderInfo = 0	
		self.observerCount = 0
		self.toolTip = uitooltip.ToolTip()
		self.toolTip.ClearToolTip()
		
		self.OpenWindow = 0
		self.CloseWindow = 0
		self.ScaleUpButton = 0
		self.ScaleDownButton = 0
		self.MiniMapHideButton = 0
		self.MiniMapShowButton = 0
		self.AtlasShowButton = 0
		self.serverinfo = None
		
		if app.ENABLE_DEFENSE_WAVE:
			self.MastHp = 0
		
		if app.ENABLE_DATETIME_UNDER_MINIMAP:
			self.minimapclock = 0

	def SetImprisonmentDuration(self, duration):
		self.imprisonmentDuration = duration
		self.imprisonmentEndTime = app.GetGlobalTimeStamp() + duration
		self.__UpdateImprisonmentDurationText()

	def __UpdateImprisonmentDurationText(self):
		restTime = max(self.imprisonmentEndTime - app.GetGlobalTimeStamp(), 0)
		imprisonmentEndTimeText = localeinfo.SecondToDHM(restTime)
		if imprisonmentEndTimeText != self.imprisonmentEndTimeText:
			self.imprisonmentEndTimeText = imprisonmentEndTimeText
			self.serverinfo.SetText("%s: %s" % (uiscriptlocale.AUTOBAN_QUIZ_REST_TIME, self.imprisonmentEndTimeText))

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
		
		self.isLoaded = 1
		
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/minimap_window.py")
		except:
			import exception
			exception.Abort("MiniMap.LoadWindow.LoadScript")
		
		try:
			self.OpenWindow = self.GetChild("OpenWindow")
			self.MiniMapWindow = self.GetChild("MiniMapWindow")
			
			self.ScaleUpButton = self.GetChild("ScaleUpButton")
			self.ScaleUpButton.ShowToolTip = lambda arg = localeinfo.MINIMAP_INC_SCALE : self.OverInToolTip(arg)
			self.ScaleUpButton.HideToolTip = lambda : self.OverOutToolTip()
			
			self.ScaleDownButton = self.GetChild("ScaleDownButton")
			self.ScaleDownButton.ShowToolTip = lambda arg = localeinfo.MINIMAP_DEC_SCALE : self.OverInToolTip(arg)
			self.ScaleDownButton.HideToolTip = lambda : self.OverOutToolTip()
			
			self.MiniMapHideButton = self.GetChild("MiniMapHideButton")
			self.MiniMapHideButton.ShowToolTip = lambda arg = localeinfo.UI_CLOSE : self.OverInToolTip(arg)
			self.MiniMapHideButton.HideToolTip = lambda : self.OverOutToolTip()
			
			self.AtlasShowButton = self.GetChild("AtlasDownButton")
			self.AtlasShowButton.ShowToolTip = lambda arg = localeinfo.MINIMAP_SHOW_AREAMAP : self.OverInToolTip(arg)
			self.AtlasShowButton.HideToolTip = lambda : self.OverOutToolTip()
			
			self.CloseWindow = self.GetChild("CloseWindow")
			self.CloseWindow.ShowToolTip = lambda arg = localeinfo.UI_CLOSE : self.OverInToolTip(arg)
			self.CloseWindow.HideToolTip = lambda : self.OverOutToolTip()
			
			self.MiniMapShowButton = self.GetChild("MiniMapShowButton")
			self.MiniMapShowButton.ShowToolTip = lambda arg = localeinfo.MINIMAP : self.OverInToolTip(arg)
			self.MiniMapShowButton.HideToolTip = lambda : self.OverOutToolTip()
			
			self.observerCount = self.GetChild("ObserverCount")
			self.renderInfo = self.GetChild("RenderInfo")
			self.serverinfo = self.GetChild("ServerInfo")
			
			if app.ENABLE_DEFENSE_WAVE:
				self.MastHp = self.GetChild("MastHp")
				self.MastWindow = self.GetChild("MastWindow")
				self.ActualMastText = self.GetChild("ActualMastText")
				self.MastTimerText = self.GetChild("MastTimerText")
				self.MastHp.OnMouseOverIn = ui.__mem_func__(self.MastHp.ShowToolTip)
				self.MastHp.OnMouseOverOut = ui.__mem_func__(self.MastHp.HideToolTip)
				self.MastHp.SetShowToolTipEvent(self.MastHp.OnMouseOverIn)
				self.MastHp.SetHideToolTipEvent(self.MastHp.OnMouseOverOut)
			
			if app.ENABLE_DATETIME_UNDER_MINIMAP:
				self.minimapclock = self.GetChild("MiniMapClock")
			
			if app.INGAME_WIKI:
				self.btnWiki = self.GetChild("WikiButton")
				self.btnWiki.ShowToolTip = lambda arg = localeinfo.WIKI_TITLE : self.OverInToolTip(arg)
				self.btnWiki.HideToolTip = lambda : self.OverOutToolTip()
			else:
				self.GetChild("WikiButton").Hide()
			
			self.btnBlockEquip = self.GetChild("BlockEquipButton")
			self.btnBlockEquip.ShowToolTip = lambda arg = localeinfo.EQUIPVIEW_TITLE : self.OverInToolTip(arg)
			self.btnBlockEquip.HideToolTip = lambda : self.OverOutToolTip()
			
			self.btnRanking = self.GetChild("RankingButton")
			self.btnRanking.ShowToolTip = lambda arg = localeinfo.RANKING_TITLE : self.OverInToolTip(arg)
			self.btnRanking.HideToolTip = lambda : self.OverOutToolTip()
			
			self.infoValue2 = self.GetChild("textInfoValue2")
		except:
			import exception
			exception.Abort("MiniMap.LoadWindow.Bind")
		
		if app.ENABLE_DATETIME_UNDER_MINIMAP:
			self.minimapclock.Show()
		
		self.serverinfo.SetText(net.GetServerInfo())
		self.ScaleUpButton.SetEvent(ui.__mem_func__(self.ScaleUp))
		self.ScaleDownButton.SetEvent(ui.__mem_func__(self.ScaleDown))
		self.MiniMapHideButton.SetEvent(ui.__mem_func__(self.HideMiniMap))
		self.MiniMapShowButton.SetEvent(ui.__mem_func__(self.ShowMiniMap))
		
		if miniMap.IsAtlas():
			self.AtlasShowButton.SetEvent(ui.__mem_func__(self.ShowAtlas))
		
		if app.INGAME_WIKI:
			self.btnWiki.SetEvent(ui.__mem_func__(self.OnPressBtnWiki))
		
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			self.btnBlockEquip.SetEvent(ui.__mem_func__(self.ClickPvpEquipment))
		else:
			self.btnBlockEquip.Hide()
		
		self.btnRanking.SetEvent(ui.__mem_func__(self.OpenRanking))
		
		if app.ENABLE_DEFENSE_WAVE:
			self.MastHp.SetPercentage(12000000, 12000000)
			self.MastWindow.Hide()
		
		self.ShowMiniMap()

	def Destroy(self):
		if app.INGAME_WIKI:
			self.btnWiki = None
		
		self.btnBlockEquip = None
		self.btnRanking = None
		
		self.HideMiniMap()
		self.AtlasWindow.Destroy()
		self.AtlasWindow = None
		self.infoValue2 = None
		self.ClearDictionary()
		self.__Initialize()

	def UpdateObserverCount(self, observerCount):
		if observerCount>0:
			self.observerCount.Show()
		elif observerCount<=0:
			self.observerCount.Hide()
		
		self.observerCount.SetText(localeinfo.MINIMAP_OBSERVER_COUNT % observerCount)

	if app.ENABLE_DEFENSE_WAVE:
		def SetMastHP(self, hp):
			self.MastHp.SetPercentage(hp, 12000000)
			self.MastHp.SetToolTipText(localeinfo.MASK_HP % (localeinfo.AddPointToNumberString(hp), localeinfo.AddPointToNumberString(12000000)))
			self.ActualMastText.SetText(localeinfo.MASK_HP % (localeinfo.AddPointToNumberString(hp), localeinfo.AddPointToNumberString(12000000)))

		def setMastTimer(self, text):
			texts = [localeinfo.IDRA_TEXT1, localeinfo.IDRA_TEXT2, localeinfo.IDRA_TEXT3, localeinfo.IDRA_TEXT4, localeinfo.IDRA_TEXT5, localeinfo.IDRA_TEXT6, localeinfo.IDRA_TEXT7, localeinfo.IDRA_TEXT8, localeinfo.IDRA_TEXT9, localeinfo.IDRA_TEXT10]
			arg = text.split("|")
			if int(arg[0]) > len(texts):
				return
			
			if int(arg[0]) == 2 or int(arg[0]) == 4:
				self.MastTimerText.SetText(texts[int(arg[0])] % (int(arg[1]), int(arg[2])))
			elif int(arg[0]) == 9:
				self.MastTimerText.SetText(texts[int(arg[0])] % (int(arg[1])))
			else:
				self.MastTimerText.SetText(texts[int(arg[0])])

		def SetMastWindow(self, i):
			if i:
				self.MastWindow.Show()
			else:
				self.MastWindow.Hide()

		def OnMouseOverIn(self):
			if self.toolTip:
				self.toolTip.ShowToolTip()

		def OnMouseOverOut(self):
			if self.toolTip:
				self.toolTip.HideToolTip()

	def OnUpdate(self):
		if app.ENABLE_DATETIME_UNDER_MINIMAP:
			if systemSetting.GetTimePm():
				self.minimapclock.SetText(time.strftime('[%I:%M:%S %p - %d/%m/%Y]'))
			else:
				self.minimapclock.SetText(time.strftime('[%X - %d/%m/%Y]'))
		
		nRenderFPS = app.GetRenderFPS()
		fps = "%3d" % (nRenderFPS)
		self.renderInfo.SetText(uiscriptlocale.AEON_PERFORMANCE+ " " + str(fps))
		
		(x, y, z) = player.GetMainCharacterPosition()
		miniMap.Update(x, y)
		
		added = False
		if self.MiniMapWindow.IsIn() == True:
			(mouseX, mouseY) = wndMgr.GetMousePosition()
			(bFind, sName, iPosX, iPosY, dwTextColor) = miniMap.GetInfo(mouseX, mouseY)
			if bFind != 0:
				if self.canSeeInfo:
					self.infoValue2.SetText("(%d, %d)" % (iPosX, iPosY))
				else:
					self.infoValue2.SetText("(%s)" % (localeinfo.UI_POS_UNKNOWN))
				
				self.infoValue2.SetPackedFontColor(dwTextColor)
				added = True
		
		if not added:
			self.infoValue2.SetText("(%d, %d)" % (int(x / 100), int(y / 100)))
			self.infoValue2.SetPackedFontColor(-10420)
		
		if self.imprisonmentDuration:
			self.__UpdateImprisonmentDurationText()

	def OnRender(self):
		(x, y) = self.GetGlobalPosition()
		fx = float(x)
		fy = float(y)
		miniMap.Render(fx + 4.0 + 12.0, fy + 5.0 + 13.0)

	def Close(self):
		self.HideMiniMap()

	def HideMiniMap(self):
		miniMap.Hide()
		self.OpenWindow.Hide()
		self.CloseWindow.Show()

	def ShowMiniMap(self):
		if not self.canSeeInfo:
			return

		miniMap.Show()
		self.OpenWindow.Show()
		self.CloseWindow.Hide()

	def isShowMiniMap(self):
		return miniMap.isShow()

	def ScaleUp(self):
		miniMap.ScaleUp()

	def ScaleDown(self):
		miniMap.ScaleDown()

	def ShowAtlas(self):
		if not miniMap.IsAtlas():
			return
		
		if not self.AtlasWindow.IsShow():
			self.AtlasWindow.Show()
		else:
			self.AtlasWindow.Hide()

	def ToggleAtlasWindow(self):
		if not miniMap.IsAtlas():
			return
		
		if self.AtlasWindow.IsShow():
			self.AtlasWindow.Hide()
		else:
			self.AtlasWindow.Show()

	if app.INGAME_WIKI:
		def OnPressBtnWiki(self):
			try:
				import newconstinfo
				newconstinfo._interface_instance.ToggleWikiNew()
			except:
				pass

	def ClickPvpEquipment(self):
		try:
			import constinfo
			if constinfo.equipview == 0:
				net.SendChatPacket("/pvp_block_equipment BLOCK")
			else:
				net.SendChatPacket("/pvp_block_equipment UNBLOCK")
		except:
			pass

	def OpenRanking(self):
		try:
			import newconstinfo
			newconstinfo._interface_instance.OpenRanking()
		except:
			pass

	def OverInToolTip(self, arg):
		arglen = len(str(arg))
		pos_x, pos_y = wndMgr.GetMousePosition()
		
		if self.toolTip:
			self.toolTip.ClearToolTip()
			self.toolTip.SetThinBoardSize(11 * arglen)
			self.toolTip.SetToolTipPosition(pos_x + 50, pos_y + 50)
			self.toolTip.AppendTextLine(arg, 0xffffff00)
			self.toolTip.Show()
		
		self.ShowToolTip = True

	def OverOutToolTip(self):
		if self.toolTip:
			self.toolTip.Hide()
		
		self.ShowToolTip = False
