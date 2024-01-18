METIN2THEMA = "m2bg.mp3"

loginMusic = "login_window.mp3"
createMusic = "characterselect.mp3"
selectMusic = "characterselect.mp3"
fieldMusic = METIN2THEMA

def SaveLastPlayFieldMusic():
	global fieldMusic
	try:
		f = old_open("bgm/lastplay.inf", "w")
		f.write(fieldMusic)
		f.close()
	except:
		pass

def LoadLastPlayFieldMusic():
	global fieldMusic
	
	try:
		f = old_open("bgm/lastplay.inf", "r")
		fieldMusic = f.read()
		f.close()
	except IOError:
		f = old_open("bgm/lastplay.inf", "w")
		f.write(fieldMusic)
		f.close()
	except:
		pass
