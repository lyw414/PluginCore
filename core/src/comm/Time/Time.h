
/**
 * @details     时间相关操作
 *              
 * @brief       时间相关操作
 * @file        Log.h
 *
 * @version     1.0
 * @author      ywlong
 * @date        2024年11月1日
 */

#ifndef __LYW_PLUGIN_CORE_TIME_FILE_H__
#define __LYW_PLUGIN_CORE_TIME_FILE_H__
#include "PluginCoreCommDefine.h"

namespace LYW_PLUGIN_CORE
{
    class _VISIBLE_ Time
    {
        public:

            /**
             * @brief               获取ms时间
             *
             * @return              时间
             */
            static uint64 TickCountMS();

            /**
             * @brief               获取字符串日期
             *
             * @param str[32]       输出日期
             *
             * @return              true 成功 false 失败
             */
            static bool DateStr(byte str[32]);
    };
}
#endif
