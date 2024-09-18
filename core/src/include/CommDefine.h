#ifndef __LYW_PLUGIN_CODE_COMM_DEFINE__
#define __LYW_PLUGIN_CODE_COMM_DEFINE__

#include <stdio.h>
#include "PluginCoreCommDefine.h"

namespace LYW_PLUGIN_CORE
{
    #define  LOG_ERROR(args...) printf(args)
    #define  LOG_WAINNING(args...) printf(args)
    #define  LOG_INFO(args...) printf(args)
    #define  LOG_DEBUG(args...) printf(args)

    typedef short int16;
    typedef unsigned short uint16;
    typedef int int32;
    typedef unsigned int uint32;
    typedef long long int64;
    typedef unsigned long long uint64;
    typedef char byte;
    typedef unsigned char ubyte;
    typedef char int8;
    typedef unsigned char uint8;
    typedef void * pvoid;
}

#endif
