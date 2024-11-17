#include "Time/Time.h"

#include <time.h>
#include <string.h>

namespace LYW_PLUGIN_CORE
{
    uint64 Time::TickCountMS()
    {
        struct timespec tp;
        clock_gettime(CLOCK_MONOTONIC_COARSE, &tp);
        return (tp.tv_sec * 1000LL + tp.tv_nsec / 1000000);
    }

    bool Time::DateStr(byte str[64])
    {
        time_t now = 0;
        struct tm date;

        if (NULL == str)
        {
            return false;
        }
        
        memset((byte *)str, 0x00, 32);

        time(&now);

        localtime_r(&now, &date);

        snprintf(str, 64, "%04d-%02d-%02d %02d:%02d:%02d", date.tm_year + 1900, date.tm_mon + 1,  date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
            return true;
    }
}

