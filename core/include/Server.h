/**
 * @details     插件服务入口
 *              
 * @brief       插件服务入口
 * @file        Server.h
 *
 * @version     1.0
 * @author      ywlong
 * @date        2024年10月31日
 */

#ifndef __LYW_PLUGIN_SERVER_FILE_H__
#define __LYW_PLUGIN_SERVER_FILE_H__

#include "PluginCoreCommDefine.h"
#include <string>

namespace LYW_PLUGIN_CORE
{
    class _VISIBLE_ CServer
    {
        public:

            /**
             * @brief                           构造
             *
             * @param logWriteFunc              日志写回调
             * @param pluginCFG                 插件配置文件
             * @param translateCFG              转换ID配置文件
             * @param sessionCount              最大会话数
             * @param taskCount                 最大任务数
             * @param holdThread                保持线程数
             * @param maxThread                 最大线程数
             */
            CServer(LogWriteFunc logWriteFunc, const std::string pluginCFG, const std::string translateCFG, uint32 sessionCount = 8192, uint32 taskCount = 256, uint32 holdThread = 4, uint32 maxThread = 16);

            /**
             * @brief                           析构
             */
            virtual ~CServer();

            virtual void Init();

            virtual void UnInit();

            int32 Start();

            int32 Stop();
    };
}
#endif
