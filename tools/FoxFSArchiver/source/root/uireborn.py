import app
import player
import net

import ui
import chat
import wndMgr
import uitooltip
import uicommon
import constinfo
import mousemodule
import time
import localeinfo
import grp
import playersettingmodule
import getclass
import background
import os
import uitip
from _weakref import proxy

# POSITIONS #
infoNumber_x = 400 
infoNumber_y = 75 	
infoText_x = 138
infoText_y = 15
infoStatus_x = 605
infoStatus_y = 155
infoButton_x = 10
infoButton_y = 265
# POSITIONS #

pShow = 0
infoPercentage_x = 0
infoPercentage_y = 100

REBORN_FACE_LIST = {
	playersettingmodule.RACE_WARRIOR_M	: "d:/ymir work/ui/public/reborn_image/face/warriorM.tga",
	playersettingmodule.RACE_WARRIOR_W	: "d:/ymir work/ui/public/reborn_image/face/warriorF.tga",
	playersettingmodule.RACE_ASSASSIN_M	: "d:/ymir work/ui/public/reborn_image/face/ninjaM.tga",
	playersettingmodule.RACE_ASSASSIN_W	: "d:/ymir work/ui/public/reborn_image/face/ninjaF.tga",
	playersettingmodule.RACE_SURA_M		: "d:/ymir work/ui/public/reborn_image/face/suraM.tga",
	playersettingmodule.RACE_SURA_W		: "d:/ymir work/ui/public/reborn_image/face/suraF.tga",
	playersettingmodule.RACE_SHAMAN_M	: "d:/ymir work/ui/public/reborn_image/face/shamanM.tga",
	playersettingmodule.RACE_SHAMAN_W	: "d:/ymir work/ui/public/reborn_image/face/shamanF.tga",
	#playersettingmodule.RACE_WOLFMAN_M	: "icon/face/wolfman_m.tga",
}
REBORN_IMAGE_LIST = ["d:/ymir work/ui/public/reborn_image/complete.png", "d:/ymir work/ui/public/reborn_image/necomplete.png", "d:/ymir work/ui/public/XLarge_Button_01.sub", "d:/ymir work/ui/public/XLarge_Button_02.sub", "d:/ymir work/ui/public/XLarge_Button_03.sub", "d:/ymir work/ui/public/reborn_image/anonymous_1.png","d:/ymir work/ui/public/reborn_image/anonymous_2.png","d:/ymir work/ui/public/reborn_image/anonymous_3.png"]
REBORN_TOOLTIP_COLOR = [0xffffffcc, 0xffe6ff5f, 0xff88f6ff, 0xffff2121]

REBORN_SEND_MESSAGE = (localeinfo.REBORN_MESSAGE_1, localeinfo.REBORN_MESSAGE_2)	
REBORN_NAME_GRADE = (localeinfo.REBORN_NAME_GRADE_0, localeinfo.REBORN_NAME_GRADE_1, localeinfo.REBORN_NAME_GRADE_2, localeinfo.REBORN_NAME_GRADE_3)
REBORN_NUMBER_BONUS = (localeinfo.REBORN_TEXT_BONUS_1, localeinfo.REBORN_TEXT_BONUS_2, localeinfo.REBORN_TEXT_BONUS_3, localeinfo.REBORN_TEXT_BONUS_4)
REBORN_TOOLTIP_COMPLETE = (localeinfo.REBORN_TOOLTIP_COMPLETE_1, localeinfo.REBORN_TOOLTIP_COMPLETE_2, localeinfo.REBORN_TOOLTIP_COMPLETE_3)
REBORN_TOOLTIP_DISABLE = (localeinfo.REBORN_TOOLTIP_UNFINISHED_1, localeinfo.REBORN_TOOLTIP_UNFINISHED_2, localeinfo.REBORN_TOOLTIP_UNFINISHED_3)					
REBORN_INFO_TEXT = (localeinfo.REBORN_SLOT_1, localeinfo.REBORN_SLOT_2, localeinfo.REBORN_SLOT_3, localeinfo.REBORN_TITLE, localeinfo.REBORN_INFO, localeinfo.REBORN_INFO_ACTIVE, localeinfo.REBORN_INFO_GRADE, localeinfo.REBORN_INFO_BTN_1, localeinfo.REBORN_INFO_BTN_2, localeinfo.REBORN_INFO_BTN_3)			
REBORN_BONUS_NAME = (localeinfo.REBORN_NAME_BONUS_1, localeinfo.REBORN_NAME_BONUS_2, localeinfo.REBORN_NAME_BONUS_3, localeinfo.REBORN_NAME_BONUS_4)

REBORN_BONUS_VALUE_1 = ("500", "3", "2", "5")
REBORN_BONUS_VALUE_2 = ("1000", "3", "2", "5")
REBORN_BONUS_VALUE_3 = ("1500", "4", "2", "5")
REBORN_GLOBAL_TEXT = "|cFF6af200[ i ] [PASS]Sistemul de reborn iti permite sa reincepi jocul de la nivelul 30 in schimbul[PASS] unor bonusuri atat pvm cat si pvp.[PASS]Poti face acest lucru de 3 ori iar bonusurile se acumuleaza![PASS]Pentru orice nelamurire legata de sistem contacteaza un membru al echipei![PASS]Mult succes!"

class RebornClass(ui.BoardWithTitleBar):
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.__BuildWindow()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def __BuildWindow(self):
		self.SetSize(720, 200)
		self.AddFlag('movable')
		self.AddFlag('float')
		self.SetTitleName(REBORN_INFO_TEXT[3])
		self.SetCloseEvent(ui.__mem_func__(self.Close))
		self.SetCenterPosition()
		
		self.modulePython = getclass.ClassModulePython()
		race = net.GetMainActorRace()	
		face_image = REBORN_FACE_LIST[race]
		
		self.BoardMessage = uitip.BigBoard()
		self.BoardMessage.SetTop()
		
		self.ThinboardGlobal = ui.ThinBoard()
		self.ThinboardGlobal.SetParent(self)
		self.ThinboardGlobal.SetSize(350, 110)
		self.ThinboardGlobal.SetPosition(15, 55)
		self.ThinboardGlobal.Show()	
		
		self.sStatusDisable_a = self.modulePython.ExpandedImage(self, infoStatus_x, infoStatus_y, REBORN_IMAGE_LIST[1])	
		self.sStatusDisable_b = self.modulePython.ExpandedImage(self, infoStatus_x+40, infoStatus_y, REBORN_IMAGE_LIST[1])
		self.sStatusDisable_c = self.modulePython.ExpandedImage(self, infoStatus_x+40+40, infoStatus_y, REBORN_IMAGE_LIST[1])
		
		self.sStatusEnable_a = self.modulePython.ExpandedImage(self, infoStatus_x, infoStatus_y, REBORN_IMAGE_LIST[0])
		self.sStatusEnable_b = self.modulePython.ExpandedImage(self, infoStatus_x+40, infoStatus_y, REBORN_IMAGE_LIST[0])
		self.sStatusEnable_c = self.modulePython.ExpandedImage(self, infoStatus_x+40+40, infoStatus_y, REBORN_IMAGE_LIST[0])	

		sHide = [self.sStatusEnable_a,self.sStatusEnable_b,self.sStatusEnable_c]
		for i in sHide:
			i.Hide()	
		
		self.sSlotBar_a = self.modulePython.TextLine(self.ThinboardGlobal, REBORN_INFO_TEXT[0], infoText_x, infoText_y, self.modulePython.RGB(255, 155, 122))	
		self.sSlotBar_b = self.modulePython.TextLine(self.ThinboardGlobal, REBORN_INFO_TEXT[1], infoText_x, infoText_y+30, self.modulePython.RGB(255, 155, 122))		
		self.sSlotBar_c = self.modulePython.TextLine(self.ThinboardGlobal, REBORN_INFO_TEXT[2], infoText_x, infoText_y+30+30, self.modulePython.RGB(255, 155, 122))	
		# Numbering bonus
		self.sInfoBonusNumber_a = self.modulePython.TextLine(self, REBORN_NUMBER_BONUS[0], infoNumber_x, infoNumber_y, self.modulePython.RGB(41, 252, 167))				
		self.sInfoBonusNumber_b = self.modulePython.TextLine(self, REBORN_NUMBER_BONUS[1], infoNumber_x, infoNumber_y+20, self.modulePython.RGB(41, 252, 167))				
		self.sInfoBonusNumber_c = self.modulePython.TextLine(self, REBORN_NUMBER_BONUS[2], infoNumber_x, infoNumber_y+20+20, self.modulePython.RGB(41, 252, 167))				
		self.sInfoBonusNumber_d = self.modulePython.TextLine(self, REBORN_NUMBER_BONUS[3], infoNumber_x, infoNumber_y+20+20+20, self.modulePython.RGB(41, 252, 167))	
		# Name bonus
		self.sInfoBonusType_a = self.modulePython.TextLine(self, REBORN_BONUS_NAME[0], infoNumber_x+52, infoNumber_y, self.modulePython.RGB(252, 231, 41))				
		self.sInfoBonusType_b = self.modulePython.TextLine(self, REBORN_BONUS_NAME[1], infoNumber_x+52, infoNumber_y+20, self.modulePython.RGB(252, 231, 41))				
		self.sInfoBonusType_c = self.modulePython.TextLine(self, REBORN_BONUS_NAME[2], infoNumber_x+52, infoNumber_y+20+20, self.modulePython.RGB(252, 231, 41))				
		self.sInfoBonusType_d = self.modulePython.TextLine(self, REBORN_BONUS_NAME[3], infoNumber_x+52, infoNumber_y+20+20+20, self.modulePython.RGB(252, 231, 41))	
		# Value bonus
		self.sGradeBonus1_a = self.modulePython.SlotWithText(self, REBORN_BONUS_VALUE_1[0], infoNumber_x+200, infoNumber_y, 30,15) # Get value bonus -> Max.HP
		self.sGradeBonus1_b = self.modulePython.SlotWithText(self, REBORN_BONUS_VALUE_1[1], infoNumber_x+200, infoNumber_y+20, 30,15) # Get value bonus -> Semi-humans	
		self.sGradeBonus1_c = self.modulePython.SlotWithText(self, REBORN_BONUS_VALUE_1[2], infoNumber_x+200, infoNumber_y+20+20, 30,15) # Get value bonus -> Critical chance
		self.sGradeBonus1_d = self.modulePython.SlotWithText(self, REBORN_BONUS_VALUE_1[3], infoNumber_x+200, infoNumber_y+20+20+20, 30,15) # Get value bonus -> Penetration chance
		self.sGradeBonus2_a = self.modulePython.SlotWithText(self, REBORN_BONUS_VALUE_2[0], infoNumber_x+200+40, infoNumber_y, 30,15) # Get value bonus -> Max.HP
		self.sGradeBonus2_b = self.modulePython.SlotWithText(self, REBORN_BONUS_VALUE_2[1], infoNumber_x+200+40, infoNumber_y+20, 30,15) # Get value bonus -> Semi-humans	
		self.sGradeBonus2_c = self.modulePython.SlotWithText(self, REBORN_BONUS_VALUE_2[2], infoNumber_x+200+40, infoNumber_y+20+20, 30,15) # Get value bonus -> Critical chance
		self.sGradeBonus2_d = self.modulePython.SlotWithText(self, REBORN_BONUS_VALUE_2[3], infoNumber_x+200+40, infoNumber_y+20+20+20, 30,15) # Get value bonus -> Penetration chance
		self.sGradeBonus3_a = self.modulePython.SlotWithText(self, REBORN_BONUS_VALUE_3[0], infoNumber_x+200+40+40, infoNumber_y, 30,15) # Get value bonus -> Max.HP
		self.sGradeBonus3_b = self.modulePython.SlotWithText(self, REBORN_BONUS_VALUE_3[1], infoNumber_x+200+40+40, infoNumber_y+20, 30,15) # Get value bonus -> Semi-humans	
		self.sGradeBonus3_c = self.modulePython.SlotWithText(self, REBORN_BONUS_VALUE_3[2], infoNumber_x+200+40+40, infoNumber_y+20+20, 30,15) # Get value bonus -> Critical chance
		self.sGradeBonus3_d = self.modulePython.SlotWithText(self, REBORN_BONUS_VALUE_3[3], infoNumber_x+200+40+40, infoNumber_y+20+20+20, 30,15) # Get value bonus -> Penetration chance
		# Button to accept reborn
		self.buttonGetToAccept = self.modulePython.Button(self, REBORN_INFO_TEXT[4], REBORN_INFO_TEXT[5], 400, 164, self.MakeReborn, REBORN_IMAGE_LIST[2], REBORN_IMAGE_LIST[3], REBORN_IMAGE_LIST[4])	
		# self.buttonShowAnonymous = self.modulePython.Button(self, "", REBORN_INFO_TEXT[9], 675, 11, self.ShowFaceAnonymous, "d:/ymir work/ui/game/windows/btn_plus_up.sub", "d:/ymir work/ui/game/windows/btn_plus_over.sub", "d:/ymir work/ui/game/windows/btn_plus_down.sub")	
		# Set image face
		self.sSetImageFace = ui.ImageBox()
		self.sSetImageFace.SetParent(self)
		self.sSetImageFace.SetPosition(35, 60)
		self.sSetImageFace.LoadImage(face_image)
		self.sSetImageFace.Show()
		# Text informations title from actual reborn and what bonus is give you
		self.sTitle = ui.TextLine()
		self.sTitle.SetFontName("Tahoma:17")
		self.sTitle.SetParent(self)
		self.sTitle.SetPosition(177,45)
		self.sTitle.SetText(REBORN_INFO_TEXT[6])
		self.sTitle.SetWindowHorizontalAlignCenter()
		self.sTitle.SetHorizontalAlignCenter()
		self.sTitle.SetOutline()
		self.sTitle.Show()
		# Make gauge with state reborn
		self.sPercentage = ui.Gauge()
		self.sPercentage.SetParent(self)
		self.sPercentage.SetPosition(25, 175)
		self.sPercentage.MakeGauge(325, "red")
		self.sPercentage.Show()

		self.slotBar_1 = self.modulePython.SlotWithText(self.sSlotBar_a, '' + player.GetName(), 100, 0, 100,18) # Get name update
		self.slotBar_2 = self.modulePython.SlotWithText(self.sSlotBar_b, '' + str(player.GetStatus(player.LEVEL)), 100, 0, 100,18)	# Get update from level text
		self.slotBar_3 = self.modulePython.SlotWithText(self.sSlotBar_c, REBORN_NAME_GRADE[2], 100, 0, 100,18) # Get name from actual grade reborn
		self.GlobalLoad()	

	def MakeAnonymous(self):	
		net.SendChatPacket("/set_reborn anonymous") # Send command to server with argument anonymous for enable/disable reborn on name visible
		
	def ShowFaceAnonymous(self):
		global pShow
		if pShow == 0:
			pShow = 1
			k, v = player.GetReborn() # Get Function table actual reborn from src	
			if k == 3 and not constinfo.mStrAnonymous == 1:
				self.SetSize(830, 200)
				self.sTitle.SetPosition(165,45)
				self.buttonGetAnonymous = self.modulePython.Button(self, "", "", infoStatus_x+40+40+30, infoStatus_y-90, self.MakeAnonymous, REBORN_IMAGE_LIST[5], REBORN_IMAGE_LIST[6], REBORN_IMAGE_LIST[7])
		else:
			pShow = 0
			k, v = player.GetReborn() # Get Function table actual reborn from src	
			if k == 3 and not constinfo.mStrAnonymous == 1:
				self.SetSize(720, 200)
				self.sTitle.SetPosition(165,45)
				self.buttonGetAnonymous = self.modulePython.Button(self, "", "", infoStatus_x+40+40+30, infoStatus_y-90, self.MakeAnonymous, REBORN_IMAGE_LIST[5], REBORN_IMAGE_LIST[6], REBORN_IMAGE_LIST[7])
				self.buttonGetAnonymous.Hide()			
			
	def GlobalLoad(self):	
		if constinfo.mStrAnonymous == 1:
			self.SetSize(830, 200)
			self.buttonGetAnonymous = self.modulePython.Button(self, "", "", infoStatus_x+40+40+30, infoStatus_y-90, self.MakeAnonymous, REBORN_IMAGE_LIST[5], REBORN_IMAGE_LIST[6], REBORN_IMAGE_LIST[7])		

			self.slotBar_3.SetText(REBORN_NAME_GRADE[3])
			self.sPercentage.SetPercentage(infoPercentage_x+10+5+10+5+10+5+10+5+10+5+10+5+10,infoPercentage_y) # Update gauge to 100%
			
			sOpen = [self.sStatusEnable_a,self.sStatusEnable_b,self.sStatusEnable_c]
			sHide = [self.sStatusDisable_a,self.sStatusDisable_b,self.sStatusDisable_c]	
			for i in sOpen:
				i.Show()
			for i in sHide:
				i.Hide()	
				
	def MakeReborn(self):	
		k, v = player.GetReborn() # Get Function table actual reborn from src	
		if k == 3:
			self.Hide()
			self.sPopupFinish = uicommon.PopupDialog()
			self.sPopupFinish.SetWidth(550)
			self.sPopupFinish.SetText(REBORN_SEND_MESSAGE[1])
			self.sPopupFinish.Show()
		else:
			self.Hide() # Closing the initial window of the X initial brand
			
			self.Background = ui.Bar()
			self.Background.AddFlag("attach")
			self.Background.AddFlag("not_pick")
			self.Background.SetPosition(0,0)
			self.Background.SetSize(wndMgr.GetScreenWidth(),wndMgr.GetScreenHeight())
			self.Background.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.71))
			self.Background.Show()
			
			self.RequestBoard = ui.BoardWithTitleBar()
			self.RequestBoard.SetTitleName(REBORN_INFO_TEXT[3])
			self.RequestBoard.SetCloseEvent(ui.__mem_func__(self.CoseBoardReborn))
			self.RequestBoard.SetParent(self.Background)
			self.RequestBoard.SetSize(400,300)
			self.RequestBoard.SetTop()
			self.RequestBoard.SetCenterPosition()
			self.RequestBoard.Show()
			
			self.TextInformations = ui.MultiTextLine()
			self.TextInformations.SetParent(self.RequestBoard)
			self.TextInformations.SetPosition(10,40)
			self.TextInformations.SetWidth(385)
			self.TextInformations.SetText(REBORN_GLOBAL_TEXT)
			self.TextInformations.SetTextHorizontalAlignCenter()
			self.TextInformations.Show()
			
			self.LineOnText = ui.Line()
			self.LineOnText.AddFlag("attach")
			self.LineOnText.AddFlag("not_pick")
			self.LineOnText.SetParent(self.RequestBoard)
			self.LineOnText.SetPosition(10,180)
			self.LineOnText.SetSize(385,0)
			self.LineOnText.SetColor(0xffffffff)
			self.LineOnText.Show()
			
			self.s_ActivateReborn = self.modulePython.Button(self.RequestBoard, REBORN_INFO_TEXT[7], REBORN_INFO_TEXT[7], infoButton_x, infoButton_y, self.AcceptReborn, REBORN_IMAGE_LIST[2], REBORN_IMAGE_LIST[3], REBORN_IMAGE_LIST[4])
			self.s_DeclineReborn = self.modulePython.Button(self.RequestBoard, REBORN_INFO_TEXT[8], REBORN_INFO_TEXT[8], infoButton_x+200, infoButton_y, self.DeclineReborn, REBORN_IMAGE_LIST[2], REBORN_IMAGE_LIST[3], REBORN_IMAGE_LIST[4])
		
	def DeclineReborn(self):
		sHide = [self.RequestBoard, # Hide window with accept and decline
						self.Background] # Hide black background that occupies the entire screen
		for i in sHide:
			i.Hide()
			
		self.Show() # After you have clicked on the button box Initial accept will return again
		
	def AcceptReborn(self):
		sHide = [self.RequestBoard, # Hide window with accept and decline
						self.Background] # Hide black background that occupies the entire screen
		for i in sHide:
			i.Hide()
			
		net.SendChatPacket("/set_reborn accept") # Send command to server with argument accept
		self.Show() # After you have clicked on the button box Initial accept will return again
		
	def Destroy(self):
		self.Hide() # Destroy the initial window 
			
	def Close(self):
		self.Hide() # Closing the initial window of the X initial brand
		
	def CoseBoardReborn(self):
		sHide = [self.RequestBoard, # Hide window with accept and decline
						self.Background] # Hide black background that occupies the entire screen
		for i in sHide:
			i.Hide()
			
	def OpenBigNotice(self, message):	
		msg = int(message) # Get value with (int)
		if msg == 1:
			self.BoardMessage.SetTip(REBORN_SEND_MESSAGE[0] + REBORN_NAME_GRADE[1] + " !")
		elif msg == 2:	
			self.BoardMessage.SetTip(REBORN_SEND_MESSAGE[0] + REBORN_NAME_GRADE[2] + " !")		
		elif msg == 3:		
			self.BoardMessage.SetTip(REBORN_SEND_MESSAGE[0] + REBORN_NAME_GRADE[3] + " !")	
						
	def OpenArgument(self, reborn):
		arg = str(reborn) # Get value with (str)
		
		if arg == "reborn_1":
			self.OpenBigNotice(1)
		elif arg == "reborn_2":
			self.OpenBigNotice(1+1)
		elif arg == "reborn_3":
			self.OpenBigNotice(1+1+1)
		elif arg == "pro_0":
			constinfo.mStrAnonymous = 0
		elif arg == "pro_1":
			constinfo.mStrAnonymous = 1
						
	def UpdateFunction_NoneType(self):	
		self.slotBar_3[1].SetText(REBORN_NAME_GRADE[0])
		self.sPercentage.SetPercentage(infoPercentage_x,infoPercentage_y) # Remove complete gauge from reborn	
				
	def UpdateFunction_A(self):	
		self.sStatusDisable_a.Hide()
		self.sStatusEnable_a.Show()	
			
		self.slotBar_3[1].SetText(REBORN_NAME_GRADE[1])
		self.sPercentage.SetPercentage(infoPercentage_x+10+5+10+5,infoPercentage_y) # Update gauge +30%
				
	def UpdateFunction_B(self):	
		sHide = [self.sStatusDisable_a,self.sStatusDisable_b]
		sOpen = [self.sStatusEnable_a,self.sStatusEnable_b]
		
		for i in sOpen:
			i.Show()
			
		for i in sHide:
			i.Hide()
			
		self.slotBar_3[1].SetText(REBORN_NAME_GRADE[2])
		self.sPercentage.SetPercentage(infoPercentage_x+10+5+10+5+10+5+10+5,infoPercentage_y) # Update gauge +60%
	
	def UpdateFunction_C(self):		
		sHide = [self.sStatusDisable_a,self.sStatusDisable_b,self.sStatusDisable_c]
		sOpen = [self.sStatusEnable_a,self.sStatusEnable_b,self.sStatusEnable_c]
		
		for i in sHide:
			i.Hide()
		
		for i in sOpen:
			i.Show()
			
		self.slotBar_3[1].SetText(REBORN_NAME_GRADE[3])
		self.sPercentage.SetPercentage(infoPercentage_x+10+5+10+5+10+5+10+5+10+5+10+5+10,infoPercentage_y) # Update gauge to 100%		

	def OnUpdate(self):	
		(mouseX, mouseY) = wndMgr.GetMousePosition()		
		
		if TRUE == self.sStatusEnable_a.IsIn():
			self.comp_1 = uitooltip.ToolTip()
			self.comp_1.SetPosition(mouseX+50+50+50+25, mouseY-25-25-25-25)
			self.comp_1.AppendDescription(REBORN_TOOLTIP_COMPLETE[0], None, REBORN_TOOLTIP_COLOR[0])	
			
		if TRUE == self.sStatusEnable_b.IsIn():
			self.comp_2 = uitooltip.ToolTip()
			self.comp_2.SetPosition(mouseX+50+50+50+25-40, mouseY-25-25-25-25)
			self.comp_2.AppendDescription(REBORN_TOOLTIP_COMPLETE[1], None, REBORN_TOOLTIP_COLOR[1])	
			
		if TRUE == self.sStatusEnable_c.IsIn():
			self.comp_3 = uitooltip.ToolTip()
			self.comp_3.SetPosition(mouseX+50+50+50+25-40-40, mouseY-25-25-25-25)
			self.comp_3.AppendDescription(REBORN_TOOLTIP_COMPLETE[2], None, REBORN_TOOLTIP_COLOR[2])	
			
		if TRUE == self.sStatusDisable_a.IsIn():
			self.necomp_1 = uitooltip.ToolTip()
			self.necomp_1.SetPosition(mouseX+50+50+50+25, mouseY-25-25-25-25)
			self.necomp_1.AppendDescription(REBORN_TOOLTIP_DISABLE[0], None, REBORN_TOOLTIP_COLOR[3])	
			
		if TRUE == self.sStatusDisable_b.IsIn():
			self.necomp_2 = uitooltip.ToolTip()
			self.necomp_2.SetPosition(mouseX+50+50+50+25-40, mouseY-25-25-25-25)
			self.necomp_2.AppendDescription(REBORN_TOOLTIP_DISABLE[1], None, REBORN_TOOLTIP_COLOR[3])	
			
		if TRUE == self.sStatusDisable_c.IsIn():
			self.necomp_3 = uitooltip.ToolTip()
			self.necomp_3.SetPosition(mouseX+50+50+50+25-40-40, mouseY-25-25-25-25)
			self.necomp_3.AppendDescription(REBORN_TOOLTIP_DISABLE[2], None, REBORN_TOOLTIP_COLOR[3])		

		k, v = player.GetReborn() # Get Function table actual reborn from src	
		
		self.slotBar_1[1].SetText(player.GetName())
		self.slotBar_2[1].SetText(str(player.GetStatus(player.LEVEL)))
			
		if constinfo.mStrAnonymous == 1:
			self.UpdateFunction_C()
			
		if k < 1: # If you not have active Reborn
			self.UpdateFunction_NoneType()
		if k == 1: # If you have active -> Reborn I
			self.UpdateFunction_A()
		if k == 2: # If you have active -> Reborn II
			self.UpdateFunction_B()
		if k == 3: # If you have active -> Reborn III
			self.UpdateFunction_C()
			
	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnPressExitKey(self):
		self.Close()
		return True