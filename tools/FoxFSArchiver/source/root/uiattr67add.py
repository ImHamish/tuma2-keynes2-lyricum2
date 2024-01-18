import ui
import uicommon
import uitooltip
import mousemodule
import uiscriptlocale
import localeinfo
import newconstinfo

import app
import item
import net
import player

MATERIAL_MAX_COUNT=10
ROOT_PATH="d:/ymir work/ui/game/attr6th7th/"
SUCCESS_PER_MATERIAL=2.0
SUPPORT_MAX_COUNT=5

class Attr67AddWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.is_loaded=False
		
		self.regist_slot=0
		self.regist_item_inven_pos=-1
		
		self.material_up_button=0
		self.material_down_button=0
		self.material_count_text=0
		self.material_slot=0
		self.material_item_vnum=-1
		self.material_item_count=0
		
		self.support_up_button=0
		self.support_down_button=0
		self.support_count_text=0
		self.support_slot=0
		self.support_item_inven_pos=-1
		self.support_item_vnum=-1
		self.support_item_count=0
		self.support_increase_per=0
		
		self.attr_add_button=0
		
		self.total_success_text=0
		self.total_success_per=0.0
		
		self.question_button=0
		self.question_button_tooltip=0
		self.question_button_tooltip_list=[\
			localeinfo.ATTR_6TH_7TH_DESC1,
			localeinfo.ATTR_6TH_7TH_DESC2,
			localeinfo.ATTR_6TH_7TH_DESC3,
			localeinfo.ATTR_6TH_7TH_DESC4
		]
		
		self.attr_add_question_dlg=uicommon.QuestionDialog2()
		self.attr_add_question_dlg.Close()
		
		self.start_pos_x=0
		self.start_pos_y=0
		
		self.item_tooltip=uitooltip.ItemToolTip()
		
		self.__LoadWindow()

	def __del__(self):
		self.Destroy()
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		try:
			self.__LoadScript("attr67adddialog.py")
		except:
			import exception
			exception.Abort("Attr67AddWindow.__LoadScript")
		
		try:
			self.__BindObject()
		except:
			import exception
			exception.Abort("Attr67AddWindow.__BindObject")
		
		try:
			self.__BindEvent()
		except:
			import exception
			exception.Abort("Attr67AddWindow.__BindEvent")
		
		self.is_loaded=True

	def __LoadScript(self,fileName):
		pyScrLoader=ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self,"UIScript/"+fileName)

	def __BindObject(self):
		self.material_up_button=self.GetChild("material_slot_arrow_up_button")
		self.material_down_button=self.GetChild("material_slot_arrow_down_button")
		self.material_count_text=self.GetChild("material_slot_count_text")
		
		self.support_up_button=self.GetChild("support_slot_arrow_up_button")
		self.support_down_button=self.GetChild("support_slot_arrow_down_button")
		self.support_count_text=self.GetChild("support_slot_count_text")
		
		self.attr_add_button=self.GetChild("attr_add_button")
		
		self.regist_slot=self.GetChild("regist_slot")
		
		self.material_slot=self.GetChild("material_slot")
		
		self.support_slot=self.GetChild("support_slot")
		
		self.total_success_text=self.GetChild("TotalSuccessText")

	def __BindEvent(self):
		self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))
		
		if self.material_up_button:
			self.material_up_button.SetEvent(ui.__mem_func__(self.ClickMaterialButton),True)
		
		if self.material_down_button:
			self.material_down_button.SetEvent(ui.__mem_func__(self.ClickMaterialButton),False)
		
		if self.material_slot:
			self.material_slot.SetOverInItemEvent(ui.__mem_func__(self.OverInMaterialSlot))
			self.material_slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		
		if self.support_up_button:
			self.support_up_button.SetEvent(ui.__mem_func__(self.ClickSuppotButton),True)
		
		if self.support_down_button:
			self.support_down_button.SetEvent(ui.__mem_func__(self.ClickSuppotButton),False)
		
		if self.support_slot:
			self.support_slot.SetOverInItemEvent(ui.__mem_func__(self.OverInSupportSlot))
			self.support_slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		
		if self.regist_slot:
			self.regist_slot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptyRegistSlot))
			self.regist_slot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemRegistSlot))
			self.regist_slot.SetOverInItemEvent(ui.__mem_func__(self.OverInRegistSlot))
			self.regist_slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		
		if self.attr_add_button:
			self.attr_add_button.SetEvent(ui.__mem_func__(self.__ClickAttrAddButton))
		
		if self.total_success_text:
			self.total_success_text.SetText(localeinfo.ATTR_6TH_7TH_TOTAL_SUCCESS_PERCENT % self.total_success_per)
		
		if self.support_slot:
			self.support_slot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySupportSlot))
			self.support_slot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSupportSlot))
		
		self.question_button=self.GetChild("question_button")
		self.question_button_tooltip=self.__CreateGameTypeToolTip("",self.question_button_tooltip_list)
		self.question_button_tooltip.SetTop()
		self.question_button.SetToolTipWindow(self.question_button_tooltip)

	def ClickMaterialButton(self,is_up_button_click):
		if self.material_item_vnum < 0:
			return
		
		count=self.material_item_count
		if is_up_button_click:
			if count < MATERIAL_MAX_COUNT:
				if count < player.GetItemCountByVnum(self.material_item_vnum):
					count += 1
		else:
			count -= 1
		
		self.material_item_count=max(0,count)
		self.__CalculateTotalSuccessPer()

	def ClickSuppotButton(self,is_up_button_click):
		if self.support_item_vnum < 0:
			return
		
		if self.support_item_inven_pos < 0:
			return
		
		count=self.support_item_count
		if is_up_button_click:
			if count < SUPPORT_MAX_COUNT:
				if count < player.GetItemCountByVnum(self.support_item_vnum):
					count += 1
		else:
			count -= 1
		
		self.support_item_count=max(0,count)
		self.__CalculateTotalSuccessPer()

	def CantAttachToAttrSlot(self,inven_slot):
		itemIndex=player.GetItemIndex(inven_slot)
		if itemIndex:
			attrCount=0
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				if player.GetItemAttribute(inven_slot,i)[0] != 0:
					attrCount += 1
			
			if attrCount < player.ATTRIBUTE_SLOT_MAX_NUM-2 or attrCount>=player.ATTRIBUTE_SLOT_MAX_NUM:
				return True
		
		return False

	if app.WJ_ENABLE_TRADABLE_ICON:
		def OnTop(self):
			if self.regist_item_inven_pos>=0:
				return
			
			interface=newconstinfo._interface_instance
			if interface:
				interface.SetOnTopWindow(player.ON_TOP_WND_ATTR67)
				interface.RefreshMarkInventoryBag()

		def __SetOnTopWindowNone(self):
			interface=newconstinfo._interface_instance
			if interface:
				interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				interface.RefreshMarkInventoryBag()

	def Close(self):
		if self.item_tooltip:
			self.item_tooltip.HideToolTip()
		
		if self.attr_add_question_dlg:
			self.attr_add_question_dlg.Close()
		
		self.__ClearData()
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.__SetOnTopWindowNone()
		
		net.SendAttr67ClosePacket()
		self.Hide()

	def Destroy(self):
		self.ClearDictionary()
		
		self.is_loaded=False
		
		self.regist_slot=0
		self.regist_item_inven_pos=-1
		
		self.material_up_button=0
		self.material_down_button=0
		self.material_count_text=0
		self.material_slot=0
		self.material_item_vnum=-1
		self.material_item_count=0
		
		self.support_up_button=0
		self.support_down_button=0
		self.support_count_text=0
		self.support_slot=0
		self.support_item_inven_pos=-1
		self.support_item_vnum=-1
		self.support_item_count=0
		self.support_increase_per=0
		
		self.attr_add_button=0
		
		self.total_success_text=0
		self.total_success_per=0.0
		
		self.question_button=0
		self.question_button_tooltip=0
		self.question_button_tooltip_list=[]
		
		self.attr_add_question_dlg=0
		
		self.start_pos_x=0
		self.start_pos_y=0
		
		self.item_tooltip=0

	def OnPressEscapeKey(self):
		self.Close()

	def OnUpdate(self):
		self.__RangeCheck()
		#if app.WJ_ENABLE_TRADABLE_ICON:
		#	self.__CheckCantMouseEvent()

	def OverInMaterialSlot(self,slot_index):
		if self.item_tooltip and self.material_item_vnum>=0:
			self.item_tooltip.SetItemToolTip(self.material_item_vnum)

	def OverInRegistSlot(self,slot_index):
		if self.item_tooltip and self.regist_item_inven_pos>=0:
			self.item_tooltip.SetInventoryItem(self.regist_item_inven_pos)

	def OverInSupportSlot(self,slot_index):
		if self.item_tooltip and self.support_item_inven_pos>=0:
			self.item_tooltip.SetInventoryItem(self.support_item_inven_pos)

	def OverOutItem(self):
		if self.item_tooltip:
			self.item_tooltip.HideToolTip()

	def SelectEmptyRegistSlot(self,slot_index):
		if not mousemodule.mouseController.isAttached():
			return
		
		attachedSlotType=mousemodule.mouseController.GetAttachedType()
		attachedSlotPos=mousemodule.mouseController.GetAttachedSlotNumber()
		attachedItemVnum=player.GetItemIndex(attachedSlotPos)
		if not attachedItemVnum:
			return
		
		item.SelectItem(attachedItemVnum)
		
		if player.SLOT_TYPE_INVENTORY != attachedSlotType:
			mousemodule.mouseController.DeattachObject()
			return
		
		if attachedSlotPos>=player.ITEM_SLOT_COUNT:
			mousemodule.mouseController.DeattachObject()
			return
		
		itemType=item.GetItemType()
		
		if not itemType in (item.ITEM_TYPE_WEAPON,item.ITEM_TYPE_ARMOR):
			mousemodule.mouseController.DeattachObject()
			return
		
		if itemType==item.ITEM_TYPE_WEAPON:
			subTypes=[\
						item.WEAPON_SWORD,
						item.WEAPON_DAGGER,
						item.WEAPON_BOW,
						item.WEAPON_TWO_HANDED,
						item.WEAPON_BELL,
						item.WEAPON_FAN,
			]
			
			if item.GetItemSubType() not in subTypes:
				mousemodule.mouseController.DeattachObject()
				return
		
		if self.CantAttachToAttrSlot(attachedSlotPos):
			mousemodule.mouseController.DeattachObject()
			return
		
		mousemodule.mouseController.DeattachObject()
		
		if self.regist_slot:
			self.regist_item_inven_pos=attachedSlotPos
			self.regist_slot.SetItemSlot(slot_index,attachedItemVnum)
			self.regist_slot.RefreshSlot()
		
		if self.material_slot:
			item.SelectItem(attachedItemVnum)
			self.material_item_vnum=item.GetAttr67MaterialVnum()
			self.material_slot.SetItemSlot(slot_index,self.material_item_vnum)
			self.material_slot.RefreshSlot()
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.__SetOnTopWindowNone()

	def SelectItemRegistSlot(self,slot_index):
		if mousemodule.mouseController.isAttached():
			return
		
		self.__ClearData()
		self.SetTop()

	def SelectEmptySupportSlot(self,slot_index):
		if not mousemodule.mouseController.isAttached():
			return
		
		attachedSlotType=mousemodule.mouseController.GetAttachedType()
		attachedSlotPos=mousemodule.mouseController.GetAttachedSlotNumber()
		attachedItemVnum=player.GetItemIndex(attachedSlotPos)
		item.SelectItem(attachedItemVnum)
		
		if attachedItemVnum < 72064 or attachedItemVnum > 72067:
			return
		
		mousemodule.mouseController.DeattachObject()

		if self.support_slot:
			self.support_item_inven_pos=attachedSlotPos
			self.support_item_vnum=attachedItemVnum
			self.support_item_count=1
			self.support_increase_per=item.GetValue(1)
			self.support_slot.SetItemSlot(slot_index,attachedItemVnum)
			self.support_slot.RefreshSlot()
		
		self.__CalculateTotalSuccessPer()

	def SelectItemSupportSlot(self,slot_index):
		if mousemodule.mouseController.isAttached():
			return
		
		self.__ClearSupportSlot()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.__SetOnTopWindowNone()

	def Show(self):
		if not self.is_loaded:
			self.__LoadWindow()
		
		(self.start_pos_x,self.start_pos_y,z)=player.GetMainCharacterPosition()
		
		net.SendAttr67AddPacket()
		self.SetCenterPosition()
		self.SetTop()
		
		ui.ScriptWindow.Show(self)

	def __CalculateTotalSuccessPer(self):
		if self.material_count_text:
			self.material_count_text.SetText(str(self.material_item_count))
		
		if self.support_count_text:
			self.support_count_text.SetText(str(self.support_item_count))
		
		self.total_success_per=float((self.material_item_count*SUCCESS_PER_MATERIAL)+(self.support_increase_per / SUPPORT_MAX_COUNT)*self.support_item_count)
		
		if self.total_success_text:
			self.total_success_text.SetText(localeinfo.ATTR_6TH_7TH_TOTAL_SUCCESS_PERCENT % self.total_success_per)

	#if app.WJ_ENABLE_TRADABLE_ICON:
	#	def __CheckCantMouseEvent(self):
	#		interface=newconstinfo._interface_instance
	#		if interface:
	#			inven=interface.wndInventory
	#			if not inven:
	#				return
	#			
	#			page=inven.GetInventoryPageIndex()
	#			min_range=page*player.INVENTORY_PAGE_SIZE
	#			max_range=(page+1)*player.INVENTORY_PAGE_SIZE
	#			
	#			if self.regist_item_inven_pos>=0:
	#				if (min_range)<=self.regist_item_inven_pos < (max_range):
	#					lock_idx=self.regist_item_inven_pos-(min_range)
	#					inven.wndItem.SetCantMouseEventSlot(lock_idx)
	#			
	#			if self.support_item_inven_pos>=0:
	#				if (min_range)<=self.support_item_inven_pos < (max_range):
	#					lock_idx=self.support_item_inven_pos-(min_range)
	#					inven.wndItem.SetCantMouseEventSlot(lock_idx)

	def __ClearData(self):
		self.__ClearRegistSlot()
		self.__ClearMaterialSlot()
		self.__ClearSupportSlot()
		self.__CalculateTotalSuccessPer()

	def __ClearRegistSlot(self):
		self.regist_item_inven_pos=-1
		if self.regist_slot:
			for i in xrange(self.regist_slot.GetSlotCount()):
				self.regist_slot.ClearSlot(i)
			
			self.regist_slot.RefreshSlot()

	def __ClearMaterialSlot(self):
		self.material_item_vnum=-1
		self.material_item_count=0
		if self.material_slot:
			for i in xrange(self.material_slot.GetSlotCount()):
				self.material_slot.ClearSlot(i)
			
			self.material_slot.RefreshSlot()
		
		self.__CalculateTotalSuccessPer()

	def __ClearSupportSlot(self):
		self.support_item_inven_pos=-1
		self.support_item_count=0
		if self.support_slot:
			for i in xrange(self.support_slot.GetSlotCount()):
				self.support_slot.ClearSlot(i)
			
			self.support_slot.RefreshSlot()
		
		self.__CalculateTotalSuccessPer()

	def __ClickAttrAddButton(self):
		if self.regist_item_inven_pos < 0:
			return
		
		if self.material_item_count<=0:
			return
		
		if self.attr_add_question_dlg:
			self.attr_add_question_dlg.SetText1(localeinfo.ATTR_6TH_7TH_ADD_QUESTION1)
			self.attr_add_question_dlg.SetText2(localeinfo.ATTR_6TH_7TH_ADD_QUESTION2)
			self.attr_add_question_dlg.SetAcceptEvent(ui.__mem_func__(self.__SendAttr67AddPacket))
			self.attr_add_question_dlg.SetCancelEvent(ui.__mem_func__(self.attr_add_question_dlg.Close))
			self.attr_add_question_dlg.Open()

	def __CreateGameTypeToolTip(self,title,descList):
		toolTip=uitooltip.ToolTip()
		#toolTip.SetTitle(title)
		toolTip.AppendSpace(7)
		
		for desc in descList:
			toolTip.AutoAppendTextLine(desc)
		
		toolTip.AlignHorizonalCenter()
		toolTip.SetTop()
		return toolTip

	def __RangeCheck(self):
		USE_LIMIT_RANGE=1000
		(x,y,z)=player.GetMainCharacterPosition()
		if abs(x-self.start_pos_x) > USE_LIMIT_RANGE or abs(y-self.start_pos_y) > USE_LIMIT_RANGE:
			self.Close()

	def __SendAttr67AddPacket(self):
		if self.attr_add_question_dlg:
			self.attr_add_question_dlg.Close()
		
		net.SendAttr67AddPacket(self.regist_item_inven_pos,self.material_item_count,self.support_item_inven_pos,self.support_item_count)
		self.Close()
