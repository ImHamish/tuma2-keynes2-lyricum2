# coding: latin_1

import app
import player
import net
import chat
import wndMgr

import ui
import uicommon
import _weakref
import localeinfo

if app.USE_AUTO_HUNT:
	import constinfo

IMG_FOLDER = "d:/ymir work/ui/game/loot_filter/"

class FilterWindow(ui.BoardWithTitleBar):
	tooltipItem = None
	childrenDict = {}

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)

		import newconstinfo
		self.tooltipItem = newconstinfo.GetInterfaceInstance().tooltipItem if newconstinfo.GetInterfaceInstance() else None

		self.__LoadWindow()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def Destroy(self):
		self.interfaceInst = None

		if self.childrenDict.has_key("qstdlg"):
			self.childrenDict["qstdlg"].Close()
			del self.childrenDict["qstdlg"]

		self.childrenDict = {}

	def __LoadWindow(self):
		self.Destroy()

		self.SetCloseEvent(lambda arg="EXIT":self.__ClickButton(arg))
		self.SetTitleName(localeinfo.LOOTING_SYSTEM_TITLE)
		self.AddFlag("movable")
		self.AddFlag("float")

		bgReal = CreateWindow(ui.ExpandedImageBox(), self, (12, 33), IMG_FOLDER+"bg.tga")
		self.childrenDict["bgReal"] = bgReal

		bg = CreateWindow(ui.Window(), bgReal, (2, 2), "","",(bgReal.GetWidth() - 4, bgReal.GetHeight() - 7))
		bg.OnMouseWheel = ui.__mem_func__(self.OnRunMouseWheel)
		bg.SetInsideRender(True)
		self.childrenDict["bg"] = bg

		self.SetSize(12 + bgReal.GetWidth() + 12, bgReal.GetHeight() + 69)
		self.SetCenterPosition()

		resetBtn = CreateWindow(ui.Button(), self, (12, 33+bgReal.GetHeight()+7))
		resetBtn.SetUpVisual(IMG_FOLDER+"big_btn_0.tga")
		resetBtn.SetOverVisual(IMG_FOLDER+"big_btn_1.tga")
		resetBtn.SetDownVisual(IMG_FOLDER+"big_btn_2.tga")
		resetBtn.SetText(localeinfo.LOOTING_SYSTEM_INIT)
		resetBtn.SAFE_SetEvent(self.__ClickButton, "RESET")
		self.childrenDict["resetBtn"] = resetBtn

		saveBtn = CreateWindow(ui.Button(), self, (12 + bgReal.GetWidth() - resetBtn.GetWidth(), 33+bgReal.GetHeight()+7))
		saveBtn.SetUpVisual(IMG_FOLDER+"big_btn_0.tga")
		saveBtn.SetOverVisual(IMG_FOLDER+"big_btn_1.tga")
		saveBtn.SetDownVisual(IMG_FOLDER+"big_btn_2.tga")
		saveBtn.SetText(localeinfo.LOOTING_SYSTEM_SAVE)
		saveBtn.SAFE_SetEvent(self.__ClickButton, "SAVE")
		self.childrenDict["saveBtn"] = saveBtn

		scrollBar = CreateWindow(ui.ScrollBar(), bg, (bgReal.GetWidth() - 18, 4))
		scrollBar.SetScrollBarSize(bgReal.GetHeight() - 10)
		scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScroll))
		self.childrenDict["scrollBar"] = scrollBar

		__filterData = {
			0 : {
				"name": localeinfo.LOOTING_SYSTEM_AUTO,
				"job": False,
				"refine": False,
				"level": False,
				"info": localeinfo.LOOTING_SYSTEM_AUTO_TOOLTIP,
				"sub":{},
			},
			1 : {
				"name": localeinfo.LOOTING_SYSTEM_WEAPON,
				"job": True,
				"refine": True,
				"level": True,
				"info": localeinfo.LOOTING_SYSTEM_QUESTION_WEAPON,
				"sub":{},
			},
			2 : {
				"name": localeinfo.LOOTING_SYSTEM_ARMOR,
				"job": True,
				"refine": True,
				"level": True,
				"info": localeinfo.LOOTING_SYSTEM_QUESTION_ARMOR,
				"sub":{},
			},
			3 : {
				"name": localeinfo.LOOTING_SYSTEM_HEAD,
				"job": True,
				"refine": True,
				"level": True,
				"info": localeinfo.LOOTING_SYSTEM_QUESTION_HEAD,
				"sub":{},
			},
			4 : {
				"name": localeinfo.LOOTING_SYSTEM_COMMON,
				"job": True,
				"refine": True,
				"level": True,
				"info": localeinfo.LOOTING_SYSTEM_QUESTION_COMMON,
				"sub":{
					0 : localeinfo.LOOTING_SYSTEM_CATEGORY_COMMON_FOOTS,
					1 : localeinfo.LOOTING_SYSTEM_CATEGORY_COMMON_BELT,
					2 : localeinfo.LOOTING_SYSTEM_CATEGORY_COMMON_WRIST,
					3 : localeinfo.LOOTING_SYSTEM_CATEGORY_COMMON_NECK,
					4 : localeinfo.LOOTING_SYSTEM_CATEGORY_COMMON_EAR,
					5 : localeinfo.LOOTING_SYSTEM_CATEGORY_COMMON_SHIELD,
					6 : localeinfo.LOOTING_SYSTEM_CATEGORY_COMMON_GLOVE,
					7 : localeinfo.LOOTING_SYSTEM_CATEGORY_COMMON_PENDANT,
					8 : localeinfo.LOOTING_SYSTEM_CATEGORY_COMMON_ROD,
					9 : localeinfo.LOOTING_SYSTEM_CATEGORY_COMMON_PICK,
				},
			},
			5 : {
				"name": localeinfo.LOOTING_SYSTEM_COSTUME,
				"job": False,
				"refine": False,
				"level": False,
				"info": localeinfo.LOOTING_SYSTEM_QUESTION_COSTUME,
				"sub":{
					0 : localeinfo.LOOTING_SYSTEM_CATEGORY_COSTUME_WEAPON,
					1 : localeinfo.LOOTING_SYSTEM_CATEGORY_COSTUME_ARMOR,
					2 : localeinfo.LOOTING_SYSTEM_CATEGORY_COSTUME_HAIR,
					3 : localeinfo.LOOTING_SYSTEM_CATEGORY_COSTUME_ACCE,
					4 : localeinfo.LOOTING_SYSTEM_CATEGORY_COSTUME_AURA,
					5 : localeinfo.LOOTING_SYSTEM_CATEGORY_COSTUME_ETC,
				},
			},
			6 : {
				"name": localeinfo.LOOTING_SYSTEM_DS,
				"job": False,
				"refine": False,
				"level": False,
				"info": localeinfo.LOOTING_SYSTEM_QUESTION_DS,
				"sub": {
					0 : localeinfo.LOOTING_SYSTEM_CATEGORY_DS_DS,
					1 : localeinfo.LOOTING_SYSTEM_CATEGORY_DS_ETC,
				},
			},
			7 : {
				"name": localeinfo.LOOTING_SYSTEM_UNIQUE,
				"job": False,
				"refine": False,
				"level": False,
				"info": localeinfo.LOOTING_SYSTEM_QUESTION_UNIQUE,
				"sub":{
					0 : localeinfo.LOOTING_SYSTEM_CATEGORY_UNIQUE_ABILITY,
					1 : localeinfo.LOOTING_SYSTEM_CATEGORY_UNIQUE_ETC,
				},
			},
			8 : {
				"name": localeinfo.LOOTING_SYSTEM_REFINE,
				"job": False,
				"refine": False,
				"level": False,
				"info": localeinfo.LOOTING_SYSTEM_QUESTION_REFINE,
				"sub":{
					0 : localeinfo.LOOTING_SYSTEM_CATEGORY_REFINE_MATERIAL,
					1 : localeinfo.LOOTING_SYSTEM_CATEGORY_REFINE_STONE,
					2 : localeinfo.LOOTING_SYSTEM_CATEGORY_REFINE_ETC,
					3 : localeinfo.LOOTING_SYSTEM_CATEGORY_REFINE_ALCHEMY,
				},
			},
			9 : {
				"name": localeinfo.LOOTING_SYSTEM_POTION,
				"job": False,
				"refine": False,
				"level": False,
				"info": localeinfo.LOOTING_SYSTEM_QUESTION_POTION,
				"sub":{
					0 : localeinfo.LOOTING_SYSTEM_CATEGORY_POTION_ABILITY,
					1 : localeinfo.LOOTING_SYSTEM_CATEGORY_POTION_HAIRDYE,
					2 : localeinfo.LOOTING_SYSTEM_CATEGORY_POTION_ETC,
				},
			},
			10 : {
				"name": localeinfo.LOOTING_SYSTEM_FISH_MINING,
				"job": False,
				"refine": False,
				"level": False,
				"info": localeinfo.LOOTING_SYSTEM_QUESTION_FISH_MINING,
				"sub":{
					0 : localeinfo.LOOTING_SYSTEM_CATEGORY_FISH_MINING_FOOD,
					1 : localeinfo.LOOTING_SYSTEM_CATEGORY_FISH_MINING_STONE,
					2 : localeinfo.LOOTING_SYSTEM_CATEGORY_FISH_MINING_ETC,
				},
			},
			11 : {
				"name": localeinfo.LOOTING_SYSTEM_MOUNT_PET,
				"job": False,
				"refine": False,
				"level": False,
				"info": localeinfo.LOOTING_SYSTEM_QUESTION_MOUNT_PET,
				"sub":{
					0 : localeinfo.LOOTING_SYSTEM_CATEGORY_MOUNT_PET_CHARGED_PET,
					1 : localeinfo.LOOTING_SYSTEM_CATEGORY_MOUNT_PET_MOUNT,
					2 : localeinfo.LOOTING_SYSTEM_CATEGORY_MOUNT_PET_FREE_PET,
					3 : localeinfo.LOOTING_SYSTEM_CATEGORY_MOUNT_PET_EGG,
				},
			},
			12 : {
				"name": localeinfo.LOOTING_SYSTEM_SKILL_BOOK,
				"job": True,
				"refine": False,
				"level": False,
				"info": localeinfo.LOOTING_SYSTEM_QUESTION_SKILL_BOOK,
				"sub":{
					0 : localeinfo.LOOTING_SYSTEM_CATEGORY_COMMON,
				},
			},
			13 : {
				"name": localeinfo.LOOTING_SYSTEM_ETC,
				"job": False,
				"refine": False,
				"level": False,
				"info": localeinfo.LOOTING_SYSTEM_QUESTION_ETC,
				"sub":{
					0 : localeinfo.LOOTING_SYSTEM_CATEGORY_ETC_GIFTBOX,
					1 : localeinfo.LOOTING_SYSTEM_CATEGORY_ETC_MATRIMONY,
					2 : localeinfo.LOOTING_SYSTEM_CATEGORY_ETC_SEAL,
					3 : localeinfo.LOOTING_SYSTEM_CATEGORY_ETC_PARTY,
					4 : localeinfo.LOOTING_SYSTEM_CATEGORY_ETC_POLYMORPH,
					5 : localeinfo.LOOTING_SYSTEM_CATEGORY_ETC_RECIPE,
					6 : localeinfo.LOOTING_SYSTEM_CATEGORY_ETC_WEAPON_ARROW,
					7 : localeinfo.LOOTING_SYSTEM_CATEGORY_ETC_ETC,
				},
			},
			14 : {
				"name": localeinfo.LOOTING_SYSTEM_EVENT,
				"job": False,
				"refine": False,
				"level": False,
				"info": localeinfo.LOOTING_SYSTEM_QUESTION_EVENT,
				"sub":{},
			},
		}

		BIG_TITLE_START_X = 5
		CLASS_TITLE_START_X = 7
		REFINE_LEVEL_INPUT_START_X = 8
		SUB_TITLE_START_X = 12

		BIG_TITLE_STEP_HEIGHT = 28
		NEXT_CATEGORY_STEP_HEIGHT = 8

		__listDict = {}

		__jobList = [localeinfo.LOOTING_SYSTEM_CATEGORY_JOB_WARRIOR , localeinfo.LOOTING_SYSTEM_CATEGORY_JOB_ASSASSIN, localeinfo.LOOTING_SYSTEM_CATEGORY_JOB_SURA, localeinfo.LOOTING_SYSTEM_CATEGORY_JOB_SHAMAN]

		yPos = 5
		for categoryIdx, categoryData in __filterData.items():
			titleImg = CreateWindow(ui.ExpandedImageBox(), bg, (BIG_TITLE_START_X, yPos, True), IMG_FOLDER+"filter_title.tga")
			__listDict["{}titleImg".format(categoryIdx)] = titleImg
			__listDict["{}titleText".format(categoryIdx)] = CreateWindow(ui.TextLine(), titleImg, (0, 0), categoryData["name"] if categoryData.has_key("name") else "None", "all:align")
			titleInfo = CreateWindow(ui.Button(), titleImg, (0, 0))
			titleInfo.SetUpVisual(IMG_FOLDER+"info_btn_0.tga")
			titleInfo.SetOverVisual(IMG_FOLDER+"info_btn_1.tga")
			titleInfo.SetDownVisual(IMG_FOLDER+"info_btn_1.tga")
			titleInfo.SetShowToolTipEvent(ui.__mem_func__(self.__OverInInfo), categoryData["info"])
			titleInfo.SetHideToolTipEvent(ui.__mem_func__(self.__OverOut))
			titleInfo.SetPosition(titleImg.GetWidth() - (5 + titleInfo.GetWidth()), 4)
			__listDict["{}titleInfo".format(categoryIdx)] = titleInfo

			statusBtn = CreateWindow(ui.ToggleButton(), bg, (BIG_TITLE_START_X + titleImg.GetWidth() + 3, yPos + 3, True))
			statusBtn.SetUpVisual(IMG_FOLDER+"small_btn_0.tga")
			statusBtn.SetOverVisual(IMG_FOLDER+"small_btn_1.tga")
			statusBtn.SetDownVisual(IMG_FOLDER+"small_btn_2.tga")
			statusBtn.SetToggleUpEvent(self.__ClickStatus, categoryIdx, "OFF")
			statusBtn.SetToggleDownEvent(self.__ClickStatus, categoryIdx, "ON")
			statusBtn.SetText("OFF")
			statusBtn.SetWindowName("StatusBtn")

			__listDict["{}statusBtn".format(categoryIdx)] = statusBtn

			yPos += BIG_TITLE_STEP_HEIGHT

			jobIsEnabled = categoryData["job"] if categoryData.has_key("job") else False
			if jobIsEnabled:
				classImg = CreateWindow(ui.ExpandedImageBox(), bg, (CLASS_TITLE_START_X, yPos, True), IMG_FOLDER+"class_title.tga")
				__listDict["{}classImg".format(categoryIdx)] = classImg
				__listDict["{}classText".format(categoryIdx)] = CreateWindow(ui.TextLine(), classImg, (0, 0), localeinfo.LOOTING_SYSTEM_CATEGORY_JOB,"all:align")

				yPos+= 18
				
				for j in xrange(len(__jobList)):
					x =  144 if ((j+1)%2) == 0 else SUB_TITLE_START_X
					__listDict["{}{}jobSub".format(categoryIdx, j)] = CreateWindow(CheckBox(__jobList[j]), bg, (x, yPos, True))
					if ((j+1)%2) == 0:
						yPos+=19
					else:
						if len(__jobList)-1 == j:
							yPos+=19
				yPos+=2

			subDict = categoryData["sub"] if categoryData.has_key("sub") else {}
			if len(subDict) > 0:
				subImage = CreateWindow(ui.ExpandedImageBox(), bg, (CLASS_TITLE_START_X, yPos, True), IMG_FOLDER+"class_title.tga")
				__listDict["{}subImage".format(categoryIdx)] = subImage
				__listDict["{}subText".format(categoryIdx)] = CreateWindow(ui.TextLine(), subImage, (0, 0), localeinfo.LOOTING_SYSTEM_CATEGORY_JOB_PUBLIC,"all:align")
				yPos+= 18
				for j, subName in subDict.items():
					x =  144 if ((j+1)%2) == 0 else SUB_TITLE_START_X
					__listDict["{}{}sub".format(categoryIdx, j)] = CreateWindow(CheckBox(subName), bg, (x, yPos, True))

					if ((j+1)%2) == 0:
						yPos+=19
					else:
						if len(subDict)-1 == j:
							yPos+=19
				yPos+=2

			refineIsActive = categoryData["refine"] if categoryData.has_key("refine") else False
			if refineIsActive:
				refineImg = CreateWindow(ui.ExpandedImageBox(), bg, (REFINE_LEVEL_INPUT_START_X, yPos, True), IMG_FOLDER+"small_info.tga")
				__listDict["{}refineImg".format(categoryIdx)] = refineImg
				__listDict["{}refineText".format(categoryIdx)] = CreateWindow(ui.TextLine(), refineImg, (0, 0), localeinfo.LOOTING_SYSTEM_REFINE,"all:align")

				minRefineSlot = CreateWindow(ui.ExpandedImageBox(), bg, (REFINE_LEVEL_INPUT_START_X + 136, yPos, True), IMG_FOLDER+"input.tga")
				__listDict["{}minRefineSlot".format(categoryIdx)] = minRefineSlot

				minRefine = CreateWindow(ui.EditLine(), minRefineSlot, (2, 2), "", "", (minRefineSlot.GetWidth(), minRefineSlot.GetHeight()))
				minRefine.SetText("0")
				minRefine.SetMax(3)
				minRefine.SetNumberMode()
				minRefine.OnPressEscapeKey = lambda arg="EXIT":self.__ClickButton(arg)
				minRefine.OnIMEUpdate = lambda arg=_weakref.proxy(minRefine):self.__IMEUpdate(arg)
				__listDict["{}minRefine".format(categoryIdx)] = minRefine

				__listDict["{}refineSymbol".format(categoryIdx)] = CreateWindow(ui.TextLine(), bg, (REFINE_LEVEL_INPUT_START_X + 136 + 35, yPos + 2, True), "~")

				maxRefineSlot = CreateWindow(ui.ExpandedImageBox(), bg, (REFINE_LEVEL_INPUT_START_X + 136 + 46, yPos, True), IMG_FOLDER+"input.tga")
				__listDict["{}maxRefineSlot".format(categoryIdx)] = maxRefineSlot

				maxRefine = CreateWindow(ui.EditLine(), maxRefineSlot, (2, 2), "", "", (maxRefineSlot.GetWidth(), maxRefineSlot.GetHeight()))
				maxRefine.SetText("200")
				maxRefine.SetMax(3)
				maxRefine.SetNumberMode()
				maxRefine.OnIMEUpdate = lambda arg=_weakref.proxy(maxRefine):self.__IMEUpdate(arg)
				maxRefine.OnPressEscapeKey = lambda arg="EXIT":self.__ClickButton(arg)
				__listDict["{}maxRefine".format(categoryIdx)] = maxRefine

				yPos+=18

			levelIsActive = categoryData["level"] if categoryData.has_key("level") else False
			if levelIsActive:
				levelImg = CreateWindow(ui.ExpandedImageBox(), bg, (REFINE_LEVEL_INPUT_START_X, yPos, True), IMG_FOLDER+"small_info.tga")
				__listDict["{}levelImg".format(categoryIdx)] = levelImg
				__listDict["{}levelText".format(categoryIdx)] = CreateWindow(ui.TextLine(), levelImg, (0, 0), localeinfo.LOOTING_SYSTEM_WEARING_LEVEL,"all:align")

				minLevelSlot = CreateWindow(ui.ExpandedImageBox(), bg, (REFINE_LEVEL_INPUT_START_X + 136, yPos, True), IMG_FOLDER+"input.tga")
				__listDict["{}minLevelSlot".format(categoryIdx)] = minLevelSlot

				minLevel = CreateWindow(ui.EditLine(), minLevelSlot, (2, 2), "", "", (minLevelSlot.GetWidth(), minLevelSlot.GetHeight()))
				minLevel.SetText("0")
				minLevel.SetMax(3)
				minLevel.SetNumberMode()
				minLevel.OnIMEUpdate = lambda arg=_weakref.proxy(minLevel):self.__IMEUpdate(arg)
				minLevel.OnPressEscapeKey = lambda arg="EXIT":self.__ClickButton(arg)
				__listDict["{}minLevel".format(categoryIdx)] = minLevel

				__listDict["{}levelSymbol".format(categoryIdx)] = CreateWindow(ui.TextLine(), bg, (REFINE_LEVEL_INPUT_START_X + 136 + 35, yPos + 2, True), "~")

				maxLevelSlot = CreateWindow(ui.ExpandedImageBox(), bg, (REFINE_LEVEL_INPUT_START_X + 136 + 46, yPos, True), IMG_FOLDER+"input.tga")
				__listDict["{}maxLevelSlot".format(categoryIdx)] = maxLevelSlot

				maxLevel = CreateWindow(ui.EditLine(), maxLevelSlot, (2, 2), "", "", (maxLevelSlot.GetWidth(), maxLevelSlot.GetHeight()))
				maxLevel.SetText("200")
				maxLevel.SetMax(3)
				maxLevel.SetNumberMode()
				maxLevel.OnIMEUpdate = lambda arg=_weakref.proxy(maxLevel):self.__IMEUpdate(arg)
				maxLevel.OnPressEscapeKey = lambda arg="EXIT":self.__ClickButton(arg)
				__listDict["{}maxLevel".format(categoryIdx)] = maxLevel

				yPos+=18

			yPos += NEXT_CATEGORY_STEP_HEIGHT

		self.childrenDict["jobList"] = __jobList
		self.childrenDict["filterData"] = __filterData
		self.childrenDict["listDict"] = __listDict
		self.__OnScroll()
		self.__LoadData()

	def __IMEUpdate(self, editLinePointer):
		ui.EditLine.OnIMEUpdate(editLinePointer)

		text = editLinePointer.GetText()
		if len(text) > 1:
			if text[0] == "0":
				editLinePointer.SetText(text.replace("0", ""))
				editLinePointer.OnSetFocus()

		if len(editLinePointer.GetText()) == 0:
			editLinePointer.SetText("0")
			editLinePointer.OnSetFocus()

	def GetSettings(self, isDefault = True):
		_dict = {}

		__listDict = self.childrenDict["listDict"] if self.childrenDict.has_key("listDict") and isDefault == False else {}

		__filterData = self.childrenDict["filterData"] if self.childrenDict.has_key("filterData") else {}

		for categoryIdx, categoryData in __filterData.items():
			_dict["{}status".format(categoryIdx)] = 1 if isDefault else __listDict["{}statusBtn".format(categoryIdx)].IsDown()

			isJobNeed = categoryData["job"] if categoryData.has_key("job") else False
			if isJobNeed:
				__jobList = self.childrenDict["jobList"] if self.childrenDict.has_key("jobList") else []
				for j in xrange(len(__jobList)):
					_dict["{}{}job".format(categoryIdx, j)] = 1 if isDefault else __listDict["{}{}jobSub".format(categoryIdx, j)].GetStatus()

			isRefineNeed = categoryData["refine"] if categoryData.has_key("refine") else False
			if isRefineNeed:
				_dict["{}0refine".format(categoryIdx)] = 0 if isDefault else int(__listDict["{}minRefine".format(categoryIdx)].GetText() if len(__listDict["{}minRefine".format(categoryIdx)].GetText()) != 0 else "0")
				_dict["{}1refine".format(categoryIdx)] = 200 if isDefault else int(__listDict["{}maxRefine".format(categoryIdx)].GetText() if len(__listDict["{}maxRefine".format(categoryIdx)].GetText()) != 0 else "200")

			isLevelNeed = categoryData["level"] if categoryData.has_key("level") else False
			if isLevelNeed:
				_dict["{}0level".format(categoryIdx)] = 0 if isDefault else int(__listDict["{}minLevel".format(categoryIdx)].GetText() if len(__listDict["{}minLevel".format(categoryIdx)].GetText()) != 0 else "0")
				_dict["{}1level".format(categoryIdx)] = 120 if isDefault else int(__listDict["{}maxLevel".format(categoryIdx)].GetText() if len(__listDict["{}maxLevel".format(categoryIdx)].GetText()) != 0 else "120")

			isSubNeed = categoryData["sub"] if categoryData.has_key("sub") else {}
			for subIdx, subName in isSubNeed.items():
				_dict["{}{}sub".format(categoryIdx, subIdx)] = 1 if isDefault else __listDict["{}{}sub".format(categoryIdx, subIdx)].GetStatus()

		return _dict

	def __LoadData(self):
		filterData = {}

		charName = player.GetName()
		if len(charName) == 0:
			filterData = self.GetSettings(True)
		else:
			myPath = "UserData\\filter"

			import os
			if not os.path.exists(os.getcwd() + os.sep + myPath):
				filterData = self.GetSettings(True)
			else:
				filePath = myPath + "\\" + charName + ".data"
				if os.path.isfile(filePath):
					myFile = old_open(filePath, "rb")
					myData = myFile.read()
					myFile.close()

					dData = eval(myData)
					filterData = dData
				else:
					filterData = self.GetSettings(True)

		self.childrenDict["settings"] = filterData
		self.RefreshSettings()

	def RefreshSettings(self):
		__filterData = self.childrenDict["filterData"] if self.childrenDict.has_key("filterData") else {}
		__settings = self.childrenDict["settings"] if self.childrenDict.has_key("settings") else {}
		__listDict = self.childrenDict["listDict"] if self.childrenDict.has_key("listDict") else {}

		for categoryIdx, categoryData in __filterData.items():
			filterStatus = __settings["{}status".format(categoryIdx)] if __settings.has_key("{}status".format(categoryIdx)) else True
			if filterStatus:
				__listDict["{}statusBtn".format(categoryIdx)].Down()
			else:
				__listDict["{}statusBtn".format(categoryIdx)].SetUp()
			__listDict["{}statusBtn".format(categoryIdx)].SetText("ON" if filterStatus else "OFF")

			isJobNeed = categoryData["job"] if categoryData.has_key("job") else False
			if isJobNeed:
				for j in xrange(len(self.childrenDict["jobList"] if self.childrenDict.has_key("jobList") else [])):
					__listDict["{}{}jobSub".format(categoryIdx, j)].SetStatus(__settings["{}{}job".format(categoryIdx, j)] if __settings.has_key("{}{}job".format(categoryIdx, j)) else True)

			isRefineNeed = categoryData["refine"] if categoryData.has_key("refine") else False
			if isRefineNeed:
				__listDict["{}minRefine".format(categoryIdx)].SetText(str(__settings["{}0refine".format(categoryIdx)] if __settings.has_key("{}0refine".format(categoryIdx)) else 0))
				__listDict["{}maxRefine".format(categoryIdx)].SetText(str(__settings["{}1refine".format(categoryIdx)] if __settings.has_key("{}1refine".format(categoryIdx)) else 200))

			isLevelNeed = categoryData["level"] if categoryData.has_key("level") else False
			if isLevelNeed:
				__listDict["{}minLevel".format(categoryIdx)].SetText(str(__settings["{}0level".format(categoryIdx)] if __settings.has_key("{}0level".format(categoryIdx)) else 0))
				__listDict["{}maxLevel".format(categoryIdx)].SetText(str(__settings["{}1level".format(categoryIdx)] if __settings.has_key("{}1level".format(categoryIdx)) else 120))

			isSubNeed = categoryData["sub"] if categoryData.has_key("sub") else {}
			for subIdx, subName in isSubNeed.items():
				__listDict["{}{}sub".format(categoryIdx, subIdx)].SetStatus(__settings["{}{}sub".format(categoryIdx, subIdx)] if __settings.has_key("{}{}sub".format(categoryIdx, subIdx)) else 1)

	def __OnScroll(self):
		scrollBar = self.childrenDict["scrollBar"] if self.childrenDict.has_key("scrollBar") else None
		bg = self.childrenDict["bg"] if self.childrenDict.has_key("bg") else None
		listDict = self.childrenDict["listDict"] if self.childrenDict.has_key("listDict") else {}
		if scrollBar != None and bg != None and len(listDict) > 0:
			windowHeight = bg.GetHeight()
			screenSize = 0
			for childName, child in listDict.items():
				if child.exPos[0] != 0 and child.exPos[1] != 0:
					if child.exPos[1] + child.GetHeight() > screenSize:
						screenSize= child.exPos[1] + child.GetHeight()

			if screenSize > windowHeight:
				scrollLen = screenSize-windowHeight
				basePos = int(scrollBar.GetPos()*scrollLen)
				for childName, child in listDict.items():
					if child.exPos[0] != 0 and child.exPos[1] != 0:
						child.SetPosition(child.exPos[0],child.exPos[1]-basePos)
				scrollBar.SetMiddleBarSize(float(windowHeight-5)/float(screenSize))
			else:
				scrollBar.SetMiddleBarSize(1.0)

	def __OverOut(self):
		if self.tooltipItem:
			self.tooltipItem.Hide()

	def __OverInInfo(self, infoMsg):
			if self.tooltipItem:
				self.tooltipItem.ClearToolTip()
				self.tooltipItem.AutoAppendNewTextLineResize(infoMsg)
				self.tooltipItem.ShowToolTip()

	def __ClickStatus(self, categoryIdx, clickType):
		listDict = self.childrenDict["listDict"] if self.childrenDict.has_key("listDict") else None
		if listDict:
			statusBtn = listDict["{}statusBtn".format(categoryIdx)] if listDict.has_key("{}statusBtn".format(categoryIdx)) else None
			if statusBtn:
				statusBtn.SetText(clickType)

	def __ClickButton(self, clickType):
		if not self.childrenDict.has_key("qstdlg"):
			qstdlg = uicommon.QuestionDialog()
			self.childrenDict["qstdlg"] = qstdlg

		qstdlg = self.childrenDict["qstdlg"]

		if clickType == "SAVE":
			qstdlg.SetText(localeinfo.LOOTING_SYSTEM_EXIT_WITH_SAVING)
		elif clickType == "RESET":
			qstdlg.SetText(localeinfo.LOOTING_SYSTEM_INIT_QUESTION_DIALOG_TITLE)
		else:
			qstdlg.SetText(localeinfo.LOOTING_SYSTEM_EXIT_WITHOUT_SAVING)

		qstdlg.SetAcceptEvent(lambda arg = True: self.AnswerDialog(arg))
		qstdlg.SetCancelEvent(lambda arg = False: self.AnswerDialog(arg))
		qstdlg.Open()
		qstdlg.clickType = clickType

	def AnswerDialog(self, retStatus):
		if not self.childrenDict.has_key("qstdlg"):
			return

		qstdlg = self.childrenDict["qstdlg"]

		if retStatus:
			if qstdlg.clickType == "EXIT":
				self.Hide()
			else:
				settings = self.GetSettings(False if qstdlg.clickType == "SAVE" else True)

				charName = player.GetName()
				if len(charName) == 0:
					self.Hide()
					return

				myPath = "UserData\\filter"

				import os
				if not os.path.exists(os.getcwd() + os.sep + myPath):
					os.mkdir(os.getcwd() + os.sep + myPath)

				filePath = myPath + "\\" + charName + ".data"
				if os.path.isfile(filePath):
					myFile = old_open(filePath, "w")
					myFile.close()

				myFile = old_open(filePath, "w")
				myData = str(settings)
				myFile.write(myData)
				myFile.close()

				self.childrenDict["settings"] = settings
				self.RefreshSettings()
				self.ClearFilterSettings()
				self.SendFilterSettings(False)

				if qstdlg.clickType == "SAVE":
					self.Hide()

		qstdlg.Close()

	def OnRunMouseWheel(self, nLen):
		scrollBar = self.childrenDict["scrollBar"] if self.childrenDict.has_key("scrollBar") else None
		if scrollBar:
			if nLen > 0:
				scrollBar.OnUp()
				return True
			else:
				scrollBar.OnDown()
				return True
			return False

	def Show(self):
		self.__LoadData()

		self.SetTop()
		wndMgr.Show(self.hWnd)

	def Hide(self):
		self.AnswerDialog(False)
		wndMgr.Hide(self.hWnd)

	def OnPressEscapeKey(self):
		self.__ClickButton("EXIT")
		return True

	def ClearFilterSettings(self):
		net.SendChatPacket("/filter clear")
		player.PickupFilterClear()

	def SendFilterSettings(self, needLoad = True):
		if needLoad:
			self.__LoadData()

		__filterData = self.childrenDict["filterData"] if self.childrenDict.has_key("filterData") else {}
		__settings = self.childrenDict["settings"] if self.childrenDict.has_key("settings") else {}

		cmd = ""
		for categoryIdx, categoryData in __filterData.items():
			cmd = ""
			filterStatus = __settings["{}status".format(categoryIdx)] if __settings.has_key("{}status".format(categoryIdx)) else True
			if not filterStatus:
				net.SendChatPacket("/filter unset {}".format(categoryIdx))
				player.PickupFilterUnset(categoryIdx)
				continue
			elif categoryIdx == 0:
				net.SendChatPacket("/filter set {} 1".format(categoryIdx))
				player.PickupFilterSet(categoryIdx, "/filter set {} 1".format(categoryIdx))
				continue

			isJobNeed = categoryData["job"] if categoryData.has_key("job") else False
			cmd+= "1" if isJobNeed else "0"
			cmd += " "

			isRefineNeed = categoryData["refine"] if categoryData.has_key("refine") else False
			cmd+= "1" if isRefineNeed else "0"
			cmd += " "

			isLevelNeed = categoryData["level"] if categoryData.has_key("level") else False
			cmd+= "1" if isLevelNeed else "0"
			cmd += " "

			for j in xrange(len(self.childrenDict["jobList"] if self.childrenDict.has_key("jobList") else [])):
				if j != 0:
					cmd+="-"
				cmd+= ("1" if __settings["{}{}job".format(categoryIdx, j)] else "0") if __settings.has_key("{}{}job".format(categoryIdx, j)) else "0"

			cmd+= " "

			cmd+= str(__settings["{}0refine".format(categoryIdx)]) if __settings.has_key("{}0refine".format(categoryIdx)) else "0"
			cmd+="-"
			cmd+= str(__settings["{}1refine".format(categoryIdx)]) if __settings.has_key("{}1refine".format(categoryIdx)) else "200"
			cmd+=" "

			cmd+= str(__settings["{}0level".format(categoryIdx)]) if __settings.has_key("{}0level".format(categoryIdx)) else "0"
			cmd+="-"
			cmd+= str(__settings["{}1level".format(categoryIdx)]) if __settings.has_key("{}1level".format(categoryIdx)) else "120"

			isSubNeed = categoryData["sub"] if categoryData.has_key("sub") else {}
			if len(isSubNeed) > 0:
				cmd+=" "
				for subIdx, subName in isSubNeed.items():
					if subIdx != 0:
						cmd+="-"
					cmd+= str(1 if __settings["{}{}sub".format(categoryIdx, subIdx)] else 0) if __settings.has_key("{}{}sub".format(categoryIdx, subIdx)) else "1"

			net.SendChatPacket("/filter set {} {}".format(categoryIdx, cmd))
			player.PickupFilterSet(categoryIdx, "/filter set {} {}".format(categoryIdx, cmd))

class CheckBox(ui.ExpandedImageBox):
	def Destroy(self):
		self.checkStatus = False
		self.text = None

	def __init__(self, printText):
		ui.ExpandedImageBox.__init__(self)
		self.Destroy()
		self.LoadImage(IMG_FOLDER+"checkbox_deactive.tga")
		self.SetEvent(ui.__mem_func__(self.__Click), "mouse_click")
		self.text = CreateWindow(ui.TextLine(), self, (0, 0), printText, "all:align")

	def __Click(self):
		self.checkStatus = not self.checkStatus
		self.LoadImage(IMG_FOLDER+"checkbox_deactive.tga" if not self.checkStatus else IMG_FOLDER+"checkbox_active.tga")

	def SetStatus(self, status):
		self.checkStatus = status
		self.LoadImage(IMG_FOLDER+"checkbox_deactive.tga" if not self.checkStatus else IMG_FOLDER+"checkbox_active.tga")

	def GetStatus(self):
		return self.checkStatus

def CreateWindow(window, parent, windowPos, windowArgument = "", windowPositionRule = "", windowSize = (-1, -1), windowFontName = -1):
	window.SetParent(parent)
	window.SetPosition(*windowPos)
	if windowSize != (-1, -1):
		window.SetSize(*windowSize)

	if windowPositionRule:
		splitList = windowPositionRule.split(":")
		if len(splitList) == 2:
			(type, mode) = (splitList[0], splitList[1])
			if type == "all":
				window.SetHorizontalAlignCenter()
				window.SetVerticalAlignCenter()
				window.SetWindowHorizontalAlignCenter()
				window.SetWindowVerticalAlignCenter()
			elif type == "horizontal":
				if isinstance(window, ui.TextLine):
					if mode == "center":
						window.SetHorizontalAlignCenter()
					elif mode == "right":
						window.SetHorizontalAlignRight()
					elif mode == "left":
						window.SetHorizontalAlignLeft()
				else:
					if mode == "center":
						window.SetWindowHorizontalAlignCenter()
					elif mode == "right":
						window.SetWindowHorizontalAlignRight()
					elif mode == "left":
						window.SetWindowHorizontalAlignLeft()
			elif type == "vertical":
				if isinstance(window, ui.TextLine):
					if mode == "center":
						window.SetVerticalAlignCenter()
					elif mode == "top":
						window.SetVerticalAlignTop()
					elif mode == "bottom":
						window.SetVerticalAlignBottom()
				else:
					if mode == "top":
						window.SetWindowVerticalAlignTop()
					elif mode == "center":
						window.SetWindowVerticalAlignCenter()
					elif mode == "bottom":
						window.SetWindowVerticalAlignBottom()

	if windowArgument:
		if isinstance(window, ui.TextLine):
			if windowFontName != -1:
				window.SetFontName(windowFontName)
			window.SetText(windowArgument)
		elif isinstance(window, ui.NumberLine):
			window.SetNumber(windowArgument)
		elif isinstance(window, ui.ExpandedImageBox) or isinstance(window, ui.ImageBox):
			window.LoadImage(windowArgument if windowArgument.find("gr2") == -1 else "icon/item/27995.tga")

	window.Show()
	return window

if app.USE_AUTO_HUNT and not app.USE_PICKUP_FILTER_ICO_AS_AFFECT:
	class FilterIco(ui.Window):
		filterIcoX = 0
		filterIcoY = 0

		def __init__(self, inst):
			self.interfaceInst = _weakref.proxy(inst)

			ui.Window.__init__(self, "TOP_MOST")

			self.AddFlag("attach")
			self.AddFlag("movable")

			tmp = ui.ExpandedImageBox()
			tmp.LoadImage(IMG_FOLDER + "filterico.tga")

			w = tmp.GetWidth()
			h = tmp.GetHeight()

			self.SetSize(w, h)

			tmp.AddFlag("not_pick")
			tmp.SetParent(self)
			tmp.SetPosition(0, 0)
			tmp.Show()

			if constinfo.filterIcoX == 0 and constinfo.filterIcoY == 0:
				self.SetPosition(wndMgr.GetScreenWidth() - w - 10, (wndMgr.GetScreenWidth() - h) / 2)
				(x, y) = self.GetGlobalPosition()

				constinfo.filterIcoX = x
				constinfo.filterIcoY = y

				self.filterIcoX = x
				self.filterIcoY = y

			self.SetOnMouseLeftButtonUpEvent(self.Click)

			self.ico = tmp

			self.Show()

		def __del__(self):
			ui.Window.__del__(self)

		def Destroy(self):
			self.interfaceInst = None

			if self.ico:
				self.ico.SetOnMouseLeftButtonUpEvent(None)
				del self.ico

		def Click(self):
			if self.interfaceInst:
				if self.filterIcoX != constinfo.filterIcoX and self.filterIcoY != constinfo.filterIcoY:
					self.filterIcoX = constinfo.filterIcoX
					self.filterIcoY = constinfo.filterIcoY
					return

				self.interfaceInst.OpenFilterWindow()

		def Show(self):
			(x, y) = self.GetGlobalPosition()
			if x != constinfo.filterIcoX and y != constinfo.filterIcoY:
				self.SetPosition(constinfo.filterIcoX, constinfo.filterIcoY)
				self.filterIcoX = constinfo.filterIcoX
				self.filterIcoY = constinfo.filterIcoY

			wndMgr.Show(self.hWnd)

		def OnMoveWindow(self, x, y):
			constinfo.filterIcoX = x
			constinfo.filterIcoY = y
