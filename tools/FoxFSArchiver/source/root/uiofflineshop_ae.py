import app
import uipickmoney
import grp
import mousemodule
import player
import net
import offlineshop
import ui
import item
import dbg
import uicommon
import uitooltip
import localeinfo
import ime

import constinfo
import time
import chat

import uishopsearch
import uiofflineshop_history

try:
	app.WJ_ENABLE_TRADABLE_ICON	
except Exception:
	app.WJ_ENABLE_TRADABLE_ICON=False

PAGE_MYSHOP=0
PAGE_BUILDER=1
PAGE_SAFEBOX=2
PAGE_SHOP=3
PAGE_HISTORY=4

OFFLINESHOP_SLOT_COUNT=10 * 8
CACHE_ITEMS_ID={}

PATH_SHOP="cream/"
PATH_ROOT="cream/shopeditor/"
PATH_ROOT_BUILD="cream/shopbuilder/"
PATH_ROOT_SEARCH="cream/searchshop/"

def IsEditOrBuildMode():
	interface=offlineshop.GetOfflineshopBoard()
	if not interface:
		return False
	
	return interface.IsBuildingShop()

def IsBuildingShop():
	interface=offlineshop.GetOfflineshopBoard()
	if not interface:
		return False
	
	return interface.IsBuildingShop()

def IsSaleSlot(win,slot):
	#interface=offlineshop.GetOfflineshopBoard()
	#if not interface:
	#	return False
	#
	return False

def MakeRadioButton(parent,x,y,path,up,over,down):
	button=ui.RadioButton()
	button.SetParent(parent)
	button.SetPosition(x,y)
	button.SetUpVisual(path+up)
	button.SetOverVisual(path+over)
	button.SetDownVisual(path+down)
	button.Show()
	return button

class OfflineShopRenewal(ui.ScriptWindow):
	def __init__(self,wndBind):
		ui.ScriptWindow.__init__(self)
		
		offlineshop.SetOfflineshopBoard(self)
		
		self.ShopSafeboxItems=[]
		self.ShopSafeboxValuteAmount=0
		self.ShopSafeboxValuteText=0
		self.shopOfflinePopup=0
		
		self.ShopItemForSale=[]
		self.ShopItemSold=[]
		self.ShopOpenInfo={}
		
		self.wndOffShop=wndBind

	def SendNotification(self,dwItemID,dwItemPrice,dwItemCount):
		item.SelectItem(dwItemID)
		itemName=item.GetItemName()
		
		if self.shopOfflinePopup==0:
			self.shopOfflinePopup=uicommon.ShopOfflinePopup()
		
		self.shopOfflinePopup.AddNotification(dwItemID,itemName,dwItemPrice,dwItemCount)

	def ShopFilterResult(self ,size):
		self.wndOffShop.wndSearchShop.ShopFilterResult(size)

	def ShopFilterResultItem_Alloc(self):
		self.wndOffShop.wndSearchShop.ShopFilterResultItem_Alloc()
	
	def ShopFilterResultItem_SetValue(self, key,index,*args):
		self.wndOffShop.wndSearchShop.ShopFilterResultItem_SetValue(key,index,*args)
	
	def ShopFilterResult_Show(self):
		self.wndOffShop.wndSearchShop.ShopFilterResult_Show()
		
	def SearchFilter_BuyFromSearch(self,ownerid,itemid):
		self.wndOffShop.wndSearchShop.SearchFilter_BuyFromSearch(ownerid,itemid)

	def IsBuildingShop(self):
		if self.wndOffShop.IsShow()==False:
			return False
		
		return self.wndOffShop.wBoard[PAGE_BUILDER].IsShow() or self.wndOffShop.wBoard[PAGE_MYSHOP].IsShow()

	def ShopBuilding_AddItem(self,win,slot):
		if player.GetItemIndex(win,slot) ==0:
			return
		
		if win==player.INVENTORY and player.IsEquipmentSlot(slot):
			return
		
		if self.IsForSaleSlot(win,slot):
			return
		
		itemIndex=player.GetItemIndex(win,slot)
		if itemIndex==0:
			return False
		
		item.SelectItem(itemIndex)
		
		_,size=item.GetItemSize()
		
		page,pos=self.wndOffShop.GetEmptyPosition(size)
		if pos < 0:
			return
		
		if self.__IsSaleableSlot(win,slot):
			price=self.wndOffShop.LoadInputPrice(player.GetItemIndex(win,slot))
			bCanSkipDlg=False
			self.wndOffShop.OpenInputPriceDialog(win,slot,page,pos,False,bCanSkipDlg)

	def IsForSaleSlot(self,win,slot):
		if self.wndOffShop.IsShow()==False:
			return
		
		if self.wndOffShop.wBoard[PAGE_BUILDER].IsShow():
			if self.wndOffShop.PriceInputBoard:
				if self.wndOffShop.PriceInputBoard.sourceWindowType==win and\
					self.wndOffShop.PriceInputBoard.sourceSlotPos==slot:
					return True
		
			for page in xrange(4):
				for privatePos,(itemWindowType,itemSlotIndex,priceInfo)\
					in self.wndOffShop.itemStockBuilder[page].items():
					if itemWindowType==win and itemSlotIndex==slot:
						return True
		elif self.wndOffShop.wBoard[PAGE_MYSHOP].IsShow():
			if self.wndOffShop.PriceInputBoard:
				if self.wndOffShop.PriceInputBoard.sourceWindowType==win and\
					self.wndOffShop.PriceInputBoard.sourceSlotPos==slot:
					return True
		
		return False

	def __IsSaleableSlot(self,win ,pos):
		if win==player.INVENTORY and player.IsEquipmentSlot(pos):
			return False
		
		if self.IsBuildingShop() and self.IsForSaleSlot(win,pos):
			return False
		
		if not win in (player.INVENTORY,player.DRAGON_SOUL_INVENTORY):
			return False
		
		itemIndex=player.GetItemIndex(win,pos)
		if itemIndex==0:
			return False
		
		item.SelectItem(itemIndex)
		if item.IsAntiFlag(item.ITEM_ANTIFLAG_MYSHOP) or item.IsAntiFlag(item.ITEM_ANTIFLAG_GIVE):
			return False
		
		return True

	def IsForAuctionSlot(self,win,slot):
		return False

	def IsBuildingAuction(self):
		return False

	def Destroy(self):
		self.ClearDictionary()
		
		self.wndOffShop=0
		self.shopOfflinePopup=0
		offlineshop.SetOfflineshopBoard(0)

	def OpenShop(self,owner_id,duration,count,name):
		self.ShopItemSold=[]
		self.ShopItemForSale=[]
		
		self.ShopOpenInfo["owner_id"]=owner_id
		self.ShopOpenInfo["duration"]=duration
		self.ShopOpenInfo["count"]=count
		self.ShopOpenInfo["name"]=name
		self.ShopOpenInfo["my_shop"]=False

	def OpenShopItem_Alloc(self):
		self.ShopItemForSale.append({})

	def OpenShopItem_SetValue(self,key,index,*args):
		if key=="id":
			self.ShopItemForSale[index][key]=args[0]
		elif key=="vnum" or key=='trans':
			self.ShopItemForSale[index][key]=args[0]
		elif key=="count":
			self.ShopItemForSale[index][key]=args[0]
		elif key=="attr":
			if not key in self.ShopItemForSale[index]:
				self.ShopItemForSale[index][key]={}
			
			attr_index=args[0]
			attr_type =args[1]
			attr_value=args[2]
			
			self.ShopItemForSale[index][key][attr_index]={}
			self.ShopItemForSale[index][key][attr_index]["type"] =attr_type
			self.ShopItemForSale[index][key][attr_index]["value"]=attr_value
		elif key=="socket":
			if not key in self.ShopItemForSale[index]:
				self.ShopItemForSale[index][key]={}
			
			socket_index=args[0]
			socket_val=args[1]
			self.ShopItemForSale[index][key][socket_index]=socket_val
		elif key=="price":
			self.ShopItemForSale[index][key]=args[0]

	def OpenShop_End(self):
		self.wndOffShop.RefreshOpenShopPage()
		
		if not self.wndOffShop.IsShow():
			self.wndOffShop.Show()

	def OpenShopOwner_Start(self,owner_id,duration ,count ,name):
		self.ShopItemSold=[]
		self.ShopItemForSale=[]
		self.MyShopOffers=[]
		
		self.ShopOpenInfo["owner_id"]=owner_id
		self.ShopOpenInfo["duration"]=duration
		self.ShopOpenInfo["count"]=count
		self.ShopOpenInfo["name"]=name
		self.ShopOpenInfo["my_shop"]=True

	def OpenShopOwner_End(self):
		self.wndOffShop.RefreshMyShopPage()
		if not self.wndOffShop.IsShow():
			self.wndOffShop.Show()

	def OpenShopOwnerItemSold_Alloc(self ):
		self.ShopItemSold.append({})

	def OpenShopOwnerItemSold_SetValue(self, key ,index ,*args):
		if key=="id":
			self.ShopItemSold[index][key]=args[0]
		elif key=="vnum" or key=='trans':
			self.ShopItemSold[index][key]=args[0]
		elif key=="count":
			self.ShopItemSold[index][key]=args[0]
		elif key=="attr":
			if not key in self.ShopItemSold[index]:
				self.ShopItemSold[index][key]={}
			
			attr_index=args[0]
			attr_type =args[1]
			attr_value=args[2]
			
			self.ShopItemSold[index][key][attr_index]={}
			self.ShopItemSold[index][key][attr_index]["type"] =attr_type
			self.ShopItemSold[index][key][attr_index]["value"]=attr_value
		elif key=="socket":
			if not key in self.ShopItemSold[index]:
				self.ShopItemSold[index][key]={}
			
			socket_index=args[0]
			socket_val=args[1]
			
			self.ShopItemSold[index][key][socket_index]=socket_val
		elif key=="price":
			self.ShopItemSold[index][key]=args[0]

	def OpenShopOwnerItemSold_Show(self):
		pass

	def OpenShopOwnerItem_Alloc(self):
		self.ShopItemForSale.append({})

	def OpenShopOwnerItem_SetValue(self,key,index,*args):
		if key=="id":
			self.ShopItemForSale[index][key]=args[0]
		elif key=="vnum" or key=='trans':
			self.ShopItemForSale[index][key]=args[0]
		elif key=="count":
			self.ShopItemForSale[index][key]=args[0]
		elif key=="attr":
			if not key in self.ShopItemForSale[index]:
				self.ShopItemForSale[index][key]={}
			
			attr_index=args[0]
			attr_type=args[1]
			attr_value=args[2]
			
			self.ShopItemForSale[index][key][attr_index]={}
			self.ShopItemForSale[index][key][attr_index]["type"] =attr_type
			self.ShopItemForSale[index][key][attr_index]["value"]=attr_value
		elif key=="socket":
			if not key in self.ShopItemForSale[index]:
				self.ShopItemForSale[index][key]={}
			
			socket_index=args[0]
			socket_val=args[1]
			
			self.ShopItemForSale[index][key][socket_index]=socket_val
		elif key=="price":
			self.ShopItemForSale[index][key]=args[0]

	def OpenShopOwnerNoShop(self):
		for v in self.wndOffShop.wBoard.values():
			v.Hide()
		
		self.wndOffShop.ResetCreateShopPage()
		
		if not self.wndOffShop.IsShow():
			self.wndOffShop.Show()

	def ShopSafebox_Clear(self):
		self.ShopSafeboxItems=[]

	def ShopSafebox_SetValutes(self,yang):
		self.ShopSafeboxValuteAmount=yang
		self.wndOffShop.wndTextYang.SetText(localeinfo.NumberToMoneyString(yang))

	def ShopSafebox_AllocItem(self):
		self.ShopSafeboxItems.append({})

	def ShopSafebox_SetValue(self,key ,*args):
		elm=self.ShopSafeboxItems[-1]
		if key in ("id","vnum","count",'trans'):
			elm[key]=args[0]
		elif key=="socket":
			if not key in elm:
				elm[key]=[0 for x in xrange(player.METIN_SOCKET_MAX_NUM)]
			elm[key][args[0]]=args[1]
		elif key in ("attr_type","attr_value"):
			if not 'attr' in elm:
				elm['attr']={}
			
			if not args[0] in elm['attr']:
				elm['attr'][args[0]]={}
			
			elm['attr'][args[0]][key.replace('attr_','')]=args[1]

	def ShopSafebox_RefreshEnd(self):
		self.wndOffShop.RefreshSafeboxPage()

class OfflineShopWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		
		self.bIsLoaded=False
		
		self.wndShop=OfflineShopRenewal(self)
		self.wndShop.Hide()
		
		self.wndSearchShop=uishopsearch.ShopSearch()
		self.wndSearchShop.Hide()
		
		self.xShopStart=0
		self.yShopStart=0
		self.ShopOpenID=0
		
		self.__Reinit()
		self.__Reinit2(True)
		self.__LoadWindow()
		
		self.SetCenterPosition()
	
	def SetItemToolTip(self,tooltip):
		self.tooltipItem=tooltip
		if self.wndHistory:
			self.wndHistory.SetItemToolTip(tooltip)

	def BindInterface(self,interface):
		self.interface=interface
		if self.wndSearchShop:
			self.wndSearchShop.SetInterface(interface)

	def __Reinit2(self,isDestroy=False):
		self.wBoard={}
		self.wBar={}
		self.dictName={}
		self.itemStock={}
		
		self.ClearStockBuilder(isDestroy)

	def __Reinit(self):
		self.MyShopEditNameDlg=0
		self.WithdrawQuestionDialog=0
		self.QuestionDialog=0
		self.PriceInputBoard=0
		self.ShopName=0
		self.tooltipItem=0
		self.interface=0
		self.PageGrid=0

	def OnPressEscapeKey(self):
		if self.NameEdit.IsFocus():
			self.NameEdit.KillFocus()
			return False
		
		if self.QuestionDialog:
			if self.QuestionDialog.IsShow():
				return False
		
		if self.WithdrawQuestionDialog:
			return False
		
		self.Close()
		return True

	def Destroy(self):
		self.ClearDictionary()
		self.CloseDialogs(True)
		
		self.__Reinit()
		self.__Reinit2(True)
		
		if self.wndShop:
			self.wndShop.Destroy()
			self.wndShop=0
		
		if self.wndHistory:
			self.wndHistory.Clear()
			self.wndHistory.Destroy()
			self.wndHistory=0
		
		if self.wndSearchShop:
			self.wndSearchShop.Destroy()
			self.wndSearchShop.Hide()
			self.wndSearchShop=0

	def CloseDialogs(self,isDestroy=False):
		if self.WithdrawQuestionDialog:
			self.WithdrawQuestionDialog.Close()
			self.WithdrawQuestionDialog=0
		
		if self.QuestionDialog:
			self.OnCloseQuestionDialog()
		
		self.CancelInputPrice(isDestroy)

	def ClearStockBuilder(self,isDestroy=False):
		self.itemStockBuilder={}
		self.itemStockBuilder[0]={}
		self.itemStockBuilder[1]={}
		self.itemStockBuilder[2]={}
		self.itemStockBuilder[3]={}
		
		self.RefreshItemSlotInv(isDestroy)

	if app.WJ_ENABLE_TRADABLE_ICON:
		def OnTop(self):
			if self.interface==0:
				return
			
			if self.wBoard[PAGE_BUILDER].IsShow() or self.wBoard[PAGE_MYSHOP].IsShow():
				self.interface.SetOnTopWindow(player.ON_TOP_WND_SHOP)
			else:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
			
			self.interface.RefreshMarkInventoryBag()

	def Close(self,isDestroy=False):
		self.CancelInputPrice()
		self.ClearStockBuilder(isDestroy)
		self.CloseDialogs(isDestroy)
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			if self.interface:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				if not isDestroy:
					self.interface.RefreshMarkInventoryBag()
		
		if self.MyShopEditNameDlg:
			self.MyShopEditNameDlg.Hide()
			self.MyShopEditNameDlg=0
		
		offlineshop.SendCloseBoard()
		self.Hide()

	def Open(self):
		offlineshop.SendCloseBoard()
		offlineshop.SendOpenShopOwner()

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)
		
		self.SetTop()
		(self.xShopStart,self.yShopStart,z)=player.GetMainCharacterPosition()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.wndShop=0

	def __LoadWindow(self):
		if self.bIsLoaded:
			return
		
		self.bIsLoaded=True
		offlineshop.RefreshItemNameMap()
		
		self.AddFlag("movable")
		self.__InitPage(369,410,PAGE_MYSHOP,localeinfo.OFFLINESHOP_RENEWAL_EDIT,PATH_ROOT_BUILD+"bg.dds")
		self.__InitPage(369,405,PAGE_BUILDER,localeinfo.OFFLINESHOP_RENEWAL_CREATE,PATH_ROOT_BUILD+"bg.dds")
		self.__InitPage(369,385,PAGE_SAFEBOX,localeinfo.OFFLINESHOP_RENEWAL_SAFEBOX,PATH_ROOT_BUILD+"bg.dds")
		#if not app.DISABLE_OFFLINESHOP_CHANGENAME:
		self.__InitPage(369,340,PAGE_SHOP,localeinfo.OFFLINESHOP_RENEWAL_CHANGENAME,PATH_ROOT_BUILD+"bg.dds")
		
		self.__InitPage(369,410,PAGE_HISTORY,localeinfo.OFFLINESHOP_RENEWAL_HISTORY,PATH_ROOT_BUILD+"bg.dds")
		
		self.itemSlot=[]
		for xPage in xrange(2):
			itemSlot=ui.GridSlotWindow()
			itemSlot.SetParent(self)
			itemSlot.SetPosition(23,79)
			itemSlot.ArrangeSlot(0,10,8,32,32,0,0)
			itemSlot.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub",1.0,1.0,1.0,1.0)
			
			itemSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			itemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			itemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			itemSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			itemSlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
			itemSlot.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
			itemSlot.SetGridSpecial(10,8)
			itemSlot.Hide()
			
			if xPage==0:
				itemSlot.Show()
			
			self.itemSlot.append(itemSlot)

	def __ChangePage(self,page):
		self.CancelInputPrice()
		for index in xrange(len(self.wBoard)):
			if not self.wBoard.has_key(index):
				continue
			
			if index==page:
				self.wBoard[page].Show()
				self.SetSize(self.wBoard[page].GetWidth(),self.wBoard[page].GetHeight())
			else:
				self.wBoard[index].Hide()
		
		for idxPage in xrange(len(self.dictPages)):
			if page==PAGE_HISTORY:
				break
			elif page==PAGE_SHOP:
				self.dictPages[idxPage].SetPosition(17+27*idxPage,35)
			elif page==PAGE_SAFEBOX:
				self.dictPages[idxPage].SetPosition(17+27*idxPage,35)
			elif page==PAGE_BUILDER:
				self.dictPages[idxPage].SetPosition(17+27*idxPage,35)
			else:
				self.dictPages[idxPage].SetPosition(35+30*idxPage,351)
			
			self.dictPages[idxPage].SetParent(self.wBoard[page])
		
		if app.WJ_ENABLE_TRADABLE_ICON:	
			self.OnTop()
		
		if page==PAGE_HISTORY:
			for slot in self.itemSlot:
				slot.Hide()
			
			self.wndHistory.Show()
		else:
			self.wndHistory.Hide()
			self.itemSlot[self.PageGrid].Show()
	
	def __InitPage(self,width,height,page,title,bg=PATH_ROOT+"bg.dds"):
		if page==PAGE_HISTORY:
			self.wBoard[page]=ui.Board()
			self.wBoard[page].SetParent(self)
			self.wBoard[page].SetSize(width,height)
			self.wBoard[page].AddFlag("not_pick")
			self.wBoard[page].Show()
		else:
			self.wBoard[page]=ui.ExpandedImageBox()
			self.wBoard[page].SetParent(self)
			self.wBoard[page].AddFlag("not_pick")
			self.wBoard[page].LoadImage(bg)
			self.wBoard[page].Show()
		
		self.wBar[page]=ui.TitleBar()
		self.wBar[page].SetParent(self.wBoard[page])
		self.wBar[page].MakeTitleBar(self.wBoard[page].GetWidth() - 12,"red")
		self.wBar[page].SetPosition(6,6)
		self.wBar[page].SetCloseEvent(ui.__mem_func__(self.Close))
		self.dictName[page]=ui.MakeTextLineNew(self.wBar[page],0,4,title)
		self.dictName[page].SetWindowHorizontalAlignCenter()
		self.dictName[page].SetHorizontalAlignCenter()
		self.wBar[page].Show()
		
		self.SetSize(self.wBoard[page].GetWidth(),self.wBoard[page].GetHeight())
		
		if page==PAGE_BUILDER:
			self.NameSlotBar=ui.MakeImageBox(self.wBoard[page],PATH_ROOT_BUILD+"field_name.dds",0,35)
			self.NameSlotBar.SetWindowHorizontalAlignCenter()
			
			self.NameEdit=ui.EditLine()
			self.NameEdit.SetParent(self.NameSlotBar)
			self.NameEdit.SetMax(40)
			self.NameEdit.SetSize(self.NameSlotBar.GetWidth(),15)
			self.NameEdit.SetPosition(8,7)
			self.NameEdit.SetLimitWidth(self.NameEdit.GetWidth())
			self.NameEdit.SetEscapeEvent(ui.__mem_func__(self.OnPressNameEscapeKey))
			self.NameEdit.SetOutline()
			self.NameEdit.Show()
			
			self.btnGoSafeboxCRT=ui.MakeButton(self.wBoard[page],315,35,"Safebox",PATH_ROOT_BUILD,"btn_cosmetic_norm.dds","btn_cosmetic_hover.dds","btn_cosmetic_down.dds")
			self.btnCreateOk=ui.MakeButton(self.wBoard[page],150,350,False,PATH_ROOT_BUILD,"btn_norm.dds","btn_hover.dds","btn_down.dds")
			
			self.btnCreateOk.SetText(localeinfo.UI_OK)
			
			self.btnGoSafeboxCRT.SetEvent(ui.__mem_func__(self.ManageWindow))
			self.btnCreateOk.SetEvent(ui.__mem_func__(self.OnCreateShop))
		elif page==PAGE_MYSHOP:
			self.wndSlotTotalYang=ui.MakeImageBox(self.wBoard[page],PATH_ROOT+"field_total_cost.dds",20,37)
			
			self.wndTotalPrice=ui.MakeTextLineNew(self.wndSlotTotalYang,6,7,localeinfo.OFFLINESHOP_RENEWAL_TOTAL%0)
			self.wndTotalPrice.SetWindowHorizontalAlignLeft()
			self.wndTotalPrice.SetHorizontalAlignLeft()
			
			self.wndBoardRefill=ui.MakeImageBox(self.wBoard[page],PATH_ROOT+"field_time.dds",155,354)
			
			self.btnRefill=ui.MakeButton(self.wBoard[page],155 - 30,354,localeinfo.OFFLINESHOP_RENEWAL_EXPANDTIME,PATH_ROOT,"btn_refill_time_norm.dds","btn_refill_time_hover.dds","btn_refill_time_down.dds")
			self.btnRefill.SetEvent(ui.__mem_func__(self.AskIncreaseTimeShop))
			
			self.wndRemainTime=ui.MakeTextLineNew(self.wndBoardRefill,0,6,localeinfo.OFFLINESHOP_RENEWAL_LEFTTIME%0)
			self.wndRemainTime.SetWindowHorizontalAlignCenter()
			self.wndRemainTime.SetHorizontalAlignCenter()
			
			self.btnCloseShop=ui.MakeButton(self.wBoard[page],315,37,localeinfo.OFFLINESHOP_RENEWAL_CLOSE,PATH_ROOT,"btn_close_shop_norm.dds","btn_close_shop_hover.dds","btn_close_shop_down.dds")
			self.btnCloseShop.SetEvent(ui.__mem_func__(self.AskClosePrivateShop))
			
			if not app.DISABLE_OFFLINESHOP_CHANGENAME:
				self.btnChangeName=ui.MakeButton(self.wBoard[page],275,37,localeinfo.OFFLINESHOP_RENEWAL_CHANGENAME,PATH_ROOT,"btn_teleport_norm.dds","btn_teleport_hover.dds","btn_teleport_down.dds")
				self.btnChangeName.SetEvent(ui.__mem_func__(self.ChangeNameShop))
			
			self.btnHistory=ui.MakeButton(self.wBoard[page],235,37,localeinfo.OFFLINESHOP_RENEWAL_HISTORY,PATH_ROOT,"btn_history_norm.dds","btn_history_hover.dds","btn_history_down.dds")
			self.btnHistory.SetEvent(ui.__mem_func__(self.ManageHistoryWindow))
			
			self.btnGoToSafebox=ui.MakeButton(self.wBoard[page],196,37,localeinfo.OFFLINESHOP_RENEWAL_SAFEBOX,PATH_ROOT_BUILD,"btn_cosmetic_norm.dds","btn_cosmetic_hover.dds","btn_cosmetic_down.dds")
			self.btnGoToSafebox.SetEvent(ui.__mem_func__(self.ManageWindow))
			
			self.dictPages={}
			
			self.dictPages[0]=MakeRadioButton(self.wBoard[page],0,0,PATH_SHOP+"pages/","page_1_norm.png","page_1_over.png","page_1_down.png")
			self.dictPages[0].SetToolTipText(localeinfo.OFFLINESHOP_RENEWAL_INV1)
			
			self.dictPages[1]=MakeRadioButton(self.wBoard[page],0,0,PATH_SHOP+"pages/","page_2_norm.png","page_2_over.png","page_2_down.png")
			self.dictPages[1].SetToolTipText(localeinfo.OFFLINESHOP_RENEWAL_INV2)
			
			for idxPage in xrange(len(self.dictPages)):
				self.dictPages[idxPage].SetEvent(ui.__mem_func__(self.SetPageGrid),idxPage)
			
			self.dictPages[0].Down()
		elif page==PAGE_SAFEBOX:
			self.btnGoBack=ui.MakeButton(self.wBoard[page],315,35,localeinfo.OFFLINESHOP_RENEWAL_GOBACK,PATH_ROOT_BUILD,"btn_cosmetic_norm.dds","btn_cosmetic_hover.dds","btn_cosmetic_down.dds")
			self.btnGoBack.SetEvent(ui.__mem_func__(self.ManageWindow))
			
			self.btnWithdrawYang=ui.MakeButton(self.wBoard[page],190,355,False,PATH_ROOT_SEARCH,"btn_filter_norm.dds","btn_filter_hover.dds","btn_filter_down.dds")
			self.btnWithdrawYang.SetText(localeinfo.OFFLINESHOP_RENEWAL_WITHDRAW)
			
			self.btnWithdrawYang.SetEvent(ui.__mem_func__(self.WithdrawMoney))
			
			self.wndYangField=ui.MakeImageBox(self.wBoard[page],PATH_ROOT+"field_yang.dds",30,353)
			self.wndTextYang=ui.MakeTextLineNew(self.wndYangField,8,6," Yang")
			self.wndTextYang.SetWindowHorizontalAlignRight()
			self.wndTextYang.SetHorizontalAlignRight()
		elif page==PAGE_HISTORY:
			self.btnHistory2=ui.MakeButton(self.wBoard[page],23,35,localeinfo.OFFLINESHOP_RENEWAL_GOBACK,PATH_ROOT_BUILD,"btn_cosmetic_norm.dds","btn_cosmetic_hover.dds","btn_cosmetic_down.dds")
			self.btnHistory2.SetEvent(ui.__mem_func__(self.ManageHistoryWindow))
			
			self.wndHistory=uiofflineshop_history.HistoryWindow(self)
			self.wndHistory.SetParent(self.wBoard[page])
			#self.wndHistory.SetSize(303,336 - 89+100)
			#self.wndHistory.SetSize(369,410)
			self.wndHistory.SetSize(369,320)
			self.wndHistory.SetPosition(8,80)
			self.wndHistory.Hide()
			self.wndHistory.SetInsideRender(True)
			
			self.HistoryScrollBar=ui.ScrollBarNewNew()
			self.HistoryScrollBar.SetParent(self.wBoard[page])
			self.HistoryScrollBar.SetScrollBarSize(311)
			self.HistoryScrollBar.SetPosition(369-20,90)
			self.wndHistory.SetScrollBar(self.HistoryScrollBar)
			self.HistoryScrollBar.Show()
			
			self.wndInfo=ui.MakeImageBox(self.wBoard[page],PATH_ROOT+"bar_complete_new.tga",8,64)
			self.wndItemNameHistory=ui.MakeTextLineNew(self.wBoard[page],46,70,localeinfo.OFFLINESHOP_RENEWAL_ITEMNAME)
			self.wndTimeHistory=ui.MakeTextLineNew(self.wBoard[page],160,70,localeinfo.OFFLINESHOP_RENEWAL_ITEMREASON)
			self.wndPriceHistory=ui.MakeTextLineNew(self.wBoard[page],280,70,localeinfo.OFFLINESHOP_RENEWAL_ITEMPRICE)
			
			self.wndNoSellHistory=ui.MakeTextLineNew(self.wBoard[page],0,200,localeinfo.OFFLINESHOP_RENEWAL_EMPTY_HISTORY)
			self.wndNoSellHistory.SetWindowHorizontalAlignCenter()
			self.wndNoSellHistory.SetHorizontalAlignCenter()
			self.wndNoSellHistory.Show()

	def OnWheelHistory(self, nLen):
		if not self.HistoryScrollBar:
			return
		
		if nLen > 0:
			self.HistoryScrollBar.OnUp()
		else:
			self.HistoryScrollBar.OnDown()

	def OpenSearch(self):
		if self.wndSearchShop.IsShow()==FALSE:
			self.wndSearchShop.Show()
		else:
			self.wndSearchShop.Hide()
	
	def ManageHistoryWindow(self):
		if self.wBoard[PAGE_MYSHOP].IsShow():
			self.__ChangePage(PAGE_HISTORY)
		else:
			self.__ChangePage(PAGE_MYSHOP)
			self.RefreshItemSlot()
	
	def ManageWindow(self):
		if self.wBoard[PAGE_SAFEBOX].IsShow():
			offlineshop.SendCloseBoard()
			offlineshop.SendOpenShopOwner()
			self.SetPageGrid(0)
		else:
			offlineshop.SendCloseBoard()
			offlineshop.SendSafeboxOpen()
			self.SetPageGrid(0)

	def SetPageGrid(self,page):
		if self.wBoard[PAGE_HISTORY].IsShow():
			return
		
		if page < 0 or page > 1:
			return
		
		self.PageGrid=page
		
		for idxPage in xrange(len(self.dictPages)):
			if idxPage==page:
				self.dictPages[idxPage].Down()
			else:
				self.dictPages[idxPage].SetUp()
		
		self.RefreshItemSlot()

	if not app.DISABLE_OFFLINESHOP_CHANGENAME:
		def ChangeNameShop(self):
			if self.MyShopEditNameDlg==0:
				self.MyShopEditNameDlg	= uicommon.InputDialogWithDescription()
				self.MyShopEditNameDlg.SetMaxLength(35)
				self.MyShopEditNameDlg.SetDescription(localeinfo.OFFLINESHOP_EDIT_SHOPNAME_DESCRIPTION)
				self.MyShopEditNameDlg.SetAcceptEvent(self.__OnAcceptChangeShopNameDlg)
				self.MyShopEditNameDlg.SetCancelEvent(self.__OnCancelChangeShopNameDlg)
				self.MyShopEditNameDlg.SetTitle(localeinfo.OFFLINESHOP_EDIT_SHOPNAME_TITLE)
			
			self.MyShopEditNameDlg.inputValue.SetText("")
			self.MyShopEditNameDlg.Open()

		def __OnAcceptChangeShopNameDlg(self):
			newname=self.MyShopEditNameDlg.GetText()
			self.MyShopEditNameDlg.Hide()
			
			offlineshop.SendChangeName(newname)

		def __OnCancelChangeShopNameDlg(self):
			self.MyShopEditNameDlg.Hide()

	def OnPressNameEscapeKey(self):
		if self.OnPressEscapeKey():
			return
		
		if self.NameEdit.IsShowCursor() or self.NameEdit.GetText() != "":
			self.NameEdit.SetText("")
			self.NameEdit.SetEndPosition()

	def RefreshOpenShopPage(self):
		wasOpen=(self.IsShow() and self.wBoard[PAGE_SHOP].IsShow())
		
		name=self.wndShop.ShopOpenInfo["name"]
		ownerName= name[:name.find('@')] if '@' in name else "NONAME"
		name= name[name.find('@')+1:] if '@' in name else name
		
		if self.ShopOpenID != self.wndShop.ShopOpenInfo["owner_id"]:
			self.SetPageGrid(0)
		
		self.ShopOpenID=self.wndShop.ShopOpenInfo["owner_id"]
		
		self.dictName[PAGE_SHOP].SetText(ownerName+localeinfo.OFFLINESHOP_RENEWAL_SHOP)
		self.__ChangePage(PAGE_SHOP)
		
		self.RefreshItemSlot()
		
		if len(self.wndShop.ShopItemForSale) <= 0:
			self.Close()
			return

	def RefreshMyShopPage(self):
		wasOpen=(self.wBoard[PAGE_MYSHOP].IsShow())
		self.__ChangePage(PAGE_MYSHOP)
		
		wndRemainTimeLeft=self.wndShop.ShopOpenInfo["duration"]
		self.wndRemainTime.SetText(localeinfo.OFFLINESHOP_RENEWAL_LEFTTIME2%(localeinfo.SecondToDHM(wndRemainTimeLeft * 60)))
		
		name=self.wndShop.ShopOpenInfo["name"]
		
		if '@' in name:
			name=name[name.find('@')+1:]
		
		self.ShopName=name
		self.NameEdit.SetText(self.ShopName)
		
		self.RefreshItemSlot()
		
		self.wndHistory.Clear()
		self.HistoryScrollBar.Hide()
		
		if len(self.wndShop.ShopItemSold)==0:
			self.wndNoSellHistory.Show()
		else:
			self.wndNoSellHistory.Hide()
		
		for idx,item in enumerate(self.wndShop.ShopItemSold):
			self.wndHistory.AppendItem(idx,item)

	def ResetCreateShopPage(self):
		self.ClearStockBuilder()
		self.__ChangePage(PAGE_BUILDER)
		self.RefreshItemSlot()

	def RefreshSafeboxPage(self):
		wasOpen=(self.wBoard[PAGE_SAFEBOX].IsShow())
		self.__ChangePage(PAGE_SAFEBOX)
		self.RefreshItemSlot()

	def SetPrivateShopBuilderItemNew(self,invenType,invenPos,price):
		itemVnum=player.GetItemIndex(invenType,invenPos)
		if 0==itemVnum:
			return
		
		item.SelectItem(itemVnum)
		self.tooltipItem.ClearToolTip()
		self.tooltipItem.AppendSellingPrice(price)
		
		metinSlot=[]
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(player.GetItemMetinSocket(invenPos,i))
		
		attrSlot=[]
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(player.GetItemAttribute(invenPos,i))
		
		self.tooltipItem.AddItemData(itemVnum,metinSlot,attrSlot,transmutation=player.GetItemTransmutation(invenPos,i) if app.ENABLE_CHANGELOOK else 0)

	def OverInItem(self,slotIndex):
		if 0==self.tooltipItem:
			return
		
		if self.wBoard[PAGE_BUILDER].IsShow():
			if slotIndex in self.itemStockBuilder[self.PageGrid]:
				window,pos,price=self.itemStockBuilder[self.PageGrid][slotIndex]
				self.SetPrivateShopBuilderItemNew(window,pos,price)
		elif slotIndex in self.itemStock:
			id,vnum,count,trans,price,sockets,attrs=self.itemStock[slotIndex]
			
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.AddItemData(vnum,sockets,attrs,transmutation=trans)
			
			if self.wBoard[PAGE_SAFEBOX].IsShow()==FALSE:
				self.tooltipItem.AppendPrice(price)
			
			if self.wBoard[PAGE_MYSHOP].IsShow():
				self.tooltipItem.AppendSpace(5)
				self.tooltipItem.AppendTextLine(localeinfo.OFFLINESHOP_RENEWAL_ITEM_REMOVE)
				
				self.tooltipItem.AppendSpace(5)
				self.tooltipItem.AppendTextLine(localeinfo.OFFLINESHOP_RENEWAL_ITEM_EDIT)
			elif self.wBoard[PAGE_SAFEBOX].IsShow():
				self.tooltipItem.AppendSpace(5)
				self.tooltipItem.AppendTextLine(localeinfo.OFFLINESHOP_RENEWAL_ITEM_WITHDRAW)

	def OverOutItem(self):
		if 0 != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def GetEmptyPosition(self,size):
		for page in xrange(2):
			iPos=self.itemSlot[page].GetEmptyGrid(size)
			if iPos >= 0:
				return (page,iPos)
		
		return (-1,-1)

	def GetGridCachePosItemID(self,size,item_id):
		for page in xrange(len(self.itemSlot)):
			if not CACHE_ITEMS_ID.has_key(page):
				continue
			
			if CACHE_ITEMS_ID[page].has_key(item_id):
				return (page,CACHE_ITEMS_ID[page][item_id])
		
		return self.GetEmptyPosition(size)

	def RefreshItemSlot(self):
		items=[]
		if self.wBoard[PAGE_MYSHOP].IsShow() or self.wBoard[PAGE_SHOP].IsShow():
			items=self.wndShop.ShopItemForSale
		elif self.wBoard[PAGE_SAFEBOX].IsShow():
			items=self.wndShop.ShopSafeboxItems
		
		for itemSlotIdx in xrange(len(self.itemSlot)):
			for index in xrange(OFFLINESHOP_SLOT_COUNT):
				self.itemSlot[itemSlotIdx].ClearSlot(index)
			
			self.itemSlot[itemSlotIdx].ClearGrid()
			self.itemSlot[itemSlotIdx].Hide()
		
		self.itemSlot[self.PageGrid].Show()
		
		TotalPrice=0
		self.itemStock={}
		
		if self.wBoard[PAGE_BUILDER].IsShow():
			for page in xrange(2):
				for i in xrange(OFFLINESHOP_SLOT_COUNT):
					if not i in self.itemStockBuilder[page]:
						self.itemSlot[page].ClearSlot(i)
						continue
					
					window,pos,price=self.itemStockBuilder[page][i]
					vnum=player.GetItemIndex(window,pos)
					if vnum <= 0:
						continue
					
					self.itemSlot[page].SetItemSlot(i,vnum,player.GetItemCount(window,pos))
					if app.ENABLE_CHANGELOOK:
						if player.GetItemTransmutation(window,pos):
							self.itemSlot[page].DisableCoverButton(i)
						else:
							self.itemSlot[page].EnableCoverButton(i)
					
					item.SelectItem(vnum)
					_,size=item.GetItemSize()
					self.itemSlot[page].PutItemGrid(i,size)
			
			self.itemSlot[self.PageGrid].RefreshSlot()
			return
		
		for index in xrange(len(items)):
			itemVnum=items[index]["vnum"]
			itemCount=items[index]["count"]
			
			if itemVnum <= 0:
				continue
			
			item.SelectItem(itemVnum)
			_,size=item.GetItemSize()
			
			PageItemSlot,iPos=self.GetGridCachePosItemID(size,items[index]["id"])
			
			if iPos < 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_RENEWAL_WARNING)
				return
			
			self.itemSlot[PageItemSlot].PutItemGrid(iPos,size)
			
			itemCountGrid=itemCount if itemCount > 1 else 0
			self.itemSlot[PageItemSlot].SetItemSlot(iPos,itemVnum,itemCountGrid)
			if app.ENABLE_CHANGELOOK:
				if items[index]["trans"]:
					self.itemSlot[PageItemSlot].DisableCoverButton(iPos)
				else:
					self.itemSlot[PageItemSlot].EnableCoverButton(iPos)
			
			sockets=[items[index]["socket"][num] for num in xrange(player.METIN_SOCKET_MAX_NUM)]
			attrs=[(items[index]["attr"][num]['type'],items[index]["attr"][num]['value']) for num in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]	
			
			ItemPrice=0
			if items[index].has_key("price"):
				ItemPrice=int(items[index]["price"])
				TotalPrice += int(items[index]["price"])
			
			transmutation=0
			if items[index].has_key("trans"):
				transmutation=items[index]["trans"]
			
			if PageItemSlot==self.PageGrid:
				self.itemStock[iPos]=(items[index]["id"],itemVnum,itemCount,transmutation,ItemPrice,sockets,attrs)
			
			if not CACHE_ITEMS_ID.has_key(PageItemSlot):
				CACHE_ITEMS_ID[PageItemSlot]={}
			
			CACHE_ITEMS_ID[PageItemSlot][items[index]["id"]]=iPos
		
		self.wndTotalPrice.SetText(localeinfo.OFFLINESHOP_RENEWAL_TOTAL2%localeinfo.NumberToMoneyString(TotalPrice))
		
		if self.PriceInputBoard and self.wBoard[PAGE_MYSHOP].IsShow():
			CurrentPageGrid=self.PriceInputBoard.CurrentPageGrid
			targetSlotPos=self.PriceInputBoard.targetSlotPos
			isEditMode=self.PriceInputBoard.isEditMode
		
		self.itemSlot[self.PageGrid].RefreshSlot()

	def SelectItemSlot(self,itemSlotIndex):
		if constinfo.GET_ITEM_QUESTION_DIALOG_STATUS()==1:
			return
		
		if mousemodule.mouseController.isAttached():
			mousemodule.mouseController.DeattachObject()
		
		if self.itemStock.has_key(itemSlotIndex)==False:
			return
		
		if self.wBoard[PAGE_BUILDER].IsShow():
			return
		
		id,vnum,count,trans,price,sockets_v,attrs_s=self.itemStock[itemSlotIndex]
		if self.wBoard[PAGE_MYSHOP].IsShow() or self.wBoard[PAGE_SAFEBOX].IsShow():
			if app.IsPressed(app.DIK_LALT):
				sockets={}
				for num in xrange(player.METIN_SOCKET_MAX_NUM):
					sockets[num]=(int(sockets_v[num]),1)
				
				attrs={}
				for num in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					attrs[num]=(int(attrs_s[num][0]),int(attrs_s[num][1]))
				
				link=player.GetItemShopOfflineLink(vnum,sockets,attrs)
				ime.PasteString(link)

	def UseItemSlot(self,slotIndex):
		if constinfo.GET_ITEM_QUESTION_DIALOG_STATUS()==1:
			return
		
		if mousemodule.mouseController.isAttached():
			mousemodule.mouseController.DeattachObject()
		
		if self.QuestionDialog:
			self.OnCloseQuestionDialog()
		
		if self.wBoard[PAGE_BUILDER].IsShow():
			if slotIndex in self.itemStockBuilder[self.PageGrid]:
				invenType,invenPos,price=self.itemStockBuilder[self.PageGrid][slotIndex]
				del self.itemStockBuilder[self.PageGrid][slotIndex]
				
				self.RefreshItemSlot()
				self.RefreshItemSlotInv()
			
			return
		else:
			if self.itemStock.has_key(slotIndex)==False:
				return
			
			id,vnum,count,trans,price,sockets,attrs=self.itemStock[slotIndex]
			
			if self.wBoard[PAGE_SAFEBOX].IsShow():
				offlineshop.SendSafeboxGetItem(id)
				self.ClearCachePos(self.PageGrid,id)
				return
			
			if self.wBoard[PAGE_MYSHOP].IsShow() and (app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL))==FALSE:
				self.OpenInputPriceDialog(vnum,count,id,slotIndex,True)
				return
			
			item.SelectItem(vnum)
			QuestionDialog=uicommon.QuestionDialog()
			
			if self.wBoard[PAGE_SHOP].IsShow():
				if count > 1:
					text=localeinfo.OFFLINESHOP_RENEWAL_BUY1%(item.GetItemName(),count,localeinfo.NumberToMoneyString(price))
				else:
					text=localeinfo.OFFLINESHOP_RENEWAL_BUY2%(item.GetItemName(),localeinfo.NumberToMoneyString(price))
			elif self.wBoard[PAGE_MYSHOP].IsShow():
				offlineshop.SendRemoveItem(id)
				self.ClearCachePos(self.PageGrid,id)
				return
			else:
				return
			
			QuestionDialog.SetText(text)
			QuestionDialog.SetAcceptEvent(ui.__mem_func__(self.OnCancelShopItem))
			QuestionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			QuestionDialog.Open()
			self.QuestionDialog=QuestionDialog
			self.QuestionDialog.slotIndex=slotIndex
			self.QuestionDialog.itemStock=self.itemStock[slotIndex]
		
		constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def OnCancelShopItem(self):
		if not self.QuestionDialog:
			return True
		
		id,vnum,count,trans,price,sockets,attrs=self.QuestionDialog.itemStock
		
		if self.wBoard[PAGE_MYSHOP].IsShow():
			offlineshop.SendRemoveItem(id)
			self.ClearCachePos(self.PageGrid,id)
		elif self.wBoard[PAGE_SHOP].IsShow():
			offlineshop.SendBuyItem(self.wndShop.ShopOpenInfo["owner_id"],id,price)
		else:
			offlineshop.SendSafeboxGetItem(id)
			self.ClearCachePos(self.PageGrid,id)
		
		constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)
		self.OnCloseQuestionDialog()
		return True

	def OnCloseQuestionDialog(self):
		if self.QuestionDialog==0:
			return
		
		self.QuestionDialog.Close()
		
		self.QuestionDialog.itemStock=0
		self.QuestionDialog=0
		constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def SelectEmptySlot(self,selectedSlotPos):
		if self.wBoard[PAGE_SAFEBOX].IsShow() or self.wBoard[PAGE_SHOP].IsShow():
			self.OverOutItem()
			if mousemodule.mouseController.isAttached():
				mousemodule.mouseController.DeattachObject()
			return
		
		isAttached=mousemodule.mouseController.isAttached()
		if isAttached:
			attachedSlotType=mousemodule.mouseController.GetAttachedType()
			attachedSlotPos=mousemodule.mouseController.GetAttachedSlotNumber()
			mousemodule.mouseController.DeattachObject()
			
			attachedInvenType=player.SlotTypeToInvenType(attachedSlotType)
			self.OpenInputPriceDialog(attachedInvenType,attachedSlotPos,self.PageGrid,selectedSlotPos)

	def OpenInputPriceDialog(self,window,slot,page,EmptyGridPos,editMode=False,skipDlg=False):
		if self.PriceInputBoard:
			return
		
		if editMode==False:
			itemVNum=player.GetItemIndex(window,slot)
			itemCount=player.GetItemCount(window,slot)
			item.SelectItem(itemVNum)
			if item.IsAntiFlag(item.ANTIFLAG_GIVE) or item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
				chat.AppendChat(chat.CHAT_TYPE_INFO,localeinfo.PRIVATE_SHOP_CANNOT_SELL_ITEM)
				return
		else:
			itemVNum=window
			itemCount=slot
		
		PriceInputBoard=uicommon.MoneyInputDialogNew(itemVNum)
		PriceInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptInputPrice))
		PriceInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelInputPrice))
		PriceInputBoard.Open()
		
		self.PriceInputBoard=PriceInputBoard
		self.PriceInputBoard.itemVNum=itemVNum
		self.PriceInputBoard.sourceWindowType=window
		self.PriceInputBoard.sourceSlotPos=slot
		self.PriceInputBoard.targetSlotPos=EmptyGridPos
		self.PriceInputBoard.page=page
		self.PriceInputBoard.CurrentPageGrid=self.PageGrid
		self.PriceInputBoard.isEditMode=editMode
		
		if editMode==False:
			price=self.LoadInputPrice(player.GetItemIndex(window,slot))
			count=player.GetItemCount(window,slot)
			if price > 0:
				if count > 1:
					self.PriceInputBoard.SetValue(str(price*count))
				else:
					self.PriceInputBoard.SetValue(str(price))
				
				self.PriceInputBoard.inputValue.SetEndPosition()
		
		self.RefreshItemSlotInv()
		if skipDlg:
			self.AcceptInputPrice()

	def AcceptInputPrice(self):
		if not self.PriceInputBoard:
			return True
		
		Text=self.PriceInputBoard.inputValue.GetText()
		if not Text:
			return True
		
		if int(Text) <= 0:
			return True
		
		attachedInvenType=self.PriceInputBoard.sourceWindowType
		sourceSlotPos=self.PriceInputBoard.sourceSlotPos
		targetSlotPos=self.PriceInputBoard.targetSlotPos
		targetPage=self.PriceInputBoard.page
		isEditMode=self.PriceInputBoard.isEditMode
		
		yang=long(Text)
		count=player.GetItemCount(attachedInvenType,sourceSlotPos)
		if count > 1:
			yang=yang/count
		else:
			yang
		
		self.SaveInputPrice(player.GetItemIndex(attachedInvenType,sourceSlotPos),yang)
		self.OverOutItem()
		
		if self.wBoard[PAGE_BUILDER].IsShow():
			for page in xrange(2):
				for privatePos,(itemWindowType,itemSlotIndex,priceInfo) in self.itemStockBuilder[page].items():
					if itemWindowType==attachedInvenType and itemSlotIndex==sourceSlotPos:
						del self.itemStockBuilder[page][privatePos]
			
			self.itemStockBuilder[targetPage][targetSlotPos]=(attachedInvenType,sourceSlotPos,long(Text))
		else:
			if isEditMode:
				offlineshop.SendEditItem(self.PriceInputBoard.page,long(Text))
			else:
				offlineshop.SendAddItem(attachedInvenType, sourceSlotPos,long(Text))
		
		self.PriceInputBoard=0
		self.RefreshItemSlot()
		self.RefreshItemSlotInv()
		return True

	def RefreshItemSlotInv(self,isDestroy=False):
		if isDestroy:
			return
		
		if self.interface==0:
			return
		
		if self.interface.wndInventory:
			self.interface.wndInventory.RefreshItemSlot()

	def CancelInputPrice(self,isDestroy=False):
		if self.PriceInputBoard:
			self.PriceInputBoard.Destroy()
		
		self.PriceInputBoard=0
		
		if not isDestroy:
			self.RefreshItemSlot()
			self.RefreshItemSlotInv()
		
		return True

	def SaveInputPrice(self,vnum,price):
		import os
		path="offlineshop"
		
		if not os.path.exists(path):
			os.makedirs(path)
		
		n=path+"/"+str(vnum)+".txt"
		f=file(n,"w+")
		f.write(str(price))
		f.close()

	def LoadInputPrice(self,vnum):
		import os
		path="offlineshop"
		
		if not os.path.exists(path):
			os.makedirs(path)
		
		oldPrice=0
		n=path+"/"+str(vnum)+".txt"
		
		if os.path.exists(n):
			fd=open( n,'r')
			oldPrice=int(fd.readlines()[0])
		
		return oldPrice

	def AskClosePrivateShop(self):
		if self.QuestionDialog:
			self.OnCloseQuestionDialog()
		
		if not self.QuestionDialog:
			QuestionDialog=uicommon.QuestionDialog()
			QuestionDialog.SetText(localeinfo.PRIVATE_SHOP_CLOSE_QUESTION)
			QuestionDialog.SetAcceptEvent(ui.__mem_func__(self.OnClosePrivateShop))
			QuestionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			self.QuestionDialog=QuestionDialog
		
		self.QuestionDialog.Open()
		return True
	
	def OnClosePrivateShop(self):
		offlineshop.SendForceCloseShop()
		
		global CACHE_ITEMS_ID
		CACHE_ITEMS_ID={}
		
		constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)
		self.OnCloseQuestionDialog()
		return True

	def WithdrawMoney(self):
		if self.WithdrawQuestionDialog:
			return
		
		shopMoney=self.wndShop.ShopSafeboxValuteAmount
		if shopMoney <= 0:
			return
		
		WithdrawQuestionDialog=uicommon.MoneyInputDialog()
		WithdrawQuestionDialog.SetTitle(localeinfo.EXCHANGE_MONEY)
		WithdrawQuestionDialog.SetMoneyHeaderText("Sum: ")
		WithdrawQuestionDialog.SetAcceptEvent(ui.__mem_func__(self.SumWithdraw)) 
		WithdrawQuestionDialog.SetCancelEvent(ui.__mem_func__(self.CloseSumWithdraw))
		WithdrawQuestionDialog.Open()
		WithdrawQuestionDialog.SetMaxLength(len(str(shopMoney)))
		WithdrawQuestionDialog.SetValue(shopMoney)
		WithdrawQuestionDialog.inputValue.SetEndPosition()	
		self.WithdrawQuestionDialog=WithdrawQuestionDialog

	def SumWithdraw(self):
		if not self.WithdrawQuestionDialog:
			return
		
		shopMoney=int(self.WithdrawQuestionDialog.GetText())
		if len(self.WithdrawQuestionDialog.GetText()) <= 0:
			return
		
		if int(self.WithdrawQuestionDialog.GetText()) > self.wndShop.ShopSafeboxValuteAmount:
			shopMoney=self.wndShop.ShopSafeboxValuteAmount
		
		offlineshop.SendSafeboxGetValutes(shopMoney)
		
		self.WithdrawQuestionDialog.Close()
		self.WithdrawQuestionDialog=0

	def CloseSumWithdraw(self):
		if not self.WithdrawQuestionDialog:
			return False
		
		self.WithdrawQuestionDialog.Close()
		self.WithdrawQuestionDialog=0
		return True

	def OnCreateShop(self):
		if not self.NameEdit.GetText():
			return
		
		if 0==len(self.itemStockBuilder):
			return
		
		itemLst=[]
		
		for page in xrange(2):
			for privatePos,(itemWindowType,itemSlotIndex,itemPrice) in self.itemStockBuilder[page].items():
				tupleinfo=(itemWindowType,itemSlotIndex,itemPrice)
				itemLst.append(tupleinfo)
		
		if (len(itemLst)==0):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_RENEWAL_WARNING2)
			return
		
		totaltime=7200
		
		itemTuple=tuple(itemLst)
		offlineshop.SendShopCreate(self.NameEdit.GetText(),totaltime,itemTuple)

	def OnUpdate(self):
		if self.wBoard[PAGE_SHOP].IsShow():
			USE_SHOP_LIMIT_RANGE=1500
			(x,y,z)=player.GetMainCharacterPosition()
			if abs(x - self.xShopStart) > USE_SHOP_LIMIT_RANGE or abs(y - self.yShopStart) > USE_SHOP_LIMIT_RANGE:
				self.Close()

	def AskIncreaseTimeShop(self):
		if self.QuestionDialog:
			self.OnCloseQuestionDialog()
		
		if not self.QuestionDialog:
			QuestionDialog=uicommon.QuestionDialog()
			QuestionDialog.SetText(localeinfo.OFFLINESHOP_RENEWAL_EXPANDTIME_DLG)
			QuestionDialog.SetAcceptEvent(ui.__mem_func__(self.OnRefillShopTime))
			QuestionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			self.QuestionDialog=QuestionDialog
		
		self.QuestionDialog.Open()
		return True

	def GetMaxRefill(self,ex_day):
		while True:
			if self.wndShop.ShopOpenInfo["duration"]+ex_day > 7200:
				ex_day -= 1
				continue
			
			break
		
		return ex_day

	def OnRefillShopTime(self):
		Extend1Day=self.GetMaxRefill(1440)
		if Extend1Day <= 0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.OFFLINESHOP_RENEWAL_WARNING3)
			self.OnCloseQuestionDialog()
			return
		
		offlineshop.SendExtendTime(Extend1Day)
		constinfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)
		self.OnCloseQuestionDialog()
		return True

	def ClearCachePos(self,page,id):
		if CACHE_ITEMS_ID.has_key(page):
			if CACHE_ITEMS_ID[page].has_key(id):
				del CACHE_ITEMS_ID[page][id]
