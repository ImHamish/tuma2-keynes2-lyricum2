import app
import net

import background
import ui
import uioption
import uisystemoption
import uigameoption
import uiscriptlocale
import networkmodule
import constinfo
import localeinfo
if app.ENABLE_NEW_GAMEOPTION:
	import uigameoptionnew

if app.ENABLE_INGAME_CHCHANGE:
	import uichannel


SYSTEM_MENU_FOR_PORTAL = False
	
no_ch_maps = {
	"metin2_map_oxevent",
	"metin2_map_wedding_01",
	"metin2_map_t1",
	"metin2_map_t2",
	"metin2_map_t3",
	"metin2_map_t4",
	"metin2_map_sungzi",
	"metin2_map_sungzi_flame_hill_01",
	"metin2_map_sungzi_flame_hill_02",
	"metin2_map_t2",
	"metin2_map_a1",
	"metin2_map_b1",
	"metin2_map_c1"
}
	
###################################################################################################
## System

ENABLED_SYSTEMOPTIONS = False

class SystemDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()

	def __Initialize(self):
		self.systemOptionDlg = None
		self.gameOptionDlg = None
		if app.ENABLE_INGAME_CHCHANGE:
			self.channelUI = uichannel.ChangeChannel()
			self.channelUI.Hide()

	def LoadDialog(self):
		if SYSTEM_MENU_FOR_PORTAL:
			self.__LoadSystemMenu_ForPortal()
		else:
			self.__LoadSystemMenu_Default()

	def __LoadSystemMenu_Default(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/systemdialog.py")
		global ENABLED_SYSTEMOPTIONS
		if ENABLED_SYSTEMOPTIONS:
			self.GetChild("system_option_button").SAFE_SetEvent(self.__ClickSystemOptionButton)
		else:
			self.GetChild("system_option_button").Hide()
		
		self.GetChild("game_option_button").SAFE_SetEvent(self.__ClickGameOptionButton)
		self.GetChild("change_button").SAFE_SetEvent(self.__ClickChangeCharacterButton)
		self.GetChild("logout_button").SAFE_SetEvent(self.__ClickLogOutButton)
		self.GetChild("exit_button").SAFE_SetEvent(self.__ClickExitButton)
		self.GetChild("cancel_button").SAFE_SetEvent(self.Close)
		self.GetChild("mall_button").SAFE_SetEvent(self.__ClickInGameShopButton)
		
		if app.ENABLE_INGAME_CHCHANGE:
			if background.GetCurrentMapName() in no_ch_maps:
				self.GetChild("movechannel_button").Down()
				self.GetChild("movechannel_button").Disable()
				return
			
		self.GetChild("movechannel_button").SAFE_SetEvent(self.OnClickCHChange)
		
		if constinfo.IN_GAME_WIKI_HELP_ENABLE:
			self.GetChild("help_button").SAFE_SetEvent(self.__ClickInGameWikiButton)

	def __LoadSystemMenu_ForPortal(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/systemdialog_forportal.py")
		global ENABLED_SYSTEMOPTIONS
		if ENABLED_SYSTEMOPTIONS:
			self.GetChild("system_option_button").SAFE_SetEvent(self.__ClickSystemOptionButton)
		else:
			self.GetChild("system_option_button").Hide()
		
		self.GetChild("game_option_button").SAFE_SetEvent(self.__ClickGameOptionButton)
		if app.ENABLE_INGAME_CHCHANGE:
			self.GetChild("movechannel_button").SAFE_SetEvent(self.OnClickCHChange)
		# if app.ENABLE_INGAME_CHCHANGE:
			# self.GetChild("movechannel_button").SAFE_SetEvent(self.__ClickChChangeButton)
		self.GetChild("change_button").SAFE_SetEvent(self.__ClickChangeCharacterButton)
		self.GetChild("exit_button").SAFE_SetEvent(self.__ClickExitButton)
		self.GetChild("cancel_button").SAFE_SetEvent(self.Close)

	def Destroy(self):
		self.ClearDictionary()

		if self.gameOptionDlg:
			self.gameOptionDlg.Destroy()

		if self.systemOptionDlg:
			self.systemOptionDlg.Destroy()

		self.__Initialize()

	def OpenDialog(self):
		self.Show()

	def __ClickChangeCharacterButton(self):
		self.Close()
		net.ExitGame()

	def __OnClosePopupDialog(self):
		self.popup = None

	def __ClickLogOutButton(self):
		if SYSTEM_MENU_FOR_PORTAL:
			if app.loggined:
				self.Close()
				net.ExitApplication()
			else:
				self.Close()
				net.LogOutGame()
		else:
			self.Close()
			net.LogOutGame()


	def __ClickExitButton(self):
		self.Close()
		net.ExitApplication()

	def __ClickSystemOptionButton(self):
		self.Close()

		if not self.systemOptionDlg:
			self.systemOptionDlg = uisystemoption.OptionDialog()

		self.systemOptionDlg.Show()

	if app.ENABLE_INGAME_CHCHANGE:
		def OnClickCHChange(self):
			self.Close()
			self.channelUI.Show()

	def __ClickGameOptionButton(self):
		self.Close()

		if not self.gameOptionDlg:
			if app.ENABLE_NEW_GAMEOPTION:
				self.gameOptionDlg = uigameoptionnew.GameOptionWindow()
			else:
				self.gameOptionDlg = uiGameOption.OptionDialog()

		self.gameOptionDlg.Show()

	if app.ENABLE_DOGMODE_RENEWAL:
		def OnCancelDogMode(self):
			try:
				self.gameOptionDlg.OnCancelDogMode()
			except:
				pass

	def __ClickInGameShopButton(self):
		if app.ENABLE_INGAME_ITEMSHOP:
			try:
				import newconstinfo
				newconstinfo._interface_instance.OpenItemShopWindow()
				self.Close()
			except Exception as e:
				import dbg
				dbg.TraceError(str(e))
		else:
			self.Close()

	def __ClickInGameWikiButton(self):
		self.Close()
		# if app.INGAME_WIKI:
			# net.ToggleWikiWindow()

	def Close(self):
		self.Hide()
		return True

	def RefreshMobile(self):
		if self.gameOptionDlg:
			self.gameOptionDlg.RefreshMobile()
		#self.optionDialog.RefreshMobile()

	def OnMobileAuthority(self):
		if self.gameOptionDlg:
			self.gameOptionDlg.OnMobileAuthority()
		#self.optionDialog.OnMobileAuthority()

	def OnBlockMode(self, mode):
		uigameoptionnew.blockMode = mode
		if self.gameOptionDlg:
			self.gameOptionDlg.OnBlockMode(mode)
		#self.optionDialog.OnBlockMode(mode)

	def OnChangePKMode(self):
		if self.gameOptionDlg:
			self.gameOptionDlg.OnChangePKMode()
		#self.optionDialog.OnChangePKMode()

	def OnPressExitKey(self):
		self.Close()
		return True

	def OnPressEscapeKey(self):
		self.Close()
		return True

