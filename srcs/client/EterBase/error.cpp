#include "StdAfx.h"

#include <stdio.h>
#include <time.h>
#include <winsock.h>
#include <imagehlp.h>
#ifdef USE_CRASH_RPT
#include "Debug.h"

static constexpr bool s_bShowSymbolName = true;
static constexpr bool s_bShowLineNumber = true;
#endif

FILE* fException;

/*
static char __msg[4000], __cmsg[4000];
static int32_t __idx;
CLZObject __l;
*/

/*
typedef bool
(CALLBACK* PENUMLOADED_MODULES_CALLBACK)(
	__in PCSTR ModuleName,
	__in ULONG ModuleBase,
	__in ULONG ModuleSize,
	__in_opt PVOID UserContext
	);
*/

#if _MSC_VER >= 1400
bool CALLBACK EnumerateLoadedModulesProc(PCSTR ModuleName, ULONG ModuleBase, ULONG ModuleSize, PVOID UserContext)
#else
bool CALLBACK EnumerateLoadedModulesProc(PSTR ModuleName, ULONG ModuleBase, ULONG ModuleSize, PVOID UserContext)
#endif
{
	uint32_t offset = *((uint32_t*)UserContext);

	if (offset >= ModuleBase && offset <= ModuleBase + ModuleSize)
	{
		fprintf(fException, "%s", ModuleName);
		//__idx += sprintf(__msg + __idx, "%s", ModuleName);
		return false;
	}
	else
		return true;
}

LONG __stdcall EterExceptionFilter(_EXCEPTION_POINTERS* pExceptionInfo)
{
	HANDLE hProcess = GetCurrentProcess();
	HANDLE hThread = GetCurrentThread();

#ifdef USE_CRASH_RPT
    std::filesystem::path cur_path = std::filesystem::current_path();

    {
        auto logfile = std::to_string(GetLogFileTimeStamp());

        auto src_path = cur_path.string() + "\\syserr\\" + logfile + ".txt";
        auto copy_path = cur_path.string() + "\\syserr\\crash\\" + logfile + ".txt";

        std::filesystem::copy_file(src_path, copy_path);
    }

    auto error_log_file = cur_path.string() + "\\syserr\\crash\\error_log.txt";
    fException = fopen(error_log_file.c_str(), "wt");
#else
    fException = fopen("ErrorLog.txt", "wt");
#endif

	if (fException)
	{
#ifdef USE_CRASH_RPT
        SymInitialize(hProcess, NULL, TRUE);
#endif

		char module_name[256];
		time_t module_time;

		HMODULE hModule = GetModuleHandle(NULL);

		GetModuleFileName(hModule, module_name, sizeof(module_name));
		module_time = (time_t)GetTimestampForLoadedLibrary(hModule);

		fprintf(fException, "Module Name: %s\n", module_name);
		fprintf(fException, "Time Stamp: 0x%08x - %s\n", static_cast<uint32_t>(module_time), ctime(&module_time));
		fprintf(fException, "\n");
		fprintf(fException, "Exception Type: 0x%08x\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
		fprintf(fException, "\n");
		/*
		{
			__idx += sprintf(__msg + __idx, "Module Name: %s\n", module_name);
			__idx += sprintf(__msg + __idx, "Time Stamp: 0x%08x - %s\n", module_time, ctime(&module_time));
			__idx += sprintf(__msg + __idx, "\n");
			__idx += sprintf(__msg + __idx, "Exception Type: 0x%08x\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
			__idx += sprintf(__msg + __idx, "\n");
		}
		*/

		CONTEXT& context = *pExceptionInfo->ContextRecord;

		fprintf(fException, "eax: 0x%08x\tebx: 0x%08x\n", context.Eax, context.Ebx);
		fprintf(fException, "ecx: 0x%08x\tedx: 0x%08x\n", context.Ecx, context.Edx);
		fprintf(fException, "esi: 0x%08x\tedi: 0x%08x\n", context.Esi, context.Edi);
		fprintf(fException, "ebp: 0x%08x\tesp: 0x%08x\n", context.Ebp, context.Esp);
		fprintf(fException, "\n");
		/*
		{
			__idx += sprintf(__msg + __idx, "eax: 0x%08x\tebx: 0x%08x\n", context.Eax, context.Ebx);
			__idx += sprintf(__msg + __idx, "ecx: 0x%08x\tedx: 0x%08x\n", context.Ecx, context.Edx);
			__idx += sprintf(__msg + __idx, "esi: 0x%08x\tedi: 0x%08x\n", context.Esi, context.Edi);
			__idx += sprintf(__msg + __idx, "ebp: 0x%08x\tesp: 0x%08x\n", context.Ebp, context.Esp);
			__idx += sprintf(__msg + __idx, "\n");
		}
		*/

#ifdef USE_CRASH_RPT
        STACKFRAME rStackFrame = { 0 };
        rStackFrame.AddrPC.Offset = context.Eip;
        rStackFrame.AddrStack.Offset = context.Esp;
        rStackFrame.AddrFrame.Offset = context.Ebp;
        rStackFrame.AddrPC.Mode = AddrModeFlat;
        rStackFrame.AddrStack.Mode = AddrModeFlat;
        rStackFrame.AddrFrame.Mode = AddrModeFlat;

        DWORD dwDisplament = 0;
        while (StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &rStackFrame, &context, NULL, &SymFunctionTableAccess, &SymGetModuleBase, NULL))
        {
            DWORD dwAddress = rStackFrame.AddrPC.Offset;
            if (dwAddress == 0)
            {
                break;
            }

            // Get symbol name
            TCHAR szBuffer[MAX_PATH + sizeof(IMAGEHLP_SYMBOL)] = { 0 };
            PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL)szBuffer;
            pSymbol->SizeOfStruct = sizeof(szBuffer);
            pSymbol->MaxNameLength = sizeof(szBuffer) - sizeof(IMAGEHLP_SYMBOL);

            // Get module name
            IMAGEHLP_MODULE moduleInfo = { 0 };
            moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);
            TCHAR* szModule = "";
            if (SymGetModuleInfo(hProcess, dwAddress, &moduleInfo))
            {
                szModule = moduleInfo.ModuleName;
            }

            if (SymGetSymFromAddr(hProcess, rStackFrame.AddrPC.Offset, &dwDisplament, pSymbol))
            {
                // Get file name and line count.
                IMAGEHLP_LINE ImageLine = { 0 };
                ImageLine.SizeOfStruct = sizeof(IMAGEHLP_LINE);

                if (SymGetLineFromAddr(hProcess, rStackFrame.AddrPC.Offset, &dwDisplament, &ImageLine))
                {
                    //fprintf(fException, "%08x %s!%s [%s @ %lu]\n", pSymbol->Address, szModule, pSymbol->Name, ImageLine.FileName, ImageLine.LineNumber);
                    fprintf(fException, "%08x\t%s!%s @ %lu\n",
                        pSymbol->Address,
                        szModule,
                        s_bShowSymbolName ? pSymbol->Name : "",
                        s_bShowLineNumber ? ImageLine.LineNumber : 0
                    );
                }
                else
                {
                    fprintf(fException, "%08x\t%s!%s\n", pSymbol->Address, szModule, s_bShowSymbolName ? pSymbol->Name : "");
                }
            }
        }

        SymCleanup(hProcess);
#else
		STACKFRAME stackFrame = { 0, };
		stackFrame.AddrPC.Offset = context.Eip;
		stackFrame.AddrPC.Mode = AddrModeFlat;
		stackFrame.AddrStack.Offset = context.Esp;
		stackFrame.AddrStack.Mode = AddrModeFlat;
		stackFrame.AddrFrame.Offset = context.Ebp;
		stackFrame.AddrFrame.Mode = AddrModeFlat;

		for (int32_t i = 0; i < 512 && stackFrame.AddrPC.Offset; ++i)
		{
			if (StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stackFrame, &context, NULL, NULL, NULL, NULL) != false)
			{
				fprintf(fException, "0x%08x\t", stackFrame.AddrPC.Offset);
				//__idx += sprintf(__msg + __idx, "0x%08x\t", stackFrame.AddrPC.Offset);
				EnumerateLoadedModules(hProcess, (PENUMLOADED_MODULES_CALLBACK)EnumerateLoadedModulesProc, &stackFrame.AddrPC.Offset);
				fprintf(fException, "\n");

				//__idx += sprintf(__msg + __idx, "\n");
			}
			else
			{
				break;
			}
		}
#endif

		fprintf(fException, "\n");
		//__idx += sprintf(__msg + __idx, "\n");

		/*
		uint8_t* stack = (uint8_t*)(context.Esp);
		fprintf(fException, "stack %08x - %08x\n", context.Esp, context.Esp + 1024);
		//__idx += sprintf(__msg + __idx, "stack %08x - %08x\n", context.Esp, context.Esp + 1024);

		for (i = 0; i < 16; ++i)
		{
			fprintf(fException, "%08X : ", context.Esp + i * 16);
			//__idx += sprintf(__msg + __idx, "%08X : ", context.Esp + i * 16);
			for (int32_t j = 0; j < 16; ++j)
			{
				fprintf(fException, "%02X ", stack[i * 16 + j]);
				//__idx += sprintf(__msg + __idx, "%02X ", stack[i * 16 + j]);
			}
			fprintf(fException, "\n");
			//__idx += sprintf(__msg + __idx, "\n");
		}
		fprintf(fException, "\n");
		//__idx += sprintf(__msg + __idx, "\n");
		*/

		fflush(fException);

		fclose(fException);
		fException = NULL;

		//WinExec()
		/*
		CreateProcess("cmd.exe", NULL, NULL, NULL, false,
			CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS, NULL, NULL, NULL, NULL);
		MessageBox(NULL, "게임 실행에 치명적인 문제가 발생하였습니다.\n게임을 종료하고 에러 로그를 남깁니다.\n에러 로그를 서버에 보내시겠습니까?", "에러 발생!", MB_YESNO);
		*/

		//__l.BeginCompressInBuffer(__msg, __idx, __cmsg);
		/*
		if (__l.Compress())
		{
			//fprintf(fException, "Compress printing\n");
			// send this to server
			SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			ioctlsocket(s, FIONBIO, 0);

			if (s == INVALID_SOCKET)
			{
				//fprintf(fException, "INVALID %X\n", WSAGetLastError());
			}

			sockaddr_in sa;
			sa.sin_family = AF_INET;
			sa.sin_port = htons(19294);
			sa.sin_addr.s_addr = inet_addr("147.46.127.42");
			if (connect(s, (sockaddr*)&sa, sizeof(sa)))
			{
				//fprintf(fException, "%X\n", WSAGetLastError());
			}

			int32_t total = 0;
			int32_t ret = 0;
			while (1)
			{
				//ret = send(s, (char*)__msg + total, __idx - total, 0);
				ret = send(s, (char*)__l.GetBuffer() + total, __l.GetSize() - total, 0);
				//fprintf(fException, "send %d\n", ret);
				if (ret < 0)
				{
					//fprintf(fException, "%X\n", WSAGetLastError());
					break;
				}
				total += ret;
				if (total >= __idx)
					//if (total >= __l.GetSize())
					break;
			}
			//__l.GetBuffer();
			Sleep(500);
			closesocket(s);
		}
		*/

#ifndef USE_CRASH_RPT
        WinExec("errorlog.exe", SW_SHOW);
#endif
	}

#ifdef USE_CRASH_RPT
    // Dump DxDiag Information
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi;

    auto crash_path = cur_path.string() + "\\syserr\\crash";
    if (CreateProcess("C:\\Windows\\System32\\dxdiag.exe", "/dontskip /whql:off /t dxdiag_log.txt", NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, crash_path.c_str(), &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
#endif

	return EXCEPTION_EXECUTE_HANDLER;
}

#ifndef USE_CRASH_RPT
void SetEterExceptionHandler()
{
    SetUnhandledExceptionFilter(EterExceptionFilter);
}
#endif

#ifdef USE_CRASH_RPT
#include <crashrpt/CrashRpt.h>

#ifdef _DEBUG
#pragma comment(lib, "CrashRptd.lib")
#pragma comment(lib, "CrashRptProbed.lib")
#else
#pragma comment(lib, "CrashRpt.lib")
#pragma comment(lib, "CrashRptProbe.lib")
#endif // _DEBUG

int CALLBACK CrashCallback(CR_CRASH_CALLBACK_INFO* pInfo)
{
    PEXCEPTION_POINTERS pExceptionInfo = pInfo->pExceptionInfo->pexcptrs;

    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();

    std::filesystem::path cur_path = std::filesystem::current_path();

    {
        auto logfile = std::to_string(GetLogFileTimeStamp());

        auto src_path = cur_path.string() + "\\syserr\\" + logfile + ".txt";
        auto copy_path = cur_path.string() + "\\syserr\\crash\\" + logfile + ".txt";

        std::filesystem::copy_file(src_path, copy_path);
    }

    auto error_log_file = cur_path.string() + "\\syserr\\crash\\error_log.txt";
    fException = fopen(error_log_file.c_str(), "wt");
    if (fException)
    {
        SymInitialize(hProcess, NULL, TRUE);

        HMODULE hModule = GetModuleHandle(NULL);
        TCHAR szModuleName[256];
        GetModuleFileName(hModule, szModuleName, sizeof(szModuleName));
        std::time_t timestamp = static_cast<std::time_t>(GetTimestampForLoadedLibrary(hModule));

        fprintf(fException, "Module Name: %s\n", szModuleName);
        fprintf(fException, "Time Stamp: %s - %s\n", std::to_string(timestamp).c_str(), std::ctime(&timestamp));
        fprintf(fException, "Exception Type: 0x%08x\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
        fprintf(fException, "\n");

        CONTEXT& rContext = *pExceptionInfo->ContextRecord;

        fprintf(fException, "eax: 0x%08x\tebx: 0x%08x\n", rContext.Eax, rContext.Ebx);
        fprintf(fException, "ecx: 0x%08x\tedx: 0x%08x\n", rContext.Ecx, rContext.Edx);
        fprintf(fException, "esi: 0x%08x\tedi: 0x%08x\n", rContext.Esi, rContext.Edi);
        fprintf(fException, "ebp: 0x%08x\tesp: 0x%08x\n", rContext.Ebp, rContext.Esp);
        fprintf(fException, "\n");

        // Initialize stack walking.
        STACKFRAME rStackFrame = { 0 };
        rStackFrame.AddrPC.Offset = rContext.Eip;
        rStackFrame.AddrStack.Offset = rContext.Esp;
        rStackFrame.AddrFrame.Offset = rContext.Ebp;
        rStackFrame.AddrPC.Mode = AddrModeFlat; // EIP
        rStackFrame.AddrStack.Mode = AddrModeFlat; // EBP
        rStackFrame.AddrFrame.Mode = AddrModeFlat; // ESP

        DWORD dwDisplament = 0;
        while (StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &rStackFrame, &rContext, NULL, &SymFunctionTableAccess, &SymGetModuleBase, NULL))
        {
            DWORD dwAddress = rStackFrame.AddrPC.Offset;
            if (dwAddress == 0)
            {
                break;
            }

            // Get symbol name
            TCHAR szBuffer[MAX_PATH + sizeof(IMAGEHLP_SYMBOL)] = { 0 };
            PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL)szBuffer;
            pSymbol->SizeOfStruct = sizeof(szBuffer);
            pSymbol->MaxNameLength = sizeof(szBuffer) - sizeof(IMAGEHLP_SYMBOL);

            // Get module name
            IMAGEHLP_MODULE moduleInfo = { 0 };
            moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);
            TCHAR* szModule = "";
            if (SymGetModuleInfo(hProcess, dwAddress, &moduleInfo))
            {
                szModule = moduleInfo.ModuleName;
            }

            if (SymGetSymFromAddr(hProcess, rStackFrame.AddrPC.Offset, &dwDisplament, pSymbol))
            {
                IMAGEHLP_LINE ImageLine = { 0 };
                ImageLine.SizeOfStruct = sizeof(IMAGEHLP_LINE);

                if (SymGetLineFromAddr(hProcess, rStackFrame.AddrPC.Offset, &dwDisplament, &ImageLine))
                {
                    fprintf(fException, "%08x\t%s!%s @ %lu\n",
                        pSymbol->Address,
                        szModule,
                        s_bShowSymbolName ? pSymbol->Name : "",
                        s_bShowLineNumber ? ImageLine.LineNumber : 0
                    );
                }
                else
                {
                    fprintf(fException, "%08x\t%s!%s\n", pSymbol->Address, szModule, s_bShowSymbolName ? pSymbol->Name : "");
                }
            }
        }

        SymCleanup(hProcess);

        fprintf(fException, "\n");
        fflush(fException);

        fclose(fException);
        fException = NULL;
    }

    // Dump DxDiag Information
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi;

    auto crash_path = cur_path.string() + "\\syserr\\crash";
    if (CreateProcess("C:\\Windows\\System32\\dxdiag.exe", "/dontskip /whql:off /t dxdiag_log.txt", NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, crash_path.c_str(), &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }

    // Proceed with crash report generation. 
    // This return code also makes CrashRpt to not call this callback function for 
    // the next crash report generation stage.
    return CR_CB_DODEFAULT;
}

void CrashRptInitialize()
{
    CR_INSTALL_INFO info;
    memset(&info, 0, sizeof(CR_INSTALL_INFO));
    info.cb = sizeof(CR_INSTALL_INFO);

    info.pszAppName = APP_NAME;
    info.pszAppVersion = "1.0.0";
    info.pszEmailSubject = CRASH_RPT_EMAIL_SUBJECT;
    info.pszEmailTo = CRASH_RPT_EMAIL_TO;
    info.pszUrl = CRASH_RPT_URL;

    info.uPriorities[CR_HTTP] = 3; // First try send report over HTTP
    info.uPriorities[CR_SMTP] = 2; // Second try send report over SMTP
    info.uPriorities[CR_SMAPI] = 1; // Third try send report over Simple MAPI

    // Install all available exception handlers
    info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS;

    // Restart the app on crash
    info.dwFlags |= CR_INST_APP_RESTART;
    info.dwFlags |= CR_INST_SEND_QUEUED_REPORTS;
    info.pszRestartCmdLine = "/restart";

    // Define the Privacy Policy URL
    //info.pszPrivacyPolicyURL = CRASH_RPT_POLICY_URL;

    // Install crash reporting
    crInstall(&info);

    // Set crash callback function
    crSetCrashCallback(CrashCallback, NULL);

    // Add our log file to the error report
    std::filesystem::path cur_path = std::filesystem::current_path();

    // syserr.txt
    auto syserr_file = cur_path.string() + "\\syserr\\crash\\" + std::to_string(GetLogFileTimeStamp()) + ".txt";
    crAddFile2(syserr_file.c_str(), NULL, "System Error", CR_AF_MISSING_FILE_OK);

    // error_log.txt
    auto error_log_file = cur_path.string() + "\\syserr\\crash\\error_log.txt";
    crAddFile2(error_log_file.c_str(), NULL, "Error Log", CR_AF_MISSING_FILE_OK);

    // dxdiag_log.txt
    auto dxdiag_log_file = cur_path.string() + "\\syserr\\crash\\dxdiag_log.txt";
    crAddFile2(dxdiag_log_file.c_str(), NULL, "DirectX Dialog", CR_AF_MISSING_FILE_OK);

    // We want the screenshot of the entire desktop to be added on crash.
    crAddScreenshot2(CR_AS_VIRTUAL_SCREEN, CR_AV_QUALITY_LOW);
}

void CrashRptUninitialize()
{
    crUninstall();
}
#endif // USE_CRASH_RPT
