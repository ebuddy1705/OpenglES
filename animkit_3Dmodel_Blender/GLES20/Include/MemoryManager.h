#ifndef MEMORY_H_
#define MEMORY_H_

#include "MemoryTemplates.h"

#ifdef __APPLE__
#include <TargetConditionals.h>

#if((TARGET_IPHONE_SIMULATOR == 0) && (TARGET_OS_IPHONE == 1) && (USEMEMORYMANAGER))
#include ".mmgr.h"
#else
#include "nommgr.h"
#endif



#endif
#endif
