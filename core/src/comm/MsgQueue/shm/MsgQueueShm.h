/**
 * @details     消息队列 - 共享缓存
 *              
 * @brief       消息队列 - 共享缓存
 * @file        
 *
 * @version     1.0
 * @author      lyw
 * @date        2025年2月5日
 */

#ifndef __LYW_PLUGIN_CORE_MSQ_QUEUE_SHM_FILE_H__
#define __LYW_PLUGIN_CORE_MSQ_QUEUE_SHM_FILE_H__

#include "MsgQueue/MsgQueueBasic.h"

namespace LYW_PLUGIN_CORE
{
    class _VISIBLE_ MsgQueueShm : public MsgQueueBasic
    {
        public:
            MsgQueueShm();

            ~MsgQueueShm();
    };
}
#endif
