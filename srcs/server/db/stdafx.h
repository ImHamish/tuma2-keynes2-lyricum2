#ifndef __INC_METIN_II_DB_SERV_STDAFX_H__
#define __INC_METIN_II_DB_SERV_STDAFX_H__

#include "../../config/CommonDefines.h"

#include "../CommonDefines.h"
#include "../libthecore/stdafx.h"

#ifndef _WIN32
#include <semaphore.h>
#else
#define isdigit iswdigit
#define isspace iswspace
#endif
#include <vector>
#include <map>

#include "../common/service.h"
#include "../common/CommonDefines.h"
#include "../common/length.h"
#include "../common/tables.h"
#include "../common/singleton.h"
#include "../common/stl.h"

#include <m2_services.hpp>
#include <m2_utils.hpp>
#include <m2_length.hpp>
#include <m2_itemlength.hpp>
#include <m2_constants.hpp>
#include <m2_tables.hpp>
#include <m2_vnumhelper.hpp>

#endif
