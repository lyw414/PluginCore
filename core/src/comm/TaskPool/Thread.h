/**
 * @details     线程池
 *              
 * @brief       线程池
 * @file        Thread.h
 *
 * @version     1.0
 * @author      ywlong
 * @date        2024年10月31日
 */

#ifndef __LYW_PLUGIN_CORE_THREAD_FILE_H__
#define __LYW_PLUGIN_CORE_THREAD_FILE_H__
#include "PluginCoreCommDefine.h"
#include <string>

namespace LYW_PLUGIN_CORE
{
    /**
     * @brief               线程池
     */
    class Thread
    {

        protected:
            typedef Function<void(pvoid)> ThreadEnterFunc; ///< 线程函数
        private:
            /**
             * @brief           线程状态
             */
            typedef enum tag_ThreadState
            {
                THREAD_OCCUPY,      ///< 线程忙
                THREAD_FREE,        ///< 线程空闲
                THREAD_DROP,        ///< 线程丢弃 - 执行完后会自行销毁
                THREAD_FINISHED,    ///< 线程执行完成 
            } eThreadState;

            /**
             * @brief           线程节点
             */
            typedef struct tag_ThreadNode
            {
                eThreadState state; ///< 线程状态
                uint64 startTime; ///< 开始执行时间 0 未开始
            } ThreadNode_t;

        public:
            /**
             * @brief                   构造
             *
             * @param holdThread        保持线程数
             * @param maxThread         最大线程数
             */
            Thread(uint32 holdThread, uint32 maxThread);

            /**
             * @brief                   析构
             */
            virtual ~Thread();



        protected:
            /**
             * @brief                   阻塞等待任务
             *
             * @param timeout           超时时间 ms  < 0 表示阻塞
             *
             * @return                  != NULL 任务资源 == NULL 失败
             */
            virtual pvoid * WaitTask(int32 timeout) = 0;


            /**
             * @brief                   执行任务
             *
             * @param taskNode          任务资源 由 @WaitTask 获取
             */
            virtual void ExcuteTask(pvoid * taskNode) = 0;

        private:
            /**
             * @brief                   创建线程
             *
             * @param threadNode        线程节点
             * 
             * @return                  true 成功 false 失败
             */
            bool CreateWorKThread(ThreadNode_t *threadNode);

            /**
             * @brief                   销毁工作线程
             *
             * @param threadNode        线程节点
             *
             * @return                  true 成功 false 失败
             */
            bool DestroyWorKThread(ThreadNode_t *threadNode);

            /**
             * @brief                   工作线程线程入口
             *
             * @param ptr               用户指针
             *
             * @return                  NULL
             */
            static pvoid WorkThreadEnter(pvoid ptr);

            /**
             * @brief                   工作线程
             */
            void WorkThread(pvoid usrPtr);

            /**
             * @brief                   守护
             */
            void Daemon();

        private:
            

    };
}

#endif
