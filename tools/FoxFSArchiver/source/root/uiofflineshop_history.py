import app
import player
import net
import offlineshop
import ui
import item

import uitooltip
import localeinfo
import ime
import uipickmoney
import grp
import mousemodule

from _weakref import proxy

PATH_ROOT="cream/shopeditor/"
PATH_ROOT_SEARCH="cream/searchshop/"

class HistoryWindow(ui.Window):
	class NewItem(ui.Window):
		def __init__(self,index,data):
			ui.Window.__init__(self)
			self.background=0
			self.bSelected=False
			self.OnResetThings()
			self.Index=index
			self.Data=data
			self.background=ui.ExpandedImageBox()
			self.background.SetParent(self)
			
			if self.Index != 0:
				self.background.LoadImage(PATH_ROOT+"bar_without_top_new.tga")
			else:
				self.background.LoadImage(PATH_ROOT+"bar_complete_new.tga")
			
			self.background.OnMouseOverIn=ui.__mem_func__(self.OnMouseOverInImage)
			self.background.OnMouseOverOut=ui.__mem_func__(self.OnMouseOverOutImage)
			self.background.Show()
			
			self.SetSize(self.background.GetWidth(),26)
			
			item.SelectItem(data["vnum"])
			
			itemName=item.GetItemName()
			itemName=itemName if len(itemName) < 15 else itemName[:15]+"..."
			
			self.wndNameBG=ui.Window()
			self.wndNameBG.SetParent(self.background)
			self.wndNameBG.SetSize(90,20)
			self.wndNameBG.SetPosition(6,4)
			self.wndNameBG.Show()
			self.wndNameBG.OnMouseOverIn=ui.__mem_func__(self.OnMouseOverInImage)
			self.wndNameBG.OnMouseOverOut=ui.__mem_func__(self.OnMouseOverOutImage)
			self.wndName=ui.MakeTextLineNew(self,0,2,self.HasAttrItem(data)+itemName)
			self.wndName.SetParent(self.wndNameBG)
			self.wndName.SetWindowHorizontalAlignCenter()
			self.wndName.SetHorizontalAlignCenter()
			
			self.wndReasonBG=ui.Window()
			self.wndReasonBG.SetParent(self.background)
			self.wndReasonBG.SetSize(70,20)
			self.wndReasonBG.SetPosition(128,4)
			self.wndReasonBG.Show()
			self.wndReasonBG.OnMouseOverIn=ui.__mem_func__(self.OnMouseOverInImage)
			self.wndReasonBG.OnMouseOverOut=ui.__mem_func__(self.OnMouseOverOutImage)
			self.wndReason=ui.MakeTextLineNew(self,0,2,localeinfo.OFFLINESHOP_RENEWAL_ITEM_BOUGHT)
			self.wndReason.SetParent(self.wndReasonBG)
			self.wndReason.SetWindowHorizontalAlignCenter()
			self.wndReason.SetHorizontalAlignCenter()
			
			self.wndPriceBG=ui.Window()
			self.wndPriceBG.SetParent(self.background)
			self.wndPriceBG.SetSize(120,20)
			self.wndPriceBG.SetPosition(220,4)
			self.wndPriceBG.Show()
			self.wndPriceBG.OnMouseOverIn=ui.__mem_func__(self.OnMouseOverInImage)
			self.wndPriceBG.OnMouseOverOut=ui.__mem_func__(self.OnMouseOverOutImage)
			self.wndPrice=ui.MakeTextLineNew(self,0,2,localeinfo.NumberToMoneyString(data["price"]))
			self.wndPrice.SetParent(self.wndPriceBG)
			self.wndPrice.SetWindowHorizontalAlignCenter()
			self.wndPrice.SetHorizontalAlignCenter()
			
			self.OnMouseOverOutImage()

		def HasAttrItem(self,Data):
			attrs=[(Data["attr"][num]['type'],Data["attr"][num]['value']) for num in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			if attrs:
				for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					apply=attrs[i][0]
					if 0 > apply:
						return "|cFFFFC700"
			
			return "|cFFF1E6C0"

		def __del__(self):
			ui.Window.__del__(self)

		def OnMouseOverInImage(self):
			if self.tooltipItem:
				sockets=[self.Data["socket"][num] for num in xrange(player.METIN_SOCKET_MAX_NUM)]
				attrs=[(self.Data["attr"][num]['type'],self.Data["attr"][num]['value']) for num in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
				
				self.tooltipItem.ClearToolTip()
				
				self.tooltipItem.AppendIcon(self.Data["vnum"],self.Data["count"])
				self.tooltipItem.AddItemData(self.Data["vnum"],sockets,attrs)
				
				self.tooltipItem.UpdateRect()
			
			self.background.SetAlpha(1.0)

		def OnMouseOverOutImage(self):
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()
			
			self.background.SetAlpha(0.8)

		def SetItemToolTip(self,tooltip):
			self.tooltipItem=tooltip

		def OnResetThings(self):
			self.IsSelected=False
			self.bIsBlocked=False
			self.vnum=0
			self.xBase=0
			self.yBase=0
			self.clickEvent=0
			if self.background != 0:
				self.background.Hide()
			
			self.background=0
			self.tooltipItem=0
			self.Data=0

		def SetParent(self,parent):
			ui.Window.SetParent(self,parent)
			self.parent=proxy(parent)

		def SetBasePosition(self,x,y):
			self.xBase=x
			self.yBase=y

		def GetBasePosition(self):
			return (self.xBase,self.yBase)

		def SetClickEvent(self,event):
			self.clickEvent=event

		def Destroy(self):
			self.OnResetThings()

		def OnRender(self):
			xList,yList=self.parent.GetGlobalPosition()
			widthList,heightList=self.parent.GetWidth(),self.parent.GetHeight()
			
			textList=[self.wndName,self.wndPrice,self.wndReason]
			for text in textList:
				if text:
					xText,yText=text.GetGlobalPosition()
					if yText < yList or yText+text.GetTextSize()[1] > yList+heightList:
						text.Hide()
						#self.background.Hide()
					else:
						text.Show()
						#self.background.Show()

	def __init__(self, parent):
		ui.Window.__init__(self)
		self.parent=parent
		self.OnResetThings()

	def __del__(self):
		ui.Window.__del__(self)
		self.OnResetThings()

	def SetItemToolTip(self,tooltip):
		self.tooltipItem=tooltip

	def Destroy(self):
		self.OnResetThings()

	def OnResetThings(self):
		self.SetFuncDown=0
		self.SelectIndexFunc=0
		self.itemList=[]
		self.scrollBar=0
		self.selectEvent=0
		self.tooltipItem=0

	def SetEventSelect(self,event):
		self.SelectIndexFunc=event

	def SetScrollBar(self,scrollBar):
		scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScroll))
		scrollBar.SetScrollStep(0.2)
		self.scrollBar=scrollBar

	def SetSelectEvent(self,event):
		self.selectEvent=event

	def __OnScroll(self):
		self.RefreshItemPosition(True)

	def GetTotalHeightItems(self):
		totalHeight=0
		if self.itemList:
			for itemH in self.itemList:
				totalHeight += itemH.GetHeight()
		
		return totalHeight

	def GetItemCount(self):
		return len(self.itemList)

	def AppendItem(self,index,data):
		item=self.NewItem(index,data)
		item.SetParent(self)
		item.SetItemToolTip(self.tooltipItem)
		
		if len(self.itemList) == 0:
			item.SetBasePosition(0,10)
		else:
			x,y=self.itemList[-1].GetLocalPosition()
			y += 2
			item.SetBasePosition(0,10+y+self.itemList[-1].GetHeight())
		
		item.Show()
		self.itemList.append(item)
		
		self.ResetPosScrollBar()
		self.ResizeScrollBar()
		self.RefreshItemPosition()
		
		if len(self.itemList) <= 12:
			self.scrollBar.Hide()
		else:
			self.scrollBar.Show()

	def ResizeScrollBar(self):
		totalHeight=float(self.GetTotalHeightItems())
		if totalHeight:
			scrollBarHeight=min(float(self.GetHeight() - 10) / totalHeight,1.0)
		else:
			scrollBarHeight=1.0
		
		self.scrollBar.SetMiddleBarSize(scrollBarHeight)
	
	def ResetPosScrollBar(self):
		self.scrollBar.SetPos(0)

	def RefreshItemPosition(self,bScroll=False):
		scrollPos=self.scrollBar.GetPos()
		totalHeight=self.GetTotalHeightItems() - self.GetHeight()
		
		idx,CurIdx,yAccumulate=0,0,0
		for item in self.itemList[idx:]:
			if bScroll:
				setPos=yAccumulate - int(scrollPos * totalHeight)
				item.SetPosition(0,setPos+10)
			else:
				item.SetPosition(0,yAccumulate+10)
			
			item.SetBasePosition(0,yAccumulate+10)

			CurIdx += 1
			yAccumulate += item.GetHeight()

	def Clear(self):
		range=len(self.itemList)
		
		if range > 0:
			for item in self.itemList:
				item.OnResetThings()
				item.Hide()
				del item
		
		self.itemList=[]

	def OnRunMouseWheel(self, nLen):
		if self.IsInPosition():
			self.parent.OnWheelHistory(nLen)
			return True
		
		return False
