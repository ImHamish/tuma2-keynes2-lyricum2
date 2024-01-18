import app
import net
import ui
import localeinfo
import serverinfo
import ServerStateChecker

class ChangeChannel(ui.ScriptWindow):
	CH_COUNT = 4

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Fill_Up_ChannelList()
		self.Load()

	def __del__(self):
		self.ClearDictionary()
		self.channelBtn = []
		ui.ScriptWindow.__del__(self)

	def Load(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/movechanneldialog.py")
		except:
			import exception
			exception.Abort("ChangeChannelWindow.Load")
		
		self.channelBtn = []
		for i in xrange(self.CH_COUNT):
			btn = self.GetChild("btn_channel%d" % (i))
			btn.SetText(localeinfo.CHANNEL + " " + str(i + 1))
			btn.SetEvent(ui.__mem_func__(self.OnSelectChannel), i + 1)
			self.channelBtn.append(btn)
		
		self.GetChild("btn_accept").SetEvent(ui.__mem_func__(self.OnAccept))
		self.GetChild("btn_close").SetEvent(ui.__mem_func__(self.OnClose))
		self.GetChild("title").SetCloseEvent(ui.__mem_func__(self.OnClose))

	def OnSelectChannel(self, idx):
		self.ChangeChannelNumber = idx
		self.StartChannelNumber = None
		
		for btn in self.channelBtn:
			btn.SetUp()
			btn.Enable()
		
		self.channelBtn[idx-1].Down()
		self.channelBtn[idx-1].Disable()

	def OnAccept(self):
		if self.ChangeChannelNumber == self.StartChannelNumber:
			return
		
		net.SendChatPacket("/channel %d" % self.ChangeChannelNumber)
		self.__SaveChannelInfo()
		self.StartChannelNumber = self.ChangeChannelNumber
		self.OnClose()

	def OnOpen(self):
		self.StartChannelNumber = net.GetChannelNumber()
		self.StartChannelNumber = 1
		self.OnSelectChannel(self.StartChannelNumber)
		self.Show()

	def OnClose(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.OnClose()
		return True

	def __SaveChannelInfo(self):
		loadRegionID, loadServerID, loadChannelID = self.__LoadChannelInfo()
		try:
			file = open("channel.inf", "w")
			file.write("%d %d %d" % (loadServerID, self.ChangeChannelNumber, loadRegionID))
		except:
			print "MoveChannelDialog.__SaveChannelInfo - SaveError"

	def __LoadChannelInfo(self):
		try:
			file = open("channel.inf")
			lines = file.readlines()
			if len(lines) > 0:
				tokens = lines[0].split()
				selServerID = int(tokens[0])
				selChannelID = int(tokens[1])
				if len(tokens) == 3:
					regionID = int(tokens[2])
				
				return regionID, selServerID, selChannelID
		except:
			print "ChannelChanger.__LoadChannelInfo - OpenError"
			return -1, -1, -1

	def GetChannelName(self, idx):
		return "CH %d" % idx

	def GetChannelNumber(self):
		loadRegionID, loadServerID, loadChannelID = self.__LoadChannelInfo()
		return loadChannelID

	def __Fill_Up_ChannelList(self):
		self.__RequestServerStateList()
		self.__RefreshServerStateList()
		
	def __RequestServerStateList(self):
		regionID = self.__GetRegionID()
		serverID = self.__GetServerID()
		try:
			channelDict = serverinfo.REGION_DICT[regionID][serverID]["channel"]
		except:
			return
		
		ServerStateChecker.Initialize(self)
		for id, channelDataDict in channelDict.items():
			key = channelDataDict["key"]
			ip = channelDataDict["ip"]
			udp_port = channelDataDict["udp_port"]
			ServerStateChecker.AddChannel(key, ip, udp_port)
		ServerStateChecker.Request()
		
	def __RefreshServerStateList(self):
		regionID = self.__GetRegionID()
		serverID = self.__GetServerID()
		# bakChannelID = self.ChannelList.GetSelectedItem()
		# self.ChannelList.ClearItem()
		try:
			channelDict = serverinfo.REGION_DICT[regionID][serverID]["channel"]
		except:
			return
		
		for channelID, channelDataDict in channelDict.items():
			channelName = channelDataDict["name"]
			channelState = channelDataDict["state"]
			# self.ChannelList.InsertItem(channelID, "%s %s" % (channelName, channelState))
		
		# self.ChannelList.SelectItem(bakChannelID-1)
		
	def __GetRegionID(self):
		return 0

	def __GetServerID(self):
		serverID = 1
		regionID = self.__GetRegionID()
		for i in serverinfo.REGION_DICT[regionID].keys():
			if serverinfo.REGION_DICT[regionID][i]["name"] == net.GetServerInfo().split(",")[0]:
				serverID = int(i)
				break
		
		return serverID