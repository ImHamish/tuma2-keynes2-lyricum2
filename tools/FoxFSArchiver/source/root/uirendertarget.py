import ui
import wndMgr
import renderTarget
import player
import item
import chr
import app
import constinfo

class RenderTarget(ui.ScriptWindow):
	RENDER_TARGET_INDEX = 66
	Window = None

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.curRot = 0.0
		self.curMove = 0.0
		self.curZoom = 10.0
		self.max_pos_x = wndMgr.GetScreenWidth()
		self.max_pos_y = wndMgr.GetScreenHeight()


		self.Initialize()
		self.Init()

	def Initialize(self):
		self.interface = None

	@staticmethod
	def Get():
		if RenderTarget.Window == None:
			RenderTarget.Window = RenderTarget()

		return RenderTarget.Window

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.Initialize()

	def DisplayUser(self, vRace=0, vItemWeapon=0, vItemArmor=0, vItemHair=0, vItemFace=0):
		renderTarget.SetVisibility(self.RENDER_TARGET_INDEX, True)
		renderTarget.SelectModel(self.RENDER_TARGET_INDEX, vRace)

		if vRace > 7:
			return

		playerRace = player.GetRace()

		if vItemArmor != 0:
			renderTarget.SetArmor(self.RENDER_TARGET_INDEX, vItemArmor)
		else:
			if playerRace == vRace:
				if player.GetItemIndex(item.COSTUME_SLOT_START) == 0:
					renderTarget.SetArmor(self.RENDER_TARGET_INDEX, player.GetItemIndex(player.EQUIPMENT_SLOT_START))
				else:
					renderTarget.SetArmor(self.RENDER_TARGET_INDEX, player.GetItemIndex(item.COSTUME_SLOT_START))
			else:
				renderTarget.SetArmor(self.RENDER_TARGET_INDEX, 0)

		if vItemWeapon != 0:
			renderTarget.SetWeapon(self.RENDER_TARGET_INDEX, vItemWeapon)
		else:
			if playerRace == vRace:
				# if  player.GetItemIndex(item.COSTUME_SLOT_WEAPON) == 0:
				renderTarget.SetWeapon(self.RENDER_TARGET_INDEX, player.GetItemIndex(player.EQUIPMENT_SLOT_START+4))
				# else:
					# renderTarget.SetWeapon(self.RENDER_TARGET_INDEX, player.GetItemIndex(item.COSTUME_SLOT_WEAPON))
			else:
				renderTarget.SetWeapon(self.RENDER_TARGET_INDEX, 0)

		if vItemHair != 0:
			renderTarget.SetHair(self.RENDER_TARGET_INDEX, vItemHair)
		else:
			if playerRace == vRace:
				renderTarget.SetHair(self.RENDER_TARGET_INDEX, vItemHair)
			else:
				renderTarget.SetHair(self.RENDER_TARGET_INDEX, 0)

		if vItemFace != 0:
			renderTarget.SetHead(self.RENDER_TARGET_INDEX, vItemFace)
		else:
			if playerRace == vRace:
				renderTarget.SetHead(self.RENDER_TARGET_INDEX, player.GetItemIndex(17))
			else:
				renderTarget.SetHead(self.RENDER_TARGET_INDEX, 0)

	def Init(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/RenderTargetWindow.py")
		except:
			import exception
			exception.Abort("RenderTargetWindow.LoadDialog.LoadScript")

		try:
			self.titleBar = self.GetChild("TitleBar")
			# self.titleBar.CloseButton("show")
			# self.titleBar.SetCloseEvent(self.Close)

			self.board = self.GetChild("board")

			self.RenderTarget = self.GetChild2("RenderTarget")
			self.moveUp = self.GetChild2("mvUpCmrBtn")
			self.moveDown = self.GetChild2("mvDownCmrBtn")
			self.rotateLeft = self.GetChild2("rotLeftBtn")
			self.rotateRight = self.GetChild2("rotRightBtn")
			self.moveReset = self.GetChild2("mvResetBtn")
			self.zoomIn = self.GetChild2("zumInBtn")
			self.zoomOut = self.GetChild2("zumOutBtn")

			# self.buttons = {
				# "moveUp" : self.GetChild2("mvUpCmrBtn"),
				# "moveDown" : self.GetChild2("mvDownCmrBtn"),
				# "rotateLeft" : self.GetChild2("rotLeftBtn"),
				# "rotateRight" : self.GetChild2("rotRightBtn"),

				# "moveReset" : self.GetChild2("mvResetBtn"),

				# "zoomIn" : self.GetChild2("zumInBtn"),
				# "zoomOut" : self.GetChild2("zumOutBtn"),
			# }

			# self.buttons["moveUp"].SetEvent(ui.__mem_func__(self.__ModelUpDownCameraProgress))
			# self.moveUp.SetEvent(ui.__mem_func__(self.__ModelUpDownCameraProgress))
			# self.buttons["moveDown"].SetEvent(ui.__mem_func__(self.__ModelUpDownCameraProgress))
			# self.moveDown.SetEvent(ui.__mem_func__(self.__ModelUpDownCameraProgress))
			# self.buttons["rotateLeft"].SetEvent(ui.__mem_func__(self.__ModelRotationProgress))
			# self.rotateLeft.SetEvent(ui.__mem_func__(self.__ModelRotationProgress))
			# self.buttons["rotateRight"].SetEvent(ui.__mem_func__(self.__ModelRotationProgress))
			# self.rotateRight.SetEvent(ui.__mem_func__(self.__ModelRotationProgress))
			# self.buttons["moveReset"].SetEvent(ui.__mem_func__(self.__ResetSettings))
			# self.moveReset.SetEvent(ui.__mem_func__(self.__ResetSettings))
			# self.buttons["zoomIn"].SetEvent(ui.__mem_func__(self.__ModelZoomProgress))
			# self.zoomIn.SetEvent(ui.__mem_func__(self.parent.__ModelZoomProgress))
			# self.buttons["zoomOut"].SetEvent(ui.__mem_func__(self.__ModelZoomProgress))
			# self.zoomOut.SetEvent(ui.__mem_func__(self.parent.__ModelZoomProgress))

			self.SetCenterPosition()

		except:
			import exception
			exception.Abort("RenderTargetWindow.LoadDialog.BindObject")

		renderTarget.SetBackground(self.RENDER_TARGET_INDEX, "render_target.tga")

	def BindInterface(self, interface):
		self.interface = interface

	def Destroy(self):
		self.Close()
		self.Initialize()

	def Close(self):
		self.Hide()
		self.__ResetSettings()

	def Open(self):
		self.Show()
		self.SetTop()

	def OnPressEscapeKey(self):
		self.Close()

	def MINMAX(self, min, value, max):

		if value < min:
			return min
		elif value > max:
			return max
		else:
			return value

	def OnUpdate(self):
		self.__ModelUpDownCameraProgress()
		self.__ModelRotationProgress()
		self.__ModelZoomProgress()

		x, y = self.GetGlobalPosition()
		
		if self.curZoom > 90.0:
			self.curZoom = 90.0
			
		if self.curZoom < 4.0:
			self.curZoom = 4.0
			
		pos_x = self.MINMAX(0, x, self.max_pos_x)
		pos_y = self.MINMAX(0, y, self.max_pos_y)

		self.SetPosition(pos_x, pos_y)

	def __ModelUpDownCameraProgress(self):
		if self.buttons["moveUp"].IsDown():
			self.curMove -= 2
			renderTarget.SetRenderingPosition(self.RENDER_TARGET_INDEX, self.curMove)

		if self.buttons["moveDown"].IsDown():
			self.curMove += 2
			renderTarget.SetRenderingPosition(self.RENDER_TARGET_INDEX, self.curMove)

	def __ModelRotationProgress(self):
		if self.buttons["rotateLeft"].IsDown():
			self.curRot -= 2
			renderTarget.SetMove(self.RENDER_TARGET_INDEX, self.curRot)

		if self.buttons["rotateRight"].IsDown():
			self.curRot += 2
			renderTarget.SetMove(self.RENDER_TARGET_INDEX, self.curRot)

	def __ModelZoomProgress(self):
		if self.buttons["zoomIn"].IsDown():
			self.curZoom -= 0.5
			renderTarget.SetZoom(self.RENDER_TARGET_INDEX, self.curZoom)

		if self.buttons["zoomOut"].IsDown():
			self.curZoom += 0.5
			renderTarget.SetZoom(self.RENDER_TARGET_INDEX, self.curZoom)

	def __ResetSettings(self):
		self.curRot = 0.0
		self.curMove = 0.0
		self.curZoom = 10.0
		renderTarget.ResetSettings(self.RENDER_TARGET_INDEX)

	def AdjustPosition(self):
		if self.interface and self.interface.wndInventory and self.interface.wndInventory.GetGlobalPosition():
			x, y = self.interface.wndInventory.GetGlobalPosition()
			self.SetPosition(x - 210, y + 210)
