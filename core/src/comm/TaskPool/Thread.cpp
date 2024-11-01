#include "TaskPool/Thread.h"
#include "Log/Log.h"
#include "Time/Time.h"


#include <pthread.h>
#include <errno.h>
#include <sys/prctl.h>

namespace LYW_PLUGIN_CORE
{
    Thread::Thread(uint32 holdThread, uint32 maxThread)
    {
        uint64 m_tick = Time::TickCountMS();
    }

    pvoid Thread::WorkThreadEnter(pvoid ptr)
    {
        ThreadNode_t *node = (ThreadNode_t *)ptr;
        
        //设置线程名称
        prctl(PR_SET_NAME, "TaskPool_Work_T", 0, 0, 0);
        
        if (NULL == node)
        {
            LOG_ERROR("threadNode is Nil!");
        }
        
        //执行工作线程
        node->self->WorkThread(node);

        return NULL;
    }

    bool Thread::CreateWorKThread(ThreadNode_t *threadNode)
    {
        pthread_t thread = {0};
        pthread_attr_t attr = {0};    

        if (NULL == threadNode)
        {
            LOG_ERROR("threadNode is Nil!");
            return false;
        }

        //设置detach属性
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        if (pthread_create(&thread, &attr, WorkThreadEnter, threadNode) > 0 )
        {
            LOG_ERROR("Pthread Create Failed!! errno [%d]", errno);
            return false;
        }

        return true;
    }

    void Thread::WorkThread(ThreadNode_t *node)
    {
        void * task = NULL;

        while (THREAD_DROP != node->state)
        {
            //等待任务开始 
            if (THREAD_DROP == node->state)
            {
                break;
            }

            //设置线程空闲
            node->state = THREAD_FREE;

            //阻塞获取任务
            if (NULL != (task = WaitTask(1000)))
            {
                //获取到任务
                //设置开始时间
                node->startTime = __sync_fetch_and_add(&m_tick, 0);
                node->state = THREAD_OCCUPY;

                ExcuteTask(task);
            }
        }
        
        //线程退出
        node->state = THREAD_FINISHED;
        LOG_INFO("Thread Finished!");
    }

    void Thread::Daemon()
    {
        m_tick = Time::TickCountMS();
    }
}
