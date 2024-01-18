import app
import chrmgr
import player
import net
import dbg
import background
import wndMgr
import ui
import uiscriptlocale
import localeinfo
import colorinfo
import constinfo
import playersettingmodule
import stringcommander
import emotion
from _weakref import proxy

NAME_COLOR_DICT = {
	chrmgr.NAMECOLOR_PC : colorinfo.CHR_NAME_RGB_PC,
	chrmgr.NAMECOLOR_NPC : colorinfo.CHR_NAME_RGB_NPC,
	chrmgr.NAMECOLOR_MOB : colorinfo.CHR_NAME_RGB_MOB,
	chrmgr.NAMECOLOR_PVP : colorinfo.CHR_NAME_RGB_PVP,
	chrmgr.NAMECOLOR_PK : colorinfo.CHR_NAME_RGB_PK,
	chrmgr.NAMECOLOR_PARTY : colorinfo.CHR_NAME_RGB_PARTY,
	chrmgr.NAMECOLOR_WARP : colorinfo.CHR_NAME_RGB_WARP,
	chrmgr.NAMECOLOR_WAYPOINT : colorinfo.CHR_NAME_RGB_WAYPOINT,
	chrmgr.NAMECOLOR_EMPIRE_MOB : colorinfo.CHR_NAME_RGB_EMPIRE_MOB,
	chrmgr.NAMECOLOR_EMPIRE_NPC : colorinfo.CHR_NAME_RGB_EMPIRE_NPC,
	chrmgr.NAMECOLOR_EMPIRE_PC+1 : colorinfo.CHR_NAME_RGB_EMPIRE_PC_A,
	chrmgr.NAMECOLOR_EMPIRE_PC+2 : colorinfo.CHR_NAME_RGB_EMPIRE_PC_B,
	chrmgr.NAMECOLOR_EMPIRE_PC+3 : colorinfo.CHR_NAME_RGB_EMPIRE_PC_C,
}

TITLE_COLOR_DICT = (
	colorinfo.TITLE_RGB_GOOD_4,
	colorinfo.TITLE_RGB_GOOD_3,
	colorinfo.TITLE_RGB_GOOD_2,
	colorinfo.TITLE_RGB_GOOD_1,
	colorinfo.TITLE_RGB_NORMAL,
	colorinfo.TITLE_RGB_EVIL_1,
	colorinfo.TITLE_RGB_EVIL_2,
	colorinfo.TITLE_RGB_EVIL_3,
	colorinfo.TITLE_RGB_EVIL_4,
)

def __main__():
	for nameIndex, nameColor in NAME_COLOR_DICT.items():
		chrmgr.RegisterNameColor(nameIndex, *nameColor)
	
	count = 0
	for rgb in TITLE_COLOR_DICT:
		chrmgr.RegisterTitleColor(count, rgb[0], rgb[1], rgb[2])
		count += 1
	
	for i in xrange(len(localeinfo.TITLE_NAME_LIST)):
		chrmgr.RegisterTitleName(i, localeinfo.TITLE_NAME_LIST[i], localeinfo.TITLE_NAME_LIST[i])
	
	emotion.RegisterEmotionIcons()
	playersettingmodule.LoadGuildBuildingList(localeinfo.GUILD_BUILDING_LIST_TXT)

class LoadingWindow(ui.ScriptWindow):
	def __init__(self, stream):
		ui.Window.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_LOAD, self)
		self.stream = proxy(stream)

	def __del__(self):
		net.SetPhaseWindow(net.PHASE_WINDOW_LOAD, 0)
		ui.Window.__del__(self)

	def Open(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/loadingwindow.py")
		except:
			import exception
			exception.Abort("LodingWindow.Open - LoadScriptFile Error")
		
		net.SendSelectCharacterPacket(self.stream.GetCharacterSlot())
		
		import constinfo
		if constinfo.INTROLOADING == 1:
			constinfo.INTROLOADING = 0
			self.GetChild("bg_element").Show()
		else:
			self.GetChild("bg_element").Show()
		
		if app.ENABLE_OFFLINESHOP_REWORK:
			import constinfo
			constinfo.shopSearchWaitTime=0
		
		self.Show()

	def Close(self):
		self.ClearDictionary()
		self.Hide()

	def OnPressEscapeKey(self):
		self.stream.SetLoginPhase()
		return True

	def DEBUG_LoadData(self, playerX, playerY):
		self.LoadData(playerX, playerY)

	def LoadData(self, playerX, playerY):
		playersettingmodule.RegisterSkill(net.GetMainActorRace(), net.GetMainActorSkillGroup(), net.GetMainActorEmpire())
		background.SetViewDistanceSet(background.DISTANCE0, 25600)
		background.SelectViewDistanceNum(background.DISTANCE0)
		app.SetGlobalCenterPosition(playerX, playerY)
		net.StartGame()
