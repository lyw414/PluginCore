/**
 * @details     任务池
 *              
 * @brief       任务池
 * @file        TaskPool.h
 *
 * @version     1.0
 * @author      ywlong
 * @date        2024年10月31日
 */

#ifndef __LYW_PLUGIN_CORE_TASK_POLL_FILE_H__
#define __LYW_PLUGIN_CORE_TASK_POLL_FILE_H__
#include "PluginCoreCommDefine.h"
#include "TaskPool/Thread.h"
#include "TaskPool/Task.h"

namespace LYW_PLUGIN_CORE
{
    /**
     * @brief           任务池
     *
     */
    class TaskPool : public Thread,
                     public Task
    {
        public:
            
            /**
             * @brief                   构造
             *
             * @param taskCount         任务节点数
             * @param holdThread        保持线程
             * @param maxThread         最大线程 
             */
            TaskPool(uint32 taskCount = 256, uint32 holdThread = 4, uint32 maxThread = 16);
            
            /**
             * @brief                   析构
             */
            ~TaskPool();

            /**
             * @brief                   异步执行任务
             *
             * @param id                任务归属ID
             * @param func              任务回调
             * @param param             参数资源指针（不维护生命周期）
             * @param len               参数长度
             * @param usrPtr            用户指针
             * @param describe[128]     任务描述
             *
             * @return 
             */
            TaskHandle AsyncExc(ID id, TaskFunc func, pvoid param, int32 len, pvoid usrPtr, const byte describe[128] = NULL);

            /**
             * @brief                   等待任务完成
             *
             * @param taskHandle        任务句柄
             * @param timeout           超时事件ms  < 0 表示阻塞
             *
             * @return                  任务状态 TASK_FINISHED 表示成功
             */
            eTaskState Wait(TaskHandle taskHandle, int32 timeout);

            /**
             * @brief                   等待任务完成
             *
             * @param id                任务归属ID
             * @param timeout           超时时间ms < 0 表示阻塞
             *
             * @return 
             */
            eTaskState WaitByID(ID id, int32 timeout);

            /**
             * @brief                   移除任务 只有等待执行的任务可以移除 处于执行中的任务会返回失败
             *
             * @param taskHandle        任务句柄
             * @param recycleFunc       资源回收函数
             *
             * @return                  true 成功 false 失败
             */
            bool Remove(TaskHandle taskHandle, TaskFunc &recycleFunc);

            /**
             * @brief 
             *
             * @param id                任务绑定id
             * @param recycleFunc       资源回收函数
             *
             * @return                  true 成功 false 失败
             */
            bool RemoveByID(ID id, TaskFunc &recycleFunc);

        private:

            /**
             * @brief                   阻塞等待任务
             *
             * @param timeout           超时时间 ms  < 0 表示阻塞
             *
             * @return                  != NULL 任务资源 == NULL 失败
             */
            virtual pvoid WaitTask(int32 timeout);

            /**
             * @brief                   执行任务
             *
             * @param taskNode          任务资源 由 @WaitTask 获取
             */
            virtual void ExcuteTask(pvoid taskNode);

            /**
             * @brief                   任务资源平常
             *
             * @return                  0 ~ 10 0表示空闲 10表示繁忙
             */
            virtual int32 TaskResourceAssessment();

            /**
             * @brief                   守护线程
             */
            void DaemonRun();

            /**
             * @brief                   守护线程
             *
             * @param ptr               this 指针
             *
             * @return                  NULL
             */
            pvoid Daemon(pvoid *ptr);

        private:
            
            bool m_isRun;               ///< 任务是否运行

    };
}
#endif
