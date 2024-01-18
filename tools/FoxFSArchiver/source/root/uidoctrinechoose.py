import app
import player
import net
import chat
import ui
import uicommon
import localeinfo
import mousemodule
import playersettingmodule

JOB_NAME_DICT={
					0 : [localeinfo.SKILL_SELECT_1A, localeinfo.SKILL_SELECT_1B],
					1 : [localeinfo.SKILL_SELECT_2A, localeinfo.SKILL_SELECT_2B],
					2 : [localeinfo.SKILL_SELECT_3A, localeinfo.SKILL_SELECT_3B],
					3 : [localeinfo.SKILL_SELECT_4A, localeinfo.SKILL_SELECT_4B],
}

class DoctrineWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.Initialize()
		self.LoadWindow()

	def __del__(self):
		self.ClearDictionary()
		self.Initialize()
		ui.ScriptWindow.__del__(self)

	def Initialize(self):
		self.selectedJob=1
		self.isLoaded=0
		self.titlebar=0
		self.skillSlot=[]
		self.acceptButton=[]
		self.questionDialog=0
		self.toolTipSkill=0

	def LoadWindow(self):
		if self.isLoaded==1:
			return
		
		self.isLoaded=1
		
		try:
			PythonScriptLoader=ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "uiscript/doctrinechoosewindow.py")
		except Exception, msg:
			import sys
			import dbg
			(type,msg,tb)=sys.exc_info()
			dbg.TraceError("DoctrineWindow.LoadWindow.Load-%s:%s"%(type,msg))
			app.Abort()
			return 0
		
		try:
			self.titlebar=self.GetChild("TitleBar")
			self.titlebar.SetCloseEvent(ui.__mem_func__(self.Close))
			
			for i in xrange(2):
				tmp=self.GetChild("doctrine%d_slot"%(i+1))
				if i==0:
					tmp.SetOverInItemEvent(ui.__mem_func__(self.OverInSlotLeft))
				else:
					tmp.SetOverInItemEvent(ui.__mem_func__(self.OverInSlotRight))
				
				tmp.SetOverOutItemEvent(ui.__mem_func__(self.OverOutSlot))
				
				self.skillSlot.append(tmp)
				
				tmp=self.GetChild("doctrine%d_button"%(i+1))
				tmp.SetText(JOB_NAME_DICT[self.GetRealRace()][0] if i == 0 else JOB_NAME_DICT[self.GetRealRace()][1])
				tmp.SAFE_SetEvent(self.OnAcceptQuestionDialog, i+1)
				self.acceptButton.append(tmp)
		except Exception, msg:
			import sys
			import dbg
			(type,msg,tb)=sys.exc_info()
			dbg.TraceError("DoctrineWindow.LoadWindow.Bind-%s:%s"%(type,msg))
			app.Abort()
			return 0
		
		for i in xrange(8):
			r=self.GetSkillIndex()
			if i == 6:
				if r==1:
					self.skillSlot[0].SetSkillSlotNew(i, 236, 3, 0)
					self.skillSlot[1].SetSkillSlotNew(i, 240, 3, 0)
				elif r==31:
					self.skillSlot[0].SetSkillSlotNew(i, 237, 3, 0)
					self.skillSlot[1].SetSkillSlotNew(i, 241, 3, 0)
				elif r==61:
					self.skillSlot[0].SetSkillSlotNew(i, 238, 3, 0)
					self.skillSlot[1].SetSkillSlotNew(i, 242, 3, 0)
				elif r==91:
					self.skillSlot[0].SetSkillSlotNew(i, 239, 3, 0)
					self.skillSlot[1].SetSkillSlotNew(i, 243, 3, 0)
			elif i == 7:
				if r==1:
					self.skillSlot[0].SetSkillSlotNew(i, 176, 3, 0)
					self.skillSlot[1].SetSkillSlotNew(i, 176, 3, 0)
				elif r==31:
					self.skillSlot[0].SetSkillSlotNew(i, 177, 3, 0)
					self.skillSlot[1].SetSkillSlotNew(i, 178, 3, 0)
				elif r==61:
					self.skillSlot[0].SetSkillSlotNew(i, 179, 3, 0)
					self.skillSlot[1].SetSkillSlotNew(i, 180, 3, 0)
				elif r==91:
					self.skillSlot[0].SetSkillSlotNew(i, 181, 3, 0)
					self.skillSlot[1].SetSkillSlotNew(i, 182, 3, 0)
			else:
				self.skillSlot[0].SetSkillSlotNew(i, r + i, 3, 0)
				self.skillSlot[1].SetSkillSlotNew(i, r + i + 15, 3, 0)

	def OnCloseQuestionDialog(self):
		if self.questionDialog:
			self.questionDialog.Close()
			del self.questionDialog
			self.questionDialog=0

	def OnSelectJob(self):
		net.SendChatPacket("/doctrine_choose %d" % self.selectedJob)
		self.OnCloseQuestionDialog()
		self.Close()

	def OnAcceptQuestionDialog(self, index):
		self.selectedJob=index
		
		if self.questionDialog:
			self.OnCloseQuestionDialog()
		
		questionDialog=uicommon.QuestionDialog()
		questionDialog.SetText(localeinfo.SKILL_SELECT_DIALOG2 % JOB_NAME_DICT[self.GetRealRace()][index-1])
		questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnSelectJob))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		questionDialog.Open()
		self.questionDialog=questionDialog

	def OverInSlotLeft(self, index):
		if self.toolTipSkill:
			if mousemodule.mouseController.isAttached():
				return
			
			self.toolTipSkill.ClearToolTip()
			
			r=self.GetSkillIndex()
			if index == 6:
				if r==1:
					self.toolTipSkill.SetSkillOnlyName(index, 236, 0)
				elif r==31:
					self.toolTipSkill.SetSkillOnlyName(index, 237, 0)
				elif r==61:
					self.toolTipSkill.SetSkillOnlyName(index, 238, 0)
				elif r==91:
					self.toolTipSkill.SetSkillOnlyName(index, 239, 0)
			elif index == 7:
				if r==1:
					self.toolTipSkill.SetSkillOnlyName(index, 176, 0)
				elif r==31:
					self.toolTipSkill.SetSkillOnlyName(index, 177, 0)
				elif r==61:
					self.toolTipSkill.SetSkillOnlyName(index, 179, 0)
				elif r==91:
					self.toolTipSkill.SetSkillOnlyName(index, 181, 0)
			else:
				self.toolTipSkill.SetSkillOnlyName(index, r+index, 0)

	def OverInSlotRight(self, index):
		if self.toolTipSkill:
			if mousemodule.mouseController.isAttached():
				return
			
			self.toolTipSkill.ClearToolTip()
			
			r=self.GetSkillIndex()
			if index == 6:
				if r==1:
					self.toolTipSkill.SetSkillOnlyName(index, 240, 0)
				elif r==31:
					self.toolTipSkill.SetSkillOnlyName(index, 241, 0)
				elif r==61:
					self.toolTipSkill.SetSkillOnlyName(index, 242, 0)
				elif r==91:
					self.toolTipSkill.SetSkillOnlyName(index, 243, 0)
			elif index == 7:
				if r==1:
					self.toolTipSkill.SetSkillOnlyName(index, 176, 0)
				elif r==31:
					self.toolTipSkill.SetSkillOnlyName(index, 178, 0)
				elif r==61:
					self.toolTipSkill.SetSkillOnlyName(index, 180, 0)
				elif r==91:
					self.toolTipSkill.SetSkillOnlyName(index, 182, 0)
			else:
				self.toolTipSkill.SetSkillOnlyName(index, r+index+15, 0)

	def OverOutSlot(self):
		if self.toolTipSkill:
			self.toolTipSkill.HideToolTip()

	def GetSkillIndex(self):
		r=self.GetRealRace()*30+1
		return r

	def GetRealRace(self):
		race=net.GetMainActorRace()
		if race>=4:
			return race-4
		else:
			return race

	def SelectJob(self):
		if self.selectedJob==0:
			self.wndPopupDialog.Open()
		else:
			self.questionDialog.Open()

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.OnCloseQuestionDialog()
		self.OverOutSlot()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def RefreshSkill(self):
		if self.IsShow():
			group=net.GetMainActorSkillGroup()
			if group==1 or group==2:
				self.Close()

	def SetSkillToolTip(self, toolTip):
		self.toolTipSkill=toolTip
