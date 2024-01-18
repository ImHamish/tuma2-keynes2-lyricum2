#pragma once

#include <windows.h>
#include "../eterBase/Singleton.h"
#include "../eterBase/Stl.h"
#include "../eterBase/MappedFile.h"
#include <FoxFS.h>
#include "FileSystem.h"


class CFileSystem : public CSingleton<CFileSystem>
{
	public:
		CFileSystem();
		virtual ~CFileSystem();

		bool Get (CMappedFile& rMappedFile, const char* c_szFileName, LPCVOID* pData, const char* func, bool ignoreChecks = false);
		void OutputError(int errorCode, bool showAsError, const std::string & stFilename, const char* func);
		static void OutputError (int errorCode, bool showAsError, const std::wstring &  stFilename, const char* func);
		bool GetFromPack (CMappedFile& rMappedFile, const char* c_szFileName, LPCVOID* pData);
		static bool GetFromFile (CMappedFile& rMappedFile, const char* c_szFileName, LPCVOID* pData);

		bool isExist (const char* c_szFileName, const char* func, bool ignoreChecks) const;
		bool isExistInPack (const char* c_szFileName, const char* func) const;

		bool LoadArchive (const std::wstring &stName) const;
		bool UnloadArchive (const std::wstring & stName) const;

	protected:

		FoxFS::FileSystem * m_pFoxFS;
};

