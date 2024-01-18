import app
import player
import net
import ui
import mousemodule
import snd
import item
import chat
import uicommon
import uiprivateshopbuilder
import constinfo
import ime
import uipickmoney
import localeinfo

if app.__ENABLE_NEW_OFFLINESHOP__:
	import offlineshop
	import uiofflineshop

class ExtraInventoryWindow(ui.ScriptWindow):
	if app.WJ_ENABLE_TRADABLE_ICON:
		bindWnds = []
		interface = None

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.wndDragonSoulRefine = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Open(self):
		self.Show()
		self.SetTop()
		self.SetCenterPosition()

	if app.WJ_ENABLE_TRADABLE_ICON:
		def BindWindow(self, wnd):
			self.bindWnds.append(wnd)

		def BindInterfaceClass(self, interface):
			self.interface = interface

	def LoadWindow(self, parent):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/extrainventory.py")
		except:
			import exception
			exception.Abort("ExtraInventoryWindow.LoadWindow.LoadObject")

		try:
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			# self.GetChild("RefreshButton").SetEvent(ui.__mem_func__(self.SortExtraInventory))
			self.GetChild("Mall_cat").SetEvent(ui.__mem_func__(self.ClickMallButton))
			self.GetChild("Safebox_cat").SetEvent(ui.__mem_func__(self.ClickSafeboxButton))
			
			self.wndItem = self.GetChild("ItemSlot")

			self.inventoryTab = []
			for x in xrange(player.EXTRA_INVENTORY_PAGE_COUNT / (player.EXTRA_INVENTORY_CATEGORY_COUNT)):
				self.inventoryTab.append(self.GetChild("Inventory_Tab_%02d" % (x + 1)))

			self.categoryTab = []
			for x in xrange(player.EXTRA_INVENTORY_CATEGORY_COUNT):
				self.categoryTab.append(self.GetChild("Cat_%02d" % x))
		except:
			import exception
			exception.Abort("ExtraInventoryWindow.LoadWindow.BindObject")
		
		self.wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		self.wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		self.wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		self.wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		self.wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.wndItem.SetOverInEmptySlotEvent(ui.__mem_func__(self.OverOutItem))
		self.wndItem.SetOverOutEmptySlotEvent(ui.__mem_func__(self.OverOutItem))
		
		for x in xrange(player.EXTRA_INVENTORY_PAGE_COUNT / (player.EXTRA_INVENTORY_CATEGORY_COUNT)):
			self.inventoryTab[x].SetEvent(lambda arg = x: self.SetInventoryPage(arg))
		self.inventoryTab[0].Down()

		for x in xrange(player.EXTRA_INVENTORY_CATEGORY_COUNT):
			self.categoryTab[x].SetEvent(lambda arg = x: self.SetCategory(arg))
		self.categoryTab[0].Down()

		self.inventoryPageIndex = 0
		self.category = 0
		self.sellingSlotNumber = -1
		self.questionDialog = None
		self.tooltipItem = None

		self.dlgPickMoney = uipickmoney.PickMoneyDialog()
		self.dlgPickMoney.LoadDialog()
		self.dlgPickMoney.Hide()
		if app.ENABLE_HIGHLIGHT_SYSTEM:
			self.listHighlightedSlot = []
			for i in xrange(player.EXTRA_INVENTORY_CATEGORY_COUNT):
				self.listHighlightedSlot.append([])
		
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoulRefine = parent
		
		self.SetInventoryPage(0)

	if app.ENABLE_HIGHLIGHT_SYSTEM:
		def HighlightSlot(self, slot):
			category = 0
			if slot >= 180 and slot < 360:
				category = 1
			if slot >= 360 and slot < 540:
				category = 2
			if slot >= 540 and slot < 720:
				category = 3
			if slot >= 720 and slot < 900:
				category = 4
			
			if not slot in self.listHighlightedSlot[category]:
				self.listHighlightedSlot[category].append(slot)

	def Destroy(self):
		self.ClearDictionary()
		self.tooltipItem = None
		self.wndItem = None
		self.inventoryTab = []
		self.categoryTab = []
		self.dlgPickMoney.Destroy()
		self.dlgPickMoney = None
		self.wndDragonSoulRefine = None
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.bindWnds = []

	def Close(self):
		if constinfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			self.OnCloseQuestionDialog()
			return

		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

		if self.dlgPickMoney:
			self.dlgPickMoney.Close()

		self.Hide()

	def GetInventoryPageIndex(self):
		return self.inventoryPageIndex + (self.category * 4)

	def SetInventoryPage(self, page):
		self.inventoryPageIndex = page

		for x in xrange(player.EXTRA_INVENTORY_PAGE_COUNT / (player.EXTRA_INVENTORY_CATEGORY_COUNT)):
			if x != page:
				self.inventoryTab[x].SetUp()
		
		self.RefreshItemSlot()

	def SetCategory(self, category):
		self.category = category

		for x in xrange(player.EXTRA_INVENTORY_CATEGORY_COUNT):
			if x != category:
				self.categoryTab[x].SetUp()
		
		self.RefreshItemSlot()

	def OnPickItem(self, count):
		itemSlotIndex = self.dlgPickMoney.itemGlobalSlotIndex
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(player.EXTRA_INVENTORY, itemSlotIndex):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return
		
		division = self.dlgPickMoney.division
		if division:
			net.SendItemDivisionPacket(itemSlotIndex, player.EXTRA_INVENTORY)
		else:
			selectedItemVNum = player.GetItemIndex(player.EXTRA_INVENTORY, itemSlotIndex)
			mousemodule.mouseController.AttachObject(self, player.SLOT_TYPE_EXTRA_INVENTORY, itemSlotIndex, selectedItemVNum, count)

	def __InventoryLocalSlotPosToGlobalSlotPos(self, local):
		return self.inventoryPageIndex * player.EXTRA_INVENTORY_PAGE_SIZE + local + (self.category * (player.EXTRA_INVENTORY_PAGE_SIZE * 4)) 

	if app.WJ_ENABLE_TRADABLE_ICON:
		def RefreshMarkSlots(self, localIndex=None):
			if not self.interface:
				return
			
			unusable = False
			
			onTopWnd = self.interface.GetOnTopWindow()
			if localIndex:
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(localIndex)
				if onTopWnd == player.ON_TOP_WND_SHOP:
					if player.IsAntiFlagBySlot(player.EXTRA_INVENTORY, slotNumber, item.ANTIFLAG_SELL):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
						unusable = True
				elif onTopWnd == player.ON_TOP_WND_EXCHANGE:
					if player.IsAntiFlagBySlot(player.EXTRA_INVENTORY, slotNumber, item.ANTIFLAG_GIVE):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
						unusable = True
				elif onTopWnd == player.ON_TOP_WND_PRIVATE_SHOP:
					if player.IsAntiFlagBySlot(player.EXTRA_INVENTORY, slotNumber, item.ITEM_ANTIFLAG_MYSHOP):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
						unusable = True
				elif onTopWnd == player.ON_TOP_WND_SAFEBOX:
					if player.IsAntiFlagBySlot(player.EXTRA_INVENTORY, slotNumber, item.ANTIFLAG_SAFEBOX):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
						unusable = True
				
				if app.ENABLE_AURA_SYSTEM and not unusable:
					if player.IsAuraRefineWindowOpen() and player.FindActivatedAuraSlot(player.EXTRA_INVENTORY, slotNumber) != player.AURA_SLOT_MAX:
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
						unusable = True
				
				if unusable == False:
					self.wndItem.SetUsableSlotOnTopWnd(localIndex)
				
				return
			
			for i in xrange(player.EXTRA_INVENTORY_PAGE_SIZE):
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
				if onTopWnd == player.ON_TOP_WND_SHOP:
					if player.IsAntiFlagBySlot(player.EXTRA_INVENTORY, slotNumber, item.ANTIFLAG_SELL):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
						unusable = True
				elif onTopWnd == player.ON_TOP_WND_EXCHANGE:
					if player.IsAntiFlagBySlot(player.EXTRA_INVENTORY, slotNumber, item.ANTIFLAG_GIVE):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
						unusable = True
				elif onTopWnd == player.ON_TOP_WND_PRIVATE_SHOP:
					if player.IsAntiFlagBySlot(player.EXTRA_INVENTORY, slotNumber, item.ITEM_ANTIFLAG_MYSHOP):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
						unusable = True
				elif onTopWnd == player.ON_TOP_WND_SAFEBOX:
					if player.IsAntiFlagBySlot(player.EXTRA_INVENTORY, slotNumber, item.ANTIFLAG_SAFEBOX):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
						unusable = True
				
				if app.ENABLE_AURA_SYSTEM and not unusable:
					if player.IsAuraRefineWindowOpen() and player.FindActivatedAuraSlot(player.EXTRA_INVENTORY, slotNumber) != player.AURA_SLOT_MAX:
						self.wndItem.SetUnusableSlotOnTopWnd(i)
						unusable = True
				
				if unusable == False:
					self.wndItem.SetUsableSlotOnTopWnd(i)

	def RefreshItemSlot(self):
		getItemVNum = player.GetItemIndex
		getItemCount = player.GetItemCount
		
		for i in xrange(player.EXTRA_INVENTORY_PAGE_SIZE):
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
			itemCount = getItemCount(player.EXTRA_INVENTORY, slotNumber)
			if 0 == itemCount:
				self.wndItem.ClearSlot(i)
				continue
			elif 1 == itemCount:
				itemCount = 0
			
			itemVnum = getItemVNum(player.EXTRA_INVENTORY, slotNumber)
			if not itemVnum:
				continue
			
			self.wndItem.SetItemSlot(i, itemVnum, itemCount)
			
			wasActivated = 0
			
			if app.ENABLE_HIGHLIGHT_SYSTEM and slotNumber in self.listHighlightedSlot[self.category]:
				self.wndItem.ActivateSlotEffect(i, (255.00 / 255.0), (20.00 / 255.0), (22.00 / 255.0), 1.0)
				wasActivated = 1
			
			if wasActivated == 0 and app.ENABLE_AURA_SYSTEM:
				slotNumberChecked = 0
				if player.IsAuraRefineWindowOpen() and player.FindActivatedAuraSlot(player.EXTRA_INVENTORY, slotNumber) != player.AURA_SLOT_MAX:
					self.wndItem.ActivateSlotEffect(i, (56.00 / 255.0), (237.00 / 255.0), (77.00 / 255.0), 1.0)
					wasActivated = 1
			
			if wasActivated == 0 and app.ENABLE_HIGHLIGHT_SYSTEM:
				self.wndItem.DeactivateSlot(i)
			
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.RefreshMarkSlots(i)
		
		self.wndItem.RefreshSlot()
		if app.WJ_ENABLE_TRADABLE_ICON:
			map(lambda wnd:wnd.RefreshExtraLockedSlot(), self.bindWnds)

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def SellItem(self):
		
		# offlineshop-updated 04/08/19
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(player.EXTRA_INVENTORY, self.sellingSlotNumber):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return
		
		if self.sellingSlotitemIndex == player.GetItemIndex(player.EXTRA_INVENTORY, self.sellingSlotNumber):
			if self.sellingSlotitemCount == player.GetItemCount(player.EXTRA_INVENTORY, self.sellingSlotNumber):
				net.SendShopSellPacketNew(self.sellingSlotNumber, self.questionDialog.count, player.EXTRA_INVENTORY)
				snd.PlaySound("sound/ui/money.wav")
		self.OnCloseQuestionDialog()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return

		self.questionDialog.Close()
		self.questionDialog = None
		constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def SelectEmptySlot(self, selectedSlotPos):
		if constinfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return
		
		if app.ENABLE_AURA_SYSTEM:
			if player.IsAuraRefineWindowOpen():
				return
		
		if app.ENABLE_CHANGELOOK:
			if player.GetChangeLookWindowOpen() == 1:
				return
		
		selectedSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(selectedSlotPos)

		if mousemodule.mouseController.isAttached():
			attachedSlotType = mousemodule.mouseController.GetAttachedType()
			attachedSlotPos = mousemodule.mouseController.GetAttachedSlotNumber()
			attachedCount = mousemodule.mouseController.GetAttachedItemCount()
			
			# offlineshop-updated 04/08/19
			if app.__ENABLE_NEW_OFFLINESHOP__:
				if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(attachedSlotType,attachedSlotPos):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
					return
			
			if player.SLOT_TYPE_EXTRA_INVENTORY == attachedSlotType:
				self.__SendMoveItemPacket(attachedSlotPos, selectedSlotPos, attachedCount)
			elif player.SLOT_TYPE_SAFEBOX == attachedSlotType:
				net.SendSafeboxCheckoutPacket(attachedSlotPos, player.EXTRA_INVENTORY, selectedSlotPos)
			elif app.ENABLE_AURA_SYSTEM and player.SLOT_TYPE_AURA == attachedSlotType:
				net.SendAuraRefineCheckOut(attachedSlotPos, player.GetAuraRefineWindowType())
			
			mousemodule.mouseController.DeattachObject()

	def SelectItemSlot(self, itemSlotIndex):
		if constinfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return
		
		itemSlotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(itemSlotIndex)

		selectedItemVNum = player.GetItemIndex(player.EXTRA_INVENTORY, itemSlotIndex)
		itemCount = player.GetItemCount(player.EXTRA_INVENTORY, itemSlotIndex)

		if mousemodule.mouseController.isAttached():
			attachedSlotType = mousemodule.mouseController.GetAttachedType()
			attachedSlotPos = mousemodule.mouseController.GetAttachedSlotNumber()
			attachedCount = mousemodule.mouseController.GetAttachedItemCount()

			if player.SLOT_TYPE_EXTRA_INVENTORY == attachedSlotType:
				self.__SendMoveItemPacket(attachedSlotPos, itemSlotIndex, attachedCount)

			mousemodule.mouseController.DeattachObject()
		else:
			curCursorNum = app.GetCursor()
			if app.SELL == curCursorNum:
				self.__SellItem(itemSlotIndex)
				
			elif app.BUY == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.SHOP_BUY_INFO)
			
			elif app.IsPressed(app.DIK_LALT):
				link = player.GetItemLink(player.EXTRA_INVENTORY, itemSlotIndex)
				ime.PasteString(link)
			
			elif app.IsPressed(app.DIK_LSHIFT):
				
				if itemCount > 1:
					self.dlgPickMoney.SetTitleName(localeinfo.PICK_ITEM_TITLE)
					self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
					self.dlgPickMoney.Open(itemCount, 1, False)
					self.dlgPickMoney.itemGlobalSlotIndex = itemSlotIndex
				else:
					mousemodule.mouseController.AttachObject(self, player.SLOT_TYPE_EXTRA_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)
			else:
				mousemodule.mouseController.AttachObject(self, player.SLOT_TYPE_EXTRA_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)
				snd.PlaySound("sound/ui/pick.wav")

	def __DropSrcItemToDestItemInInventory(self, srcItemVID, srcItemSlotPos, dstItemSlotPos):
		if app.ENABLE_AURA_SYSTEM and player.IsAuraRefineWindowOpen():
			return
		
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop() and (uiofflineshop.IsSaleSlot(player.EXTRA_INVENTORY, srcItemSlotPos) or uiofflineshop.IsSaleSlot(player.EXTRA_INVENTORY , dstItemSlotPos)):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return
		
		
		if item.IsKey(srcItemVID):
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
		else:
			self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)

	def __SellItem(self, itemSlotPos):
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return
		
		self.sellingSlotNumber = itemSlotPos
		
		itemIndex = player.GetItemIndex(player.EXTRA_INVENTORY, itemSlotPos)
		itemCount = player.GetItemCount(player.EXTRA_INVENTORY, itemSlotPos)

		self.sellingSlotitemIndex = itemIndex
		self.sellingSlotitemCount = itemCount

		item.SelectItem(itemIndex)

		if item.IsAntiFlag(item.ANTIFLAG_SELL):
			popup = uicommon.PopupDialog()
			popup.SetText(localeinfo.SHOP_CANNOT_SELL_ITEM)
			popup.SetAcceptEvent(self.__OnClosePopupDialog)
			popup.Open()
			self.popup = popup
			return

		itemPrice = item.GetISellItemPrice()

		if item.Is1GoldItem():
			itemPrice = itemCount / itemPrice / 5
		else:
			itemPrice = itemPrice * itemCount / 5

		item.GetItemName(itemIndex)
		itemName = item.GetItemName()

		self.questionDialog = uicommon.QuestionDialog()
		self.questionDialog.SetText(localeinfo.DO_YOU_SELL_ITEM(itemName, itemCount, itemPrice))
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.SellItem))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		self.questionDialog.Open()
		self.questionDialog.count = itemCount

		constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def __OnClosePopupDialog(self):
		self.pop = None

	def OverOutItem(self):
		self.wndItem.SetUseMode(False)
		self.wndItem.SetUsableItem(False)
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverInItem(self, slotNumber):
		overSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(slotNumber)
		if app.ENABLE_HIGHLIGHT_SYSTEM:
			if overSlotPos in self.listHighlightedSlot[self.category]:
				self.wndItem.DeactivateSlot(slotNumber)
				self.listHighlightedSlot[self.category].remove(overSlotPos)
		
		self.wndItem.SetUsableItem(True)
		
		if mousemodule.mouseController.isAttached():
			attachedItemType = mousemodule.mouseController.GetAttachedType()
			if attachedItemType == player.SLOT_TYPE_EXTRA_INVENTORY:
				attachedItemVNum = mousemodule.mouseController.GetAttachedItemIndex()
				if self.__CanUseSrcItemToDstItem(attachedItemVNum, overSlotPos):
					self.wndItem.SetUseMode(True)
				else:
					self.wndItem.SetUseMode(False)
		
		self.ShowToolTip(overSlotPos)

	def __CanUseSrcItemToDstItem(self, srcItemVNum, dstSlotPos):
		if item.IsKey(srcItemVNum):
			if player.CanUnlock(srcItemVNum, player.EXTRA_INVENTORY, dstSlotPos):
				return True


	if app.__ENABLE_NEW_OFFLINESHOP__:
		def __AddTooltipSaleMode(self, slot):
			itemIndex = player.GetItemIndex(player.EXTRA_INVENTORY,slot)
			if itemIndex !=0:
				item.SelectItem(itemIndex)
				if item.IsAntiFlag(item.ANTIFLAG_MYSHOP) or item.IsAntiFlag(item.ANTIFLAG_GIVE):
					return
				
				self.tooltipItem.AddRightClickForSale()

	def ShowToolTip(self, slotIndex):
		if self.tooltipItem:
			itemVnum = player.GetItemIndex(player.EXTRA_INVENTORY, slotIndex)
			if not itemVnum:
				return

			self.tooltipItem.SetInventoryItem(slotIndex, player.EXTRA_INVENTORY)

			if app.__ENABLE_NEW_OFFLINESHOP__:
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
		if None != self.tooltipItem:
			self.tooltipItem.SetTop()
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			map(lambda wnd:wnd.RefreshExtraLockedSlot(), self.bindWnds)
			self.RefreshMarkSlots()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def UseItemSlot(self, slotIndex):
		curCursorNum = app.GetCursor()
		if app.SELL == curCursorNum:
			return

		if constinfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			return

		slotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)
		
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			if self.wndDragonSoulRefine.IsShow():
				self.wndDragonSoulRefine.AutoSetItem((player.EXTRA_INVENTORY, slotIndex), 1)
				return
		
		itemVnum 	= player.GetItemIndex(player.EXTRA_INVENTORY, slotIndex)
		if not itemVnum:
			return
		
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop():
				item.SelectItem(itemVnum)
				
				if not item.IsAntiFlag(item.ANTIFLAG_GIVE) and not item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
					if app.ENABLE_OFFLINESHOP_REWORK:
						offlineshop.ShopBuilding_AddItem(player.EXTRA_INVENTORY, slotIndex)
					else:
						offlineshop.ShopBuilding_AddItem(player.EXTRA_INVENTORY, slotIndex)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				
				return
			elif uiofflineshop.IsBuildingAuction():
				item.SelectItem(itemVnum)
				if not item.IsAntiFlag(item.ANTIFLAG_GIVE) and not item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
					offlineshop.AuctionBuilding_AddItem(player.EXTRA_INVENTORY,slotIndex)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				
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
					net.SendMultipleOpening(player.EXTRA_INVENTORY, slotIndex, player.GetItemCount(player.EXTRA_INVENTORY, slotIndex))
				else:
					self.__UseItem(slotIndex)
		else:
			self.__UseItem(slotIndex)

		mousemodule.mouseController.DeattachObject()
		self.OverOutItem()

	if app.ENABLE_AURA_SYSTEM:
		def __UseItemAuraQuestionDialog_OnAccept(self):
			self.questionDialog.Close()
			net.SendAuraRefineCheckIn(*(self.questionDialog.srcItem + self.questionDialog.dstItem + (player.GetAuraRefineWindowType(),)))
			self.questionDialog.srcItem = (0, 0)
			self.questionDialog.dstItem = (0, 0)

		def __UseItemAuraQuestionDialog_Close(self):
			self.questionDialog.Close()

			self.questionDialog.srcItem = (0, 0)
			self.questionDialog.dstItem = (0, 0)

		def __UseItemAura(self, slotIndex):
			AuraSlot = player.FineMoveAuraItemSlot()
			UsingAuraSlot = player.FindActivatedAuraSlot(player.EXTRA_INVENTORY, slotIndex)
			AuraVnum = player.GetItemIndex(player.EXTRA_INVENTORY, slotIndex)
			item.SelectItem(AuraVnum)
			
			if player.GetAuraCurrentItemSlotCount() >= player.AURA_SLOT_MAX <= UsingAuraSlot:
				return
			
			if player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_ABSORB:
				isAbsorbItem = False
				if item.GetItemType() == item.ITEM_TYPE_COSTUME:
					if item.GetItemSubType() == item.COSTUME_TYPE_AURA:
						if player.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM) == 0:
							if UsingAuraSlot == player.AURA_SLOT_MAX:
								if AuraSlot != player.AURA_SLOT_MAIN:
									return
								
								net.SendAuraRefineCheckIn(player.EXTRA_INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_ABSORBITEM)
							return
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_ABSORBITEM)
						return
				elif item.GetItemType() == item.ITEM_TYPE_ARMOR:
					if item.GetItemSubType() in [item.ARMOR_SHIELD, item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR]:
						if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN) == player.NPOS():
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return
						
						isAbsorbItem = True
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_ABSORBITEM)
						return
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_ABSORBITEM)
					return
				
				if isAbsorbItem:
					if UsingAuraSlot == player.AURA_SLOT_MAX:
						if AuraSlot != player.AURA_SLOT_SUB:
							if player.FindUsingAuraSlot(player.AURA_SLOT_SUB) == player.NPOS():
								AuraSlot = player.AURA_SLOT_SUB
							else:
								return
						
						self.questionDialog = uicommon.QuestionDialog()
						self.questionDialog.SetText(localeinfo.AURA_NOTICE_DEL_ABSORDITEM)
						self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemAuraQuestionDialog_OnAccept))
						self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemAuraQuestionDialog_Close))
						self.questionDialog.srcItem = (player.EXTRA_INVENTORY, slotIndex)
						self.questionDialog.dstItem = (player.AURA_REFINE, AuraSlot)
						self.questionDialog.Open()
			elif player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_GROWTH:
				if UsingAuraSlot == player.AURA_SLOT_MAX:
					if AuraSlot == player.AURA_SLOT_MAIN:
						if item.GetItemType() == item.ITEM_TYPE_COSTUME:
							if item.GetItemSubType() == item.COSTUME_TYPE_AURA:
								socketLevelValue = player.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
								curLevel = (socketLevelValue / 100000) - 1000
								curExp = socketLevelValue % 100000;
								if curLevel >= player.AURA_MAX_LEVEL:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_MAX_LEVEL)
									return
								
								if curExp >= player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_GROWTHITEM)
									return
								
								net.SendAuraRefineCheckIn(player.EXTRA_INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_AURAITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_AURAITEM)
							return
					elif AuraSlot == player.AURA_SLOT_SUB:
						if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN) != player.NPOS():
							if item.GetItemType() == item.ITEM_TYPE_RESOURCE:
								if item.GetItemSubType() == item.RESOURCE_AURA:
									if UsingAuraSlot == player.AURA_SLOT_MAX:
										if AuraSlot != player.AURA_SLOT_SUB:
											return
										
										net.SendAuraRefineCheckIn(player.EXTRA_INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
								else:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_AURARESOURCE)
									return
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_AURARESOURCE)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return
			elif player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_EVOLVE:
				if UsingAuraSlot == player.AURA_SLOT_MAX:
					if AuraSlot == player.AURA_SLOT_MAIN:
						if item.GetItemType() == item.ITEM_TYPE_COSTUME:
							if item.GetItemSubType() == item.COSTUME_TYPE_AURA:
								socketLevelValue = player.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
								curLevel = (socketLevelValue / 100000) - 1000
								curExp = socketLevelValue % 100000;
								if curLevel >= player.AURA_MAX_LEVEL:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_MAX_LEVEL)
									return
								
								if curLevel != player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_LEVEL_MAX) or curExp < player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
									return
								
								if player.FindUsingAuraSlot(AuraSlot) != player.NPOS():
									return
								
								net.SendAuraRefineCheckIn(player.EXTRA_INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_AURAITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_AURAITEM)
							return
					elif AuraSlot == player.AURA_SLOT_SUB:
						Cell = player.FindUsingAuraSlot(player.AURA_SLOT_MAIN)
						if Cell == player.NPOS():
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return
						
						socketLevelValue = player.GetItemMetinSocket(*(Cell + (player.ITEM_SOCKET_AURA_CURRENT_LEVEL,)))
						curLevel = (socketLevelValue / 100000) - 1000
						curExp = socketLevelValue % 100000;
						if curLevel >= player.AURA_MAX_LEVEL:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_MAX_LEVEL)
							return
						
						if curExp < player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
							return
						
						if AuraVnum != player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_MATERIAL_VNUM):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
							return
						
						if player.GetItemCount(player.EXTRA_INVENTORY, slotIndex) < player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_MATERIAL_COUNT):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.AURA_IMPOSSIBLE_EVOLUTION_ITEMCOUNT)
							return
						
						if UsingAuraSlot == player.AURA_SLOT_MAX:
							if AuraSlot != player.AURA_SLOT_MAX:
								if player.FindUsingAuraSlot(AuraSlot) != player.NPOS():
									return
							
							net.SendAuraRefineCheckIn(player.EXTRA_INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())

	def __UseItem(self, slotIndex):
		ItemVNum = player.GetItemIndex(player.EXTRA_INVENTORY, slotIndex)
		if not ItemVNum:
			return
		
		if app.ENABLE_CHANGELOOK:
			if player.GetChangeLookWindowOpen() == 1:
				return
		
		if app.ENABLE_AURA_SYSTEM:
			if player.IsAuraRefineWindowOpen():
				self.__UseItemAura(slotIndex)
				return
		
		item.SelectItem(ItemVNum)
		
		if item.IsFlag(item.ITEM_FLAG_CONFIRM_WHEN_USE):
			self.questionDialog = uicommon.QuestionDialog()
			self.questionDialog.SetText(localeinfo.INVENTORY_REALLY_USE_ITEM)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
			self.questionDialog.Open()
			self.questionDialog.slotIndex = slotIndex

			constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
		else:
			self.__SendUseItemPacket(slotIndex)

	def __UseItemQuestionDialog_OnCancel(self):
		self.OnCloseQuestionDialog()

	def __UseItemQuestionDialog_OnAccept(self):
		self.__SendUseItemPacket(self.questionDialog.slotIndex)
		self.OnCloseQuestionDialog()

	def __SendUseItemPacket(self, slotPos):
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemUsePacket(player.EXTRA_INVENTORY, slotPos)

	def __SendMoveItemPacket(self, srcSlotPos, dstSlotPos, srcItemCount):
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.MOVE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		if srcSlotPos == dstSlotPos:
			return

		net.SendItemMovePacket(player.EXTRA_INVENTORY, srcSlotPos, player.EXTRA_INVENTORY, dstSlotPos, srcItemCount)

	def __SendUseItemToItemPacket(self, srcSlotPos, dstSlotPos):
		if uiprivateshopbuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.MOVE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		if srcSlotPos == dstSlotPos:
			return

		net.SendItemUseToItemPacket(player.EXTRA_INVENTORY, srcSlotPos, player.EXTRA_INVENTORY, dstSlotPos)

	# if app.ENABLE_SORT_INVEN:
		# def SortExtraInventory(self):
			# net.SendChatPacket("/sort_extra_inventory")

		# def Sort_ExtraInventoryDone(self):
			# if app.ENABLE_HIGHLIGHT_SYSTEM:
				# for i in xrange(player.EXTRA_INVENTORY_CATEGORY_COUNT):
					# del self.listHighlightedSlot[i][:]
			# self.RefreshItemSlot()

	def ClickMallButton(self):
		print "click_mall_button"
		net.SendChatPacket("/click_mall")

	def ClickSafeboxButton(self):
		print "click_safebox_button"
		net.SendChatPacket("/click_safebox")
