#include "Log.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <execinfo.h>
#include <stdlib.h>

#include <dlfcn.h>

namespace LYW_PLUGIN_CORE
{

    Log::Log()
    {
        m_write = LogWriteFunc(&Log::DefaultWrite, this);
        
        pthread_mutex_init(&m_lock, NULL);

        strcpy(m_levelTag[LOG_LEVEL_ERROR], "ERROR");
        strcpy(m_levelTag[LOG_LEVEL_WARN], "WARN");
        strcpy(m_levelTag[LOG_LEVEL_INFO], "INFO");
        strcpy(m_levelTag[LOG_LEVEL_DEBUG], "DEBUG");

        m_level = LOG_LEVEL_INFO;
    }

    Log::~Log()
    {
        pthread_mutex_destroy(&m_lock);
    }

    int32 Log::DefaultWrite(const byte * data, int32 len)
    {
        printf("%s", data);
        return 0;
    }

    Log * Log::GetInstance()
    {
        static Log * self = new Log;
        return self;
    }

    void Log::WriteLog(eLogLevel level, const byte * file, const byte * func, int32 line, const char *fmt, ...)
    {
        byte head[4096] = {0};
        va_list args;
        if (level > m_level)
        {
            return;
        }
        sprintf(head, "[%s][%s %s %d]::", m_levelTag[level], file, func, line);

        va_start(args, fmt);
        vsprintf(head + strlen(head), fmt, args);
        va_end(args);

        pthread_mutex_lock(&m_lock);
        m_write(head, strlen(head));
        m_write("\n\x00", 2);
        pthread_mutex_unlock(&m_lock);
        return;
    }

    void Log::WriteHexLog(eLogLevel level, const byte * file, const byte * func, int32 line, const byte *data, int32 dataLen , const char *fmt, ...)
    {
        byte head[4096] = {0};
        int32 index = 0;
        ubyte tmp = 0x00;
        va_list args;
        if (level > m_level)
        {
            return;
        }
        sprintf(head, "[%s_HEX][%s %s %d]::", m_levelTag[level], file, func, line);

        va_start(args, fmt);
        vsprintf(head + strlen(head), fmt, args);
        va_end(args);

        sprintf(head + strlen(head), "\nHEX::");

        pthread_mutex_lock(&m_lock);
        m_write(head, strlen(head));

        memset(head, 0x00, sizeof(head));
        index = 0;
        sprintf(head, "\n%04d| ", 0);
        index = strlen(head);

        for (int32 iLoop = 0; iLoop < dataLen; iLoop++)
        {
            if (index + 32 < sizeof(head))
            {
                tmp = (((ubyte)data[iLoop]) >> 4) & 0x0F;
                head[index] = tmp <= 0x09 ? (tmp + 0x30) : (tmp - 0x0A) + 0x41;
                index++;

                tmp = (((ubyte)data[iLoop]) & 0x0F);
                head[index] = tmp <= 0x09 ? (tmp + 0x30) : (tmp - 0x0A) + 0x41;
                index++;

                head[index] = ' ';
                index++;
                if (((iLoop + 1) % 16) == 0)
                {
                    sprintf(head + index, "\n%04d| ", (iLoop + 1) / 16);
                    index += strlen(head + index);
                }
            }
            else
            {
                m_write(head, index);

                index = 0;
                memset(head, 0x00, sizeof(head));
            }
        }

        m_write(head, index);
        m_write("\n\x00", 2);

        pthread_mutex_unlock(&m_lock);
        return;
    }

    void Log::StackInfo(const byte * file, const byte * func, int32 line, const byte *fmt, ...)
    {

        byte head[4096] = {0};
        va_list args;

        pvoid array[10] = {NULL};
        size_t size = 0; 
        byte ** strings = NULL; 
        int32 count = 0; 
        
        size = backtrace(array, 10);

        strings = backtrace_symbols(array, size); 

        sprintf(head, "[LOG_STACK_INFO][%s %s %d]::",  file, func, line);
        va_start(args, fmt);
        vsprintf(head + strlen(head), fmt, args);
        va_end(args);
        
        pthread_mutex_lock(&m_lock);
        m_write(head, strlen(head));
        m_write("\nSTACK::\n", 9);

        for (int32 iLoop = 1; iLoop < size; iLoop++)
        {
            m_write(strings[iLoop], strlen(strings[iLoop]));
            m_write("\n", 1);
        }

        m_write("\n\x00", 2);
        pthread_mutex_unlock(&m_lock);

        free(strings);
        return;

    }

    void Log::SetLogLeve(eLogLevel level)
    {
        m_level = level;
    }
}

