import os

fileString = ""
for file in os.listdir("make_xml"):
	if file.endswith(".xml"):
		fileString += "make_xml/%s " % (str(file))

os.system('"FoxFSArchiver" %s' % fileString)
print "XML Files were created"

