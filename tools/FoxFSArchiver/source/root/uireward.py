import app
import player
import net

import ui
import grp
import wndMgr
import item
import newconstinfo
import localeinfo

IMG_DIR = "d:/ymir work/ui/game/reward_info/"
IMG_ICON_DIR = "d:/ymir work/ui/game/reward_info/icons/"

reward_info = {
	# eventName, eventImgFolder, [items]
	player.REWARD_105 : [localeinfo.REWARD_105, "lvl_105",[ [89005,2] ] ],
	# player.REWARD_120 : [localeinfo.REWARD_120, "lvl_120",[ [89005,1] ] ],
	# player.REWARD_PET_115 : [localeinfo.REWARD_PET_115, "pet_115",[ [89001,1] ] ],
	# player.REWARD_LEGENDARY_SKILL : [localeinfo.REWARD_LEGENDARY_SKILL, "legendary_skill",[ [89001,1] ] ],
	# player.REWARD_LEGENDARY_SKILL_SET : [localeinfo.REWARD_LEGENDARY_SKILL_SET, "legendary_skill_set",[ [89001,1] ] ],
	player.REWARD_KILL_DRAGON : [localeinfo.REWARD_KILL_DRAGON, "dragon",[ [89004,1] ] ],
	player.REWARD_KILL_RAZADOR : [localeinfo.REWARD_KILL_RAZADOR, "razador",[ [89004,1] ] ],
	player.REWARD_KILL_NEMERE : [localeinfo.REWARD_KILL_NEMERE, "nemere",[ [89004,1] ] ],
	# player.REWARD_OFFLINESHOP_SLOT : [localeinfo.REWARD_OFFLINESHOP_SLOT, "offlineshop_slot",[ [89001,1] ] ],
	# player.REWARD_INVENTORY_SLOT : [localeinfo.REWARD_INVENTORY_SLOT, "inventory_slot",[ [89001,1] ] ],
	player.REWARD_AVERAGE : [localeinfo.REWARD_AVERAGE, "average",[ [89004,1],[89005,2]] ],
	# player.REWARD_ELEMENT : [localeinfo.REWARD_ELEMENT, "element",[ [89001,1] ] ],
	# player.REWARD_BATTLEPASS : [localeinfo.REWARD_BATTLEPASS, "battlepass",[ [89001,1] ] ],
	# player.REWARD_CUSTOM_SASH : [localeinfo.REWARD_CUSTOM_SASH, "self_sash",[ [89001,1] ] ],
	# player.REWARD_AURA : [localeinfo.REWARD_AURA, "aura",[ [89001,1] ] ],
	# player.REWARD_ENERGY : [localeinfo.REWARD_ENERGY, "energy_crystal",[ [89001,1] ] ],
	# player.REWARD_112_BIO : [localeinfo.REWARD_112_BIO, "112_bio",[ [89001,1] ] ],
	# player.REWARD_120_BIO : [localeinfo.REWARD_120_BIO, "120_bio",[ [89001,1] ] ],
	# player.REWARD_LEADER_SHIP : [localeinfo.REWARD_LEADER_SHIP, "leadership",[ [89001,1] ] ],
	# player.REWARD_BUFFI_LEGENDARY_SKILL : [localeinfo.REWARD_BUFFI_LEGENDARY_SKILL, "buffi_shaman",[ [89001,1] ] ],
}

class ScrollBar(ui.Window):
	SCROLLBAR_WIDTH = 8
	SCROLL_BTN_XDIST = 0
	SCROLL_BTN_YDIST = 5
	class MiddleBar(ui.DragButton):
		def __init__(self):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")
			self.SetWindowName("scrollbar_middlebar")
		def MakeImage(self):
			top = ui.ExpandedImageBox()
			top.SetParent(self)
			top.LoadImage(IMG_DIR+"scroll_bar.tga")
			top.AddFlag("not_pick")
			top.Show()
			bottom = ui.ExpandedImageBox()
			bottom.SetParent(self)
			bottom.LoadImage(IMG_DIR+"scroll_bar.tga")
			bottom.AddFlag("not_pick")
			bottom.Show()
			middle = ui.ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage(IMG_DIR+"scroll_bar.tga")
			middle.AddFlag("not_pick")
			middle.Show()
			self.top = top
			self.bottom = bottom
			self.middle = middle
			self.h = self.middle.GetHeight()

		def SetSize(self, height):
			minHeight = self.top.GetHeight() + self.bottom.GetHeight() + self.h
			height = max(minHeight, height)
			ui.DragButton.SetSize(self, 9, height)

			self.middle.SetRenderingRect(0, 0, 0, float(height) / self.h)

	def __init__(self):
		ui.Window.__init__(self)
		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = None
		self.eventArgs = None
		self.lockFlag = False
		self.CreateScrollBar()
		self.SetScrollBarSize(0)
		self.scrollStep = 0.03
		self.SetWindowName("NONAME_ScrollBar")
	def __del__(self):
		ui.Window.__del__(self)

	def CreateScrollBar(self):
		topImage = ui.ExpandedImageBox()
		topImage.SetParent(self)
		topImage.AddFlag("not_pick")
		topImage.LoadImage(IMG_DIR+"scrollbar_top.tga")
		topImage.Show()
		bottomImage = ui.ExpandedImageBox()
		bottomImage.SetParent(self)
		bottomImage.AddFlag("not_pick")
		bottomImage.LoadImage(IMG_DIR+"scrollbar_buttom.tga")
		bottomImage.Show()
		middleImage = ui.ExpandedImageBox()
		middleImage.SetParent(self)
		middleImage.AddFlag("not_pick")
		middleImage.SetPosition(0, topImage.GetHeight())
		middleImage.LoadImage(IMG_DIR+"scrollbar_mid.tga")
		middleImage.Show()
		self.topImage = topImage
		self.bottomImage = bottomImage
		self.middleImage = middleImage
		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.MakeImage()
		middleBar.SetSize(0) # set min height
		self.middleBar = middleBar
	def Destroy(self):
		self.eventScroll = None
		self.eventArgs = None
	def SetScrollEvent(self, event, *args):
		self.eventScroll = event
		self.eventArgs = args

	def SetMiddleBarSize(self, pageScale):
		self.middleBar.SetSize(int(pageScale * float(self.GetHeight() - self.SCROLL_BTN_YDIST * 2)))
		realHeight = self.GetHeight() - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()
		self.pageSize = realHeight

	def SetScrollBarSize(self, height):
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.pageSize = height - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()
		middleImageScale = float((height - self.SCROLL_BTN_YDIST*2) - self.middleImage.GetHeight()) / float(self.middleImage.GetHeight())
		self.middleImage.SetRenderingRect(0, 0, 0, middleImageScale)
		self.bottomImage.SetPosition(0, height - self.bottomImage.GetHeight())
		self.middleBar.SetRestrictMovementArea(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST,self.middleBar.GetWidth(), height - self.SCROLL_BTN_YDIST * 2)
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST)

	def SetScrollStep(self, step):
		self.scrollStep = step
	def GetScrollStep(self):
		return self.scrollStep
	def GetPos(self):
		return self.curPos
	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)
	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)
	def SetPos(self, pos, moveEvent = True):
		pos = max(0.0, pos)
		pos = min(1.0, pos)
		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, int(newPos) + self.SCROLL_BTN_YDIST)
		if moveEvent == True:
			self.OnMove()
	def OnMove(self):
		if self.lockFlag:
			return
		if 0 == self.pageSize:
			return
		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal - self.SCROLL_BTN_YDIST) / float(self.pageSize)
		if self.eventScroll:
			apply(self.eventScroll, self.eventArgs)
	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		newPos = float(yMouseLocalPosition) / float(self.GetHeight())
		self.SetPos(newPos)
	def LockScroll(self):
		self.lockFlag = True
	def UnlockScroll(self):
		self.lockFlag = False

class Grid:
	def __init__(self, width, height):
		self.width = width
		self.height = height
		self.reset()
	def find_blank(self, width, height):
		if width > self.width or height > self.height:
			return -1
		for row in range(self.height):
			for col in range(self.width):
				index = row * self.width + col
				if self.is_empty(index, width, height):
					return index
		return -1
	def put(self, pos, width, height):
		if not self.is_empty(pos, width, height):
			return False
		for row in range(height):
			start = pos + (row * self.width)
			self.grid[start] = True
			col = 1
			while col < width:
				self.grid[start + col] = True
				col += 1
		return True
	def clear(self, pos, width, height):
		if pos < 0 or pos >= (self.width * self.height):
			return
		for row in range(height):
			start = pos + (row * self.width)
			self.grid[start] = True
			col = 1
			while col < width:
				self.grid[start + col] = False
				col += 1
	def is_empty(self, pos, width, height):
		if pos < 0:
			return False
		row = pos // self.width
		if (row + height) > self.height:
			return False
		if (pos + width) > ((row * self.width) + self.width):
			return False
		for row in range(height):
			start = pos + (row * self.width)
			if self.grid[start]:
				return False
			col = 1
			while col < width:
				if self.grid[start + col]:
					return False
				col += 1
		return True
	def get_size(self):
		return self.width * self.height
	def reset(self):
		self.grid = [False] * (self.width * self.height)

class RewardWindow(ui.BoardWithTitleBar):
	class RewardItem(ui.ImageBox):
		def Destroy(self):
			self.children = {}
			self.rewardIndex = 0
		def __del__(self):
			ui.ImageBox.__del__(self)
		def __init__(self):
			ui.ImageBox.__init__(self)
			self.LoadImage(IMG_DIR+"item.tga")
			self.Destroy()

		def LoadReward(self, rewardIndex, rewardData):
			self.rewardIndex = rewardIndex

			rewardImage = ui.ImageBox()
			rewardImage.SetParent(self)
			rewardImage.LoadImage(IMG_ICON_DIR+rewardData[1]+".tga")
			rewardImage.Show()
			self.children["rewardImage"] = rewardImage

			rewardName = ui.TextLine()
			rewardName.SetParent(self)
			rewardName.SetHorizontalAlignLeft()
			rewardName.SetText(rewardData[0])
			rewardName.SetPosition(95,55)
			rewardName.SetPackedFontColor(grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0))
			rewardName.Show()
			self.children["rewardName"] = rewardName

			grid = Grid(3,2)
			grid.reset()

			rewardItems = ui.GridSlotWindow()
			rewardItems.SetParent(self)
			rewardItems.SetPosition(345, 9)
			rewardItems.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			rewardItems.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			rewardItems.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			rewardItems.ArrangeSlot(0, 3, 2, 32, 32, 0, 0)
			rewardItems.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
			for rewardItem in rewardData[2]:
				(itemVnum,itemCount) = (rewardItem[0],rewardItem[1])
				item.SelectItem(itemVnum)
				(width,height) = item.GetItemSize()
				pos = grid.find_blank(width, height)
				grid.put(pos,width,height)
				rewardItems.SetItemSlot(pos, itemVnum, itemCount)
			rewardItems.RefreshSlot()
			rewardItems.Show()
			self.children["rewardItems"] = rewardItems

		def OverInItem(self, index):
			interface = newconstinfo.GetInterfaceInstance()
			if interface:
				if interface.tooltipItem:
					interface.tooltipItem.SetItemToolTip(reward_info[self.rewardIndex][2][index][0])

		def OverOutItem(self):
			interface = newconstinfo.GetInterfaceInstance()
			if interface:
				if interface.tooltipItem:
					interface.tooltipItem.HideToolTip()

		def SetRewardPlayer(self, playerName):
			rewardPlayer = None
			if not self.children.has_key("rewardPlayer"):
				rewardPlayer = ui.TextLine()
				rewardPlayer.SetParent(self)
				rewardPlayer.SetHorizontalAlignLeft()
				self.children["rewardPlayer"] = rewardPlayer
			else:
				rewardPlayer=self.children["rewardPlayer"]

			if player.GetName() == playerName:
				rewardPlayer.SetPackedFontColor(grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0))
			rewardPlayer.SetText(playerName)
			rewardPlayer.SetPosition(95,18)
			rewardPlayer.Show()

	def Destroy(self):
		self.ListBox = None
		self.boardImage = None
		self.isFirstOpen = False
	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.Destroy()
		self.LoadWindow()

	def LoadWindow(self):
		self.SetSize(484, 378)
		self.AddFlag("movable")
		self.AddFlag("attach")
		self.AddFlag("float")
		self.SetTitleName(localeinfo.REWARD_WINDOW_TITLE)
		self.SetCloseEvent(self.Close)
		self.SetCenterPosition()

		boardImage = ui.ImageBox()
		boardImage.SetParent(self)
		boardImage.SetPosition(8, 30)
		boardImage.AddFlag("not_pick")
		boardImage.LoadImage(IMG_DIR+"board.tga")
		boardImage.Show()
		self.boardImage = boardImage

		ListBox = ui.ListBoxEx()
		ListBox.SetParent(boardImage)
		ListBox.SetPosition(4,4)
		ListBox.SetSize(boardImage.GetWidth(), boardImage.GetHeight())
		ListBox.AddFlag("not_pick")
		ListBox.SetViewItemCount(4)
		ListBox.SetItemSize(450, 81)
		ListBox.SetItemStep(83)
		ListBox.Show()
		self.ListBox = ListBox

		scrollBar = ScrollBar()
		#scrollBar = ui.ScrollBar()
		scrollBar.SetParent(boardImage)
		scrollBar.SetPosition(460, 0)
		scrollBar.SetScrollBarSize(boardImage.GetHeight())
		scrollBar.Show()
		self.scrollBar = scrollBar

		ListBox.SetScrollBar(scrollBar)
		self.LoadRewards()

	def OnRunMouseWheel(self, iLen):
		if not self.scrollBar or not self.scrollBar.IsShow():
			return False

		if not self.IsInPosition():
			return False

		if iLen > 0:
			self.scrollBar.OnUp()
		else:
			self.scrollBar.OnDown()

		return True

	def SetRewardPlayers(self, data):
		if len(data) > 0:
			splitRewards = data[:len(data)-1].split("#")
			for reward in splitRewards:
				rewardList = reward.split("|")
				if len(rewardList) == 2:
					listBoxData = self.ListBox.itemList
					for listboxItem in listBoxData:
						(rewardIndex,playerName) = (int(rewardList[0]),str(rewardList[1]))
						if playerName.isspace() or playerName == "" or playerName== " ":
							playerName = ""
						if listboxItem.rewardIndex == rewardIndex:
							listboxItem.SetRewardPlayer(playerName)
							break
	def LoadRewards(self):
		for key, data in reward_info.items():
			rewardItem = self.RewardItem()
			rewardItem.LoadReward(key, data)
			self.ListBox.AppendItem(rewardItem)

		iLen = len(reward_info.items())
		if iLen > 4 and self.scrollBar:
			self.scrollBar.SetMiddleBarSize(float(4) / float(iLen))
			self.scrollBar.SetScrollStep(float(1) / float(iLen - 4))

	def Close(self):
		self.Hide()

	def Open(self):
		if self.isFirstOpen == False:
			net.SendChatPacket("/update_reward_data")
			self.isFirstOpen = True
		self.Show()

	def OnPressEscapeKey(self):
		self.Close()
		return True


