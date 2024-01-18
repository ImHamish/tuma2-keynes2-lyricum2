#ifndef CLIENT_ETERBASE_ERROR_H
#define CLIENT_ETERBASE_ERROR_H

#if VSTD_HAS_PRAGMA_ONCE
#pragma once
#endif

#ifdef USE_CRASH_RPT
extern void CrashRptInitialize();
extern void CrashRptUninitialize();
#else
extern void SetEterExceptionHandler();
#endif

#endif // CLIENT_ETERBASE_ERROR_H
