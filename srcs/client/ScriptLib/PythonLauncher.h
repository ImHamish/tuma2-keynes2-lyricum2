#pragma once
#include <frameobject.h>

#include "../eterBase/Singleton.h"

class CPythonLauncher : public CSingleton<CPythonLauncher>
{
	public:
		CPythonLauncher();
		virtual ~CPythonLauncher();

		static void Clear();

		bool Create(const char* c_szProgramName="eter.python");
		void SetTraceFunc(int (*pFunc)(PyObject * obj, PyFrameObject * f, int what, PyObject *arg));
		bool RunLine(const char* c_szLine) const;
		bool RunFile(const char* c_szFileName) const;
		bool RunMemoryTextFile(const char* c_szFileName, UINT uFileSize, const VOID* c_pvFileData) const;
		const char* GetError();

	protected:
		PyObject* m_poModule;
		PyObject* m_poDic;
};
