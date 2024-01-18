import app
import player
import net
import snd
import item
import chat
import grp
import ime
import wndMgr

import ui
import mousemodule
import uiscriptlocale
import uirefine
import uiattachmetin
import uipickmoney
import uicommon
import uiprivateshopbuilder
import constinfo
import localeinfo
import newconstinfo

if app.__ENABLE_NEW_OFFLINESHOP__:
	import offlineshop
	import uiofflineshop

if app.ENABLE_CAPITALE_SYSTEM:
	import uitooltip

if app.ENABLE_ACCE_SYSTEM:
	import acce

if app.ITEM_CHECKINOUT_UPDATE:
	import exchange

if app.ENABLE_CHANGELOOK:
	import changelook

ITEM_MALL_BUTTON_ENABLE=True
ITEM_FLAG_APPLICABLE=1<<14

if app.USE_AUTO_AGGREGATE:
	class BraveCapeWindow(ui.ScriptWindow):
		isLoaded = 0
		tooltipItem = None
		__children = {}
		imgDir = "d:/ymir work/ui/game/bravery_cape/"

		class SliderBarNew(ui.Window):
			def __init__(self):
				ui.Window.__init__(self)
				self.curPos = 1.0
				self.pageSize = 1.0
				self.eventChange = None
				self.__Load()

			def __Load(self):
				img = ui.ImageBox()
				img.SetParent(self)
				img.LoadImage(BraveCapeWindow.imgDir + "slider_bg.tga")
				img.Show()
				self.backGroundImage = img

				self.SetSize(self.backGroundImage.GetWidth(), self.backGroundImage.GetHeight())

				cursor = ui.DragButton()
				cursor.AddFlag("movable")
				cursor.AddFlag("restrict_y")
				cursor.SetParent(self)
				cursor.SetMoveEvent(ui.__mem_func__(self.__OnMove))
				cursor.SetUpVisual(BraveCapeWindow.imgDir + "drag.tga")
				cursor.SetOverVisual(BraveCapeWindow.imgDir + "drag.tga")
				cursor.SetDownVisual(BraveCapeWindow.imgDir + "drag.tga")
				cursor.Show()
				self.cursor = cursor

				self.cursor.SetRestrictMovementArea(0, 0, self.backGroundImage.GetWidth(), 0)
				self.pageSize = self.backGroundImage.GetWidth() - self.cursor.GetWidth()

			def __OnMove(self):
				(xLocal, yLocal) = self.cursor.GetLocalPosition()
				self.curPos = float(xLocal) / float(self.pageSize)
				if self.eventChange:
					self.eventChange()

			def SetSliderPos(self, pos):
				self.curPos = pos
				self.cursor.SetPosition(int(self.pageSize * pos), 0)

			def GetSliderPos(self):
				return self.curPos

			def SetEvent(self, event):
				self.eventChange = event

			def Enable(self):
				self.cursor.Show()

			def Disable(self):
				self.cursor.Hide()

		def __init__(self, wndEquipmentInst):
			if self.isLoaded == 1:
				return

			ui.ScriptWindow.__init__(self)

			from _weakref import proxy
			self.wndEquipment = proxy(wndEquipmentInst)

			self.layer = None
			self.bg = None
			self.expand = None
			self.minimize = None
			self.wndItem = None
			self.startBtn = None
			self.stopBtn = None
			self.origWidth = 0
			self.origHeight = 0

			self.__children = {}

			self.__LoadWindow()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def Destroy(self):
			self.SaveData()

			self.wndItem.ClearSlot(0)

			delList = [\
						"timeTextVisual",
						"timeSlider",
						"timeBg",
						"timeText",
						"rangeTextVisual",
						"rangeSlider",
						"rangeBg",
						"rangeText",
			]

			for k, v in self.__children.items():
				if v in delList:
					del v
					v = None

			self.__children = {}

			self.ClearDictionary()
			self.isLoaded = 0

		def CreateWindow(self, classPtr, parent, pos):
			window = classPtr
			window.SetParent(parent)
			window.SetPosition(*pos)
			window.Show()
			return window

		def __LoadWindow(self):
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "uiscript/bravecapewindow.py")
			except:
				import exception
				exception.Abort("BraveCapeWindow.LoadWindow.LoadObject")

			try:
				self.layer = self.GetChild("layer")
				self.bg = self.GetChild("bg")
				self.expand = self.GetChild("expand")
				self.minimize = self.GetChild("minimize")
				self.wndItem = self.GetChild("item")
				self.startBtn = self.GetChild("start")
				self.stopBtn = self.GetChild("stop")
				self.origWidth = self.GetWidth()
				self.origHeight = self.GetHeight()

				self.expand.SetEvent(ui.__mem_func__(self.__OpenInventory))
				self.minimize.SetEvent(ui.__mem_func__(self.__CloseInventory))

				self.wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
				self.wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
				self.wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
				self.wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
				self.wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
				self.wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
				self.wndItem.SetItemSlot(0, 70038, 0)

				self.startBtn.SAFE_SetEvent(self.__ClickStatusBtn, "ACTIVE")
				self.stopBtn.SAFE_SetEvent(self.__ClickStatusBtn, "DEACTIVE")
			except:
				import exception
				exception.Abort("BraveCapeWindow.LoadWindow.BindObject")

			self.isLoaded = 1

			timeTextVisual = self.CreateWindow(ui.TextLine(), self.bg, (50 + 13 if localeinfo.IsARABIC() else 13, 60))
			timeTextVisual.SetText(localeinfo.BRAVERY_CAPE_SECOND)
			self.__children["timeTextVisual"] = timeTextVisual

			timeSlider = self.CreateWindow(self.SliderBarNew(), self.bg, (13, 73 + 5))
			timeSlider.SetEvent(ui.__mem_func__(self.OnChangeTimeSlider))
			self.__children["timeSlider"] = timeSlider

			timeBg = self.CreateWindow(ui.ImageBox(), self.bg, (77, 64))
			timeBg.LoadImage(BraveCapeWindow.imgDir + "input_output.tga")
			self.__children["timeBg"] = timeBg

			timeText = self.CreateWindow(ui.TextLine(), timeBg, (0, 0))
			timeText.SetAllAlign()
			timeText.SetText("0")
			self.__children["timeText"] = timeText

			rangeTextVisual = self.CreateWindow(ui.TextLine(), self.bg, (50 + 13 if localeinfo.IsARABIC() else 13, 73 + 22 + 17 + 5 - 18))
			rangeTextVisual.SetText(localeinfo.BRAVERY_CAPE_RANGE)
			self.__children["rangeTextVisual"] = rangeTextVisual

			rangeSlider = self.CreateWindow(self.SliderBarNew(), self.bg, (13, 73 + 22 + 17 + 5))
			rangeSlider.SetEvent(ui.__mem_func__(self.OnChangeRangeSlider))
			self.__children["rangeSlider"] = rangeSlider

			rangeBg = self.CreateWindow(ui.ImageBox(), self.bg, (77, 95 + 8))
			rangeBg.LoadImage(BraveCapeWindow.imgDir + "input_output.tga")
			self.__children["rangeBg"] = rangeBg

			rangeText = self.CreateWindow(ui.TextLine(), rangeBg, (0, 0))
			rangeText.SetAllAlign()
			rangeText.SetText("0")
			self.__children["rangeText"] = rangeText

			self.__children["firstOpened"] = app.GetGlobalTimeStamp() + 5
			self.__children["second"] = 0
			self.__children["range"] = 0
			self.__children["status"] = False

			self.Refresh()

		def SaveData(self):
			myPath = "UserData\\bravery_cape"

			import os
			if not os.path.exists(os.getcwd() + os.sep + myPath):
				os.mkdir(os.getcwd() + os.sep + myPath)

			filePath = myPath + "\\" + player.GetName() + ".data"
			old_open(filePath, "w").close()

			myData = "{}#{}#{}\n".format(1 if (self.__children["status"] if self.__children.has_key("status") else False) == True else 0, self.__children["second"], self.__children["range"])
			myFile = old_open(filePath, "wb")
			myFile.write(myData)
			myFile.close()
		
		def LoadData(self):
			myPath = "UserData\\bravery_cape"

			import os
			if not os.path.exists(os.getcwd() + os.sep + myPath):
				return

			filePath = myPath + "\\" + player.GetName() + ".data"
			if os.path.isfile(filePath):
				myFile = old_open(filePath, "rb")
				myContent = myFile.read()
				myLines = myContent.split("#")

				if len(myLines) == 3:
					self.__children["status"] = True if int(myLines[0]) == 1 else False
					self.__children["second"] = int(myLines[1])
					self.__children["range"] = int(myLines[2])
					self.Refresh()

					if self.__children["status"]:
						self.__ClickStatusBtn("ACTIVE")

				myFile.close()

		def IsOpeningInventory(self):
			return self.layer.IsShow()

		def AdjustPosition(self):
			x, y = self.wndEquipment.GetGlobalPosition()

			if self.IsOpeningInventory():
				self.SetSize(self.origWidth, self.origHeight)
				self.SetPosition(x - self.origWidth, y)
			else:
				self.SetPosition(x - 10, y)
				self.SetSize(10, self.origHeight)

		def __OpenInventory(self):
			self.layer.Show()
			self.expand.Hide()

			self.AdjustPosition()

		def __CloseInventory(self):
			self.layer.Hide()
			self.expand.Show()

			self.AdjustPosition()

		def SelectEmptySlot(self, iPos):
			if mousemodule.mouseController.isAttached():
				mousemodule.mouseController.DeattachObject()

		def SelectItemSlot(self, iPos):
			if mousemodule.mouseController.isAttached():
				mousemodule.mouseController.DeattachObject()

		def UseItemSlot(self, iPos):
			pass

		def OverOutItem(self):
			if not self.tooltipItem:
				if self.wndEquipment and self.wndEquipment.parent:
					self.tooltipItem = self.wndEquipment.parent.tooltipItem

			if self.tooltipItem:
				self.tooltipItem.HideToolTip()

		def OverInItem(self, overSlotPos):
			if not self.tooltipItem:
				if self.wndEquipment and self.wndEquipment.parent:
					self.tooltipItem = self.wndEquipment.parent.tooltipItem

			if self.tooltipItem:
				self.tooltipItem.SetItemToolTip(70038)

		def Refresh(self):
			(secondSlider, rangeSlider, posTime, posSlider) = (self.__children["second"], self.__children["range"], 0.0, 0.0)
			if secondSlider > 6000:
				secondSlider = 6000

			if rangeSlider > 14000:
				rangeSlider = 14000

			self.__children["timeText"].SetText(str((secondSlider / 100)))
			self.__children["rangeText"].SetText(str(rangeSlider + 1000))

			self.__children["timeSlider"].SetSliderPos((1.0 / 6000.0) * secondSlider)
			self.__children["rangeSlider"].SetSliderPos((1.0 / 14000.0) * rangeSlider)

			self.__children["second"] = secondSlider
			self.__children["range"] = rangeSlider

		def OnChangeRangeSlider(self):
			val = int(((1.0 / 14000.0) * (self.__children["rangeSlider"].GetSliderPos() * 14000)) * 14000)
			self.__children["range"] = val
			self.Refresh()

		def OnChangeTimeSlider(self):
			val = int(((1.0 / 6000.0) * (self.__children["timeSlider"].GetSliderPos() * 6000)) * 6000)
			self.__children["second"] = val
			self.Refresh()

		def SetStatus(self, status):
			self.__children["status"] = True if int(status) == 1 else False

			if self.__children["status"]:
				self.startBtn.SetUpVisual(BraveCapeWindow.imgDir + "start_btn_2.tga")
				self.startBtn.SetOverVisual(BraveCapeWindow.imgDir + "start_btn_1.tga")
				self.startBtn.SetDownVisual(BraveCapeWindow.imgDir + "start_btn_0.tga")

				self.stopBtn.SetUpVisual(BraveCapeWindow.imgDir + "stop_btn_0.tga")
				self.stopBtn.SetOverVisual(BraveCapeWindow.imgDir + "stop_btn_1.tga")
				self.stopBtn.SetDownVisual(BraveCapeWindow.imgDir + "stop_btn_2.tga")
			else:
				self.startBtn.SetUpVisual(BraveCapeWindow.imgDir + "start_btn_0.tga")
				self.startBtn.SetOverVisual(BraveCapeWindow.imgDir + "start_btn_1.tga")
				self.startBtn.SetDownVisual(BraveCapeWindow.imgDir + "start_btn_2.tga")

				self.stopBtn.SetUpVisual(BraveCapeWindow.imgDir + "stop_btn_2.tga")
				self.stopBtn.SetOverVisual(BraveCapeWindow.imgDir + "stop_btn_1.tga")
				self.stopBtn.SetDownVisual(BraveCapeWindow.imgDir + "stop_btn_0.tga")

			self.Refresh()

		def __ClickStatusBtn(self, event):
			if event == "ACTIVE":
				net.SendChatPacket("/auto_aggregate active {} {}".format((self.__children["second"] / 100) + 2, self.__children["range"] + 1000))
			elif event == "DEACTIVE":
				net.SendChatPacket("/auto_aggregate deactive")

		def Show(self, openMe = False):
			self.LoadData()

			ui.ScriptWindow.Show(self)

			if openMe:
				self.__OpenInventory()
			else:
				self.__CloseInventory()

			self.SetTop()

		def Close(self):
			self.Hide()

		def OnTop(self):
			if self.wndEquipment:
				self.wndEquipment.SetClickCapeWindow(True)
				self.wndEquipment.SetTop()

class ExpandedMoneyTaskBar(ui.ScriptWindow):
	def __init__(self):
		ui.Window.__init__(self)
		self.SetWindowName("ExpandedMoneyTaskBar")
		toolTipMoneyTitle=None
		# if app.ENABLE_HALLOWEEN_EVENT_2022:
			# toolTipSkullTitle = None
		# if app.ENABLE_GAYA_RENEWAL:
			# toolTipGayaTitle=None
		
		self.firstTime=0
		self.lastGold=-1
		self.lastGoldBK=-1
		self.lastGoldFlagLess=0
		self.lastGoldFlagBig=0
		self.lastGoldRange=0
		self.lastGoldRangeBK=0
		self.lastGoldSize=0

		# if app.ENABLE_HALLOWEEN_EVENT_2022:
			# self.lastSkull = -1
			# self.lastSkullBK = -1
			# self.lastSkullFlagLess = 0
			# self.lastSkullFlagBig = 0
			# self.lastSkullRange = 0
			# self.lastSkullRangeBK = 0
			# self.lastSkullSize = 0

		# if app.ENABLE_GAYA_RENEWAL:
			# self.lastGaya=-1
			# self.lastGayaBK=-1
			# self.lastGayaFlagLess=0
			# self.lastGayaFlagBig=0
			# self.lastGayaRange=0
			# self.lastGayaRangeBK=0
			# self.lastGayaSize=0

	def LoadWindow(self):
		try:
			pyScrLoader=ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/expandedmoneytaskbar.py")
		except:
			import exception
			exception.Abort("ExpandedMoneyTaskBar.LoadWindow.LoadObject")
		
		self.expandedMoneyTaskBarBoard=self.GetChild("ExpanedMoneyTaskBar_Board")
		self.money=self.GetChild("Money")
		self.wndMoneySlotIcon=self.GetChild("Money_Icon")
		self.wndMoneySlotIcon.SetStringEvent("MOUSE_OVER_IN", ui.__mem_func__(self.__ShowMoneyTitleToolTip))
		self.wndMoneySlotIcon.SetStringEvent("MOUSE_OVER_OUT", ui.__mem_func__(self.__HideMoneyTitleToolTip))
		self.__SetCapitalSystemToolTip()
		# if app.ENABLE_HALLOWEEN_EVENT_2022:
			# self.Skull = self.GetChild("Skull")
			# self.wndSkullSlotIcon = self.GetChild("Skull_Icon")
			# self.wndSkullSlotIcon.SetStringEvent("MOUSE_OVER_IN", ui.__mem_func__(self.__ShowSkullTitleToolTip))
			# self.wndSkullSlotIcon.SetStringEvent("MOUSE_OVER_OUT", ui.__mem_func__(self.__HideSkullTitleToolTip))


		# if app.ENABLE_GAYA_RENEWAL:
			# self.Gem=self.GetChild("Gem")
			# self.wndGemSlotIcon=self.GetChild("Gem_Icon")
			# self.wndGemSlotIcon.SetStringEvent("MOUSE_OVER_IN", ui.__mem_func__(self.__ShowGayaTitleToolTip))
			# self.wndGemSlotIcon.SetStringEvent("MOUSE_OVER_OUT", ui.__mem_func__(self.__HideGayaTitleToolTip))

	def Show(self):
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	def OnUpdate(self):
		if self.lastGoldFlagLess==1:
			d=(self.lastGoldRangeBK) / 15
			if self.lastGoldRange <= 0:
				self.lastGoldFlagLess=0
				self.lastGoldRange=0
				self.lastGoldRangeBK=0
				self.lastGold=self.lastGoldBK
				self.money.SetText(localeinfo.NumberToMoneyString(self.lastGoldBK))
				return
			
			if d < 1:
				d=1
			
			self.lastGold -= d
			self.money.SetText(localeinfo.NumberToMoneyString(self.lastGold))
			self.lastGoldRange -= d
		elif self.lastGoldFlagBig==1:
			d=(self.lastGoldRangeBK) / 15
			if self.lastGoldRange <= 0:
				self.lastGoldFlagBig=0
				self.lastGoldRange=0
				self.lastGoldRangeBK=0
				self.lastGold=self.lastGoldBK
				self.money.SetText(localeinfo.NumberToMoneyString(self.lastGoldBK))
				return
			
			if d < 1:
				d=1
			
			self.lastGold += d
			self.money.SetText(localeinfo.NumberToMoneyString(self.lastGold))
			self.lastGoldRange -= d
		
		# if app.ENABLE_HALLOWEEN_EVENT_2022:
			# if self.lastSkullFlagLess == 1:
				# d = (self.lastSkullRangeBK) / 15
				# if self.lastSkullRange <= 0:
					# self.lastSkullFlagLess = 0
					# self.lastSkullRange = 0
					# self.lastSkullRangeBK = 0
					# self.lastSkull = self.lastSkullBK
					# self.Skull.SetText(localeinfo.NumberToSkullString(self.lastSkullBK))
					# return
				
				# if d < 1:
					# d = 1
				
				# self.lastSkull -= d
				# self.Skull.SetText(localeinfo.NumberToSkullString(self.lastSkull))
				# self.lastSkullRange -= d
			# elif self.lastSkullFlagBig == 1:
				# d = (self.lastSkullRangeBK) / 15
				# if self.lastSkullRange <= 0:
					# self.lastSkullFlagBig = 0
					# self.lastSkullRange = 0
					# self.lastSkullRangeBK = 0
					# self.lastSkull = self.lastSkullBK
					# self.Skull.SetText(localeinfo.NumberToSkullString(self.lastSkullBK))
					# return
				
				# if d < 1:
					# d = 1
				
				# self.lastSkull += d
				# self.Skull.SetText(localeinfo.NumberToSkullString(self.lastSkull))
				# self.lastSkullRange -= d

		
		# if app.ENABLE_GAYA_RENEWAL:
			# if self.lastGayaFlagLess==1:
				# d=(self.lastGayaRangeBK) / 15
				# if self.lastGayaRange <= 0:
					# self.lastGayaFlagLess=0
					# self.lastGayaRange=0
					# self.lastGayaRangeBK=0
					# self.lastGaya=self.lastGayaBK
					# self.Gem.SetText(localeinfo.NumberToGayaString(self.lastGayaBK))
					# return
				
				# if d < 1:
					# d=1
				
				# self.lastGaya -= d
				# self.Gem.SetText(localeinfo.NumberToGayaString(self.lastGaya))
				# self.lastGayaRange -= d
			# elif self.lastGayaFlagBig==1:
				# d=(self.lastGayaRangeBK) / 15
				# if self.lastGayaRange <= 0:
					# self.lastGayaFlagBig=0
					# self.lastGayaRange=0
					# self.lastGayaRangeBK=0
					# self.lastGaya=self.lastGayaBK
					# self.Gem.SetText(localeinfo.NumberToGayaString(self.lastGayaBK))
					# return
				
				# if d < 1:
					# d=1
				
				# self.lastGaya += d
				# self.Gem.SetText(localeinfo.NumberToGayaString(self.lastGaya))
				# self.lastGayaRange -= d

	def RefreshStatus(self):
		if self.lastGold==-1:
			self.lastGold=player.GetMoney()
			self.money.SetText(localeinfo.NumberToMoneyString(self.lastGold))
		else:
			money=player.GetMoney()
			if money != self.lastGold:
				if money < self.lastGold:
					self.lastGoldFlagBig=0
					self.lastGoldFlagLess=1
					self.lastGoldRange=self.lastGold - money
					self.lastGoldRangeBK=self.lastGoldRange
				else:
					self.lastGoldFlagLess=0
					self.lastGoldFlagBig=1
					self.lastGoldRange=money - self.lastGold
					self.lastGoldRangeBK=self.lastGoldRange
				
				self.lastGoldBK=money
		
		# if app.ENABLE_HALLOWEEN_EVENT_2022:
			# if self.lastSkull == -1:
				# self.lastSkull = player.GetSkull()
				# self.Skull.SetText(localeinfo.NumberToSkullString(self.lastSkull))
			# else:
				# money = player.GetSkull()
				# if money != self.lastSkull:
					# if money < self.lastSkull:
						# self.lastSkullFlagBig = 0
						# self.lastSkullFlagLess = 1
						# self.lastSkullRange = self.lastSkull - money
						# self.lastSkullRangeBK = self.lastSkullRange
					# else:
						# self.lastSkullFlagLess = 0
						# self.lastSkullFlagBig = 1
						# self.lastSkullRange = money - self.lastSkull
						# self.lastSkullRangeBK = self.lastSkullRange
					
					# self.lastSkullBK = money

		
		# if app.ENABLE_GAYA_RENEWAL:
			# if self.lastGaya==-1:
				# self.lastGaya=player.GetGem()
				# self.Gem.SetText(localeinfo.NumberToGayaString(self.lastGaya))
			# else:
				# money=player.GetGem()
				# if money != self.lastGaya:
					# if money < self.lastGaya:
						# self.lastGayaFlagBig=0
						# self.lastGayaFlagLess=1
						# self.lastGayaRange=self.lastGaya - money
						# self.lastGayaRangeBK=self.lastGayaRange
					# else:
						# self.lastGayaFlagLess=0
						# self.lastGayaFlagBig=1
						# self.lastGayaRange=money - self.lastGaya
						# self.lastGayaRangeBK=self.lastGayaRange
					
					# self.lastGayaBK=money

	def SetTop(self):
		super(ExpandedMoneyTaskBar, self).SetTop()

	def SetToggleButtonEvent(self, eButton, kEventFunc):
		self.toggleButtonDict[eButton].SetEvent(kEventFunc)

	def __SetCapitalSystemToolTip(self):
		self.toolTipMoneyTitle=uitooltip.ToolTip(20)
		self.toolTipMoneyTitle.AutoAppendTextLine(localeinfo.CHEQUE_SYSTEM_UNIT_YANG, uitooltip.ToolTip.PRICE_INFO_COLOR)
		self.toolTipMoneyTitle.AlignHorizonalCenter()
		# if app.ENABLE_HALLOWEEN_EVENT_2022:
			# self.toolTipSkullTitle = uitooltip.ToolTip(20)
			# self.toolTipSkullTitle.AutoAppendTextLine(localeinfo.UNIT_SKULLS, uitooltip.ToolTip.SKULL_PRICE_COLOR)
			# self.toolTipSkullTitle.AlignHorizonalCenter()

		# if app.ENABLE_GAYA_RENEWAL:
			# self.toolTipGayaTitle=uitooltip.ToolTip(20)
			# self.toolTipGayaTitle.AutoAppendTextLine(localeinfo.GAYA_SYSTEM_UNIT_GAYA, uitooltip.ToolTip.GAYA_PRICE_COLOR)
			# self.toolTipGayaTitle.AlignHorizonalCenter()

	def __ShowMoneyTitleToolTip(self):
		self.toolTipMoneyTitle.ShowToolTip()

	def __HideMoneyTitleToolTip(self):
		self.toolTipMoneyTitle.HideToolTip()

	# if app.ENABLE_HALLOWEEN_EVENT_2022:
		# def __ShowSkullTitleToolTip(self):
			# self.toolTipSkullTitle.ShowToolTip()
	
		# def __HideSkullTitleToolTip(self):
			# self.toolTipSkullTitle.HideToolTip()


	# if app.ENABLE_GAYA_RENEWAL:
		# def __ShowGayaTitleToolTip(self):
			# self.toolTipGayaTitle.ShowToolTip()

		# def __HideGayaTitleToolTip(self):
			# self.toolTipGayaTitle.HideToolTip()

class CostumeWindow(ui.ScriptWindow):
	availCostumeSlots = [item.COSTUME_SLOT_START-player.EQUIPMENT_SLOT_START,\
							item.COSTUME_SLOT_START+1-player.EQUIPMENT_SLOT_START,\
							item.COSTUME_SLOT_START+3-player.EQUIPMENT_SLOT_START,\
							item.COSTUME_SLOT_WEAPON-player.EQUIPMENT_SLOT_START,\
							player.COSTUME_PETSKIN_SLOT-player.EQUIPMENT_SLOT_START,\
							player.COSTUME_MOUNTSKIN_SLOT-player.EQUIPMENT_SLOT_START,\
							item.COSTUME_SLOT_AURA-player.EQUIPMENT_SLOT_START]

	tooltipItem = None

	isLoaded = 0

	def __init__(self, parent):
		ui.ScriptWindow.__init__(self)
		self.parent = parent
		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		if self.isLoaded==1:
			return
		
		self.isLoaded=1
		
		try:
			pyScrLoader=ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/costumewindow.py")
		except Exception, msg:
			import sys
			import dbg
			(type,msg,tb)=sys.exc_info()
			dbg.TraceError("CostumeWindow.LoadWindow.Load - %s : %s"%(type,msg))
			app.Abort()
			return 0
		
		try:
			titleBar=self.GetChild("TitleBar")
			titleBar.SetCloseEvent(ui.__mem_func__(self.CloseByTitle))
			self.titleBar=titleBar
			
			wndCostumeEquip=self.GetChild("costume_slot")
			wndCostumeEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.parent.SelectEmptySlot))
			wndCostumeEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.parent.SelectItemSlot))
			wndCostumeEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.parent.UseItemSlot))
			wndCostumeEquip.SetUseSlotEvent(ui.__mem_func__(self.parent.UseItemSlot))
			wndCostumeEquip.SetOverInItemEvent(ui.__mem_func__(self.parent.OverInItem))
			wndCostumeEquip.SetOverOutItemEvent(ui.__mem_func__(self.parent.OverOutItem))
			wndCostumeEquip.SetOverInEmptySlotEvent(ui.__mem_func__(self.parent.OverOutItem))
			wndCostumeEquip.SetOverOutEmptySlotEvent(ui.__mem_func__(self.parent.OverOutItem))
			self.wndCostumeEquip=wndCostumeEquip
			
			if app.ENABLE_HIDE_COSTUME_SYSTEM:
				self.visibleButtonListNo=[]
				self.visibleButtonListYes=[]
				for i in xrange(app.HIDE_PART_MAX):
					if app.ENABLE_ACCE_SYSTEM and\
						i == app.HIDE_STOLE_PART:
						self.visibleButtonListNo.append(0)
						self.visibleButtonListYes.append(0)
					else:
						tmp=self.GetChild("nohide_button_piece%d"%(i))
						tmp.SAFE_SetEvent(self.OnSetVisibleCostume, i)
						tmp.SetToolTipText(localeinfo.HIDE_COSTUME)
						self.visibleButtonListNo.append(tmp)
						
						tmp=self.GetChild("yeshide_button_piece%d"%(i))
						tmp.SAFE_SetEvent(self.OnSetVisibleCostume, i)
						tmp.SetToolTipText(localeinfo.SHOW_COSTUME)
						self.visibleButtonListYes.append(tmp)
			else:
				maxBtnNum = app.HIDE_PART_MAX if app.ENABLE_ACCE_SYSTEM else app.HIDE_PART_MAX - 1
				for i in xrange(maxBtnNum):
					if app.ENABLE_ACCE_SYSTEM and\
						i == app.HIDE_STOLE_PART:
						continue
					else:
						self.GetChild("nohide_button_piece%d"%(i)).Hide()
						self.GetChild("yeshide_button_piece%d"%(i)).Hide()
		except:
			import sys
			import dbg
			(type,msg,tb)=sys.exc_info()
			dbg.TraceError("CostumeWindow.LoadWindow.Bind - %s : %s"%(type,msg))
			app.Abort()
			return 0

	def Destroy(self):
		self.tooltipItem = None

		self.parent=0
		self.wndCostumeEquip=0
		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.visibleButtonListNo=[]
			self.visibleButtonListYes=[]
		
		self.ClearDictionary()

	def RefreshSlot(self):
		for slotNumber in self.availCostumeSlots:
			slotNumber += player.EQUIPMENT_SLOT_START
			itemVnum = player.GetItemIndex(slotNumber)
			if not itemVnum:
				self.wndCostumeEquip.ClearSlot(slotNumber)
				continue
			
			itemCount = player.GetItemCount(slotNumber)
			if itemCount <= 1:
				itemCount = 0
			
			self.wndCostumeEquip.SetItemSlot(slotNumber, itemVnum, itemCount)
			if app.ENABLE_CHANGELOOK:
				if player.GetItemTransmutation(slotNumber):
					self.wndCostumeEquip.DisableCoverButton(slotNumber)
				else:
					self.wndCostumeEquip.EnableCoverButton(slotNumber)

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def RefreshVisibleCostume(self):
			for i in xrange(app.HIDE_PART_MAX):
				if i == app.HIDE_STOLE_PART:
					continue
				
				try:
					stat = constinfo.hiddenStatusDict[i]
					if int(stat)==1:
						self.visibleButtonListYes[i].Show()
						self.visibleButtonListNo[i].Hide()
					else:
						self.visibleButtonListYes[i].Hide()
						self.visibleButtonListNo[i].Show()
				except:
					pass

		def RefreshVisibleCostumeOnce(self, part, stat):
			try:
				if part == app.HIDE_STOLE_PART:
					return

				if stat==1:
					self.visibleButtonListYes[part].Show()
					self.visibleButtonListNo[part].Hide()
				else:
					self.visibleButtonListYes[part].Hide()
					self.visibleButtonListNo[part].Show()
			except:
				pass

		def OnSetVisibleCostume(self, part):
			net.SendChatPacket("/hide_costume %d"%(part))

	def Open(self):
		self.LoadWindow()
		
		constinfo.isOpenedCostumeWindowWhenClosingEquipment=1
		
		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.RefreshVisibleCostume()
		
		self.RefreshSlot()
		self.Show()

	def Close(self):
		if constinfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			self.OnCloseQuestionDialog()
			return

		if not self.tooltipItem and self.parent:
			self.tooltipItem = self.parent.tooltipItem

		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

		self.Hide()

	def CloseByTitle(self):
		self.Close()
		constinfo.isOpenedCostumeWindowWhenClosingEquipment=0

	def OnPressEscapeKey(self):
		self.parent.Close()
		constinfo.isOpenedCostumeWindowWhenClosingEquipment=0
		return True

class EquipmentWindow(ui.ScriptWindow):
	availPrimarySlots = [0,1,2,3,4,5,6,9,10,\
							item.EQUIPMENT_BELT-player.EQUIPMENT_SLOT_START,\
							18,\
							item.COSTUME_SLOT_START+2-player.EQUIPMENT_SLOT_START]

	availSecondarySlots = [7,8,\
							player.COSTUME_EFFECT_BODY_SLOT-player.EQUIPMENT_SLOT_START,\
							player.COSTUME_EFFECT_WEAPON_SLOT-player.EQUIPMENT_SLOT_START]

	isLoaded = 0

	if app.USE_AUTO_AGGREGATE:
		isOpenedBraveCapeWindowWhenClosingInventory = 0
		IsClickBraveCapeWindow = False
		wndBraveCape = None

	def __init__(self, parent):
		ui.ScriptWindow.__init__(self)
		self.parent = parent
		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		if self.isLoaded==1:
			return
		
		self.isLoaded=1
		
		try:
			pyScrLoader=ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/equipmentwindow.py")
		except Exception, msg:
			import sys
			import dbg
			(type,msg,tb)=sys.exc_info()
			dbg.TraceError("EquipmentWindow.LoadWindow.Load - %s : %s"%(type,msg))
			app.Abort()
			return 0
		
		try:
			titleBar=self.GetChild("TitleBar")
			titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
			self.titleBar=titleBar
			
			wndPrimaryEquip=self.GetChild("primary_slot")
			wndPrimaryEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.parent.SelectEmptySlot))
			wndPrimaryEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.parent.SelectItemSlot))
			wndPrimaryEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.parent.UseItemSlot))
			wndPrimaryEquip.SetUseSlotEvent(ui.__mem_func__(self.parent.UseItemSlot))
			wndPrimaryEquip.SetOverInItemEvent(ui.__mem_func__(self.parent.OverInItem))
			wndPrimaryEquip.SetOverOutItemEvent(ui.__mem_func__(self.parent.OverOutItem))
			wndPrimaryEquip.SetOverInEmptySlotEvent(ui.__mem_func__(self.parent.OverOutItem))
			wndPrimaryEquip.SetOverOutEmptySlotEvent(ui.__mem_func__(self.parent.OverOutItem))
			self.wndPrimaryEquip=wndPrimaryEquip
			
			wndSecondaryEquip=self.GetChild("secondary_slot")
			wndSecondaryEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.parent.SelectEmptySlot))
			wndSecondaryEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.parent.SelectItemSlot))
			wndSecondaryEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.parent.UseItemSlot))
			wndSecondaryEquip.SetUseSlotEvent(ui.__mem_func__(self.parent.UseItemSlot))
			wndSecondaryEquip.SetOverInItemEvent(ui.__mem_func__(self.parent.OverInItem))
			wndSecondaryEquip.SetOverOutItemEvent(ui.__mem_func__(self.parent.OverOutItem))
			wndSecondaryEquip.SetOverInEmptySlotEvent(ui.__mem_func__(self.parent.OverOutItem))
			wndSecondaryEquip.SetOverOutEmptySlotEvent(ui.__mem_func__(self.parent.OverOutItem))
			self.wndSecondaryEquip=wndSecondaryEquip
			
			self.dsButton=self.GetChild("dragon_soul_button")
			self.dsButton.SetEvent(ui.__mem_func__(self.parent.OnPressDSButton))
			if app.ENABLE_EXTRA_INVENTORY:
				self.extraInvButton=self.GetChild("extra_inventory_button")
				self.extraInvButton.SetEvent(ui.__mem_func__(self.parent.OnPressExtraInvButton))
			else:
				self.GetChild("extra_inventory_button").Hide()
			
			if app.__ENABLE_NEW_OFFLINESHOP__:
				self.offlineshopButton=self.GetChild("premium_private_shop_button")
				self.offlineshopButton.SetEvent(ui.__mem_func__(self.parent.OnPressOfflineShopButton))
			else:
				self.GetChild("premium_private_shop_button").Hide()
			
			self.costumeButton=self.GetChild("costume_button")
			self.costumeButton.SetEvent(ui.__mem_func__(self.parent.OnPressCostumeButton))
			
			if app.ENABLE_HIDE_COSTUME_SYSTEM:
				self.visibleButtonListNo=[]
				self.visibleButtonListYes=[]
				
				for i in xrange(app.HIDE_PART_MAX):
					if app.ENABLE_ACCE_SYSTEM and\
						i == app.HIDE_STOLE_PART:
						tmp=self.GetChild("nohide_button_piece%d"%(i))
						tmp.SAFE_SetEvent(self.OnSetVisibleCostume, i)
						tmp.SetToolTipText(localeinfo.HIDE_COSTUME)
						self.visibleButtonListNo.append(tmp)
						
						tmp=self.GetChild("yeshide_button_piece%d"%(i))
						tmp.SAFE_SetEvent(self.OnSetVisibleCostume, i)
						tmp.SetToolTipText(localeinfo.SHOW_COSTUME)
						self.visibleButtonListYes.append(tmp)
					else:
						self.visibleButtonListNo.append(0)
						self.visibleButtonListYes.append(0)
			else:
				maxBtnNum = app.HIDE_PART_MAX if app.ENABLE_ACCE_SYSTEM else app.HIDE_PART_MAX - 1
				for i in xrange(maxBtnNum):
					if app.ENABLE_ACCE_SYSTEM and\
						i == app.HIDE_STOLE_PART:
						self.GetChild("nohide_button_piece%d"%(i)).Hide()
						self.GetChild("yeshide_button_piece%d"%(i)).Hide()
					else:
						continue

			if app.USE_AUTO_AGGREGATE:
				self.wndBraveCape = BraveCapeWindow(self)
		except:
			import sys
			import dbg
			(type,msg,tb)=sys.exc_info()
			dbg.TraceError("EquipmentWindow.LoadWindow.Bind - %s : %s"%(type,msg))
			app.Abort()
			return 0

	def Destroy(self):
		if app.USE_AUTO_AGGREGATE and self.wndBraveCape:
			self.wndBraveCape.Destroy()
			del self.wndBraveCape
			self.wndBraveCape = None

		self.parent=0
		self.titleBar=0
		self.wndPrimaryEquip=0
		self.wndSecondaryEquip=0
		self.dsButton=0
		if app.ENABLE_EXTRA_INVENTORY:
			self.extraInvButton=0
		
		if app.__ENABLE_NEW_OFFLINESHOP__:
			self.offlineshopButton=0
		
		self.costumeButton=0
		
		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.visibleButtonListNo=[]
			self.visibleButtonListYes=[]
		
		self.ClearDictionary()

	def RefreshSlot(self):
		for slotNumber in self.availPrimarySlots:
			slotNumber += player.EQUIPMENT_SLOT_START
			itemVnum = player.GetItemIndex(slotNumber)
			if not itemVnum:
				self.wndPrimaryEquip.ClearSlot(slotNumber)
				continue
			
			itemCount = player.GetItemCount(slotNumber)
			if itemCount <= 1:
				itemCount = 0
			
			self.wndPrimaryEquip.SetItemSlot(slotNumber, itemVnum, itemCount)
			if app.ENABLE_CHANGELOOK:
				if player.GetItemTransmutation(slotNumber):
					self.wndPrimaryEquip.DisableCoverButton(slotNumber)
				else:
					self.wndPrimaryEquip.EnableCoverButton(slotNumber)
		
		for slotNumber in self.availSecondarySlots:
			slotNumber += player.EQUIPMENT_SLOT_START
			itemVnum = player.GetItemIndex(slotNumber)
			if not itemVnum:
				self.wndSecondaryEquip.ClearSlot(slotNumber)
				continue
			
			itemCount = player.GetItemCount(slotNumber)
			if itemCount <= 1:
				itemCount = 0
			
			self.wndSecondaryEquip.SetItemSlot(slotNumber, itemVnum, itemCount)
			if app.ENABLE_CHANGELOOK:
				if player.GetItemTransmutation(slotNumber):
					self.wndSecondaryEquip.DisableCoverButton(slotNumber)
				else:
					self.wndSecondaryEquip.EnableCoverButton(slotNumber)

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def RefreshVisibleCostume(self):
			for i in xrange(app.HIDE_PART_MAX):
				if i != app.HIDE_STOLE_PART:
					continue
				
				try:
					stat = constinfo.hiddenStatusDict[i]
					if int(stat)==1:
						self.visibleButtonListYes[i].Show()
						self.visibleButtonListNo[i].Hide()
					else:
						self.visibleButtonListYes[i].Hide()
						self.visibleButtonListNo[i].Show()
				except:
					pass

		def RefreshVisibleCostumeOnce(self, part, stat):
			try:
				if part != app.HIDE_STOLE_PART:
					return
				
				if stat==1:
					self.visibleButtonListYes[part].Show()
					self.visibleButtonListNo[part].Hide()
				else:
					self.visibleButtonListYes[part].Hide()
					self.visibleButtonListNo[part].Show()
			except:
				pass

		def OnSetVisibleCostume(self, part):
			net.SendChatPacket("/hide_costume %d"%(part))

	def Open(self):
		self.LoadWindow()
		
		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.RefreshVisibleCostume()
		
		self.RefreshSlot()

		if app.USE_AUTO_AGGREGATE and self.wndBraveCape:
			self.wndBraveCape.Show(self.isOpenedBraveCapeWindowWhenClosingInventory)

		self.Show()

	def Close(self):
		if constinfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			self.OnCloseQuestionDialog()
			return
		
		if self.parent.wndCostume and\
			self.parent.wndCostume.IsShow():
			self.parent.wndCostume.Close()
		
		if self.parent.tooltipItem:
			self.parent.tooltipItem.HideToolTip()

		if app.USE_AUTO_AGGREGATE and self.wndBraveCape:
			self.isOpenedBraveCapeWindowWhenClosingInventory = self.wndBraveCape.IsOpeningInventory()
			self.wndBraveCape.Close()

		self.Hide()

	def OnPressEscapeKey(self):
		self.parent.Close()
		return True

	if app.USE_AUTO_AGGREGATE:
		def OnTop(self):
			if self.wndBraveCape:
				if self.IsClickBraveCapeWindow == False:
					self.wndBraveCape.SetTop()
				else:
					self.IsClickBraveCapeWindow = False

		def AdjustPosition(self, x, y, bCape):
			if self.wndBraveCape:
				self.wndBraveCape.AdjustPosition()

		def OnMoveWindow(self, x, y):
			if self.wndBraveCape:
				self.wndBraveCape.AdjustPosition()

		def SetClickCapeWindow(self, isclick):
			self.IsClickBraveCapeWindow = isclick

		def SetBraveCapeStatus(self, status):
			if self.wndBraveCape:
				self.wndBraveCape.SetStatus(status)

class MenuWindow(ui.ScriptWindow):
	BUTTONS_COUNT = 7
	
	def Func(self, arg):
		if not self.inventory:
			return
		
		self.inventory.SetTop()
		if arg == 0:
			self.inventory.OnPressOfflineShopSearch()
		elif arg == 1:
			self.inventory.OpenSwitchbot()
		elif arg == 2:
			self.inventory.OnPressBtnBiolog()
		elif arg == 3:
			self.inventory.OnPressBtnDungeoninfo()
		elif arg == 4:
			self.inventory.OpenCalendarWindow()
		elif arg == 5:
			self.inventory.AntiExpBtn()
		elif arg == 6:
			self.inventory.OpenWikiWindow() 

	def __init__(self, inventory):
		ui.ScriptWindow.__init__(self)
		
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/inventory_menu.py")
		except:
			import exception
			exception.Abort("InventoryMenuWindow.LoadWindow.LoadObject")
		
		self.board = self.GetChild("board")
		self.buttons = []
		for i in xrange(self.BUTTONS_COUNT):
			self.buttons.append(self.GetChild("button%d" % (i + 1)))
			self.buttons[i].SetEvent(lambda arg = i : self.Func(arg))
		self.width = self.board.GetWidth()
		self.height = self.board.GetHeight()
		self.inventory = inventory
		self.AdjustPosition()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()
		self.board = None
		self.buttons = []

	def Show(self):
		ui.ScriptWindow.Show(self)

	def Hide(self):
		ui.ScriptWindow.Hide(self)

	def AdjustPosition(self):
		(x, y) = self.inventory.GetGlobalPosition()
		self.SetPosition(x - self.width + 4, y + 32);

	def OnMoveWindow(self):
		(x, y) = self.GetGlobalPosition()
		self.inventory.AdjustPosition(x + self.width - 4, y - 32)

class InventoryWindow(ui.ScriptWindow):	
	USE_TYPE_TUPLE=("USE_CLEAN_SOCKET", "USE_CHANGE_ATTRIBUTE", "USE_ADD_ATTRIBUTE", "USE_ADD_ATTRIBUTE2", "USE_ADD_ACCESSORY_SOCKET", "USE_PUT_INTO_ACCESSORY_SOCKET", "USE_PUT_INTO_BELT_SOCKET", "USE_PUT_INTO_RING_SOCKET", "USE_CHANGE_COSTUME_ATTR", "USE_RESET_COSTUME_ATTR", "USE_REMOVE_ACCESSORY_SOCKETS")
	
	if app.USE_ATTR_6TH_7TH:
		USE_TYPE_TUPLE += ("USE_CHANGE_ATTRIBUTE2",)
	
	if app.ENABLE_ATTR_COSTUMES:
		removeAttrDialog=0
	
	if app.ENABLE_NEW_PET_EDITS:
		enchantPetDialog=0
	
	questionDialog=0
	tooltipItem=0
	dlgPick=0
	if app.WJ_ENABLE_TRADABLE_ICON:
		bindWnds=[]
	
	sellingSlotNumber=-1
	isLoaded=0
	wndEquipment=0
	wndmenu = 0
	wndCostume=0

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		if self.wndmenu:
			self.wndmenu.Show()
		ui.ScriptWindow.Show(self)

	def Hide(self):
		if self.wndmenu:
			self.wndmenu.Hide()
		ui.ScriptWindow.Hide(self)

	if app.WJ_ENABLE_TRADABLE_ICON:
		def BindWindow(self, wnd):
			self.bindWnds.append(wnd)

	def BindInterfaceClass(self, interface):
		self.interface = interface
		self.wndmenu.bind_interface(interface)

	def LoadWindow(self):
		if self.isLoaded==1:
			return
		
		self.isLoaded=1
		
		try:
			pyScrLoader=ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/inventorywindow.py")
		except Exception, msg:
			import sys
			import dbg
			(type,msg,tb)=sys.exc_info()
			dbg.TraceError("InventoryWindow.LoadWindow.Load - %s : %s"%(type,msg))
			app.Abort()
			return 0
		
		try:
			titleBar=self.GetChild("TitleBar")
			titleBar.SetCloseEvent(ui.__mem_func__(self.CloseByTitle))
			self.titleBar=titleBar
			
			self.inventoryTab=[]
			for i in xrange(player.INVENTORY_PAGE_COUNT):
				self.inventoryTab.append(self.GetChild("Inventory_Tab_%02d"%(i+1)))
				self.inventoryTab[i].SAFE_SetEvent(self.SetInventoryPage, i)
			
			
			wndItem=self.GetChild("ItemSlot")
			wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
			wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
			wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			wndItem.SetOverInEmptySlotEvent(ui.__mem_func__(self.OverOutItem))
			wndItem.SetOverOutEmptySlotEvent(ui.__mem_func__(self.OverOutItem))
			self.wndItem=wndItem

			self.wndmenu = MenuWindow(self)
		except:
			import sys
			import dbg
			(type,msg,tb)=sys.exc_info()
			dbg.TraceError("InventoryWindow.LoadWindow.Bind - %s : %s"%(type,msg))
			app.Abort()
			return 0
		
		self.wndEquipment=EquipmentWindow(self)
		
		dlgPick=uipickmoney.PickMoneyDialog()
		dlgPick.LoadDialog()
		dlgPick.Hide()
		self.dlgPick=dlgPick
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.attachMetinDialog=uiattachmetin.AttachMetinDialog(self)
			self.BindWindow(self.attachMetinDialog)
		else:
			self.attachMetinDialog=uiattachmetin.AttachMetinDialog()
		
		self.attachMetinDialog.Hide()
		
		if app.ENABLE_HIGHLIGHT_SYSTEM:
			self.listHighlightedSlot=[]
		
		if app.ENABLE_ACCE_SYSTEM or app.ENABLE_STOLE_COSTUME:
			self.listAttachedAcces=[]
		
		if app.ENABLE_ATTR_TRANSFER_SYSTEM:
			self.listAttachedTransfer=[]
			for i in xrange(3):
				self.listAttachedTransfer.append(999)
		
		if app.ENABLE_CHANGELOOK:
			self.listAttachedClSlot=[]

		self.RefreshItemSlot()

	def Destroy(self):
		if self.wndEquipment:
			self.wndEquipment.Destroy()
			del self.wndEquipment
			self.wndEquipment=0
            
		if self.wndmenu:
			self.wndmenu.Destroy()
			del self.wndmenu
			self.wndmenu=0
		
		if self.wndCostume:
			self.wndCostume.Destroy()
			del self.wndCostume
			self.wndCostume=0
		
		if self.dlgPick:
			self.dlgPick.Destroy()
			del self.dlgPick
			self.dlgPick=0
		
		if self.questionDialog:
			self.questionDialog.Destroy()
			del self.questionDialog
			self.questionDialog=0
		
		if self.attachMetinDialog:
			self.attachMetinDialog.Destroy()
			del self.attachMetinDialog
			self.attachMetinDialog=0
		
		if app.NEW_PET_SYSTEM:
			if self.enchantPetDialog:
				self.enchantPetDialog.Close()
				del self.enchantPetDialog
				self.enchantPetDialog=0
		
		if app.ENABLE_ATTR_COSTUMES:
			if self.removeAttrDialog:
				self.removeAttrDialog.Close()
				del self.removeAttrDialog
				self.removeAttrDialog=0

		if self.wndmenu:
			self.wndmenu.Close()
			del self.wndmenu
			self.wndmenu=0

		self.titleBar=0
		self.inventoryTab=[]
		self.wndItem=0
		self.tooltipItem=0
		
		if app.ENABLE_HIGHLIGHT_SYSTEM:
			self.listHighlightedSlot=[]
		
		if app.ENABLE_ACCE_SYSTEM or app.ENABLE_STOLE_COSTUME:
			self.listAttachedAcces=[]
		
		if app.ENABLE_ATTR_TRANSFER_SYSTEM:
			self.listAttachedTransfer=[]
		
		if app.ENABLE_CHANGELOOK:
			self.listAttachedClSlot=[]
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.bindWnds=[]
		
		self.ClearDictionary()

	def __InventoryLocalSlotPosToGlobalSlotPos(self, local):
		if player.IsEquipmentSlot(local) or player.IsCostumeSlot(local) or player.IsBeltInventorySlot(local):
			return local
		
		return constinfo.inventoryPageIndex*player.INVENTORY_PAGE_SIZE + local

	def SelectEmptySlot(self, selectedSlotPos):
		if constinfo.GET_ITEM_QUESTION_DIALOG_STATUS()==1:
			return
		elif app.ENABLE_AURA_SYSTEM and player.IsAuraRefineWindowOpen():
			return
		elif app.ENABLE_CHANGELOOK and player.GetChangeLookWindowOpen():
			return
		
		if app.USE_ATTR_6TH_7TH:
			interface=newconstinfo._interface_instance
			if interface and interface.wndAttr67Add.IsShow():
				return
		
		selectedSlotPos=self.__InventoryLocalSlotPosToGlobalSlotPos(selectedSlotPos)
		
		if mousemodule.mouseController.isAttached():
			attachedSlotType=mousemodule.mouseController.GetAttachedType()
			attachedSlotPos=mousemodule.mouseController.GetAttachedSlotNumber()
			attachedItemCount=mousemodule.mouseController.GetAttachedItemCount()
			attachedItemIndex=mousemodule.mouseController.GetAttachedItemIndex()
			if app.__ENABLE_NEW_OFFLINESHOP__:
				if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(player.SlotTypeToInvenType(attachedSlotType), attachedSlotPos):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
					return
			
			if player.SLOT_TYPE_INVENTORY==attachedSlotType:
				attachedInvenType=player.SlotTypeToInvenType(attachedSlotType)
				if player.IsDSEquipmentSlot(attachedInvenType, attachedSlotPos):
					mousemodule.mouseController.DeattachObject()
					return
				
				itemCount=player.GetItemCount(attachedSlotPos)
				attachedCount=mousemodule.mouseController.GetAttachedItemCount()
				self.__SendMoveItemPacket(attachedSlotPos, selectedSlotPos, attachedCount)
				if item.IsRefineScroll(attachedItemIndex):
					self.wndItem.SetUseMode(False)
			elif app.ENABLE_SWITCHBOT_WORLDARD and player.SLOT_TYPE_SWITCHBOT==attachedSlotType:
				attachedCount=mousemodule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.SWITCHBOT, attachedSlotPos, player.INVENTORY, selectedSlotPos, attachedCount)
			elif app.ENABLE_AURA_SYSTEM and player.SLOT_TYPE_AURA==attachedSlotType:
				net.SendAuraRefineCheckOut(attachedSlotPos, player.GetAuraRefineWindowType())
			elif player.SLOT_TYPE_PRIVATE_SHOP==attachedSlotType:
				mousemodule.mouseController.RunCallBack("INVENTORY")
			elif player.SLOT_TYPE_SHOP==attachedSlotType:
				net.SendShopBuyPacket(attachedSlotPos)
			elif player.SLOT_TYPE_SAFEBOX==attachedSlotType:
				if player.ITEM_MONEY==attachedItemIndex:
					net.SendSafeboxWithdrawMoneyPacket(mousemodule.mouseController.GetAttachedItemCount())
					snd.PlaySound("sound/ui/money.wav")
				else:
					net.SendSafeboxCheckoutPacket(attachedSlotPos, selectedSlotPos)
			elif player.SLOT_TYPE_MALL==attachedSlotType:
				net.SendMallCheckoutPacket(attachedSlotPos, selectedSlotPos)
			
			mousemodule.mouseController.DeattachObject()

	def SelectItemSlot(self, itemSlotIndex):
		if constinfo.GET_ITEM_QUESTION_DIALOG_STATUS()==1:
			return
		
		itemSlotIndex=self.__InventoryLocalSlotPosToGlobalSlotPos(itemSlotIndex)
		
		if mousemodule.mouseController.isAttached():
			attachedSlotType=mousemodule.mouseController.GetAttachedType()
			attachedSlotPos=mousemodule.mouseController.GetAttachedSlotNumber()
			attachedItemVID=mousemodule.mouseController.GetAttachedItemIndex()
			if app.ENABLE_EXTRA_INVENTORY:
				if attachedSlotType==player.SLOT_TYPE_INVENTORY or attachedSlotType==player.SLOT_TYPE_EXTRA_INVENTORY:
					attachedInvenType=player.SlotTypeToInvenType(attachedSlotType)
					if player.IsDSEquipmentSlot(attachedInvenType, attachedSlotPos):
						mousemodule.mouseController.DeattachObject()
						return
					
					if attachedItemVID==player.GetItemIndex(itemSlotIndex)  and attachedItemVID >= 70000 and attachedItemVID <= 99999:
						self.__SendMoveItemPacket(attachedSlotPos, itemSlotIndex, 0)
					else:
						self.__DropSrcItemToDestItemInInventory(attachedItemVID, attachedSlotPos, itemSlotIndex, attachedSlotType)
			else:
				if player.SLOT_TYPE_INVENTORY==attachedSlotType:
					attachedInvenType=player.SlotTypeToInvenType(attachedSlotType)
					if player.IsDSEquipmentSlot(attachedInvenType, attachedSlotPos):
						mousemodule.mouseController.DeattachObject()
						return
					
					self.__DropSrcItemToDestItemInInventory(attachedItemVID, attachedSlotPos, itemSlotIndex)
			
			mousemodule.mouseController.DeattachObject()
		else:
			curCursorNum=app.GetCursor()
			if app.SELL==curCursorNum:
				self.__SellItem(itemSlotIndex)
			elif app.BUY==curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.SHOP_BUY_INFO)
			elif app.IsPressed(app.DIK_LALT):
				link=player.GetItemLink(itemSlotIndex)
				ime.PasteString(link)
			elif app.IsPressed(app.DIK_LSHIFT):
				itemCount=player.GetItemCount(itemSlotIndex)
				if itemCount > 1:
					self.dlgPick.SetTitleName(localeinfo.PICK_ITEM_TITLE)
					self.dlgPick.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
					self.dlgPick.Open(itemCount, 1, False)
					self.dlgPick.itemGlobalSlotIndex=itemSlotIndex
				else:
					selectedItemVNum=player.GetItemIndex(itemSlotIndex)
					mousemodule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum)
			elif app.IsPressed(app.DIK_LCONTROL):
				itemIndex=player.GetItemIndex(itemSlotIndex)
				if True==item.CanAddToQuickSlotItem(itemIndex):
					player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_INVENTORY, itemSlotIndex)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.QUICKSLOT_REGISTER_DISABLE_ITEM)
			else:
				selectedItemVNum=player.GetItemIndex(itemSlotIndex)
				itemCount=player.GetItemCount(itemSlotIndex)
				mousemodule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)
				if self.__IsUsableItemToItem(selectedItemVNum, itemSlotIndex):
					self.wndItem.SetUseMode(True)
				else:
					self.wndItem.SetUseMode(False)
				
				snd.PlaySound("sound/ui/pick.wav")

	def UseItemSlot(self, slotIndex):
		slotIndex=self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)
		
		itemVnum=player.GetItemIndex(player.INVENTORY, slotIndex)
		if not itemVnum:
			return
		
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop():
				item.SelectItem(itemVnum)
				if not item.IsAntiFlag(item.ANTIFLAG_GIVE) and not item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
					if app.ENABLE_OFFLINESHOP_REWORK:
						offlineshop.ShopBuilding_AddItem(player.INVENTORY, slotIndex)
					else:
						offlineshop.ShopBuilding_AddInventoryItem(slotIndex)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				
				return
			elif uiofflineshop.IsBuildingAuction():
				item.SelectItem(itemVnum)
				if not item.IsAntiFlag(item.ANTIFLAG_GIVE) and not item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
					offlineshop.AuctionBuilding_AddInventoryItem(globalSlot)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				
				return
		
		curCursorNum=app.GetCursor()
		if app.SELL==curCursorNum:
			return
		elif constinfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			return
		elif app.ENABLE_DRAGON_SOUL_SYSTEM and self.wndDragonSoulRefine.IsShow():
			self.wndDragonSoulRefine.AutoSetItem((player.INVENTORY, slotIndex), 1)
			return
		elif app.ENABLE_ACCE_SYSTEM and self.isShowAcceWindow():
			acce.Add(player.INVENTORY, slotIndex, 255)
			return
		elif app.ENABLE_CHANGELOOK and self.isShowChangeLookWindow():
			changelook.Add(player.INVENTORY, slotIndex, 255)
			return
		
		if app.ITEM_CHECKINOUT_UPDATE:
			if self.wndSafeBox.IsShow() and slotIndex < player.EQUIPMENT_SLOT_START:
				net.SendSafeboxCheckinPacket(slotIndex)
				return
			
			if exchange.isTrading() and slotIndex < player.EQUIPMENT_SLOT_START:
				net.SendExchangeItemAddPacket(player.INVENTORY, slotIndex, -1)
				return
		
		if app.ENABLE_NEW_PET_EDITS:
			if itemVnum==55002:
				metinSocket=[player.GetItemMetinSocket(slotIndex, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
				if metinSocket[0] != 0:
					self.questionDialog=uicommon.QuestionDialog()
					self.questionDialog.SetText(localeinfo.PETBOX_RIMUOVE)
					self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UsePetBoxDialog_OnAccept))
					self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
					self.questionDialog.Open()
					self.questionDialog.slotIndex=slotIndex
					constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
					return

		if app.INGAME_WIKI and app.IsPressed(app.DIK_LCONTROL):
			view = 0

			if app.USE_MULTIPLE_OPENING:
				if itemVnum != 27987:
					view = item.CanBeMultipleOpened(itemVnum)
			else:
				item.SelectItem(itemVnum)
				view = 1 if item.GetItemType() in (item.ITEM_TYPE_GIFTBOX, item.ITEM_GACHA) else 0

			if view:
				try:
					import newconstinfo
					if not newconstinfo._game_instance.wndWiki.IsShow():
						net.ToggleWikiWindow()

					newconstinfo._game_instance.wndWiki.CloseBaseWindows()
					newconstinfo._game_instance.wndWiki.mainChestWindow.Show([itemVnum,])
					return
				except Exception as e:
					import dbg
					dbg.TraceError(str(e))

					mousemodule.mouseController.DeattachObject()
					self.OverOutItem()
					return

		if app.USE_MULTIPLE_OPENING and (app.IsPressed(app.DIK_LSHIFT) or app.IsPressed(app.DIK_RSHIFT)):
			if itemVnum > 0:
				if item.CanBeMultipleOpened(itemVnum) == 1:
					net.SendMultipleOpening(player.INVENTORY, slotIndex, player.GetItemCount(slotIndex))
				else:
					self.__UseItem(slotIndex)
		else:
			self.__UseItem(slotIndex)

		mousemodule.mouseController.DeattachObject()
		self.OverOutItem()

	def OverInItem(self, overSlotPos):
		if app.ENABLE_HIGHLIGHT_SYSTEM:
			slotNumber=self.__InventoryLocalSlotPosToGlobalSlotPos(overSlotPos)
			if slotNumber in self.listHighlightedSlot:
				self.wndItem.DeactivateSlot(overSlotPos)
				self.listHighlightedSlot.remove(slotNumber)
		
		overSlotPos=self.__InventoryLocalSlotPosToGlobalSlotPos(overSlotPos)
		self.wndItem.SetUsableItem(False)
		
		if mousemodule.mouseController.isAttached():
			attachedItemType=mousemodule.mouseController.GetAttachedType()
			
			if app.ENABLE_EXTRA_INVENTORY:
				if attachedItemType==player.SLOT_TYPE_INVENTORY or attachedItemType==player.SLOT_TYPE_EXTRA_INVENTORY:
					attachedSlotPos=mousemodule.mouseController.GetAttachedSlotNumber()
					attachedItemVNum=mousemodule.mouseController.GetAttachedItemIndex()
					if attachedItemVNum==player.ITEM_MONEY:
						pass
					elif self.__CanUseSrcItemToDstItem(attachedItemVNum, attachedSlotPos, overSlotPos):
						self.wndItem.SetUsableItem(True)
						self.wndItem.SetUseMode(True)
						self.ShowToolTip(overSlotPos)
						return
					elif attachedItemVNum==player.GetItemIndex(overSlotPos) and attachedItemVNum >= 70000 and attachedItemVNum <= 99999:
						self.wndItem.SetUsableItem(False)
						self.wndItem.SetUseMode(False)
						self.ShowToolTip(overSlotPos)
						return
			else:
				if player.SLOT_TYPE_INVENTORY==attachedItemType:
					attachedSlotPos=mousemodule.mouseController.GetAttachedSlotNumber()
					attachedItemVNum=mousemodule.mouseController.GetAttachedItemIndex()
					if attachedItemVNum==player.ITEM_MONEY: # @fixme005
						pass
					elif self.__CanUseSrcItemToDstItem(attachedItemVNum, attachedSlotPos, overSlotPos):
						self.wndItem.SetUsableItem(True)
						self.ShowToolTip(overSlotPos)
						return
		
		self.ShowToolTip(overSlotPos)

	def OverOutItem(self):
		self.wndItem.SetUsableItem(False)
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def Open(self):
		self.LoadWindow()
		if self.wndEquipment and\
			not self.wndEquipment.IsShow():
			self.wndEquipment.Open()
			self.wndEquipment.SetTop()
		
		if constinfo.isOpenedCostumeWindowWhenClosingEquipment:
			if not self.wndCostume:
				self.wndCostume=CostumeWindow(self)
			
			self.wndCostume.Open()
			self.wndCostume.SetTop()
		
		self.SetInventoryPage(constinfo.inventoryPageIndex)
		self.Show()
		self.SetTop()

		if self.wndmenu:
			self.wndmenu.Show()

	def Close(self, withEquip = 1):
		if constinfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			self.OnCloseQuestionDialog()
			return

		if withEquip == 1 and\
			self.wndEquipment and\
			self.wndEquipment.IsShow():
			self.wndEquipment.Close()

			if self.wndCostume:
				self.wndCostume.Close()

		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

		if self.dlgPick:
			self.dlgPick.Close()

		if self.wndmenu:
			self.wndmenu.Hide()

		self.Hide()

	def CloseByTitle(self):
		self.Close(0)

	if app.ENABLE_HIGHLIGHT_SYSTEM:
		def HighlightSlot(self, slot):
			if not slot in self.listHighlightedSlot:
				self.listHighlightedSlot.append(slot)

	def SetInventoryPage(self, page):
		constinfo.inventoryPageIndex=page
		
		for btn in self.inventoryTab:
			btn.SetUp()
		
		self.inventoryTab[page].Down()
		
		self.RefreshBagSlotWindow()

	def OnPickItem(self, count):
		itemSlotIndex=self.dlgPick.itemGlobalSlotIndex
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(player.INVENTORY, itemSlotIndex):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return
		
		division=self.dlgPick.division
		if division:
			net.SendItemDivisionPacket(itemSlotIndex, player.INVENTORY)
		else:
			selectedItemVNum=player.GetItemIndex(player.INVENTORY, itemSlotIndex)
			mousemodule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, count)

	def GetInventoryPageIndex(self):
		return constinfo.inventoryPageIndex

	if app.ENABLE_ATTR_TRANSFER_SYSTEM:
		def TransferSlot(self, main, target):
			self.listAttachedTransfer[main]=target
			self.RefreshMarkSlots()
			self.RefreshBagSlotWindow()

		def ClearTransferSlot(self):
			for i in xrange(3):
				self.listAttachedTransfer[i]=999
			
			self.RefreshMarkSlots()
			self.RefreshBagSlotWindow()

	if app.WJ_ENABLE_TRADABLE_ICON:
		def RefreshMarkSlots(self, localIndex=0):
			unusable=False
			onTopWnd=newconstinfo._interface_instance.GetOnTopWindow()
			if localIndex:
				slotNumber=self.__InventoryLocalSlotPosToGlobalSlotPos(localIndex)
				if onTopWnd==player.ON_TOP_WND_NONE:
					self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				elif onTopWnd==player.ON_TOP_WND_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SELL):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
						unusable=True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				elif onTopWnd==player.ON_TOP_WND_EXCHANGE:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_GIVE):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
						unusable=True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				elif onTopWnd==player.ON_TOP_WND_PRIVATE_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ITEM_ANTIFLAG_MYSHOP):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
						unusable=True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				elif onTopWnd==player.ON_TOP_WND_SAFEBOX:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SAFEBOX):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
						unusable=True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				
				if app.ENABLE_ACCE_SYSTEM and not unusable:
					self.acceSlot=[]
					for r in xrange(acce.WINDOW_MAX_MATERIALS):
						(isHere, iCell)=acce.GetAttachedItem(r)
						if isHere:
							if iCell==slotNumber:
								if not slotNumber in self.acceSlot:
									self.acceSlot.append(slotNumber)
								
								self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
								unusable=True
							else:
								for j in self.acceSlot:
									if j != slotNumber:
										self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				
				if app.ENABLE_ATTR_TRANSFER_SYSTEM and not unusable:
					self.transferSlot=[]
					for r in xrange(len(self.listAttachedTransfer)):
						idx=self.listAttachedTransfer[r]
						if idx != 999:
							if idx==slotNumber:
								if not slotNumber in self.acceSlot:
									self.transferSlot.append(slotNumber)
								
								self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
								unusable=True
							else:
								for j in self.transferSlot:
									if j != slotNumber:
										self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				
				if app.ENABLE_AURA_SYSTEM and not unusable:
					if player.FindActivatedAuraSlot(player.INVENTORY, slotNumber) != player.AURA_SLOT_MAX:
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
						unusable=True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				
				return
			
			for i in xrange(player.INVENTORY_PAGE_SIZE):
				slotNumber=self.__InventoryLocalSlotPosToGlobalSlotPos(i)
				if onTopWnd==player.ON_TOP_WND_NONE:
					self.wndItem.SetUsableSlotOnTopWnd(i)
				elif onTopWnd==player.ON_TOP_WND_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SELL):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
						unusable=True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)
				elif onTopWnd==player.ON_TOP_WND_EXCHANGE:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_GIVE):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
						unusable=True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)
				elif onTopWnd==player.ON_TOP_WND_PRIVATE_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ITEM_ANTIFLAG_MYSHOP):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
						unusable=True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)
				elif onTopWnd==player.ON_TOP_WND_SAFEBOX:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SAFEBOX):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
						unusable=True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)
				
				if app.ENABLE_ACCE_SYSTEM and not unusable:
					self.acceSlot=[]
					for r in xrange(acce.WINDOW_MAX_MATERIALS):
						(isHere, iCell)=acce.GetAttachedItem(r)
						if isHere:
							if iCell==slotNumber:
								if not slotNumber in self.acceSlot:
									self.acceSlot.append(slotNumber)
								
								self.wndItem.SetUnusableSlotOnTopWnd(i)
								unusable=True
							else:
								for j in self.acceSlot:
									if j != slotNumber:
										self.wndItem.SetUsableSlotOnTopWnd(i)
				
				if app.ENABLE_ATTR_TRANSFER_SYSTEM and not unusable:
					self.transferSlot=[]
					for r in xrange(len(self.listAttachedTransfer)):
						idx=self.listAttachedTransfer[r]
						if idx != 999:
							if idx==slotNumber:
								if not slotNumber in self.transferSlot:
									self.transferSlot.append(idx)
								
								self.wndItem.SetUnusableSlotOnTopWnd(i)
								unusable=True
							else:
								for j in self.transferSlot:
									if j != slotNumber:
										self.wndItem.SetUsableSlotOnTopWnd(i)
				
				if app.ENABLE_AURA_SYSTEM and not unusable:
					if player.IsAuraRefineWindowOpen() and player.FindActivatedAuraSlot(player.INVENTORY, slotNumber) != player.AURA_SLOT_MAX:
						self.wndItem.SetUnusableSlotOnTopWnd(i)
						unusable=True
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

	def RefreshBagSlotWindow(self):
		getItemVNum=player.GetItemIndex
		getItemCount=player.GetItemCount
		
		for i in xrange(player.INVENTORY_PAGE_SIZE):
			slotNumber=self.__InventoryLocalSlotPosToGlobalSlotPos(i)
			itemCount=getItemCount(slotNumber)
			if 0==itemCount:
				self.wndItem.ClearSlot(i)
				continue
			elif 1==itemCount:
				itemCount=0
			
			itemVnum=getItemVNum(slotNumber)
			if not itemVnum:
				continue
			
			self.wndItem.SetItemSlot(i, itemVnum, itemCount)
			
			if app.ENABLE_CHANGELOOK:
				changelookvnum=player.GetItemTransmutation(slotNumber)
				if changelookvnum:
					self.wndItem.DisableCoverButton(i)
				else:
					self.wndItem.EnableCoverButton(i)
			
			wasActivated=0
			
			if app.ENABLE_HIGHLIGHT_SYSTEM:
				if slotNumber in self.listHighlightedSlot:
					self.wndItem.ActivateSlotEffect(i, (255.00 / 255.0), (20.00 / 255.0), (22.00 / 255.0), 1.0)
					wasActivated=1
			
			if wasActivated==0 and app.ENABLE_ACCE_SYSTEM:
				slotNumberChecked=0
				for j in xrange(acce.WINDOW_MAX_MATERIALS):
					(isHere, iCell)=acce.GetAttachedItem(j)
					if isHere:
						if iCell==slotNumber:
							self.wndItem.ActivateSlot(i)
							wasActivated=1
							
							if not slotNumber in self.listAttachedAcces:
								self.listAttachedAcces.append(slotNumber)
							
							slotNumberChecked=1
							break
					else:
						if slotNumber in self.listAttachedAcces and not slotNumberChecked:
							self.listAttachedAcces.remove(slotNumber)
			
			if wasActivated==0 and app.ENABLE_CHANGELOOK:
				slotNumberChecked=0
				for j in xrange(changelook.WINDOW_MAX_MATERIALS):
					(isHere, iCell)=changelook.GetAttachedItem(j)
					if isHere:
						if iCell==slotNumber:
							self.wndItem.ActivateSlotEffect(i, (238.00 / 255.0), (11.00 / 255.0), (11.00 / 255.0), 1.0)
							wasActivated=1
							
							if not slotNumber in self.listAttachedClSlot:
								self.listAttachedClSlot.append(slotNumber)
							
							slotNumberChecked=1
							break
					else:
						if slotNumber in self.listAttachedClSlot and not slotNumberChecked:
							self.listAttachedClSlot.remove(slotNumber)
			
			if wasActivated==0 and app.ENABLE_AURA_SYSTEM:
				slotNumberChecked=0
				if player.IsAuraRefineWindowOpen() and player.FindActivatedAuraSlot(player.INVENTORY, slotNumber) != player.AURA_SLOT_MAX:
					self.wndItem.ActivateSlotEffect(i, (56.00 / 255.0), (237.00 / 255.0), (77.00 / 255.0), 1.0)
					wasActivated=1
			
			if wasActivated==0:
				if constinfo.IS_AUTO_POTION(itemVnum):
					metinSocket=[player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					isActivated=0 != metinSocket[0]
					if isActivated:
						self.wndItem.ActivateSlot(i)
						wasActivated=1
						
						potionType=0;
						if constinfo.IS_AUTO_POTION_HP(itemVnum):
							potionType=player.AUTO_POTION_TYPE_HP
						elif constinfo.IS_AUTO_POTION_SP(itemVnum):
							potionType=player.AUTO_POTION_TYPE_SP
						
						usedAmount=int(metinSocket[1])
						totalAmount=int(metinSocket[2])
						player.SetAutoPotionInfo(potionType, isActivated, (totalAmount - usedAmount), totalAmount, self.__InventoryLocalSlotPosToGlobalSlotPos(i))
				elif app.ENABLE_NEW_USE_POTION and player.GetItemTypeBySlot(slotNumber)==item.ITEM_TYPE_USE and player.GetItemSubTypeBySlot(slotNumber)==item.USE_NEW_POTIION:
					metinSocket=[player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					isActivated=0 != metinSocket[1]
					if isActivated:
						self.wndItem.ActivateSlot(i)
						wasActivated=1
				elif app.ENABLE_SOUL_SYSTEM and player.GetItemTypeBySlot(slotNumber)==item.ITEM_TYPE_SOUL:
					metinSocket=[player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					if 0 != metinSocket[1]:
						self.wndItem.ActivateSlot(i)
						wasActivated=1
				elif app.NEW_PET_SYSTEM and constinfo.IS_PET_SEAL(itemVnum):
					metinSocket=[player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					isActivated=0 != metinSocket[0]
					if isActivated:
						self.wndItem.ActivateSlot(i)
						wasActivated=1
				elif constinfo.IS_PET_SEAL_OLD(itemVnum):
					metinSocket=[player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					isActivated=0 != metinSocket[2]
					if isActivated:
						self.wndItem.ActivateSlot(i)
						wasActivated=1
			
			if app.ENABLE_HIGHLIGHT_SYSTEM and not wasActivated:
				self.wndItem.DeactivateSlot(i)
			
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.RefreshMarkSlots(i)
		
		self.wndItem.RefreshSlot()
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			map(lambda wnd:wnd.RefreshLockedSlot(), self.bindWnds)

	def RefreshItemSlot(self):
		self.wndEquipment.RefreshSlot()
		if self.wndCostume:
			self.wndCostume.RefreshSlot()
		
		self.RefreshBagSlotWindow()

	def RefreshInventoryCell(self, slotIndex):
		for i in xrange(player.INVENTORY_PAGE_SIZE):
			slotNumber=self.__InventoryLocalSlotPosToGlobalSlotPos(i)
			if slotIndex==slotNumber:
				itemCount=player.GetItemCount(slotNumber)
				if 0==itemCount:
					self.wndItem.ClearSlot(i)
					break
				elif 1==itemCount:
					itemCount=0
				
				itemVnum=player.GetItemIndex(slotNumber)
				if not itemVnum:
					break
				
				self.wndItem.SetItemSlot(i, itemVnum, itemCount)
				
				wasActivated=0
				if app.ENABLE_HIGHLIGHT_SYSTEM:
					if slotNumber in self.listHighlightedSlot:
						self.wndItem.ActivateSlotEffect(i, (255.00 / 255.0), (20.00 / 255.0), (22.00 / 255.0), 1.0)
						wasActivated=1
				
				if app.ENABLE_ACCE_SYSTEM:
					slotNumberChecked=0
					for j in xrange(acce.WINDOW_MAX_MATERIALS):
						(isHere, iCell)=acce.GetAttachedItem(j)
						if isHere:
							if iCell==slotNumber:
								self.wndItem.ActivateSlot(i)
								wasActivated=1
								
								if not slotNumber in self.listAttachedAcces:
									self.listAttachedAcces.append(slotNumber)
								
								slotNumberChecked=1
								break
						else:
							if slotNumber in self.listAttachedAcces and not slotNumberChecked:
								self.listAttachedAcces.remove(slotNumber)
				
				if app.ENABLE_AURA_SYSTEM:
					slotNumberChecked=0
					if player.IsAuraRefineWindowOpen() and player.FindActivatedAuraSlot(player.INVENTORY, slotNumber) != player.AURA_SLOT_MAX:
						self.wndItem.ActivateSlotEffect(i, (56.00 / 255.0), (237.00 / 255.0), (77.00 / 255.0), 1.0)
						wasActivated=1
				
				if constinfo.IS_AUTO_POTION(itemVnum):
					metinSocket=[player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					isActivated=0 != metinSocket[0]
					if isActivated:
						self.wndItem.ActivateSlot(i)
						wasActivated=1
						
						potionType=0;
						if constinfo.IS_AUTO_POTION_HP(itemVnum):
							potionType=player.AUTO_POTION_TYPE_HP
						elif constinfo.IS_AUTO_POTION_SP(itemVnum):
							potionType=player.AUTO_POTION_TYPE_SP
						
						usedAmount=int(metinSocket[1])
						totalAmount=int(metinSocket[2])
						player.SetAutoPotionInfo(potionType, isActivated, (totalAmount - usedAmount), totalAmount, self.__InventoryLocalSlotPosToGlobalSlotPos(i))
				elif app.ENABLE_NEW_USE_POTION and player.GetItemTypeBySlot(slotNumber)==item.ITEM_TYPE_USE and player.GetItemSubTypeBySlot(slotNumber)==item.USE_NEW_POTIION:
					metinSocket=[player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					isActivated=0 != metinSocket[1]
					if isActivated:
						self.wndItem.ActivateSlot(i)
						wasActivated=1
				elif app.ENABLE_SOUL_SYSTEM and player.GetItemTypeBySlot(slotNumber)==item.ITEM_TYPE_SOUL:
					metinSocket=[player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					if 0 != metinSocket[1]:
						self.wndItem.ActivateSlot(i)
						wasActivated=1
				elif app.NEW_PET_SYSTEM and constinfo.IS_PET_SEAL(itemVnum):
					metinSocket=[player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					isActivated=0 != metinSocket[0]
					if isActivated:
						self.wndItem.ActivateSlot(i)
						wasActivated=1
				elif constinfo.IS_PET_SEAL_OLD(itemVnum):
					metinSocket=[player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					isActivated=0 != metinSocket[2]
					if isActivated:
						self.wndItem.ActivateSlot(i)
						wasActivated=1
				
				if app.ENABLE_HIGHLIGHT_SYSTEM and not wasActivated:
					self.wndItem.DeactivateSlot(i)
				
				#if app.WJ_ENABLE_TRADABLE_ICON:
				#	self.RefreshMarkSlots(i)
				
				self.wndItem.RefreshSlot()
				
				#if app.WJ_ENABLE_TRADABLE_ICON:
				#	map(lambda wnd:wnd.RefreshLockedSlot(), self.bindWnds)
				
				break

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem=tooltipItem

	def SellItem(self):
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(player.INVENTORY, self.sellingSlotNumber):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return

		if self.sellingSlotitemIndex==player.GetItemIndex(self.sellingSlotNumber):
			if self.sellingSlotitemCount==player.GetItemCount(self.sellingSlotNumber):
				net.SendShopSellPacketNew(self.sellingSlotNumber, self.questionDialog.count, player.INVENTORY)
				snd.PlaySound("sound/ui/money.wav")
		self.OnCloseQuestionDialog()

	def OnDetachMetinFromItem(self):
		if not self.questionDialog:
			return
		
		#net.SendItemUseToItemPacket(self.questionDialog.sourcePos, self.questionDialog.targetPos)
		self.__SendUseItemToItemPacket(self.questionDialog.sourcePos, self.questionDialog.targetPos)
		self.OnCloseQuestionDialog()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return

		self.questionDialog.Close()
		self.questionDialog=0
		constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	if app.NEW_PET_SYSTEM:
		def UseTransportBox(self):
			self.__SendUseItemToItemPacket(self.questionDialog.src, self.questionDialog.dst)
			self.OnCloseQuestionDialog()
		
		def UseProtein(self):
			self.__SendUseItemToItemPacket(self.questionDialog.src, self.questionDialog.dst)
			self.OnCloseQuestionDialog()

	if app.ENABLE_AURA_SYSTEM:
		def __UseItemAuraQuestionDialog_OnAccept(self):
			self.questionDialog.Close()
			net.SendAuraRefineCheckIn(*(self.questionDialog.srcItem + self.questionDialog.dstItem + (player.GetAuraRefineWindowType(),)))
			self.questionDialog.srcItem=(0, 0)
			self.questionDialog.dstItem=(0, 0)

		def __UseItemAuraQuestionDialog_Close(self):
			self.questionDialog.Close()

			self.questionDialog.srcItem=(0, 0)
			self.questionDialog.dstItem=(0, 0)

		def __UseItemAura(self, slotIndex):
			AuraSlot=player.FineMoveAuraItemSlot()
			UsingAuraSlot=player.FindActivatedAuraSlot(player.INVENTORY, slotIndex)
			AuraVnum=player.GetItemIndex(slotIndex)
			item.SelectItem(AuraVnum)
			
			if player.GetAuraCurrentItemSlotCount() >= player.AURA_SLOT_MAX <= UsingAuraSlot:
				return
			
			if player.IsEquipmentSlot(slotIndex):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_EQUIPITEM)
				return
			
			if player.GetAuraRefineWindowType()==player.AURA_WINDOW_TYPE_ABSORB:
				isAbsorbItem=False
				if item.GetItemType()==item.ITEM_TYPE_COSTUME:
					if item.GetItemSubType()==item.COSTUME_TYPE_AURA:
						if player.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM)==0:
							if UsingAuraSlot==player.AURA_SLOT_MAX:
								if AuraSlot != player.AURA_SLOT_MAIN:
									return
								
								net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_ABSORBITEM)
							return
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_ABSORBITEM)
						return
				elif item.GetItemType()==item.ITEM_TYPE_ARMOR:
					if item.GetItemSubType() in [item.ARMOR_SHIELD, item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR]:
						if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN)==player.NPOS():
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return
						
						isAbsorbItem=True
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_ABSORBITEM)
						return
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_ABSORBITEM)
					return
				
				if isAbsorbItem:
					if UsingAuraSlot==player.AURA_SLOT_MAX:
						if AuraSlot != player.AURA_SLOT_SUB:
							if player.FindUsingAuraSlot(player.AURA_SLOT_SUB)==player.NPOS():
								AuraSlot=player.AURA_SLOT_SUB
							else:
								return
						
						self.questionDialog=uicommon.QuestionDialog()
						self.questionDialog.SetText(localeinfo.AURA_NOTICE_DEL_ABSORDITEM)
						self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemAuraQuestionDialog_OnAccept))
						self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemAuraQuestionDialog_Close))
						self.questionDialog.srcItem=(player.INVENTORY, slotIndex)
						self.questionDialog.dstItem=(player.AURA_REFINE, AuraSlot)
						self.questionDialog.Open()
			elif player.GetAuraRefineWindowType()==player.AURA_WINDOW_TYPE_GROWTH:
				if UsingAuraSlot==player.AURA_SLOT_MAX:
					if AuraSlot==player.AURA_SLOT_MAIN:
						if item.GetItemType()==item.ITEM_TYPE_COSTUME:
							if item.GetItemSubType()==item.COSTUME_TYPE_AURA:
								socketLevelValue=player.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
								curLevel=(socketLevelValue / 100000) - 1000
								curExp=socketLevelValue%100000;
								if curLevel >= player.AURA_MAX_LEVEL:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_MAX_LEVEL)
									return
								
								if curExp >= player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_GROWTHITEM)
									return
								
								net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_AURAITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_AURAITEM)
							return
					elif AuraSlot==player.AURA_SLOT_SUB:
						if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN) != player.NPOS():
							if item.GetItemType()==item.ITEM_TYPE_RESOURCE:
								if item.GetItemSubType()==item.RESOURCE_AURA:
									if UsingAuraSlot==player.AURA_SLOT_MAX:
										if AuraSlot != player.AURA_SLOT_SUB:
											return
										
										net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
								else:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_AURARESOURCE)
									return
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_AURARESOURCE)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return
			elif player.GetAuraRefineWindowType()==player.AURA_WINDOW_TYPE_EVOLVE:
				if UsingAuraSlot==player.AURA_SLOT_MAX:
					if AuraSlot==player.AURA_SLOT_MAIN:
						if item.GetItemType()==item.ITEM_TYPE_COSTUME:
							if item.GetItemSubType()==item.COSTUME_TYPE_AURA:
								socketLevelValue=player.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
								curLevel=(socketLevelValue / 100000) - 1000
								curExp=socketLevelValue%100000;
								if curLevel >= player.AURA_MAX_LEVEL:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_MAX_LEVEL)
									return
								
								if curLevel != player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_LEVEL_MAX) or curExp < player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
									return
								
								if player.FindUsingAuraSlot(AuraSlot) != player.NPOS():
									return
								
								net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_AURAITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_AURAITEM)
							return
					elif AuraSlot==player.AURA_SLOT_SUB:
						Cell=player.FindUsingAuraSlot(player.AURA_SLOT_MAIN)
						if Cell==player.NPOS():
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return
						
						socketLevelValue=player.GetItemMetinSocket(*(Cell + (player.ITEM_SOCKET_AURA_CURRENT_LEVEL,)))
						curLevel=(socketLevelValue / 100000) - 1000
						curExp=socketLevelValue%100000;
						if curLevel >= player.AURA_MAX_LEVEL:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_MAX_LEVEL)
							return
						
						if curExp < player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
							return
						
						if AuraVnum != player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_MATERIAL_VNUM):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
							return
						
						if player.GetItemCount(slotIndex) < player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_MATERIAL_COUNT):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_EVOLUTION_ITEMCOUNT)
							return
						
						if UsingAuraSlot==player.AURA_SLOT_MAX:
							if AuraSlot != player.AURA_SLOT_MAX:
								if player.FindUsingAuraSlot(AuraSlot) != player.NPOS():
									return
							
							net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())

	def __DropSrcItemToDestItemInInventory(self, srcItemVID, srcItemSlotPos, dstItemSlotPos, destWindow=0):
		if app.ENABLE_AURA_SYSTEM and player.IsAuraRefineWindowOpen():
			return
		
		if app.ENABLE_CHANGELOOK and player.GetChangeLookWindowOpen()==1:
			return
		
		if app.ENABLE_EXTRA_INVENTORY:
			if srcItemSlotPos==dstItemSlotPos and not item.IsMetin(srcItemVID):
				return
		else:
			if srcItemSlotPos==dstItemSlotPos:
				return
		
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop() and (uiofflineshop.IsSaleSlot(player.INVENTORY, srcItemSlotPos) or uiofflineshop.IsSaleSlot(player.INVENTORY , dstItemSlotPos)):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return
		
		if app.NEW_PET_SYSTEM:
			if srcItemVID >= 55701 and srcItemVID <= 55711 and player.GetItemIndex(dstItemSlotPos)==55002:
				self.questionDialog=uicommon.QuestionDialog()
				self.questionDialog.SetText(localeinfo.PETSPECIAL_QUESTION1)
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.UseTransportBox))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				self.questionDialog.Open()
				self.questionDialog.src=srcItemSlotPos
				self.questionDialog.dst=dstItemSlotPos
				
			if srcItemVID==55001 and player.GetItemIndex(dstItemSlotPos) >= 55701 and player.GetItemIndex(dstItemSlotPos) <= 55711:
				self.questionDialog=uicommon.QuestionDialog()
				self.questionDialog.SetText(localeinfo.PETSPECIAL_QUESTION2)
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.UseProtein))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				self.questionDialog.Open()
				self.questionDialog.src=srcItemSlotPos
				self.questionDialog.dst=dstItemSlotPos
		
		if app.ENABLE_SOULBIND_SYSTEM and item.IsSealScroll(srcItemVID):
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
		elif item.IsRefineScroll(srcItemVID):
			self.RefineItem(srcItemSlotPos, dstItemSlotPos)
			self.wndItem.SetUseMode(False)
		elif item.IsMetin(srcItemVID):
			self.AttachMetinToItem(srcItemSlotPos, dstItemSlotPos)
		elif item.IsDetachScroll(srcItemVID):
			self.DetachMetinFromItem(srcItemSlotPos, dstItemSlotPos)
		elif item.IsKey(srcItemVID):
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
		elif (player.GetItemFlags(srcItemSlotPos) & ITEM_FLAG_APPLICABLE)==ITEM_FLAG_APPLICABLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
		elif item.GetUseType(srcItemVID) in self.USE_TYPE_TUPLE:
			if destWindow==player.SLOT_TYPE_EXTRA_INVENTORY:
				self.__SendUseItemToItemPacketExtra(srcItemSlotPos, dstItemSlotPos)
			else:
				self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
		else:
			if app.USE_NEW_ENCHANT_ATTR and item.GetUseType(srcItemVID)=="USE_SPECIAL_ENCHANT":
				self.__SendSpecialEnchant(srcItemSlotPos, dstItemSlotPos)
				return
			elif app.USE_AURA_ATTR_REMOVER and item.GetUseType(srcItemVID)=="USE_AURA_ATTRREMOVER":
				self.__SendAuraRemover(srcItemSlotPos, dstItemSlotPos)
				return
			elif app.ENABLE_ATTR_COSTUMES and (item.GetUseType(srcItemVID)=="USE_CHANGE_ATTR_COSTUME" or item.GetUseType(srcItemVID)=="USE_ADD_ATTR_COSTUME1" or item.GetUseType(srcItemVID)=="USE_ADD_ATTR_COSTUME2"):
				if destWindow==player.SLOT_TYPE_EXTRA_INVENTORY:
					self.__SendUseItemToItemPacketExtra(srcItemSlotPos, dstItemSlotPos)
				else:
					self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
				return
			elif app.ENABLE_ATTR_COSTUMES and item.GetUseType(srcItemVID)=="USE_REMOVE_ATTR_COSTUME":
				if destWindow==player.SLOT_TYPE_EXTRA_INVENTORY:
					self.__SendRemoveAttrCostume(srcItemSlotPos, dstItemSlotPos)
				return
			elif app.ENABLE_NEW_PET_EDITS and item.GetUseType(srcItemVID)=="USE_PET_REVIVE" and self.__CanUseSrcItemToDstItem(srcItemVID, srcItemSlotPos, dstItemSlotPos)==True:
				if destWindow==player.SLOT_TYPE_EXTRA_INVENTORY:
					self.__SendUseItemToItemPacketExtra(srcItemSlotPos, dstItemSlotPos)
				else:
					self.__SendRevivePet(srcItemSlotPos, dstItemSlotPos)
				return
			elif app.ENABLE_NEW_PET_EDITS and item.GetUseType(srcItemVID)=="USE_PET_ENCHANT" and self.__CanUseSrcItemToDstItem(srcItemVID, srcItemSlotPos, dstItemSlotPos)==True:
				if destWindow==player.SLOT_TYPE_EXTRA_INVENTORY:
					self.__SendUseItemToItemPacketExtra(srcItemSlotPos, dstItemSlotPos)
				else:
					self.__SendEnchantPet(srcItemSlotPos, dstItemSlotPos)
				return
			elif app.ENABLE_REMOTE_ATTR_SASH_REMOVE and item.GetUseType(srcItemVID)=="USE_ATTR_SASH_REMOVE":
				if destWindow==player.SLOT_TYPE_EXTRA_INVENTORY:
					self.__SendUseItemToItemPacketExtra(srcItemSlotPos, dstItemSlotPos)
				else:
					self.__SendRemoveAttrSash(srcItemSlotPos, dstItemSlotPos)
				return
			elif app.ENABLE_STOLE_COSTUME and item.GetUseType(srcItemVID)=="USE_ENCHANT_STOLE":
				if destWindow==player.SLOT_TYPE_EXTRA_INVENTORY:
					self.__SendUseItemToItemPacketExtra(srcItemSlotPos, dstItemSlotPos)
				else:
					self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
				return
			#snd.PlaySound("sound/ui/drop.wav")

			if player.IsEquipmentSlot(dstItemSlotPos):
				if item.IsEquipmentVID(srcItemVID):
					self.__UseItem(srcItemSlotPos)
			else:
				if destWindow==player.SLOT_TYPE_EXTRA_INVENTORY:
					self.__SendUseItemToItemPacketExtra(srcItemSlotPos, dstItemSlotPos)
				else:
					self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)
				#net.SendItemMovePacket(srcItemSlotPos, dstItemSlotPos, 0)

	if app.ENABLE_REMOTE_ATTR_SASH_REMOVE:
		def __OnRemoveAttrSash(self):
			if self.questionDialog:
				self.__SendUseItemToItemPacket(self.questionDialog.src, self.questionDialog.dst)
				self.OnCloseQuestionDialog()

		def __SendRemoveAttrSash(self, srcItemSlotPos, dstItemSlotPos):
				self.questionDialog=uicommon.QuestionDialog()
				self.questionDialog.SetText(localeinfo.ALERT_SASH_REMOVE_ATTR)
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__OnRemoveAttrSash))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				self.questionDialog.Open()
				self.questionDialog.src=srcItemSlotPos
				self.questionDialog.dst=dstItemSlotPos

	if app.ENABLE_NEW_PET_EDITS:
		def __SendEnchantPet(self, srcItemSlotPos, dstItemSlotPos):
			self.OnCloseEnchantPet()
			
			import uipetenchant
			self.enchantPetDialog=uipetenchant.Main()
			self.enchantPetDialog.Open(self, srcItemSlotPos, dstItemSlotPos)

		def OnCloseEnchantPet(self):
			if self.enchantPetDialog:
				self.enchantPetDialog.CloseAsk()
				self.enchantPetDialog=0

		def __OnRevivePet(self):
			if self.questionDialog:
				self.__SendUseItemToItemPacket(self.questionDialog.src, self.questionDialog.dst)
				self.OnCloseQuestionDialog()

		def __SendRevivePet(self, srcItemSlotPos, dstItemSlotPos):
				self.questionDialog=uicommon.QuestionDialog()
				self.questionDialog.SetText(localeinfo.ALERT_REVIVE_PET)
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__OnRevivePet))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				self.questionDialog.Open()
				self.questionDialog.src=srcItemSlotPos
				self.questionDialog.dst=dstItemSlotPos

	if app.USE_NEW_ENCHANT_ATTR:
		def __OnSendSpecialEnchant(self):
			if self.questionDialog:
				self.__SendUseItemToItemPacket(self.questionDialog.src, self.questionDialog.dst)
				self.OnCloseQuestionDialog()

		def __SendSpecialEnchant(self, srcItemSlotPos, dstItemSlotPos):
				dstItemVNum=player.GetItemIndex(dstItemSlotPos)
				if dstItemVNum==0:
					return False
				
				item.SelectItem(dstItemVNum)
				if item.GetItemType() != item.ITEM_TYPE_WEAPON:
					return False
				
				subTypes=[\
							item.WEAPON_SWORD,
							item.WEAPON_DAGGER,
							item.WEAPON_BOW,
							item.WEAPON_TWO_HANDED,
							item.WEAPON_BELL,
							item.WEAPON_FAN,
				]
				
				if item.GetItemSubType() not in subTypes:
					return False
				
				bRet=1
				bHasPM=0
				bHasPA=0
				for i in xrange(player.ATTRIBUTE_SLOT_NORM_NUM):
					(t, v)=player.GetItemAttribute(dstItemSlotPos, i)
					if t == 0:
						bRet=0
						break
					elif t==item.APPLY_NORMAL_HIT_DAMAGE_BONUS:
						bHasPM=1
					elif t==item.APPLY_SKILL_DAMAGE_BONUS:
						bHasPA=1
				
				if bRet==0:
					chat.AppendChat(chat.CHAT_TYPE_INFO,localeinfo.ALERT_SPECIAL_ENCHANT_CANNOT2)
					return
				
				if bHasPM==0 or bHasPA==0:
					chat.AppendChat(chat.CHAT_TYPE_INFO,localeinfo.ALERT_SPECIAL_ENCHANT_CANNOT1)
					return
				
				itemVnum=player.GetItemIndex(srcItemSlotPos)
				if not itemVnum:
					return
				
				item.SelectItem(itemVnum)
				itemName1=item.GetItemName(itemVnum)
				
				itemVnum=player.GetItemIndex(dstItemSlotPos)
				if not itemVnum:
					return
				
				item.SelectItem(itemVnum)
				itemName2=item.GetItemName(itemVnum)
				
				self.questionDialog=uicommon.QuestionDialog()
				self.questionDialog.SetText(localeinfo.ALERT_SPECIAL_ENCHANT%(itemName1,itemName2))
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__OnSendSpecialEnchant))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				self.questionDialog.Open()
				self.questionDialog.src=srcItemSlotPos
				self.questionDialog.dst=dstItemSlotPos

	if app.USE_AURA_ATTR_REMOVER:
		def __OnSendAuraRemover(self):
			if self.questionDialog:
				self.__SendUseItemToItemPacket(self.questionDialog.src, self.questionDialog.dst)
				self.OnCloseQuestionDialog()

		def __SendAuraRemover(self, srcItemSlotPos, dstItemSlotPos):
				dstItemVNum=player.GetItemIndex(dstItemSlotPos)
				if dstItemVNum==0:
					return
				
				item.SelectItem(dstItemVNum)
				if item.GetItemType() != item.ITEM_TYPE_COSTUME:
					return
				
				if item.GetItemSubType()!=item.COSTUME_TYPE_AURA:
					return
				
				metinSlot=[player.GetItemMetinSocket(player.INVENTORY, dstItemSlotPos, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
				if not int(metinSlot[player.ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM]):
					return
				
				itemVnum=player.GetItemIndex(srcItemSlotPos)
				if not itemVnum:
					return
				
				item.SelectItem(itemVnum)
				itemName1=item.GetItemName(itemVnum)
				
				itemVnum=player.GetItemIndex(dstItemSlotPos)
				if not itemVnum:
					return
				
				item.SelectItem(itemVnum)
				itemName2=item.GetItemName(itemVnum)
				
				self.questionDialog=uicommon.QuestionDialog2()
				self.questionDialog.SetText1(localeinfo.ALERT_SPECIAL_AURA1)
				self.questionDialog.SetText2(localeinfo.ALERT_SPECIAL_AURA2%(itemName1,itemName2))
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__OnSendAuraRemover))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				self.questionDialog.AutoResize()
				self.questionDialog.Open()
				self.questionDialog.src=srcItemSlotPos
				self.questionDialog.dst=dstItemSlotPos

	if app.ENABLE_ATTR_COSTUMES:
		def __SendRemoveAttrCostume(self, srcItemSlotPos, dstItemSlotPos):
			dstItemVNum=player.GetItemIndex(dstItemSlotPos)
			if dstItemVNum==0:
				return
			
			item.SelectItem(dstItemVNum)
			if item.GetItemType() != item.ITEM_TYPE_COSTUME:
				return
			
			toContinue=False
			for i in xrange(player.ATTRIBUTE_SLOT_RARE_NUM):
				i += player.ATTRIBUTE_SLOT_NORM_NUM
				(t, v)=player.GetItemAttribute(dstItemSlotPos, i)
				if t != 0:
					toContinue=True
					break
			
			if toContinue:
				self.OnCloseRemoveAttrCostume()
				
				import uiattrdialog
				self.removeAttrDialog=uiattrdialog.Remove()
				self.removeAttrDialog.Open(self, srcItemSlotPos, dstItemSlotPos)

		def OnCloseRemoveAttrCostume(self):
			if self.removeAttrDialog:
				self.removeAttrDialog.CloseAsk()
				self.removeAttrDialog=0

	def __SellItem(self, itemSlotPos):
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return
		
		
		if not player.IsEquipmentSlot(itemSlotPos):
			self.sellingSlotNumber=itemSlotPos
			itemIndex=player.GetItemIndex(itemSlotPos)
			itemCount=player.GetItemCount(itemSlotPos)


			self.sellingSlotitemIndex=itemIndex
			self.sellingSlotitemCount=itemCount

			item.SelectItem(itemIndex)
			## 20140220
			if item.IsAntiFlag(item.ANTIFLAG_SELL):
				popup=uicommon.PopupDialog()
				popup.SetText(localeinfo.SHOP_CANNOT_SELL_ITEM)
				popup.SetAcceptEvent(self.__OnClosePopupDialog)
				popup.Open()
				self.popup=popup
				return

			itemPrice=item.GetISellItemPrice()

			if item.Is1GoldItem():
				itemPrice=itemCount / itemPrice / 5
			else:
				itemPrice=itemPrice * itemCount / 5

			item.GetItemName(itemIndex)
			itemName=item.GetItemName()

			self.questionDialog=uicommon.QuestionDialog()
			self.questionDialog.SetText(localeinfo.DO_YOU_SELL_ITEM(itemName, itemCount, itemPrice))
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.SellItem))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			self.questionDialog.Open()
			self.questionDialog.count=itemCount

			constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def __OnClosePopupDialog(self):
		self.pop=0

	def RefineItem(self, scrollSlotPos, targetSlotPos):
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return
		
		scrollIndex=player.GetItemIndex(scrollSlotPos)
		targetIndex=player.GetItemIndex(targetSlotPos)
		
		if player.REFINE_OK != player.CanRefine(scrollIndex, targetSlotPos):
			return
		
		if app.USE_AUTO_REFINE:
			constinfo.AUTO_REFINE_TYPE=1
			constinfo.AUTO_REFINE_DATA["ITEM"][0]=scrollSlotPos
			constinfo.AUTO_REFINE_DATA["ITEM"][1]=targetSlotPos
		
		self.__SendUseItemToItemPacket(scrollSlotPos, targetSlotPos)

	def DetachMetinFromItem(self, scrollSlotPos, targetSlotPos):
		scrollIndex=player.GetItemIndex(scrollSlotPos)
		targetIndex=player.GetItemIndex(targetSlotPos)
		
		if not player.CanDetach(scrollIndex, targetSlotPos):
			if not scrollIndex:
				return
			
			if app.ENABLE_CHANGELOOK and changelook.IsCleanScroll(scrollIndex):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CHANGE_LOOK_FAILURE_CLEAN)
				return
			
			item.SelectItem(scrollIndex)
			
			if app.ENABLE_ACCE_SYSTEM and item.GetValue(0)==acce.CLEAN_ATTR_VALUE0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.ACCE_FAILURE_CLEAN)
				return
			
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_METIN_INSEPARABLE_ITEM)
			return
		
		self.questionDialog=uicommon.QuestionDialog()
		self.questionDialog.SetText(localeinfo.REFINE_DO_YOU_SEPARATE_METIN)
		
		if app.ENABLE_ACCE_SYSTEM:
			item.SelectItem(targetIndex)
			if item.GetItemType()==item.ITEM_TYPE_COSTUME and item.GetItemSubType()==item.COSTUME_TYPE_ACCE:
				item.SelectItem(scrollIndex)
				if item.GetValue(0)==acce.CLEAN_ATTR_VALUE0:
					self.questionDialog.SetText(localeinfo.ACCE_DO_YOU_CLEAN)
		
		if app.ENABLE_CHANGELOOK:
			item.SelectItem(targetIndex)
			if item.GetItemType()==item.ITEM_TYPE_WEAPON or item.GetItemType()==item.ITEM_TYPE_ARMOR or item.GetItemType()==item.ITEM_TYPE_COSTUME:
				item.SelectItem(scrollIndex)
				if changelook.IsCleanScroll(scrollIndex):
					self.questionDialog.SetText(localeinfo.CHANGE_LOOK_DO_YOU_CLEAN)
		
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnDetachMetinFromItem))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		self.questionDialog.Open()
		self.questionDialog.sourcePos=scrollSlotPos
		self.questionDialog.targetPos=targetSlotPos

	def AttachMetinToItem(self, metinSlotPos, targetSlotPos):
		if app.ENABLE_EXTRA_INVENTORY:
			metinIndex=player.GetItemIndex(player.EXTRA_INVENTORY, metinSlotPos)
		else:
			metinIndex=player.GetItemIndex(metinSlotPos)
		targetIndex=player.GetItemIndex(targetSlotPos)

		item.SelectItem(metinIndex)
		itemName=item.GetItemName()

		result=player.CanAttachMetin(metinIndex, targetSlotPos)

		if player.ATTACH_METIN_NOT_MATCHABLE_ITEM==result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_CAN_NOT_ATTACH(itemName))

		if player.ATTACH_METIN_NO_MATCHABLE_SOCKET==result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_NO_SOCKET(itemName))

		elif player.ATTACH_METIN_NOT_EXIST_GOLD_SOCKET==result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_NO_GOLD_SOCKET(itemName))

		elif player.ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT==result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.ATTACH_METIN_OK != result:
			return

		self.attachMetinDialog.Open(metinSlotPos, targetSlotPos)

	def __IsUsableItemToItem(self, srcItemVNum, srcSlotPos):
		if app.NEW_PET_SYSTEM:
			if srcItemVNum >= 55701 and srcItemVNum <= 55711:
				return True
			if srcItemVNum==55001:
				return True

		if item.IsRefineScroll(srcItemVNum):
			return True
		elif item.IsMetin(srcItemVNum):
			return True
		elif item.IsDetachScroll(srcItemVNum):
			return True
		elif item.IsKey(srcItemVNum):
			return True
		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE)==ITEM_FLAG_APPLICABLE:
			return True
		else:
			if item.GetUseType(srcItemVNum) in self.USE_TYPE_TUPLE:
				return True
			
			if app.USE_NEW_ENCHANT_ATTR and item.GetUseType(srcItemVNum)=="USE_SPECIAL_ENCHANT":
				return True
			
			if app.USE_AURA_ATTR_REMOVER and item.GetUseType(srcItemVNum)=="USE_AURA_ATTRREMOVER":
				return True
			
			if app.ENABLE_ATTR_COSTUMES and (item.GetUseType(srcItemVNum)=="USE_CHANGE_ATTR_COSTUME" or item.GetUseType(srcItemVNum)=="USE_ADD_ATTR_COSTUME1" or item.GetUseType(srcItemVNum)=="USE_ADD_ATTR_COSTUME2" or item.GetUseType(srcItemVNum)=="USE_REMOVE_ATTR_COSTUME"):
				return True
			
			if app.ENABLE_NEW_PET_EDITS and (item.GetUseType(srcItemVNum)=="USE_PET_REVIVE" or item.GetUseType(srcItemVNum)=="USE_PET_ENCHANT"):
				return True
			
			if app.ENABLE_REMOTE_ATTR_SASH_REMOVE and item.GetUseType(srcItemVNum)=="USE_ATTR_SASH_REMOVE":
				return True
			
			if app.ENABLE_STOLE_COSTUME and item.GetUseType(srcItemVNum)=="USE_ENCHANT_STOLE":
				return True
		
		return False

	def __CanUseSrcItemToDstItem(self, srcItemVNum, srcSlotPos, dstSlotPos):
		if app.NEW_PET_SYSTEM:
			if srcItemVNum >= 55701 and  srcItemVNum <= 55711 and player.GetItemIndex(dstSlotPos)==55002:
				return True		
			if srcItemVNum==55001 and player.GetItemIndex(dstSlotPos) >= 55701 and player.GetItemIndex(dstSlotPos) <= 55711:
				return True

		#if app.ENABLE_EXTRA_INVENTORY:
		#	if srcSlotPos==dstSlotPos:
		#		return False
		#	elif item.IsMetin(srcItemVNum):
		#		if player.ATTACH_METIN_OK==player.CanAttachMetin(srcItemVNum, dstSlotPos):
		#			itemVnumDest=player.GetItemIndex(dstSlotPos)
		#			return True
		#else:
		if srcSlotPos==dstSlotPos:
			return False
		
		if item.IsRefineScroll(srcItemVNum):
			if player.REFINE_OK==player.CanRefine(srcItemVNum, dstSlotPos):
				return True
		elif item.IsMetin(srcItemVNum):
			if player.ATTACH_METIN_OK==player.CanAttachMetin(srcItemVNum, dstSlotPos):
				return True
		elif item.IsDetachScroll(srcItemVNum):
			if player.DETACH_METIN_OK==player.CanDetach(srcItemVNum, dstSlotPos):
				return True
		elif item.IsKey(srcItemVNum):
			if player.CanUnlock(srcItemVNum, dstSlotPos):
				return True

		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE)==ITEM_FLAG_APPLICABLE:
			return True

		else:
			useType=item.GetUseType(srcItemVNum)
			if "USE_CLEAN_SOCKET"==useType:
				if self.__CanCleanBrokenMetinStone(dstSlotPos):
					return True
			elif "USE_CHANGE_ATTRIBUTE"==useType:
				if self.__CanChangeItemAttrList(dstSlotPos):
					return True
			elif "USE_ADD_ATTRIBUTE"==useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return True
			elif "USE_ADD_ATTRIBUTE2"==useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return True
			elif "USE_ADD_ACCESSORY_SOCKET"==useType:
				if self.__CanAddAccessorySocket(dstSlotPos):
					return True
				else:
					dstItemVNum=player.GetItemIndex(dstSlotPos)
					item.SelectItem(dstItemVNum)
					if item.ITEM_TYPE_BELT==item.GetItemType():
						return True
			elif "USE_PUT_INTO_ACCESSORY_SOCKET"==useType:
				if self.__CanPutAccessorySocket(dstSlotPos, srcItemVNum):
					return True
			elif "USE_PUT_INTO_BELT_SOCKET"==useType:
				dstItemVNum=player.GetItemIndex(dstSlotPos)
				item.SelectItem(dstItemVNum)
				if item.ITEM_TYPE_BELT==item.GetItemType():
					return True
			elif "USE_CHANGE_COSTUME_ATTR"==useType:
				if self.__CanChangeCostumeAttrList(dstSlotPos):
					return True
			elif "USE_RESET_COSTUME_ATTR"==useType:
				if self.__CanResetCostumeAttr(dstSlotPos):
					return True
			elif "USE_REMOVE_ACCESSORY_SOCKETS"==useType:
				if self.__CanRemoveAccessorySockets(dstSlotPos, srcItemVNum):
					return True
			elif app.USE_NEW_ENCHANT_ATTR and item.GetUseType(srcItemVNum)=="USE_SPECIAL_ENCHANT":
				dstItemVNum=player.GetItemIndex(dstSlotPos)
				if dstItemVNum==0:
					return False
				
				item.SelectItem(dstItemVNum)
				if item.GetItemType() != item.ITEM_TYPE_WEAPON:
					return False
				
				subTypes=[\
							item.WEAPON_SWORD,
							item.WEAPON_DAGGER,
							item.WEAPON_BOW,
							item.WEAPON_TWO_HANDED,
							item.WEAPON_BELL,
							item.WEAPON_FAN,
				]
				
				if item.GetItemSubType() not in subTypes:
					return False
				
				bRet=1
				bHasPM=0
				bHasPA=0
				for i in xrange(player.ATTRIBUTE_SLOT_NORM_NUM):
					(t, v)=player.GetItemAttribute(dstSlotPos, i)
					if t == 0:
						bRet=0
						break
					elif t==item.APPLY_NORMAL_HIT_DAMAGE_BONUS:
						bHasPM=1
					elif t==item.APPLY_SKILL_DAMAGE_BONUS:
						bHasPA=1
				
				if bRet==0:
					return False
				
				if bHasPM==0 or bHasPA==0:
					return False
				
				return True
			elif app.USE_AURA_ATTR_REMOVER and item.GetUseType(srcItemVNum)=="USE_AURA_ATTRREMOVER":
				if not app.ENABLE_AURA_SYSTEM:
					return False
				
				dstItemVNum=player.GetItemIndex(dstSlotPos)
				if dstItemVNum==0:
					return False
				
				item.SelectItem(dstItemVNum)
				if item.GetItemType() != item.ITEM_TYPE_COSTUME:
					return False
				
				if item.GetItemSubType()!=item.COSTUME_TYPE_AURA:
					return False
				
				metinSlot=[player.GetItemMetinSocket(player.INVENTORY, dstSlotPos, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
				if not int(metinSlot[player.ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM]):
					return False
				
				return True
			elif app.ENABLE_ATTR_COSTUMES and item.GetUseType(srcItemVNum)=="USE_CHANGE_ATTR_COSTUME":
				dstItemVNum=player.GetItemIndex(dstSlotPos)
				if dstItemVNum==0:
					return False
				
				item.SelectItem(dstItemVNum)
				if item.GetItemType() != item.ITEM_TYPE_COSTUME:
					return False
				
				for i in xrange(player.ATTRIBUTE_SLOT_NORM_NUM):
					(t, v)=player.GetItemAttribute(dstSlotPos, i)
					if t != 0:
						return True
				
				return False
			elif app.ENABLE_ATTR_COSTUMES and (item.GetUseType(srcItemVNum)=="USE_ADD_ATTR_COSTUME1" or item.GetUseType(srcItemVNum)=="USE_ADD_ATTR_COSTUME2"):
				dstItemVNum=player.GetItemIndex(dstSlotPos)
				if dstItemVNum==0:
					return False
				
				item.SelectItem(dstItemVNum)
				if item.GetItemType() != item.ITEM_TYPE_COSTUME:
					return False
				
				for i in xrange(player.ATTRIBUTE_SLOT_RARE_NUM):
					i += player.ATTRIBUTE_SLOT_NORM_NUM
					(t, v)=player.GetItemAttribute(dstSlotPos, i)
					if t==0:
						return True
				
				return False
			elif app.ENABLE_ATTR_COSTUMES and item.GetUseType(srcItemVNum)=="USE_REMOVE_ATTR_COSTUME":
				dstItemVNum=player.GetItemIndex(dstSlotPos)
				if dstItemVNum==0:
					return False
				
				item.SelectItem(dstItemVNum)
				if item.GetItemType() != item.ITEM_TYPE_COSTUME:
					return False
				
				for i in xrange(player.ATTRIBUTE_SLOT_RARE_NUM):
					i += player.ATTRIBUTE_SLOT_NORM_NUM
					(t, v)=player.GetItemAttribute(dstSlotPos, i)
					if t != 0:
						return True
				
				return False
			elif app.ENABLE_NEW_PET_EDITS and (item.GetUseType(srcItemVNum)=="USE_PET_REVIVE" or item.GetUseType(srcItemVNum)=="USE_PET_ENCHANT"):
				dstItemVNum=player.GetItemIndex(dstSlotPos)
				if dstItemVNum==0:
					return False
				
				item.SelectItem(dstItemVNum)
				if constinfo.IS_PET_SEAL(dstItemVNum):
					return True
				
				return False
			elif app.ENABLE_REMOTE_ATTR_SASH_REMOVE and item.GetUseType(srcItemVNum)=="USE_ATTR_SASH_REMOVE":
				dstItemVNum=player.GetItemIndex(dstSlotPos)
				if dstItemVNum==0:
					return False
				
				item.SelectItem(dstItemVNum)
				if item.GetItemType() != item.ITEM_TYPE_COSTUME:
					return False
				
				if item.GetItemSubType() != item.COSTUME_TYPE_ACCE:
					return False
				
				metinSlot=[player.GetItemMetinSocket(player.INVENTORY, dstSlotPos, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
				itemAbsorbedVnum=int(metinSlot[acce.ABSORBED_SOCKET])
				if itemAbsorbedVnum:
					return True
				
				return False
			elif app.ENABLE_STOLE_COSTUME and item.GetUseType(srcItemVNum)=="USE_ENCHANT_STOLE":
				dstItemVNum=player.GetItemIndex(dstSlotPos)
				if dstItemVNum==0:
					return False
				
				item.SelectItem(dstItemVNum)
				if item.GetItemType() != item.ITEM_TYPE_COSTUME:
					return False
				
				if item.GetItemSubType() != item.COSTUME_TYPE_STOLE:
					return False
				
				grade=item.GetValue(0)
				if grade < 1 or grade > 4:
					return False
				
				maxAttr=player.ATTRIBUTE_SLOT_NORM_NUM + player.ATTRIBUTE_SLOT_RARE_NUM
				for i in xrange(maxAttr):
					(t, v)=player.GetItemAttribute(dstSlotPos, i)
					if t != 0:
						return True
				
				return False
			elif app.USE_ATTR_6TH_7TH and "USE_CHANGE_ATTRIBUTE2"==useType:
				dstItemVNum = player.GetItemIndex(player.INVENTORY,dstSlotPos)
				if dstItemVNum == 0:
					return False
				
				item.SelectItem(dstItemVNum)
				if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
					return False
				
				attrCount = 0
				for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
						attrCount += 1
				
				if attrCount > player.ATTRIBUTE_SLOT_MAX_NUM - 2 and attrCount <= player.ATTRIBUTE_SLOT_MAX_NUM:
					return True
				
				return False
		
		return False

	def __CanCleanBrokenMetinStone(self, dstSlotPos):
		dstItemVNum=player.GetItemIndex(dstSlotPos)
		if dstItemVNum==0:
			return False

		item.SelectItem(dstItemVNum)

		if item.ITEM_TYPE_WEAPON != item.GetItemType():
			return False

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemMetinSocket(dstSlotPos, i)==constinfo.ERROR_METIN_STONE:
				return True

		return False

	def __CanChangeItemAttrList(self, dstSlotPos):
		dstItemVNum=player.GetItemIndex(dstSlotPos)
		if dstItemVNum==0:
			return False

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
			return False

		for i in xrange(player.ATTRIBUTE_SLOT_NORM_NUM):
			(t, v) = player.GetItemAttribute(dstSlotPos, i)
			if t != 0:
				return True

		return False

	def __CanChangeCostumeAttrList(self, dstSlotPos):
		dstItemVNum=player.GetItemIndex(dstSlotPos)
		if dstItemVNum==0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_COSTUME:
			return False

		for i in xrange(player.ATTRIBUTE_SLOT_NORM_NUM):
			(t, v) = player.GetItemAttribute(dstSlotPos, i)
			if t != 0:
				return True

		return False

	def __CanResetCostumeAttr(self, dstSlotPos):
		dstItemVNum=player.GetItemIndex(dstSlotPos)
		if dstItemVNum==0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_COSTUME:
			return False

		for i in xrange(player.ATTRIBUTE_SLOT_NORM_NUM):
			(t, v) = player.GetItemAttribute(dstSlotPos, i)
			if t != 0:
				return True

		return False

	def __CanPutAccessorySocket(self, dstSlotPos, mtrlVnum):
		dstItemVNum=player.GetItemIndex(dstSlotPos)
		if dstItemVNum==0:
			return False
		
		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return False

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return False

		curCount=player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount=player.GetItemMetinSocket(dstSlotPos, 1)
		if mtrlVnum != constinfo.GET_ACCESSORY_MATERIAL_VNUM(dstItemVNum, item.GetItemSubType()):
			if app.ENABLE_INFINITE_RAFINES:
				if mtrlVnum != constinfo.GET_ACCESSORY_MATERIAL_VNUM2(dstItemVNum, item.GetItemSubType()):
					return False
			else:
				return False

		if curCount>=maxCount:
			return False

		return True

	def __CanRemoveAccessorySockets(self, dstSlotPos, mtrlVnum):
		dstItemVNum=player.GetItemIndex(dstSlotPos)
		if dstItemVNum==0:
			return False
		
		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return False

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return False

		return player.GetItemMetinSocket(dstSlotPos, 0) > 0

	def __CanAddAccessorySocket(self, dstSlotPos):
		dstItemVNum=player.GetItemIndex(dstSlotPos)
		if dstItemVNum==0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return False

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return False

		curCount=player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount=player.GetItemMetinSocket(dstSlotPos, 1)

		ACCESSORY_SOCKET_MAX_SIZE=3
		if maxCount >= ACCESSORY_SOCKET_MAX_SIZE:
			return False

		return True

	def __CanAddItemAttr(self, dstSlotPos):
		dstItemVNum=player.GetItemIndex(dstSlotPos)
		if dstItemVNum==0:
			return False

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
			return False

		attrCount=0
		for i in xrange(player.ATTRIBUTE_SLOT_NORM_NUM):
			(t, v) = player.GetItemAttribute(dstSlotPos, i)
			if t != 0:
				attrCount += 1

		if attrCount < 5:
			return True

		return False

	def ShowToolTip(self, slotIndex):
		if self.tooltipItem:
			itemVnum = player.GetItemIndex(slotIndex)
			if not itemVnum:
				return

			self.tooltipItem.SetInventoryItem(slotIndex)

			if app.__ENABLE_NEW_OFFLINESHOP__ and not player.IsEquipmentSlot(slotIndex):
				if uiofflineshop.IsBuildingShop() or uiofflineshop.IsBuildingAuction():
					item.SelectItem(itemVnum)
					if not (item.IsAntiFlag(item.ANTIFLAG_MYSHOP) or item.IsAntiFlag(item.ANTIFLAG_GIVE)):
						self.tooltipItem.AddRightClickForSale()
						return

			if app.USE_MULTIPLE_OPENING:
				if item.CanBeMultipleOpened(itemVnum):
					self.tooltipItem.AppendSpace(5)
					self.tooltipItem.AppendTextLine(localeinfo.TEXTLINE_MULTIPLE_CHEST_OPEN)

	def OnTop(self):
		if self.tooltipItem:
			self.tooltipItem.SetTop()
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			map(lambda wnd:wnd.RefreshLockedSlot(), self.bindWnds)
			self.RefreshMarkSlots()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	if app.ENABLE_NEW_PET_EDITS:
		def __UsePetBoxDialog_OnAccept(self):
			self.__UseItem(self.questionDialog.slotIndex)
			self.__UseItemQuestionDialog_OnCancel()

	def __UseItem(self, slotIndex):
		if app.ENABLE_CHANGELOOK:
			if player.GetChangeLookWindowOpen()==1:
				return
		
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(player.INVENTORY, slotIndex):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return
		
		if app.ENABLE_AURA_SYSTEM:
			if player.IsAuraRefineWindowOpen():
				self.__UseItemAura(slotIndex)
				return
		
		ItemVNum=player.GetItemIndex(slotIndex)
		item.SelectItem(ItemVNum)
		
		if item.IsFlag(item.ITEM_FLAG_CONFIRM_WHEN_USE):
			self.questionDialog=uicommon.QuestionDialog()
			self.questionDialog.SetText(localeinfo.INVENTORY_REALLY_USE_ITEM)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
			self.questionDialog.Open()
			self.questionDialog.slotIndex=slotIndex

			constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

		else:
			self.__SendUseItemPacket(slotIndex)
			#net.SendItemUsePacket(slotIndex)

	def __UseItemQuestionDialog_OnCancel(self):
		self.OnCloseQuestionDialog()

	def __UseItemQuestionDialog_OnAccept(self):
		self.__SendUseItemPacket(self.questionDialog.slotIndex)
		self.OnCloseQuestionDialog()

	def __SendUseItemToItemPacket(self, srcSlotPos, dstSlotPos):
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return
		
		net.SendItemUseToItemPacket(srcSlotPos, dstSlotPos)

	def __SendUseItemPacket(self, slotPos):
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemUsePacket(slotPos)

	def __SendMoveItemPacket(self, srcSlotPos, dstSlotPos, srcItemCount):
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.MOVE_ITEM_FAILURE_PRIVATE_SHOP)
			return
		
		net.SendItemMovePacket(srcSlotPos, dstSlotPos, srcItemCount)

	def __SendUseItemToItemPacketExtra(self, srcSlotPos, dstSlotPos):
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return
		
		net.SendItemUseToItemPacket(player.EXTRA_INVENTORY, srcSlotPos, player.INVENTORY, dstSlotPos)

	def SetDragonSoulRefineWindow(self, wndDragonSoulRefine):
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoulRefine=wndDragonSoulRefine
			
	if app.ENABLE_ACCE_SYSTEM:
		def SetAcceWindow(self, wndAcceCombine, wndAcceAbsorption):
			self.wndAcceCombine=wndAcceCombine
			self.wndAcceAbsorption=wndAcceAbsorption

		def isShowAcceWindow(self):
			if self.wndAcceCombine:
				if self.wndAcceCombine.IsShow():
					return 1

			if self.wndAcceAbsorption:
				if self.wndAcceAbsorption.IsShow():
					return 1
			
			return 0

	if app.ITEM_CHECKINOUT_UPDATE:
		def SetSafeboxWindow(self, wndSafeBox):
			self.wndSafeBox=wndSafeBox

	if app.ENABLE_CHANGELOOK:
		def isShowChangeLookWindow(self):
			try:
				if newconstinfo._interface_instance.wndChangeLook.IsShow():
					return 1
				
				return 0
			except Exception as e:
				import dbg
				dbg.TraceError(str(e))
			
			return 0

	def OnPressDSButton(self):
		newconstinfo._interface_instance.ToggleDragonSoulWindowWithNoInfo()

	if app.ENABLE_EXTRA_INVENTORY:
		def OnPressExtraInvButton(self):
			newconstinfo._interface_instance.ToggleExtraInventoryWindow()

	if app.__ENABLE_NEW_OFFLINESHOP__:
		def OnPressOfflineShopButton(self):
			newconstinfo._interface_instance.OnPressBtnOfflineshop()

	def OnPressCostumeButton(self):
		if not self.wndCostume:
			self.wndCostume=CostumeWindow(self)
			self.wndCostume.Open()
			self.wndCostume.SetTop()
		else:
			if self.wndCostume.IsShow():
				self.wndCostume.CloseByTitle()
			else:
				self.wndCostume.Open()

	# def OnMoveWindow(self, x, y):
		# if self.wndmenu != None:
			# self.wndmenu.AdjustPosition()

	def OnMoveWindow(self, x, y):
		if self.wndmenu:
			self.wndmenu.AdjustPosition()
			self.wndmenu.SetTop()
			self.wndmenu.SetFocus()

	def OpenSwitchbot(self):
		net.SBOpen()

	def OnPressBtnBiolog(self):
		newconstinfo._interface_instance.ClickBiologistButton()

	def OnPressBtnDungeoninfo(self):
		newconstinfo._interface_instance.ToggleDungeonInfoWindow()

	def OnPressOfflineShopSearch(self):
		if app.ENABLE_RENEWAL_OX:
			import background
			if background.GetCurrentMapName() == "metin2_map_oxevent":
				import chr
				
				if not chr.IsGameMaster(player.GetMainCharacterIndex()):
					import chat
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CANNOT_USE_IN_THIS_MAP)
					return
		
		if newconstinfo._interface_instance.wndShopOffline:
			newconstinfo._interface_instance.wndShopOffline.OpenSearch()

	def OpenCalendarWindow(self):
		newconstinfo._interface_instance.OpenEventCalendar()

	def OpenWikiWindow(self):
		newconstinfo._interface_instance.ToggleWikiNew()

	def AntiExpBtn(self):
		net.SendChatPacket("/manage_exp")

	if app.USE_AUTO_AGGREGATE:
		def SetBraveCapeStatus(self, status):
			if self.wndEquipment:
				self.wndEquipment.SetBraveCapeStatus(status)
