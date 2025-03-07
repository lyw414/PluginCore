/**
 * @details     消息队列
 *              
 * @brief       消息队列
 * @file        
 *
 * @version     1.0
 * @author      lyw
 * @date        2025年2月5日
 */

#ifndef __LYW_PLUGIN_CORE_MSQ_QUEUE_FILE_H__
#define __LYW_PLUGIN_CORE_MSQ_QUEUE_FILE_H__

#include "MsgQueue/MsgQueueBasic.h"

namespace LYW_PLUGIN_CORE
{
    class _VISIBLE_ MsgQueue : public MsgQueueBasic
    {
        public:

            /**
             * @brief                   构造
             *  
             * @param mode              模式
             */
            MsgQueue(eMsgQueueMode mode);

            /**
             * @brief                   析构
             */
            ~MsgQueue();
                                       
            /**
             * @brief                   链接至消息队列
             *
             * @param szInfo[256]       链接详情
             *
             * @return  消息队列句柄 < 0 失败
             */
            virtual Handle Connect(const byte szInfo[256]);

            /**
             * @brief                   断开链接
             *
             * @param handle            消息句柄
             *
             * @return 错误码
             */
            virtual eErrCode DisConnect(Handle handle);

            /**
             * @brief                   发布消息
             *
             * @param id                接受者ID id < 0 时 表示广播
             * @param eventID           消息ID
             * @param msg               消息
             * @param msgLen            下下哦
             *
             * @return session  < 0 时 表示失败
             */
            virtual eErrCode Pub(ID id, EventID eventID, pvoid msg, int32 msgLen);

            /**
             * @brief                   订阅消息
             *
             * @param id                发布者ID id < 0 时 表示订阅所有
             * @param eventID           消息ID 
             * @param subFunc           订阅处理 - 同步处理 请勿阻塞，或自行异步处理
             * @param usrPtr            用户参数
             *
             * @return 订阅句柄 < 0 时 表示失败
             */
            virtual SubHandle Sub(ID id, EventID eventID, SubFunc subFunc, pvoid usrPtr);

            /**
             * @brief                   取消订阅消息
             *
             * @param handle            订阅句柄
             *
             * @return 错误码
             */
            virtual eErrCode UnSub(SubHandle handle);

            /**
             * @brief                   取消消息订阅
             *
             * @param handle            链接句柄
             *
             * @return 错误码
             */
            virtual eErrCode UnSub();
        private:

        private:
            MsgQueueBasic *m_msgQueue;  ///< 消息队列实例
            
            Handle m_connectHandle; ///< 连接句柄
    };
} 

#endif
