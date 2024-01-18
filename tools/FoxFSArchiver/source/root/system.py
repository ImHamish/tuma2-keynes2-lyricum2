import app
import pack
import sys
import dbg

sys.path = []

class TraceFile:
	def write(self, msg):
		dbg.Trace(msg)

class TraceErrorFile:
	def write(self, msg):
		dbg.TraceError(msg)

class LogBoxFile:
	def __init__(self):
		self.stderrSave = sys.stderr
		self.msg = ""

	def __del__(self):
		self.restore()

	def restore(self):
		sys.stderr = self.stderrSave

	def write(self, msg):
		self.msg = self.msg + msg

	def show(self):
		dbg.TraceError(self.msg)
		dbg.LogBox(self.msg,"Error")

sys.stdout = TraceFile()
sys.stderr = TraceErrorFile()
#
# pack file support (must move to system.py, systemrelease.pyc)
#
import marshal
import imp

class EterPackModuleFinder(object):
	def find_module(self, fullname, path=None):
		if imp.is_builtin(fullname):
			return None

		path = fullname.replace('.', '/')

		loader = self.__MakeLoader(path + ".py", False, False)
		if loader:
			if __DEBUG__:
				dbg.Tracen("Found and loaded module {0}".format(fullname))
			return loader

		loader = self.__MakeLoader(path + ".pyc", True, False)
		if loader:
			if __DEBUG__:
				dbg.Tracen("Found and loaded module {0}".format(fullname))
			return loader

		loader = self.__MakeLoader(path + "/__init__.py", False, True)
		if loader:
			if __DEBUG__:
				dbg.Tracen("Found and loaded package {0}".format(fullname))
			return loader

		loader = self.__MakeLoader(path + "/__init__.pyc", True, True)
		if loader:
			if __DEBUG__:
				dbg.Tracen("Found and loaded package {0}".format(fullname))
			return loader

		if __DEBUG__:
			dbg.Tracen("Failed to find {0}".format(fullname))
		return None

	def __MakeLoader(self, filename, is_compiled, is_package):
		if not pack.Exist(filename):
			return None

		try:
			data = pack_file(filename,'r').read()
		except IOError:
			return None

		if is_compiled:
			if data[:4] != imp.get_magic():
				raise ImportError("Bad magic")

			code = marshal.loads(data[8:])
		else:
			code = compile(data, filename, "exec")

		return EterPackModuleLoader(filename, code, is_package)

sys.meta_path = [EterPackModuleFinder()]

class pack_file_iterator(object):
	def __init__(self, packfile):
		self.pack_file = packfile

	def next(self):
		tmp = self.pack_file.readline()
		if tmp:
			return tmp
		raise StopIteration

_chr = __builtins__.chr

__builtins__.NEW_PACK_FILE = True
if NEW_PACK_FILE:
	__builtins__.old_len = len
class pack_file(object):
	def __init__(self, filename, mode = 'rb'):
		assert mode in ('r', 'rb')
		if not pack.Exist(filename):
			raise IOError, 'No file or directory'
		self.data = pack.Get(filename)
		if NEW_PACK_FILE and not self.data:
			tmp = old_open(filename)
			self.data = tmp and tmp.read() or ''
		if mode == 'r':
			self.data=_chr(10).join(self.data.split(_chr(13)+_chr(10)))
		if NEW_PACK_FILE:
			self.mode = mode
			self.closed = False
			self.tell_size = 0L

	def __iter__(self):
		return pack_file_iterator(self)

	def read(self, length = 0):
		if not self.data:
			return ''
		if length:
			tmp = self.data[:length]
			if NEW_PACK_FILE:
				self.tell_size += old_len(tmp)
			self.data = self.data[length:]
			return tmp
		else:
			tmp = self.data
			if NEW_PACK_FILE:
				self.tell_size += old_len(tmp)
			self.data = ''
			return tmp

	def readline(self):
		return self.read(self.data.find(_chr(10))+1)

	def readlines(self):
		return [x for x in self]

	if NEW_PACK_FILE:
		def tell(self):
			return self.tell_size
		def close(self):
			self.closed = True
			self.data = ''
		def flush(self):
			pass
		def seek(self):
			pass

old_open = open
def open(filename, mode = 'rb'):
	if pack.Exist(filename) and mode in ('r', 'rb'):
		return pack_file(filename, mode)
	else:
		return old_open(filename, mode)

__builtins__.open = open
__builtins__.old_open = old_open
__builtins__.new_open = open
__builtins__.pack_open = open

_ModuleType = type(sys)

old_import = __import__
def _process_result(code, fqname):
	# did get_code() return an actual module? (rather than a code object)
	is_module = isinstance(code, _ModuleType)

	# use the returned module, or create a new one to exec code into
	if is_module:
		module = code
	else:
		module = imp.new_module(fqname)

	# insert additional values into the module (before executing the code)
	#module.__dict__.update(values)

	# the module is almost ready... make it visible
	sys.modules[fqname] = module

	# execute the code within the module's namespace
	if not is_module:
		exec code in module.__dict__

	# fetch from sys.modules instead of returning module directly.
	# also make module's __name__ agree with fqname, in case
	# the "exec code in module.__dict__" played games on us.
	module = sys.modules[fqname]
	module.__name__ = fqname
	return module

module_do = lambda x:None

def __hybrid_import(name,globals=None,locals=None,fromlist=None, level=-1):
	module_name = name.lower()
	if module_name in sys.modules:
		newmodule = sys.modules[module_name]
		if __DEBUG__:
			print newmodule
		
		return newmodule

	filename = module_name + '.py'
	if pack.Exist(filename):
		newmodule = _process_result(compile(pack_file(filename,'r').read(),filename,'exec'),name)
		module_do(newmodule)
		if __DEBUG__:
			print newmodule
		
		return newmodule
	else:
		newmodule = old_import(name, globals, locals, fromlist)
		if __DEBUG__:
			print newmodule
		
		return newmodule

def splitext(p):
	root, ext = '', ''
	for c in p:
		if c in ['/']:
			root, ext = root + ext + c, ''
		elif c == '.':
			if ext:
				root, ext = root + ext, c
			else:
				ext = c
		elif ext:
			ext = ext + c
		else:
			root = root + c
	return root, ext

def __IsCompiledFile__(sFileName):
	sBase, sExt = splitext(sFileName)
	sExt=sExt.lower()
	if sExt==".pyc" or sExt==".pyo":
		return 1
	else:
		return 0

def __LoadTextFile__(sFileName):
	sText=open(sFileName,'r').read()
	return compile(sText, sFileName, "exec")

def __LoadCompiledFile__(sFileName):
	kFile=open(sFileName)
	if kFile.read(4)!=imp.get_magic():
		raise

	kFile.read(4)

	kData=kFile.read()
	return marshal.loads(kData)

def execfile(fileName, dict):
	if __IsCompiledFile__(fileName):
		code=__LoadCompiledFile__(fileName)
	else:
		code=__LoadTextFile__(fileName)
		exec code in dict

import __builtin__
__builtin__.__import__ = __hybrid_import

#module_do = exec_add_module_do
__builtin__.old_execfile = __builtin__.execfile
__builtin__.execfile = execfile

"""
#
# PSYCO installation (must move to system.py, systemrelease.pyc)
#
try:
	import psyco
	#from psyco.classes import *

	def bind_me(bindable_list):
		try:
			for x in bindable_list:
				try:
					psyco.bind(x)
				except:
					pass
		except:
			pass

	_prev_psyco_old_module_do = module_do
	def module_bind(module):
		_prev_psyco_old_module_do(module)
		#print 'start binding' + str(module)
		try:
			psyco.bind(module)
		except:
			pass
		for x in module.__dict__.itervalues():
			try:
				psyco.bind(x)
			except:
				pass
		#print 'end binding'

	dbg.Trace("PSYCO installed\\n")

except Exception, msg:
	bind_me = lambda x:None
	dbg.Trace("No PSYCO support : %s\\n" % msg)
"""

def GetExceptionString(excTitle):
	(excType, excMsg, excTraceBack)=sys.exc_info()
	excText=""
	excText+=_chr(10)

	import traceback
	traceLineList=traceback.extract_tb(excTraceBack)

	for traceLine in traceLineList:
		if traceLine[3]:
			excText+="%s(line:%d) %s - %s" % (traceLine[0], traceLine[1], traceLine[2], traceLine[3])
		else:
			excText+="%s(line:%d) %s"  % (traceLine[0], traceLine[1], traceLine[2])

		excText+=_chr(10)

	excText+=_chr(10)
	excText+="%s - %s:%s" % (excTitle, excType, excMsg)
	excText+=_chr(10)

	return excText

def ShowException(excTitle):
	excText=GetExceptionString(excTitle)
	dbg.TraceError(excText)
	app.Abort()

	return 0

def RunMainScript(name):
	try:
		execfile(name, __main__.__dict__)
	except RuntimeError, msg:
		msg = str(msg)

		import localeinfo
		if localeinfo.error:
			msg = localeinfo.error.get(msg, msg)

		dbg.TraceError(msg)
		dbg.LogBox(msg)
		app.Abort()

	except:
		msg = GetExceptionString("Run")
		dbg.TraceError(msg)
		dbg.LogBox(msg)
		app.Abort()

import debuginfo
debuginfo.SetDebugMode(__DEBUG__)

loginMark = "-cs"

RunMainScript("prototype.py")
