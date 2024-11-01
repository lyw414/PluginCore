#include <stdio.h>
/**
 * @details     插件核心功能接口
 *              
 * @brief       插件核心功能接口
 * @file        PluginCoreAPI.h
 *
 * @version     1.0
 * @author      ywlong
 * @date        2024年10月31日
 */

#ifndef __LYW_PLUGIN_CORE_API_FILE_H__
#define __LYW_PLUGIN_CORE_API_FILE_H__

#include "PluginCoreCommDefine.h"

namespace LYW_PLUGIN_CORE
{

    /**
     * @brief       插件核心功能接口
     */
    class _VISIBLE_ PluginCoreAPI
    {
        public:
            PluginCoreAPI();

            virtual ~PluginCoreAPI();


    };
}

#endif
