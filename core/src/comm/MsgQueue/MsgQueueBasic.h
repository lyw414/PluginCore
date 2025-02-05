/**
 * @details     消息队列基类
 *              
 * @brief       消息队列基类
 * @file        
 *
 * @version     1.0
 * @author      lyw
 * @date        2025年2月5日
 */

#ifndef __LYW_PLUGIN_CORE_MSQ_QUEUE_BASIC_FILE_H__
#define __LYW_PLUGIN_CORE_MSQ_QUEUE_BASIC_FILE_H__

#include "PluginCoreCommDefine.h"

namespace LYW_PLUGIN_CORE
{
    class _VISIBLE_ MsgQueueBasic
    {
        public:
            
            typedef enum tag_MsgQueueMode
            {
                SHM_MODE,   ///< 共享缓存模式
                TCP_MODE,   ///< TCP 模式 - 暂时不支持
                UDP_MODE,   ///< UDP 模式 = 暂时不支持
            } eMsgQueueMode;


            typedef int32 Handle;       ///< 链接句柄
            typedef int32 SubHandle;    ///< 订阅句柄
            typedef int32 Session;      ///< 发布会话

            
            typedef Function<void(pvoid, int32, pvoid)> SubFunc;    ///< 订阅处理回调

        public:

            /**
             * @brief                   构造
             */
            MsgQueueBasic();

            /**
             * @brief                   析构
             */
            ~MsgQueueBasic();

            /**
             * @brief                   链接至消息队列
             *
             * @param szInfo[256]       链接详情
             *
             * @return  消息队列句柄 < 0 失败
             */
            virtual Handle Connect(const byte szInfo[256]) = 0;

            /**
             * @brief                   断开链接
             *
             * @param handle            消息句柄
             *
             * @return 错误码
             */
            virtual eErrCode DisConnect(Handle handle) = 0;

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
            virtual Session Pub(ID id, EventID eventID, pvoid msg, int32 msgLen) = 0;

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
            virtual SubHandle Sub(ID id, EventID eventID, SubFunc subFunc, pvoid usrPtr) = 0;

            /**
             * @brief                   取消订阅消息
             *
             * @param handle            订阅句柄
             *
             * @return 错误码
             */
            virtual eErrCode UnSubBySubHandle(SubHandle handle) = 0;

            /**
             * @brief                   取消消息订阅
             *
             * @param handle            链接句柄
             *
             * @return 错误码
             */
            virtual eErrCode UnSub(Handle handle) = 0;

            /**
             * @brief                   等待消息
             *
             * @param session           发布session
             * @param timeout           超时时间 ms <= 0 表示阻塞
             *
             * @return 错误码
             */
            virtual eErrCode Wait(Session session, int32 timeout) = 0;
    };
}

#endif
