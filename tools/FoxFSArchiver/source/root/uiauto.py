import app
import chrmgr
import player
import chat
import net
import guild
import item
import skill
import wndMgr

import ui
import uiscriptlocale
import localeinfo
import mousemodule
import uitooltip

class AutoWindow(ui.ScriptWindow):
	isToolTipSkill = 0
	isToolTipItem = 0

	AUTO_COOLTIME_POS_Y = 2 if app.USE_AUTO_HUNT_NO_POTIONS else 4
	AUTO_COOLTIME_POS_X = 6
	AUTO_COOLTIME_MAX = AUTO_COOLTIME_POS_Y * AUTO_COOLTIME_POS_X

	TOOL_TIP_CLEAR_HEIGHT = 12
	TOOL_TIP_CLEAR_WIDTH = 66

	ITEM_REMAIN_TIME_MAX = 9999

	toolTipList = [\
		localeinfo.AUTO_TOOLTIP_LINE1,
		localeinfo.AUTO_TOOLTIP_LINE2,
		localeinfo.AUTO_TOOLTIP_LINE3,
		localeinfo.AUTO_TOOLTIP_LINE4,
		localeinfo.AUTO_TOOLTIP_LINE5
	]

	bLoaded = 0

	tooltipSkill = None
	tooltipItem = None

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.bReaded = 0
		self.bOnOff = 0
		
		self.autoSlot = None
		self.autoSlotDict = {}
		for i in xrange(player.AUTO_POTION_SLOT_MAX):
			self.autoSlotDict[i] = player.USHRT_MAX if i >= player.AUTO_POTION_SLOT_START else 0
		
		self.timeEditDict = {}
		self.buttonList = []
		
		self.autoSaveButton = None
		
		self.clearSkillButton = None
		self.clearSkillPotion = None
		self.clearAllButton = None
		
		self.toolTip = None
		self.toolTipButton = None
		self.toolTipClear = None

		import newconstinfo
		self.tooltipSkill = newconstinfo.GetInterfaceInstance().tooltipSkill if newconstinfo.GetInterfaceInstance() else None
		self.tooltipItem = newconstinfo.GetInterfaceInstance().tooltipItem if newconstinfo.GetInterfaceInstance() else None

		self.LoadWindow()

	def __del__(self):
		self.tooltipSkill = None
		self.tooltipItem = None

		self.bLoaded = 0
		self.bReaded = 0
		self.bOnOff = 0
		
		self.timeEditDict = {}
		self.buttonList = []
		
		self.autoSaveButton = None
		
		self.autoSlot = None
		self.autoSlotDict = {}
		
		self.clearSkillButton = None
		self.clearSkillPotion = None
		self.clearAllButton = None
		
		if self.toolTip:
			del self.toolTip
			self.toolTip = None
		
		self.toolTipButton = None
		
		if self.toolTipClear:
			del self.toolTipClear
			self.toolTipClear = None
		
		self.ClearDictionary()
		
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			if app.USE_AUTO_HUNT_NO_POTIONS:
				pyScrLoader.LoadScriptFile(self, "uiscript/autowindow2.py")
			else:
				pyScrLoader.LoadScriptFile(self, "uiscript/autowindow.py")
			
			GetObject = self.GetChild
			
			GetObject("board").SetCloseEvent(ui.__mem_func__(self.Hide))
			
			tmpBtn = GetObject("AutoSaveOnButton")
			tmpBtn.SetEvent(ui.__mem_func__(self.OnAutoSave))
			self.autoSaveButton = tmpBtn
			
			tmpBtn = GetObject("AutoStartOnButton")
			tmpBtn.SetEvent(ui.__mem_func__(self.OnClickAutoOnOffButton), chrmgr.AUTO_ONOFF_START, True)
			self.buttonList.append(tmpBtn)
			
			tmpBtn = GetObject("AutoStartOffButton")
			tmpBtn.SetEvent(ui.__mem_func__(self.OnClickAutoOnOffButton), chrmgr.AUTO_ONOFF_START, False)
			self.buttonList.append(tmpBtn)
			
			tmpBtn = GetObject("AutoAttackButton")
			tmpBtn.SetToggleDownEvent(ui.__mem_func__(self.OnClickAutoOnOffButton), chrmgr.AUTO_ONOFF_ATTACK, True)
			tmpBtn.SetToggleUpEvent(ui.__mem_func__(self.OnClickAutoOnOffButton), chrmgr.AUTO_ONOFF_ATTACK, False)
			self.buttonList.append(tmpBtn)

			tmpBtn = GetObject("AutoRangeButton")
			tmpBtn.SetToggleDownEvent(ui.__mem_func__(self.OnClickAutoOnOffButton), chrmgr.AUTO_ONOFF_RANGE, True)
			tmpBtn.SetToggleUpEvent(ui.__mem_func__(self.OnClickAutoOnOffButton), chrmgr.AUTO_ONOFF_RANGE, False)
			self.buttonList.append(tmpBtn)

			tmpBtn = GetObject("AutoMetinStonesButton")
			tmpBtn.SetToggleDownEvent(ui.__mem_func__(self.OnClickAutoOnOffButton), chrmgr.AUTO_ONOFF_METINSTONES, True)
			tmpBtn.SetToggleUpEvent(ui.__mem_func__(self.OnClickAutoOnOffButton), chrmgr.AUTO_ONOFF_METINSTONES, False)
			self.buttonList.append(tmpBtn)

			tmpBtn = GetObject("AutoPotionButton")
			tmpBtn.SetToggleDownEvent(ui.__mem_func__(self.OnClickAutoOnOffButton), chrmgr.AUTO_ONOFF_POTION, True)
			tmpBtn.SetToggleUpEvent(ui.__mem_func__(self.OnClickAutoOnOffButton), chrmgr.AUTO_ONOFF_POTION, False)
			self.buttonList.append(tmpBtn)

			if app.USE_AUTO_HUNT_NO_POTIONS:
				tmpBtn.Hide()

			tmpBtn = GetObject("AutoSkillButton")
			tmpBtn.SetToggleDownEvent(ui.__mem_func__(self.OnClickAutoOnOffButton), chrmgr.AUTO_ONOFF_SKILL, True)
			tmpBtn.SetToggleUpEvent(ui.__mem_func__(self.OnClickAutoOnOffButton), chrmgr.AUTO_ONOFF_SKILL, False)
			self.buttonList.append(tmpBtn)
			
			tmpBtn = GetObject("AutoRestartHereButton")
			tmpBtn.SetToggleDownEvent(ui.__mem_func__(self.OnClickAutoOnOffButton), chrmgr.AUTO_ONOFF_RESTART, True)
			tmpBtn.SetToggleUpEvent(ui.__mem_func__(self.OnClickAutoOnOffButton), chrmgr.AUTO_ONOFF_RESTART, False)
			self.buttonList.append(tmpBtn)
			
			self.toolTipClear = uitooltip.ToolTip()
			self.toolTipClear.ClearToolTip()
			self.toolTipClear.SetThinBoardSize(11 * len(localeinfo.AUTO_TOOLTIP_CLEAR))
			self.toolTipClear.SetTitle(localeinfo.AUTO_TOOLTIP_CLEAR)
			
			tmpBtn = GetObject("AutoSkillClearButton")
			tmpBtn.SetEvent(ui.__mem_func__(self.OnAutoSkillClear))
			tmpBtn.OnMouseOverIn = lambda arg = True : ui.__mem_func__(self.toolTipClear.Show)()
			tmpBtn.OnMouseOverOut = lambda arg = False : ui.__mem_func__(self.toolTipClear.Hide)()
			self.clearSkillButton = tmpBtn

			if not app.USE_AUTO_HUNT_NO_POTIONS:
				tmpBtn = GetObject("AutoPotionClearButton")
				tmpBtn.SetEvent(ui.__mem_func__(self.OnAutoPotionClear))
				tmpBtn.OnMouseOverIn = lambda arg = True : ui.__mem_func__(self.toolTipClear.Show)()
				tmpBtn.OnMouseOverOut = lambda arg = False : ui.__mem_func__(self.toolTipClear.Hide)()
				self.clearSkillPotion = tmpBtn

			tmpBtn = GetObject("AutoAllClearButton")
			tmpBtn.SetEvent(ui.__mem_func__(self.OnAutoAllClear))
			tmpBtn.OnMouseOverIn = lambda arg = True : ui.__mem_func__(self.toolTipClear.Show)()
			tmpBtn.OnMouseOverOut = lambda arg = False : ui.__mem_func__(self.toolTipClear.Hide)()
			self.clearAllButton = tmpBtn

			for i in xrange(self.AUTO_COOLTIME_MAX):
				tmpEditLine = GetObject("editline" + str(i))
				tmpEditLine.SetEscapeEvent(ui.__mem_func__(self.Hide))
				self.timeEditDict[i] = tmpEditLine
			
			autoSlot = GetObject("Auto_Active_Skill_Slot_Table")
			autoSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			autoSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.OnSelectActiveEmptySlot))
			autoSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.OnSelectActiveSlot))
			autoSlot.SetOverInItemEvent(ui.__mem_func__(self.OnOverInToolTip))
			autoSlot.SetOverOutItemEvent(ui.__mem_func__(self.OnOverOutToolTip))
			self.autoSlot = autoSlot
			
			toolTip = uitooltip.ToolTip()
			toolTip.SetTitle(localeinfo.AUTO_TOOLTIP_TITLE)
			toolTip.AppendSpace(5)
			
			for e in self.toolTipList:
				toolTip.AutoAppendTextLine(e)
			
			toolTip.AlignHorizonalCenter()
			#toolTip.SetTop()
			self.toolTip = toolTip
			
			self.toolTipButton = GetObject("AutoToolTipButton")
			self.toolTipButton.SetToolTipWindow(self.toolTip)
		except Exception as e:
			import dbg
			dbg.TraceError(str(e))
			app.Abort()
		
		self.OnReadAutoInfo()
		
		if self.buttonList:
			self.buttonList[chrmgr.AUTO_ONOFF_START + 1].Down()
			self.buttonList[chrmgr.AUTO_ONOFF_START + 1].Disable()
		
		self.bLoaded = 1
		return 1

	def Hide(self):
		if self.bLoaded == 1:
			for k, v in self.timeEditDict.items():
				v.KillFocus()
			
			self.OnSaveAutoInfo()
			self.OnOverOutToolTip()
		
		ui.ScriptWindow.Hide(self)

	def Show(self):
		if self.bLoaded == 0:
			return
		
		self.SetCenterPosition()
		self.OnReadAutoInfo()
		self.RefreshAutoPotionSlot()
		self.RefreshAutoSkillSlot()
		
		if not player.GetPremiumHunt() > 0:
			for i in range(5, 8):
				self.buttonList[i].Down()
				self.buttonList[i].Disable()
		
		ui.ScriptWindow.Show(self)

	def OnReadAutoInfo(self):
		charName = player.GetName()
		if len(charName) == 0:
			return
		
		player.ClearAutoAllSlot()
		
		import os
		if os.path.exists("UserData/auto/%s.data" % (charName)) == False:
			self.RefreshAutoPotionSlot()
			self.RefreshAutoSkillSlot()
			return
		
		try:
			if os.path.exists("UserData/auto/") == False:
				os.makedirs("UserData/auto/")
			
			infoFile = open("UserData/auto/%s.data" % (charName), "rb")
			fileData = app.DecryptByComputer(infoFile.read())
			lines = fileData.split("\n")
			
			for line in lines:
				if line == "":
					break
				
				splittedLine = line.split(":")
				
				index = int(splittedLine[0])
				
				if index >= player.AUTO_POTION_SLOT_MAX:
					continue
				
				vnum = int(splittedLine[1])
				coolTime = int(splittedLine[2])
				
				player.SetAutoSlotIndex(index, vnum)
				
				checkedCoolTime = player.CheckSlotCoolTime(index, vnum, coolTime)
				player.SetAutoSlotCoolTime(index, checkedCoolTime)
				
				if self.timeEditDict.has_key(index):
					self.timeEditDict[index].SetText(str(checkedCoolTime))
			
			infoFile.close()
		except Exception as e:
			import dbg
			dbg.TraceError(str(e))
			app.Abort()
		
		self.RefreshAutoPotionSlot()
		self.RefreshAutoSkillSlot()

	def OnSaveAutoInfo(self):
		charName = player.GetName()
		if len(charName) == 0:
			return
		
		import os
		
		if os.path.exists("UserData/auto/") == False:
			os.makedirs("UserData/auto/")
		
		try:
			infoFile = open("UserData/auto/%s.data" % (charName), "wb")
			
			fileData = ""
			
			for i in xrange(player.AUTO_POTION_SLOT_MAX):
				if self.autoSlotDict.has_key(i) and\
					self.timeEditDict.has_key(i):
					fileData += str(i) + ":" + str(self.autoSlotDict[i]) + ":"
					
					if self.timeEditDict[i].GetText() == "":
						fileData += "0"
					else:
						fileData += str(player.CheckSlotCoolTime(i, self.autoSlotDict[i], int(self.timeEditDict[i].GetText())))
				else:
					fileData += str(i)+":0:0"
				
				fileData += "\n"
			
			encData = app.EncryptByComputer(fileData)
			infoFile.write(encData)
			infoFile.close()
			
			for i in xrange(player.AUTO_POTION_SLOT_MAX):
				if app.USE_AUTO_HUNT_NO_POTIONS and i >= 12:
					player.SetAutoSlotCoolTime(i, 0)
					continue

				coolTime = self.timeEditDict[i].GetText()
				coolTime = self.CheckCoolTimeText(coolTime)
				coolTime = player.CheckSlotCoolTime(i, self.autoSlotDict[i], int(coolTime))
				
				if self.autoSlotDict[i] == 0:
					self.timeEditDict[i].SetText("")
				
				if coolTime != 0:
					player.SetAutoSlotCoolTime(i, int(coolTime))
					self.timeEditDict[i].SetText(str(coolTime))
		except Exception as e:
			import dbg
			dbg.TraceError(str(e))
			app.Abort()

	def OnAutoSkillClear(self):
		if self.GetAutoStartonoff() == False:
			player.ClearAutoSkillSlot()
			self.RefreshAutoSkillSlot()
			
			for i in self.autoSlotDict:
				i = 0

	def OnAutoPotionClear(self):
		if self.GetAutoStartonoff() == False:
			player.ClearAutoPotionSlot()
			
			for k, v in self.autoSlotDict.items():
				v = player.USHRT_MAX
			
			self.RefreshAutoPotionSlot()

	def Checkout(self, iPos):
		if self.GetAutoStartonoff() == True:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AUTO_HUNT_CANNOT_EDIT)
			mousemodule.mouseController.DeattachObject()
			return
		
		if iPos < player.AUTO_SKILL_SLOT_MAX:
			player.ClearAutoSkillSlot(iPos)
			
			if self.autoSlotDict.has_key(iPos):
				self.autoSlotDict[iPos] = 0
			
			self.RefreshAutoSkillSlot()
		else:
			player.ClearAutoPotionSlot(iPos)
			
			if self.autoSlotDict.has_key(iPos):
				self.autoSlotDict[iPos] = player.USHRT_MAX
			
			self.RefreshAutoPotionSlot()
		
		mousemodule.mouseController.DeattachObject()

	def OnAutoAllClear(self):
		if self.GetAutoStartonoff() == False:
			for i in range(chrmgr.AUTO_ONOFF_ATTACK, chrmgr.AUTO_ONOFF_MAX):
				self.OnClickAutoOnOffButton(i, False)
			
			if not player.GetPremiumHunt() > 0:
				if len(self.buttonList) >= 8:
					for i in range(5, 8):
						self.buttonList[i].Down()
						self.buttonList[i].Disable()

	def CheckCoolTimeText(self, time):
		iTime = 0
		
		try:
			iTime = int(time)
		except:
			return 0
		
		return iTime

	def OnClickAutoOnOffButton(self, type, onoff, command = False):
		if self.bLoaded == 0:
			return
		
		if type == chrmgr.AUTO_ONOFF_START:
			if onoff == True:
				for i in xrange(player.AUTO_POTION_SLOT_MAX):
					if app.USE_AUTO_HUNT_NO_POTIONS and i >= 12:
						continue

					coolTime = self.timeEditDict[i].GetText()
					coolTime = self.CheckCoolTimeText(coolTime)
					coolTime = player.CheckSlotCoolTime(i, self.autoSlotDict[i], int(coolTime))

					if self.autoSlotDict[i] == 0:
						self.timeEditDict[i].SetText("")

					if coolTime != 0:
						player.SetAutoSlotCoolTime(i, int(coolTime))
						self.timeEditDict[i].SetText(str(coolTime))
			else:
				if not app.USE_AUTO_HUNT_NO_POTIONS:
					for i in range(player.AUTO_POTION_SLOT_START, player.AUTO_POTION_SLOT_MAX):
						self.SetAutoCooltime(i, 0)

			self.bOnOff = onoff
		else:
			chrmgr.SetAutoSystemByType(type, onoff)

		cmd_packet = str(int(self.bOnOff == True))

		for i in range(chrmgr.AUTO_ONOFF_ATTACK, chrmgr.AUTO_ONOFF_MAX):
			cmd_packet += " " + str(chrmgr.GetAutoSystemByType(i))

		net.SendChatPacket("/auto_onoff " + cmd_packet)

		if command == True:
			if chrmgr.GetAutoOnOff() == False:
				self.Hide()
				return

		if type == chrmgr.AUTO_ONOFF_START:
			if onoff == True:
				self.buttonList[type].Down()
				self.buttonList[type].Disable()
				
				self.buttonList[type + 1].SetUp()
				self.buttonList[type + 1].Enable()
			else:
				self.buttonList[type].SetUp()
				self.buttonList[type].Enable()
				
				self.buttonList[type + 1].Down()
				self.buttonList[type + 1].Disable()
		else:
			if onoff != True:
				self.buttonList[type + 1].SetText(uiscriptlocale.AUTO_OFF)
				self.buttonList[type + 1].SetUp()
			else:
				self.buttonList[type + 1].SetText(uiscriptlocale.AUTO_ON)
				self.buttonList[type + 1].Down()

	def OnAutoSave(self):
		self.OnSaveAutoInfo()

	def OnActivateSkill(self):
		self.RefreshAutoSkillSlot()

	def OnDeactivateSkill(self, slotIndex):
		for i in xrange(player.AUTO_SKILL_SLOT_MAX):
			position = player.GetAutoSlotIndex(slotIndex)
			if slotIndex == position:
				self.autoSlot.DeactivateSlot(i)

	def OnUseSkill(self):
		self.RefreshAutoSkillSlot()

	def SetAutoCooltime(self, slotIndex, coolTime):
		self.autoSlot.SetSlotCoolTime(slotIndex, coolTime, 0)

	def OnGameOver(self):
		if not player.GetPremiumHunt() > 0:
			self.OnClickAutoOnOffButton(chrmgr.AUTO_ONOFF_ATTACK, False)

	def GetAutoStartonoff(self):
		return self.bOnOff

	def RefreshAutoPotionSlot(self):
		if app.USE_AUTO_HUNT_NO_POTIONS:
			return

		if not self.autoSlot:
			return

		for i in range(player.AUTO_POTION_SLOT_START, player.AUTO_POTION_SLOT_MAX):
			itemPos = player.GetAutoSlotIndex(i)
			
			if itemPos == player.USHRT_MAX:
				self.autoSlot.ClearSlot(i)
				
				self.timeEditDict[i].SetText("")
				self.autoSlotDict[i] = player.USHRT_MAX
				continue
			
			itemVnum = player.GetItemIndex(itemPos)
			itemCount = player.GetItemCount(itemPos)
			if itemCount <= 1:
				itemCount = 0
			
			self.autoSlot.SetItemSlot(i, itemVnum, itemCount)
			self.autoSlotDict[i] = itemPos
			
			coolTime = player.GetAutoSlotCoolTime(i)
			if self.timeEditDict[i].GetText() == "":
				self.timeEditDict[i].SetText(str(coolTime))
			
			if itemVnum == 0:
				self.autoSlot.ClearSlot(i)
				self.timeEditDict[i].SetText("")
				player.SetAutoSlotIndex(i, player.USHRT_MAX)
		
		self.autoSlot.RefreshSlot()

	def RefreshAutoSkillSlot(self):
		for i in xrange(player.AUTO_SKILL_SLOT_MAX):
			skillPos = player.GetAutoSlotIndex(i)
			if skillPos == 0:
				self.autoSlot.ClearSlot(i)
				self.timeEditDict[i].SetText("")
				self.autoSlotDict[i] = 0
				continue
			
			skill_index = player.GetSkillIndex(skillPos)
			if 0 == skill_index:
				self.autoSlot.ClearSlot(i)
			
			skill_type = skill.GetSkillType(skill_index)
			if skill.SKILL_TYPE_GUILD == skill_type:
				import guild
				skill_grade = 0
				skill_level = guild.GetSkillLevel(skillPos)
			else:
				skill_grade = player.GetSkillGrade(skillPos)
				skill_level = player.GetSkillLevel(skillPos)
			
			self.autoSlot.SetSkillSlotNew(i, skill_index, skill_grade, skill_level)
			self.autoSlot.SetSlotCountNew(i, skill_grade, skill_level)
			self.autoSlot.SetCoverButton(i)
			
			if player.IsSkillCoolTime(skillPos):
				(coolTime, elapsed_time) = player.GetSkillCoolTime(skillPos)
				self.autoSlot.SetSlotCoolTime(i, coolTime, elapsed_time)
			elif player.IsToggleSkill(skillPos):
				coolTime = player.ToggleCoolTime(skillPos)
			
			if player.IsSkillActive(skillPos):
				self.autoSlot.ActivateSlot(i)
			
			self.autoSlotDict[i] = skillPos
			
			coolTime = player.GetAutoSlotCoolTime(i)
			if self.timeEditDict[i].GetText() == "":
				self.timeEditDict[i].SetText(str(coolTime))
		
		self.autoSlot.RefreshSlot()

	def RefreshSkillCoolTime(self):
		pass

	def OnAddAutoSlot(self, slotIndex):
		attachedSlotType = mousemodule.mouseController.GetAttachedType()
		attachedSlotNumber = mousemodule.mouseController.GetAttachedSlotNumber()
		attachedItemVnum = mousemodule.mouseController.GetAttachedItemIndex()
		
		if slotIndex < player.AUTO_SKILL_SLOT_MAX:
			if player.SLOT_TYPE_SKILL == attachedSlotType:
				player.SetAutoSlotIndex(slotIndex, attachedSlotNumber)
				self.RefreshAutoSkillSlot()
			elif player.SLOT_TYPE_AUTO == attachedSlotType:
				if slotIndex == attachedSlotNumber:
					return
				
				if attachedSlotNumber > player.AUTO_SKILL_SLOT_MAX:
					return
				
				player.SetAutoSlotIndex(slotIndex, attachedItemVnum)
				self.RefreshAutoSkillSlot()
		else:
			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				itemVnum = player.GetItemIndex(attachedSlotNumber)
				if not itemVnum:
					return

				if not player.ItemCanBeAddedToPotionSlots(itemVnum):
					item.SelectItem(itemVnum)
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AUTO_HUNT_ITEM_CANNOT_BE_ADDED % (item.GetItemName()))
					return

				player.SetAutoSlotIndex(slotIndex, attachedSlotNumber)
				self.RefreshAutoPotionSlot()
			elif player.SLOT_TYPE_AUTO == attachedSlotType:
				if slotIndex == attachedSlotNumber:
					return
				elif attachedSlotNumber < player.AUTO_SKILL_SLOT_MAX:
					return

				player.SetAutoSlotIndex(slotIndex, attachedItemVnum)
				self.RefreshAutoPotionSlot()

				mousemodule.mouseController.DeattachObject()

				self.OnOverInToolTip(slotIndex)

		self.OnAutoSave()

		if mousemodule.mouseController.isAttached():
			mousemodule.mouseController.DeattachObject()

	def OnSelectActiveEmptySlot(self, slotIndex):
		if not mousemodule.mouseController.isAttached():
			return
		
		self.OnAddAutoSlot(slotIndex)

	def OnSelectActiveSlot(self, slotIndex):
		if mousemodule.mouseController.isAttached():
			mousemodule.mouseController.DeattachObject()
		
		if slotIndex < player.AUTO_SKILL_SLOT_MAX:
			vnum = self.autoSlotDict[slotIndex]
			if vnum == 0:
				return
		else:
			vnum = self.autoSlotDict[slotIndex]
			if vnum == player.USHRT_MAX:
				return
		
		mousemodule.mouseController.AttachObject(self, player.SLOT_TYPE_AUTO, slotIndex, vnum)

	def OnOverInToolTip(self, slotIndex):
		if mousemodule.mouseController.isAttached():
			return
		
		if slotIndex < player.AUTO_SKILL_SLOT_MAX:
			position = player.GetAutoSlotIndex(slotIndex)
			if position == 0:
				return
			
			skill_index = player.GetSkillIndex(position)
			skill_type = skill.GetSkillType(skill_index)
			if skill.SKILL_TYPE_GUILD == skill_type:
				import guild
				skill_grade = 0
				skill_level = guild.GetSkillLevel(position)
			else:
				skill_grade = player.GetSkillGrade(position)
				skill_level = player.GetSkillLevel(position)

			if self.tooltipSkill:
				self.isToolTipSkill = 1
				self.tooltipSkill.SetSkillNew(position, skill_index, skill_grade, skill_level)
		else:
			itemPos = player.GetAutoSlotIndex(slotIndex)
			if itemPos == player.USHRT_MAX:
				return
			
			if self.tooltipItem:
				self.isToolTipItem = 1
				self.tooltipItem.SetInventoryItem(itemPos)

	def OnOverOutToolTip(self):
		if self.isToolTipSkill == 1:
			self.isToolTipSkill = 0
			if self.tooltipSkill:
				self.tooltipSkill.HideToolTip()

		if self.isToolTipItem == 1:
			self.isToolTipItem = 0
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()

	def OnPressEscapeKey(self):
		self.Hide()
		return True
