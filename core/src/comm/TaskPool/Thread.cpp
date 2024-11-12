#include "TaskPool/Thread.h"
#include "Log/Log.h"
#include "Time/Time.h"


#include <pthread.h>
#include <errno.h>
#include <sys/prctl.h>
#include <unistd.h>

namespace LYW_PLUGIN_CORE
{
    Thread::Thread(uint32 holdThread, uint32 maxThread)
    {
        uint64 m_tick = Time::TickCountMS();

        m_checkTick = m_tick;

        m_checkRecord = 0;

        m_threadNode.Resize(32);

        m_maxThreadCount = maxThread;

        m_holdThreadCount = holdThread;

    }

    Thread::~Thread()
    {
        int32 retry = 3;
        for (int32 iLoop = 0; iLoop < m_threadNode.Size(); iLoop++)
        {
            if (m_threadNode.IsIndexValid(iLoop) && m_threadNode[iLoop] != NULL)
            {
                m_threadNode[iLoop]->state = THREAD_DROP;
             }
        }

        for (int32 iLoop = 0; iLoop < m_threadNode.Size(); iLoop++)
        {
            retry = 3;
            if (m_threadNode.IsIndexValid(iLoop) && m_threadNode[iLoop] != NULL)
            {
                while (retry > 0 && THREAD_FINISHED != m_threadNode[iLoop]->state)
                {
                    sleep(1);
                    retry --;
                }

                if (retry <= 0)
                {
                    LOG_STACK_INFO("Thread Not Finished! MayCause Core!");
                }
            }
        }
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

    bool Thread::CreateWorkThread(ThreadNode_t *threadNode)
    {
        pthread_t thread = {0};
        pthread_attr_t attr;    

        if (NULL == threadNode)
        {
            LOG_ERROR("threadNode is Nil!");
            return false;
        }

        //设置detach属性
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        threadNode->self = this;
        threadNode->startTime = 0;
        threadNode->state = THREAD_FREE;

        if (pthread_create(&thread, &attr, WorkThreadEnter, threadNode) > 0 )
        {
            LOG_ERROR("Pthread Create Failed!! errno [%d]", errno);
            return false;
        }

        return true;
    }

    bool Thread::DestroyWorKThread(ThreadNode_t *threadNode)
    {
        if (NULL == threadNode)
        {
            return false;
        }
        return true;
    }


    void Thread::WorkThread(ThreadNode_t *node)
    {
        pvoid task = NULL;

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
            task = WaitTask(1000);
            if (NULL != task)
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

    int32 Thread::TaskResourceAssessment()
    {
        return 0;
    }

    void Thread::Daemon()
    {
        //int32 opt = ;
        m_tick = Time::TickCountMS();

        eThreadOpt opt = THREAD_OPT_NONE;

        int32 freeThreadCount = 0;
        //非阻塞线程（执行时间 不超过1s, 包括空闲线程）
        int32 noBlockThreadCount = 0;

        //线程调整
        //执行时间超过 1s的线程 都不计算到线程池总数里
        for (int32 iLoop = 0; iLoop < m_threadNode.Size(); iLoop++)
        {
            if (m_threadNode.IsIndexValid(iLoop) && m_threadNode[iLoop] != NULL)
            {
                //查看线程节点状态
                switch(m_threadNode[iLoop]->state)
                {
                    case THREAD_OCCUPY:
                    {
                        //执行时间超过3s 不计入总数
                        if (m_threadNode[iLoop]->startTime + 3000 >= m_tick)
                        {
                            //不能通过执行时间去判断当前线程是计算密集型 还是 IO密集型 （针对IO密集型线程 可以通过增加线程提升效率） 
                            noBlockThreadCount++;
                        }

                        break;
                    }
                    case THREAD_FREE:
                    {
                        freeThreadCount++;
                        noBlockThreadCount++;
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
        }

        //1秒内连续5次探测均出现限制线程 
        if (m_checkTick + 200 < m_tick || m_checkTick > m_tick)
        {
            m_checkTick = m_tick;

            if (freeThreadCount > m_holdThreadCount)
            {
                if (m_checkRecord < 0)
                {
                    m_checkRecord = 0;
                }

                m_checkRecord++;
            }
            else if (freeThreadCount < m_holdThreadCount)
            {
                if (m_checkRecord > 0)
                {
                    m_checkRecord = 0;
                }
                m_checkRecord--;
            }
            else
            {
                m_checkRecord = 0;
                opt = THREAD_OPT_NONE;
            }

            if (m_checkRecord > 5 && noBlockThreadCount > m_holdThreadCount)
            {
                opt = THREAD_OPT_FREE;
                m_checkRecord = 0;
            }

            //资源评测超过6 不会移除线程
            //线程总数不足持有线程数量
            //持续探测无空闲线程 
            if ((noBlockThreadCount < m_holdThreadCount) || (m_checkRecord < -5 || (TaskResourceAssessment() >= 6)))
            {
                if (noBlockThreadCount < m_maxThreadCount)
                {
                    opt = THREAD_OPT_CREATE;
                    m_checkRecord = 0;
                }
                else
                {
                    opt = THREAD_OPT_NONE;
                }
            }
        }
        
        switch (opt)
        {
            case THREAD_OPT_CREATE:
            {
                int32 nodeIndex = -1;

                for (int32 iLoop = 0; iLoop < m_threadNode.Size(); iLoop++)
                {
                    //找到空闲的所以并插入
                    if (!m_threadNode.IsIndexValid(iLoop))
                    {
                        nodeIndex = iLoop;
                    }
                }

                if (nodeIndex < 0)
                {
                    nodeIndex = m_threadNode.Size();
                }

                ThreadNode_t *node = new(std::nothrow) ThreadNode_t;
                if (NULL == node)
                {
                    LOG_STACK_INFO("New Failed\n");
                    return;
                }
                
                //创建线程
                CreateWorkThread(node);
                
                //登记
                m_threadNode[nodeIndex] = node;
                break;
            }
            case THREAD_OPT_FREE:
            {
                for (int32 iLoop = 0; iLoop < m_threadNode.Size(); iLoop++)
                {
                    if (m_threadNode.IsIndexValid(iLoop) && m_threadNode[iLoop] != NULL)
                    {
                        //仅操作空闲线程
                        if (THREAD_FREE == m_threadNode[iLoop]->state)
                        {
                            //空闲线程修改状态
                            m_threadNode[iLoop]->state = THREAD_DROP;
                            m_threadNode.Erase(iLoop);
                            break;
                        }
                    }
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
}
