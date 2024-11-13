/**
 * @details     任务队列
 *              
 * @brief       任务队列
 * @file        Task.h
 *
 * @version     1.0
 * @author      ywlong
 * @date        2024年10月31日
 */

#ifndef __LYW_PLUGIN_CORE_TASK_FILE_H__
#define __LYW_PLUGIN_CORE_TASK_FILE_H__

#include "PluginCoreCommDefine.h"
#include "NodeList/NodeList.h"

namespace LYW_PLUGIN_CORE
{

    /**
     * @brief               任务队列
     */
    class Task
    {
        protected:

            /**
             * @brief           任务节点
             */
            typedef struct tag_TaskNode
            {
                int32 ownerID;          ///< 任务归属ID
                byte descrybe[128];     ///< 任务描述
                Function<void(pvoid, int32, pvoid)> func;   ///< 任务执行函数
                pvoid param;            ///< 参数指针
                int32 len;              ///< 参数长度
                pvoid usrPtr;           ///< 用户指针
            } TaskNode_t;

        public:

            /**
             * @brief                   构造
             *
             * @param maxTaskCount      任务队列深度
             */
            Task(uint32 maxTaskCount);

            /**
             * @brief                   析构
             */
            virtual ~Task();
        
        private:
            NodeList::Node<TaskNode_t> *m_nodeArray;     ///< 任务节点

            NodeList m_waitNodeList;    ///< 等待队列

            NodeList m_excNodeList;     ///< 执行队列

            NodeList m_freeNodeList;    ///< 空闲队列

            uint32 m_maxTaskCount;      ///< 最大任务数
    };
}
#endif

