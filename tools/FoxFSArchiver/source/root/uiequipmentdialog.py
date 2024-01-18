import app
import chr
import item
import player
import wndMgr

import ui

class CostumeEquipmentDialog(ui.ScriptWindow):
	availCostumeSlots = [item.COSTUME_SLOT_START-player.EQUIPMENT_SLOT_START,\
							item.COSTUME_SLOT_START+1-player.EQUIPMENT_SLOT_START,\
							item.COSTUME_SLOT_START+3-player.EQUIPMENT_SLOT_START,\
							item.COSTUME_SLOT_WEAPON-player.EQUIPMENT_SLOT_START,\
							player.COSTUME_PETSKIN_SLOT-player.EQUIPMENT_SLOT_START,\
							player.COSTUME_MOUNTSKIN_SLOT-player.EQUIPMENT_SLOT_START,\
							item.COSTUME_SLOT_AURA-player.EQUIPMENT_SLOT_START]

	def __init__(self,wndEquipment):
		if not wndEquipment:
			import exception
			exception.Abort("wndEquipment parameter must be set to CostumeEquipmentDialog")
			return
		
		ui.ScriptWindow.__init__(self)
		self.isLoaded=0
		
		self.wndEquipment=wndEquipment
		self.wndPrimaryEquip=0
		
		self.__LoadDialog()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Open(self):
		self.RefreshSlot()
		self.Show()

	def Close(self):
		self.Hide()

	def Destroy(self):
		self.Close()
		
		self.ClearDictionary()
		
		self.titleBar=0
		self.wndPrimaryEquip=0

	def __LoadDialog(self):
		if self.isLoaded == 1:
			return
		
		self.isLoaded=1
		
		try:
			pyScrLoader=ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self,"uiscript/costumeequipmentdialog.py")
		except:
			import exception
			exception.Abort("CostumeEquipmentDialog.LoadWindow.LoadObject")
		
		try:
			titleBar=self.GetChild("TitleBar")
			titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
			self.titleBar=titleBar
			
			wndPrimaryEquip=self.GetChild("costume_slot")
			wndPrimaryEquip.SetOverInItemEvent(ui.__mem_func__(self.wndEquipment.OverInItem))
			wndPrimaryEquip.SetOverOutItemEvent(ui.__mem_func__(self.wndEquipment.OverOutItem))
			self.wndPrimaryEquip=wndPrimaryEquip
		except:
			import exception
			exception.Abort("CostumeEquipmentDialog.LoadWindow.BindObject")
		
		self.Close()

	def RefreshSlot(self):
		equipmentDict=self.wndEquipment.itemDataDict
		
		for slotNum in self.availCostumeSlots:
			if equipmentDict.has_key(slotNum+player.EQUIPMENT_SLOT_START):
				self.wndPrimaryEquip.SetItemSlot(slotNum+player.EQUIPMENT_SLOT_START,\
												equipmentDict[slotNum+player.EQUIPMENT_SLOT_START][0],\
												equipmentDict[slotNum+player.EQUIPMENT_SLOT_START][1])

class EquipmentDialog(ui.ScriptWindow):
	availPrimarySlots = [0,1,2,3,4,5,6,9,10,\
							item.EQUIPMENT_BELT-player.EQUIPMENT_SLOT_START,\
							18,\
							item.COSTUME_SLOT_START+2-player.EQUIPMENT_SLOT_START]
	
	availSecondarySlots = [7,8,\
							player.COSTUME_EFFECT_BODY_SLOT-player.EQUIPMENT_SLOT_START,\
							player.COSTUME_EFFECT_WEAPON_SLOT-player.EQUIPMENT_SLOT_START]

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()
		
		self.vid=0
		self.eventClose=0
		self.itemDataDict={}
		self.tooltipItem=0
		self.wndCostume=CostumeEquipmentDialog(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		try:
			PythonScriptLoader=ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self,"uiscript/equipmentdialog.py")
			
			wndPrimaryEquip=self.GetChild("primary_slot")
			wndPrimaryEquip.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			wndPrimaryEquip.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			self.wndPrimaryEquip=wndPrimaryEquip
			
			wndSecondaryEquip=self.GetChild("secondary_slot")
			wndSecondaryEquip.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			wndSecondaryEquip.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			self.wndSecondaryEquip=wndSecondaryEquip
			
			self.costumeButton=self.GetChild("costume_button")
			self.costumeButton.SetEvent(ui.__mem_func__(self.OnPressCostumeButton))
		except:
			import exception
			exception.Abort("EquipmentDialog.LoadDialog.BindObject")

	def Open(self,vid):
		self.vid=vid
		self.itemDataDict={}
		
		self.Show()

	def Close(self,bPass=1):
		self.itemDataDict={}
		self.tooltipItem=0
		
		self.Hide()
		
		if self.wndCostume and\
			self.wndCostume.IsShow():
			self.wndCostume.Close()
		
		if bPass==1 and self.eventClose!=0:
			self.eventClose(self.vid)

	def Destroy(self):
		self.Close(0)
		
		self.ClearDictionary()
		
		self.wndPrimaryEquip=0
		self.wndSecondaryEquip=0
		
		if self.wndCostume:
			self.wndCostume.Destroy()
			del self.wndCostume
			self.wndCostume=0
		
		self.costumeButton=0
		
		self.eventClose=0

	def SetItemToolTip(self,tooltipItem):
		self.tooltipItem=tooltipItem

	def SetCloseEvent(self,event):
		self.eventClose=event

	def OnPressCostumeButton(self):
		if self.wndCostume.IsShow():
			self.wndCostume.Close()
		else:
			self.wndCostume.Open()

	def SetEquipmentDialogItem(self,slotIndex,vnum,count):
		if count<=1:
			count=0
		
		if vnum==0:
			bDone=1
		else:
			bDone=0
			for slotNum in self.availPrimarySlots:
				if slotNum==slotIndex:
					self.wndPrimaryEquip.SetItemSlot(player.EQUIPMENT_SLOT_START+slotIndex,vnum,count)
					bDone=1
					break
			
			if bDone==0:
				for slotNum in self.availSecondarySlots:
					if slotNum==slotIndex:
						self.wndSecondaryEquip.SetItemSlot(player.EQUIPMENT_SLOT_START+slotIndex,vnum,count)
						bDone=1
						break
			
			if bDone==0:
				for slotNum in self.wndCostume.availCostumeSlots:
					if slotNum==slotIndex:
						bDone=1
		
		if bDone==1:
			socketList=[]
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				socketList.append(0)
			
			attrList=[]
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				attrList.append((0,0))
			
			self.itemDataDict[player.EQUIPMENT_SLOT_START+slotIndex]=(vnum,count,socketList,attrList)

	def SetEquipmentDialogSocket(self,slotIndex,socketIndex,value):
		slotIndex += player.EQUIPMENT_SLOT_START
		
		if not slotIndex in self.itemDataDict:
			return
		
		if socketIndex < 0 or socketIndex > player.METIN_SOCKET_MAX_NUM:
			return
		
		self.itemDataDict[slotIndex][2][socketIndex]=value

	def SetEquipmentDialogAttr(self,slotIndex,attrIndex,type,value):
		slotIndex += player.EQUIPMENT_SLOT_START
		
		if not slotIndex in self.itemDataDict:
			return
		
		if attrIndex < 0 or attrIndex > player.ATTRIBUTE_SLOT_MAX_NUM:
			return
		
		self.itemDataDict[slotIndex][3][attrIndex]=(type,value)

	def OverInItem(self,slotIndex):
		if self.tooltipItem==0:
			self.OverOutItem()
			return
		elif not slotIndex in self.itemDataDict:
			self.OverOutItem()
			return
		
		itemVnum=self.itemDataDict[slotIndex][0]
		if itemVnum==0:
			return
		
		self.tooltipItem.ClearToolTip()
		
		self.tooltipItem.AddItemData(itemVnum,self.itemDataDict[slotIndex][2],self.itemDataDict[slotIndex][3])
		self.tooltipItem.ShowToolTip()

	def OverOutItem(self):
		if self.tooltipItem!=0:
			self.tooltipItem.HideToolTip()

	def OnMoveWindow(self,x,y):
		if self.wndCostume!=0:
			self.wndCostume.AdjustPositionAndSize()
