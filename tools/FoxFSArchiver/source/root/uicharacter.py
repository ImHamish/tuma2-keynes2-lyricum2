import app
import chr
import player
import net
import ui
import uiscriptlocale
import dbg
import snd
import mousemodule
import wndMgr
import skill
import playersettingmodule
import quest
import uitooltip
import constinfo
import uicharacterdetails
import emotion
import localeinfo
import item
ENABLE_GUI_SELECT=True

SHOW_ONLY_ACTIVE_SKILL=False

HIDE_SUPPORT_SKILL_POINT=True

SHOW_LIMIT_SUPPORT_SKILL_LIST=[121, 122, 123, 124, 126, 127, 129, 128, 131, 137, 138, 139, 140]

FACE_IMAGE_DICT={
	playersettingmodule.RACE_WARRIOR_M	: "icon/face/warrior_m.tga",
	playersettingmodule.RACE_WARRIOR_W	: "icon/face/warrior_w.tga",
	playersettingmodule.RACE_ASSASSIN_M	: "icon/face/assassin_m.tga",
	playersettingmodule.RACE_ASSASSIN_W	: "icon/face/assassin_w.tga",
	playersettingmodule.RACE_SURA_M		: "icon/face/sura_m.tga",
	playersettingmodule.RACE_SURA_W		: "icon/face/sura_w.tga",
	playersettingmodule.RACE_SHAMAN_M	: "icon/face/shaman_m.tga",
	playersettingmodule.RACE_SHAMAN_W	: "icon/face/shaman_w.tga",
}

def unsigned32(n):
	return n & 0xFFFFFFFFL

class CharacterWindow(ui.ScriptWindow):
	ACTIVE_PAGE_SLOT_COUNT=8 
	SUPPORT_PAGE_SLOT_COUNT=12
	
	PAGE_SLOT_COUNT=12
	PAGE_HORSE=2
	
	SKILL_GROUP_NAME_DICT={
		playersettingmodule.JOB_WARRIOR:{1:localeinfo.SKILL_GROUP_WARRIOR_1,2:localeinfo.SKILL_GROUP_WARRIOR_2,},
		playersettingmodule.JOB_ASSASSIN:{1:localeinfo.SKILL_GROUP_ASSASSIN_1,2:localeinfo.SKILL_GROUP_ASSASSIN_2,},
		playersettingmodule.JOB_SURA:{1:localeinfo.SKILL_GROUP_SURA_1,2:localeinfo.SKILL_GROUP_SURA_2,},
		playersettingmodule.JOB_SHAMAN:{1:localeinfo.SKILL_GROUP_SHAMAN_1,2:localeinfo.SKILL_GROUP_SHAMAN_2,},
	}
	
	STAT_DESCRIPTION={
		"HTH":localeinfo.STAT_TOOLTIP_CON,
		"INT":localeinfo.STAT_TOOLTIP_INT,
		"STR":localeinfo.STAT_TOOLTIP_STR,
		"DEX":localeinfo.STAT_TOOLTIP_DEX,
	}
	
	STAT_MINUS_DESCRIPTION=localeinfo.STAT_MINUS_DESCRIPTION

	def __init__(self):
		self.isOpenedDetailsWnd=False
		self.chDetailsWnd=None

		ui.ScriptWindow.__init__(self)
		self.state="STATUS"
		self.isLoaded=0

		self.toolTipSkill=0
		self.__Initialize()
		self.__LoadWindow()

		self.statusPlusCommandDict={
			"HTH":"/stat ht",
			"INT":"/stat iq",
			"STR":"/stat st",
			"DEX":"/stat dx",
		}
		
		self.status2PlusCommandDict={
			"HTH":"/stat2 ht",
			"INT":"/stat2 iq",
			"STR":"/stat2 st",
			"DEX":"/stat2 dx",
		}
		
		self.statusMinusCommandDict={
			"HTH-":"/stat- ht",
			"INT-":"/stat- iq",
			"STR-":"/stat- st",
			"DEX-":"/stat- dx",
		}

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.refreshToolTip=0
		self.curSelectedSkillGroup=0
		self.canUseHorseSkill=-1
		self.toolTip=None
		self.toolTipJob=None
		self.toolTipAlignment=None
		self.toolTipSkill=None

		self.faceImage=None
		self.statusPlusValue=None
		self.activeSlot=None
		self.tabDict=None
		self.tabButtonDict=None
		self.pageDict=None
		self.titleBarDict=None
		self.statusPlusButtonDict=None
		self.statusMinusButtonDict=None

		self.skillPageDict=None
		
		self.questShowingStartIndex=0
		self.questScrollBar=None
		self.questSlot=None
		self.questNameList=None
		self.questLastTimeList=None
		self.questLastCountList=None
		self.skillGroupButton=()
		
		self.activeSlot=None
		self.activeSkillPointValue=None
		self.supportSkillPointValue=None
		self.skillGroupButton1=None
		self.skillGroupButton2=None

		
		self.activeSkillGroupName=None

		self.guildNameSlot=None
		self.guildNameValue=None
		self.characterNameSlot=None
		self.characterNameValue=None

		self.emotionToolTip=None
		self.soloEmotionSlot=None
		self.specialEmotionSlot=None
		self.dualEmotionSlot=None

		self.change_base=None
		self.change_base_btn=None
		self.change_conqueror=None
		self.change_conqueror_btn=None
		self.ShowToolTip=False
		self.chDetailsWnd=uicharacterdetails.CharacterDetailsUI(self)

	def OnTop(self):
		if self.chDetailsWnd:
			self.chDetailsWnd.SetTop()


	def Hide(self):
		if self.chDetailsWnd:
			self.isOpenedDetailsWnd=self.chDetailsWnd.IsShow()
			self.chDetailsWnd.Close()

		
		wndMgr.Hide(self.hWnd)

	def Show(self):
		self.__LoadWindow()
		self.__InitCharacterDetailsUIButton()
		
		ui.ScriptWindow.Show(self)
		if self.chDetailsWnd and self.isOpenedDetailsWnd:
			self.chDetailsWnd.Open()

	def __LoadScript(self, fileName):
		pyScrLoader=ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)

	def __BindObject(self):
		self.toolTip=uitooltip.ToolTip()
		self.toolTipJob=uitooltip.ToolTip()
		self.toolTipAlignment=uitooltip.ToolTip(130)

		self.faceImage=self.GetChild("Face_Image")

		faceSlot=self.GetChild("Face_Slot")
		if 949 == app.GetDefaultCodePage():
			faceSlot.SetStringEvent("MOUSE_OVER_IN", self.__ShowJobToolTip)
			faceSlot.SetStringEvent("MOUSE_OVER_OUT", self.__HideJobToolTip)

		self.statusPlusValue=self.GetChild("Status_Plus_Value")

		self.characterNameSlot=self.GetChild("Character_Name_Slot")
		self.characterNameValue=self.GetChild("Character_Name")
		self.guildNameSlot=self.GetChild("Guild_Name_Slot")
		self.guildNameValue=self.GetChild("Guild_Name")
		self.characterNameSlot.SetStringEvent("MOUSE_OVER_IN", self.__ShowAlignmentToolTip)
		self.characterNameSlot.SetStringEvent("MOUSE_OVER_OUT", self.__HideAlignmentToolTip)

		self.activeSlot=self.GetChild("Skill_Active_Slot")
		self.activeSkillPointValue=self.GetChild("Active_Skill_Point_Value")
		self.supportSkillPointValue=self.GetChild("Support_Skill_Point_Value")
		self.supportSkillPointValue.Hide()
		self.GetChild("Support_Skill_Point_Label").Hide()
		self.skillGroupButton1=self.GetChild("Skill_Group_Button_1")
		self.skillGroupButton2=self.GetChild("Skill_Group_Button_2")
		
		self.activeSkillGroupName=self.GetChild("Active_Skill_Group_Name")
		
		self.GetChild("Tab_Button_01").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_TAB_CHARACTER)
		self.GetChild("Tab_Button_01").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("Tab_Button_02").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_TAB_SKILL)
		self.GetChild("Tab_Button_02").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("Tab_Button_03").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_TAB_EMOTICON)
		self.GetChild("Tab_Button_03").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("Tab_Button_04").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_TAB_QUEST)
		self.GetChild("Tab_Button_04").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("Lv_ToolTip").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_BASE_LEVEL)
		self.GetChild("Lv_ToolTip").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("Exp_ToolTip").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_EXP)
		self.GetChild("Exp_ToolTip").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("passive_expanded_btn").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_PASSIVE)
		self.GetChild("passive_expanded_btn").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("change_base_button").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_BASE_LEVEL)
		self.GetChild("change_base_button").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("change_conqueror_button").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_CONQUEROR_LEVEL)
		self.GetChild("change_conqueror_button").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("Char_Info_Status_img").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_STAT)
		self.GetChild("Char_Info_Status_img").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("Status_Plus_Label").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_POINT)
		self.GetChild("Status_Plus_Label").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("HTH_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_IMG_CON)
		self.GetChild("HTH_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("INT_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_IMG_INT)
		self.GetChild("INT_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("STR_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_IMG_STR)
		self.GetChild("STR_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("DEX_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_IMG_DEX)
		self.GetChild("DEX_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("SUNGMA_STR_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_SUNGMA_STR)
		self.GetChild("SUNGMA_STR_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("SUNGMA_HP_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_SUNGMA_HP)
		self.GetChild("SUNGMA_HP_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("SUNGMA_MOVE_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_SUNGMA_MOVE)
		self.GetChild("SUNGMA_MOVE_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("SUNGMA_IMMUNE_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_SUNGMA_IMMUNE)
		self.GetChild("SUNGMA_IMMUNE_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("HEL_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_HP)
		self.GetChild("HEL_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("SP_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_SP)
		self.GetChild("SP_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("ATT_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_ATT)
		self.GetChild("ATT_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("DEF_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_DEF)
		self.GetChild("DEF_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("MSPD_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_MOVE_SPEED)
		self.GetChild("MSPD_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("ASPD_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_ATT_SPEED)
		self.GetChild("ASPD_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("CSPD_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_CAST_SPEED)
		self.GetChild("CSPD_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("MATT_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_MAG_ATT)
		self.GetChild("MATT_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("MDEF_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_MAG_DEF)
		self.GetChild("MDEF_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("ER_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_DODGE_PER)
		self.GetChild("ER_IMG").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("HTH_Plus").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_CON, True)
		self.GetChild("HTH_Plus").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("STR_Plus").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_STR, True)
		self.GetChild("STR_Plus").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("INT_Plus").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_INT, True)
		self.GetChild("INT_Plus").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("DEX_Plus").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_DEX, True)
		self.GetChild("DEX_Plus").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("sungma_str_plus").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_SUNGMA_STR)
		self.GetChild("sungma_str_plus").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("sungma_str_plus").Hide()
		self.GetChild("sungma_hp_plus").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_SUNGMA_HP)
		self.GetChild("sungma_hp_plus").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("sungma_hp_plus").Hide()
		self.GetChild("sungma_move_plus").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_SUNGMA_MOVE)
		self.GetChild("sungma_move_plus").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("sungma_move_plus").Hide()
		self.GetChild("sungma_immune_plus").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_SUNGMA_IMMUNE)
		self.GetChild("sungma_immune_plus").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("sungma_immune_plus").Hide()
		self.GetChild("HTH_Minus").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_CON)
		self.GetChild("HTH_Minus").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("STR_Minus").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_STR)
		self.GetChild("STR_Minus").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("INT_Minus").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_INT)
		self.GetChild("INT_Minus").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("DEX_Minus").SetShowToolTipEvent(ui.__mem_func__(self.__OverInImage), "mouse_over_in", localeinfo.STAT_TOOLTIP_DEX)
		self.GetChild("DEX_Minus").SetHideToolTipEvent(ui.__mem_func__(self.__OverOutImage), "mouse_over_out")
		self.GetChild("Active_Skill_Point_Label").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_POINT)
		self.GetChild("Active_Skill_Point_Label").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("Support_Skill_ToolTip").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_SUPPORT_SKILL)
		self.GetChild("Support_Skill_ToolTip").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("Action_Bar_Img").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_ACTION)
		self.GetChild("Action_Bar_Img").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("Reaction_Bar_Img").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_REACTION)
		self.GetChild("Reaction_Bar_Img").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.GetChild("Special_Action_Bar_Img").SetStringEventIntro(ui.__mem_func__(self.__OverInImage), "MOUSE_OVER_IN", "MOUSE_OVER_IN", localeinfo.STAT_TOOLTIP_SPECIAL_ACTION)
		self.GetChild("Special_Action_Bar_Img").SetStringEventIntro(ui.__mem_func__(self.__OverOutImage), "MOUSE_OVER_OUT", "MOUSE_OVER_OUT")
		self.change_base=self.GetChild("base_info")
		self.change_base_btn=self.GetChild("change_base_button")
		self.change_base_btn.SetEvent(ui.__mem_func__(self.SelectChange), 0)
		self.change_conqueror=self.GetChild("sungma_info")
		self.change_conqueror_btn=self.GetChild("change_conqueror_button")
		self.change_conqueror_btn.SetEvent(ui.__mem_func__(self.SelectChange), 1)

		if app.IS_KEYNES2:
			self.change_base_btn.Hide()
			self.change_conqueror_btn.Hide()
			
			self.GetChild("change_base_buttonImg").SetStringEvent("MOUSE_OVER_IN", ui.__mem_func__(self.__OverInImage), 0, localeinfo.STAT_TOOLTIP_BASE_LEVEL)
			self.GetChild("change_base_buttonImg").SetStringEvent("MOUSE_OVER_OUT", ui.__mem_func__(self.__OverOutImage), 0)
		else:
			self.GetChild("change_base_buttonImg").Hide()

		self.tabDict={
			"STATUS"	: self.GetChild("Tab_01"),
			"SKILL"		: self.GetChild("Tab_02"),
			"EMOTICON"	: self.GetChild("Tab_03"),
			"QUEST"		: self.GetChild("Tab_04"),
		}

		self.tabButtonDict={
			"STATUS"	: self.GetChild("Tab_Button_01"),
			"SKILL"		: self.GetChild("Tab_Button_02"),
			"EMOTICON"	: self.GetChild("Tab_Button_03"),
			"QUEST"		: self.GetChild("Tab_Button_04")
		}

		self.pageDict={
			"STATUS"	: self.GetChild("Character_Page"),
			"SKILL"		: self.GetChild("Skill_Page"),
			"EMOTICON"	: self.GetChild("Emoticon_Page"),
			"QUEST"		: self.GetChild("Quest_Page")
		}

		self.titleBarDict={
			"STATUS"	: self.GetChild("Character_TitleBar"),
			"SKILL"		: self.GetChild("Skill_TitleBar"),
			"EMOTICON"	: self.GetChild("Emoticon_TitleBar"),
			"QUEST"		: self.GetChild("Quest_TitleBar")
		}

		self.statusPlusButtonDict={
			"HTH"		: self.GetChild("HTH_Plus"),
			"INT"		: self.GetChild("INT_Plus"),
			"STR"		: self.GetChild("STR_Plus"),
			"DEX"		: self.GetChild("DEX_Plus"),
		}

		self.statusMinusButtonDict={
			"HTH-"		: self.GetChild("HTH_Minus"),
			"INT-"		: self.GetChild("INT_Minus"),
			"STR-"		: self.GetChild("STR_Minus"),
			"DEX-"		: self.GetChild("DEX_Minus"),
		}

		self.skillPageDict={
			"ACTIVE":self.GetChild("Skill_Active_Slot"),
			"SUPPORT":self.GetChild("Skill_ETC_Slot"),
			"HORSE":self.GetChild("Skill_Active_Slot"),
		}

		self.skillPageStatDict={
			"SUPPORT"	: player.SKILL_SUPPORT,
			"ACTIVE"	: player.SKILL_ACTIVE,
			"HORSE"		: player.SKILL_HORSE,
		}

		self.skillGroupButton=(
			self.GetChild("Skill_Group_Button_1"),
			self.GetChild("Skill_Group_Button_2"),
		)

		self.soloEmotionSlot=self.GetChild("SoloEmotionSlot")
		self.specialEmotionSlot=self.GetChild("SpecialEmotionSlot")
		self.dualEmotionSlot=self.GetChild("DualEmotionSlot")
		self.__SetEmotionSlot()
		
		self.questShowingStartIndex=0
		self.questScrollBar=self.GetChild("Quest_ScrollBar")
		self.questScrollBar.SetScrollEvent(ui.__mem_func__(self.OnQuestScroll))
		self.questSlot=self.GetChild("Quest_Slot")
		for i in xrange(quest.QUEST_MAX_NUM):
			self.questSlot.HideSlotBaseImage(i)
			self.questSlot.SetCoverButton(i,\
											"d:/ymir work/ui/game/quest/slot_button_01.sub",\
											"d:/ymir work/ui/game/quest/slot_button_02.sub",\
											"d:/ymir work/ui/game/quest/slot_button_03.sub",\
											"d:/ymir work/ui/game/quest/slot_button_03.sub", True)
		
		self.questNameList=[]
		self.questLastTimeList=[]
		self.questLastCountList=[]
		for i in xrange(quest.QUEST_MAX_NUM):
			self.questNameList.append(self.GetChild("Quest_Name_0" + str(i)))
			self.questLastTimeList.append(self.GetChild("Quest_LastTime_0" + str(i)))
			self.questLastCountList.append(self.GetChild("Quest_LastCount_0" + str(i)))
		
		self.MainBoard=self.GetChild("board")
		self.ExpandBtn=ui.MakeButton(self.MainBoard, 240, 157, "", "d:/ymir work/ui/game/belt_inventory/", "btn_minimize_normal.tga", "btn_minimize_over.tga", "btn_minimize_down.tga")
		self.ExpandBtn.SetEvent(ui.__mem_func__(self.__ClickExpandButton))
		self.MinimizeBtn=ui.MakeButton(self.MainBoard, 240, 157, "", "d:/ymir work/ui/game/belt_inventory/", "btn_expand_normal.tga", "btn_expand_over.tga", "btn_expand_down.tga")
		self.MinimizeBtn.SetEvent(ui.__mem_func__(self.__ClickMinimizeButton))
		
		self.GetChild("passive_expanded_btn").Hide()
		
		self.SelectChange(0)

	def SelectChange(self, arg):
		if arg == 0:
			self.change_base_btn.Down()
			self.change_conqueror_btn.SetUp()
			self.change_base.Show()
			self.change_conqueror.Hide()
		else:
			self.change_conqueror_btn.Down()
			self.change_base_btn.SetUp()
			self.change_conqueror.Show()
			self.change_base.Hide()
		
		self.RefreshStatus()
		self.__RefreshStatusPlusButtonList()

	def __OverInImage(self, act, arg, statPlus=False):
		arglen=len(str(arg))
		pos_x, pos_y=wndMgr.GetMousePosition()
		
		self.toolTip.ClearToolTip()
		self.toolTip.SetThinBoardSize(11 * arglen)
		self.toolTip.SetToolTipPosition(pos_x + 50, pos_y + 50)
		self.toolTip.AppendTextLine(arg, self.toolTip.FONT_COLOR)
		if statPlus:
			self.toolTip.AppendTextLine(localeinfo.INFO_INCREASE_CHAR_STAT)
		
		self.toolTip.Show()
		self.ShowToolTip=True

	def __OverOutImage(self, act):
		self.toolTip.Hide()
		self.ShowToolTip=False

	def ToolTipProgress(self) :
		if self.ShowToolTip:
			pos_x, pos_y=wndMgr.GetMousePosition()
			self.toolTip.SetToolTipPosition(pos_x + 50, pos_y + 50)

	def __InitCharacterDetailsUIButton(self):
		self.ExpandBtn.Show()
		self.MinimizeBtn.Hide()

	def __ClickExpandButton(self):
		if self.chDetailsWnd:
			(x, y)=self.GetLocalPosition()
			self.chDetailsWnd.Open()
			self.chDetailsWnd.AdjustPosition(x, y)
		
		self.ExpandBtn.Hide()
		self.MinimizeBtn.Show()

	def __ClickMinimizeButton(self):
		if self.chDetailsWnd:
			self.chDetailsWnd.Close()
		self.MinimizeBtn.Hide()
		self.ExpandBtn.Show()

	def AdjustPosition(self, x, y, width):
		self.SetPosition(x - width, y)

	def OnMoveWindow(self, x, y):
		#print "OnMoveWindow x %s y %s" % (x, y)
		if self.chDetailsWnd:
			self.chDetailsWnd.AdjustPosition(x, y)

	def __SetSkillSlotEvent(self):
		for skillPageValue in self.skillPageDict.itervalues():
			skillPageValue.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			skillPageValue.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectSkill))
			skillPageValue.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			skillPageValue.SetUnselectItemSlotEvent(ui.__mem_func__(self.ClickSkillSlot))
			skillPageValue.SetUseSlotEvent(ui.__mem_func__(self.ClickSkillSlot))
			skillPageValue.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			skillPageValue.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			skillPageValue.SetPressedSlotButtonEvent(ui.__mem_func__(self.OnPressedSlotButton))
			skillPageValue.AppendSlotButton("d:/ymir work/ui/game/windows/btn_plus_up.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_over.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_down.sub")

	def __SetEmotionSlot(self):
		self.emotionToolTip=uitooltip.ToolTip()
		for slot in (self.soloEmotionSlot, self.dualEmotionSlot, self.specialEmotionSlot):
			slot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			slot.SetSelectItemSlotEvent(ui.__mem_func__(self.__SelectEmotion))
			slot.SetUnselectItemSlotEvent(ui.__mem_func__(self.__ClickEmotionSlot))
			slot.SetUseSlotEvent(ui.__mem_func__(self.__ClickEmotionSlot))
			slot.SetOverInItemEvent(ui.__mem_func__(self.__OverInEmotion))
			slot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutEmotion))
			slot.AppendSlotButton("d:/ymir work/ui/game/windows/btn_plus_up.sub", "d:/ymir work/ui/game/windows/btn_plus_over.sub", "d:/ymir work/ui/game/windows/btn_plus_down.sub")

		for slotIdx, datadict in emotion.EMOTION_DICT.items():
			emotionIdx=slotIdx

			slot=self.soloEmotionSlot
			if slotIdx > 50:
				slot=self.dualEmotionSlot
			elif slotIdx >= 19:
				slot=self.specialEmotionSlot
			
			slot.SetEmotionSlot(slotIdx, emotionIdx)
			slot.SetCoverButton(slotIdx)

	def __SelectEmotion(self, slotIndex):
		if not slotIndex in emotion.EMOTION_DICT:
			return

		if app.IsPressed(app.DIK_LCONTROL):
			player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_EMOTION, slotIndex)
			return

		mousemodule.mouseController.AttachObject(self, player.SLOT_TYPE_EMOTION, slotIndex, slotIndex)

	def __ClickEmotionSlot(self, slotIndex):
		print "click emotion"
		if not slotIndex in emotion.EMOTION_DICT:
			return

		print "check acting"
		if player.IsActingEmotion():
			return

		command=emotion.EMOTION_DICT[slotIndex]["command"]
		print "command", command

		if slotIndex > 50:
			vid=player.GetTargetVID()

			if 0 == vid or vid == player.GetMainCharacterIndex() or chr.IsNPC(vid) or chr.IsEnemy(vid):
				import chat
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeinfo.EMOTION_CHOOSE_ONE)
				return

			command += " " + chr.GetNameByVID(vid)

		print "send_command", command
		net.SendChatPacket(command)

	def ActEmotion(self, emotionIndex):
		self.__ClickEmotionSlot(emotionIndex)

	def __OverInEmotion(self, slotIndex):
		if self.toolTip == None:
			self.Close()
			
		if self.emotionToolTip:

			if not slotIndex in emotion.EMOTION_DICT:
				return

			self.emotionToolTip.ClearToolTip()
			self.emotionToolTip.SetTitle(emotion.EMOTION_DICT[slotIndex]["name"])
			self.emotionToolTip.AlignHorizonalCenter()
			self.emotionToolTip.ShowToolTip()

	def __OverOutEmotion(self):
		if self.emotionToolTip:
			self.emotionToolTip.HideToolTip()

	def __BindEvent(self):
		for i in xrange(len(self.skillGroupButton)):
			self.skillGroupButton[i].SetEvent(lambda arg=i: self.__SelectSkillGroup(arg))
		
		self.RefreshQuest()
		self.__HideJobToolTip()

		for (tabKey, tabButton) in self.tabButtonDict.items():
			tabButton.SetEvent(ui.__mem_func__(self.__OnClickTabButton), tabKey)

		for (statusPlusKey, statusPlusButton) in self.statusPlusButtonDict.items():
			statusPlusButton.SAFE_SetEvent(self.__OnClickStatusPlusButton, statusPlusKey)
		#	statusPlusButton.ShowToolTip=lambda arg=statusPlusKey: self.__OverInStatButton(arg)
		#	statusPlusButton.HideToolTip=lambda arg=statusPlusKey: self.__OverOutStatButton()

		for (statusMinusKey, statusMinusButton) in self.statusMinusButtonDict.items():
			statusMinusButton.SAFE_SetEvent(self.__OnClickStatusMinusButton, statusMinusKey)
		#	statusMinusButton.ShowToolTip=lambda arg=statusMinusKey: self.__OverInStatMinusButton(arg)
		#	statusMinusButton.HideToolTip=lambda arg=statusMinusKey: self.__OverOutStatMinusButton()

		for titleBarValue in self.titleBarDict.itervalues():
			titleBarValue.SetCloseEvent(ui.__mem_func__(self.Close))
		
		self.questSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.__SelectQuest))

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded=1

		try:
			self.__LoadScript("uiscript/characterwindow.py")
			self.__BindObject()
			self.__BindEvent()
		except:
			import exception
			exception.Abort("CharacterWindow.__LoadWindow")

		#self.tabButtonDict["EMOTICON"].Disable()
		self.SetState("STATUS")

	def Destroy(self):
		if self.chDetailsWnd:
			self.chDetailsWnd.Close()
			del self.chDetailsWnd
			self.chDetailsWnd=None
		self.ClearDictionary()

		self.__Initialize()


	def Close(self):
		if 0 != self.toolTipSkill and self.toolTipSkill != None:
			self.toolTipSkill.HideToolTip()
		if self.chDetailsWnd and self.chDetailsWnd.IsShow():
			self.chDetailsWnd.Close()

		self.Hide()

	def SetSkillToolTip(self, toolTipSkill):
		self.toolTipSkill=toolTipSkill

	def __OnClickStatusPlusButton(self, statusKey):
		try:
			if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
				statusPlusCommand=self.status2PlusCommandDict[statusKey]
				net.SendChatPacket(statusPlusCommand)
			else:
				statusPlusCommand=self.statusPlusCommandDict[statusKey]
				net.SendChatPacket(statusPlusCommand)
		except KeyError, msg:
			dbg.TraceError("CharacterWindow.__OnClickStatusPlusButton KeyError: %s", msg)

	def __OnClickStatusMinusButton(self, statusKey):
		try:
			statusMinusCommand=self.statusMinusCommandDict[statusKey]
			net.SendChatPacket(statusMinusCommand)
		except KeyError, msg:
			dbg.TraceError("CharacterWindow.__OnClickStatusMinusButton KeyError: %s", msg)


	def __OnClickTabButton(self, stateKey):
		self.SetState(stateKey)

	def SetState(self, stateKey):
		self.state=stateKey
		
		for (tabKey, tabButton) in self.tabButtonDict.items():
			if stateKey!=tabKey:
				tabButton.SetUp()

		for tabValue in self.tabDict.itervalues():
			tabValue.Hide()

		for pageValue in self.pageDict.itervalues():
			pageValue.Hide()

		for titleBarValue in self.titleBarDict.itervalues():
			titleBarValue.Hide()

		self.titleBarDict[stateKey].Show()
		self.tabDict[stateKey].Show()
		self.pageDict[stateKey].Show()


	def GetState(self):
		return self.state

	def __GetTotalAtkText(self):
		minAtk=player.GetStatus(player.ATT_MIN)
		maxAtk=player.GetStatus(player.ATT_MAX)
		atkBonus=player.GetStatus(player.ATT_BONUS)
		attackerBonus=player.GetStatus(player.ATTACKER_BONUS)

		if minAtk==maxAtk:
			return "%d" % (minAtk+atkBonus+attackerBonus)
		else:
			return "%d-%d" % (minAtk+atkBonus+attackerBonus, maxAtk+atkBonus+attackerBonus)

	def __GetTotalMagAtkText(self):
		minMagAtk=player.GetStatus(player.MAG_ATT)+player.GetStatus(player.MIN_MAGIC_WEP)
		maxMagAtk=player.GetStatus(player.MAG_ATT)+player.GetStatus(player.MAX_MAGIC_WEP)

		if minMagAtk==maxMagAtk:
			return "%d" % (minMagAtk)
		else:
			return "%d-%d" % (minMagAtk, maxMagAtk)

	def __GetTotalDefText(self):
		defValue=player.GetStatus(player.DEF_GRADE)
		if constinfo.ADD_DEF_BONUS_ENABLE:
			defValue+=player.GetStatus(player.DEF_BONUS)
		return "%d" % (defValue)

	def RefreshStatus(self):
		if self.isLoaded==0:
			return

		try:
			self.GetChild("HP_Value").SetText(str(player.GetStatus(player.HP)) + '/' + str(player.GetStatus(player.MAX_HP)))
			self.GetChild("SP_Value").SetText(str(player.GetStatus(player.SP)) + '/' + str(player.GetStatus(player.MAX_SP)))

			if self.change_conqueror_btn.IsDown():
				self.GetChild("Level_Value").SetText("0")
				self.GetChild("Exp_Value").SetText("0")
				self.GetChild("RestExp_Value").SetText("0")
				self.GetChild("sungma_str_value").SetText("0")
				self.GetChild("sungma_hp_value").SetText("0")
				self.GetChild("sungma_move_value").SetText("0")
				self.GetChild("sungma_immune_value").SetText("0")
			else:
				self.GetChild("Level_Value").SetText(str(player.GetStatus(player.LEVEL)))
				self.GetChild("Exp_Value").SetText(str(unsigned32(player.GetEXP())))
				self.GetChild("RestExp_Value").SetText(str(unsigned32(player.GetStatus(player.NEXT_EXP)) - unsigned32(player.GetStatus(player.EXP))))
				self.GetChild("STR_Value").SetText(str(player.GetStatus(player.ST)))
				self.GetChild("DEX_Value").SetText(str(player.GetStatus(player.DX)))
				self.GetChild("HTH_Value").SetText(str(player.GetStatus(player.HT)))
				self.GetChild("INT_Value").SetText(str(player.GetStatus(player.IQ)))

			self.GetChild("ATT_Value").SetText(self.__GetTotalAtkText())
			self.GetChild("DEF_Value").SetText(self.__GetTotalDefText())

			self.GetChild("MATT_Value").SetText(self.__GetTotalMagAtkText())
			#self.GetChild("MATT_Value").SetText(str(player.GetStatus(player.MAG_ATT)))

			self.GetChild("MDEF_Value").SetText(str(player.GetStatus(player.MAG_DEF)))
			self.GetChild("ASPD_Value").SetText(str(player.GetStatus(player.ATT_SPEED)))
			self.GetChild("MSPD_Value").SetText(str(player.GetStatus(player.MOVING_SPEED)))
			self.GetChild("CSPD_Value").SetText(str(player.GetStatus(player.CASTING_SPEED)))
			self.GetChild("ER_Value").SetText(str(player.GetStatus(player.EVADE_RATE)))

		except:
			#import exception
			#exception.Abort("CharacterWindow.RefreshStatus.BindObject")
			pass

		self.__RefreshStatusPlusButtonList()
		self.__RefreshStatusMinusButtonList()
		self.RefreshAlignment()

		if self.refreshToolTip:
			self.refreshToolTip()
		
		if self.chDetailsWnd and self.chDetailsWnd.IsShow():
			self.chDetailsWnd.RefreshLabel()

	def __RefreshStatusPlusButtonList(self):
		if self.isLoaded == 0:
			return
		
		if self.change_conqueror_btn.IsDown():
			statusPlusPoint=0
		else:
			statusPlusPoint=player.GetStatus(player.STAT)
		
		if statusPlusPoint > 0:
			self.statusPlusValue.SetText(str(statusPlusPoint))
			self.ShowStatusPlusButtonList()
		else:
			self.statusPlusValue.SetText("0")
			self.HideStatusPlusButtonList()

	def __RefreshStatusMinusButtonList(self):
		if self.isLoaded==0:
			return

		statusMinusPoint=self.__GetStatMinusPoint()

		if statusMinusPoint>0:
			self.__ShowStatusMinusButtonList()
		else:
			self.__HideStatusMinusButtonList()

	def RefreshAlignment(self):
		point, grade=player.GetAlignmentData()

		import colorinfo
		COLOR_DICT={	0:colorinfo.TITLE_RGB_GOOD_4,
						1:colorinfo.TITLE_RGB_GOOD_3,
						2:colorinfo.TITLE_RGB_GOOD_2,
						3:colorinfo.TITLE_RGB_GOOD_1,
						4:colorinfo.TITLE_RGB_NORMAL,
						5:colorinfo.TITLE_RGB_EVIL_1,
						6:colorinfo.TITLE_RGB_EVIL_2,
						7:colorinfo.TITLE_RGB_EVIL_3,
						8:colorinfo.TITLE_RGB_EVIL_4, }
		colorList=COLOR_DICT.get(grade, colorinfo.TITLE_RGB_NORMAL)
		gradeColor=ui.GenerateColor(colorList[0], colorList[1], colorList[2])

		self.toolTipAlignment.ClearToolTip()
		self.toolTipAlignment.AutoAppendTextLine(localeinfo.TITLE_NAME_LIST[grade], gradeColor)
		self.toolTipAlignment.AutoAppendTextLine(localeinfo.ALIGNMENT_NAME + str(point))

		if app.ENABLE_AFFECT_ALIGNMENT:
			if grade == 0 and point == 20000:
				self.toolTipAlignment.AutoAppendTextLine(uitooltip.GET_AFFECT_STRING(item.APPLY_ATTBONUS_MONSTER, 25))
			elif grade == 8 and point <= -15000:
				self.toolTipAlignment.AutoAppendTextLine(uitooltip.GET_AFFECT_STRING(item.APPLY_ATTBONUS_HUMAN, 10))
				self.toolTipAlignment.AutoAppendTextLine(uitooltip.GET_AFFECT_STRING(item.APPLY_MAX_HP, 1000))

		# import chat
		# chat.AppendChat(7, "grade {} point {}".format(grade, point))

		self.toolTipAlignment.AlignHorizonalCenter()

	def __ShowStatusMinusButtonList(self):
		for (stateMinusKey, statusMinusButton) in self.statusMinusButtonDict.items():
			statusMinusButton.Show()

	def __HideStatusMinusButtonList(self):
		for (stateMinusKey, statusMinusButton) in self.statusMinusButtonDict.items():
			statusMinusButton.Hide()

	def ShowStatusPlusButtonList(self):
		for (statePlusKey, statusPlusButton) in self.statusPlusButtonDict.items():
			statusPlusButton.Show()

	def HideStatusPlusButtonList(self):
		for (statePlusKey, statusPlusButton) in self.statusPlusButtonDict.items():
			statusPlusButton.Hide()

	def SelectSkill(self, skillSlotIndex):


		mouseController=mousemodule.mouseController

		if False == mouseController.isAttached():

			srcSlotIndex=self.__RealSkillSlotToSourceSlot(skillSlotIndex)
			selectedSkillIndex=player.GetSkillIndex(srcSlotIndex)

			if skill.CanUseSkill(selectedSkillIndex):

				if app.IsPressed(app.DIK_LCONTROL):

					player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_SKILL, srcSlotIndex)
					return

				mouseController.AttachObject(self, player.SLOT_TYPE_SKILL, srcSlotIndex, selectedSkillIndex)

		else:

			mouseController.DeattachObject()

	def SelectEmptySlot(self, SlotIndex):
		mousemodule.mouseController.DeattachObject()

	## ToolTip
	def OverInItem(self, slotNumber):

		if mousemodule.mouseController.isAttached():
			return

		if 0 == self.toolTipSkill or self.toolTipSkill == None:
			return

		srcSlotIndex=self.__RealSkillSlotToSourceSlot(slotNumber)
		
		skillIndex=player.GetSkillIndex(srcSlotIndex)
		skillLevel=player.GetSkillLevel(srcSlotIndex)
		skillGrade=player.GetSkillGrade(srcSlotIndex)
		skillType=skill.GetSkillType(skillIndex)

		## ACTIVE
		if skill.SKILL_TYPE_ACTIVE == skillType:
			overInSkillGrade=self.__GetSkillGradeFromSlot(slotNumber)
			if overInSkillGrade == skill.SKILL_GRADE_COUNT-1 and skillGrade == skill.SKILL_GRADE_COUNT:
				self.toolTipSkill.SetSkillNew(srcSlotIndex, skillIndex, skillGrade, skillLevel)
			elif overInSkillGrade == skillGrade:
				self.toolTipSkill.SetSkillNew(srcSlotIndex, skillIndex, overInSkillGrade, skillLevel)
			else:
				self.toolTipSkill.SetSkillOnlyName(srcSlotIndex, skillIndex, overInSkillGrade)

		else:
			self.toolTipSkill.SetSkillNew(srcSlotIndex, skillIndex, skillGrade, skillLevel)

	def OverOutItem(self):
		if self.toolTipSkill == None:
			self.Close()
		if 0 != self.toolTipSkill and self.toolTipSkill != None :
			self.toolTipSkill.HideToolTip()

	## Quest
	def __SelectQuest(self, slotIndex):
		if self.toolTip == None:
			self.Close()
		
		questIndex=quest.GetQuestIndex(self.questShowingStartIndex + slotIndex)
		
		import event
		event.QuestButtonClick(-2147483648 + questIndex)

	def RefreshQuest(self):
		if self.toolTip == None:
			self.Close()
		
		if self.isLoaded == 0:
			return
		
		questCount=quest.GetQuestCount()
		questRange=range(quest.QUEST_MAX_NUM)

		if questCount > quest.QUEST_MAX_NUM:
			self.questScrollBar.Show()
		else:
			self.questScrollBar.Hide()

		for i in questRange[:questCount]:
			(questName, questIcon, questCounterName, questCounterValue)=quest.GetQuestData(self.questShowingStartIndex+i)
			self.questNameList[i].SetText(questName)
			self.questNameList[i].Show()
			self.questLastCountList[i].Show()
			self.questLastTimeList[i].Show()

			if len(questCounterName) > 0:
				self.questLastCountList[i].SetText("%s:%d" % (questCounterName, questCounterValue))
			else:
				self.questLastCountList[i].SetText("")

			## Icon
			self.questSlot.SetSlot(i, i, 1, 1, questIcon)

		for i in questRange[questCount:]:
			self.questNameList[i].Hide()
			self.questLastTimeList[i].Hide()
			self.questLastCountList[i].Hide()
			self.questSlot.ClearSlot(i)
			self.questSlot.HideSlotBaseImage(i)

		self.__UpdateQuestClock()

	def __UpdateQuestClock(self):
		if self.toolTip == None:
			self.Close()
		if "QUEST" == self.state:
			for i in xrange(min(quest.GetQuestCount(), quest.QUEST_MAX_NUM)):
				(lastName, lastTime)=quest.GetQuestLastTime(i)
				clockText=localeinfo.QUEST_UNLIMITED_TIME
				
				if len(lastName) > 0:
					if lastTime <= 0:
						clockText=localeinfo.QUEST_TIMEOVER
					else:
						questLastMinute=lastTime / 60
						questLastSecond=lastTime % 60
						clockText=lastName + ":"
						if questLastMinute > 0:
							clockText += str(questLastMinute) + localeinfo.QUEST_MIN
							if questLastSecond > 0:
								clockText += " "
						
						if questLastSecond > 0:
							clockText += str(questLastSecond) + localeinfo.QUEST_SEC
				
				self.questLastTimeList[i].SetText(clockText)

	def __GetStatMinusPoint(self):
		POINT_STAT_RESET_COUNT=112
		return player.GetStatus(POINT_STAT_RESET_COUNT)

	def __OverInStatMinusButton(self, stat):
		try:
			self.__ShowStatToolTip(self.STAT_MINUS_DESCRIPTION[stat] % self.__GetStatMinusPoint())
		except KeyError:
			pass

		self.refreshToolTip=lambda arg=stat: self.__OverInStatMinusButton(arg)

	def __OverOutStatMinusButton(self):
		self.__HideStatToolTip()
		self.refreshToolTip=0

	def __OverInStatButton(self, stat):
		try:
			self.__ShowStatToolTip(self.STAT_DESCRIPTION[stat])
		except KeyError:
			pass

	def __OverOutStatButton(self):
		self.__HideStatToolTip()

	def __ShowStatToolTip(self, statDesc):
		if self.toolTip == None:
			self.Close()
		if self.toolTip != None :
			self.toolTip.ClearToolTip()
			self.toolTip.AppendTextLine(localeinfo.INFO_INCREASE_CHAR_STAT)
			self.toolTip.AppendTextLine(statDesc)
			self.toolTip.Show()

	def __HideStatToolTip(self):
		if self.toolTip == None:
			self.Close()
		if self.toolTip != None :
			self.toolTip.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnUpdate(self):
		self.ToolTipProgress()
		self.__UpdateQuestClock()

	## Skill Process
	def __RefreshSkillPage(self, name, slotCount):
		global SHOW_LIMIT_SUPPORT_SKILL_LIST
		skillPage=self.skillPageDict[name]
		
		startSlotIndex=skillPage.GetStartIndex()
		if "ACTIVE" == name:
			if self.PAGE_HORSE == self.curSelectedSkillGroup:
				startSlotIndex += slotCount

		getSkillType=skill.GetSkillType
		getSkillIndex=player.GetSkillIndex
		getSkillGrade=player.GetSkillGrade
		getSkillLevel=player.GetSkillLevel
		getSkillLevelUpPoint=skill.GetSkillLevelUpPoint
		getSkillMaxLevel=skill.GetSkillMaxLevel
		for i in xrange(slotCount+1):
			slotIndex=i + startSlotIndex
			skillIndex=getSkillIndex(slotIndex)
			
			for j in xrange(skill.SKILL_GRADE_COUNT):
				skillPage.ClearSlot(self.__GetRealSkillSlot(j, i))
			
			if 0 == skillIndex:
				continue
			
			skillGrade=getSkillGrade(slotIndex)
			skillLevel=getSkillLevel(slotIndex)
			skillType=getSkillType(skillIndex)
			
			if player.SKILL_INDEX_RIDING == skillIndex:
				if 1 == skillGrade:
					skillLevel += 19
				elif 2 == skillGrade:
					skillLevel += 29
				elif 3 == skillGrade:
					skillLevel=40
				
				skillPage.SetSkillSlotNew(slotIndex, skillIndex, max(skillLevel-1, 0), skillLevel)
				skillPage.SetSlotCount(slotIndex, skillLevel)
			elif skill.SKILL_TYPE_ACTIVE == skillType:
				for j in xrange(skill.SKILL_GRADE_COUNT):
					realSlotIndex=self.__GetRealSkillSlot(j, slotIndex)
					skillPage.SetSkillSlotNew(realSlotIndex, skillIndex, j, skillLevel)
					skillPage.SetCoverButton(realSlotIndex)
					
					if (skillGrade == skill.SKILL_GRADE_COUNT) and j == (skill.SKILL_GRADE_COUNT-1):
						skillPage.SetSlotCountNew(realSlotIndex, skillGrade, skillLevel)
					elif (not self.__CanUseSkillNow()) or (skillGrade != j):
						skillPage.SetSlotCount(realSlotIndex, 0)
						skillPage.DisableCoverButton(realSlotIndex)
					else:
						skillPage.SetSlotCountNew(realSlotIndex, skillGrade, skillLevel)
			else:
				if not SHOW_LIMIT_SUPPORT_SKILL_LIST or skillIndex in SHOW_LIMIT_SUPPORT_SKILL_LIST:
					realSlotIndex=self.__GetETCSkillRealSlotIndex(slotIndex)
					skillPage.SetSkillSlot(realSlotIndex, skillIndex, skillLevel)
					skillPage.SetSlotCountNew(realSlotIndex, skillGrade, skillLevel)
					if skill.CanUseSkill(skillIndex):
						skillPage.SetCoverButton(realSlotIndex)
			
			skillPage.RefreshSlot()
  
	def RefreshSkill(self):

		if self.isLoaded==0:
			return

		if self.__IsChangedHorseRidingSkillLevel():
			self.RefreshCharacter()
			return


		global SHOW_ONLY_ACTIVE_SKILL
		if SHOW_ONLY_ACTIVE_SKILL:
			self.__RefreshSkillPage("ACTIVE", self.ACTIVE_PAGE_SLOT_COUNT)
		else:
			self.__RefreshSkillPage("ACTIVE", self.ACTIVE_PAGE_SLOT_COUNT)
			self.__RefreshSkillPage("SUPPORT", self.SUPPORT_PAGE_SLOT_COUNT)

		self.RefreshSkillPlusButtonList()

	def CanShowPlusButton(self, skillIndex, skillLevel, curStatPoint):

		if 0 == skillIndex:
			return False

		if not skill.CanLevelUpSkill(skillIndex, skillLevel):
			return False

		return True

	def __RefreshSkillPlusButton(self, name):
		global HIDE_SUPPORT_SKILL_POINT
		if HIDE_SUPPORT_SKILL_POINT and "SUPPORT" == name:
			return

		slotWindow=self.skillPageDict[name]
		slotWindow.HideAllSlotButton()

		slotStatType=self.skillPageStatDict[name]
		if 0 == slotStatType:
			return

		statPoint=player.GetStatus(slotStatType)
		startSlotIndex=slotWindow.GetStartIndex()
		if "HORSE" == name:
			startSlotIndex += self.ACTIVE_PAGE_SLOT_COUNT

		if statPoint > 0:
			for i in xrange(self.PAGE_SLOT_COUNT):
				slotIndex=i + startSlotIndex
				skillIndex=player.GetSkillIndex(slotIndex)
				skillGrade=player.GetSkillGrade(slotIndex)
				skillLevel=player.GetSkillLevel(slotIndex)

				if skillIndex == 0:
					continue
				if skillGrade != 0:
					continue

				if name == "HORSE":
					if player.GetStatus(player.LEVEL) >= skill.GetSkillLevelLimit(skillIndex):
						if skillLevel < 20:
							slotWindow.ShowSlotButton(self.__GetETCSkillRealSlotIndex(slotIndex))

				else:
					if "SUPPORT" == name:
						if not SHOW_LIMIT_SUPPORT_SKILL_LIST or skillIndex in SHOW_LIMIT_SUPPORT_SKILL_LIST:
							if self.CanShowPlusButton(skillIndex, skillLevel, statPoint):
								slotWindow.ShowSlotButton(slotIndex)
					else:
						if self.CanShowPlusButton(skillIndex, skillLevel, statPoint):
							slotWindow.ShowSlotButton(slotIndex)


	def RefreshSkillPlusButtonList(self):

		if self.isLoaded==0:
			return

		self.RefreshSkillPlusPointLabel()

		if not self.__CanUseSkillNow():
			return

		try:
			if self.PAGE_HORSE == self.curSelectedSkillGroup:
				self.__RefreshSkillPlusButton("HORSE")
			else:
				self.__RefreshSkillPlusButton("ACTIVE")

			self.__RefreshSkillPlusButton("SUPPORT")

		except:
			import exception
			exception.Abort("CharacterWindow.RefreshSkillPlusButtonList.BindObject")

	def RefreshSkillPlusPointLabel(self):
		if self.isLoaded==0:
			return

		if self.PAGE_HORSE == self.curSelectedSkillGroup:
			activeStatPoint=player.GetStatus(player.SKILL_HORSE)
			self.activeSkillPointValue.SetText(uiscriptlocale.UICHAR_STATUS_VALUE % (activeStatPoint))

		else:
			activeStatPoint=player.GetStatus(player.SKILL_ACTIVE)
			self.activeSkillPointValue.SetText(uiscriptlocale.UICHAR_STATUS_VALUE % (activeStatPoint))

		supportStatPoint=max(0, player.GetStatus(player.SKILL_SUPPORT))
		self.supportSkillPointValue.SetText(uiscriptlocale.UICHAR_STATUS_VALUE % (supportStatPoint))

	## Skill Level Up Button
	def OnPressedSlotButton(self, slotNumber):
		srcSlotIndex=self.__RealSkillSlotToSourceSlot(slotNumber)

		skillIndex=player.GetSkillIndex(srcSlotIndex)
		curLevel=player.GetSkillLevel(srcSlotIndex)
		maxLevel=skill.GetSkillMaxLevel(skillIndex)

		net.SendChatPacket("/skillup " + str(skillIndex))

	## Use Skill
	def ClickSkillSlot(self, slotIndex):

		srcSlotIndex=self.__RealSkillSlotToSourceSlot(slotIndex)
		skillIndex=player.GetSkillIndex(srcSlotIndex)
		skillType=skill.GetSkillType(skillIndex)
		if not self.__CanUseSkillNow():
			if skill.SKILL_TYPE_ACTIVE == skillType:
				return

		for slotWindow in self.skillPageDict.values():
			if slotWindow.HasSlot(slotIndex):
				if skill.CanUseSkill(skillIndex):
					player.ClickSkillSlot(srcSlotIndex)
					return

		mousemodule.mouseController.DeattachObject()

	def OnUseSkill(self, slotIndex, coolTime):

		skillIndex=player.GetSkillIndex(slotIndex)
		skillType=skill.GetSkillType(skillIndex)

		## ACTIVE
		if skill.SKILL_TYPE_ACTIVE == skillType:
			skillGrade=player.GetSkillGrade(slotIndex)
			slotIndex=self.__GetRealSkillSlot(skillGrade, slotIndex)
		## ETC
		else:
			slotIndex=self.__GetETCSkillRealSlotIndex(slotIndex)

		for slotWindow in self.skillPageDict.values():
			if slotWindow.HasSlot(slotIndex):
				slotWindow.SetSlotCoolTime(slotIndex, coolTime)
				return

	def OnActivateSkill(self, slotIndex):

		skillGrade=player.GetSkillGrade(slotIndex)
		slotIndex=self.__GetRealSkillSlot(skillGrade, slotIndex)

		for slotWindow in self.skillPageDict.values():
			if slotWindow.HasSlot(slotIndex):
				slotWindow.ActivateSlot(slotIndex)
				return

	def OnDeactivateSkill(self, slotIndex):

		skillGrade=player.GetSkillGrade(slotIndex)
		slotIndex=self.__GetRealSkillSlot(skillGrade, slotIndex)

		for slotWindow in self.skillPageDict.values():
			if slotWindow.HasSlot(slotIndex):
				slotWindow.DeactivateSlot(slotIndex)
				return

	def __ShowJobToolTip(self):
		self.toolTipJob.ShowToolTip()

	def __HideJobToolTip(self):
		self.toolTipJob.HideToolTip()

	def __SetJobText(self, mainJob, subJob):
		if player.GetStatus(player.LEVEL)<5:
			subJob=0

	def __ShowAlignmentToolTip(self):
		if self.toolTipAlignment == None:
			self.Close()
		if self.toolTipAlignment != None :
			self.toolTipAlignment.ShowToolTip()

	def __HideAlignmentToolTip(self):
		if self.toolTipAlignment == None:
			self.Close()
		if self.toolTipAlignment != None :
			self.toolTipAlignment.HideToolTip()

	def RefreshCharacter(self):

		if self.isLoaded==0:
			return

		## Name
		try:
			characterName=player.GetName()
			guildName=player.GetGuildName()
			self.characterNameValue.SetText(characterName)
			self.guildNameValue.SetText(guildName)
			if not guildName:
				self.characterNameSlot.SetPosition(109, 34)

				self.guildNameSlot.Hide()
			else:

				self.characterNameSlot.SetPosition(153, 34)
				self.guildNameSlot.Show()
		except:
			import exception
			exception.Abort("CharacterWindow.RefreshCharacter.BindObject")

		race=net.GetMainActorRace()
		group=net.GetMainActorSkillGroup()
		empire=net.GetMainActorEmpire()

		## Job Text
		job=chr.RaceToJob(race)
		self.__SetJobText(job, group)

		## FaceImage
		try:
			faceImageName=FACE_IMAGE_DICT[race]

			try:
				self.faceImage.LoadImage(faceImageName)
			except:
				print "CharacterWindow.RefreshCharacter(race=%d, faceImageName=%s)" % (race, faceImageName)
				self.faceImage.Hide()

		except KeyError:
			self.faceImage.Hide()

		## GroupName
		self.__SetSkillGroupName(race, group)

		## Skill
		if 0 == group:
			self.__SelectSkillGroup(0)

		else:
			self.__SetSkillSlotData(race, group, empire)

			if self.__CanUseHorseSkill():
				self.__SelectSkillGroup(0)

	def __SetSkillGroupName(self, race, group):
		
		job=chr.RaceToJob(race)

		if not self.SKILL_GROUP_NAME_DICT.has_key(job):
			return

		nameList=self.SKILL_GROUP_NAME_DICT[job]

		if 0 == group:
			self.skillGroupButton1.SetText(nameList[1])
			self.skillGroupButton2.SetText(nameList[2])
			self.skillGroupButton1.Show()
			self.skillGroupButton2.Show()
			self.activeSkillGroupName.Hide()
		else:
			if self.__CanUseHorseSkill():
				self.activeSkillGroupName.Hide()
				self.skillGroupButton1.SetText(nameList.get(group, "Noname"))
				self.skillGroupButton2.SetText(localeinfo.SKILL_GROUP_HORSE)
				self.skillGroupButton1.Show()
				self.skillGroupButton2.Show()

			else:
				self.activeSkillGroupName.SetText(nameList.get(group, "Noname"))
				self.activeSkillGroupName.Show()
				
				self.skillGroupButton1.Hide()
				self.skillGroupButton2.Hide()

	def __SetSkillSlotData(self, race, group, empire=0):
		## SkillIndex
		playersettingmodule.RegisterSkill(race, group, empire)

		## Event
		self.__SetSkillSlotEvent()

		## Refresh
		self.RefreshSkill()

	def __SelectSkillGroup(self, index):
		
		for btn in self.skillGroupButton:
			btn.SetUp()
		self.skillGroupButton[index].Down()

		if self.__CanUseHorseSkill():
			if 0 == index:
				index=net.GetMainActorSkillGroup()-1
			elif 1 == index:
				index=self.PAGE_HORSE

		
		self.curSelectedSkillGroup=index
		self.__SetSkillSlotData(net.GetMainActorRace(), index+1, net.GetMainActorEmpire())

	def __CanUseSkillNow(self):
		if 0 == net.GetMainActorSkillGroup():
			return False

		return True

	def __CanUseHorseSkill(self):

		slotIndex=player.GetSkillSlotIndex(player.SKILL_INDEX_RIDING)

		if not slotIndex:
			return False

		grade=player.GetSkillGrade(slotIndex)
		level=player.GetSkillLevel(slotIndex)
		if level < 0:
			level *= -1
		if grade >= 1 and level >= 1:
			return True

		return False

	def __IsChangedHorseRidingSkillLevel(self):
		ret=False

		if -1 == self.canUseHorseSkill:
			self.canUseHorseSkill=self.__CanUseHorseSkill()

		if self.canUseHorseSkill != self.__CanUseHorseSkill():
			ret=True

		self.canUseHorseSkill=self.__CanUseHorseSkill()
		return ret

	def __GetRealSkillSlot(self, skillGrade, skillSlot):
		return skillSlot + min(skill.SKILL_GRADE_COUNT-1, skillGrade)*skill.SKILL_GRADE_STEP_COUNT

	def __GetETCSkillRealSlotIndex(self, skillSlot):
		if skillSlot > 100:
			return skillSlot
		return skillSlot % self.ACTIVE_PAGE_SLOT_COUNT

	def __RealSkillSlotToSourceSlot(self, realSkillSlot):
		if realSkillSlot > 100:
			return realSkillSlot
		if self.PAGE_HORSE == self.curSelectedSkillGroup:
			return realSkillSlot + self.ACTIVE_PAGE_SLOT_COUNT
		return realSkillSlot % skill.SKILL_GRADE_STEP_COUNT

	def __GetSkillGradeFromSlot(self, skillSlot):
		return int(skillSlot / skill.SKILL_GRADE_STEP_COUNT)

	def SelectSkillGroup(self, index):
		self.__SelectSkillGroup(index)

	def OnQuestScroll(self):
		if self.toolTip == None:
			self.Close()
		questCount=quest.GetQuestCount()
		scrollLineCount=max(0, questCount - quest.QUEST_MAX_NUM)
		startIndex=int(scrollLineCount * self.questScrollBar.GetPos())

		if startIndex != self.questShowingStartIndex:
			self.questShowingStartIndex=startIndex
			self.RefreshQuest()
