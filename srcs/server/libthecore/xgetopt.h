// XGetopt.h  Version 1.2
//
// Author:  Hans Dietrich
//          hdietrich2@hotmail.com
//
// This software is released into the public domain.
// You are free to use it in any way you like.
//
// This software is provided "as is" with no expressed
// or implied warranty.  I accept no liability for any
// damage or loss of business that this software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XGETOPT_H
#define XGETOPT_H

extern int32_t optind, opterr, optreset;
extern TCHAR* optarg;

int32_t getopt(int32_t argc, TCHAR* argv[], TCHAR* optstring);

#endif //XGETOPT_H
