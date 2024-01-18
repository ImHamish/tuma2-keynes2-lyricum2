import app
import player
import net

import ui
import wndMgr
import grp
import chat
import uitooltip
import item
import mousemodule
import uicommon
import cPickle as pickle
import localeinfo

WORLDARD_PATCH = "Switchbot/design/"
MAX_BONUS = 5
MAX_SAVE  = 8
VNUM_ITEM_CHANGE = 71084
MAX_CHANGE = 400

ITEMS_ZODIAC = [
300,301,302,303,304,305,306,307,308,309,329,339,349,359,
310,311,312,313,314,315,316,317,318,319,369,379,389,399,
1180,1181,1182,1183,1184,1185,1186,1187,1188,1189,1199,1209,1219,1229,
2200,2201,2202,2203,2204,2205,2206,2207,2208,2209,2219,2229,2239,2249,
3220,3221,3222,3223,3224,3225,3226,3227,3228,3229,3239,3249,3259,3269,
5160,5161,5162,5163,5164,5165,5166,5167,5168,5169,5179,5189,5199,5209,
7300,7301,7302,7303,7304,7305,7306,7307,7308,7309,7319,7329,7339,7349,
19290,19291,19292,19293,19294,19295,19296,19297,19298,19299,19309,19310,19311,19312,
19490,19491,19492,19493,19494,19495,19496,19497,19498,19499,19509,19510,19511,19512,
19690,19691,19692,19693,19694,19695,19696,19697,19698,19699,19709,19710,19711,19712,
19890,19891,19892,19893,19894,19895,19896,19897,19898,19899,19909,19910,19911,19912,
]

class SwitchBotListElements(ui.Window):
	def __init__(self,wndSwitchBot,wndThinBoard,wndIndex):
		ui.Window.__init__(self,"TOP_MOST")
		self.wndSwitchBot = wndSwitchBot
		self.wndThinBoard = wndThinBoard
		self.wndIndex = wndIndex
		self.wndIsOpen = False
		self.id = -1
		self.name = ""

		self.__LoadWindow()

	def __del__(self): 
		ui.Window.__del__(self)

	def Show(self):
		ui.Window.Show(self)

		self.SetOpen(True)
		self.AdjustPositionAndSize()
		
	def __LoadWindow(self):
		self.listBox = ui.ComboBox.ListBoxWithBoard("TOP_MOST")
		self.listBox.SetPickAlways()
		self.listBox.SetParent(self)
		self.listBox.SetPosition(0,0)
		self.listBox.SetEvent(self.OnSelectItem)
		self.listBox.Show()

	def InsertBonus(self,index,name):
		self.listBox.InsertItem(index, name)
		self.listBox.ArrangeItem()

	def GetBasePosition(self):
		x, y = self.wndThinBoard.GetGlobalPosition()
		return x-1, y+23

	def AdjustPositionAndSize(self):
		bx, by = self.GetBasePosition()
		self.SetPosition(bx, by)

	def GetName(self):
		return self.name

	def GetId(self):
		return self.id

	def GetIndex(self):
		return self.wndIndex

	def ResetAll(self):
		self.id = -1
		self.name = ""

	def OnSelectItem(self,id,name):
		self.id = id
		self.name = name
		self.wndSwitchBot.SwitchBotListSelect(self)
		self.Close()

	def SetId(self,id):
		self.id = id

	def SetOpen(self,value):
		self.wndIsOpen = value

	def IsOpen(self):
		return	self.wndIsOpen

	def Close(self):
		self.listBox.ClearItem()
		self.SetOpen(False)
		wndMgr.Hide(self.hWnd)

class SwitchBotListBonus(SwitchBotListElements):
	def __init__(self,wndSwitchBot,wndThinBoard,wndIndex):
		SwitchBotListElements.__init__(self,wndSwitchBot,wndThinBoard,wndIndex)
		self.listBox.SetSize(205,20)

	def __del__(self): 
		SwitchBotListElements.__del__(self)

	def Select(self):
		self.wndSwitchBot.SetNameBonusSlot(self.GetIndex(),self.GetId(),self.GetName())
		self.wndSwitchBot.ResetValueBonus(self.GetIndex())

class SwitchBotListValue(SwitchBotListElements):
	def __init__(self,wndSwitchBot,wndThinBoard,wndIndex):
		SwitchBotListElements.__init__(self,wndSwitchBot,wndThinBoard,wndIndex)
		self.listBox.SetSize(39,20)

	def __del__(self): 
		SwitchBotListElements.__del__(self)

	def Select(self):
		self.wndSwitchBot.SetValueBonusSlot(self.GetIndex(),self.GetId(),self.GetName())

class UiSwitchBot(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.get_affect 	= uitooltip.ItemToolTip().AFFECT_DICT
		self.toolTip 		= uitooltip.ItemToolTip()

		self.accept_message = uicommon.QuestionDialog2()
		
		self.elements = {}
		self.elements["list_bonus"] = []
		self.elements["save_bonus"] = {}
		self.elements["select_presets"] = -1
		self.list_get_bonus_id_m_h = [[72,5,55],[71,5,25]] #Media - Habilidad [id,give_value,max_value]

		self.ScollPos = 0

		for i in xrange(0,MAX_BONUS):
			self.elements["id_select_bonus_%d"%i] = -1
			self.elements["id_select_value_%d"%i] = -1
		self.elements["item_change_select_pos"] = 0 
		self.elements["vnum_change_0"] = [71084, 88959] # Vnum Change Default
		self.elements["vnum_change_1"] = [71151,76023] # Vnum Green Change
		self.elements["vnum_change_2"] = [70064,70401,88963] #Vnum Costume Change
		self.elements["vnum_change_3"] = [86051,88965] #Vnum Talisman Change
		self.elements["vnum_change_4"] = [86060] #Vnum Zodiac Change
		self.elements["list_chage_item_actual"] = []

		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		ui.ScriptWindow.Show(self)
		self.AdjustPositionAndSize()

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/switchbot_windows.py")
		except:
			import exception
			exception.Abort("UiSwitchBot.LoadWindow.LoadObject")
		try:
			self.board 			= self.GetChild("board")
			self.base_save		= self.GetChild("base_save")
			self.scrollbar 		= self.GetChild("scrollbar")
			self.slot_item 		= self.GetChild("slot_item")
			self.change_button 	= self.GetChild("change_button")
			self.save_button 	= self.GetChild("save_button")
			self.prev_button = self.GetChild("prev_button")
			self.next_button = self.GetChild("next_button")
			self.TitleBar 		= self.GetChild("TitleBar")

		except:
			import exception
			exception.Abort("UiSwitchBot.LoadWindow.LoadElements")

		self.slot_item.LockSlot(1)
		self.slot_item.SetSelectEmptySlotEvent(ui.__mem_func__(self.__OnSelectEmptySlot))
		self.slot_item.SetSelectItemSlotEvent(ui.__mem_func__(self.__OnSelectItemSlot))
		self.slot_item.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.slot_item.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.prev_button.SetEvent(ui.__mem_func__(self.prevbutton))
		self.next_button.SetEvent(ui.__mem_func__(self.nextbutton))

		self.save_button.SetEvent(self.SavePresets)
		self.change_button.SetEvent(self.ChangeBonus)

		self.TitleBar.SetCloseEvent(ui.__mem_func__(self.Close))

		self.BonusElements()
		self.SaveElements()
		self.LoadPresets()
		self.RefreshPresets()


		self.scrollbar.SetScrollEvent(ui.__mem_func__(self.RefreshPresets))

	def prevbutton(self):
		if len(self.elements["list_chage_item_actual"]) <= 0:
			return

		if self.elements["item_change_select_pos"]-1 < 0:
			self.elements["item_change_select_pos"] = len(self.elements["list_chage_item_actual"])-1
		else:
			self.elements["item_change_select_pos"] -=1

		self.func_add_change_item()

	def nextbutton(self):
		if len(self.elements["list_chage_item_actual"]) <= 0:
			return

		if self.elements["item_change_select_pos"]+1 >= len(self.elements["list_chage_item_actual"]):
			self.elements["item_change_select_pos"] = 0
		else:
			self.elements["item_change_select_pos"] += 1

		self.func_add_change_item()

	def GetVnumItemChangeSelect(self):
		if len(self.elements["list_chage_item_actual"]) <= 0:
			self.elements["item_change_select_pos"] = 0
			return 0
		return int(self.elements["list_chage_item_actual"][self.elements["item_change_select_pos"]])

	def IsItemZodiac(self,item_vnum):
		if item_vnum in ITEMS_ZODIAC:
			return True
		return False
		
	def GetListChangeItem(self,index_item):
		self.elements["list_chage_item_actual"] = []

		item.SelectItem(index_item)
		itemType = item.GetItemType()
		itemSubType = item.GetItemSubType()

		if item.ITEM_TYPE_COSTUME != itemType:
			for i in xrange(item.LIMIT_MAX_NUM):
				(limitType, limitValue) = item.GetLimit(i)
				if item.LIMIT_LEVEL == limitType:
					level_item = limitValue

		self.elements["list_bonus_add"] = []

		if item.ITEM_TYPE_COSTUME != itemType:
			if self.IsItemZodiac(index_item):
				self.elements["list_bonus_add"].append(4)
			else:
				if itemType == item.ITEM_TYPE_ARMOR and itemSubType == item.ARMOR_PENDANT:
					self.elements["list_bonus_add"].append(3)
				else:
					self.elements["list_bonus_add"].append(0)
				if level_item <= 40:
					self.elements["list_bonus_add"].append(1)
		else:
			self.elements["list_bonus_add"].append(2)

		for c in self.elements["list_bonus_add"]:
			for a in self.elements["vnum_change_%d"%c]:
				count = player.GetItemCountbyVnumExtraInventory(a)
				if count > 0:
					if a not in self.elements["list_chage_item_actual"]:
						self.elements["list_chage_item_actual"].append(a)

		if self.elements["item_change_select_pos"] >= len(self.elements["list_chage_item_actual"]):
			self.elements["item_change_select_pos"] = 0


	def LoadPresets(self):
		try:
			try:
				f = old_open("switchbot.wa", "rb")
				self.elements["save_bonus"] = pickle.load(f)
				f.close()
			except IOError:
				f = old_open("switchbot.wa", "wb")
				f.close()
			except:
				pass
		except IOError:
			self.elements["save_bonus"] = {}
		
		self.scrollbar.SetPos(0.0)
		self.Diff = len(self.elements["save_bonus"]) - (MAX_SAVE)
		if self.Diff > 0:
			stepSize = 1.0 / self.Diff
			self.scrollbar.SetScrollStep( stepSize )
		self.ScollPos = 0

	def BonusElements(self):
		for i in xrange(0,MAX_BONUS):
			self.elements["slot_bonus_%d"%i] = ui.Button()
			self.elements["slot_bonus_%d"%i].SetParent(self.board)
			self.elements["slot_bonus_%d"%i].SetUpVisual(WORLDARD_PATCH+"slot_bonus_new.tga")
			self.elements["slot_bonus_%d"%i].SetOverVisual(WORLDARD_PATCH+"slot_bonus_new.tga")
			self.elements["slot_bonus_%d"%i].SetDownVisual(WORLDARD_PATCH+"slot_bonus_new.tga")
			self.elements["slot_bonus_%d"%i].SetPosition(87,58+i*24)
			self.elements["slot_bonus_%d"%i].SetEvent(self.SelectBonus,i)
			self.elements["slot_bonus_%d"%i].Show()

			self.elements["bonus_text_%d"%i] = ui.TextLine()
			self.elements["bonus_text_%d"%i].SetParent(self.elements["slot_bonus_%d"%i])
			self.elements["bonus_text_%d"%i].SetPosition(7,4)
			self.elements["bonus_text_%d"%i].SetText(".")
			self.elements["bonus_text_%d"%i].Show()

			self.elements["slot_value_%d"%i] = ui.Button()
			self.elements["slot_value_%d"%i].SetParent(self.board)
			self.elements["slot_value_%d"%i].SetUpVisual(WORLDARD_PATCH+"slot_valor.tga")
			self.elements["slot_value_%d"%i].SetOverVisual(WORLDARD_PATCH+"slot_valor.tga")
			self.elements["slot_value_%d"%i].SetDownVisual(WORLDARD_PATCH+"slot_valor.tga")
			self.elements["slot_value_%d"%i].SetPosition(295,58+i*24)
			self.elements["slot_value_%d"%i].SetEvent(self.SelectValue,i)
			self.elements["slot_value_%d"%i].Show()

			self.elements["value_text_%d"%i] = ui.TextLine()
			self.elements["value_text_%d"%i].SetParent(self.elements["slot_value_%d"%i])
			self.elements["value_text_%d"%i].SetPosition(20,3)
			self.elements["value_text_%d"%i].SetText("0")
			self.elements["value_text_%d"%i].SetHorizontalAlignCenter()
			self.elements["value_text_%d"%i].Show()

			self.elements["button_reset_bonus_%d"%i] = ui.Button()
			self.elements["button_reset_bonus_%d"%i].SetParent(self.board)
			self.elements["button_reset_bonus_%d"%i].SetUpVisual(WORLDARD_PATCH+"elim_bonus_1.tga")
			self.elements["button_reset_bonus_%d"%i].SetOverVisual(WORLDARD_PATCH+"elim_bonus_2.tga")
			self.elements["button_reset_bonus_%d"%i].SetDownVisual(WORLDARD_PATCH+"elim_bonus_3.tga")
			self.elements["button_reset_bonus_%d"%i].SetPosition(337,58+i*24)
			self.elements["button_reset_bonus_%d"%i].SetEvent(self.ResetBonus,i)
			self.elements["button_reset_bonus_%d"%i].Show()

			self.elements["list_bonus_%d"%i] = SwitchBotListBonus(self,self.elements["slot_bonus_%d"%i],i)
			self.elements["list_value_%d"%i] = SwitchBotListValue(self,self.elements["slot_value_%d"%i],i)

	def SaveElements(self):
		for i in xrange(0,MAX_SAVE):
			self.elements["slot_save_%d"%i] = ui.RadioButton()
			self.elements["slot_save_%d"%i].SetParent(self.base_save)
			self.elements["slot_save_%d"%i].SetUpVisual(WORLDARD_PATCH+"boton_lista_1.tga")
			self.elements["slot_save_%d"%i].SetOverVisual(WORLDARD_PATCH+"boton_lista_2.tga")
			self.elements["slot_save_%d"%i].SetDownVisual(WORLDARD_PATCH+"boton_lista_2.tga")
			self.elements["slot_save_%d"%i].SetPosition(4,7+i*18)
			
			self.elements["slot_save_%d"%i].Show()

			self.elements["bonus_save_text_%d"%i] = ui.TextLine()
			self.elements["bonus_save_text_%d"%i].SetParent(self.elements["slot_save_%d"%i])
			self.elements["bonus_save_text_%d"%i].SetPosition(7,1)
			self.elements["bonus_save_text_%d"%i].SetText("")
			self.elements["bonus_save_text_%d"%i].Show()


			self.elements["button_delete_save_%d"%i] = ui.Button()
			self.elements["button_delete_save_%d"%i].SetParent(self.base_save)
			self.elements["button_delete_save_%d"%i].SetUpVisual(WORLDARD_PATCH+"elim_lista_1.tga")
			self.elements["button_delete_save_%d"%i].SetOverVisual(WORLDARD_PATCH+"elim_lista_2.tga")
			self.elements["button_delete_save_%d"%i].SetDownVisual(WORLDARD_PATCH+"elim_lista_3.tga")
			self.elements["button_delete_save_%d"%i].SetPosition(142,7+i*18)
			
			self.elements["button_delete_save_%d"%i].Show()

	def func_clear_bonus_item(self):
		self.elements["select_presets"] = -1
		self.elements["list_bonus"] = []
		self.CloseListBonusAll()
		for i in xrange(0,MAX_BONUS):
			self.ResetBonus(i)

	def func_set_bonus_items(self,id_bonus,bonus_value_0,bonus_value_1,bonus_value_2,bonus_value_3,bonus_value_4):
		self.elements["list_bonus"].append([id_bonus,self.func_get_repeat_bonus(bonus_value_0,bonus_value_1,bonus_value_2,bonus_value_3,bonus_value_4)])

	def func_get_repeat_bonus(self,valor1,valor2,valor3,valor4,valor5):
		get_bonus = [valor1,valor2,valor3,valor4,valor5]
		list_new_bonus = []

		for key in get_bonus:
			if key not in list_new_bonus:
				list_new_bonus.append(key)	

		return  list_new_bonus

	def func_add_mh(self):
		for j in xrange(0,len(self.list_get_bonus_id_m_h)):
			list_value = []
			value_i = (self.list_get_bonus_id_m_h[j][2] / self.list_get_bonus_id_m_h[j][1])
			for i in xrange(1,value_i+1):
				list_value.append((self.list_get_bonus_id_m_h[j][1]*i))
			self.elements["list_bonus"].append([self.list_get_bonus_id_m_h[j][0],list_value])

	def func_add_change_item(self):
		vnum_item_change = self.GetVnumItemChangeSelect()

		self.slot_item.ClearSlot(1)
		if vnum_item_change <= 0:
			return

		#for i in xrange(player.INVENTORY_PAGE_SIZE*player.INVENTORY_PAGE_COUNT):
		#	itemVNum = player.GetItemIndex(i)
		#	itemCount = player.GetItemCount(i)
		#	if itemVNum == vnum_item_change:
		#		count+=itemCount
		
		#for i in xrange(player.EXTRA_INVENTORY_PAGE_SIZE*4):
		#	itemVNum = player.GetItemIndex(player.EXTRA_INVENTORY,i)
		#	itemCount = player.GetItemCount(player.EXTRA_INVENTORY,i)
		#	if itemVNum == vnum_item_change:
		#		count+=itemCount
		
		
		count = player.GetItemCountbyVnumExtraInventory(vnum_item_change)

		
		if count <= 0:
			self.slot_item.ClearSlot(1)
		else:
			if count <= MAX_CHANGE:
				self.slot_item.SetItemSlot(1,vnum_item_change,count)
			else:
				self.slot_item.SetItemSlot(1,vnum_item_change,MAX_CHANGE)

	def func_count_bonus_item(self):
		count = 0
		for i in xrange(0,MAX_BONUS):
			(type,value) = player.GetItemAttribute(player.SWITCHBOT,0, i)
			if type != 0:
				count += 1
		return count

	def func_count_select_bonus(self):
		count = 0
		for i in xrange(0,MAX_BONUS):
			if self.elements["id_select_bonus_%d"%i] >= 0:
				count+=1

		return count


	def func_max_select_bonus(self):
		if self.func_count_bonus_item() > 0:
			if self.func_count_select_bonus() >= self.func_count_bonus_item():
				return True
			else:
				return False
		else:
			return True

		return False

	def func_count_select_value(self):
		count = 0
		for i in xrange(0,MAX_BONUS):
			if self.elements["id_select_value_%d"%i] >= 0:
				count+=1

		return count

	def func_repeat_bonus(self):
		list_bonus_total = []
		list_bonus_new = []
		for i in xrange(0,MAX_BONUS):
			if self.elements["id_select_bonus_%d"%i] >= 0:
				list_bonus_total.append([self.elements["id_select_bonus_%d"%i]])

		count = 0
		count_bonus_repeat = 0

		for i in list_bonus_total:
			if i not in list_bonus_new:
				list_bonus_new.append(list_bonus_total[count])
			else:
				count_bonus_repeat = 1
			count += 1

		return  count_bonus_repeat

	def func_check_change(self):
		if player.GetItemIndex(player.SWITCHBOT, 0) <= 0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.SWITCHBOT_NORMAL_TEXT1)
			return False
		
		if self.func_count_select_bonus() <= 0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.SWITCHBOT_NORMAL_TEXT1)
			return False
		
		if self.func_count_select_bonus() != self.func_count_select_value():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.SWITCHBOT_NORMAL_TEXT3)
			return False
		
		if self.func_repeat_bonus() == 1:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.SWITCHBOT_NORMAL_TEXT4)
			return False
		
		if self.GetVnumItemChangeSelect() <= 0:
			return False
		
		return True

	def func_get_list_bonus(self):
		return self.elements["list_bonus"]

	def func_get_id_select_bonus(self,index):
		return self.elements["id_select_bonus_%d"%index]

	def func_get_id_select_value(self,index):
		return self.elements["id_select_value_%d"%index]

	def func_get_list_bonus_select(self):
		list_bonus = []
		for i in xrange(0,MAX_BONUS):
			get_list_bonus = self.func_get_list_bonus()
			get_select_bonus_id = self.func_get_id_select_bonus(i)
			get_select_value_id = self.func_get_id_select_value(i)
				
			if get_select_bonus_id >= 0:
				type = get_list_bonus[get_select_bonus_id][0]
				value = get_list_bonus[get_select_bonus_id][1][get_select_value_id]
				list_bonus.append([type,value])
			else:
				list_bonus.append([0,0])

		return list_bonus

	def func_get_list_bonus_id_select(self):
		list_id = []
		for i in xrange(0,MAX_BONUS):
			get_list_bonus = self.func_get_list_bonus()
			get_select_bonus_id = self.func_get_id_select_bonus(i)
			get_select_value_id = self.func_get_id_select_value(i)
				
			if get_select_bonus_id >= 0:
				type = get_list_bonus[get_select_bonus_id][0]
				list_id.append([get_select_bonus_id,get_select_value_id])
			else:
				list_id.append([-1,-1])

		return list_id

	def SetNameBonusSlot(self,index,id_select,name):
		self.elements["bonus_text_%d"%index].SetText(name)
		self.elements["id_select_bonus_%d"%index] = id_select

	def ResetNameBonus(self,index):
		self.elements["bonus_text_%d"%index].SetText(".")
		self.elements["id_select_bonus_%d"%index] = -1

	def ResetValueBonus(self,index):
		self.elements["value_text_%d"%index].SetText("0")
		self.elements["id_select_value_%d"%index] = -1
	
	def SetValueBonusSlot(self,index,id_select,value):

		name = self.get_affect[self.elements["list_bonus"][self.elements["id_select_bonus_%d"%index]][0]](int(value))
		self.SetNameBonusSlot(index,self.elements["id_select_bonus_%d"%index],name)

		self.elements["value_text_%d"%index].SetText(value)
		self.elements["id_select_value_%d"%index] = id_select

	def __OnSelectEmptySlot(self, selectedSlotPos):
		isAttached = mousemodule.mouseController.isAttached()
		if isAttached:
			attachedSlotType = mousemodule.mouseController.GetAttachedType()
			attachedSlotPos = mousemodule.mouseController.GetAttachedSlotNumber()
			mousemodule.mouseController.DeattachObject()
			itemIndex = player.GetItemIndex(attachedSlotPos)
			itemCount = player.GetItemCount(attachedSlotPos)

			item.SelectItem(itemIndex)
			itemType = item.GetItemType()
			itemSubType =  item.GetItemSubType()

			if ((item.ITEM_TYPE_WEAPON == itemType) or (item.ITEM_TYPE_ARMOR == itemType)) \
				or (item.ITEM_TYPE_COSTUME == itemType and ((itemSubType == item.COSTUME_TYPE_BODY) or (itemSubType == item.COSTUME_TYPE_HAIR) or (itemSubType == item.COSTUME_TYPE_WEAPON))):
				if player.SLOT_TYPE_INVENTORY == attachedSlotType:
					attachedCount = mousemodule.mouseController.GetAttachedItemCount()
					net.SendItemMovePacket(player.INVENTORY, attachedSlotPos, player.SWITCHBOT, 0, attachedCount)

	def __OnSelectItemSlot(self, selectedSlotPos):
		isAttached = mousemodule.mouseController.isAttached()
		if isAttached:
			attachedSlotType = mousemodule.mouseController.GetAttachedType()
			attachedSlotPos = mousemodule.mouseController.GetAttachedSlotNumber()
			mousemodule.mouseController.DeattachObject()
			itemIndex = player.GetItemIndex(attachedSlotPos)
			itemCount = player.GetItemCount(attachedSlotPos)

			item.SelectItem(itemIndex)
			itemType = item.GetItemType()
			itemSubType =  item.GetItemSubType()

			if ((item.ITEM_TYPE_WEAPON == itemType) or (item.ITEM_TYPE_ARMOR == itemType)) \
				or (item.ITEM_TYPE_COSTUME == itemType and ((itemSubType == item.COSTUME_TYPE_BODY) or (itemSubType == item.COSTUME_TYPE_HAIR) or (itemSubType == item.COSTUME_TYPE_WEAPON))):
				if player.SLOT_TYPE_INVENTORY == attachedSlotType:
					attachedCount = mousemodule.mouseController.GetAttachedItemCount()
					net.SendItemMovePacket(player.INVENTORY, attachedSlotPos, player.SWITCHBOT, 0, attachedCount)

		else:
			if selectedSlotPos > 0:
				return

			itemVnum = player.GetItemIndex(player.SWITCHBOT, 0)
			itemCount = player.GetItemCount(player.SWITCHBOT, 0)
			mousemodule.mouseController.AttachObject(self, player.SLOT_TYPE_SWITCHBOT, 0, itemVnum, itemCount)


	def SelectBonus(self,index):
		list_bonus_element = self.elements["list_bonus_%d"%index]
			

		if list_bonus_element:
			if list_bonus_element.IsOpen():
				list_bonus_element.Close()
			else:
				self.CloseListBonusAll()

				if len(self.elements["list_bonus"]) <= 0:
					return

				if self.func_max_select_bonus() and list_bonus_element.GetId() == -1:
					return

				list_bonus_element.Show()

				for i in xrange(0,len(self.elements["list_bonus"])):
					name = self.get_affect[self.elements["list_bonus"][i][0]](0)
					#replace = ["+","0","%"]
					#for x in replace:
						#name = name.replace(x,"")
					list_bonus_element.InsertBonus(i,name)

	def SelectValue(self,index):
		list_value_element = self.elements["list_value_%d"%index]
		list_bonus_element = self.elements["list_bonus_%d"%index]

		if list_value_element:
			if list_value_element.IsOpen():
				list_value_element.Close()
			else:
				self.CloseListBonusAll()

				if list_bonus_element.GetId() == -1:
					return

				list_value_element.Show()

				for i in xrange(0,len(self.elements["list_bonus"][list_bonus_element.GetId()][1])):
					list_value_element.InsertBonus(i,str(self.elements["list_bonus"][list_bonus_element.GetId()][1][i]))

	def SwitchBotListSelect(self, item):
		self.selectedItem = item

		if self.selectedItem:
			self.selectedItem.Select()


	def ResetBonus(self,index):
		self.ResetValueBonus(index)
		self.ResetNameBonus(index)

		list_bonus_element = self.elements["list_bonus_%d"%index]
		list_value_element = self.elements["list_value_%d"%index]

		if list_bonus_element:
			list_bonus_element.ResetAll()

		if list_value_element:
			list_value_element.ResetAll()

	def CloseListBonusAll(self):
		for i in xrange(0,MAX_BONUS):
			list_bonus_element = self.elements["list_bonus_%d"%i]
			list_value_element = self.elements["list_value_%d"%i]

			if list_bonus_element:
				list_bonus_element.Close()

			if list_value_element:
				list_value_element.Close()


	def ChangeBonus(self):
		if self.func_check_change():
			list_bonus = self.func_get_list_bonus_select()
			self.accept_message.SetText1(localeinfo.SWITCHBOT_NORMAL_TEXT5)
			self.accept_message.SetText2(localeinfo.SWITCHBOT_NORMAL_TEXT6)
			self.accept_message.SetAcceptEvent(lambda arg=TRUE,  bonus  = list_bonus: self.AcceptChangeBonus(arg,bonus))
			self.accept_message.SetCancelEvent(lambda arg=FALSE, bonus  = list_bonus: self.AcceptChangeBonus(arg,bonus))
			self.accept_message.Open()

	def AcceptChangeBonus(self,arg,list_bonus):
		self.accept_message.Close()
		if arg:
			vnum_item_change = self.GetVnumItemChangeSelect()
			net.SBChange(list_bonus,vnum_item_change)

	def RefreshItemSlot(self):
		itemIndex = player.GetItemIndex(player.SWITCHBOT, 0)
		itemCount = player.GetItemCount(player.SWITCHBOT, 0)

		self.slot_item.SetItemSlot(0, itemIndex, itemCount)

		if itemIndex > 0:
			self.GetListChangeItem(itemIndex)
			self.func_add_change_item()
		else:
			self.slot_item.ClearSlot(1)

		self.slot_item.RefreshSlot()

	def SelectPresets(self,index,index1,name):
		if player.GetItemIndex(player.SWITCHBOT, 0) <= 0:
			return

		for i in xrange(0,MAX_BONUS):
			self.ResetBonus(i)

		dict = self.elements["save_bonus"].get(index1, None)
		for bonus_dict in xrange(0,len(dict["bonus"])):
			(type,value) 				 = dict["bonus"][bonus_dict]
			(id_select_b,id_select_v)    = dict["id"][bonus_dict]
			if type > 0:
				list_bonus_element = self.elements["list_bonus_%d"%bonus_dict]

				self.SetNameBonusSlot(bonus_dict,id_select_b,self.get_affect[type](0))
				list_bonus_element.SetId(id_select_b)

				self.SetValueBonusSlot(bonus_dict,id_select_v,str(value))

		self.elements["select_presets"] = index1

		for i in xrange(MAX_SAVE):
			self.elements["slot_save_%d"%i].SetUp()

		self.ClickRadioButton(index,index1)

	def ClickRadioButton(self,index,index1):
		if self.elements["select_presets"] == index1:
			self.elements["slot_save_%d"%index].Down()
		else:
			self.elements["slot_save_%d"%index].SetUp()

	def RefreshPresets(self):
		self.ScollPos = int(self.scrollbar.GetPos() * self.Diff)

		if self.ScollPos < 0:
			return

		for i in xrange(MAX_SAVE):
			idx = i + self.ScollPos
			if idx < len(self.elements["save_bonus"]):
				count = 0
				for index_s in self.elements["save_bonus"].iterkeys():
					dict = self.elements["save_bonus"].get(index_s, None)
					if not dict:
						continue
					name = dict["name"]
					if idx == count:
						self.ShowPresets(i,index_s,name)
						self.ClickRadioButton(i,index_s)
					count +=1
			else:
				self.EmptyPresets(i)


	def ShowPresets(self, index, index1, name):
		if(index < MAX_SAVE):
			self.elements["slot_save_%d"%index].Show()
			self.elements["button_delete_save_%d"%index].Show()
			self.elements["bonus_save_text_%d"%index].SetText(name)


			self.elements["button_delete_save_%d"%index].SetEvent(self.DeletePresets,index1)
			self.elements["slot_save_%d"%index].SetEvent(self.SelectPresets,index,index1,name)

	def EmptyPresets(self, index):
		if(index < MAX_SAVE):
			self.elements["slot_save_%d"%index].Hide()
			self.elements["button_delete_save_%d"%index].Hide()

	def GetCountListSave(self):
		count = 0
		while self.elements["save_bonus"].has_key(count):
			count += 1
			
		return count

	def SavePresets(self):
		if player.GetItemIndex(player.SWITCHBOT, 0) <= 0:
			return

		name_presets = uicommon.InputDialog()
		name_presets.SetTitle("Name Presets")
		name_presets.SetAcceptEvent(ui.__mem_func__(self.OnAddPresets))
		name_presets.SetCancelEvent(ui.__mem_func__(self.OnCancelAddPresets))
		name_presets.Open()
		self.name_presets = name_presets

	def OnAddPresets(self):
		text = self.name_presets.GetText()
		if text:
			bonus = self.func_get_list_bonus_select()
			get_id = self.func_get_list_bonus_id_select()
			new_dict = {
				"name" 	: text,
				"bonus" : bonus,
				"id"	: get_id,
			}

			self.elements["save_bonus"][self.GetCountListSave()] = new_dict
			self.SaveDictPresets()

		self.OnCancelAddPresets()

	def DeletePresets(self,index):
		dict = self.elements["save_bonus"].get(index, None)
		if not dict:
			return

		del self.elements["save_bonus"][index]

		self.elements["select_presets"] = -1

		self.SaveDictPresets()


	def SaveDictPresets(self):
		try:
			f = old_open("switchbot.wa", "wb")
			pickle.dump(self.elements["save_bonus"], f)
			f.close()
		except:
			pass
		
		self.LoadPresets()
		self.RefreshPresets()

	def OnCancelAddPresets(self):
		self.name_presets.Close()
		self.name_presets = None
		return True

	def AdjustPositionAndSize(self):

		for i in xrange(0,MAX_BONUS):
			list_bonus_element = self.elements["list_bonus_%d"%i]
			list_value_element = self.elements["list_value_%d"%i]

			if list_bonus_element:
				list_bonus_element.AdjustPositionAndSize()

			if list_value_element:
				list_value_element.AdjustPositionAndSize()


	def OverInItem(self,index):
		if index == 0:
			self.toolTip.ClearToolTip()
			self.toolTip.SetInventoryItem(index,player.SWITCHBOT)

		else:
			vnum_item_change = self.GetVnumItemChangeSelect()
			if vnum_item_change > 0:
				self.toolTip.SetItemToolTip(vnum_item_change)

	def OverOutItem(self):
		self.toolTip.HideToolTip()


	def OnMoveWindow(self, x, y):
		self.AdjustPositionAndSize()

	def Close(self):
		self.func_clear_bonus_item()
		net.SBClose()
		wndMgr.Hide(self.hWnd)
		
	def OnPressEscapeKey(self):
		self.Close()
		return TRUE