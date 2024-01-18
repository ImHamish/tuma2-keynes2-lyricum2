#ifndef __INC_LIBTHECORE_TYPEDEF_H__
#define __INC_LIBTHECORE_TYPEDEF_H__

#ifndef _WIN32
typedef int32_t socket_t;
typedef uint32_t DWORD;
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef int32_t INT;
typedef int32_t BOOL;
typedef uint32_t UINT;
#else
typedef SOCKET socket_t;

	#if !defined(_W64)
		#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
				#define _W64 __w64
			#else
				#define _W64
		#endif
	#endif

#endif

#endif // __INC_LIBTHECORE_TYPEDEF_H__
