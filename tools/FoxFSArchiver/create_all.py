import glob
import os

os.chdir("xml/")
files = []
for file in glob.glob("*.xml"):
	files.append(file)

os.chdir("..")
filearchiver_cmdline = "FoxFSArchiver"
for myFile in files:
	filearchiver_cmdline += " xml\\" + myFile

os.system(filearchiver_cmdline)