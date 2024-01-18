import app
import chr
import player
import net

import ui
import wndMgr
import messenger
import guild
import nonplayer
import constinfo
import uitooltip
import item
import localeinfo

if app.ENABLE_VIEW_ELEMENT:
	ELEMENT_IMAGE_DIC = {1: "elect", 2: "fire", 3: "ice", 4: "wind", 5: "earth", 6 : "dark"}

class TargetBoard(ui.ThinBoard):
	BUTTON_NAME_LIST = (
		localeinfo.TARGET_BUTTON_WHISPER,
		localeinfo.COMMAND_MAP1,
		localeinfo.TARGET_BUTTON_EXCHANGE,
		localeinfo.TARGET_BUTTON_FIGHT,
		localeinfo.TARGET_BUTTON_ACCEPT_FIGHT,
		localeinfo.TARGET_BUTTON_AVENGE,
		localeinfo.TARGET_BUTTON_FRIEND,
		localeinfo.TARGET_BUTTON_INVITE_PARTY,
		localeinfo.TARGET_BUTTON_LEAVE_PARTY,
		localeinfo.TARGET_BUTTON_EXCLUDE,
		localeinfo.TARGET_BUTTON_INVITE_GUILD,
		localeinfo.TARGET_BUTTON_DISMOUNT,
		localeinfo.TARGET_BUTTON_EXIT_OBSERVER,
		localeinfo.TARGET_BUTTON_VIEW_EQUIPMENT,
		localeinfo.TARGET_BUTTON_REQUEST_ENTER_PARTY,
		localeinfo.TARGET_BUTTON_BUILDING_DESTROY,
		localeinfo.TARGET_BUTTON_EMOTION_ALLOW,
		"VOTE_BLOCK_CHAT",
	)

	if app.ENABLE_MESSENGER_BLOCK:
		BUTTON_NAME_LIST = BUTTON_NAME_LIST + (localeinfo.TARGET_BUTTON_BLOCK, localeinfo.TARGET_BUTTON_UNBLOCK,)

	GRADE_NAME =	{
						nonplayer.PAWN : localeinfo.TARGET_LEVEL_PAWN,
						nonplayer.S_PAWN : localeinfo.TARGET_LEVEL_S_PAWN,
						nonplayer.KNIGHT : localeinfo.TARGET_LEVEL_KNIGHT,
						nonplayer.S_KNIGHT : localeinfo.TARGET_LEVEL_S_KNIGHT,
						nonplayer.BOSS : localeinfo.TARGET_LEVEL_BOSS,
						nonplayer.KING : localeinfo.TARGET_LEVEL_KING,
					}
	EXCHANGE_LIMIT_RANGE = 3000

	def __init__(self):
		ui.ThinBoard.__init__(self)
		if app.ENABLE_MESSENGER_BLOCK:
			self.AddFlag("float")
		
		name = ui.TextLine()
		name.SetParent(self)
		name.SetDefaultFontName()
		name.SetOutline()
		name.Show()

		hpGauge = ui.Gauge()
		hpGauge.SetParent(self)
		hpGauge.MakeGauge(130, "red")
		hpGauge.Hide()
		if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
			hpDecimal = ui.TextLine()
			hpDecimal.SetParent(hpGauge)
			hpDecimal.SetDefaultFontName()
			hpDecimal.SetPosition(-100, 5)
			hpDecimal.SetOutline()
			hpDecimal.Hide()

		closeButton = ui.Button()
		closeButton.SetParent(self)
		closeButton.SetUpVisual("d:/ymir work/ui/public/close_button_01.sub")
		closeButton.SetOverVisual("d:/ymir work/ui/public/close_button_02.sub")
		closeButton.SetDownVisual("d:/ymir work/ui/public/close_button_03.sub")
		closeButton.SetPosition(30, 13)

		if localeinfo.IsARABIC():
			hpGauge.SetPosition(55, 17)
			hpGauge.SetWindowHorizontalAlignLeft()
			closeButton.SetWindowHorizontalAlignLeft()
		else:
			hpGauge.SetPosition(175, 17)
			hpGauge.SetWindowHorizontalAlignRight()
			closeButton.SetWindowHorizontalAlignRight()
		
		closeButton.SetEvent(ui.__mem_func__(self.OnPressedCloseButton))
		closeButton.Show()
		
		if app.INGAME_WIKI:
			dropInfoButton = ui.Button()
			dropInfoButton.SetParent(self)
			dropInfoButton.SetUpVisual("d:/ymir work/ui/pattern/q_mark_01.tga")
			dropInfoButton.SetOverVisual("d:/ymir work/ui/pattern/q_mark_02.tga")
			dropInfoButton.SetDownVisual("d:/ymir work/ui/pattern/q_mark_01.tga")
			dropInfoButton.SetEvent(ui.__mem_func__(self.OnPressDropInfo))
			dropInfoButton.Hide()
		
		self.buttonDict = {}
		self.showingButtonList = []
		for buttonName in self.BUTTON_NAME_LIST:
			button = ui.Button()
			button.SetParent(self)

			if localeinfo.IsARABIC():
				button.SetUpVisual("d:/ymir work/ui/public/Small_Button_01.sub")
				button.SetOverVisual("d:/ymir work/ui/public/Small_Button_02.sub")
				button.SetDownVisual("d:/ymir work/ui/public/Small_Button_03.sub")
			else:
				button.SetUpVisual("d:/ymir work/ui/public/small_thin_button_01.sub")
				button.SetOverVisual("d:/ymir work/ui/public/small_thin_button_02.sub")
				button.SetDownVisual("d:/ymir work/ui/public/small_thin_button_03.sub")

			button.SetWindowHorizontalAlignCenter()
			button.SetText(buttonName)
			button.Hide()
			self.buttonDict[buttonName] = button
			self.showingButtonList.append(button)

		self.buttonDict[localeinfo.TARGET_BUTTON_WHISPER].SetEvent(ui.__mem_func__(self.OnWhisper))
		self.buttonDict[localeinfo.TARGET_BUTTON_EXCHANGE].SetEvent(ui.__mem_func__(self.OnExchange))
		self.buttonDict[localeinfo.TARGET_BUTTON_FIGHT].SetEvent(ui.__mem_func__(self.OnPVP))
		self.buttonDict[localeinfo.TARGET_BUTTON_ACCEPT_FIGHT].SetEvent(ui.__mem_func__(self.OnPVP))
		self.buttonDict[localeinfo.TARGET_BUTTON_AVENGE].SetEvent(ui.__mem_func__(self.OnPVP))
		self.buttonDict[localeinfo.TARGET_BUTTON_FRIEND].SetEvent(ui.__mem_func__(self.OnAppendToMessenger))
		self.buttonDict[localeinfo.TARGET_BUTTON_FRIEND].SetEvent(ui.__mem_func__(self.OnAppendToMessenger))
		self.buttonDict[localeinfo.COMMAND_MAP1].SetEvent(ui.__mem_func__(self.OnMap1))
		self.buttonDict[localeinfo.TARGET_BUTTON_INVITE_PARTY].SetEvent(ui.__mem_func__(self.OnPartyInvite))
		self.buttonDict[localeinfo.TARGET_BUTTON_LEAVE_PARTY].SetEvent(ui.__mem_func__(self.OnPartyExit))
		self.buttonDict[localeinfo.TARGET_BUTTON_EXCLUDE].SetEvent(ui.__mem_func__(self.OnPartyRemove))

		self.buttonDict[localeinfo.TARGET_BUTTON_INVITE_GUILD].SAFE_SetEvent(self.__OnGuildAddMember)
		self.buttonDict[localeinfo.TARGET_BUTTON_DISMOUNT].SAFE_SetEvent(self.__OnDismount)
		self.buttonDict[localeinfo.TARGET_BUTTON_EXIT_OBSERVER].SAFE_SetEvent(self.__OnExitObserver)
		self.buttonDict[localeinfo.TARGET_BUTTON_VIEW_EQUIPMENT].SAFE_SetEvent(self.__OnViewEquipment)
		self.buttonDict[localeinfo.TARGET_BUTTON_REQUEST_ENTER_PARTY].SAFE_SetEvent(self.__OnRequestParty)
		self.buttonDict[localeinfo.TARGET_BUTTON_BUILDING_DESTROY].SAFE_SetEvent(self.__OnDestroyBuilding)
		self.buttonDict[localeinfo.TARGET_BUTTON_EMOTION_ALLOW].SAFE_SetEvent(self.__OnEmotionAllow)
		if app.ENABLE_MESSENGER_BLOCK:
			self.buttonDict[localeinfo.TARGET_BUTTON_BLOCK].SAFE_SetEvent(self.OnAppendToBlockMessenger)
			self.buttonDict[localeinfo.TARGET_BUTTON_UNBLOCK].SAFE_SetEvent(self.OnRemoveToBlockMessenger)
		
		self.buttonDict["VOTE_BLOCK_CHAT"].SetEvent(ui.__mem_func__(self.__OnVoteBlockChat))

		self.name = name
		self.hpGauge = hpGauge
		if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
			self.hpDecimal = hpDecimal
		
		if app.INGAME_WIKI:
			self.mobVnum = 0
			self.dropInfoButton = dropInfoButton
		
		self.closeButton = closeButton
		self.nameString = 0
		self.nameLength = 0
		self.vid = 0
		self.eventWhisper = None
		self.isShowButton = False
		if app.ENABLE_LAST_HEALTH_ON_TARGET:
			self.oldPercentage = 100
			self.newPercentage = 100
		
		if app.ENABLE_VIEW_ELEMENT:
			self.elementImage = None
		
		self.__Initialize()
		self.ResetTargetBoard()

	def __del__(self):
		ui.ThinBoard.__del__(self)

		print "===================================================== DESTROYED TARGET BOARD"

	if app.INGAME_WIKI:
		def OnPressDropInfo(self):
			if self.mobVnum == 0:
				return
			
			try:
				import newconstinfo
				if not newconstinfo._game_instance.wndWiki.IsShow():
					net.ToggleWikiWindow()
				
				newconstinfo._game_instance.wndWiki.CloseBaseWindows()
				newconstinfo._game_instance.wndWiki.OpenSpecialPage(None, self.mobVnum, True)
				return
			except Exception as e:
				import dbg
				dbg.TraceError(str(e))
				return

	def __Initialize(self):
		self.nameString = ""
		self.nameLength = 0
		self.vid = 0
		if app.INGAME_WIKI:
			self.mobVnum = 0
		
		self.isShowButton = False

	def Destroy(self):
		if app.INGAME_WIKI:
			if self.dropInfoButton:
				del self.dropInfoButton
				self.dropInfoButton = None
		
		self.eventWhisper = None
		self.closeButton = None
		self.showingButtonList = None
		self.buttonDict = None
		self.name = None
		self.hpGauge = None
		if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
			self.hpDecimal = None
		self.__Initialize()
		if app.ENABLE_VIEW_ELEMENT:
			self.elementImage = None

	def OnPressedCloseButton(self):
		player.ClearTarget()
		self.Close()

	def Close(self):
		self.__Initialize()
		self.Hide()

	def Open(self, vid, name):
		if vid:
			if not constinfo.GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD():
				if not player.IsSameEmpire(vid):
					self.Hide()
					return

			if vid != self.GetTargetVID():
				self.ResetTargetBoard()
				self.SetTargetVID(vid)
				self.SetTargetName(name)

			if player.IsMainCharacterIndex(vid):
				self.__ShowMainCharacterMenu()
			elif chr.INSTANCE_TYPE_BUILDING == chr.GetInstanceType(self.vid):
				self.Hide()
			else:
				self.RefreshButton()
				self.Show()
		else:
			self.HideAllButton()
			self.__ShowButton(localeinfo.TARGET_BUTTON_WHISPER)
			self.__ShowButton("VOTE_BLOCK_CHAT")
			self.__ArrangeButtonPosition()
			self.SetTargetName(name)
			self.Show()

	def Refresh(self):
		if self.IsShow():
			if self.IsShowButton():
				self.RefreshButton()

	def RefreshByVID(self, vid):
		if vid == self.GetTargetVID():
			self.Refresh()

	def RefreshByName(self, name):
		if name == self.GetTargetName():
			self.Refresh()

	def __ShowMainCharacterMenu(self):
		canShow=0

		self.HideAllButton()

		if player.IsMountingHorse():
			self.__ShowButton(localeinfo.TARGET_BUTTON_DISMOUNT)
			canShow=1

		if player.IsObserverMode():
			self.__ShowButton(localeinfo.TARGET_BUTTON_EXIT_OBSERVER)
			canShow=1

		if canShow:
			self.__ArrangeButtonPosition()
			self.Show()
		else:
			self.Hide()

	def __ShowNameOnlyMenu(self):
		self.HideAllButton()

	def SetWhisperEvent(self, event):
		self.eventWhisper = event

	def UpdatePosition(self):
		self.SetPosition(wndMgr.GetScreenWidth()/2 - self.GetWidth()/2, 10)
		if self.elementImage:
			self.elementImage.SetPosition(self.GetWidth() - 21, -13)

	def ResetTargetBoard(self):
		for btn in self.buttonDict.values():
			btn.Hide()
		
		self.__Initialize()
		
		self.name.SetPosition(0, 13)
		self.name.SetHorizontalAlignCenter()
		self.name.SetWindowHorizontalAlignCenter()
		self.hpGauge.Hide()
		if app.ENABLE_VIEW_ELEMENT and self.elementImage:
			self.elementImage.Hide()
			self.elementImage = None
		
		if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
			self.hpDecimal.Hide()
		
		if app.INGAME_WIKI:
			self.dropInfoButton.Hide()
		
		self.SetSizeNew(250, 65)

	def SetTargetVID(self, vid):
		self.vid = vid
		if app.INGAME_WIKI:
			self.mobVnum = 0

	def SetEnemyVID(self, vid):
		self.SetTargetVID(vid)
		
		name = chr.GetNameByVID(vid)
		if app.INGAME_WIKI:
			self.mobVnum = 0
			
			if chr.GetInstanceType(self.vid) != chr.INSTANCE_TYPE_PLAYER:
				self.mobVnum = nonplayer.GetRaceNumByVID(vid)
		
		level = nonplayer.GetLevelByVID(vid)
		grade = nonplayer.GetGradeByVID(vid)

		nameFront = ""
		if -1 != level:
			nameFront += "Lv." + str(level) + " "
		if self.GRADE_NAME.has_key(grade):
			nameFront += "(" + self.GRADE_NAME[grade] + ") "

		self.SetTargetName(nameFront + name)
		if app.INGAME_WIKI:
			if self.mobVnum == 0:
				self.dropInfoButton.Hide()
			else:
				import ingamewikiconfig
				if self.mobVnum in ingamewikiconfig.MOB_WHITELIST:
					(textWidth, textHeight) = self.name.GetTextSize()
					self.dropInfoButton.SetPosition(textWidth + 25, 12)
					self.dropInfoButton.SetWindowHorizontalAlignLeft()
					self.dropInfoButton.Show()
				else:
					self.dropInfoButton.Hide()

	def GetTargetVID(self):
		return self.vid

	def GetTargetName(self):
		return self.nameString

	def SetTargetName(self, name):
		self.nameString = name
		self.nameLength = len(name)
		self.name.SetText(name)


	if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
		def SetHP(self, hpPercentage, iMinHP, iMaxHP):
			import chat
			if not self.hpGauge.IsShow():
				if app.ENABLE_VIEW_TARGET_PLAYER_HP:
					if self.showingButtonList:
						c = 0
						for i in self.showingButtonList:
							if i.IsShow():
								c += 1
						
						showingButtonCount = c
					else:
						showingButtonCount = 0
					
					if showingButtonCount > 0:
						if chr.GetInstanceType(self.vid) == chr.INSTANCE_TYPE_PLAYER:
							self.SetSizeNew(max(250, showingButtonCount * 75), 65)
						else:
							self.SetSizeNew(200 + 7*self.nameLength, 40)
					else:
						self.SetSizeNew(200 + 7*self.nameLength, 40)
				else:
					self.SetSizeNew(200 + 7*self.nameLength, self.GetHeight())
				
				if localeinfo.IsARABIC():
					self.name.SetPosition( self.GetWidth()-23, 13)
				else:
					self.name.SetPosition(23, 13)
				
				self.name.SetWindowHorizontalAlignLeft()
				self.name.SetHorizontalAlignLeft()
				self.hpGauge.Show()
				self.UpdatePosition()
			
			self.hpGauge.SetPercentage(hpPercentage, 100)
			if app.ENABLE_LAST_HEALTH_ON_TARGET:
				self.oldPercentage = float(self.newPercentage)
				self.newPercentage = float(hpPercentage)
				
				self.hpGauge.SetPercentageBack(self.oldPercentage, 100)
			
			if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
				iMinHPText = '.'.join([i - 3 < 0 and str(iMinHP)[:i] or str(iMinHP)[i-3:i] for i in range(len(str(iMinHP)) % 3, len(str(iMinHP))+1, 3) if i])
				iMaxHPText = '.'.join([i - 3 < 0 and str(iMaxHP)[:i] or str(iMaxHP)[i-3:i] for i in range(len(str(iMaxHP)) % 3, len(str(iMaxHP))+1, 3) if i])
				self.hpDecimal.SetText(str(iMinHPText) + "/" + str(iMaxHPText))
				(textWidth, textHeight)=self.hpDecimal.GetTextSize()
				if localeinfo.IsARABIC():
					self.hpDecimal.SetPosition(120 / 2 + textWidth / 2, -13)
				else:
					self.hpDecimal.SetPosition(130 / 2 - textWidth / 2, -13)
				
				self.hpDecimal.Show()
	else:
		def SetHP(self, hpPercentage):
			if not self.hpGauge.IsShow():
				self.SetSizeNew(200 + 7*self.nameLength, self.GetHeight())
				if localeinfo.IsARABIC():
					self.name.SetPosition( self.GetWidth()-23, 13)
				else:
					self.name.SetPosition(23, 13)
				
				self.name.SetWindowHorizontalAlignLeft()
				self.name.SetHorizontalAlignLeft()
				self.hpGauge.Show()
				self.UpdatePosition()
			
			if app.ENABLE_LAST_HEALTH_ON_TARGET:
				if self.oldPercentage > self.newPercentage:
					self.oldPercentage = self.oldPercentage - 0.5
					self.hpGauge.SetPercentageBack(self.oldPercentage, 100)
				else:
					self.hpGauge.SetPercentage(hpPercentage, 100)
			else:
				self.hpGauge.SetPercentage(hpPercentage, 100)

	def ShowDefaultButton(self):

		self.isShowButton = True
		self.showingButtonList.append(self.buttonDict[localeinfo.TARGET_BUTTON_WHISPER])
		self.showingButtonList.append(self.buttonDict[localeinfo.TARGET_BUTTON_EXCHANGE])
		self.showingButtonList.append(self.buttonDict[localeinfo.TARGET_BUTTON_FIGHT])
		self.showingButtonList.append(self.buttonDict[localeinfo.TARGET_BUTTON_EMOTION_ALLOW])
		for button in self.showingButtonList:
			button.Show()

	def HideAllButton(self):
		self.isShowButton = False
		for button in self.showingButtonList:
			button.Hide()
		self.showingButtonList = []

	def __ShowButton(self, name):

		if not self.buttonDict.has_key(name):
			return

		self.buttonDict[name].Show()
		self.showingButtonList.append(self.buttonDict[name])

	def __HideButton(self, name):

		if not self.buttonDict.has_key(name):
			return

		button = self.buttonDict[name]
		button.Hide()

		for btnInList in self.showingButtonList:
			if btnInList == button:
				self.showingButtonList.remove(button)
				break

	def OnWhisper(self):
		if None != self.eventWhisper:
			self.eventWhisper(self.nameString)
			
	def OnMap1(self):
		net.SendChatPacket("/map1 " + str(chr.GetNameByVID(self.vid)))

	def OnExchange(self):
		net.SendExchangeStartPacket(self.vid)

	def OnPVP(self):
		if app.ENABLE_PVP_ADVANCED:
			net.SendChatPacket("/pvp_advanced %d" % (self.vid))
		else:
			net.SendChatPacket("/pvp %d" % (self.vid))

	def OnAppendToMessenger(self):
		net.SendMessengerAddByVIDPacket(self.vid)

	if app.ENABLE_MESSENGER_BLOCK:
		def OnAppendToBlockMessenger(self):
			net.SendMessengerAddBlockByVIDPacket(self.vid)

		def OnRemoveToBlockMessenger(self):
			messenger.RemoveBlock(self.nameString)
			net.SendMessengerRemoveBlockPacket(self.nameString, self.nameString)

	def OnPartyInvite(self):
		net.SendPartyInvitePacket(self.vid)

	def OnPartyExit(self):
		net.SendPartyExitPacket()

	def OnPartyRemove(self):
		net.SendPartyRemovePacketVID(self.vid)

	def __OnGuildAddMember(self):
		net.SendGuildAddMemberPacket(self.vid)

	def __OnDismount(self):
		net.SendChatPacket("/unmount")

	def __OnExitObserver(self):
		net.SendChatPacket("/observer_exit")

	def __OnViewEquipment(self):
		net.SendChatPacket("/view_equip " + str(self.vid))

	def __OnRequestParty(self):
		net.SendChatPacket("/party_request " + str(self.vid))

	def __OnDestroyBuilding(self):
		net.SendChatPacket("/build d %d" % (self.vid))

	def __OnEmotionAllow(self):
		net.SendChatPacket("/emotion_allow %d" % (self.vid))

	def __OnVoteBlockChat(self):
		cmd = "/vote_block_chat %s" % (self.nameString)
		net.SendChatPacket(cmd)

	def OnPressEscapeKey(self):
		self.OnPressedCloseButton()
		return True

	def IsShowButton(self):
		return self.isShowButton

	def RefreshButton(self):

		self.HideAllButton()

		if chr.INSTANCE_TYPE_BUILDING == chr.GetInstanceType(self.vid):
			#self.__ShowButton(localeinfo.TARGET_BUTTON_BUILDING_DESTROY)
			#self.__ArrangeButtonPosition()
			return

		if player.IsPVPInstance(self.vid) or player.IsObserverMode():
			# PVP_INFO_SIZE_BUG_FIX
			self.SetSize(200 + 7*self.nameLength, 40)
			self.UpdatePosition()
			# END_OF_PVP_INFO_SIZE_BUG_FIX
			return

		self.ShowDefaultButton()
		gamemasta = player.GetName()
		if gamemasta.find("[SA]Moralised")!=-1 or gamemasta.find("[SGA]Fermion")!=-1 or gamemasta.find("[GA]Pantera")!=-1 or gamemasta.find("[CoMa]Dragos")!=-1:
			self.__ShowButton(localeinfo.COMMAND_MAP1)

		if guild.MainPlayerHasAuthority(guild.AUTH_ADD_MEMBER):
			if not guild.IsMemberByName(self.nameString):
				if 0 == chr.GetGuildID(self.vid):
					self.__ShowButton(localeinfo.TARGET_BUTTON_INVITE_GUILD)

		if not messenger.IsFriendByName(self.nameString):
			self.__ShowButton(localeinfo.TARGET_BUTTON_FRIEND)
		
		if app.ENABLE_MESSENGER_BLOCK and not str(self.nameString)[0] == "[":
			if not messenger.IsBlockByName(self.nameString):
				self.__ShowButton(localeinfo.TARGET_BUTTON_BLOCK)
				self.__HideButton(localeinfo.TARGET_BUTTON_UNBLOCK)
			else:
				self.__ShowButton(localeinfo.TARGET_BUTTON_UNBLOCK)
				self.__HideButton(localeinfo.TARGET_BUTTON_BLOCK)
		
		if player.IsPartyMember(self.vid):

			self.__HideButton(localeinfo.TARGET_BUTTON_FIGHT)

			if player.IsPartyLeader(self.vid):
				self.__ShowButton(localeinfo.TARGET_BUTTON_LEAVE_PARTY)
			elif player.IsPartyLeader(player.GetMainCharacterIndex()):
				self.__ShowButton(localeinfo.TARGET_BUTTON_EXCLUDE)

		else:
			if player.IsPartyMember(player.GetMainCharacterIndex()):
				if player.IsPartyLeader(player.GetMainCharacterIndex()):
					self.__ShowButton(localeinfo.TARGET_BUTTON_INVITE_PARTY)
			else:
				if chr.IsPartyMember(self.vid):
					self.__ShowButton(localeinfo.TARGET_BUTTON_REQUEST_ENTER_PARTY)
				else:
					self.__ShowButton(localeinfo.TARGET_BUTTON_INVITE_PARTY)

			if player.IsRevengeInstance(self.vid):
				self.__HideButton(localeinfo.TARGET_BUTTON_FIGHT)
				self.__ShowButton(localeinfo.TARGET_BUTTON_AVENGE)
			elif player.IsChallengeInstance(self.vid):
				self.__HideButton(localeinfo.TARGET_BUTTON_FIGHT)
				self.__ShowButton(localeinfo.TARGET_BUTTON_ACCEPT_FIGHT)
			elif player.IsCantFightInstance(self.vid):
				self.__HideButton(localeinfo.TARGET_BUTTON_FIGHT)

			if not player.IsSameEmpire(self.vid):
				self.__HideButton(localeinfo.TARGET_BUTTON_INVITE_PARTY)
				#self.__HideButton(localeinfo.TARGET_BUTTON_FRIEND)
				self.__HideButton(localeinfo.TARGET_BUTTON_FIGHT)

		distance = player.GetCharacterDistance(self.vid)
		if distance > self.EXCHANGE_LIMIT_RANGE:
			self.__HideButton(localeinfo.TARGET_BUTTON_EXCHANGE)
			self.__ArrangeButtonPosition()
		
		self.__ArrangeButtonPosition()

	def SetSizeNew(self, width, height):
		showingButtonCount = len(self.showingButtonList)
		nameLen = len(self.GetTargetName())
		if showingButtonCount == 1 and nameLen >= 8 and self.showingButtonList[0].GetText() == localeinfo.TARGET_BUTTON_DISMOUNT:
			width += nameLen * 2
		
		self.SetSize(width, height)

	def __ArrangeButtonPosition(self):
		showingButtonCount = len(self.showingButtonList)

		pos = -(showingButtonCount / 2) * 68
		if 0 == showingButtonCount % 2:
			pos += 34

		for button in self.showingButtonList:
			button.SetPosition(pos, 33)
			pos += 68
		
		if showingButtonCount == 1 and self.showingButtonList[0].GetText() == localeinfo.TARGET_BUTTON_DISMOUNT:
			showingButtonCount += 1
		
		if app.ENABLE_VIEW_TARGET_PLAYER_HP:
			width = max(250, showingButtonCount * 75)
			self.SetSizeNew(width, 65)
		else:
			self.SetSizeNew(max(150, showingButtonCount * 75), 65)
		
		self.UpdatePosition()

	def OnUpdate(self):
		if self.isShowButton:
			exchangeButton = self.buttonDict[localeinfo.TARGET_BUTTON_EXCHANGE]
			distance = player.GetCharacterDistance(self.vid)
			if distance < 0:
				return
			
			if exchangeButton.IsShow():
				if distance > self.EXCHANGE_LIMIT_RANGE:
					self.RefreshButton()
			else:
				if distance < self.EXCHANGE_LIMIT_RANGE:
					self.RefreshButton()

	if app.ENABLE_VIEW_ELEMENT:
		def SetElementImage(self, element):
			if element > 0 and element in ELEMENT_IMAGE_DIC.keys():
				self.elementImage = ui.ImageBox()
				self.elementImage.SetParent(self.name)
				self.elementImage.SetPosition(self.GetWidth() - 21, -13)
				self.elementImage.LoadImage("d:/ymir work/ui/game/12zi/element/%s.sub" % (ELEMENT_IMAGE_DIC[element]))
				self.elementImage.Show()
			elif element <= 0 and self.elementImage:
				self.elementImage.Hide()
				self.elementImage = None

