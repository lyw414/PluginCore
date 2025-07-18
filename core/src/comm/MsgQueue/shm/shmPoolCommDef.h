/**
 * @details     共享缓存池通用定义
 *              
 * @brief       共享缓存池通用定义
 * @file        shmPoolCommDef.h
 *
 * @version     1.0
 * @author      lyw
 * @date        2025年2月5日
 */

#ifndef __LYW_PLUGIN_CORE_MSQ_QUEUE_SHM_POOL_COMM_DEF_FILE_H__
#define __LYW_PLUGIN_CORE_MSQ_QUEUE_SHM_POOL_COMM_DEF_FILE_H__

#include "PluginCoreCommDefine.h"

#include <pthread.h>
#include <sys/shm.h>


namespace LYW_PLUGIN_CORE
{
    class ShmPoolCommDef
    {
        public:
            /**
             * @brief                   共享缓存地址
             */
            typedef struct __attribute__((aligned(4))) tag_ShmAddr
            {
                key_t key; ///< 共享内存块键值 
                uint32 offset; ///< 偏移量
                uint32 uuid;        ///< 块id 键值存在复用的情况
                int32 index;        ///< 块索引号 < 0 表示无效
            } ShmAddr_t;
    };
}
#endif
