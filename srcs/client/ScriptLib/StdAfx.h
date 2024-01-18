#pragma once

#include "../../config/CommonDefines.h"

#include "../CommonDefines.h"

#include "../eterLib/StdAfx.h"
#include "../eterGrnLib/StdAfx.h"

#ifdef AT
#undef AT // @warme667
#endif

#pragma warning(push)
#pragma warning(disable:5033) // warning C5033: 'register' is no longer a supported storage class
//#include <Python.h>
//#include <node.h>
//#include <grammar.h>
//#include <token.h>
//#include <parsetok.h>
//#include <errcode.h>
//#include <compile.h>
//#include <symtable.h>
//#include <eval.h>
//#include <marshal.h>
#include <python.h>
#include <node.h>
#include <grammar.h>
#include <token.h>
#include <parsetok.h>
#include <errcode.h>
#include <compile.h>
#include <eval.h>
#include <marshal.h>
#pragma warning(pop)

#ifdef BYTE
#undef BYTE
#endif

#ifdef AT
#undef AT // @warme667
#endif

#include "PythonUtils.h"
#include "PythonLauncher.h"
#include "Resource.h"

void initdbg();
