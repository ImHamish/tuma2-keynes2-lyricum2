import app
import player
import net
import pack
import dbg
import chat
import wndMgr
import systemSetting
import snd
import ui
import constinfo
import uiselectmusic
import musicinfo
import localeinfo
import uiscriptlocale
import background
import cfg
if app.USE_ENVIRONMENT_OPTIONS:
	import uiphasecurtain

MUSIC_FILENAME_MAX_LEN=25
blockMode=0
IMG_DIR="d:/ymir work/ui/game/gameoption/"

class GameOptionWindow(ui.BoardWithTitleBar):
	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def Destroy(self):
		self.pvpModeButtonDict={}
		self.blockButtonList=[]
		self.viewChatButtonList=[]
		self.nameColorModeButtonList=[]
		self.viewTargetBoardButtonList=[]
		self.alwaysShowNameButtonList=[]
		
		if app.OUTLINE_NAMES_TEXTLINE:
			self.namesTypeButtonList=[]

		if app.WJ_SHOW_MOB_INFO:
			self.showMobInfoButtonList=[]
		self.renderOptButtonList = []
		self.showDamageButtonList=[]
		self.cameraModeButtonList=[]
		self.ctrlSoundVolume=None
		self.ctrlMusicVolume=None
		self.selectMusicFile=None
		self.musicListDlg=None
		self.loading_distance_btns = []
		if app.ENABLE_PERSPECTIVE_VIEW:
			self.cameraPerspective=None
		
		self.hideModeButtonList=[]
		
		if app.ENABLE_DOGMODE_RENEWAL:
			self.dogModeButtonList=[]
			self.dogMode=0
			self.dogModeApply=None
		
		self.timePmButtonList=[]
		
		self.tilingMode=0
		self.tilingModeButtonList=[]
		self.tilingApplyButton=None
		
		if app.USE_FOG_FIX:
			self.fogButtonList = []
		
		if app.USE_ENVIRONMENT_OPTIONS:
			self.nightModeButtonList = []
			self.snowModeButtonList = []
			self.snowTextureModeButtonList = []
		
		for j in xrange(10):
			for x in xrange(10):
				if self.children.has_key("optionWindow%d%d"%(j,x)):
					window=self.children["optionWindow%d%d"%(j,x)]
					if window:
						window.Hide()
						window.scrollBar=None
						window.ClearDictionary()
						window.Destroy()
		
		self.children={}
		self.Hide()

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.children={}
		self.Destroy()
		self.__LoadWindow()
		self.SetTitleName(uiscriptlocale.GAMEOPTION_TITLE)

	def __LoadWindow(self):
		self.AddFlag("movable")
		self.AddFlag("attach")

		self.renderDistances = [15000.0, 5000.0, 1500.0]
		self.loading_distances = [2, 1.0, 0.0]

		option_data={
			localeinfo.GAME_OPTIONS_MAIN : {
				"index": 0,
				localeinfo.GAME_OPTIONS_MAIN_0:{"index": 0,},
				localeinfo.GAME_OPTIONS_MAIN_1:{"index": 1,},
				localeinfo.GAME_OPTIONS_MAIN_2:{"index": 2,},
			},
			localeinfo.GAME_OPTIONS_MAIN2 : {
			"index": 1,
				localeinfo.GAME_OPTIONS_MAIN_4:{"index": 0,},
				localeinfo.GAME_OPTIONS_MAIN_5:{"index": 1,},
				localeinfo.GAME_OPTIONS_MAIN_6:{"index": 2,},
				localeinfo.GAME_OPTIONS_MAIN_7:{"index": 3,},
			},
		}
		
		for key, optionTitles in option_data.items():
			if self.IsCanContinue(key):
				continue
			
			if not optionTitles.has_key("index"):
				continue
			
			optionIndex=optionTitles["index"]
			
			bg=ui.ImageBox()
			bg.SetParent(self)
			bg.AddFlag("attach")
			bg.SetPosition(8,65)
			bg.LoadImage(IMG_DIR+"bg.tga")
			bg.Show()
			self.children["bg%d"%optionIndex]=bg
			
			titleButton=ui.RadioButton()
			titleButton.SetParent(self)
			if optionIndex == 0:
				titleButton.SetUpVisual(IMG_DIR+"first_title_0.tga")
				titleButton.SetOverVisual(IMG_DIR+"first_title_1.tga")
				titleButton.SetDownVisual(IMG_DIR+"first_title_2.tga")
				titleButton.SetPosition(16+(optionIndex*(titleButton.GetWidth()+5)), 40)
			elif optionIndex == len(option_data)-1:
				titleButton.SetUpVisual(IMG_DIR+"end_title_0.tga")
				titleButton.SetOverVisual(IMG_DIR+"end_title_1.tga")
				titleButton.SetDownVisual(IMG_DIR+"end_title_2.tga")
				titleButton.SetPosition(16+(optionIndex*(titleButton.GetWidth()+5))-5, 40)
			else:
				titleButton.SetUpVisual(IMG_DIR+"middle_title_0.tga")
				titleButton.SetOverVisual(IMG_DIR+"middle_title_1.tga")
				titleButton.SetDownVisual(IMG_DIR+"middle_title_2.tga")
				titleButton.SetPosition(16+(optionIndex*(titleButton.GetWidth()+5)), 40)
			
			titleButton.SetText(key)
			titleButton.SAFE_SetEvent(self.SetOptionType, optionIndex)
			titleButton.Show()
			self.children["titleBtn%d"%optionIndex]=titleButton
			
			for keyEx, optionTypes in optionTitles.items():
				if self.IsCanContinue(keyEx):
					continue
				
				if not optionTypes.has_key("index"):
					continue
				
				categoryIndex=optionTypes["index"]
				optionCategory=ui.RadioButton()
				optionCategory.SetParent(bg)
				optionCategory.SetUpVisual(IMG_DIR+"category_0.tga")
				optionCategory.SetOverVisual(IMG_DIR+"category_1.tga")
				optionCategory.SetDownVisual(IMG_DIR+"category_2.tga")
				optionCategory.SetPosition(7, 10+(categoryIndex*(optionCategory.GetHeight()+5)))
				optionCategory.SAFE_SetEvent(self.SetCategoryType, categoryIndex)
				optionCategory.SetText(keyEx)
				optionCategory.Show()
				self.children["optionCategory%d%d"%(optionIndex,categoryIndex)]=optionCategory
				
				optionWindow=self.CreateOptionWindow(bg, optionIndex,categoryIndex)
				self.children["optionWindow%d%d"%(optionIndex,categoryIndex)]=optionWindow

		self.SetOptionType(0)
		self.SetSize(8+bg.GetWidth()+10,65+bg.GetHeight()+10)
		self.SetCenterPosition()
		self.UpdateScrollBar()

	def Open(self):
		self.RefreshBlock()
		self.Show()

	def OnPressEscapeKey(self):
		self.Hide()
		return True

	def __ClickRadioButton(self, buttonList, buttonIndex):
		try:
			btn=buttonList[buttonIndex]
		except IndexError:
			return
		
		for eachButton in buttonList:
			eachButton.SetUp()
		
		btn.Down()

	def IsCanContinue(self, dictKey):
		if dictKey == "index":
			return True
		
		return False

	def SetOptionType(self, optionType):
		self.children["optionType"]=optionType
		for j in xrange(10):
			if self.children.has_key("titleBtn%d"%j):
				titleBtn=self.children["titleBtn%d"%j]
				if j == optionType:
					titleBtn.Down()
				else:
					titleBtn.SetUp()
			
			if self.children.has_key("bg%d"%j):
				bg=self.children["bg%d"%j]
				if j == optionType:
					bg.Show()
				else:
					bg.Hide()
		
		self.SetCategoryType(0)

	def SetCategoryType(self, categoryType):
		for j in xrange(3):
			for x in xrange(5):
				if self.children.has_key("optionWindow%d%d" % (j,x)):
					self.children["optionWindow%d%d"%(j,x)].Hide()
		
		self.children["categoryType"]=categoryType
		currentCategoryType=self.children["optionType"]
		
		for j in xrange(10):
			if self.children.has_key("optionCategory%d%d"%(currentCategoryType,j)):
				categoryBtn=self.children["optionCategory%d%d"%(currentCategoryType,j)]
				if j == categoryType:
					categoryBtn.Down()
				else:
					categoryBtn.SetUp()
		
		if self.children.has_key("optionWindow%d%d" % (currentCategoryType,categoryType)):
			self.children["optionWindow%d%d"%(currentCategoryType,categoryType)].Show()
		
		self.UpdateScrollBar()

	def CreateOptionWindow(self, windowBG, optionIndex, categoryIndex):
		scriptWindow=ui.ScriptWindow()
		scriptWindow.SetParent(windowBG)
		try:
			fileName = "uiscript/game_option_%d_%d.py"%(optionIndex, categoryIndex)
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(scriptWindow, fileName)
			self.LoadCatetoryOptions(scriptWindow, optionIndex, categoryIndex)
		except Exception as e:
			import dbg
			dbg.TraceError(str(e))
		
		if scriptWindow.GetWidth() > 0:
			scrollBar=ScrollBarNew()
			scrollBar.SetParent(scriptWindow)
			scrollBar.SetPosition(scriptWindow.GetWidth()-scrollBar.GetWidth(),0)
			scrollBar.SetScrollBarSize(scriptWindow.GetHeight()-5)
			scrollBar.Hide()
			scrollBar.SetScrollEvent(ui.__mem_func__(self.UpdateScrollBar))
			scriptWindow.scrollBar=scrollBar
			scriptWindow.OnRunMouseWheel=ui.__mem_func__(self.OnRunMouseWheel)
			scriptChildrens=scriptWindow.Children
			
			for child in scriptChildrens:
				(_x,_y)=child.GetLocalPosition()
				child.SetPosition(_x,_y, True)
			
			scriptWindow.Show()
		else:
			scriptWindow.scrollBar=None
		
		return scriptWindow

	def LoadCatetoryOptions(self, scriptWindow, optionIndex, categoryIndex):
		GetObject=scriptWindow.GetChild
		if optionIndex == 0:
			if categoryIndex == 0:
				self.pvpModeButtonDict[player.PK_MODE_PEACE]=GetObject("pvp_peace")
				self.pvpModeButtonDict[player.PK_MODE_PEACE].SAFE_SetEvent(self.__OnClickPvPModePeaceButton)
				self.pvpModeButtonDict[player.PK_MODE_REVENGE]=GetObject("pvp_revenge")
				self.pvpModeButtonDict[player.PK_MODE_REVENGE].SAFE_SetEvent(self.__OnClickPvPModeRevengeButton)
				self.pvpModeButtonDict[player.PK_MODE_GUILD]=GetObject("pvp_guild")
				self.pvpModeButtonDict[player.PK_MODE_GUILD].SAFE_SetEvent(self.__OnClickPvPModeGuildButton)
				self.pvpModeButtonDict[player.PK_MODE_FREE]=GetObject("pvp_free")
				self.pvpModeButtonDict[player.PK_MODE_FREE].SAFE_SetEvent(self.__OnClickPvPModeFreeButton)
				self.__SetPeacePKMode()
				
				block_exchange_button=GetObject("block_exchange_button")
				block_exchange_button.SetToggleUpEvent(self.__OnClickBlockExchangeButton)
				block_exchange_button.SetToggleDownEvent(self.__OnClickBlockExchangeButton)
				self.blockButtonList.append(block_exchange_button)
				
				block_party_button=GetObject("block_party_button")
				block_party_button.SetToggleUpEvent(self.__OnClickBlockPartyButton)
				block_party_button.SetToggleDownEvent(self.__OnClickBlockPartyButton)
				self.blockButtonList.append(block_party_button)
				
				block_guild_button=GetObject("block_guild_button")
				block_guild_button.SetToggleUpEvent(self.__OnClickBlockGuildButton)
				block_guild_button.SetToggleDownEvent(self.__OnClickBlockGuildButton)
				self.blockButtonList.append(block_guild_button)
				
				block_whisper_button=GetObject("block_whisper_button")
				block_whisper_button.SetToggleUpEvent(self.__OnClickBlockWhisperButton)
				block_whisper_button.SetToggleDownEvent(self.__OnClickBlockWhisperButton)
				self.blockButtonList.append(block_whisper_button)
				
				block_friend_button=GetObject("block_friend_button")
				block_friend_button.SetToggleUpEvent(self.__OnClickBlockFriendButton)
				block_friend_button.SetToggleDownEvent(self.__OnClickBlockFriendButton)
				self.blockButtonList.append(block_friend_button)
				
				block_party_request_button=GetObject("block_party_request_button")
				block_party_request_button.SetToggleUpEvent(self.__OnClickBlockPartyRequest)
				block_party_request_button.SetToggleDownEvent(self.__OnClickBlockPartyRequest)
				self.blockButtonList.append(block_party_request_button)
				
				self.RefreshBlock()
				
				self.viewChatButtonList.append(GetObject("view_chat_on_button"))
				self.viewChatButtonList[0].SAFE_SetEvent(self.__OnClickViewChat)
				self.viewChatButtonList.append(GetObject("view_chat_off_button"))
				self.viewChatButtonList[1].SAFE_SetEvent(self.__OnClickViewChat)
				self.RefreshViewChat()
				
				self.nameColorModeButtonList.append(GetObject("name_color_normal"))
				self.nameColorModeButtonList[0].SAFE_SetEvent(self.__OnClickNameColorMode)
				self.nameColorModeButtonList.append(GetObject("name_color_empire"))
				self.nameColorModeButtonList[1].SAFE_SetEvent(self.__OnClickNameColorMode)
				self.RefreshNameColor()
				
				self.viewTargetBoardButtonList.append(GetObject("target_board_view"))
				self.viewTargetBoardButtonList[0].SAFE_SetEvent(self.__SetTargetBoardViewMode)
				self.viewTargetBoardButtonList.append(GetObject("target_board_no_view"))
				self.viewTargetBoardButtonList[1].SAFE_SetEvent(self.__SetTargetBoardViewMode)
				self.RefreshTargetBoard()
			elif categoryIndex == 1:
				self.cameraModeButtonList.append(GetObject("camera_level0"))
				self.cameraModeButtonList[0].SAFE_SetEvent(self.__OnClickCameraMode, 0)
				self.cameraModeButtonList.append(GetObject("camera_level1"))
				self.cameraModeButtonList[1].SAFE_SetEvent(self.__OnClickCameraMode, 1)
				self.cameraModeButtonList.append(GetObject("camera_level2"))
				self.cameraModeButtonList[2].SAFE_SetEvent(self.__OnClickCameraMode, 2)
				if app.ENABLE_SAVECAMERA_PREFERENCES:
					self.__OnClickCameraMode(systemSetting.GetCameraType())
				else:
					self.RefreshCameraMode()
				
				if app.ENABLE_PERSPECTIVE_VIEW:
					cameraPerspective=GetObject("camera_perspective_controller")
					cameraPerspective.SetSliderPos(float(systemSetting.GetFieldPerspective() / 100))
					cameraPerspective.SetEvent(ui.__mem_func__(self.OnChangeFieldPerspective))
					self.cameraPerspective=cameraPerspective
				else:
					GetObject("camera_perspective_window").Hide()
			elif categoryIndex == 2:
				ctrlSoundVolume=GetObject("sound_volume_controller")
				ctrlSoundVolume.SetSliderPos(float(systemSetting.GetSoundVolume()) / 5.0)
				ctrlSoundVolume.SetEvent(ui.__mem_func__(self.OnChangeSoundVolume))
				self.ctrlSoundVolume=ctrlSoundVolume
				
				ctrlMusicVolume=GetObject("music_volume_controller")
				ctrlMusicVolume.SetSliderPos(float(systemSetting.GetMusicVolume()))
				ctrlMusicVolume.SetEvent(ui.__mem_func__(self.OnChangeMusicVolume))
				self.ctrlMusicVolume=ctrlMusicVolume
				GetObject("bgm_button").SAFE_SetEvent(self.__OnClickChangeMusicButton)
				self.selectMusicFile=GetObject("bgm_file")
		elif optionIndex == 1:
			if categoryIndex == 0:
				self.tilingModeButtonList.append(GetObject("tiling_cpu"))
				self.tilingModeButtonList.append(GetObject("tiling_gpu"))
				self.tilingModeButtonList[0].SAFE_SetEvent(self.__OnClickTilingModeCPUButton)
				self.tilingModeButtonList[1].SAFE_SetEvent(self.__OnClickTilingModeGPUButton)
				self.tilingApplyButton=GetObject("tiling_apply")
				self.tilingApplyButton.SAFE_SetEvent(self.__OnClickTilingApplyButton)
				self.__SetCurTilingMode()
				
				self.showDamageButtonList.append(GetObject("show_damage_on_button"))
				self.showDamageButtonList[0].SAFE_SetEvent(self.__OnClickShowDamage)
				self.showDamageButtonList.append(GetObject("show_damage_off_button"))
				self.showDamageButtonList[1].SAFE_SetEvent(self.__OnClickShowDamage)
				self.RefreshShowDamage()

			elif categoryIndex == 1:
				self.timePmButtonList.append(GetObject("time_pm_on"))
				self.timePmButtonList.append(GetObject("time_pm_off"))
				self.timePmButtonList[0].SAFE_SetEvent(self.__OnClickTimePmOnButton)
				self.timePmButtonList[1].SAFE_SetEvent(self.__OnClickTimePmOffButton)
				self.RefreshTimePm()
			elif categoryIndex == 2:
				self.alwaysShowNameButtonList.append(GetObject("always_show_name_on_button"))
				self.alwaysShowNameButtonList[0].SAFE_SetEvent(self.__OnClickAlwaysShowName)
				self.alwaysShowNameButtonList.append(GetObject("always_show_name_off_button"))
				self.alwaysShowNameButtonList[1].SAFE_SetEvent(self.__OnClickAlwaysShowName)
				self.RefreshAlwaysShowName()
				
				if app.OUTLINE_NAMES_TEXTLINE:
					self.namesTypeButtonList.append(GetObject("name_type1_button"))
					self.namesTypeButtonList.append(GetObject("name_type2_button"))
					self.namesTypeButtonList[0].SAFE_SetEvent(self.__OnClickNamesType1)
					self.namesTypeButtonList[1].SAFE_SetEvent(self.__OnClickNamesType2)
					self.RefreshNamesType()
				else:
					GetObject("name_type_window").Hide()

				self.renderOptButtonList.append(GetObject("effect_range_low"))
				self.renderOptButtonList.append(GetObject("effect_range_medium"))
				self.renderOptButtonList.append(GetObject("effect_range_high"))
				for i in xrange(len(self.renderOptButtonList)):
					self.renderOptButtonList[i].SetToggleUpEvent(lambda arg = i : ui.__mem_func__(self.__OnClickChangeRenderOpt)(arg))
					self.renderOptButtonList[i].SetToggleDownEvent(lambda arg = i : ui.__mem_func__(self.__OnClickChangeRenderOpt)(arg))
				self.RefreshRenderOpt()

				self.loading_distance_btns.append(GetObject("loading_distance_min"))
				self.loading_distance_btns.append(GetObject("loading_distance_mid"))
				self.loading_distance_btns.append(GetObject("loading_distance_max"))

				for i in xrange(len(self.loading_distance_btns)):
					self.loading_distance_btns[i].SetToggleUpEvent(lambda arg = i : ui.__mem_func__(self.OnChangeLoadingDistance)(arg))
					self.loading_distance_btns[i].SetToggleDownEvent(lambda arg = i : ui.__mem_func__(self.OnChangeLoadingDistance)(arg))
				self.RefreshLoadingDistance()

				if app.WJ_SHOW_MOB_INFO:
					self.showMobInfoButtonList.append(GetObject("show_mob_level_button"))
					self.showMobInfoButtonList.append(GetObject("show_mob_AI_flag_button"))
					self.showMobInfoButtonList[0].SetToggleUpEvent(self.__OnClickShowMobLevelButton)
					self.showMobInfoButtonList[0].SetToggleDownEvent(self.__OnClickShowMobLevelButton)
					self.showMobInfoButtonList[1].SetToggleUpEvent(self.__OnClickShowMobAIFlagButton)
					self.showMobInfoButtonList[1].SetToggleDownEvent(self.__OnClickShowMobAIFlagButton)
					self.RefreshShowMobInfo()
				else:
					GetObject("monster_info_window").Hide()
				
				if app.ENABLE_DOGMODE_RENEWAL:
					self.dogModeButtonList=[]
					self.dogModeButtonList.append(GetObject("dog_mode_0"))
					self.dogModeButtonList.append(GetObject("dog_mode_1"))
					self.dogModeButtonList[0].SAFE_SetEvent(self.__OnClickDogModeOnButton)
					self.dogModeButtonList[1].SAFE_SetEvent(self.__OnClickDogModeOffButton)
					self.dogModeApply=GetObject("dogmode_apply")
					self.dogModeApply.SAFE_SetEvent(self.__OnClickDogModeApplyButton)
					self.RefreshDogMode()
				else:
					GetObject("dog_mode_window").Hide()
				
				for i in xrange(7):
					self.hideModeButtonList.append(GetObject("hidemode1_%d" % i))
					self.hideModeButtonList[i].SetToggleUpEvent(self.__OnClickHideOptionUp, i)
					self.hideModeButtonList[i].SetToggleDownEvent(self.__OnClickHideOptionDown, i)
				
				GetObject("hidemode1_2").Hide()
				GetObject("hidemode1_4").Hide()
				
				self.RefreshHideMode()
			elif categoryIndex == 3:
				if app.USE_FOG_FIX:
					self.fogButtonList.append(GetObject("fog_off"))
					self.fogButtonList.append(GetObject("fog_on"))
					self.fogButtonList[0].SAFE_SetEvent(self.__OnClickFogModeOffButton)
					self.fogButtonList[1].SAFE_SetEvent(self.__OnClickFogModeOnButton)
					self.RefreshFogMode()
				else:
					GetObject("fog_mode_window").Hide()
				
				if app.USE_ENVIRONMENT_OPTIONS:
					self.curtain = uiphasecurtain.PhaseCurtain()
					self.curtain.speed = 0.03
					self.curtain.Hide()
					
					self.nightModeButtonList.append(GetObject("night_mode_off"))
					self.nightModeButtonList.append(GetObject("night_mode_on"))
					self.nightModeButtonList[0].SAFE_SetEvent(self.__OnClickNightModeOffButton)
					self.nightModeButtonList[1].SAFE_SetEvent(self.__OnClickNightModeOnButton)
					self.RefreshNightMode()
					
					self.snowModeButtonList.append(GetObject("snow_mode_off"))
					self.snowModeButtonList.append(GetObject("snow_mode_on"))
					self.snowModeButtonList[0].SAFE_SetEvent(self.__OnClickSnowModeOffButton)
					self.snowModeButtonList[1].SAFE_SetEvent(self.__OnClickSnowModeOnButton)
					self.RefreshSnowMode()
					
					self.snowTextureModeButtonList.append(GetObject("snowtexture_mode_off"))
					self.snowTextureModeButtonList.append(GetObject("snowtexture_mode_on"))
					self.snowTextureModeButtonList[0].SAFE_SetEvent(self.__OnClickSnowTextureModeOffButton)
					self.snowTextureModeButtonList[1].SAFE_SetEvent(self.__OnClickSnowTextureModeOnButton)
					self.RefreshSnowTextureMode()
				else:
					GetObject("night_mode_window").Hide()
					GetObject("snow_mode_window").Hide()
					GetObject("snowtexture_mode_window").Hide()

	def GetCurrentWindow(self):
		if self.children.has_key("optionType") and self.children.has_key("categoryType"):
			(optionType,categoryType)=(self.children["optionType"],self.children["categoryType"])
			if self.children.has_key("optionWindow%d%d"%(optionType,categoryType)):
				return self.children["optionWindow%d%d"%(optionType,categoryType)]
		
		return None

	def GetCurrentScrollBar(self):
		window=self.GetCurrentWindow()
		if window != None:
			return window.scrollBar
		
		return None

	def OnRunMouseWheel(self, nLen):
		if self.IsInPosition():
			scrollBar=self.GetCurrentScrollBar()
			if scrollBar:
				if scrollBar.IsShow():
					if nLen > 0:
						scrollBar.OnUp()
					else:
						scrollBar.OnDown()
					return True
		
		return False

	def UpdateScrollBar(self):
		window=self.GetCurrentWindow()
		if window == None:
			return
		
		scrollBar=window.scrollBar
		if scrollBar == None:
			return
		
		scriptChildrens=window.Children
		if len(scriptChildrens) == 0:
			return
		
		windowHeight=window.GetHeight()
		
		# OLD SHIT
		#lastChildren=scriptChildrens[len(scriptChildrens)-1]
		#screenSize=lastChildren.exPos[1]+lastChildren.GetHeight()
		
		screenSize=0
		for child in scriptChildrens:
			childHeight=child.exPos[1] + child.GetHeight()
			if childHeight > screenSize:
				screenSize=childHeight
		
		if screenSize > windowHeight:
			scrollLen=screenSize-windowHeight
			basePos=int(scrollBar.GetPos()*scrollLen)
			for child in scriptChildrens:
				(_x,_y)=child.exPos
				child.SetPosition(_x,_y-basePos)
			
			scrollBar.SetMiddleBarSize(float(windowHeight-5)/float(screenSize))
			scrollBar.Show()
		else:
			scrollBar.Hide()
		
		_wy=window.GetGlobalPosition()[1]
		elementDictionary=window.ElementDictionary
		for childName, child in elementDictionary.items():
			(_x,_y)=child.GetGlobalPosition()
			childHeight=child.GetHeight()
			textLines=[]
			images=[]
			if isinstance(child, ui.ExpandedImageBox):
				images.append(child)
			elif isinstance(child, ui.TextLine):
				textLines.append(child)
			elif isinstance(child, ui.ToggleButton) or isinstance(child, ui.RadioButton):
				if child.ButtonText != None:
					textLines.append(child.ButtonText)
				images.append(child)
			elif isinstance(child, ui.SliderBar):
				if child.backGroundImage != None:
					images.append(child.backGroundImage)
				if child.cursor != None:
					images.append(child.cursor)
			
			for childItem in textLines:
				if _y < _wy:
					if childItem.IsShow():
						childItem.Hide()
				elif _y > (_wy+windowHeight-20):
					if childItem.IsShow():
						childItem.Hide()
				else:
					if not childItem.IsShow():
						childItem.Show()
			
			for childItem in images:
				if _y < _wy:
					childItem.SetRenderingRect(0,ui.calculateRect(childHeight-abs(_y-_wy),childHeight),0,0)
				elif _y+childHeight > (_wy+windowHeight-4):
					calculate=(_wy+windowHeight-4) - (_y+childHeight)
					if calculate == 0:
						return
					f=ui.calculateRect(childHeight-abs(calculate),childHeight)
					childItem.SetRenderingRect(0,0,0,f)
				else:
					childItem.SetRenderingRect(0,0,0,0)

	def __SetPeacePKMode(self):
		self.__SetPKMode(player.PK_MODE_PEACE)

	def __SetPKMode(self, mode):
		for btn in self.pvpModeButtonDict.values():
			btn.SetUp()
		
		if self.pvpModeButtonDict.has_key(mode):
			self.pvpModeButtonDict[mode].Down()

	def __CheckPvPProtectedLevelPlayer(self):
		if player.GetStatus(player.LEVEL)<constinfo.PVPMODE_PROTECTED_LEVEL:
			self.__SetPeacePKMode()
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OPTION_PVPMODE_PROTECT % (constinfo.PVPMODE_PROTECTED_LEVEL))
			return True
		
		return False

	def __RefreshPVPButtonList(self):
		self.__SetPKMode(player.GetPKMode())

	def __OnClickPvPModePeaceButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return
		
		self.__RefreshPVPButtonList()
		
		if constinfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 0", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OPTION_PVPMODE_NOT_SUPPORT)

	def __OnClickPvPModeRevengeButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return
		
		self.__RefreshPVPButtonList()
		if constinfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 1", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OPTION_PVPMODE_NOT_SUPPORT)

	def __OnClickPvPModeFreeButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return
		
		self.__RefreshPVPButtonList()
		
		if constinfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 2", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OPTION_PVPMODE_NOT_SUPPORT)

	def __OnClickPvPModeGuildButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return
		
		self.__RefreshPVPButtonList()
		if 0 == player.GetGuildID():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OPTION_PVPMODE_CANNOT_SET_GUILD_MODE)
			return
		
		if constinfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 4", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OPTION_PVPMODE_NOT_SUPPORT)

	def OnChangePKMode(self):
		self.__RefreshPVPButtonList()

	def RefreshBlock(self):
		global blockMode
		for i in xrange(len(self.blockButtonList)):
			if 0 != (blockMode & (1 << i)):
				self.blockButtonList[i].Down()
			else:
				self.blockButtonList[i].SetUp()

	def OnBlockMode(self, mode):
		global blockMode
		blockMode=mode
		self.RefreshBlock()

	def __OnClickBlockExchangeButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_EXCHANGE))

	def __OnClickBlockPartyButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_PARTY))

	def __OnClickBlockGuildButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_GUILD))

	def __OnClickBlockWhisperButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_WHISPER))

	def __OnClickBlockFriendButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_FRIEND))

	def __OnClickBlockPartyRequest(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_PARTY_REQUEST))

	def RefreshViewChat(self):
		if systemSetting.IsViewChat():
			self.viewChatButtonList[0].Down()
			self.viewChatButtonList[1].SetUp()
		else:
			self.viewChatButtonList[0].SetUp()
			self.viewChatButtonList[1].Down()

	def __OnClickViewChat(self):
		systemSetting.SetViewChatFlag(not systemSetting.IsViewChat())
		self.RefreshViewChat()

	def __OnClickNameColorMode(self):
		constinfo.SET_CHRNAME_COLOR_INDEX(not constinfo.GET_CHRNAME_COLOR_INDEX())
		self.RefreshNameColor()

	def RefreshNameColor(self):
		index=constinfo.GET_CHRNAME_COLOR_INDEX()
		self.nameColorModeButtonList[index].Down()
		self.nameColorModeButtonList[not index].SetUp()

	def RefreshTargetBoard(self):
		index=constinfo.GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD()
		self.viewTargetBoardButtonList[index].Down()
		self.viewTargetBoardButtonList[not index].SetUp()

	def __SetTargetBoardViewMode(self):
		constinfo.SET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD(not constinfo.GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD())
		self.RefreshTargetBoard()

	if app.OUTLINE_NAMES_TEXTLINE:
		def RefreshNamesType(self):
			if not systemSetting.GetNamesType():
				self.namesTypeButtonList[0].Down()
				self.namesTypeButtonList[1].SetUp()
			else:
				self.namesTypeButtonList[0].SetUp()
				self.namesTypeButtonList[1].Down()

		def __OnClickNamesType1(self):
			systemSetting.SetNamesType(False)
			self.RefreshNamesType()

		def __OnClickNamesType2(self):
			systemSetting.SetNamesType(True)
			self.RefreshNamesType()

	if app.WJ_SHOW_MOB_INFO:
		def RefreshShowMobInfo(self):
			if systemSetting.IsShowMobLevel():
				self.showMobInfoButtonList[0].Down()
			else:
				self.showMobInfoButtonList[0].SetUp()
			if systemSetting.IsShowMobAIFlag():
				self.showMobInfoButtonList[1].Down()
			else:
				self.showMobInfoButtonList[1].SetUp()

		def __OnClickShowMobLevelButton(self):
			systemSetting.SetShowMobLevel(not systemSetting.IsShowMobLevel())
			self.RefreshShowMobInfo()

		def __OnClickShowMobAIFlagButton(self):
			systemSetting.SetShowMobAIFlag(not systemSetting.IsShowMobAIFlag())
			self.RefreshShowMobInfo()

	if app.ENABLE_DOGMODE_RENEWAL:
		def RefreshDogMode(self, mode = -1):
			if mode == -1:
				dog = systemSetting.IsDogMode()
			else:
				dog = mode
			
			self.dogMode = dog
			
			if self.dogMode == 1:
				self.dogModeButtonList[0].Down()
				self.dogModeButtonList[1].SetUp()
			else:
				self.dogModeButtonList[0].SetUp()
				self.dogModeButtonList[1].Down()

		def __OnClickDogModeOnButton(self):
			self.RefreshDogMode(1)

		def __OnClickDogModeOffButton(self):
			self.RefreshDogMode(0)

		def __OnClickDogModeApplyButton(self):
			self.dogModeApply.SetUp()
			
			if systemSetting.IsDogMode():
				if self.dogMode == 1:
					self.__NotifyChatLine(localeinfo.SYSTEM_OPTION_DOGMODE_ALREADY)
					return
			else:
				if self.dogMode == 0:
					self.__NotifyChatLine(localeinfo.SYSTEM_OPTION_DOGMODE_ALREADY)
					return
			
			systemSetting.SetDogMode(self.dogMode)
			self.__NotifyChatLine(localeinfo.SYSTEM_OPTION_DOGMODE_EXIT)
			constinfo.restart = 1
			constinfo.dogmode = 1
			net.LogOutGame()

		def OnCancelDogMode(self):
			if self.dogMode == 0:
				systemSetting.SetDogMode(1)
				self.__OnClickDogModeOnButton()
			else:
				systemSetting.SetDogMode(0)
				self.__OnClickDogModeOffButton()
			
			constinfo.restart = 0
			constinfo.dogmode = -1

	def __OnClickHideOptionUp(self, arg):
		systemSetting.SetHideModeStatus(arg, 0)
		
		if arg == 4:
			import uiprivateshopbuilder
			uiprivateshopbuilder.UpdateADBoard()

	def RefreshHideMode(self):
		(b1, b2, b3, b4, b5, b6, b7)=systemSetting.GetHideModeStatus()
		if b1:
			self.hideModeButtonList[0].Down()
		else:
			self.hideModeButtonList[0].SetUp()
		
		if b2:
			self.hideModeButtonList[1].Down()
		else:
			self.hideModeButtonList[1].SetUp()
		
		if b3:
			self.hideModeButtonList[2].Down()
		else:
			self.hideModeButtonList[2].SetUp()
		
		if b4:
			self.hideModeButtonList[3].Down()
		else:
			self.hideModeButtonList[3].SetUp()
		
		if b5:
			self.hideModeButtonList[4].Down()
			import uiprivateshopbuilder
			uiprivateshopbuilder.UpdateADBoard()
		else:
			self.hideModeButtonList[4].SetUp()
		
		if b6:
			self.hideModeButtonList[5].Down()
		else:
			self.hideModeButtonList[5].SetUp()
		
		if b7:
			self.hideModeButtonList[6].Down()
		else:
			self.hideModeButtonList[6].SetUp()

	def __OnClickHideOptionDown(self, arg):
		systemSetting.SetHideModeStatus(arg, 1)
		
		if arg == 4:
			import uiprivateshopbuilder
			uiprivateshopbuilder.UpdateADBoard()

	def __OnClickHideOptionUp2(self, arg):
		systemSetting.SetHideModeStatus2(arg, 0)

	def __OnClickHideOptionDown2(self, arg):
		systemSetting.SetHideModeStatus2(arg, 1)

	if app.USE_FOG_FIX:
		def RefreshFogMode(self):
			if background.GetFogMode():
				self.fogButtonList[1].Down()
				self.fogButtonList[0].SetUp()
			else:
				self.fogButtonList[1].SetUp()
				self.fogButtonList[0].Down()

		def __OnClickFogModeOnButton(self):
			systemSetting.SetFogMode(True)
			background.SetFogMode(True)
			self.RefreshFogMode()

		def __OnClickFogModeOffButton(self):
			systemSetting.SetFogMode(False)
			background.SetFogMode(False)
			self.RefreshFogMode()

	if app.USE_ENVIRONMENT_OPTIONS:
		def __DayMode_OnCompleteChangeToLight(self):
			background.ChangeEnvironmentData(background.DAY_MODE_LIGHT)
			self.curtain.FadeIn()

		def __DayMode_OnCompleteChangeToDark(self):
			background.RegisterEnvironmentData(background.DAY_MODE_DARK, constinfo.ENVIRONMENT_NIGHT)
			background.ChangeEnvironmentData(background.DAY_MODE_DARK)
			self.curtain.FadeIn()

		def __SetNightMode(self, index):
			systemSetting.SetNightModeOption(index)
			if not background.GetDayMode():
				return
			
			if not background.IsBoomMap():
				return
			
			if 1 == index:
				self.curtain.SAFE_FadeOut(self.__DayMode_OnCompleteChangeToDark)
			else:
				self.curtain.SAFE_FadeOut(self.__DayMode_OnCompleteChangeToLight)
			
			self.RefreshNightMode()

		def RefreshNightMode(self):
			if systemSetting.GetNightModeOption():
				self.nightModeButtonList[1].Down()
				self.nightModeButtonList[0].SetUp()
			else:
				self.nightModeButtonList[1].SetUp()
				self.nightModeButtonList[0].Down()

		def __OnClickNightModeOffButton(self):
			self.__SetNightMode(0)
			self.RefreshNightMode()

		def __OnClickNightModeOnButton(self):
			self.__SetNightMode(1)
			self.RefreshNightMode()

		def __OnClickSnowModeOffButton(self):
			systemSetting.SetSnowModeOption(0)
			self.RefreshSnowMode()

		def __OnClickSnowModeOnButton(self):
			systemSetting.SetSnowModeOption(1)
			self.RefreshSnowMode()

		def RefreshSnowMode(self):
			if systemSetting.GetSnowModeOption():
				self.snowModeButtonList[1].Down()
				self.snowModeButtonList[0].SetUp()
			else:
				self.snowModeButtonList[1].SetUp()
				self.snowModeButtonList[0].Down()

		def __OnClickSnowTextureModeOffButton(self):
			systemSetting.SetSnowTextureModeOption(0)
			self.RefreshSnowTextureMode()

		def __OnClickSnowTextureModeOnButton(self):
			systemSetting.SetSnowTextureModeOption(1)
			self.RefreshSnowTextureMode()

		def RefreshSnowTextureMode(self):
			if systemSetting.GetSnowTextureModeOption():
				self.snowTextureModeButtonList[1].Down()
				self.snowTextureModeButtonList[0].SetUp()
			else:
				self.snowTextureModeButtonList[1].SetUp()
				self.snowTextureModeButtonList[0].Down()

	def RefreshTimePm(self):
		if systemSetting.GetTimePm():
			self.timePmButtonList[0].Down()
			self.timePmButtonList[1].SetUp()
		else:
			self.timePmButtonList[0].SetUp()
			self.timePmButtonList[1].Down()

	def __OnClickTimePmOnButton(self):
		systemSetting.SetTimePm(True)
		self.RefreshTimePm()

	def __OnClickTimePmOffButton(self):
		systemSetting.SetTimePm(False)
		self.RefreshTimePm()

	def RefreshAlwaysShowName(self):
		index=systemSetting.IsAlwaysShowName()
		self.alwaysShowNameButtonList[index].SetUp()
		self.alwaysShowNameButtonList[not index].Down()

	def __OnClickAlwaysShowName(self):
		systemSetting.SetAlwaysShowNameFlag(not systemSetting.IsAlwaysShowName())
		self.RefreshAlwaysShowName()

	def __NotifyChatLine(self, text):
		chat.AppendChat(chat.CHAT_TYPE_INFO, text)

	def __SetTilingMode(self, index):
		self.__ClickRadioButton(self.tilingModeButtonList, index)
		self.tilingMode=index

	def __OnClickTilingModeCPUButton(self):
		self.__NotifyChatLine(localeinfo.SYSTEM_OPTION_CPU_TILING_1)
		self.__NotifyChatLine(localeinfo.SYSTEM_OPTION_CPU_TILING_2)
		self.__NotifyChatLine(localeinfo.SYSTEM_OPTION_CPU_TILING_3)
		self.__SetTilingMode(0)

	def __OnClickTilingModeGPUButton(self):
		self.__NotifyChatLine(localeinfo.SYSTEM_OPTION_GPU_TILING_1)
		self.__NotifyChatLine(localeinfo.SYSTEM_OPTION_GPU_TILING_2)
		self.__NotifyChatLine(localeinfo.SYSTEM_OPTION_GPU_TILING_3)
		self.__SetTilingMode(1)

	def __SetCurTilingMode(self):
		import background
		if background.IsSoftwareTiling():
			self.__SetTilingMode(0)
		else:
			self.__SetTilingMode(1)

	def __OnClickTilingApplyButton(self):
		self.__NotifyChatLine(localeinfo.SYSTEM_OPTION_TILING_EXIT)
		import background
		if 0 == self.tilingMode:
			background.EnableSoftwareTiling(1)
		else:
			background.EnableSoftwareTiling(0)
		
		constinfo.restart=1
		net.LogOutGame()

	def RefreshShowDamage(self):
		index=systemSetting.IsShowDamage()
		self.showDamageButtonList[index].SetUp()
		self.showDamageButtonList[not index].Down()

	def __OnClickShowDamage(self):
		systemSetting.SetShowDamageFlag(not systemSetting.IsShowDamage())
		self.RefreshShowDamage()

	def OnChangeSoundVolume(self):
		pos=self.ctrlSoundVolume.GetSliderPos()
		snd.SetSoundVolumef(pos)
		systemSetting.SetSoundVolumef(pos)

	def OnChangeMusicVolume(self):
		pos=self.ctrlMusicVolume.GetSliderPos()
		snd.SetMusicVolume(pos * net.GetFieldMusicVolume())
		systemSetting.SetMusicVolume(pos)

	def __OnChangeMusic(self, fileName):
		self.selectMusicFile.SetText(fileName[:MUSIC_FILENAME_MAX_LEN])
		if musicinfo.fieldMusic != "":
			snd.FadeOutMusic("bgm/"+ musicinfo.fieldMusic)
		if fileName==uiselectmusic.DEFAULT_THEMA:
			musicinfo.fieldMusic=musicinfo.METIN2THEMA
		else:
			musicinfo.fieldMusic=fileName
		
		musicinfo.SaveLastPlayFieldMusic()
		if musicinfo.fieldMusic != "":
			snd.FadeInMusic("bgm/" + musicinfo.fieldMusic)

	def __OnClickChangeMusicButton(self):
		if not self.musicListDlg:
			self.musicListDlg=uiselectmusic.FileListDialog()
			self.musicListDlg.SAFE_SetSelectEvent(self.__OnChangeMusic)
		self.musicListDlg.Open()

	def RefreshCameraMode(self):
		if app.ENABLE_SAVECAMERA_PREFERENCES:
			index=systemSetting.GetCameraType()
			for btn in self.cameraModeButtonList:
				btn.SetUp()
			
			try:
				self.cameraModeButtonList[index].Down()
			except:
				pass
		else:
			index=constinfo.GET_CAMERA_MAX_DISTANCE_INDEX()
			self.cameraModeButtonList[index].Down()
			self.cameraModeButtonList[not index].SetUp()

	def __OnClickCameraMode(self, index):
		if app.ENABLE_SAVECAMERA_PREFERENCES:
			constinfo.SET_CAMERA_MAX_DISTANCE_INDEX(index)
			systemSetting.SetCameraType(index)
		else:
			constinfo.SET_CAMERA_MAX_DISTANCE_INDEX(not constinfo.GET_CAMERA_MAX_DISTANCE_INDEX())
		
		self.RefreshCameraMode()

	if app.ENABLE_PERSPECTIVE_VIEW:
		def OnChangeFieldPerspective(self):
			if not self.cameraPerspective:
				return
			
			pos=float(self.cameraPerspective.GetSliderPos())
			systemSetting.SetFieldPerspective(pos * 100)

	def OnChangeLoadingDistance(self, arg):
		cfg.Set(cfg.SAVE_OPTION, "perf_loading_dist", self.loading_distances[arg])
		self.RefreshLoadingDistance()

	def RefreshLoadingDistance(self):
		cur = float(cfg.Get(cfg.SAVE_OPTION, "perf_loading_dist", self.loading_distances[1]))
		for i in xrange(len(self.loading_distances)):
			self.loading_distance_btns[i].SetUp()
			if cur >= self.loading_distances[i]:
				if i == 0 or i > 0 and cur < self.loading_distances[i-1]:
					self.loading_distance_btns[i].Down()

	def __OnClickChangeRenderOpt(self, flag):
		cfg.Set(cfg.SAVE_OPTION, "perf_tree_range", self.renderDistances[flag])
		cfg.Set(cfg.SAVE_OPTION, "perf_gravel_range", self.renderDistances[flag])
		cfg.Set(cfg.SAVE_OPTION, "perf_effect_range", self.renderDistances[flag])
		cfg.Set(cfg.SAVE_OPTION, "perf_shop_range", self.renderDistances[flag])
		#background.SetForceRefreshTree()
		#background.SetForceRefreshGravel()
		#background.RefreshShopRange();
		self.RefreshRenderOpt()

	def RefreshRenderOpt(self):
		cur = float(cfg.Get(cfg.SAVE_OPTION, "perf_tree_range", self.renderDistances[2]))
		for i in xrange(len(self.renderDistances)):
			self.renderOptButtonList[i].SetUp()
			if cur >= self.renderDistances[i]:
				if i == 0 or i > 0 and cur < self.renderDistances[i-1]:
					self.renderOptButtonList[i].Down()

class ScrollBarNew(ui.Window):
	SCROLLBAR_WIDTH=7
	SCROLL_BTN_XDIST=0
	SCROLL_BTN_YDIST=0

	class MiddleBar(ui.DragButton):
		def __init__(self):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")
			self.SetWindowName("scrollbar_middlebar")

		def MakeImage(self):
			top=ui.ExpandedImageBox()
			top.SetParent(self)
			top.LoadImage(IMG_DIR+"scrollbar/scrollbar_top.tga")
			top.AddFlag("not_pick")
			top.Show()
			topScale=ui.ExpandedImageBox()
			topScale.SetParent(self)
			topScale.SetPosition(0, top.GetHeight())
			topScale.LoadImage(IMG_DIR+"scrollbar/scrollbar_scale.tga")
			topScale.AddFlag("not_pick")
			topScale.Show()
			bottom=ui.ExpandedImageBox()
			bottom.SetParent(self)
			bottom.LoadImage(IMG_DIR+"scrollbar/scrollbar_bottom.tga")
			bottom.AddFlag("not_pick")
			bottom.Show()
			bottomScale=ui.ExpandedImageBox()
			bottomScale.SetParent(self)
			bottomScale.LoadImage(IMG_DIR+"scrollbar/scrollbar_scale.tga")
			bottomScale.AddFlag("not_pick")
			bottomScale.Show()
			middle=ui.ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage(IMG_DIR+"scrollbar/scrollbar_mid.tga")
			middle.AddFlag("not_pick")
			middle.Show()
			self.top=top
			self.topScale=topScale
			self.bottom=bottom
			self.bottomScale=bottomScale
			self.middle=middle

		def SetSize(self, height):
			minHeight=self.top.GetHeight() + self.bottom.GetHeight() + self.middle.GetHeight()
			height=max(minHeight, height)
			ui.DragButton.SetSize(self, 10, height)
			scale=(height - minHeight) / 2 
			extraScale=0
			if (height - minHeight) % 2 == 1:
				extraScale=1
			self.topScale.SetRenderingRect(0, 0, 0, scale - 1)
			self.middle.SetPosition(0, self.top.GetHeight() + scale)
			self.bottomScale.SetPosition(0, self.middle.GetBottom())
			self.bottomScale.SetRenderingRect(0, 0, 0, scale - 1 + extraScale)
			self.bottom.SetPosition(0, height - self.bottom.GetHeight())

	def __init__(self):
		ui.Window.__init__(self)
		self.pageSize=1
		self.curPos=0.0
		self.eventScroll=None
		self.eventArgs=None
		self.lockFlag=False
		self.CreateScrollBar()
		self.SetScrollBarSize(0)
		self.scrollStep=0.4
		self.SetWindowName("NONAME_ScrollBar")

	def __del__(self):
		ui.Window.__del__(self)

	def CreateScrollBar(self):
		topImage=ui.ExpandedImageBox()
		topImage.SetParent(self)
		topImage.AddFlag("not_pick")
		topImage.LoadImage(IMG_DIR+"scrollbar/scroll_top.tga")
		topImage.Show()
		bottomImage=ui.ExpandedImageBox()
		bottomImage.SetParent(self)
		bottomImage.AddFlag("not_pick")
		bottomImage.LoadImage(IMG_DIR+"scrollbar/scroll_bottom.tga")
		bottomImage.Show()
		middleImage=ui.ExpandedImageBox()
		middleImage.SetParent(self)
		middleImage.AddFlag("not_pick")
		middleImage.SetPosition(0, topImage.GetHeight())
		middleImage.LoadImage(IMG_DIR+"scrollbar/scroll_mid.tga")
		middleImage.Show()
		self.topImage=topImage
		self.bottomImage=bottomImage
		self.middleImage=middleImage
		middleBar=self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.MakeImage()
		middleBar.SetSize(12)
		self.middleBar=middleBar

	def Destroy(self):
		self.eventScroll=None
		self.eventArgs=None

	def SetScrollEvent(self, event, *args):
		self.eventScroll=event
		self.eventArgs=args

	def SetMiddleBarSize(self, pageScale):
		self.middleBar.SetSize(int(pageScale * float(self.GetHeight() - (self.SCROLL_BTN_YDIST*2))))
		realHeight=self.GetHeight() - (self.SCROLL_BTN_YDIST*2) - self.middleBar.GetHeight()
		self.pageSize=realHeight

	def SetScrollBarSize(self, height):
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.pageSize=height - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()
		middleImageScale=float((height - self.SCROLL_BTN_YDIST*2) - self.middleImage.GetHeight()) / float(self.middleImage.GetHeight())
		self.middleImage.SetRenderingRect(0, 0, 0, middleImageScale)
		self.bottomImage.SetPosition(0, height - self.bottomImage.GetHeight())
		self.middleBar.SetRestrictMovementArea(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST, \
			self.middleBar.GetWidth(), height - self.SCROLL_BTN_YDIST * 2)
		
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST)

	def SetScrollStep(self, step):
		self.scrollStep=step

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

	def SetPos(self, pos, moveEvent=True):
		pos=max(0.0, pos)
		pos=min(1.0, pos)
		newPos=float(self.pageSize) * pos
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, int(newPos) + self.SCROLL_BTN_YDIST)
		if moveEvent == True:
			self.OnMove()

	def OnMove(self):
		if self.lockFlag:
			return
		
		if 0 == self.pageSize:
			return
		
		(xLocal, yLocal)=self.middleBar.GetLocalPosition()
		self.curPos=float(yLocal - self.SCROLL_BTN_YDIST) / float(self.pageSize)
		if self.eventScroll:
			apply(self.eventScroll, self.eventArgs)

	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition)=self.GetMouseLocalPosition()
		newPos=float(yMouseLocalPosition) / float(self.GetHeight())
		self.SetPos(newPos)

	def LockScroll(self):
		self.lockFlag=True

	def UnlockScroll(self):
		self.lockFlag=False
