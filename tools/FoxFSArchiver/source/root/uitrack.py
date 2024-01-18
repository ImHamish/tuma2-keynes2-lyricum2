import app
import player
import net
import item
import nonplayer
import systemSetting

import ui
import localeinfo

IMG_DIR_AVATAR = "d:/ymir work/ui/game/track/avatar/"
IMG_DIR = "d:/ymir work/ui/game/track/ymirdesign/"
IS_OLD_WINDOW = IMG_DIR.find("ymirdesign") > 0

def calculateRect(curValue, maxValue):
	try:
		return -1.0 + float(curValue) / float(maxValue)
	except:
		return 0.0

def FormatTime(seconds):
	if seconds <= 0:
		return "0s"
	
	m, s = divmod(seconds, 60)
	h, m = divmod(m, 60)
	d, h = divmod(h, 24)
	timeText = ""
	
	if d > 0:
		timeText += "{}d ".format(d)
	
	if h > 0:
		timeText += "{}h ".format(h)
	
	if m > 0:
		timeText += "{}m ".format(m)
	
	if s > 0:
		timeText += "{}s ".format(s)
	
	return timeText if timeText != "" else "0s"

class TrackWindow(ui.ScriptWindow):
	class TrackItem(ui.ScriptWindow):
		def __init__(self, parent, __dataDict):
			ui.ScriptWindow.__init__(self)
			
			self.parent = parent
			self.__dataDict = __dataDict
			self.__fileName = ""
			self.__LoadWindow()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def Destroy(self):
			self.__dataDict = {}
			self.parent = None
			self.__fileName = ""
			self.ClearDictionary()

		def __LoadWindow(self):
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "uiscript/track_item.py")
			except:
				import exception
				exception.Abort("TrackWindow.__LoadWindow.LoadObject")
			
			self.GetChild("avatar").LoadImage(self.__dataDict["img"])
			self.GetChild("avatar").SetPosition(5, 5)
			self.GetChild("teleport_button").SetEvent(self.__ClickTeleport)
			
			trackType = self.__dataDict["trackType"]
			
			if trackType == 0:
				ticketImageList = [self.GetChild("itemwall{}".format(i)) for i in xrange(3)] if IS_OLD_WINDOW else [self.GetChild("item{}".format(i)) for i in xrange(3)]
				map(lambda x : x.Hide(), ticketImageList)
				(ticketList, ticketListLen) = (self.__dataDict["tickets"], len(self.__dataDict["tickets"]))
				startX = (self.GetWidth()-114)+35 if ticketListLen == 1 else (self.GetWidth()-114)+15 if ticketListLen == 2 else (self.GetWidth()-114)
				for i in xrange(len(ticketList)):
					(itemVnum, itemCount) = tuple(ticketList[i])
					item.SelectItem(itemVnum)
					ticketImage = self.GetChild("item{}".format(i)) if IS_OLD_WINDOW else ticketImageList[i]
					ticketImage.LoadImage(item.GetIconImageFileName())
					ticketImage.SAFE_SetStringEvent("MOUSE_OVER_IN",self.OnMouseOverInTicket, itemVnum)
					ticketImage.SAFE_SetStringEvent("MOUSE_OVER_OUT",self.OnMouseOverOutTicket)
					ticketImage.Show()
					ticketImageList[i].SetPosition(startX+(i * 32), 40)
					ticketImageList[i].Show()
					if itemCount > 1:
						self.GetChild("itemcount{}".format(i)).SetText(str(itemCount))
						self.GetChild("itemcount{}".format(i)).Show()
					else:
						self.GetChild("itemcount{}".format(i)).Hide()
			else:
				ticketImageList = ["itemwall{}".format(i) for i in xrange(3)] if IS_OLD_WINDOW else ["item{}".format(i) for i in xrange(3)]
				ticketImageList.append("dungeon_key")
				for t in ticketImageList:
					self.ElementDictionary[t].Hide()
					self.ElementDictionary[t].Destroy()
					del self.ElementDictionary[t]
					self.ElementDictionary[t] = None
			
			#self.GetChild("name_field").SetText(self.__dataDict["name"] if trackType == 0 else nonplayer.GetMonsterName(self.__dataDict["bossVnum"]))
			self.GetChild("name_field").SetText(self.__dataDict["name"])
			#self.GetChild("level_field").SetText((localeinfo.TRACK_DUNGEON_LEVEL_FIELD if trackType == 0 else localeinfo.TRACK_BOSS_LEVEL_FIELD) % (self.__dataDict["level_range"][0], self.__dataDict["level_range"][1]))
			#self.GetChild("group_field").SetText((localeinfo.TRACK_DUNGEON_PARTY_FIELD if trackType == 0 else localeinfo.TRACK_BOSS_PARTY_FIELD) % ((localeinfo.TRACK_DUNGEON_PARTY_NOT_NEED if not self.__dataDict["group"] else localeinfo.TRACK_DUNGEON_PARTY_NEED) if trackType == 0 else (FormatTime(self.__dataDict["cooldown"]))))
			self.GetChild("group_field").Hide()
			self.GetChild("cooldown_field").SetText((localeinfo.TRACK_DUNGEON_COOLDOWN_FIELD if trackType == 0 else localeinfo.TRACK_BOSS_COOLDOWN_FIELD) % (FormatTime(self.__dataDict["cooldown"]) if trackType == 0 else self.__dataDict["mapName"]))
			
			self.__fileName = IMG_DIR+"dungeon_closed.tga"
			self.__dataDict["status"] = True if app.GetGlobalTimeStamp() > self.__dataDict["server_cooldown"] else False
			
			self.GetChild("status_field").SetText(localeinfo.TRACK_STATUS_FIELD % ("|cFF{}{}".format("6AFF84" if self.__dataDict["status"] else "FF6E79", FormatTime(self.__dataDict["server_cooldown"]-app.GetGlobalTimeStamp()) if not self.__dataDict["status"] else localeinfo.TRACK_STATUS_AVAILABLE if trackType == 0 else (localeinfo.TRACK_STATUS_ALIVE))))

		def OnMouseOverOutTicket(self):
			try:
				import newconstinfo
				newconstinfo._interface_instance.tooltipItem.HideToolTip()
			except Exception as e:
				import dbg
				dbg.TraceError(str(e))

		def OnMouseOverInTicket(self, itemVnum):
			try:
				import newconstinfo
				newconstinfo._interface_instance.tooltipItem.SetItemToolTip(itemVnum)
			except Exception as e:
				import dbg
				dbg.TraceError(str(e))

		def __ClickTeleport(self):
			net.SendChatPacket("/track_window teleport {}".format(self.__dataDict["bossVnum"]))

		def OnUpdate(self):
			if not self.__dataDict["status"]:
				leftTime = self.__dataDict["server_cooldown"]-app.GetGlobalTimeStamp()
				if leftTime <= 1:
					self.__dataDict["status"] = True
					self.__dataDict["deactiveType"] = 0
					
					if ((systemSetting.GetDungeonTrack() == 1 and self.__dataDict["trackType"] == 0) or (systemSetting.GetBossTrack() == 1 and self.__dataDict["trackType"] == 1)):
						try:
							import newconstinfo
							newconstinfo._game_instance.BINARY_OnRecvBulkWhisper((localeinfo.TRACK_DUNGEON_READY if self.__dataDict["trackType"] == 0 else localeinfo.TRACK_BOSS_READY) % self.__dataDict["name"])
						except Exception as e:
							import dbg
							dbg.TraceError(str(e))
				
				self.GetChild("status_field").SetText(localeinfo.TRACK_STATUS_FIELD % ("|cFF{}{}".format("6AFF84" if self.__dataDict["status"] else "FF6E79" if self.__dataDict["deactiveType"] == 0 else "FFDC66", ("Re-Enter " if self.__dataDict["deactiveType"] != 0 else "") + FormatTime(self.__dataDict["server_cooldown"]-app.GetGlobalTimeStamp()) if not self.__dataDict["status"] else localeinfo.TRACK_STATUS_AVAILABLE if self.__dataDict["trackType"] == 0 else (localeinfo.TRACK_STATUS_ALIVE))))
			
			img = IMG_DIR+"dungeon_available.tga" if self.__dataDict["status"] else IMG_DIR+"dungeon_closed.tga" if self.__dataDict["deactiveType"] == 0 else IMG_DIR+"dungeon_re_enter.tga"
			if self.__fileName != img:
				self.GetChild("board").LoadImage(img)
				self.__fileName = img
			else:
				if self.__dataDict["status"] and IMG_DIR+"dungeon_available.tga" != self.__fileName:
					self.GetChild("board").LoadImage(IMG_DIR+"dungeon_available.tga")
					self.__fileName = IMG_DIR+"dungeon_available.tga"

		def OnRender(self):
			_wy = self.parent.GetGlobalPosition()[1]
			#(_wx, _wy) = self.parent.GetGlobalPosition()
			
			_height = self.parent.GetHeight()
			for chilname, childItem in self.ElementDictionary.items():
				if isinstance(childItem, ui.TextLine):
					(_x,_y) = childItem.GetGlobalPosition()
					childHeight = childItem.GetHeight()
					if _y < _wy:
						if childItem.IsShow():
							childItem.Hide()
						
						continue
					
					if _y+childHeight > (_wy+_height-20):
						if childItem.IsShow():
							childItem.Hide()
						
						continue
					
					if not childItem.IsShow():
						childItem.Show()
				
				if isinstance(childItem, ui.ToggleButton) or isinstance(childItem, ui.Button) or isinstance(childItem, ui.ExpandedImageBox):
					topRender = 0
					bottomRender = 0
					itsRendered = False
					(_x,_y) = childItem.GetGlobalPosition()
					childHeight = childItem.GetHeight()
					
					if _y < _wy:
						topRender = _y-_wy
						itsRendered = True
					
					if _y+childHeight > (_wy+_height-2):
						bottomRender = (_wy+_height-2)-(_y+childHeight)
						itsRendered = True
					
					if itsRendered:
						childItem.SetRenderingRect(0,calculateRect(childHeight-abs(topRender), childHeight),0,calculateRect(childHeight-abs(bottomRender),childHeight))
					else:
						childItem.SetRenderingRect(0, 0, 0, 0)

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		
		self.isLoaded = 0
		self.__dataDict = {}
		self.__trackType = 0
		self.__packetSended = False
		self.scrollBar = None
		self.listbox = None
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		if self.scrollBar:
			self.scrollBar.Destroy()
			del self.scrollBar
			self.scrollBar = None
		
		if self.listbox:
			self.listbox.Destroy()
			del self.listbox
			self.listbox = None
		
		self.ClearDictionary()
		
		self.__dataDict = {}
		self.__trackType = 0
		self.__packetSended = False
		self.isLoaded = 0

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
		
		self.isLoaded = 1
		
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/track_window.py")
		except:
			import exception
			exception.Abort("TrackWindow.__LoadWindow.LoadObject")
			return
		
		#self.GetChild("close").SetEvent(ui.__mem_func__(self.Close))
		self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
		
		scrollBar = ScrollBarNew()
		scrollBar.SetParent(self.GetChild("board"))
		scrollBar.SetScrollBarSize(342)
		scrollBar.SetScrollStep(114)
		scrollBar.SetMiddleBarSize(1.0)
		scrollBar.SetPosition(self.GetWidth() - 210, 50)
		scrollBar.Show()
		#self.ElementDictionary["scrollBar"] = scrollBar
		self.scrollBar = scrollBar
		
		self.listbox = self.GetChild("listbox")
		self.listbox.SetScrollBar(self.scrollBar)
		self.listbox.SetExtraScrollLen(114)#112
		
		self.GetChild("dungeon_info").SAFE_SetEvent(self.SetTrackType, 0)
		self.GetChild("dungeon_info").Hide()
		#self.GetChild("worldboss_info").SAFE_SetEvent(self.SetTrackType, 1)
		self.GetChild("open_shop").SAFE_SetEvent(self.OpenShop)
		
		self.GetChild("dungeon_info_notice").SetToggleUpEvent(self.SetOptions, 0)
		self.GetChild("dungeon_info_notice").SetToggleDownEvent(self.SetOptions, 0)
		self.GetChild("dungeon_info_notice").Hide()
		
		self.GetChild("worldboss_info_notice").SetToggleUpEvent(self.SetOptions, 1)
		self.GetChild("worldboss_info_notice").SetToggleDownEvent(self.SetOptions, 1)
		
		self.__dataDict = {
			0 : [
#				{
#					"trackType": 0,
#					"name": "Spider King Barones",
#					"level_range": [75, 90],
#					"group": False,
#					"cooldown": 60 * 30,
#					"server_cooldown": 0,
#					"img": IMG_DIR_AVATAR+"baroness.tga",
#					"tickets": [[30324, 1]],
#					"bossVnum": 2092,
#					"status" : False,
#					"deactiveType" : 0,
#				},
			],
			# WorldBoss
			1 : [
				# {
					# "name": localeinfo.WORLDBOSS_NAME3,
					# "trackType": 1,
					# "level_range": [1, 120],
					# "cooldown": 0,
					# "server_cooldown": 60 * 60 * 4,
					# "img": IMG_DIR_AVATAR + "4902.tga",
					# "bossVnum": 4902,
					# "mapName": localeinfo.WORLDBOSS_ARENA,
					# "channel": 99,
					# "status" : False,
					# "regen_id" : 0,
					# "deactiveType" : 0,
				# },
				{
					"name": localeinfo.WORLDBOSS_NAME1,
					"trackType": 1,
					"level_range": [1, 120],
					"cooldown": 0,
					"server_cooldown": 60 * 60 * 24,
					"img": IMG_DIR_AVATAR + "crystaldragon.png",
					"bossVnum": 4900,
					"mapName": localeinfo.WORLDBOSS_ARENA,
					"channel": 99,
					"status" : False,
					"regen_id" : 1,
					"deactiveType" : 0,
				},
				{
					"name": localeinfo.WORLDBOSS_NAME4,
					"trackType": 1,
					"level_range": [1, 120],
					"cooldown": 0,
					"server_cooldown": 60 * 60 * 24,
					"img": IMG_DIR_AVATAR + "corrupteddragon.png",
					"bossVnum": 4903,
					"mapName": localeinfo.WORLDBOSS_ARENA,
					"channel": 99,
					"status" : False,
					"regen_id" : 2,
					"deactiveType" : 0,
				},
				#{
				#	"name": localeinfo.WORLDBOSS_NAME2,
				#	"trackType": 1,
				#	"level_range": [1, 120],
				#	"cooldown": 0,
				#	"server_cooldown": 60 * 60 * 24,
				#	"img": IMG_DIR_AVATAR + "4901.tga",
				#	"bossVnum": 4901,
				#	"mapName": localeinfo.WORLDBOSS_ARENA,
				#	"channel": 99,
				#	"status" : False,
				#	"regen_id" : 3,
				#	"deactiveType" : 0,
				#},
			],
		}
		
		self.SetTrackType(1)
		self.SetCenterPosition()

	if app.ENABLE_WORLDBOSS:
		def RefreshPoints(self):
			self.GetChild("worldboss_info").SetText(str(player.GetWbPoints()))

	def SetOptions(self, type):
		if type == 0:
			systemSetting.SetDungeonTrack(not systemSetting.GetDungeonTrack())
		elif type == 1:
			systemSetting.SetBossTrack(not systemSetting.GetBossTrack())
		
		self.RefreshOptions()

	def RefreshOptions(self):
		if systemSetting.GetDungeonTrack():
			self.GetChild("dungeon_info_notice").Down()
		else:
			self.GetChild("dungeon_info_notice").SetUp()
		
		if systemSetting.GetBossTrack():
			self.GetChild("worldboss_info_notice").Down()
		else:
			self.GetChild("worldboss_info_notice").SetUp()

	def SetTrackType(self, trackType):
		self.__trackType = trackType
		#btnList = [self.GetChild("dungeon_info"), self.GetChild("worldboss_info")]
		#btnList[trackType].Down()
		#btnList[not trackType].SetUp()
		self.Refresh()

	def Refresh(self):
		if not self.listbox:
			return
		
		self.listbox.RemoveAllItems()
		
		if self.__dataDict.has_key(self.__trackType):
			trackDataList = self.__dataDict[self.__trackType]
		else:
			trackDataList = []
		
		yItemCount = 0
		
		for data in trackDataList:
			trackItem = self.TrackItem(self.listbox, data)
			trackItem.SetParent(self.listbox)
			trackItem.SetPositionTrack(0, yItemCount * trackItem.GetHeight(), True)
			trackItem.Show()
			self.listbox.AppendItem(trackItem)
			yItemCount += 1
		
		self.listbox.RefreshAll()

	def TrackBossInfo(self, bossID, bossLeftTime, bossChannel):
		trackDataList = self.__dataDict[1] if self.__dataDict.has_key(1) else []
		for data in trackDataList:
			if data["regen_id"] == bossID:
				data["server_cooldown"] = app.GetGlobalTimeStamp()+bossLeftTime if bossLeftTime != 65535 else 0
				data["status"] = False if bossLeftTime != 65535 else True

	def TrackDungeonInfo(self, cmdData):
		trackDataList = self.__dataDict[0] if self.__dataDict.has_key(0) else []
		if len(cmdData) != 0:
			cmdTokenList = cmdData[:len(cmdData)-1].split("#")
			for cmd in cmdTokenList:
				cooldown = cmd.split("|")
				if len(cooldown) == 3:
					bossVnum = int(cooldown[0])
					for data in trackDataList:
						if data["bossVnum"] == bossVnum:
							data["server_cooldown"] = app.GetGlobalTimeStamp()+int(cooldown[1])
							data["deactiveType"] = int(cooldown[2])
							data["status"] = False

	def OpenShop(self):
		net.SendOpenShopPacket(63)

	def Open(self):
		if app.ENABLE_WORLDBOSS:
			self.RefreshPoints()
		
		self.Show()
		self.SetTop()
		if not self.__packetSended:
			self.__packetSended = True
			command = "/track_window load"
			trackDataList = self.__dataDict[1] if self.__dataDict.has_key(1) else []
			for data in trackDataList:
				command += " {}".format(data["regen_id"])
			net.SendChatPacket(command)
		
		self.RefreshOptions()

	def CheckPacket(self):
		if not self.__packetSended:
			self.__packetSended = True
			command = "/track_window load"
			trackDataList = self.__dataDict[1] if self.__dataDict.has_key(1) else []
			
			for data in trackDataList:
				command += " {}".format(data["regen_id"])
			
			net.SendChatPacket(command)

	def Close(self):
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		return self.Close()

class ScrollBarNew(ui.Window):
	SCROLLBAR_WIDTH = 10
	SCROLL_BTN_XDIST = 2
	SCROLL_BTN_YDIST = 2

	class MiddleBar(ui.DragButton):
		def __init__(self):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")
			self.SetWindowName("scrollbar_middlebar")

		def MakeImage(self):
			top = ui.ExpandedImageBox()
			top.SetParent(self)
			top.LoadImage(IMG_DIR+"scrollbar_ex/scrollbar_top.tga")
			top.AddFlag("not_pick")
			top.Show()
			
			topScale = ui.ExpandedImageBox()
			topScale.SetParent(self)
			topScale.SetPosition(0, top.GetHeight())
			topScale.LoadImage(IMG_DIR+"scrollbar_ex/scrollbar_scale.tga")
			topScale.AddFlag("not_pick")
			topScale.Show()
			
			bottom = ui.ExpandedImageBox()
			bottom.SetParent(self)
			bottom.LoadImage(IMG_DIR+"scrollbar_ex/scrollbar_bottom.tga")
			bottom.AddFlag("not_pick")
			bottom.Show()
			
			bottomScale = ui.ExpandedImageBox()
			bottomScale.SetParent(self)
			bottomScale.LoadImage(IMG_DIR+"scrollbar_ex/scrollbar_scale.tga")
			bottomScale.AddFlag("not_pick")
			bottomScale.Show()
			
			middle = ui.ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage(IMG_DIR+"scrollbar_ex/scrollbar_mid.tga")
			middle.AddFlag("not_pick")
			middle.Show()
			
			self.top = top
			self.topScale = topScale
			self.bottom = bottom
			self.bottomScale = bottomScale
			self.middle = middle

		def SetSize(self, height):
			minHeight = self.top.GetHeight() + self.bottom.GetHeight() + self.middle.GetHeight()
			height = max(minHeight, height)
			ui.DragButton.SetSize(self, 10, height)
			scale = (height - minHeight) / 2 
			extraScale = 0
			
			if (height - minHeight) % 2 == 1:
				extraScale = 1
			
			self.topScale.SetRenderingRect(0, 0, 0, scale - 1)
			self.middle.SetPosition(0, self.top.GetHeight() + scale)
			self.bottomScale.SetPosition(0, self.middle.GetBottom())
			self.bottomScale.SetRenderingRect(0, 0, 0, scale - 1 + extraScale)
			self.bottom.SetPosition(0, height - self.bottom.GetHeight())

	def __init__(self):
		ui.Window.__init__(self)
		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = None
		self.eventArgs = None
		self.lockFlag = False
		self.CreateScrollBar()
		self.SetScrollBarSize(0)
		self.scrollStep = 0.2
		self.SetWindowName("NONAME_ScrollBar")

	def __del__(self):
		ui.Window.__del__(self)

	def CreateScrollBar(self):
		topImage = ui.ExpandedImageBox()
		topImage.SetParent(self)
		topImage.AddFlag("not_pick")
		topImage.LoadImage(IMG_DIR+"scrollbar_ex/scroll_top.tga")
		topImage.Show()
		
		bottomImage = ui.ExpandedImageBox()
		bottomImage.SetParent(self)
		bottomImage.AddFlag("not_pick")
		bottomImage.LoadImage(IMG_DIR+"scrollbar_ex/scroll_bottom.tga")
		bottomImage.Show()
		
		middleImage = ui.ExpandedImageBox()
		middleImage.SetParent(self)
		middleImage.AddFlag("not_pick")
		middleImage.SetPosition(0, topImage.GetHeight())
		middleImage.LoadImage(IMG_DIR+"scrollbar_ex/scroll_mid.tga")
		middleImage.Show()
		
		self.topImage = topImage
		self.bottomImage = bottomImage
		self.middleImage = middleImage
		
		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.MakeImage()
		middleBar.SetSize(12)
		
		self.middleBar = middleBar

	def Destroy(self):
		self.eventScroll = None
		self.eventArgs = None

	def SetScrollEvent(self, event, *args):
		self.eventScroll = event
		self.eventArgs = args

	def SetMiddleBarSize(self, pageScale):
		self.middleBar.SetSize(int(pageScale * float(self.GetHeight() - (self.SCROLL_BTN_YDIST*2))))
		realHeight = self.GetHeight() - (self.SCROLL_BTN_YDIST*2) - self.middleBar.GetHeight()
		self.pageSize = realHeight

	def SetScrollBarSize(self, height):
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.pageSize = height - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()
		middleImageScale = float((height - self.SCROLL_BTN_YDIST*2) - self.middleImage.GetHeight()) / float(self.middleImage.GetHeight())
		self.middleImage.SetRenderingRect(0, 0, 0, middleImageScale)
		self.bottomImage.SetPosition(0, height - self.bottomImage.GetHeight())
		self.middleBar.SetRestrictMovementArea(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST, \
			self.middleBar.GetWidth(), height - self.SCROLL_BTN_YDIST * 2)
		
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST)

	def SetScrollStep(self, step):
		self.scrollStep = step

	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)

	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)

	def GetScrollStep(self):
		return self.scrollStep

	def GetPos(self):
		return self.curPos

	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)

	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)

	def SetPos(self, pos, moveEvent = True):
		pos = max(0.0, pos)
		pos = min(1.0, pos)
		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, int(newPos) + self.SCROLL_BTN_YDIST)
		if moveEvent == True:
			self.OnMove()

	def OnMove(self):
		if self.lockFlag:
			return
		
		if 0 == self.pageSize:
			return
		
		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal - self.SCROLL_BTN_YDIST) / float(self.pageSize)
		if self.eventScroll:
			apply(self.eventScroll, self.eventArgs)

	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		newPos = float(yMouseLocalPosition) / float(self.GetHeight())
		self.SetPos(newPos)

	def LockScroll(self):
		self.lockFlag = True

	def UnlockScroll(self):
		self.lockFlag = False
