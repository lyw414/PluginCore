/**
 * @details     日志
 *              
 * @brief       日志
 * @file        Log.h
 *
 * @version     1.0
 * @author      ywlong
 * @date        2024年10月31日
 */

#ifndef __LYW_PLUGIN_CORE_LOG_FILE_H__
#define __LYW_PLUGIN_CORE_LOG_FILE_H__
#include "PluginCoreCommDefine.h"
#include <pthread.h>

namespace LYW_PLUGIN_CORE
{
#define LOG_ERROR(fmt, ...) Log::GetInstance()->WriteLog(LYW_PLUGIN_CORE::Log::LOG_LEVEL_ERROR, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__ );

#define LOG_WARN(fmt, ...) Log::GetInstance()->WriteLog(LYW_PLUGIN_CORE::Log::LOG_LEVEL_WARN, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

#define LOG_INFO(fmt, ...) Log::GetInstance()->WriteLog(LYW_PLUGIN_CORE::Log::LOG_LEVEL_INFO, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

#define LOG_DEBUG(fmt, ...) Log::GetInstance()->WriteLog(LYW_PLUGIN_CORE::Log::LOG_LEVEL_DEBUG, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

#define LOG_ERROR_HEX(data, len, fmt, ...) Log::GetInstance()->WriteHexLog(LYW_PLUGIN_CORE::Log::LOG_LEVEL_ERROR, __FILE__, __func__, __LINE__, data,len, fmt, ##__VA_ARGS__);

#define LOG_WARN_HEX(data, len, fmt, ...) Log::GetInstance()->WriteHexLog(LYW_PLUGIN_CORE::Log::LOG_LEVEL_WARN, __FILE__, __func__, __LINE__, data,len, fmt, ##__VA_ARGS__);

#define LOG_INFO_HEX(data, len, fmt, ...) Log::GetInstance()->WriteHexLog(LYW_PLUGIN_CORE::Log::LOG_LEVEL_INFO, __FILE__, __func__, __LINE__, data,len, fmt, ##__VA_ARGS__);

#define LOG_DEBUG_HEX(data, len, fmt, ...) Log::GetInstance()->WriteHexLog(LYW_PLUGIN_CORE::Log::LOG_LEVEL_DEBUG, __FILE__, __func__, __LINE__, data,len, fmt, ##__VA_ARGS__);

#define LOG_STACK_INFO(fmt, ...) Log::GetInstance()->StackInfo(__FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

    class PluginManager;

    /**
     * @brief           日志 - 单例
     */
    class _VISIBLE_ Log
    {
        friend class PluginManager;

        public:
            /**
             * @brief           日志等级
             */
            typedef enum tag_LogLevel
            {
                LOG_LEVEL_ERROR = 0,    ///< 错误
                LOG_LEVEL_WARN = 1,     ///< 告警
                LOG_LEVEL_INFO = 2,     ///< 审计
                LOG_LEVEL_DEBUG = 3,    ///< 调试
            } eLogLevel;

        private:
            /**
             * @brief               构造
             */
            Log();

            /**
             * @brief               析构
             */
            ~Log();

            /**
             * @brief               写日志函数注册
             *
             * @param writeFunc 
             */
            void SetWriteFunction(LogWriteFunc writeFunc);

            /**
             * @brief               设置日志等级
             *
             * @param level         日志等级
             */
            void SetLogLeve(eLogLevel level);
        public:

            /**
             * @brief               单例
             *
             * @return              != NULL 日志实例
             */
            static Log * GetInstance();



            /**
             * @brief               写日志 二进制
             *
             * @param level         等级
             * @param file          文件名
             * @param func          函数名
             * @param line          行号
             * @param data          二进制数据
             * @param dataLen       二进制数据长度
             * @param fmt           格式 - 日志内容
             * @param ...           不定参
             */
           void WriteHexLog(eLogLevel level, const byte * file, const byte * func, int32 line, const byte *data, int32 dataLen , const char *fmt, ...);

           /**
            * @brief                写日志
            *
            * @param level          等级
            * @param file           文件名
            * @param func           函数名
            * @param line           行号
            * @param fmt            格式 - 日志内容
            * @param ...            不定参
            */
            void WriteLog(eLogLevel level, const byte * file, const byte * func, int32 line, const char *fmt, ...);


            /**
             * @brief               堆栈信息
             *
             * @param file          文件名
             * @param func          函数名
             * @param line          行号
             * @param fmt           格式
             * @param ...           不定参数
             */
            void StackInfo(const byte * file, const byte * func, int32 line, const byte *fmt, ...);
            
            /**
             * @brief               缺省写日志 - 打印
             *
             * @param data          日志内容
             * @param len           长度
             *
             * @return              >= 0 成功 < 0 失败
             */
            int32 DefaultWrite(const byte * data, int32 len);

        private:
            LogWriteFunc m_write;   ///< 写回调

            eLogLevel m_level;  ///< 当前日志等级

            pthread_mutex_t m_lock; ///< 锁

            byte m_levelTag[4][16]; ///< 等级标签
    };
}

#endif
