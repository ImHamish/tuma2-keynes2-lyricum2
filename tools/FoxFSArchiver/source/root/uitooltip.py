import app
import chr
import player
import net

import dbg
import item
import grp
import wndMgr
import skill
import shop
import exchange
import grpText
import safebox
import background
import nonplayer
import ui
import mousemodule
import constinfo
import localeinfo

if app.__ENABLE_NEW_OFFLINESHOP__:
	import uiofflineshop

if app.ENABLE_ACCE_SYSTEM:
	import acce

WARP_SCROLLS = [22011, 22000, 22010]

DESC_DEFAULT_MAX_COLS = 26
DESC_WESTERN_MAX_COLS = 35
DESC_WESTERN_MAX_WIDTH = 220

if app.NEW_PET_SYSTEM:
	def pointop(n):
		t = int(n)
		if t / 10 < 1:
			return "0."+n
		else:		
			return n[0:len(n)-1]+"."+n[len(n)-1:]

def chop(n):
	return round(n - 0.5, 1)

if app.INGAME_WIKI:
	def GET_AFFECT_STRING(affType, affValue):
		if 0 == affType:
			return None
		
		try:
			affectString = ItemToolTip.AFFECT_DICT[affType]
			if type(affectString) != str:
				return affectString(affValue)

			if affectString.find("%d") != -1:
				return affectString % affValue
			else:
				return affectString
		except KeyError:
			return "UNKNOWN_TYPE[%s] %s" % (affType, affValue)

def SplitDescription(desc, limit):
	total_tokens = desc.split()
	line_tokens = []
	line_len = 0
	lines = []
	for token in total_tokens:
		if "|" in token:
			sep_pos = token.find("|")
			line_tokens.append(token[:sep_pos])

			lines.append(" ".join(line_tokens))
			line_len = len(token) - (sep_pos + 1)
			line_tokens = [token[sep_pos+1:]]
		else:
			line_len += len(token)
			if len(line_tokens) + line_len > limit:
				lines.append(" ".join(line_tokens))
				line_len = len(token)
				line_tokens = [token]
			else:
				line_tokens.append(token)

	if line_tokens:
		lines.append(" ".join(line_tokens))

	return lines

PetVnum = {
	53001 : 34001,
	53003 : 34001,
	53004 : 34002,
	53005 : 34002,
	53010 : 34008,
	53011 : 34007,
	53012 : 34005,
	53013 : 34006,
	53017 : 34016,
	53025 : 34024,
	53256 : 34118,
	53242 : 34066,
	53243 : 34066,
	53244 : 34067,
	53245 : 34068,
	53246 : 34069,
	53247 : 34070,
	53248 : 34071,
	53249 : 34072,
	53250 : 34084,
	53251 : 34085,
	55701 : 34041,
	55702 : 34045,
	55703 : 34049,
	55704 : 34053,
	55705 : 34036,
	55706 : 34064,
	55707 : 34073,
	55708 : 34075,
	55709 : 34080,
	55710 : 34082,
	55711 : 34095,
	53263 : 34093,
	53264 : 34094,
	53282 : 34114,
	53295 : 8482,
	53296 : 8483,
	53279 : 65414,
	53297 : 8484,
	53298 : 8487,
	53299 : 8488,
	53300 : 8490,
	53301 : 8492,
	53283 : 34115,
	53284 : 9483,
	53285 : 9483,
	53286 : 9493,
	53287 : 9493,
	53026 : 34008,
	53027 : 34008,
	53028 : 34008,
	53029 : 34094,
	53030 : 34094,
	53031 : 34094,
	53252 : 34085,
	53253 : 34085,
	53248 : 34084,
	53249 : 34084,
	#halloween
	48301 : 34100,
	48311 : 34100,
	48321 : 34100
	#end hallowee
	#patch3
	, 49010 : 34116
	#end patch3
	#patch4
	, 49050 : 34117
	#end patch4
	, 60101 : 34120
	, 60102 : 34120
	, 60103 : 34119
	, 60104 : 34119
	, 84012 : 35001
	, 84013 : 35002
	, 84032 : 35003
	, 84052 : 35004
	, 84072 : 35005
	, 84092 : 35006
	, 84112 : 35007
	, 84133 : 35008
	, 84132 : 35009
}

class ToolTip(ui.ThinBoard):

	TOOL_TIP_WIDTH = 190
	TOOL_TIP_HEIGHT = 10

	TEXT_LINE_HEIGHT = 17

	TITLE_COLOR = grp.GenerateColor(0.9490, 0.9058, 0.7568, 1.0)
	SPECIAL_TITLE_COLOR = grp.GenerateColor(1.0, 0.7843, 0.0, 1.0)
	NORMAL_COLOR = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)
	FONT_COLOR = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)
	PRICE_COLOR = 0xffFFB96D

	HIGH_PRICE_COLOR = SPECIAL_TITLE_COLOR
	MIDDLE_PRICE_COLOR = grp.GenerateColor(0.85, 0.85, 0.85, 1.0)
	LOW_PRICE_COLOR = grp.GenerateColor(0.7, 0.7, 0.7, 1.0)

	ENABLE_COLOR = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)
	DISABLE_COLOR = grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0)

	NEGATIVE_COLOR = grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0)
	POSITIVE_COLOR = grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0)
	SPECIAL_POSITIVE_COLOR = grp.GenerateColor(0.6911, 0.8754, 0.7068, 1.0)
	SPECIAL_POSITIVE_COLOR2 = grp.GenerateColor(0.8824, 0.9804, 0.8824, 1.0)
	if app.ENABLE_CAPITALE_SYSTEM:
		PRICE_INFO_COLOR = grp.GenerateColor(1.0, 0.88, 0.0, 1.0)
	
	if app.ENABLE_HALLOWEEN_EVENT_2022:
		SKULL_PRICE_COLOR = grp.GenerateColor(161.0 / 255.0, 66.0 / 255.0, 66.0 / 255.0, 1.0)
	
	if app.ENABLE_GAYA_RENEWAL:
		GAYA_PRICE_COLOR = grp.GenerateColor(0.0, 201.0 / 255.0, 170.0 / 255.0, 1.0)
	
	# if app.__ENABLE_NEW_EFFECT_CIANITE_WEAPON__:
		# CIANITE_COLOR_WEAPON_EPIC = 0xff0D29E2
		# CIANITE_COLOR_WEAPON_LEGG = 0xffE20D0D
		# CIANITE_COLOR_WEAPON_ANTIC = 0xff18F610
		# CIANITE_COLOR_WEAPON_MISTIC = 0xffC22FF7
		
	if app.ENABLE_BUY_WITH_ITEM:
		SHOP_ITEM_COLOR = 0xff15d131
	
	if app.USE_ATTR_6TH_7TH:
		ATTR_6TH_7TH_COLOR = grp.GenerateColor(1.0, 1.0, 0.6078, 1.0)
	
	CONDITION_COLOR = 0xffBEB47D
	CAN_LEVEL_UP_COLOR = 0xff8EC292
	CANNOT_LEVEL_UP_COLOR = DISABLE_COLOR
	NEED_SKILL_POINT_COLOR = 0xff9A9CDB
	if app.ENABLE_CHANGELOOK:
		CHANGELOOK_TITLE_COLOR = 0xff8BBDFF
		CHANGELOOK_ITEMNAME_COLOR = 0xffBCE55C

	def __init__(self, width = TOOL_TIP_WIDTH, isPickable=False):
		ui.ThinBoard.__init__(self, "TOP_MOST")

		if isPickable:
			pass
		else:
			self.AddFlag("not_pick")

		self.AddFlag("float")

		self.followFlag = True
		self.toolTipWidth = width
		self.lastWidthIncrease = 0
		self.xPos = -1
		self.yPos = -1

		self.defFontName = localeinfo.UI_DEF_FONT
		self.lastKey = -1
		
		self.ClearToolTip()

	def __del__(self):
		ui.ThinBoard.__del__(self)

	def ClearToolTip(self):
		self.toolTipHeight = 12
		self.childrenList = []
		self.childrenListStone = []

	def SetFollow(self, flag):
		self.followFlag = flag

	def SetDefaultFontName(self, fontName):
		self.defFontName = fontName

	def AppendSpace(self, size):
		self.toolTipHeight += size
		self.ResizeToolTip()

	def AppendHorizontalLine(self):

		for i in xrange(2):
			horizontalLine = ui.Line()
			horizontalLine.SetParent(self)
			horizontalLine.SetPosition(0, self.toolTipHeight + 3 + i)
			horizontalLine.SetWindowHorizontalAlignCenter()
			horizontalLine.SetSize(150, 0)
			horizontalLine.Show()

			if 0 == i:
				horizontalLine.SetColor(0xff555555)
			else:
				horizontalLine.SetColor(0xff000000)

			self.childrenList.append(horizontalLine)

		self.toolTipHeight += 11
		self.ResizeToolTip()

	def AlignHorizonalCenter(self):
		for child in self.childrenList:
			(x, y)=child.GetLocalPosition()
			child.SetPosition(self.toolTipWidth/2, y)

		self.ResizeToolTip()

	if app.USE_PICKUP_FILTER:
		def AutoAppendNewTextLineResize(self, text, color = FONT_COLOR, centerAlign = True):
			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetFontName(self.defFontName)
			textLine.SetPackedFontColor(color)
			textLine.SetText(text)
			textLine.SetOutline()
			textLine.SetFeather(False)
			textLine.Show()

			(textWidth, textHeight) = textLine.GetTextSize()
			textWidth += 30
			textHeight += 10

			if self.toolTipWidth < textWidth:
				self.toolTipWidth = textWidth

			if centerAlign:
				textLine.SetPosition(self.toolTipWidth / 2, self.toolTipHeight)
				textLine.SetHorizontalAlignCenter()
			else:
				textLine.SetPosition(10, self.toolTipHeight)

			self.childrenList.append(textLine)

			self.toolTipHeight += self.TEXT_LINE_HEIGHT
			self.AlignHorizonalCenter()
			return textLine

	def AutoAppendTextLine(self, text, color = FONT_COLOR, centerAlign = True, extraWidth = 0):
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetFontName(self.defFontName)
		textLine.SetPackedFontColor(color)
		textLine.SetText(text)
		textLine.SetOutline()
		textLine.SetFeather(False)
		textLine.Show()
		if extraWidth:
			self.toolTipWidth += extraWidth
			self.ResizeToolTip()
		
		if centerAlign:
			textLine.SetPosition(self.toolTipWidth/2, self.toolTipHeight)
			textLine.SetHorizontalAlignCenter()

		else:
			textLine.SetPosition(10, self.toolTipHeight)

		self.childrenList.append(textLine)

		(textWidth, textHeight)=textLine.GetTextSize()

		textWidth += 40
		textHeight += 5

		if self.toolTipWidth < textWidth:
			self.toolTipWidth = textWidth

		self.toolTipHeight += textHeight

		return textLine

	def AutoAppendNewTextLine(self, text, color = FONT_COLOR, centerAlign = True):
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetFontName(self.defFontName)
		textLine.SetPackedFontColor(color)
		textLine.SetText(text)
		textLine.SetOutline()
		textLine.SetFeather(False)
		textLine.Show()
		textLine.SetPosition(15, self.toolTipHeight)

		self.childrenList.append(textLine)
		(textWidth, textHeight) = textLine.GetTextSize()

		textWidth += 62
		textHeight += 10

		if self.toolTipWidth < textWidth:
			self.toolTipWidth = textWidth

		self.toolTipHeight += textHeight
		self.ResizeToolTipText(textWidth, self.toolTipHeight)

		return textLine

	def AppendNewTextLineForLogin(self, text, color = FONT_COLOR):
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetFontName(self.defFontName)
		textLine.SetPackedFontColor(color)
		textLine.SetText(text)
		textLine.SetOutline()
		textLine.SetFeather(False)
		textLine.Show()
		textLine.SetPosition(28, 10)

		self.childrenList.append(textLine)
		(textWidth, textHeight) = textLine.GetTextSize()

		textWidth += 62
		textHeight += 10

		if self.toolTipWidth < textWidth:
			self.toolTipWidth = textWidth

		self.toolTipHeight += textHeight
		self.ResizeToolTipText(textWidth, self.toolTipHeight)
		return textLine

	def SetThinBoardSize(self, width, height = 12) :
		self.toolTipWidth = width 
		self.toolTipHeight = height

	def AppendTextLine(self, text, color = FONT_COLOR, centerAlign = True, height = 0):
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetFontName(self.defFontName)
		textLine.SetPackedFontColor(color)
		textLine.SetText(text)
		textLine.SetOutline()
		textLine.SetFeather(False)
		textLine.Show()

		if centerAlign:
			if height != 0:
				textLine.SetPosition(self.toolTipWidth/2, self.toolTipHeight + (height / 2))
			else:
				textLine.SetPosition(self.toolTipWidth/2, self.toolTipHeight)
			textLine.SetHorizontalAlignCenter()
		else:
			textLine.SetPosition(10, self.toolTipHeight)

		self.childrenList.append(textLine)
		
		if height == 0:
			height = self.TEXT_LINE_HEIGHT
		
		if len(text) > 32:
			newWidth = (len(text) - 32) * 4
			if self.lastWidthIncrease < newWidth:
				self.toolTipWidth -= self.lastWidthIncrease
				self.toolTipWidth += newWidth
				self.lastWidthIncrease = newWidth
				j = 0
				for item in self.childrenList:
					found = False
					for i in self.childrenListStone:
						if i == j:
							found = True
							break
					
					j += 1
					if found == False:
						(x, y) = item.GetLocalPosition()
						item.SetPosition(self.toolTipWidth / 2, y)
		
		self.toolTipHeight += height
		self.ResizeToolTip()
		return textLine

	def AppendDescription(self, desc, limit, color = FONT_COLOR):
		if localeinfo.IsEUROPE():
			self.__AppendDescription_WesternLanguage(desc, color)
		else:
			self.__AppendDescription_EasternLanguage(desc, limit, color)

	def __AppendDescription_EasternLanguage(self, description, characterLimitation, color=FONT_COLOR):
		length = len(description)
		if 0 == length:
			return

		lineCount = grpText.GetSplitingTextLineCount(description, characterLimitation)
		for i in xrange(lineCount):
			if 0 == i:
				self.AppendSpace(5)
			self.AppendTextLine(grpText.GetSplitingTextLine(description, characterLimitation, i), color)

	def __AppendDescription_WesternLanguage(self, desc, color=FONT_COLOR):
		lines = SplitDescription(desc, DESC_WESTERN_MAX_COLS)
		if not lines:
			return
		
		self.AppendSpace(5)
		for line in lines:
			self.AppendTextLine(line, color)

	def AppendDescriptionEqual(self, desc, color=FONT_COLOR):
		lines = SplitDescription(desc, DESC_WESTERN_MAX_COLS)
		if not lines:
			return
		
		for line in lines:
			self.AppendTextLine(line, color)

	def ResizeToolTip(self):
		self.SetSize(self.toolTipWidth, self.TOOL_TIP_HEIGHT + self.toolTipHeight)
		
	def ResizeToolTipText(self, x, y):
		self.SetSize(x, y)

	def SetTitle(self, name):
		self.AppendTextLine(name, self.TITLE_COLOR)

	def GetLimitTextLineColor(self, curValue, limitValue):
		if curValue < limitValue:
			return self.DISABLE_COLOR

		return self.ENABLE_COLOR

	def GetChangeTextLineColor(self, value, isSpecial=False):
		if value > 0:
			if isSpecial:
				return self.SPECIAL_POSITIVE_COLOR
			else:
				return self.POSITIVE_COLOR

		if 0 == value:
			return self.NORMAL_COLOR

		return self.NEGATIVE_COLOR

	def SetToolTipPosition(self, x = -1, y = -1):
		self.xPos = x
		self.yPos = y

	def ShowToolTip(self):
		self.SetTop()
		self.Show()

		self.OnUpdate()

	def HideToolTip(self):
		self.Hide()

	def OnUpdate(self):

		if not self.followFlag:
			return

		x = 0
		y = 0
		width = self.GetWidth()
		height = self.toolTipHeight

		if -1 == self.xPos and -1 == self.yPos:

			(mouseX, mouseY) = wndMgr.GetMousePosition()

			if mouseY < wndMgr.GetScreenHeight() - 300:
				y = mouseY + 40
			else:
				y = mouseY - height - 30

			x = mouseX - width/2

		else:

			x = self.xPos - width/2
			y = self.yPos - height

		x = max(x, 0)
		y = max(y, 0)
		x = min(x + width/2, wndMgr.GetScreenWidth() - width/2) - width/2
		y = min(y + self.GetHeight(), wndMgr.GetScreenHeight()) - self.GetHeight()

		parentWindow = self.GetParentProxy()
		if parentWindow:
			(gx, gy) = parentWindow.GetGlobalPosition()
			x -= gx
			y -= gy

		self.SetPosition(x, y)

class ItemToolTip(ToolTip):
	#ModelPreviewBoard = None
	#ModelPreview = None
	#ModelPreviewText = None

	CHARACTER_NAMES = (
		localeinfo.TOOLTIP_WARRIOR,
		localeinfo.TOOLTIP_ASSASSIN,
		localeinfo.TOOLTIP_SURA,
		localeinfo.TOOLTIP_SHAMAN
	)

	CHARACTER_COUNT = len(CHARACTER_NAMES)
	WEAR_NAMES = (
		localeinfo.TOOLTIP_ARMOR,
		localeinfo.TOOLTIP_HELMET,
		localeinfo.TOOLTIP_SHOES,
		localeinfo.TOOLTIP_WRISTLET,
		localeinfo.TOOLTIP_WEAPON,
		localeinfo.TOOLTIP_NECK,
		localeinfo.TOOLTIP_EAR,
		localeinfo.TOOLTIP_UNIQUE,
		localeinfo.TOOLTIP_SHIELD,
		localeinfo.TOOLTIP_ARROW,
	)
	WEAR_COUNT = len(WEAR_NAMES)

	AFFECT_DICT = {
		item.APPLY_MAX_HP : localeinfo.TOOLTIP_MAX_HP,
		item.APPLY_MAX_SP : localeinfo.TOOLTIP_MAX_SP,
		item.APPLY_CON : localeinfo.TOOLTIP_CON,
		item.APPLY_INT : localeinfo.TOOLTIP_INT,
		item.APPLY_STR : localeinfo.TOOLTIP_STR,
		item.APPLY_DEX : localeinfo.TOOLTIP_DEX,
		item.APPLY_ATT_SPEED : localeinfo.TOOLTIP_ATT_SPEED,
		item.APPLY_MOV_SPEED : localeinfo.TOOLTIP_MOV_SPEED,
		item.APPLY_CAST_SPEED : localeinfo.TOOLTIP_CAST_SPEED,
		item.APPLY_HP_REGEN : localeinfo.TOOLTIP_HP_REGEN,
		item.APPLY_SP_REGEN : localeinfo.TOOLTIP_SP_REGEN,
		item.APPLY_POISON_PCT : localeinfo.TOOLTIP_APPLY_POISON_PCT,
		item.APPLY_STUN_PCT : localeinfo.TOOLTIP_APPLY_STUN_PCT,
		item.APPLY_SLOW_PCT : localeinfo.TOOLTIP_APPLY_SLOW_PCT,
		item.APPLY_CRITICAL_PCT : localeinfo.TOOLTIP_APPLY_CRITICAL_PCT,
		item.APPLY_PENETRATE_PCT : localeinfo.TOOLTIP_APPLY_PENETRATE_PCT,

		item.APPLY_ATTBONUS_WARRIOR : localeinfo.TOOLTIP_APPLY_ATTBONUS_WARRIOR,
		item.APPLY_ATTBONUS_ASSASSIN : localeinfo.TOOLTIP_APPLY_ATTBONUS_ASSASSIN,
		item.APPLY_ATTBONUS_SURA : localeinfo.TOOLTIP_APPLY_ATTBONUS_SURA,
		item.APPLY_ATTBONUS_SHAMAN : localeinfo.TOOLTIP_APPLY_ATTBONUS_SHAMAN,
		item.APPLY_ATTBONUS_MONSTER : localeinfo.TOOLTIP_APPLY_ATTBONUS_MONSTER,

		item.APPLY_ATTBONUS_HUMAN : localeinfo.TOOLTIP_APPLY_ATTBONUS_HUMAN,
		item.APPLY_ATTBONUS_ANIMAL : localeinfo.TOOLTIP_APPLY_ATTBONUS_ANIMAL,
		item.APPLY_ATTBONUS_ORC : localeinfo.TOOLTIP_APPLY_ATTBONUS_ORC,
		item.APPLY_ATTBONUS_MILGYO : localeinfo.TOOLTIP_APPLY_ATTBONUS_MILGYO,
		item.APPLY_ATTBONUS_UNDEAD : localeinfo.TOOLTIP_APPLY_ATTBONUS_UNDEAD,
		item.APPLY_ATTBONUS_DEVIL : localeinfo.TOOLTIP_APPLY_ATTBONUS_DEVIL,
		item.APPLY_STEAL_HP : localeinfo.TOOLTIP_APPLY_STEAL_HP,
		item.APPLY_STEAL_SP : localeinfo.TOOLTIP_APPLY_STEAL_SP,
		item.APPLY_MANA_BURN_PCT : localeinfo.TOOLTIP_APPLY_MANA_BURN_PCT,
		item.APPLY_DAMAGE_SP_RECOVER : localeinfo.TOOLTIP_APPLY_DAMAGE_SP_RECOVER,
		item.APPLY_BLOCK : localeinfo.TOOLTIP_APPLY_BLOCK,
		item.APPLY_DODGE : localeinfo.TOOLTIP_APPLY_DODGE,
		item.APPLY_RESIST_SWORD : localeinfo.TOOLTIP_APPLY_RESIST_SWORD,
		item.APPLY_RESIST_TWOHAND : localeinfo.TOOLTIP_APPLY_RESIST_TWOHAND,
		item.APPLY_RESIST_DAGGER : localeinfo.TOOLTIP_APPLY_RESIST_DAGGER,
		item.APPLY_RESIST_BELL : localeinfo.TOOLTIP_APPLY_RESIST_BELL,
		item.APPLY_RESIST_FAN : localeinfo.TOOLTIP_APPLY_RESIST_FAN,
		item.APPLY_RESIST_BOW : localeinfo.TOOLTIP_RESIST_BOW,
		item.APPLY_RESIST_FIRE : localeinfo.TOOLTIP_RESIST_FIRE,
		item.APPLY_RESIST_ELEC : localeinfo.TOOLTIP_RESIST_ELEC,
		item.APPLY_RESIST_MAGIC : localeinfo.TOOLTIP_RESIST_MAGIC,
		item.APPLY_RESIST_WIND : localeinfo.TOOLTIP_APPLY_RESIST_WIND,
		item.APPLY_REFLECT_MELEE : localeinfo.TOOLTIP_APPLY_REFLECT_MELEE,
		item.APPLY_REFLECT_CURSE : localeinfo.TOOLTIP_APPLY_REFLECT_CURSE,
		item.APPLY_POISON_REDUCE : localeinfo.TOOLTIP_APPLY_POISON_REDUCE,
		item.APPLY_KILL_SP_RECOVER : localeinfo.TOOLTIP_APPLY_KILL_SP_RECOVER,
		item.APPLY_EXP_DOUBLE_BONUS : localeinfo.TOOLTIP_APPLY_EXP_DOUBLE_BONUS,
		item.APPLY_GOLD_DOUBLE_BONUS : localeinfo.TOOLTIP_APPLY_GOLD_DOUBLE_BONUS,
		item.APPLY_ITEM_DROP_BONUS : localeinfo.TOOLTIP_APPLY_ITEM_DROP_BONUS,
		item.APPLY_POTION_BONUS : localeinfo.TOOLTIP_APPLY_POTION_BONUS,
		item.APPLY_KILL_HP_RECOVER : localeinfo.TOOLTIP_APPLY_KILL_HP_RECOVER,
		item.APPLY_IMMUNE_STUN : localeinfo.TOOLTIP_APPLY_IMMUNE_STUN,
		item.APPLY_IMMUNE_SLOW : localeinfo.TOOLTIP_APPLY_IMMUNE_SLOW,
		item.APPLY_IMMUNE_FALL : localeinfo.TOOLTIP_APPLY_IMMUNE_FALL,
		item.APPLY_BOW_DISTANCE : localeinfo.TOOLTIP_BOW_DISTANCE,
		item.APPLY_DEF_GRADE_BONUS : localeinfo.TOOLTIP_DEF_GRADE,
		item.APPLY_ATT_GRADE_BONUS : localeinfo.TOOLTIP_ATT_GRADE,
		item.APPLY_MAGIC_ATT_GRADE : localeinfo.TOOLTIP_MAGIC_ATT_GRADE,
		item.APPLY_MAGIC_DEF_GRADE : localeinfo.TOOLTIP_MAGIC_DEF_GRADE,
		item.APPLY_MAX_STAMINA : localeinfo.TOOLTIP_MAX_STAMINA,
		item.APPLY_MALL_ATTBONUS : localeinfo.TOOLTIP_MALL_ATTBONUS,
		item.APPLY_MALL_DEFBONUS : localeinfo.TOOLTIP_MALL_DEFBONUS,
		item.APPLY_MALL_EXPBONUS : localeinfo.TOOLTIP_MALL_EXPBONUS,
		item.APPLY_MALL_ITEMBONUS : localeinfo.TOOLTIP_MALL_ITEMBONUS,
		item.APPLY_MALL_GOLDBONUS : localeinfo.TOOLTIP_MALL_GOLDBONUS,
		item.APPLY_SKILL_DAMAGE_BONUS : localeinfo.TOOLTIP_SKILL_DAMAGE_BONUS,
		item.APPLY_NORMAL_HIT_DAMAGE_BONUS : localeinfo.TOOLTIP_NORMAL_HIT_DAMAGE_BONUS,
		item.APPLY_SKILL_DEFEND_BONUS : localeinfo.TOOLTIP_SKILL_DEFEND_BONUS,
		item.APPLY_NORMAL_HIT_DEFEND_BONUS : localeinfo.TOOLTIP_NORMAL_HIT_DEFEND_BONUS,
		item.APPLY_RESIST_WARRIOR : localeinfo.TOOLTIP_APPLY_RESIST_WARRIOR,
		item.APPLY_RESIST_ASSASSIN : localeinfo.TOOLTIP_APPLY_RESIST_ASSASSIN,
		item.APPLY_RESIST_SURA : localeinfo.TOOLTIP_APPLY_RESIST_SURA,
		item.APPLY_RESIST_SHAMAN : localeinfo.TOOLTIP_APPLY_RESIST_SHAMAN,
		item.APPLY_MAX_HP_PCT : localeinfo.TOOLTIP_APPLY_MAX_HP_PCT,
		item.APPLY_MAX_SP_PCT : localeinfo.TOOLTIP_APPLY_MAX_SP_PCT,
		item.APPLY_ENERGY : localeinfo.TOOLTIP_ENERGY,
		item.APPLY_COSTUME_ATTR_BONUS : localeinfo.TOOLTIP_COSTUME_ATTR_BONUS,

		item.APPLY_MAGIC_ATTBONUS_PER : localeinfo.TOOLTIP_MAGIC_ATTBONUS_PER,
		item.APPLY_MELEE_MAGIC_ATTBONUS_PER : localeinfo.TOOLTIP_MELEE_MAGIC_ATTBONUS_PER,
		item.APPLY_RESIST_ICE : localeinfo.TOOLTIP_RESIST_ICE,
		item.APPLY_RESIST_EARTH : localeinfo.TOOLTIP_RESIST_EARTH,
		item.APPLY_RESIST_DARK : localeinfo.TOOLTIP_RESIST_DARK,
		item.APPLY_ANTI_CRITICAL_PCT : localeinfo.TOOLTIP_ANTI_CRITICAL_PCT,
		item.APPLY_ANTI_PENETRATE_PCT : localeinfo.TOOLTIP_ANTI_PENETRATE_PCT,
		item.APPLY_DESIGN_1 : localeinfo.TOOLTIP_APPLY_DESIGN_1,
		item.APPLY_FISHING_RARE : localeinfo.TOOLTIP_APPLY_FISHING_RARE,
		
		item.APPLY_ATT_RACES : localeinfo.TOOLTIP_APPLY_ATT_RACES,
		item.APPLY_RES_RACES : localeinfo.TOOLTIP_APPLY_RES_RACES,
	}

	if app.ENABLE_APPLY_MAGICRES_REDUCTION:
		AFFECT_DICT[item.APPLY_RESIST_MAGIC_REDUCTION] = localeinfo.TOOLTIP_RESIST_MAGIC_REDUCTION

	if app.ENABLE_NEW_BONUS_TALISMAN:
		AFFECT_DICT.update({
			item.APPLY_ATTBONUS_IRR_SPADA : localeinfo.TOOLTIP_APPLY_ATTBONUS_IRR_SPADA,
			item.APPLY_ATTBONUS_IRR_SPADONE : localeinfo.TOOLTIP_APPLY_ATTBONUS_IRR_SPADONE,
			item.APPLY_ATTBONUS_IRR_PUGNALE : localeinfo.TOOLTIP_APPLY_ATTBONUS_IRR_PUGNALE,
			item.APPLY_ATTBONUS_IRR_FRECCIA : localeinfo.TOOLTIP_APPLY_ATTBONUS_IRR_FRECCIA,
			item.APPLY_ATTBONUS_IRR_VENTAGLIO : localeinfo.TOOLTIP_APPLY_ATTBONUS_IRR_VENTAGLIO,
			item.APPLY_ATTBONUS_IRR_CAMPANA : localeinfo.TOOLTIP_APPLY_ATTBONUS_IRR_CAMPANA,
			item.APPLY_RESIST_MEZZIUOMINI : localeinfo.TOOLTIP_APPLY_RESIST_MEZZIUOMINI,
			item.APPLY_DEF_TALISMAN : localeinfo.TOOLTIP_APPLY_DEF_TALISMAN,
			item.APPLY_ATTBONUS_FORT_ZODIAC	: localeinfo.TOOLTIP_APPLY_ATTBONUS_FORT_ZODIAC,
			item.APPLY_ATTBONUS_DESERT : localeinfo.TOOLTIP_APPLY_ATTBONUS_DESERT,
			item.APPLY_ATTBONUS_INSECT: localeinfo.TOOLTIP_APPLY_ATTBONUS_INSECT,
			item.APPLY_ATTBONUS_ELEC : localeinfo.TOOLTIP_APPLY_ATTBONUS_ELEC,
			item.APPLY_ATTBONUS_FIRE : localeinfo.TOOLTIP_APPLY_ATTBONUS_FIRE,
			item.APPLY_ATTBONUS_ICE : localeinfo.TOOLTIP_APPLY_ATTBONUS_ICE,
			item.APPLY_ATTBONUS_WIND : localeinfo.TOOLTIP_APPLY_ATTBONUS_WIND,
			item.APPLY_ATTBONUS_EARTH : localeinfo.TOOLTIP_APPLY_ATTBONUS_EARTH,
			item.APPLY_ATTBONUS_DARK : localeinfo.TOOLTIP_APPLY_ATTBONUS_DARK,
		})
	
	if app.ENABLE_STRONG_METIN:
		AFFECT_DICT.update({
			item.APPLY_ATTBONUS_METIN : localeinfo.TOOLTIP_APPLY_ATTBONUS_METIN,
		})
		
	if app.ENABLE_STRONG_BOSS:
		AFFECT_DICT.update({
			item.APPLY_ATTBONUS_BOSS : localeinfo.TOOLTIP_APPLY_ATTBONUS_BOSS,
		})
		
	if app.ENABLE_RESIST_MONSTER:
		AFFECT_DICT.update({
			item.APPLY_RESIST_MONSTER : localeinfo.TOOLTIP_APPLY_RESIST_MONSTER,
		})
	
	if app.ENABLE_MEDI_PVM:
		AFFECT_DICT.update({
			item.APPLY_ATTBONUS_MEDI_PVM : localeinfo.TOOLTIP_APPLY_ATTBONUS_MEDI_PVM,
		})
		
	AFFECT_DICT.update({item.APPLY_PVM_CRITICAL_PCT : localeinfo.TOOLTIP_APPLY_PVM_CRITICAL_PCT,})
	
	if app.ENABLE_NEW_COMMON_BONUSES:
		AFFECT_DICT.update(
							{
								item.APPLY_DOUBLE_DROP_ITEM : localeinfo.TOOLTIP_DOUBLE_DROP_ITEM,
								item.APPLY_IRR_WEAPON_DEFENSE : localeinfo.TOOLTIP_IRR_WEAPON_DEFENSE,
							},
		)

	if app.ENABLE_HALLOWEEN_EVENT_2022 and not app.USE_NO_HALLOWEEN_EVENT_2022_BONUSES:
		AFFECT_DICT[item.APPLY_FEAR] = localeinfo.TOOLTIP_APPLY_FEAR
		AFFECT_DICT[item.APPLY_RESISTANCE_FEAR] = localeinfo.TOOLTIP_APPLY_RESISTANCE_FEAR

	if app.ENABLE_NEW_USE_POTION:
		AFFECT_DICT.update({
			item.APPLY_PARTY_DROPEXP : localeinfo.APPLY_PARTY_DROPEXP,
		})
	
	ANTI_FLAG_DICT = {
		0 : item.ITEM_ANTIFLAG_WARRIOR,
		1 : item.ITEM_ANTIFLAG_ASSASSIN,
		2 : item.ITEM_ANTIFLAG_SURA,
		3 : item.ITEM_ANTIFLAG_SHAMAN,
	}

	if app.ENABLE_RARITY_SYSTEM:
		RARITY_DESC = {
			item.RARITY_COMMON : localeinfo.RARITY_ITEM_COMMON,
			item.RARITY_EPIC : localeinfo.RARITY_ITEM_EPIC,
			item.RARITY_LEGENDARY : localeinfo.RARITY_ITEM_LEGENDARY,
			item.RARITY_ANTIQUE : localeinfo.RARITY_ITEM_ANTIQUE,
			item.RARITY_MISTIC : localeinfo.RARITY_ITEM_MISTIC,
		}
		
		RARITY_COLOR = {
			item.RARITY_COMMON : grp.GenerateColor(1.0,1.0,1.0,1.0),
			item.RARITY_EPIC : grp.GenerateColor(0.1,0.1,1.0,1.0),
			item.RARITY_LEGENDARY : grp.GenerateColor(1.0, 0.1, 0.1,1.0),
			item.RARITY_ANTIQUE : grp.GenerateColor(0.5,1.0,0.1,1.0),
			item.RARITY_MISTIC : grp.GenerateColor(1.0, 0.2, 1.0, 1.0),
		}

	if app.ENABLE_NEW_EXTRA_BONUS:
		NEW_EXTRA_BONUS_COLOR = grp.GenerateColor(1.0, 0.8, 0.1, 1.0)

	FONT_COLOR = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)

	if app.ENABLE_BUY_WITH_ITEM:
		dictIconSize = {}

	def __init__(self, *args, **kwargs):
		ToolTip.__init__(self, *args, **kwargs)
		self.itemVnum = 0
		self.isShopItem = False
		self.lastWidthIncrease = 0
		self.bCannotUseItemForceSetDisableColor = True
		self.isItemShop=0

	def __del__(self):
		ToolTip.__del__(self)

	if app.ENABLE_NEW_PET_EDITS:
		def AppendPetBonus(self, secs):
			idx = 1
			if secs >= 950400 and secs < 2246400:
				idx = 2
			
			if secs >= 2246400 and secs < 4147200:
				idx = 3
			
			if secs >= 4147200:
				idx = 4
			
			val = [[0, 500, 1200, 2100, 3000], [0, 2, 4, 7, 10], [0, 2, 4, 7, 10]]
			self.AppendTextLine(self.__GetAffectString(item.APPLY_MAX_HP, val[0][idx]))
			self.AppendTextLine(self.__GetAffectString(item.APPLY_RESIST_MONSTER, val[1][idx]))
			self.AppendTextLine(self.__GetAffectString(item.APPLY_RESIST_MEZZIUOMINI, val[2][idx]))

	def CanViewRendering(self):
		race = player.GetRace()
		job = chr.RaceToJob(race)
		if not self.ANTI_FLAG_DICT.has_key(job):
			return False

		if item.IsAntiFlag(self.ANTI_FLAG_DICT[job]):
			return False

		sex = chr.RaceToSex(race)
		
		MALE = 1
		FEMALE = 0

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE) and sex == MALE:
			return False

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE) and sex == FEMALE:
			return False

		return True

	def CanViewRenderingSex(self):
		race = player.GetRace()
		sex = chr.RaceToSex(race)
		
		MALE = 1
		FEMALE = 0

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE) and sex == MALE:
			return False

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE) and sex == FEMALE:
			return False

		return True

	def SetCannotUseItemForceSetDisableColor(self, enable):
		self.bCannotUseItemForceSetDisableColor = enable

	def CanEquip(self):
		if not item.IsEquipmentVID(self.itemVnum):
			return True

		race = player.GetRace()
		job = chr.RaceToJob(race)
		if not self.ANTI_FLAG_DICT.has_key(job):
			return False

		if item.IsAntiFlag(self.ANTI_FLAG_DICT[job]):
			return False

		sex = chr.RaceToSex(race)

		MALE = 1
		FEMALE = 0

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE) and sex == MALE:
			return False

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE) and sex == FEMALE:
			return False

		for i in xrange(item.LIMIT_MAX_NUM):
			(limitType, limitValue) = item.GetLimit(i)

			if item.LIMIT_LEVEL == limitType:
				if player.GetStatus(player.LEVEL) < limitValue:
					return False
			"""
			elif item.LIMIT_STR == limitType:
				if player.GetStatus(player.ST) < limitValue:
					return False
			elif item.LIMIT_DEX == limitType:
				if player.GetStatus(player.DX) < limitValue:
					return False
			elif item.LIMIT_INT == limitType:
				if player.GetStatus(player.IQ) < limitValue:
					return False
			elif item.LIMIT_CON == limitType:
				if player.GetStatus(player.HT) < limitValue:
					return False
			"""

		return True

	def AppendTextLine(self, text, color = FONT_COLOR, centerAlign = True, height = 0):
		if not self.CanEquip() and self.bCannotUseItemForceSetDisableColor:
			color = self.DISABLE_COLOR
		
		try:
			if len(text) > 32:
				newWidth = (len(text) - 32) * 4
				if self.lastWidthIncrease < newWidth:
					self.toolTipWidth -= self.lastWidthIncrease
					self.toolTipWidth += newWidth
					self.lastWidthIncrease = newWidth
					j = 0
					for item in self.childrenList:
						found = False
						for i in self.childrenListStone:
							if i == j:
								found = True
								break
						
						j += 1
						if found == False:
							(x, y) = item.GetLocalPosition()
							item.SetPosition(self.toolTipWidth / 2, y)
			
			self.ResizeToolTip()
			return ToolTip.AppendTextLine(self, text, color, centerAlign, height)
		except:
			pass

	def ClearToolTip(self):
		self.isShopItem = False
		self.lastWidthIncrease = 0
		self.toolTipWidth = self.TOOL_TIP_WIDTH
		ToolTip.ClearToolTip(self)

	if app.ENABLE_DS_SET:
		def AppendDSTextLine(self, text, color = FONT_COLOR, centerAlign = True, first = False):
			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetFontName(self.defFontName)
			textLine.SetPackedFontColor(color)
			textLine.SetText(text)
			textLine.SetOutline()
			textLine.SetFeather(False)
			textLine.Show()
			if first:
				self.toolTipWidth += self.toolTipWidth
			
			if centerAlign:
				textLine.SetPosition(self.toolTipWidth/2, self.toolTipHeight)
				textLine.SetHorizontalAlignCenter()
			else:
				textLine.SetPosition(10, self.toolTipHeight)
			
			self.childrenList.append(textLine)
			self.toolTipHeight += self.TEXT_LINE_HEIGHT
			
			self.ResizeToolTip()
			return textLine

		def AppendDSSetInfo(self):
			self.AppendDSTextLine(localeinfo.DS_SET_INFO, self.FONT_COLOR, True, True)
			self.AppendDSTextLine(self.__GetAffectString(item.APPLY_ATTBONUS_METIN, 10))
			self.AppendDSTextLine(self.__GetAffectString(item.APPLY_ATTBONUS_MONSTER, 10))
			self.AppendDSTextLine(self.__GetAffectString(item.APPLY_MAX_HP, 1000))

	if app.ENABLE_CHANGELOOK:
		def AppendChangeLookInformation(self, window_type, slotIndex, transmutation = -1):
			if transmutation == -1:
				if window_type == player.SAFEBOX:
					changelookvnum = safebox.GetItemTransmutation(slotIndex)
				elif window_type == player.MALL:
					changelookvnum = safebox.GetMallItemTransmutation(slotIndex)
				else:
					changelookvnum = player.GetItemTransmutation(window_type, slotIndex)
			else:
				changelookvnum = transmutation
			
			if changelookvnum > 0:
				self.AppendSpace(5)
				self.AppendTextLine("[ " + localeinfo.CHANGE_LOOK_TITLE + " ]", self.CHANGELOOK_TITLE_COLOR)
				itemName = item.GetItemNameByVnum(changelookvnum)
				
				if item.GetItemType() == item.ITEM_TYPE_COSTUME:
					if item.GetItemSubType() == item.COSTUME_TYPE_BODY:
						malefemale = ""
						if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE):
							malefemale = localeinfo.FOR_FEMALE
						
						if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE):
							malefemale = localeinfo.FOR_MALE
						itemName += " ( " + malefemale + " )"
				
				textLine = self.AppendTextLine(itemName, self.CHANGELOOK_ITEMNAME_COLOR, True)
				textLine.SetFeather()

		def AppendChangeLookInfoItemVnum(self, changelookvnum):
			if changelookvnum == 0:
				return
			
			self.AppendSpace(5)
			self.AppendTextLine("[ " + localeinfo.CHANGE_LOOK_TITLE + " ]", self.CHANGELOOK_TITLE_COLOR)
			itemName = item.GetItemNameByVnum(changelookvnum)
			
			if item.GetItemType() == item.ITEM_TYPE_COSTUME:
				if item.GetItemSubType() == item.COSTUME_TYPE_BODY:
					malefemale = ""
					if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE):
						malefemale = localeinfo.FOR_FEMALE
					
					if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE):
						malefemale = localeinfo.FOR_MALE
					itemName += " ( " + malefemale + " )"
			
			textLine = self.AppendTextLine(itemName, self.CHANGELOOK_ITEMNAME_COLOR, True)
			textLine.SetFeather()

		def AppendChangeLookInfoShopWindow(self, slotIndex):
			changelookvnum = shop.GetItemTransmutation(slotIndex)
			if not changelookvnum == 0:
				self.AppendSpace(5)
				self.AppendTextLine("[ " + localeinfo.CHANGE_LOOK_TITLE + " ]", self.CHANGELOOK_TITLE_COLOR)
				itemName = item.GetItemNameByVnum(changelookvnum)
				
				if item.GetItemType() == item.ITEM_TYPE_COSTUME:
					if item.GetItemSubType() == item.COSTUME_TYPE_BODY:
						malefemale = ""
						if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE):
							malefemale = localeinfo.FOR_FEMALE
						
						if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE):
							malefemale = localeinfo.FOR_MALE
						
						itemName += " ( " + malefemale + " )"
				
				textLine = self.AppendTextLine(itemName, self.CHANGELOOK_ITEMNAME_COLOR, True)
				textLine.SetFeather()

	if app.ENABLE_ATTR_TRANSFER_SYSTEM:
		def SetInventoryItemAttrTransfer(self, slotIndex_1, slotIndex_2, window_type = player.INVENTORY):
			itemVnum = player.GetItemIndex(window_type, slotIndex_1)
			itemVnum_2 = player.GetItemIndex(window_type, slotIndex_2)
			if itemVnum == 0 or itemVnum_2 == 0:
				return
			
			self.ClearToolTip()
			if shop.IsOpen():
				if not shop.IsPrivateShop():
					item.SelectItem(itemVnum)
					self.AppendSellingPrice(player.GetISellItemPrice(window_type, slotIndex_2))
			
			metinSlot = [player.GetItemMetinSocket(window_type, slotIndex_2, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attrSlot = [player.GetItemAttribute(window_type, slotIndex_2, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			
			trans = -1
			if app.ENABLE_CHANGELOOK:
				if window_type == player.SAFEBOX:
					trans = safebox.GetItemTransmutation(slotIndex_1)
				elif window_type == player.MALL:
					trans = safebox.GetMallItemTransmutation(slotIndex_1)
				else:
					trans = player.GetItemTransmutation(window_type, slotIndex_1)
				
				if trans == 0:
					trans = -1
			
			self.AddItemDataAttrTransfer(itemVnum, itemVnum_2, metinSlot, attrSlot, 0, 0, window_type, slotIndex_2, transmutation = trans)

		def AddItemDataAttrTransfer(self, itemVnum, itemVnum_2, metinSlot, attrSlot = 0, flags = 0, unbindTime = 0, window_type = player.INVENTORY, slotIndex = -1, transmutation = -1):
			self.itemVnum = itemVnum
			item.SelectItem(itemVnum)
			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()
			itemDesc = item.GetItemDescription()
			itemSummary = item.GetItemSummary()
			
			self.__SetItemTitle(itemVnum, metinSlot, attrSlot)
			self.AppendDescription(itemDesc, 26)
			self.AppendDescription(itemSummary, 26, self.CONDITION_COLOR)
			self.__AppendLimitInformation()
			self.__AppendAffectInformation()
			
			item.SelectItem(itemVnum_2)
			if app.ENABLE_STOLE_COSTUME and itemSubType == item.COSTUME_TYPE_STOLE:
				self.__AppendAttributeInformation(attrSlot, 0, -1, True, isAttrCostume = 1)
			else:
				self.__AppendAttributeInformation(attrSlot, 0, -1, True)
			
			item.SelectItem(itemVnum)
			
			if app.ENABLE_CHANGELOOK:
				self.AppendChangeLookInformation(window_type, slotIndex, transmutation)
			
			self.AppendWearableInformation(itemVnum, transmutation)
			bHasRealtimeFlag = 0
			for i in xrange(item.LIMIT_MAX_NUM):
				(limitType, limitValue) = item.GetLimit(i)
				if item.LIMIT_REAL_TIME == limitType:
					bHasRealtimeFlag = 1
			
			if 1 == bHasRealtimeFlag:
				self.AppendMallItemLastTime(metinSlot[0])
			
			self.ShowToolTip()

	def SetInventoryItem(self, slotIndex, window_type = player.INVENTORY):
		itemVnum = player.GetItemIndex(window_type, slotIndex)
		if 0 == itemVnum:
			return

		self.ClearToolTip()
		if shop.IsOpen():
			if not shop.IsPrivateShop():
				item.SelectItem(itemVnum)
				self.AppendSellingPrice(player.GetISellItemPrice(window_type, slotIndex))

		metinSlot = [player.GetItemMetinSocket(window_type, slotIndex, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
		attrSlot = [player.GetItemAttribute(window_type, slotIndex, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]

		if app.ATTR_LOCK:
			lockedattr = player.GetItemAttrLocked(window_type, slotIndex)
			self.AddItemData(itemVnum, metinSlot, attrSlot, 0, 0, window_type, slotIndex, lockedattr)
		else:
			self.AddItemData(itemVnum, metinSlot, attrSlot, 1, 0, 0, window_type, slotIndex)
		
		if app.INGAME_WIKI:
			self.AppendChestDropInfo(itemVnum)

	def SetShopItem(self, slotIndex):
		itemVnum = shop.GetItemID(slotIndex)
		if 0 == itemVnum:
			return

		price = shop.GetItemPrice(slotIndex)
		self.ClearToolTip()
		self.isShopItem = True

		item.SelectItem(itemVnum)
		itemType = item.GetItemType()
		itemSubType = item.GetItemSubType()

		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(shop.GetItemMetinSocket(slotIndex, i))
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(shop.GetItemAttribute(slotIndex, i))
		if app.ATTR_LOCK:
			lockedattr = shop.GetItemAttrLocked(slotIndex)
			self.AddItemData(itemVnum, metinSlot, attrSlot, 0, 0, player.INVENTORY, -1, lockedattr, shop = True)
		else:
			self.AddItemData(itemVnum, metinSlot, attrSlot, shop = True)
		
		if app.ENABLE_CHANGELOOK:
			self.AppendChangeLookInfoShopWindow(slotIndex)
		
		if app.ENABLE_BUY_WITH_ITEM:
			self.AppendSpace(5)
			#self.AppendTextLine(localeinfo.ITEM_PRICE_TITLE, self.GetPriceColor(price))
			priced = False
			if price > 0:
				self.AppendPrice(price)
				priced = True
			
			for i in xrange(shop.MAX_SHOP_PRICES):
				(vnum, count) = shop.GetBuyWithItem(slotIndex, i)
				if vnum > 0 and count > 0:
					if not priced:
						priced = True
					
					self.AppendPrice_WithItem(vnum, count)
			
			if not priced:
				self.AppendTextLine(localeinfo.ITEM_IS_FREE, self.GetPriceColor(price))
		else:
			self.AppendPrice(price)
		
		if app.ENABLE_BUY_STACK_FROM_SHOP:
			self.AppendSpace(5)
			self.AppendTextLine(localeinfo.MULTIPLE_BUYS)
		
		if app.INGAME_WIKI:
			self.AppendChestDropInfo(itemVnum)
		
		#if item.ITEM_TYPE_WEAPON == itemType and itemSubType != 6:
		#	race = 0
		#	if itemSubType == item.WEAPON_SWORD:
		#		if player.GetRace() == 6 or player.GetRace() == 2:
		#			race = player.GetRace()
		#		if player.GetRace() != 3 and player.GetRace() != 7 and player.GetRace() != 8:
		#			race = player.GetRace()
		#			if item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR) and item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN) and race != 6 and race != 2:
		#				if chr.RaceToSex(player.GetRace()) == 1:
		#					race = 2
		#				else:
		#					race = 6
		#		else:
		#			if chr.RaceToSex(player.GetRace()) == 1:
		#				race = 0
		#			else:
		#				race = 4
		#			
		#			if item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR) and item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN) and race != 6 and race != 2:
		#				if chr.RaceToSex(player.GetRace()) == 1:
		#					race = 2
		#				else:
		#					race = 6
		#	elif itemSubType == item.WEAPON_TWO_HANDED:
		#		if player.GetRace() != 0 and player.GetRace() != 4:
		#			if chr.RaceToSex(player.GetRace()) == 1:
		#				race = 0
		#			else:
		#				race = 4
		#		else:
		#			race = player.GetRace()
		#	elif itemSubType == item.WEAPON_DAGGER or itemSubType == item.WEAPON_BOW:
		#		if player.GetRace() != 1 and player.GetRace() != 5:
		#			if chr.RaceToSex(player.GetRace()) == 1:
		#				race = 5
		#			else:
		#				race = 1
		#		else:
		#			race = player.GetRace()
		#	elif itemSubType == item.WEAPON_BELL or itemSubType == item.WEAPON_FAN:
		#		if player.GetRace() != 3 and player.GetRace() != 7:
		#			if chr.RaceToSex(player.GetRace()) == 1:
		#				race = 7
		#			else:
		#				race = 3
		#		else:
		#			race = player.GetRace()
		#	
		#	self.__ModelPreview(itemVnum, 3, race)
		#elif item.ITEM_TYPE_ARMOR == itemType and itemSubType == 0:
		#	self.__ModelPreview(itemVnum, 2, self.__ItemGetRace(1))
		#elif item.ITEM_TYPE_COSTUME == itemType:
		#		if itemSubType == 0: #body
		#			self.__ModelPreview(itemVnum, 2, self.__ItemGetRace(1, 1))
		#		elif itemSubType == 1: #Hair 
		#			self.__ModelPreview(item.GetValue(3), 1, self.__ItemGetRace(1, 1)) #hier in der DB Value3 prufen  
		#		elif itemSubType == 4: #weapon
		#			race = 0
		#			itemRealSubType = item.GetValue(3)
		#			if itemRealSubType == item.WEAPON_SWORD:
		#				if player.GetRace() == 6 or player.GetRace() == 2:
		#					race = player.GetRace()
		#				elif player.GetRace() != 3 and player.GetRace() != 7 and player.GetRace() != 8:
		#					race = player.GetRace()
		#					if item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR) and item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN) and race != 6 and race != 2:
		#						if chr.RaceToSex(player.GetRace()) == 1:
		#							race = 2
		#						else:
		#							race = 6
		#				else:
		#					if chr.RaceToSex(player.GetRace()) == 1:
		#						race = 0
		#					else:
		#						race = 4
		#					
		#					if item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR) and item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN) and race != 6 and race != 2:
		#						if chr.RaceToSex(player.GetRace()) == 1:
		#							race = 2
		#						else:
		#							race = 6
		#			elif itemRealSubType == item.WEAPON_TWO_HANDED:
		#				if player.GetRace() != 0 and player.GetRace() != 4:
		#					if chr.RaceToSex(player.GetRace()) == 1:
		#						race = 0
		#					else:
		#						race = 4
		#				else:
		#					race = player.GetRace()
		#			elif itemRealSubType == item.WEAPON_DAGGER or itemRealSubType == item.WEAPON_BOW:
		#				if player.GetRace() != 1 and player.GetRace() != 5:
		#					if chr.RaceToSex(player.GetRace()) == 1:
		#						race = 5
		#					else:
		#						race = 1
		#				else:
		#					race = player.GetRace()
		#			elif itemRealSubType == item.WEAPON_BELL or itemRealSubType == item.WEAPON_FAN:
		#				if player.GetRace() != 3 and player.GetRace() != 7:
		#					if chr.RaceToSex(player.GetRace()) == 1:
		#						race = 7
		#					else:
		#						race = 3
		#				else:
		#					race = player.GetRace()
		#			
		#			self.__ModelPreview(itemVnum, 3, race)
		#		elif itemSubType == 6: #pet	
		#			self.__ModelPreview(itemVnum, 0, item.GetValue(0))

	def do_replace(_from, _field, data):
		for f, t in zip(_from, _field):
			data = data.replace(f, t)
		
		return data

	if app.ENABLE_BUY_WITH_ITEM:
		def AppendPrice_WithItem(self, vnum, price):
			item.SelectItem(vnum)
			
			if self.dictIconSize.has_key(vnum):
				height = self.dictIconSize[vnum][0]
				icon = self.dictIconSize[vnum][1]
			else:
				icon = item.GetIconImageFileName()
				iconImage = ui.ImageBox()
				iconImage.SetParent(self)
				iconImage.LoadImage(icon)
				height = iconImage.GetHeight()
				del iconImage
				self.dictIconSize[vnum] = [height, icon, item.GetItemName()]
			
			icon = icon.replace("icon\item\\", "")
			icon = icon.replace(".tga", "")
			
			text = localeinfo.ITEM_IS_WORTH % (price, icon, self.dictIconSize[vnum][2])
			color = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)
			self.AppendTextLine(text, color, True, height)

	def SetShopItemBySecondaryCoin(self, slotIndex):
		itemVnum = shop.GetItemID(slotIndex)
		if 0 == itemVnum:
			return

		price = shop.GetItemPrice(slotIndex)
		self.ClearToolTip()
		self.isShopItem = True

		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(shop.GetItemMetinSocket(slotIndex, i))
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(shop.GetItemAttribute(slotIndex, i))

		if app.ATTR_LOCK:
			lockedattr = shop.GetItemAttrLocked(slotIndex)
			self.AddItemData(itemVnum, metinSlot, attrSlot, 0, 0, player.INVENTORY, -1, lockedattr)
		else:
			self.AddItemData(itemVnum, metinSlot, attrSlot, 1)
		
		self.AppendPriceBySecondaryCoin(price)

	def SetExchangeOwnerItem(self, slotIndex):
		itemVnum = exchange.GetItemVnumFromSelf(slotIndex)
		if 0 == itemVnum:
			return

		self.ClearToolTip()

		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(exchange.GetItemMetinSocketFromSelf(slotIndex, i))
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(exchange.GetItemAttributeFromSelf(slotIndex, i))
		
		trans = -1
		if app.ENABLE_CHANGELOOK:
			trans = exchange.GetItemTransmutation(slotIndex, True)
			if trans == 0:
				trans = -1
		
		if app.ATTR_LOCK:
			lattr = exchange.GetItemAttrLocked(slotIndex, True)
			self.AddItemData(itemVnum, metinSlot, attrSlot, 0, 0, player.INVENTORY, -1, lockedattr = lattr, transmutation = trans)
		else:
			self.AddItemData(itemVnum, metinSlot, attrSlot)

	def SetExchangeTargetItem(self, slotIndex):
		itemVnum = exchange.GetItemVnumFromTarget(slotIndex)
		if 0 == itemVnum:
			return

		self.ClearToolTip()

		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(exchange.GetItemMetinSocketFromTarget(slotIndex, i))
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(exchange.GetItemAttributeFromTarget(slotIndex, i))
		
		trans = -1
		if app.ENABLE_CHANGELOOK:
			trans = exchange.GetItemTransmutation(slotIndex, False)
			if trans == 0:
				trans = -1
		
		if app.ATTR_LOCK:
			lattr = exchange.GetItemAttrLocked(slotIndex, False)
			self.AddItemData(itemVnum, metinSlot, attrSlot, 0, 0, player.INVENTORY, -1, lockedattr = lattr, transmutation = trans)
		else:
			self.AddItemData(itemVnum, metinSlot, attrSlot)

	def SetPrivateShopBuilderItem(self, invenType, invenPos, privateShopSlotIndex):
		itemVnum = player.GetItemIndex(invenType, invenPos)
		if 0 == itemVnum:
			return

		item.SelectItem(itemVnum)
		self.ClearToolTip()
		self.AppendSellingPrice(shop.GetPrivateShopItemPrice(invenType, invenPos))

		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(player.GetItemMetinSocket(invenPos, i))
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(player.GetItemAttribute(invenPos, i))

		self.AddItemData(itemVnum, metinSlot, attrSlot)

	def SetSafeBoxItem(self, slotIndex):
		itemVnum = safebox.GetItemID(slotIndex)
		if not itemVnum:
			return
		
		self.ClearToolTip()
		
		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(safebox.GetItemMetinSocket(slotIndex, i))
		
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(safebox.GetItemAttribute(slotIndex, i))
		
		self.AddItemData(itemVnum,metinSlot,attrSlot,safebox.GetItemFlags(slotIndex),\
			lockedattr=safebox.GetItemAttrLocked(slotIndex) if app.ATTR_LOCK else 0,\
			transmutation=safebox.GetItemTransmutation(slotIndex) if app.ENABLE_CHANGELOOK else 0)
		
		if app.INGAME_WIKI:
			self.AppendChestDropInfo(itemVnum)

	def SetMallItem(self, slotIndex):
		itemVnum = safebox.GetMallItemID(slotIndex)
		if not itemVnum:
			return
		
		self.ClearToolTip()
		
		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(safebox.GetMallItemMetinSocket(slotIndex, i))
		
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(safebox.GetMallItemAttribute(slotIndex, i))
		
		self.AddItemData(itemVnum, metinSlot, attrSlot)
		
		if app.INGAME_WIKI:
			self.AppendChestDropInfo(itemVnum)

	def SetItemToolTip(self, itemVnum, isGayaShop = 0, isItemShop = 0):
		self.ClearToolTip()
		
		if not itemVnum:
			return
		
		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(0)
		
		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append((0, 0))
		
		if app.ENABLE_GAYA_RENEWAL and isGayaShop == 1:
			item.SelectItem(itemVnum)
			for i in xrange(item.LIMIT_MAX_NUM):
				(limitType, limitValue) = item.GetLimit(i)
				if item.LIMIT_TIMER_BASED_ON_WEAR == limitType:
					metinSlot[0] = limitValue
				elif item.LIMIT_REAL_TIME == limitType or item.LIMIT_REAL_TIME_START_FIRST_USE == limitType:
					metinSlot[0] = limitValue
		elif app.ENABLE_INGAME_ITEMSHOP and isItemShop == 1:
			self.isItemShop=1
			
			item.SelectItem(itemVnum)
			for i in xrange(item.LIMIT_MAX_NUM):
				(limitType, limitValue) = item.GetLimit(i)
				if item.LIMIT_TIMER_BASED_ON_WEAR == limitType:
					metinSlot[0] = limitValue
				elif item.LIMIT_REAL_TIME == limitType or item.LIMIT_REAL_TIME_START_FIRST_USE == limitType:
					metinSlot[0] = app.GetGlobalTimeStamp() + limitValue
		
		self.AddItemData(itemVnum, metinSlot, attrSlot)

	def __AppendAttackSpeedInfo(self, item):
		atkSpd = item.GetValue(0)

		if atkSpd < 80:
			stSpd = localeinfo.TOOLTIP_ITEM_VERY_FAST
		elif atkSpd <= 95:
			stSpd = localeinfo.TOOLTIP_ITEM_FAST
		elif atkSpd <= 105:
			stSpd = localeinfo.TOOLTIP_ITEM_NORMAL
		elif atkSpd <= 120:
			stSpd = localeinfo.TOOLTIP_ITEM_SLOW
		else:
			stSpd = localeinfo.TOOLTIP_ITEM_VERY_SLOW

		self.AppendTextLine(localeinfo.TOOLTIP_ITEM_ATT_SPEED % stSpd, self.NORMAL_COLOR)

	def __AppendAttackGradeInfo(self):
		atkGrade = item.GetValue(1)
		self.AppendTextLine(localeinfo.TOOLTIP_ITEM_ATT_GRADE % atkGrade, self.GetChangeTextLineColor(atkGrade))


	if app.ENABLE_ACCE_SYSTEM:
		# def CalcAcceValue(self, value, abs):
			# if not value:
				# return 0
			
			# valueCalc = int((round(value * abs) / 100) - 0.5) + int(int((round(value * abs) / 100) - 0.5) > 0)
			# if valueCalc <= 0 and value > 0:
				# value = 1
			# else:
				# value = valueCalc
			
			# return value
		
		#@ikd
		def CalcAcceValue(self, value, abs):
			if not value:
				return 0
			
			valueCalc 	= round(value * abs) / 100
			valueCalc 	-= 0.5
			valueCalc 	= int(valueCalc) +1 if valueCalc > 0 else int(valueCalc)
			value 		= 1 if (valueCalc <= 0 and value > 0) else valueCalc
			return value
	

	def __AppendAttackPowerInfo(self ,itemAbsChance = 0):
		minPower = item.GetValue(3)
		maxPower = item.GetValue(4)
		addPower = item.GetValue(5)
		
		if app.ENABLE_ACCE_SYSTEM:
			if itemAbsChance != 0:
				minPower = self.CalcAcceValue(minPower, itemAbsChance)
				maxPower = self.CalcAcceValue(maxPower, itemAbsChance)
				addPower = self.CalcAcceValue(addPower, itemAbsChance)
		
		
		if maxPower > minPower:
			self.AppendTextLine(localeinfo.TOOLTIP_ITEM_ATT_POWER % (minPower+addPower, maxPower+addPower), self.POSITIVE_COLOR)
		else:
			self.AppendTextLine(localeinfo.TOOLTIP_ITEM_ATT_POWER_ONE_ARG % (minPower+addPower), self.POSITIVE_COLOR)

	def __AppendMagicAttackInfo(self, itemAbsChance = 0):
		minMagicAttackPower = item.GetValue(1)
		maxMagicAttackPower = item.GetValue(2)
		addPower = item.GetValue(5)
		
		if app.ENABLE_ACCE_SYSTEM:
			if itemAbsChance != 0:
				minMagicAttackPower = self.CalcAcceValue(minMagicAttackPower, itemAbsChance)
				maxMagicAttackPower = self.CalcAcceValue(maxMagicAttackPower, itemAbsChance)
				addPower 			= self.CalcAcceValue(addPower, itemAbsChance)
		
		


		if minMagicAttackPower > 0 or maxMagicAttackPower > 0:
			if maxMagicAttackPower > minMagicAttackPower:
				self.AppendTextLine(localeinfo.TOOLTIP_ITEM_MAGIC_ATT_POWER % (minMagicAttackPower+addPower, maxMagicAttackPower+addPower), self.POSITIVE_COLOR)
			else:
				self.AppendTextLine(localeinfo.TOOLTIP_ITEM_MAGIC_ATT_POWER_ONE_ARG % (minMagicAttackPower+addPower), self.POSITIVE_COLOR)

	def __AppendMagicDefenceInfo(self, itemAbsChance = 0):
		magicDefencePower = item.GetValue(0)

		if app.ENABLE_ACCE_SYSTEM:
			if itemAbsChance != 0:
				magicDefencePower = self.CalcAcceValue(magicDefencePower, itemAbsChance)
		


		if magicDefencePower > 0:
			self.AppendTextLine(localeinfo.TOOLTIP_ITEM_MAGIC_DEF_POWER % magicDefencePower, self.GetChangeTextLineColor(magicDefencePower))

	def __AppendAttributeInformation(self, attrSlot, itemAbsChance = 0, lockedattr = -1, isTransfer = False, isCostumeStole = 0, isAttrCostume = 0):
		if 0 != attrSlot:

			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				if isTransfer and not isAttrCostume:
					if i == 5 or i == 6:
						continue
				
				type = attrSlot[i][0]
				value = attrSlot[i][1]

				if 0 == value:
					continue

				affectString = self.__GetAffectString(type, value)
				
				if app.ENABLE_ACCE_SYSTEM:
					if item.GetItemType() == item.ITEM_TYPE_COSTUME:
						if item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
							if itemAbsChance != 0:
								value = self.CalcAcceValue(value, itemAbsChance)
								affectString = self.__GetAffectString(type, value)
				
				
				if affectString:
					if isCostumeStole:
						if value > 0:
							affectColor = self.SPECIAL_POSITIVE_COLOR
						elif value == 0:
							affectColor = self.NORMAL_COLOR
						else:
							affectColor = self.NEGATIVE_COLOR
					else:
						if isTransfer and isAttrCostume and i > 4:
							affectColor = self.__GetAttributeColor(4, value)
						else:
							affectColor = self.__GetAttributeColor(i, value)
					
					if i == lockedattr and app.ATTR_LOCK:
						affectColor = 0xff4287f5
						self.AppendTextLine("<< "+affectString+" >>", affectColor)
					else:
						self.AppendTextLine(affectString, affectColor)

	def __GetAttributeColor(self, index, value):
		if value > 0:
			if index >= player.ATTRIBUTE_SLOT_RARE_START and index < player.ATTRIBUTE_SLOT_RARE_END:
				if app.ENABLE_ATTR_COSTUMES and item.GetItemType() == item.ITEM_TYPE_COSTUME and (item.GetItemSubType() == item.COSTUME_TYPE_HAIR or item.GetItemSubType() == item.COSTUME_TYPE_BODY or item.GetItemSubType() == item.COSTUME_TYPE_WEAPON):
					return grp.GenerateColor(1.0, 0.7843, 0.0, 1.0)
				
				return self.ATTR_6TH_7TH_COLOR if app.USE_ATTR_6TH_7TH else self.SPECIAL_POSITIVE_COLOR2
			else:
				return self.SPECIAL_POSITIVE_COLOR
		elif value == 0:
			return self.NORMAL_COLOR
		else:
			return self.NEGATIVE_COLOR

	def __IsPolymorphItem(self, itemVnum):
		if itemVnum >= 70103 and itemVnum <= 70106:
			return 1
		return 0

	def __SetPolymorphItemTitle(self, monsterVnum):
		if localeinfo.IsVIETNAM():
			itemName =item.GetItemName()
			itemName+=" "
			itemName+=nonplayer.GetMonsterName(monsterVnum)
		else:
			itemName =nonplayer.GetMonsterName(monsterVnum)
			itemName+=" "
			itemName+=item.GetItemName()
		self.SetTitle(itemName)

	def __SetNormalItemTitle(self):
		self.SetTitle(item.GetItemName())

	def __SetSpecialItemTitle(self):
		self.AppendTextLine(item.GetItemName(), self.SPECIAL_TITLE_COLOR)

	def __SetItemTitle(self, itemVnum, metinSlot, attrSlot):
		if localeinfo.IsCANADA():
			if 72726 == itemVnum or 72730 == itemVnum:
				self.AppendTextLine(item.GetItemName(), grp.GenerateColor(1.0, 0.7843, 0.0, 1.0))
				return
		
		if app.ENABLE_RARITY_SYSTEM:
			rarity = item.GetRarity()
			if rarity != item.RARITY_COMMON:
				self.AppendTextLine(item.GetItemName(), self.RARITY_COLOR[rarity])
				return
		
		if self.__IsPolymorphItem(itemVnum):
			self.__SetPolymorphItemTitle(metinSlot[0])
		else:
			if self.__IsAttr(attrSlot):
				self.__SetSpecialItemTitle()
				return

			self.__SetNormalItemTitle()

	def __IsAttr(self, attrSlot):
		if not attrSlot:
			return False

		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			type = attrSlot[i][0]
			if 0 != type:
				return True

		return False

	if app.ENABLE_SOUL_SYSTEM:
		def AddRefineItemData(self, itemVnum, metinSlot, attrSlot = 0, type = 0, trans = -1):
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				metinSlotData=metinSlot[i]
				if self.GetMetinItemIndex(metinSlotData) == constinfo.ERROR_METIN_STONE:
					metinSlot[i]=player.METIN_SOCKET_TYPE_SILVER
			
			self.AddItemData(itemVnum, metinSlot, attrSlot, type, transmutation = trans)
	else:
		def AddRefineItemData(self, itemVnum, metinSlot, attrSlot = 0, trans = -1):
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				metinSlotData=metinSlot[i]
				if self.GetMetinItemIndex(metinSlotData) == constinfo.ERROR_METIN_STONE:
					metinSlot[i]=player.METIN_SOCKET_TYPE_SILVER
			
			self.AddItemData(itemVnum, metinSlot, attrSlot, transmutation = trans)
			

	def AddItemData_Offline(self, itemVnum, itemDesc, itemSummary, metinSlot, attrSlot):
		self.__SetItemTitle(itemVnum, metinSlot, attrSlot)

		#if self.__IsHair(itemVnum):
		#	self.__AppendHairIcon(itemVnum)

		### Description ###
		self.AppendDescription(itemDesc, 26)
		self.AppendDescription(itemSummary, 26, self.CONDITION_COLOR)

	if app.__ENABLE_NEW_OFFLINESHOP__:
		def AddRightClickForSale(self):
			self.AppendTextLine(localeinfo.OFFLINESHOP_TOOLTIP_RIGHT_CLICK_FOR_SALE, uiofflineshop.COLOR_TEXT_SHORTCUT)
	
	if app.NEW_PET_SYSTEM:
		def check_sigillo(self, item_vnum):
			for x in range(55701,55711+1):
				if x == item_vnum:
					return True	
			if item_vnum == 55801:
				return True
			return False

	if app.ENABLE_SOUL_SYSTEM:
		def __SoulItemToolTip(self, itemVnum, metinSlot, flags):
			self.itemVnum = itemVnum
			item.SelectItem(itemVnum)
			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()
			
			(limit_Type, limit_Value) = item.GetLimit(1)
			max_time = limit_Value
			
			if 0 != flags:
				metinSlot[2] = item.GetValue(2)
				
			data = metinSlot[2]
			keep_time = data / 10000
			remain_count = data % 10000
			
			value_index = 2 + keep_time / 60
			if value_index < 3:
				value_index = 3
			if value_index > 5:
				value_index = 5
			damage_value = float( item.GetValue(value_index) / 10.0 )
		
			self.ClearToolTip()
			
			soul_desc = ""
			if item.RED_SOUL == itemSubType:
				soul_desc = localeinfo.SOUL_ITEM_TOOLTIP_RED1 % (keep_time, max_time)
				soul_desc += localeinfo.SOUL_ITEM_TOOLTIP_RED2 % (damage_value)
			elif item.BLUE_SOUL == itemSubType:
				soul_desc = localeinfo.SOUL_ITEM_TOOLTIP_BLUE1 % (keep_time, max_time)
				soul_desc += localeinfo.SOUL_ITEM_TOOLTIP_BLUE2 % (damage_value)
			
			self.__SetNormalItemTitle()
			
			if keep_time < 10:
				desc_color = 0xfff15f5f	# RGB(241,95,95)
			else:
				desc_color = 0xff86e57f	# RGB(134,229,127)
				
			self.AppendDescription( soul_desc, 26, desc_color)
	
			self.AppendDescription(localeinfo.SOUL_ITEM_TOOLTIP_COMMON, 26, 0xfff15f5f)	# RGB(241,95,95)
			self.AppendSpace(10)

			if metinSlot[1] and keep_time >= 10 and remain_count > 0:
				self.AppendTextLine( localeinfo.SOUL_ITEM_TOOLTIP_APPLY, 0xff86e57f )		# RGB(134,229,127)
			else:
				self.AppendTextLine( localeinfo.SOUL_ITEM_TOOLTIP_UNAPPLIED, 0xfff15f5f )	# RGB(241,95,95)
				
			self.AppendSpace(5)
			
			self.AppendTextLine( localeinfo.SOUL_ITEM_TOOLTIP_REMAIN_COUNT % remain_count )
			self.AppendSpace(5)
			
			if 0 == flags:
				for i in xrange(item.LIMIT_MAX_NUM):
					(limitType, limitValue) = item.GetLimit(i)
					if item.LIMIT_REAL_TIME == limitType:
						self.AppendSoulItemLastTime(metinSlot[0])
			else:
				(limitType, limitValue) = item.GetLimit(0)
				self.AppendSoulItemLastTime(limitValue + app.GetGlobalTimeStamp())
					
			self.ShowToolTip()
			
		def AppendSoulItemLastTime(self, endTime):
			leftSec = max(0, endTime - app.GetGlobalTimeStamp())
			if leftSec > 0:
				self.AppendSpace(5)
				self.AppendTextLine(localeinfo.SOUL_ITEM_TOOLTIP_REMOVE_TIME + " : " + localeinfo.SecondToDHM(leftSec), self.NORMAL_COLOR)
	

	def AppendAntiflagInformation(self):
		antiFlagDict = {
			localeinfo.TOOLTIP_ANTIDROP: item.IsAntiFlag(item.ITEM_ANTIFLAG_DROP),
			localeinfo.TOOLTIP_ANTISELL: item.IsAntiFlag(item.ITEM_ANTIFLAG_SELL),
			localeinfo.TOOLTIP_ANTISHOP: item.IsAntiFlag(item.ITEM_ANTIFLAG_MYSHOP),
			localeinfo.TOOLTIP_ANTISAFEBOX: item.IsAntiFlag(item.ITEM_ANTIFLAG_SAFEBOX),
		}
		
		antiFlagNames = [name for name, flag in antiFlagDict.iteritems() if flag]
		if antiFlagNames:
			self.AppendSpace(5)
			textLine = self.AppendTextLine('{} {}'.format(', '.join(antiFlagNames), ""), self.NORMAL_COLOR)
			textLine.SetFeather()

	def AddItemData(self, itemVnum, metinSlot, attrSlot = 0, flags = 0, unbindTime = 0, window_type = player.INVENTORY, slotIndex = -1, lockedattr = -1, gaya = 0, wiki = 0, shop = False, transmutation = -1):
		#preview = 1
		self.itemVnum = itemVnum
		item.SelectItem(itemVnum)
		itemType = item.GetItemType()
		itemSubType = item.GetItemSubType()
		
		if itemType == item.ITEM_TYPE_ISHOP:
			desc = item.GetItemDescription()
			self.AppendTextLine(item.GetItemName(), self.TITLE_COLOR)
			if not metinSlot[0]:
				self.AppendDescription(desc, 26)
				self.ShowToolTip()
				return
			
			self.itemVnum = metinSlot[0]
			item.SelectItem(self.itemVnum)
			name = item.GetItemName()
			self.AppendDescription(desc, 26)
			self.AppendTextLine(name, self.CAN_LEVEL_UP_COLOR)
			self.AppendAntiflagInformation()
			self.ShowToolTip()
			return
		
		if itemType == item.ITEM_TYPE_USE and itemSubType == item.USE_FISH:
			self.AppendTextLine(item.GetItemName(), self.NORMAL_COLOR)
			self.AppendDescription(item.GetItemDescription(), 26)
			self.AppendSpace(5)
			self.__AppendAffectInformation()
			self.AppendSpace(5)
			duration = item.GetValue(0) / 60
			self.AppendTextLine(localeinfo.TOOLTIP_FISH_USE_TIME % (duration))
			self.AppendAntiflagInformation()
			self.ShowToolTip()
			return
		
		if app.NEW_PET_SYSTEM and itemVnum == 55002:
			self.AppendTextLine(item.GetItemName(), self.NORMAL_COLOR)
			self.AppendDescription(item.GetItemDescription(), 26)
			if gaya == 1:
				petVnum = 0
			else:
				petVnum = metinSlot[0]
			
			if petVnum != 0:
				name = nonplayer.GetMonsterName(petVnum)
				if name != "":
					self.AppendSpace(5)
					self.AppendTextLine(name, self.SPECIAL_TITLE_COLOR)
				
				self.AppendSpace(5)
				self.AppendTextLine(localeinfo.PETBOX_DESC1)
				time = app.GetGlobalTimeStamp() - metinSlot[2]
				minute = int((time / 60) % 60)
				hour = int((time / 60) / 60) % 24
				day = int(int((time / 60) / 60) / 24)
				self.AppendTextLine(localeinfo.PETBOX_DESC2 % (day, hour, minute))
				l = attrSlot[0][0]
				self.AppendTextLine(localeinfo.PETBOX_DESC3 % (l))
				e = attrSlot[0][1]
				evo = [localeinfo.PET_EVOLUTION1, localeinfo.PET_EVOLUTION2, localeinfo.PET_EVOLUTION3, localeinfo.PET_EVOLUTION4]
				self.AppendTextLine(localeinfo.PETBOX_DESC4 % (evo[e]))
				b1 = attrSlot[1][0]
				self.AppendTextLine(localeinfo.PETBOX_DESC5 % (pointop(str(b1))), self.SPECIAL_POSITIVE_COLOR)
				b2 = attrSlot[1][1]
				self.AppendTextLine(localeinfo.PETBOX_DESC6 % (pointop(str(b2))), self.SPECIAL_POSITIVE_COLOR)
				b3 = attrSlot[2][0]
				self.AppendTextLine(localeinfo.PETBOX_DESC7 % (pointop(str(b3))), self.SPECIAL_POSITIVE_COLOR)
				
				skillname = [localeinfo.PETSKILLNAME_1, localeinfo.PETSKILLNAME_2, localeinfo.PETSKILLNAME_3, localeinfo.PETSKILLNAME_4, localeinfo.PETSKILLNAME_5, localeinfo.PETSKILLNAME_6, localeinfo.PETSKILLNAME_7, localeinfo.PETSKILLNAME_8, localeinfo.PETSKILLNAME_9, localeinfo.PETSKILLNAME_10, localeinfo.PETSKILLNAME_11, localeinfo.PETSKILLNAME_12]
				skill1 = attrSlot[2][1]
				skilllv1 = attrSlot[3][0]
				if skill1 == 255:
					self.AppendTextLine(localeinfo.PETBOX_DESC8A)
				elif skill1 == 0:
					self.AppendTextLine(localeinfo.PETBOX_DESC8B)
				else:
					self.AppendTextLine(localeinfo.PETBOX_DESC8C % (skillname[skill1 - 1], skilllv1))
				skill2 = attrSlot[3][1]
				skilllv2 = attrSlot[4][0]
				if skill2 == 255:
					self.AppendTextLine(localeinfo.PETBOX_DESC9A)
				elif skill2 == 0:
					self.AppendTextLine(localeinfo.PETBOX_DESC9B)
				else:
					self.AppendTextLine(localeinfo.PETBOX_DESC9C % (skillname[skill2 - 1], skilllv2))
				skill3 = attrSlot[4][1]
				skilllv3 = attrSlot[5][0]
				if skill3 == 255:
					self.AppendTextLine(localeinfo.PETBOX_DESC10A)
				elif skill3 == 0:
					self.AppendTextLine(localeinfo.PETBOX_DESC10B)
				else:
					self.AppendTextLine(localeinfo.PETBOX_DESC10C % (skillname[skill3 - 1], skilllv3))
				skill4 = attrSlot[5][1]
				skilllv4 = attrSlot[6][0]
				if skill4 == 255:
					self.AppendTextLine(localeinfo.PETBOX_DESC11A)
				elif skill4 == 0:
					self.AppendTextLine(localeinfo.PETBOX_DESC11B)
				else:
					self.AppendTextLine(localeinfo.PETBOX_DESC11C % (skillname[skill4 - 1], skilllv4))
				
				#self.__ModelPreview(itemVnum, 6, petVnum)
				#if not wiki:
				#	self.AppendTextLine(localeinfo.VIEW_RENDERING)
			
			self.AppendAntiflagInformation()
			self.ShowToolTip()
			return
		
		if app.ENABLE_NEW_USE_POTION:
			if itemType == item.ITEM_TYPE_USE and itemSubType == item.USE_NEW_POTIION:
				self.AppendTextLine(item.GetItemName(), self.NORMAL_COLOR)
				self.AppendDescription(item.GetItemDescription(), 26)
				if not (itemVnum >= 89108 and itemVnum <= 89125):
					self.__AppendAffectInformation()
					if itemVnum == 72013 or itemVnum == 72014 or itemVnum == 72015:
						self.AppendTextLine(localeinfo.TOOLTIP_NEW_POTION30, self.POSITIVE_COLOR)
					
					self.AppendSpace(5)
				
				self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
				self.AppendSpace(5)
				if self.isItemShop==1 or gaya == 1 or shop:
					for i in xrange(item.LIMIT_MAX_NUM):
						(limitType, limitValue) = item.GetLimit(i)
						if limitType == item.LIMIT_REAL_TIME and limitValue > 0:
							self.AppendTextLine(localeinfo.LEFT_TIME + " : " + localeinfo.SecondToDHM(limitValue), self.NORMAL_COLOR)
							break
				else:
					if metinSlot[1] == 1:
						self.AppendTextLine(localeinfo.NEW_POTION_STATUS_1, 0xFF45DB45)
					else:
						self.AppendTextLine(localeinfo.NEW_POTION_STATUS_0, 0xFFDB4545)
					
					leftTime = metinSlot[0]
					if leftTime >= 0:
						for i in xrange(item.LIMIT_MAX_NUM):
							(limitType, limitValue) = item.GetLimit(i)
							if limitType != 0:
								self.AppendTextLine(localeinfo.LEFT_TIME + " : " + localeinfo.SecondToDHM(leftTime), self.NORMAL_COLOR)
								break
					else:
						for i in xrange(item.LIMIT_MAX_NUM):
							(limitType, limitValue) = item.GetLimit(i)
							if item.LIMIT_REAL_TIME == limitType and limitValue > 0:
								self.AppendTextLine(localeinfo.LEFT_TIME + " : " + localeinfo.SecondToDHM(limitValue), self.NORMAL_COLOR)
				
				self.AppendAntiflagInformation()
				self.ShowToolTip()
				return
		
		if app.ENABLE_DS_POTION_DIFFRENT:
			if itemType == item.ITEM_TYPE_USE and itemSubType == item.USE_TIME_CHARGE_PER:
				maxPercent = item.GetValue(0)
				remainPercent = metinSlot[0]
				if remainPercent <= 0:
					remainPercent = maxPercent
				self.AppendTextLine(item.GetItemName(), self.NORMAL_COLOR)
				self.AppendDescription(item.GetItemDescription(), 26)
				self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
				self.AppendSpace(5)
				self.AppendTextLine(localeinfo.DS_TIME_CHARGE % (remainPercent, remainPercent * 864, maxPercent, maxPercent * 864), self.NORMAL_COLOR)
				self.AppendAntiflagInformation()
				self.ShowToolTip()
				return
		
		if app.ENABLE_ATTR_COSTUMES and itemType == item.ITEM_TYPE_USE and (itemSubType == item.USE_ADD_ATTR_COSTUME1 or itemSubType == item.USE_ADD_ATTR_COSTUME2):
			color = grp.GenerateColor(1.0, 0.7843, 0.0, 1.0)
			if itemSubType == item.USE_ADD_ATTR_COSTUME2:
				color = grp.GenerateColor(1.0, 0.0, 0.0, 1.0)
			
			self.AppendTextLine(item.GetItemName(), color)
			self.AppendDescription(item.GetItemDescription(), 26)
			self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
			self.AppendSpace(5)
			
			type = metinSlot[0]
			value = metinSlot[1]
			if type > 0:
				affectString = self.__GetAffectString(type, value)
				if affectString:
					self.AppendTextLine(affectString, self.SPECIAL_POSITIVE_COLOR)
			
			self.AppendAntiflagInformation()
			self.ShowToolTip()
			return
		
		if 50026 == itemVnum:
			if 0 != metinSlot:
				name = item.GetItemName()
				if metinSlot[0] > 0:
					name += " "
					name += localeinfo.NumberToMoneyString(metinSlot[0])
				self.SetTitle(name)
				self.__AppendSealInformation(window_type, slotIndex) ## cyh itemseal 2013 11 11
				self.AppendAntiflagInformation()
				self.ShowToolTip()
			return
			
		# if app.__ENABLE_NEW_EFFECT_CIANITE_WEAPON__:
			# if constinfo.IS_EPIC_WEAPON(itemVnum):
				# self.AppendTextLine(localeinfo.GRADE_CIANITE_EPIC, self.CIANITE_COLOR_WEAPON_EPIC)
			# if constinfo.IS_LEGG_WEAPON(itemVnum):
				# self.AppendTextLine(localeinfo.GRADE_CIANITE_LEGG, self.CIANITE_COLOR_WEAPON_LEGG)
			# if constinfo.IS_ANTIC_WEAPON(itemVnum):
				# self.AppendTextLine(localeinfo.GRADE_CIANITE_ANTIC, self.CIANITE_COLOR_WEAPON_ANTIC)
			# if constinfo.IS_MISTIC_WEAPON(itemVnum):
				# self.AppendTextLine(localeinfo.GRADE_CIANITE_MISTC, self.CIANITE_COLOR_WEAPON_MISTIC)

		### Skill Book ###
		if 50300 == itemVnum and not self.isBook:
			if 0 != metinSlot and not self.isBook:
				self.__SetSkillBookToolTip(metinSlot[0], localeinfo.TOOLTIP_SKILLBOOK_NAME, 1)
				self.AppendAntiflagInformation()
				self.ShowToolTip()
			elif self.isBook:
				self.SetTitle(item.GetItemName())
				self.AppendDescription(item.GetItemDescription(), 26)
				self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
				self.AppendAntiflagInformation()
				self.ShowToolTip()					
			return
		elif 70037 == itemVnum :
			if 0 != metinSlot and not self.isBook2:
				self.__SetSkillBookToolTip(metinSlot[0], localeinfo.TOOLTIP_SKILL_FORGET_BOOK_NAME, 0)
				self.AppendDescription(item.GetItemDescription(), 26)
				self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
				self.AppendAntiflagInformation()
				self.ShowToolTip()
			elif self.isBook2:
				self.SetTitle(item.GetItemName())
				self.AppendDescription(item.GetItemDescription(), 26)
				self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
				self.AppendAntiflagInformation()
				self.ShowToolTip()					
			return
		elif 70055 == itemVnum:
			if 0 != metinSlot:
				self.__SetSkillBookToolTip(metinSlot[0], localeinfo.TOOLTIP_SKILL_FORGET_BOOK_NAME, 0)
				self.AppendDescription(item.GetItemDescription(), 26)
				self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
				self.AppendAntiflagInformation()
				self.ShowToolTip()
			return
		elif 70037 == itemVnum:
			if 0 != metinSlot:
				self.__SetSkillBookToolTip(metinSlot[0], localeinfo.TOOLTIP_SKILL_FORGET_BOOK_NAME, 0)
				self.AppendDescription(item.GetItemDescription(), 26)
				self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
				self.__AppendSealInformation(window_type, slotIndex) ## cyh itemseal 2013 11 11
				self.AppendAntiflagInformation()
				self.ShowToolTip()
			return
		elif 70055 == itemVnum:
			if 0 != metinSlot:
				self.__SetSkillBookToolTip(metinSlot[0], localeinfo.TOOLTIP_SKILL_FORGET_BOOK_NAME, 0)
				self.AppendDescription(item.GetItemDescription(), 26)
				self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
				self.__AppendSealInformation(window_type, slotIndex) ## cyh itemseal 2013 11 11
				self.AppendAntiflagInformation()
				self.ShowToolTip()
			return

		if app.ENABLE_SOUL_SYSTEM:
			if item.ITEM_TYPE_SOUL == itemType:
				self.__SoulItemToolTip(itemVnum, metinSlot, flags)
				return
				

		###########################################################################################

		itemDesc = item.GetItemDescription()
		itemSummary = item.GetItemSummary()

		isCostumeItem = 0
		isCostumeHair = 0
		isCostumeBody = 0
		isCostumeStole = 0
		if app.ENABLE_STOLE_COSTUME:
			if item.ITEM_TYPE_COSTUME == itemType:
				isCostumeStole = item.COSTUME_TYPE_STOLE == itemSubType
		
		if app.ENABLE_AURA_SYSTEM:
			isCostumeAura = item.COSTUME_TYPE_AURA == itemSubType
		
		if app.ENABLE_COSTUME_EFFECT:
			isCostumeEffectBody = 0
			isCostumeEffectWeapon = 0
			if item.ITEM_TYPE_COSTUME == itemType:
				isCostumeEffectBody = item.COSTUME_EFFECT_BODY == itemSubType
				isCostumeEffectWeapon = item.COSTUME_EFFECT_WEAPON == itemSubType
		
		if app.ENABLE_ACCE_SYSTEM:
			isCostumeAcce = 0
		
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			isCostumeAcce = 0
		if app.ENABLE_WEAPON_COSTUME_SYSTEM:
			isCostumeWeapon = 0

		if app.ENABLE_COSTUME_SYSTEM:
			if item.ITEM_TYPE_COSTUME == itemType:
				isCostumeItem = 1
				isCostumeHair = item.COSTUME_TYPE_HAIR == itemSubType
				isCostumeBody = item.COSTUME_TYPE_BODY == itemSubType
				if app.ENABLE_ACCE_SYSTEM:
					isCostumeAcce = itemSubType == item.COSTUME_TYPE_ACCE
				
				if app.ENABLE_ACCE_COSTUME_SYSTEM:
					isCostumeAcce = item.COSTUME_TYPE_ACCE == itemSubType
				if app.ENABLE_WEAPON_COSTUME_SYSTEM:
					isCostumeWeapon = item.COSTUME_TYPE_WEAPON == itemSubType

				#dbg.TraceError("IS_COSTUME_ITEM! body(%d) hair(%d)" % (isCostumeBody, isCostumeHair))

		self.__SetItemTitle(itemVnum, metinSlot, attrSlot)

		#self.__ModelPreviewClose()

		### Hair Preview Image ###
		#if self.__IsHair(itemVnum):
		#	self.__AppendHairIcon(itemVnum)

		### Description ###
		self.AppendDescription(itemDesc, 26)
		self.AppendDescription(itemSummary, 26, self.CONDITION_COLOR)



		if app.ENABLE_FEATURES_REFINE_SYSTEM:
			if itemVnum in (player.REFINE_VNUM_POTION_LOW, player.REFINE_VNUM_POTION_MEDIUM, player.REFINE_VNUM_POTION_EXTRA):

				# self.DESCRIPTION_VNUMS = [
					# localeinfo.REFINE_TOOLTIP_ITEM_DESCRIPTION_1,
					# localeinfo.REFINE_TOOLTIP_ITEM_DESCRIPTION_2,
					# localeinfo.REFINE_TOOLTIP_ITEM_DESCRIPTION_3,
					# localeinfo.REFINE_TOOLTIP_ITEM_DESCRIPTION_4
				# ]
				
				self.PERCENTAGE_VNUMS = {
					player.REFINE_VNUM_POTION_LOW : player.REFINE_PERCENTAGE_LOW,
					player.REFINE_VNUM_POTION_MEDIUM : player.REFINE_PERCENTAGE_MEDIUM,
					player.REFINE_VNUM_POTION_EXTRA : player.REFINE_PERCENTAGE_EXTRA
				}
				
				self.COLORS = [
					self.NORMAL_COLOR, self.SPECIAL_POSITIVE_COLOR, self.DISABLE_COLOR, self.HIGH_PRICE_COLOR
				]
					
				self.AppendSpace(5)

				# for it in xrange(len(self.DESCRIPTION_VNUMS) - 1):
					# self.AppendDescription(self.DESCRIPTION_VNUMS[it], None, self.COLORS[it])
				# self.AppendDescription(self.DESCRIPTION_VNUMS[1] % (self.PERCENTAGE_VNUMS[itemVnum]), None, self.COLORS[3])

		if app.ENABLE_RARITY_SYSTEM:
			itemRarity = item.GetRarity()
			rarityDesc = self.RARITY_DESC[itemRarity]
			self.AppendTextLine(rarityDesc, self.RARITY_COLOR[itemRarity])
			self.AppendSpace(5)


		if app.NEW_PET_SYSTEM:
			if self.check_sigillo(itemVnum) and not wiki:
				if attrSlot[0][1] != 0:
					self.AppendSpace(5)
					self.AppendTextLine(localeinfo.PET_TOOLTIP_LV % (attrSlot[3][1]), self.NORMAL_COLOR)
					self.AppendTextLine(localeinfo.PET_TOOLTIP_BNS1 % (pointop(str(attrSlot[0][1]))), self.SPECIAL_POSITIVE_COLOR)
					self.AppendTextLine(localeinfo.PET_TOOLTIP_BNS2 % (pointop(str(attrSlot[1][1]))), self.SPECIAL_POSITIVE_COLOR)
					self.AppendTextLine(localeinfo.PET_TOOLTIP_BNS3 % (pointop(str(attrSlot[2][1]))), self.SPECIAL_POSITIVE_COLOR)
					self.AppendSpace(5)
					if itemVnum != 55002:
						days = (int(metinSlot[1])/60)/24
						hours = (int(metinSlot[1]) - (days*60*24)) / 60
						mins = int(metinSlot[1]) - (days*60*24) - (hours*60)
						if app.ENABLE_NEW_PET_EDITS:
							if int(metinSlot[1]) > 525600:
								self.AppendTextLine(localeinfo.PET_TIME_TOOLTIP_INF)
							else:
								self.AppendTextLine(localeinfo.PET_TIME_TOOLTIP % (days, hours, mins), self.NORMAL_COLOR)
						else:
							self.AppendTextLine(localeinfo.PET_TIME_TOOLTIP % (days, hours, mins), self.NORMAL_COLOR)

		### Weapon ###
		if item.ITEM_TYPE_WEAPON == itemType:
			
			self.__AppendLimitInformation()
			self.AppendSpace(5)
		
			if item.WEAPON_FAN == itemSubType:
				self.__AppendMagicAttackInfo()
				self.__AppendAttackPowerInfo()
		
			else:
				self.__AppendAttackPowerInfo()
				self.__AppendMagicAttackInfo()
		
			self.__AppendAffectInformation()
			if app.ATTR_LOCK:
				#lockedattr = player.GetItemAttrLocked(window_type, slotIndex)
				self.__AppendAttributeInformation(attrSlot, 0, lockedattr)
			else:
				self.__AppendAttributeInformation(attrSlot)
			
			if app.ENABLE_CHANGELOOK:
				self.AppendChangeLookInformation(window_type, slotIndex, transmutation)
			
			self.AppendWearableInformation(itemVnum, transmutation, window_type, slotIndex)
			self.__AppendMetinSlotInfo(metinSlot)
			
			#if preview != 0:
			#	race = 0
			#	if itemSubType == item.WEAPON_SWORD:
			#		if player.GetRace() == 6 or player.GetRace() == 2:
			#			race = player.GetRace()
			#		if player.GetRace() != 3 and player.GetRace() != 7 and player.GetRace() != 8:
			#			race = player.GetRace()
			#			if item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR) and item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN) and race != 6 and race != 2:
			#				if chr.RaceToSex(player.GetRace()) == 1:
			#					race = 2
			#				else:
			#					race = 6
			#		else:
			#			if chr.RaceToSex(player.GetRace()) == 1:
			#				race = 0
			#			else:
			#				race = 4
			#			
			#			if item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR) and item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN) and race != 6 and race != 2:
			#				if chr.RaceToSex(player.GetRace()) == 1:
			#					race = 2
			#				else:
			#					race = 6
			#	elif itemSubType == item.WEAPON_TWO_HANDED:
			#		if player.GetRace() != 0 and player.GetRace() != 4:
			#			if chr.RaceToSex(player.GetRace()) == 1:
			#				race = 0
			#			else:
			#				race = 4
			#		else:
			#			race = player.GetRace()
			#	elif itemSubType == item.WEAPON_DAGGER or itemSubType == item.WEAPON_BOW:
			#		if player.GetRace() != 1 and player.GetRace() != 5:
			#			if chr.RaceToSex(player.GetRace()) == 1:
			#				race = 5
			#			else:
			#				race = 1
			#		else:
			#			race = player.GetRace()
			#	elif itemSubType == item.WEAPON_BELL or itemSubType == item.WEAPON_FAN:
			#		if player.GetRace() != 3 and player.GetRace() != 7:
			#			if chr.RaceToSex(player.GetRace()) == 1:
			#				race = 7
			#			else:
			#				race = 3
			#		else:
			#			race = player.GetRace()
			#	
			#	if itemSubType != 6:
			#		self.__ModelPreview(itemVnum, 3, race)
		## Armor ###
		elif item.ITEM_TYPE_ARMOR == itemType:
			self.__AppendLimitInformation()

			defGrade = item.GetValue(1)
			defBonus = item.GetValue(5)*2
			if defGrade > 0:
				self.AppendSpace(5)
				self.AppendTextLine(localeinfo.TOOLTIP_ITEM_DEF_GRADE % (defGrade+defBonus), self.GetChangeTextLineColor(defGrade))

			self.__AppendMagicDefenceInfo()
			self.__AppendAffectInformation()
			if app.ATTR_LOCK:
				#lockedattr = player.GetItemAttrLocked(window_type, slotIndex)
				self.__AppendAttributeInformation(attrSlot, 0, lockedattr)
			else:
				self.__AppendAttributeInformation(attrSlot)
			
			#if preview != 0 and itemSubType == 0:
			#	self.__ModelPreview(itemVnum, 2, self.__ItemGetRace(1))
			
			if app.ENABLE_CHANGELOOK:
				self.AppendChangeLookInformation(window_type, slotIndex, transmutation)
			
			self.AppendWearableInformation(itemVnum, transmutation, window_type, slotIndex)

			if itemSubType in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
				self.__AppendAccessoryMetinSlotInfo(metinSlot, constinfo.GET_ACCESSORY_MATERIAL_VNUM(itemVnum, itemSubType))
			else:
				self.__AppendMetinSlotInfo(metinSlot)

		### Ring Slot Item (Not UNIQUE) ###
		elif item.ITEM_TYPE_RING == itemType:
			self.__AppendLimitInformation()
			self.__AppendAffectInformation()
			if  app.ATTR_LOCK:
				#lockedattr = player.GetItemAttrLocked(window_type, slotIndex)
				self.__AppendAttributeInformation(attrSlot, 0, lockedattr)
			else:
				self.__AppendAttributeInformation(attrSlot)

			#self.__AppendAccessoryMetinSlotInfo(metinSlot, 99001)


		### Belt Item ###
		elif item.ITEM_TYPE_BELT == itemType:
			self.__AppendLimitInformation()
			self.__AppendAffectInformation()
			if  app.ATTR_LOCK:
				#lockedattr = player.GetItemAttrLocked(window_type, slotIndex)
				self.__AppendAttributeInformation(attrSlot, 0, lockedattr)
			else:
				self.__AppendAttributeInformation(attrSlot)

			self.__AppendAccessoryMetinSlotInfo(metinSlot, constinfo.GET_BELT_MATERIAL_VNUM(itemVnum))
		elif itemVnum in PetVnum:
			if constinfo.IS_PET_SEAL_OLD(itemVnum):
				self.__AppendAffectInformation()
			
			for i in xrange(item.LIMIT_MAX_NUM):
				(limitType, limitValue) = item.GetLimit(i)
				if item.LIMIT_REAL_TIME == limitType:
					self.AppendMallItemLastTime(metinSlot[0])
			
			#self.__ModelPreview(itemVnum, 6, PetVnum[itemVnum])
		elif 0 != isCostumeItem:
			self.__AppendLimitInformation()
			if app.ENABLE_AURA_SYSTEM and isCostumeAura:
				self.__AppendAffectInformationAura(window_type, slotIndex, metinSlot)
				self.__AppendAuraItemAffectInformation(itemVnum, window_type, slotIndex, metinSlot)
				self.__AppendAttributeInformationAura(window_type, slotIndex, attrSlot, metinSlot)
				self.__AppendAuraBoostMetinSlotInfo(itemVnum, window_type, slotIndex, metinSlot)
			else:
				if app.ENABLE_ACCE_SYSTEM and isCostumeAcce:
					## ABSORPTION RATE
					absChance = int(metinSlot[acce.ABSORPTION_SOCKET])
					if wiki:
						grade = item.GetValue(0)
						if grade == 1:
							absChance = 1
						elif grade == 2:
							absChance = 5
						elif grade == 3:
							absChance = 10
						
						if grade == 4:
							self.AppendTextLine(localeinfo.SASH_ABSORB_CHANCE2 % (11, 25), self.CONDITION_COLOR)
						else:
							self.AppendTextLine(localeinfo.SASH_ABSORB_CHANCE % (absChance), self.CONDITION_COLOR)
					else:
						self.AppendTextLine(localeinfo.SASH_ABSORB_CHANCE % (absChance), self.CONDITION_COLOR)
					## END ABSOPRTION RATE
					
					itemAbsorbedVnum = int(metinSlot[acce.ABSORBED_SOCKET])
					if itemAbsorbedVnum:
						## ATTACK / DEFENCE
						item.SelectItem(itemAbsorbedVnum)
						if item.GetItemType() == item.ITEM_TYPE_WEAPON:
							if item.GetItemSubType() == item.WEAPON_FAN:
								self.__AppendMagicAttackInfo(metinSlot[acce.ABSORPTION_SOCKET])
								item.SelectItem(itemAbsorbedVnum)
								self.__AppendAttackPowerInfo(metinSlot[acce.ABSORPTION_SOCKET])
							else:
								self.__AppendAttackPowerInfo(metinSlot[acce.ABSORPTION_SOCKET])
								item.SelectItem(itemAbsorbedVnum)
								self.__AppendMagicAttackInfo(metinSlot[acce.ABSORPTION_SOCKET])
						elif item.GetItemType() == item.ITEM_TYPE_ARMOR:
							defGrade = item.GetValue(1)
							defBonus = item.GetValue(5) * 2
							defGrade = self.CalcAcceValue(defGrade, metinSlot[acce.ABSORPTION_SOCKET])
							defBonus = self.CalcAcceValue(defBonus, metinSlot[acce.ABSORPTION_SOCKET])
							
							if defGrade > 0:
								self.AppendSpace(5)
								self.AppendTextLine(localeinfo.TOOLTIP_ITEM_DEF_GRADE % (defGrade + defBonus), self.GetChangeTextLineColor(defGrade))
							
							item.SelectItem(itemAbsorbedVnum)
							self.__AppendMagicDefenceInfo(metinSlot[acce.ABSORPTION_SOCKET])
						## END ATTACK / DEFENCE
						
						## EFFECT
						item.SelectItem(itemAbsorbedVnum)
						for i in xrange(item.ITEM_APPLY_MAX_NUM):
							(affectType, affectValue) = item.GetAffect(i)
							affectValue = self.CalcAcceValue(affectValue, metinSlot[acce.ABSORPTION_SOCKET])
							affectString = self.__GetAffectString(affectType, affectValue)
							if affectString and affectValue > 0:
								self.AppendTextLine(affectString, self.GetChangeTextLineColor(affectValue))
							
							item.SelectItem(itemAbsorbedVnum)
						# END EFFECT
						
						item.SelectItem(itemVnum)
						## ATTR
						if app.ATTR_LOCK:
							self.__AppendAttributeInformation(attrSlot, metinSlot[acce.ABSORPTION_SOCKET], lockedattr)
						else:
							self.__AppendAttributeInformation(attrSlot, metinSlot[acce.ABSORPTION_SOCKET])
						
						# END ATTR
					else:
						# ATTR
						if app.ATTR_LOCK:
							self.__AppendAttributeInformation(attrSlot, 0, lockedattr)
						else:
							self.__AppendAttributeInformation(attrSlot)
						# END ATTR
				else:
					self.__AppendAffectInformation()
					if isCostumeStole:
						self.AppendTextLine(localeinfo.GRADE_STOLA % (item.GetValue(0)), self.CONDITION_COLOR)
						self.AppendSpace(5)
					
					if app.ATTR_LOCK:
						self.__AppendAttributeInformation(attrSlot, 0, lockedattr, False, isCostumeStole)
					else:
						self.__AppendAttributeInformation(attrSlot, 0, 0, False, isCostumeStole)
			
			if app.ENABLE_CHANGELOOK:
				self.AppendChangeLookInformation(window_type, slotIndex, transmutation)
			
			self.AppendWearableInformation(itemVnum, transmutation, window_type, slotIndex)

			bHasRealtimeFlag = 0
			remainTime = 0
			for i in xrange(item.LIMIT_MAX_NUM):
				(limitType, limitValue) = item.GetLimit(i)
				if item.LIMIT_REAL_TIME == limitType:
					bHasRealtimeFlag = 1
					remainTime = limitValue
			
			if 1 == bHasRealtimeFlag:
				realTime = metinSlot[0]
				if realTime == 0:
					self.AppendMallItemRealLastTime(remainTime)
				else:
					self.AppendMallItemLastTime(realTime)
		## Rod ##
		elif item.ITEM_TYPE_ROD == itemType:

			if 0 != metinSlot:
				curLevel = item.GetValue(0) / 10
				curEXP = metinSlot[0]
				maxEXP = item.GetValue(2)
				self.__AppendLimitInformation()
				self.__AppendRodInformation(itemVnum, curLevel, curEXP, maxEXP)

		## Pick ##
		elif item.ITEM_TYPE_PICK == itemType:

			if 0 != metinSlot:
				curLevel = item.GetValue(0) / 10
				curEXP = metinSlot[0]
				maxEXP = item.GetValue(2)
				self.__AppendLimitInformation()
				self.__AppendPickInformation(curLevel, curEXP, maxEXP)

		## Lottery ##
		elif item.ITEM_TYPE_LOTTERY == itemType:
			if 0 != metinSlot:

				ticketNumber = int(metinSlot[0])
				stepNumber = int(metinSlot[1])

				self.AppendSpace(5)
				self.AppendTextLine(localeinfo.TOOLTIP_LOTTERY_STEP_NUMBER % (stepNumber), self.NORMAL_COLOR)
				self.AppendTextLine(localeinfo.TOOLTIP_LOTTO_NUMBER % (ticketNumber), self.NORMAL_COLOR);

		### Metin ###
		elif item.ITEM_TYPE_METIN == itemType:
			self.AppendMetinInformation()
			self.AppendMetinWearInformation()

		### Fish ###
		elif item.ITEM_TYPE_FISH == itemType:
			if 0 != metinSlot:
				self.__AppendFishInfo(metinSlot[0])
		elif itemType == item.ITEM_GACHA:
			if 0 != metinSlot:
				if self.isShopItem:
					restUsableCount = int(item.GetLimit(1)[1])
				else:
					restUsableCount = int(metinSlot[0])
				
				self.AppendSpace(5)
				self.AppendTextLine(localeinfo.TOOLTIP_REST_USABLE_COUNT % (restUsableCount), grp.GenerateColor(0.5, 1.0, 0.3, 1.0))
		## item.ITEM_TYPE_BLEND
		elif item.ITEM_TYPE_BLEND == itemType:
			self.__AppendLimitInformation()

			if metinSlot:
				affectType = metinSlot[0]
				affectValue = metinSlot[1]
				time = metinSlot[2]
				self.AppendSpace(5)
				affectText = self.__GetAffectString(affectType, affectValue)

				self.AppendTextLine(affectText, self.NORMAL_COLOR)

				if time > 0:
					minute = (time / 60)
					second = (time % 60)
					timeString = localeinfo.TOOLTIP_POTION_TIME

					if minute > 0:
						timeString += str(minute) + localeinfo.TOOLTIP_POTION_MIN
					if second > 0:
						timeString += " " + str(second) + localeinfo.TOOLTIP_POTION_SEC

					self.AppendTextLine(timeString)
				else:
					self.AppendTextLine(localeinfo.BLEND_POTION_NO_TIME)
			else:
				self.AppendTextLine("BLEND_POTION_NO_INFO")

		elif item.ITEM_TYPE_UNIQUE == itemType:
			if itemVnum == 30928 or itemVnum == 30929 or itemVnum == 30907 or itemVnum == 40907 or itemVnum == 30908 or itemVnum == 40908 or itemVnum == 71148 or itemVnum == 40004 or itemVnum == 40008 or itemVnum == 40009 or itemVnum == 40010 or itemVnum == 71136 or itemVnum == 30960 or itemVnum == 711361 or itemVnum == 78205 or itemVnum == 711362 or itemVnum == 71199 or itemVnum == 71135 or itemVnum == 71188 or itemVnum == 71234 or itemVnum == 71235 or itemVnum == 70058 or itemVnum == 70059 or itemVnum == 70060 or itemVnum == 71300 or itemVnum == 71301 or itemVnum == 71302 or itemVnum == 71303 or itemVnum == 71304 or itemVnum == 71305 or itemVnum == 71306 or itemVnum == 71307 or itemVnum == 71308 or itemVnum == 71309 or itemVnum == 71310 or itemVnum == 71311 or itemVnum == 71312 or itemVnum == 71313 or itemVnum == 71314 or itemVnum == 71315 or itemVnum == 71316 or itemVnum == 71317 or itemVnum == 71318 or itemVnum == 71319 or itemVnum == 71320:
				self.__AppendAffectInformation()
			
			if 0 != metinSlot:
				bHasRealtimeFlag = 0
				#bHasAnyOtherTime = 0
				remainTime = 0
				for i in xrange(item.LIMIT_MAX_NUM):
					(limitType, limitValue) = item.GetLimit(i)
					if item.LIMIT_REAL_TIME == limitType:
						bHasRealtimeFlag = 1
						remainTime = limitValue
					#elif item.LIMIT_REAL_TIME_START_FIRST_USE == limitType:
					#	bHasAnyOtherTime = 1
					#elif item.LIMIT_TIMER_BASED_ON_WEAR == limitType:
					#	bHasAnyOtherTime = 1
				
				if 1 == bHasRealtimeFlag:
					realTime = metinSlot[0]
					if realTime == 0:
						self.AppendMallItemRealLastTime(remainTime)
					else:
						self.AppendMallItemLastTime(realTime)
				#elif bHasAnyOtherTime == 1 or item.GetValue(0) > 0 or item.GetValue(1) > 0 or item.GetValue(2) > 0:
				elif 1 == item.GetValue(2):
					#time = metinSlot[player.METIN_SOCKET_MAX_NUM-1]
					time = metinSlot[player.METIN_SOCKET_MAX_NUM-2]
					self.AppendMallItemLastTime(time)
					#if 1 == item.GetValue(2):
					#	self.AppendMallItemLastTime(time)
					#else:
					#	self.AppendUniqueItemLastTime(time)

		### Use ###
		elif item.ITEM_TYPE_USE == itemType:
			self.__AppendLimitInformation()

			if item.USE_POTION == itemSubType or item.USE_POTION_NODELAY == itemSubType:
				self.__AppendPotionInformation()

			elif item.USE_ABILITY_UP == itemSubType:
				self.__AppendAbilityPotionInformation()


			if 27989 == itemVnum or 76006 == itemVnum:
				if 0 != metinSlot:
					useCount = int(metinSlot[0])

					self.AppendSpace(5)
					self.AppendTextLine(localeinfo.TOOLTIP_REST_USABLE_COUNT % (6 - useCount), self.NORMAL_COLOR)

			elif 50004 == itemVnum:
				if 0 != metinSlot:
					useCount = int(metinSlot[0])

					self.AppendSpace(5)
					self.AppendTextLine(localeinfo.TOOLTIP_REST_USABLE_COUNT % (10 - useCount), self.NORMAL_COLOR)

			elif constinfo.IS_AUTO_POTION(itemVnum):
				if 0 != metinSlot:
					isActivated = int(metinSlot[0])
					usedAmount = float(metinSlot[1])
					totalAmount = float(metinSlot[2])

					if 0 == totalAmount:
						totalAmount = 1

					self.AppendSpace(5)

					if 0 != isActivated:
						self.AppendTextLine("(%s)" % (localeinfo.TOOLTIP_AUTO_POTION_USING), self.SPECIAL_POSITIVE_COLOR)
						self.AppendSpace(5)
					
					if constinfo.IS_INFINITE_AUTO_POTION(itemVnum) == 0:
						self.AppendTextLine(localeinfo.TOOLTIP_AUTO_POTION_REST % (100.0 - ((usedAmount / totalAmount) * 100.0)), self.POSITIVE_COLOR)

			elif itemVnum in WARP_SCROLLS:
				if 0 != metinSlot:
					xPos = int(metinSlot[0])
					yPos = int(metinSlot[1])

					if xPos != 0 and yPos != 0:
						(mapName, xBase, yBase) = background.GlobalPositionToMapInfo(xPos, yPos)

						localeMapName=localeinfo.MINIMAP_ZONE_NAME_DICT.get(mapName, "")

						self.AppendSpace(5)

						if localeMapName!="":
							self.AppendTextLine(localeinfo.TOOLTIP_MEMORIZED_POSITION % (localeMapName, int(xPos-xBase)/100, int(yPos-yBase)/100), self.NORMAL_COLOR)
						else:
							self.AppendTextLine(localeinfo.TOOLTIP_MEMORIZED_POSITION_ERROR % (int(xPos)/100, int(yPos)/100), self.NORMAL_COLOR)
							dbg.TraceError("NOT_EXIST_IN_MINIMAP_ZONE_NAME_DICT: %s" % mapName)

			#####
			if item.USE_SPECIAL == itemSubType:
				bHasRealtimeFlag = 0
				remainTime = 0
				for i in xrange(item.LIMIT_MAX_NUM):
					(limitType, limitValue) = item.GetLimit(i)
					if item.LIMIT_REAL_TIME == limitType:
						bHasRealtimeFlag = 1
						remainTime = limitValue
				
				if 1 == bHasRealtimeFlag:
					realTime = metinSlot[0]
					if realTime == 0:
						self.AppendMallItemRealLastTime(remainTime)
					else:
						self.AppendMallItemLastTime(realTime)
				else:
					if 0 != metinSlot:
						time = metinSlot[player.METIN_SOCKET_MAX_NUM-1]
						if 1 == item.GetValue(2):
							self.AppendMallItemLastTime(time)

			elif item.USE_TIME_CHARGE_PER == itemSubType:
				bHasRealtimeFlag = 0
				remainTime = 0
				for i in xrange(item.LIMIT_MAX_NUM):
					(limitType, limitValue) = item.GetLimit(i)
					if item.LIMIT_REAL_TIME == limitType:
						bHasRealtimeFlag = 1
						remainTime = limitValue
				
				if 1 == bHasRealtimeFlag:
					realTime = metinSlot[0]
					if realTime == 0:
						self.AppendMallItemRealLastTime(remainTime)
					else:
						self.AppendMallItemLastTime(realTime)
			elif item.USE_TIME_CHARGE_FIX == itemSubType:
				bHasRealtimeFlag = 0
				remainTime = 0
				for i in xrange(item.LIMIT_MAX_NUM):
					(limitType, limitValue) = item.GetLimit(i)
					if item.LIMIT_REAL_TIME == limitType:
						bHasRealtimeFlag = 1
						remainTime = limitValue
				
				if 1 == bHasRealtimeFlag:
					realTime = metinSlot[0]
					if realTime == 0:
						self.AppendMallItemRealLastTime(remainTime)
					else:
						self.AppendMallItemLastTime(realTime)
		elif item.ITEM_TYPE_QUEST == itemType:
			if constinfo.IS_PET_SEAL_OLD(itemVnum):
				self.__AppendAffectInformation()
			
			for i in xrange(item.LIMIT_MAX_NUM):
				(limitType, limitValue) = item.GetLimit(i)

				if item.LIMIT_REAL_TIME == limitType:
					self.AppendMallItemLastTime(metinSlot[0])
			
		elif item.ITEM_TYPE_DS == itemType:
			self.AppendTextLine(self.__DragonSoulInfoString(itemVnum))
			self.__AppendAttributeInformation(attrSlot)
		else:
			self.__AppendLimitInformation()
		
		for i in xrange(item.LIMIT_MAX_NUM):
			(limitType, limitValue) = item.GetLimit(i)
			if item.LIMIT_REAL_TIME_START_FIRST_USE == limitType:
				self.AppendRealTimeStartFirstUseLastTime(item, metinSlot, i)
			elif item.LIMIT_TIMER_BASED_ON_WEAR == limitType:
				self.AppendTimerBasedOnWearLastTime(metinSlot)
		
		self.__AppendSealInformation(window_type, slotIndex)
		self.AppendAntiflagInformation()
		
		if chr.IsGameMaster(player.GetMainCharacterIndex()):
			self.AppendTextLine("Item Vnum: %i" % itemVnum, self.PRICE_COLOR)
		
		self.ShowToolTip()

	if app.ENABLE_AURA_SYSTEM:
		def SetAuraWindowItem(self, slotIdx):
			itemVnum = player.GetAuraItemID(slotIdx)
			if 0 == itemVnum:
				return
			
			self.ClearToolTip()
			if shop.IsOpen() and not shop.IsPrivateShop():
				self.AppendSellingPrice(item.GetISellItemPrice(item.SelectItem(itemVnum)))
			
			metinSlot = [player.GetAuraItemMetinSocket(slotIdx, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attrSlot = [player.GetAuraItemAttribute(slotIdx, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			self.AddItemData(itemVnum, metinSlot, attrSlot, window_type = player.AURA_REFINE, slotIndex = slotIdx)

		def __AppendAffectInformationAura(self, windowType, slotIndex, metinSlot):
			socketLevelValue = 0
			socketBoostValue = 0
			if windowType == player.INVENTORY:
				socketLevelValue = player.GetItemMetinSocket(windowType, slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
				if metinSlot[player.ITEM_SOCKET_AURA_CURRENT_LEVEL] != 0 and socketLevelValue == 0:
					socketLevelValue = metinSlot[player.ITEM_SOCKET_AURA_CURRENT_LEVEL]
				socketBoostValue = player.GetItemMetinSocket(windowType, slotIndex, player.ITEM_SOCKET_AURA_BOOST)
				if metinSlot[player.ITEM_SOCKET_AURA_BOOST] != 0 and socketBoostValue == 0:
					socketBoostValue = metinSlot[player.ITEM_SOCKET_AURA_BOOST]
			elif windowType == player.AURA_REFINE:
				socketLevelValue = player.GetAuraItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
				socketBoostValue = player.GetAuraItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_BOOST)
			elif windowType == player.SAFEBOX:
				socketLevelValue = safebox.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
				socketBoostValue = safebox.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_BOOST)
			
			if socketLevelValue == 0:
				return
			
			curLevel = (socketLevelValue / 100000) - 1000
			curStep = player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_STEP)
			if curStep < item.AURA_GRADE_RADIANT:
				curExpPer = 100.0 * (socketLevelValue % 100000) / player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP)
			
			self.AppendTextLine(localeinfo.AURA_LEVEL_STEP % (curLevel, curStep), self.CONDITION_COLOR)
			if curStep < item.AURA_GRADE_RADIANT:
				self.AppendTextLine(localeinfo.AURA_TOOLTIP_EXP % (curExpPer), self.CONDITION_COLOR)
			
			boostPercent = 0
			if socketBoostValue != 0:
				curBoostIndex = socketBoostValue / 100000000
				boostItemVnum = curBoostIndex + item.AURA_BOOST_ITEM_VNUM_BASE
				if boostItemVnum:
					item.SelectItem(boostItemVnum)
					boostPercent = item.GetValue(player.ITEM_VALUE_AURA_BOOST_PERCENT)
			
			if boostPercent > 0:
				self.AppendTextLine(localeinfo.AURA_DRAIN_BOOST_PER % (1.0 * curLevel / 10, boostPercent), self.CONDITION_COLOR)
			else:
				self.AppendTextLine(localeinfo.AURA_DRAIN_PER % (1.0 * curLevel / 10), self.CONDITION_COLOR)

		def __AppendAuraItemAffectInformation(self, oriItemVnum, windowType, slotIndex, metinSlot):
			socketLevelValue = 0
			socketBoostValue = 0
			
			if windowType == player.INVENTORY:
				socketLevelValue = player.GetItemMetinSocket(windowType, slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
				if metinSlot[player.ITEM_SOCKET_AURA_CURRENT_LEVEL] != 0 and socketLevelValue == 0:
					socketLevelValue = metinSlot[player.ITEM_SOCKET_AURA_CURRENT_LEVEL]
				
				socketBoostValue = player.GetItemMetinSocket(windowType, slotIndex, player.ITEM_SOCKET_AURA_BOOST)
				
				if metinSlot[player.ITEM_SOCKET_AURA_BOOST] != 0 and socketBoostValue == 0:
					socketBoostValue = metinSlot[player.ITEM_SOCKET_AURA_BOOST]
			elif windowType == player.AURA_REFINE:
				socketLevelValue = player.GetAuraItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
				socketBoostValue = player.GetAuraItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_BOOST)
			elif windowType == player.SAFEBOX:
				socketLevelValue = safebox.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
				socketBoostValue = safebox.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_BOOST)
			
			if socketLevelValue == 0:
				return
			
			curLevel = (socketLevelValue / 100000) - 1000
			boostPercent = 0.0
			if socketBoostValue != 0:
				curBoostIndex = socketBoostValue / 100000000
				boostItemVnum = curBoostIndex + item.AURA_BOOST_ITEM_VNUM_BASE
				if boostItemVnum:
					item.SelectItem(boostItemVnum)
					boostPercent = item.GetValue(player.ITEM_VALUE_AURA_BOOST_PERCENT) / 100.0
			
			#drainlate = curLevel / 10. / 100. + boostPercent
			drainlate = curLevel / 10.
			socketInDrainItemVnum = 0
			
			if windowType == player.INVENTORY:
				socketInDrainItemVnum = player.GetItemMetinSocket(windowType, slotIndex, player.ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM)
				if not metinSlot[player.ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM] == 0 and socketInDrainItemVnum == 0:
					socketInDrainItemVnum = metinSlot[player.ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM]
			elif windowType == player.AURA_REFINE:
				socketInDrainItemVnum = player.GetAuraItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM)
			elif windowType == player.SAFEBOX:
				socketInDrainItemVnum = safebox.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM)
			
			if socketInDrainItemVnum == 0:
				return
			
			item.SelectItem(socketInDrainItemVnum)
			itemType = item.GetItemType()
			if itemType == item.ITEM_TYPE_ARMOR:
				defBonus = item.GetValue(5)*2
				if item.GetValue(1) >= 1:
					#defGrade = max(((item.GetValue(1) + defBonus) * drainlate) , 1)
					defGrade = self.CalcAcceValue(item.GetValue(1) + defBonus, drainlate)
					if defGrade > 0:
						self.AppendSpace(5)
						self.AppendTextLineAbsorb(localeinfo.TOOLTIP_ITEM_DEF_GRADE % (defGrade), self.GetChangeTextLineColor(defGrade))
			
			for i in xrange(item.ITEM_APPLY_MAX_NUM):
				(affectType, affectValue) = item.GetAffect(i)
				if affectValue > 0:
					#affectValue = max((affectValue * drainlate), 1)
					affectValue = self.CalcAcceValue(affectValue, drainlate)
					affectString = self.__GetAffectString(affectType, affectValue)
					if affectString:
						self.AppendTextLineAbsorb(affectString, self.GetChangeTextLineColor(affectValue))
			
			item.SelectItem(oriItemVnum)

		def __AppendAttributeInformationAura(self, windowType, slotIndex, attrSlot, metinSlot):
			if 0 != attrSlot:
				socketLevelValue = 0
				socketBoostValue = 0
				if windowType == player.INVENTORY:
					socketLevelValue = player.GetItemMetinSocket(windowType, slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
					socketBoostValue = player.GetItemMetinSocket(windowType, slotIndex, player.ITEM_SOCKET_AURA_BOOST)
					if metinSlot[player.ITEM_SOCKET_AURA_CURRENT_LEVEL] != 0 and socketLevelValue == 0:
						socketLevelValue = metinSlot[player.ITEM_SOCKET_AURA_CURRENT_LEVEL]
					if metinSlot[player.ITEM_SOCKET_AURA_BOOST] != 0 and socketBoostValue == 0:
						socketBoostValue = metinSlot[player.ITEM_SOCKET_AURA_BOOST]
				elif windowType == player.AURA_REFINE:
					socketLevelValue = player.GetAuraItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
					socketBoostValue = player.GetAuraItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_BOOST)
				elif windowType == player.SAFEBOX:
					socketLevelValue = safebox.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
					socketBoostValue = safebox.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_BOOST)
				
				if socketLevelValue == 0:
					return
				
				curLevel = (socketLevelValue / 100000) - 1000
				boostPercent = 0.0
				if socketBoostValue != 0:
					curBoostIndex = socketBoostValue / 100000000
					boostItemVnum = curBoostIndex + item.AURA_BOOST_ITEM_VNUM_BASE
					if boostItemVnum:
						item.SelectItem(boostItemVnum)
						boostPercent = item.GetValue(player.ITEM_VALUE_AURA_BOOST_PERCENT) / 100.0
				
				#drainlate = curLevel / 10. / 100. + boostPercent
				drainlate = curLevel / 10.
				for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					type = attrSlot[i][0]
					value = attrSlot[i][1]
					if 0 >= value:
						continue
					
					#value = max(((value) * drainlate) , 1)
					value = self.CalcAcceValue(value, drainlate)
					affectString = self.__GetAffectString(type, value)
					if affectString:
						affectColor = self.__GetAttributeColor(i, value)
						self.AppendTextLine(affectString, affectColor)

		def __AppendAuraBoostMetinSlotInfo(self, oriItemVnum, windowType, slotIndex, metinSlot):
			socketBoostValue = 0
			if windowType == player.INVENTORY:
				socketBoostValue = player.GetItemMetinSocket(windowType, slotIndex, player.ITEM_SOCKET_AURA_BOOST)
				if metinSlot[player.ITEM_SOCKET_AURA_BOOST] != 0 and socketBoostValue == 0:
					socketBoostValue = metinSlot[player.ITEM_SOCKET_AURA_BOOST]
			elif windowType == player.AURA_REFINE:
				socketBoostValue = player.GetAuraItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_BOOST)
			elif windowType == player.SAFEBOX:
				socketBoostValue = safebox.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_BOOST)
			
			if socketBoostValue == 0:
				return
			
			curBoostIndex = socketBoostValue / 100000000
			socketImageDict = {
				player.METIN_SOCKET_TYPE_SILVER : "d:/ymir work/ui/game/windows/metin_slot_silver.sub",
				player.METIN_SOCKET_TYPE_GOLD : "d:/ymir work/ui/game/windows/metin_slot_gold.sub",
			}
			
			socketType = player.METIN_SOCKET_TYPE_NONE
			if item.ITEM_AURA_BOOST_ERASER < curBoostIndex < item.ITEM_AURA_BOOST_ULTIMATE:
				socketType = player.METIN_SOCKET_TYPE_SILVER
			elif curBoostIndex == item.ITEM_AURA_BOOST_ULTIMATE:
				socketType = player.METIN_SOCKET_TYPE_GOLD
			
			if player.METIN_SOCKET_TYPE_NONE == socketType:
				return
			
			boostRemainTime = socketBoostValue % 100000000
			boostItemVnum = curBoostIndex + item.AURA_BOOST_ITEM_VNUM_BASE
			
			self.AppendSpace(5)
			
			slotImage = ui.ImageBox()
			slotImage.SetParent(self)
			slotImage.LoadImage(socketImageDict[socketType])
			slotImage.Show()
			self.childrenList.append(slotImage)
			
			nameTextLine = ui.TextLine()
			nameTextLine.SetParent(self)
			nameTextLine.SetFontName(self.defFontName)
			nameTextLine.SetPackedFontColor(self.NORMAL_COLOR)
			nameTextLine.SetOutline()
			nameTextLine.SetFeather()
			nameTextLine.Show()
			self.childrenList.append(nameTextLine)
			
			if localeinfo.IsARABIC():
				slotImage.SetPosition(self.toolTipWidth - slotImage.GetWidth() - 9, self.toolTipHeight-1)
				nameTextLine.SetPosition(self.toolTipWidth - 50, self.toolTipHeight + 2)
			else:
				slotImage.SetPosition(9, self.toolTipHeight-1)
				nameTextLine.SetPosition(50, self.toolTipHeight + 2)
			
			boostItemImage = ui.ImageBox()
			boostItemImage.SetParent(self)
			boostItemImage.Show()
			self.childrenList.append(boostItemImage)
			if boostItemVnum:
				item.SelectItem(boostItemVnum)
				try:
					boostItemImage.LoadImage(item.GetIconImageFileName())
				except:
					dbg.TraceError("ItemToolTip.__AppendAuraBoostMetinSlotInfo() - Failed to find image file %d:%s" % (boostItemVnum, item.GetIconImageFileName()))
				
				nameTextLine.SetText(item.GetItemName())
				
				boostDrainTextLine = ui.TextLine()
				boostDrainTextLine.SetParent(self)
				boostDrainTextLine.SetFontName(self.defFontName)
				boostDrainTextLine.SetPackedFontColor(self.POSITIVE_COLOR)
				boostDrainTextLine.SetOutline()
				boostDrainTextLine.SetFeather()
				boostDrainTextLine.SetText(localeinfo.AURA_BOOST_DRAIN_PER % (item.GetValue(player.ITEM_VALUE_AURA_BOOST_PERCENT)))
				boostDrainTextLine.Show()
				self.childrenList.append(boostDrainTextLine)
				
				if localeinfo.IsARABIC():
					boostItemImage.SetPosition(self.toolTipWidth - boostItemImage.GetWidth() - 10, self.toolTipHeight)
					boostDrainTextLine.SetPosition(self.toolTipWidth - 50, self.toolTipHeight + 16 + 2)
				else:
					boostItemImage.SetPosition(10, self.toolTipHeight)
					boostDrainTextLine.SetPosition(50, self.toolTipHeight + 16 + 2)
				
				if 1 == item.GetValue(player.ITEM_VALUE_AURA_BOOST_UNLIMITED):
					boostRemainTime = 0
				
				if 0 != boostRemainTime:
					timeText = (localeinfo.LEFT_TIME + " : " + localeinfo.SecondToDHM(boostRemainTime))
					
					timeTextLine = ui.TextLine()
					timeTextLine.SetParent(self)
					timeTextLine.SetFontName(self.defFontName)
					timeTextLine.SetPackedFontColor(self.POSITIVE_COLOR)
					timeTextLine.SetPosition(50, self.toolTipHeight + 16 + 2 + 16 + 2)
					timeTextLine.SetOutline()
					timeTextLine.SetFeather()
					timeTextLine.Show()
					timeTextLine.SetText(timeText)
					self.childrenList.append(timeTextLine)
					self.toolTipHeight += 16 + 2
			
			self.toolTipHeight += 35
			self.ResizeToolTip()

		def AppendTextLineAbsorb(self, text, color = FONT_COLOR, centerAlign = True):
			return ToolTip.AppendTextLine(self, text, color, centerAlign)

	if app.ENABLE_BIOLOGIST_UI:
		def _BiologWindow__GetAffectString(self, type, value):
			return self.__GetAffectString(type, value)

		def _BiologWindow_Change__GetAffectString(self, type, value):
			return self.__GetAffectString(type, value)

	def __DragonSoulInfoString (self, dwVnum):
		step = (dwVnum / 100) % 10
		refine = (dwVnum / 10) % 10
		if 0 == step:
			return localeinfo.DRAGON_SOUL_STEP_LEVEL1 + " " + localeinfo.DRAGON_SOUL_STRENGTH(refine)
		elif 1 == step:
			return localeinfo.DRAGON_SOUL_STEP_LEVEL2 + " " + localeinfo.DRAGON_SOUL_STRENGTH(refine)
		elif 2 == step:
			return localeinfo.DRAGON_SOUL_STEP_LEVEL3 + " " + localeinfo.DRAGON_SOUL_STRENGTH(refine)
		elif 3 == step:
			return localeinfo.DRAGON_SOUL_STEP_LEVEL4 + " " + localeinfo.DRAGON_SOUL_STRENGTH(refine)
		elif 4 == step:
			return localeinfo.DRAGON_SOUL_STEP_LEVEL5 + " " + localeinfo.DRAGON_SOUL_STRENGTH(refine)
		else:
			return ""

	def __IsHair(self, itemVnum):
		if app.ENABLE_DS_GRADE_MYTH:
			return (self.__IsOldHair(itemVnum) or 
			self.__IsNewHair(itemVnum) or
			self.__IsNewHair2(itemVnum) or
			self.__IsNewHair3(itemVnum))
		else:
			return (self.__IsOldHair(itemVnum) or 
			self.__IsNewHair(itemVnum) or
			self.__IsNewHair2(itemVnum) or
			self.__IsNewHair3(itemVnum) or
			self.__IsCostumeHair(itemVnum)
			)

	if not app.ENABLE_DS_GRADE_MYTH:
		def __IsCostumeHair(self, itemVnum):
			return self.__IsNewHair3(itemVnum - 100000)

	if app.INGAME_WIKI:
		def AppendChestDropInfo(self, itemVnum):
			if not itemVnum:
				return
			
			item.SelectItem(itemVnum)
			if item.GetItemType() == item.ITEM_TYPE_GIFTBOX or item.GetItemType() == item.ITEM_GACHA:
				self.AppendSpace(5)
				self.AppendTextLine(localeinfo.CHEST_DROP_INFO, self.NORMAL_COLOR)

	def __IsOldHair(self, itemVnum):
		return itemVnum > 73000 and itemVnum < 74000

	def __IsNewHair(self, itemVnum):
		return itemVnum > 74000 and itemVnum < 75000

	def __IsNewHair2(self, itemVnum):
		return itemVnum > 75000 and itemVnum < 76000

	def __IsNewHair3(self, itemVnum):
		return ((74012 < itemVnum and itemVnum < 74022) or
			(74262 < itemVnum and itemVnum < 74272) or
			(74512 < itemVnum and itemVnum < 74522) or
			(74544 < itemVnum and itemVnum < 74560) or
			(74762 < itemVnum and itemVnum < 74772) or
			(45000 < itemVnum and itemVnum < 47000))

	def __IsCostumeHair(self, itemVnum):
		return app.ENABLE_COSTUME_SYSTEM and self.__IsNewHair3(itemVnum - 100000)

	def __AppendHairIcon(self, itemVnum):
		itemImage = ui.ImageBox()
		itemImage.SetParent(self)
		itemImage.Show()
		
		if self.__IsOldHair(itemVnum):
			itemImage.LoadImage("d:/ymir work/item/quest/"+str(itemVnum)+".tga")
		elif self.__IsNewHair3(itemVnum):
			itemImage.LoadImage("icon/hair/%d.sub" % (itemVnum))
		elif self.__IsNewHair(itemVnum):
			itemImage.LoadImage("d:/ymir work/item/quest/"+str(itemVnum-1000)+".tga")
		elif self.__IsNewHair2(itemVnum):
			itemImage.LoadImage("icon/hair/%d.sub" % (itemVnum))
		elif not app.ENABLE_DS_GRADE_MYTH and self.__IsCostumeHair(itemVnum):
			itemImage.LoadImage("icon/hair/%d.sub" % (itemVnum - 100000))
		
		itemImage.SetPosition(itemImage.GetWidth()/2, self.toolTipHeight)
		self.toolTipHeight += itemImage.GetHeight()
		#self.toolTipWidth += itemImage.GetWidth()/2
		self.childrenList.append(itemImage)
		self.ResizeToolTip()

	if app.ENABLE_OFFLINESHOP_REWORK:
		def AppendIcon(self, itemVnum, itemCount):
			itemImage = ui.ImageBox()
			itemImage.SetParent(self)
			itemImage.Show()
			
			item.SelectItem(itemVnum)
			itemImage.LoadImage(item.GetIconImageFileName())
			
			if itemCount > 1:
				countNumberLine = ui.NumberLine()
				countNumberLine.SetParent(itemImage)
				countNumberLine.SetWindowHorizontalAlignRight()
				countNumberLine.SetHorizontalAlignRight()
				countNumberLine.SetNumber(str(itemCount))
				countNumberLine.SetPosition(3, itemImage.GetHeight() - 8)
				countNumberLine.Show()
			
			itemImage.SetPosition((self.GetWidth() - itemImage.GetWidth()) / 2, self.toolTipHeight)
			self.toolTipHeight += itemImage.GetHeight()
			self.childrenList.append(itemImage)
			
			if itemCount > 1:
				self.childrenList.append(countNumberLine)
			
			self.ResizeToolTip()


	#def __ItemGetRace(self, preview = 0, sexFlags = 0):
	#	race = 0
	#
	#	if item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN):
	#		race = 9
	#	elif item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN):
	#		race = 1
	#	elif item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR) and item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN):
	#		race = 2
	#	elif item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR) and item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN) and item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA):
	#		race = 3
	#
	#	sex = chr.RaceToSex(player.GetRace())
	#	MALE = 1
	#	FEMALE = 0
	#	if preview:
	#		if race == 0:
	#			race = player.GetRace()
	#			if race == 0 or race == 4:
	#				if sex == MALE:
	#					race = 0
	#				else:
	#					race = 4
	#			elif race == 1 or race == 5:
	#				if sex == MALE:
	#					race = 5
	#				else:
	#					race = 1
	#			elif race == 2 or race == 6:
	#				if sex == MALE:
	#					race = 2
	#				else:
	#					race = 6
	#			elif race == 3 or race == 7:
	#				if sex == MALE:
	#					race = 7
	#				else:
	#					race = 3
	#		elif race == 9:
	#			if sex == MALE:
	#				race = 0
	#			else:
	#				race = 4
	#		elif race == 1 and sex == MALE:
	#			race = 5
	#		elif race == 2 and sex == FEMALE:
	#			race = 6
	#		elif race == 3 and sex == MALE:
	#			race = 7
	#		
	#		if sexFlags:
	#			if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE):
	#				if race == 0:
	#					race = 4
	#				elif race == 2:
	#					race = 6
	#				elif race == 5:
	#					race = 1
	#				elif race == 7:
	#					race = 3
	#			elif item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE):
	#				if race == 4:
	#					race = 0
	#				elif race == 6:
	#					race = 2
	#				elif race == 1:
	#					race = 5
	#				elif race == 3:
	#					race = 7
	#	else:
	#		if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE) and sex == MALE:
	#			race = player.GetRace() + 4
	#		
	#		if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE) and sex == FEMALE:
	#			race = player.GetRace()
	#		
	#		if race == 0:
	#			race = player.GetRace()
	#		
	#		if race == 9:
	#			race = 0
	#		
	#	return race

	def __SetSkillBookToolTip(self, skillIndex, bookName, skillGrade):
		skillName = skill.GetSkillName(skillIndex)

		if not skillName:
			return

		if localeinfo.IsVIETNAM():
			itemName = bookName + " " + skillName
		else:
			itemName = skillName + " " + bookName
		self.SetTitle(itemName)

	def __AppendPickInformation(self, curLevel, curEXP, maxEXP):
		self.AppendSpace(5)
		self.AppendTextLine(localeinfo.TOOLTIP_PICK_LEVEL % (curLevel), self.NORMAL_COLOR)
		self.AppendTextLine(localeinfo.TOOLTIP_PICK_EXP % (curEXP, maxEXP), self.NORMAL_COLOR)

		if curEXP == maxEXP:
			self.AppendSpace(5)
			self.AppendTextLine(localeinfo.TOOLTIP_PICK_UPGRADE1, self.NORMAL_COLOR)
			self.AppendTextLine(localeinfo.TOOLTIP_PICK_UPGRADE2, self.NORMAL_COLOR)
			self.AppendTextLine(localeinfo.TOOLTIP_PICK_UPGRADE3, self.NORMAL_COLOR)


	def __AppendRodInformation(self, itemVnum, curLevel, curEXP, maxEXP):
		self.AppendSpace(5)
		self.AppendTextLine(localeinfo.TOOLTIP_FISHINGROD_LEVEL % (curLevel), self.NORMAL_COLOR)
		if itemVnum >= 27400 and itemVnum <= 27490:
			self.AppendTextLine(localeinfo.TOOLTIP_ROD_FISHING % ((curLevel) * 2), self.NORMAL_COLOR)
		else:
			self.AppendTextLine(localeinfo.TOOLTIP_ROD_FISHING % (curLevel), self.NORMAL_COLOR)
		
		if maxEXP != 0 and itemVnum != 27390:
			self.AppendTextLine(localeinfo.TOOLTIP_FISHINGROD_EXP % (curEXP, maxEXP), self.NORMAL_COLOR)

		if curEXP == maxEXP:
			if itemVnum != 27390:
				self.AppendSpace(5)
			
			if itemVnum == 27490:
				self.AppendTextLine(localeinfo.TOOLTIP_FISHINGROD_UPGRADE4, self.NORMAL_COLOR)
				self.AppendTextLine(localeinfo.TOOLTIP_FISHINGROD_UPGRADE5, self.NORMAL_COLOR)
				self.AppendTextLine(localeinfo.TOOLTIP_FISHINGROD_UPGRADE6, self.NORMAL_COLOR)
			elif itemVnum == 27390:
				self.AppendTextLine(localeinfo.TOOLTIP_FISHINGROD_UPGRADE7, self.NORMAL_COLOR)
			else:
				self.AppendTextLine(localeinfo.TOOLTIP_FISHINGROD_UPGRADE1, self.NORMAL_COLOR)
				self.AppendTextLine(localeinfo.TOOLTIP_FISHINGROD_UPGRADE2, self.NORMAL_COLOR)
				self.AppendTextLine(localeinfo.TOOLTIP_FISHINGROD_UPGRADE3, self.NORMAL_COLOR)

	def __AppendLimitInformation(self):

		appendSpace = False

		for i in xrange(item.LIMIT_MAX_NUM):

			(limitType, limitValue) = item.GetLimit(i)

			if limitValue > 0:
				if False == appendSpace:
					self.AppendSpace(5)
					appendSpace = True

			else:
				continue

			if item.LIMIT_LEVEL == limitType:
				color = self.GetLimitTextLineColor(player.GetStatus(player.LEVEL), limitValue)
				self.AppendTextLine(localeinfo.TOOLTIP_ITEM_LIMIT_LEVEL % (limitValue), color)
			"""
			elif item.LIMIT_STR == limitType:
				color = self.GetLimitTextLineColor(player.GetStatus(player.ST), limitValue)
				self.AppendTextLine(localeinfo.TOOLTIP_ITEM_LIMIT_STR % (limitValue), color)
			elif item.LIMIT_DEX == limitType:
				color = self.GetLimitTextLineColor(player.GetStatus(player.DX), limitValue)
				self.AppendTextLine(localeinfo.TOOLTIP_ITEM_LIMIT_DEX % (limitValue), color)
			elif item.LIMIT_INT == limitType:
				color = self.GetLimitTextLineColor(player.GetStatus(player.IQ), limitValue)
				self.AppendTextLine(localeinfo.TOOLTIP_ITEM_LIMIT_INT % (limitValue), color)
			elif item.LIMIT_CON == limitType:
				color = self.GetLimitTextLineColor(player.GetStatus(player.HT), limitValue)
				self.AppendTextLine(localeinfo.TOOLTIP_ITEM_LIMIT_CON % (limitValue), color)
			"""

	## cyh itemseal 2013 11 11
	def __AppendSealInformation(self, window_type, slotIndex):
		if not app.ENABLE_SOULBIND_SYSTEM:
			return

		itemSealDate = player.GetItemSealDate(window_type, slotIndex)
		if itemSealDate == item.GetDefaultSealDate():
			return

		if itemSealDate == item.GetUnlimitedSealDate():
			self.AppendSpace(5)
			self.AppendTextLine(localeinfo.TOOLTIP_SEALED, self.NEGATIVE_COLOR)

		elif itemSealDate > 0:
			self.AppendSpace(5)
			hours, minutes = player.GetItemUnSealLeftTime(window_type, slotIndex)
			self.AppendTextLine(localeinfo.TOOLTIP_UNSEAL_LEFT_TIME % (hours, minutes), self.NEGATIVE_COLOR)

	def _PetSystemMain__GetAffectString(self, affectType, affectValue):
		if 0 == affectType:
			return None

		if 0 == affectValue:
			return None

		try:
			return self.AFFECT_DICT[affectType](affectValue)
		except TypeError:
			return "UNKNOWN_VALUE[%s] %s" % (affectType, affectValue)
		except KeyError:
			return "UNKNOWN_TYPE[%s] %s" % (affectType, affectValue)

	def _DungeonInfo__GetAffectString(self, affectType, affectValue):
		if 0 == affectType:
			return None

		if 0 == affectValue:
			return None

		try:
			return self.AFFECT_DICT[affectType](affectValue)
		except TypeError:
			return "UNKNOWN_VALUE[%s] %s" % (affectType, affectValue)
		except KeyError:
			return "UNKNOWN_TYPE[%s] %s" % (affectType, affectValue)

	def __GetAffectString(self, affectType, affectValue):
		if 0 == affectType:
			return None

		if 0 == affectValue:
			return None

		try:
			return self.AFFECT_DICT[affectType](affectValue)
		except TypeError:
			return "UNKNOWN_VALUE[%s] %s" % (affectType, affectValue)
		except KeyError:
			return "UNKNOWN_TYPE[%s] %s" % (affectType, affectValue)

	def __AppendAffectInformation(self):
		for i in xrange(item.ITEM_APPLY_MAX_NUM):
			(affectType, affectValue) = item.GetAffect(i)
			if app.USE_ANTI_EXP and affectType == item.APPLY_ANTI_EXP:
				continue
			
			if app.USE_MOUNT_COSTUME_SYSTEM and affectType == item.APPLY_MOUNT:
				continue
			
			if app.ENABLE_PETSKIN and affectType == item.APPLY_PETSKIN:
				continue
			
			if app.ENABLE_MOUNTSKIN and affectType == item.APPLY_MOUNTSKIN:
				continue
			
			affectString = self.__GetAffectString(affectType, affectValue)
			if affectString:
				self.AppendTextLine(affectString, self.GetChangeTextLineColor(affectValue))
		
		if app.ENABLE_NEW_EXTRA_BONUS:
			for i in xrange(item.NEW_EXTRA_BONUS_COUNT):
				(pointType, pointCount) = item.GetExtraBonus(i)
				if pointType != item.APPLY_NONE:
					affectString = self.__GetAffectString(pointType, pointCount)
					self.AppendTextLine(affectString, self.NEW_EXTRA_BONUS_COLOR)

	def AppendWearableInformation(self, itemVnum = 0, transmutation = -1, window = 0, slot = 0):
		self.AppendSpace(5)
		self.AppendTextLine(localeinfo.TOOLTIP_ITEM_WEARABLE_JOB, self.NORMAL_COLOR)
		
		flagList = (
			not item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR),
			not item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN),
			not item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA),
			not item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN))

		characterNames = ""
		for i in xrange(self.CHARACTER_COUNT):
			name = self.CHARACTER_NAMES[i]
			flag = flagList[i]
			if flag:
				characterNames += " "
				characterNames += name
		
		textLine = self.AppendTextLine(characterNames, self.NORMAL_COLOR, True)
		textLine.SetFeather()
		
		if app.ENABLE_CHANGELOOK:
			bakItem = 0
			
			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()
			
			if itemType == item.ITEM_TYPE_ARMOR and itemSubType == item.ARMOR_BODY:
				if transmutation == -1:
					if window == player.SAFEBOX:
						transmutation = safebox.GetItemTransmutation(slot)
					elif window == player.MALL:
						transmutation = safebox.GetMallItemTransmutation(slot)
					else:
						transmutation = player.GetItemTransmutation(window, slot)
				
				if itemVnum > 0 and transmutation > 0:
					item.SelectItem(transmutation)
					itemType = item.GetItemType()
					itemSubType = item.GetItemSubType()
					if itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_BODY:
						item.SelectItem(transmutation)
						bakItem = 1
		
		if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE):
			textLine = self.AppendTextLine(localeinfo.FOR_FEMALE, self.NORMAL_COLOR, True)
			textLine.SetFeather()
		
		if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE):
			textLine = self.AppendTextLine(localeinfo.FOR_MALE, self.NORMAL_COLOR, True)
			textLine.SetFeather()
		
		if app.ENABLE_CHANGELOOK and bakItem:
			item.SelectItem(itemVnum)

	def __AppendPotionInformation(self):
		self.AppendSpace(5)

		healHP = item.GetValue(0)
		healSP = item.GetValue(1)
		healStatus = item.GetValue(2)
		healPercentageHP = item.GetValue(3)
		healPercentageSP = item.GetValue(4)

		if healHP > 0:
			self.AppendTextLine(localeinfo.TOOLTIP_POTION_PLUS_HP_POINT % healHP, self.GetChangeTextLineColor(healHP))
		if healSP > 0:
			self.AppendTextLine(localeinfo.TOOLTIP_POTION_PLUS_SP_POINT % healSP, self.GetChangeTextLineColor(healSP))
		if healStatus != 0:
			self.AppendTextLine(localeinfo.TOOLTIP_POTION_CURE)
		if healPercentageHP > 0:
			self.AppendTextLine(localeinfo.TOOLTIP_POTION_PLUS_HP_PERCENT % healPercentageHP, self.GetChangeTextLineColor(healPercentageHP))
		if healPercentageSP > 0:
			self.AppendTextLine(localeinfo.TOOLTIP_POTION_PLUS_SP_PERCENT % healPercentageSP, self.GetChangeTextLineColor(healPercentageSP))

	def __AppendAbilityPotionInformation(self):

		self.AppendSpace(5)

		abilityType = item.GetValue(0)
		time = item.GetValue(1)
		point = item.GetValue(2)

		if abilityType == item.APPLY_ATT_SPEED:
			self.AppendTextLine(localeinfo.TOOLTIP_POTION_PLUS_ATTACK_SPEED % point, self.GetChangeTextLineColor(point))
		elif abilityType == item.APPLY_MOV_SPEED:
			self.AppendTextLine(localeinfo.TOOLTIP_POTION_PLUS_MOVING_SPEED % point, self.GetChangeTextLineColor(point))

		if time > 0:
			minute = (time / 60)
			second = (time % 60)
			timeString = localeinfo.TOOLTIP_POTION_TIME

			if minute > 0:
				timeString += str(minute) + localeinfo.TOOLTIP_POTION_MIN
			if second > 0:
				timeString += " " + str(second) + localeinfo.TOOLTIP_POTION_SEC

			self.AppendTextLine(timeString)

	def GetPriceColor(self, price):
			return self.LOW_PRICE_COLOR

	def AppendPrice(self, price):
		if not app.ENABLE_BUY_WITH_ITEM:
			self.AppendSpace(5)
		
		self.AppendTextLine(localeinfo.ITEM_IS_YANG  % (localeinfo.NumberToMoneyString(price)), self.GetPriceColor(price))

	def AppendPriceBySecondaryCoin(self, price):
		self.AppendSpace(5)
		self.AppendTextLine(localeinfo.TOOLTIP_BUYPRICE  % (localeinfo.NumberToSecondaryCoinString(price)), self.GetPriceColor(price))

	def AppendSellingPrice(self, price):
		if item.IsAntiFlag(item.ITEM_ANTIFLAG_SELL):
			self.AppendTextLine(localeinfo.TOOLTIP_ANTI_SELL, self.DISABLE_COLOR)
			self.AppendSpace(5)
		else:
			self.AppendTextLine(localeinfo.TOOLTIP_SELLPRICE % (localeinfo.NumberToMoneyString(price)), self.GetPriceColor(price))
			self.AppendSpace(5)

	def AppendMetinInformation(self):
		if constinfo.ENABLE_FULLSTONE_DETAILS:
			for i in xrange(item.ITEM_APPLY_MAX_NUM):
				(affectType, affectValue) = item.GetAffect(i)
				affectString = self.__GetAffectString(affectType, affectValue)
				if affectString:
					self.AppendSpace(5)
					self.AppendTextLine(affectString, self.GetChangeTextLineColor(affectValue))

	def AppendMetinWearInformation(self):

		self.AppendSpace(5)
		self.AppendTextLine(localeinfo.TOOLTIP_SOCKET_REFINABLE_ITEM, self.NORMAL_COLOR)

		flagList = (item.IsWearableFlag(item.WEARABLE_BODY),
					item.IsWearableFlag(item.WEARABLE_HEAD),
					item.IsWearableFlag(item.WEARABLE_FOOTS),
					item.IsWearableFlag(item.WEARABLE_WRIST),
					item.IsWearableFlag(item.WEARABLE_WEAPON),
					item.IsWearableFlag(item.WEARABLE_NECK),
					item.IsWearableFlag(item.WEARABLE_EAR),
					item.IsWearableFlag(item.WEARABLE_UNIQUE),
					item.IsWearableFlag(item.WEARABLE_SHIELD),
					item.IsWearableFlag(item.WEARABLE_ARROW))

		wearNames = ""
		for i in xrange(self.WEAR_COUNT):

			name = self.WEAR_NAMES[i]
			flag = flagList[i]

			if flag:
				wearNames += "  "
				wearNames += name

		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetFontName(self.defFontName)
		textLine.SetPosition(self.toolTipWidth/2, self.toolTipHeight)
		textLine.SetHorizontalAlignCenter()
		textLine.SetPackedFontColor(self.NORMAL_COLOR)
		textLine.SetText(wearNames)
		textLine.Show()
		self.childrenList.append(textLine)

		self.toolTipHeight += self.TEXT_LINE_HEIGHT
		self.ResizeToolTip()

	def GetMetinSocketType(self, number):
		if player.METIN_SOCKET_TYPE_NONE == number:
			return player.METIN_SOCKET_TYPE_NONE
		elif player.METIN_SOCKET_TYPE_SILVER == number:
			return player.METIN_SOCKET_TYPE_SILVER
		elif player.METIN_SOCKET_TYPE_GOLD == number:
			return player.METIN_SOCKET_TYPE_GOLD
		else:
			item.SelectItem(number)
			if item.METIN_NORMAL == item.GetItemSubType():
				return player.METIN_SOCKET_TYPE_SILVER
			elif item.METIN_GOLD == item.GetItemSubType():
				return player.METIN_SOCKET_TYPE_GOLD
			elif "USE_PUT_INTO_ACCESSORY_SOCKET" == item.GetUseType(number):
				return player.METIN_SOCKET_TYPE_SILVER
			elif "USE_PUT_INTO_RING_SOCKET" == item.GetUseType(number):
				return player.METIN_SOCKET_TYPE_SILVER
			elif "USE_PUT_INTO_BELT_SOCKET" == item.GetUseType(number):
				return player.METIN_SOCKET_TYPE_SILVER

		return player.METIN_SOCKET_TYPE_NONE

	def GetMetinItemIndex(self, number):
		if player.METIN_SOCKET_TYPE_SILVER == number:
			return 0
		if player.METIN_SOCKET_TYPE_GOLD == number:
			return 0

		return number

	def __AppendAccessoryMetinSlotInfo(self, metinSlot, mtrlVnum):
		ACCESSORY_SOCKET_MAX_SIZE = 3

		cur=min(metinSlot[0], ACCESSORY_SOCKET_MAX_SIZE)
		end=min(metinSlot[1], ACCESSORY_SOCKET_MAX_SIZE)

		affectType1, affectValue1 = item.GetAffect(0)
		affectList1=[0, max(1, affectValue1*10/100), max(2, affectValue1*20/100), max(3, affectValue1*40/100)]

		affectType2, affectValue2 = item.GetAffect(1)
		affectList2=[0, max(1, affectValue2*10/100), max(2, affectValue2*20/100), max(3, affectValue2*40/100)]

		affectType3, affectValue3 = item.GetAffect(2)
		affectList3=[0, max(1, affectValue3*10/100), max(2, affectValue3*20/100), max(3, affectValue3*40/100)]

		mtrlPos=0
		mtrlList=[mtrlVnum]*cur+[player.METIN_SOCKET_TYPE_SILVER]*(end-cur)
		
		for mtrl in mtrlList:
			affectString1 = self.__GetAffectString(affectType1, affectList1[mtrlPos+1]-affectList1[mtrlPos])
			affectString2 = self.__GetAffectString(affectType2, affectList2[mtrlPos+1]-affectList2[mtrlPos])
			affectString3 = self.__GetAffectString(affectType3, affectList3[mtrlPos+1]-affectList3[mtrlPos])

			leftTime = 0
			if cur == mtrlPos+1:
				leftTime=metinSlot[2]

			self.__AppendMetinSlotInfo_AppendMetinSocketData(mtrlPos, mtrl, affectString1, affectString2, affectString3, leftTime)
			mtrlPos+=1

	def __AppendMetinSlotInfo(self, metinSlot):
		if self.__AppendMetinSlotInfo_IsEmptySlotList(metinSlot):
			return

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			self.__AppendMetinSlotInfo_AppendMetinSocketData(i, metinSlot[i])

	def __AppendMetinSlotInfo_IsEmptySlotList(self, metinSlot):
		if 0 == metinSlot:
			return 1

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlotData=metinSlot[i]
			if 0 != self.GetMetinSocketType(metinSlotData):
				if 0 != self.GetMetinItemIndex(metinSlotData):
					return 0

		return 1

	def __AppendMetinSlotInfo_AppendMetinSocketData(self, index, metinSlotData, custumAffectString="", custumAffectString2="", custumAffectString3="", leftTime=0):

		slotType = self.GetMetinSocketType(metinSlotData)
		itemIndex = self.GetMetinItemIndex(metinSlotData)

		if 0 == slotType:
			return

		self.AppendSpace(5)

		slotImage = ui.ImageBox()
		slotImage.SetParent(self)
		slotImage.Show()

		## Name
		nameTextLine = ui.TextLine()
		nameTextLine.SetParent(self)
		nameTextLine.SetFontName(self.defFontName)
		nameTextLine.SetPackedFontColor(self.NORMAL_COLOR)
		nameTextLine.SetOutline()
		nameTextLine.SetFeather()
		nameTextLine.Show()
		stoneList = len(self.childrenList) - 1
		self.childrenList.append(nameTextLine)
		self.childrenListStone.append(stoneList + 1)
		if player.METIN_SOCKET_TYPE_SILVER == slotType:
			slotImage.LoadImage("d:/ymir work/ui/game/windows/metin_slot_silver.sub")
		elif player.METIN_SOCKET_TYPE_GOLD == slotType:
			slotImage.LoadImage("d:/ymir work/ui/game/windows/metin_slot_gold.sub")

		self.childrenList.append(slotImage)
		self.childrenListStone.append(stoneList + 2)
		if localeinfo.IsARABIC():
			slotImage.SetPosition(self.toolTipWidth - slotImage.GetWidth() - 9, self.toolTipHeight-1)
			nameTextLine.SetPosition(self.toolTipWidth - 50, self.toolTipHeight + 2)
		else:
			slotImage.SetPosition(9, self.toolTipHeight-1)
			nameTextLine.SetPosition(50, self.toolTipHeight + 2)

		metinImage = ui.ImageBox()
		metinImage.SetParent(self)
		metinImage.Show()
		self.childrenList.append(metinImage)
		self.childrenListStone.append(stoneList + 3)
		if itemIndex:

			item.SelectItem(itemIndex)

			## Image
			try:
				metinImage.LoadImage(item.GetIconImageFileName())
			except:
				dbg.TraceError("ItemToolTip.__AppendMetinSocketData() - Failed to find image file %d:%s" %
					(itemIndex, item.GetIconImageFileName())
				)

			nameTextLine.SetText(item.GetItemName())

			## Affect
			affectTextLine = ui.TextLine()
			affectTextLine.SetParent(self)
			affectTextLine.SetFontName(self.defFontName)
			affectTextLine.SetPackedFontColor(self.POSITIVE_COLOR)
			affectTextLine.SetOutline()
			affectTextLine.SetFeather()
			affectTextLine.Show()

			if localeinfo.IsARABIC():
				metinImage.SetPosition(self.toolTipWidth - metinImage.GetWidth() - 10, self.toolTipHeight)
				affectTextLine.SetPosition(self.toolTipWidth - 50, self.toolTipHeight + 16 + 2)
			else:
				metinImage.SetPosition(10, self.toolTipHeight)
				affectTextLine.SetPosition(50, self.toolTipHeight + 16 + 2)

			if custumAffectString:
				affectTextLine.SetText(custumAffectString)
			elif itemIndex!=constinfo.ERROR_METIN_STONE:
				affectType, affectValue = item.GetAffect(0)
				affectString = self.__GetAffectString(affectType, affectValue)
				if affectString:
					affectTextLine.SetText(affectString)
			else:
				affectTextLine.SetText(localeinfo.TOOLTIP_APPLY_NOAFFECT)

			self.childrenList.append(affectTextLine)
			self.childrenListStone.append(stoneList + 4)
			if constinfo.ENABLE_FULLSTONE_DETAILS and (not custumAffectString2) and (itemIndex!=constinfo.ERROR_METIN_STONE):
				custumAffectString2 = self.__GetAffectString(*item.GetAffect(1))

			if custumAffectString2:
				affectTextLine = ui.TextLine()
				affectTextLine.SetParent(self)
				affectTextLine.SetFontName(self.defFontName)
				affectTextLine.SetPackedFontColor(self.POSITIVE_COLOR)
				affectTextLine.SetPosition(50, self.toolTipHeight + 16 + 2 + 16 + 2)
				affectTextLine.SetOutline()
				affectTextLine.SetFeather()
				affectTextLine.Show()
				affectTextLine.SetText(custumAffectString2)
				self.childrenList.append(affectTextLine)
				self.childrenListStone.append(stoneList + 5)
				self.toolTipHeight += 16 + 2

			if constinfo.ENABLE_FULLSTONE_DETAILS and (not custumAffectString3) and (itemIndex!=constinfo.ERROR_METIN_STONE):
				custumAffectString3 = self.__GetAffectString(*item.GetAffect(2))

			if custumAffectString3:
				affectTextLine = ui.TextLine()
				affectTextLine.SetParent(self)
				affectTextLine.SetFontName(self.defFontName)
				affectTextLine.SetPackedFontColor(self.POSITIVE_COLOR)
				affectTextLine.SetPosition(50, self.toolTipHeight + 16 + 2 + 16 + 2)
				affectTextLine.SetOutline()
				affectTextLine.SetFeather()
				affectTextLine.Show()
				affectTextLine.SetText(custumAffectString3)
				self.childrenList.append(affectTextLine)
				self.childrenListStone.append(stoneList + 6)
				self.toolTipHeight += 16 + 2

			if 0 != leftTime:
				if app.ENABLE_INFINITE_RAFINES and leftTime > 86400:
					self.toolTipHeight += 35
					self.ResizeToolTip()
					return
				
				timeText = (localeinfo.LEFT_TIME + " : " + localeinfo.SecondToDHM(leftTime))

				timeTextLine = ui.TextLine()
				timeTextLine.SetParent(self)
				timeTextLine.SetFontName(self.defFontName)
				timeTextLine.SetPackedFontColor(self.POSITIVE_COLOR)
				timeTextLine.SetPosition(50, self.toolTipHeight + 16 + 2 + 16 + 2)
				timeTextLine.SetOutline()
				timeTextLine.SetFeather()
				timeTextLine.Show()
				timeTextLine.SetText(timeText)
				self.childrenList.append(timeTextLine)
				self.childrenListStone.append(stoneList + 7)
				self.toolTipHeight += 16 + 2
		else:
			nameTextLine.SetText(localeinfo.TOOLTIP_SOCKET_EMPTY)

		self.toolTipHeight += 35
		self.ResizeToolTip()

	def __AppendFishInfo(self, size):
		if size > 0:
			self.AppendSpace(5)
			self.AppendTextLine(localeinfo.TOOLTIP_FISH_LEN % (float(size) / 100.0), self.NORMAL_COLOR)

	def AppendUniqueItemLastTime(self, restMin):
		restSecond = restMin*60
		self.AppendSpace(5)
		self.AppendTextLine(localeinfo.LEFT_TIME + " : " + localeinfo.SecondToDHM(restSecond), self.NORMAL_COLOR)

	def AppendMallItemRealLastTime(self, leftSec):
		self.AppendSpace(5)
		self.AppendTextLine(localeinfo.LEFT_TIME + " : " + localeinfo.SecondToDHM(leftSec), self.NORMAL_COLOR)

	def AppendMallItemLastTime(self, endTime):
		leftSec = max(0, endTime - app.GetGlobalTimeStamp())
		self.AppendSpace(5)
		self.AppendTextLine(localeinfo.LEFT_TIME + " : " + localeinfo.SecondToDHM(leftSec), self.NORMAL_COLOR)

	def AppendTimerBasedOnWearLastTime(self, metinSlot):
		if 0 == metinSlot[0]:
			self.AppendSpace(5)
			self.AppendTextLine(localeinfo.CANNOT_USE, self.DISABLE_COLOR)
		else:
			endTime = app.GetGlobalTimeStamp() + metinSlot[0]
			self.AppendMallItemLastTime(endTime)

	def AppendRealTimeStartFirstUseLastTime(self, item, metinSlot, limitIndex):
		useCount = metinSlot[1]
		endTime = metinSlot[0]
		if 0 == useCount:
			if 0 == endTime:
				(limitType, limitValue) = item.GetLimit(limitIndex)
				endTime = limitValue
			endTime += app.GetGlobalTimeStamp()
		
		self.AppendMallItemLastTime(endTime)

	if app.ENABLE_ACCE_SYSTEM:
		def SetAcceResultItem(self, slotIndex, window_type = player.INVENTORY):
			(itemVnum, MinAbs, MaxAbs) = acce.GetResultItem()
			if not itemVnum:
				return
			
			self.ClearToolTip()
			
			metinSlot 	= [player.GetItemMetinSocket(window_type, slotIndex, i) 	for i in xrange(player.METIN_SOCKET_MAX_NUM)	]
			attrSlot 	= [player.GetItemAttribute(window_type, slotIndex, i) 		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)	]
			
			item.SelectItem(itemVnum)
			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()
			isCostumeStole = False
			if app.ENABLE_STOLE_COSTUME:
				if itemType != item.ITEM_TYPE_COSTUME and itemSubType != item.COSTUME_TYPE_ACCE and itemSubType != item.COSTUME_TYPE_STOLE:
					return
			else:
				if itemType != item.ITEM_TYPE_COSTUME and itemSubType != item.COSTUME_TYPE_ACCE:
					return
			
			absChance = MaxAbs
			itemDesc = item.GetItemDescription()
			self.__SetItemTitle(itemVnum, metinSlot, attrSlot)
			self.AppendSpace(5)
			if app.ENABLE_RARITY_SYSTEM:
				item.SelectItem(itemVnum)
				if itemSubType == item.COSTUME_TYPE_STOLE:
					itemRarity = item.GetRarity()
					rarityDesc = self.RARITY_DESC[itemRarity]
					self.AppendTextLine(rarityDesc, self.RARITY_COLOR[itemRarity])
					self.AppendSpace(5)
			
			self.AppendDescription(itemDesc, 26)
			self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
			if app.ENABLE_RARITY_SYSTEM:
				item.SelectItem(itemVnum)
				if itemSubType == item.COSTUME_TYPE_ACCE:
					itemRarity = item.GetRarity()
					rarityDesc = self.RARITY_DESC[itemRarity]
					self.AppendTextLine(rarityDesc, self.RARITY_COLOR[itemRarity])
			
			self.__AppendLimitInformation()
			
			if app.ENABLE_STOLE_COSTUME:
				if itemSubType == item.COSTUME_TYPE_STOLE:
					isCostumeStole = True
			
			## ABSORPTION RATE
			if MinAbs == MaxAbs:
				if isCostumeStole == False:
					self.AppendTextLine(localeinfo.SASH_ABSORB_CHANCE % (MinAbs), self.CONDITION_COLOR)
			else:
				if isCostumeStole == False:
					self.AppendTextLine(localeinfo.SASH_ABSORB_CHANCE2 % (MinAbs, MaxAbs), self.CONDITION_COLOR)
			## END ABSOPRTION RATE
			
			if isCostumeStole:
				item.SelectItem(itemVnum)
				self.AppendTextLine(localeinfo.GRADE_STOLA % (item.GetValue(0)), self.CONDITION_COLOR)
				self.AppendSpace(5)
			
			itemAbsorbedVnum = int(metinSlot[acce.ABSORBED_SOCKET])
			if itemAbsorbedVnum:
				## ATTACK / DEFENCE
				item.SelectItem(itemAbsorbedVnum)
				if item.GetItemType() == item.ITEM_TYPE_WEAPON:
					if item.GetItemSubType() == item.WEAPON_FAN:
						self.__AppendMagicAttackInfo(absChance)
						item.SelectItem(itemAbsorbedVnum)
						self.__AppendAttackPowerInfo(absChance)
					else:
						self.__AppendAttackPowerInfo(absChance)
						item.SelectItem(itemAbsorbedVnum)
						self.__AppendMagicAttackInfo(absChance)
				elif item.GetItemType() == item.ITEM_TYPE_ARMOR:
					defGrade = item.GetValue(1)
					defBonus = item.GetValue(5) * 2
					defGrade = self.CalcAcceValue(defGrade, absChance)
					defBonus = self.CalcAcceValue(defBonus, absChance)
					
					if defGrade > 0:
						self.AppendSpace(5)
						self.AppendTextLine(localeinfo.TOOLTIP_ITEM_DEF_GRADE % (defGrade + defBonus), self.GetChangeTextLineColor(defGrade))
					
					item.SelectItem(itemAbsorbedVnum)
					self.__AppendMagicDefenceInfo(absChance)
				## END ATTACK / DEFENCE
				
				## EFFECT
				item.SelectItem(itemAbsorbedVnum)
				for i in xrange(item.ITEM_APPLY_MAX_NUM):
					(affectType, affectValue) = item.GetAffect(i)
					affectValue = self.CalcAcceValue(affectValue, absChance)
					affectString = self.__GetAffectString(affectType, affectValue)
					if affectString and affectValue > 0:
						self.AppendTextLine(affectString, self.GetChangeTextLineColor(affectValue))
					
					item.SelectItem(itemAbsorbedVnum)
				# END EFFECT
				
			item.SelectItem(itemVnum)
			## ATTR
			self.__AppendAttributeInformation(attrSlot, MaxAbs)
			# END ATTR
			
			self.AppendWearableInformation()
			#self.AppendTextLine(localeinfo.VIEW_RENDERING)
			self.ShowToolTip()

		def SetAcceResultAbsItem(self, slotIndex1, slotIndex2, window_type = player.INVENTORY):
			itemVnumAcce = player.GetItemIndex(window_type, slotIndex1)
			itemVnumTarget = player.GetItemIndex(window_type, slotIndex2)
			if not itemVnumAcce or not itemVnumTarget:
				return
			
			self.ClearToolTip()
			
			item.SelectItem(itemVnumAcce)
			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()
			if itemType != item.ITEM_TYPE_COSTUME and itemSubType != item.COSTUME_TYPE_ACCE:
				return
			
			metinSlot = [player.GetItemMetinSocket(window_type, slotIndex1, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attrSlot = [player.GetItemAttribute(window_type, slotIndex2, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			
			itemDesc = item.GetItemDescription()
			self.__SetItemTitle(itemVnumAcce, metinSlot, attrSlot)
			self.AppendDescription(itemDesc, 26)
			self.AppendDescription(item.GetItemSummary(), 26, self.CONDITION_COLOR)
			item.SelectItem(itemVnumAcce)
			self.__AppendLimitInformation()
			
			## ABSORPTION RATE
			self.AppendTextLine(localeinfo.SASH_ABSORB_CHANCE % (metinSlot[acce.ABSORPTION_SOCKET]), self.CONDITION_COLOR)
			## END ABSOPRTION RATE
			
			## ATTACK / DEFENCE
			itemAbsorbedVnum = itemVnumTarget
			item.SelectItem(itemAbsorbedVnum)
			if item.GetItemType() == item.ITEM_TYPE_WEAPON:
				if item.GetItemSubType() == item.WEAPON_FAN:
					self.__AppendMagicAttackInfo(metinSlot[acce.ABSORPTION_SOCKET])
					item.SelectItem(itemAbsorbedVnum)
					self.__AppendAttackPowerInfo(metinSlot[acce.ABSORPTION_SOCKET])
				else:
					self.__AppendAttackPowerInfo(metinSlot[acce.ABSORPTION_SOCKET])
					item.SelectItem(itemAbsorbedVnum)
					self.__AppendMagicAttackInfo(metinSlot[acce.ABSORPTION_SOCKET])
			elif item.GetItemType() == item.ITEM_TYPE_ARMOR:
				defGrade = item.GetValue(1)
				defBonus = item.GetValue(5) * 2
				defGrade = self.CalcAcceValue(defGrade, metinSlot[acce.ABSORPTION_SOCKET])
				defBonus = self.CalcAcceValue(defBonus, metinSlot[acce.ABSORPTION_SOCKET])
				
				if defGrade > 0:
					self.AppendSpace(5)
					self.AppendTextLine(localeinfo.TOOLTIP_ITEM_DEF_GRADE % (defGrade + defBonus), self.GetChangeTextLineColor(defGrade))
				
				item.SelectItem(itemAbsorbedVnum)
				self.__AppendMagicDefenceInfo(metinSlot[acce.ABSORPTION_SOCKET])
			## END ATTACK / DEFENCE
			
			## EFFECT
			item.SelectItem(itemAbsorbedVnum)
			for i in xrange(item.ITEM_APPLY_MAX_NUM):
				(affectType, affectValue) = item.GetAffect(i)
				affectValue = self.CalcAcceValue(affectValue, metinSlot[acce.ABSORPTION_SOCKET])
				affectString = self.__GetAffectString(affectType, affectValue)
				if affectString and affectValue > 0:
					self.AppendTextLine(affectString, self.GetChangeTextLineColor(affectValue))
				
				item.SelectItem(itemAbsorbedVnum)
			## END EFFECT
			
			## ATTR
			item.SelectItem(itemAbsorbedVnum)
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				type = attrSlot[i][0]
				value = attrSlot[i][1]
				if not value:
					continue
				
				value = self.CalcAcceValue(value, metinSlot[acce.ABSORPTION_SOCKET])
				affectString = self.__GetAffectString(type, value)
				if affectString and value > 0:
					affectColor = self.__GetAttributeColor(i, value)
					self.AppendTextLine(affectString, affectColor)
				
				item.SelectItem(itemAbsorbedVnum)
			## END ATTR
			
			## WEARABLE
			item.SelectItem(itemVnumAcce)
			self.AppendSpace(5)
			self.AppendTextLine(localeinfo.TOOLTIP_ITEM_WEARABLE_JOB, self.NORMAL_COLOR)
			
			item.SelectItem(itemVnumAcce)
			flagList = (
						not item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR),
						not item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN),
						not item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA),
						not item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN)
			)
			
			characterNames = ""
			for i in xrange(self.CHARACTER_COUNT):
				name = self.CHARACTER_NAMES[i]
				flag = flagList[i]
				if flag:
					characterNames += " "
					characterNames += name
			
			textLine = self.AppendTextLine(characterNames, self.NORMAL_COLOR, True)
			textLine.SetFeather()
			
			item.SelectItem(itemVnumAcce)
			if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE):
				textLine = self.AppendTextLine(localeinfo.FOR_FEMALE, self.NORMAL_COLOR, True)
				textLine.SetFeather()
			
			if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE):
				textLine = self.AppendTextLine(localeinfo.FOR_MALE, self.NORMAL_COLOR, True)
				textLine.SetFeather()
			## END WEARABLE
			
			self.ShowToolTip()
	
	
	

class HyperlinkItemToolTip(ItemToolTip):
	def __init__(self):
		ItemToolTip.__init__(self, isPickable=True)

	def SetHyperlinkItem(self, tokens):
		minTokenCount = 3 + player.METIN_SOCKET_MAX_NUM
		if app.ENABLE_CHANGELOOK:
			minTokenCount += 1
		
		if app.ENABLE_MULTI_LANGUAGE:
			minTokenCount += 1
		
		maxTokenCount = minTokenCount + 2 * player.ATTRIBUTE_SLOT_MAX_NUM
		maxTokenCount += 1
		
		if tokens and len(tokens) >= minTokenCount and len(tokens) <= maxTokenCount:
			head, vnum, flag = tokens[:3]
			itemVnum = int(vnum, 16)
			metinSlot = [int(metin, 16) for metin in tokens[3:3+player.METIN_SOCKET_MAX_NUM]]
			
			changelookvnum = -1
			if app.ENABLE_CHANGELOOK:
				changelookvnum = int(tokens[minTokenCount], 16)
			
			rests = tokens[minTokenCount + 1:]
			
			if rests:
				attrSlot = []

				rests.reverse()
				while rests:
					key = int(rests.pop(), 16)
					if rests:
						val = int(rests.pop())
						attrSlot.append((key, val))
					
				attrSlot += [(0, 0)] * (player.ATTRIBUTE_SLOT_MAX_NUM - len(attrSlot))
			else:
				attrSlot = [(0, 0)] * player.ATTRIBUTE_SLOT_MAX_NUM
			
			self.ClearToolTip()
			
			if app.ENABLE_CHANGELOOK:
				self.AddItemData(itemVnum, metinSlot, attrSlot, 1, transmutation = changelookvnum)
			else:
				self.AddItemData(itemVnum, metinSlot, attrSlot, 1)
			
			ItemToolTip.OnUpdate(self)

	def OnUpdate(self):
		pass

	def OnMouseLeftButtonDown(self):
		self.Hide()

class SkillToolTip(ToolTip):

	POINT_NAME_DICT = {
		player.LEVEL : localeinfo.SKILL_TOOLTIP_LEVEL,
		player.IQ : localeinfo.SKILL_TOOLTIP_INT,
	}

	SKILL_TOOL_TIP_WIDTH = 200
	PARTY_SKILL_TOOL_TIP_WIDTH = 340

	PARTY_SKILL_EXPERIENCE_AFFECT_LIST = (	( 2, 2,  10,),
											( 8, 3,  20,),
											(14, 4,  30,),
											(22, 5,  45,),
											(28, 6,  60,),
											(34, 7,  80,),
											(38, 8, 100,), )

	PARTY_SKILL_PLUS_GRADE_AFFECT_LIST = (	( 4, 2, 1, 0,),
											(10, 3, 2, 0,),
											(16, 4, 2, 1,),
											(24, 5, 2, 2,), )

	PARTY_SKILL_ATTACKER_AFFECT_LIST = (	( 36, 3, ),
											( 26, 1, ),
											( 32, 2, ), )

	SKILL_GRADE_NAME = {	player.SKILL_GRADE_MASTER : localeinfo.SKILL_GRADE_NAME_MASTER,
							player.SKILL_GRADE_GRAND_MASTER : localeinfo.SKILL_GRADE_NAME_GRAND_MASTER,
							player.SKILL_GRADE_PERFECT_MASTER : localeinfo.SKILL_GRADE_NAME_PERFECT_MASTER, }

	AFFECT_NAME_DICT =	{
							"HP" : localeinfo.TOOLTIP_SKILL_AFFECT_ATT_POWER,
							"ATT_GRADE" : localeinfo.TOOLTIP_SKILL_AFFECT_ATT_GRADE,
							"DEF_GRADE" : localeinfo.TOOLTIP_SKILL_AFFECT_DEF_GRADE,
							"ATT_SPEED" : localeinfo.TOOLTIP_SKILL_AFFECT_ATT_SPEED,
							"MOV_SPEED" : localeinfo.TOOLTIP_SKILL_AFFECT_MOV_SPEED,
							"DODGE" : localeinfo.TOOLTIP_SKILL_AFFECT_DODGE,
							"RESIST_NORMAL" : localeinfo.TOOLTIP_SKILL_AFFECT_RESIST_NORMAL,
							"REFLECT_MELEE" : localeinfo.TOOLTIP_SKILL_AFFECT_REFLECT_MELEE,
						}
	AFFECT_APPEND_TEXT_DICT =	{
									"DODGE" : "%",
									"RESIST_NORMAL" : "%",
									"REFLECT_MELEE" : "%",
								}

	def __init__(self):
		ToolTip.__init__(self, self.SKILL_TOOL_TIP_WIDTH)
	def __del__(self):
		ToolTip.__del__(self)

	def SetSkill(self, skillIndex, skillLevel = -1):

		if 0 == skillIndex:
			return

		if skill.SKILL_TYPE_GUILD == skill.GetSkillType(skillIndex):

			if self.SKILL_TOOL_TIP_WIDTH != self.toolTipWidth:
				self.toolTipWidth = self.SKILL_TOOL_TIP_WIDTH
				self.ResizeToolTip()

			self.AppendDefaultData(skillIndex)
			self.AppendSkillConditionData(skillIndex)
			self.AppendGuildSkillData(skillIndex, skillLevel)

		else:

			if self.SKILL_TOOL_TIP_WIDTH != self.toolTipWidth:
				self.toolTipWidth = self.SKILL_TOOL_TIP_WIDTH
				self.ResizeToolTip()

			slotIndex = player.GetSkillSlotIndex(skillIndex)
			skillGrade = player.GetSkillGrade(slotIndex)
			skillLevel = player.GetSkillLevel(slotIndex)
			skillCurrentPercentage = player.GetSkillCurrentEfficientPercentage(slotIndex)
			skillNextPercentage = player.GetSkillNextEfficientPercentage(slotIndex)

			self.AppendDefaultData(skillIndex)
			self.AppendSkillConditionData(skillIndex)
			self.AppendSkillDataNew(slotIndex, skillIndex, skillGrade, skillLevel, skillCurrentPercentage, skillNextPercentage)
			self.AppendSkillRequirement(skillIndex, skillLevel)

		self.ShowToolTip()

	def SetSkillNew(self, slotIndex, skillIndex, skillGrade, skillLevel):

		if 0 == skillIndex:
			return

		if player.SKILL_INDEX_TONGSOL == skillIndex:

			slotIndex = player.GetSkillSlotIndex(skillIndex)
			skillLevel = player.GetSkillLevel(slotIndex)

			self.AppendDefaultData(skillIndex)
			self.AppendPartySkillData(skillGrade, skillLevel)

		elif player.SKILL_INDEX_RIDING == skillIndex:

			slotIndex = player.GetSkillSlotIndex(skillIndex)
			self.AppendSupportSkillDefaultData(skillIndex, skillGrade, skillLevel, 30)

		elif player.SKILL_INDEX_SUMMON == skillIndex:

			maxLevel = 10

			self.ClearToolTip()
			self.__SetSkillTitle(skillIndex, skillGrade)

			## Description
			description = skill.GetSkillDescription(skillIndex)
			self.AppendDescription(description, 25)

			if skillLevel == 10:
				self.AppendSpace(5)
				self.AppendTextLine(localeinfo.TOOLTIP_SKILL_LEVEL_MASTER % (skillLevel), self.NORMAL_COLOR)
				self.AppendTextLine(localeinfo.SKILL_SUMMON_DESCRIPTION % (skillLevel*10), self.NORMAL_COLOR)

			else:
				self.AppendSpace(5)
				self.AppendTextLine(localeinfo.TOOLTIP_SKILL_LEVEL % (skillLevel), self.NORMAL_COLOR)
				self.__AppendSummonDescription(skillLevel, self.NORMAL_COLOR)

				self.AppendSpace(5)
				self.AppendTextLine(localeinfo.TOOLTIP_SKILL_LEVEL % (skillLevel+1), self.NEGATIVE_COLOR)
				self.__AppendSummonDescription(skillLevel+1, self.NEGATIVE_COLOR)

		elif skill.SKILL_TYPE_GUILD == skill.GetSkillType(skillIndex):

			if self.SKILL_TOOL_TIP_WIDTH != self.toolTipWidth:
				self.toolTipWidth = self.SKILL_TOOL_TIP_WIDTH
				self.ResizeToolTip()

			self.AppendDefaultData(skillIndex)
			self.AppendSkillConditionData(skillIndex)
			self.AppendGuildSkillData(skillIndex, skillLevel)
		else:
			if self.SKILL_TOOL_TIP_WIDTH != self.toolTipWidth:
				self.toolTipWidth = self.SKILL_TOOL_TIP_WIDTH
				self.ResizeToolTip()

			slotIndex = player.GetSkillSlotIndex(skillIndex)

			skillCurrentPercentage = player.GetSkillCurrentEfficientPercentage(slotIndex)
			skillNextPercentage = player.GetSkillNextEfficientPercentage(slotIndex)

			self.AppendDefaultData(skillIndex, skillGrade)
			self.AppendSkillConditionData(skillIndex)
			self.AppendSkillDataNew(slotIndex, skillIndex, skillGrade, skillLevel, skillCurrentPercentage, skillNextPercentage)
			self.AppendSkillRequirement(skillIndex, skillLevel)

		self.ShowToolTip()

	def __SetSkillTitle(self, skillIndex, skillGrade):
		self.SetTitle(skill.GetSkillName(skillIndex, skillGrade))
		self.__AppendSkillGradeName(skillIndex, skillGrade)

	def __AppendSkillGradeName(self, skillIndex, skillGrade):
		if self.SKILL_GRADE_NAME.has_key(skillGrade):
			self.AppendSpace(5)
			self.AppendTextLine(self.SKILL_GRADE_NAME[skillGrade] % (skill.GetSkillName(skillIndex, 0)), self.CAN_LEVEL_UP_COLOR)

	def SetSkillOnlyName(self, slotIndex, skillIndex, skillGrade):
		if 0 == skillIndex:
			return

		slotIndex = player.GetSkillSlotIndex(skillIndex)

		self.toolTipWidth = self.SKILL_TOOL_TIP_WIDTH
		self.ResizeToolTip()

		self.ClearToolTip()
		self.__SetSkillTitle(skillIndex, skillGrade)
		self.AppendDefaultData(skillIndex, skillGrade)
		self.AppendSkillConditionData(skillIndex)
		self.ShowToolTip()

	def AppendDefaultData(self, skillIndex, skillGrade = 0):
		self.ClearToolTip()
		self.__SetSkillTitle(skillIndex, skillGrade)

		## Level Limit
		levelLimit = skill.GetSkillLevelLimit(skillIndex)
		if levelLimit > 0:

			color = self.NORMAL_COLOR
			if player.GetStatus(player.LEVEL) < levelLimit:
				color = self.NEGATIVE_COLOR

			self.AppendSpace(5)
			self.AppendTextLine(localeinfo.TOOLTIP_ITEM_LIMIT_LEVEL % (levelLimit), color)

		## Description
		description = skill.GetSkillDescription(skillIndex)
		self.AppendDescription(description, 25)

	def AppendSupportSkillDefaultData(self, skillIndex, skillGrade, skillLevel, maxLevel):
		self.ClearToolTip()
		self.__SetSkillTitle(skillIndex, skillGrade)

		## Description
		description = skill.GetSkillDescription(skillIndex)
		self.AppendDescription(description, 25)

		if 1 == skillGrade:
			skillLevel += 19
		elif 2 == skillGrade:
			skillLevel += 29
		elif 3 == skillGrade:
			skillLevel = 40

		self.AppendSpace(5)
		self.AppendTextLine(localeinfo.TOOLTIP_SKILL_LEVEL_WITH_MAX % (skillLevel, maxLevel), self.NORMAL_COLOR)

	def AppendSkillConditionData(self, skillIndex):
		conditionDataCount = skill.GetSkillConditionDescriptionCount(skillIndex)
		if conditionDataCount > 0:
			self.AppendSpace(5)
			for i in xrange(conditionDataCount):
				self.AppendTextLine(skill.GetSkillConditionDescription(skillIndex, i), self.CONDITION_COLOR)

	def AppendGuildSkillData(self, skillIndex, skillLevel):
		skillMaxLevel = 7
		skillCurrentPercentage = float(skillLevel) / float(skillMaxLevel)
		skillNextPercentage = float(skillLevel+1) / float(skillMaxLevel)
		## Current Level
		if skillLevel > 0:
			if self.HasSkillLevelDescription(skillIndex, skillLevel):
				self.AppendSpace(5)
				if skillLevel == skillMaxLevel:
					self.AppendTextLine(localeinfo.TOOLTIP_SKILL_LEVEL_MASTER % (skillLevel), self.NORMAL_COLOR)
				else:
					self.AppendTextLine(localeinfo.TOOLTIP_SKILL_LEVEL % (skillLevel), self.NORMAL_COLOR)

				#####

				for i in xrange(skill.GetSkillAffectDescriptionCount(skillIndex)):
					self.AppendTextLine(skill.GetSkillAffectDescription(skillIndex, i, skillCurrentPercentage), self.ENABLE_COLOR)

				## Cooltime
				coolTime = skill.GetSkillCoolTime(skillIndex, skillCurrentPercentage)
				if coolTime > 0:
					self.AppendTextLine(localeinfo.TOOLTIP_SKILL_COOL_TIME + str(coolTime), self.ENABLE_COLOR)

				## SP
				needGSP = skill.GetSkillNeedSP(skillIndex, skillCurrentPercentage)
				if needGSP > 0:
					self.AppendTextLine(localeinfo.TOOLTIP_NEED_GSP % (needGSP), self.ENABLE_COLOR)

		## Next Level
		if skillLevel < skillMaxLevel:
			if self.HasSkillLevelDescription(skillIndex, skillLevel+1):
				self.AppendSpace(5)
				self.AppendTextLine(localeinfo.TOOLTIP_NEXT_SKILL_LEVEL_1 % (skillLevel+1, skillMaxLevel), self.DISABLE_COLOR)

				#####

				for i in xrange(skill.GetSkillAffectDescriptionCount(skillIndex)):
					self.AppendTextLine(skill.GetSkillAffectDescription(skillIndex, i, skillNextPercentage), self.DISABLE_COLOR)

				## Cooltime
				coolTime = skill.GetSkillCoolTime(skillIndex, skillNextPercentage)
				if coolTime > 0:
					self.AppendTextLine(localeinfo.TOOLTIP_SKILL_COOL_TIME + str(coolTime), self.DISABLE_COLOR)

				## SP
				needGSP = skill.GetSkillNeedSP(skillIndex, skillNextPercentage)
				if needGSP > 0:
					self.AppendTextLine(localeinfo.TOOLTIP_NEED_GSP % (needGSP), self.DISABLE_COLOR)

	def AppendSkillDataNew(self, slotIndex, skillIndex, skillGrade, skillLevel, skillCurrentPercentage, skillNextPercentage):

		self.skillMaxLevelStartDict = { 0 : 17, 1 : 7, 2 : 10, }
		self.skillMaxLevelEndDict = { 0 : 20, 1 : 10, 2 : 10, }

		skillLevelUpPoint = 1
		realSkillGrade = player.GetSkillGrade(slotIndex)
		skillMaxLevelStart = self.skillMaxLevelStartDict.get(realSkillGrade, 15)
		skillMaxLevelEnd = self.skillMaxLevelEndDict.get(realSkillGrade, 20)

		## Current Level
		if skillLevel > 0:
			if self.HasSkillLevelDescription(skillIndex, skillLevel):
				self.AppendSpace(5)
				if skillGrade == skill.SKILL_GRADE_COUNT:
					pass
				elif skillLevel == skillMaxLevelEnd:
					self.AppendTextLine(localeinfo.TOOLTIP_SKILL_LEVEL_MASTER % (skillLevel), self.NORMAL_COLOR)
				else:
					self.AppendTextLine(localeinfo.TOOLTIP_SKILL_LEVEL % (skillLevel), self.NORMAL_COLOR)
				self.AppendSkillLevelDescriptionNew(skillIndex, skillCurrentPercentage, self.ENABLE_COLOR)

		## Next Level
		if skillGrade != skill.SKILL_GRADE_COUNT:
			if skillLevel < skillMaxLevelEnd:
				if self.HasSkillLevelDescription(skillIndex, skillLevel+skillLevelUpPoint):
					self.AppendSpace(5)
					if skillIndex == 141 or skillIndex == 142:
						self.AppendTextLine(localeinfo.TOOLTIP_NEXT_SKILL_LEVEL_3 % (skillLevel+1), self.DISABLE_COLOR)
						idx = 1
						
						self.AppendTextLine(localeinfo.TOOLTIP_NEXT_SKILL_LEVEL_1 % (skillLevel+idx, skillMaxLevelEnd), self.DISABLE_COLOR)
					self.AppendSkillLevelDescriptionNew(skillIndex, skillNextPercentage, self.DISABLE_COLOR)

	def AppendSkillLevelDescriptionNew(self, skillIndex, skillPercentage, color):

		affectDataCount = skill.GetNewAffectDataCount(skillIndex)
		if affectDataCount > 0:
			for i in xrange(affectDataCount):
				type, minValue, maxValue = skill.GetNewAffectData(skillIndex, i, skillPercentage)

				if not self.AFFECT_NAME_DICT.has_key(type):
					continue

				minValue = int(minValue)
				maxValue = int(maxValue)
				affectText = self.AFFECT_NAME_DICT[type]

				if "HP" == type:
					if minValue < 0 and maxValue < 0:
						minValue *= -1
						maxValue *= -1

					else:
						affectText = localeinfo.TOOLTIP_SKILL_AFFECT_HEAL

				affectText += str(minValue)
				if minValue != maxValue:
					affectText += " - " + str(maxValue)
				affectText += self.AFFECT_APPEND_TEXT_DICT.get(type, "")
				self.AppendTextLine(affectText, color)

		else:
			for i in xrange(skill.GetSkillAffectDescriptionCount(skillIndex)):
				self.AppendTextLine(skill.GetSkillAffectDescription(skillIndex, i, skillPercentage), color)


		## Duration
		duration = skill.GetDuration(skillIndex, skillPercentage)
		if duration > 0:
			self.AppendTextLine(localeinfo.TOOLTIP_SKILL_DURATION % (duration), color)

		## Cooltime
		coolTime = skill.GetSkillCoolTime(skillIndex, skillPercentage)
		if coolTime > 0:
			self.AppendTextLine(localeinfo.TOOLTIP_SKILL_COOL_TIME + str(coolTime), color)

		## SP
		needSP = skill.GetSkillNeedSP(skillIndex, skillPercentage)
		if needSP != 0:
			continuationSP = skill.GetSkillContinuationSP(skillIndex, skillPercentage)

			if skill.IsUseHPSkill(skillIndex):
				self.AppendNeedHP(needSP, continuationSP, color)
			else:
				self.AppendNeedSP(needSP, continuationSP, color)

	def AppendSkillRequirement(self, skillIndex, skillLevel):

		skillMaxLevel = skill.GetSkillMaxLevel(skillIndex)

		if skillLevel >= skillMaxLevel:
			return

		isAppendHorizontalLine = False

		## Requirement
		if skill.IsSkillRequirement(skillIndex):

			if not isAppendHorizontalLine:
				isAppendHorizontalLine = True
				self.AppendHorizontalLine()

			requireSkillName, requireSkillLevel = skill.GetSkillRequirementData(skillIndex)

			color = self.CANNOT_LEVEL_UP_COLOR
			if skill.CheckRequirementSueccess(skillIndex):
				color = self.CAN_LEVEL_UP_COLOR
			self.AppendTextLine(localeinfo.TOOLTIP_REQUIREMENT_SKILL_LEVEL % (requireSkillName, requireSkillLevel), color)

		## Require Stat
		requireStatCount = skill.GetSkillRequireStatCount(skillIndex)
		if requireStatCount > 0:

			for i in xrange(requireStatCount):
				type, level = skill.GetSkillRequireStatData(skillIndex, i)
				if self.POINT_NAME_DICT.has_key(type):

					if not isAppendHorizontalLine:
						isAppendHorizontalLine = True
						self.AppendHorizontalLine()

					name = self.POINT_NAME_DICT[type]
					color = self.CANNOT_LEVEL_UP_COLOR
					if player.GetStatus(type) >= level:
						color = self.CAN_LEVEL_UP_COLOR
					self.AppendTextLine(localeinfo.TOOLTIP_REQUIREMENT_STAT_LEVEL % (name, level), color)

	def HasSkillLevelDescription(self, skillIndex, skillLevel):
		if skill.GetSkillAffectDescriptionCount(skillIndex) > 0:
			return True
		if skill.GetSkillCoolTime(skillIndex, skillLevel) > 0:
			return True
		if skill.GetSkillNeedSP(skillIndex, skillLevel) > 0:
			return True

		return False

	def AppendMasterAffectDescription(self, index, desc, color):
		self.AppendTextLine(desc, color)

	def AppendNextAffectDescription(self, index, desc):
		self.AppendTextLine(desc, self.DISABLE_COLOR)

	def AppendNeedHP(self, needSP, continuationSP, color):

		self.AppendTextLine(localeinfo.TOOLTIP_NEED_HP % (needSP), color)

		if continuationSP > 0:
			self.AppendTextLine(localeinfo.TOOLTIP_NEED_HP_PER_SEC % (continuationSP), color)

	def AppendNeedSP(self, needSP, continuationSP, color):

		if -1 == needSP:
			self.AppendTextLine(localeinfo.TOOLTIP_NEED_ALL_SP, color)

		else:
			self.AppendTextLine(localeinfo.TOOLTIP_NEED_SP % (needSP), color)

		if continuationSP > 0:
			self.AppendTextLine(localeinfo.TOOLTIP_NEED_SP_PER_SEC % (continuationSP), color)

	def AppendPartySkillData(self, skillGrade, skillLevel):
		# @fixme008 BEGIN
		def comma_fix(vl):
			return vl.replace("%,0f", "%.0f")
		# @fixme008 END

		if 1 == skillGrade:
			skillLevel += 19
		elif 2 == skillGrade:
			skillLevel += 29
		elif 3 == skillGrade:
			skillLevel =  40

		if skillLevel <= 0:
			return

		skillIndex = player.SKILL_INDEX_TONGSOL
		slotIndex = player.GetSkillSlotIndex(skillIndex)
		skillPower = player.GetSkillCurrentEfficientPercentage(slotIndex)
		if localeinfo.IsBRAZIL():
			k = skillPower
		else:
			k = player.GetSkillLevel(skillIndex) / 100.0
		self.AppendSpace(5)
		self.AutoAppendTextLine(localeinfo.TOOLTIP_PARTY_SKILL_LEVEL % skillLevel, self.NORMAL_COLOR)

		# @fixme008 BEGIN
		if skillLevel>=10:
			self.AutoAppendTextLine(comma_fix(localeinfo.PARTY_SKILL_ATTACKER) % chop( 10 + 60 * k ))

		if skillLevel>=20:
			self.AutoAppendTextLine(comma_fix(localeinfo.PARTY_SKILL_BERSERKER) 	% chop(1 + 5 * k))
			self.AutoAppendTextLine(comma_fix(localeinfo.PARTY_SKILL_TANKER) 	% chop(50 + 1450 * k))

		if skillLevel>=25:
			self.AutoAppendTextLine(comma_fix(localeinfo.PARTY_SKILL_BUFFER) % chop(5 + 45 * k ))

		if skillLevel>=35:
			self.AutoAppendTextLine(comma_fix(localeinfo.PARTY_SKILL_SKILL_MASTER) % chop(25 + 600 * k ))

		if skillLevel>=40:
			self.AutoAppendTextLine(comma_fix(localeinfo.PARTY_SKILL_DEFENDER) % chop( 5 + 30 * k ))
		# @fixme008 END

		self.AlignHorizonalCenter()

	def __AppendSummonDescription(self, skillLevel, color):
		if skillLevel > 1:
			self.AppendTextLine(localeinfo.SKILL_SUMMON_DESCRIPTION % (skillLevel * 10), color)
		elif 1 == skillLevel:
			self.AppendTextLine(localeinfo.SKILL_SUMMON_DESCRIPTION % (15), color)
		elif 0 == skillLevel:
			self.AppendTextLine(localeinfo.SKILL_SUMMON_DESCRIPTION % (10), color)

