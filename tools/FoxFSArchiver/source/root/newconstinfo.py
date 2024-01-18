_game_instance = None
_interface_instance = None

def GetGameInstance():
	global _game_instance
	return _game_instance

def SetGameInstance(inst):
	global _game_instance

	if inst:
		from _weakref import proxy
		_game_instance = proxy(inst)
	else:
		_game_instance = None

def GetInterfaceInstance():
	global _interface_instance
	return _interface_instance

def SetInterfaceInstance(inst):
	global _interface_instance

	if inst:
		from _weakref import proxy
		_interface_instance = proxy(inst)
	else:
		_interface_instance = None
