#ifndef _LIB_PYTHON_LINK_H_
#define _LIB_PYTHON_LINK_H_

/* For an MSVC DLL, we can nominate the .lib files used by extensions */
#ifdef MS_COREDLL
#	ifndef Py_BUILD_CORE /* not building the core - must be an ext */
#		if defined(_MSC_VER)
			/* So MSVC users need not specify the .lib file in
			their Makefile (other compilers are generally
			taken care of by distutils.) */
#			ifdef _DEBUG
#				pragma comment(lib,"python27_d.lib")
#			else
#				pragma comment(lib,"python27.lib")
#			endif /* _DEBUG */
#		endif /* _MSC_VER */
#	endif /* Py_BUILD_CORE */
#else
#pragma comment(lib,"python27-static.lib")
#endif /* MS_COREDLL */
#endif
