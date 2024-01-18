import app
import net
import player
import ui
import mousemodule
import snd
import item
import chat
import grp
import uiscriptlocale
import uirefine
import uiattachmetin
import uipickmoney
import uicommon
import uiprivateshopbuilder
import localeinfo
import constinfo
import ime
import wndMgr
import uitooltip
import changelook

if app.WJ_ENABLE_TRADABLE_ICON:
	INVENTORY_PAGE_SIZE = player.INVENTORY_PAGE_SIZE

class ChangeLookWindow(ui.ScriptWindow):
	USE_CHANGELOOKWINDOW_LIMIT_RANGE = 500
	CHANGELOOK_SLOT_LEFT = 0
	CHANGELOOK_SLOT_RIGHT = 1
	CHANGELOOK_SLOT_MAX = 2
	CHANGELOOK_COST = 100000000

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.tooltipitem = None
		self.xChangeLookWindowStart = 0
		self.yChangeLookWindowStart = 0
		self.ChangeLookToolTIpButton = None
		self.ChangeLookToolTip = None
		self.popup = None
		
		self.ChangeLookCost = None
		self.ChangeLookToolTipList = [
			localeinfo.CHANGE_TOOLTIP_LINE1,
			localeinfo.CHANGE_TOOLTIP_LINE2,
			localeinfo.CHANGE_TOOLTIP_LINE3,
			localeinfo.CHANGE_TOOLTIP_LINE4,
			localeinfo.CHANGE_TOOLTIP_LINE5,
			localeinfo.CHANGE_TOOLTIP_LINE6
		]
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.ItemListIdx = []

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.isLoaded = 0
		self.tooltipitem = None
		self.pop = None
		self.ChangeLookToolTIpButton = None
		self.ChangeLookToolTip = None
		self.ChangeLookCost = None
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.ItemListIdx = []

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/changelookwindow.py")
		except:
			import exception
			exception.Abort("ChangeLookWindow.__LoadWindow.UIScript/ChangeLookWindow.py")
		
		try:
			wnditem = self.GetChild("ChangeLookSlot")
			
			self.GetChild("CancelButton").SetEvent(ui.__mem_func__(self.Close))
			self.GetChild("AcceptButton").SetEvent(ui.__mem_func__(self.Accept))
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			
			self.ChangeLookCost = self.GetChild("Cost")
			self.ChangeLookCost.SetText(localeinfo.CHANGE_LOOK_COST % (localeinfo.NumberToMoneyString(self.CHANGELOOK_COST)))
			
			self.GetChild("TitleName").SetText(localeinfo.CHANGE_LOOK_TITLE)
		except:
			import exception
			exception.Abort("ChangeLookWindow.__LoadWindow.ChangeLookSlot")
		
		wnditem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wnditem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		wnditem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wnditem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wnditem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wnditem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wnditem.Show()
		
		self.wnditem = wnditem
		
		self.ChangeLookToolTIpButton = self.GetChild("ChangeLookToolTIpButton")
		self.ChangeLookToolTip = self.__CreateGameTypeToolTip(localeinfo.CHANGE_TOOLTIP_TITLE, self.ChangeLookToolTipList)
		self.ChangeLookToolTip.SetTop()
		self.ChangeLookToolTIpButton.SetToolTipWindow(self.ChangeLookToolTip)

	def __CreateGameTypeToolTip(self, title, descList):
		toolTip = uitooltip.ToolTip()
		toolTip.SetTitle(title)
		toolTip.AppendSpace(7)
		
		for desc in descList:
			toolTip.AutoAppendTextLine(desc)
		
		toolTip.AlignHorizonalCenter()
		toolTip.SetTop()
		return toolTip

	def OnUpdate(self):
		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xChangeLookWindowStart) > changelook.LIMIT_RANGE or abs(y - self.yChangeLookWindowStart) > changelook.LIMIT_RANGE:
			self.Close()
		
		for i in xrange(changelook.WINDOW_MAX_MATERIALS):
			(isHere, iCell) = changelook.GetAttachedItem(i)
			if isHere:
				if changelook.IsTransmutationTicket(player.GetItemIndex(iCell)):
					self.ChangeLookCost.SetText(localeinfo.CHANGE_LOOK_COST % (localeinfo.NumberToMoneyString(0)))
				else:
					self.ChangeLookCost.SetText(localeinfo.CHANGE_LOOK_COST % (localeinfo.NumberToMoneyString(changelook.GetCost())))
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.ChangeLookItemMouseEvent()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Open(self):
		if self.isLoaded == 0:
			self.isLoaded = 1
			self.__LoadWindow()
		
		self.SetCenterPosition()
		ui.ScriptWindow.Show(self)
		(self.xChangeLookWindowStart, self.yChangeLookWindowStart, z) = player.GetMainCharacterPosition()
		self.RefreshChangeLookWindow()
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			try:
				import newconstinfo
				newconstinfo._interface_instance.SetOnTopWindow(player.ON_TOP_WND_CHANGE_LOOK)
				newconstinfo._interface_instance.RefreshMarkInventoryBag()
			except Exception as e:
				import dbg
				dbg.TraceError(str(e))

	def Close(self):
		self.Hide()
		changelook.SendCloseRequest()
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			try:
				import newconstinfo
				newconstinfo._interface_instance.SetOnTopWindow(player.ON_TOP_WND_NONE)
				newconstinfo._interface_instance.RefreshMarkInventoryBag()
			except Exception as e:
				import dbg
				dbg.TraceError(str(e))
			
			self.ItemListIdx = None

	def Accept(self):
		(isHere, iCell) = changelook.GetAttachedItem(1)
		if not isHere:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CHANGE_LOOK_INSERT_ITEM)
		else:
			popup = uicommon.QuestionDialog()
			popup.SetText(localeinfo.CHANGE_LOOK_CHANGE_ITEM)
			popup.SetAcceptEvent(self.SendAccept)
			popup.SetCancelEvent(self.OnCloseEvent)
			popup.Open()
			self.pop = popup

	def SendAccept(self):
		self.pop.Close()
		self.pop = None
		changelook.SendRefineRequest()

	def SetItemToolTip(self, tooltip):
		self.tooltipitem = tooltip

	def __ShowToolTip(self, slotIndex):
		if self.tooltipitem:
			(isHere, iCell) = changelook.GetAttachedItem(slotIndex)
			if isHere:
				self.tooltipitem.SetInventoryItem(iCell)

	def OverInItem(self, slotIndex):
		self.wnditem.SetUsableItem(False)
		self.__ShowToolTip(slotIndex)

	def OverOutItem(self):
		self.wnditem.SetUsableItem(False)
		if self.tooltipitem:
			self.tooltipitem.HideToolTip()

	def OnCloseEvent(self):
		self.pop.Close()
		self.pop = None

	def __OnClosePopupDialog(self):
		self.popup = None

	def IsOpened(self):
		if self.IsShow() and self.isLoaded:
			return True
		
		return False

	def RefreshChangeLookWindow(self):
		for i in xrange(changelook.WINDOW_MAX_MATERIALS):
			self.wnditem.ClearSlot(i)
		
		self.wnditem.RefreshSlot()

	def SelectEmptySlot(self, selectedSlotPos):
		if mousemodule.mouseController.isAttached():
			attachedSlotType = mousemodule.mouseController.GetAttachedType()
			attachedSlotPos = mousemodule.mouseController.GetAttachedSlotNumber()
			attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
			ItemVNum = player.GetItemIndex(attachedSlotPos)
			
			(isLeft, iLeftCell) = changelook.GetAttachedItem(self.CHANGELOOK_SLOT_LEFT)
			
			if selectedSlotPos == self.CHANGELOOK_SLOT_MAX:
				if not changelook.IsTransmutationTicket(ItemVNum):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CHANGE_LOOK_DO_NOT_REGISTER_ITEM)
					return
			
			if selectedSlotPos > self.CHANGELOOK_SLOT_LEFT:
				if not isLeft:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CHANGE_LOOK_INSERT_CHANGE_LOOK_ITEM)
					return
			
			if selectedSlotPos < self.CHANGELOOK_SLOT_MAX:
				if player.GetItemTransmutation(attachedSlotPos):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CHANGE_ALREADY_REGISTER)
					return
				
				if not item.IsPossibleChangeLook(ItemVNum):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CHANGE_LOOK_DO_NOT_CHANGE_LOOK_ITEM)
					return
			
			if attachedSlotPos > player.EQUIPMENT_SLOT_START - 1:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CHANGE_LOOK_DO_NOT_EQUIP_ITEM)
				return
			
			if attachedSlotType == player.SLOT_TYPE_INVENTORY and attachedInvenType == player.INVENTORY:
				if selectedSlotPos == self.CHANGELOOK_SLOT_LEFT:
					changelook.Add(attachedInvenType, attachedSlotPos, selectedSlotPos)
				else:
					if iLeftCell == attachedSlotPos:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CHANGE_ALREADY_REGISTER)
						return
					
					changelook.Add(attachedInvenType, attachedSlotPos, selectedSlotPos)
			
			mousemodule.mouseController.DeattachObject()

	def UseItemSlot(self, slotIndex):
		(isHere, iCell) = changelook.GetAttachedItem(slotIndex)
		if slotIndex == self.CHANGELOOK_SLOT_LEFT:
			(isRight, iRightCell) = changelook.GetAttachedItem(self.CHANGELOOK_SLOT_RIGHT)
			if not isRight:
				mousemodule.mouseController.DeattachObject()
				changelook.Remove(slotIndex)
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CHANGE_LOOK_CHECK_OUT_REGISTER_ITEM)
		else:
			mousemodule.mouseController.DeattachObject()
			changelook.Remove(slotIndex)

	def SelectItemSlot(self, selectedSlotPos):
		if mousemodule.mouseController.isAttached():
			attachedSlotType = mousemodule.mouseController.GetAttachedType()
			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				attachedSlotPos = mousemodule.mouseController.GetAttachedSlotNumber()
				snd.PlaySound("sound/ui/drop.wav")
			
			mousemodule.mouseController.DeattachObject()
		else:
			curCursorNum = app.GetCursor()
			if app.SELL == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.SAFEBOX_SELL_DISABLE_SAFEITEM)
			elif app.BUY == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.SHOP_BUY_INFO)
			else:
				(isHere, iCell) = changelook.GetAttachedItem(selectedSlotPos)
				if selectedSlotPos == self.CHANGELOOK_SLOT_LEFT:
					(isRight, iRightCell) = changelook.GetAttachedItem(self.CHANGELOOK_SLOT_RIGHT)
					if not isRight:
						selectedItemID = player.GetItemIndex(iCell)
						mousemodule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, selectedSlotPos, selectedItemID)
						snd.PlaySound("sound/ui/pick.wav")
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.CHANGE_LOOK_CHECK_OUT_REGISTER_ITEM)
				else:
					selectedItemID = player.GetItemIndex(iCell)
					mousemodule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, selectedSlotPos, selectedItemID)
					snd.PlaySound("sound/ui/pick.wav")

	def Refresh(self):
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.ItemListIdx = []
		
		for i in xrange(changelook.WINDOW_MAX_MATERIALS):
			self.wnditem.ClearSlot(i)
			(isHere, iCell) = changelook.GetAttachedItem(i)
			if isHere:
				self.wnditem.SetItemSlot(i, player.GetItemIndex(iCell), 1)
				if app.WJ_ENABLE_TRADABLE_ICON:
					self.ItemListIdx.append(iCell)

	if app.WJ_ENABLE_TRADABLE_ICON:
		def CantTradableItem(self, slotIndex):
			itemIndex = player.GetItemIndex(slotIndex)
			if itemIndex:
				if not item.IsPossibleChangeLook(itemIndex):
					return True
			
			return False

		def OnTop(self):
			self.tooltipitem.SetTop()
			try:
				import newconstinfo
				newconstinfo._interface_instance.SetOnTopWindow(player.ON_TOP_WND_CHANGE_LOOK)
				newconstinfo._interface_instance.RefreshMarkInventoryBag()
			except Exception as e:
				import dbg
				dbg.TraceError(str(e))

		def ChangeLookItemMouseEvent(self):
			if not self.ItemListIdx:
				return
			
			pass
			#page = self.inven.GetInventoryPageIndex()
			#for i in self.ItemListIdx:
			#	if (page * INVENTORY_PAGE_SIZE) <= i < ((page + 1) * INVENTORY_PAGE_SIZE): # range 0 ~ 44, 45 ~ 89
			#		lock_idx = i - (page * INVENTORY_PAGE_SIZE)
			#		self.inven.wndItem.SetCantMouseEventSlot(lock_idx)
			#
			#	if app.ENABLE_SPECIAL_INVENTORY_SYSTEM:
			#		if self.invenSpecial.GetInventoryType() == 0:
			#			if (item.SKILL_BOOK_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (item.SKILL_BOOK_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
			#				lock_idx = i - (item.SKILL_BOOK_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
			#				self.invenSpecial.wndItem.SetCantMouseEventSlot(lock_idx)
			#
			#		if self.invenSpecial.GetInventoryType() == 1:
			#			if (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
			#				lock_idx = i - (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
			#				self.invenSpecial.wndItem.SetCantMouseEventSlot(lock_idx)
			#
			#		if self.invenSpecial.GetInventoryType() == 2:
			#			if (item.STONE_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (item.STONE_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
			#				lock_idx = i - (item.STONE_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
			#				self.invenSpecial.wndItem.SetCantMouseEventSlot(lock_idx)
			#
			#		if self.invenSpecial.GetInventoryType() == 3:
			#			if (item.GIFT_BOX_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (item.GIFT_BOX_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
			#				lock_idx = i - (item.GIFT_BOX_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
			#				self.invenSpecial.wndItem.SetCantMouseEventSlot(lock_idx)
