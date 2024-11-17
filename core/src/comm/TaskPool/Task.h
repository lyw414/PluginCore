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

#include <pthread.h>

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
                byte describe[128];     ///< 任务描述
                Function<void(pvoid, int32, pvoid)> func;   ///< 任务执行函数
                pvoid param;            ///< 参数指针
                int32 len;              ///< 参数长度
                pvoid usrPtr;           ///< 用户指针
                int32 index;            ///< 数组索引
                uint32 uuid;            ///< 任务uuid（生命周期）
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

            /**
             * @brief                   添加任务
             *
             * @param id                任务ID
             * @param func              任务函数
             * @param paran             参数指针
             * @param len               参数长度
             * @param usrPtr            用户指针
             * @param descrybe[128]     任务描述
             *
             * @return                  任务句柄 < 0 添加失败 
             */
            TaskHandle AddTask(ID id, TaskFunc func, pvoid param, int32 len, pvoid usrPtr, const byte describe[128] = NULL);
            
            /**
             * @brief                   弹出任务并添加至执行队列
             *
             * @param timeout           超时时间
             *
             * @return                  任务资源 NULL 失败
             */
            TaskNode_t *PopTask(int32 timeout);


            /**
             * @brief                   执行任务执行结束添加至空闲队列
             * @param taskNode          任务节点
             *
             */
            void ExcTask(TaskNode_t *taskNode);


            /**
             * @brief                   移除任务（处于执行中的任务则等待任务完成）
             *
             * @param id                任务添加时绑定的id
             * @param recycleFunc       资源回收函数 参数分别为任务添加时候的参数
             * @param timeout           超时时间 ms
             *
             * @return                  任务状态 TASK_FINISHED 表示全部移除成功 其余均为失败
             */
            eTaskState RemoveTaskByID(ID id, TaskRecycleFunc recycleFunc, int32 timeout);


            /**
             * @brief                   移除任务（处于执行中的任务则等待任务完成）
             *
             * @param taskHandel        任务句柄 
             * @param recycleFunc       资源回收函数 参数分别为任务添加时候的参数
             * @param timeout           超时时间 ms
             *
             * @return                  任务状态 TASK_FINISHED 表示全部移除成功 其余均为失败
             */
            eTaskState RemoveTask(TaskHandle taskHandle, TaskRecycleFunc recycleFunc, int32 timeout);

            /**
             * @brief                   等待任务
             *
             * @param taskHandle        任务句柄
             * @param recycleFunc       资源回收函数 参数分别为任务添加时候的参数
             * @param timeout           超时时间 ms
             *
             * @return                  任务状态 TASK_FINISHED 表示全部移除成功 其余均为失败
             */
            eTaskState WaitTask(TaskHandle taskHandle, TaskRecycleFunc recycleFunc, int32 timeout);


            /**
             * @brief                   等待任务
             *
             * @param id                任务绑定id
             * @param recycleFunc       资源回收函数 参数分别为任务添加时候的参数
             * @param timeout           超时时间 ms
             *
             * @return                  任务状态 TASK_FINISHED 表示全部移除成功 其余均为失败
             */
            eTaskState WaitTaskByID(ID id, TaskRecycleFunc recycleFunc, int32 timeout);


        
        private:
            NodeList::Node<TaskNode_t> *m_nodeArray;     ///< 任务节点

            NodeList m_waitNodeList;    ///< 等待队列

            NodeList m_excNodeList;     ///< 执行队列

            NodeList m_freeNodeList;    ///< 空闲队列

            uint32 m_maxTaskCount;      ///< 最大任务数

            pthread_mutex_t m_lock;     ///< 互斥量
            
            pthread_cond_t m_cond;      ///< 条件变量

            pthread_mutex_t m_condLock; ///< 条件变量锁

    };
}
#endif

