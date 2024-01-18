#ifndef FOXFS_FILESYSTEM_H
#define FOXFS_FILESYSTEM_H

#include <string>
#include <map>

#include "Archive.h"

#include <windows.h>      
#include <iphlpapi.h>   


namespace FoxFS
{
	
	class FileSystem
	{
		struct KeyInfo
		{
			unsigned char key[32];
			unsigned char iv[32];
		};
		
	public:
		FileSystem();
		~FileSystem();
				
		int load(const wchar_t* filename);

		int unload(const wchar_t* filename);
		
		unsigned int size(const char* filename) const;

		int exists(const char* filename) const;

		int get(const char* filename, void* buffer, unsigned int maxsize, unsigned int* outsize) const;

	private:
		std::map<std::basic_string<wchar_t>, Archive*> archives;
		
		std::map<std::string, KeyInfo> keys;
	
		mutable CRITICAL_SECTION mutex;

	};
	
}

#endif